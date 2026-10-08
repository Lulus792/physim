"""Independent AT-SPI consumer; discovers the real rendered fixture via the registry."""

import argparse, json, os, subprocess, sys, time
from pathlib import Path

p = argparse.ArgumentParser()
p.add_argument("--fixture", required=True)
p.add_argument("--work", required=True, type=Path)
p.add_argument("--inner", action="store_true")
a = p.parse_args()
a.work.mkdir(parents=True, exist_ok=True)
if not a.inner:
    command = [
        "dbus-run-session",
        "--",
        "/usr/bin/python3",
        str(Path(__file__).resolve()),
        "--fixture",
        str(Path(a.fixture).resolve()),
        "--work",
        str(a.work.resolve()),
        "--inner",
    ]
    environment = dict(os.environ, GSETTINGS_BACKEND="memory")
    raise SystemExit(subprocess.call(command, env=environment))
import dbus

bus = dbus.SessionBus()
status = bus.get_object("org.a11y.Bus", "/org/a11y/bus")
properties = dbus.Interface(status, "org.freedesktop.DBus.Properties")
properties.Set("org.a11y.Status", "IsEnabled", dbus.Boolean(True))
# X11 also advertises a bus on its root window. Pin the private test bus before
# libatspi is imported, so simultaneous displays/tests cannot redirect queries.
os.environ["AT_SPI_BUS_ADDRESS"] = str(dbus.Interface(status, "org.a11y.Bus").GetAddress())
import pyatspi
from gi.repository import GLib

env = dict(os.environ)
env["NO_AT_BRIDGE"] = "0"
env["LIBGL_ALWAYS_SOFTWARE"] = "1"
env["PHYSIM_A11Y_TRACE"] = "1"
stdout = (a.work / "fixture.stdout").open("wb")
stderr = (a.work / "fixture.stderr").open("wb")
process = subprocess.Popen(
    [a.fixture], stdin=subprocess.PIPE, stdout=stdout, stderr=stderr, env=env
)
events = []
checks = []
discovery = []


def pump():
    context = GLib.MainContext.default()
    while context.pending():
        context.iteration(False)


def wait(fn, label, seconds=15):
    end = time.monotonic() + seconds
    last = None
    while time.monotonic() < end:
        pump()
        if process.poll() is not None:
            raise AssertionError(
                f"fixture exited {process.returncode}: " + (a.work / "fixture.stderr").read_text()
            )
        try:
            value = fn()
            if value:
                return value
        except Exception as error:
            last = repr(error)
        time.sleep(0.03)
    raise AssertionError(label + ": " + str(last))


def app_find():
    desktop = pyatspi.Registry.getDesktop(0)
    observed = []
    for app in desktop:
        observed.append({"name": app.name, "pid": app.get_process_id()})
        if app.name == "Physim" and app.get_process_id() == process.pid:
            return app
    if not discovery or discovery[-1] != observed:
        discovery.append(observed)
    address = dbus.Interface(
        bus.get_object("org.a11y.Bus", "/org/a11y/bus"), "org.a11y.Bus"
    ).GetAddress()
    raw_bus = dbus.bus.BusConnection(address)
    children = dbus.Interface(
        raw_bus.get_object("org.a11y.atspi.Registry", "/org/a11y/atspi/accessible/root"),
        "org.a11y.atspi.Accessible",
    ).GetChildren()
    raw = []
    for name, path in children:
        proxy = raw_bus.get_object(name, path)
        props = dbus.Interface(proxy, "org.freedesktop.DBus.Properties")
        raw.append(
            {
                "bus": str(name),
                "path": str(path),
                "name": str(props.Get("org.a11y.atspi.Accessible", "Name")),
            }
        )
    if raw:
        discovery.append({"raw": raw})
    raw_bus.close()
    return None


def named(parent, name):
    for child in parent:
        if child.name == name:
            return child
    return None


def showing(node):
    return node.getState().contains(pyatspi.STATE_SHOWING)


def enabled(node):
    return node.getState().contains(pyatspi.STATE_ENABLED)


def event(e):
    events.append({"type": e.type, "detail1": e.detail1})


pyatspi.Registry.registerEventListener(
    event, "object:children-changed", "object:state-changed", "object:bounds-changed"
)
try:
    app = wait(app_find, "registered application")
    assert app.getRole() == pyatspi.ROLE_APPLICATION
    wa = wait(lambda: named(app, "Physim AT-SPI A"), "first window")
    wb = wait(lambda: named(app, "Physim AT-SPI B"), "second window")
    assert app.childCount == 2
    button = wait(lambda: named(wa, "Öffnen …"), "first native button")
    other = wait(lambda: named(wb, "Öffnen …"), "second native button")
    text = wait(lambda: named(wa, "Bereit 🌍"), "native UTF-8 text")
    assert button.getRole() == pyatspi.ROLE_PUSH_BUTTON and text.getRole() == pyatspi.ROLE_LABEL
    assert button.getApplication() == app and other.getApplication() == app
    before = button.queryComponent().getExtents(pyatspi.WINDOW_COORDS)
    assert before.width > 0 and before.height == 24
    assert showing(button) and enabled(button) and enabled(other)
    assert button.queryAction().nActions == 1 and button.queryAction().getName(0) == "click"
    address = dbus.Interface(
        bus.get_object("org.a11y.Bus", "/org/a11y/bus"), "org.a11y.Bus"
    ).GetAddress()
    raw_bus = dbus.bus.BusConnection(address)
    refs = dbus.Interface(
        raw_bus.get_object("org.a11y.atspi.Registry", "/org/a11y/atspi/accessible/root"),
        "org.a11y.atspi.Accessible",
    ).GetChildren()
    provider = next(
        str(name)
        for name, path in refs
        if dbus.Interface(raw_bus.get_object(name, path), "org.freedesktop.DBus.Properties").Get(
            "org.a11y.atspi.Accessible", "Name"
        )
        == "Physim"
    )
    cache = dbus.Interface(
        raw_bus.get_object(provider, "/org/a11y/atspi/cache"), "org.a11y.atspi.Cache"
    ).GetItems()
    assert len(cache) == 21 and sum(int(item[7]) == 43 for item in cache) == 2
    object_path = next(str(item[0][1]) for item in cache if str(item[6]) == "Öffnen …")
    proxy = raw_bus.get_object(provider, object_path)
    for iface, method, signature, args, expected in [
        (
            "org.a11y.atspi.Action",
            "DoAction",
            "s",
            (dbus.String("wrong"),),
            "org.freedesktop.DBus.Error.InvalidArgs",
        ),
        (
            "org.a11y.atspi.Action",
            "DoAction",
            "i",
            (dbus.Int32(-1),),
            "org.freedesktop.DBus.Error.InvalidArgs",
        ),
        (
            "org.a11y.atspi.Component",
            "GetExtents",
            "u",
            (dbus.UInt32(99),),
            "org.freedesktop.DBus.Error.NotSupported",
        ),
        (
            "org.freedesktop.DBus.Properties",
            "Set",
            "ssv",
            (
                dbus.String("org.a11y.atspi.Accessible"),
                dbus.String("Name"),
                dbus.String("changed", variant_level=1),
            ),
            "org.freedesktop.DBus.Error.PropertyReadOnly",
        ),
    ]:
        try:
            raw_bus.call_blocking(provider, object_path, iface, method, signature, args, timeout=5)
            raise AssertionError("invalid RPC accepted")
        except dbus.exceptions.DBusException as error:
            assert error.get_dbus_name() == expected, (method, error)
    assert button.name == "Öffnen …"
    for path in [
        "/org/a11y/atspi/accessible/w0",
        "/org/a11y/atspi/accessible/w4294967296",
        "/org/a11y/atspi/accessible/w1/n18446744073709551616",
        "/org/a11y/atspi/accessible/w01",
        "/org/a11y/atspi/accessible/w1/n0",
    ]:
        try:
            raw_bus.call_blocking(
                provider, path, "org.a11y.atspi.Accessible", "GetRole", "", (), timeout=5
            )
            raise AssertionError("invalid object path accepted")
        except dbus.exceptions.DBusException as error:
            assert error.get_dbus_name() == "org.a11y.atspi.Error.Defunct"
    local = button.queryComponent().getExtents(pyatspi.WINDOW_COORDS)
    assert (
        wa.queryComponent().getAccessibleAtPoint(local.x + 1, local.y + 1, pyatspi.WINDOW_COORDS)
        == button
    )
    screen_hit = button.queryComponent().getExtents(pyatspi.DESKTOP_COORDS)
    assert (
        wa.queryComponent().getAccessibleAtPoint(
            screen_hit.x + 1, screen_hit.y + 1, pyatspi.DESKTOP_COORDS
        )
        == button
    )
    assert (
        wa.queryComponent().getAccessibleAtPoint(
            screen_hit.x + 1,
            screen_hit.y + 1,
            2,  # AT-SPI parent coordinates; legacy PyAT-SPI exports only screen/window.
        )
        == button
    )
    try:
        text.queryAction()
        raise AssertionError("static text advertises Action")
    except NotImplementedError:
        pass
    checkbox = wait(lambda: named(wa, "Vektoren"), "native checkbox")
    other_checkbox = wait(lambda: named(wb, "Vektoren"), "other window checkbox")
    assert checkbox.getRole() == pyatspi.ROLE_CHECK_BOX
    assert checkbox.getState().contains(pyatspi.STATE_CHECKABLE)
    assert not checkbox.getState().contains(pyatspi.STATE_CHECKED)
    assert checkbox.queryAction().nActions == 1 and checkbox.queryAction().getName(0) == "toggle"
    assert checkbox.queryAction().doAction(0)
    wait(lambda: checkbox.getState().contains(pyatspi.STATE_CHECKED), "checked notification")
    wait(lambda: "TOGGLE A 1 1" in (a.work / "fixture.stdout").read_text(), "actual checkbox value")
    assert not other_checkbox.getState().contains(pyatspi.STATE_CHECKED)
    assert checkbox.queryAction().doAction(0)
    wait(lambda: not checkbox.getState().contains(pyatspi.STATE_CHECKED), "unchecked notification")
    wait(lambda: "TOGGLE A 2 0" in (a.work / "fixture.stdout").read_text(), "second checkbox value")
    assert sum(int(item[7]) == 7 for item in cache) == 2
    group = wait(lambda: named(wa, "Darstellung"), "radio group")
    code_group = wait(lambda: named(wa, "Code"), "independent group")
    radio = wait(lambda: named(group, "22 px"), "radio child")
    peer = wait(lambda: named(group, "16 px"), "selected peer")
    assert group.getRole() == pyatspi.ROLE_GROUPING and group.childCount == 2
    assert radio.getRole() == pyatspi.ROLE_RADIO_BUTTON and radio.parent == group
    assert radio.getIndexInParent() == 1 and peer.getIndexInParent() == 0
    assert peer.getState().contains(pyatspi.STATE_CHECKED) and not radio.getState().contains(
        pyatspi.STATE_CHECKED
    )
    assert radio.queryAction().getName(0) == "select" and radio.queryAction().doAction(0)
    wait(
        lambda: radio.getState().contains(pyatspi.STATE_CHECKED)
        and not peer.getState().contains(pyatspi.STATE_CHECKED),
        "exclusive radio selection",
    )
    wait(
        lambda: "CHOICE A 0 1" in (a.work / "fixture.stdout").read_text(), "actual radio selection"
    )
    assert named(code_group, "16 px").getState().contains(pyatspi.STATE_CHECKED)
    assert named(named(wb, "Darstellung"), "16 px").getState().contains(pyatspi.STATE_CHECKED)
    assert radio.queryAction().doAction(0)
    time.sleep(0.08)
    pump()
    assert radio.getState().contains(pyatspi.STATE_CHECKED)
    rwindow = radio.queryComponent().getExtents(pyatspi.WINDOW_COORDS)
    gwindow = group.queryComponent().getExtents(pyatspi.WINDOW_COORDS)
    rparent = radio.queryComponent().getExtents(2)
    assert rparent.x == rwindow.x - gwindow.x and rparent.y == rwindow.y - gwindow.y
    assert (
        group.queryComponent().getAccessibleAtPoint(
            rwindow.x + 1, rwindow.y + 1, pyatspi.WINDOW_COORDS
        )
        == radio
    )
    pwindow = peer.queryComponent().getExtents(pyatspi.WINDOW_COORDS)
    assert pwindow.x + pwindow.width + 1 < rwindow.x, "fixture needs an actual gap between options"
    assert (
        group.queryComponent().getAccessibleAtPoint(
            pwindow.x + pwindow.width + 1, rwindow.y + 1, pyatspi.WINDOW_COORDS
        )
        == group
    )
    assert peer.queryAction().doAction(0)
    wait(
        lambda: peer.getState().contains(pyatspi.STATE_CHECKED)
        and not radio.getState().contains(pyatspi.STATE_CHECKED),
        "reverse radio selection",
    )
    wait(
        lambda: "CHOICE A 0 0" in (a.work / "fixture.stdout").read_text(),
        "actual reverse radio selection",
    )
    assert (
        sum(int(item[7]) == 44 for item in cache) == 8
        and sum(int(item[7]) == 99 for item in cache) == 4
    )
    try:
        group.queryAction()
        raise AssertionError("group advertises Action")
    except NotImplementedError:
        pass
    assert checkbox.queryComponent().grabFocus()
    wait(lambda: checkbox.getState().contains(pyatspi.STATE_FOCUSED), "actual keyboard focus")
    assert checkbox.getState().contains(pyatspi.STATE_FOCUSABLE)
    assert not button.getState().contains(pyatspi.STATE_FOCUSED)
    assert not text.queryComponent().grabFocus() and not group.queryComponent().grabFocus()
    process.stdin.write(b"k")
    process.stdin.flush()
    wait(
        lambda: "TOGGLE A 3 1" in (a.work / "fixture.stdout").read_text(),
        "keyboard activation of focused checkbox",
    )
    assert checkbox.queryAction().doAction(0)
    wait(lambda: "TOGGLE A 4 0" in (a.work / "fixture.stdout").read_text(), "restore checkbox")
    assert button.queryAction().doAction(0)
    wait(lambda: "PRESS A 1" in (a.work / "fixture.stdout").read_text(), "real UI press delivery")
    wait(lambda: not enabled(button), "disabled state notification")
    assert not button.queryAction().doAction(0)
    assert not checkbox.queryAction().doAction(0)
    assert not checkbox.queryComponent().grabFocus()
    assert not radio.queryAction().doAction(0)
    assert enabled(other)
    assert other.queryAction().doAction(0)
    wait(
        lambda: "PRESS B 1" in (a.work / "fixture.stdout").read_text(),
        "second window independent action",
    )
    process.stdin.write(b"e")
    process.stdin.flush()
    wait(lambda: enabled(button), "reenabled state notification")
    process.stdin.write(b"h")
    process.stdin.flush()
    wait(lambda: not showing(button), "hidden window state notification")
    assert not button.queryAction().doAction(0)
    assert not checkbox.queryAction().doAction(0)
    assert not radio.queryAction().doAction(0)
    process.stdin.write(b"s")
    process.stdin.flush()
    wait(lambda: showing(button), "shown window state notification")
    screen = button.queryComponent().getExtents(pyatspi.DESKTOP_COORDS)
    process.stdin.write(b"m")
    process.stdin.flush()
    wait(
        lambda: button.queryComponent().getExtents(pyatspi.DESKTOP_COORDS).x != screen.x,
        "screen geometry update",
    )
    process.stdin.write(b"r")
    process.stdin.flush()
    wait(lambda: named(wa, "Öffnen …") is None, "removed control cache invalidation")
    wait(lambda: named(wa, "Vektoren") is None, "removed checkbox")
    wait(lambda: named(wa, "Darstellung") is None, "removed group")
    try:
        assert not radio.queryAction().doAction(0)
    except (GLib.Error, NotImplementedError):
        pass
    try:
        assert not checkbox.queryAction().doAction(0)
    except (GLib.Error, NotImplementedError):
        pass
    try:
        result = button.queryAction().doAction(0)
        assert not result
    except (GLib.Error, NotImplementedError):
        pass
    process.stdin.write(b"c")
    process.stdin.flush()
    wait(lambda: app.childCount == 1, "window unregister notification")
    assert "PRESS A 2" not in (a.work / "fixture.stdout").read_text()
    checks = [
        "checkbox role, actual two-way toggle, checked events and independent windows",
        "radio hierarchy, sibling indices, parent geometry, exclusive/idempotent selection and independent groups",
        "actual keyboard focus, focus states, keyboard activation and non-focusable rejection",
        "registry discovery",
        "single application/two windows",
        "UTF-8 roles",
        "positive window/screen extents",
        "real native press once per window",
        "disabled/reenabled state",
        "static text read only",
        "removed control/window invalidation",
        "bulk cache schema",
        "invalid RPC and read-only property rejection",
        "hit testing",
        "hidden window action rejection",
    ]
    raw_bus.close()
    process.stdin.write(b"q")
    process.stdin.flush()
    assert process.wait(timeout=10) == 0
    for setting in [
        dict(NO_AT_BRIDGE="1"),
        dict(NO_AT_BRIDGE="0", DBUS_SESSION_BUS_ADDRESS="unix:path=/nonexistent/physim-test-bus"),
    ]:
        fallback_env = dict(env, **setting)
        result = subprocess.run(
            [a.fixture, "--fallback"],
            input=b"q",
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            env=fallback_env,
            timeout=15,
        )
        assert result.returncode == 0, (setting, result.stderr.decode())
    checks.append("normal rendered GUI with disabled or absent accessibility bus")
    pump()
    assert any(e["type"].startswith("object:children-changed") for e in events) and any(
        e["type"].startswith("object:state-changed") for e in events
    )
    assert any(
        e["type"].startswith("object:state-changed:checked") and e["detail1"] == 1 for e in events
    )
    assert any(
        e["type"].startswith("object:state-changed:checked") and e["detail1"] == 0 for e in events
    )
finally:
    if process.poll() is None:
        process.kill()
        process.wait()
    stdout.close()
    stderr.close()
    (a.work / "results.json").write_text(
        json.dumps(
            {
                "checks": checks,
                "events": events,
                "discovery": discovery,
                "fixture_pid": process.pid,
                "exit_code": process.returncode,
            },
            ensure_ascii=False,
            indent=2,
        )
        + "\n"
    )
print(
    "Linux AT-SPI: independent registry discovery, two actual rendered windows, roles/UTF-8, coordinates, native actions, state/cache updates and safe unregister passed"
)
