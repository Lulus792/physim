"""Verify that a packaged app completes real workflows when opened by macOS."""
import argparse
import json
import os
from pathlib import Path
import plistlib
import subprocess
import sys
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--app", type=Path, required=True)
    parser.add_argument("--work", type=Path, required=True)
    args = parser.parse_args()
    if sys.platform != "darwin":
        parser.error("Requires macOS and an active graphical session")
    app = args.app.resolve()
    if app.suffix != ".app" or not (app / "Contents/MacOS/physim").is_file():
        parser.error("Pass a packaged Physim.app")
    with (app / "Contents/Info.plist").open("rb") as source:
        metadata = plistlib.load(source)
    if metadata.get("CFBundleExecutable") != "physim":
        raise AssertionError("Unexpected bundle executable")
    if args.work.resolve() == app or app in args.work.resolve().parents:
        parser.error("Verification work must be outside the signed app bundle")
    args.work.mkdir(parents=True, exist_ok=True)
    root = Path(tempfile.mkdtemp(prefix="LaunchServices ä ", dir=args.work)).resolve()
    cwd = root / "Empty working directory"
    cwd.mkdir()
    # Finder launches must not depend on a developer shell's SDK or compiler paths.
    environment = {key: value for key, value in os.environ.items()
                   if not key.startswith(("PHYSIM_", "DYLD_", "SDL_")) and key not in
                   {"CC", "CXX", "CFLAGS", "CXXFLAGS", "LDFLAGS", "SDKROOT", "CPATH",
                    "C_INCLUDE_PATH", "LIBRARY_PATH", "MACOSX_DEPLOYMENT_TARGET", "DEVELOPER_DIR"}}
    environment.update(PATH="/usr/bin:/bin:/usr/sbin:/sbin", PHYSIM_TEST_TRACE="1")
    launches = []
    for mode, extension in (("pendulum", "c"), ("language_full", "phys")):
        project = root / ("Project ä " + mode)
        stdout, stderr = root / (mode + ".stdout.log"), root / (mode + ".stderr.log")
        stdout.touch()
        stderr.touch()
        # open -W reports LaunchServices errors, not necessarily the app's exit code.
        # Require the app's marker emitted after all shutdown work as well.
        command = ["/usr/bin/open", "-W", "-n", "-a", str(app),
                   "--stdout", str(stdout), "--stderr", str(stderr),
                   "--args", "--self-test", str(project), mode]
        entry = {"mode": mode, "command": command, "cwd": str(cwd), "path": environment["PATH"]}
        launches.append(entry)
        report = root / "launches.json"
        report.write_text(json.dumps(launches, ensure_ascii=False, indent=2), encoding="utf-8")
        try:
            result = subprocess.run(command, cwd=cwd, env=environment, capture_output=True,
                                    timeout=210)
        except subprocess.TimeoutExpired:
            entry["error"] = "LaunchServices did not finish within 210 seconds"
            report.write_text(json.dumps(launches, ensure_ascii=False, indent=2), encoding="utf-8")
            raise
        entry.update(returncode=result.returncode,
                     output=(result.stdout + result.stderr).decode("utf-8", errors="replace"))
        report.write_text(json.dumps(launches, ensure_ascii=False, indent=2), encoding="utf-8")
        output = stdout.read_text(encoding="utf-8") if stdout.exists() else ""
        errors = stderr.read_text(encoding="utf-8") if stderr.exists() else ""
        if result.returncode or "APP SELF-TEST: PASSED" not in output or "APP TEST EXIT: 0" not in output:
            raise AssertionError(f"{mode} failed via LaunchServices:\n{entry['output']}\n{output}\n{errors}")
        manifest = (project / "physim.project").read_text(encoding="utf-8")
        if f"experiment=main.{extension}" not in manifest or f"analysis=analysis.{extension}" not in manifest:
            raise AssertionError(f"{mode}: source language not preserved")
        for name in (f"main.{extension}", f"analysis.{extension}", "build/Debug/experiment.so",
                     "build/Debug/analysis.so", "editor.bmp", "simulation.bmp",
                     "analysis.bmp" if mode == "pendulum" else "language-analysis.bmp"):
            path = project / name
            if not path.is_file() or path.stat().st_size == 0:
                raise AssertionError(f"Missing launch artifact: {path}")
        if not list((project / "runs").glob("*.psrun")) or not list((project / "runs").glob("*.psreport")):
            raise AssertionError(f"{mode}: missing saved simulation or analysis")
        if (project / "CMakeLists.txt").exists() or (project / "build/CMakeCache.txt").exists():
            raise AssertionError(f"{mode}: unexpected CMake project files")
        print(f"LaunchServices GUI workflow passed: {mode}", flush=True)
    # Opening and compiling user projects must not modify the signed application.
    subprocess.run(["/usr/bin/codesign", "--verify", "--deep", "--strict", str(app)], check=True)
    (root / "PASSED.txt").write_text(
        "Packaged app opened through macOS with a system PATH: C and Physim workflows passed.\n",
        encoding="utf-8")
    print(f"LaunchServices verification complete: {root}")


if __name__ == "__main__":
    main()
