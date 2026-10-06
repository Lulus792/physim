"""Independent analytic physics, C/Physim parity, mixed analyses and rejected model ranges."""
from pathlib import Path
import csv,math,struct,subprocess,sys,zlib
runner,analysis,c_model,phys_model,c_analysis,phys_analysis,report_probe,work=sys.argv[1:]
work=Path(work)
def run(command):
    return subprocess.run(list(map(str,command)),capture_output=True,timeout=60)
def read(path):
    rows=[];scenes=[];metadata=None;footer=False
    with path.open('rb') as f:
        assert f.read(16)==b'PSRUN17\n'+struct.pack('<II',1,0x01020304)
        while header:=f.read(12):
            kind,size,crc=struct.unpack('<III',header);data=f.read(size)
            assert size<=8192 and len(data)==size and zlib.crc32(data)==crc
            if kind==1:metadata=data.decode()
            if kind==2:
                assert size==4+13*167 and struct.unpack_from('<I',data)[0]==13
                names=['position.y','velocity.y','mass','force.weight','force.buoyancy','force.drag','reynolds','energy.kinetic','energy.dissipated','energy.potential','energy.balance','reference.y','reference.velocity']
                dimensions=[(1,0,0),(1,0,-1),(0,1,0),(1,1,-2),(1,1,-2),(1,1,-2),(0,0,0),(2,1,-2),(2,1,-2),(2,1,-2),(2,1,-2),(1,0,0),(1,0,-1)]
                for i,(name,dimension) in enumerate(zip(names,dimensions)):
                    offset=4+i*167
                    assert data[offset:offset+48].split(b'\0',1)[0].decode()==name
                    assert struct.unpack_from('<7b',data,offset+160)==dimension+(0,0,0,0)
            if kind==3:rows.append(struct.unpack('<14d',data))
            if kind==4:assert struct.unpack('<Q',data)[0]==len(rows);footer=True
            if kind==5:
                assert struct.unpack_from('<I',data)[0]==3
                assert struct.unpack_from('<II',data,12)==(13,5)
                scenes.append(data)
    assert footer and len(rows)==1001 and len(scenes)>10
    return rows,scenes,metadata
def close(x,y,tol=1e-12):return math.isfinite(x) and math.isfinite(y) and abs(x-y)<=tol*max(1,abs(x),abs(y))
settings=[('default',{},.001),('rising',{'materialDensity':500},.0005),('neutral',{'materialDensity':1000},.001),('viscous',{'viscosity':200},.0005),('smaller',{'radius':.025,'viscosity':50},.0005),('vacuum',{'mediumDensity':0,'viscosity':0},.001),('half-step',{},.0005)]
errors={};runs={}
for name,changes,dt in settings:
    props=dict(materialDensity=2500,mediumDensity=1000,viscosity=100,radius=.05);props.update(changes)
    volume=4/3*math.pi*props['radius']**3;mass=props['materialDensity']*volume
    weight=-mass*9.80665;buoyancy=props['mediumDensity']*volume*9.80665
    drag_coefficient=6*math.pi*props['viscosity']*props['radius'];rate=drag_coefficient/mass
    acceleration=(weight+buoyancy)/mass
    pair=[]
    for language,module in [('c',c_model),('phys',phys_model)]:
        path=work/f'{name}-{language}.psrun';args=[runner,module,path,'--steps','1000','--dt',str(dt),'--seed','42','--record-scenes']
        for key,value in changes.items():args+=['--param',f'{key}={value}']
        result=run(args);assert result.returncode==0,(name,language,result.stderr)
        rows,scenes,metadata=read(path);pair.append((rows,scenes));runs[name,language]=path
        for text in ('integrator=RK4','forces=weight, full-volume buoyancy, Stokes drag','validity=Re<=0.1; lambda*dt<=0.25','excluded=walls, contacts, surface, added mass, history force, non-Newtonian rheology'):assert text in metadata
        maximum=0
        for i,row in enumerate(rows):
            t,y,v,m,w,b,d,re,k,e,p,total,ry,rv=row
            onset=-math.expm1(-rate*t) if rate else 0
            exact_v=acceleration/rate*onset if rate else acceleration*t
            exact_y=.5+acceleration/rate*(t-onset/rate) if rate else .5+.5*acceleration*t*t
            assert t==i*dt and all(math.isfinite(x) for x in row)
            assert close(m,mass) and close(w,weight) and close(b,buoyancy) and close(d,-drag_coefficient*v)
            assert close(ry,exact_y) and close(rv,exact_v)
            assert abs(y-exact_y)<1e-8 and abs(v-exact_v)<2e-7,(name,i,y,v,exact_y,exact_v)
            assert 0<=re<=.1 and k>=0 and e>=0 and close(k,.5*mass*v*v) and close(p,-(weight+buoyancy)*y)
            assert abs(total-rows[0][11])<1e-7 and close(total,k+e+p)
            maximum=max(maximum,abs(v-exact_v))
        if language=='c':errors[name]=maximum
    a,b=pair
    for ra,rb in zip(a[0],b[0]):assert all(close(x,y) for x,y in zip(ra,rb))
    for sa,sb in zip(a[1],b[1]):
        assert len(sa)==len(sb) and sa[:28]==sb[:28]
        offset=28+13*8
        for i in range(5):
            start=offset+i*176
            assert sa[start:start+8]==sb[start:start+8] and sa[start+96:start+176]==sb[start+96:start+176]
            for field in range(8,96,8):assert close(struct.unpack_from('<d',sa,start+field)[0],struct.unpack_from('<d',sb,start+field)[0])
    if name=='neutral':assert all(r[1]==.5 and r[2]==0 and r[11]==0 for r in a[0])
assert errors['half-step']<errors['default']/14,errors
# All four experiment/analysis-language combinations use the complete default run.
for source in ('c','phys'):
    for language,module in [('c',c_analysis),('phys',phys_analysis)]:
        prefix=work/f'report-{source}-{language}';result=run([analysis,module,runs['default',source],prefix]);assert result.returncode==0,result.stderr
        result=run([report_probe,str(prefix)+'.psreport',runs['default',source]]);assert result.returncode==0,result.stderr
        with Path(str(prefix)+'-material_check.csv').open(newline='') as f:lines=list(csv.reader(f))
        assert len(lines)==1002 and len(lines[0])==5
        rows=read(runs['default',source])[0]
        for line,row in zip(lines[1:],rows):assert all(close(float(x),y) for x,y in zip(line,(row[0],row[2],row[13],row[2]-row[13],row[11])))
# The shared contract rejects invalid physics and unstable steps, preserving data prefixes.
for name,extra in [('unstable',['--dt','.01']),('inviscid-fluid',['--param','viscosity=0']),('high-reynolds',['--param','mediumDensity=9000','--param','viscosity=10'])]:
    for language,module in [('c',c_model),('phys',phys_model)]:
        result=run([runner,module,work/f'invalid-{name}-{language}.psrun','--steps','1000','--dt','.001',*extra]);assert result.returncode!=0,(name,language)
        assert b'lambda*dt' in result.stderr if name=='unstable' else b'positive viscosity' in result.stderr if name=='inviscid-fluid' else b'Reynolds' in result.stderr,(name,result.stderr)
# 80-digit oracle detects cancellation independently of C/Physim parity.
from decimal import Decimal, localcontext
with localcontext() as context:
    context.prec=80
    for viscosity in (0,1e-12,1e-8,1e-4,1,200,222.222,222.223,1000):
        rate=4.5e-4*viscosity
        dt=.001
        for language,module in [('c',c_model),('phys',phys_model)]:
            path=work/f'phi-{viscosity}-{language}.psrun'
            result=run([runner,module,path,'--steps','1000','--dt',str(dt),'--record-scenes',
                        '--param','materialDensity=10000','--param','mediumDensity=0',
                        '--param','radius=1','--param',f'viscosity={viscosity}'])
            assert result.returncode==0,(viscosity,language,result.stderr)
            rows,_,_=read(path)
            lam=Decimal(str(rate));acc=Decimal('-9.80665')
            for row in rows:
                t=Decimal(str(row[0]))
                if lam:
                    onset=1-(-lam*t).exp()
                    ey=Decimal('.5')+acc/lam*(t-onset/lam)
                    ev=acc/lam*onset
                else:ey=Decimal('.5')+acc*t*t/2;ev=acc*t
                assert close(row[12],float(ey),2e-13),(viscosity,language,row[0],row[12],ey)
                assert close(row[13],float(ev),2e-13),(viscosity,language,row[0],row[13],ev)
print('Material reference: 18 logarithmic C/Physim runs against 80-digit Decimal oracle passed')
print('Material tutorial: seven analytic scenarios, complete C/Physim measurements/scenes, fourth-order refinement, four mixed analyses and six rejected models passed')
