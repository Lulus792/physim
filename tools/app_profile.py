"""Validate an actual app frame trace and summarize CPU, memory and receive rates."""
import argparse
import csv
import hashlib
import json
import math
from pathlib import Path
import statistics


def distribution(values):
    ordered = sorted(values)
    return {"median": statistics.median(ordered),
            "p95": ordered[math.ceil(len(ordered) * .95) - 1],
            "p99": ordered[math.ceil(len(ordered) * .99) - 1], "max": max(ordered)}


def analyze(directory, allow_partial=False):
    raw = directory / "frames.csv"
    status = json.loads((directory / "status.json").read_text(encoding="utf-8"))
    if type(status.get("schema")) is not int or status["schema"] != 1:
        raise ValueError("Unsupported app profile schema")
    if not isinstance(status.get("complete"), bool) or not isinstance(status.get("scope"), str):
        raise ValueError("Invalid app profile status")
    for key in ("written", "dropped"):
        if type(status.get(key)) is not int or status[key] < 0:
            raise ValueError("Invalid app profile count")
    if not status.get("complete") and not allow_partial:
        raise ValueError("Incomplete trace; inspect dropped frames or use --allow-partial")
    with raw.open(encoding="utf-8", newline="") as file:
        rows = list(csv.DictReader(file))
    if not rows or len(rows) != status["written"]:
        raise ValueError("Empty or truncated app frame trace")
    timing = ("time_seconds", "interval_seconds", "frame_seconds", "event_seconds", "work_seconds",
              "ui_seconds", "render_seconds", "present_seconds", "documentation_seconds", "capture_seconds",
              "startup_ready_seconds", "user_cpu_seconds", "system_cpu_seconds", "scene_setup_seconds",
              "scene_tessellation_seconds", "scene_submission_seconds", "ui_conversion_seconds", "ui_upload_seconds")
    counts = ("frame", "peak_resident_bytes", "runner_bytes", "job_bytes", "scene_vertex_bytes",
              "scene_index_bytes", "scene_retained_bytes", "ui_vertex_bytes", "ui_index_bytes")
    previous = None
    for row in rows:
        for key in timing:
            row[key] = float(row[key])
            if not math.isfinite(row[key]) or row[key] < 0:
                raise ValueError("Invalid app timing: " + key)
        for key in counts:
            row[key] = int(row[key])
            if row[key] < 0:
                raise ValueError("Invalid app counter: " + key)
        for key in ("captured", "has_scene", "rendered"):
            if row[key] not in ("0", "1"):
                raise ValueError("Invalid app state: " + key)
            row[key] = row[key] == "1"
        if row["interval_seconds"] <= 0 or row["peak_resident_bytes"] <= 0:
            raise ValueError("Invalid app resource/interval snapshot")
        parts = sum(row[k] for k in ("event_seconds", "work_seconds", "ui_seconds", "render_seconds",
                                    "present_seconds", "documentation_seconds", "capture_seconds"))
        if parts > row["frame_seconds"] + 5e-9 or row["frame_seconds"] > row["interval_seconds"] + 2e-9:
            raise ValueError("Inconsistent app frame intervals")
        if not row["has_scene"] and any(row[k] for k in ("scene_vertex_bytes", "scene_index_bytes", "scene_retained_bytes",
                                                      "scene_setup_seconds", "scene_tessellation_seconds", "scene_submission_seconds")):
            raise ValueError("Stale scene statistics in a frame without a scene")
        if previous:
            if row["frame"] <= previous["frame"] or row["time_seconds"] < previous["time_seconds"]:
                raise ValueError("Nonmonotone frame trace")
            for key in ("user_cpu_seconds", "system_cpu_seconds", "peak_resident_bytes"):
                if row[key] < previous[key]:
                    raise ValueError("Nonmonotone process snapshot")
        previous = row
    if status["complete"] and [r["frame"] for r in rows] != list(range(len(rows))):
        raise ValueError("Complete trace has missing frames")
    if status["dropped"] < 0 or (status["complete"] and status["dropped"]):
        raise ValueError("Inconsistent dropped-frame status")
    first, last = rows[0], rows[-1]
    steady = [r for r in rows if not r["captured"] and r["rendered"]]
    result = {"schema": 1, "trace_complete": status["complete"], "frames": len(rows),
              "dropped": status["dropped"], "scope": status["scope"],
              "startup_ready_seconds": first["startup_ready_seconds"],
              "peak_resident_bytes": last["peak_resident_bytes"],
              "user_cpu_seconds_observed": last["user_cpu_seconds"] - first["user_cpu_seconds"],
              "system_cpu_seconds_observed": last["system_cpu_seconds"] - first["system_cpu_seconds"],
              "captured_frames": sum(r["captured"] for r in rows),
              "render_failures": sum(not r["rendered"] for r in rows),
              "scene_frames": sum(r["has_scene"] for r in rows),
              "runner_bytes_observed": sum(r["runner_bytes"] for r in rows),
              "job_bytes_observed": sum(r["job_bytes"] for r in rows),
              "raw_sha256": hashlib.sha256(raw.read_bytes()).hexdigest(),
              "status_sha256": hashlib.sha256((directory / "status.json").read_bytes()).hexdigest(),
              "uncaptured_rendered_frames": len(steady)}
    result["timings"] = {k: distribution([r[k] for r in steady]) for k in
                         ("frame_seconds", "interval_seconds", "event_seconds", "work_seconds", "ui_seconds",
                          "render_seconds", "present_seconds", "documentation_seconds")} if steady else {}
    result["receive_rates_bytes_per_second"] = {k: distribution([r[k] / r["interval_seconds"] for r in rows])
                                               for k in ("runner_bytes", "job_bytes")}
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("directory", type=Path)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--allow-partial", action="store_true")
    args = parser.parse_args()
    result = analyze(args.directory.resolve(), args.allow_partial)
    with args.output.open("x", encoding="utf-8") as file:
        file.write(json.dumps(result, indent=2) + "\n")
    print(f"App profile verified: {result['frames']} frames, complete={result['trace_complete']}")


if __name__ == "__main__":
    main()
