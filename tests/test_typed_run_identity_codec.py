"""Independent wire check for typed C/handle/Physim identity writers."""
from pathlib import Path
import struct
import subprocess
import sys
import zlib
work=Path(sys.argv[1]);probe=sys.argv[2];files=[work/name for name in ('typed-c.psrun','typed-handle-c.psrun','typed-language.psrun')]
raw=files[0].read_bytes();assert all(p.read_bytes()==raw for p in files[1:])
assert raw[:16]==b'PSRUN17\n'+struct.pack('<II',1,0x01020304)
at=16;rows=0;metadata=None;footer=False
while at<len(raw):
    kind,n,crc=struct.unpack_from('<III',raw,at);data=raw[at+12:at+12+n]
    assert len(data)==n and n<=8192 and zlib.crc32(data)==crc
    if kind==1:metadata=data.decode('utf-8').splitlines()
    elif kind==3:assert struct.unpack('<dd',data)==(0,7);rows+=1
    elif kind==4:assert struct.unpack('<Q',data)==(rows,);footer=True;assert at+12+n==len(raw)
    at+=12+n
assert footer and rows==1
assert metadata.count('project_id=0123456789abcdef0123456789abcdef')==1
assert metadata.count('experiment_id=fedcba9876543210fedcba9876543210')==1
assert not (work/'collision-language.psrun').exists()
print('Independent typed identity codec: exact C/handle/Physim files, canonical single IDs, CRC/index/footer and collision non-creation passed')

kind,n,crc=struct.unpack_from('<III',raw,16);original=raw[28:28+n]
project=b'project_id=0123456789abcdef0123456789abcdef'
experiment=b'experiment_id=fedcba9876543210fedcba9876543210'
variants={
 'duplicate':(original+project+b'\n','identity 5'),
 'uppercase-key':(original.replace(project,project.upper()),'identity 6'),
 'uppercase-value':(original.replace(project,b'project_id='+project.split(b'=')[1].upper()),'identity 5'),
 'invalid-utf8':(original.replace(project,b'project_id='+b'\xff'*32),'identity 5'),
 'bad-value':(original.replace(project,b'project_id=BAD'),'identity 5'),
 'unavailable':(original.replace(project,b'project_id=unavailable').replace(experiment,b'experiment_id=unavailable'),'identity 6'),
 'missing':(original.replace(project+b'\n',b'').replace(experiment+b'\n',b''),'identity 6'),
 'nul':(original+b'\0hidden','open 5'),
}
for name,(payload,expected) in variants.items():
    path=work/(name+'-identity.psrun');path.write_bytes(raw[:16]+struct.pack('<III',1,len(payload),zlib.crc32(payload))+payload+raw[28+n:])
    r=subprocess.run([probe,str(path)],capture_output=True,text=True,timeout=5)
    assert r.returncode==0 and r.stdout.strip()==expected,(name,r.stdout,r.stderr)
print('Independent CRC-correct metadata mutations: duplicate/bad/unavailable/absent/embedded-NUL identity behavior and output preservation passed')

future=(work/'typed-future.psrun').read_bytes();_,n,crc=struct.unpack_from('<III',future,16);payload=future[28:28+n]
assert zlib.crc32(payload)==crc
lines=payload.decode().splitlines()
assert lines.count('project_id=fedcba9876543210fedcba9876543210')==1
assert lines.count('experiment_id=0123456789abcdef0123456789abcdef')==1
print('Future-only identity configuration preserves existing archives and changes subsequent writers')
