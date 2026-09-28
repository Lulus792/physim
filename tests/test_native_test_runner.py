"""Check failure handling and reporting of the CMake-independent test runner."""
import argparse
import importlib.util
import json
import os
from pathlib import Path
import sys
import tempfile
from types import SimpleNamespace


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--work", type=Path, required=True)
    args = parser.parse_args()
    args.work.mkdir(parents=True, exist_ok=True)
    directory = Path(tempfile.mkdtemp(prefix="test runner ä ", dir=args.work)).resolve()
    source = Path(__file__).resolve().parent.parent
    spec = importlib.util.spec_from_file_location("native_tests", source / "tools/native_tests.py")
    runner = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = runner
    sys.dont_write_bytecode = True
    spec.loader.exec_module(runner)

    def check(code, **options):
        return runner.execute_case([sys.executable, "-c", code], work=directory,
                                   env=os.environ, timeout=options.pop("timeout", 10), **options)

    assert check("print('ok')", stdout="ok\n")["status"] == "passed"
    error_program = "import sys; print('cleaned'); print('source:4: error',file=sys.stderr); sys.exit(70)"
    assert check(error_program, exit_code=70, stdout="cleaned\n", stderr_pattern="source:4: error")["status"] == "passed"
    assert check(error_program)["status"] == "failed"
    assert check(error_program, exit_code=70, stdout="wrong\n")["status"] == "failed"
    assert check(error_program, exit_code=70, stderr_pattern="missing diagnostic")["status"] == "failed"
    assert check("import time; time.sleep(60)", timeout=.2)["status"] == "timeout"
    assert runner.execute_case([str(directory / "missing-program")], work=directory,
                               env=os.environ, timeout=1)["status"] == "failed"

    # A failed build and failed test must not suppress later cases or leave a
    # success-only summary. Use real child processes for the executable cases.
    runner.catalog = lambda: [
        runner.Case("build_error", ("missing.c",)),
        runner.Case("exit_error", (), arguments=("-c", "raise SystemExit(7)")),
        runner.Case("later_success", (), arguments=("-c", "print('still ran')")),
    ]

    class FixtureBuilder:
        args = SimpleNamespace(no_app=True)
        env = os.environ
        def __init__(self):
            self.directory = directory
        def executable(self, name, sources, libraries, **options):
            if sources:
                raise RuntimeError("Expected compiler failure")
            return Path(sys.executable)

    try:
        runner.run_suite(FixtureBuilder(), {"platform": None, "core": None}, source)
        raise AssertionError("A failed suite returned success")
    except RuntimeError as error:
        assert "2 native tests failed" in str(error)
    reports = list((directory / "test-results").glob("*/results.json"))
    assert len(reports) == 1
    results = json.loads(reports[0].read_text(encoding="utf-8"))
    assert [case["status"] for case in results] == ["build_failed", "failed", "passed"]
    assert results[-1]["stdout"] == "still ran\n"
    try:
        runner.run_suite(FixtureBuilder(), {}, source, "no-such-test")
        raise AssertionError("Empty test selection returned success")
    except RuntimeError as error:
        assert "No native tests match" in str(error)
    print("Native test runner: expected exits, diagnostics, timeouts, launch/build failures and complete reports passed")


if __name__ == "__main__":
    main()
