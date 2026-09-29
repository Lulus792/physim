"""Verify CI's per-application WER configuration with a real access violation."""
import argparse
import importlib.util
import json
from pathlib import Path
import shutil
import struct
import subprocess
import sys
import tempfile


def exception_code(dump):
    data = dump.read_bytes()
    if len(data) < 32 or data[:4] != b"MDMP":
        raise AssertionError(f"No valid minidump header: {dump}")
    streams, directory = struct.unpack_from("<II", data, 8)
    if directory + streams * 12 > len(data):
        raise AssertionError("Truncated minidump directory")
    for i in range(streams):
        kind, size, offset = struct.unpack_from("<III", data, directory + i * 12)
        if kind == 6:  # ExceptionStream: thread ID, alignment, MINIDUMP_EXCEPTION.
            if size < 12 or offset + size > len(data):
                raise AssertionError("Truncated minidump exception stream")
            return struct.unpack_from("<I", data, offset + 8)[0]
    return None


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--work", type=Path, required=True)
    parser.add_argument("--dumps", type=Path, required=True)
    args = parser.parse_args()
    if sys.platform != "win32":
        parser.error("This probe requires Windows and a configured per-application WER dump folder")
    repo = Path(__file__).resolve().parent.parent
    spec = importlib.util.spec_from_file_location("crash_probe_build", repo / "tools/build.py")
    native = importlib.util.module_from_spec(spec)
    sys.dont_write_bytecode = True
    spec.loader.exec_module(native)
    args.work.mkdir(parents=True, exist_ok=True)
    root = Path(tempfile.mkdtemp(prefix="crash dump probe ä ", dir=args.work)).resolve()
    (root / "app").mkdir()
    shutil.copy2(repo / "app/utf8.manifest", root / "app/utf8.manifest")
    (root / "probe.c").write_text('''#include <windows.h>
int main(void) {
    SetErrorMode(SEM_FAILCRITICALERRORS);
    volatile int *address = (volatile int *)0;
    *address = 42;
    return 0;
}
''', encoding="utf-8")
    native.ROOT = root
    options = argparse.Namespace(build_dir=root / "build", compiler="cl",
                                 config="Debug", no_app=True, jobs=1)
    env = native.compiler_environment()
    with native.build_lock(options.build_dir):
        builder = native.Builder(options, env)
        program = builder.executable("physim-crash-probe", ["probe.c"], [])
    with subprocess.Popen([str(program)], cwd=root, env=env,
                          stdout=subprocess.PIPE, stderr=subprocess.PIPE) as process:
        try:
            stdout, stderr = process.communicate(timeout=30)
        except subprocess.TimeoutExpired:
            process.kill()
            process.communicate()
            raise
        record = {"command": str(program), "pid": process.pid, "exit_code": process.returncode,
                  "stdout": stdout.decode("utf-8", errors="replace"),
                  "stderr": stderr.decode("utf-8", errors="replace")}
    (root / "results.json").write_text(json.dumps(record, indent=2) + "\n", encoding="utf-8")
    if process.returncode != 0xC0000005:
        raise AssertionError(f"Expected a Windows access violation: {record}")
    dump = args.dumps.resolve() / f"physim-crash-probe.exe.{process.pid}.dmp"
    exception = exception_code(dump)
    if exception != 0xC0000005:
        raise AssertionError(f"Minidump did not record the access violation: {exception}")
    record["dump"] = str(dump)
    record["exception"] = f"0x{exception:08X}"
    (root / "results.json").write_text(json.dumps(record, indent=2) + "\n", encoding="utf-8")
    print(f"Windows crash capture verified: {dump}")


if __name__ == "__main__":
    main()
