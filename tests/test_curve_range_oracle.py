"""Exact rational references for cubic Bezier position, derivative and subdivision."""

import argparse
import math
import random
import subprocess
from fractions import Fraction
from pathlib import Path


def cases():
    rng = random.Random(359792)
    tiny = math.ulp(0.0)
    largest = float.fromhex("0x1.fffffffffffffp1023")
    result = []
    for p in [
        [0.0, tiny, 0.0, 0.0],
        [largest, largest, math.nextafter(largest, 0.0), math.nextafter(largest, 0.0)],
        [1.0, math.nextafter(1.0, 2.0), math.nextafter(1.0, 2.0), 1.0],
        [largest, -largest, -largest, largest],
        [tiny, -tiny, tiny, -tiny],
        [largest] * 4,
        [0.0, 1.0, 2.0, 3.0],
    ]:
        for t in [
            0.0,
            tiny,
            0.25,
            0.5,
            math.nextafter(0.5, 0.0),
            0.75,
            math.nextafter(1.0, 0.0),
            1.0,
        ]:
            result.append((t, [[x, -x, 0.0] for x in p]))
    for _ in range(600):
        t = rng.choice([rng.random(), tiny, 0.5, 0.0, 1.0, math.nextafter(1.0, 0.0)])
        points = [
            [math.ldexp(rng.uniform(-1.0, 1.0), rng.randrange(-1074, 1024)) for _ in range(3)]
            for _ in range(4)
        ]
        result.append((t, points))
    for _ in range(300):
        center = math.ldexp(rng.uniform(-1.0, 1.0), rng.randrange(-1022, 1024))
        points = []
        for i in range(4):
            center = math.nextafter(center, rng.choice([-math.inf, math.inf]))
            points.append([center, -center, center])
        result.append((rng.random(), points))
    return result


def rounded(value):
    try:
        return float(value)
    except OverflowError:
        return math.copysign(math.inf, 1 if value > 0 else -1)


def reference(row):
    t, points = row
    t = Fraction(t)
    u = 1 - t
    q = [[Fraction(v) for v in p] for p in points]

    def mix(a, b):
        return [u * x + t * y for x, y in zip(a, b)]

    a, b, c = [mix(q[i], q[i + 1]) for i in range(3)]
    d, e = mix(a, b), mix(b, c)
    p = mix(d, e)
    tangent = [3 * (y - x) for x, y in zip(d, e)]
    left, right = [q[0], a, d, p], [p, e, c, q[3]]
    values = list(map(rounded, p + tangent))
    status = 0 if all(map(math.isfinite, values)) else 10
    if status:
        values = [7.0, 8.0, 9.0, 10.0, 11.0, 12.0]
    split = [rounded(v) for side in [left, right] for point in side for v in point]
    # The API explicitly copies endpoints, including their signed zeros.
    if row[0] == 0:
        split = points[0] * 4 + [v for point in points for v in point]
        if not status:
            values[:3] = points[0]
    elif row[0] == 1:
        split = [v for point in points for v in point] + points[3] * 4
        if not status:
            values[:3] = points[3]
    else:
        split[:3] = points[0]
        split[-3:] = points[3]
    return status, values, split


def same_bits(actual, expected):
    return len(actual) == len(expected) and all(
        a.hex() == b.hex() for a, b in zip(actual, expected)
    )


def selected(data):
    return data[:56] + data[56::43]


def literal(value):
    if value == 0:
        return "-0.0" if math.copysign(1.0, value) < 0 else "0.0"
    m, e = math.frexp(abs(value))
    return (
        ("-" if value < 0 else "") + "(" + format(m * 2, ".17g") + " * pow(2," + str(e - 1) + "))"
    )


def fixture_text(data):
    lines = ["// Independent Fraction references use the exact binary inputs."]
    for i, (t, points) in enumerate(selected(data)):
        lines.append(
            f"let c{i} = Bezier3("
            + ",".join("Vec3(" + ",".join(map(literal, p)) + ")" for p in points)
            + ")"
        )
        lines.append(f"let p{i} = attempt(c{i}.position({literal(t)}))")
        lines.append(f"if let value = p{i}:")
        lines += [
            '    print("ok")',
            "    print(value.x)",
            "    print(value.y)",
            "    print(value.z)",
        ]
        lines.append(f"    let tangent = c{i}.tangent({literal(t)})")
        lines += [
            "    print(tangent.x)",
            "    print(tangent.y)",
            "    print(tangent.z)",
            "else:",
            '    print("error")',
        ]
        # Inspect copied control points independently of possibly overflowing tangents.
        for side, method in [("l", "splitLeft"), ("r", "splitRight")]:
            lines.append(f"let {side}{i} = c{i}.{method}({literal(t)})")
            for index in range(4):
                for axis in "xyz":
                    lines.append(f"print({side}{i}.controlPoint({index}).{axis})")
    # Keep each independent case in its own function so compiler/debug tracking
    # does not grow across thousands of live module variables in one main.
    bodies = []
    begin = 1
    for i in range(len(selected(data))):
        end = next(
            (j for j in range(begin + 1, len(lines)) if lines[j].startswith("let c")), len(lines)
        )
        body = lines[begin:end]
        if i == 0:
            body += [
                "if let badLow = attempt(c0.controlPoint(-1)):",
                "    assert(false)",
                "if let badHigh = attempt(c0.controlPoint(4)):",
                "    assert(false)",
            ]
        bodies += [f"func curveCase{i}():"] + ["    " + line for line in body] + [""]
        begin = end
    calls = [f"curveCase{i}()" for i in range(len(selected(data)))]
    return lines[0] + "\n" + "\n".join(bodies + calls) + "\n"


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
            " ".join(v.hex() for v in [t] + [v for p in points for v in p]) + "\n"
            for t, points in data
        )
        rows = subprocess.check_output(
            [str(args.c.resolve())], input=text, text=True, timeout=60
        ).splitlines()
        assert len(rows) == len(data)
        failures = []
        for i, (row, source) in enumerate(zip(rows, data)):
            tokens = row.split()
            status, values, split = reference(source)
            actual = list(map(float.fromhex, tokens[2:]))
            if (
                int(tokens[0]) != status
                or int(tokens[1]) != 0
                or not same_bits(actual, values + split)
            ):
                failures.append((i, source, tokens[:8], status))
        assert not failures, f"{len(failures)} wrong Bezier cases; first={failures[:2]}"
    if args.language:
        assert args.fixture.read_text(encoding="utf-8") == fixture_text(data)
        tokens = iter(
            subprocess.check_output(
                [str(args.language.resolve())], text=True, timeout=60
            ).splitlines()
        )
        for i, row in enumerate(selected(data)):
            status, values, split = reference(row)
            assert next(tokens) == ("error" if status else "ok"), i
            if not status:
                observed = [float(next(tokens)) for _ in range(6)]
                assert same_bits(observed, values), (i, observed, values)
            observed = [float(next(tokens)) for _ in range(24)]
            assert same_bits(observed, split), (i, observed, split)
        assert next(tokens, None) is None
    print(
        f"Bezier rational oracle: {len(data)} C cases and {len(selected(data))} Physim cases; exact position, tangent, split controls and atomic range errors"
    )


if __name__ == "__main__":
    main()
