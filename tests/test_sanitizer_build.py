"""Prove that the direct compiler build detects actual memory and undefined behavior errors."""
import argparse
import importlib.util
import json
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
    spec = importlib.util.spec_from_file_location("sanitizer_build", repo / "tools/build.py")
    native = importlib.util.module_from_spec(spec)
    sys.dont_write_bytecode = True
    spec.loader.exec_module(native)
    args.work.mkdir(parents=True, exist_ok=True)
    root = Path(tempfile.mkdtemp(prefix="sanitizer probe ä ", dir=args.work)).resolve()
    (root / "app").mkdir()
    shutil.copy2(repo / "app/utf8.manifest", root / "app/utf8.manifest")
    (root / "src").mkdir()
    for name in ("platform.c", "platform.h"):
        shutil.copy2(repo / "src" / name, root / "src" / name)
    (root / "probe.c").write_text('''#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char **argv) {
    if (argc > 1 && argv[1][0] == 'u') {
        volatile int maximum = INT_MAX;
        printf("%d\\n", maximum + 1);
    } else {
        unsigned char *data = malloc(8);
        if (!data) return 2;
        volatile int index = argc > 1 ? 8 : 7;
        data[index] = 42;
        printf("%u\\n", data[index]);
        free(data);
    }
    return 0;
}
''', encoding="utf-8")
    (root / "module.c").write_text('''#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((visibility("default")))
#endif
int probe_globals[2] = {PROBE_VALUE, -5};
EXPORT int *probe_values(void) { return probe_globals; }
''', encoding="utf-8")
    (root / "reload.c").write_text('''#include "platform.h"
#include <stdio.h>
#include <string.h>
int main(int argc, char **argv) {
    if (argc < 3) return 2;
    for (int iteration = 0; iteration < 32; ++iteration) {
        for (int module = 1; module <= 2; ++module) {
            void *handle = ps_module_open(argv[module]);
            if (!handle) return 3;
            void *symbol = ps_module_symbol(handle, "probe_values");
            int *(*values_fn)(void) = NULL;
            _Static_assert(sizeof values_fn == sizeof symbol, "function pointer size");
            memcpy(&values_fn, &symbol, sizeof values_fn);
            if (!values_fn) return 4;
            int *values = values_fn();
            if (values[0] != (module == 1 ? 42 : 17) || values[1] != -5) return 5;
            values[0] += 1; /* Reload must restore initial state, not retain the image. */
            if (argc > 3 && iteration == 31 && module == 2) {
                volatile int index = 2;
                printf("%d\\n", values[index]);
            }
            ps_module_close(handle);
        }
    }
    puts("42");
    return 0;
}
''', encoding="utf-8")
    native.ROOT = root
    options = argparse.Namespace(build_dir=root / "build", compiler=args.compiler,
                                 no_app=True, config="Debug", jobs=2, sanitizers=False)
    env = native.compiler_environment()
    records = []

    def run(builder, program, arguments, expected):
        result = subprocess.run([str(program), *arguments], cwd=root, env=builder.env,
                                capture_output=True, timeout=30)
        record = dict(command=[str(program), *arguments], sanitizers=options.sanitizers,
                      exit_code=result.returncode, stdout=result.stdout.decode("utf-8", errors="replace"),
                      stderr=result.stderr.decode("utf-8", errors="replace"))
        records.append(record)
        (root / "results.json").write_text(json.dumps(records, indent=2) + "\n", encoding="utf-8")
        if expected:
            assert result.returncode != 0 and expected in record["stderr"], record
        else:
            assert result.returncode == 0 and record["stdout"].strip() == "42", record

    with native.build_lock(options.build_dir):
        builder = native.Builder(options, env)
        program = builder.executable("sanitizer-probe", ["probe.c"], [])
        run(builder, program, [], None)
        ordinary = program.read_bytes()
        options.sanitizers = True
        builder = native.Builder(options, env)
        program = builder.executable("sanitizer-probe", ["probe.c"], [])
        assert program.read_bytes() != ordinary, "Sanitizer flag reused an uninstrumented executable"
        run(builder, program, [], None)
        run(builder, program, ["address"], "AddressSanitizer: heap-buffer-overflow")
        if not native.WINDOWS:
            run(builder, program, ["undefined"], "runtime error: signed integer overflow")
        modules = [builder.executable("module-" + str(value), ["module.c"], [],
                                      defines=("PROBE_VALUE=" + str(value),), module=True)
                   for value in (42, 17)]
        reload_program = builder.executable("sanitizer-reload", ["reload.c", "src/platform.c"], [])
        if native.MAC:
            # Keep the actual Apple Clang registration/destructor output with CI evidence.
            native.run([builder.cc, "-S", "-fsanitize=address,undefined", "-DPROBE_VALUE=42",
                        str(root / "module.c"), "-o", str(root / "module.s")], builder.env)
        run(builder, reload_program, list(map(str, modules)), None)
        run(builder, reload_program, [*map(str, modules), "overflow"],
            "AddressSanitizer: global-buffer-overflow")
    print(f"Sanitizer instrumentation detected the injected errors; evidence: {root}")


if __name__ == "__main__":
    main()
