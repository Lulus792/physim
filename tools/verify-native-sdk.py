"""Verify a relocated direct-build SDK with real compilers, runners and optional GUI tests."""
import argparse
import hashlib
import importlib.util
import json
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--sdk", type=Path, required=True)
    parser.add_argument("--work", type=Path, required=True)
    parser.add_argument("--compiler")
    parser.add_argument("--app-tests", action="store_true")
    args = parser.parse_args()
    repo = Path(__file__).resolve().parent.parent
    if args.sdk.resolve() == args.work.resolve() or args.sdk.resolve() in args.work.resolve().parents:
        parser.error("Verification work must be outside the SDK being copied")
    args.work.mkdir(parents=True, exist_ok=True)
    root = Path(tempfile.mkdtemp(prefix="Native SDK ä ", dir=args.work)).resolve()
    staging = root / "staging"
    shutil.copytree(args.sdk.resolve(), staging)
    sdk = root / "Relocated SDK ä"
    staging.rename(sdk)
    metadata = json.loads((sdk / "physim-sdk.json").read_text(encoding="utf-8"))
    if metadata.get("format") != 1 or metadata.get("platform") != sys.platform:
        raise RuntimeError("Expected a native SDK for this platform")
    if (sdk / "cmake").exists() or any(sdk.rglob("CMakeLists.txt")) or any(sdk.rglob("*.cmake")):
        raise RuntimeError("Native SDK must not contain legacy CMake build files")
    for name, digest in metadata["files"].items():
        if any(part == ".DS_Store" or part.startswith("._") for part in Path(name).parts):
            raise RuntimeError(f"SDK contains local Finder metadata: {name}")
        path = (sdk / name).resolve()
        if sdk not in path.parents or hashlib.sha256(path.read_bytes()).hexdigest() != digest:
            raise RuntimeError(f"SDK integrity check failed: {name}")
    suffix = ".exe" if sys.platform == "win32" else ""
    module_suffix = ".dll" if sys.platform == "win32" else ".so"
    spec = importlib.util.spec_from_file_location("physim_build", repo / "tools/build.py")
    native = importlib.util.module_from_spec(spec)
    sys.dont_write_bytecode = True
    spec.loader.exec_module(native)
    # Compile the independent public-API probe and header units against the
    # installed include directory and static archive, not repository sources.
    consumer = root / "Consumer ä"
    (consumer / "app").mkdir(parents=True)
    shutil.copy2(repo / "tools/sdk_probe.c", consumer / "probe.c")
    shutil.copy2(repo / "tools/sdk_series_probe.c", consumer / "series-probe.c")
    shutil.copy2(repo / "app/utf8.manifest", consumer / "app/utf8.manifest")
    native.ROOT = consumer
    options = argparse.Namespace(build_dir=consumer / "build", compiler=args.compiler,
                                 config=metadata["config"], no_app=True, jobs=4)
    env = native.compiler_environment()
    log = root / "verification.log"

    def checked(command, *, output=None):
        result = subprocess.run([str(p) for p in command], cwd=root, env=env,
                                capture_output=True, timeout=300)
        with log.open("ab") as file:
            file.write(("COMMAND: " + repr([str(p) for p in command]) + "\n").encode("utf-8"))
            if output is None:
                file.write(result.stdout)
            file.write(result.stderr)
            file.write(f"\nRESULT: {result.returncode}\n".encode())
        if result.returncode:
            raise RuntimeError(f"Command failed: {command}\n{result.stdout.decode('utf-8', errors='replace')}\n{result.stderr.decode('utf-8', errors='replace')}")
        if output is not None:
            output.write_bytes(result.stdout)

    with native.build_lock(options.build_dir):
        builder = native.Builder(options, env)
        builder.includes = [sdk / "include"]
        builder.headers = native.digest_files(sorted((sdk / "include").rglob("*.h")))
        library = sdk / "lib" / ("physim-core.lib" if native.WINDOWS else "libphysim-core.a")
        probe = builder.executable("sdk-probe", ["probe.c"], [library])
        series_probe=builder.executable("sdk-series-probe",["series-probe.c"],[library])
        shutil.copy2(repo / "tests/test_run_index.c",consumer / "run-index-language-fixture.c")
        shutil.copy2(repo / "tests/test_allocator.h",consumer / "test_allocator.h")
        fixture=builder.executable("run-index-language-fixture",["run-index-language-fixture.c"],[library])
        checked([fixture,root])
        header_sources = []
        for header in sorted((sdk / "include/physim").glob("*.h")):
            name = "header_" + header.stem + ".c"
            (consumer / name).write_text(f"#include <physim/{header.name}>\n", encoding="utf-8")
            header_sources.append(name)
        builder.objects(header_sources)

        def check_analysis(run, name, analysis, kind=None):
            report = root / (name + "-report")
            checked([sdk / "bin" / ("physim-analysis-runner" + suffix), analysis, run, report])
            checked([probe, run, report.with_suffix(".psreport")] + ([kind] if kind else []))

        def check_modules(name, experiment, analysis, language=False):
            run = root / (name + ".psrun")
            checked([sdk / "bin" / ("physim-runner" + suffix), experiment, run,
                     "--steps", "200", "--dt", ".005", "--seed", "42"])
            check_analysis(run, name, analysis, "language" if language else None)
            print(f"Installed SDK: {name} passed", flush=True)

        for name in native.EXAMPLES:
            check_modules("bundled-" + name, sdk / "bin" / (name + module_suffix),
                          sdk / "bin" / ("pendulum_analysis" + module_suffix))

        for name in native.LANGUAGE_PROGRAMS:
            source = consumer / (name + ".c")
            checked([sdk / "bin" / ("physimc" + suffix), "--emit-c",
                     sdk / "examples/language" / (name + ".phys")], output=source)
            program = builder.executable(name, [source.name], [library], language=True)
            checked([program])
            print(f"Installed language program: {name} passed", flush=True)

        # Exercise the installed source distribution independently of its archive.
        # No compiler input below comes from the repository's include/src trees.
        shutil.copytree(sdk / "src", consumer / "src")
        rebuilt_core = builder.archive("sdk-rebuilt-core", [f"src/{name}.c" for name in native.CORE])
        rebuilt_probe = builder.executable("sdk-rebuilt-probe", ["probe.c"], [rebuilt_core])
        rebuilt_series_probe=builder.executable("sdk-rebuilt-series-probe",["series-probe.c"],[rebuilt_core])
        checked([rebuilt_probe, root / "bundled-pendulum.psrun", root / "bundled-pendulum-report.psreport"])
        shutil.copy2(sdk / "examples/pendulum/analysis.c", consumer / "c-analysis.c")
        c_analysis = builder.executable("sdk-c-analysis", ["c-analysis.c"], [rebuilt_core], module=True)
        c_modules = {}
        for name in native.EXAMPLES:
            source = consumer / ("c-" + name + ".c")
            shutil.copy2(sdk / "examples" / name / "main.c", source)
            experiment = builder.executable("sdk-c-" + name, [source.name], [rebuilt_core], module=True)
            c_modules[name] = experiment
            check_modules("source-c-" + name, experiment, c_analysis)

        modules = {}
        for name, mode, relative in native.language_examples():
            if mode == "--emit-c":
                continue
            source = consumer / (name + ".c")
            checked([sdk / "bin" / ("physimc" + suffix), mode, sdk / "examples" / relative], output=source)
            modules[name] = builder.executable("language-" + name, [source.name], [rebuilt_core],
                                               module=True, language=True)
            print(f"Installed language module: {name} rebuilt", flush=True)
        shutil.copy2(repo / "tests/test_language_run_index_memory.c",consumer / "run-index-language-memory.c")
        shutil.copy2(repo / "tests/test_language_run_index_report.c",consumer / "run-index-language-report.c")
        for kind,archive in (("installed",library),("rebuilt",rebuilt_core)):
            memory=builder.executable("run-index-language-memory-"+kind,["run-index-language-memory.c"],[archive])
            checked([memory,root / "index α.psrun"])
        report=root / "Indexed language report"
        checked([sdk / "bin" / ("physim-analysis-runner"+suffix),modules["run_index_analysis"],root / "million.psrun",report])
        checker=builder.executable("run-index-language-report",["run-index-language-report.c"],[rebuilt_core])
        checked([checker,Path(str(report)+".psreport")])
        print("Installed SDK indexed language: owned handles, allocation failures, selected input path and all 256 report rows passed",flush=True)

        # Logging must also survive relocation and a rebuild of the shipped Core.
        shutil.copy2(sdk / "examples/logging/main.c", consumer / "logging.c")
        for kind, archive in (("installed", library), ("rebuilt", rebuilt_core)):
            experiment = builder.executable("logging-" + kind, ["logging.c"], [archive], module=True)
            directory = root / ("Logging " + kind)
            directory.mkdir()
            checked([sys.executable, repo / "tests/test_runner_logging.py",
                     sdk / "bin" / ("physim-runner" + suffix), experiment, directory])
        directory = root / "Logging Physim"
        directory.mkdir()
        checked([sys.executable, repo / "tests/test_runner_logging.py",
                 sdk / "bin" / ("physim-runner" + suffix), modules["logging"], directory])
        print("Installed SDK logging: installed/rebuilt C and Physim passed", flush=True)
        shutil.copy2(sdk / "examples/scene_frames/main.c",consumer / "frames.c")
        frames=builder.executable("frames-rebuilt",["frames.c"],[rebuilt_core],module=True)
        for name,experiment in (("C",frames),("Physim",modules["scene_frames"])):
            checked([sdk / "bin" / ("physim-runner"+suffix),experiment,root / ("Frames "+name+".psrun"),
                     "--steps","20","--dt",".01","--record-scenes"])
        print("Installed SDK coordinate frames: rebuilt C/Physim modules passed",flush=True)
        shutil.copy2(repo / "tests/test_scene_frames.c",consumer / "frame-probe.c")
        for kind,archive in (("installed",library),("rebuilt",rebuilt_core)):
            program=builder.executable("frame-probe-"+kind,["frame-probe.c"],[archive])
            directory=root / ("Frame probe "+kind);directory.mkdir()
            checked([program,directory])
        print("Installed SDK coordinate frames: independent installed/rebuilt API probes passed",flush=True)
        shutil.copy2(repo / "tests/test_diagnostic.c",consumer / "diagnostic-probe.c")
        shutil.copy2(repo / "tests/fixtures/legacy_analysis_diagnostic.c",consumer / "legacy-diagnostic.c")
        shutil.copy2(sdk / "examples/diagnostics/main.c",consumer / "diagnostic-experiment.c")
        shutil.copy2(sdk / "examples/diagnostics/analysis.c",consumer / "diagnostic-analysis.c")
        for kind,archive in (("installed",library),("rebuilt",rebuilt_core)):
            program=builder.executable("diagnostic-probe-"+kind,["diagnostic-probe.c"],[archive])
            directory=root / ("Diagnostic probe "+kind);directory.mkdir()
            checked([program,directory])
            experiment=builder.executable("diagnostic-experiment-"+kind,["diagnostic-experiment.c"],[archive],module=True)
            analysis=builder.executable("diagnostic-analysis-"+kind,["diagnostic-analysis.c"],[archive],module=True)
            legacy=builder.executable("legacy-diagnostic-"+kind,["legacy-diagnostic.c"],[archive],module=True)
            directory=root / ("Diagnostic runners "+kind);directory.mkdir()
            checked([sys.executable,repo / "tests/test_runner_diagnostic.py",sdk / "bin" / ("physim-runner"+suffix),
                     experiment,modules["diagnostic_experiment"],sdk / "bin" / ("physim-analysis-runner"+suffix),
                     analysis,modules["diagnostic_analysis"],legacy,directory])
        print("Installed SDK diagnostics: installed/rebuilt C, Physim and legacy ABI passed",flush=True)
        shutil.copy2(repo / "tests/test_run_index.c",consumer / "run-index-probe.c")
        shutil.copy2(repo / "tests/test_allocator.h",consumer / "test_allocator.h")
        for kind,archive in (("installed",library),("rebuilt",rebuilt_core)):
            program=builder.executable("run-index-probe-"+kind,["run-index-probe.c"],[archive])
            directory=root / ("Run index "+kind);directory.mkdir()
            checked([program,directory])
            checked([sys.executable,repo / "tests/test_run_index_codec.py",directory])
        print("Installed SDK run index: installed/rebuilt Core, million rows and independent codec passed",flush=True)


        # Preserve the former SDK comparison: nine experiments, both general
        # Physim analyses, the sensor report and six C/Physim combinations.
        # Broader physics equivalence is checked by the normal integration suite.
        mixed = {"sensors", "spinning_body", "box_contacts", "joint_pendulum", "coupled_bodies", "fast_sphere"}
        for name in ("pendulum", "projectile", "spring", *sorted(mixed)):
            run = root / ("language-" + name + ".psrun")
            checked([sdk / "bin" / ("physim-runner" + suffix), modules[name], run,
                     "--steps", "200", "--dt", ".005", "--seed", "42"])
            for analyzer in ("analysis", "analysis_integral"):
                check_analysis(run, "language-" + name + "-" + analyzer, modules[analyzer], "language")
            if name == "sensors":
                check_analysis(run, "language-sensors-special", modules["analysis_sensors"], "sensor-language")
            if name in mixed:
                check_analysis(run, "language-" + name + "-c", c_analysis)
            print(f"Installed language experiment and analyses: {name} passed", flush=True)

        for name, experiment in (("bundled-pendulum", sdk / "bin" / ("pendulum" + module_suffix)),
                                 ("source-c-pendulum", c_modules["pendulum"]),
                                 ("language-pendulum", modules["pendulum"])):
            run = root / ("adaptive-" + name + ".psrun")
            checked([sdk / "bin" / ("physim-runner" + suffix), experiment, run,
                     "--steps", "500", "--dt", ".005", "--adaptive",
                     "--min-dt", "1e-8", "--max-dt", ".1"])
            check_analysis(run, "adaptive-" + name + "-c", c_analysis, "adaptive")
            check_analysis(run, "adaptive-" + name + "-phys", modules["analysis"], "adaptive-language")
            checked([rebuilt_probe, run, root / ("adaptive-" + name + "-c-report.psreport"), "adaptive"])
            print(f"Installed adaptive experiment and C/Physim analyses: {name} passed", flush=True)
            series=root/("target-series-"+name)
            checked([sdk / "bin" / ("physim-batch" + suffix),
                     sdk / "bin" / ("physim-runner" + suffix),experiment,series,
                     "angle","3","1000",".1","42","--until",".7","--adaptive",
                     "--min-dt","1e-6","--max-dt",".2","--sweep","length=.5:2.5","--workers","3"])
            checked([series_probe,series,"installed"]);checked([rebuilt_series_probe,series,"rebuilt"])
            print(f"Installed target-time study: {name} passed",flush=True)

        if metadata["app"]:
            for name in native.EXAMPLES + ["language"]:
                project = root / ("Project " + name)
                project.mkdir()
                language = name == "language"
                extension = ".phys" if language else ".c"
                experiment = sdk / ("examples/language/pendulum.phys" if language else f"examples/{name}/main.c")
                analysis = sdk / ("examples/language/analysis.phys" if language else "examples/pendulum/analysis.c")
                shutil.copy2(experiment, project / ("main" + extension))
                shutil.copy2(analysis, project / ("analysis" + extension))
                (project / "physim.project").write_text(
                    f"physim_project=1\nexperiment=main{extension}\nanalysis=analysis{extension}\nprofile=Release\n", encoding="utf-8")
                before = {p.name: p.read_bytes() for p in project.iterdir()}
                command = [sdk / "bin" / ("physim-build" + suffix), "--project", project,
                           "--sdk", sdk, "--physimc", sdk / "bin" / ("physimc" + suffix),
                           "--output", project / "build/Release", "--profile", "Release"]
                if args.compiler:
                    command += ["--cc", builder.cc]
                checked(command)
                check_modules("rebuilt-" + name, project / "build/Release" / ("experiment" + module_suffix),
                              project / "build/Release" / ("analysis" + module_suffix), language)
                if set(p.name for p in project.iterdir()) != set(before) | {"build"}:
                    raise RuntimeError("Project build wrote files outside build/")
                if any((project / name).read_bytes() != data for name, data in before.items()):
                    raise RuntimeError("Project build changed its source files")
            if args.app_tests:
                for example in native.EXAMPLES + ["language_full"]:
                    checked([sdk / "bin" / ("physim" + suffix), "--self-test", root / ("App " + example), example])
                    print(f"Installed SDK GUI workflow: {example} passed", flush=True)
        elif args.app_tests:
            raise RuntimeError("App tests require an SDK with the app")
    (root / "PASSED.txt").write_text(
        "Native SDK relocation, independent headers, installed and rebuilt core archives, eight bundled and rebuilt C templates, "
        "seventeen language programs, 32 rebuilt language modules, nine language experiments with both general analyses, "
        "specialized sensor analysis and six C/Physim combinations passed.\n"
        "Adaptive bundled/source C and Physim pendulums, actual variable sample times, energy and both analysis languages passed.\n" +
        "Common-target-time parameter studies from bundled/source C and Physim pendulums passed with installed and rebuilt probes.\n" +
        "Logging through installed/rebuilt C Core and Physim passed with exclusive JSONL and opt-in wire events.\n" +
        "Structured diagnostics through installed/rebuilt Core, C/Physim runners and legacy ABI passed.\n" +
        "Run indexes through installed/rebuilt Core, legacy/recovered files, allocator failures and million-row independent codec passed.\n" +
        "Indexed language values, copied owners, failure cleanup and all 256 SI report rows passed.\n" +
        ("Nine projects rebuilt without CMake; sources unchanged and outputs confined to build/.\n" if metadata["app"] else "") +
        ("Eight C template GUI workflows and the complete Physim language GUI workflow passed.\n" if args.app_tests else ""), encoding="utf-8")
    print(f"Native SDK verified: {root}")


if __name__ == "__main__":
    main()
