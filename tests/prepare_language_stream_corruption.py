"""Make a damaged tail after two valid rows, without using Physim's decoder."""
from pathlib import Path
import struct
import sys
import zlib
root=Path(sys.argv[1]);data=bytearray((root/'stream α.psrun').read_bytes())
assert data[:16]==b'PSRUN17\n'+struct.pack('<II',1,0x01020304)
at=16;rows=0;corrupted=False
while at<len(data):
    kind,size,crc=struct.unpack_from('<III',data,at);payload=data[at+12:at+12+size]
    assert len(payload)==size and zlib.crc32(payload)==crc
    if kind==3:
        rows+=1
        if rows==3:data[at+12]^=1;corrupted=True
    at+=12+size
assert at==len(data) and rows==3 and corrupted
(root/'damaged.psrun').write_bytes(data)
