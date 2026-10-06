"""Independent 1-D impulse, trajectory and energy oracles for both tutorial languages."""
from pathlib import Path
import csv,math,struct,subprocess,sys,zlib
runner,analysis,c_model,phys_model,c_analysis,phys_analysis,probe,work=sys.argv[1:]
work=Path(work)
names=['a.position','b.position','a.velocity','b.velocity','energy','momentum.x','energy.dissipated','energy.balance','a.impulse','collision.time','collision.count']
def command(args,expected=0):
    result=subprocess.run(list(map(str,args)),capture_output=True,timeout=60)
    assert result.returncode==expected,(args,result.returncode,result.stderr)
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
                assert size==4+11*167
                dims=[(1,0,0)]*2+[(1,0,-1)]*2+[(2,1,-2),(1,1,-1),(2,1,-2),(2,1,-2),(1,1,-1),(0,0,1),(0,0,0)]
                for i,(name,dim) in enumerate(zip(names,dims)):
                    at=4+i*167;assert data[at:at+48].split(b'\0')[0].decode()==name
                    assert struct.unpack_from('<7b',data,at+160)==dim+(0,0,0,0)
            if kind==3:rows.append(struct.unpack('<12d',data))
            if kind==4:assert struct.unpack('<Q',data)[0]==len(rows);footer=True
            if kind==5:
                assert struct.unpack_from('<I',data)[0]==3 and struct.unpack_from('<I',data,12)[0]==11
                assert struct.unpack_from('<I',data,16)[0] in (8,10)
                scenes.append(data)
    assert footer and rows
    return rows,scenes,metadata

def near(a,b,tol=2e-11):return math.isfinite(a) and math.isfinite(b) and abs(a-b)<=tol*max(1,abs(a),abs(b))
settings=[('elastic',{},.005,400),('partial',{'restitution':.5},.005,400),('inelastic',{'restitution':0},.005,400),
    ('unequal-elastic',{'massA':2},.005,400),('unequal-inelastic',{'massA':2,'restitution':0},.005,400),
    ('unequal-partial',{'massA':.3,'massB':4,'restitution':.35,'velocityA':1.2,'velocityB':0},.005,400),
    ('separating',{'velocityA':-.6,'velocityB':.6},.005,400),('resting',{'velocityA':0,'velocityB':0},.005,400),
    ('comoving',{'velocityA':.4,'velocityB':.4},.005,400),('coarse-ccd',{'velocityA':5,'velocityB':-5},.3,4),
    ('boundary',{'velocityA':1,'velocityB':-1,'restitution':.5},.2,10)]
total=0
for name,changes,dt,steps in settings:
    props=dict(massA=1,massB=1,velocityA=.6,velocityB=-.6,restitution=1);props.update(changes)
    ma,mb,ua,ub,e=(props[k] for k in ('massA','massB','velocityA','velocityB','restitution'))
    closing=ua-ub;contact=1.6/closing if closing>0 else math.inf
    impulse=-(1+e)*ma*mb/(ma+mb)*closing
    va=ua+impulse/ma;vb=ub-impulse/mb
    dissipated=.5*ma*mb/(ma+mb)*(1-e*e)*closing*closing
    initial=.5*ma*ua*ua+.5*mb*ub*ub;momentum=ma*ua+mb*ub
    pair=[]
    for language,module in [('c',c_model),('phys',phys_model)]:
        path=work/f'{name}-{language}.psrun';args=[runner,module,path,'--steps',str(steps),'--dt',str(dt),'--record-scenes']
        for key,value in changes.items():args+=['--param',f'{key}={value}']
        command(args);rows,scenes,metadata=read(path);pair.append((rows,scenes));total+=len(rows)
        assert len(rows)==steps+1 and 'ccd=linear sphere sweep, impulse, remaining time' in metadata
        for key,unit,dim in [('massA','kg','0,1,0,0,0,0,0'),('massB','kg','0,1,0,0,0,0,0'),('velocityA','m/s','1,0,-1,0,0,0,0'),('velocityB','m/s','1,0,-1,0,0,0,0'),('restitution','1','0,0,0,0,0,0,0')]:
            assert f'parameter_unit.{key}={unit}\n' in metadata and f'parameter_dimension.{key}={dim}\n' in metadata
        impulses=[];previous_count=0
        for i,row in enumerate(rows):
            t,xa,xb,aval,bval,k,p,d,balance,j,event,count=row
            assert t==i*dt and count in (0,1) and count>=previous_count
            occurred=count==1
            if occurred:
                assert abs(event-contact)<2e-11 and t+2e-11>=contact
                ea=-1+ua*contact+va*(t-contact);eb=1+ub*contact+vb*(t-contact)
                assert near(aval,va) and near(bval,vb) and near(d,dissipated)
            else:
                assert t<=contact+2e-11 and event==0 and d==0
                ea=-1+ua*t;eb=1+ub*t;assert near(aval,ua) and near(bval,ub)
            assert near(xa,ea) and near(xb,eb) and xb-xa>=.4-2e-11
            assert near(k,.5*ma*aval*aval+.5*mb*bval*bval) and near(p,momentum) and near(balance,initial) and near(k+d,balance)
            if j:
                assert count==1 and previous_count==0 and near(j,impulse);impulses.append(j)
            previous_count=count
        expected_hit=contact<=steps*dt+2e-11
        assert rows[-1][-1]==int(expected_hit) and len(impulses)==int(expected_hit)
        if e==0 and expected_hit:assert near(rows[-1][3],rows[-1][4])
        for analyzer,model in [('c',c_analysis),('phys',phys_analysis)]:
            prefix=work/f'report-{name}-{language}-{analyzer}'
            command([analysis,model,path,prefix]);command([probe,str(prefix)+'.psreport',path])
            with Path(str(prefix)+'-collision.csv').open(newline='') as f:lines=list(csv.reader(f))
            assert len(lines)==len(rows)+1 and len(lines[0])==12
            for line,row in zip(lines[1:],rows):assert all(near(float(x),y) for x,y in zip(line,row))
    (a,sa),(b,sb)=pair
    for ra,rb in zip(a,b):assert all(near(x,y) for x,y in zip(ra,rb))
    assert len(sa)==len(sb)
    for x,y in zip(sa,sb):
        assert len(x)==len(y) and x[:28]==y[:28]
        count=struct.unpack_from('<I',x,16)[0];offset=28+11*8
        for i in range(count):
            at=offset+i*176;assert x[at:at+8]==y[at:at+8] and x[at+96:at+176]==y[at+96:at+176]
            for field in range(8,96,8):assert near(struct.unpack_from('<d',x,at+field)[0],struct.unpack_from('<d',y,at+field)[0])
# Event time and post-impact trajectories remain exact with one very large interval.
for language,module in [('c',c_model),('phys',phys_model)]:
    path=work/f'one-step-{language}.psrun';command([runner,module,path,'--steps','1','--dt','1','--param','massA=2','--param','restitution=0','--param','velocityA=1','--param','velocityB=-1'])
    row=read(path)[0][-1];assert near(row[1],-.2+(1-.8)/3) and near(row[2],.2+(1-.8)/3) and near(row[10],.8) and row[11]==1
    for label,args in [('restitution',['--param','restitution=1.1']),('mass',['--param','massA=0'])]:
        command([runner,module,work/f'invalid-{label}-{language}.psrun','--steps','1',*args],5)
for language,module in [('c',c_analysis),('phys',phys_analysis)]:
    for label,inputs in [('empty',[]),('multiple',[work/'elastic-c.psrun',work/'inelastic-phys.psrun'])]:
        prefix=work/f'rejected-{label}-{language}'
        result=subprocess.run([analysis,module,'--runs',str(prefix),*map(str,inputs)],capture_output=True,timeout=30)
        assert result.returncode!=0 and not Path(str(prefix)+'.psreport').exists()
print(f'Collision tutorial: {total} samples, eleven exact scenarios, full scenes, forty-four mixed reports and coarse/boundary CCD passed')
