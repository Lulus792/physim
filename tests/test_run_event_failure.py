"""Real controlled-error and abrupt-abort prefixes preserve archived warnings."""
from pathlib import Path
import struct
import subprocess
import sys
import zlib
runner,module,probe,folder=sys.argv[1:];work=Path(folder)
for seed in (42,43):
    path=work/('failure-'+str(seed)+'.psrun')
    result=subprocess.run([runner,module,str(path),'--steps','2','--seed',str(seed)],capture_output=True,timeout=10)
    assert result.returncode!=0
    raw=path.read_bytes();at=16;events=[];kinds=[]
    while at<len(raw):
        kind,n,crc=struct.unpack_from('<III',raw,at);data=raw[at+12:at+12+n]
        assert len(data)==n and zlib.crc32(data)==crc;kinds.append(kind)
        if kind==8:events.append(data[16:].decode('utf-8'))
        at+=12+n
    assert at==len(raw) and 4 not in kinds
    assert events==['initial event','before failure α']+(['destroy after failure'] if seed==42 else [])
    r=subprocess.run([probe,str(path),'events'],capture_output=True,text=True,timeout=5)
    assert r.returncode==0 and r.stdout.startswith('status 7 count '+str(len(events))+' samples 1 complete 0'),r.stdout
print('Runner event prefixes: controlled error/destroy and abrupt abort retain CRC-protected events with recovered status')
