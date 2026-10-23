"""Frozen native before/after counterexample; no full-UI similarity score."""
import gzip,hashlib,json,math,pathlib
from PIL import Image
HERE=pathlib.Path(__file__).resolve().parent
validation=json.loads((HERE/'validation.json').read_text(encoding='utf-8'))
stage=validation['final_source']
data=json.loads(gzip.decompress((HERE/f'linux{stage}-tree_design-validation.json.gz').read_bytes()))
checks=next(x for x in data['checks'] if x['font_mode']=='system')
expanded=next(x for x in checks['records'] if x['variant']=='dark' and x['kind']=='expanded')
collapsed=next(x for x in checks['records'] if x['variant']=='dark' and x['kind']=='collapsed')
root=next(x for x in expanded['rows'] if x['id']==100)
assert root['label']=='Pendel' and root['paint']=='Pendel'
before=Image.open(HERE/'before-scene-tree.png').convert('RGB')
capture=json.loads((HERE/'before-capture.json').read_text(encoding='utf-8'))
assert hashlib.sha256(before.tobytes()).hexdigest()==capture['rgb_sha256']
def compare(name,rect,color):
    after=Image.open(HERE/name).convert('RGB');x,y,w,h=rect
    points=[(xx,yy) for yy in range(math.floor(y),math.ceil(y+h)) for xx in range(math.floor(x),math.ceil(x+w)) if max(abs(after.getpixel((xx,yy))[i]-color[i]) for i in range(3))<=40]
    assert len(points)>35,('positive native caption absent',name,len(points))
    same=sum(max(abs(before.getpixel(p)[i]-color[i]) for i in range(3))<=40 for p in points)
    assert same/len(points)<.3,('old image unexpectedly contains caption',name,same,len(points))
    return dict(positive_glyph_pixels=len(points),old_matching_pixels=same)
results=dict(root=compare(f'linux{stage}-system-dark-expanded.png',root['title'],(237,238,242)),footer=compare(f'linux{stage}-system-dark-collapsed.png',collapsed['options'],(166,169,177)))
assert len(checks['records'])==36 and len(checks['actions'])==6
assert len(data['checks'])==2
print(json.dumps(dict(scope=__doc__,passed=True,checks=results),indent=2))
