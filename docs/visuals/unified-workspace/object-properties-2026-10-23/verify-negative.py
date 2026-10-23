"""Check real pre-change pictures, parser failure and actual native object tasks."""
from pathlib import Path
import gzip,hashlib,json,sys
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[3]
sys.path.insert(0,str(ROOT/'tests'))
from check_graphic_raster import png
from check_transport_design import pixels,pixel
from check_inspector_design import rect
def read(name):return json.loads(gzip.decompress((HERE/name).read_bytes()))
before=png(HERE/'before-object-properties.png');assert pixel(before,1154,307)!=(138,186,255)
old=read('linux1-object_design-steps.json.gz')[0]
assert old['exit_code']==0 and 'OBJECT DESIGN SELF-TEST: PASSED' in old['stdout']
result=read('linux1-object_design-result.json.gz');assert result['status']=='failed'
assert 'TypeError' in result['steps'][-1]['stderr']
old_line=next(line for line in old['stdout'].splitlines() if line.startswith('OBJECT DESIGN kind=header variant=large ') and 'id=11 ' in line)
old_title=rect(old_line,'title');old_heading=rect(old_line,'heading')
assert old_title[0]>old_heading[0]+1 and old_title[2]<120
assert pixels(png(HERE/'negative-narrow-word-wrap.png'),old_title,(32,36,44),35)
steps=read('linux2-object_design-steps.json.gz');validation=read('linux2-object_design-validation.json.gz')
assert len(steps)==2 and len(validation['checks'])==2
for step,check in zip(steps,validation['checks']):
    assert step['exit_code']==0 and 'OBJECT DESIGN SELF-TEST: PASSED' in step['stdout']
    assert 'OBJECT ACTIONS hide=1 show-all=1 keyboard-reselect=1 source-and-run=1' in step['stdout']
    records=check['records'];assert len(records)==36 and all(r['preserved']==1 for r in records)
    assert len([r for r in records if r['kind']=='collapsed' and r['id']==3 and r['count']==0])==6
    named=next(r for r in records if r['kind']=='header' and r['variant']=='large' and r['id']==11)
    assert named['title'][0]==named['heading'][0] and named['title'][2]>old_title[2] and named['title'][3]<old_title[3]
system=validation['checks'][0]['records'];r=next(r for r in system if r['kind']=='header' and r['variant']=='dark' and r['id']==3)
after=png(HERE/'linux2-system-dark-3-header.png');avatar=r['avatar'];assert pixel(after,avatar[0]+avatar[2]/2,avatar[1]+avatar[3]/2)==(138,186,255)
assert pixels(after,r['title'],(237,238,242),35)
assert r['properties'][0]['card'][2]<r['heading'][2]/2
for name,record in json.loads((HERE/'artifacts.json').read_text(encoding='utf-8')).items():
    b=(HERE/name).read_bytes();assert len(b)==record['bytes'] and hashlib.sha256(b).hexdigest()==record['sha256'],name
print('FROZEN OBJECT: native before-state and parser failure retained; current 72 native records and actual hide/show/reselect tasks verified; whole UI remains open')
