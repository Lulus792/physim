"""Drive real SDL/Zenity file dialogs in the installed app under X11."""
import argparse
import json
import os
from pathlib import Path
import signal
import subprocess
import sys
import tempfile
import time


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--app", type=Path, required=True)
    parser.add_argument("--work", type=Path, required=True)
    args = parser.parse_args()
    if sys.platform != "linux":
        parser.error("Requires Linux, X11, a window manager, Zenity, AT-SPI, xdotool and scrot")
    args.work.mkdir(parents=True, exist_ok=True)
    root = Path(tempfile.mkdtemp(prefix="Native dialogs ä ", dir=args.work)).resolve()
    actions = []
    (root / "selected").mkdir()
    import pyatspi
    from gi.repository import GLib
    environment = dict(os.environ, SDL_FILE_DIALOG_DRIVER="zenity", GDK_BACKEND="x11",
                       GSETTINGS_BACKEND="memory", LC_ALL="C.UTF-8", HOME=str(root / "selected"),
                       XDG_CONFIG_HOME=str(root / ".config"), XDG_DATA_HOME=str(root / ".local/share"))

    def xdo(*arguments, allow_empty=False):
        result = subprocess.run(["xdotool", *map(str, arguments)], capture_output=True,
                                timeout=5, env=environment)
        if result.returncode and not (allow_empty and result.returncode == 1):
            raise RuntimeError(f"xdotool {arguments}: {result.stderr.decode(errors='replace')}")
        return result.stdout.decode("utf-8", errors="replace").strip()

    def screenshot(label):
        subprocess.run(["scrot", str(root / (label + ".png"))], env=environment,
                       capture_output=True, check=True, timeout=5)

    def find_widget(names, role=None):
        context = GLib.MainContext.default()
        while context.pending():
            context.iteration(False)
        def visit(node):
            try:
                if (node.name in names and (role is None or node.getRoleName() == role)
                        and node.getState().contains(pyatspi.STATE_SHOWING)):
                    return node
                for child in node:
                    found = visit(child)
                    if found is not None:
                        return found
            except GLib.Error:
                # A previous Zenity process can disappear during a tree read.
                return None
            return None

        for application in pyatspi.Registry.getDesktop(0):
            try:
                if application.name.lower() == "zenity":
                    found = visit(application)
                    if found is not None:
                        return found
            except GLib.Error:
                continue
        return None

    def click_widget(widget, window):
        # GTK 4's accessible coordinates must be translated through its X11
        # window; using them as desktop coordinates can click outside it.
        box = widget.queryComponent().getExtents(pyatspi.WINDOW_COORDS)
        if box.width <= 0 or box.height <= 0:
            raise RuntimeError(f"Invalid widget bounds: {widget.name}")
        xdo("mousemove", "--window", window, box.x + box.width // 2, box.y + box.height // 2)
        xdo("mousedown", "1")
        time.sleep(.1)
        xdo("mouseup", "1")

    log = root / "app.log"
    selected = root / "selected"
    with log.open("wb") as output, subprocess.Popen(
            [str(args.app.resolve()), "--workspace-state-test", str(selected), "native-dialogs"],
            stdout=output, stderr=subprocess.STDOUT, env=environment, cwd=root,
            start_new_session=True) as process:
        def wait_for(predicate, reason):
            deadline = time.monotonic() + 12
            while time.monotonic() < deadline:
                value = predicate()
                if value:
                    return value
                if process.poll() is not None:
                    raise RuntimeError(f"App exited during {reason}:\n{log.read_text(errors='replace')}")
                time.sleep(.05)
            raise TimeoutError(f"Timed out during {reason}:\n{log.read_text(errors='replace')}")

        try:
            for kind, name in (("folder", "Selected folder ä"), ("file", "External file ä.txt"),
                               ("addition", "Added folder ä"), ("cancel", None)):
                wait_for(lambda: f"NATIVE DIALOG REQUEST: {kind}" in log.read_text(errors="replace"),
                         f"{kind} request")
                window = wait_for(
                    lambda: xdo("search", "--onlyvisible", "--class", "[Zz]enity", allow_empty=True),
                    f"{kind} dialog").splitlines()[-1]
                title = xdo("getwindowname", window)
                actions.append({"action": kind, "window": window, "title": title,
                                "path": str(selected / name) if name else None})
                (root / "actions.json").write_text(json.dumps(actions, ensure_ascii=False, indent=2),
                                                    encoding="utf-8")
                xdo("windowactivate", "--sync", window)
                xdo("windowmove", window, "100", "100")
                # A mapped window can still be setting up its chooser widgets.
                time.sleep(.5)
                screenshot(kind + "-opened")
                if name:
                    # Browse a private Home containing the actual test paths.
                    # AT-SPI locates real GTK widgets across GTK 3/4 layouts;
                    # XTest clicks them without typing remapped Unicode keys.
                    home = wait_for(lambda: find_widget(("Home", "Open your personal folder")),
                                    f"{kind} Home control")
                    click_widget(home, window)
                    target = wait_for(lambda: find_widget((name,)), f"{kind} path row")
                    click_widget(target, window)
                    screenshot(kind + "-selected")
                    accept = wait_for(lambda: (button if (button := find_widget(("OK",), "push button"))
                                                and button.getState().contains(pyatspi.STATE_SENSITIVE)
                                                else None), f"{kind} enabled OK button")
                    click_widget(accept, window)
                else:
                    cancel = wait_for(lambda: find_widget(("Cancel",), "push button"),
                                      "cancel button")
                    click_widget(cancel, window)
                wait_for(lambda: window not in xdo("search", "--onlyvisible", "--class",
                                                   "[Zz]enity", allow_empty=True).splitlines(),
                         f"{kind} {'confirmation' if name else 'cancellation'}")
            if process.wait(timeout=15) != 0:
                raise RuntimeError(log.read_text(errors="replace"))
        except Exception:
            try:
                screenshot("failure")
            except (OSError, subprocess.SubprocessError) as error:
                (root / "screenshot-error.txt").write_text(str(error), encoding="utf-8")
            raise
        finally:
            # Include Zenity children when a failed interaction leaves a dialog open.
            try:
                os.killpg(process.pid, signal.SIGKILL)
            except ProcessLookupError:
                pass
            process.wait(timeout=5)
    if "NATIVE DIALOGS: PASSED" not in log.read_text(errors="replace"):
        raise AssertionError("The app did not confirm the selected paths and unchanged state after cancellation")
    if (selected / "native-dialogs.bmp").stat().st_size < 1000:
        raise AssertionError("Missing final workspace screenshot")

    # A missing backend must return control to the app and preserve the workspace.
    environment["SDL_FILE_DIALOG_DRIVER"] = "physim-no-such-dialog-driver"
    result = subprocess.run(
        [str(args.app.resolve()), "--workspace-state-test", str(root / "unavailable"), "native-dialog-error"],
        capture_output=True, timeout=20, env=environment, cwd=root)
    (root / "unavailable.log").write_bytes(result.stdout + result.stderr)
    if result.returncode or b"NATIVE DIALOG ERROR: PASSED" not in result.stdout:
        raise AssertionError((result.stdout + result.stderr).decode(errors="replace"))
    (root / "PASSED.txt").write_text(
        "Real Zenity folder/file/add-folder selection, Unicode paths, cancellation and unavailable backend passed.\n",
        encoding="utf-8")
    print(f"Installed native Linux dialogs verified: {root}")


if __name__ == "__main__":
    try:
        main()
    except Exception as error:
        # Keep the actual dialog phase and SDL output visible in the job's
        # annotation, rather than reporting only the shell's exit code.
        if os.environ.get("GITHUB_ACTIONS") == "true":
            message = str(error)[-4000:]
            message = message.replace("%", "%25").replace("\r", "%0D").replace("\n", "%0A")
            print(f"::error title=Native Linux file dialogs::{message}", flush=True)
        raise
