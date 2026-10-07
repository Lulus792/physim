"""Exact rational elimination independent of the C/Physim floating solver."""
import argparse,json,math,subprocess
from fractions import Fraction
from pathlib import Path

def literal(text):
    value=float.fromhex(text)
    if value==0:return '0.0'
    m,e=math.frexp(abs(value))
    return ('-' if value<0 else '')+'('+format(2*m,'.17g')+' * pow(2,'+str(e-1)+'))'

def language_source(cases):
    alphabet='0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz'
    text='// Identical binary inputs; dictionary encoding avoids repeating large literal matrices.\n'
    text+='func decode(table: [Float64], pattern: String) -> [Float64]:\n    let alphabet = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"\n    var values: [Float64] = []\n    for symbol in pattern:\n        for index in 0..<table.count:\n            if alphabet[index] == symbol:\n                values.append(table[index])\n                break\n    assert(values.count == pattern.count)\n    return values\n'
    for i,c in enumerate(cases):
        unique=list(dict.fromkeys(c['a']));assert len(unique)<=len(alphabet)
        encoded=''.join(alphabet[unique.index(v)] for v in c['a'])
        assert [unique[alphabet.index(v)] for v in encoded]==c['a']
        matrix='decode(['+','.join(map(literal,unique))+'],"'+encoded+'")'
        text+='let result'+str(i)+' = attempt(linearSolve('+matrix+',['+','.join(map(literal,c['b']))+'],'+str(c['tol'])+'))\n'
        text+='if let values = result'+str(i)+':\n    print("ok")\n    for value in values:\n        print(value)\nelse:\n    print("error")\n'
    return text

def reference(c):
    n=len(c['b']);m=[[Fraction.from_float(float.fromhex(v)) for v in c['a'][i*n:(i+1)*n]]+[Fraction.from_float(float.fromhex(c['b'][i]))] for i in range(n)]
    for k in range(n):
        pivot=next((i for i in range(k,n) if m[i][k]),None)
        if pivot is None:return None
        m[k],m[pivot]=m[pivot],m[k]
        for i in range(k+1,n):
            f=m[i][k]/m[k][k]
            for j in range(k,n+1):m[i][j]-=f*m[k][j]
    x=[Fraction(0) for _ in range(n)]
    for i in range(n-1,-1,-1):x[i]=(m[i][n]-sum(m[i][j]*x[j] for j in range(i+1,n)))/m[i][i]
    return x

def check(case,status,values):
    expected=case.get('error',0);assert status==expected,(case['name'],status,expected)
    if expected:
        if values is not None:assert values==[float(7+i) for i in range(len(case['b']))]
        return
    exact=reference(case);assert exact is not None
    n=len(exact);bound=max(Fraction.from_float(math.ulp(0.0))*8,max(map(abs,exact))*n*n*64*Fraction.from_float(math.ulp(1.0)))
    for a,e in zip(values,exact):assert math.isfinite(a) and abs(Fraction.from_float(a)-e)<=bound,(case['name'],a,float(e))
    for i,b in enumerate(case['b']):
        terms=[Fraction.from_float(float.fromhex(case['a'][i*n+j]))*Fraction.from_float(values[j]) for j in range(n)]
        rhs=Fraction.from_float(float.fromhex(b));residual=abs(sum(terms)-rhs)
        scale=sum(map(abs,terms))+abs(rhs)
        assert residual<=max(Fraction.from_float(math.ulp(0.0))*8,scale*n*n*64*Fraction.from_float(math.ulp(1.0))),(case['name'],'residual',i)

def main():
    p=argparse.ArgumentParser()
    for name in ['c','language','cases','fixture']:p.add_argument('--'+name,required=True)
    a=p.parse_args();cases=json.loads(Path(a.cases).read_text());assert Path(a.fixture).read_text()==language_source(cases)
    data=''.join(str(len(c['b']))+' '+float(c['tol']).hex()+' '+' '.join(c['a']+c['b'])+'\n' for c in cases)
    r=subprocess.run([a.c],input=data,text=True,capture_output=True,timeout=60);assert r.returncode==0,r.stderr
    lines=r.stdout.splitlines();assert len(lines)==len(cases)
    for c,line in zip(cases,lines):
        fields=line.split();assert len(fields)==len(c['b'])+1;check(c,int(fields[0]),[float.fromhex(v) for v in fields[1:]])
    r=subprocess.run([a.language],text=True,capture_output=True,timeout=60);assert r.returncode==0,r.stderr
    lines=iter(r.stdout.splitlines())
    for c in cases:
        label=next(lines)
        if label=='ok':check(c,0,[float(next(lines)) for _ in c['b']])
        else:assert label=='error' and c.get('error',0)!=0
    assert next(lines,None) is None
    print(f'Linear systems: {len(cases)} C/Physim exact rational solutions/residuals, 1..32 dimensions, row permutations/scales and error rollback passed')
if __name__=='__main__':main()
