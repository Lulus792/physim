"""Direct test catalog and runner. No CMake files are read or executed.

These cases reuse existing C and language regressions and their expectations.
Further language workflow wrappers and display workflows are migrated separately.
"""
from dataclasses import dataclass
from fnmatch import fnmatchcase
import hashlib
import json
from pathlib import Path
import re
import subprocess
import sys
import tempfile
import time


@dataclass(frozen=True)
class Case:
    name: str
    sources: tuple[str, ...]
    libraries: tuple[str, ...] = ("platform", "core")
    arguments: tuple[str, ...] = ()
    defines: tuple[str, ...] = ()
    app: bool = False
    timeout: int = 60
    exit_code: int = 0
    stdout: str | None = None
    stderr_pattern: str | None = None
    language_source: str | None = None
    language_checks: tuple = ()
    language_mode: str = "--emit-c"
    language_harness: str | None = None
    language_module_paths: tuple[str, ...] = ()
    language_workflow: dict | None = None


def catalog():
    cases = []
    for name in ("core", "numerics", "mechanics", "hashmap", "string_view", "array",
                 "memory", "memory_owners", "math", "box_contacts", "contact_graph",
                 "distance_joint", "constraint_graph", "broad_phase", "measurement",
                 "buoyancy", "contacts", "resample", "series", "report", "scene_view"):
        cases.append(Case(name, (f"tests/test_{name}.c",)))
    for name, source in (("sphere_sweep", "sweep"), ("series_select", "select"),
                         ("scene_protocol", "scene"), ("parameters_api", "parameters"),
                         ("language_values", "language_values"),
                         ("language_string_storage", "language_string"),
                         ("language_locale", "language_locale")):
        cases.append(Case(name, (f"tests/test_{source}.c",)))
    for part in ("lexer", "parser", "checker"):
        cases.append(Case("language_" + part, (f"tests/test_language_{part}.c",),
                          ("language",), timeout=120))
    cases.extend([
        Case("parameter_catalog", ("tests/test_parameter_catalog.c", "app/parameter_catalog.c")),
        Case("documentation", ("tests/test_documentation.c", "app/documentation.c")),
        Case("plot_view", ("tests/test_plot_view.c", "app/plot_view.c")),
        Case("crc", ("tests/test_crc.c",)),
        Case("crc_report_roundtrip", ("tests/test_crc.c",),
             arguments=("--roundtrip", "{source}/tests/fixtures/crc-report-v1.bin")),
        Case("protocol_mutations", ("tests/fuzz_protocol.c",),
             defines=("PS_FUZZ_STANDALONE",), timeout=120),
        Case("run_mutations", ("tests/fuzz_run.c",), timeout=180),
        Case("report_mutations", ("tests/fuzz_report.c",), timeout=300),
        Case("language_values_exit", ("tests/test_language_values_exit.c",), exit_code=70,
             stdout="standalone owners released\n",
             stderr_pattern=r"array-exit\.phys:8:4: runtime error: owned array exit probe"),
        Case("png", ("tests/test_png.c", "app/png.c"), ("zlib", "core"),
             arguments=("{work}/png-reference.png",), defines=("Z_PREFIX",)),
        Case("project_file", ("tests/test_project_file.c",), ("project", "core"),
             arguments=("{work}",), app=True),
        Case("ui_geometry", ("tests/test_ui_geometry.c", "app/ui_geometry.c", "app/ui_backend.c"), app=True),
        Case("editor_clipboard", ("tests/test_editor_clipboard.c", "app/ui_backend.c"), app=True),
        Case("font_shape", ("tests/test_font_shape.c",), app=True),
        Case("library", ("tests/test_library.c", "app/library.c"), app=True),
        Case("autosave", ("tests/test_autosave.c", "app/autosave.c"), app=True),
        Case("preferences", ("tests/test_preferences.c", "app/preferences.c"),
             arguments=("{work}",), app=True),
        Case("workspace_state", ("tests/test_workspace_state.c", "app/workspace_state.c", "app/autosave.c"),
             arguments=("{work}",), app=True),
        Case("workspace_tree", ("tests/test_workspace_tree.c", "app/workspace_tree.c", "app/autosave.c"),
             arguments=("{work}",), app=True),
        Case("text_document", ("tests/test_text_document.c",), ("project", "core"),
             arguments=("{work}",), app=True),
    ])
    corpus = json.loads((Path(__file__).resolve().parent.parent / "tests/native_language_cases.json").read_text(encoding="utf-8"))
    if corpus.get("format") != 1:
        raise RuntimeError("Unsupported native language test corpus")
    for group in corpus["groups"]:
        if not group["checks"]:
            raise RuntimeError(f"Language group has no checks: {group['name']}")
        if "runtime_source" in group:
            cases.append(Case("language_" + group["name"] + "_runtime", (), ("core",),
                              language_source=group["runtime_source"]))
        cases.append(Case("language_" + group["name"], (), language_checks=tuple(group["checks"])))
    for program in corpus.get("programs", []):
        cases.append(Case("language_" + program["name"], (), ("core",),
                          language_source=program["source"], language_mode=program.get("mode", "--emit-c"),
                          language_harness=program.get("harness"), exit_code=program.get("exit_code", 0),
                          language_module_paths=tuple(program.get("module_paths", ())),
                          stdout=program.get("stdout"), stderr_pattern=program.get("stderr_pattern")))
    for workflow in corpus.get("workflows", []):
        if not workflow["modules"] or not workflow["steps"]:
            raise RuntimeError(f"Empty language workflow: {workflow['name']}")
        cases.append(Case("language_" + workflow["name"], (), language_workflow=workflow))
    names = [case.name for case in cases]
    if len(set(names)) != len(names):
        raise RuntimeError("Duplicate native test names")
    return cases


def build_language_program(name, fixture, mode, module, builder, libraries, work, steps, *, module_paths=()):
    """Record emission/build failures before callers can run a stale artifact."""
    compiler = builder.bin / ("physimc.exe" if sys.platform == "win32" else "physimc")
    arguments = [str(compiler), mode]
    for path in module_paths:
        arguments.extend(["--module-path", str(path)])
    emitted = execute_case([*arguments, str(fixture)], work=work, env=builder.env, timeout=30)
    code = emitted.pop("stdout", "")
    steps.append(emitted)
    if emitted["status"] != "passed":
        emitted["stdout"] = code
        return None
    generated = builder.directory / "generated" / name / "main.c"
    generated.parent.mkdir(parents=True, exist_ok=True)
    content = code.encode("utf-8")
    emitted["generated_source"] = str(generated)
    emitted["source_sha256"] = hashlib.sha256(content).hexdigest()
    if not generated.is_file() or generated.read_bytes() != content:
        pending = generated.with_suffix(".pending.c")
        pending.write_bytes(content)
        pending.replace(generated)
    try:
        return builder.executable("physim-test-" + name, [str(generated)], libraries,
                                  language=True, module=module)
    except (OSError, RuntimeError) as error:
        steps.append({"status": "build_failed", "reason": str(error)})
        return None


def write_language_fixture(directory, filename, content):
    path = (directory / filename).resolve()
    if not path.is_relative_to(directory.resolve()):
        raise RuntimeError(f"Language fixture escapes its test directory: {filename}")
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding="utf-8", newline="\n")
    return path


def check_workflow_outputs(step, work):
    artifacts = []
    errors = []
    contents = step.get("file_contents", {})
    for filename in dict.fromkeys([*step.get("files", ()), *contents]):
        path = work / filename
        if not path.is_file():
            errors.append("Missing workflow output: " + filename)
            continue
        data = path.read_bytes()
        artifacts.append({"path": str(path), "bytes": len(data), "sha256": hashlib.sha256(data).hexdigest()})
        if filename in contents:
            try:
                actual = data.decode("utf-8").replace("\r\n", "\n")
            except UnicodeDecodeError:
                actual = None
            if actual != contents[filename]:
                errors.append("Workflow output content did not match: " + filename)
    for filename in step.get("absent_files", ()):
        path = work / filename
        absent = not path.exists()
        artifacts.append({"path": str(path), "absent": absent})
        if not absent:
            errors.append("Rejected operation created an output: " + filename)
    return {"status": "failed" if errors else "passed", "reason": "\n".join(errors), "artifacts": artifacts}


def prepare_compiler_case(check, source, work):
    """Materialize each CLI/import probe independently, including absent inputs."""
    for filename, content in check.get("files", {}).items():
        write_language_fixture(work, filename, content)
    for name in check.get("directories", ()):
        path = (work / name).resolve()
        if not path.is_relative_to(work.resolve()):
            raise RuntimeError(f"Language directory escapes its test directory: {name}")
        path.mkdir(parents=True, exist_ok=True)
    entry = check.get("entry", check["name"] + ".phys")
    content = check.get("source")
    if "source_file" in check:
        content = (source / check["source_file"]).read_text(encoding="utf-8")
    if "repeat_source" in check:
        repeated = check["repeat_source"]
        content = repeated["text"] * repeated["count"]
    path = work / entry
    if content is not None:
        path = write_language_fixture(work, entry, content)
    arguments = check.get("arguments", [check.get("mode", "--check"), "{entry}"])
    values = {"work": str(work), "root": str(source), "entry": str(path)}
    return [argument.format_map(values) for argument in arguments]


def language_case(case, builder, libraries, source, work):
    started = time.monotonic()
    compiler = builder.bin / ("physimc.exe" if sys.platform == "win32" else "physimc")
    steps = []
    if case.language_workflow:
        workflow = case.language_workflow
        paths = {"work": str(work)}
        for module in workflow["modules"]:
            program = build_language_program(case.name + "-" + module["name"], source / module["source"],
                                             module["mode"], True, builder, [libraries["core"]], work, steps)
            if program is None:
                break
            paths[module["name"]] = str(program)
        else:
            for step in workflow["steps"]:
                executable = builder.bin / (step["program"] + (".exe" if sys.platform == "win32" else ""))
                command = [str(executable), *(arg.format_map(paths) for arg in step["arguments"])]
                result = execute_case(command, work=work, env=builder.env, timeout=step.get("timeout", 60),
                                      exit_code=step.get("exit_code", 0), stdout=step.get("stdout"),
                                      stderr_pattern=step.get("stderr_pattern"), stderr=step.get("stderr"),
                                      stdout_patterns=step.get("stdout_patterns", ()))
                if result["status"] == "passed":
                    result.update(check_workflow_outputs(step, work))
                steps.append(result)
                if result["status"] != "passed":
                    break
    elif case.language_source:
        program = build_language_program(case.name, source / case.language_source, case.language_mode,
                                         bool(case.language_harness), builder,
                                         [libraries[name] for name in case.libraries], work, steps,
                                         module_paths=[source / path for path in case.language_module_paths])
        if program is not None:
            try:
                command = [str(program)]
                if case.language_harness:
                    harness = builder.executable("physim-test-" + case.name + "-harness",
                                                 [case.language_harness],
                                                 [libraries["platform"], libraries["core"]])
                    command.insert(0, str(harness))
                steps.append(execute_case(command, work=work, env=builder.env, timeout=case.timeout,
                                          exit_code=case.exit_code, stdout=case.stdout,
                                          stderr_pattern=case.stderr_pattern))
            except (OSError, RuntimeError) as error:
                steps.append({"status": "build_failed", "reason": str(error)})
    else:
        for check in case.language_checks:
            check_work = work / check["name"]
            check_work.mkdir()
            arguments = prepare_compiler_case(check, source, check_work)
            result = execute_case([str(compiler), *arguments], work=check_work,
                                  env=builder.env, timeout=15, exit_code=check["exit_code"],
                                  stdout=check.get("stdout"), stderr_pattern=check.get("stderr_pattern"),
                                  stderr=check.get("stderr"), stdout_patterns=check.get("stdout_patterns", ()),
                                  stdout_captures=check.get("stdout_captures", ()),
                                  stdout_counts=check.get("stdout_counts", ()))
            result["name"] = check["name"]
            steps.append(result)
    failures = [step for step in steps if step["status"] != "passed"]
    return {"status": "failed" if failures else "passed", "steps": steps,
            "seconds": round(time.monotonic() - started, 3),
            "reason": "\n".join(step.get("reason", "") + "\n" + step.get("stderr", "") for step in failures)}


def execute_case(command, *, work: Path, env, timeout: float, exit_code=0,
                 stdout=None, stderr_pattern=None, stderr=None, stdout_patterns=(), stdout_captures=(), stdout_counts=()):
    """Keep expected failures distinct from crashes, timeouts and launch failures."""
    started = time.monotonic()
    result = {"command": [str(p) for p in command], "status": "failed"}
    try:
        completed = subprocess.run(command, cwd=work, env=env, capture_output=True, timeout=timeout)
        output = completed.stdout.decode("utf-8", errors="replace").replace("\r\n", "\n")
        error = completed.stderr.decode("utf-8", errors="replace").replace("\r\n", "\n")
        result.update(exit_code=completed.returncode, stdout=output, stderr=error)
        if completed.returncode != exit_code:
            result["reason"] = f"Exit code {completed.returncode}; expected {exit_code}"
        elif stdout is not None and output != stdout:
            result["reason"] = "Standard output did not match the expected text"
        elif stderr_pattern is not None and not re.search(stderr_pattern, error):
            result["reason"] = "Expected diagnostic was missing"
        elif stderr is not None and error != stderr:
            result["reason"] = "Standard error did not match the expected text"
        elif any(not re.search(pattern, output) for pattern in stdout_patterns):
            result["reason"] = "Expected output pattern was missing"
        elif any(len(list(re.finditer(item["pattern"], output))) != item["count"] for item in stdout_counts):
            result["reason"] = "Output pattern occurred an unexpected number of times"
        else:
            result["status"] = "passed"
            for capture in stdout_captures:
                match = re.search(capture["pattern"], output)
                if match is None or match.group(1).strip() != capture["expected"]:
                    result.update(status="failed", reason="First output capture did not match the expected text")
                    break
    except subprocess.TimeoutExpired as error:
        result.update(status="timeout", reason=f"Exceeded {timeout} seconds",
                      stdout=(error.stdout or b"").decode("utf-8", errors="replace"),
                      stderr=(error.stderr or b"").decode("utf-8", errors="replace"))
    except OSError as error:
        result["reason"] = str(error)
    result["seconds"] = round(time.monotonic() - started, 3)
    return result


def run_suite(builder, libraries, source: Path, pattern="*"):
    cases = [case for case in catalog() if (not case.app or not builder.args.no_app)
             and fnmatchcase(case.name, pattern)]
    if not cases:
        raise RuntimeError(f"No native tests match {pattern!r} in this configuration")
    root = builder.directory / "test-results"
    root.mkdir(exist_ok=True)
    directory = Path(tempfile.mkdtemp(prefix="run-", dir=root))
    results = []
    for case in cases:
        work = directory / case.name
        work.mkdir()
        try:
            if case.language_source or case.language_checks or case.language_workflow:
                result = language_case(case, builder, libraries, source, work)
            else:
                program = builder.executable("physim-test-" + case.name, list(case.sources),
                                             [libraries[name] for name in case.libraries],
                                             sdl=case.app, defines=case.defines)
                arguments = [arg.format(work=work.as_posix(), source=source.as_posix()) for arg in case.arguments]
                result = execute_case([str(program), *arguments], work=work, env=builder.env,
                                      timeout=case.timeout, exit_code=case.exit_code,
                                      stdout=case.stdout, stderr_pattern=case.stderr_pattern)
        except (OSError, RuntimeError) as error:
            result = {"status": "build_failed", "reason": str(error)}
        result["name"] = case.name
        results.append(result)
        (work / "result.json").write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
        # Update after every case so interrupted runs retain completed evidence.
        (directory / "results.json").write_text(json.dumps(results, indent=2) + "\n", encoding="utf-8")
        print(f"TEST {case.name}: {result['status']}", flush=True)
        if result["status"] != "passed":
            print(result.get("reason", "") + "\n" + result.get("stdout", "") + result.get("stderr", ""), flush=True)
    failures = sum(result["status"] != "passed" for result in results)
    print(f"Native tests: {len(results) - failures}/{len(results)} passed. Results: {directory}")
    if failures:
        raise RuntimeError(f"{failures} native tests failed; see {directory}")
