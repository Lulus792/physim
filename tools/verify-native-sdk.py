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
    for name, digest in metadata["files"].items():
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
        header_sources = []
        for header in sorted((sdk / "include/physim").glob("*.h")):
            name = "header_" + header.stem + ".c"
            (consumer / name).write_text(f"#include <physim/{header.name}>\n", encoding="utf-8")
            header_sources.append(name)
        builder.objects(header_sources)

        def check_modules(name, experiment, analysis, language=False):
            run = root / (name + ".psrun")
            report = root / (name + "-report")
            checked([sdk / "bin" / ("physim-runner" + suffix), experiment, run,
                     "--steps", "200", "--dt", ".005", "--seed", "42"])
            checked([sdk / "bin" / ("physim-analysis-runner" + suffix), analysis, run, report])
            checked([probe, run, report.with_suffix(".psreport")] + (["language"] if language else []))
            print(f"Installed SDK: {name} passed", flush=True)

        for name in native.EXAMPLES:
            check_modules("bundled-" + name, sdk / "bin" / (name + module_suffix),
                          sdk / "bin" / ("pendulum_analysis" + module_suffix))

        programs = "energy motion flight_phases sampling phase_space rotation_path particles rigid_body contacts distance_joints constraint_graph sweeps coordinate_frames optional_values optional_bindings".split()
        for name in programs:
            source = consumer / (name + ".c")
            checked([sdk / "bin" / ("physimc" + suffix), "--emit-c",
                     sdk / "examples/language" / (name + ".phys")], output=source)
            program = builder.executable(name, [source.name], [library])
            checked([program])
            print(f"Installed language program: {name} passed", flush=True)

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
                for example in ("pendulum", "language_full"):
                    checked([sdk / "bin" / ("physim" + suffix), "--self-test", root / ("App " + example), example])
        elif args.app_tests:
            raise RuntimeError("App tests require an SDK with the app")
    (root / "PASSED.txt").write_text("Native SDK relocation, public headers, templates and language programs passed.\n", encoding="utf-8")
    print(f"Native SDK verified: {root}")


if __name__ == "__main__":
    main()
