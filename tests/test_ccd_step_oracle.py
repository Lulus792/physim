"""Independent rational 1D hard-sphere event sequences, momentum and energy."""

import argparse
import random
import subprocess
from fractions import Fraction
from pathlib import Path


def cases():
    rng = random.Random(387794)
    data = []
    for _ in range(180):
        n = rng.randrange(2, 7)
        x = 0.0
        bodies = []
        ids = list(range(1, n + 1))
        rng.shuffle(ids)
        for i in range(n):
            x += rng.uniform(1.5, 3.5)
            bodies.append((ids[i], rng.uniform(0.5, 3), x, rng.uniform(-15, 15)))
        data.append((rng.uniform(0.2, 1), bodies))
    data.append((1.0, [(3, 1.0, -4.0, 10.0), (2, 1.0, 0.0, 0.0), (1, 1.0, 4.0, 0.0)]))
    return data


def reference(dt, bodies):
    m = [Fraction(b[1]) for b in bodies]
    x = [Fraction(b[2]) for b in bodies]
    v = [Fraction(b[3]) for b in bodies]
    remaining = Fraction(dt)
    events = 0
    while remaining:
        first = remaining + 1
        pair = None
        # Order never changes for disjoint equal-radius balls. Adjacent pairs
        # alone determine the next event, independently of the C broad phase.
        for i in range(len(x) - 1):
            speed = v[i] - v[i + 1]
            if speed > 0:
                t = (x[i + 1] - x[i] - 1) / speed
                if 0 <= t <= remaining and t < first:
                    first = t
                    pair = i
        if pair is None:
            x = [p + speed * remaining for p, speed in zip(x, v)]
            break
        x = [p + speed * first for p, speed in zip(x, v)]
        remaining -= first
        i = pair
        a, b = v[i], v[i + 1]
        total = m[i] + m[i + 1]
        v[i] = ((m[i] - m[i + 1]) * a + 2 * m[i + 1] * b) / total
        v[i + 1] = (2 * m[i] * a + (m[i + 1] - m[i]) * b) / total
        events += 1
        assert events < 100
    return list(map(float, x)), list(map(float, v)), events


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--probe", type=Path, required=True)
    args = parser.parse_args()
    data = cases()
    text = "".join(
        f"{len(bodies)} {dt.hex()}\n"
        + "".join(
            str(i) + " " + " ".join(a.hex() for a in [m, x, v]) + "\n" for i, m, x, v in bodies
        )
        for dt, bodies in data
    )
    rows = subprocess.check_output(
        [str(args.probe.resolve())], input=text, text=True, timeout=120
    ).splitlines()
    assert len(rows) == len(data)
    total = 0
    for number, ((dt, bodies), line) in enumerate(zip(data, rows)):
        x, v, events = reference(dt, bodies)
        tokens = line.split()
        assert int(tokens[0]) == 0, (number, line)
        assert int(tokens[1]) == events, (number, int(tokens[1]), events)
        actual = list(map(float.fromhex, tokens[2:]))
        assert len(actual) == 2 * len(bodies)
        assert max(abs(a - b) for a, b in zip(actual[::2], x)) < 2e-4, (number, actual, x)
        assert max(abs(a - b) for a, b in zip(actual[1::2], v)) < 2e-8, (number, actual, v)
        momentum0 = sum(m * velocity for _, m, _, velocity in bodies)
        momentum = sum(b[1] * velocity for b, velocity in zip(bodies, actual[1::2]))
        energy0 = sum(0.5 * m * velocity * velocity for _, m, _, velocity in bodies)
        energy = sum(0.5 * b[1] * velocity * velocity for b, velocity in zip(bodies, actual[1::2]))
        assert abs(momentum - momentum0) < 2e-8 and abs(energy - energy0) < 2e-7, (
            number,
            momentum,
            energy,
        )
        total += events
    print(
        f"CCD event oracle: {len(data)} rational multi-body sequences, {total} elastic events; canonical IDs, positions, velocities, momentum and energy passed"
    )


if __name__ == "__main__":
    main()
