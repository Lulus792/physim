"""Actual app workflows with opt-in profiling, plus disabled and failure paths."""
import argparse
import importlib.util
import json
import os
from pathlib import Path
import subprocess
import csv
import shutil
import sys

p = argparse.ArgumentParser(description=__doc__)
for name in ("app", "root", "work"):
    p.add_argument("--" + name, type=Path, required=True)
p.add_argument("--cc")
a = p.parse_args()
a.work.mkdir(parents=True, exist_ok=True)
spec = importlib.util.spec_from_file_location("app_profile", a.root / "tools/app_profile.py")
report = importlib.util.module_from_spec(spec)
spec.loader.exec_module(report)


def launch(project, mode, profile, state=False):
    env = dict(os.environ)
    env.pop("PHYSIM_PROFILE_DIR", None)
    if profile:
        env["PHYSIM_PROFILE_DIR"] = str(profile.resolve())
    if a.cc:
        env["PHYSIM_CC"] = a.cc
    arguments = ["--workspace-state-test", str(project.resolve()), "toolbar-keyboard"] if state else ["--self-test", str(project.resolve()), mode]
    r = subprocess.run([str(a.app.resolve()), *arguments],
                       env=env, capture_output=True, timeout=160)
    assert r.returncode == 0 and b"SELF-TEST: PASSED" in r.stdout, (r.stdout, r.stderr)
    return r


summaries = {}
for mode in ("pendulum", "language_full"):
    profile = a.work / (mode + " trace α")
    r = launch(a.work / (mode + " project"), mode, profile)
    (a.work / (mode + " stdout.txt")).write_bytes(r.stdout)
    (a.work / (mode + " stderr.txt")).write_bytes(r.stderr)
    summary = report.analyze(profile, allow_partial=True)
    assert summary["frames"] > 20 and summary["startup_ready_seconds"] > 0
    assert summary["scene_frames"] > 0 and summary["runner_bytes_observed"] > 0
    assert summary["job_bytes_observed"] > 0 and not summary["render_failures"]
    assert summary["captured_frames"] > 0 and summary["uncaptured_rendered_frames"] > 0
    assert summary["user_cpu_seconds_observed"] >= 0 and summary["system_cpu_seconds_observed"] >= 0
    (a.work / (mode + " summary.json")).write_text(json.dumps(summary, indent=2) + "\n")
    summaries[mode] = summary
    if mode == "pendulum":
        for fault in ("nan", "truncated", "stale-scene", "interval", "partial"):
            corrupt = a.work / ("corrupt " + fault)
            shutil.copytree(profile, corrupt)
            with (corrupt / "frames.csv").open(newline="") as file:
                rows = list(csv.DictReader(file))
            if fault == "nan": rows[0]["frame_seconds"] = "nan"
            elif fault == "truncated": rows.pop()
            elif fault == "stale-scene":
                rows[0]["has_scene"] = "0"; rows[0]["scene_vertex_bytes"] = "40"
            elif fault == "interval": rows[0]["event_seconds"] = "1000"
            else:
                rows.pop(1)
                status = json.loads((corrupt / "status.json").read_text())
                status.update(complete=False, written=len(rows), dropped=1)
                (corrupt / "status.json").write_text(json.dumps(status))
            with (corrupt / "frames.csv").open("w", newline="") as file:
                writer = csv.DictWriter(file, fieldnames=rows[0].keys())
                writer.writeheader(); writer.writerows(rows)
            try: report.analyze(corrupt)
            except ValueError: pass
            else: raise AssertionError("Corrupt trace accepted: " + fault)
            if fault == "partial":
                partial = report.analyze(corrupt, allow_partial=True)
                assert not partial["trace_complete"] and partial["dropped"] == 1
        protected = a.work / "existing report.json"
        protected.write_bytes(b"existing owner report")
        reject = subprocess.run([sys.executable, str(a.root / "tools/app_profile.py"), str(profile),
                                 "--output", str(protected), "--allow-partial"], capture_output=True)
        assert reject.returncode != 0 and protected.read_bytes() == b"existing owner report"
    # Completion marker and the original raw trace survive a rejected reuse.
    before = {q.name: q.read_bytes() for q in profile.iterdir() if q.is_file()}
    failed = launch(a.work / (mode + " reused project"), mode, profile, state=True)
    assert b"App profiling failed" in failed.stderr and b"failed=1" in failed.stderr
    assert before == {q.name: q.read_bytes() for q in profile.iterdir() if q.is_file()}
    # Explicitly disabled path uses the original workflow and creates no trace.
    disabled = launch(a.work / (mode + " disabled project"), mode, None, state=True)
    assert b"App profiling:" not in disabled.stderr

(a.work / "results.json").write_text(json.dumps(summaries, indent=2) + "\n")
print("Actual C/Physim app profiling: phases, startup, scene/UI, resources, pipe bytes, disabled path and rejected reuse passed")
