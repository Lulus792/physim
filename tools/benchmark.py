"""Run verified native workloads and archive raw measurements plus host metadata."""
import argparse
import csv
from datetime import datetime, timezone
import hashlib
import io
import json
import math
import os
from pathlib import Path
import platform
import statistics
import subprocess
import sys


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("executable", type=Path)
    parser.add_argument("--output", type=Path, required=True,
                        help="New result directory; existing directories are rejected")
    parser.add_argument("--samples", type=int, default=100000)
    parser.add_argument("--repeats", type=int, default=5)
    parser.add_argument("--baseline", type=Path, help="Result directory from the same host and build configuration")
    parser.add_argument("--max-regression", type=float, default=0.20,
                        help="Allowed median duration increase, default 0.20 (20 percent)")
    args = parser.parse_args()
    executable = args.executable.resolve(strict=True)
    if not 2 <= args.samples <= 1000000 or not 1 <= args.repeats <= 30:
        parser.error("samples must be 2..1000000 and repeats 1..30")
    if not math.isfinite(args.max_regression) or args.max_regression < 0:
        parser.error("max-regression must be finite and nonnegative")
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=False)
    metadata = {
        "schema": 1, "utc": datetime.now(timezone.utc).isoformat(),
        "platform": platform.platform(), "machine": platform.machine(),
        "processor": platform.processor(), "logical_cpus": os.cpu_count(),
        "executable": str(executable),
        "executable_sha256": hashlib.sha256(executable.read_bytes()).hexdigest(),
        "samples": args.samples, "repeats": args.repeats,
        "baseline": str(args.baseline.resolve()) if args.baseline else None,
        "max_regression": args.max_regression if args.baseline else None,
        "cache_policy": "warm OS cache; no cache flushing; all repetitions retained",
        "command": [str(executable), str(args.samples), str(args.repeats)],
    }
    root = Path(__file__).resolve().parent.parent
    sources = [root / "CMakeLists.txt", Path(__file__).resolve()]
    for folder in ("src", "include", "tools", "app", "runners", "tests"):
        sources.extend(p for p in (root / folder).rglob("*") if p.suffix in (".c", ".h"))
    metadata["working_tree_source_sha256"] = {
        str(p.relative_to(root)): hashlib.sha256(p.read_bytes()).hexdigest()
        for p in sorted(set(sources))
    }
    (output / "metadata.json").write_text(json.dumps(metadata, indent=2), encoding="utf-8")
    with (output / "raw.csv").open("wb") as stdout, (output / "stderr.txt").open("wb") as stderr:
        completed = subprocess.run(metadata["command"], cwd=output, stdout=stdout,
                                   stderr=stderr, timeout=3600, check=False)
    if completed.returncode:
        raise RuntimeError(f"Benchmark failed ({completed.returncode}); see {output / 'stderr.txt'}")
    rows = list(csv.DictReader(io.StringIO((output / "raw.csv").read_text(encoding="utf-8"))))
    grouped = {}
    for row in rows:
        grouped.setdefault(row["workload"], []).append(row)
    if not grouped or any(len(group) != args.repeats for group in grouped.values()):
        raise RuntimeError("Missing or incomplete benchmark measurements")
    summary = {}
    for workload, group in grouped.items():
        durations = [float(row["seconds"]) for row in group]
        median = statistics.median(durations)
        summary[workload] = {
            "median_seconds": median, "min_seconds": min(durations),
            "max_seconds": max(durations), "units": int(group[0]["units"]),
            "scratch_bytes": max(int(row["scratch_bytes"]) for row in group),
            "compiler": group[0]["compiler"], "configuration": group[0]["configuration"],
        }
        print(f"{workload}: median {median:.6f} s ({len(group)} repetitions)")
    (output / "summary.json").write_text(json.dumps(summary, indent=2), encoding="utf-8")
    if args.baseline:
        baseline_meta = json.loads((args.baseline / "metadata.json").read_text(encoding="utf-8"))
        baseline = json.loads((args.baseline / "summary.json").read_text(encoding="utf-8"))
        for key in ("schema", "platform", "machine", "processor", "logical_cpus", "samples", "cache_policy"):
            if metadata[key] != baseline_meta[key]:
                raise RuntimeError(f"Incompatible baseline: {key}")
        comparison = {}
        for name, previous in baseline.items():
            current = summary.get(name)
            if current is None or any(current[key] != previous[key]
                                      for key in ("units", "compiler", "configuration")):
                raise RuntimeError(f"Incompatible workload: {name}")
            old = previous["median_seconds"]
            if not math.isfinite(old) or old <= 0:
                raise RuntimeError(f"Invalid baseline duration: {name}")
            ratio = current["median_seconds"] / old
            comparison[name] = {"duration_ratio": ratio, "passed": ratio <= 1 + args.max_regression}
        (output / "comparison.json").write_text(json.dumps(comparison, indent=2), encoding="utf-8")
        regressions = [name for name, result in comparison.items() if not result["passed"]]
        if regressions:
            raise RuntimeError("Performance threshold exceeded: " + ", ".join(regressions))
    print(f"Verified results: {output}")


if __name__ == "__main__":
    try:
        main()
    except (OSError, ValueError, KeyError, TypeError, RuntimeError, subprocess.TimeoutExpired) as error:
        print(error, file=sys.stderr)
        sys.exit(1)
