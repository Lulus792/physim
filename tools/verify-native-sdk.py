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
    for name, mode, _ in native.language_examples():
        extension = suffix if mode == "--emit-c" else module_suffix
        product = "bin/language-" + name + extension
        if product not in metadata["files"] or not (sdk / product).is_file():
            raise RuntimeError(f"SDK is missing its compiled language example: {product}")
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
        checked([sys.executable,repo / "tests/test_documentation_tracks.py","--root",sdk,
                 "--catalog",repo / "tests/tutorial_sources.json"])
        shutil.copy2(sdk / "examples/documentation/c_workflow.c",consumer / "c-workflow-intro.c")
        c_intro=builder.executable("c-workflow-intro",["c-workflow-intro.c"],[])
        checked([c_intro])
        checked([sdk / "bin" / ("physimc"+suffix),"--emit-c",sdk / "examples/documentation/physim_workflow.phys"],
                 output=consumer / "physim-workflow-intro.c")
        physim_intro=builder.executable("physim-workflow-intro",["physim-workflow-intro.c"],[library],language=True)
        checked([physim_intro])
        print("Installed SDK learning routes: both complete paths, published sources and native C/Physim introduction programs passed",flush=True)
        probe = builder.executable("sdk-probe", ["probe.c"], [library])
        series_probe=builder.executable("sdk-series-probe",["series-probe.c"],[library])
        shutil.copy2(repo / "tests/test_run_index.c",consumer / "run-index-language-fixture.c")
        shutil.copy2(repo / "tests/test_allocator.h",consumer / "test_allocator.h")
        fixture=builder.executable("run-index-language-fixture",["run-index-language-fixture.c"],[library])
        checked([fixture,root])
        header_sources = []
        for header in sorted((sdk / "include/physim").glob("*.h")):
            name = "header_" + header.stem + ".c"
            (consumer / name).write_text(f"#include <stdio.h>\n#include <locale.h>\n#include <physim/{header.name}>\n", encoding="utf-8")
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
        # Exercise the installed host tail and private controller through public
        # C/Physim analyses; only independent harness sources come from this kit.
        shutil.copy2(repo / "tests/fixtures/analysis_batch.c",consumer / "batch-service-analysis.c")
        shutil.copy2(repo / "tests/fixtures/analysis_diagnostic_tail.c",consumer / "old-diagnostic-tail.c")
        shutil.copy2(repo / "tests/test_language_batch_report.c",consumer / "batch-service-probe.c")
        shutil.copy2(repo / "tests/fixtures/target_experiment.c",consumer / "batch-target-model.c")
        shutil.copy2(repo / "tests/fixtures/language/target_experiment.phys",consumer / "batch-target-model.phys")
        shutil.copy2(repo / "tests/fixtures/language/batch_analysis.phys",consumer / "batch-service-analysis.phys")
        batch_c=builder.executable("batch-service-analysis",["batch-service-analysis.c"],[library],module=True)
        old_tail=builder.executable("old-diagnostic-tail",["old-diagnostic-tail.c"],[library],module=True)
        batch_probe=builder.executable("batch-service-probe",["batch-service-probe.c"],[library])
        target_c=builder.executable("batch-target-model",["batch-target-model.c"],[library],module=True)
        for name,mode in (("batch-target-model","--emit-experiment"),("batch-service-analysis","--emit-analysis")):
            source=consumer / (name+"-phys.c")
            checked([sdk / "bin" / ("physimc"+suffix),mode,consumer / (name+".phys")],output=source)
        target_phys=builder.executable("batch-target-model-phys",["batch-target-model-phys.c"],[library],module=True,language=True)
        batch_phys=builder.executable("batch-service-analysis-phys",["batch-service-analysis-phys.c"],[library],module=True,language=True)
        checked([sys.executable,repo / "tests/test_language_batch.py",
                 "--compiler",sdk / "bin" / ("physimc"+suffix),
                 "--analysis-runner",sdk / "bin" / ("physim-analysis-runner"+suffix),
                 "--runner",sdk / "bin" / ("physim-runner"+suffix),"--batch",sdk / "bin" / ("physim-batch"+suffix),
                 "--c-analysis",batch_c,"--phys-analysis",batch_phys,"--c-model",target_c,"--phys-model",target_phys,
                 "--c-source",consumer / "batch-target-model.c","--phys-source",consumer / "batch-target-model.phys",
                 "--probe",batch_probe,"--legacy",old_tail,"--work",root / "Batch services"])
        print("Installed SDK Batch services: forty C/Physim/CLI configurations, pause/resume, exclusive files and old diagnostic tail passed",flush=True)
        shutil.copy2(sdk / "examples/documentation/batch_analysis.c",consumer / "batch-documentation.c")
        batch_rebuilt=builder.executable("batch-documentation-rebuilt",["batch-documentation.c"],[rebuilt_core],module=True)
        for name,c_analyzer,phys_analyzer,c_model,phys_model in (
                ("installed",sdk / "bin" / ("batch_analysis"+module_suffix),sdk / "bin" / ("language-batch_analysis"+module_suffix),
                 sdk / "bin" / ("uncertain_projectile"+module_suffix),sdk / "bin" / ("language-uncertain_projectile"+module_suffix)),
                ("rebuilt",batch_rebuilt,modules["batch_analysis"],c_modules["uncertain_projectile"],modules["uncertain_projectile"])):
            checked([sys.executable,repo / "tests/test_batch_documentation.py",sdk / "bin" / ("physim-analysis-runner"+suffix),
                     c_analyzer,phys_analyzer,c_model,phys_model,batch_probe,root / ("Batch documentation "+name)])
        print("Installed SDK Batch documentation: installed/rebuilt Core, eight mixed analyses and 2048 independently checked raw archives passed",flush=True)
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
        shutil.copy2(repo / "tests/test_contact_world.c",consumer / "contact-world-probe.c")
        shutil.copy2(sdk / "examples/contact_stack/main.c",consumer / "contact-stack.c")
        shutil.copy2(repo / "tests/test_language_contact_world_memory.c",consumer / "contact-world-memory.c")
        for kind,archive in (("installed",library),("rebuilt",rebuilt_core)):
            memory=builder.executable("contact-world-memory-"+kind,["contact-world-memory.c"],[archive]);checked([memory])
            program=builder.executable("contact-world-probe-"+kind,["contact-world-probe.c"],[archive])
            checked([program])
            experiment=builder.executable("contact-stack-"+kind,["contact-stack.c"],[archive],module=True)
            directory=root / ("Contact world "+kind);directory.mkdir()
            checked([sys.executable,repo / "tests/test_runner_contact_world.py",sdk / "bin" / ("physim-runner"+suffix),experiment,directory])
            parity=root / ("Contact world parity "+kind);parity.mkdir()
            language_stack=modules["contact_stack"] if kind=="rebuilt" else sdk / "bin" / ("language-contact_stack"+module_suffix)
            checked([sys.executable,repo / "tests/test_contact_world_language_parity.py",sdk / "bin" / ("physim-runner"+suffix),experiment,language_stack,parity])
        print("Installed SDK contact world: installed/rebuilt Core, analytic warm stack, lifecycle and real recorded simulations passed",flush=True)


        for source in ("material_main", "material_analysis"):
            shutil.copy2(sdk / "examples/documentation" / (source+".c"),consumer / (source+".c"))
        shutil.copy2(repo / "tests/test_material_tutorial_report.c",consumer / "material-report-probe.c")
        for kind,archive in (("installed",library),("rebuilt",rebuilt_core)):
            experiment=builder.executable("material-main-"+kind,["material_main.c"],[archive],module=True)
            analyzer=builder.executable("material-analysis-"+kind,["material_analysis.c"],[archive],module=True)
            material_probe=builder.executable("material-report-"+kind,["material-report-probe.c"],[archive])
            directory=root / ("Material tutorial "+kind);directory.mkdir()
            language_main=modules["material_main"] if kind=="rebuilt" else sdk / "bin" / ("language-material_main"+module_suffix)
            language_analysis=modules["material_analysis"] if kind=="rebuilt" else sdk / "bin" / ("language-material_analysis"+module_suffix)
            checked([sys.executable,repo / "tests/test_material_tutorial.py",sdk / "bin" / ("physim-runner"+suffix),
                     sdk / "bin" / ("physim-analysis-runner"+suffix),experiment,language_main,analyzer,language_analysis,material_probe,directory])
        print("Installed SDK material tutorial: installed/rebuilt Core, seven analytic scenarios and all four analysis combinations passed",flush=True)

        for source in ("thermal_main","thermal_analysis"):
            shutil.copy2(sdk / "examples/documentation" / (source+".c"),consumer / (source+".c"))
        shutil.copy2(repo / "tests/test_thermal_tutorial_report.c",consumer / "thermal-tutorial-probe.c")
        shutil.copy2(repo / "tests/test_thermodynamics.c",consumer / "thermal-core-check.c")
        for kind,archive in (("installed",library),("rebuilt",rebuilt_core)):
            thermal_core=builder.executable("thermal-core-"+kind,["thermal-core-check.c"],[archive])
            checked([thermal_core])
            thermal_experiment=builder.executable("thermal-experiment-"+kind,["thermal_main.c"],[archive],module=True)
            thermal_analyzer=builder.executable("thermal-analysis-"+kind,["thermal_analysis.c"],[archive],module=True)
            thermal_probe=builder.executable("thermal-probe-"+kind,["thermal-tutorial-probe.c"],[archive])
            directory=root / ("Thermal tutorial "+kind);directory.mkdir()
            thermal_language=modules["thermal_main"] if kind=="rebuilt" else sdk / "bin" / ("language-thermal_main"+module_suffix)
            thermal_analysis=modules["thermal_analysis"] if kind=="rebuilt" else sdk / "bin" / ("language-thermal_analysis"+module_suffix)
            checked([sys.executable,repo / "tests/test_thermal_tutorial.py",sdk / "bin" / ("physim-runner"+suffix),
                     sdk / "bin" / ("physim-analysis-runner"+suffix),thermal_experiment,thermal_language,
                     thermal_analyzer,thermal_analysis,thermal_probe,directory])
        print("Installed SDK thermodynamics: installed/rebuilt Core, six Decimal scenarios, 48 mixed analyses and atomic numeric errors passed",flush=True)

        for source in ("rc_main","rc_analysis"):
            shutil.copy2(sdk / "examples/documentation" / (source+".c"),consumer / (source+".c"))
        shutil.copy2(repo / "tests/test_rc_tutorial_report.c",consumer / "rc-tutorial-probe.c")
        shutil.copy2(repo / "tests/test_electromagnetism.c",consumer / "electromagnetism-core-check.c")
        for kind,archive in (("installed",library),("rebuilt",rebuilt_core)):
            em_core=builder.executable("electromagnetism-core-"+kind,["electromagnetism-core-check.c"],[archive])
            checked([em_core])
            rc_experiment=builder.executable("rc-experiment-"+kind,["rc_main.c"],[archive],module=True)
            rc_analyzer=builder.executable("rc-analysis-"+kind,["rc_analysis.c"],[archive],module=True)
            rc_probe=builder.executable("rc-probe-"+kind,["rc-tutorial-probe.c"],[archive])
            directory=root / ("RC tutorial "+kind);directory.mkdir()
            rc_language=modules["rc_main"] if kind=="rebuilt" else sdk / "bin" / ("language-rc_main"+module_suffix)
            rc_analysis=modules["rc_analysis"] if kind=="rebuilt" else sdk / "bin" / ("language-rc_analysis"+module_suffix)
            checked([sys.executable,repo / "tests/test_rc_tutorial.py",sdk / "bin" / ("physim-runner"+suffix),
                     sdk / "bin" / ("physim-analysis-runner"+suffix),rc_experiment,rc_language,
                     rc_analyzer,rc_analysis,rc_probe,directory])
        print("Installed SDK electromagnetism: installed/rebuilt Core, fields, Lorentz work, seven Decimal RC scenarios and 56 mixed analyses passed",flush=True)

        shutil.copy2(sdk / "examples/spring/main.c",consumer / "spring-tutorial.c")
        shutil.copy2(sdk / "examples/documentation/spring_analysis.c",consumer / "spring-tutorial-analysis.c")
        shutil.copy2(repo / "tests/test_spring_tutorial_report.c",consumer / "spring-tutorial-probe.c")
        for kind,archive in (("installed",library),("rebuilt",rebuilt_core)):
            spring_experiment=builder.executable("spring-tutorial-"+kind,["spring-tutorial.c"],[archive],module=True)
            spring_analyzer=builder.executable("spring-tutorial-analysis-"+kind,["spring-tutorial-analysis.c"],[archive],module=True)
            spring_probe=builder.executable("spring-tutorial-probe-"+kind,["spring-tutorial-probe.c"],[archive])
            directory=root / ("Spring tutorial "+kind);directory.mkdir()
            spring_language=modules["spring"] if kind=="rebuilt" else sdk / "bin" / ("language-spring"+module_suffix)
            spring_analysis=modules["spring_analysis"] if kind=="rebuilt" else sdk / "bin" / ("language-spring_analysis"+module_suffix)
            checked([sys.executable,repo / "tests/test_spring_tutorial.py",sdk / "bin" / ("physim-runner"+suffix),
                     sdk / "bin" / ("physim-analysis-runner"+suffix),spring_experiment,spring_language,spring_analyzer,spring_analysis,spring_probe,directory])
        print("Installed SDK spring tutorial: installed/rebuilt Core, four damping regimes and sixteen mixed analyses passed",flush=True)

        for source in ("pendulum_main", "pendulum_analysis"):
            shutil.copy2(sdk / "examples/documentation" / (source+".c"),consumer / (source+".c"))
        shutil.copy2(repo / "tests/test_pendulum_tutorial_report.c",consumer / "pendulum-tutorial-probe.c")
        for kind,archive in (("installed",library),("rebuilt",rebuilt_core)):
            pendulum_experiment=builder.executable("pendulum-tutorial-"+kind,["pendulum_main.c"],[archive],module=True)
            pendulum_analyzer=builder.executable("pendulum-tutorial-analysis-"+kind,["pendulum_analysis.c"],[archive],module=True)
            pendulum_probe=builder.executable("pendulum-tutorial-probe-"+kind,["pendulum-tutorial-probe.c"],[archive])
            directory=root / ("Pendulum tutorial "+kind);directory.mkdir()
            pendulum_language=modules["pendulum_main"] if kind=="rebuilt" else sdk / "bin" / ("language-pendulum_main"+module_suffix)
            pendulum_analysis=modules["pendulum_analysis"] if kind=="rebuilt" else sdk / "bin" / ("language-pendulum_analysis"+module_suffix)
            checked([sys.executable,repo / "tests/test_pendulum_tutorial.py",sdk / "bin" / ("physim-runner"+suffix),
                     sdk / "bin" / ("physim-analysis-runner"+suffix),pendulum_experiment,pendulum_language,
                     pendulum_analyzer,pendulum_analysis,pendulum_probe,directory])
        pendulum_documented=root / "Pendulum documented project";pendulum_documented.mkdir()
        shutil.copy2(sdk / "examples/documentation/pendulum_main.c",pendulum_documented / "main.c")
        shutil.copy2(sdk / "examples/documentation/pendulum_analysis.c",pendulum_documented / "analysis.c")
        (pendulum_documented / "physim.project").write_text("physim_project=1\n",encoding="utf-8")
        pendulum_output=pendulum_documented / "build/Release"
        checked([sdk / "bin" / ("physim-build"+suffix),"--project",pendulum_documented,"--sdk",sdk,
                 "--output",pendulum_output,"--physimc",sdk / "bin" / ("physimc"+suffix),"--profile","Release"])
        pendulum_results=root / "Pendulum documented results";pendulum_results.mkdir()
        checked([sys.executable,repo / "tests/test_pendulum_tutorial.py",sdk / "bin" / ("physim-runner"+suffix),
                 sdk / "bin" / ("physim-analysis-runner"+suffix),pendulum_output / ("experiment"+module_suffix),
                 sdk / "bin" / ("language-pendulum_main"+module_suffix),pendulum_output / ("analysis"+module_suffix),
                 sdk / "bin" / ("language-pendulum_analysis"+module_suffix),pendulum_probe,pendulum_results])
        print("Installed SDK pendulum tutorial: installed/rebuilt Core, documented native build, five integrators and mixed/adaptive reports passed",flush=True)

        for source in ("collision_main", "collision_analysis"):
            shutil.copy2(sdk / "examples/documentation" / (source+".c"),consumer / (source+".c"))
        shutil.copy2(repo / "tests/test_collision_tutorial_report.c",consumer / "collision-tutorial-probe.c")
        for kind,archive in (("installed",library),("rebuilt",rebuilt_core)):
            collision_experiment=builder.executable("collision-tutorial-"+kind,["collision_main.c"],[archive],module=True)
            collision_analyzer=builder.executable("collision-tutorial-analysis-"+kind,["collision_analysis.c"],[archive],module=True)
            collision_probe=builder.executable("collision-tutorial-probe-"+kind,["collision-tutorial-probe.c"],[archive])
            directory=root / ("Collision tutorial "+kind);directory.mkdir()
            collision_language=modules["collision_main"] if kind=="rebuilt" else sdk / "bin" / ("language-collision_main"+module_suffix)
            collision_analysis=modules["collision_analysis"] if kind=="rebuilt" else sdk / "bin" / ("language-collision_analysis"+module_suffix)
            checked([sys.executable,repo / "tests/test_collision_tutorial.py",sdk / "bin" / ("physim-runner"+suffix),
                     sdk / "bin" / ("physim-analysis-runner"+suffix),collision_experiment,collision_language,
                     collision_analyzer,collision_analysis,collision_probe,directory])
        collision_documented=root / "Collision documented project";collision_documented.mkdir()
        shutil.copy2(sdk / "examples/documentation/collision_main.c",collision_documented / "main.c")
        shutil.copy2(sdk / "examples/documentation/collision_analysis.c",collision_documented / "analysis.c")
        (collision_documented / "physim.project").write_text("physim_project=1\n",encoding="utf-8")
        collision_output=collision_documented / "build/Release"
        checked([sdk / "bin" / ("physim-build"+suffix),"--project",collision_documented,"--sdk",sdk,
                 "--output",collision_output,"--physimc",sdk / "bin" / ("physimc"+suffix),"--profile","Release"])
        collision_results=root / "Collision documented results";collision_results.mkdir()
        checked([sys.executable,repo / "tests/test_collision_tutorial.py",sdk / "bin" / ("physim-runner"+suffix),
                 sdk / "bin" / ("physim-analysis-runner"+suffix),collision_output / ("experiment"+module_suffix),
                 sdk / "bin" / ("language-collision_main"+module_suffix),collision_output / ("analysis"+module_suffix),
                 sdk / "bin" / ("language-collision_analysis"+module_suffix),collision_probe,collision_results])
        print("Installed SDK collision tutorial: installed/rebuilt Core, documented native build, eleven exact scenarios and mixed reports passed",flush=True)

        for source in ("monte_carlo_main", "monte_carlo_analysis"):
            shutil.copy2(sdk / "examples/documentation" / (source+".c"),consumer / (source+".c"))
        shutil.copy2(repo / "tests/test_monte_carlo_tutorial_report.c",consumer / "monte-carlo-tutorial-probe.c")
        for kind,archive in (("installed",library),("rebuilt",rebuilt_core)):
            monte_carlo_experiment=builder.executable("monte-carlo-tutorial-"+kind,["monte_carlo_main.c"],[archive],module=True)
            monte_carlo_analyzer=builder.executable("monte-carlo-tutorial-analysis-"+kind,["monte_carlo_analysis.c"],[archive],module=True)
            monte_carlo_probe=builder.executable("monte-carlo-tutorial-probe-"+kind,["monte-carlo-tutorial-probe.c"],[archive])
            directory=root / ("Monte Carlo tutorial "+kind);directory.mkdir()
            monte_carlo_language=modules["monte_carlo_main"] if kind=="rebuilt" else sdk / "bin" / ("language-monte_carlo_main"+module_suffix)
            monte_carlo_analysis=modules["monte_carlo_analysis"] if kind=="rebuilt" else sdk / "bin" / ("language-monte_carlo_analysis"+module_suffix)
            checked([sys.executable,repo / "tests/test_monte_carlo_tutorial.py",sdk / "bin" / ("physim-runner"+suffix),
                     sdk / "bin" / ("physim-batch"+suffix), sdk / "bin" / ("physim-analysis-runner"+suffix),monte_carlo_experiment,monte_carlo_language,
                     monte_carlo_analyzer,monte_carlo_analysis,monte_carlo_probe,directory])
        monte_carlo_documented=root / "Monte Carlo documented project";monte_carlo_documented.mkdir()
        shutil.copy2(sdk / "examples/documentation/monte_carlo_main.c",monte_carlo_documented / "main.c")
        shutil.copy2(sdk / "examples/documentation/monte_carlo_analysis.c",monte_carlo_documented / "analysis.c")
        (monte_carlo_documented / "physim.project").write_text("physim_project=1\n",encoding="utf-8")
        monte_carlo_output=monte_carlo_documented / "build/Release"
        checked([sdk / "bin" / ("physim-build"+suffix),"--project",monte_carlo_documented,"--sdk",sdk,
                 "--output",monte_carlo_output,"--physimc",sdk / "bin" / ("physimc"+suffix),"--profile","Release"])
        monte_carlo_results=root / "Monte Carlo documented results";monte_carlo_results.mkdir()
        checked([sys.executable,repo / "tests/test_monte_carlo_tutorial.py",sdk / "bin" / ("physim-runner"+suffix),
                 sdk / "bin" / ("physim-batch"+suffix), sdk / "bin" / ("physim-analysis-runner"+suffix),monte_carlo_output / ("experiment"+module_suffix),
                 sdk / "bin" / ("language-monte_carlo_main"+module_suffix),monte_carlo_output / ("analysis"+module_suffix),
                 sdk / "bin" / ("language-monte_carlo_analysis"+module_suffix),monte_carlo_probe,monte_carlo_results])
        print("Installed SDK Monte Carlo tutorial: installed/rebuilt Core, documented native build, seeded ensembles, worker reproducibility, constant populations and mixed reports passed",flush=True)

        shutil.copy2(sdk / "examples/documentation/main.c",consumer / "saved-run-producer.c")
        shutil.copy2(sdk / "examples/documentation/saved_run_analysis.c",consumer / "saved-run-analysis.c")
        shutil.copy2(repo / "tests/test_saved_run_tutorial_report.c",consumer / "saved-run-probe.c")
        checked([sdk / "bin" / ("physimc"+suffix),"--emit-experiment",sdk / "examples/documentation/language_main.phys"],output=consumer / "saved-run-producer-phys.c")
        for kind,archive in (("installed",library),("rebuilt",rebuilt_core)):
            saved_producer=builder.executable("saved-run-producer-"+kind,["saved-run-producer.c"],[archive],module=True)
            saved_producer_phys=builder.executable("saved-run-producer-phys-"+kind,["saved-run-producer-phys.c"],[archive],module=True,language=True)
            saved_analyzer=builder.executable("saved-run-analysis-"+kind,["saved-run-analysis.c"],[archive],module=True)
            saved_probe=builder.executable("saved-run-probe-"+kind,["saved-run-probe.c"],[archive])
            saved_language=modules["saved_run_analysis"] if kind=="rebuilt" else sdk / "bin" / ("language-saved_run_analysis"+module_suffix)
            directory=root / ("Saved run tutorial "+kind);directory.mkdir()
            checked([sys.executable,repo / "tests/test_saved_run_tutorial.py",sdk / "bin" / ("physim-runner"+suffix),
                     sdk / "bin" / ("physim-analysis-runner"+suffix),saved_producer,saved_producer_phys,saved_analyzer,saved_language,saved_probe,directory])
        saved_documented=root / "Saved documented producer";saved_documented.mkdir()
        shutil.copy2(sdk / "examples/documentation/main.c",saved_documented / "main.c")
        shutil.copy2(sdk / "examples/documentation/analysis.c",saved_documented / "analysis.c")
        (saved_documented / "physim.project").write_text("physim_project=1\n",encoding="utf-8")
        saved_output=saved_documented / "build/Release"
        checked([sdk / "bin" / ("physim-build"+suffix),"--project",saved_documented,"--sdk",sdk,
                 "--output",saved_output,"--physimc",sdk / "bin" / ("physimc"+suffix),"--profile","Release"])
        saved_consumer=root / "Saved documented analysis";saved_consumer.mkdir()
        shutil.copy2(sdk / "examples/documentation/saved_run_analysis.c",saved_consumer / "analysis.c")
        (saved_consumer / "physim.project").write_text("physim_project=2\nkind=analysis\nanalysis=analysis.c\n",encoding="utf-8")
        saved_consumer_output=saved_consumer / "build/Release"
        checked([sdk / "bin" / ("physim-build"+suffix),"--project",saved_consumer,"--sdk",sdk,
                 "--output",saved_consumer_output,"--physimc",sdk / "bin" / ("physimc"+suffix),"--profile","Release"])
        if (saved_consumer_output / ("experiment"+module_suffix)).exists():
            raise RuntimeError("Documented analysis-only project built an experiment")
        saved_results=root / "Saved documented results";saved_results.mkdir()
        checked([sys.executable,repo / "tests/test_saved_run_tutorial.py",sdk / "bin" / ("physim-runner"+suffix),
                 sdk / "bin" / ("physim-analysis-runner"+suffix),saved_output / ("experiment"+module_suffix),saved_producer_phys,
                 saved_consumer_output / ("analysis"+module_suffix),sdk / "bin" / ("language-saved_run_analysis"+module_suffix),saved_probe,saved_results])
        print("Installed SDK saved run tutorial: installed/rebuilt Core, documented producer and analysis-only build, sixteen mixed reports and immutable archives passed",flush=True)

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
                checked([sdk / "bin" / ("physim"+suffix),"--docs-test",root / "Documentation routes"])
                print("Installed SDK documentation window: both learning route buttons, shared links, route home and small-window rendering passed",flush=True)
        elif args.app_tests:
            raise RuntimeError("App tests require an SDK with the app")
    (root / "PASSED.txt").write_text(
        "Native SDK relocation, independent headers, installed and rebuilt core archives, eight bundled and rebuilt C templates, "
        f"{len(native.LANGUAGE_PROGRAMS)} language programs, {len(modules)} rebuilt language modules, nine language experiments with both general analyses, "
        "specialized sensor analysis and six C/Physim combinations passed.\n"
        "Adaptive bundled/source C and Physim pendulums, actual variable sample times, energy and both analysis languages passed.\n" +
        "Common-target-time parameter studies from bundled/source C and Physim pendulums passed with installed and rebuilt probes.\n" +
        "Logging through installed/rebuilt C Core and Physim passed with exclusive JSONL and opt-in wire events.\n" +
        "Structured diagnostics through installed/rebuilt Core, C/Physim runners and legacy ABI passed.\n" +
        "Run indexes through installed/rebuilt Core, legacy/recovered files, allocator failures and million-row independent codec passed.\n" +
        "Indexed language values, copied owners, failure cleanup and all 256 SI report rows passed.\n" +
        "Persistent contact world and projected warm graph through installed/rebuilt Core and real C/Physim stack runners, copied snapshots and allocation failures passed.\n" +
        "Custom material/medium tutorial through installed/rebuilt Core, analytic scenarios, C/Physim parity and all mixed analyses passed.\n" +
        "Spring tutorial through installed/rebuilt Core, four damping regimes, typed parameters and sixteen mixed analyses passed.\n" +
        "Pendulum tutorial through installed/rebuilt Core, five integrators and mixed/adaptive reports passed.\n" +
        "Collision tutorial through installed/rebuilt Core, eleven exact scenarios and mixed reports passed.\n" +
        "Monte Carlo tutorial through installed/rebuilt Core, seeded ensembles, worker reproducibility and mixed reports passed.\n" +
        "Saved run tutorial through installed/rebuilt Core, eight archive scenarios, sixteen mixed reports and independent analysis-only projects passed.\n" +
        "Thermodynamics through installed/rebuilt Core, independent Decimal oracles, mixed reports and extreme-value contracts passed.\n" +
        "Electromagnetism through installed/rebuilt Core, Coulomb/gradient/Lorentz tests, seven RC Decimal oracles and mixed reports passed.\n" +
        "Both complete C/Physim learning routes, eight paired model source groups and executable language introductions passed.\n" +
        ("Nine projects rebuilt without CMake; sources unchanged and outputs confined to build/.\n" if metadata["app"] else "") +
        ("Eight C template GUI workflows, the complete Physim language GUI workflow and independent documentation route navigation passed.\n" if args.app_tests else ""), encoding="utf-8")
    print(f"Native SDK verified: {root}")


if __name__ == "__main__":
    main()
