"""Capture verified OpenGL UI workloads, image references and CPU latency percentiles."""
import argparse
import csv
from datetime import datetime, timezone
import hashlib
import json
import math
import os
from pathlib import Path
import platform
import statistics
import subprocess
import sys


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def percentile(values, fraction):
    ordered = sorted(values)
    return ordered[max(0, math.ceil(len(ordered) * fraction) - 1)]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("executable", type=Path)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--smoke", action="store_true")
    args = parser.parse_args()
    executable = args.executable.resolve(strict=True)
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=False)
    native = output / "native"
    command = [str(executable), str(native)] + (["--smoke"] if args.smoke else [])
    metadata = {
        "schema": 2, "utc": datetime.now(timezone.utc).isoformat(),
        "platform": platform.platform(), "processor": platform.processor(),
        "logical_cpus": os.cpu_count(), "command": command,
        "executable_sha256": digest(executable),
        "window": "1080x740, hidden, default Nuklear font, no vsync",
        "warmup_frames": 10, "measured_frames": 6 if args.smoke else 300,
        "timing": "render wall/user/system CPU; construction and swap wall time separate; frame sum excludes event pumping, instrumentation and captures; no GPU completion timing",
        "resource_scope": "current native process, all threads, excludes children; peak resident bytes are lifetime high-water mark",
        "percentiles": "nearest rank",
    }
    root = Path(__file__).resolve().parent.parent
    sources = [root / "tools/build.py", root / "tools/benchmark_build.h",
               root / "tools/ui_benchmark.c", root / "tools/benchmark_usage.h",
               root / "src/platform.c", root / "src/platform.h", Path(__file__).resolve()]
    metadata["working_tree_source_sha256"] = {
        path.relative_to(root).as_posix(): digest(path) for path in sorted(
            sources
            + [p for p in (root / "app").iterdir() if p.suffix in (".c", ".h", ".inc")])
    }
    (output / "metadata.json").write_text(json.dumps(metadata, indent=2), encoding="utf-8")
    with (output / "stdout.txt").open("wb") as stdout, (output / "stderr.txt").open("wb") as stderr:
        result = subprocess.run(command, stdout=stdout, stderr=stderr, timeout=120, check=False)
    if result.returncode:
        raise RuntimeError(f"UI workload failed ({result.returncode}); see {output / 'stderr.txt'}")
    with (native / "frames.csv").open(encoding="utf-8", newline="") as source:
        rows = list(csv.DictReader(source))
    if len(rows) != 3 * metadata["measured_frames"]:
        raise RuntimeError("Missing or unexpected UI measurements")
    previous_peak = 0
    for row in rows:
        peak = int(row["peak_resident_bytes"])
        if peak <= 0 or peak < previous_peak:
            raise RuntimeError("Invalid process lifetime peak memory")
        previous_peak = peak
        total = sum(float(row[key]) for key in ("construction_seconds", "total_seconds", "present_seconds"))
        if not math.isclose(total, float(row["frame_seconds"]), abs_tol=3e-9):
            raise RuntimeError("Inconsistent frame timing")
    summary = {}
    for name in ("empty", "dense", "plot"):
        selected = [row for row in rows if row["workload"] == name]
        if len(selected) != metadata["measured_frames"]:
            raise RuntimeError(f"Incomplete workload: {name}")
        if sorted(int(row["frame"]) for row in selected) != list(range(metadata["measured_frames"])):
            raise RuntimeError(f"Duplicate or missing frames: {name}")
        if any(int(row["allocations"]) for row in selected):
            raise RuntimeError(f"Steady-state allocation: {name}")
        first = native / f"{name}-first.bmp"
        if first.read_bytes() != (native / f"{name}-last.bmp").read_bytes():
            raise RuntimeError(f"Repeated frame changed pixels: {name}")
        summary[name] = {"image_sha256": digest(first), "allocations": 0}
        for metric in ("total_seconds", "conversion_seconds", "upload_seconds", "construction_seconds",
                       "present_seconds", "frame_seconds", "user_cpu_seconds", "system_cpu_seconds"):
            values = [float(row[metric]) for row in selected]
            if any(not math.isfinite(v) or v < 0 for v in values):
                raise RuntimeError(f"Invalid timing: {name}/{metric}")
            summary[name][metric] = {"median": statistics.median(values),
                                     "p95": percentile(values, .95),
                                     "p99": percentile(values, .99)}
        for metric in ("vertex_bytes", "index_bytes", "retained_bytes", "peak_resident_bytes"):
            summary[name][metric] = max(int(row[metric]) for row in selected)
        print(f"{name}: median conversion {summary[name]['conversion_seconds']['median'] * 1000:.4f} ms, zero allocations")
    original = (native / "empty-first.bmp").read_bytes()
    for filename in ("empty-after-export.bmp", "empty-restored.bmp"):
        if (native / filename).read_bytes() != original:
            raise RuntimeError(f"Export/resize changed the subsequent frame: {filename}")
    (output / "summary.json").write_text(json.dumps(summary, indent=2), encoding="utf-8")
    print(f"UI rendering verified: {output}")


if __name__ == "__main__":
    try:
        main()
    except (OSError, ValueError, KeyError, RuntimeError, subprocess.TimeoutExpired) as error:
        print(error, file=sys.stderr)
        sys.exit(1)
