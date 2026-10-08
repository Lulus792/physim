"""Verify production scene geometry and archive CPU/server timings and images."""
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


def summary(values):
    ordered = sorted(values)
    if any(not math.isfinite(x) or x < 0 for x in ordered):
        raise RuntimeError("Invalid scene timing")
    return {"median": statistics.median(ordered),
            "p95": ordered[math.ceil(len(ordered) * .95) - 1],
            "p99": ordered[math.ceil(len(ordered) * .99) - 1]}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("executable", type=Path)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--smoke", action="store_true")
    parser.add_argument("--no-gpu", action="store_true")
    args = parser.parse_args()
    executable = args.executable.resolve(strict=True)
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=False)
    native = output / "native"
    command = [str(executable), str(native)]
    if args.smoke: command.append("--smoke")
    if args.no_gpu: command.append("--no-gpu")
    root = Path(__file__).resolve().parent.parent
    sources = [root / "tools/build.py", root / "tools/scene_benchmark.c",
               root / "tools/benchmark_build.h", root / "tools/benchmark_usage.h",
               root / "src/platform.c", root / "src/platform.h", Path(__file__).resolve()]
    sources += [p for p in (root / "app").iterdir() if p.suffix in (".c", ".h", ".inc")]
    sources += [p for folder in ("src", "include") for p in (root / folder).rglob("*")
                if p.suffix in (".c", ".h", ".inc")]
    metadata = {"schema": 1, "utc": datetime.now(timezone.utc).isoformat(),
                "platform": platform.platform(), "processor": platform.processor(),
                "logical_cpus": os.cpu_count(), "command": command,
                "executable_sha256": digest(executable),
                "working_tree_source_sha256": {p.relative_to(root).as_posix(): digest(p) for p in sorted(set(sources))},
                "window": "640x480, hidden, no vsync, no grid, fixed camera",
                "warmup_frames": 5, "measured_frames": 6 if args.smoke else 60,
                "gpu_disabled": args.no_gpu,
                "cpu_scope": "scene call only; all process threads for CPU; lifetime process peak RSS",
                "gpu_scope": "inclusive GL_TIME_ELAPSED server interval around scene call; includes stalls, excludes UI/capture/swap; serialized query collection outside CPU interval",
                "percentiles": "nearest rank"}
    (output / "metadata.json").write_text(json.dumps(metadata, indent=2) + "\n", encoding="utf-8")
    with (output / "stdout.txt").open("wb") as stdout, (output / "stderr.txt").open("wb") as stderr:
        result = subprocess.run(command, stdout=stdout, stderr=stderr, timeout=180)
    if result.returncode:
        raise RuntimeError(f"Scene workload failed ({result.returncode}); see {output / 'stderr.txt'}")
    with (native / "frames.csv").open(encoding="utf-8", newline="") as file:
        rows = list(csv.DictReader(file))
    names = ("empty", "spheres", "translucent", "path")
    if len(rows) != 4 * metadata["measured_frames"] or {r["workload"] for r in rows} != set(names):
        raise RuntimeError("Incomplete or unexpected scene measurements")
    previous_peak = 0
    availability = {r["gpu_available"] for r in rows}
    if len(availability) != 1 or not availability <= {"0", "1"}:
        raise RuntimeError("Inconsistent GPU availability")
    gpu = availability == {"1"}
    if args.no_gpu and gpu: raise RuntimeError("Disabled timer reported as available")
    for row in rows:
        peak = int(row["peak_resident_bytes"])
        if peak <= 0 or peak < previous_peak: raise RuntimeError("Invalid lifetime peak RAM")
        previous_peak = peak
        parts = sum(float(row[k]) for k in ("setup_seconds", "tessellation_seconds", "submission_seconds"))
        if parts > float(row["total_seconds"]) + 3e-9: raise RuntimeError("Inconsistent scene intervals")
        if not gpu and (row["gpu_seconds"] or float(row["query_wait_seconds"]) != 0):
            raise RuntimeError("Unavailable GPU timing fabricated")
    results = {}
    for name in names:
        selected = [r for r in rows if r["workload"] == name]
        if sorted(int(r["frame"]) for r in selected) != list(range(metadata["measured_frames"])):
            raise RuntimeError("Duplicate or missing scene frames")
        first = native / (name + "-first.bmp")
        if first.read_bytes() != (native / (name + "-last.bmp")).read_bytes():
            raise RuntimeError("Scene pixels changed on repetition: " + name)
        result = {"image_sha256": digest(first), "gpu_available": gpu}
        for key in ("total_seconds", "setup_seconds", "tessellation_seconds", "submission_seconds",
                    "user_cpu_seconds", "system_cpu_seconds", "query_wait_seconds"):
            result[key] = summary([float(r[key]) for r in selected])
        result["gpu_seconds"] = summary([float(r["gpu_seconds"]) for r in selected]) if gpu else None
        for key in ("vertices", "vertex_bytes", "index_bytes", "retained_bytes"):
            values = {int(r[key]) for r in selected}
            if len(values) != 1 or min(values) < 0: raise RuntimeError("Unstable geometry: " + key)
            result[key] = values.pop()
        if result["retained_bytes"] < result["vertex_bytes"] + result["index_bytes"]:
            raise RuntimeError("Invalid retained geometry capacity")
        result["peak_resident_bytes"] = max(int(r["peak_resident_bytes"]) for r in selected)
        results[name] = result
        print(f"{name}: {result['vertices']} vertices, median tessellation {result['tessellation_seconds']['median'] * 1000:.4f} ms, GPU available={gpu}")
    if len({v["image_sha256"] for v in results.values()}) != 4:
        raise RuntimeError("Distinct reference scenes produced identical images")
    (output / "summary.json").write_text(json.dumps(results, indent=2) + "\n", encoding="utf-8")
    print(f"Scene rendering verified: {output}")


if __name__ == "__main__":
    try:
        main()
    except (OSError, ValueError, KeyError, RuntimeError, subprocess.TimeoutExpired) as error:
        print(error, file=sys.stderr)
        sys.exit(1)
