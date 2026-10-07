"""Package only the independent harness needed to verify an installed Linux SDK.

Repository file dependencies come from the verifier's explicit `repo / "path"`
expressions. Dynamic repository paths fail packaging instead of producing a
silently incomplete kit. Physim implementation and development headers belong
exclusively to the SDK under test.
"""
import argparse
import ast
import hashlib
import io
import json
from pathlib import Path
import re
import tarfile

ROOT = Path(__file__).resolve().parent.parent
ENTRY_POINTS = ("tools/verify-native-sdk.py", "tools/wait-window-manager.py",
                "tests/test_linux_native_dialogs.py")


def repository_paths(source):
    tree = ast.parse(source)
    paths = set()
    for node in ast.walk(tree):
        if (isinstance(node, ast.BinOp) and isinstance(node.op, ast.Div)
                and isinstance(node.left, ast.Name) and node.left.id == "repo"):
            if not isinstance(node.right, ast.Constant) or not isinstance(node.right.value, str):
                raise ValueError("Verifier repository paths must be explicit string literals")
            paths.add(node.right.value)
    return paths


def inputs(root):
    names = set(ENTRY_POINTS)
    names.update(repository_paths((root / ENTRY_POINTS[0]).read_text(encoding="utf-8")))
    pending = list(names)
    while pending:
        name = pending.pop()
        path = root / name
        if (Path(name).is_absolute() or ".." in Path(name).parts or
                not (name.startswith(("tools/", "tests/")) or name=="app/utf8.manifest") or
                not path.is_file() or root.resolve() not in path.resolve().parents):
            raise ValueError(f"Invalid or missing verification input: {name}")
        if path.suffix in (".c", ".h"):
            for header in re.findall(r'^\s*#\s*include\s*"([^"\n]+)"',path.read_text(encoding="utf-8"),re.M):
                if header.startswith("physim/"):
                    continue  # Public headers must come from the SDK under test.
                local = (Path(name).parent / header).as_posix()
                if local not in names:
                    names.add(local);pending.append(local)
    return sorted(names)


def package(root, output):
    names = inputs(root)
    manifest = {name: hashlib.sha256((root / name).read_bytes()).hexdigest() for name in names}
    output.parent.mkdir(parents=True, exist_ok=True)
    created = False
    try:
        file = output.open("xb")
        created = True
        with file, tarfile.open(fileobj=file, mode="w:gz") as archive:
            for name in names:
                archive.add(root / name, arcname=name, recursive=False)
            data = (json.dumps({"format":1,"files":manifest},indent=2)+"\n").encode("utf-8")
            info = tarfile.TarInfo("verification-kit.json");info.size=len(data);info.mode=0o644
            archive.addfile(info,io.BytesIO(data))
    except FileExistsError:
        raise
    except BaseException:
        if created:
            output.unlink(missing_ok=True)
        raise
    return names


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output",type=Path,required=True)
    args = parser.parse_args()
    names = package(ROOT,args.output)
    print(f"Independent verification kit: {len(names)} files, no Physim implementation: {args.output}")


if __name__ == "__main__":
    main()
