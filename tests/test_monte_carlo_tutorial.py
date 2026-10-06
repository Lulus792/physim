"""Independent seeded draws, exact motion, full archived ensembles and confidence reports."""
from pathlib import Path
import csv,math,statistics,struct,subprocess,sys,zlib
runner,batch,analysis,c_model,phys_model,c_analysis,phys_analysis,probe,work=sys.argv[1:]
work=Path(work)
def command(args):
    result=subprocess.run(list(map(str,args)),capture_output=True,timeout=180)
    assert result.returncode==0,(args,result.returncode,result.stderr)
    return result

def read(path):
    rows=[];scenes=[];metadata='';footer=False
    with path.open('rb') as f:
        assert f.read(16)==b'PSRUN17\n'+struct.pack('<II',1,0x01020304)
        while h:=f.read(12):
            kind,size,crc=struct.unpack('<III',h);data=f.read(size)
            assert len(data)==size and zlib.crc32(data)==crc
            if kind==1:metadata=data.decode()
            if kind==2:
                assert size==4+9*167
                names=['position.x','position.y','velocity.x','velocity.y','nominal.x','nominal.y','uncertainty.x','uncertainty.y','energy']
                for i,name in enumerate(names):
                    at=4+i*167;assert data[at:at+48].split(b'\0')[0].decode()==name
                    dim=(1,0,-1) if i in (2,3) else (2,1,-2) if i==8 else (1,0,0)
                    assert struct.unpack_from('<7b',data,at+160)==dim+(0,0,0,0)
            if kind==3:rows.append(struct.unpack('<10d',data))
            if kind==4:assert struct.unpack('<Q',data)[0]==len(rows);footer=True
            if kind==5:
                assert struct.unpack_from('<I',data)[0]==3 and struct.unpack_from('<I',data,12)[0]==9
                scenes.append(data)
    assert footer and rows
    return rows,scenes,metadata

def near(x,y,tol=2e-12):return math.isfinite(x) and math.isfinite(y) and abs(x-y)<=tol*max(1,abs(x),abs(y))
class OracleRng:
    def __init__(self,seed):
        self.state=0;self.next();self.state=(self.state+seed)&((1<<64)-1);self.next()
    def next(self):
        old=self.state;self.state=(old*6364136223846793005+1442695040888963407)&((1<<64)-1)
        value=(((old>>18)^old)>>27)&0xffffffff;rot=old>>59
        return ((value>>rot)|(value<<((-rot)&31)))&0xffffffff
    def normal(self,mean,sigma):
        if not sigma:return mean
        u=(self.next()+.5)/4294967296;v=(self.next()+.5)/4294967296
        return mean+sigma*math.sqrt(-2*math.log(u))*math.cos(2*math.pi*v)

def verify(path,seed,sx=.15,sy=.25,mx=3,my=5):
    rows,scenes,metadata=read(path);rng=OracleRng(seed);vx=rng.normal(mx,sx);vy=rng.normal(my,sy)
    assert f'seed={seed}\n' in metadata and 'integrator=exact ballistic propagation' in metadata
    for key in ('meanVx','meanVy','sigmaVx','sigmaVy'):
        assert f'parameter_unit.{key}=m/s\n' in metadata and f'parameter_dimension.{key}=1,0,-1,0,0,0,0\n' in metadata
    for row in rows:
        t,x,y,ux,uy,nx,ny,sigx,sigy,e=row
        expected=(-2+vx*t,vy*t-.5*9.80665*t*t,vx,vy-9.80665*t,-2+mx*t,my*t-.5*9.80665*t*t,sx*t,sy*t,.5*(vx*vx+vy*vy))
        assert all(near(a,b) for a,b in zip(row[1:],expected)),(seed,row,expected)
    return rows,scenes

series={};endpoints={}
for language,module in [('c',c_model),('phys',phys_model)]:
    for workers in (1,4):
        root=work/f'{language}-workers-{workers}'
        command([batch,runner,module,root,'position.x','256','32','.03125','42','--workers',str(workers),'--until','1'])
        with (root/'endpoints.csv').open(newline='') as f:journal=list(csv.DictReader(f))
        assert len(journal)==256
        values=[]
        for i,line in enumerate(journal):
            rows,_=verify(root/f'run-{i+1:04d}.psrun',42+i)
            assert len(rows)==33 and rows[-1][0]==1 and int(line['seed'])==42+i and int(line['status'])==1
            assert near(float(line['value']),rows[-1][1]);values.append((rows[-1][1],rows[-1][2]))
        endpoints[language,workers]=values;series[language,workers]=root
    assert (series[language,1]/'endpoints.csv').read_bytes()==(series[language,4]/'endpoints.csv').read_bytes()
assert all(all(near(a,b) for a,b in zip(x,y)) for x,y in zip(endpoints['c',1],endpoints['phys',1]))
xs,ys=zip(*endpoints['c',1])
assert abs(statistics.mean(xs)-1)<.03 and abs(statistics.stdev(xs)-.15)<.025
assert abs(statistics.mean(ys)-(5-9.80665/2))<.05 and abs(statistics.stdev(ys)-.25)<.04
# Both full-series analyzers read actual archived endpoints, not replacement draws.
for source in ('c','phys'):
    root=series[source,4]
    for language,module in [('c',c_analysis),('phys',phys_analysis)]:
        prefix=work/f'report-{source}-{language}'
        command([analysis,module,root/'run-0001.psrun',prefix]);command([probe,str(prefix)+'.psreport',root])
        with Path(str(prefix)+'-endpoints.csv').open(newline='') as f:lines=list(csv.reader(f))
        assert len(lines)==257 and len(lines[0])==3
        for i,line in enumerate(lines[1:]):assert near(float(line[0]),i+1) and all(near(float(a),b) for a,b in zip(line[1:],endpoints[source,4][i]))
# Degenerate initial distributions produce constant populations, one-bin histograms and zero-width intervals.
for language,module in [('c',c_model),('phys',phys_model)]:
    root=work/f'constant-{language}';command([batch,runner,module,root,'position.x','256','32','.03125','42','--workers','4','--until','1','--param','sigmaVx=0','--param','sigmaVy=0'])
    for i in range(256):verify(root/f'run-{i+1:04d}.psrun',42+i,0,0)
    for analyzer,model in [('c',c_analysis),('phys',phys_analysis)]:
        prefix=work/f'constant-report-{language}-{analyzer}'
        command([analysis,model,root/'run-0001.psrun',prefix]);command([probe,str(prefix)+'.psreport',root])
# Complete scenes, UTF-8 paths and high-bit seeds, independent of series concurrency.
for seed in (0,42,2**63+123,2**64-1):
    pair=[]
    for language,module in [('c',c_model),('phys',phys_model)]:
        path=work/f'scene-{seed}-{language}.psrun';command([runner,module,path,'--steps','32','--dt','.03125','--seed',str(seed),'--record-scenes'])
        pair.append(verify(path,seed))
    (a,sa),(b,sb)=pair
    for ra,rb in zip(a,b):assert all(near(x,y) for x,y in zip(ra,rb))
    assert len(sa)==len(sb)
    for x,y in zip(sa,sb):
        assert len(x)==len(y) and x[:28]==y[:28]
        count=struct.unpack_from('<I',x,16)[0];points=struct.unpack_from('<I',x,24)[0];offset=28+9*8
        assert count in (9,11) and points in (0,64)
        for i in range(count):
            at=offset+i*176;assert x[at:at+8]==y[at:at+8] and x[at+96:at+176]==y[at+96:at+176]
            for field in range(8,96,8):assert near(struct.unpack_from('<d',x,at+field)[0],struct.unpack_from('<d',y,at+field)[0])
        for at in range(offset+count*176,len(x),8):assert near(struct.unpack_from('<d',x,at)[0],struct.unpack_from('<d',y,at)[0])
# A missing archived member is rejected without a partial success report.
root=series['c',4];member=root/'run-0256.psrun';held=root/'held.psrun';member.rename(held)
for language,module in [('c',c_analysis),('phys',phys_analysis)]:
    prefix=work/f'missing-{language}';result=subprocess.run([analysis,module,str(root/'run-0001.psrun'),str(prefix)],capture_output=True,timeout=60)
    assert result.returncode!=0 and not Path(str(prefix)+'.psreport').exists()
held.rename(member)
# Incomplete bytes, a mismatched common endpoint and a changed population are
# each rejected by both readers, with the original ensemble restored afterwards.
original=member.read_bytes()
for failure in ('truncated','time','population'):
    try:
        if failure=='truncated':member.write_bytes(original[:-8])
        else:
            member.unlink()
            args=[runner,c_model,member,'--steps','32','--dt','.0625' if failure=='time' else '.03125','--seed','297']
            if failure=='population':args += ['--param','meanVx=4']
            command(args)
        for language,module in [('c',c_analysis),('phys',phys_analysis)]:
            prefix=work/f'{failure}-{language}'
            result=subprocess.run([analysis,module,str(root/'run-0001.psrun'),str(prefix)],capture_output=True,timeout=60)
            assert result.returncode!=0 and not Path(str(prefix)+'.psreport').exists(),(failure,language)
    finally:member.write_bytes(original)
print('Monte Carlo tutorial: 1536 archived runs, independent seeded ballistic oracles, worker reproducibility, eight mixed analyses, constant populations all scene fields and rejected malformed/mixed ensembles passed')
