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
        parser.error("Requires Linux, X11, a window manager, Zenity, xdotool and scrot")
    args.work.mkdir(parents=True, exist_ok=True)
    root = Path(tempfile.mkdtemp(prefix="Native dialogs ä ", dir=args.work)).resolve()
    actions = []
    environment = dict(os.environ, SDL_FILE_DIALOG_DRIVER="zenity", GDK_BACKEND="x11",
                       LC_ALL="C.UTF-8")

    def xdo(*arguments, allow_empty=False):
        result = subprocess.run(["xdotool", *map(str, arguments)], capture_output=True,
                                timeout=5, env=environment)
        if result.returncode and not (allow_empty and result.returncode == 1):
            raise RuntimeError(f"xdotool {arguments}: {result.stderr.decode(errors='replace')}")
        return result.stdout.decode("utf-8", errors="replace").strip()

    def screenshot(label):
        subprocess.run(["scrot", str(root / (label + ".png"))], env=environment,
                       capture_output=True, check=True, timeout=5)

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
                screenshot(kind + "-opened")
                if name:
                    xdo("key", "--clearmodifiers", "ctrl+l")
                    xdo("key", "--clearmodifiers", "ctrl+a")
                    xdo("type", "--clearmodifiers", "--delay", "1", str(selected / name))
                    screenshot(kind + "-entered")
                    # Return in GTK's location entry can navigate into a folder
                    # without accepting it. Activate Zenity's _OK button instead
                    # (both GTK 3 and GTK 4; LC_ALL above fixes the UI language).
                    xdo("key", "--clearmodifiers", "alt+o")
                else:
                    xdo("key", "--clearmodifiers", "Escape")
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
    main()
