"""Independent .psrun event decoder and CRC-preserving malformed-file probes."""
from pathlib import Path
import struct
import subprocess
import sys
import zlib
work, probe = Path(sys.argv[1]), sys.argv[2]
def chunks(raw):
    assert raw[:16] == b'PSRUN17\n'+struct.pack('<II',1,0x01020304)
    at=16; result=[]
    while at<len(raw):
        kind,n,crc=struct.unpack_from('<III',raw,at); data=raw[at+12:at+12+n]
        assert len(data)==n and n<=8192 and zlib.crc32(data)==crc
        result.append((kind,data)); at+=12+n
    assert at==len(raw)
    return result
def wire(chunks):
    return b'PSRUN17\n'+struct.pack('<II',1,0x01020304)+b''.join(struct.pack('<III',k,len(p),zlib.crc32(p))+p for k,p in chunks)
def inspect(path):
    result=chunks(path.read_bytes());events=[]; samples=[]
    for kind,data in result:
        if kind==8:
            version,level,time=struct.unpack_from('<IId',data)
            assert version==1 and 1<=level<=4 and len(data)>16 and b'\0' not in data[16:]
            events.append((level,time,data[16:].decode('utf-8')))
        elif kind==3:samples.append(struct.unpack('<dd',data))
    assert events==[(3,.125,'warning α\nline\t☃'),(4,.25,'finished')]
    assert samples==[(0,7)] and result[-1]==(4,struct.pack('<Q',1))
    return result
c=inspect(work/'events-c.psrun'); language=inspect(work/'events-language.psrun')
assert c==language, 'C and Physim must emit byte-identical metadata/schema/events/index/footer'
def run(path,mode,expected):
    r=subprocess.run([probe,str(path),mode],capture_output=True,text=True,timeout=5)
    assert r.returncode==0,(r.stdout,r.stderr)
    words=r.stdout.split();assert words[:2]==['status',str(expected)],(path,mode,r.stdout)
    return words
assert run(work/'events-c.psrun','events',6)[2:]==['count','2','samples','1','complete','1']
assert run(work/'events-c.psrun','samples',6)[2:]==['count','1','samples','1','complete','1']
first=next(i for i,(k,p) in enumerate(c) if k==8); payload=c[first][1]
variants={
    'empty':(payload[:16],5),
    'bad-level':(payload[:4]+struct.pack('<I',5)+payload[8:],5),
    'nan-time':(payload[:8]+struct.pack('<d',float('nan'))+payload[16:],5),
    'nul':(payload+b'\0',5),
    'utf8':(payload[:16]+b'\xff',5),
    'too-long':(payload[:16]+b'x'*1025,5),
    'version':(struct.pack('<I',2)+payload[4:],4),
}
for name,(p,status) in variants.items():
    altered=c.copy();altered[first]=(8,p);path=work/(name+'.psrun');path.write_bytes(wire(altered))
    run(path,'events',status);run(path,'samples',status)
# CRC errors/truncation stop at the prefix. Unknown chunks remain skippable.
raw=wire(c); offset=16+sum(12+len(p) for k,p in c[:first]);bad=bytearray(raw);bad[offset+8]^=1
for name,data in [('crc',bad),('truncated',raw[:offset+13])]:
    path=work/(name+'.psrun');path.write_bytes(data);run(path,'events',7);run(path,'samples',7)
extra=c.copy();extra.insert(first,(0x12345678,b'future extension'));path=work/'unknown.psrun';path.write_bytes(wire(extra));run(path,'events',6);run(path,'samples',6)
path=work/'incomplete.psrun';path.write_bytes(wire(c[:-1]));assert run(path,'events',7)[2:4]==['count','2']
print('Independent event codec: exact C/Physim bytes, version/CRC/UTF-8/level/time/size errors, incomplete tail and unknown chunks passed')
