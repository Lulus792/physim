"""Check that tutorial code blocks match the sources exercised by integration tests."""
import argparse
import json
from pathlib import Path


def verify(root, record):
    # read_text normalizes CRLF and lone CR, as the previous checks did.
    tutorial = (root / record["document"]).read_text(encoding="utf-8")
    for source in record["sources"]:
        content = (root / source["path"]).read_text(encoding="utf-8")
        block = "```" + source["language"] + "\n" + content + "```"
        if block not in tutorial:
            raise RuntimeError(f"{record['document']}: code block differs from tested source {source['path']}")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("group")
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parent.parent)
    args = parser.parse_args()
    catalog = json.loads((args.root / "tests/tutorial_sources.json").read_text(encoding="utf-8"))
    if args.group not in catalog:
        parser.error(f"Unknown tutorial group: {args.group}")
    verify(args.root, catalog[args.group])
    print(f"{args.group}: tutorial code matches its tested sources")


if __name__ == "__main__":
    main()
