"""Compare real C/Physim runs, complete measurements and recorded scene bytes."""
from pathlib import Path
import math,struct,subprocess,sys,zlib
runner,c_module,phys_module,work=sys.argv[1:]
work=Path(work)
def read(path):
    samples=[];scenes=[]
    with path.open('rb') as f:
        assert f.read(16)==b'PSRUN17\n'+struct.pack('<II',1,0x01020304)
        while header:=f.read(12):
            kind,size,crc=struct.unpack('<III',header);data=f.read(size)
            assert len(data)==size and zlib.crc32(data)==crc
            if kind==3:samples.append(struct.unpack('<12d',data))
            if kind==5:scenes.append(data)
    return samples,scenes
for mode in (0,1):
    runs=[]
    for name,module in (('c',c_module),('phys',phys_module)):
        path=work/f'{name}-{mode}.psrun'
        subprocess.run([runner,module,str(path),'--steps','1000','--dt','.005','--param',f'warmStart={mode}','--record-scenes'],check=True,capture_output=True,timeout=45)
        runs.append(read(path))
    (a,sa),(b,sb)=runs
    assert len(a)==len(b)==1001 and len(sa)>100 and len(sa)==len(sb)
    for i,(ra,rb) in enumerate(zip(a,b)):
        for j,(x,y) in enumerate(zip(ra,rb)):
            assert math.isfinite(x) and math.isfinite(y) and abs(x-y)<=1e-12*max(1,abs(x),abs(y)),(mode,i,j,x,y)
    for x,y in zip(sa,sb):
        # Wire header, channel values and canonical geometry: rotated scene
        # constructors normalize quaternions, so compare finite doubles by
        # the same tolerance as measurements rather than raw float bits.
        assert len(x)==len(y) and x[:28]==y[:28]
        channels,objects=struct.unpack_from('<II',x,12)
        first=28+channels*8
        for offset in range(28,first,8):
            a,b=struct.unpack_from('<d',x,offset)[0],struct.unpack_from('<d',y,offset)[0]
            assert abs(a-b)<=1e-12*max(1,abs(a),abs(b))
        for i in range(objects):
            offset=first+i*176
            assert x[offset:offset+8]==y[offset:offset+8]
            for field in range(8,96,8):
                a,b=struct.unpack_from('<d',x,offset+field)[0],struct.unpack_from('<d',y,offset+field)[0]
                assert math.isfinite(a) and math.isfinite(b) and abs(a-b)<=1e-12*max(1,abs(a),abs(b)),(mode,i,field,a,b)
            assert x[offset+96:offset+176]==y[offset+96:offset+176]
        assert x[first+objects*176:]==y[first+objects*176:]
print('ContactWorld C/Physim parity: 1001 complete measurements and matching complete scenes in both warm/cold modes passed')
