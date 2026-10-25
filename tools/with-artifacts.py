"""Run a development command with build, cache and temporary storage in one root."""
import argparse
import json
import os
from pathlib import Path
import subprocess
import sys
import artifact_paths


def main():
    for stream in (sys.stdout, sys.stderr):
        if hasattr(stream, "reconfigure"):
            stream.reconfigure(encoding="utf-8", errors="replace")
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path,
                        help="Artifact directory; defaults to this project's build directory")
    parser.add_argument("--check", action="store_true", help="Print the child environment without running a command")
    parser.add_argument("command", nargs=argparse.REMAINDER)
    args = parser.parse_args()
    try:
        root = artifact_paths.checked(args.root) if args.root else artifact_paths.default_root()
    except (OSError, ValueError) as error:
        parser.error(str(error))
    source = Path(__file__).resolve().parent.parent
    if not Path(root.anchor).is_dir():
        parser.error("The artifact drive is unavailable")
    if sys.platform == "linux" and len(root.parts) >= 3 and root.parts[1] == "mnt" and len(root.parts[2]) == 1:
        if not os.path.ismount(Path(*root.parts[:3])):
            parser.error("The requested Windows drive is not mounted in WSL")
    if root == Path(root.anchor) or root == source or root in source.parents:
        parser.error("Choose a dedicated artifact directory, not a drive or source root")
    command = args.command[1:] if args.command[:1] == ["--"] else args.command
    if not args.check and not command:
        parser.error("Pass a command after --, or use --check")
    environment = dict(os.environ)
    folders = {"PHYSIM_ARTIFACT_ROOT": root,
               "TMPDIR": root / "tmp" / sys.platform,
               "XDG_CACHE_HOME": root / "cache" / sys.platform,
               "PYTHONPYCACHEPREFIX": root / "cache" / sys.platform / "python"}
    for key, directory in folders.items():
        directory.mkdir(parents=True, exist_ok=True)
        environment[key] = str(directory)
    environment["TMP"] = environment["TEMP"] = environment["TMPDIR"]
    # Keep XDG_RUNTIME_DIR unchanged: Unix sockets require a native runtime directory.
    if args.check:
        print(json.dumps({key: environment[key] for key in
                          (*folders, "TMP", "TEMP")}, indent=2))
        return 0
    try:
        return subprocess.call(command, env=environment)
    except OSError as error:
        print(f"Cannot start development command: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
