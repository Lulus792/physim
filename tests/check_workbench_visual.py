"""Compare a native 1204x866 default workbench capture with the chosen reference.

Requires Pillow. Checks common chrome, not illustrative code/context contents.
The source image is only read; no scaling, recoloring or retouching is applied.
"""
import argparse
import hashlib
import json
from pathlib import Path
from PIL import Image

p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--native',type=Path,required=True)
p.add_argument('--theme',choices=('dark','light'),required=True)
p.add_argument('--report',type=Path,required=True)
a=p.parse_args()
reference=Path(__file__).resolve().parents[1]/'docs/visuals/develop-designs'/f'glass-cool-{a.theme}.png'
images=[Image.open(path).convert('RGB') for path in (reference,a.native)]
report=dict(reference=str(reference),native=str(a.native),theme=a.theme,
            sha256=[hashlib.sha256(path.read_bytes()).hexdigest() for path in (reference,a.native)],
            scope='Default 1204x866 chrome: menu text ink, surfaces, selection and panel dividers',checks=[],metrics={})
def check(ok,name):report['checks'].append(dict(name=name,passed=bool(ok)))
check(all(im.size==(1204,866) for im in images),'actual client size 1204x866')
if report['checks'][0]['passed']:
    ink=[]
    for im in images:
        # Measure antialiased coverage relative to the actual flat menu
        # surface. A fixed dark/light RGB cutoff biases ClearType and grayscale
        # rasterizers differently and can trim whole edge columns in light mode.
        background=im.getpixel((49,9))
        pixels=[(x,y) for y in range(8,29) for x in range(49,85)
                if max(abs(c-b) for c,b in zip(im.getpixel((x,y)),background))>32]
        ink.append([max(x for x,y in pixels)-min(x for x,y in pixels)+1,
                    max(y for x,y in pixels)-min(y for x,y in pixels)+1] if pixels else [0,0])
    report['metrics']['Datei_ink_width_height']=ink
    check(.80*ink[0][0]<=ink[1][0]<=1.25*ink[0][0] and .85*ink[0][1]<=ink[1][1]<=1.25*ink[0][1],
          'menu ink has reference scale; no miniature labels')
    for name,point in [('workspace surface',(180,500)),('code surface',(930,610)),('active source selection',(10,165))]:
        values=[im.getpixel(point) for im in images];report['metrics'][name]=values
        check(values[0]==values[1],name)
    border=(59,61,69) if a.theme=='dark' else (190,195,206)
    def edge(im,axis,positions,span):
        def count(pos):
            return sum(max(abs(c-b) for c,b in zip(im.getpixel((pos,t) if axis=='x' else (t,pos)),border))<=32 for t in span)
        best=max(positions,key=count)
        return [best,count(best)/len(span)]
    for name,axis,positions,span in [
        ('workspace divider','x',range(215,222),range(115,670)),
        ('context divider','x',range(945,952),range(115,800)),
        ('protocol top','y',range(677,684),range(225,940)),
        ('protocol header rule','y',range(718,725),range(225,940)),
        ('editor status rule','y',range(805,812),range(225,940)),
    ]:
        values=[edge(im,axis,positions,span) for im in images];report['metrics'][name]=values
        check(abs(values[0][0]-values[1][0])<=1 and min(v[1] for v in values)>=.65,name)
report['passed']=all(c['passed'] for c in report['checks'])
a.report.parent.mkdir(parents=True,exist_ok=True)
a.report.write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
for c in report['checks']:print(('PASS ' if c['passed'] else 'FAIL ')+c['name'])
raise SystemExit(0 if report['passed'] else 1)
