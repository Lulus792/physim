"""Bounded direct H-reference check of the clear article top and plain footer.

The reference contains a 35 px mock window title, the native PNG a client area.
Only explicitly listed flat samples are compared at their own coordinates;
neither input is resized, cropped, recolored or masked. This is not a full H,
typography, content, interaction or platform-equivalence score. Pillow optional.
"""
import argparse
import hashlib
import json
from pathlib import Path

from PIL import Image


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--native',type=Path,required=True)
    parser.add_argument('--theme',choices=('dark','light'),required=True)
    parser.add_argument('--report',type=Path,required=True)
    parser.add_argument('--native-title-height',type=int,default=0,
                        help='35 only for the original-reference selfcheck; native client captures use 0')
    args=parser.parse_args()
    reference=Path(__file__).resolve().parents[1]/'docs/visuals/f1-layouts'/f'h-{args.theme}.png'
    ref=Image.open(reference).convert('RGB');native=Image.open(args.native).convert('RGB')
    checks=[]
    def check(name,expected,actual,points=None):
        checks.append(dict(name=name,reference=expected,native=actual,points=points,passed=expected==actual))
    check('same reference width',ref.width,native.width)
    check('supported native client/reference size',True,native.width==1080 and native.height>=540)
    if native.width==1080 and native.height>=540:
        for x in (268,310,356,390,764,808):
            a=(x,112);b=(x,args.native_title_height+77)
            check(f'clear article top at x={x}',ref.getpixel(a),native.getpixel(b),dict(reference=a,native=b))
        for x in (930,980,1050):
            a=(x,ref.height-24);b=(x,native.height-24)
            check(f'plain footer at x={x}',ref.getpixel(a),native.getpixel(b),dict(reference=a,native=b))
    report=dict(scope=__doc__.strip(),reference=str(reference),native=str(args.native),
                sha256=[hashlib.sha256(p.read_bytes()).hexdigest() for p in (reference,args.native)],
                sizes=[ref.size,native.size],native_title_height=args.native_title_height,
                checks=checks,passed=all(c['passed'] for c in checks))
    args.report.parent.mkdir(parents=True,exist_ok=True)
    args.report.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    for row in checks:print(('PASS ' if row['passed'] else 'FAIL ')+row['name'])
    raise SystemExit(0 if report['passed'] else 1)


if __name__=='__main__':main()
