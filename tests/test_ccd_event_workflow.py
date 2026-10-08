"""Actual C/Physim runs: two impacts inside one dt plus sample/CRC parity."""

import argparse
import math
import struct
import subprocess
import zlib
from pathlib import Path

p = argparse.ArgumentParser()
for name in ("runner", "c-model", "phys-model", "work"):
    p.add_argument("--" + name, required=True)
a = p.parse_args()
work = Path(a.work)
work.mkdir(parents=True, exist_ok=True)
runs = []
for label, model in [("c", a.c_model), ("phys", a.phys_model)]:
    path = work / (label + ".psrun")
    r = subprocess.run(
        [a.runner, model, str(path), "--steps", "1", "--dt", "1"], capture_output=True, timeout=60
    )
    assert r.returncode == 0, (r.returncode, r.stderr)
    rows = []
    footer = False
    schema = False
    with path.open("rb") as f:
        assert f.read(16) == b"PSRUN17\n" + struct.pack("<II", 1, 0x01020304)
        while h := f.read(12):
            kind, size, crc = struct.unpack("<III", h)
            data = f.read(size)
            assert len(data) == size and zlib.crc32(data) == crc
            if kind == 2:
                assert struct.unpack_from("<I", data)[0] == 11
                schema = True
            if kind == 3:
                rows.append(struct.unpack("<12d", data))
            if kind == 4:
                assert struct.unpack("<Q", data)[0] == 2
                footer = True
    assert schema and footer and len(rows) == 2
    assert rows[0][:7] == (0, -4, 0, 4, 10, 0, 0)
    final = rows[1]
    assert final[0] == 1
    for actual, expected in zip(final[1:7], [-1, 3, 8, 0, 0, 10]):
        assert math.isclose(actual, expected, abs_tol=1e-4), (label, final)
    assert math.isclose(final[7], 50, abs_tol=1e-8) and final[8] == 2 and final[9] == 2
    assert final[10] < 1e-8 and final[11] < 1e-8
    runs.append(rows)
for c, phys in zip(*runs):
    assert all(math.isclose(x, y, abs_tol=1e-10) for x, y in zip(c, phys)), (c, phys)
print(
    "Actual C/Physim CCD runs: two sequential events within one dt, conserved energy, full-step positions, CRC/footer and channel parity passed"
)
