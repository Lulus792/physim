"""Check integer PCG transitions and Box-Muller order independently of C/Physim."""
import argparse,math,subprocess
p=argparse.ArgumentParser();p.add_argument('--c',required=True);p.add_argument('--language',required=True);a=p.parse_args()
mask=(1<<64)-1;increment=1442695040888963407
class Oracle:
    def __init__(self,seed):
        self.state=0;self.next();self.state=(self.state+seed)&mask;self.next()
    def next(self):
        before=self.state;self.state=(before*6364136223846793005+increment)&mask
        word=(((before>>18)^before)>>27)&0xffffffff;rotation=before>>59
        return ((word>>rotation)|(word<<((-rotation)&31)))&0xffffffff
    def uniform(self):return (self.next()+0.5)/(1<<32)
    def normal(self):
        radius=self.uniform();angle=self.uniform()
        return 2+3*math.sqrt(-2*math.log(radius))*math.cos(2*math.pi*angle)
    def mixed(self,i):
        return self.uniform() if i%5==0 else self.normal() if i%5==1 else 7 if i%5==2 else 5 if i%5==3 else 2
def run(path):
    r=subprocess.run([path],capture_output=True,text=True,timeout=30)
    assert r.returncode==0,(r.returncode,r.stderr);return r.stdout.splitlines()
lines=run(a.c);at=0;expected=[]
for seed in (0,42,1<<63,mask):
    for kind in ('raw','normal','mixed'):
        rng=Oracle(seed)
        for i in range(32):
            fields=lines[at].split();at+=1
            assert fields[:3]==[kind,str(seed),str(i)],fields
            value=rng.next() if kind=='raw' else rng.normal() if kind=='normal' else rng.mixed(i)
            if kind=='raw':assert int(fields[3])==value,fields
            else:assert math.isclose(float(fields[3]),value,rel_tol=2e-14,abs_tol=2e-14),(fields,value)
            assert int(fields[4])==rng.state and int(fields[5])==increment,fields
            if kind=='mixed':expected.append(value)
assert at==len(lines)
actual=run(a.language);assert len(actual)==len(expected)
for x,y in zip(actual,expected):assert math.isclose(float(x),y,rel_tol=2e-14,abs_tol=2e-14),(x,y)
print('RNG reference: four full-width seeds, integer transitions, explicit normal order, degenerate draws, snapshots and isolated C/Physim streams passed')
