"""Independent exact rational references for identity-frame momentum queries."""
import math
import random
import subprocess
import sys
from fractions import Fraction as F

def rounded(value):
    try:
        return float(value)
    except OverflowError:
        return math.copysign(math.inf, -1 if value < 0 else 1)

rng = random.Random(381382)
def value(positive=False):
    result = math.ldexp(rng.uniform(.5,1),rng.randrange(-1073,1024))
    return result if positive or rng.randrange(2) else -result

rows = [[value(True) for _ in range(4)] + [value() for _ in range(12)] for _ in range(1000)]
# Deliberately cancel enormous orbital products while keeping tiny spin terms.
for exponent in (-1074,-1000,-500,0,500,1023):
    for sign in (-1,1):
        x=float.fromhex('0x1.fffffffffffffp1023')
        rows.append([x,1.,1.,1.,x,sign*x,0.,-x,-sign*x,0.,x,sign*x,0.,0.,0.,math.ldexp(1.,exponent)])
data=''.join(' '.join(v.hex() for v in row)+'\n' for row in rows)
run=subprocess.run([sys.argv[1]],input=data,text=True,capture_output=True,check=True)
lines=run.stdout.splitlines()
assert len(lines)==len(rows)
for index,(row,line) in enumerate(zip(rows,lines)):
    mass,*rest=map(F,row)
    inertia,position,origin,velocity,omega=[rest[i:i+3] for i in range(0,15,3)]
    p=[rounded(mass*v) for v in velocity]
    l=[]
    for k in range(3):
        j,t=(k+1)%3,(k+2)%3
        l.append(rounded((position[j]-origin[j])*mass*velocity[t] -
                         (position[t]-origin[t])*mass*velocity[j] + inertia[k]*omega[k]))
    tokens=line.split()
    for offset,expected in ((0,p),(4,l)):
        status=int(tokens[offset])
        actual=[float.fromhex(v) for v in tokens[offset+1:offset+4]]
        # PS_NUMERIC=10; genuine overflow must leave the sentinel unchanged.
        if all(math.isfinite(v) for v in expected):
            assert status==0 and actual==expected,(index,offset,status,actual,expected)
        else:
            assert status==10 and actual==[17.,19.,23.],(index,offset,status,actual,expected)
print(f'{len(rows)} exact rational momentum references passed')
