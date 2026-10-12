"""Check real bundled section links using the native Markdown parser, not a slug mirror."""
import argparse
import json
import os
from pathlib import Path
import re
import subprocess


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--probe", type=Path, required=True)
    args = parser.parse_args()
    root = args.root.resolve()
    checked = 0
    failures = []
    ui = root / "app/documentation_ui.inc"
    if ui.is_file():
        topics = {path: code == "true" for path, code in
                  re.findall(r'\{"[^"]+", "([^"]+)", (true|false)\}', ui.read_text(encoding="utf-8"))}
    else:
        catalog = json.loads((root / "tests/documentation_topics.json").read_text(encoding="utf-8"))
        if catalog.get("format") != 1:
            raise SystemExit("Unsupported documentation topic catalog")
        topics = {entry["path"]: entry["code"] for entry in catalog["topics"]}
    for name, code in topics.items():
        result = subprocess.run([str(args.probe.resolve()), "--page", str(root / name), "code" if code else "markdown"],
                                capture_output=True, text=True, timeout=20)
        if result.returncode:
            failures.append(f"Native topic cannot be parsed: {name}: {result.stderr[:200]}")
        if code or not name.endswith(".md"):
            continue
        body = re.sub(r"```[^\n]*\n.*?```", "", (root / name).read_text(encoding="utf-8"), flags=re.S)
        for target in re.findall(r"(?<!!)\[[^\]]*\]\(([^\s)]+)(?:\s+[^)]*)?\)", body):
            if re.match(r"^[a-z]+:", target):
                continue
            path = target.split("#", 1)[0]
            destination = ((root / name).parent / path).resolve() if path else root / name
            if not destination.is_relative_to(root) or destination.relative_to(root).as_posix() not in topics:
                failures.append(f"Native topic link unreachable: {name}: {target}")
    pages = sorted({root / "README.md", *root.glob("docs/**/*.md"), *root.glob("brains/physim/**/*.md"),
                    *(root / name for name in topics if name.endswith(".md"))})
    for page in pages:
        # Discard fenced code; example Markdown is not a navigable page link.
        source = re.sub(r"```[^\n]*\n.*?```", "", page.read_text(encoding="utf-8"), flags=re.S)
        for target in re.findall(r"(?<!!)\[[^\]]*\]\(([^\s)]+)(?:\s+[^)]*)?\)", source):
            if "#" not in target or re.match(r"^[a-z]+:", target):
                continue
            path, anchor = target.split("#", 1)
            destination = (page.parent / path).resolve() if path else page
            if destination.suffix != ".md":
                continue
            if not destination.is_relative_to(root) or not destination.is_file():
                failures.append(f"{page.relative_to(root)}: missing {target}")
                continue
            result = subprocess.run([str(args.probe.resolve()), "--anchor", str(destination), anchor],
                                    capture_output=True, text=True, timeout=20,
                                    env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"})
            if result.returncode:
                failures.append(f"{page.relative_to(root)}: unresolved {target}: {result.stderr[:200]}")
            checked += 1
    if not checked:
        failures.append("No bundled section links were checked")
    if failures:
        raise SystemExit("\n".join(failures))
    print(f"Native documentation anchors: {checked} bundled section links passed")
    print(f"Native documentation index: {len(topics)} pages parsed and internal links reachable")


if __name__ == "__main__":
    main()
