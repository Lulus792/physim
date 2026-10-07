"""Exact Fraction oracle for binary inputs; compare C and actual Physim operators."""
import argparse,json,math,random,subprocess
from fractions import Fraction
from pathlib import Path

def literal(value):
    value=float.fromhex(value)
    if value==0:return '-0.0' if math.copysign(1,value)<0 else '0.0'
    mantissa,exponent=math.frexp(abs(value));prefix='-' if value<0 else ''
    return prefix+'('+format(2*mantissa,'.17g')+' * pow(2,'+str(exponent-1)+'))'

def language_source(cases):
    text='// Binary inputs shared with the independent Fraction oracle.\n'
    for column,key in [('leftValues','a'),('rightValues','b'),('leftScales','sa'),('rightScales','sb')]:
        text+='let '+column+' = ['+',\n    '.join(literal(c[key]) for c in cases)+']\n'
    text+='let subtraction = ['+','.join('true' if c['subtract'] else 'false' for c in cases)+']\n'
    text+='''let metre = Unit(1,0,0,0,0,0,0,1,"m")
for i in 0..<leftValues.count:
    let left = Quantity(leftValues[i],Unit(1,0,0,0,0,0,0,leftScales[i],"a"))
    let right = Quantity(rightValues[i],Unit(1,0,0,0,0,0,0,rightScales[i],"b"))
    let answer = attempt(subtraction[i] ? left - right : left + right)
    if let value = answer:
        assert(value.unit.isCompatible(metre))
        print("ok")
        print(value.value)
        print(value.unit.convert(1,metre))
    else:
        print("numeric")
'''
    return text

def exact(case):
    a,b,sa,sb=(Fraction.from_float(float.fromhex(case[k])) for k in ('a','b','sa','sb'))
    converted=b*sb/sa;result=a-converted if case['subtract'] else a+converted
    try:value=float(result)
    except OverflowError:return 10,None,result,converted
    if not math.isfinite(value) or (value==0 and result):return 10,None,result,converted
    return 0,value,result,converted

def verify(case,status,value,scale,dimensions=None):
    wanted,reference,result,converted=exact(case)
    assert status==wanted,(case,status,wanted,value,reference)
    if status:
        if dimensions is not None:assert value==19 and scale==1 and dimensions==(0,1)
        return
    assert math.isfinite(value) and scale==float.fromhex(case['sa'])
    if case.get("exact_reference",False):assert value==reference,(case,value,reference)
    if dimensions is not None:assert dimensions==(1,0)
    # Dyadic conversions use exact scale ratios. One final subnormal rounding
    # can add a single ulp; ordinary non-dyadic conversion retains Double error.
    ratio=Fraction.from_float(float.fromhex(case['sb']))/Fraction.from_float(float.fromhex(case['sa']))
    power_two=(ratio.numerator & (ratio.numerator-1)==0 and ratio.denominator & (ratio.denominator-1)==0)
    error=abs(Fraction.from_float(value)-result)
    bound=Fraction.from_float(math.ulp(reference)) if power_two else max(Fraction.from_float(math.ulp(reference))/2,8*Fraction.from_float(math.ulp(1.0))*(abs(Fraction.from_float(float.fromhex(case['a'])))+abs(converted)))
    assert error<=bound,(case,value,reference,float(error),float(bound))

def main():
    p=argparse.ArgumentParser()
    for name in ('c','language','cases','fixture'):p.add_argument('--'+name,required=True)
    a=p.parse_args();cases=json.loads(Path(a.cases).read_text())
    assert Path(a.fixture).read_text()==language_source(cases),'Physim fixture differs from exact oracle input cases'
    rng=random.Random(71923);all_cases=cases.copy()
    for _ in range(10000):
        def value():return math.ldexp(rng.choice((1.0,1.25,1.5,1.75))*rng.choice((-1,1)),rng.randint(-1074,1023))
        all_cases.append(dict(a=value().hex(),b=value().hex(),sa=math.ldexp(1.0,rng.randint(-1074,1023)).hex(),sb=math.ldexp(1.0,rng.randint(-1074,1023)).hex(),subtract=bool(rng.randrange(2))))
    for _ in range(10000):
        def arbitrary():return math.ldexp(rng.uniform(1,1.999)*rng.choice((-1,1)),rng.randint(-1000,1000))
        all_cases.append(dict(a=arbitrary().hex(),b=arbitrary().hex(),sa=abs(arbitrary()).hex(),sb=abs(arbitrary()).hex(),subtract=bool(rng.randrange(2))))
    text=''.join(' '.join(c[k] for k in ('a','b','sa','sb'))+' '+str(int(c['subtract']))+'\n' for c in all_cases)
    run=subprocess.run([a.c],input=text,capture_output=True,text=True,timeout=60);assert run.returncode==0,run.stderr
    lines=run.stdout.splitlines();assert len(lines)==len(all_cases)
    for case,line in zip(all_cases,lines):
        status,value,scale,length,time=line.split();verify(case,int(status),float(value),float(scale),(int(length),int(time)))
    run=subprocess.run([a.language],capture_output=True,text=True,timeout=60);assert run.returncode==0,run.stderr
    lines=iter(run.stdout.splitlines())
    for case in cases:
        status=next(lines)
        if status=='ok':verify(case,0,float(next(lines)),float(next(lines)))
        else:assert status=='numeric';verify(case,10,None,None)
    assert next(lines,None) is None
    print(f'Quantity sums: exact Fraction reference for {len(all_cases)} C inputs, {len(cases)} actual Physim operator cases, left units, numeric limits and preserved outputs passed')
if __name__=='__main__':main()
