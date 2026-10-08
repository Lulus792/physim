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
a = parser.parse_args()
a.work.mkdir(parents=True, exist_ok=True)
app = a.app.resolve()
work = a.work.resolve()
(work / "settings").mkdir(exist_ok=True)
if sys.platform == "darwin":
    result = subprocess.run(
        [str(app), "--settings-test", str(work / "settings"), "checkbox-native"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        timeout=45,
    )
    (work / "app.stdout").write_bytes(result.stdout)
    (work / "app.stderr").write_bytes(result.stderr)
    assert result.returncode == 0 and b"SETTINGS CHECKBOX SELF-TEST: PASSED" in result.stdout
    print(
        "Actual macOS settings: AppKit checkbox toggles the settings draft twice and preserves applied view flags"
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
    [str(app), "--settings-test", str(work / "settings"), "checkbox-remote"],
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
    "Actual Linux settings: independent AT-SPI checkbox actions toggle the settings draft twice and preserve applied view flags"
)
