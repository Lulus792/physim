"""Check tolerance decisions against exact rational binary64 values."""

import argparse
import math
import random
import subprocess
from fractions import Fraction
from pathlib import Path


def cases():
    rng = random.Random(358792)
    result = []
    tiny = math.ulp(0.0)
    largest = float.fromhex("0x1.fffffffffffffp1023")
    for a, b in [
        (0.0, tiny),
        (-tiny, tiny),
        (largest, -largest),
        (1.0, math.nextafter(1.0, 2.0)),
        (1.0, -tiny),
        (0.0, -0.0),
    ]:
        for absolute in [0.0, tiny, 1.0, largest]:
            for relative in [0.0, tiny, 0.5, 1.0, math.nextafter(1.0, 0.0), 2.0, largest]:
                result.append((a, b, absolute, relative))
    for _ in range(1500):
        exponent = rng.randrange(-1074, 1024)
        a = math.ldexp(rng.uniform(-1.0, 1.0), exponent)
        b = math.ldexp(rng.uniform(-1.0, 1.0), exponent)
        if a == b:
            continue
        width = abs(Fraction(a) - Fraction(b))
        scale = max(abs(Fraction(a)), abs(Fraction(b)))
        relative = rng.random() * float(min(Fraction(2), width / scale))
        residual = width - Fraction(relative) * scale
        if residual >= 0 and residual <= Fraction(largest):
            boundary = float(residual)
            for absolute in [
                boundary,
                math.nextafter(boundary, 0.0),
                math.nextafter(boundary, math.inf),
            ]:
                if math.isfinite(absolute):
                    result.append((a, b, absolute, relative))
        if width / scale <= Fraction(largest):
            boundary = float(width / scale)
            for relative in [
                boundary,
                math.nextafter(boundary, 0.0),
                math.nextafter(boundary, math.inf),
            ]:
                result.append((a, b, 0.0, relative))
    for invalid in [math.inf, -math.inf, math.nan]:
        for index in range(4):
            row = [1.0, 1.0, 0.0, 0.0]
            row[index] = invalid
            result.append(tuple(row))
    result += [(1.0, 1.0, -tiny, 0.0), (1.0, 1.0, 0.0, -tiny)]
    return result


def expected(row):
    a, b, absolute, relative = row
    return (
        all(map(math.isfinite, row))
        and absolute >= 0
        and relative >= 0
        and abs(Fraction(a) - Fraction(b))
        <= Fraction(absolute) + Fraction(relative) * max(abs(Fraction(a)), abs(Fraction(b)))
    )


def selected(data):
    finite = [row for row in data if all(map(math.isfinite, row))]
    return finite[:168] + finite[168::97] + finite[-2:]


def literal(value):
    if value == 0:
        return "0.0"
    mantissa, exponent = math.frexp(abs(value))
    return (
        ("-" if value < 0 else "")
        + "("
        + format(mantissa * 2, ".17g")
        + " * pow(2,"
        + str(exponent - 1)
        + "))"
    )


def fixture_text(data):
    return (
        "// Exact binary tolerance inputs; expected decisions use independent Fraction arithmetic.\n"
        + "".join("print(isClose(" + ",".join(map(literal, row)) + "))\n" for row in selected(data))
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
        result = subprocess.run(
            [str(args.c.resolve())],
            input="".join(" ".join(v.hex() for v in row) + "\n" for row in data),
            text=True,
            capture_output=True,
            check=True,
            timeout=60,
        )
        observed = result.stdout.splitlines()
        assert len(observed) == len(data)
        failures = [
            (i, row, observed[i], expected(row))
            for i, row in enumerate(data)
            if observed[i] != f"{int(expected(row))} {int(expected(row))}"
        ]
        assert not failures, f"{len(failures)} incorrect tolerance decisions; first={failures[:3]}"
    if args.language:
        assert args.fixture and args.fixture.read_text(encoding="utf-8") == fixture_text(data)
        result = subprocess.run(
            [str(args.language.resolve())], text=True, capture_output=True, check=True, timeout=60
        )
        assert result.stdout.splitlines() == [str(expected(row)).lower() for row in selected(data)]
    print(
        f"Exact tolerance decisions: {len(data)} C cases, {len(selected(data))} Physim cases; boundaries, subnormals, overflow, symmetry and invalid inputs"
    )


if __name__ == "__main__":
    main()
