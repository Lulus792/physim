"""Independent t-density quadrature and exact integer binomial normalization.

The implementation inverts incomplete beta fractions; this oracle integrates
the density directly, with an exact-binomial/Decimal normalization. No SciPy,
Physim CDF or third-party runtime is needed on CI hosts.
"""
import argparse
from decimal import Decimal, localcontext
from functools import lru_cache
import math
from pathlib import Path
import subprocess
import sys
if hasattr(sys, 'set_int_max_str_digits'):
    sys.set_int_max_str_digits(0)
PI = Decimal('3.141592653589793238462643383279502884197169399375105820974944592307816406286')
DEGREES = (1, 2, 3, 4, 5, 9, 29, 99, 999, 99999, 100000, 1000000)
LEVELS = (.1, .5, .95, .99, math.nextafter(1, 0))
CASES = [(0.0, 1.0, df + 1, level) for df in DEGREES for level in LEVELS]
CASES += [(0.0,1.0,10,1e-12),(3.0,0.0,20,.95),(0.0,1.0,1,.95),
          (0.0,-1.0,2,.95),(0.0,1.0,2,1.0),(0.0,1.0,2,0.0)]

def language_source():
    text = '// Generated exact input cases for an independent density-quadrature oracle.\n'
    for index, (mean, sd, count, confidence) in enumerate(CASES):
        text += f'let result{index} = attempt(MeanInterval.fromSample({mean!r},{sd!r},{count},{confidence!r}))\n'
        text += f'if result{index} != nil:\n    let value = result{index}.unwrap()\n    print("ok")\n'
        for method in ('criticalValue','standardError','lower','upper','confidence'):
            text += f'    print(value.{method}())\n'
        text += 'else:\n    print("error")\n'
    return text

@lru_cache(None)
def normalization(df):
    if df > 100000:
        # Density at zero from the Wallis-ratio expansion. O(df^-3) here is
        # below 1e-18; integration still uses the full finite-degree density.
        return (1-1/(4*df)+1/(32*df*df))/math.sqrt(2*math.pi)
    with localcontext() as context:
        context.prec = 80
        k = df // 2
        choose = Decimal(math.comb(2*k,k))
        four = Decimal(4)**k
        if df % 2:
            return float(four / (PI * Decimal(df).sqrt() * choose))
        return float(Decimal(k)*choose/(four*Decimal(df).sqrt()))

def integrate(function, tolerance=2e-13):
    def recurse(a,b,fa,fm,fb,estimate,tol,depth):
        mid=(a+b)/2;left=(a+mid)/2;right=(mid+b)/2
        fl=function(left);fr=function(right)
        first=(mid-a)*(fa+4*fl+fm)/6;second=(b-mid)*(fm+4*fr+fb)/6
        difference=first+second-estimate
        if abs(difference)<=15*tol:
            return first+second+difference/15
        assert depth, ('quadrature exhausted', a, b, difference)
        return recurse(a,mid,fa,fl,fm,first,tol/2,depth-1)+recurse(mid,b,fm,fr,fb,second,tol/2,depth-1)
    fa=function(0);fm=function(.5);fb=function(1)
    return recurse(0,1,fa,fm,fb,(fa+4*fm+fb)/6,tolerance,24)

def probability(df, critical, central):
    f0=normalization(df);power=(df+1)/2;q=critical*critical/df
    if central:
        return 2*f0*critical*integrate(lambda x: math.exp(-power*math.log1p(q*x*x)))
    # Transform the semi-infinite upper tail x=t/(1-v), then scale out f(t)*t.
    def density_ratio(v):
        s=1-v
        if not s:return (1+q)/q if df==1 else 0.0
        return math.exp(power*(math.log1p(q)-math.log1p(q/(s*s)))-2*math.log(s))
    return f0*critical*math.exp(-power*math.log1p(q))*integrate(density_ratio)

def check(case,status,values):
    mean,sd,count,confidence=case
    valid=count>=2 and sd>=0 and 0<confidence<1
    assert (status==0)==valid,(case,status)
    if not valid:return
    critical,se,lower,upper,level=values
    assert all(math.isfinite(v) for v in values) and critical>0 and level==confidence
    assert math.isclose(se,sd/math.sqrt(count),rel_tol=4e-15,abs_tol=0)
    assert math.isclose(lower,mean-critical*se,rel_tol=4e-15,abs_tol=1e-15)
    assert math.isclose(upper,mean+critical*se,rel_tol=4e-15,abs_tol=1e-15)
    if sd==0:assert lower==mean==upper
    central=confidence<.5
    expected=confidence if central else (1-confidence)/2
    observed=probability(count-1,critical,central)
    assert abs(observed-expected)<=3e-11*expected,(case,critical,observed,expected)

def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('--c',required=True);parser.add_argument('--language',required=True)
    parser.add_argument('--fixture',required=True)
    args=parser.parse_args()
    assert Path(args.fixture).read_text()==language_source()
    for case in CASES:
        result=subprocess.run([args.c,*map(str,case)],text=True,capture_output=True,timeout=10)
        assert result.returncode==0,result.stderr
        fields=result.stdout.split();assert len(fields)==6
        check(case,int(fields[0]),list(map(float,fields[1:])))
    result=subprocess.run([args.language],text=True,capture_output=True,timeout=30)
    assert result.returncode==0,result.stderr
    lines=iter(result.stdout.splitlines())
    for case in CASES:
        status=next(lines)
        assert status in ('ok','error')
        check(case,0 if status=='ok' else 1,[float(next(lines)) for _ in range(5)] if status=='ok' else [])
    assert next(lines,None) is None
    print(f'Student-t mean intervals: {len(CASES)} C/Physim cases, independent density quadrature, exact integer normalization, extreme confidence, asymptotic degrees and invalid inputs passed')

if __name__=='__main__':main()
