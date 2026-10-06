"""Four damping regimes through both models, complete scene parity and mixed analyses."""
from pathlib import Path
import csv,math,struct,subprocess,sys,zlib
runner,analysis,c_model,phys_model,c_analysis,phys_analysis,probe,work=sys.argv[1:]
work=Path(work)
def command(args):
    p=subprocess.run(list(map(str,args)),capture_output=True,timeout=60)
    assert p.returncode==0,p.stderr
    return p
names=['position.x','velocity.x','energy.kinetic','energy.spring','energy','energy.dissipated','energy.balance','force.spring.x','force.damper.x','force.total.x','power.dissipated']
def read(path):
    rows=[];scenes=[];metadata='';footer=False
    with path.open('rb') as f:
        assert f.read(16)==b'PSRUN17\n'+struct.pack('<II',1,0x01020304)
        while h:=f.read(12):
            kind,size,crc=struct.unpack('<III',h);data=f.read(size)
            assert size<=8192 and len(data)==size and zlib.crc32(data)==crc
            if kind==1:metadata=data.decode()
            if kind==2:
                assert size==4+11*167 and struct.unpack_from('<I',data)[0]==11
                dimensions=[(1,0,0),(1,0,-1)]+[(2,1,-2)]*5+[(1,1,-2)]*3+[(2,1,-3)]
                for i,(name,dimension) in enumerate(zip(names,dimensions)):
                    off=4+i*167;assert data[off:off+48].split(b'\0')[0].decode()==name
                    assert struct.unpack_from('<7b',data,off+160)==dimension+(0,0,0,0)
            if kind==3:rows.append(struct.unpack('<12d',data))
            if kind==4:assert struct.unpack('<Q',data)[0]==len(rows);footer=True
            if kind==5:
                assert struct.unpack_from('<I',data)[0]==3 and struct.unpack_from('<II',data,12)==(11,13)
                scenes.append(data)
    assert footer
    return rows,scenes,metadata
def exact(c,t):
    alpha=c/2
    if c<8:
        w=math.sqrt(16-alpha*alpha);decay=math.exp(-alpha*t)
        return .35*decay*(math.cos(w*t)+alpha/w*math.sin(w*t)),-.35*decay*16/w*math.sin(w*t)
    if c==8:return .35*(1+4*t)*math.exp(-4*t),-.35*16*t*math.exp(-4*t)
    root=math.sqrt(alpha*alpha-16);a=-alpha+root;b=-alpha-root
    ca=-.35*b/(a-b);cb=.35-ca
    return ca*math.exp(a*t)+cb*math.exp(b*t),a*ca*math.exp(a*t)+b*cb*math.exp(b*t)
def near(a,b):return math.isfinite(a) and math.isfinite(b) and abs(a-b)<=1e-12*max(1,abs(a),abs(b))
for damping in [0,1.2,8,12]:
    simulations=[]
    for language,module in [('c',c_model),('phys',phys_model)]:
        path=work/f'{language}-{damping}.psrun';command([runner,module,path,'--steps','2000','--dt','.005','--param',f'damping={damping}','--record-scenes'])
        rows,scenes,metadata=read(path);simulations.append((rows,scenes))
        assert len(rows)==2001 and len(scenes)>100
        assert f'parameter.damping={damping:g}\n' in metadata and 'parameter_unit.damping=N s/m\n' in metadata
        assert 'parameter_dimension.damping=0,1,-1,0,0,0,0\n' in metadata and 'dissipated_work=integrated c*v^2' in metadata
        previous_energy=.98;previous_work=0
        for i,row in enumerate(rows):
            t,x,v,k,u,e,d,balance,fs,fd,total,power=row;ex,ev=exact(damping,t)
            assert t==i*.005 and all(math.isfinite(z) for z in row)
            assert abs(x-ex)<3e-8 and abs(v-ev)<1e-7
            exact_energy=.5*ev*ev+8*ex*ex
            assert abs(e-exact_energy)<1e-7 and abs(d-(.98-exact_energy))<1e-7 and abs(balance-.98)<1e-7
            assert near(k,.5*v*v) and near(u,8*x*x) and near(balance,e+d) and near(fs,-16*x) and near(fd,-damping*v) and near(total,fs+fd) and near(power,damping*v*v)
            assert e<=previous_energy+1e-10 and d>=previous_work-1e-13 and power>=0
            previous_energy=e;previous_work=d
        for analyzer,model in [('c',c_analysis),('phys',phys_analysis)]:
            prefix=work/f'report-{language}-{analyzer}-{damping}';command([analysis,model,path,prefix]);command([probe,str(prefix)+'.psreport',path])
            with Path(str(prefix)+'-spring_check.csv').open(newline='') as f:csv_rows=list(csv.reader(f))
            assert len(csv_rows)==2002 and len(csv_rows[0])==6
            for line,row in zip(csv_rows[1:],rows):assert all(near(float(a),b) for a,b in zip(line,(row[0],row[1],row[2],row[5],row[6],row[7])))
    (a,sa),(b,sb)=simulations
    for ra,rb in zip(a,b):assert all(near(x,y) for x,y in zip(ra,rb))
    assert len(sa)==len(sb)
    for x,y in zip(sa,sb):
        assert len(x)==len(y) and x[:28]==y[:28]
        offset=28+11*8
        for i in range(13):
            at=offset+i*176;assert x[at:at+8]==y[at:at+8] and x[at+96:at+176]==y[at+96:at+176]
            for field in range(8,96,8):assert near(struct.unpack_from('<d',x,at+field)[0],struct.unpack_from('<d',y,at+field)[0])
        for at in range(offset+13*176,len(x),8):assert near(struct.unpack_from('<d',x,at)[0],struct.unpack_from('<d',y,at)[0])
# Fourth-order refinement is independent of the model's own force helper.
for language,module in [('c',c_model),('phys',phys_model)]:
    errors=[]
    for dt,steps in [(0.05,20),(.025,40)]:
        path=work/f'{language}-refine-{dt}.psrun';command([runner,module,path,'--steps',str(steps),'--dt',str(dt)])
        row=read(path)[0][-1];x,v=exact(1.2,1);errors.append(math.hypot(row[1]-x,(row[2]-v)/4))
    assert 14<errors[0]/errors[1]<18,errors
    p=subprocess.run([runner,module,str(work/f'{language}-invalid.psrun'),'--steps','10','--dt','.005','--param','damping=-1'],capture_output=True)
    assert p.returncode==5 and (b'Invalid argument' in p.stderr or b'Invalid parameter' in p.stderr),p.stderr
print('Spring tutorial: 16008 measurements, four analytic regimes, all scene fields, parameter units, refinement and sixteen mixed analyses passed')
