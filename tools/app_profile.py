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
    if type(status.get("schema")) is not int or status["schema"] not in (1, 2):
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
    if status["dropped"] == 0 and [r["frame"] for r in rows] != list(range(len(rows))):
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
    result["processes"] = []
    if status["schema"] == 2:
        for key in ("process_written", "process_dropped"):
            if type(status.get(key)) is not int or status[key] < 0:
                raise ValueError("Invalid process record count")
        if status["complete"] and status["process_dropped"]:
            raise ValueError("Complete trace has dropped process records")
        process_file = directory / "processes.csv"
        with process_file.open(encoding="utf-8", newline="") as file:
            processes = list(csv.DictReader(file))
        if len(processes) != status["process_written"]:
            raise ValueError("Truncated process trace")
        previous_sequence = -1
        previous_time = 0
        for record in processes:
            for key in ("sequence", "process_id", "kind", "exit_code", "usage_scope"):
                record[key] = int(record[key])
            record["time_seconds"] = float(record["time_seconds"])
            if record["sequence"] <= previous_sequence or record["process_id"] <= 0 or record["kind"] < 0:
                raise ValueError("Invalid process identity/order")
            if not math.isfinite(record["time_seconds"]) or record["time_seconds"] < previous_time:
                raise ValueError("Invalid process timestamp")
            if record["usage_scope"] not in (0, 1, 2):
                raise ValueError("Unsupported process accounting scope")
            for key in ("timed_out", "usage_available"):
                if record[key] not in ("0", "1"):
                    raise ValueError("Invalid process state")
                record[key] = record[key] == "1"
            for key in ("user_cpu_seconds", "system_cpu_seconds", "peak_resident_bytes"):
                if not record["usage_available"]:
                    if record[key]: raise ValueError("Unavailable process usage fabricated")
                    record[key] = None
                else:
                    record[key] = int(record[key]) if key == "peak_resident_bytes" else float(record[key])
                    if not math.isfinite(record[key]) or record[key] < 0:
                        raise ValueError("Invalid process usage")
            if record["usage_available"] and (record["usage_scope"] == 0 or record["peak_resident_bytes"] == 0):
                raise ValueError("Missing available process accounting")
            previous_sequence, previous_time = record["sequence"], record["time_seconds"]
        if not status["process_dropped"] and [r["sequence"] for r in processes] != list(range(len(processes))):
            raise ValueError("Missing process records")
        result["processes"] = processes
        result["process_records_dropped"] = status["process_dropped"]
        result["process_resources_unavailable"] = sum(not r["usage_available"] for r in processes)
        result["process_raw_sha256"] = hashlib.sha256(process_file.read_bytes()).hexdigest()
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
