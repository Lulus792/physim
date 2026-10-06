"""Independent nonlinear period/Taylor oracles, five integrators, scenes and reports."""
from pathlib import Path
import csv,math,struct,subprocess,sys,zlib
runner,analysis,c_model,phys_model,c_analysis,phys_analysis,probe,work=sys.argv[1:]
work=Path(work)
methods=['Euler','symplectic Euler','RK4','velocity Verlet','Dormand-Prince 5(4)']
def command(args,expected=0):
    r=subprocess.run(list(map(str,args)),capture_output=True,timeout=90)
    assert r.returncode==expected,(args,r.returncode,r.stderr)
    return r

def read(path):
    rows=[];scenes=[];metadata='';footer=False
    with path.open('rb') as f:
        assert f.read(16)==b'PSRUN17\n'+struct.pack('<II',1,0x01020304)
        while h:=f.read(12):
            kind,size,crc=struct.unpack('<III',h);data=f.read(size)
            assert len(data)==size and zlib.crc32(data)==crc
            if kind==1:metadata=data.decode()
            if kind==2:
                assert size==4+6*167
                names=['angle','angular_velocity','position.x','position.y','energy','sensor.angle']
                dimensions=[(0,0,0),(0,0,-1),(1,0,0),(1,0,0),(2,1,-2),(0,0,0)]
                for i,(name,dimension) in enumerate(zip(names,dimensions)):
                    at=4+i*167;assert data[at:at+48].split(b'\0')[0].decode()==name
                    assert struct.unpack_from('<7b',data,at+160)==dimension+(0,0,0,0)
            if kind==3:rows.append(struct.unpack('<7d',data))
            if kind==4:assert struct.unpack('<Q',data)[0]==len(rows);footer=True
            if kind==5:
                assert struct.unpack_from('<I',data)[0]==3 and struct.unpack_from('<II',data,12)==(6,8)
                scenes.append(data)
    assert footer and rows
    return rows,scenes,metadata

def near(x,y,tol=1e-12):return math.isfinite(x) and math.isfinite(y) and abs(x-y)<=tol*max(1,abs(x),abs(y))
def period(length,amplitude):
    # Complete elliptic integral by independent Simpson quadrature, no ODE solver.
    n=4096;k=math.sin(abs(amplitude)/2);h=math.pi/2/n
    values=[1/math.sqrt(1-k*k*math.sin(i*h)**2) for i in range(n+1)]
    return 4*math.sqrt(length/9.80665)*h/3*(values[0]+values[-1]+4*sum(values[1:-1:2])+2*sum(values[2:-1:2]))
def crossings(rows):
    times=[]
    for previous,current in zip(rows,rows[1:]):
        if previous[1]<0<=current[1]:times.append(previous[0]+(current[0]-previous[0])*(-previous[1])/(current[1]-previous[1]))
    return [(b-a) for a,b in zip(times,times[1:])]
def taylor(length,amplitude,t):
    # Coefficients of theta, sin(theta), cos(theta), independently of RK stages.
    theta=[amplitude,0.0];s=[math.sin(amplitude)];c=[math.cos(amplitude)]
    for n in range(60):
        if n:
            s.append(sum(k*theta[k]*c[n-k] for k in range(1,n+1))/n)
            c.append(-sum(k*theta[k]*s[n-k] for k in range(1,n+1))/n)
        theta.append(-9.80665/length*s[n]/((n+1)*(n+2)))
    return sum(x*t**i for i,x in enumerate(theta)),sum(i*x*t**(i-1) for i,x in enumerate(theta) if i)

runs={};drifts={};total_rows=0
for method in range(5):
    pair=[]
    for language,module in [('c',c_model),('phys',phys_model)]:
        path=work/f'{language}-{method}.psrun'
        command([runner,module,path,'--steps','4000','--dt','.005','--param',f'integrator={method}','--record-scenes'])
        rows,scenes,metadata=read(path);pair.append((rows,scenes));runs[language,method]=path;total_rows+=len(rows)
        assert len(rows)==4001 and len(scenes)>100 and f'integrator={methods[method]}\n' in metadata
        for name,unit,dimension in [('length','m','1,0,0,0,0,0,0'),('initialAngle','rad','0,0,0,0,0,0,0'),('integrator','1','0,0,0,0,0,0,0')]:
            assert f'parameter_unit.{name}={unit}\n' in metadata and f'parameter_dimension.{name}={dimension}\n' in metadata
        for i,(t,a,w,x,y,e,sensor) in enumerate(rows):
            assert t==i*.005 and near(x,1.5*math.sin(a)) and near(y,-1.5*math.cos(a)) and near(sensor,a)
            assert near(e,.5*1.5**2*w*w+9.80665*1.5*(1-math.cos(a)))
        drift=max(abs(r[5]-rows[0][5]) for r in rows);drifts[language,method]=drift
        periods=crossings(rows);assert len(periods)>=6
        if method in (2,4):assert drift<1e-8 and abs(sum(periods)/len(periods)-period(1.5,.45))<2e-6
        if method==3:assert drift<1e-4 and abs(sum(periods)/len(periods)-period(1.5,.45))<5e-5
        if method==1:assert drift<.02
        if method==0:assert rows[-1][5]>rows[0][5]*1.5
    (a,sa),(b,sb)=pair
    for ra,rb in zip(a,b):assert all(near(x,y) for x,y in zip(ra,rb))
    assert len(sa)==len(sb)
    for x,y in zip(sa,sb):
        assert x[:28]==y[:28] and len(x)==len(y)
        offset=28+6*8
        for i in range(8):
            at=offset+i*176;assert x[at:at+8]==y[at:at+8] and x[at+96:at+176]==y[at+96:at+176]
            for field in range(8,96,8):assert near(struct.unpack_from('<d',x,at+field)[0],struct.unpack_from('<d',y,at+field)[0])
# Step-halving against a nonlinear analytic Taylor expansion, independent of the integrators.
for method,order in [(0,1),(1,1),(2,4),(3,2)]:
    for language,module in [('c',c_model),('phys',phys_model)]:
        errors=[];exact_a,exact_w=taylor(1.5,.45,.5)
        for dt in (.02,.01):
            path=work/f'refine-{method}-{language}-{dt}.psrun'
            command([runner,module,path,'--steps',str(round(.5/dt)),'--dt',str(dt),'--param',f'integrator={method}'])
            row=read(path)[0][-1];errors.append(math.hypot(row[1]-exact_a,(row[2]-exact_w)/math.sqrt(9.80665/1.5)))
        ratio=errors[0]/errors[1];assert .85*2**order<ratio<1.2*2**order,(method,language,errors,ratio)
# Changed length/amplitude, zero angle and adaptive unequal time axes.
for label,length,angle in [('short',.7,.9),('negative',2.2,-.7),('equilibrium',1.5,0)]:
    for language,module in [('c',c_model),('phys',phys_model)]:
        path=work/f'{label}-{language}.psrun';command([runner,module,path,'--steps','8000','--dt','.0025','--param','integrator=2','--param',f'length={length}','--param',f'initialAngle={angle}'])
        rows=read(path)[0]
        if angle:
            intervals=crossings(rows);assert intervals and abs(sum(intervals)/len(intervals)-period(length,angle))<5e-7
        else:assert all(r[1]==0 and r[2]==0 and r[5]==0 for r in rows)
for language,module in [('c',c_model),('phys',phys_model)]:
    path=work/f'adaptive-{language}.psrun'
    command([runner,module,path,'--steps','100000','--dt','.2','--adaptive','--min-dt','1e-8','--max-dt','.2','--until','20','--param','integrator=4','--record-scenes'])
    rows,_,metadata=read(path);assert rows[-1][0]==20 and len(rows)<2000 and 'step_mode=adaptive\n' in metadata
    durations=[b[0]-a[0] for a,b in zip(rows,rows[1:])];assert max(durations)-min(durations)>1e-4
    assert max(abs(r[5]-rows[0][5]) for r in rows)<1e-6
    intervals=crossings(rows);assert abs(sum(intervals)/len(intervals)-period(1.5,.45))<3e-4
    runs[language,5]=path
    result=command([runner,module,work/f'fractional-{language}.psrun','--steps','1','--param','integrator=1.5'],5);assert b'integer' in result.stderr
    result=command([runner,module,work/f'wrong-adaptive-{language}.psrun','--steps','1','--adaptive','--param','integrator=2'],7);assert b'integrator=4' in result.stderr
# Both analysis languages compare mixed C/Physim sources on their own axes, including adaptive data.
for group,inputs in [('c',[runs['c',i] for i in range(5)]),('phys',[runs['phys',i] for i in range(5)]),('mixed',[runs['c',0],runs['phys',1],runs['c',2],runs['phys',3],runs['c',4],runs['phys',5]]),('maximum',[runs['c',0],runs['phys',1],runs['c',2],runs['phys',3],runs['c',4],runs['phys',5],runs['c',5],runs['phys',2]])]:
    for language,module in [('c',c_analysis),('phys',phys_analysis)]:
        prefix=work/f'comparison-{group}-{language}'
        command([analysis,module,'--runs',prefix,*inputs]);command([probe,str(prefix)+'.psreport',*inputs])
        for i,path in enumerate(inputs):
            rows=read(path)[0]
            with Path(str(prefix)+f'-pendulum_{i+1}.csv').open(newline='') as f:lines=list(csv.reader(f))
            assert len(lines)==len(rows)+1 and len(lines[0])==4
            for line,row in zip(lines[1:],rows):assert all(near(float(a),b) for a,b in zip(line,(row[0],row[1],row[5],row[5]-rows[0][5])))
# A short run has no invented period row; a one-input legacy entry remains usable.
short=work/'refine-0-c-0.02.psrun'
for language,module in [('c',c_analysis),('phys',phys_analysis)]:
    prefix=work/f'single-{language}';command([analysis,module,short,prefix]);command([probe,str(prefix)+'.psreport',short])
# Empty input is rejected before publishing a usable report.
for language,module in [('c',c_analysis),('phys',phys_analysis)]:
    prefix=work/f'empty-{language}'
    result=subprocess.run([analysis,module,'--runs',str(prefix)],capture_output=True,timeout=30)
    assert result.returncode!=0 and not Path(str(prefix)+'.psreport').exists()
print(f'Pendulum tutorial: {total_rows} primary samples, five integrators, full scenes, nonlinear period/Taylor oracles, adaptive axes and mixed reports passed')
