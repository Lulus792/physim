"""Restore selected original test files from a verified local storage archive."""
import argparse
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import shutil
import sys
import tempfile
import zipfile


def digest(path):
    h = hashlib.sha256()
    with path.open("rb") as stream:
        while block := stream.read(4 * 1024 * 1024):
            h.update(block)
    return h.hexdigest()


def relative_name(value):
    path = PurePosixPath(value)
    if not value or path.is_absolute() or ".." in path.parts or "\\" in value or ":" in value:
        raise ValueError("Archive path escapes its destination")
    return path


def restore(archive_folder, destination, prefix):
    archive = archive_folder / "files.zip"
    index = archive_folder / "index.jsonl"
    receipt = json.loads((archive_folder / "verified.json").read_text(encoding="utf-8"))
    if digest(archive) != receipt["archive_sha256"] or digest(index) != receipt["index_sha256"]:
        raise ValueError("Archive or index checksum mismatch")
    prefix = relative_name(prefix).as_posix().rstrip("/")
    records = []
    with index.open(encoding="utf-8") as stream:
        for line in stream:
            row = json.loads(line)
            name = relative_name(row["path"]).as_posix()
            if name == prefix or name.startswith(prefix + "/"):
                records.append(row)
    if not records:
        raise ValueError("No archived files match the requested prefix")
    destination = destination.resolve()
    source = Path(__file__).resolve().parent.parent
    if destination == Path(destination.anchor) or destination == source or destination in source.parents:
        raise ValueError("Choose a dedicated output directory")
    restored = existing = 0
    with zipfile.ZipFile(archive) as zipped:
        for row in records:
            target = destination / row["path"]
            if target.is_symlink() or not target.resolve().is_relative_to(destination):
                raise ValueError("Destination contains an escaping link")
            if target.exists():
                if not target.is_file() or digest(target) != row["sha256"]:
                    raise FileExistsError(f"Existing file differs: {target}")
                existing += 1
                continue
            target.parent.mkdir(parents=True, exist_ok=True)
            handle, name = tempfile.mkstemp(prefix=".physim-restore-", dir=target.parent)
            temporary = Path(name)
            try:
                with os.fdopen(handle, "wb") as output, zipped.open(row["path"]) as original:
                    shutil.copyfileobj(original, output, 4 * 1024 * 1024)
                if temporary.stat().st_size != row["bytes"] or digest(temporary) != row["sha256"]:
                    raise ValueError("Restored bytes differ from the original")
                os.utime(temporary, ns=(row["mtime_ns"], row["mtime_ns"]))
                # A hard link publishes atomically without replacing an existing file.
                os.link(temporary, target)
                restored += 1
            finally:
                temporary.unlink(missing_ok=True)
    return {"restored": restored, "already_identical": existing}


def main():
    for stream in (sys.stdout, sys.stderr):
        if hasattr(stream, "reconfigure"):
            stream.reconfigure(encoding="utf-8", errors="replace")
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("archive_folder", type=Path)
    parser.add_argument("--destination", type=Path, required=True)
    parser.add_argument("--prefix", required=True, help="Relative run, directory or file to restore")
    args = parser.parse_args()
    try:
        print(json.dumps(restore(args.archive_folder, args.destination, args.prefix)))
        return 0
    except (OSError, ValueError, KeyError, zipfile.BadZipFile) as error:
        print(f"Cannot restore test archive: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
