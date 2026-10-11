"""Decode saved runs independently and compare every exported energy balance row."""
import csv
import math
from pathlib import Path
import struct
import subprocess
import sys
import zlib

runner,analyzer,c_model,phys_model,c_analysis,c_doc_analysis,phys_analysis,c_probe,label_probe,work=sys.argv[1:]
work=Path(work);work.mkdir()
def command(*args):
    r=subprocess.run(list(map(str,args)),capture_output=True,timeout=90)
    assert r.returncode==0,(args,r.returncode,r.stdout,r.stderr)
def read(path):
    rows=[];names=[];finished=False
    with path.open('rb') as file:
        assert file.read(16)==b'PSRUN17\n'+struct.pack('<II',1,0x01020304)
        while header:=file.read(12):
            kind,size,crc=struct.unpack('<III',header);data=file.read(size)
            assert len(data)==size and zlib.crc32(data)==crc
            if kind==2:
                count=struct.unpack_from('<I',data)[0];assert count==11
                names=[data[4+i*167:52+i*167].split(b'\0')[0].decode() for i in range(count)]
            elif kind==3:rows.append(struct.unpack('<12d',data))
            elif kind==4:
                assert struct.unpack('<Q',data)[0]==len(rows);finished=True
    assert finished and len(rows)==2001
    return [{**dict(zip(names,row[1:])),'time':row[0]} for row in rows]
reference=None
for language,model in [('c',c_model),('phys',phys_model)]:
    run=work/(language+'.psrun')
    command(runner,model,run,'--steps','2000','--dt','.004','--param','integrator=2',
            '--param','mass=2','--param','airDensity=1.225','--param','area=.08','--param','dragCoefficient=.8')
    rows=read(run)
    initial=rows[0]['energy.balance'];last_loss=0
    for row in rows:
        assert row['energy.dissipated']>=last_loss
        assert row['energy.balance']==row['energy']+row['energy.dissipated']
        assert abs(row['energy.balance']-initial)<1e-8
        last_loss=row['energy.dissipated']
    assert rows[-1]['energy']<rows[0]['energy']*.9
    if reference is None:reference=rows
    else:
        assert all(abs(row[key]-ref[key])<2e-10 for row,ref in zip(rows,reference) for key in row)
    for analysis,module,probe in [('c',c_analysis,c_probe),('c-doc',c_doc_analysis,label_probe),('phys',phys_analysis,label_probe)]:
        prefix=work/(language+'-'+analysis)
        command(analyzer,module,run,prefix)
        if analysis=='c':command(probe,str(prefix)+'.psreport',run)
        else:command(probe,str(prefix)+'.psreport','Run 1 / RK4')
        path=Path(str(prefix)+('-balance.csv' if analysis=='c' else '-balance_1.csv'))
        with path.open(newline='',encoding='utf-8') as file:export=list(csv.reader(file))
        assert len(export)==len(rows)+1 and len(export[0])==5
        for row,actual in zip(rows,export[1:]):
            expected=[row['time'],row['energy'],row['energy.dissipated'],row['energy.balance'],row['energy.balance']-initial]
            assert all(math.isfinite(float(a)) and abs(float(a)-e)<1e-12 for a,e in zip(actual,expected))
print('Pendulum balance: two complete C/Physim runs, six mixed reports and 12006 full CSV rows passed')
