"""Compare every exported energy row with the original independently decoded run."""
import csv
import math
from pathlib import Path
import struct
import sys
import zlib

run, exported = map(Path, sys.argv[1:])
rows = []
with run.open("rb") as f:
    assert f.read(16) == b"PSRUN17\n" + struct.pack("<II", 1, 0x01020304)
    names = []
    finished = False
    while header := f.read(12):
        kind, size, crc = struct.unpack("<III", header)
        data = f.read(size)
        assert len(data) == size and zlib.crc32(data) == crc
        if kind == 2:
            count = struct.unpack_from("<I", data)[0]
            assert size == 4 + 167 * count
            names = [data[4 + i * 167:52 + i * 167].split(b"\0")[0].decode()
                     for i in range(count)]
        elif kind == 3:
            values = struct.unpack("<" + "d" * (len(names) + 1), data)
            rows.append((values[0], values[names.index("energy") + 1]))
        elif kind == 4:
            assert struct.unpack("<Q", data)[0] == len(rows)
            finished = True
assert finished and rows
with exported.open(newline="", encoding="utf-8") as f:
    values = list(csv.reader(f))
assert values[0] == ["time [s]", "energy [J]", "affine(energy) [J]"]
assert len(values) == len(rows) + 1
initial = rows[0][1]
for (time, energy), row in zip(rows, values[1:]):
    actual = list(map(float, row))
    assert len(actual) == 3 and all(math.isfinite(x) for x in actual)
    assert actual[:2] == [time, energy]
    assert abs(actual[2] - (energy - initial)) < 1e-12 * max(1, abs(energy - initial))
print(f"Energy CSV: all {len(rows)} original time/energy/change rows passed")
