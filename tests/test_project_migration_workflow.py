"""Actual migrated C/Physim projects: builds, samples, bytes and incremental cache."""

import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import struct
import subprocess
import zlib

p = argparse.ArgumentParser(description=__doc__)
for name in ("builder", "sdk", "compiler", "runner", "work"):
    p.add_argument("--" + name, type=Path, required=True)
p.add_argument("--cc")
a = p.parse_args()
a.work.mkdir(parents=True, exist_ok=True)
module = ".dll" if os.name == "nt" else ".so"


def command(args, ok=True):
    r = subprocess.run(
        list(map(str, args)), stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=180
    )
    assert (r.returncode == 0) == ok, (args, r.returncode, r.stdout, r.stderr)
    return r.stdout.decode()


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def samples(path):
    result = []
    with path.open("rb") as f:
        assert f.read(16) == b"PSRUN17\n" + struct.pack("<II", 1, 0x01020304)
        while h := f.read(12):
            kind, size, crc = struct.unpack("<III", h)
            data = f.read(size)
            assert len(data) == size and zlib.crc32(data) == crc
            if kind in (3, 4):
                result.append((kind, data))
    assert result and result[-1][0] == 4
    return result


checks = []
for lang in ("c", "phys"):
    root = a.work / ("Legacy " + lang + " α")
    root.mkdir()
    src = "main.c" if lang == "c" else "main.phys"
    analysis = "analysis.c" if lang == "c" else "analysis.phys"
    shutil.copy2(
        a.sdk / ("examples/pendulum/main.c" if lang == "c" else "examples/language/pendulum.phys"),
        root / src,
    )
    shutil.copy2(
        a.sdk
        / ("examples/pendulum/analysis.c" if lang == "c" else "examples/language/analysis.phys"),
        root / analysis,
    )
    if lang == "c":
        (root / analysis).write_text(
            (root / analysis)
            .read_text()
            .replace("__DATE__", '"Oct 08 2026"')
            .replace("__TIME__", '"12:00:00"')
        )
    (root / "runs").mkdir()
    (root / "runs/retained.psrun").write_bytes(b"preserved archived bytes")
    original = f"physim_project=1\r\n# Legacy α\r\nexperiment={src}\r\nanalysis={analysis}\r\nprofile=Release\r\nextension.keep=owner\r\nsimulation.seed=42\r\n".encode()
    manifest = root / "physim.project"
    manifest.write_bytes(original)
    protected = {
        q.relative_to(root).as_posix(): digest(q)
        for q in root.rglob("*")
        if q.is_file() and q != manifest
    }
    build = [
        a.builder,
        "--project",
        root,
        "--sdk",
        a.sdk,
        "--output",
        root / "build/Release",
        "--physimc",
        a.compiler,
    ]
    if a.cc:
        build += ["--cc", a.cc]
    command(build)
    model = root / ("build/Release/experiment" + module)
    first = a.work / (lang + "-before.psrun")
    command([a.runner, model, first, "--steps", "20", "--dt", ".01"])
    cached = {
        q.relative_to(root / "build").as_posix(): digest(q)
        for q in (root / "build").rglob("*")
        if q.is_file()
    }
    output = command([a.builder, "--migrate-project", "--project", root])
    assert "1 -> 2: migrated" in output
    expected = original.replace(
        b"physim_project=1\r\n", b"physim_project=2\r\nkind=experiment\r\n", 1
    )
    assert (
        manifest.read_bytes() == expected and (root / "physim.project.bak").read_bytes() == original
    )
    assert all(digest(root / name) == h for name, h in protected.items())
    output = command(build)
    assert "Compile:" not in output and "Link:" not in output, output
    assert cached == {
        q.relative_to(root / "build").as_posix(): digest(q)
        for q in (root / "build").rglob("*")
        if q.is_file()
    }
    second = a.work / (lang + "-after.psrun")
    command([a.runner, model, second, "--steps", "20", "--dt", ".01"])
    assert samples(first) == samples(second)
    output = command([a.builder, "--project", root, "--migrate-project"])
    assert "already current" in output and (root / "physim.project.bak").read_bytes() == original
    assert manifest.read_bytes() == expected and all(
        digest(root / name) == h for name, h in protected.items()
    )
    checks.append(lang + " source/run/cache preservation and identical measurements")
    bad = a.work / ("Future " + lang)
    bad.mkdir()
    (bad / "physim.project").write_bytes(b"physim_project=99\nkind=experiment\n")
    command([a.builder, "--migrate-project", "--project", bad], False)
    assert (
        bad / "physim.project"
    ).read_bytes() == b"physim_project=99\nkind=experiment\n" and not (
        bad / "physim.project.bak"
    ).exists()
(root.parent / "results.json").write_text(json.dumps(checks, indent=2) + "\n")
print(
    "Actual project migration: C/Physim native builds, exact manifests/backups, independent CRC/samples, retained sources/runs/cache, no-op and future-version rejection passed"
)
