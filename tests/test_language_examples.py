"""Run native language examples and verify incremental emission/error recovery."""
import argparse
import importlib.util
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--bin", type=Path, required=True)
    parser.add_argument("--work", type=Path, required=True)
    parser.add_argument("--compiler")
    parser.add_argument("--config", choices=("Debug", "Release"), default="Debug")
    args = parser.parse_args()
    repo = Path(__file__).resolve().parent.parent
    spec = importlib.util.spec_from_file_location("example_build", repo / "tools/build.py")
    native = importlib.util.module_from_spec(spec)
    sys.dont_write_bytecode = True
    spec.loader.exec_module(native)
    binaries = args.bin.resolve()
    suffix = ".exe" if native.WINDOWS else ""
    module_suffix = ".dll" if native.WINDOWS else ".so"
    args.work.mkdir(parents=True, exist_ok=True)
    work = Path(tempfile.mkdtemp(prefix="language examples ä ", dir=args.work)).resolve()
    env = native.compiler_environment()

    def checked(program, *arguments):
        result = subprocess.run([str(program), *map(str, arguments)], cwd=work, env=env,
                                capture_output=True, timeout=60)
        assert result.returncode == 0, (program, result.returncode, result.stdout, result.stderr)
        return result.stdout.decode("utf-8").replace("\r\n", "\n")

    for name, mode, _ in native.language_examples():
        path = binaries / ("language-" + name + (suffix if mode == "--emit-c" else module_suffix))
        assert path.is_file(), path
        if mode == "--emit-c":
            output = checked(path)
            if name == "energy":
                assert output == "9\n1\n45\n", output
    run = work / "pendulum.psrun"
    checked(binaries / ("physim-runner" + suffix), binaries / ("language-pendulum" + module_suffix),
            run, "--steps", "200", "--dt", ".005", "--seed", "42")
    checked(binaries / ("physim-analysis-runner" + suffix), binaries / ("language-analysis" + module_suffix),
            run, work / "report")
    assert run.is_file() and (work / "report.psreport").is_file()

    # Real Physim/C compilers, isolated mutable source, and the already built core.
    fixture = work / "fixture"
    (fixture / "examples").mkdir(parents=True)
    (fixture / "app").mkdir()
    shutil.copy2(repo / "app/utf8.manifest", fixture / "app/utf8.manifest")
    shutil.copytree(repo / "include", fixture / "include")
    source = fixture / "examples/check.phys"
    native.ROOT = fixture
    native.language_examples = lambda: [("check", "--emit-c", "check.phys")]
    options = argparse.Namespace(build_dir=fixture / "build", compiler=args.compiler,
                                 no_app=True, config=args.config, jobs=2)
    core = binaries.parent / "lib" / ("physim-core.lib" if native.WINDOWS else "libphysim-core.a")
    with native.build_lock(options.build_dir):
        builder = native.Builder(options, env)

        def build():
            return builder.build_language_examples(binaries / ("physimc" + suffix), core)[0]

        source.write_text("print(1)\n", encoding="utf-8")
        program = build()
        assert checked(program) == "1\n"
        generated = options.build_dir / "examples/check.c"
        stamps = [path.stat().st_mtime_ns for path in (generated, program)]
        build()
        assert stamps == [path.stat().st_mtime_ns for path in (generated, program)]
        previous = [path.read_bytes() for path in (generated, program)]
        source.write_text('let invalid: Int64 = "text"\n', encoding="utf-8")
        try:
            build()
            raise AssertionError("Invalid example was accepted")
        except RuntimeError:
            pass
        assert previous == [path.read_bytes() for path in (generated, program)]
        assert checked(program) == "1\n"
        assert not list((options.build_dir / "examples").glob("*.pending.c"))
        source.write_text("print(2)\n", encoding="utf-8")
        assert checked(build()) == "2\n"
        assert set((fixture / "examples").iterdir()) == {source}
    print(f"15 programs, 27 modules, runner/analysis, incremental emission and error recovery passed: {work}")


if __name__ == "__main__":
    main()
