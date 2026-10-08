"""Actual migration button: legacy upgrade, stale snapshot and dirty guard."""

import argparse
import os
from pathlib import Path
import subprocess

p = argparse.ArgumentParser(description=__doc__)
p.add_argument("--app", type=Path, required=True)
p.add_argument("--work", type=Path, required=True)
a = p.parse_args()
a.work.mkdir(parents=True, exist_ok=True)
for size, noisy, mode in ((size, noisy, mode) for size in ("large", "small")
                         for noisy in (False, True) for mode in ("normal", "stale", "dirty")):
    root = a.work / ("noisy" if noisy else "quiet") / size / mode
    root.mkdir(parents=True)
    manifest = root / "physim.project"
    old = b"physim_project=1\n# retained\n"
    manifest.write_bytes(old)
    (root / "main.c").write_bytes(b"/* retained experiment */\n")
    (root / "analysis.c").write_bytes(b"/* retained analysis */\n")
    (root / "runs").mkdir()
    (root / "runs/keep.psrun").write_bytes(b"archive retained")
    environment = dict(os.environ)
    environment.pop("PHYSIM_TEST_SMALL", None)
    environment.pop("PHYSIM_TEST_MIGRATION_NOISE", None)
    if size == "small":
        environment["PHYSIM_TEST_SMALL"] = "1"
    if noisy:
        environment["PHYSIM_TEST_MIGRATION_NOISE"] = "1"
    r = subprocess.run(
        [str(a.app.resolve()), "--project-migration-test", str(root.resolve()), mode],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        timeout=45,
        env=environment,
    )
    (root / "app.stdout").write_bytes(r.stdout)
    (root / "app.stderr").write_bytes(r.stderr)
    assert r.returncode == 0 and b"PROJECT MIGRATION UI SELF-TEST: PASSED" in r.stdout, (
        (size, noisy, mode),
        r.stdout,
        r.stderr,
    )
    if mode == "normal":
        assert (
            manifest.read_bytes() == b"physim_project=2\nkind=experiment\n# retained\n"
            and (root / "physim.project.bak").read_bytes() == old
        )
    else:
        assert (
            manifest.read_bytes() == old + (b"# external change\n" if mode == "stale" else b"")
            and not (root / "physim.project.bak").exists()
        )
    assert (
        (root / "main.c").read_bytes() == b"/* retained experiment */\n"
        and (root / "analysis.c").read_bytes() == b"/* retained analysis */\n"
        and (root / "runs/keep.psrun").read_bytes() == b"archive retained"
    )
print(
    "Actual app project migration: twelve quiet/noisy cases at two window sizes, visible button, source/run preservation and stale/dirty guards passed"
)
