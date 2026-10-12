"""Independent nine-property SI/value oracle for actual C and Physim programs."""
import csv
import math
import subprocess
import sys

expected = {
    'density': (1200, (-3, 1, 0, 0, 0, 0, 0)),
    'dynamic_viscosity': (.006, (-1, 1, -1, 0, 0, 0, 0)),
    'kinematic_viscosity': (5e-6, (2, 0, -1, 0, 0, 0, 0)),
    'friction': (.25, (0, 0, 0, 0, 0, 0, 0)),
    'restitution': (.8, (0, 0, 0, 0, 0, 0, 0)),
    'thermal_conductivity': (.5, (1, 1, -3, 0, -1, 0, 0)),
    'specific_heat': (2000, (2, 0, -2, 0, -1, 0, 0)),
    'electrical_conductivity': (250, (-3, -1, 3, 2, 0, 0, 0)),
    'permittivity': (2e-11, (-3, -1, 4, 2, 0, 0, 0)),
}
assert len(sys.argv) == 3
results = []
for language, program in zip(('C', 'Physim'), sys.argv[1:]):
    result = subprocess.run([program], capture_output=True, text=True, timeout=15)
    assert result.returncode == 0, (language, result.stderr)
    rows = list(csv.reader(result.stdout.splitlines()))
    assert len(rows) == 27, (language, rows)
    values = {}
    for row in rows:
        assert len(row) == (10 if language == 'C' else 3), row
        query, name, value = int(row[0]), row[1], float(row[2])
        assert query in (0, 1, 2) and name in expected and (query, name) not in values
        reference, dimensions = expected[name]
        assert math.isclose(value, reference, rel_tol=2e-14, abs_tol=0), row
        if language == 'C':
            assert tuple(map(int, row[3:])) == dimensions, row
        values[query, name] = value
    assert len(values) == 3 * len(expected)
    results.append(values)
assert len(results) == 2
for key in results[0]:
    assert math.isclose(results[0][key], results[1][key], rel_tol=2e-14, abs_tol=0), key
print('Nine synthetic material properties: 54 real C/Physim samples, independent SI/value references, domain rejection and viscosity/density relation passed')
