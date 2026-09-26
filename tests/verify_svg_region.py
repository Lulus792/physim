"""Independent XML checks for the SVG region fixtures from --plot-test."""
from pathlib import Path
import sys
import xml.etree.ElementTree as ET

root = Path(sys.argv[1])
ns = {"s": "http://www.w3.org/2000/svg"}
for i in range(3):
    tree = ET.parse(root / f"figure-region-{i}.svg")
    clip = tree.find('.//s:clipPath[@id="plot-region"]/s:rect', ns)
    assert clip is not None
    assert tuple(float(clip.get(k)) for k in ("x", "y", "width", "height")) == (110, 90, 1040, 480)
    groups = tree.findall('.//s:g[@clip-path="url(#plot-region)"]', ns)
    assert len(groups) == 1
    group = groups[0]
    if i == 0:
        line = group.find('s:polyline', ns)
        assert line.get('points').strip() == '-930.000,330.000 2190.000,330.000'
    elif i == 1:
        bar = group.find('s:rect', ns)
        assert tuple(float(bar.get(k)) for k in ('x', 'y', 'width', 'height')) == (-410, -390, 2080, 960)
    else:
        assert [float(c.get('cx')) for c in group.findall('s:circle', ns)] == [-930, 2190]
    assert not group.findall('s:text', ns), 'Legend must remain outside clipping group'
    ticks = tree.findall('.//s:text[@y="594"]', ns)
    assert [float(t.text) for t in ticks] == [0, .25, .5, .75, 1]
for path in (root / 'runs').glob('*-ausschnitt.svg'):
    tree = ET.parse(path)
    ticks = tree.findall('.//s:text[@y="594"]', ns)
    values = [float(t.text) for t in ticks]
    assert len(values) == 5 and all(a < b for a, b in zip(values, values[1:]))
    assert 'X-Offset:' in ''.join(tree.getroot().itertext())
print('SVG regions: clipping groups, crossing geometry, unclipped legend and distinct ticks passed')
