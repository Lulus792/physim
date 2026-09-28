"""Regression for the direct repository build, using the real system compiler."""
import argparse
import hashlib
import importlib.util
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--work", type=Path, required=True)
    parser.add_argument("--compiler")
    args = parser.parse_args()
    repo = Path(__file__).resolve().parent.parent
    spec = importlib.util.spec_from_file_location("physim_build", repo / "tools/build.py")
    module = importlib.util.module_from_spec(spec)
    sys.dont_write_bytecode = True
    spec.loader.exec_module(module)
    args.work.mkdir(parents=True, exist_ok=True)
    source = Path(tempfile.mkdtemp(prefix="bootstrap source ä ", dir=args.work)).resolve()
    for name in ("src", "include", "app", "tests"):
        (source / name).mkdir()
    shutil.copyfile(repo / "app/utf8.manifest", source / "app/utf8.manifest")
    (source / "include/value.h").write_text("#define VALUE 1\n", encoding="utf-8")
    helper = source / "src/helper.c"
    helper_text = '#include "value.h"\nint value(void) { return VALUE; }\n'
    helper.write_text(helper_text, encoding="utf-8")
    (source / "src/main.c").write_text('#include <stdio.h>\nint value(void);\n'
                                      'int main(void) { printf("%d\\n", value()); return 0; }\n', encoding="utf-8")
    module.ROOT = source
    options = argparse.Namespace(build_dir=source / "build", config="Debug", no_app=True,
                                 compiler=args.compiler, jobs=2, test=False)
    env = module.compiler_environment()

    def build():
        with module.build_lock(options.build_dir):
            builder = module.Builder(options, env)
            library = builder.archive("value", ["src/helper.c"])
            return builder.executable("probe", ["src/main.c"], [library])

    def fingerprint(path):
        return hashlib.sha256(path.read_bytes()).hexdigest()

    program = build()
    assert subprocess.check_output([str(program)]).strip() == b"1"
    artifacts = [p for p in options.build_dir.rglob("*") if p.suffix in (".o", ".obj", ".a", ".lib")]
    times = {p: p.stat().st_mtime_ns for p in artifacts + [program]}
    build()
    assert times == {p: p.stat().st_mtime_ns for p in times}, "Unchanged build rewrote outputs"
    (source / "include/value.h").write_text("#define VALUE 2\n", encoding="utf-8")
    build()
    assert subprocess.check_output([str(program)]).strip() == b"2", "Header change was not rebuilt"
    good = fingerprint(program)
    helper.write_text("#error expected compiler failure\n", encoding="utf-8")
    try:
        build()
        raise AssertionError("Compiler failure was ignored")
    except RuntimeError:
        assert fingerprint(program) == good
    helper.write_text("int missing(void); int value(void) { return missing(); }\n", encoding="utf-8")
    try:
        build()
        raise AssertionError("Link failure was ignored")
    except RuntimeError:
        assert fingerprint(program) == good
    helper.write_text(helper_text, encoding="utf-8")
    build()
    assert subprocess.check_output([str(program)]).strip() == b"2"
    with program.open("ab") as file:
        file.write(b"corrupt cached artifact")
    broken = fingerprint(program)
    build()
    assert fingerprint(program) != broken, "Corrupted output was reused"
    generated = options.build_dir / "generated source ä"
    generated.mkdir()
    generated_source = generated / "main.c"
    generated_source.write_text('#include <stdio.h>\nint main(void) { puts("generated"); return 0; }\n', encoding="utf-8")
    with module.build_lock(options.build_dir):
        builder = module.Builder(options, env)
        generated_program = builder.executable("generated-probe", [str(generated_source)], [], language=True)
        assert subprocess.check_output([str(generated_program)]).strip() == b"generated"
        assert {p.name for p in generated.iterdir()} == {"main.c"}, "Objects leaked into generated sources"
        try:
            builder.compile(str(source.parent / "outside.c"))
            raise AssertionError("Source outside repository/build directory was accepted")
        except RuntimeError as error:
            assert "outside the repository/build directory" in str(error)
    # Test-only headers must invalidate their test programs too, including
    # allocator/memory guards that are not in public include directories.
    test_header = source / "tests/guard.h"
    test_source = source / "tests/guard.c"
    test_source.write_text('#include "guard.h"\nint main(void) { return TEST_EXIT; }\n', encoding="utf-8")
    for code in (0, 99):
        test_header.write_text(f"#define TEST_EXIT {code}\n", encoding="utf-8")
        with module.build_lock(options.build_dir):
            builder = module.Builder(options, env)
            test_program = builder.executable("test-header-probe", [str(test_source)], [])
        assert subprocess.run([str(test_program)]).returncode == code, "Test header change was not rebuilt"
    with module.build_lock(options.build_dir):
        try:
            with module.build_lock(options.build_dir):
                raise AssertionError("Concurrent build was allowed")
        except RuntimeError:
            pass
    assert not list(source.rglob("CMake*"))
    assert {p.name for p in source.iterdir()} == {"src", "include", "app", "tests", "build"}
    print("Direct build: Unicode/generated paths, incremental headers, failure recovery, artifact integrity and lock passed")


if __name__ == "__main__":
    main()
