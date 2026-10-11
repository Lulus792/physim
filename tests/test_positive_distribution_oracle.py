"""Decimal moments plus independent integer PCG and transformed draws."""
import argparse
from decimal import Decimal, localcontext
import math
from pathlib import Path
import subprocess

MASK=(1<<64)-1
INCREMENT=1442695040888963407
SEEDS=(0,42,1<<63,MASK)
PARAMETERS=((3,math.ulp(0.0),0.0),(3,2.0,0.0),(3,1e-300,0.0),(3,float.fromhex('0x1.fffffffffffffp+1023'),0.0),
            (4,.25,.5),(4,0.0,0.0),(4,0.0,1e-200),(4,-800.0,30.0),
            (4,-400.0,25.0),(4,600.0,.001),(4,-1000.0,0.0),(4,1000.0,0.0),
            (3,0.0,0.0),(3,-1.0,0.0),(4,0.0,-1.0))
CASES=[(*parameters,seed) for seed in SEEDS for parameters in PARAMETERS]

class Oracle:
    def __init__(self,seed):
        self.state=0;self.next();self.state=(self.state+seed)&MASK;self.next()
    def next(self):
        old=self.state;self.state=(old*6364136223846793005+INCREMENT)&MASK
        word=(((old>>18)^old)>>27)&0xffffffff;rotation=old>>59
        return ((word>>rotation)|(word<<((-rotation)&31)))&0xffffffff
    def uniform(self):return (self.next()+.5)/(1<<32)

def signed(seed):return seed if seed<(1<<63) else seed-(1<<64)

def language_source():
    text='// Generated transformed-distribution reference inputs.\n'
    for i,(kind,a,b,seed) in enumerate(CASES):
        argument='pow(2,-1074)' if a==math.ulp(0.0) else repr(a)
        factory=f'Distribution.exponential({argument})' if kind==3 else f'Distribution.logNormal({argument},{b!r})'
        text+=f'var rng{i} = Rng({signed(seed)})\nlet dist{i} = attempt({factory})\n'
        text+=f'if dist{i} != nil:\n    let dist = dist{i}.unwrap()\n    let mean = attempt(dist.mean())\n    let sd = attempt(dist.standardDeviation())\n    if mean != nil && sd != nil:\n        print("moment ok")\n        print(mean.unwrap())\n        print(sd.unwrap())\n    else:\n        print("moment error")\n'
        text+=f'    let value = attempt(rng{i}.sample(dist))\n    if value != nil:\n        print("sample ok")\n        print(value.unwrap())\n    else:\n        print("sample error")\nelse:\n    print("invalid")\n'
        text+=f'print(rng{i}.sample(Distribution.uniform(0,1)))\n'
    return text

def valid(kind,a,b):return a>0 and b==0 if kind==3 else b>=0

def moments(kind,a,b):
    if not valid(kind,a,b):return 1,8.0,9.0
    if kind==3:return 0,a,a
    with localcontext() as context:
        context.prec=100
        mu=Decimal.from_float(a);sigma=Decimal.from_float(b);q=sigma*sigma
        log_mean=mu+q/2
        if abs(log_mean)>10000:return 10,8.0,9.0
        mean=log_mean.exp()
        if not b:sd=Decimal(0)
        else:
            # Direct high-precision mathematical formula; tiny expm1 is expanded
            # before subtractive cancellation and large exponents remain Decimal.
            excess=q*(1+q/2+q*q/6) if q<Decimal('1e-30') else q.exp()-1
            sd=mean*excess.sqrt()
        m=float(mean);s=float(sd)
        if not math.isfinite(m) or not math.isfinite(s) or m<=0 or (b>0 and s<=0):return 10,8.0,9.0
        return 0,m,s

def sample(kind,a,b,seed):
    rng=Oracle(seed);saved=rng.state
    if not valid(kind,a,b):return 1,7.0,rng
    try:
        if kind==3:
            # Uniform values are exactly representable; Decimal computes the
            # inverse survival transformation independently of the C libm log.
            u=rng.uniform()
            with localcontext() as context:
                context.prec=90
                value=float(-Decimal.from_float(a)*Decimal.from_float(u).ln())
        elif b:
            radius=rng.uniform();angle=rng.uniform()
            normal=a+b*math.sqrt(-2*math.log(radius))*math.cos(2*math.pi*angle)
            value=math.exp(normal)
        else:value=math.exp(a)
    except OverflowError:value=math.inf
    if not math.isfinite(value) or value<=0:
        rng.state=saved;return 10,7.0,rng
    return 0,value,rng

def near(actual,expected):
    return math.isfinite(actual) and abs(actual-expected)<=max(8*math.ulp(expected),abs(expected)*4e-13)

def main():
    p=argparse.ArgumentParser();p.add_argument('--c',required=True);p.add_argument('--language',required=True);p.add_argument('--fixture',required=True);a=p.parse_args()
    assert Path(a.fixture).read_text()==language_source()
    for case in CASES:
        kind,mu,sigma,seed=case
        result=subprocess.run([a.c,*map(str,case)],text=True,capture_output=True,timeout=10)
        assert result.returncode==0,result.stderr
        fields=result.stdout.split();assert len(fields)==7
        status,mean,sd=moments(kind,mu,sigma)
        assert int(fields[0])==status and near(float(fields[1]),mean) and near(float(fields[2]),sd),(case,fields,(status,mean,sd))
        status,value,rng=sample(*case)
        assert int(fields[3])==status and near(float(fields[4]),value),(case,fields,value)
        assert int(fields[5])==rng.state and int(fields[6])==INCREMENT,(case,fields,rng.state)
    result=subprocess.run([a.language],text=True,capture_output=True,timeout=60);assert result.returncode==0,result.stderr
    lines=iter(result.stdout.splitlines())
    for case in CASES:
        kind,mu,sigma,seed=case;label=next(lines)
        if not valid(kind,mu,sigma):assert label=='invalid'
        else:
            status,mean,sd=moments(kind,mu,sigma)
            assert label==('moment error' if status else 'moment ok'),(case,label,status)
            if not status:assert near(float(next(lines)),mean) and near(float(next(lines)),sd)
            status,value,rng=sample(*case);label=next(lines)
            assert label==('sample error' if status else 'sample ok'),(case,label,status)
            if not status:assert near(float(next(lines)),value)
        _,_,rng=sample(*case)
        assert float(next(lines))==rng.uniform(),case
    assert next(lines,None) is None
    print(f'Positive distributions: {len(CASES)} C/Physim cases, Decimal moments/exponential draws, independent PCG states, full-width seeds, lognormal draw order and atomic errors passed')

if __name__=='__main__':main()
