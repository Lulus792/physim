"""Independent UTC epoch/wire/CRC reference, including strictly normalized replay."""
from pathlib import Path
import struct
import subprocess
import sys
import time
import zlib
from run_archive_reference import execution_independent_bytes,start_time
work=Path(sys.argv[1]);probe=sys.argv[2]
a=(work/'start-c.psrun').read_bytes();b=(work/'start-language.psrun').read_bytes();assert execution_independent_bytes(a)==execution_independent_bytes(b)
now=time.time_ns()
for raw in (a,b):
    sec,nanos=start_time(raw);assert now-120_000_000_000<=sec*1_000_000_000+nanos<=now+1_000_000_000
_,n,_=struct.unpack_from('<III',a,16);metadata=a[28:28+n]
def checked(name,payload,expected):
    path=work/(name+'.psrun');path.write_bytes(a[:16]+struct.pack('<III',1,len(payload),zlib.crc32(payload))+payload+a[28+n:])
    r=subprocess.run([probe,str(path)],capture_output=True,text=True,timeout=5);assert r.returncode==0 and r.stdout.strip()==expected,(name,r.stdout,r.stderr)
lines=metadata.splitlines();sec=next(l for l in lines if l.startswith(b'start_unix_seconds='));ns=next(l for l in lines if l.startswith(b'start_nanoseconds='))
checked('missing',metadata.replace(sec+b'\n',b'').replace(ns+b'\n',b''),'status 6')
checked('partial',metadata.replace(ns+b'\n',b''),'status 5')
checked('duplicate',metadata+ns+b'\n','status 5')
checked('overflow',metadata.replace(sec,b'start_unix_seconds=+9223372036854775808'),'status 5')
checked('nanos-overflow',metadata.replace(ns,b'start_nanoseconds=1000000000'),'status 5')
checked('epoch-min',metadata.replace(sec,b'start_unix_seconds=-9223372036854775808').replace(ns,b'start_nanoseconds=999999999'),'time -9223372036854775808 999999999')
checked('embedded-nul',metadata+b'\0hidden','open 5')
# The reference normalizer must still detect numeric changes.
changed=bytearray(a);at=16
while at<len(a):
    kind,n,_=struct.unpack_from('<III',a,at)
    if kind==3:
        payload=struct.pack('<dd',0,8);changed[at:at+12+n]=struct.pack('<III',kind,n,zlib.crc32(payload))+payload;break
    at+=12+n
assert execution_independent_bytes(changed)!=execution_independent_bytes(a)
print('Independent start-time codec: real OS UTC bounds, exact C/Physim payload/index/footer parity, corrupted/legacy/extreme timestamps and preserved numeric differences passed')
