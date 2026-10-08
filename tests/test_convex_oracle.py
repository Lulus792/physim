"""Independent world-vertex projections and contact witness checks."""

import argparse
import math
import random
import subprocess
from pathlib import Path


def add(a, b):
    return tuple(x + y for x, y in zip(a, b))


def sub(a, b):
    return tuple(x - y for x, y in zip(a, b))


def mul(a, s):
    return tuple(x * s for x in a)


def dot(a, b):
    return math.fsum(x * y for x, y in zip(a, b))


def cross(a, b):
    return (a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0])


def unit(a):
    return mul(a, 1 / math.hypot(*a))


def hull(vertices):
    # Fixtures have triangular faces except box (whose triangulation is explicit).
    import itertools

    faces = []
    for a, b, c in itertools.combinations(range(len(vertices)), 3):
        n = cross(sub(vertices[b], vertices[a]), sub(vertices[c], vertices[a]))
        if math.hypot(*n) < 1e-10:
            continue
        sides = [dot(n, sub(p, vertices[a])) for i, p in enumerate(vertices) if i not in (a, b, c)]
        if all(x < -1e-10 for x in sides):
            faces.append((a, b, c))
        elif all(x > 1e-10 for x in sides):
            faces.append((a, c, b))
    return faces


BOX = [
    (-1.0, -1.0, -1.0),
    (1.0, -1.0, -1.0),
    (1.0, 1.0, -1.0),
    (-1.0, 1.0, -1.0),
    (-1.0, -1.0, 1.0),
    (1.0, -1.0, 1.0),
    (1.0, 1.0, 1.0),
    (-1.0, 1.0, 1.0),
]
BT = [
    (0, 2, 1),
    (0, 3, 2),
    (4, 5, 6),
    (4, 6, 7),
    (0, 1, 5),
    (0, 5, 4),
    (3, 7, 6),
    (3, 6, 2),
    (0, 4, 7),
    (0, 7, 3),
    (1, 2, 6),
    (1, 6, 5),
]
TETRA = [(1.0, 1.0, 1.0), (1.0, -1.0, -1.0), (-1.0, 1.0, -1.0), (-1.0, -1.0, 1.0)]
OCTA = [
    (1.0, 0.0, 0.0),
    (-1.0, 0.0, 0.0),
    (0.0, 1.0, 0.0),
    (0.0, -1.0, 0.0),
    (0.0, 0.0, 1.0),
    (0.0, 0.0, -1.0),
]


def world(vertices, pos, q):
    x, y, z, w = q
    n = sum(a * a for a in q)
    matrix = [
        (1 - 2 * (y * y + z * z) / n, 2 * (x * y - z * w) / n, 2 * (x * z + y * w) / n),
        (2 * (x * y + z * w) / n, 1 - 2 * (x * x + z * z) / n, 2 * (y * z - x * w) / n),
        (2 * (x * z - y * w) / n, 2 * (y * z + x * w) / n, 1 - 2 * (x * x + y * y) / n),
    ]
    return [add(pos, tuple(dot(row, p) for row in matrix)) for p in vertices]


def features(vertices, triangles):
    faces = [
        unit(cross(sub(vertices[b], vertices[a]), sub(vertices[c], vertices[a])))
        for a, b, c in triangles
    ]
    edges = sorted({tuple(sorted(e)) for a, b, c in triangles for e in [(a, b), (b, c), (c, a)]})
    return faces, [unit(sub(vertices[b], vertices[a])) for a, b in edges]


def reference(a, b):
    va, ta, pa, qa = a
    vb, tb, pb, qb = b
    va = world(va, pa, qa)
    vb = world(vb, pb, qb)
    fa, ea = features(va, ta)
    fb, eb = features(vb, tb)
    axes = fa + fb + [unit(cross(x, y)) for x in ea for y in eb if math.hypot(*cross(x, y)) > 1e-12]
    depth = math.inf
    for axis in axes:
        aa = [dot(p, axis) for p in va]
        bb = [dot(p, axis) for p in vb]
        overlap = min(max(aa) - min(bb), max(bb) - min(aa))
        if overlap < -1e-10:
            return False, 0, va, vb, fa, fb
        depth = min(depth, max(0, overlap))
    return True, depth, va, vb, fa, fb


def data():
    rng = random.Random(385792)
    shapes = [(BOX, BT), (TETRA, hull(TETRA)), (OCTA, hull(OCTA))]
    result = []
    for _ in range(500):
        pair = []
        for side in range(2):
            v, t = rng.choice(shapes)
            scales = [rng.uniform(0.2, 2) for _ in range(3)]
            vertices = [tuple(p[i] * scales[i] for i in range(3)) for p in v]
            q = [rng.uniform(-1, 1) for _ in range(4)]
            q = unit(q)
            pos = tuple(rng.uniform(-2.5, 2.5) for _ in range(3))
            pair.append((vertices, t, pos, q))
        result.append(pair)
    for distance in [0.0, 0.1, 1.0, 1.5, 2.0, 2.000001, 3.0]:
        result.append(
            [
                (BOX, BT, (0.0, 0.0, 0.0), (0.0, 0.0, 0.0, 1.0)),
                (BOX, BT, (distance, 0.0, 0.0), (0.0, 0.0, 0.0, 1.0)),
            ]
        )
    return result + [[b, a] for a, b in result]


def encode(mesh):
    v, t, p, q = mesh
    return (
        f"{len(v)} {len(t)}\n"
        + "\n".join(" ".join(x.hex() for x in vertex) for vertex in v)
        + "\n"
        + "\n".join(" ".join(map(str, tri)) for tri in t)
        + "\n"
        + " ".join(x.hex() for x in list(p) + list(q))
        + "\n"
    )


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--probe", type=Path, required=True)
    args = parser.parse_args()
    cases = data()
    inputs = "".join(encode(a) + encode(b) for a, b in cases)
    rows = subprocess.check_output(
        [str(args.probe.resolve())], input=inputs, text=True, timeout=60
    ).splitlines()
    assert len(rows) == len(cases)
    contacts = 0
    for number, ((a, b), row) in enumerate(zip(cases, rows)):
        expected, depth, va, vb, fa, fb = reference(a, b)
        tokens = row.split()
        status = int(tokens[0])
        hit = bool(int(tokens[1]))
        values = list(map(float.fromhex, tokens[2:]))
        d = values[0]
        n = tuple(values[1:4])
        p = tuple(values[4:7])
        assert status == 0 and hit == expected, (number, row, expected)
        if not hit:
            continue
        contacts += 1
        assert math.isclose(d, depth, rel_tol=2e-10, abs_tol=2e-10), (number, d, depth)
        assert math.isclose(math.hypot(*n), 1, abs_tol=1e-12)
        # Reconstructed witnesses must lie on/in their respective original shapes.
        wa = add(p, mul(n, 0.5 * d))
        wb = sub(p, mul(n, 0.5 * d))
        for witness, vertices, triangles, normals in [(wa, va, a[1], fa), (wb, vb, b[1], fb)]:
            distances = [
                dot(normal, sub(witness, vertices[tri[0]]))
                for tri, normal in zip(triangles, normals)
            ]
            assert max(distances) <= 2e-9 and min(abs(x) for x in distances) <= 2e-9, (
                number,
                witness,
                distances,
            )
    print(
        f"Convex independent SAT oracle: {len(cases)} box/tetrahedron/octahedron pairs, {contacts} contacts with surface witnesses"
    )


if __name__ == "__main__":
    main()
