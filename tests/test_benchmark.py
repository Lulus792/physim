"""Exercise evidence preservation and comparison failures with the real workload."""
import json
import hashlib
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile


driver = Path(sys.argv[1]).resolve()
executable = Path(sys.argv[2]).resolve()
workspace = Path.cwd().resolve()
with tempfile.TemporaryDirectory(prefix="benchmark-driver-", dir=workspace) as directory:
    root = Path(directory).resolve()
    assert root.is_relative_to(workspace)

    def invoke(output, *extra, expected=0):
        result = subprocess.run([sys.executable, str(driver), str(executable),
                                 "--samples", "257", "--repeats", "1",
                                 "--output", str(root / output), *extra],
                                capture_output=True, text=True, timeout=30)
        assert result.returncode == expected, result.stdout + result.stderr
        return result

    invoke("baseline")
    metadata_path = root / "baseline" / "metadata.json"
    original = metadata_path.read_bytes()
    metadata = json.loads(original)
    sources = metadata["working_tree_source_sha256"]
    for name in ("tools/build.py", "tools/benchmark_build.h"):
        assert sources[name] == hashlib.sha256((driver.parent.parent / name).read_bytes()).hexdigest()
    baseline = json.loads((root / "baseline/summary.json").read_text())
    for item in baseline.values():
        assert item["configuration"] in ("", "Debug", "Release", "RelWithDebInfo", "MinSizeRel")
        assert item["compiler"].startswith(("MSVC-", "ClangCL-", "Clang-", "AppleClang-", "GCC-"))
    invoke("baseline", expected=1)
    assert metadata_path.read_bytes() == original
    invoke("invalid", "--samples", "1", expected=2)
    assert not (root / "invalid").exists()
    invoke("candidate", "--baseline", str(root / "baseline"), "--max-regression", "1000000")
    comparison = json.loads((root / "candidate" / "comparison.json").read_text())
    assert len(comparison) == 8 and all(item["passed"] for item in comparison.values())
    summary_path = root / "baseline" / "summary.json"
    summary = json.loads(summary_path.read_text())
    for value in summary.values():
        value["median_seconds"] = 1e-15
    summary_path.write_text(json.dumps(summary))
    result = invoke("regression", "--baseline", str(root / "baseline"), expected=1)
    assert "Performance threshold exceeded" in result.stderr
    assert (root / "regression" / "raw.csv").stat().st_size > 0
    metadata = json.loads(original)
    metadata["samples"] = 999
    metadata_path.write_text(json.dumps(metadata))
    result = invoke("incompatible", "--baseline", str(root / "baseline"), expected=1)
    assert "Incompatible baseline: samples" in result.stderr
    # The driver must work in a source tree without any CMake configuration.
    independent = root / "without cmake"
    (independent / "tools").mkdir(parents=True)
    for name in ("benchmark.py", "build.py", "benchmark.c", "benchmark_build.h"):
        shutil.copy2(driver.parent / name, independent / "tools" / name)
    driver = independent / "tools/benchmark.py"
    invoke("without-cmake")
    independent_meta = json.loads((root / "without-cmake/metadata.json").read_text())
    assert "CMakeLists.txt" not in independent_meta["working_tree_source_sha256"]
    assert "tools/build.py" in independent_meta["working_tree_source_sha256"]
print("Benchmark driver: evidence, validation and regression gate verified")
