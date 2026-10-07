"""Independent schema/sample/CSV checks for actual C and Physim SI declarations."""
import argparse,csv,struct,subprocess,zlib
from pathlib import Path
p=argparse.ArgumentParser()
for name in ('runner','c-model','phys-model','exporter','work'):p.add_argument('--'+name,required=True)
a=p.parse_args();work=Path(a.work);work.mkdir(exist_ok=True,parents=True)
for label,module in [('c',a.c_model),('phys',a.phys_model)]:
    path=work/(label+'.psrun');out=work/(label+'.csv')
    r=subprocess.run([a.runner,module,str(path),'--steps','2','--dt','.25'],capture_output=True,timeout=30)
    assert r.returncode==0,(r.returncode,r.stderr)
    rows=[];schema=False;footer=False;metadata=''
    with path.open('rb') as f:
        assert f.read(16)==b'PSRUN17\n'+struct.pack('<II',1,0x01020304)
        while h:=f.read(12):
            kind,size,crc=struct.unpack('<III',h);data=f.read(size);assert len(data)==size and zlib.crc32(data)==crc
            if kind==1:metadata=data.decode()
            if kind==2:
                assert size==338 and struct.unpack_from('<I',data)[0]==2
                for i,(name,unit,dimensions) in enumerate([('length','m',(1,0,0,0,0,0,0)),('after','1',(0,0,0,0,0,0,0))]):
                    at=4+167*i;assert data[at:at+48].split(b'\0')[0].decode()==name
                    assert data[at+48:at+64].split(b'\0')[0].decode()==unit
                    assert struct.unpack_from('<7b',data,at+160)==dimensions
                schema=True
            if kind==3:rows.append(struct.unpack('<3d',data))
            if kind==4:assert struct.unpack('<Q',data)[0]==3;footer=True
    assert schema and footer and rows==[(0,1.25,7),(.25,1.25,7),(.5,1.25,7)]
    assert 'input=125 cm\nstored=1.25 m' in metadata
    r=subprocess.run([a.exporter,str(path),str(out)],capture_output=True,timeout=30)
    assert r.returncode==0,(r.returncode,r.stderr)
    with out.open(newline='') as f:data=list(csv.reader(f))
    assert data[0]==['time [s]','length [m]','after [1]'],data[0]
    assert [tuple(map(float,row)) for row in data[1:]]==rows
print('Channel declarations: C/Physim caught rejections, compact indices, actual SI schemas, values and CSV parity passed')
