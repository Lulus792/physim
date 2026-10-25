"""Check local storage exclusions without ever resolving a forbidden location."""
import argparse
import importlib.util
import json
from pathlib import Path
import tempfile
from unittest.mock import patch


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--work", type=Path, required=True)
    args = parser.parse_args()
    args.work.mkdir(parents=True, exist_ok=True)
    spec = importlib.util.spec_from_file_location(
        "artifact_paths", Path(__file__).resolve().parent.parent / "tools/artifact_paths.py")
    storage = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(storage)
    with tempfile.TemporaryDirectory(prefix="storage policy ä ", dir=args.work) as folder:
        project = Path(folder).resolve()
        storage.PROJECT = project
        assert storage.default_root() == project / "build"
        config = project / ".physim-storage.json"
        config.write_text(json.dumps({"version": 1, "artifact_root": "artifacts",
                                      "blocked_roots": ["F:/", "/mnt/f"]}), encoding="utf-8")
        assert storage.default_root() == project / "artifacts"
        assert storage.checked(project / "allowed") == project / "allowed"
        forbidden = ["F:/Physim", "f:/Physim", "F:relative", "F:\\Physim",
                     "\\\\?\\F:\\Physim", "/mnt/f", "/mnt/f/Physim",
                     "/mnt/f/../d/Physim"]
        for value in forbidden:
            # Enforce rejection before touching an unavailable/retired drive.
            with patch.object(Path, "resolve", side_effect=AssertionError("Forbidden path resolved")):
                try:
                    storage.checked(value)
                except ValueError as error:
                    assert "blocks this location" in str(error), str(error)
                else:
                    raise AssertionError("Forbidden location accepted: " + value)
        # A resolved junction target must also respect the machine policy.
        with patch.object(Path, "resolve", return_value=Path("/mnt/f/Physim")):
            try:
                storage.checked(project / "junction")
            except ValueError:
                pass
            else:
                raise AssertionError("Forbidden resolved location accepted")
        restore_spec = importlib.util.spec_from_file_location(
            "restore_archive", Path(__file__).resolve().parent.parent / "tools/restore-test-archive.py")
        restore = importlib.util.module_from_spec(restore_spec)
        restore_spec.loader.exec_module(restore)
        restore.artifact_paths = storage
        for source, destination in ((Path("F:/archive"), project / "output"),
                                    (project / "archive", Path("/mnt/f/output"))):
            with patch.object(restore, "digest", side_effect=AssertionError("Archive accessed")):
                try:
                    restore.restore(source, destination, "run")
                except ValueError as error:
                    assert "blocks this location" in str(error), str(error)
                else:
                    raise AssertionError("Archive used a forbidden location")
        config.write_text('{"version": 99}', encoding="utf-8")
        try:
            storage.default_root()
        except ValueError as error:
            assert "Unsupported" in str(error)
        else:
            raise AssertionError("Unknown policy version accepted")
    print("Storage defaults, 8 lexical exclusions, resolved exclusion, archive exclusions and version check passed")


if __name__ == "__main__":
    main()
