"""Independent archive/CRC reference for actual C/Physim pendulum layer declarations."""
from pathlib import Path
import math
import struct
import subprocess
import sys
import zlib
runner,c_model,phys_model,work=sys.argv[1:];work=Path(work);work.mkdir(parents=True,exist_ok=True)
keys=('geometry','laws','medium','losses','numerics','measurement','sampling')
for case,(density,coefficient,area,noise) in enumerate(((0,.47,.01,0),(1.225,.47,.01,0),(0,.47,.01,.02),(1.225,0,.01,.02))):
    rows=[];layers=[]
    for language,model in (('c',c_model),('phys',phys_model)):
        path=work/f'{case}-{language}.psrun';parameters={'length':1.2,'mass':2,'initialAngle':.3,'airDensity':density,'dragCoefficient':coefficient,'area':area,'sensorNoise':noise,'integrator':2}
        command=[runner,model,str(path),'--steps','10','--dt','.005','--seed','42']
        for key,value in parameters.items():command+=['--param',f'{key}={value}']
        p=subprocess.run(command,capture_output=True,timeout=20);assert p.returncode==0,(p.stdout,p.stderr)
        raw=path.read_bytes();assert raw[:16]==b'PSRUN17\n'+struct.pack('<II',1,0x01020304)
        at=16;metadata=None;values=[];complete=False
        while at<len(raw):
            kind,n,crc=struct.unpack_from('<III',raw,at);payload=raw[at+12:at+12+n];assert n<=8192 and len(payload)==n and zlib.crc32(payload)==crc
            if kind==1:metadata=dict(x.split('=',1) for x in payload.decode().splitlines() if '=' in x)
            if kind==3:values.append(struct.unpack('<'+'d'*(n//8),payload))
            if kind==4:assert struct.unpack('<Q',payload)[0]==len(values);complete=True
            at+=12+n
        assert complete and len(values)==11
        declared={k:metadata['layer.'+k] for k in keys};layers.append(declared);rows.append(values)
        expected={'geometry':'ideal','laws':'active','medium':'active' if density else 'omitted','losses':'active' if density and coefficient and area else 'omitted','numerics':'active','measurement':'active' if noise else 'ideal','sampling':'active' if noise else 'omitted'}
        assert {k:v.split('|',1)[0] for k,v in declared.items()}==expected
        assert all('|' in v and v.split('|',1)[1] for v in declared.values())
        assert 'externally' in declared['sampling'] and metadata['parameter.mass']=='2' and metadata['parameter_unit.mass']=='kg'
    assert layers[0]==layers[1]
    for a,b in zip(*rows):
        assert len(a)==len(b) and all(math.isclose(x,y,rel_tol=2e-12,abs_tol=2e-12) for x,y in zip(a,b))
print('Model layers: eight complete C/Physim archives; vacuum, drag, exact/noisy measurement and explicit model-only sampling; CRC, parameters and unchanged trajectory parity passed')
