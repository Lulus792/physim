"""Exact binary rational unit conversion, range statuses and output preservation."""

import argparse
import math
import random
import subprocess
from fractions import Fraction
from pathlib import Path


def cases():
    rng = random.Random(373792)
    data = []
    for _ in range(2000):
        v = math.ldexp(rng.uniform(-1, 1), rng.randrange(-1074, 1024))
        scale = math.ldexp(rng.uniform(0.5, 1), rng.randrange(-1073, 1024))
        data.append((0, v, scale, scale))
    for _ in range(2000):
        v = math.ldexp(rng.uniform(-1, 1), rng.randrange(-1074, 1024))
        a = math.ldexp(rng.uniform(0.5, 1), rng.randrange(-1073, 1024))
        b = math.ldexp(rng.uniform(0.5, 1), rng.randrange(-1073, 1024))
        data.extend([(0, v, a, b), (0, v, 1.0, b), (0, v, a, 1.0)])
    tiny = math.ulp(0.0)
    largest = float.fromhex("0x1.fffffffffffffp1023")
    for v in [0.0, -0.0, tiny, -tiny, 1.0, -1.0, largest, -largest]:
        for a in [tiny, 0.5, 1.0, 2.0, largest]:
            for b in [tiny, 0.5, 1.0, 2.0, largest]:
                data.append((0, v, a, b))
    for value in [math.inf, -math.inf, math.nan, 0.0, -1.0]:
        data += [(0, 1.0, value, 1.0), (0, 1.0, 1.0, value)]
    data += [
        (0, math.inf, 1.0, 1.0),
        (0, math.nan, 1.0, 1.0),
        (1, 1.0, 1.0, 1.0),
        (2, 1.0, 1.0, 1.0),
    ]
    return data


def reference(row):
    mode, value, a, b = row
    if mode or not all(map(math.isfinite, [value, a, b])) or a <= 0 or b <= 0:
        return 1, 77.0, 88.0
    if value == 0:
        return 0, value, value
    exact = Fraction(value) * Fraction(a) / Fraction(b)
    try:
        converted = float(exact)
    except OverflowError:
        return 10, 77.0, 88.0
    if not math.isfinite(converted) or converted == 0:
        return 10, 77.0, 88.0
    return 0, converted, converted


def selected(data):
    return data[:2000:137] + data[2000:8000:127] + data[8000:8200] + [data[-2]]


def literal(value):
    if value == 0:
        return "-0.0" if math.copysign(1.0, value) < 0 else "0.0"
    m, e = math.frexp(abs(value))
    return (
        ("-" if value < 0 else "") + "(" + format(m * 2, ".17g") + " * pow(2," + str(e - 1) + "))"
    )


def fixture_text(data):
    lines = ["// Exact binary inputs, with independent Fraction references."]
    for i, (mode, value, a, b) in enumerate(selected(data)):
        target_dimension = "0,0,1" if mode else "1,0,0"
        lines += [
            f"func conversionCase{i}():",
            f'    let a = Unit(1,0,0,0,0,0,0,{literal(a)},"from")',
            f'    let b = Unit({target_dimension},0,0,0,0,{literal(b)},"to")',
            "    func dynamicTarget() -> Unit:",
            "        return b",
            f"    let converted = attempt(a.convert({literal(value)},dynamicTarget()))",
            "    if let result = converted:",
            '        print("ok")',
            "        print(result)",
            "    else:",
            '        print("error")',
            f"    let quantity = attempt(Quantity({literal(value)},a).converted(dynamicTarget()))",
            "    if let result = quantity:",
            '        print("ok")',
            "        print(result.value)",
            "    else:",
            '        print("error")',
            "",
        ]
    lines += [f"conversionCase{i}()" for i in range(len(selected(data)))]
    return "\n".join(lines) + "\n"


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
            str(mode) + " " + " ".join(v.hex() for v in [value, a, b]) + "\n"
            for mode, value, a, b in data
        )
        rows = subprocess.check_output(
            [str(args.c.resolve())], input=text, text=True, timeout=60
        ).splitlines()
        assert len(rows) == len(data)
        failures = []
        for i, (source, row) in enumerate(zip(data, rows)):
            status, value, qvalue = reference(source)
            tokens = row.split()
            if (
                int(tokens[0]) != status
                or int(tokens[2]) != status
                or float.fromhex(tokens[1]).hex() != value.hex()
                or float.fromhex(tokens[3]).hex() != qvalue.hex()
            ):
                failures.append((i, source, row, (status, value.hex())))
        assert not failures, f"{len(failures)} wrong conversions; first={failures[:2]}"
    if args.language:
        assert args.fixture.read_text(encoding="utf-8") == fixture_text(data)
        tokens = iter(
            subprocess.check_output(
                [str(args.language.resolve())], text=True, timeout=60
            ).splitlines()
        )
        for i, row in enumerate(selected(data)):
            status, value, qvalue = reference(row)
            for expected in [value, qvalue]:
                assert next(tokens) == ("error" if status else "ok"), i
                if not status:
                    actual = float(next(tokens))
                    assert actual.hex() == expected.hex(), (i, actual.hex(), expected.hex())
        assert next(tokens, None) is None
    print(
        f"Unit conversion oracle: {len(data)} C cases, {len(selected(data))} Physim cases; exact rounding, identities, signed zero, ranges, dimensions, aliases and atomic errors"
    )


if __name__ == "__main__":
    main()
