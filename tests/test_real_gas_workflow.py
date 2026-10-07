"""Independent 60-digit molar vdW oracle for real runs and mixed reports."""
import argparse,struct,subprocess,zlib
from decimal import Decimal,localcontext
from pathlib import Path
p=argparse.ArgumentParser()
for name in ('runner','analysis','c-model','phys-model','c-analysis','phys-analysis','probe','work'):p.add_argument('--'+name,required=True)
a=p.parse_args();work=Path(a.work);work.mkdir(exist_ok=True,parents=True)
def run(args):
    r=subprocess.run(list(map(str,args)),capture_output=True,timeout=60)
    assert r.returncode==0,(args,r.returncode,r.stderr)
def reference(time):
    with localcontext() as ctx:
        ctx.prec=60;D=Decimal;R=D('8.31446261815324');T=D(450);A=D('.4');B=D('.00004');V=D('.005')-D('.004')*min(D(1),D(str(time)));W=V-B
        return list(map(float,[V,R*T/V,R*T/W-A/V**2,D('20.8')*T,D('20.8')*T-A/V,R*(W/(D('.005')-B)).ln(),-R*T/W**2+2*A/V**3]))
names=['volume','ideal_pressure','vdw_pressure','ideal_energy','vdw_energy','entropy_change','pressure_derivative']
# Full per-channel SI dimensions, including derivative in Pa/m³.
dimensions=[(3,0,0,0,0,0,0),(-1,1,-2,0,0,0,0),(-1,1,-2,0,0,0,0),(2,1,-2,0,0,0,0),(2,1,-2,0,0,0,0),(2,1,-2,0,-1,0,0),(-4,1,-2,0,0,0,0)]
for label,module in [('c',a.c_model),('phys',a.phys_model)]:
    path=work/(label+'.psrun');run([a.runner,module,path,'--steps','6','--dt','.25','--seed','42'])
    rows=[];metadata='';schema=False;footer=False
    with path.open('rb') as f:
        assert f.read(16)==b'PSRUN17\n'+struct.pack('<II',1,0x01020304)
        while h:=f.read(12):
            kind,size,crc=struct.unpack('<III',h);data=f.read(size);assert len(data)==size and zlib.crc32(data)==crc
            if kind==1:metadata=data.decode()
            if kind==2:
                assert size==4+7*167 and struct.unpack_from('<I',data)[0]==7
                for i,(name,dim) in enumerate(zip(names,dimensions)):
                    at=4+i*167;assert data[at:at+48].split(b'\0')[0].decode()==name
                    assert struct.unpack_from('<7b',data,at+160)==dim
                schema=True
            if kind==3:rows.append(struct.unpack('<8d',data))
            if kind==4:assert struct.unpack('<Q',data)[0]==7;footer=True
    assert schema and footer and len(rows)==7
    for i,row in enumerate(rows):
        assert row[0]==.25*i
        for name,actual,expected in zip(names,row[1:],reference(row[0])):
            assert abs(actual-expected)<=3e-13*max(1,abs(expected)),(label,name,row[0],actual,expected)
        assert row[3]>0 and row[-1]<0
    for key in ['source=synthetic coefficients; not a calibrated gas','attraction=0.4 Pa m6/mol2','covolume=0.00004 m3/mol','molar_cv=20.8 J/(mol K)','excluded=phase coexistence,Maxwell construction,latent heat,temperature-dependent coefficients']:
        assert key in metadata,key
    for analyzer,analysis in [('c',a.c_analysis),('phys',a.phys_analysis)]:
        prefix=work/(label+'-'+analyzer);run([a.analysis,analysis,path,prefix]);run([a.probe,str(prefix)+'.psreport'])
print('Real gas: independent Decimal oracle, seven SI channels, coefficient metadata, clamped compression and four mixed reports passed')
