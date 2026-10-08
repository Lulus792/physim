"""Independent rational inertias and identity-frame kinetic energy references."""

import argparse
import math
import random
import subprocess
from fractions import Fraction
from decimal import Decimal, localcontext
from pathlib import Path


def cases():
    rng = random.Random(382792)
    largest = float.fromhex("0x1.fffffffffffffp1023")
    tiny = math.ulp(0.0)
    data = []
    for mass, size in [
        (1e-300, 1e200),
        (tiny, 1e160),
        (1e300, 1e-150),
        (tiny, 1.0),
        (largest, largest),
        (0.0, largest),
    ]:
        for mode in [0, 1]:
            data.append((mode, mass, [size] * 3, [0.0] * 3, [0.0] * 3))
    for _ in range(500):
        mass = math.ldexp(rng.uniform(0.5, 1), rng.randrange(-1073, 1024))
        sizes = [math.ldexp(rng.uniform(0.5, 1), rng.randrange(-1073, 1024)) for _ in range(3)]
        data.append((rng.randrange(2), mass, sizes, [0.0] * 3, [0.0] * 3))
    for _ in range(500):
        mass = math.ldexp(rng.uniform(0.5, 1), rng.randrange(-1073, 1024))
        sizes = [math.ldexp(rng.uniform(0.5, 1), rng.randrange(-1073, 1024)) for _ in range(3)]
        v = [math.ldexp(rng.uniform(-1, 1), rng.randrange(-1074, 1024)) for _ in range(3)]
        w = [math.ldexp(rng.uniform(-1, 1), rng.randrange(-1074, 1024)) for _ in range(3)]
        data.append((rng.randrange(2), mass, sizes, v, w))
    for mode in [0, 1]:
        for mass in [0.0, tiny, 1e-300, 1.0]:
            for speed in [tiny, 1e-150, 1e150, largest]:
                data.append((mode, mass, [1.0] * 3, [speed, 0.0, 0.0], [0.0] * 3))
    data += [
        (0, -1.0, [1.0] * 3, [0.0] * 3, [0.0] * 3),
        (1, 1.0, [0.0, 1.0, 1.0], [0.0] * 3, [0.0] * 3),
    ]
    return data


def rounded(value):
    try:
        return float(value)
    except OverflowError:
        return math.inf


def reference(row):
    mode, mass, sizes, v, w = row
    if mass < 0 or min(sizes) <= 0:
        return 1, [0.0] * 3, 1, 99.0
    m = Fraction(mass)
    s = list(map(Fraction, sizes))
    inertia = (
        [rounded(Fraction(2, 5) * m * s[0] * s[0])] * 3
        if mode == 0
        else [
            rounded(m * (s[1] * s[1] + s[2] * s[2]) / 12),
            rounded(m * (s[0] * s[0] + s[2] * s[2]) / 12),
            rounded(m * (s[0] * s[0] + s[1] * s[1]) / 12),
        ]
    )
    if mass and any(not math.isfinite(x) or x <= 0 for x in inertia):
        return 10, [0.0] * 3, 10, 99.0
    if mass == 0 and any(x != 0 for x in v + w):
        return 0, inertia, 1, 99.0
    energy = rounded(
        (
            sum(m * Fraction(x) * Fraction(x) for x in v)
            + sum(Fraction(i) * Fraction(x) * Fraction(x) for i, x in zip(inertia, w))
        )
        / 2
    )
    return 0, inertia, 0 if math.isfinite(energy) else 10, energy if math.isfinite(energy) else 99.0


def selected(data):
    return data[:12] + data[12::29]


def literal(value):
    if value == 0:
        return "-0.0" if math.copysign(1.0, value) < 0 else "0.0"
    m, e = math.frexp(abs(value))
    return (
        ("-" if value < 0 else "") + "(" + format(m * 2, ".17g") + " * pow(2," + str(e - 1) + "))"
    )


def vec(values):
    return "Vec3(" + ",".join(map(literal, values)) + ")"


def fixture_text(data):
    lines = [
        "// Rational homogeneous solid-body references; identity principal axes.",
        "func bodyEnergy(value: Body, velocity: Vec3, angular: Vec3) -> Float64:",
        "    var body = value",
        "    body.setState(Vec3(0,0,0),velocity,Quat(0,0,0,1),angular)",
        "    return body.kineticEnergy()",
        "",
    ]
    for i, (mode, mass, sizes, v, w) in enumerate(selected(data)):
        constructor = (
            f"Body.sphere({literal(mass)},{literal(sizes[0])})"
            if mode == 0
            else f"Body.box({literal(mass)},{vec(sizes)})"
        )
        lines += [
            f"func bodyCase{i}():",
            f"    let candidate = attempt({constructor})",
            "    if let value = candidate:",
            '        print("body")',
            "        print(value.inertia.x)",
            "        print(value.inertia.y)",
            "        print(value.inertia.z)",
            f"        let result = attempt(bodyEnergy(value,{vec(v)},{vec(w)}))",
            "        if let value = result:",
            '            print("energy")',
            "            print(value)",
            "        else:",
            '            print("error")',
            "    else:",
            '        print("error")',
            "",
        ]
    lines += [f"bodyCase{i}()" for i in range(len(selected(data)))]
    return "\n".join(lines) + "\n"


def check_rotated(probe):
    """Independent 120-digit quaternion matrix; no production rotation helper."""
    rng = random.Random(382793)
    tiny = math.ulp(0.0)
    q = [0.0, 0.0, math.sin(math.pi / 8), math.cos(math.pi / 8)]
    data = [
        (1.0, [tiny] * 3, [0.0] * 3, [1.6e308, 1.6e308, 0.0], q),
        (1e-300, [1e-300, 2e-300, 3e-300], [1e200, 0.0, 0.0], [1e200, -1e200, 1e200], q),
        (1.0, [1.0] * 3, [0.0] * 3, [1e308, 1e308, 0.0], q),
        (1.0, [tiny] * 3, [0.0] * 3, [tiny] * 3, q),
    ]
    for _ in range(256):
        q = [rng.uniform(-1, 1) for _ in range(4)]
        length = math.hypot(*q)
        q = [x / length for x in q]
        # Bounded principal-axis condition number keeps the comparison meaningful
        # even when a rotated component nearly cancels.
        inertia = math.ldexp(rng.uniform(0.5, 1.0), rng.randrange(-1073, 1024))
        axes = [inertia * rng.uniform(0.5, 1.0) for _ in range(3)]
        axes = [max(tiny, x) for x in axes]
        speed = math.ldexp(0.75, rng.randrange(-1073, 1024))
        angular = [speed * rng.uniform(-1, 1) for _ in range(3)]
        data.append((1.0, axes, [0.0] * 3, angular, q))
    text = "".join(
        "2 " + " ".join(x.hex() for x in [m] + i + v + w + q) + "\n" for m, i, v, w, q in data
    )
    rows = subprocess.check_output(
        [str(probe.resolve())], input=text, text=True, timeout=60
    ).splitlines()
    assert len(rows) == len(data)
    for number, ((mass, inertia, velocity, angular, quat), row) in enumerate(zip(data, rows)):
        with localcontext() as ctx:
            ctx.prec = 120
            d = Decimal.from_float
            x, y, z, w = map(d, quat)
            n = x * x + y * y + z * z + w * w
            # Transpose of body-to-world rotation, expressed without square roots.
            matrix = [
                [1 - 2 * (y * y + z * z) / n, 2 * (x * y + z * w) / n, 2 * (x * z - y * w) / n],
                [2 * (x * y - z * w) / n, 1 - 2 * (x * x + z * z) / n, 2 * (y * z + x * w) / n],
                [2 * (x * z + y * w) / n, 2 * (y * z - x * w) / n, 1 - 2 * (x * x + y * y) / n],
            ]
            local = [sum(a * d(b) for a, b in zip(axis, angular)) for axis in matrix]
            exact = (
                d(mass) * sum(d(a) ** 2 for a in velocity)
                + sum(d(i) * a * a for i, a in zip(inertia, local))
            ) / 2
            expected = float(exact)
        tokens = row.split()
        status, energy = int(tokens[4]), float.fromhex(tokens[5])
        if math.isinf(expected):
            assert status == 10 and energy == 99, (number, row)
        else:
            assert status == 0 and math.isclose(
                energy, expected, rel_tol=2e-14, abs_tol=2 * tiny
            ), (number, row, expected)
    print(
        f"Rotated body energy: {len(data)} independent Decimal references; isotropic/anisotropic axes, scaled range, overflow and atomic errors"
    )


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--c", type=Path)
    parser.add_argument("--language", type=Path)
    parser.add_argument("--fixture", type=Path)
    parser.add_argument("--write-fixture", type=Path)
    args = parser.parse_args()
    data = cases()
    if args.write_fixture:
        args.write_fixture.write_text(fixture_text(data), encoding="utf-8")
    if args.c:
        text = "".join(
            str(mode) + " " + " ".join(x.hex() for x in [mass] + sizes + v + w) + "\n"
            for mode, mass, sizes, v, w in data
        )
        rows = subprocess.check_output(
            [str(args.c.resolve())], input=text, text=True, timeout=60
        ).splitlines()
        assert len(rows) == len(data)
        failures = []
        for i, (source, line) in enumerate(zip(data, rows)):
            status, inertia, es, energy = reference(source)
            row = line.split()
            if (
                int(row[0]) != status
                or int(row[4]) != es
                or [float.fromhex(x).hex() for x in row[1:4]] != [x.hex() for x in inertia]
                or float.fromhex(row[5]).hex() != energy.hex()
            ):
                failures.append((i, source, line, (status, inertia, es, energy)))
        assert not failures, f"{len(failures)} wrong body cases; first={failures[:2]}"
    if args.c:
        check_rotated(args.c)
    if args.language:
        assert args.fixture.read_text(encoding="utf-8") == fixture_text(data)
        tokens = iter(
            subprocess.check_output(
                [str(args.language.resolve())], text=True, timeout=60
            ).splitlines()
        )
        for i, row in enumerate(selected(data)):
            status, inertia, es, energy = reference(row)
            assert next(tokens) == ("error" if status else "body"), i
            if not status:
                assert [float(next(tokens)).hex() for _ in range(3)] == [
                    x.hex() for x in inertia
                ], i
                assert next(tokens) == ("error" if es else "energy"), i
                if not es:
                    assert float(next(tokens)).hex() == energy.hex(), i
        assert next(tokens, None) is None
    print(
        f"Body rational oracle: {len(data)} C cases, {len(selected(data))} Physim cases; solid sphere/box inertia, identity-frame energy, range statuses and atomic errors"
    )


if __name__ == "__main__":
    main()
