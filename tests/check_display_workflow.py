"""Run the app's window workflows and check their persisted files without CMake."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import struct
import sys
import zlib


LANGUAGE_MODES = ("full", "mixed", "projectile", "sensors", "body", "contact", "joint",
                  "graph", "sweep", "spring", "buoyancy", "collision", "box_collision")
SIMPLE = {
    "floating_workflow": (["--self-test", "{directory}", "buoyancy"], 150, True),
    "plot_workflow": (["--plot-test", "{directory}"], 35, True),
    "plot_input_isolation": (["--plot-test-noise", "{directory}"], 35, True),
    "graphics": (["--renderer-test"], 30, False),
    "batch_workflow": (["--batch-test", "{directory}"], 160, True),
    "window_smoke": (["--smoke"], 30, False),
    "workspace_workflow": (["--workspace-test", "{directory}"], 30, True),
    "workspace_tree_workflow": (["--workspace-state-test", "{directory}", "tree"], 25, True),
    "documents_build": (["--workspace-state-test", "{directory}", "documents-build"], 130, False),
    "documentation_window": (["--docs-test", "{directory}"], 30, True),
}
for size in ("small", "large"):
    for name, mode in (("toolbar", "toolbar"), ("documents", "documents")):
        SIMPLE[f"{name}_{size}"] = (["--workspace-state-test", "{directory}", mode], 25, size == "small")
for name, fixture in (("unicode", "unicode_identifiers"), ("multiline", "multiline_strings")):
    SIMPLE[f"language_editor_{name}_preview"] = (["--syntax-preview-test", "{directory}",
        "{root}/tests/fixtures/language/" + fixture + ".phys"], 30, False)


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def read(path):
    return path.read_text(encoding="utf-8")


def write(path, text):
    path.write_text(text, encoding="utf-8", newline="\n")


def exact(path, expected):
    require(read(path) == expected, f"Unexpected contents: {path}")


def fingerprint(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


class Workflow:
    def __init__(self, app, work, root, small, compiler=None, trace=False):
        self.app, self.work, self.root = app, work, root
        self.trace = trace
        self.env = dict(os.environ)
        self.env["PHYSIM_TEST_TRACE"] = "1"
        self.env.pop("PHYSIM_TEST_SMALL", None)
        if small:
            self.env["PHYSIM_TEST_SMALL"] = "1"
        if compiler:
            self.env["PHYSIM_CC"] = compiler
        self.steps = []

    def run(self, *arguments, timeout=25, marker=None, expected_exit_code=0):
        command = [str(self.app), *map(str, arguments)]
        step = dict(command=command)
        if self.trace:
            require(sys.platform == "linux", "System call tracing requires Linux and strace >= 6.6")
            trace = self.work / f"app-{len(self.steps) + 1:03d}.strace"
            # EXITKILL also stops the traced app/children when subprocess.run
            # kills the tracer on timeout; no retry can hide the original hang.
            command = ["strace", "-f", "--kill-on-exit", "-tt", "-T", "-s", "128",
                       "-e", "trace=%process,%network,poll,ppoll,select,pselect6,futex",
                       "-o", str(trace), "--", *command]
            step.update(trace_command=command, syscall_trace=str(trace))
        try:
            result = subprocess.run(command, cwd=self.work, env=self.env, capture_output=True,
                                    timeout=timeout, encoding="utf-8", errors="replace")
            step.update(exit_code=result.returncode, stdout=result.stdout, stderr=result.stderr)
        except subprocess.TimeoutExpired as error:
            def output(value):
                return value.decode("utf-8", errors="replace") if isinstance(value, bytes) else value or ""
            step.update(status="timeout", stdout=output(error.stdout), stderr=output(error.stderr))
            print(step["stdout"], end="", flush=True)
            print(step["stderr"], end="", file=sys.stderr, flush=True)
            raise RuntimeError(f"App exceeded {timeout} seconds: {command}") from error
        except OSError as error:
            step.update(status="launch_failed", reason=str(error))
            raise
        finally:
            self.steps.append(step)
            (self.work / "app-steps.json").write_text(json.dumps(self.steps, indent=2) + "\n", encoding="utf-8")
        print(result.stdout, end="", flush=True)
        print(result.stderr, end="", file=sys.stderr, flush=True)
        require(result.returncode == expected_exit_code, f"App exited {result.returncode}, expected {expected_exit_code}: {command}")
        require(marker is None or marker in result.stdout, f"Missing success marker {marker!r}: {command}")


def project_settings(flow, directory):
    project = directory / "project"
    sources = [project / "main.phys", project / "analysis.c"]
    for mode in ("write", "read", "debug"):
        flow.run("--workspace-state-test", directory, f"project-settings-{mode}", timeout=120)
        output = flow.steps[-1]["stdout"]
        require(re.search(r"PROJECT SETTINGS .*PASSED", output), "Missing project settings success marker")
        description = read(project / "physim.project")
        for value in ("simulation.dt=0.125", "simulation.seed=18446744073709551615", "parameter.friction=0.25"):
            require(value in description, f"Project setting was lost: {value}")
        require(f"profile={'Release' if mode == 'write' else 'Debug'}" in description, "Wrong project profile")
        if mode == "write":
            with (project / "physim.project").open("a", encoding="utf-8", newline="\n") as file:
                file.write("# extension preserved\ncustom.note=hello\n")
            hashes = [fingerprint(path) for path in sources]
        else:
            require("# extension preserved\ncustom.note=hello" in description, "Project extension lost")
            require(hashes == [fingerprint(path) for path in sources], "Settings rewrote sources")
        require(not any((project / name).exists() for name in ("main.phys.bak", "analysis.c.bak", "CMakeLists.txt")),
                "Settings created source backups or CMake files")
    require("profile=Release" in read(project / "build/Release/build.config"), "Restored profile was not built")


def timed_series(flow, directory, scaled=False):
    records = []
    for language in ("c", "phys"):
        root = directory / language
        root.mkdir(parents=True)
        mode=("series-cm-" if scaled else "series-")+language
        flow.run("--workspace-state-test", root, mode, timeout=130,
                 marker="TIMED SERIES " + mode + " SELF-TEST: PASSED")
        project = root / "project"
        directories = list((project / "runs").glob("*-batch"))
        require(len(directories) == 1, "Invalid target input created an extra series")
        batch = directories[0]
        manifest = read(batch / "series.txt")
        require("physim_batch=4" in manifest and "step_mode=adaptive" in manifest, "Timed series configuration missing")
        require("parameter=length" in manifest and "parameter_start=0.5" in manifest and "parameter_end=2.5" in manifest,
                "Pendulum length study not recorded")
        png = (batch / "study.png").read_bytes()
        require(png[:8] == b"\x89PNG\r\n\x1a\n" and struct.unpack(">II", png[16:24]) == (1200, 850),
                "Study PNG dimensions or signature differ")
        svg = read(batch / "study.svg")
        symbol="cm" if scaled else "m"
        require(">length ["+symbol+"]</text>" in svg and ">angle [rad]</text>" in svg,
                "Study SVG loses a declared unit")
        require("parameter_value_storage=SI" in manifest and "parameter_unit.length="+symbol in manifest,
                "Study manifest loses parameter storage or display unit")
        runs = sorted(batch.glob("run-*.psrun"))
        require(len(runs) == 3, "Study must contain three complete runs")
        values = []
        for run in runs:
            data = run.read_bytes()
            at = 16
            points = []
            while at < len(data):
                kind, length, crc = struct.unpack_from("<III", data, at)
                payload = data[at + 12:at + 12 + length]
                require(zlib.crc32(payload) == crc, "Study run CRC mismatch")
                if kind == 3:
                    points.append(struct.unpack("<" + "d" * (length // 8), payload))
                at += 12 + length
            require(points[0][0] == 0 and points[-1][0] == 0.7, "Study endpoint differs from target")
            values.append(points)
        require(len({len(points) for points in values}) > 1, "Different lengths should require different accepted step counts")
        records.append(values)
        replay = project / "runs/timed-summary.psreport"
        replay.write_bytes((batch / "summary.psreport").read_bytes())
        protected = runs + [batch / "summary.psreport", replay]
        hashes = [fingerprint(path) for path in protected]
        flow.run("--workspace-state-test", root, "series-read", timeout=130,
                 marker="TIMED SERIES series-read SELF-TEST: PASSED")
        require(hashes == [fingerprint(path) for path in protected], "Reopening a study changed its data")
    require(records[0] == records[1], "Timed C/Physim studies differ in accepted times or values")


def layouts(flow, directory):
    for language in ("c", "phys"):
        root = directory / language
        root.mkdir(parents=True)
        flow.run("--workspace-state-test", root, "layouts-" + language, timeout=130,
                 marker="LAYOUTS layouts-" + language + " SELF-TEST: PASSED")
        runs = list((root / "project/runs").glob("*.psrun"))
        require(len(runs) == 1, "Applying layouts restarted the simulation")
        protected = runs + [root / "project/main.c" if language == "c" else root / "project/main.phys"]
        hashes = [fingerprint(path) for path in protected]
        for name in ("saved", "applied", "empty", "restored"):
            require((root / ("layouts-" + name + ".bmp")).exists(), "Layout capture missing: " + name)
        catalog = root / "layouts.bin"
        data = catalog.read_bytes()
        require(data[:8] == b"PSLAYT01" and len(data) == 344 and struct.unpack_from("<I", data, 12)[0] == 1,
                "Expected one saved layout")
        flow.run("--workspace-state-test", root, "layouts-read", timeout=130,
                 marker="LAYOUTS layouts-read SELF-TEST: PASSED")
        require(catalog.read_bytes() == data, "Reopening wrote the layout catalog")
        damaged = data[:100]
        catalog.write_bytes(damaged)
        flow.run("--workspace-state-test", root, "layouts-corrupt", timeout=130,
                 marker="LAYOUTS layouts-corrupt SELF-TEST: PASSED")
        require(catalog.read_bytes() == damaged, "Corrupt layout catalog was overwritten")
        flow.run("--workspace-state-test", root, "layouts-reset", timeout=130,
                 marker="LAYOUTS layouts-reset SELF-TEST: PASSED")
        empty = catalog.read_bytes()
        require(len(empty) == 20 and struct.unpack_from("<I", empty, 12)[0] == 0,
                "Explicit catalog reset did not persist")
        require(hashes == [fingerprint(path) for path in protected], "Layout actions changed sources or run data")


def inspector(flow, directory):
    for language in ("c", "phys"):
        root=directory/language
        root.mkdir(parents=True)
        flow.run("--workspace-state-test",root,"inspector-"+language,timeout=130,
                 marker="INSPECTOR inspector-"+language+" SELF-TEST: PASSED")
        runs=list((root/"project/runs").glob("*.psrun"))
        require(len(runs)==1,"Inspector movement restarted the simulation")
        protected=runs+[root/"project/main.c" if language=="c" else root/"project/main.phys"]
        hashes=[fingerprint(path) for path in protected]
        for name in ("docked","floating","hidden","tabs","resized"):
            require((root/("inspector-"+name+".bmp")).exists(),"Inspector capture missing: "+name)
        flow.run("--workspace-state-test",root,"inspector-read",timeout=130,
                 marker="INSPECTOR inspector-read SELF-TEST: PASSED")
        require(hashes==[fingerprint(path) for path in protected],"Reopening inspector changed source or run")


def adaptive(flow, directory):
    prefixes = []
    for language in ("c", "phys"):
        root = directory / language
        root.mkdir(parents=True)
        flow.run("--workspace-state-test", root, "adaptive-" + language, timeout=130,
                 marker="ADAPTIVE adaptive-" + language + " SELF-TEST: PASSED")
        project = root / "project"
        runs = sorted((project / "runs").glob("*.psrun"))
        require(len(runs) == 2, "Adaptive reset must retain the original and create one new run")
        require("simulation.steps=adaptive" in read(project / "physim.project"), "Adaptive mode not saved")
        for name in ("adaptive-paused.bmp", "adaptive-reset.bmp", "adaptive-recorded.bmp"):
            require((root / name).exists(), "Adaptive capture missing")
        # Compare every overlapping accepted C/Physim sample, including actual times.
        samples = []
        data = runs[-1].read_bytes()
        at = 16
        while at < len(data):
            kind, length, crc = struct.unpack_from("<III", data, at)
            payload = data[at + 12:at + 12 + length]
            require(zlib.crc32(payload) == crc, "Adaptive run chunk CRC mismatch")
            if kind == 3:
                samples.append(struct.unpack("<" + "d" * (length // 8), payload))
            at += 12 + length
        prefixes.append(samples)
        replay = project / "runs/adaptive-replay.psrun"
        replay.write_bytes(data)
        original = [fingerprint(path) for path in runs + [replay]]
        flow.run("--workspace-state-test", root, "adaptive-replay", timeout=130,
                 marker="TIMELINE adaptive-replay SELF-TEST: PASSED")
        require(original == [fingerprint(path) for path in runs + [replay]], "Adaptive replay changed a run")
    require(min(map(len, prefixes)) > 5, "Too few adaptive C/Physim samples")
    require(all(a == b for a, b in zip(*prefixes)), "Adaptive C/Physim pendulum values or times differ")


def hierarchy(flow, directory):
    for language in ("c", "phys"):
        root = directory / language
        root.mkdir(parents=True)
        flow.run("--workspace-state-test", root, "hierarchy-" + language, timeout=130,
                 marker="HIERARCHY hierarchy-" + language + " SELF-TEST: PASSED")
        runs = list((root / "project/runs").glob("*.psrun"))
        require(len(runs) == 1,
                "Navigating the scene hierarchy created another simulation run")
        original = runs[0]
        before = fingerprint(original)
        data = original.read_bytes()
        legacy = bytearray(data[:16])
        at = 16
        while at < len(data):
            kind, length, crc = struct.unpack_from("<III", data, at)
            payload = data[at + 12:at + 12 + length]
            if kind == 5:
                version, = struct.unpack_from("<I", payload)
                require(version == 2, "Hierarchy source does not contain version-2 scenes")
                channels, objects = struct.unpack_from("<II", payload, 12)
                head = 4 + 24 + channels * 8
                flat = [payload[head + i * 176:head + i * 176 + 172]
                        for i in range(objects)
                        if struct.unpack_from("<I", payload, head + i * 176)[0] != 8]
                old = bytearray(payload[:head])
                struct.pack_into("<I", old, 0, 1)
                struct.pack_into("<I", old, 16, len(flat))
                old.extend(b"".join(flat))
                old.extend(payload[head + objects * 176:])
                legacy.extend(struct.pack("<III", 5, len(old), zlib.crc32(old)))
                legacy.extend(old)
            else:
                legacy.extend(data[at:at + 12 + length])
            at += 12 + length
        path = root / "project/runs/v1.psrun"
        path.write_bytes(legacy)
        legacy_before = fingerprint(path)
        flow.run("--workspace-state-test", root, "hierarchy-v1", timeout=130,
                 marker="TIMELINE v1 SELF-TEST: PASSED")
        require(fingerprint(original) == before and fingerprint(path) == legacy_before,
                "Replaying legacy scenes changed the original or version-1 file")


def docking(flow, directory):
    for language in ("c", "phys"):
        root = directory / language
        root.mkdir(parents=True)
        flow.run("--workspace-state-test", root, "dock-live-" + language, timeout=130,
                 marker="DOCK dock-live-" + language + " SELF-TEST: PASSED")
        require(len(list((root / "project/runs").glob("*.psrun"))) == 1,
                "Moving panels created another simulation run")
    for mode in ("write", "read", "corrupt"):
        if mode == "corrupt":
            write(directory / "preferences.bin", "damaged layout")
        flow.run("--workspace-state-test", directory, "dock-" + mode, timeout=30,
                 marker="DOCK dock-" + mode + " SELF-TEST: PASSED")
    exact(directory / "preferences.bin", "damaged layout")


def timeline(flow, directory):
    for language in ("c", "phys"):
        root = directory / language
        root.mkdir(parents=True)
        flow.run("--workspace-state-test", root, "timeline-" + language, timeout=130,
                 marker="TIMELINE timeline-" + language + " SELF-TEST: PASSED")
        runs = list((root / "project/runs").glob("*.psrun"))
        require(len(runs) == 2, "Browsing or playback created a simulation run")
        original = max(runs, key=lambda p: p.stat().st_size)
        before = {p: fingerprint(p) for p in runs}
        content = original.read_bytes()
        # Strip only optional scene chunks; measurement bytes and footer remain identical.
        legacy = bytearray(content[:16])
        at = 16
        scene_count = 0
        while at < len(content):
            kind, length, crc = struct.unpack_from("<III", content, at)
            end = at + 12 + length
            require(end <= len(content), "Truncated source run")
            if kind != 5:
                legacy.extend(content[at:end])
            else:
                scene_count += 1
            at = end
        require(scene_count >= 4, "Runner did not archive its scene states")
        negative = bytearray(legacy)
        at = 16
        while at < len(negative):
            kind, length, crc = struct.unpack_from("<III", negative, at)
            if kind == 3:
                time = struct.unpack_from("<d", negative, at + 12)[0]
                struct.pack_into("<d", negative, at + 12, time - 1)
                struct.pack_into("<I", negative, at + 8, zlib.crc32(negative[at + 12:at + 12 + length]))
            at += 12 + length
        for mode, data in (("open", content), ("legacy", bytes(legacy)),
                           ("legacy-negative", bytes(negative)), ("recovered", content[:-20])):
            name = "timeline-" + mode
            fixture = root / "project/runs" / (name + ".psrun")
            require(not fixture.exists(), "Timeline fixture would overwrite a file")
            fixture.write_bytes(data)
            flow.run("--workspace-state-test", root, name, timeout=130,
                     marker="TIMELINE " + name + " SELF-TEST: PASSED")
        for path, digest in before.items():
            require(fingerprint(path) == digest, "Review changed the original run")


def speed(flow, directory):
    for mode in ("c", "phys"):
        root = directory / mode
        root.mkdir(parents=True)
        flow.run("--workspace-state-test", root, "speed-" + mode, timeout=130,
                 marker="SPEED speed-" + mode + " SELF-TEST: PASSED")
        project = root / "project"
        require(len(list((project / "runs").glob("*.psrun"))) == 2, "Speed changes replaced a run")
        require("simulation.speed=0" in read(project / "physim.project"), "Pacing choice not saved")
        for name in ("speed-paused.bmp", "speed-live.bmp", "speed-reset.bmp"):
            require((root / name).exists(), "Pacing capture missing")
        flow.run("--workspace-state-test", root, "speed-read", timeout=20,
                 marker="SPEED speed-read SELF-TEST: PASSED")


def reset(flow, directory):
    for mode in ("c", "phys", "parameters", "hang"):
        root = directory / mode
        root.mkdir(parents=True)
        flow.run("--workspace-state-test", root, "reset-" + mode, timeout=130,
                 marker="RESET reset-" + mode + " SELF-TEST: PASSED")
        project = root / "project"
        runs = list((project / "runs").glob("*.psrun"))
        require(len(runs) == 4, "Reset overwrote a run or created duplicates")
        language = "c" if mode in ("c", "hang") else "phys"
        experiment = "main." + language
        source = fingerprint(project / experiment)
        for run in runs:
            require(fingerprint(Path(str(run) + ".experiment." + language)) == source,
                    "Reset lost the original source snapshot")
            require(Path(str(run) + ".limits.txt").exists(), "Reset did not archive run limits")
        require(not (project / "reset-source-hidden").exists(), "Failed-start source not restored")
        for name in ("reset-initial.bmp", "reset-step.bmp", "reset-retry.bmp"):
            require((root / name).exists(), "Reset control capture missing")


def settings(flow, directory):
    for mode in ("write", "read", "reset", "defaults", "maxwrite", "maxread", "corrupt"):
        if mode == "corrupt":
            write(directory / "preferences.bin", "damaged preferences")
        flow.run("--settings-test", directory, mode)
    exact(directory / "preferences.bin", "damaged preferences")


def themes(flow, directory):
    directory.mkdir()
    for mode in ("light", "light-read", "contrast", "contrast-read", "cancel", "defaults", "dark-read"):
        path = directory / "preferences.bin"
        before = fingerprint(path) if path.exists() else None
        flow.run("--settings-test", directory, "theme-" + mode,
                 marker="THEME " + mode + " SELF-TEST: PASSED")
        if mode.endswith("read") or mode == "cancel":
            require(before == fingerprint(path), "Read/cancel changed stored theme preferences")
    for mode in ("light", "contrast", "defaults"):
        require((directory / ("theme-" + mode + ".bmp")).exists(), "Theme editor capture missing")
        require((directory / ("docs-theme-" + mode + ".bmp")).exists(), "Theme documentation capture missing")
        expected = (246, 247, 250) if mode == "light" else (0, 0, 0) if mode == "contrast" else (30, 31, 35)
        for prefix in ("theme-", "theme-settings-", "docs-theme-", "theme-plot-"):
            data = (directory / (prefix + mode + ".bmp")).read_bytes()
            require(data[:2] == b"BM", "Invalid theme capture")
            offset = struct.unpack_from("<I", data, 10)[0]
            width, height = struct.unpack_from("<ii", data, 18)
            bits = struct.unpack_from("<H", data, 28)[0]
            require(bits in (24, 32) and width > 0 and height != 0, "Unsupported capture layout")
            stride = ((width * bits + 31) // 32) * 4
            pixel = bytes(reversed(expected))
            count = sum(data[at:at + 3] == pixel
                        for row in range(abs(height))
                        for at in range(offset + row * stride, offset + row * stride + width * (bits // 8), bits // 8))
            require(count > width * abs(height) // 100, f"Theme background was not rendered: {prefix}{mode}")


def documents_input_isolation(flow, directory):
    directory.mkdir(parents=True)
    for size in ("small", "large"):
        if size == "small":
            flow.env["PHYSIM_TEST_SMALL"] = "1"
        else:
            flow.env.pop("PHYSIM_TEST_SMALL", None)
        flow.run("--workspace-state-test", directory / (size + "-unfiltered"), "documents-unfiltered",
                 expected_exit_code=1)
        require("First workflow failure at stage 4" in flow.steps[-1]["stderr"],
                "Unfiltered pointer noise did not reproduce the missed file selection")
        flow.run("--workspace-state-test", directory / size, "documents-noise",
                 timeout=25, marker="DOCUMENT WORKFLOW: PASSED")
        exact(directory / size / "notes α.txt", "Saved on close: γ\n")
        exact(directory / size / "notes α.txt.bak", "Text über SDL: β = 2\n")
        exact(directory / size / "second.txt", "external")


def toolbar_input_isolation(flow, directory):
    directory.mkdir()
    for size in ("small", "large"):
        if size == "small":
            flow.env["PHYSIM_TEST_SMALL"] = "1"
        else:
            flow.env.pop("PHYSIM_TEST_SMALL", None)
        flow.run("--workspace-state-test", directory / size, "toolbar-noise", timeout=25,
                 marker="TOOLBAR AND TABS: PASSED")


def workspace_state(flow, directory):
    for mode in ("write", "read", "read-analysis", "read-simulate", "missing-addition", "missing-root", "forget", "empty", "write-folder", "read-folder",
                 "read-folder-shortened", "read-folder-recovery", "corrupt"):
        if mode == "missing-addition":
            (directory / "notes ä.txt").rename(directory / "notes-moved.txt")
        elif mode == "missing-root":
            (directory / "project").rename(directory / "project-moved")
            before = fingerprint(directory / "workspace.bin")
        elif mode == "corrupt":
            write(directory / "workspace.bin", "damaged workspace")
        elif mode == "read-folder-shortened":
            write(directory / "view λ.txt", "λ")
        flow.run("--workspace-state-test", directory, mode, timeout=20)
        if mode == "missing-root":
            require(before == fingerprint(directory / "workspace.bin"), "Missing root lost the saved workspace")
        if mode in ("read-folder-shortened", "read-folder-recovery"):
            exact(directory / "view λ.txt", "λ")
    exact(directory / "workspace.bin", "damaged workspace")


def document_recovery(flow, root):
    root.mkdir()
    for mode in ("restore", "discard", "conflict", "cancel", "corrupt", "reset", "late-corrupt", "revert"):
        directory = root / mode
        flow.run("--workspace-state-test", directory, "drafts-write")
        workspace = directory / "workspace"
        draft_folder = directory / "app-data/document-drafts"
        drafts = list(draft_folder.glob("*.draft"))
        require(len(list(workspace.iterdir())) == 2 and len(drafts) == 2, "Expected two sources and two separate drafts")
        original, second = "original α\n", "second original\n"
        exact(workspace / "notes α.txt", original)
        exact(workspace / "second.txt", second)
        if mode == "conflict":
            write(workspace / "notes α.txt", "external α\n")
        elif mode in ("corrupt", "reset"):
            for draft in drafts:
                write(draft, "damaged snapshot")
        flow.run("--workspace-state-test", directory, f"drafts-{mode}")
        actual = read(workspace / "notes α.txt")
        actual_second = read(workspace / "second.txt")
        remaining = list(draft_folder.glob("*.draft"))
        if mode == "cancel":
            require(actual == original and actual_second == second, "Cancel changed sources")
            require(all(path.exists() for path in drafts), "Cancel removed drafts")
        elif mode in ("reset", "late-corrupt"):
            expected = "still in editor\n" if mode == "reset" else "first draft β 2\n"
            expected_second = second if mode == "reset" else "second draft γ\n"
            require(len(remaining) == 1 and actual == expected and actual_second == expected_second,
                    "Reset did not preserve independent document recovery")
            exact(remaining[0], "damaged snapshot")
        elif mode == "corrupt":
            for draft in drafts:
                exact(draft, "damaged snapshot")
            require(actual == "still in editor\n", "Paused autosave prevented normal saving")
        else:
            require(actual == (original if mode in ("discard", "revert") else "first draft β 2\n"),
                    "Recovery saved unexpected source contents")
            if mode == "conflict":
                exact(workspace / "notes α.txt.bak", "external α\n")
            require(actual_second == "second draft γ\n", "Second document did not recover independently")
            require(not remaining, "Resolved drafts remain")


def autosave(flow, root):
    root.mkdir()
    for mode in ("restore", "discard", "conflict", "cancel", "corrupt"):
        project = root / mode
        flow.run("--autosave-crash", project, timeout=30)
        original = [read(project / name) for name in ("main.c", "analysis.c")]
        if mode == "conflict":
            with (project / "main.c").open("a", encoding="utf-8", newline="\n") as file:
                file.write("\n/* external edit */\n")
        elif mode == "corrupt":
            write(project / ".physim-autosave", "damaged snapshot")
        flow.run("--autosave-recover", project, mode, timeout=30)
        if mode == "cancel":
            require((project / ".physim-autosave").is_file(), "Cancel lost snapshot")
            require(original == [read(project / name) for name in ("main.c", "analysis.c")], "Cancel changed sources")
        elif mode == "corrupt":
            exact(project / ".physim-autosave", "damaged snapshot")


SPECIAL = {"layouts_workflow": layouts, "inspector_workflow": inspector, "timed_series_workflow": timed_series,
           "parameter_units_workflow": lambda flow,directory: timed_series(flow,directory,True), "adaptive_workflow": adaptive, "docking_workflow": docking, "hierarchy_workflow": hierarchy, "documents_input_isolation": documents_input_isolation, "timeline_workflow": timeline, "speed_workflow": speed, "reset_workflow": reset, "project_settings_workflow": project_settings, "settings_workflow": settings, "themes_workflow": themes,
           "workspace_state_workflow": workspace_state, "documents_recovery": document_recovery,
           "autosave_workflow": autosave, "toolbar_input_isolation": toolbar_input_isolation}


def main():
    # App diagnostics and paths can contain Unicode even when Python inherits a
    # legacy Windows console encoding through a captured subprocess pipe.
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    sys.stderr.reconfigure(encoding="utf-8", errors="replace")
    parser = argparse.ArgumentParser(description=__doc__)
    language_cases = [f"language_{mode}_workflow" for mode in LANGUAGE_MODES]
    parser.add_argument("case", choices=sorted([*SIMPLE, *SPECIAL, *language_cases]))
    parser.add_argument("--app", type=Path, required=True)
    parser.add_argument("--work", type=Path, required=True)
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--cc", help="Compiler for projects created by the app")
    args = parser.parse_args()
    directory = args.work.resolve() / "workflow ä"
    require(not directory.exists(), f"Workflow directory already exists: {directory}")
    small = SIMPLE[args.case][2] if args.case in SIMPLE else True
    flow = Workflow(args.app.resolve(), args.work.resolve(), args.root.resolve(), small, args.cc,
                    trace=os.environ.get("PHYSIM_TEST_STRACE_CASE") == args.case)
    if args.case in SPECIAL:
        SPECIAL[args.case](flow, directory)
    elif args.case in language_cases:
        mode = args.case.removesuffix("_workflow")
        flow.run("--self-test", directory, mode, timeout=210, marker="APP SELF-TEST: PASSED")
        experiment = "main.c" if mode == "language_mixed" else "main.phys"
        description = read(directory / "physim.project")
        require(f"experiment={experiment}" in description and "analysis=analysis.phys" in description,
                "Language selection was not preserved")
        require(not (directory / "CMakeLists.txt").exists() and not (directory / "build/CMakeCache.txt").exists(),
                "App generated CMake files")
        require((directory / "build/Debug").is_dir(), "Missing native project build directory")
        artifacts = [experiment, "analysis.phys", "diagnostics.bmp", "editor.bmp", "simulation.bmp",
                     "language-analysis.bmp", "language-analysis-editor.bmp"]
        if mode == "language_sensors":
            artifacts.append("language-statistics.bmp")
        for name in artifacts:
            require((directory / name).is_file() and (directory / name).stat().st_size > 0, f"Missing/empty artifact: {name}")
    else:
        arguments, timeout, _ = SIMPLE[args.case]
        paths = dict(directory=str(directory), root=str(flow.root))
        flow.run(*(arg.format_map(paths) for arg in arguments), timeout=timeout)
        if args.case in ("documents_small", "documents_large"):
            exact(directory / "notes α.txt", "Saved on close: γ\n")
            exact(directory / "notes α.txt.bak", "Text über SDL: β = 2\n")
            exact(directory / "second.txt", "external")
        elif args.case == "documents_build":
            exact(directory / "project with spaces/helper.h", "#define DOCUMENT_TEST_VERSION 4\n")
            require("DOCUMENT_TEST_VERSION >= 2" in read(directory / "project with spaces/main.c"), "Build input not saved")
    print(f"{args.case}: app workflow and persisted files passed")


if __name__ == "__main__":
    main()
