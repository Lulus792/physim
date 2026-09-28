"""Wait for a live EWMH window manager before starting tests in an Xvfb session."""
import argparse
import re
import subprocess
import time


def property_text(*arguments):
    try:
        result = subprocess.run(["xprop", *arguments], capture_output=True, text=True, timeout=1)
    except subprocess.TimeoutExpired:
        return ""
    return result.stdout if result.returncode == 0 else ""


def ready():
    # EWMH requires both the root and the support window to point at the same
    # live support window; a stale root property alone does not prove readiness.
    # https://specifications.freedesktop.org/wm/1.5/ar01s03.html
    root = property_text("-root", "_NET_SUPPORTING_WM_CHECK")
    match = re.search(r"window id # (0x[0-9a-fA-F]+)", root)
    if not match or int(match[1], 16) == 0:
        return False
    child = property_text("-id", match[1], "_NET_SUPPORTING_WM_CHECK")
    child_match = re.search(r"window id # (0x[0-9a-fA-F]+)", child)
    return bool(child_match and int(child_match[1], 16) == int(match[1], 16))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--timeout", type=float, default=10)
    args = parser.parse_args()
    if not 0 < args.timeout <= 60:
        parser.error("timeout must be greater than zero and at most 60 seconds")
    deadline = time.monotonic() + args.timeout
    while time.monotonic() < deadline:
        if ready():
            print("X11 window manager is ready")
            return
        time.sleep(.1)
    raise RuntimeError("No live EWMH window manager appeared; inspect the Openbox log")


if __name__ == "__main__":
    main()
