"""Build Physim directly with a C17 compiler; no generated build system required.

Uses only Python's standard library. SDL must already be installed when building
the app. All generated files, signatures and locks live in the build directory.
"""
from __future__ import annotations

import argparse
from concurrent.futures import ThreadPoolExecutor
from contextlib import contextmanager
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parent.parent
WINDOWS = sys.platform == "win32"
MAC = sys.platform == "darwin"
CORE = "core memory array string_view hashmap math data snapshot analysis scene numerics units series report report_export mechanics box_contacts collision measurement".split()
LANGUAGE = "lexer parser checker emitter builtins".split()
APP = "main timeline docking layout_catalog channel_units run_import ui_backend ui_sdl ui_geometry graphics documentation library preferences workspace_state workspace_catalog workspace_tree plot_view report_image png".split()
PROJECT = "project_file text_document autosave parameter_catalog".split()
ZLIB = "adler32 crc32 deflate trees zutil".split()
EXAMPLES = "pendulum projectile collision box_floor spring uncertain_projectile box_collision buoyancy".split()
LANGUAGE_PROGRAMS = "energy motion flight_phases sampling phase_space rotation_path particles rigid_body contacts distance_joints constraint_graph sweeps coordinate_frames optional_values optional_bindings".split()
LANGUAGE_EXPERIMENTS = ("pendulum pendulum_rk4 pendulum_integrator pendulum_rk45 pendulum_verlet "
    "projectile projectile_drag collision box_collision buoyancy random_samples scene_shapes "
    "spring sensors uncertain_projectile spinning_body box_contacts joint_pendulum coupled_bodies fast_sphere logging").split()
LANGUAGE_ANALYSES = "analysis analysis_collision analysis_box_collision analysis_buoyancy analysis_sensors analysis_integral".split()


def language_examples():
    """Names, emission modes and paths relative to the installed examples directory."""
    return ([(name, "--emit-c", f"language/{name}.phys") for name in LANGUAGE_PROGRAMS] +
            [(name, "--emit-experiment", f"language/{name}.phys") for name in LANGUAGE_EXPERIMENTS] +
            [(name, "--emit-analysis", f"language/{name}.phys") for name in LANGUAGE_ANALYSES] +
            [("drag_analysis", "--emit-analysis", "documentation/drag_analysis.phys")])


def run(args: list[str], env: dict[str, str], *, capture: bool = False, directory: Path | None = None) -> str:
    result = subprocess.run(args, env=env, cwd=directory or ROOT, stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT, encoding="utf-8", errors="replace")
    if result.returncode or not capture:
        if result.stdout:
            print(result.stdout, end="", flush=True)
    if result.returncode:
        raise RuntimeError(f"Command failed ({result.returncode}): {subprocess.list2cmdline(args)}")
    return result.stdout


def compiler_environment() -> dict[str, str]:
    env = dict(os.environ)
    if not WINDOWS or (env.get("VCINSTALLDIR") and env.get("VSCMD_ARG_TGT_ARCH") == "x64"
                       and shutil.which("cl", path=env.get("PATH"))):
        return env
    vswhere = Path(env.get("ProgramFiles(x86)", "C:/Program Files (x86)")) / "Microsoft Visual Studio/Installer/vswhere.exe"
    if not vswhere.is_file():
        raise RuntimeError("Install Visual Studio 2022 C++ Build Tools and a Windows SDK.")
    installation = run([str(vswhere), "-latest", "-products", "*", "-requires",
                        "Microsoft.VisualStudio.Component.VC.Tools.x86.x64", "-property",
                        "installationPath"], env, capture=True).strip()
    setup = Path(installation) / "VC/Auxiliary/Build/vcvars64.bat"
    if not installation or not setup.is_file():
        raise RuntimeError("Visual Studio C++ x64 tools were not found.")
    # Only the installed toolchain path enters cmd.exe. Source/build paths never do.
    command = f'""{setup}" >nul && set"'
    result = subprocess.run(f'{env.get("COMSPEC", "cmd.exe")} /d /s /c {command}',
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
    for line in result.stdout.decode("mbcs").splitlines():
        if "=" in line and not line.startswith("="):
            key, value = line.split("=", 1)
            # Windows environment keys are case insensitive; avoid duplicate PATH entries.
            env[key.upper()] = value
    return {key.upper(): value for key, value in env.items()}


def digest_files(paths: list[Path]) -> str:
    digest = hashlib.sha256()
    for path in paths:
        digest.update(str(path).encode("utf-8"))
        digest.update(b"\0")
        digest.update(path.read_bytes())
        digest.update(b"\0")
    return digest.hexdigest()


@contextmanager
def build_lock(directory: Path):
    directory.mkdir(parents=True, exist_ok=True)
    with (directory / ".physim-build.lock").open("a+b") as lock:
        if os.fstat(lock.fileno()).st_size == 0:
            lock.write(b"0")
            lock.flush()
        lock.seek(0)
        try:
            if WINDOWS:
                import msvcrt
                msvcrt.locking(lock.fileno(), msvcrt.LK_NBLCK, 1)
            else:
                import fcntl
                fcntl.flock(lock.fileno(), fcntl.LOCK_EX | fcntl.LOCK_NB)
        except OSError as error:
            raise RuntimeError(f"Another build is using {directory}") from error
        try:
            yield
        finally:
            if WINDOWS:
                lock.seek(0)
                msvcrt.locking(lock.fileno(), msvcrt.LK_UNLCK, 1)
            else:
                fcntl.flock(lock.fileno(), fcntl.LOCK_UN)


class Builder:
    def __init__(self, args: argparse.Namespace, env: dict[str, str]):
        self.args, self.env = args, dict(env)
        self.directory = args.build_dir.resolve()
        self.bin = self.directory / "bin"
        self.lib = self.directory / "lib"
        self.bin.mkdir(exist_ok=True)
        self.lib.mkdir(exist_ok=True)
        self.cc = shutil.which(args.compiler or ("cl" if WINDOWS else "cc"), path=env.get("PATH"))
        if not self.cc:
            raise RuntimeError("C17 compiler not found; pass --compiler with its executable path.")
        self.msvc = Path(self.cc).name.lower() in ("cl.exe", "cl", "clang-cl.exe", "clang-cl")
        if WINDOWS and not self.msvc:
            raise RuntimeError("Windows builds require MSVC or clang-cl.")
        self.fuzzing = getattr(args, "fuzzer", False)
        self.linker = None
        if self.fuzzing:
            if self.msvc and Path(self.cc).stem.lower() != "clang-cl":
                raise RuntimeError("libFuzzer requires Clang; pass --compiler clang-cl on Windows or --compiler clang elsewhere.")
            version = run([self.cc, "--version"], env, capture=True)
            if "clang" not in version.lower():
                raise RuntimeError("libFuzzer requires a Clang compiler.")
            if MAC:
                resource = Path(run([self.cc, "-print-resource-dir"], env, capture=True).strip())
                if not (resource / "lib/darwin/libclang_rt.fuzzer_osx.a").is_file():
                    raise RuntimeError("This Clang installation has no libFuzzer runtime; install LLVM (for example brew install llvm@20) and pass its bin/clang with --compiler.")
        self.sanitizers = getattr(args, "sanitizers", False)
        if MAC and self.sanitizers:
            # Apple ld rejects some LLVM sanitizer relocations on arm64.
            self.linker = shutil.which("ld64.lld", path=str(Path(self.cc).parent) + os.pathsep + env.get("PATH", ""))
            if not self.linker:
                raise RuntimeError("macOS sanitizer builds require ld64.lld; install llvm@20 and lld@20 and add their bin directories to PATH.")
        if self.sanitizers and WINDOWS:
            if Path(self.cc).stem.lower() == "clang-cl":
                resource = Path(run([self.cc, "/clang:-print-resource-dir"], env, capture=True).strip())
                runtime_dir = resource / "lib/windows"
            else:
                runtime_dir = Path(self.cc).parent
            runtimes = sorted(runtime_dir.glob("clang_rt.asan*.dll"))
            if not runtimes:
                raise RuntimeError(f"AddressSanitizer runtime not found in {runtime_dir}; install the compiler's ASan component.")
            for runtime in runtimes:
                destination = self.bin / runtime.name
                if not destination.is_file() or destination.read_bytes() != runtime.read_bytes():
                    shutil.copy2(runtime, destination)
            self.env["PATH"] = str(self.bin) + os.pathsep + self.env.get("PATH", "")
        self.ar = shutil.which("lib" if self.msvc else "ar", path=env.get("PATH"))
        if not self.ar:
            raise RuntimeError("Static library tool not found (lib.exe / ar).")
        self.includes = [ROOT / "include", ROOT / "src", ROOT / "app", ROOT / "third_party/zlib-1.3.2"]
        self.sdl_library: Path | None = None
        self.runtime: Path | None = None
        if not args.no_app:
            prefix = args.sdl.resolve() if args.sdl else ROOT / ("third_party/SDL3-3.2.30" if WINDOWS else "build-sdl-install")
            self.includes.extend([ROOT / "third_party", prefix / "include"])
            if not (prefix / "include/SDL3/SDL.h").is_file():
                raise RuntimeError("SDL3 headers not found. Pass --sdl with an SDL3 installation prefix.")
            if WINDOWS:
                self.sdl_library = prefix / "lib/x64/SDL3.lib"
                runtime = prefix / "lib/x64/SDL3.dll"
            else:
                name = "libSDL3.0.dylib" if MAC else "libSDL3.so.0"
                runtime = next((prefix / d / name for d in ("lib", "lib64") if (prefix / d / name).is_file()), prefix / "lib" / name)
                self.sdl_library = runtime
            if not self.sdl_library.is_file() or not runtime.is_file():
                raise RuntimeError(f"Shared SDL3 library not found in {prefix}")
            destination = self.bin / runtime.name
            self.runtime = destination
            if not destination.is_file() or destination.read_bytes() != runtime.read_bytes():
                shutil.copy2(runtime, destination)
        headers = sorted({p for base in self.includes for p in base.rglob("*")
                          if p.suffix in (".h", ".inc") and p.is_file()})
        # Conservative dependency tracking: changing any included project/SDL header
        # rebuilds all objects. Source edits still rebuild just the affected objects.
        self.headers = digest_files(headers)
        self.test_headers = digest_files(sorted(p for p in (ROOT / "tests").rglob("*")
                                                if p.suffix in (".h", ".inc") and p.is_file()))
        self.tool_headers = digest_files(sorted(p for p in (ROOT / "tools").rglob("*")
                                                if p.suffix in (".h", ".inc") and p.is_file()))
        self.toolchain = digest_files([Path(self.cc), Path(self.ar)] + ([Path(self.linker)] if self.linker else []))
        self.environment = {k: env.get(k, "") for k in
                            ("INCLUDE", "LIB", "CL", "_CL_", "CPATH", "C_INCLUDE_PATH", "SDKROOT", "MACOSX_DEPLOYMENT_TARGET")}

    def signature(self, args: list[str], inputs: list[Path]) -> str:
        data = [args, digest_files(inputs), self.toolchain, self.environment]
        return hashlib.sha256(json.dumps(data, sort_keys=True).encode("utf-8")).hexdigest()

    def execute(self, output: Path, command: list[str], inputs: list[Path], extra: str = "") -> bool:
        stamp = output.with_name(output.name + ".json")
        signature = self.signature(command, inputs) + extra
        try:
            state = json.loads(stamp.read_text(encoding="utf-8"))
            if not getattr(self.args, "rebuild", False) and state == {"input": signature, "output": digest_files([output])}:
                return False
        except (OSError, ValueError):
            pass
        run(command, self.env)
        temporary = output.with_name(output.stem + ".pending" + output.suffix)
        os.replace(temporary, output)
        temporary_stamp = stamp.with_suffix(".pending")
        temporary_stamp.write_text(json.dumps({"input": signature, "output": digest_files([output])}), encoding="utf-8")
        os.replace(temporary_stamp, stamp)
        return True

    def compile(self, source: str, defines: tuple[str, ...] = (), *, language: bool = False) -> Path:
        path = (ROOT / source).resolve()
        if path.is_relative_to(self.directory):
            relative = Path("generated") / path.relative_to(self.directory)
        elif path.is_relative_to(ROOT):
            relative = path.relative_to(ROOT)
        else:
            raise RuntimeError(f"Source is outside the repository/build directory: {path}")
        variant = hashlib.sha256(repr((defines, language)).encode()).hexdigest()[:10]
        output = self.directory / "obj" / variant / relative.with_suffix(".obj" if self.msvc else ".o")
        output.parent.mkdir(parents=True, exist_ok=True)
        temporary = output.with_name(output.stem + ".pending" + output.suffix)
        debug = self.args.config == "Debug"
        if self.msvc:
            # ClangCL ASan does not support the debug CRT. Keep debug symbols
            # and optimization settings, but use the shared release CRT for ASan.
            command = [self.cc, "/nologo", "/c", "/std:c17", "/utf-8", "/W3" if language else "/W4", "/D_CRT_SECURE_NO_WARNINGS",
                       "/MDd" if debug and not self.sanitizers else "/MD", "/Od" if debug else "/O2", "/Z7",
                       "/fp:strict" if language else "/fp:precise"]
            command += ["/I" + str(p) for p in self.includes] + ["/D" + d for d in defines]
            if not debug:
                command.append("/DNDEBUG")
            if self.sanitizers:
                command.append("/fsanitize=address")
            command += [str(path), "/Fo" + str(temporary)]
        else:
            command = [self.cc, "-c", "-std=c17", "-fPIC", "-g", "-O0" if debug else "-O2",
                       "-D_POSIX_C_SOURCE=200809L",
                       "-fno-fast-math", "-ffp-contract=off"]
            if not language:
                command += ["-Wall", "-Wextra", "-Wpedantic", "-Wshadow"]
            command += ["-I" + str(p) for p in self.includes] + ["-D" + d for d in defines]
            if not debug:
                command.append("-DNDEBUG")
            if self.sanitizers:
                command += ["-fsanitize=address,undefined", "-fno-sanitize-recover=all", "-fno-omit-frame-pointer"]
            command += [str(path), "-o", str(temporary)]
        if self.fuzzing:
            command.append("-fsanitize=fuzzer-no-link")
        headers = self.headers + (self.test_headers if path.is_relative_to(ROOT / "tests") else "")
        if path.is_relative_to(ROOT / "tools"):
            headers += self.tool_headers
        changed = self.execute(output, command, [path], headers)
        if changed:
            print(f"Compiled {source}", flush=True)
        return output

    def objects(self, sources: list[str], defines: tuple[str, ...] = (), *, language: bool = False) -> list[Path]:
        with ThreadPoolExecutor(max_workers=self.args.jobs) as pool:
            return list(pool.map(lambda source: self.compile(source, defines, language=language), sources))

    def archive(self, name: str, sources: list[str], defines: tuple[str, ...] = ()) -> Path:
        objects = self.objects(sources, defines)
        output = self.lib / (name + ".lib" if self.msvc else "lib" + name + ".a")
        temporary = output.with_name(output.stem + ".pending" + output.suffix)
        # ar replaces members but retains removed sources: always create a fresh archive.
        temporary.unlink(missing_ok=True)
        command = ([self.ar, "/nologo", "/OUT:" + str(temporary)] if self.msvc else
                   [self.ar, "rcs", str(temporary)]) + [str(p) for p in objects]
        self.execute(output, command, objects)
        return output

    def executable(self, name: str, sources: list[str], libraries: list[Path], *, sdl: bool = False,
                   defines: tuple[str, ...] = (), module: bool = False, language: bool = False) -> Path:
        objects = self.objects(sources, defines, language=language)
        suffix = (".dll" if WINDOWS else ".so") if module else (".exe" if WINDOWS else "")
        output = self.bin / (name + suffix)
        temporary = output.with_name(output.stem + ".pending" + output.suffix)
        inputs = objects + libraries + ([self.sdl_library] if sdl else [])
        command = [self.cc, *map(str, inputs)]
        if self.linker:
            command.append("--ld-path=" + self.linker)
        if self.sanitizers:
            command += (["/MD", "/fsanitize=address"] if self.msvc else
                        ["-fsanitize=address,undefined", "-fno-sanitize-recover=all"])
        if self.fuzzing and name == "physim-protocol-libfuzzer":
            command.append("-fsanitize=fuzzer")
        if self.msvc:
            manifest = ROOT / "app/utf8.manifest"
            command += ["/nologo", "/Fe:" + str(temporary), "/link", "/INCREMENTAL:NO", "/DEBUG",
                        "/PDB:" + str(temporary.with_suffix(".pdb")), "/MANIFEST:EMBED", "/MANIFESTINPUT:" + str(manifest),
                        "/PDBALTPATH:" + output.with_suffix(".pdb").name,
                        "user32.lib", "shell32.lib", "advapi32.lib"]
            inputs += [manifest]
            if name in ("physim", "physim-ui-benchmark"):
                command.append("dwmapi.lib")
            if self.fuzzing and name == "physim-protocol-libfuzzer":
                # LLVM's Windows fuzzer archive uses the static CRT by default;
                # ASan and all our objects use the shared CRT.
                command += ["/NODEFAULTLIB:libcmt", "/NODEFAULTLIB:libucrt", "/NODEFAULTLIB:libvcruntime",
                            "msvcrt.lib", "ucrt.lib", "vcruntime.lib"]
            if module:
                command.insert(1, "/LD")
                command.append("/IMPLIB:" + str(self.lib / (name + ".lib")))
        else:
            command += ["-o", str(temporary), "-lm"]
            if not MAC:
                command += ["-ldl", "-pthread"]
            if sdl:
                command += ["-Wl,-rpath,@executable_path" if MAC else "-Wl,-rpath,$ORIGIN"]
            if module:
                command += ["-bundle", "-Wl,-undefined,error"] if MAC else ["-shared"]
        if self.execute(output, command, inputs):
            if self.msvc:
                os.replace(temporary.with_suffix(".pdb"), output.with_suffix(".pdb"))
            print(f"Linked {output.name}", flush=True)
        return output

    def benchmark(self, name, libraries):
        if name == "physim-benchmark":
            return self.executable(name, ["tools/benchmark.c"], [libraries["platform"], libraries["core"]])
        if name == "physim-ui-benchmark":
            return self.executable(name, ["tools/ui_benchmark.c", "app/graphics.c", "app/ui_geometry.c",
                                         "app/ui_backend.c", "app/ui_sdl.c", "app/png.c"],
                                   [libraries["platform"], libraries["zlib"], libraries["core"]],
                                   sdl=True, defines=("Z_PREFIX",))
        raise RuntimeError(f"Unknown benchmark target: {name}")

    def build_language_examples(self, compiler: Path, core: Path) -> list[Path]:
        generated = self.directory / "examples"
        generated.mkdir(exist_ok=True)
        products = []
        for name, mode, relative in language_examples():
            # Check the compiler exit before replacing the last complete C source.
            command = [str(compiler), mode, str(ROOT / "examples" / relative)]
            result = subprocess.run(command, cwd=ROOT, env=self.env, capture_output=True, timeout=30)
            if result.stderr:
                print(result.stderr.decode("utf-8", errors="replace"), end="", file=sys.stderr, flush=True)
            if result.returncode:
                raise RuntimeError(f"Example translation failed ({result.returncode}): {relative}")
            path = generated / (name + ".c")
            data = result.stdout
            if not path.is_file() or path.read_bytes() != data:
                pending = path.with_suffix(".pending.c")
                pending.write_bytes(data)
                os.replace(pending, path)
            products.append(self.executable("language-" + name, [str(path)], [core],
                                            module=mode != "--emit-c", language=True))
        return products

    def build(self):
        products = []
        core = self.archive("physim-core", [f"src/{name}.c" for name in CORE])
        products.append(core)
        if self.fuzzing:
            self.executable("physim-protocol-libfuzzer", ["tests/fuzz_protocol.c", "src/protocol.c"], [core])
            self.executable("physim-protocol-seeds", ["tests/fuzz_protocol.c", "src/protocol.c"], [core],
                            defines=("PS_FUZZ_STANDALONE",))
            print(f"Fuzzer build complete: {self.bin}")
            return
        platform = self.archive("physim-platform", ["src/platform.c", "src/protocol.c"])
        language = self.archive("physim-language", [f"src/language/{name}.c" for name in LANGUAGE])
        batch = self.archive("physim-batch", ["src/batch.c"])
        zlib = self.archive("physim-zlib", [f"third_party/zlib-1.3.2/{name}.c" for name in ZLIB], ("Z_PREFIX",))
        libraries = {"core": core, "platform": platform, "language": language, "batch": batch, "zlib": zlib}
        compiler = self.executable("physimc", ["src/language/main.c", "src/language/loader.c"], [language])
        products.append(compiler)
        if getattr(self.args, "examples", False):
            products.extend(self.build_language_examples(compiler, core))
        products.append(self.executable("physim-runner", ["runners/experiment.c"], [platform, core]))
        products.append(self.executable("physim-analysis-runner", ["runners/analysis.c"], [platform, core]))
        products.append(self.executable("physim-batch", ["runners/batch.c"], [batch, platform, core]))
        for name in EXAMPLES:
            products.append(self.executable(name, [f"examples/{name}/main.c"], [core], module=True))
        products.append(self.executable("pendulum_analysis", ["examples/pendulum/analysis.c"], [core], module=True))
        if not self.args.no_app:
            project = self.archive("physim-project", [f"app/{name}.c" for name in PROJECT])
            libraries["project"] = project
            products.append(self.executable("physim-build", ["app/build_main.c"], [project, platform, core], sdl=True))
            products.append(self.executable("physim", [f"app/{name}.c" for name in APP],
                            [project, batch, platform, language, zlib, core], sdl=True,
                            defines=("Z_PREFIX", f'PS_SOURCE_DIR="{ROOT.as_posix()}"')))
            products.append(self.runtime)
        if self.args.benchmarks:
            self.benchmark("physim-benchmark", libraries)
            if not self.args.no_app:
                self.benchmark("physim-ui-benchmark", libraries)
        if self.args.test or self.args.test_display:
            spec = importlib.util.spec_from_file_location("native_tests", Path(__file__).with_name("native_tests.py"))
            tests = importlib.util.module_from_spec(spec)
            sys.modules[spec.name] = tests
            sys.dont_write_bytecode = True
            spec.loader.exec_module(tests)
            tests.run_suite(self, libraries, ROOT, self.args.test_filter, display=self.args.test_display)
        print(f"Build complete: {self.bin}")
        if self.args.install:
            self.install(products, self.args.install.resolve())

    def install(self, products: list[Path], output: Path):
        if output.exists():
            raise RuntimeError("Installation requires a new output directory.")
        for name in ("include", "examples", "docs", "src", "third_party"):
            source = (ROOT / name).resolve()
            if source == output or source in output.parents:
                raise RuntimeError("Installation must be outside the SDK source directories.")
        if WINDOWS and self.args.config != "Release":
            raise RuntimeError("Windows packages require --config Release (redistributable runtime).")
        if WINDOWS:
            redist = Path(self.env.get("VCTOOLSREDISTDIR", "")) / "x64/Microsoft.VC143.CRT"
            runtime = sorted(redist.glob("*.dll"))
            if not (redist / "vcruntime140.dll").is_file():
                raise RuntimeError("Visual Studio x64 redistributable runtime was not found.")
            products = products + runtime
        output.parent.mkdir(parents=True, exist_ok=True)
        # Publish only a complete installation. On failure the staging directory
        # remains available for diagnosis and the requested output is untouched.
        staging = Path(tempfile.mkdtemp(prefix=output.name + ".pending-", dir=output.parent))
        for product in products:
            destination = staging / ("lib" if product == self.lib / ("physim-core.lib" if self.msvc else "libphysim-core.a") else "bin") / product.name
            destination.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(product, destination)
        for directory in ("include", "examples", "docs"):
            shutil.copytree(ROOT / directory, staging / directory,
                            ignore=shutil.ignore_patterns("__pycache__", "*.pyc", "build", "runs", "CMakeLists.txt", "*.cmake"))
        for name in CORE:
            destination = staging / "src" / (name + ".c")
            destination.parent.mkdir(exist_ok=True)
            shutil.copy2(ROOT / "src" / (name + ".c"), destination)
        shutil.copy2(ROOT / "src/report_internal.h", staging / "src/report_internal.h")
        shutil.copy2(ROOT / "src/report_mask.inc",staging / "src/report_mask.inc")
        shutil.copy2(ROOT / "src/text_validation.h", staging / "src/text_validation.h")
        shutil.copy2(ROOT / "src/number_parse.h", staging / "src/number_parse.h")
        shutil.copy2(ROOT / "src/pchip.h", staging / "src/pchip.h")
        (staging / "licenses").mkdir()
        for source, name in (("third_party/Nuklear-LICENSE", "Nuklear-LICENSE"),
                             ("third_party/zlib-1.3.2/LICENSE", "zlib-LICENSE.txt"),
                             ("third_party/SDL3-LICENSE.txt", "SDL3-LICENSE.txt"),
                             ("third_party/README.md", "Dependencies.md")):
            shutil.copy2(ROOT / source, staging / "licenses" / name)
        shutil.copy2(ROOT / "tools/SDK-README.md", staging / "README.md")
        for name in ("LICENSE", "Physim_Projektplan.md"):
            shutil.copy2(ROOT / name, staging / name)
        # Relative paths and content hashes make moved packages independently auditable.
        files = {p.relative_to(staging).as_posix(): hashlib.sha256(p.read_bytes()).hexdigest()
                 for p in sorted(staging.rglob("*")) if p.is_file()}
        (staging / "physim-sdk.json").write_text(json.dumps({"format": 1, "config": self.args.config,
            "platform": sys.platform, "app": not self.args.no_app, "files": files}, indent=2) + "\n", encoding="utf-8")
        staging.rename(output)
        print(f"Installed SDK: {output}")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--config", choices=("Debug", "Release"), default="Debug")
    parser.add_argument("--build-dir", type=Path)
    parser.add_argument("--compiler", help="C17 compiler executable (cl/clang-cl on Windows, cc/clang/gcc elsewhere)")
    parser.add_argument("--sdl", type=Path, help="SDL3 installation prefix")
    parser.add_argument("--no-app", action="store_true", help="Build the library, language compiler and runners without SDL")
    parser.add_argument("--jobs", type=int, default=min(os.cpu_count() or 1, 8))
    parser.add_argument("--benchmarks", action="store_true", help="Build performance tools; --no-app omits the OpenGL benchmark")
    parser.add_argument("--sanitizers", action="store_true", help="Validate with AddressSanitizer (also UndefinedBehaviorSanitizer on Linux/macOS)")
    parser.add_argument("--fuzzer", action="store_true", help="Build only the Clang libFuzzer IPC harness and seed generator, with sanitizers and without SDL")
    parser.add_argument("--examples", action="store_true", help="Also build all standalone Physim language examples and experiment/analysis modules")
    test_mode = parser.add_mutually_exclusive_group()
    test_mode.add_argument("--test", action="store_true", help="Run tests without windows or CTest")
    test_mode.add_argument("--test-display", action="store_true", help="Run window and graphics tests (requires a graphical desktop)")
    parser.add_argument("--test-filter", action="append", help="Select test names using a glob; repeat to combine groups (with --test or --test-display)")
    parser.add_argument("--rebuild", action="store_true", help="Recompile and relink all selected targets")
    parser.add_argument("--install", type=Path, help="Install an SDK and portable app to a new directory")
    args = parser.parse_args()
    if args.test_filter and not (args.test or args.test_display):
        parser.error("--test-filter requires --test or --test-display")
    if args.test_display and args.no_app:
        parser.error("--test-display requires the app; remove --no-app")
    if args.fuzzer:
        if args.test or args.test_display or args.install or args.benchmarks or args.examples:
            parser.error("--fuzzer builds a separate developer tool; omit --test, --test-display, --install, --benchmarks and --examples")
        args.no_app = True
        args.sanitizers = True
    if args.sanitizers and args.install:
        parser.error("Sanitizer builds are for validation; use a separate build without --sanitizers to install a portable SDK")
    if args.jobs < 1 or args.jobs > 64:
        parser.error("--jobs must be between 1 and 64")
    args.build_dir = args.build_dir or ROOT / "build" / "native" / (args.config + ("-fuzzer" if args.fuzzer else "-sanitized" if args.sanitizers else ""))
    if args.build_dir.resolve() == ROOT or args.build_dir.resolve() in ROOT.parents:
        parser.error("Use a separate build directory, not the source root or its parents")
    try:
        with build_lock(args.build_dir.resolve()):
            Builder(args, compiler_environment()).build()
    except (OSError, RuntimeError, subprocess.SubprocessError) as error:
        print(f"Build failed: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
