"""Independent wire reference: ignore only the two validated execution-clock values."""
import re
import struct
import zlib

def start_time(raw):
    assert raw[:16]==b'PSRUN17\n'+struct.pack('<II',1,0x01020304)
    kind,n,crc=struct.unpack_from('<III',raw,16);payload=raw[28:28+n]
    assert kind==1 and len(payload)==n and zlib.crc32(payload)==crc
    seconds=re.findall(rb'(?:\A|[\r\n])start_unix_seconds=([+-][0-9]{19})(?=[\r\n]|\Z)',payload)
    nanos=re.findall(rb'(?:\A|[\r\n])start_nanoseconds=([0-9]{9})(?=[\r\n]|\Z)',payload)
    if not seconds and not nanos:return None
    assert len(seconds)==len(nanos)==1
    sec,ns=int(seconds[0]),int(nanos[0]);assert -(1<<63)<=sec<(1<<63) and 0<=ns<1000000000
    return sec,ns

def execution_independent_bytes(raw):
    captured=start_time(raw);at=16;output=bytearray(raw[:16])
    while at<len(raw):
        kind,n,crc=struct.unpack_from('<III',raw,at);payload=raw[at+12:at+12+n]
        assert n<=8192 and len(payload)==n and zlib.crc32(payload)==crc
        if kind==1 and captured is not None:
            payload,count=re.subn(rb'(?m)(^start_unix_seconds=)[+-][0-9]{19}',rb'\g<1>+0000000000000000000',payload);assert count==1
            payload,count=re.subn(rb'(?m)(^start_nanoseconds=)[0-9]{9}',rb'\g<1>000000000',payload);assert count==1
            assert len(payload)==n # Preserve every absolute index offset.
        output+=struct.pack('<III',kind,n,zlib.crc32(payload))+payload;at+=12+n
    assert at==len(raw)
    return bytes(output)
