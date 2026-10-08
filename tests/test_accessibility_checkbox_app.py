"""Actual settings draft: native checkbox actions through AppKit or AT-SPI."""

import argparse
import json
import os
from pathlib import Path
import subprocess
import sys
import time

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--app", type=Path, required=True)
parser.add_argument("--work", type=Path, required=True)
parser.add_argument("--inner", action="store_true")
parser.add_argument("--options", action="store_true")
parser.add_argument("--focus", action="store_true")
a = parser.parse_args()
a.work.mkdir(parents=True, exist_ok=True)
app = a.app.resolve()
work = a.work.resolve()
assert not (a.focus and a.options)
mode = "focus" if a.focus else "options" if a.options else "checkbox"
marker = b"SETTINGS FOCUS SELF-TEST: PASSED" if a.focus else (
    b"SETTINGS RADIO SELF-TEST: PASSED" if a.options else b"SETTINGS CHECKBOX SELF-TEST: PASSED"
)
(work / "settings").mkdir(exist_ok=True)
if sys.platform == "darwin":
    result = subprocess.run(
        [str(app), "--settings-test", str(work / "settings"), mode + "-native"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        timeout=45,
    )
    (work / "app.stdout").write_bytes(result.stdout)
    (work / "app.stderr").write_bytes(result.stderr)
    assert result.returncode == 0 and marker in result.stdout
    print(
        "Actual macOS native settings: "
        + mode
        + " actions update drafts and preserve applied configuration"
    )
    raise SystemExit(0)
assert sys.platform == "linux"
if not a.inner:
    command = [
        "dbus-run-session",
        "--",
        "/usr/bin/python3",
        str(Path(__file__).resolve()),
        "--app",
        str(app),
        "--work",
        str(work),
        "--inner",
    ]
    if a.options:
        command.append("--options")
    if a.focus:
        command.append("--focus")
    raise SystemExit(subprocess.call(command, env=dict(os.environ, GSETTINGS_BACKEND="memory")))
import dbus

bus = dbus.SessionBus()
status = bus.get_object("org.a11y.Bus", "/org/a11y/bus")
dbus.Interface(status, "org.freedesktop.DBus.Properties").Set(
    "org.a11y.Status", "IsEnabled", dbus.Boolean(True)
)
os.environ["AT_SPI_BUS_ADDRESS"] = str(dbus.Interface(status, "org.a11y.Bus").GetAddress())
import pyatspi
from gi.repository import GLib

out = (work / "app.stdout").open("wb")
err = (work / "app.stderr").open("wb")
process = subprocess.Popen(
    [str(app), "--settings-test", str(work / "settings"), mode + "-remote"],
    stdout=out,
    stderr=err,
    env=dict(os.environ, NO_AT_BRIDGE="0", LIBGL_ALWAYS_SOFTWARE="1"),
)
events = []
pyatspi.Registry.registerEventListener(
    lambda e: events.append({"type": e.type, "value": e.detail1}), "object:state-changed:checked"
)


def wait(fn, label):
    deadline = time.monotonic() + 15
    while time.monotonic() < deadline:
        ctx = GLib.MainContext.default()
        while ctx.pending():
            ctx.iteration(False)
        if process.poll() is not None:
            raise AssertionError((label, process.returncode, (work / "app.stderr").read_text()))
        value = fn()
        if value:
            return value
        time.sleep(0.02)
    raise AssertionError(label)


def find():
    for application in pyatspi.Registry.getDesktop(0):
        if application.name == "Physim" and application.get_process_id() == process.pid:
            for window in application:
                for control in window:
                    if control.name == "Vektoren" and control.getRole() == pyatspi.ROLE_CHECK_BOX:
                        return control
    return None


try:
    if a.focus:
        wait(lambda: "FOCUS READY" in (work / "app.stdout").read_text(), "focus ready")

        def focus_target():
            for application in pyatspi.Registry.getDesktop(0):
                if application.name == "Physim" and application.get_process_id() == process.pid:
                    for window in application:
                        for child in window:
                            if (
                                child.name == "Standardwerte"
                                and child.getRole() == pyatspi.ROLE_PUSH_BUTTON
                            ):
                                return child
            return None

        target = wait(focus_target, "unfocused dock control")
        assert target.getState().contains(
            pyatspi.STATE_FOCUSABLE
        ) and not target.getState().contains(pyatspi.STATE_FOCUSED)
        assert target.queryComponent().grabFocus()
        assert process.wait(timeout=15) == 0
        assert marker.decode() in (work / "app.stdout").read_text()
    elif a.options:
        steps = [
            ("Schriftgröße der Oberfläche", "22 px"),
            ("Schriftgröße der Oberfläche", "16 px"),
            ("Darstellung", "Hell"),
            ("Darstellung", "Dunkel"),
            ("Schriftgröße im Code-Editor", "20 px"),
            ("Schriftgröße im Code-Editor", "16 px"),
        ]

        def find_group(label):
            for application in pyatspi.Registry.getDesktop(0):
                if application.name == "Physim" and application.get_process_id() == process.pid:
                    for window in application:
                        for child in window:
                            if child.name == label and child.getRole() == pyatspi.ROLE_GROUPING:
                                return child
            return None

        for number, (name, label) in enumerate(steps):
            wait(
                lambda: f"RADIO READY {number}" in (work / "app.stdout").read_text(), "radio ready"
            )
            group = wait(lambda: find_group(name), "actual settings radio group")
            choices = list(group)
            target = next(c for c in choices if c.name == label)
            assert target.parent == group and target.getRole() == pyatspi.ROLE_RADIO_BUTTON
            assert target.queryAction().getName(0) == "select" and target.queryAction().doAction(0)
            if number < 5:
                wait(lambda: target.getState().contains(pyatspi.STATE_CHECKED), "radio selected")
                assert sum(c.getState().contains(pyatspi.STATE_CHECKED) for c in group) == 1
                wait(
                    lambda: f"RADIO SELECTED {number}" in (work / "app.stdout").read_text(),
                    "actual draft selection",
                )
        assert process.wait(timeout=15) == 0
        assert marker.decode() in (work / "app.stdout").read_text()
    else:
        wait(lambda: "CHECKBOX READY " in (work / "app.stdout").read_text(), "settings ready")
        checkbox = wait(find, "real settings checkbox")
        initial = checkbox.getState().contains(pyatspi.STATE_CHECKED)
        assert checkbox.getState().contains(pyatspi.STATE_CHECKABLE)
        assert checkbox.queryAction().getName(0) == "toggle"
        assert checkbox.queryAction().doAction(0)
        wait(
            lambda: checkbox.getState().contains(pyatspi.STATE_CHECKED) != initial,
            "changed settings state",
        )
        wait(
            lambda: "CHECKBOX TOGGLED" in (work / "app.stdout").read_text(),
            "actual settings draft changed",
        )
        assert checkbox.queryAction().doAction(0)
        assert process.wait(timeout=15) == 0
        assert "SETTINGS CHECKBOX SELF-TEST: PASSED" in (work / "app.stdout").read_text()
finally:
    if process.poll() is None:
        process.kill()
        process.wait()
    out.close()
    err.close()
    (work / "results.json").write_text(
        json.dumps({"events": events, "exit_code": process.returncode}, indent=2) + "\n"
    )
print(
    "Actual Linux native settings: independent AT-SPI "
    + mode
    + " actions update drafts and preserve applied configuration"
)
