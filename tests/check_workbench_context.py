"""Bounded direct PNG comparison of the API card's straight border segments.

Uses the unchanged selected reference, without resizing, masks or retouching.
This is not a typography, corner-antialiasing, interaction or whole-UI score.
Pillow is optional, as for check_unified_chrome.py; native workflows need none.
"""
import argparse
import hashlib
import json
from pathlib import Path

from PIL import Image


def frame(image, border, minimum):
    counts={x:sum(image.getpixel((x,y))==border for y in range(390,690))
            for x in range(1190,1432)}
    columns=[x for x,count in counts.items() if count>=minimum]
    if len(columns)!=2:
        return dict(columns=columns,maximum_vertical_pixels=max(counts.values()),rows=[])
    left,right=columns
    rows=[y for y in range(390,690)
          if all(image.getpixel((x,y))==border for x in range(left+8,right-7))]
    return dict(columns=columns,maximum_vertical_pixels=max(counts.values()),rows=rows)


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--native',type=Path,required=True)
    parser.add_argument('--theme',choices=('light','dark'),required=True)
    parser.add_argument('--report',type=Path,required=True)
    parser.add_argument('--match-reference-height',action='store_true',
                        help='Also require the original top/bottom for the matching Windows C-card state')
    args=parser.parse_args()
    reference=Path(__file__).resolve().parents[1]/'docs/visuals/unified-workspace'/f'develop-{args.theme}.png'
    images=[Image.open(path).convert('RGB') for path in (reference,args.native)]
    border=(190,195,206) if args.theme=='light' else (59,61,69)
    checks=[]
    def check(name,passed,expected,actual):
        checks.append(dict(name=name,passed=bool(passed),reference=expected,native=actual))
    check('actual client size',images[1].size==images[0].size,images[0].size,images[1].size)
    frames=[]
    if images[0].size==images[1].size==(1440,1000):
        frames=[frame(image,border,180) for image in images]
        expected,actual=frames
        check('reference card has two straight sides',len(expected['columns'])==2,2,len(expected['columns']))
        check('native sides have the exact reference border color',len(actual['columns'])==2,2,len(actual['columns']))
        check('same card horizontal bounds',actual['columns']==expected['columns'],expected['columns'],actual['columns'])
        check('two native horizontal rules',len(actual['rows'])==2,2,len(actual['rows']))
        if args.match_reference_height:
            check('same card vertical bounds',actual['rows']==expected['rows'],expected['rows'],actual['rows'])
    else:
        check('supported comparison size',False,(1440,1000),images[1].size)
    report=dict(scope=__doc__.strip(),reference=str(reference),native=str(args.native),
                sha256=[hashlib.sha256(path.read_bytes()).hexdigest() for path in (reference,args.native)],
                border=border,frames=frames,checks=checks,passed=all(c['passed'] for c in checks))
    args.report.parent.mkdir(parents=True,exist_ok=True)
    args.report.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    for item in checks:print(('PASS ' if item['passed'] else 'FAIL ')+item['name'])
    raise SystemExit(0 if report['passed'] else 1)


if __name__=='__main__':
    main()
