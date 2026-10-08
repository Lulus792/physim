"""Independent exact rational transforms and Decimal normalized inverse-transpose."""

import argparse
import json
import math
import random
import subprocess
from decimal import Decimal, localcontext
from fractions import Fraction
from pathlib import Path


def make_case(mode, matrix, vector, name):
    return dict(
        mode=mode, matrix=[x.hex() for x in matrix], vector=[x.hex() for x in vector], name=name
    )


def cases():
    rng = random.Random(357)
    result = []
    for index in range(900):
        matrix = [0.0] * 16
        matrix[15] = 1.0
        for row in range(3):
            for col in range(4):
                matrix[col * 4 + row] = math.ldexp(
                    rng.choice([-1.0, 1.0]) * rng.uniform(0.5, 1), rng.randint(-1074, 1023)
                )
        vector = [
            math.ldexp(rng.choice([-1.0, 1.0]) * rng.uniform(0.5, 1), rng.randint(-1074, 1023))
            for _ in range(3)
        ]
        mode = index % 2
        if mode == 0 and index % 4 == 0:
            for col in range(4):
                matrix[col * 4 + 3] = math.ldexp(
                    rng.choice([-1.0, 1.0]) * rng.uniform(0.5, 1), rng.randint(-1074, 1023)
                )
        result.append(make_case(mode, matrix, vector, "random-" + str(index)))
    tiny = math.ulp(0.0)
    maximum = float.fromhex("0x1.fffffffffffffp1023")
    for value in [
        1.0,
        math.nextafter(1.0, 0.0),
        math.nextafter(1.0, math.inf),
        1.5,
        math.nextafter(1.5, 0.0),
        math.nextafter(1.5, math.inf),
    ]:
        for exponent in [-1074, -1073, -1072, -1024, -1023, -1022, -1021, -1020]:
            for weight in [
                0.5,
                math.nextafter(0.5, 0.0),
                math.nextafter(0.5, math.inf),
                1.0,
                math.nextafter(1.0, 0.0),
            ]:
                matrix = [0.0] * 16
                matrix[0] = weight
                matrix[4] = tiny
                matrix[15] = 1.0
                result.append(
                    make_case(
                        1,
                        matrix,
                        [math.ldexp(value, exponent), math.ldexp(1.0, -53), 0.0],
                        "rounding-" + str(len(result)),
                    )
                )
    for sign in [-1.0, 1.0]:
        for small in [tiny, math.ldexp(1.0, -1022), 1.0, maximum]:
            matrix = [0.0] * 16
            matrix[0] = maximum
            matrix[4] = sign * small
            matrix[8] = -maximum
            matrix[5] = matrix[10] = matrix[15] = 1.0
            result.append(make_case(1, matrix, [1.0, 1.0, 1.0], "cancellation-" + str(len(result))))
    for exponent in [-1074, -1022, -500, 0, 500, 1023]:
        for sign in [-1.0, 1.0]:
            matrix = [0.0] * 16
            matrix[0] = sign * math.ldexp(1.0, exponent)
            matrix[5] = matrix[10] = matrix[15] = 1.0
            for vector in [
                [1.0, 0.0, 0.0],
                [math.ldexp(1.0, exponent), 1.0, 0.0],
                [1.0, -2.0, 3.0],
            ]:
                result.append(make_case(2, matrix, vector, "normal-scale-" + str(len(result))))
    for index in range(100):
        matrix = [0.0] * 16
        matrix[15] = 1.0
        # A = diagonal(row scales) * a well-conditioned triangular shear.
        scales = [math.ldexp(rng.choice([-1.0, 1.0]), rng.randint(-1074, 1023)) for _ in range(3)]
        for row in range(3):
            for col in range(row, 3):
                matrix[col * 4 + row] = scales[row] * (
                    1.0 if row == col else rng.choice([-0.25, 0.25, 0.5])
                )
        vector = [rng.uniform(-2, 2) for _ in range(3)]
        result.append(make_case(2, matrix, vector, "normal-shear-" + str(index)))
    matrix = [0.0] * 16
    matrix[0] = matrix[5] = matrix[10] = 1.0
    result.append(make_case(0, matrix, [1.0, 2.0, 3.0], "zero-w"))
    matrix[15] = 1.0
    matrix[0] = 0.0
    result.append(make_case(2, matrix, [1.0, 0.0, 0.0], "singular-normal"))
    matrix[0] = maximum
    result.append(make_case(1, matrix, [2.0, 0.0, 0.0], "true-overflow"))
    matrix[15] = maximum
    result.append(make_case(0, matrix, [2.0, 0.0, 0.0], "finite-projective-quotient"))
    return result


def reference(case):
    matrix = list(map(lambda x: Fraction(float.fromhex(x)), case["matrix"]))
    vector = list(map(lambda x: Fraction(float.fromhex(x)), case["vector"]))
    if case["mode"] in [0, 1]:
        values = vector + [Fraction(case["mode"] == 0)]
        sums = [sum(matrix[col * 4 + row] * values[col] for col in range(4)) for row in range(4)]
        if case["mode"] == 0:
            if not sums[3]:
                return 8, None
            sums = [x / sums[3] for x in sums[:3]]
        else:
            sums = sums[:3]
        try:
            answer = list(map(float, sums))
        except OverflowError:
            return 10, None
        if not all(map(math.isfinite, answer)):
            return 10, None
        return 0, answer
    # Solve the transposed 3x3 system over rationals, independently of the C solver.
    rows = [[matrix[row * 4 + col] for col in range(3)] + [vector[row]] for row in range(3)]
    for col in range(3):
        pivot = next((row for row in range(col, 3) if rows[row][col]), None)
        if pivot is None:
            return 8, None
        rows[col], rows[pivot] = rows[pivot], rows[col]
        for row in range(col + 1, 3):
            factor = rows[row][col] / rows[col][col]
            for j in range(col, 4):
                rows[row][j] -= factor * rows[col][j]
    solution = [Fraction(0)] * 3
    for row in range(2, -1, -1):
        solution[row] = (
            rows[row][3] - sum(rows[row][j] * solution[j] for j in range(row + 1, 3))
        ) / rows[row][row]
    with localcontext() as context:
        context.prec = 100
        values = [Decimal(x.numerator) / Decimal(x.denominator) for x in solution]
        length = sum(x * x for x in values).sqrt()
        return 0, [float(x / length) for x in values]


def literal(text):
    value = float.fromhex(text)
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


def selected(data):
    return (
        data[::25]
        + data[-4:]
        + [
            c
            for c in data
            if c["name"].startswith("normal-scale") or c["name"].startswith("cancellation")
        ]
    )


def language_source(data):
    source = "// Exact binary input fixtures; rational/Decimal references live in the independent oracle.\n"
    for index, case in enumerate(data):
        m = case["matrix"]
        v = case["vector"]
        source += (
            "let m"
            + str(index)
            + " = Mat4("
            + ",".join(
                "Vec4(" + ",".join(literal(x) for x in m[col * 4 : col * 4 + 4]) + ")"
                for col in range(4)
            )
            + ")\n"
        )
        source += "let v" + str(index) + " = Vec3(" + ",".join(map(literal, v)) + ")\n"
        name = ["transformPoint", "transformDirection", "transformNormal"][case["mode"]]
        source += (
            "let r"
            + str(index)
            + " = attempt(m"
            + str(index)
            + "."
            + name
            + "(v"
            + str(index)
            + "))\n"
        )
        source += (
            "if let value = r"
            + str(index)
            + ':\n    print("ok")\n    print(value.x)\n    print(value.y)\n    print(value.z)\nelse:\n    print("error")\n'
        )
    return source


def check(case, code, values):
    expected, reference_values = reference(case)
    assert code == expected, (case["name"], code, expected)
    if code:
        if values is not None:
            assert values == [7.0, 8.0, 9.0], (case["name"], "non-atomic error", values)
    elif case["mode"] in [0, 1]:
        assert values == reference_values, (
            case["name"],
            "incorrect rounded exact transform",
            values,
            reference_values,
        )
    else:
        assert all(math.isfinite(v) for v in values)
        assert max(abs(a - b) for a, b in zip(values, reference_values)) <= 2e-13, (
            case["name"],
            "normal direction",
            values,
            reference_values,
        )
        assert abs(math.hypot(*values) - 1) <= 2e-15, (case["name"], "normal length", values)


def run_c(program, data):
    text = "".join(str(c["mode"]) + " " + " ".join(c["matrix"] + c["vector"]) + "\n" for c in data)
    result = subprocess.run([program], input=text, text=True, capture_output=True, timeout=120)
    assert result.returncode == 0, result.stderr
    lines = result.stdout.splitlines()
    assert len(lines) == len(data)
    for case, line in zip(data, lines):
        fields = line.split()
        assert len(fields) == 4
        check(case, int(fields[0]), [float.fromhex(x) for x in fields[1:]])


def main():
    parser = argparse.ArgumentParser()
    for name in ["c", "language", "fixture"]:
        parser.add_argument("--" + name, required=True)
    args = parser.parse_args()
    data = cases()
    subset = selected(data)
    assert Path(args.fixture).read_text() == language_source(subset)
    run_c(args.c, data)
    result = subprocess.run([args.language], capture_output=True, text=True, timeout=120)
    assert result.returncode == 0, result.stderr
    lines = iter(result.stdout.splitlines())
    for case in subset:
        label = next(lines)
        if label == "error":
            assert reference(case)[0] != 0, case["name"]
        else:
            assert label == "ok"
            check(case, 0, [float(next(lines)) for _ in range(3)])
    assert next(lines, None) is None
    print(
        f"Transforms: {len(data)} C and {len(subset)} Physim exact rational point/direction and Decimal normal references, extrema, ties, shears and atomic errors passed"
    )


if __name__ == "__main__":
    main()
