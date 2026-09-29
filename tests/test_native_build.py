"""Exercise the real project builder, compilers and runners without project CMake files."""
import argparse
import hashlib
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import uuid

parser = argparse.ArgumentParser()
parser.add_argument("--builder", type=Path, required=True)
parser.add_argument("--sdk", type=Path, required=True)
parser.add_argument("--compiler", type=Path, required=True)
parser.add_argument("--runner", type=Path, required=True)
parser.add_argument("--analysis-runner", type=Path, required=True)
parser.add_argument("--work", type=Path, required=True)
parser.add_argument("--cc")
args = parser.parse_args()
args.work.mkdir(parents=True, exist_ok=True)
root = Path(tempfile.mkdtemp(prefix="native project ä ", dir=args.work)).resolve()
extension = ".dll" if os.name == "nt" else ".so"


def command(argv, success=True):
    # Python/launchers may normalize Windows environment names. Discovery must
    # still find ProgramFiles(x86), independently of its spelling in the parent.
    environment = {key.upper(): value for key, value in os.environ.items()} if os.name == "nt" else None
    result = subprocess.run(list(map(str, argv)), capture_output=True, timeout=150, env=environment)
    output = (result.stdout + result.stderr).decode("utf-8", errors="replace")
    if (result.returncode == 0) != success:
        raise AssertionError(f"Command returned {result.returncode}: {argv}\n{output}")
    return output


def build(project, profile="Debug", success=True):
    argv = [args.builder, "--project", project, "--sdk", args.sdk, "--output",
            project / "build" / profile, "--physimc", args.compiler, "--profile", profile]
    if args.cc:
        argv += ["--cc", args.cc]
    output = command(argv, success)
    print(f"{project.name} / {profile}: {'passed' if success else 'expected failure'}", flush=True)
    return output


def fingerprint(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def run_modules(project, profile):
    output = project / "build" / profile
    run = output / ("test-" + uuid.uuid4().hex)
    command([args.runner, output / ("experiment" + extension), "--describe"])
    command([args.runner, output / ("experiment" + extension), run.with_suffix(".psrun"), "--steps", "100"])
    command([args.analysis_runner, output / ("analysis" + extension), run.with_suffix(".psrun"), run])
    assert run.with_suffix(".psrun").stat().st_size > 0


project = root / "C sources"
project.mkdir()
for name in ("main.c", "analysis.c"):
    shutil.copyfile(args.sdk / "examples" / "pendulum" / name, project / name)
# The sample reports its compilation time. Use stable literals in this fixture
# so that the no-change test measures source/header reuse, not a changing clock.
analysis_fixture = project / "analysis.c"
analysis_fixture.write_text(analysis_fixture.read_text(encoding="utf-8")
                           .replace("__DATE__", '"Sep 28 2026"')
                           .replace("__TIME__", '"12:00:00"'), encoding="utf-8")
manifest = project / "physim.project"
manifest_text = "physim_project=1\nexperiment=main.c\nanalysis=analysis.c\n"
manifest.write_text(manifest_text, encoding="utf-8")
# Follow a transitive header; cached objects must not hide subsequent edits.
(project / "nested.h").write_text("#define NATIVE_VALUE 1\n", encoding="utf-8")
(project / "helper.h").write_text('#include "nested.h"\n', encoding="utf-8")
with (project / "main.c").open("a", encoding="utf-8") as file:
    file.write('\n#include "helper.h"\nint native_test_value(void) { return NATIVE_VALUE; }\n')
before = {p.name: fingerprint(p) for p in project.iterdir()}
assert "Build successful" in build(project)
run_modules(project, "Debug")
out = project / "build" / "Debug"
modules = [out / (name + extension) for name in ("experiment", "analysis")]
mtimes = [p.stat().st_mtime_ns for p in modules]
assert "Build up to date" in build(project)
assert mtimes == [p.stat().st_mtime_ns for p in modules]
assert set(p.name for p in project.iterdir()) == set(before) | {"build"}
assert all(fingerprint(project / name) == digest for name, digest in before.items())
assert not list(project.rglob("CMakeLists.txt"))
assert not list(project.rglob("CMakeCache.txt"))
initial_object = fingerprint(out / "experiment.obj")
(project / "nested.h").write_text("#define NATIVE_VALUE 2\n", encoding="utf-8")
assert "Build successful" in build(project)
assert fingerprint(out / "experiment.obj") != initial_object

# A failed link may leave freshly compiled objects. The next build still must link them.
good_modules = [fingerprint(p) for p in modules]
analysis = project / "analysis.c"
analysis_text = analysis.read_text(encoding="utf-8")
(project / "nested.h").write_text("#define NATIVE_VALUE 3\n", encoding="utf-8")
analysis.write_text(analysis_text + "\nextern void missing_native_symbol(void);\n"
                    "void native_broken(void) { missing_native_symbol(); }\n", encoding="utf-8")
build(project, success=False)
assert good_modules == [fingerprint(p) for p in modules]
analysis.write_text(analysis_text, encoding="utf-8")
assert "Link:" in build(project)
run_modules(project, "Debug")
assert not (out / "build.pending").exists()

# Compiler errors preserve the previously published modules, too.
good_modules = [fingerprint(p) for p in modules]
(project / "nested.h").write_text("#error deliberate native build failure\n", encoding="utf-8")
assert "deliberate native build failure" in build(project, success=False)
assert good_modules == [fingerprint(p) for p in modules]
(project / "nested.h").write_text("#define NATIVE_VALUE 3\n", encoding="utf-8")
build(project)

# Legacy CMake configuration is neither read nor changed by the native builder.
legacy = project / "CMakeLists.txt"
legacy.write_text("THIS IS NOT VALID CMAKE\n", encoding="utf-8")
assert "Build up to date" in build(project)
assert legacy.read_text(encoding="utf-8") == "THIS IS NOT VALID CMAKE\n"
manifest.write_bytes(manifest_text.replace("\n", "\r\n").encode("utf-8"))
assert "Build up to date" in build(project)
manifest.write_text(manifest_text + "experiment=main.phys\n", encoding="utf-8")
assert "Invalid physim.project" in build(project, success=False)
manifest.write_text(manifest_text, encoding="utf-8")
command([args.builder, "--project", project, "--sdk", args.sdk, "--output", out,
         "--physimc", args.compiler, "--cc", "physim-no-such-compiler"], success=False)

# A held cache lock rejects a second writer; releasing it allows the next build.
with (out / "build.lock").open("r+b") as lock:
    if os.name != "nt":
        import fcntl
        fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
    assert "already in use" in build(project, success=False)
build(project)
assert "Build successful" in build(project, "Release")
run_modules(project, "Release")
assert "Build up to date" in build(project, "Release")

# Relocated projects reuse no stale absolute include paths.
moved = root / "moved C project ä"
project.rename(moved)
assert "Build successful" in build(moved)
run_modules(moved, "Debug")

language = root / "Physim sources"
language.mkdir()
shutil.copyfile(args.sdk / "examples/language/pendulum.phys", language / "main.phys")
shutil.copyfile(args.sdk / "examples/language/analysis.phys", language / "analysis.phys")
(language / "physim.project").write_text(
    "physim_project=1\nexperiment=main.phys\nanalysis=analysis.phys\n", encoding="utf-8")
for profile in ("Debug", "Release"):
    assert "Build successful" in build(language, profile)
    run_modules(language, profile)
    generated = [language / "build" / profile / (name + ".physim.c")
                 for name in ("experiment", "analysis")]
    generated_stamps = [(fingerprint(p), p.stat().st_mtime_ns) for p in generated]
    assert "Build up to date" in build(language, profile)
    assert generated_stamps == [(fingerprint(p), p.stat().st_mtime_ns) for p in generated]

# A language error preserves generated C and the runnable modules, including
# when the experiment translated successfully before the analysis failed.
out = language / "build/Debug"
published = [out / (name + suffix) for name in ("experiment", "analysis")
             for suffix in (".physim.c", extension)]
for source_name in ("main.phys", "analysis.phys"):
    source = language / source_name
    good_source = source.read_bytes()
    previous = [(fingerprint(p), p.stat().st_mtime_ns) for p in published]
    source.write_text('let broken: Int64 = "text"\n', encoding="utf-8")
    failure = build(language, success=False)
    assert source_name in failure and "error:" in failure, failure
    assert previous == [(fingerprint(p), p.stat().st_mtime_ns) for p in published]
    assert not any(out.glob("*.physim.c.next"))
    run_modules(language, "Debug")
    source.write_bytes(good_source)
    assert "Build successful" in build(language)
    assert not (out / "build.pending").exists()
    run_modules(language, "Debug")
assert set(p.name for p in language.iterdir()) == {"main.phys", "analysis.phys", "physim.project", "build"}
# Resolve all relative paths before subprocesses change to the build directory.
relative = lambda path: os.path.relpath(path)
argv = [args.builder, "--project", relative(language), "--sdk", relative(args.sdk),
        "--output", relative(language / "build/Debug"), "--physimc", relative(args.compiler)]
if args.cc:
    argv += ["--cc", args.cc]
assert "Build up to date" in command(argv)
# Without an override, the command-line builder follows the app-maintained profile.
description = language / "physim.project"
description.write_text(description.read_text(encoding="utf-8") + "profile=Release\n", encoding="utf-8")
argv = [args.builder, "--project", language, "--sdk", args.sdk,
        "--output", language / "build/Release", "--physimc", args.compiler]
if args.cc:
    argv += ["--cc", args.cc]
output = command(argv)
assert "Profile: Release" in output and "Build up to date" in output
output = command(argv + ["--profile", "Debug", "--output", language / "build/Debug"])
assert "Profile: Debug" in output and "Build up to date" in output
print(f"Native builds, incremental headers, failure recovery, locks, relocation and runners passed: {root}")
