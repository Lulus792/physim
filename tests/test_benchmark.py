"""Exercise evidence preservation and comparison failures with the real workload."""
import json
import hashlib
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import csv
import math
import importlib.util
import io
from unittest.mock import patch


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
    assert metadata["schema"] == 2 and "excludes children" in metadata["resource_scope"]
    rows = list(csv.DictReader((root / "baseline/raw.csv").open()))
    peaks = [int(row["peak_resident_bytes"]) for row in rows]
    assert min(peaks) > 0 and peaks == sorted(peaks)
    file_sizes = [baseline[name]["io_bytes"] for name in
          ("write_16_channels", "read_validate_16_channels", "analysis_snapshot")]
    # The logical file includes 257 time samples, sixteen values each, plus framing.
    assert file_sizes[0] == file_sizes[1] == file_sizes[2] and file_sizes[0] > 257 * 17 * 8
    for row in rows:
        assert math.isfinite(float(row["user_cpu_seconds"])) and float(row["user_cpu_seconds"]) >= 0
        assert math.isfinite(float(row["system_cpu_seconds"])) and float(row["system_cpu_seconds"]) >= 0
        if int(row["io_bytes"]):
            assert math.isclose(float(row["bytes_per_second"]),
                                int(row["io_bytes"]) / float(row["seconds"]), rel_tol=1e-4)
        else:
            assert float(row["bytes_per_second"]) == 0
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
    metadata = json.loads(original)
    metadata["schema"] = 1
    metadata_path.write_text(json.dumps(metadata))
    result = invoke("legacy-baseline", "--baseline", str(root / "baseline"), expected=1)
    assert "Incompatible baseline: schema" in result.stderr
    # Inject corrupted native records into the real driver. A successful child
    # exit alone must not publish a summary containing NaN CPU or invalid memory.
    spec = importlib.util.spec_from_file_location("resource_benchmark_driver", driver)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    native_run = subprocess.run
    for fault in ("nan-cpu", "decreasing-peak", "duplicate-repeat", "missing-workload"):
        corrupted = [dict(row) for row in rows]
        if fault == "nan-cpu":
            corrupted[0]["user_cpu_seconds"] = "nan"
        elif fault == "decreasing-peak":
            corrupted[1]["peak_resident_bytes"] = "1"
        elif fault == "duplicate-repeat":
            corrupted[0]["repeat"] = "1"
        else:
            corrupted.pop()
        buffer = io.StringIO()
        writer = csv.DictWriter(buffer, fieldnames=rows[0].keys())
        writer.writeheader()
        writer.writerows(corrupted)

        def fake_native(command, **kwargs):
            if command[0] != str(executable):
                return native_run(command, **kwargs)
            kwargs["stdout"].write(buffer.getvalue().encode())
            return subprocess.CompletedProcess(command, 0)

        target = root / fault
        with patch.object(sys, "argv", [str(driver), str(executable), "--samples", "257",
                                       "--repeats", "1", "--output", str(target)]), \
                patch.object(module.subprocess, "run", fake_native):
            try:
                module.main()
            except RuntimeError:
                pass
            else:
                raise AssertionError("Corrupted measurements accepted: " + fault)
        assert (target / "raw.csv").is_file() and not (target / "summary.json").exists()
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
