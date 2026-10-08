"""Independent stored-run checks of selectable pendulum methods from an SDK."""
import math
from pathlib import Path
import struct
import subprocess
import sys
import zlib

runner, analysis, c_analysis, phys_analysis, probe, work, *models = sys.argv[1:]
work = Path(work)
work.mkdir()
methods = ["Euler", "symplectic Euler", "RK4", "velocity Verlet", "Dormand-Prince 5(4)"]
assert len(models) == 6


def command(args, success=True):
    result = subprocess.run(list(map(str, args)), capture_output=True, timeout=90)
    assert (result.returncode == 0) == success, (args, result.returncode, result.stderr)
    return result


def read(path):
    rows, metadata, footer = [], {}, False
    names = ["angle", "angular_velocity", "position.x", "position.y", "energy",
             "sensor.angle", "velocity.x", "velocity.y", "speed"]
    with path.open("rb") as file:
        assert file.read(16) == b"PSRUN17\n" + struct.pack("<II", 1, 0x01020304)
        while header := file.read(12):
            kind, size, crc = struct.unpack("<III", header)
            data = file.read(size)
            assert len(data) == size and zlib.crc32(data) == crc
            if kind == 1:
                metadata = dict(line.split("=", 1) for line in data.decode().splitlines() if "=" in line)
            elif kind == 2:
                assert struct.unpack_from("<I", data)[0] == 9 and size == 4 + 9 * 167
                for i, name in enumerate(names):
                    at = 4 + i * 167
                    assert data[at:at+48].split(b"\0")[0].decode() == name
                    if i >= 6:
                        assert data[at+48:at+64].split(b"\0")[0] == b"m/s"
                        assert struct.unpack_from("<7b", data, at+160) == (1, 0, -1, 0, 0, 0, 0)
            elif kind == 3:
                rows.append(struct.unpack("<10d", data))
            elif kind == 4:
                assert struct.unpack("<Q", data)[0] == len(rows)
                footer = True
    assert footer and rows
    return metadata, rows


def near(a, b):
    return math.isfinite(a) and math.isfinite(b) and abs(a-b) <= 5e-10 * max(1, abs(a), abs(b))


references = {}
for model_index, model in enumerate(models):
    for method in range(5):
        for density in (0, 1.225):
            path = work / f"run-{model_index}-{method}-{density}.psrun"
            args = [runner, model, path, "--steps", "200", "--dt", ".005", "--seed", "42",
                    "--param", f"integrator={method}", "--param", "length=1.2",
                    "--param", "initialAngle=.6", "--param", "mass=2",
                    "--param", f"airDensity={density}", "--param", "dragCoefficient=.8",
                    "--param", "area=.08", "--param", "sensorNoise=.02"]
            if method == 3 and density:
                result = command(args, False)
                assert b"Velocity Verlet requires zero velocity-dependent drag" in result.stderr
                continue
            command(args)
            metadata, rows = read(path)
            assert metadata["integrator"] == methods[method] and metadata["parameter.integrator"] == str(method)
            assert metadata["parameter_dimension.integrator"] == "0,0,0,0,0,0,0"
            assert len(rows) == 201
            for i, (time, angle, omega, x, y, energy, sensor, vx, vy, speed) in enumerate(rows):
                assert time == i*.005
                assert near(x, 1.2*math.sin(angle)) and near(y, -1.2*math.cos(angle))
                assert near(vx, 1.2*math.cos(angle)*omega) and near(vy, 1.2*math.sin(angle)*omega)
                assert speed >= 0 and near(speed, 1.2*abs(omega)) and near(speed, math.hypot(vx, vy))
                assert near(x*vx+y*vy, 0) and near(energy, speed*speed+2*9.80665*(y+1.2))
            if method < 2:
                omega = -.005*9.80665/1.2*math.sin(.6)
                assert near(rows[1][2], omega) and near(rows[1][1], .6 if method == 0 else .6+.005*omega)
            if model_index == 0:
                references[method, density] = rows
            else:
                assert all(all(near(a, b) for a, b in zip(left, right))
                           for left, right in zip(rows, references[method, density]))
    for bad in (-1, 1.5, 5):
        result = command([runner, model, work/f"invalid-{model_index}-{bad}.psrun",
                          "--steps", "1", "--param", f"integrator={bad}"], False)
        if bad == 1.5:
            assert b"integer" in result.stderr
    path = work/f"adaptive-{model_index}.psrun"
    command([runner, model, path, "--steps", "500", "--dt", ".005", "--adaptive",
             "--min-dt", "1e-8", "--max-dt", ".1"])
    for label, module, mode in (("c", c_analysis, "adaptive"),
                                ("phys", phys_analysis, "adaptive-language")):
        prefix = work/f"adaptive-report-{model_index}-{label}"
        command([analysis, module, path, prefix])
        command([probe, path, str(prefix)+".psreport", mode])

print("SDK pendulum: six models, five selectable methods, nine stored SI channels, medium parity, rejected choices and twelve adaptive reports passed")
