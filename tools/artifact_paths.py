"""Project-local artifact paths and optional machine storage restrictions."""
import json
from pathlib import Path, PureWindowsPath

PROJECT = Path(__file__).resolve().parent.parent


def policy():
    path = PROJECT / ".physim-storage.json"
    if not path.is_file():
        return {}
    value = json.loads(path.read_text(encoding="utf-8-sig"))
    if value.get("version") != 1:
        raise ValueError("Unsupported local storage configuration")
    return value


def checked(path):
    """Reject blocked paths lexically before resolving or creating directories."""
    rules = policy()

    def reject(candidate):
        raw = str(candidate).replace("\\", "/").casefold().rstrip("/")
        if raw.startswith("//?/"):
            raw = raw[4:]
        drive = PureWindowsPath(raw).drive.casefold()
        for blocked in rules.get("blocked_roots", []):
            prefix = blocked.replace("\\", "/").casefold().rstrip("/")
            if raw == prefix or raw.startswith(prefix + "/") or (len(prefix) == 2 and prefix[1] == ":" and drive == prefix):
                raise ValueError("The local storage configuration blocks this location: " + str(candidate))

    path = Path(path).expanduser()
    reject(path)
    result = path.resolve()
    reject(result)
    return result


def default_root():
    return checked(PROJECT / policy().get("artifact_root", "build"))
