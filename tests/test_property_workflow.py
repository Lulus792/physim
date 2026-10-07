"""Read real C/Physim property runs and mixed analyses with independent SI oracles."""
import argparse,math,struct,subprocess,zlib
from pathlib import Path
p=argparse.ArgumentParser()
for name in ('runner','analysis','c-model','phys-model','c-analysis','phys-analysis','probe','work'):p.add_argument('--'+name,required=True)
a=p.parse_args();work=Path(a.work);work.mkdir(exist_ok=True,parents=True)
def run(args):
    r=subprocess.run(list(map(str,args)),capture_output=True,timeout=60)
    assert r.returncode==0,(args,r.returncode,r.stderr)
for label,module in [('c',a.c_model),('phys',a.phys_model)]:
    path=work/(label+'.psrun');run([a.runner,module,path,'--steps','4','--dt','.25','--seed','42'])
    rows=[];metadata='';footer=False
    with path.open('rb') as f:
        assert f.read(16)==b'PSRUN17\n'+struct.pack('<II',1,0x01020304)
        while h:=f.read(12):
            kind,size,crc=struct.unpack('<III',h);data=f.read(size);assert len(data)==size and zlib.crc32(data)==crc
            if kind==1:metadata=data.decode()
            if kind==2:
                assert size==4+3*167
                for i,(name,dim) in enumerate(zip(['temperature','pressure','density'],[(0,0,0,0,1,0,0),(-1,1,-2,0,0,0,0),(-3,1,0,0,0,0,0)])):
                    at=4+i*167;assert data[at:at+48].split(b'\0')[0].decode()==name
                    assert struct.unpack_from('<7b',data,at+160)==dim
            if kind==3:rows.append(struct.unpack('<4d',data))
            if kind==4:assert struct.unpack('<Q',data)[0]==5;footer=True
    assert footer and len(rows)==5
    for i,(time,t,pressure,density) in enumerate(rows):
        assert time==.25*i and t==273+100*time and pressure==500000*time
        assert abs(density-(1200-.2*t+1e-6*pressure))<5e-13
    for key in ['property.density.source=synthetic affine reference','property.domain=273..373 K; 0..500000 Pa','property.axes.temperature=273,373','property.values=1145.4,1145.9,1125.4,1125.9','property.interpolation=bilinear; no extrapolation']:
        assert key in metadata,key
    for analyzer,analysis in [('c',a.c_analysis),('phys',a.phys_analysis)]:
        prefix=work/(label+'-'+analyzer);run([a.analysis,analysis,path,prefix]);run([a.probe,str(prefix)+'.psreport'])
print('Property workflow: real C/Physim runs, stored source/domain/table metadata, independent SI oracle and four mixed reports passed')
