"""Independently compare actual medium-app exports with every original run row."""
import csv
import hashlib
import math
from pathlib import Path
import struct
import sys
import xml.etree.ElementTree as ET
import zlib


def near(a, b):
    return math.isfinite(a) and math.isfinite(b) and abs(a-b) < 2e-12*max(1, abs(a), abs(b))


def png(path):
    data = path.read_bytes()
    assert data[:8] == b"\x89PNG\r\n\x1a\n"
    at, compressed, width, height, ended = 8, [], 0, 0, False
    while at < len(data):
        size = struct.unpack_from(">I", data, at)[0]
        kind, payload = data[at+4:at+8], data[at+8:at+8+size]
        assert zlib.crc32(kind+payload) == struct.unpack_from(">I", data, at+8+size)[0]
        if kind == b"IHDR":
            width, height, *encoding = struct.unpack(">IIBBBBB", payload)
            assert encoding == [8, 2, 0, 0, 0]
        elif kind == b"IDAT":
            compressed.append(payload)
        elif kind == b"IEND":
            assert size == 0
            ended = True
        at += size+12
    assert ended and at == len(data) and width == 2400 and height == 1700
    pixels = zlib.decompress(b"".join(compressed))
    stride = width*3+1
    assert len(pixels) == stride*height and all(pixels[y*stride] == 0 for y in range(height))
    assert len(set(pixels)) > 16


def check(project):
    runs = list((project/"runs").glob("*.psrun"))
    assert len(runs) == 1
    run = runs[0]
    digest = hashlib.sha256(run.read_bytes()).digest()
    rows, header, metadata, finished = [], [], {}, False
    names = ["angle", "angular_velocity", "position.x", "position.y", "energy",
             "sensor.angle", "velocity.x", "velocity.y", "speed"]
    dimensions = [(0,0,0), (0,0,-1), (1,0,0), (1,0,0), (2,1,-2),
                  (0,0,0), (1,0,-1), (1,0,-1), (1,0,-1)]
    with run.open("rb") as file:
        assert file.read(16) == b"PSRUN17\n"+struct.pack("<II",1,0x01020304)
        while chunk := file.read(12):
            kind, size, crc = struct.unpack("<III",chunk)
            data = file.read(size)
            assert len(data) == size and zlib.crc32(data) == crc
            if kind == 1:
                metadata = dict(line.split("=",1) for line in data.decode().splitlines() if "=" in line)
            elif kind == 2:
                assert struct.unpack_from("<I",data)[0] == 9 and size == 4+9*167
                header = ["time [s]"]
                for i, (name, dimension) in enumerate(zip(names, dimensions)):
                    at = 4+i*167
                    assert data[at:at+48].split(b"\0")[0].decode() == name
                    unit = data[at+48:at+64].split(b"\0")[0].decode()
                    assert struct.unpack_from("<7b",data,at+160) == dimension+(0,0,0,0)
                    if i >= 6:
                        assert unit == "m/s"
                    header.append(f"{name} [{unit}]")
            elif kind == 3:
                rows.append(struct.unpack("<10d",data))
            elif kind == 4:
                assert struct.unpack("<Q",data)[0] == len(rows)
                finished = True
    assert finished and len(rows) >= 3001 and rows[-1][0] >= 6
    assert metadata["integrator"] == "RK4" and metadata["parameter.integrator"] == "2"
    assert float(metadata["parameter.mass"]) == 2 and float(metadata["parameter.airDensity"]) == 1.225
    assert float(metadata["parameter.sensorNoise"]) == .02
    for t, angle, omega, x, y, energy, sensed, vx, vy, speed in rows:
        assert near(x,1.5*math.sin(angle)) and near(y,-1.5*math.cos(angle))
        assert near(vx,1.5*math.cos(angle)*omega) and near(vy,1.5*math.sin(angle)*omega)
        assert speed >= 0 and near(speed,math.hypot(vx,vy))
        assert near(energy,speed*speed+2*9.80665*(y+1.5))
    assert rows[-1][5] < rows[0][5]*.99
    noise = [r[6]-r[1] for r in rows]
    mean = sum(noise)/len(noise)
    assert abs(mean) < .002 and .017 < math.sqrt(sum((n-mean)**2 for n in noise)/(len(noise)-1)) < .023
    exports = {}
    for path in (project/"runs").glob("*.csv"):
        with path.open(newline="",encoding="utf-8") as file:
            exports[path] = list(csv.reader(file))
    raw = [values for values in exports.values() if values[0] == header]
    assert len(raw) == 1 and len(raw[0]) == len(rows)+1
    assert [tuple(map(float,row)) for row in raw[0][1:]] == rows
    energy_exports = [values for path,values in exports.items()
                      if path.name.endswith("-energy.csv") or "-pendulum_" in path.name]
    assert len(energy_exports) == 1 and len(energy_exports[0]) == len(rows)+1
    columns = len(energy_exports[0][0])
    assert columns in (3,4)
    for line, row in zip(energy_exports[0][1:],rows):
        expected = (row[0],row[5],row[5]-rows[0][5]) if columns == 3 else (row[0],row[1],row[5],row[5]-rows[0][5])
        assert len(line) == columns and all(near(float(a),b) for a,b in zip(line,expected))
    peaks = [rows[i] for i in range(1,len(rows)-1)
             if rows[i-1][1] < rows[i][1] > rows[i+1][1] and rows[i][1] > 0]
    assert len(peaks) >= 2
    decrements = [math.log(a[1])-math.log(b[1]) for a,b in zip(peaks,peaks[1:])]
    rates = [delta/(b[0]-a[0]) for delta,a,b in zip(decrements,peaks,peaks[1:])]
    peak_exports = [values for path,values in exports.items() if "-peaks_" in path.name]
    interval_exports = [values for path,values in exports.items() if "-decay_" in path.name]
    assert len(peak_exports) == len(interval_exports) == 1
    assert [tuple(map(float,row)) for row in peak_exports[0][1:]] == [(r[0],r[1],0) for r in peaks]
    assert len(interval_exports[0]) == len(rates)+1
    for row, a, b, delta, rate in zip(interval_exports[0][1:],peaks,peaks[1:],decrements,rates):
        assert all(near(float(x),y) for x,y in zip(row,(a[0],b[0],a[1],b[1],delta,rate,0))) and len(row) == 7
    table = [values for values in exports.values() if values[0][0] == "row"]
    assert len(table) == 1 and len(table[0]) == 2 and len(table[0][0]) == 6
    assert table[0][0][1:3] == ["Peak intervals [1]", "Mean log decrement [1]"]
    assert all(label.endswith("[Hz]") for label in table[0][0][3:])
    expected = (len(rates),sum(decrements)/len(decrements),sum(rates)/len(rates),min(rates),max(rates))
    assert all(near(float(a),b) for a,b in zip(table[0][1][1:],expected)) and min(rates) > 0
    svgs, images = list((project/"runs").glob("*-diagramm.svg")), list((project/"runs").glob("*-diagramm.png"))
    assert len(svgs) == len(images) == 1
    svg = ET.fromstring(svgs[0].read_bytes())
    text = " ".join(svg.itertext())
    assert "Mechanische Energieänderung" in text or "Energy change" in text
    assert "J" in text and svg.tag.endswith("svg")
    png(images[0])
    assert hashlib.sha256(run.read_bytes()).digest() == digest
    print(f"Medium app exports: all {len(rows)} raw SI and energy rows, {len(peaks)} peaks, {len(rates)} decay intervals, summary, SVG and decoded PNG passed")


if __name__ == "__main__":
    assert len(sys.argv) == 2
    check(Path(sys.argv[1]))
