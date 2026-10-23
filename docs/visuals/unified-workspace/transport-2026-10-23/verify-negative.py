"""Replay frozen semantic counterexamples and check final native action records."""
from pathlib import Path
import gzip,json,re,sys
HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[3]
sys.path.insert(0,str(ROOT/'tests'))
from check_transport_design import pixels
from check_graphic_raster import png

def gz(name):return gzip.decompress((HERE/name).read_bytes()).decode('utf-8',errors='replace')
for platform in ('windows','linux'):
    steps=json.loads(gz(platform+'-source2-transport_design-steps.json.gz'))
    assert len(steps)==1 and steps[0]['exit_code']==1
    assert 'failed at stage 28' in steps[0]['stderr']
model=gz('transport-gdb-source2-model.log.gz')
rows=re.findall(r'NODE id=\d+ role=1 enabled=(\d) focusable=(\d) window=laboratory-transport/menu/\w+ label=([^\r\n]+)',model)
assert len(rows)==8 and rows[0][:2]==('1','1') and all(r[:2]==('0','0') for r in rows[1:]),rows
assert rows[2][2]=='1× Echtzeit'
assert 'enabled-before=1 routed=0' in gz('transport-gdb-source2-emit.log.gz')
assert 'TRANSPORT DEBUG AFTER: open=1' in gz('transport-gdb-source2.log.gz')
for platform in ('windows-release','linux-sanitized'):
    steps=json.loads(gz(platform+'-transport_design-steps.json.gz'))
    assert len(steps)==2 and all(s['exit_code']==0 for s in steps)
    for step in steps:
        assert 'TRANSPORT DESIGN SELF-TEST: PASSED' in step['stdout']
        assert len(re.findall(r'TRANSPORT ACTIONS variant=\w+ native-enter=1 step-dt=1 offline=1 native-menu=1 escape-return=1 source=1',step['stdout']))==6
# The Windows reset outline is visible with partial antialias coverage. Reject
# a blank nearby area as well, so the revised threshold cannot pass bare fill.
validation=json.loads((HERE/'windows-release-transport.json').read_text(encoding='utf-8'))
record=validation['checks'][0]['records'][1];image=png(HERE/'windows-release-system-dark-normal.png')
rect=record['icon'][3]
assert len(pixels(image,rect,(237,238,242),55))==1
assert len(pixels(image,rect,(237,238,242),90))>=2
assert not pixels(image,(rect[0],180,rect[2],rect[3]),(237,238,242),90)
for name in ('lifecycle-current','lifecycle-old-debug'):
    assert 'role=50004 found=0 hr=0x00000000' in gz(name+'-probe.stderr.gz')
print('FROZEN TRANSPORT COUNTEREXAMPLES: reproduced; final24 variant/font/platform action tasks passed')
