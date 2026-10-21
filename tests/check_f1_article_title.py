"""Measure only the shared H heading's ink edges; not a whole-UI score.

The original has a 1 px frame and a 34 px mock title bar. Native screenshots
contain client pixels. Both full input images remain unchanged. The fixed
heading region and foreground thresholds are recorded in the JSON report.
"""
import argparse
import hashlib
import json
from pathlib import Path

from PIL import Image


def heading_box(image, theme, origin):
    points=[]
    for y in range(125,190):
        for x in range(270,810):
            rgb=image.getpixel((x+origin[0],y+origin[1]))
            if min(rgb)>180 if theme=='dark' else max(rgb)<100:
                points.append((x,y))
    if not points:
        return None
    return [min(x for x,y in points),min(y for x,y in points),
            max(x for x,y in points)+1,max(y for x,y in points)+1]


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--native',type=Path,required=True)
    parser.add_argument('--theme',choices=('dark','light'),required=True)
    parser.add_argument('--report',type=Path,required=True)
    parser.add_argument('--reference-selfcheck',action='store_true')
    args=parser.parse_args()
    reference=Path(__file__).resolve().parents[1]/'docs/visuals/f1-layouts'/f'h-{args.theme}.png'
    if args.reference_selfcheck and args.native.resolve()!=reference.resolve():
        parser.error('Selfcheck requires the unchanged original reference as input')
    origin=[1,35] if args.reference_selfcheck else [0,0]
    ref=Image.open(reference).convert('RGB');native=Image.open(args.native).convert('RGB')
    supported=ref.width==1080 and native.width==1080 and native.height>=540
    expected=heading_box(ref,args.theme,[1,35]) if supported else None
    actual=heading_box(native,args.theme,origin) if supported else None
    deltas=[b-a for a,b in zip(expected,actual)] if expected and actual else None
    # A wrapped title touching this fixed ROI is not a complete title measure.
    complete=bool(actual and actual[0]>270 and actual[1]>125 and actual[2]<810 and actual[3]<190)
    checks=[] if not deltas else [dict(edge=edge,delta_px=delta,passed=abs(delta)<=1)
        for edge,delta in zip(('left','top','right','bottom'),deltas)]
    report=dict(scope=__doc__.strip(),reference=str(reference),native=str(args.native),
        sha256=[hashlib.sha256(p.read_bytes()).hexdigest() for p in (reference,args.native)],
        sizes=[ref.size,native.size],region=[270,125,810,190],reference_origin=[1,35],native_origin=origin,
        foreground='min(R,G,B)>180' if args.theme=='dark' else 'max(R,G,B)<100',
        reference_box=expected,native_box=actual,supported=supported,region_complete=complete,checks=checks,
        aligned=supported and complete and len(checks)==4 and all(c['passed'] for c in checks))
    args.report.parent.mkdir(parents=True,exist_ok=True)
    args.report.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    print(json.dumps(dict(reference=expected,native=actual,deltas=deltas,aligned=report['aligned'])))
    raise SystemExit(0 if report['aligned'] else 1)


if __name__=='__main__':main()
