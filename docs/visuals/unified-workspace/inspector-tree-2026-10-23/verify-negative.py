"""Replay real card/header defects and selected actual native positive evidence."""
from pathlib import Path
import gzip,hashlib,json,sys
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[3]
sys.path.insert(0,str(ROOT/'tests'))
from check_graphic_raster import png
from check_transport_design import pixels,pixel
from check_inspector_design import rule_coverage

def read(name):return json.loads(gzip.decompress((HERE/name).read_bytes()))
def bands(im):
    rows=sorted({y for x,y in pixels(im,(889,235,175,58),(255,255,255),0)});groups=[]
    for y in rows:
        if not groups or y>groups[-1][-1]+1:groups.append([y])
        else:groups[-1].append(y)
    return [(min(g),max(g)) for g in groups]

assert not pixels(png(HERE/'negative-card-clip-origin.png'),(1156,294,40,20),(237,238,242),30)
assert pixel(png(HERE/'negative-half-card-border.png'),1137,491)==(44,46,52)
assert bands(png(HERE/'negative-header-word-wrap.png'))==[(245,260),(274,282)]
for label in ('windows3','windows5'):
    s=read(label+'-inspector_design-steps.json.gz')[0]
    assert s['exit_code']==1 and 'First workflow failure at stage 0' in s['stderr'] and 'keyboard=0' in s['stderr']
manifest=json.loads((HERE/'validation.json').read_text(encoding='utf-8'))
for run in manifest['runs']:
    label=run['label'];steps=read(label+'-inspector_design-steps.json.gz');validation=read(label+'-inspector_design-validation.json.gz')
    assert len(steps)==2 and all(s['exit_code']==0 and 'INSPECTOR DESIGN SELF-TEST: PASSED' in s['stdout'] for s in steps)
    assert len(validation['checks'])==2
    for c in validation['checks']:
        assert len(c['records'])==18 and len(c['actions'])==6
        assert all((v['actual_wheel'] and v['scroll']>0) if v['variant'] not in ('dark','light') else (not v['actual_wheel'] and v['scroll']==0) for v in c['actions'])
    system=validation['checks'][0]['records'];closed=next(r for r in system if r['kind']=='closed' and r['variant']=='dark')
    image=png(HERE/f'{label}-system-dark-closed.png')
    assert pixels(image,closed['title'],(237,238,242),35)
    rule_coverage(image,closed['sections'][0]['rect'],(59,61,69),(30,31,35))
    card=next(r for r in system if r['kind']=='card-0' and r['variant']=='dark');entry=card['cards'][0];bounds=entry['card']
    image=png(HERE/f'{label}-system-dark-card-0.png')
    assert pixels(image,entry['input'],(237,238,242),65)
    assert pixels(image,(bounds[0]-1,bounds[1]+15,3,bounds[3]-30),(59,61,69),15)
    assert abs(entry['input'][1]+entry['input'][3]-(bounds[1]+bounds[3]-12))<.02
    for r in system:
        header=r['header']
        for key in ('heading-title','heading-meta'):
            b=r[key];assert b[0]>=header[0] and b[1]>=header[1] and b[0]+b[2]<=header[0]+header[2]+1 and b[1]+b[3]<=header[1]+header[3]+1
    if label.startswith('linux'):
        assert bands(png(HERE/f'{label}-system-contrast-card-0.png'))==[(245,260)]
for name,record in json.loads((HERE/'artifacts.json').read_text(encoding='utf-8')).items():
    b=(HERE/name).read_bytes();assert len(b)==record['bytes'] and hashlib.sha256(b).hexdigest()==record['sha256'],name
print('FROZEN INSPECTOR: real overlap, clipped border and word-wrap defects reproduced; current selected native evidence verified; Windows final pending='+str(manifest['windows_final_pending']))
