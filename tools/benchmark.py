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
        "schema": 2, "utc": datetime.now(timezone.utc).isoformat(),
        "platform": platform.platform(), "machine": platform.machine(),
        "processor": platform.processor(), "logical_cpus": os.cpu_count(),
        "executable": str(executable),
        "executable_sha256": hashlib.sha256(executable.read_bytes()).hexdigest(),
        "samples": args.samples, "repeats": args.repeats,
        "baseline": str(args.baseline.resolve()) if args.baseline else None,
        "max_regression": args.max_regression if args.baseline else None,
        "cache_policy": "warm OS cache; no cache flushing; all repetitions retained",
        "resource_scope": "current native process, all threads, excludes children; peak resident bytes are lifetime high-water mark",
        "io_scope": "logical run file bytes including metadata/chunks/footer; not physical device traffic",
        "command": [str(executable), str(args.samples), str(args.repeats)],
    }
    root = Path(__file__).resolve().parent.parent
    sources = [root / "tools/build.py", Path(__file__).resolve()]
    for folder in ("src", "include", "tools", "app", "runners", "tests"):
        sources.extend(p for p in (root / folder).rglob("*") if p.suffix in (".c", ".h"))
    metadata["working_tree_source_sha256"] = {
        p.relative_to(root).as_posix(): hashlib.sha256(p.read_bytes()).hexdigest()
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
    expected = {"write_16_channels", "read_validate_16_channels", "analysis_snapshot",
                "series_statistics", "series_derivative", "report_preview_8_curves",
                "report_curve_copy", "report_curve_view"}
    if set(grouped) != expected:
        raise RuntimeError("Missing or unexpected benchmark workloads")
    previous_peak = 0
    for row in rows:
        peak = int(row["peak_resident_bytes"])
        if peak <= 0 or peak < previous_peak:
            raise RuntimeError("Invalid process lifetime peak memory")
        previous_peak = peak
        for key in ("seconds", "units_per_second", "user_cpu_seconds", "system_cpu_seconds", "bytes_per_second"):
            value = float(row[key])
            if not math.isfinite(value) or value < 0:
                raise RuntimeError(f"Invalid measurement: {key}")
        if int(row["units"]) <= 0 or int(row["io_bytes"]) < 0 or int(row["scratch_bytes"]) < 0:
            raise RuntimeError("Invalid workload count")
    summary = {}
    for workload, group in grouped.items():
        if sorted(int(row["repeat"]) for row in group) != list(range(args.repeats)):
            raise RuntimeError(f"Duplicate or missing repetitions: {workload}")
        if any(row[key] != group[0][key] for row in group for key in
               ("units", "io_bytes", "compiler", "configuration")):
            raise RuntimeError(f"Inconsistent workload: {workload}")
        durations = [float(row["seconds"]) for row in group]
        median = statistics.median(durations)
        summary[workload] = {
            "median_seconds": median, "min_seconds": min(durations),
            "max_seconds": max(durations), "units": int(group[0]["units"]),
            "scratch_bytes": max(int(row["scratch_bytes"]) for row in group),
            "compiler": group[0]["compiler"], "configuration": group[0]["configuration"],
        }
        for key in ("user_cpu_seconds", "system_cpu_seconds", "bytes_per_second"):
            values = [float(row[key]) for row in group]
            summary[workload][key] = {"median": statistics.median(values),
                                      "min": min(values), "max": max(values)}
        summary[workload]["peak_resident_bytes"] = max(int(row["peak_resident_bytes"]) for row in group)
        summary[workload]["io_bytes"] = int(group[0]["io_bytes"])
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
