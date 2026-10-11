"""Actual C/Physim CCD runs: analytic impacts, SI provenance and CRC parity."""

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
def read(path):
    rows, metadata = [], None
    footer = schema = False
    with path.open("rb") as f:
        assert f.read(16) == b"PSRUN17\n" + struct.pack("<II", 1, 0x01020304)
        while h := f.read(12):
            kind, size, crc = struct.unpack("<III", h)
            data = f.read(size)
            assert size <= 8192 and len(data) == size and zlib.crc32(data) == crc
            if kind == 1:
                metadata = dict(line.split("=", 1) for line in data.decode().splitlines() if "=" in line)
            if kind == 2:
                assert struct.unpack_from("<I", data)[0] == 11
                schema = True
            if kind == 3:
                rows.append(struct.unpack("<12d", data))
            if kind == 4:
                assert struct.unpack("<Q", data)[0] == 2
                footer = True
    assert schema and footer and len(rows) == 2 and metadata is not None
    return rows, metadata

for speed in (0, 2, 4, 10, 20, 100):
    runs = []
    for label, model in [("c", a.c_model), ("phys", a.phys_model)]:
        path = work / f"{label}-{speed}.psrun"
        r = subprocess.run([a.runner, model, str(path), "--steps", "1", "--dt", "1",
                            "--seed", "42", "--param", f"speed={speed}"], capture_output=True, timeout=60)
        assert r.returncode == 0, (r.returncode, r.stderr)
        rows, metadata = read(path)
        assert float(metadata['parameter.speed']) == speed == float(metadata['initial_speed_m_s'])
        assert float(metadata['parameter_default.speed']) == 10
        assert float(metadata['parameter_min.speed']) == 0 and float(metadata['parameter_max.speed']) == 100
        assert metadata.get('parameter_unit.speed') == 'm/s', metadata
        assert float(metadata['parameter_scale.speed']) == 1 and metadata['parameter_dimension.speed'] == '1,0,-1,0,0,0,0'
        assert float(metadata['dt_s']) == 1 and int(metadata['seed']) == 42
        assert rows[0][:7] == (0, -4, 0, 4, speed, 0, 0)
        # Independent equal-mass elastic transfer: impacts at 3/u and 6/u.
        if speed < 3:
            expected, events = [-4 + speed, 0, 4, speed, 0, 0], 0
        elif speed < 6:
            expected, events = [-1, speed - 3, 4, 0, speed, 0], 1
        else:
            expected, events = [-1, 3, speed - 2, 0, 0, speed], 2
        final = rows[1]
        assert final[0] == 1
        for actual, value in zip(final[1:7], expected):
            assert math.isclose(actual, value, abs_tol=1e-4), (label, speed, final)
        assert math.isclose(sum(final[4:7]), speed, abs_tol=1e-8)
        assert math.isclose(final[7], .5 * speed * speed, abs_tol=1e-8)
        assert final[8] == events and final[9] == events and final[10] < 1e-8 and final[11] < 1e-8
        runs.append(rows)
    for c, phys in zip(*runs):
        assert all(math.isclose(x, y, abs_tol=1e-10) for x, y in zip(c, phys)), (c, phys)

for label, model in [("c", a.c_model), ("phys", a.phys_model)]:
    for speed in (-.1, 100.1):
        path = work / f"invalid-{label}-{speed}.psrun"
        r = subprocess.run([a.runner, model, str(path), "--steps", "1", "--param", f"speed={speed}"],
                           capture_output=True, timeout=60)
        assert r.returncode == 5 and not path.exists(), (label, speed, r.returncode, r.stderr)
print("Actual C/Physim CCD: 12 complete runs, free flight/one/two analytic impacts, momentum/energy, SI provenance, CRC/footer parity and four preflight rejections passed")
