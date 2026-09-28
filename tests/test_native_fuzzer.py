"""Run a bounded IPC libFuzzer campaign, preserving seeds, findings and diagnostics."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess
import sys
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--bin", type=Path, required=True)
    parser.add_argument("--work", type=Path, required=True)
    args = parser.parse_args()
    suffix = ".exe" if sys.platform == "win32" else ""
    binary = args.bin.resolve()
    fuzzer = binary / ("physim-protocol-libfuzzer" + suffix)
    generator = binary / ("physim-protocol-seeds" + suffix)
    for program in (fuzzer, generator):
        if not program.is_file():
            parser.error(f"Missing program: {program}; build with tools/build.py --fuzzer first")
    args.work.mkdir(parents=True, exist_ok=True)
    root = Path(tempfile.mkdtemp(prefix="fuzzer run ä ", dir=args.work)).resolve()
    corpus = root / "corpus"
    artifacts = root / "artifacts"
    corpus.mkdir()
    artifacts.mkdir()
    report = dict(programs={p.name: hashlib.sha256(p.read_bytes()).hexdigest()
                            for p in (fuzzer, generator)}, steps=[])

    def save():
        (root / "results.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")

    def checked(command, work, timeout=30):
        record = dict(command=[str(p) for p in command], cwd=str(work))
        try:
            result = subprocess.run(record["command"], cwd=work, capture_output=True, timeout=timeout)
            record.update(exit_code=result.returncode,
                          stdout=result.stdout.decode("utf-8", errors="replace"),
                          stderr=result.stderr.decode("utf-8", errors="replace"),
                          status="passed" if result.returncode == 0 else "failed")
        except subprocess.TimeoutExpired as error:
            record.update(status="timeout", stdout=(error.stdout or b"").decode("utf-8", errors="replace"),
                          stderr=(error.stderr or b"").decode("utf-8", errors="replace"))
        report["steps"].append(record)
        save()
        if record["status"] != "passed":
            raise RuntimeError(f"Fuzzer check failed; see {root}\n{record['stdout']}\n{record['stderr']}")
        return record

    checked([generator, "--write-seeds"], corpus)
    seeds = [corpus / "snapshot.seed", corpus / "frame.seed"]
    assert all(seed.is_file() and seed.stat().st_size > 1 for seed in seeds), "Missing valid decoder seeds"
    report["seeds"] = {seed.name: dict(size=seed.stat().st_size,
                                     sha256=hashlib.sha256(seed.read_bytes()).hexdigest()) for seed in seeds}
    # Both the standalone harness and libFuzzer must accept each valid seed.
    for seed in seeds:
        checked([generator, seed], root)
        checked([fuzzer, seed, "-runs=1"], root)
    campaign = checked([fuzzer, corpus, "-runs=10000", "-seed=42", "-max_len=8213", "-timeout=10",
                        "-print_final_stats=1", "-artifact_prefix=" + artifacts.as_posix() + "/"], root, 120)
    output = campaign["stderr"]
    coverage = [int(value) for value in re.findall(r"\bcov: (\d+)", output)]
    report["coverage"] = coverage
    report["completed"] = bool(re.search(r"Done 10000 runs", output))
    report["findings"] = [p.name for p in artifacts.iterdir()]
    report["passed"] = (report["completed"] and len(coverage) > 1 and max(coverage) > coverage[0]
                        and not report["findings"])
    save()
    assert report["passed"], f"Campaign did not complete, add coverage or remain free of findings; see {root}"
    print(f"libFuzzer: 10000 runs passed, coverage {coverage[0]} -> {max(coverage)}; evidence: {root}")


if __name__ == "__main__":
    main()
