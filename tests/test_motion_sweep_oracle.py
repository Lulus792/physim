"""Analytic first-entry references for full-turn rods and quadratic flight."""

import argparse
import math
import random
import subprocess
from pathlib import Path


def cases():
    rng = random.Random(387793)
    data = []
    for _ in range(60):
        l = rng.uniform(1.0, 4.0)
        w = l * rng.uniform(0.015, 0.06)
        h = l * rng.uniform(0.35, 0.7)
        r = w * rng.uniform(1.0, 2.0)
        angle = rng.choice([math.pi, 2 * math.pi, 4 * math.pi, 12 * math.pi])
        for mode in [0, 1, 2]:
            data.append((mode, l, w, h, angle, 0.0, 0.0, r, 1e-7, 8192))
    for _ in range(40):
        l = rng.uniform(1.0, 4.0)
        w = 0.05 * l
        h = rng.uniform(0.5, 2.0)
        k = 8 * (h - w)
        # Identical start/end positions but an interior plane crossing.
        data.append((0, l, w, h, 0.0, -k, k, 0.1, 1e-7, 8192))
    for mode in [0, 1, 2]:
        for angle in [0.0, math.pi, 2 * math.pi, 20 * math.pi]:
            data.append((mode, 2.0, 0.1, 5.0, angle, 0.0, 0.0, 0.1, 1e-7, 8192))
    data += [
        (0, 2.0, 0.1, 1.5, math.pi, 0.0, 0.0, 0.1, 1e-8, 1),
        (0, 2.0, 0.1, 1.5, math.pi, 0.0, 0.0, 0.1, -1.0, 10),
    ]
    return data


def entry(row, envelope):
    mode, l, w, h, angle, linear, quadratic, r, tol, it = row
    if it == 1:
        return "unresolved", 0.0
    if tol <= 0:
        return "invalid", 0.0
    if angle == 0:
        if quadratic:
            # h-w-envelope + linear*t + quadratic*t² = 0.
            d = linear * linear - 4 * quadratic * (h - w - envelope)
            if d < 0:
                return "clear", 0.0
            return "hit", (-linear - math.sqrt(d)) / (2 * quadratic)
        return "clear", 0.0
    if mode == 0:
        if h - envelope > math.hypot(l, w):
            return "clear", 0.0
        theta = math.asin((h - envelope) / math.hypot(l, w)) - math.atan2(w, l)
    elif mode == 1:
        if h > math.hypot(l, w) + r + envelope:
            return "clear", 0.0
        theta = math.acos((w + r + envelope) / h)
    else:
        if h > math.hypot(l, w) + math.sqrt(2) * r + envelope:
            return "clear", 0.0
        amplitude = math.hypot(h - r, r)
        theta = math.acos((w + envelope) / amplitude) - math.atan2(r, h - r)
    t = theta / angle
    return ("hit", t) if t <= 1 else ("clear", 0.0)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--probe", type=Path, required=True)
    args = parser.parse_args()
    data = cases()
    text = "".join(
        str(row[0]) + " " + " ".join(v.hex() for v in row[1:-1]) + " " + str(row[-1]) + "\n"
        for row in data
    )
    rows = subprocess.check_output(
        [str(args.probe.resolve())], input=text, text=True, timeout=120
    ).splitlines()
    assert len(rows) == len(data)
    hits = 0
    for number, (row, line) in enumerate(zip(data, rows)):
        expected, physical = entry(row, 0.0)
        _, early = entry(row, row[-2])
        tokens = line.split()
        status = int(tokens[0])
        hit = bool(int(tokens[1]))
        v = list(map(float.fromhex, tokens[2:]))
        fraction, depth = v[:2]
        normal = v[2:5]
        point = v[5:8]
        if expected == "invalid":
            assert status == 1 and hit and fraction == 0.75
            continue
        if expected == "unresolved":
            assert status == 9 and hit and fraction == 0.75, (number, line)
            continue
        assert status == 0, (number, row, line, expected)
        assert hit == (expected == "hit"), (number, row, line, expected)
        if not hit:
            continue
        hits += 1
        assert early - 2e-10 <= fraction <= physical + 2e-10, (number, fraction, early, physical)
        assert math.isclose(math.hypot(*normal), 1, abs_tol=1e-10) and depth == 0
        mode, l, w, h, angle, linear, quadratic, r, tol, it = row
        theta = fraction * angle
        if mode == 0:
            gap = (
                h
                + linear * fraction
                + quadratic * fraction * fraction
                - l * abs(math.sin(theta))
                - w * abs(math.cos(theta))
            )
            assert -0.1 * tol <= gap <= 1.01 * tol, (number, gap, tol)
            assert abs(point[1]) <= tol and normal[1] < -0.999999
        elif mode == 1:
            assert abs(math.hypot(point[0], point[1] - h, point[2]) - r) <= tol
            local_y = -point[0] * math.sin(theta) + point[1] * math.cos(theta)
            assert abs(local_y - w) <= tol, (number, point, local_y, w)
        else:
            # The first witness is the target's bottom-right corner; the rod's
            # upper long face meets that corner while its ends remain farther away.
            assert abs(point[0] - r) <= tol and abs(point[1] - (h - r)) <= tol, (
                number,
                point,
                r,
                h,
            )
    print(
        f"Motion analytic oracle: {len(data)} cases, {hits} first contacts; plane/rod/sphere, full turns, quadratic interior crossing, clear paths and unresolved/invalid atomic results"
    )


if __name__ == "__main__":
    main()
