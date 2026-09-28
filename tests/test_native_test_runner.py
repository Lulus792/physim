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
    counts = (dict(pattern=r"Shared\.phys", count=1), dict(pattern=r"Other\.phys", count=0))
    assert check("print('Shared.phys')", stdout_counts=counts)["status"] == "passed"
    assert check("print('Shared.phys Shared.phys')", stdout_counts=counts)["status"] == "failed"
    assert check("print('Shared.phys Other.phys')", stdout_counts=counts)["status"] == "failed"
    assert check("import time; time.sleep(60)", timeout=.2)["status"] == "timeout"
    assert runner.execute_case([str(directory / "missing-program")], work=directory,
                               env=os.environ, timeout=1)["status"] == "failed"

    probe = directory / "compiler source ä"
    probe.mkdir()
    arguments = runner.prepare_compiler_case(dict(name="missing"), source, probe)
    assert arguments == ["--check", str(probe / "missing.phys")]
    assert not (probe / "missing.phys").exists()
    arguments = runner.prepare_compiler_case(dict(name="entry", entry="Local/Main ä.phys", source="import Shared\n",
        files={"Local/Shared.phys": "let value = 1\n"},
        arguments=["--deps", "--module-path", "{work}", "{entry}"]), source, probe)
    assert arguments == ["--deps", "--module-path", str(probe), str(probe / "Local/Main ä.phys")]
    assert (probe / "Local/Shared.phys").read_bytes() == b"let value = 1\n"
    runner.prepare_compiler_case(dict(name="oversized", repeat_source=dict(text=" ", count=1048577)), source, probe)
    assert (probe / "oversized.phys").stat().st_size == 1048577

    # Output existence alone cannot validate an export. Preserve exact values,
    # normalize only CRLF and reject artifacts from failed operations.
    expected_csv = '"time [s]","distance [m]"\n0,1\n'
    csv_path = directory / "export.csv"
    csv_path.write_bytes(expected_csv.replace("\n", "\r\n").encode("utf-8"))
    output_check = dict(file_contents={"export.csv": expected_csv}, absent_files=["rejected.csv"])
    result = runner.check_workflow_outputs(output_check, directory)
    assert result["status"] == "passed" and len(result["artifacts"][0]["sha256"]) == 64
    csv_path.write_text(expected_csv.replace("0,1", "0,2"), encoding="utf-8")
    assert runner.check_workflow_outputs(output_check, directory)["status"] == "failed"
    csv_path.write_bytes(b"\xff\xfe")
    assert runner.check_workflow_outputs(output_check, directory)["status"] == "failed"
    csv_path.write_bytes(expected_csv.encode("utf-8"))
    (directory / "rejected.csv").write_text("", encoding="utf-8")
    assert runner.check_workflow_outputs(output_check, directory)["status"] == "failed"
    assert runner.check_workflow_outputs(dict(file_contents={"missing.csv": ""}), directory)["status"] == "failed"

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

    # Source-map checks must reject unrelated build failures, wrong source lines,
    # and successful compilation even when the expected words appear in output.
    for scenario in ("msvc", "clang", "wrong_line", "wrong_file", "success", "unrelated"):
        map_work = directory / ("map_" + scenario)
        map_work.mkdir()

        class MapBuilder(LanguageBuilder):
            def __init__(self):
                self.directory = map_work
            def compile(self, path, **options):
                assert "#line 1 \"source_map_probe.phys\"\n#error PHYSIM_SOURCE_MAP_PROBE\n" in Path(path).read_text()
                location = "source_map_probe.phys(1)" if scenario == "msvc" else "source_map_probe.phys:1:2"
                if scenario == "wrong_line":
                    location = "source_map_probe.phys:2:2"
                elif scenario == "wrong_file":
                    location = "source_map_probe.c:1:2"
                print("unrelated compiler error" if scenario == "unrelated" else location + ": error: PHYSIM_SOURCE_MAP_PROBE")
                if scenario != "success":
                    raise RuntimeError("C compilation rejected the probe")

        def mapped_process(command, **options):
            code = '#line 1 "source_map_probe.phys"\n#line 2 "source_map_probe.phys"\n'
            return execute([sys.executable, "-c", "print(" + repr(code) + ", end='')"], **options)

        runner.execute_case = mapped_process
        try:
            result = runner.language_case(runner.Case("map", (), libraries=(), language_probe="source_map"),
                                          MapBuilder(), {}, source, map_work)
            assert result["status"] == ("passed" if scenario in ("msvc", "clang") else "failed")
        finally:
            runner.execute_case = execute

    # Verify preservation rejects modified artifacts and temporary debris as
    # well as incorrect failures. The rejected source must never reach C build.
    for scenario in ("preserved", "changed_c", "changed_binary", "pending", "wrong_error", "crashed"):
        preserve_work = directory / scenario
        preserve_work.mkdir()
        build_calls = []

        class PreserveBuilder(LanguageBuilder):
            def __init__(self):
                self.directory = preserve_work
            def executable(self, *args, **options):
                build_calls.append(args)
                binary = preserve_work / "complete.exe"
                binary.write_bytes(b"complete program")
                return binary

        def preserve_process(command, **options):
            if "--emit-c" not in command:
                code = "print('native checks passed\\ntrue\\n42\\n0.5')"
            elif "immutable = 2" not in Path(command[-1]).read_text(encoding="utf-8"):
                code = "print('complete C')"
            else:
                generated = preserve_work / "generated/preserve/main.c"
                if scenario == "changed_c":
                    generated.write_bytes(b"truncated C")
                elif scenario == "changed_binary":
                    (preserve_work / "complete.exe").write_bytes(b"truncated binary")
                elif scenario == "pending":
                    generated.with_suffix(".pending.c").write_bytes(b"partial")
                diagnostic = "unrelated failure" if scenario == "wrong_error" else "preserved.phys:2:1: error: Assignment requires a mutable var binding"
                code = "import sys; print(" + repr(diagnostic) + ", file=sys.stderr); sys.exit(" + ("7" if scenario == "crashed" else "1") + ")"
            return execute([sys.executable, "-c", code], **options)

        runner.execute_case = preserve_process
        try:
            result = runner.language_case(runner.Case("preserve", (), libraries=(), language_probe="failed_emission"),
                                          PreserveBuilder(), {}, source, preserve_work)
            assert result["status"] == ("passed" if scenario == "preserved" else "failed")
            assert len(build_calls) == 1
            assert (preserve_work / "sources ä/preserved.phys").read_text(encoding="utf-8") == (source / "tests/fixtures/language/native.phys").read_text(encoding="utf-8")
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

    # Integration workflows combine language modules, C variants and verifiers.
    # No dependent verifier may run after a failed build or missing artifact.
    for scenario in ("success", "emission", "c_module", "verifier", "prebuilt", "runtime", "missing_output"):
        integration_work = directory / ("integration_" + scenario)
        integration_work.mkdir()
        prebuilt = integration_work / ("runner.exe" if sys.platform == "win32" else "runner")
        if scenario != "prebuilt":
            prebuilt.write_bytes(b"prebuilt runner")
        commands = []
        builds = []
        integration = dict(artifacts={
            "module": dict(source="fixture.phys", mode="--emit-experiment"),
            "reference": dict(sources=["reference.c"], libraries=["core"], module=True, defines=["PROBE=1"]),
            "verifier": dict(sources=["verifier.c"], libraries=["core"]),
            "runner": dict(prebuilt="runner")}, steps=[
                dict(program="verifier", arguments=["{module}", "{reference}", "{work}/result.dat", "{root}/tests/fixture.c"],
                     timeout=10, stdout_patterns=["verified"], files=["result.dat"]),
                dict(program="runner", arguments=["{work}/result.dat"], timeout=10, stdout="finished\n")])

        class IntegrationBuilder(LanguageBuilder):
            def __init__(self):
                self.directory = integration_work
                self.bin = integration_work
            def executable(self, name, sources, libraries, **options):
                builds.append(name)
                if name == "integration-reference":
                    assert options["module"] and options["defines"] == ("PROBE=1",)
                    if scenario == "c_module":
                        raise RuntimeError("C reference build failed")
                if name == "integration-verifier" and scenario == "verifier":
                    # Existing output must never be run after a build fails.
                    (integration_work / name).write_bytes(b"stale verifier")
                    raise RuntimeError("Verifier build failed")
                program = integration_work / name
                program.write_bytes(b"built artifact")
                return program

        def integration_process(command, **options):
            commands.append(command)
            if "--emit-experiment" in command:
                code = "raise SystemExit(1)" if scenario == "emission" else "print('generated module')"
            elif Path(command[0]).name == "integration-verifier":
                assert len(command) == 5 and "files ä" in command[-2]
                assert Path(command[-1]) == source / "tests/fixture.c"
                code = "print('verified')"
                if scenario == "runtime":
                    code += "; raise SystemExit(7)"
                elif scenario != "missing_output":
                    code += "; from pathlib import Path; Path('result.dat').write_bytes(b'checked')"
            else:
                code = "from pathlib import Path; assert Path('result.dat').read_bytes() == b'checked'; print('finished')"
            return execute([sys.executable, "-c", code], **options)

        runner.execute_case = integration_process
        try:
            result = runner.language_case(runner.Case("integration", (), integration=integration),
                                          IntegrationBuilder(), {"core": None}, source, integration_work)
            assert result["status"] == ("passed" if scenario == "success" else "failed")
            assert len(commands) == (3 if scenario == "success" else 2 if scenario in ("runtime", "missing_output") else 1)
            if scenario == "success":
                artifacts = [step for step in result["steps"] if "artifact" in step]
                assert len(artifacts) == 4 and all(len(step["sha256"]) == 64 for step in artifacts)
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
    before = set((directory / "test-results").glob("*/results.json"))
    runner.run_suite(FixtureBuilder(), {"platform": None, "core": None}, source,
                     ["later_*", "*success", "no-such-test"])
    after = set((directory / "test-results").glob("*/results.json"))
    assert len(after - before) == 1
    combined = json.loads((after - before).pop().read_text(encoding="utf-8"))
    assert len(combined) == 1 and combined[0]["name"] == "later_success", "Overlapping filters ran a test twice"
    try:
        runner.run_suite(FixtureBuilder(), {"platform": None, "core": None}, source, None)
        raise AssertionError("Omitted CLI filter did not run all tests")
    except RuntimeError as error:
        assert "2 native tests failed" in str(error)
    print("Native test runner: expected exits, diagnostics, timeouts, launch/build failures and complete reports passed")


if __name__ == "__main__":
    main()
