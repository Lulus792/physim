"""Independent exact constant-slope/acceleration solutions for C/Physim methods."""
import argparse,json,math,subprocess
from pathlib import Path
from fractions import Fraction

def literal(text):
    value=float.fromhex(text)
    if value==0:return '-0.0' if math.copysign(1,value)<0 else '0.0'
    m,e=math.frexp(abs(value));return ('-' if value<0 else '')+'('+format(2*m,'.17g')+' * pow(2,'+str(e-1)+'))'

def language_source(cases):
    text='// Shared binary inputs; exact solutions are checked by the independent Fraction oracle.\n'
    for name,key in [('positions','q'),('velocities','v'),('rates','rate'),('durations','dt')]:text+='let '+name+' = ['+',\n    '.join(literal(c[key]) for c in cases)+']\n'
    text+='let methods = ['+','.join(str(c['method']) for c in cases)+']\n'
    text+='''var currentRate: Float64 = 0
func constant(time: Float64, state: [Float64]) -> [Float64]:
    return [currentRate]
func solve(method: Int64, q: Float64, v: Float64, dt: Float64) -> [Float64]:
    if method == 0:
        return [eulerStep(constant,[q],0,dt)[0],v]
    if method == 1:
        let result = Vec2(q,v).symplectic(currentRate,dt)
        return [result.x,result.y]
    if method == 2:
        return [rk4Step(constant,[q],0,dt)[0],v]
    if method == 3:
        return verletStep(constant,[q,v],0,dt)
    return [rk45IntegrateWithSteps(constant,[q],0,dt,1e-8,1e-6,8,dt,dt,dt)[0],v]
for i in 0..<methods.count:
    currentRate = rates[i]
    let result = attempt(solve(methods[i],positions[i],velocities[i],durations[i]))
    if let values = result:
        print("ok")
        print(values[0])
        print(values[1])
    else:
        print("numeric")
'''
    return text

def expected(case):
    q,v,rate,dt=[Fraction.from_float(float.fromhex(case[k])) for k in ('q','v','rate','dt')]
    method=case['method']
    if method in (0,2,4):return [(q+dt*rate,abs(q)+abs(dt*rate)),(v,abs(v))]
    if method==1:return [(q+dt*(v+dt*rate),abs(q)+abs(dt*v)+abs(dt*dt*rate)),(v+dt*rate,abs(v)+abs(dt*rate))]
    return [(q+dt*v+dt*dt*rate/2,abs(q)+abs(dt*v)+abs(dt*dt*rate/2)),(v+dt*rate,abs(v)+abs(dt*rate))]

def check(case,status,values):
    reference=expected(case)
    try:rounded=[float(value) for value,_ in reference]
    except OverflowError:rounded=[math.inf]
    if any(not math.isfinite(v) for v in rounded):
        assert status!=0,(case,status,values)
        if values is not None and case['method']!=1:assert values==[float.fromhex(case['q']),float.fromhex(case['v'])]
        return
    assert status==0,(case,status,rounded)
    for actual,(value,magnitude) in zip(values,reference):
        assert math.isfinite(actual)
        bound=max(Fraction.from_float(math.ulp(0.0))*8,64*Fraction.from_float(math.ulp(1.0))*magnitude)
        assert abs(Fraction.from_float(actual)-value)<=bound,(case,actual,float(value),float(bound))

def main():
    p=argparse.ArgumentParser()
    for name in ('c','language','cases','fixture'):p.add_argument('--'+name,required=True)
    a=p.parse_args();cases=json.loads(Path(a.cases).read_text());assert Path(a.fixture).read_text()==language_source(cases)
    data=''.join(str(c['method'])+' '+' '.join(c[k] for k in ('q','v','rate','dt'))+'\n' for c in cases)
    r=subprocess.run([a.c],input=data,text=True,capture_output=True,timeout=60);assert r.returncode==0,r.stderr
    lines=r.stdout.splitlines();assert len(lines)==len(cases)
    for case,line in zip(cases,lines):
        status,q,v=line.split();check(case,int(status),[float(q),float(v)])
    r=subprocess.run([a.language],text=True,capture_output=True,timeout=60);assert r.returncode==0,r.stderr
    lines=iter(r.stdout.splitlines())
    for case in cases:
        label=next(lines)
        if label=='ok':check(case,0,[float(next(lines)),float(next(lines))])
        else:assert label=='numeric';check(case,10,None)
    assert next(lines,None) is None
    print(f'ODE range: exact rational constant solutions for {len(cases)} C and Physim cases across Euler/symplectic/RK4/Verlet/RK45, finite endpoints and true-range failures passed')
if __name__=='__main__':main()
