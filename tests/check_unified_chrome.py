"""Compare shared native chrome with the selected PNG, without altering either image.

This verifies a bounded part of the design: client size, the three tab rectangles,
the two horizontal boundaries and five semantic surfaces. The optional workbench
check also compares side and log/status boundaries. It is not a whole-app
score. Content, fonts, interactions and the rest of each workspace need their own
proofs. Pillow is required, as for the existing workbench visual checker.
"""
import argparse
import hashlib
import json
from pathlib import Path

from PIL import Image

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--native', type=Path, required=True)
parser.add_argument('--area', choices=('develop', 'simulate', 'analyze'), required=True)
parser.add_argument('--theme', choices=('dark', 'light'), required=True)
parser.add_argument('--report', type=Path, required=True)
parser.add_argument('--workbench', action='store_true')
args = parser.parse_args()
if args.workbench and args.area != 'develop':
    parser.error('--workbench requires --area develop')
reference = Path(__file__).resolve().parents[1] / 'docs/visuals/unified-workspace' / f'{args.area}-{args.theme}.png'
paths = (reference, args.native)
images = [Image.open(path).convert('RGB') for path in paths]
report = dict(scope=__doc__.strip(), reference=str(reference), native=str(args.native),
              sha256=[hashlib.sha256(path.read_bytes()).hexdigest() for path in paths], checks=[])


def check(name, expected, actual):
    report['checks'].append(dict(name=name, expected=expected, actual=actual, passed=expected == actual))


def coverage(image, y, color):
    runs = []
    for x in range(image.width):
        if image.getpixel((x, y)) == color:
            if runs and runs[-1][1] == x:
                runs[-1][1] = x + 1
            else:
                runs.append([x, x + 1])
    return runs


check('actual client size', images[0].size, images[1].size)
if images[0].size == images[1].size == (1440, 1000):
    for name, point in [('menu surface', (5, 10)), ('path surface', (5, 100)),
                        ('path left inset', (10, 110)), ('workspace surface', (5, 150)),
                        ('footer surface', (800, 990))]:
        check(name, images[0].getpixel(point), images[1].getpixel(point))
    # Derive selected-tab coverage from the reference, including its 8 px gaps.
    # Scan above its text, so no rasterizer-specific glyph pixels are excluded.
    color = (53, 54, 62) if args.theme == 'dark' else (224, 227, 234)
    for y in (43, 50, 69):
        check(f'selected tab coverage at y={y}',
              coverage(images[0], y, color), coverage(images[1], y, color))
    border = (59, 61, 69) if args.theme == 'dark' else (190, 195, 206)
    for y in (78, 117):
        check(f'full one-pixel boundary at y={y}',
              sum(images[0].getpixel((x, y)) == border for x in range(1440)),
              sum(images[1].getpixel((x, y)) == border for x in range(1440)))
    if args.workbench:
        # Scan continuous empty boundary spans. Text and variable real project
        # content are outside these spans; do not mask, retouch or rescale pixels.
        side_boundaries = [[x for x in range(180, 1280)
                            if all(im.getpixel((x, y)) == border for y in range(200, 790))]
                           for im in images]
        check('workspace and code-context boundaries', *side_boundaries)
        log_boundaries = [[y for y in range(750, 972)
                           if all(im.getpixel((x, y)) == border for x in range(230, 1150))]
                          for im in images]
        check('protocol, body and editor-status boundaries', *log_boundaries)
report['passed'] = all(item['passed'] for item in report['checks'])
args.report.parent.mkdir(parents=True, exist_ok=True)
args.report.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
for item in report['checks']:
    print(('PASS ' if item['passed'] else 'FAIL ') + item['name'])
raise SystemExit(0 if report['passed'] else 1)
