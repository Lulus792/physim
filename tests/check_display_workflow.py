"""Run the app's window workflows and check their persisted files without CMake."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys


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

    def run(self, *arguments, timeout=25, marker=None):
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
        require(result.returncode == 0, f"App failed ({result.returncode}): {command}")
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


def settings(flow, directory):
    for mode in ("write", "read", "reset", "defaults", "maxwrite", "maxread", "corrupt"):
        if mode == "corrupt":
            write(directory / "preferences.bin", "damaged preferences")
        flow.run("--settings-test", directory, mode)
    exact(directory / "preferences.bin", "damaged preferences")


def workspace_state(flow, directory):
    for mode in ("write", "read", "missing-addition", "missing-root", "forget", "empty", "write-folder", "read-folder", "corrupt"):
        if mode == "missing-addition":
            (directory / "notes ä.txt").rename(directory / "notes-moved.txt")
        elif mode == "missing-root":
            (directory / "project").rename(directory / "project-moved")
            before = fingerprint(directory / "workspace.bin")
        elif mode == "corrupt":
            write(directory / "workspace.bin", "damaged workspace")
        flow.run("--workspace-state-test", directory, mode, timeout=20)
        if mode == "missing-root":
            require(before == fingerprint(directory / "workspace.bin"), "Missing root lost the saved workspace")
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


SPECIAL = {"project_settings_workflow": project_settings, "settings_workflow": settings,
           "workspace_state_workflow": workspace_state, "documents_recovery": document_recovery,
           "autosave_workflow": autosave}


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
