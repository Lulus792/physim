"""Replay actual failures and the final native input/raster evidence."""
from pathlib import Path
import gzip,json,sys
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[3]
sys.path.insert(0,str(ROOT/'tests'))
from check_graphic_raster import png
from check_transport_design import pixel,pixels

def steps(name):return json.loads(gzip.decompress((HERE/name).read_bytes()))
old=steps('linux4-timeline_context-steps.json.gz')[0]
assert old['exit_code']==1
assert 'AFTER OPEN: dragging=1 browsing=0 count=0' in old['stdout']
assert 'FINAL: dragging=0 browsing=1 count=0 time=5' in old['stdout']
for platform in ('windows5','linux5'):
    old=steps(platform+'-timeline_context-steps.json.gz')[0]
    assert old['exit_code']==1 and 'FINAL: dragging=0 browsing=0 count=0 time=8' in old['stdout']
for platform in ('windows7','linux7'):
    final=steps(platform+'-timeline_context-steps.json.gz')[0]
    assert final['exit_code']==0 and 'FINAL: dragging=0 browsing=0 count=0 time=0' in final['stdout']
    final=steps(platform+'-timeline_design-steps.json.gz')
    assert len(final)==2 and all(s['exit_code']==0 for s in final)
    for s in final:
        assert 'actual-focus-loss=1 popout=1 return=1' in s['stdout']
        assert s['stdout'].count('automatic-return=1 time-seek=1 preserved=1')==6
# True old pixels show the tooltip, inset range frame and extra separator rows.
background=(246,247,249);accent=(23,86,158)
assert pixel(png(HERE/'negative-tooltip.png'),311,140)!=background
assert pixel(png(HERE/'negative-range-inset.png'),350,90)==accent
for platform in ('linux7',):
    final=png(HERE/(platform+'-detached-browse.png'))
    assert pixel(final,311,140)==background
    assert pixel(final,350,90)==background
old=png(HERE/'negative-separator.png')
rows={y for x,y in pixels(old,(2,613,1076,9),accent,1)}
assert rows=={613,614,617,620,621},rows
for platform in ('windows7','linux7'):
    final=png(HERE/(platform+'-minimum.png'))
    assert {y for x,y in pixels(final,(2,613,1076,9),accent,1)}=={617}
# Keep the original one-pixel tolerance; source5 genuinely missed by two pixels.
from PIL import Image
im=Image.open(ROOT/'docs/visuals/unified-workspace/simulate-dark.png').convert('RGB')
reference=(im.width,im.height,im.tobytes(),None)
def box(im):
    p=pixels(im,(155,896,1273,24),(138,186,255),0)
    return min(x for x,y in p),min(y for x,y in p),max(x for x,y in p),max(y for x,y in p)
a=box(reference);b=box(png(HERE/'negative-range-start.png'));assert max(abs(x-y) for x,y in zip(a,b))==2
b=box(png(HERE/'windows7-dark.png'));assert max(abs(x-y) for x,y in zip(a,b))<=1
print('FROZEN TIMELINE COUNTEREXAMPLES: reproduced; final24 variant/font/platform input tasks passed')
