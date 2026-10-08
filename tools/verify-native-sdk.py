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


def verify_accessibility_checkbox(repo,sdk,root,suffix,checked):
    checked([sys.executable,repo / "tests/test_accessibility_checkbox_app.py",
             "--app",sdk / "bin" / ("physim"+suffix),"--work",root / "Native checkbox"],timeout=90)
    checked([sys.executable,repo / "tests/test_accessibility_checkbox_app.py",
             "--app",sdk / "bin" / ("physim"+suffix),"--work",root / "Native options","--options"],timeout=90)
    print("Relocated SDK native checkbox: actual settings drafts, AppKit/AT-SPI checkbox/radio actions and applied configuration passed",flush=True)


def verify_documentation_bounds(sdk, files):
    """All packaged Markdown pages fit the actual offline viewer byte budget."""
    for name in files:
        if name.endswith(".md") and (name.startswith("docs/") or name in ("README.md", "Physim_Projektplan.md")):
            if (sdk / name).stat().st_size > 256 * 1024:
                raise RuntimeError(f"SDK page exceeds offline viewer limit: {name}")


def verify_scalar_search(repo, sdk, consumer, builder, library, rebuilt_core, suffix, checked):
    """Shared full/focused acceptance against installed and SDK-source-built Core."""
    shutil.copy2(repo / "tests/test_scalar_range.c", consumer / "scalar-range-check.c")
    shutil.copy2(repo / "tests/scalar_range_probe.c", consumer / "scalar-range-probe.c")
    checked([sdk / "bin" / ("physimc" + suffix), "--emit-c",
             repo / "tests/fixtures/language/scalar_range.phys"],
            output=consumer / "scalar-range-language.c")
    checked([sdk / "bin" / ("physimc" + suffix), "--emit-c",
             repo / "tests/fixtures/language/module_block_closures.phys"],
            output=consumer / "module-block-closures.c")
    for kind, archive in (("installed", library), ("rebuilt", rebuilt_core)):
        scalar_check = builder.executable("scalar-range-check-" + kind, ["scalar-range-check.c"], [archive])
        scalar_probe = builder.executable("scalar-range-probe-" + kind, ["scalar-range-probe.c"], [archive])
        scalar_language = builder.executable("scalar-range-language-" + kind, ["scalar-range-language.c"], [archive], language=True)
        closure_check = builder.executable("module-block-closures-" + kind, ["module-block-closures.c"], [archive], language=True)
        checked([scalar_check]);checked([closure_check])
        checked([sys.executable, repo / "tests/test_scalar_range_oracle.py",
                 "--c", scalar_probe, "--language", scalar_language,
                 "--fixture", repo / "tests/fixtures/language/scalar_range.phys"])
    print("Installed/rebuilt SDK scalar search: exact binary tolerance decisions, C/Physim reports, callback bounds/counters and owned module-block closures passed", flush=True)


def verify_transform_range(repo, sdk, consumer, builder, library, rebuilt_core, suffix, checked):
    """Exact point/direction and independent normal references for packaged Core."""
    shutil.copy2(repo / "tests/test_transform_range.c", consumer / "transform-range-check.c")
    shutil.copy2(repo / "tests/transform_range_probe.c", consumer / "transform-range-probe.c")
    checked([sdk / "bin" / ("physimc" + suffix), "--emit-c",
             repo / "tests/fixtures/language/transform_range.phys"],
            output=consumer / "transform-range-language.c")
    for kind, archive in (("installed", library), ("rebuilt", rebuilt_core)):
        check = builder.executable("transform-range-check-" + kind, ["transform-range-check.c"], [archive])
        probe = builder.executable("transform-range-probe-" + kind, ["transform-range-probe.c"], [archive])
        language = builder.executable("transform-range-language-" + kind, ["transform-range-language.c"], [archive], language=True)
        checked([check])
        checked([sys.executable, repo / "tests/test_transform_range_oracle.py", "--c", probe,
                 "--language", language, "--fixture", repo / "tests/fixtures/language/transform_range.phys"])
    print("Installed/rebuilt SDK transforms: exact point/direction rounding, Decimal normals, cancellation and atomic errors passed", flush=True)



def verify_run_stream(repo, consumer, builder, library, rebuilt_core, checked):
    """Exercise opaque public handles using only packaged headers and archives."""
    shutil.copy2(repo / "tests/test_run_stream.c", consumer / "run-stream-check.c")
    shutil.copy2(repo / "tests/test_allocator.h", consumer / "test_allocator.h")
    for kind, archive in (("installed", library), ("rebuilt", rebuilt_core)):
        executable = builder.executable("run-stream-check-" + kind, ["run-stream-check.c"], [archive])
        work = consumer / ("Run streams ä " + kind)
        work.mkdir()
        checked([executable, work])
    print("Installed/rebuilt SDK run streams: ownership, generation reuse, bounded slots, allocation failure, atomic reads, snapshots and recoverable abort passed", flush=True)


def verify_close_range(repo, sdk, consumer, builder, library, rebuilt_core, suffix, checked):
    """Independent exact tolerance decisions against packaged C and Physim APIs."""
    shutil.copy2(repo / "tests/close_range_probe.c", consumer / "close-range-probe.c")
    checked([sdk / "bin" / ("physimc" + suffix), "--emit-c",
             repo / "tests/fixtures/language/close_range.phys"], output=consumer / "close-range-language.c")
    for kind, archive in (("installed", library), ("rebuilt", rebuilt_core)):
        probe = builder.executable("close-range-probe-" + kind, ["close-range-probe.c"], [archive])
        language = builder.executable("close-range-language-" + kind, ["close-range-language.c"], [archive], language=True)
        checked([sys.executable, repo / "tests/test_close_range_oracle.py", "--c", probe,
                 "--language", language, "--fixture", repo / "tests/fixtures/language/close_range.phys"])
    print("Installed/rebuilt SDK comparisons: exact rational boundaries, symmetry, subnormals, overflow and C/Physim parity passed", flush=True)


def verify_curve_range(repo, sdk, consumer, builder, library, rebuilt_core, suffix, checked):
    """Exact rational cubic geometry against packaged public C/Physim APIs."""
    shutil.copy2(repo / "tests/curve_range_probe.c", consumer / "curve-range-probe.c")
    checked([sdk / "bin" / ("physimc" + suffix), "--emit-c",
             repo / "tests/fixtures/language/curve_range.phys"], output=consumer / "curve-range-language.c")
    for kind, archive in (("installed", library), ("rebuilt", rebuilt_core)):
        probe = builder.executable("curve-range-probe-" + kind, ["curve-range-probe.c"], [archive])
        language = builder.executable("curve-range-language-" + kind, ["curve-range-language.c"], [archive], language=True)
        checked([sys.executable, repo / "tests/test_curve_range_oracle.py", "--c", probe,
                 "--language", language, "--fixture", repo / "tests/fixtures/language/curve_range.phys"])
    print("Installed/rebuilt SDK Bezier curves: rational position, tangent, split controls, cancellation and C/Physim parity passed", flush=True)


def verify_unit_conversion(repo, sdk, consumer, builder, library, rebuilt_core, suffix, checked):
    """Exact conversion plus typed runtime diagnostics from packaged APIs."""
    shutil.copy2(repo / "tests/unit_conversion_probe.c", consumer / "unit-conversion-probe.c")
    shutil.copy2(repo / "tests/test_unit_runtime_codes.c", consumer / "unit-runtime-codes.c")
    shutil.copy2(repo / "tests/test_channel_quantity.c", consumer / "channel-quantity.c")
    shutil.copy2(repo / "tests/fixtures/channel_declaration.c", consumer / "channel-declaration.c")
    shutil.copy2(repo / "tests/channel_export_probe.c", consumer / "channel-export.c")
    checked([sdk / "bin" / ("physimc" + suffix), "--emit-experiment",
             repo / "tests/fixtures/language/channel_declaration.phys"], output=consumer / "channel-declaration-language.c")
    checked([sdk / "bin" / ("physimc" + suffix), "--emit-c",
             repo / "tests/fixtures/language/unit_conversion_range.phys"], output=consumer / "unit-conversion-language.c")
    for kind, archive in (("installed", library), ("rebuilt", rebuilt_core)):
        probe = builder.executable("unit-conversion-probe-" + kind, ["unit-conversion-probe.c"], [archive])
        runtime = builder.executable("unit-runtime-codes-" + kind, ["unit-runtime-codes.c"], [archive])
        language = builder.executable("unit-conversion-language-" + kind, ["unit-conversion-language.c"], [archive], language=True)
        checked([runtime])
        sampler = builder.executable("channel-quantity-" + kind, ["channel-quantity.c"], [archive])
        c_model = builder.executable("channel-declaration-" + kind, ["channel-declaration.c"], [archive], module=True)
        phys_model = builder.executable("channel-declaration-language-" + kind, ["channel-declaration-language.c"], [archive], language=True, module=True)
        exporter = builder.executable("channel-export-" + kind, ["channel-export.c"], [archive])
        checked([sampler])
        checked([sys.executable, repo / "tests/test_channel_declaration_workflow.py",
                 "--runner", sdk / "bin" / ("physim-runner" + suffix), "--c-model", c_model,
                 "--phys-model", phys_model, "--exporter", exporter,
                 "--work", consumer / ("Typed channel runs ä " + kind)])
        checked([sys.executable, repo / "tests/test_unit_conversion_oracle.py", "--c", probe,
                 "--language", language, "--fixture", repo / "tests/fixtures/language/unit_conversion_range.phys"])
    print("Installed/rebuilt SDK units: exact rational conversions, SI definitions, identity bits, range/dimension failures, aliases, typed diagnostics, typed SI channel sampling, actual C/Physim sensor runs/CSV and parity passed", flush=True)

def verify_body_range(repo, sdk, consumer, builder, library, rebuilt_core, suffix, checked):
    """Independent solid-body and rotated-energy references for packaged Core."""
    shutil.copy2(repo / "tests/body_range_probe.c", consumer / "body-range-probe.c")
    checked([sdk / "bin" / ("physimc" + suffix), "--emit-c",
             repo / "tests/fixtures/language/body_range.phys"], output=consumer / "body-range-language.c")
    for kind, archive in (("installed", library), ("rebuilt", rebuilt_core)):
        probe = builder.executable("body-range-probe-" + kind, ["body-range-probe.c"], [archive])
        language = builder.executable("body-range-language-" + kind, ["body-range-language.c"], [archive], language=True)
        checked([sys.executable, repo / "tests/test_body_range_oracle.py", "--c", probe,
                 "--language", language, "--fixture", repo / "tests/fixtures/language/body_range.phys"])
    print("Installed/rebuilt SDK bodies: rational sphere/box inertia, identity energy, independent Decimal rotated energy and atomic range errors passed", flush=True)


def verify_convex_contacts(repo, sdk, consumer, builder, library, rebuilt_core, suffix, checked):
    """Public geometry, impulse response and C/Physim parity from packaged Core."""
    shutil.copy2(repo / "tests/test_ccd_step.c", consumer / "ccd-step-check.c")
    shutil.copy2(repo / "tests/test_ccd_step_memory.c", consumer / "ccd-step-memory.c")
    shutil.copy2(repo / "tests/test_allocator.h", consumer / "test_allocator.h")
    shutil.copy2(repo / "tests/ccd_step_probe.c", consumer / "ccd-step-probe.c")
    shutil.copy2(sdk / "examples/ccd_events/main.c", consumer / "ccd-events.c")
    checked([sdk / "bin" / ("physimc" + suffix), "--emit-c",
             repo / "tests/fixtures/language/ccd_step.phys"], output=consumer / "ccd-step-language.c")
    checked([sdk / "bin" / ("physimc" + suffix), "--emit-experiment",
             sdk / "examples/language/ccd_events.phys"], output=consumer / "ccd-events-language.c")
    shutil.copy2(repo / "tests/test_motion_sweep.c", consumer / "motion-sweep-check.c")
    shutil.copy2(repo / "tests/motion_sweep_probe.c", consumer / "motion-sweep-probe.c")
    checked([sdk / "bin" / ("physimc" + suffix), "--emit-c",
             repo / "tests/fixtures/language/motion_sweep.phys"], output=consumer / "motion-sweep-language.c")
    shutil.copy2(repo / "tests/test_convex.c", consumer / "convex-check.c")
    shutil.copy2(repo / "tests/test_convex_sweep.c", consumer / "convex-sweep-check.c")
    shutil.copy2(repo / "tests/convex_sweep_probe.c", consumer / "convex-sweep-probe.c")
    shutil.copy2(repo / "tests/test_convex_runtime.c", consumer / "convex-runtime.c")
    shutil.copy2(repo / "tests/convex_probe.c", consumer / "convex-probe.c")
    shutil.copy2(sdk / "examples/convex_contacts/main.c", consumer / "convex-example.c")
    checked([sdk / "bin" / ("physimc" + suffix), "--emit-c",
             repo / "tests/fixtures/language/convex.phys"], output=consumer / "convex-language.c")
    checked([sdk / "bin" / ("physimc" + suffix), "--emit-c",
             sdk / "examples/language/convex_contacts.phys"], output=consumer / "convex-example-language.c")
    checked([sdk / "bin" / ("language-convex_contacts" + suffix)])
    for kind, archive in (("installed", library), ("rebuilt", rebuilt_core)):
        ccd_check = builder.executable("ccd-step-check-" + kind, ["ccd-step-check.c"], [archive])
        ccd_memory = builder.executable("ccd-step-memory-" + kind, ["ccd-step-memory.c"], [archive])
        ccd_probe = builder.executable("ccd-step-probe-" + kind, ["ccd-step-probe.c"], [archive])
        ccd_language = builder.executable("ccd-step-language-" + kind, ["ccd-step-language.c"], [archive], language=True)
        ccd_c = builder.executable("ccd-events-" + kind, ["ccd-events.c"], [archive], module=True)
        ccd_phys = builder.executable("ccd-events-language-" + kind, ["ccd-events-language.c"], [archive], module=True, language=True)
        checked([ccd_check]); checked([ccd_memory]); checked([ccd_language])
        checked([sys.executable, repo / "tests/test_ccd_step_oracle.py", "--probe", ccd_probe])
        checked([sys.executable, repo / "tests/test_ccd_event_workflow.py",
                 "--runner", sdk / "bin" / ("physim-runner" + suffix),
                 "--c-model", ccd_c, "--phys-model", ccd_phys, "--work", consumer / ("ccd-runs-" + kind)])
        check = builder.executable("convex-check-" + kind, ["convex-check.c"], [archive])
        probe = builder.executable("convex-probe-" + kind, ["convex-probe.c"], [archive])
        language = builder.executable("convex-language-" + kind, ["convex-language.c"], [archive], language=True)
        example = builder.executable("convex-example-" + kind, ["convex-example.c"], [archive])
        example_language = builder.executable("convex-example-language-" + kind, ["convex-example-language.c"], [archive], language=True)
        runtime = builder.executable("convex-runtime-" + kind, ["convex-runtime.c"], [archive])
        checked([check]); checked([runtime]); checked([language]); checked([example]); checked([example_language])
        checked([sys.executable, repo / "tests/test_convex_oracle.py", "--probe", probe])
        sweep_check = builder.executable("convex-sweep-check-" + kind, ["convex-sweep-check.c"], [archive])
        sweep_probe = builder.executable("convex-sweep-probe-" + kind, ["convex-sweep-probe.c"], [archive])
        checked([sweep_check])
        checked([sys.executable, repo / "tests/test_convex_sweep_oracle.py", "--probe", sweep_probe])
        motion_check = builder.executable("motion-sweep-check-" + kind, ["motion-sweep-check.c"], [archive])
        motion_probe = builder.executable("motion-sweep-probe-" + kind, ["motion-sweep-probe.c"], [archive])
        motion_language = builder.executable("motion-sweep-language-" + kind, ["motion-sweep-language.c"], [archive], language=True)
        checked([motion_check]); checked([motion_language])
        checked([sys.executable, repo / "tests/test_motion_sweep_oracle.py", "--probe", motion_probe])
    print("Installed/rebuilt SDK convex contacts: validated closed meshes, independent SAT/witnesses, mixed contacts, explicit inertia, impulse response, C/Physim event examples, linear/rotating/quadratic sweeps, typed motion values, multi-body full-frame event stepping, owned CCD results and unresolved atomic limits passed", flush=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--sdk", type=Path, required=True)
    parser.add_argument("--work", type=Path, required=True)
    parser.add_argument("--compiler")
    parser.add_argument("--app-tests", action="store_true")
    parser.add_argument("--scalar-only", action="store_true",
                        help="Verify scalar search and module-block closures only; no GUI or other domain acceptance")
    parser.add_argument("--transform-only", action="store_true",
                        help="Verify checked transformation range only; no GUI or full domain acceptance")
    parser.add_argument("--stream-only", action="store_true",
                        help="Verify opaque Run stream handles only; no GUI or full domain acceptance")
    parser.add_argument("--comparison-only", action="store_true",
                        help="Verify exact C/Physim tolerance decisions only; no GUI or full domain acceptance")
    parser.add_argument("--curve-only", action="store_true",
                        help="Verify exact cubic curves only; no GUI or full domain acceptance")
    parser.add_argument("--units-only", action="store_true",
                        help="Verify unit conversion and diagnostics only; no GUI or full domain acceptance")
    parser.add_argument("--body-only", action="store_true",
                        help="Verify solid-body inertia and energy range only; no full mechanics or GUI acceptance")
    parser.add_argument("--accessibility-only", action="store_true",
                        help="Verify the packaged app's actual native checkbox and radio settings; requires a display")
    parser.add_argument("--convex-only", action="store_true",
                        help="Verify convex geometry and bounded multi-body CCD bindings; no full mechanics or GUI acceptance")
    args = parser.parse_args()
    if args.accessibility_only and any((args.scalar_only,args.transform_only,args.stream_only,
                                      args.comparison_only,args.curve_only,args.units_only,
                                      args.body_only,args.convex_only,args.app_tests)):
        parser.error("--accessibility-only cannot be combined with other focused modes or --app-tests")
    if args.convex_only and (args.body_only or args.units_only or args.curve_only or args.comparison_only or args.stream_only or args.transform_only or args.scalar_only or args.app_tests):
        parser.error("--convex-only cannot be combined with other focused modes or --app-tests")
    if args.body_only and (args.scalar_only or args.transform_only or args.stream_only or args.comparison_only or args.curve_only or args.units_only or args.app_tests):
        parser.error("--body-only cannot be combined with other focused modes or --app-tests")
    if args.units_only and (args.scalar_only or args.transform_only or args.stream_only or args.comparison_only or args.curve_only or args.app_tests):
        parser.error("--units-only cannot be combined with other focused modes or --app-tests")
    if args.curve_only and (args.scalar_only or args.transform_only or args.stream_only or args.comparison_only or args.app_tests):
        parser.error("--curve-only cannot be combined with other focused modes or --app-tests")
    if args.comparison_only and (args.scalar_only or args.transform_only or args.stream_only or args.app_tests):
        parser.error("--comparison-only cannot be combined with other focused modes or --app-tests")
    if args.stream_only and (args.scalar_only or args.transform_only or args.app_tests):
        parser.error("--stream-only cannot be combined with other focused modes or --app-tests")
    if args.transform_only and (args.scalar_only or args.app_tests):
        parser.error("--transform-only cannot be combined with --scalar-only or --app-tests")
    if args.scalar_only and args.app_tests:
        parser.error("--scalar-only cannot be combined with --app-tests")
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
    verify_documentation_bounds(sdk, metadata["files"])
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

    def checked(command, *, output=None, timeout=300):
        result = subprocess.run([str(p) for p in command], cwd=root, env=env,
                                capture_output=True, timeout=timeout)
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
        if args.accessibility_only:
            if not metadata["app"] or sys.platform not in ("darwin","linux"):
                raise RuntimeError("Native checkbox verification requires a macOS/Linux app SDK")
            verify_accessibility_checkbox(repo,sdk,root,suffix,checked)
            (root / "PASSED.txt").write_text("Focused native settings checkbox SDK verification passed; relocated manifest, actual AppKit/AT-SPI checkbox/radio actions and settings drafts, applied configuration preserved; no full screenreader or product acceptance.\n",encoding="utf-8")
            print(f"Native checkbox SDK verified: {root}")
            return
        if args.convex_only:
            shutil.copytree(sdk / "src", consumer / "src")
            rebuilt_core = builder.archive("sdk-rebuilt-core", [f"src/{name}.c" for name in native.CORE])
            verify_convex_contacts(repo, sdk, consumer, builder, library, rebuilt_core, suffix, checked)
            (root / "PASSED.txt").write_text("Focused C/Physim convex SDK verification passed; installed/rebuilt Core, independent geometry witnesses, explicit inertia, mixed contacts, linear/rotating/quadratic sweeps/event examples, owned full-frame CCD controller, manifest and relocation; no full mechanics or GUI acceptance.\n", encoding="utf-8")
            print(f"Convex SDK verified: {root}")
            return
        if args.body_only:
            shutil.copytree(sdk / "src", consumer / "src")
            rebuilt_core = builder.archive("sdk-rebuilt-core", [f"src/{name}.c" for name in native.CORE])
            verify_body_range(repo, sdk, consumer, builder, library, rebuilt_core, suffix, checked)
            (root / "PASSED.txt").write_text("Focused C/Physim body SDK verification passed; installed/rebuilt Core, rational inertia/identity energy, Decimal rotated energy, manifest and relocation; no full mechanics or GUI acceptance.\n", encoding="utf-8")
            print(f"Body SDK verified: {root}")
            return
        if args.units_only:
            shutil.copytree(sdk / "src", consumer / "src")
            rebuilt_core = builder.archive("sdk-rebuilt-core", [f"src/{name}.c" for name in native.CORE])
            verify_unit_conversion(repo, sdk, consumer, builder, library, rebuilt_core, suffix, checked)
            (root / "PASSED.txt").write_text("Focused C/Physim unit SDK verification passed; installed/rebuilt Core, exact rational oracle, SI definitions, typed diagnostics, manifest and relocation; no full domain or GUI acceptance.\n", encoding="utf-8")
            print(f"Unit SDK verified: {root}")
            return
        if args.curve_only:
            shutil.copytree(sdk / "src", consumer / "src")
            rebuilt_core = builder.archive("sdk-rebuilt-core", [f"src/{name}.c" for name in native.CORE])
            verify_curve_range(repo, sdk, consumer, builder, library, rebuilt_core, suffix, checked)
            (root / "PASSED.txt").write_text("Focused C/Physim cubic curve SDK verification passed; installed/rebuilt Core, exact rational oracle, manifest and relocation; no full domain or GUI acceptance.\n", encoding="utf-8")
            print(f"Curve SDK verified: {root}")
            return
        if args.comparison_only:
            shutil.copytree(sdk / "src", consumer / "src")
            rebuilt_core = builder.archive("sdk-rebuilt-core", [f"src/{name}.c" for name in native.CORE])
            verify_close_range(repo, sdk, consumer, builder, library, rebuilt_core, suffix, checked)
            (root / "PASSED.txt").write_text("Focused C/Physim comparison SDK verification passed; installed/rebuilt Core, exact rational oracle, manifest and relocation; no full domain or GUI acceptance.\n", encoding="utf-8")
            print(f"Comparison SDK verified: {root}")
            return
        if args.stream_only:
            shutil.copytree(sdk / "src", consumer / "src")
            rebuilt_core = builder.archive("sdk-rebuilt-core", [f"src/{name}.c" for name in native.CORE])
            verify_run_stream(repo, consumer, builder, library, rebuilt_core, checked)
            (root / "PASSED.txt").write_text("Focused Run stream SDK verification passed; installed/rebuilt Core, opaque handles, manifest and relocation; no full domain or GUI acceptance.\n", encoding="utf-8")
            print(f"Run stream SDK verified: {root}")
            return
        if args.transform_only:
            shutil.copytree(sdk / "src", consumer / "src")
            rebuilt_core = builder.archive("sdk-rebuilt-core", [f"src/{name}.c" for name in native.CORE])
            verify_transform_range(repo, sdk, consumer, builder, library, rebuilt_core, suffix, checked)
            (root / "PASSED.txt").write_text("Focused transformation range SDK verification passed; installed/rebuilt Core, exact rational C/Physim points/directions, Decimal normals, manifest and relocation; no full domain or GUI acceptance.\n", encoding="utf-8")
            print(f"Transformation SDK verified: {root}")
            return
        if args.scalar_only:
            shutil.copytree(sdk / "src", consumer / "src")
            rebuilt_core = builder.archive("sdk-rebuilt-core", [f"src/{name}.c" for name in native.CORE])
            verify_scalar_search(repo, sdk, consumer, builder, library, rebuilt_core, suffix, checked)
            (root / "PASSED.txt").write_text(
                "Focused scalar-search SDK verification passed.\n"
                "Installed/rebuilt Core, exact C/Physim stopping decisions, callback bounds/counters,\n"
                "owned module-block closures, manifest and relocation; no full domain or GUI acceptance.\n", encoding="utf-8")
            print(f"Scalar-search SDK verified: {root}")
            return
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
        shutil.copy2(repo / "tests/test_series_numeric_extremes.c", consumer / "series-numeric-extremes.c")
        for kind, archive in (("installed", library), ("rebuilt", rebuilt_core)):
            numeric_probe = builder.executable("series-numeric-extremes-" + kind,
                                               ["series-numeric-extremes.c"], [archive])
            checked([numeric_probe, root / ("Series numeric " + kind)])
        checked([sdk / "bin" / ("physimc" + suffix), "--emit-analysis",
                 repo / "tests/fixtures/language/analysis_numeric_extremes.phys"],
                output=consumer / "analysis-numeric-extremes.c")
        for kind, archive in (("installed", library), ("rebuilt", rebuilt_core)):
            numeric_analysis = builder.executable("analysis-numeric-extremes-" + kind,
                ["analysis-numeric-extremes.c"], [archive], language=True, module=True)
            checked([sdk / "bin" / ("physim-analysis-runner" + suffix), numeric_analysis,
                     "--runs", root / ("Language numeric " + kind)])
        print("Installed/rebuilt SDK calculus: C and Physim extreme-value checks passed", flush=True)
        shutil.copy2(repo / "tests/rng_reference_probe.c", consumer / "rng-reference-probe.c")
        checked([sdk / "bin" / ("physimc" + suffix), "--emit-c",
                 repo / "tests/fixtures/language/rng_reference.phys"],
                output=consumer / "rng-reference-language.c")
        for kind, archive in (("installed", library), ("rebuilt", rebuilt_core)):
            rng_probe = builder.executable("rng-reference-" + kind,
                                           ["rng-reference-probe.c"], [archive])
            rng_language = builder.executable("rng-language-" + kind,
                ["rng-reference-language.c"], [archive], language=True)
            checked([sys.executable, repo / "tests/test_rng_reference.py",
                     "--c", rng_probe, "--language", rng_language])
        print("Installed/rebuilt SDK RNG: independent integer/normal references and C/Physim value snapshots passed", flush=True)
        shutil.copy2(repo / "tests/test_ode_range.c", consumer / "ode-range-check.c")
        shutil.copy2(repo / "tests/ode_range_probe.c", consumer / "ode-range-probe.c")
        checked([sdk / "bin" / ("physimc" + suffix), "--emit-c",
                 repo / "tests/fixtures/language/ode_range.phys"],
                output=consumer / "ode-range-language.c")
        for kind, archive in (("installed", library), ("rebuilt", rebuilt_core)):
            ode_check = builder.executable("ode-range-check-" + kind, ["ode-range-check.c"], [archive])
            ode_probe = builder.executable("ode-range-probe-" + kind, ["ode-range-probe.c"], [archive])
            ode_language = builder.executable("ode-range-language-" + kind, ["ode-range-language.c"], [archive], language=True)
            checked([ode_check])
            checked([sys.executable, repo / "tests/test_ode_range_oracle.py",
                     "--c", ode_probe, "--language", ode_language,
                     "--cases", repo / "tests/fixtures/ode_range_cases.json",
                     "--fixture", repo / "tests/fixtures/language/ode_range.phys"])
        print("Installed/rebuilt SDK ODE range: five C/Physim methods, 244 exact constant-solution references, 32 states and true overflow rollback passed", flush=True)
        verify_convex_contacts(repo, sdk, consumer, builder, library, rebuilt_core, suffix, checked)
        verify_body_range(repo, sdk, consumer, builder, library, rebuilt_core, suffix, checked)
        verify_unit_conversion(repo, sdk, consumer, builder, library, rebuilt_core, suffix, checked)
        verify_curve_range(repo, sdk, consumer, builder, library, rebuilt_core, suffix, checked)
        verify_close_range(repo, sdk, consumer, builder, library, rebuilt_core, suffix, checked)
        verify_run_stream(repo, consumer, builder, library, rebuilt_core, checked)
        verify_scalar_search(repo, sdk, consumer, builder, library, rebuilt_core, suffix, checked)
        verify_transform_range(repo, sdk, consumer, builder, library, rebuilt_core, suffix, checked)
        shutil.copy2(repo / "tests/test_linear_range.c", consumer / "linear-range-check.c")
        shutil.copy2(repo / "tests/linear_range_probe.c", consumer / "linear-range-probe.c")
        checked([sdk / "bin" / ("physimc" + suffix), "--emit-c",
                 repo / "tests/fixtures/language/linear_range.phys"],
                output=consumer / "linear-range-language.c")
        for kind, archive in (("installed", library), ("rebuilt", rebuilt_core)):
            linear_check = builder.executable("linear-range-check-" + kind, ["linear-range-check.c"], [archive])
            linear_probe = builder.executable("linear-range-probe-" + kind, ["linear-range-probe.c"], [archive])
            linear_language = builder.executable("linear-range-language-" + kind, ["linear-range-language.c"], [archive], language=True)
            checked([linear_check])
            checked([sys.executable, repo / "tests/test_linear_range_oracle.py",
                     "--c", linear_probe, "--language", linear_language,
                     "--cases", repo / "tests/fixtures/linear_range_cases.json",
                     "--fixture", repo / "tests/fixtures/language/linear_range.phys"])
        print("Installed/rebuilt SDK linear systems: C/Physim independent rational solutions/residuals, 1..32 dimensions and range rollback passed", flush=True)
        shutil.copy2(repo / "tests/test_series_si.c", consumer / "series-si-check.c")
        shutil.copy2(repo / "tests/test_series_si_report.c", consumer / "series-si-report.c")
        checked([sdk / "bin" / ("physimc" + suffix), "--emit-analysis",
                 repo / "tests/fixtures/language/analysis_series_si.phys"],
                output=consumer / "series-si-language.c")
        for kind, archive in (("installed", library), ("rebuilt", rebuilt_core)):
            si_check = builder.executable("series-si-check-" + kind, ["series-si-check.c"], [archive])
            si_report = builder.executable("series-si-report-" + kind, ["series-si-report.c"], [archive])
            si_language = builder.executable("series-si-language-" + kind, ["series-si-language.c"], [archive], module=True, language=True)
            checked([sys.executable, repo / "tests/test_series_si_workflow.py",
                     "--c-check", si_check, "--language", si_language,
                     "--analysis-runner", sdk / "bin" / ("physim-analysis-runner" + suffix),
                     "--report-probe", si_report, "--work", root / ("Series SI " + kind)])
        print("Installed/rebuilt SDK Series SI: cm/ms imports, multiblock algebra/calculus/resampling, canonical CSV/report and atomic conversion/quota failures passed", flush=True)
        shutil.copy2(repo / "tests/test_quantity_sum.c", consumer / "quantity-sum-check.c")
        shutil.copy2(repo / "tests/quantity_sum_probe.c", consumer / "quantity-sum-probe.c")
        checked([sdk / "bin" / ("physimc" + suffix), "--emit-c",
                 repo / "tests/fixtures/language/quantity_sum_values.phys"],
                output=consumer / "quantity-sum-language.c")
        for kind, archive in (("installed", library), ("rebuilt", rebuilt_core)):
            sum_check = builder.executable("quantity-sum-check-" + kind, ["quantity-sum-check.c"], [archive])
            sum_probe = builder.executable("quantity-sum-probe-" + kind, ["quantity-sum-probe.c"], [archive])
            sum_language = builder.executable("quantity-sum-language-" + kind, ["quantity-sum-language.c"], [archive], language=True)
            checked([sum_check])
            checked([sys.executable, repo / "tests/test_quantity_sum_oracle.py",
                     "--c", sum_probe, "--language", sum_language,
                     "--cases", repo / "tests/fixtures/quantity_sum_cases.json",
                     "--fixture", repo / "tests/fixtures/language/quantity_sum_values.phys"])
        print("Installed/rebuilt SDK quantity sums: overflowing conversion, subnormal tie cases, exact Fraction references, left units and atomic errors passed", flush=True)
        shutil.copy2(repo / "tests/test_channel_declaration.c", consumer / "channel-declaration-check.c")
        shutil.copy2(repo / "tests/fixtures/channel_declaration.c", consumer / "channel-declaration-model.c")
        shutil.copy2(repo / "tests/channel_export_probe.c", consumer / "channel-export-probe.c")
        checked([sdk / "bin" / ("physimc" + suffix), "--emit-experiment",
                 repo / "tests/fixtures/language/channel_declaration.phys"],
                output=consumer / "channel-declaration-phys.c")
        for kind, archive in (("installed", library), ("rebuilt", rebuilt_core)):
            channel_check = builder.executable("channel-declaration-check-" + kind, ["channel-declaration-check.c"], [archive])
            channel_work = root / ("Channel declaration C " + kind);channel_work.mkdir()
            checked([channel_check, channel_work])
            channel_c = builder.executable("channel-declaration-model-" + kind, ["channel-declaration-model.c"], [archive], module=True)
            channel_phys = builder.executable("channel-declaration-phys-" + kind, ["channel-declaration-phys.c"], [archive], module=True, language=True)
            channel_export = builder.executable("channel-export-probe-" + kind, ["channel-export-probe.c"], [archive])
            checked([sys.executable, repo / "tests/test_channel_declaration_workflow.py",
                     "--runner", sdk / "bin" / ("physim-runner" + suffix),
                     "--c-model", channel_c, "--phys-model", channel_phys,
                     "--exporter", channel_export, "--work", root / ("Channel declaration parity " + kind)])
        print("Installed/rebuilt SDK channel declarations: canonical SI, bounded UTF-8, transactional rejection and actual C/Physim run/CSV parity passed", flush=True)
        shutil.copy2(repo / "tests/test_properties.c", consumer / "properties-check.c")
        shutil.copy2(repo / "tests/test_property_report.c", consumer / "property-report-probe.c")
        checked([sdk / "bin" / ("physimc" + suffix), "--emit-c",
                 repo / "tests/fixtures/language/property_values.phys"],
                output=consumer / "property-values.c")
        for name in ("property_main", "property_analysis"):
            shutil.copy2(sdk / "examples/documentation" / (name + ".c"), consumer / (name + ".c"))
        for kind, archive in (("installed", library), ("rebuilt", rebuilt_core)):
            property_check = builder.executable("properties-check-" + kind, ["properties-check.c"], [archive])
            property_values = builder.executable("property-values-" + kind, ["property-values.c"], [archive], language=True)
            checked([property_check]);checked([property_values])
            property_model = builder.executable("property-model-" + kind, ["property_main.c"], [archive], module=True)
            property_analysis = builder.executable("property-analysis-" + kind, ["property_analysis.c"], [archive], module=True)
            property_probe = builder.executable("property-report-" + kind, ["property-report-probe.c"], [archive])
            checked([sys.executable, repo / "tests/test_property_workflow.py",
                     "--runner", sdk / "bin" / ("physim-runner" + suffix),
                     "--analysis", sdk / "bin" / ("physim-analysis-runner" + suffix),
                     "--c-model", property_model, "--phys-model", sdk / "bin" / ("language-property_main" + module_suffix),
                     "--c-analysis", property_analysis, "--phys-analysis", sdk / "bin" / ("language-property_analysis" + module_suffix),
                     "--probe", property_probe, "--work", root / ("Property workflow " + kind)])
        print("Installed/rebuilt SDK properties: SI domains, owned C/Physim data, stored metadata and mixed analyses passed", flush=True)
        shutil.copy2(repo / "tests/test_real_gas.c", consumer / "real-gas-check.c")
        shutil.copy2(repo / "tests/test_real_gas_report.c", consumer / "real-gas-report-probe.c")
        checked([sdk / "bin" / ("physimc" + suffix), "--emit-c",
                 repo / "tests/fixtures/language/real_gas_values.phys"],
                output=consumer / "real-gas-values.c")
        for name in ("real_gas_main", "real_gas_analysis"):
            shutil.copy2(sdk / "examples/documentation" / (name + ".c"), consumer / (name + ".c"))
        for kind, archive in (("installed", library), ("rebuilt", rebuilt_core)):
            real_check = builder.executable("real-gas-check-" + kind, ["real-gas-check.c"], [archive])
            real_values = builder.executable("real-gas-values-" + kind, ["real-gas-values.c"], [archive], language=True)
            checked([real_check]);checked([real_values])
            real_model = builder.executable("real-gas-model-" + kind, ["real_gas_main.c"], [archive], module=True)
            real_analysis = builder.executable("real-gas-analysis-" + kind, ["real_gas_analysis.c"], [archive], module=True)
            real_probe = builder.executable("real-gas-report-" + kind, ["real-gas-report-probe.c"], [archive])
            checked([sys.executable, repo / "tests/test_real_gas_workflow.py",
                     "--runner", sdk / "bin" / ("physim-runner" + suffix),
                     "--analysis", sdk / "bin" / ("physim-analysis-runner" + suffix),
                     "--c-model", real_model, "--phys-model", sdk / "bin" / ("language-real_gas_main" + module_suffix),
                     "--c-analysis", real_analysis, "--phys-analysis", sdk / "bin" / ("language-real_gas_analysis" + module_suffix),
                     "--probe", real_probe, "--work", root / ("Real gas workflow " + kind)])
        print("Installed/rebuilt SDK real gas: ideal limit, unstable algebra, atomic errors, independent Decimal runs and mixed reports passed", flush=True)
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

        for source in ("string_main","string_analysis"):
            shutil.copy2(sdk / "examples/documentation" / (source+".c"),consumer / (source+".c"))
        shutil.copy2(repo / "tests/test_string_tutorial_report.c",consumer / "string-tutorial-probe.c")
        shutil.copy2(repo / "tests/test_waves_optics.c",consumer / "waves-optics-core-check.c")
        shutil.copy2(repo / "tests/test_wave_array_memory.c",consumer / "wave-array-memory.c")
        for kind,archive in (("installed",library),("rebuilt",rebuilt_core)):
            wave_core=builder.executable("waves-optics-core-"+kind,["waves-optics-core-check.c"],[archive])
            wave_memory=builder.executable("wave-memory-"+kind,["wave-array-memory.c"],[archive])
            checked([wave_core]);checked([wave_memory])
            string_experiment=builder.executable("string-experiment-"+kind,["string_main.c"],[archive],module=True)
            string_analyzer=builder.executable("string-analysis-"+kind,["string_analysis.c"],[archive],module=True)
            string_probe=builder.executable("string-probe-"+kind,["string-tutorial-probe.c"],[archive])
            directory=root / ("String tutorial "+kind);directory.mkdir()
            string_language=modules["string_main"] if kind=="rebuilt" else sdk / "bin" / ("language-string_main"+module_suffix)
            string_analysis=modules["string_analysis"] if kind=="rebuilt" else sdk / "bin" / ("language-string_analysis"+module_suffix)
            checked([sys.executable,repo / "tests/test_string_tutorial.py",sdk / "bin" / ("physim-runner"+suffix),
                     sdk / "bin" / ("physim-analysis-runner"+suffix),string_experiment,string_language,
                     string_analyzer,string_analysis,string_probe,directory])
        print("Installed SDK waves/optics: installed/rebuilt Core, 4096-node/ownership failures, six grid profiles, second-order refinement and 48 mixed analyses passed",flush=True)

        for source in ("transport_main","transport_analysis"):
            shutil.copy2(sdk / "examples/documentation" / (source+".c"),consumer / (source+".c"))
        shutil.copy2(repo / "tests/test_transport_tutorial_report.c",consumer / "transport-tutorial-probe.c")
        shutil.copy2(repo / "tests/test_fluid.c",consumer / "fluid-core-check.c")
        shutil.copy2(repo / "tests/test_fluid_array_memory.c",consumer / "fluid-array-memory.c")
        for kind,archive in (("installed",library),("rebuilt",rebuilt_core)):
            fluid_core=builder.executable("fluid-core-"+kind,["fluid-core-check.c"],[archive])
            fluid_memory=builder.executable("fluid-memory-"+kind,["fluid-array-memory.c"],[archive])
            checked([fluid_core]);checked([fluid_memory])
            transport_experiment=builder.executable("transport-experiment-"+kind,["transport_main.c"],[archive],module=True)
            transport_analyzer=builder.executable("transport-analysis-"+kind,["transport_analysis.c"],[archive],module=True)
            transport_probe=builder.executable("transport-probe-"+kind,["transport-tutorial-probe.c"],[archive])
            directory=root / ("Transport tutorial "+kind);directory.mkdir()
            transport_language=modules["transport_main"] if kind=="rebuilt" else sdk / "bin" / ("language-transport_main"+module_suffix)
            transport_analysis=modules["transport_analysis"] if kind=="rebuilt" else sdk / "bin" / ("language-transport_analysis"+module_suffix)
            checked([sys.executable,repo / "tests/test_transport_tutorial.py",sdk / "bin" / ("physim-runner"+suffix),
                     sdk / "bin" / ("physim-analysis-runner"+suffix),transport_experiment,transport_language,
                     transport_analyzer,transport_analysis,transport_probe,directory])
        print("Installed SDK fluid: installed/rebuilt Core, max networks/4096-cell ownership failures, six hydraulic/Fourier profiles and 48 mixed analyses passed",flush=True)

        for domain,oracle,domain_probe in (
                ("thermal",repo / "tests/test_thermal_tutorial.py",thermal_probe),
                ("rc",repo / "tests/test_rc_tutorial.py",rc_probe),
                ("string",repo / "tests/test_string_tutorial.py",string_probe),
                ("transport",repo / "tests/test_transport_tutorial.py",transport_probe),
                ("property",repo / "tests/test_property_workflow.py",property_probe),
                ("real_gas",repo / "tests/test_real_gas_workflow.py",real_probe)):
            checked([sys.executable,repo / "tests/test_native_domain_build.py","--domain",domain,
                     "--builder",sdk / "bin" / ("physim-build"+suffix),"--sdk",sdk,
                     "--compiler",sdk / "bin" / ("physimc"+suffix),
                     "--runner",sdk / "bin" / ("physim-runner"+suffix),
                     "--analysis-runner",sdk / "bin" / ("physim-analysis-runner"+suffix),
                     "--oracle",oracle,"--probe",domain_probe,"--work",root / ("Documented native "+domain)])
        print("Installed SDK native domain projects: twelve cold C/Physim builds, unchanged cache reuse and six complete independent tutorial oracles passed",flush=True)

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
                for example in [name for name in native.EXAMPLES if name != "ccd_events"] + ["language_full"]:
                    checked([sdk / "bin" / ("physim" + suffix), "--self-test", root / ("App " + example), example])
                    print(f"Installed SDK GUI workflow: {example} passed", flush=True)
                checked([sdk / "bin" / ("physim"+suffix),"--docs-test",root / "Documentation routes"])
                print("Installed SDK documentation window: both learning route buttons, shared links, route home and small-window rendering passed",flush=True)
                previous_small=env.pop("PHYSIM_TEST_SMALL",None)
                try:
                    for size in ("large","small"):
                        if size=="small":env["PHYSIM_TEST_SMALL"]="1"
                        output=root / ("keyboard-menu-"+size+".txt")
                        checked([sdk / "bin" / ("physim"+suffix),"--workspace-state-test",
                                 root / ("Keyboard menu "+size),"toolbar-keyboard"],output=output)
                        if "KEYBOARD MENU SELF-TEST: PASSED" not in output.read_text(encoding="utf-8"):
                            raise RuntimeError("Installed SDK keyboard menu did not complete")
                finally:
                    env.pop("PHYSIM_TEST_SMALL",None)
                    if previous_small is not None:env["PHYSIM_TEST_SMALL"]=previous_small
                print("Installed SDK keyboard menus: both window sizes, disabled actions, text isolation, save, pointer return and focus loss passed",flush=True)
                for ui_size in (16,18,20,22):
                    directory=root / ("UI typography "+str(ui_size));directory.mkdir()
                    for mode in ("ui-size-"+str(ui_size),"ui-size-"+str(ui_size)+"-read"):
                        output=root / (mode+".txt")
                        checked([sdk / "bin" / ("physim"+suffix),"--settings-test",directory,mode],output=output)
                        if "UI SIZE SELF-TEST: PASSED" not in output.read_text(encoding="utf-8"):
                            raise RuntimeError("Installed SDK UI typography did not complete")
                    data=(directory / "preferences.bin").read_bytes()
                    if len(data)!=312 or data[:8]!=b"PSPREF05" or int.from_bytes(data[304:308],"little")!=ui_size:
                        raise RuntimeError("Installed SDK UI size was not persisted")
                print("Installed SDK UI typography: four sizes, restart, independent code fonts, open documentation, plots and menus passed",flush=True)
                directory=root / "Keyboard settings";directory.mkdir()
                for mode in ("keyboard","keyboard-read"):
                    output=root / ("settings-"+mode+".txt")
                    checked([sdk / "bin" / ("physim"+suffix),"--settings-test",directory,mode],output=output)
                    if "SETTINGS KEYBOARD SELF-TEST: PASSED" not in output.read_text(encoding="utf-8"):
                        raise RuntimeError("Installed SDK keyboard settings did not complete")
                print("Installed SDK settings: keyboard-only traversal, all toggles, restart, cancellation and large-font focus scrolling passed",flush=True)
                if sys.platform in ("darwin","linux"):
                    verify_accessibility_checkbox(repo,sdk,root,suffix,checked)
                for mode in ("docs-keyboard","docs-keyboard-22","docs-keyboard-delayed"):
                    directory=root / mode;directory.mkdir();output=root / (mode+".txt")
                    checked([sdk / "bin" / ("physim"+suffix),"--workspace-state-test",directory,mode],output=output)
                    if "DOCUMENTATION KEYBOARD SELF-TEST: PASSED" not in output.read_text(encoding="utf-8"):
                        raise RuntimeError("Installed SDK keyboard documentation did not complete")
                print("Installed SDK documentation: keyboard learning tracks, topics, contents, links, code copy, search and input order at 16/22 px passed",flush=True)
                for mode in ("manager-keyboard","manager-keyboard-22","manager-errors"):
                    directory=root / mode;directory.mkdir();output=root / (mode+".txt")
                    checked([sdk / "bin" / ("physim"+suffix),"--workspace-state-test",directory,mode],output=output,timeout=910)
                    marker="PROJECT MANAGER ERRORS SELF-TEST: PASSED" if mode=="manager-errors" else "PROJECT MANAGER KEYBOARD SELF-TEST: PASSED"
                    if marker not in output.read_text(encoding="utf-8"):
                        raise RuntimeError("Installed SDK keyboard project creation did not complete")
                print("Installed SDK projects: all 32 template/language pairs and two independent analyses built, recorded/analyzed with keyboard at 16/22 px; error guards passed",flush=True)
        elif args.app_tests:
            raise RuntimeError("App tests require an SDK with the app")
    (root / "PASSED.txt").write_text(
        f"Native SDK relocation, independent headers, installed and rebuilt core archives, {len(native.EXAMPLES)} bundled and rebuilt C examples, "
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
        "Waves/optics through installed/rebuilt Core, Snell/TIR/lens invariants, owned 4096-node grids, allocation failures and mixed string reports passed.\n" +
        "Fluid through installed/rebuilt Core, anchored max networks, conservative 4096-cell transport, allocation failures and mixed tutorial reports passed.\n" +
        "Scalar search through installed/rebuilt Core, exact rational tolerance decisions, C/Physim reports and owned module-block closures passed.\n" +
        "ODE range through installed/rebuilt Core, five C/Physim methods, constant-solution Fraction references and true overflow rollback passed.\n" +
        "Linear systems through installed/rebuilt Core, independent rational C/Physim solutions/residuals, 1..32 dimensions and true overflow rollback passed.\n" +
        "Series SI imports through installed/rebuilt Core, multiblock algebra/calculus/resampling, independent C/Physim report/CSV checks and transactional errors passed.\n" +
        "Quantity sums through installed/rebuilt Core, exact Fraction oracles, overflowing conversion and subnormal ties passed.\n" +
        "Canonical SI channel declarations through installed/rebuilt Core, bounded UTF-8, atomic errors and actual C/Physim run/CSV parity passed.\n" +
        "Material properties and real gas through installed/rebuilt Core, stored SI metadata, independent Decimal references and mixed analyses passed.\n" +
        "Twelve cold native C/Physim domain project builds, unchanged cache reuse and six complete independent tutorial oracles passed.\n" +
        "Both complete C/Physim learning routes, eight paired model source groups and executable language introductions passed.\n" +
        ("Nine projects rebuilt without CMake; sources unchanged and outputs confined to build/.\n" if metadata["app"] else "") +
        ("Eight C template GUI workflows, the complete Physim language GUI workflow, independent documentation route navigation, keyboard menus in both window sizes and four UI typography sizes with restart, keyboard settings, keyboard documentation and keyboard project matrix passed.\n" if args.app_tests else ""), encoding="utf-8")
    print(f"Native SDK verified: {root}")


if __name__ == "__main__":
    main()
