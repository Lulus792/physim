"""Compare real C/Physim runs, complete measurements and recorded scene bytes."""
from pathlib import Path
import math,struct,subprocess,sys,zlib
runner,c_module,phys_module,work=sys.argv[1:]
work=Path(work)
def read(path):
    samples=[];scenes=[];metadata=None;schema=footer=False
    with path.open('rb') as f:
        assert f.read(16)==b'PSRUN17\n'+struct.pack('<II',1,0x01020304)
        while header:=f.read(12):
            kind,size,crc=struct.unpack('<III',header);data=f.read(size)
            assert len(data)==size and zlib.crc32(data)==crc
            if kind==1:metadata=dict(line.split('=',1) for line in data.decode().splitlines() if '=' in line)
            if kind==2:assert struct.unpack_from('<I',data)[0]==11;schema=True
            if kind==4:assert struct.unpack('<Q',data)[0]==1001;footer=True
            if kind==3:samples.append(struct.unpack('<12d',data))
            if kind==5:scenes.append(data)
    assert metadata is not None and schema and footer
    return samples,scenes,metadata
for mode,iterations in ((0,4),(1,4),(.5,1),(.5,16)):
    runs=[]
    for name,module in (('c',c_module),('phys',phys_module)):
        path=work/f'{name}-{mode}-{iterations}.psrun'
        subprocess.run([runner,module,str(path),'--steps','1000','--dt','.005','--param',f'warmStart={mode}','--param',f'iterations={iterations}','--seed','42','--record-scenes'],check=True,capture_output=True,timeout=45)
        runs.append(read(path))
    (a,sa,ma),(b,sb,mb)=runs
    for metadata in (ma,mb):
        for name,value,default,low,high,key in [('warmStart',mode,1,0,1,'warm_start_fraction'),('iterations',iterations,4,1,256,'solver_iterations')]:
            assert float(metadata[f'parameter.{name}'])==value==float(metadata[key])
            assert float(metadata[f'parameter_default.{name}'])==default
            assert float(metadata[f'parameter_min.{name}'])==low and float(metadata[f'parameter_max.{name}'])==high
            assert metadata[f'parameter_unit.{name}']=='1' and metadata[f'parameter_dimension.{name}']=='0,0,0,0,0,0,0'
            assert float(metadata[f'parameter_scale.{name}'])==1
        assert float(metadata['dt_s'])==.005 and int(metadata['seed'])==42
        for key,value in [('box_mass_kg',1),('box_side_m',1),('restitution',0),('friction',.5),('bounce_threshold_m_s',.5),('penetration_slop_m',.00001),('correction_fraction',.8)]:
            assert float(metadata[key])==value

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
for name,module in (('c',c_module),('phys',phys_module)):
    for setting in ('warmStart=-.1','warmStart=1.1','iterations=0','iterations=257','iterations=1.5'):
        path=work/f'invalid-{name}-{setting}.psrun'
        result=subprocess.run([runner,module,str(path),'--steps','1','--param',setting],capture_output=True,timeout=45)
        assert result.returncode==5 and not path.exists(),(name,setting,result.returncode,result.stderr)
print('ContactWorld C/Physim: eight complete 1001-row runs, matching scenes, SI/solver provenance and ten preflight rejections passed')
