"""Independent time-interval projections and distance minimization/root references."""

import argparse
import math
import random
import subprocess
from fractions import Fraction
from pathlib import Path
from test_convex_oracle import (
    BOX,
    BT,
    TETRA,
    OCTA,
    hull,
    world,
    features,
    add,
    sub,
    mul,
    dot,
    cross,
    unit,
    encode,
)


def interval_reference(a, b, da, db):
    va, ta, pa, qa = a
    vb, tb, pb, qb = b
    va = world(va, pa, qa)
    vb = world(vb, pb, qb)
    fa, ea = features(va, ta)
    fb, eb = features(vb, tb)
    axes = fa + fb + [unit(cross(x, y)) for x in ea for y in eb if math.hypot(*cross(x, y)) > 1e-12]
    first, last = 0.0, 1.0
    initial = True
    for axis in axes:
        aa = [dot(p, axis) for p in va]
        bb = [dot(p, axis) for p in vb]
        lo, hi = min(aa) - max(bb), max(aa) - min(bb)
        initial = initial and lo <= 0 <= hi
        speed = dot(sub(db, da), axis)
        if not speed:
            if not lo <= 0 <= hi:
                return False, 0, initial
        else:
            times = sorted((lo / speed, hi / speed))
            first = max(first, times[0])
            last = min(last, times[1])
    return first <= last, first, initial


def distance(point, vertices, triangles, normals):
    # Independent Gram-system barycentric feasibility, then segment minimizers.
    if all(dot(n, sub(point, vertices[t[0]])) <= 0 for t, n in zip(triangles, normals)):
        return 0.0
    closest = math.inf
    for t, n in zip(triangles, normals):
        a, b, c = [vertices[i] for i in t]
        u, v, w = sub(b, a), sub(c, a), sub(point, a)
        uu, uv, vv, uw, vw = dot(u, u), dot(u, v), dot(v, v), dot(u, w), dot(v, w)
        denominator = uu * vv - uv * uv
        x, y = (uw * vv - vw * uv) / denominator, (vw * uu - uw * uv) / denominator
        if x >= 0 and y >= 0 and x + y <= 1:
            closest = min(closest, abs(dot(n, w)))
        for start, end in [(a, b), (b, c), (c, a)]:
            edge = sub(end, start)
            parameter = max(0, min(1, dot(sub(point, start), edge) / dot(edge, edge)))
            closest = min(closest, math.hypot(*sub(point, add(start, mul(edge, parameter)))))
    return closest


def sphere_reference(a, b, da, db, radius):
    vb, tb, pb, qb = b
    vertices = world(vb, pb, qb)
    normals, _ = features(vertices, tb)
    origin = a[2]
    motion = sub(da, db)

    def f(t):
        return distance(add(origin, mul(motion, t)), vertices, tb, normals)

    if f(0) <= radius:
        return True, 0.0
    # Distance to a closed convex set along a line is convex. The independent
    # reference brackets its minimum, then solves the first distance==radius root.
    low, high = 0.0, 1.0
    for _ in range(80):
        x = low + (high - low) / 3
        y = high - (high - low) / 3
        if f(x) < f(y):
            high = y
        else:
            low = x
    minimum = (low + high) / 2
    if f(minimum) > radius + 1e-11:
        return False, 0.0
    low, high = 0.0, minimum
    for _ in range(70):
        middle = (low + high) / 2
        if f(middle) > radius:
            low = middle
        else:
            high = middle
    return True, (low + high) / 2


def data():
    rng = random.Random(387792)
    shapes = [(BOX, BT), (TETRA, hull(TETRA)), (OCTA, hull(OCTA))]
    cases = []
    for _ in range(500):
        pair = []
        for side in range(2):
            v, t = rng.choice(shapes)
            scales = [rng.uniform(0.3, 1.5) for _ in range(3)]
            vertices = [tuple(p[i] * scales[i] for i in range(3)) for p in v]
            q = unit([rng.uniform(-1, 1) for _ in range(4)])
            pos = tuple(rng.uniform(-4, 4) for _ in range(3))
            pair.append((vertices, t, pos, q))
        da = tuple(rng.uniform(-12, 12) for _ in range(3))
        db = tuple(rng.uniform(-12, 12) for _ in range(3))
        cases.append((0, *pair, da, db, 0.5))
    for _ in range(180):
        v, t = rng.choice(shapes)
        q = unit([rng.uniform(-1, 1) for _ in range(4)])
        a = (BOX, BT, tuple(rng.uniform(-4, 4) for _ in range(3)), (0.0, 0.0, 0.0, 1.0))
        b = (v, t, (0.0, 0.0, 0.0), q)
        da = tuple(rng.uniform(-10, 10) for _ in range(3))
        db = tuple(rng.uniform(-2, 2) for _ in range(3))
        cases.append((1, a, b, da, db, rng.uniform(0.2, 0.8)))
    # Exact rational axis-aligned box times, including both moving partners.
    for gap in [3.0, 5.0, 10.0, 100.0, 1e12]:
        for speed in [0.0, 2.0, 8.0, 20.0, 2e12]:
            a = (BOX, BT, (0.0, 0.0, 0.0), (0.0, 0.0, 0.0, 1.0))
            b = (BOX, BT, (gap, 0.0, 0.0), (0.0, 0.0, 0.0, 1.0))
            cases.append((0, a, b, (speed + 1.0, 0.0, 0.0), (1.0, 0.0, 0.0), 0.5))
    return cases


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--probe", type=Path, required=True)
    args = parser.parse_args()
    cases = data()
    text = "".join(
        str(mode)
        + "\n"
        + encode(a)
        + encode(b)
        + " ".join(x.hex() for x in list(da) + list(db) + [r])
        + "\n"
        for mode, a, b, da, db, r in cases
    )
    rows = subprocess.check_output(
        [str(args.probe.resolve())], input=text, text=True, timeout=60
    ).splitlines()
    assert len(rows) == len(cases)
    counts = [0, 0]
    for number, ((mode, a, b, da, db, r), row) in enumerate(zip(cases, rows)):
        if mode:
            expected, time = sphere_reference(a, b, da, db, r)
            initial = time == 0
        else:
            expected, time, initial = interval_reference(a, b, da, db)
        if number >= 680:
            speed = Fraction(da[0]) - Fraction(db[0])
            gap = Fraction(b[2][0]) - 2
            expected = speed > 0 and gap <= speed
            time = float(gap / speed) if expected else 0.0
        tokens = row.split()
        status = int(tokens[0])
        hit = bool(int(tokens[1]))
        values = list(map(float.fromhex, tokens[2:]))
        fraction, depth = values[:2]
        normal = tuple(values[2:5])
        point = tuple(values[5:8])
        assert status == 0 and hit == expected, (number, mode, row, expected, time)
        if not hit:
            continue
        counts[mode] += 1
        assert math.isclose(fraction, time, rel_tol=2e-9, abs_tol=2e-9), (
            number,
            mode,
            fraction,
            time,
        )
        assert 0 <= fraction <= 1 and math.isclose(math.hypot(*normal), 1, abs_tol=1e-11)
        if not initial:
            assert depth == 0, (number, depth)
        if mode and not initial:
            center = add(a[2], mul(da, fraction))
            assert math.isclose(math.hypot(*sub(point, center)), r, abs_tol=2e-9), (
                number,
                point,
                center,
                r,
            )
            vertices, triangles, pos, q = b
            vertices = world(vertices, add(pos, mul(db, fraction)), q)
            normals, _ = features(vertices, triangles)
            distances = [dot(n, sub(point, vertices[t[0]])) for t, n in zip(triangles, normals)]
            assert max(distances) <= 2e-8 and min(abs(d) for d in distances) <= 2e-8, (
                number,
                distances,
            )

        elif not initial:
            for value, motion in [(a, da), (b, db)]:
                vertices, triangles, pos, q = value
                vertices = world(vertices, add(pos, mul(motion, fraction)), q)
                normals, _ = features(vertices, triangles)
                distances = [dot(n, sub(point, vertices[t[0]])) for t, n in zip(triangles, normals)]
                assert max(distances) <= 2e-8 and min(abs(d) for d in distances) <= 2e-8, (
                    number,
                    distances,
                )
    print(
        f"Convex sweep oracle: {len(cases)} cases, {counts[0]} polyhedron and {counts[1]} sphere hits; independent intervals, rational times, distance roots and surface witnesses"
    )


if __name__ == "__main__":
    main()
