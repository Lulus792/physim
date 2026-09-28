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
        cases.append(Case("language_" + group["name"] + "_runtime", (), ("core",),
                          language_source=group["runtime_source"]))
        cases.append(Case("language_" + group["name"], (), language_checks=tuple(group["checks"])))
    return cases


def language_case(case, builder, libraries, source, work):
    started = time.monotonic()
    compiler = builder.bin / ("physimc.exe" if sys.platform == "win32" else "physimc")
    steps = []
    if case.language_source:
        emitted = execute_case([str(compiler), "--emit-c", str(source / case.language_source)],
                               work=work, env=builder.env, timeout=30)
        code = emitted.pop("stdout", "")
        steps.append(emitted)
        if emitted["status"] == "passed":
            generated = builder.directory / "generated" / case.name / "main.c"
            generated.parent.mkdir(parents=True, exist_ok=True)
            content = code.encode("utf-8")
            emitted["generated_source"] = str(generated)
            emitted["source_sha256"] = hashlib.sha256(content).hexdigest()
            if not generated.is_file() or generated.read_bytes() != content:
                pending = generated.with_suffix(".pending.c")
                pending.write_bytes(content)
                pending.replace(generated)
            try:
                program = builder.executable("physim-test-" + case.name, [str(generated)],
                                             [libraries[name] for name in case.libraries], language=True)
                steps.append(execute_case([str(program)], work=work, env=builder.env,
                                          timeout=case.timeout))
            except (OSError, RuntimeError) as error:
                steps.append({"status": "build_failed", "reason": str(error)})
        else:
            emitted["stdout"] = code
    else:
        for check in case.language_checks:
            path = work / (check["name"] + ".phys")
            path.write_text(check["source"], encoding="utf-8", newline="\n")
            result = execute_case([str(compiler), "--check", str(path)], work=work,
                                  env=builder.env, timeout=15, exit_code=check["exit_code"],
                                  stdout=check.get("stdout"), stderr_pattern=check["stderr_pattern"])
            result["name"] = check["name"]
            steps.append(result)
    failures = [step for step in steps if step["status"] != "passed"]
    return {"status": "failed" if failures else "passed", "steps": steps,
            "seconds": round(time.monotonic() - started, 3),
            "reason": "\n".join(step.get("reason", "") + "\n" + step.get("stderr", "") for step in failures)}


def execute_case(command, *, work: Path, env, timeout: float, exit_code=0,
                 stdout=None, stderr_pattern=None):
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
        else:
            result["status"] = "passed"
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
            if case.language_source or case.language_checks:
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
