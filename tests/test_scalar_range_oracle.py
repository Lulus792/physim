"""Check public C/Physim search reports against exact binary rational tolerances."""

import argparse
import json
import math
import random
import subprocess
from fractions import Fraction
from pathlib import Path


def cases():
    rng = random.Random(792)
    result = []

    def add(method, mode, lo, hi, absolute, relative, scale, target, limit):
        if (
            lo == hi
            or absolute <= 0
            or not all(map(math.isfinite, [lo, hi, absolute, relative, scale, target]))
        ):
            return
        result.append(
            dict(
                method=method,
                mode=mode,
                lo=lo.hex(),
                hi=hi.hex(),
                absolute=absolute.hex(),
                relative=relative.hex(),
                scale=scale.hex(),
                target=target.hex(),
                limit=limit,
            )
        )

    for exponent in [-1070, -1022, -500, 0, 500, 1023]:
        scale = math.ldexp(1.0, exponent)
        for direction in [-1, 1]:
            for _ in range(100):
                center = direction * scale * (1 + rng.random() * 0.8)
                lo = hi = center
                for _ in range(1 + rng.randrange(20)):
                    lo = math.nextafter(lo, -math.inf)
                    hi = math.nextafter(hi, math.inf)
                absolute = max(math.ulp(center), scale * 1e-15)
                target = lo * 0.5 + hi * 0.5
                add(0, 1, lo, hi, absolute, 0.0, scale, target, 100)
                add(1, 2, lo, hi, absolute, 0.0, scale, target, 100)
        for _ in range(500):
            lo, hi = sorted([scale * rng.uniform(-1.8, 1.8), scale * rng.uniform(-1.8, 1.8)])
            width = (Fraction(hi) - Fraction(lo)) / 2
            absolute = math.nextafter(float(width), 0.0)
            add(1, 0, lo, hi, absolute, 0.0, scale, 0.0, 1)
            relative = rng.random() * 0.5
            b = (1 - 0.6180339887498948482) * lo + 0.6180339887498948482 * hi
            residual = width - Fraction(relative) * abs(Fraction(b))
            if residual > 0:
                absolute = math.nextafter(float(residual), 0.0)
                add(1, 0, lo, hi, absolute, relative, scale, 0.0, 1)
    largest = float.fromhex("0x1.fffffffffffffp1023")
    tiny = math.ulp(0.0)
    for relative in [0.0, tiny, 0.5, math.nextafter(1.0, 0.0)]:
        for lo, hi in [
            (-largest, largest),
            (0.0, math.ldexp(1.0, 1023)),
            (-27 * tiny, -19 * tiny),
            (-tiny, 2 * tiny),
        ]:
            for absolute in [tiny, 1.0, largest]:
                add(1, 0, lo, hi, absolute, relative, 1.0, 0.0, 100)
    return result


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
    # Include each explicit extreme plus samples of every scale/method/boundary.
    return data[::40] + data[-48:]


def language_source(data):
    text = "// Generated from the independent binary-input oracle; no solver-derived references.\n"
    for key in ["method", "mode", "limit"]:
        text += "let " + key + " = [" + ",".join(str(c[key]) for c in data) + "]\n"
    for key in ["lo", "hi", "absolute", "relative", "scale", "target"]:
        text += "let " + key + " = [" + ",".join(literal(c[key]) for c in data) + "]\n"
    text += """for index in 0..<method.count:
    let divisor = scale[index]
    let center = target[index]
    let kind = mode[index]
    let callback = func(x: Float64) -> Float64:
        if kind == 0:
            return 1.0
        let value = x / divisor - center / divisor
        if kind == 1:
            return value
        return abs(value)
    var result: ScalarResult? = nil
    if method[index] == 0:
        result = attempt(rootBisectReported(callback,lo[index],hi[index],absolute[index],relative[index],limit[index]))
    else:
        result = attempt(minimizeGoldenReported(callback,lo[index],hi[index],absolute[index],relative[index],limit[index]))
    if let report = result:
        print("ok")
        print(report.x)
        print(report.value)
        print(report.lower)
        print(report.upper)
        print(report.iterations)
        print(report.evaluations)
    else:
        print("error")
"""
    return text


def check(c, code, report, calls=None, outside=False):
    assert code in [0, 9], (c, code)
    assert not outside, (c, "callback escaped input interval")
    if calls is not None:
        assert report[5] == calls, (c, "evaluation counter", report, calls)
    x, value, lo, hi, iterations, evaluations = report
    assert all(map(math.isfinite, [x, value, lo, hi]))
    assert float.fromhex(c["lo"]) <= lo <= x <= hi <= float.fromhex(c["hi"]), (c, report)
    assert iterations <= c["limit"] and evaluations == iterations + 2
    if code == 0 and (c["method"] or value != 0):
        width = (Fraction(hi) - Fraction(lo)) / 2
        tolerance = Fraction(float.fromhex(c["absolute"])) + Fraction(
            float.fromhex(c["relative"])
        ) * abs(Fraction(x))
        assert width <= tolerance, (c, "false convergence", report, str(width), str(tolerance))
    if code == 0 and c["mode"] in [1, 2]:
        target = Fraction(float.fromhex(c["target"]))
        # The linear/absolute callbacks undergo double rounding; allow four
        # representable units around their known mathematical zero/minimum.
        margin = Fraction(math.ulp(x)) * 4 + Fraction(math.ulp(float.fromhex(c["target"]))) * 4
        assert Fraction(lo) - margin <= target <= Fraction(hi) + margin, (
            c,
            "lost known solution",
            report,
        )


def c_results(program, data):
    text = "".join(
        f"{c['method']} {c['mode']} "
        + " ".join(c[k] for k in ["lo", "hi", "absolute", "relative", "scale", "target"])
        + f" {c['limit']}\n"
        for c in data
    )
    result = subprocess.run([program], input=text, text=True, capture_output=True, timeout=60)
    assert result.returncode == 0, result.stderr
    lines = result.stdout.splitlines()
    assert len(lines) == len(data)
    records = []
    for c, line in zip(data, lines):
        fields = line.split()
        assert len(fields) == 11
        code = int(fields[0])
        report = [float.fromhex(v) for v in fields[1:5]] + [int(v) for v in fields[5:7]]
        check(c, code, report, int(fields[7]), bool(int(fields[8])))
        if c["method"] and c["mode"] == 0:
            initial_x = Fraction(float.fromhex(fields[10]))
            initial_width = (
                Fraction(float.fromhex(c["hi"])) - Fraction(float.fromhex(c["lo"]))
            ) / 2
            initial_tolerance = Fraction(float.fromhex(c["absolute"])) + Fraction(
                float.fromhex(c["relative"])
            ) * abs(initial_x)
            converged_initially = code == 0 and report[4] == 0
            assert converged_initially == (initial_width <= initial_tolerance), (
                c,
                "incorrect initial stopping decision",
                report,
            )
        records.append((code, report))
    return records


def main():
    parser = argparse.ArgumentParser()
    for name in ["c", "language", "fixture"]:
        parser.add_argument("--" + name, required=True)
    args = parser.parse_args()
    data = cases()
    subset = selected(data)
    assert Path(args.fixture).read_text() == language_source(subset)
    c_results(args.c, data)
    expected = c_results(args.c, subset)
    result = subprocess.run([args.language], text=True, capture_output=True, timeout=60)
    assert result.returncode == 0, result.stderr
    lines = iter(result.stdout.splitlines())
    for c, (code, reference) in zip(subset, expected):
        label = next(lines)
        if label == "error":
            assert code == 9, (c, code)
        else:
            assert label == "ok" and code == 0, (c, label, code)
            report = [float(next(lines)) for _ in range(4)] + [int(next(lines)) for _ in range(2)]
            check(c, 0, report)
            assert report == reference, (c, "C/Physim disagreement", report, reference)
    assert next(lines, None) is None
    print(
        f"Scalar search: {len(data)} C and {len(subset)} Physim reports, exact rational convergence, callback intervals/counts and known solutions passed"
    )


if __name__ == "__main__":
    main()
