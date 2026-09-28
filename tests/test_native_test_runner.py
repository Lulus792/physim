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
    assert check("print('first second')", stderr="", stdout_patterns=("first", "second"))["status"] == "passed"
    assert check("print('first')", stdout_patterns=("first", "second"))["status"] == "failed"
    assert check("import sys; print('warning', file=sys.stderr)", stderr="")["status"] == "failed"
    captures = (dict(pattern=r"literal\{([^}]*)\}", expected="97,10,0"),)
    assert check("print('literal{ 97,10,0 } literal{13,0}')", stdout_captures=captures)["status"] == "passed"
    assert check("print('literal{13,0} literal{97,10,0}')", stdout_captures=captures)["status"] == "failed"
    assert check("print('no literal')", stdout_captures=captures)["status"] == "failed"
    assert check("import time; time.sleep(60)", timeout=.2)["status"] == "timeout"
    assert runner.execute_case([str(directory / "missing-program")], work=directory,
                               env=os.environ, timeout=1)["status"] == "failed"

    # Exercise the language pipeline with real child-process failures. An
    # emitter failure must never compile or execute a previous generated file.
    execute = runner.execute_case
    invocations = []

    def language_process(command, **options):
        invocations.append(command)
        if "--emit-c" in command:
            code = "import sys; print('emitter failed', file=sys.stderr); sys.exit(2)"
        else:
            code = "import sys; print('expected type error', file=sys.stderr); sys.exit(1)"
        return execute([sys.executable, "-c", code], **options)

    class LanguageBuilder:
        env = os.environ
        bin = directory
        def __init__(self):
            self.directory = directory
        def executable(self, *args, **options):
            raise AssertionError("Failed emission must not build or run stale code")

    runner.execute_case = language_process
    try:
        result = runner.language_case(
            runner.Case("emitter_error", (), libraries=(), language_source="fixture.phys"),
            LanguageBuilder(), {}, source, directory)
        assert result["status"] == "failed" and len(result["steps"]) == 1
        assert result["steps"][0]["exit_code"] == 2
        checks = tuple(dict(name=name, source="invalid", exit_code=1, stderr_pattern=pattern)
                       for name, pattern in (("wrong", "missing"), ("later", "expected type error")))
        result = runner.language_case(runner.Case("diagnostics", (), language_checks=checks),
                                      LanguageBuilder(), {}, source, directory)
        assert result["status"] == "failed"
        assert [step["status"] for step in result["steps"]] == ["failed", "passed"]
        assert len(invocations) == 3
    finally:
        runner.execute_case = execute

    # A runtime-error fixture only passes with the required exit and source
    # diagnostic. The expected failure must not be applied to the emitter.
    def runtime_process(command, **options):
        code = "print('generated source')" if "--emit-c" in command else error_program
        return execute([sys.executable, "-c", code], **options)

    class RuntimeBuilder(LanguageBuilder):
        def executable(self, *args, **options):
            return Path(sys.executable)

    runner.execute_case = runtime_process
    try:
        case = runner.Case("runtime_error", (), libraries=(), language_source="fixture.phys",
                           exit_code=70, stdout="cleaned\n", stderr_pattern="source:4: error")
        result = runner.language_case(case, RuntimeBuilder(), {}, source, directory)
        assert result["status"] == "passed"
        assert [step["exit_code"] for step in result["steps"]] == [0, 70]
    finally:
        runner.execute_case = execute

    # Workflow success requires every build, process, diagnostic and output.
    # A missing artifact or failed module must stop dependent runner steps.
    workflow = dict(modules=[dict(name="module", source="fixture.phys", mode="--emit-experiment")],
                    steps=[dict(program="first", arguments=["{module}"], stdout_patterns=["first OK"], files=["result.dat"]),
                           dict(program="second", arguments=["{work}/result.dat"], stdout_patterns=["second OK"])])
    for scenario in ("success", "emission", "build", "missing", "process"):
        workflow_work = directory / scenario
        workflow_work.mkdir()
        commands = []

        class WorkflowBuilder(RuntimeBuilder):
            def executable(self, *args, **options):
                if scenario == "build":
                    raise RuntimeError("Expected module build failure")
                return super().executable(*args, **options)

        def workflow_process(command, **options):
            commands.append(command)
            if "--emit-experiment" in command:
                code = "raise SystemExit(2)" if scenario == "emission" else "print('generated module')"
            elif Path(command[0]).stem == "first":
                code = "print('first OK')"
                if scenario == "process":
                    code += "; raise SystemExit(7)"
                elif scenario != "missing":
                    code += "; from pathlib import Path; Path('result.dat').write_text('saved')"
            else:
                code = "from pathlib import Path; assert Path('result.dat').read_text() == 'saved'; print('second OK')"
            return execute([sys.executable, "-c", code], **options)

        runner.execute_case = workflow_process
        try:
            result = runner.language_case(runner.Case("workflow_" + scenario, (), language_workflow=workflow),
                                          WorkflowBuilder(), {"core": None}, source, workflow_work)
            assert result["status"] == ("passed" if scenario == "success" else "failed")
            assert len(commands) == (3 if scenario == "success" else 1 if scenario in ("emission", "build") else 2)
        finally:
            runner.execute_case = execute

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
