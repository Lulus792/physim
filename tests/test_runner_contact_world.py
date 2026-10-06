"""Real automatic-contact simulation, independent format parser and warm/cold comparison."""
from pathlib import Path
import math
import struct
import subprocess
import sys
import zlib
runner,module,work=sys.argv[1:]
work=Path(work)
results={}
for mode,enabled in (('warm',1),('cold',0)):
    path=work/(mode+'.psrun')
    process=subprocess.run([runner,module,str(path),'--steps','1000','--dt','.005','--param',f'warmStart={enabled}','--record-scenes'],capture_output=True,timeout=40)
    assert process.returncode==0,process.stderr
    rows=[];snapshots=0;footer=False;index=False;metadata=None
    with path.open('rb') as file:
        assert file.read(16)==b'PSRUN17\n'+struct.pack('<II',1,0x01020304)
        while header:=file.read(12):
            kind,size,crc=struct.unpack('<III',header);data=file.read(size)
            assert size<=8192 and len(data)==size and zlib.crc32(data)==crc
            if kind==1:metadata=data.decode('utf-8')
            elif kind==2:assert struct.unpack_from('<I',data)[0]==11
            elif kind==3:
                row=struct.unpack('<12d',data);assert all(math.isfinite(x) for x in row);rows.append(row)
            elif kind==5:
                assert struct.unpack_from('<I',data)[0]==3
                channels,objects=struct.unpack_from('<II',data,12);assert channels==11 and 5<=objects<=25
                snapshots+=1
            elif kind==7:index=True
            elif kind==4:assert struct.unpack('<Q',data)[0]==1001;footer=True
    assert footer and index and len(rows)==1001 and snapshots>100
    assert 'contacts=automatic sphere/box/plane discrete' in metadata and f'parameter.warmStart={enabled}\n' in metadata
    for i,row in enumerate(rows):
        assert row[0]==i*.005 and .45<row[1]<.55 and 3.4<row[2]<3.6 and row[4]>=0
        assert 0<=row[7]<=row[6]<=row[5]<=512 and row[8]==row[5]-row[6] and 0<=row[9]<=512
    if enabled:assert sum(row[7] for row in rows)>1000
    else:assert all(row[7]==0 for row in rows)
    results[mode]=sum(row[10] for row in rows[201:])/800
assert results['warm']<.85*results['cold'],results
print('Contact stack: 1001 rows in each mode, valid recorded geometry, lifecycle, canonical index and lower warm residual passed',results)
