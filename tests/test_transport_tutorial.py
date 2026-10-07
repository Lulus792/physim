"""Independent hydraulic split and complex Fourier amplification transport oracle."""
from pathlib import Path
import cmath,csv,math,struct,subprocess,sys,zlib
runner,analyzer,c_model,phys_model,c_analysis,phys_analysis,probe,work=sys.argv[1:]
work=Path(work);work.mkdir(parents=True,exist_ok=True)
def command(args,code=0):
    p=subprocess.run(list(map(str,args)),capture_output=True,timeout=60)
    assert p.returncode==code,(args,p.returncode,p.stdout,p.stderr)
    return p
names=['concentration.center','reference.center','error.center','mass.perArea','flow.inlet','pressure.junction','reynolds','stability']
dimensions=[(-3,1,0,0,0,0,0)]*3+[(-2,1,0,0,0,0,0),(3,0,-1,0,0,0,0),(-1,1,-2,0,0,0,0)]+[(0,0,0,0,0,0,0)]*2
def read(path):
    raw=path.read_bytes();assert raw[:16]==b'PSRUN17\n'+struct.pack('<II',1,0x01020304)
    rows=[];scenes=[];metadata='';at=16;footer=False;schema=False
    while at<len(raw):
        kind,size,crc=struct.unpack_from('<III',raw,at);data=raw[at+12:at+12+size];assert len(data)==size and zlib.crc32(data)==crc
        if kind==1:metadata=data.decode()
        if kind==2:
            assert struct.unpack_from('<I',data)[0]==8 and size==4+8*167
            for i,name in enumerate(names):
                offset=4+i*167;assert data[offset:offset+48].split(b'\0')[0].decode()==name and struct.unpack_from('<7b',data,offset+160)==dimensions[i]
            schema=True
        if kind==3:rows.append(struct.unpack('<9d',data))
        if kind==4:assert struct.unpack('<Q',data)[0]==len(rows);footer=True
        if kind==5:scenes.append(data)
        at+=12+size
    assert footer and schema and rows and scenes
    return rows,scenes,metadata
def near(x,y):return math.isfinite(x) and abs(x-y)<=3e-12*max(1,abs(y))
scenarios=[(100,.01,.005,.0001),(-100,.01,.005,.0001),(0,.01,.005,.0001),(100,.01,.005,0),(100,.02,.005,.0001),(100,.01,.003,.0001)]
measurements=0
for index,(inlet,viscosity,radius,diffusion) in enumerate(scenarios):
    simulations=[];nodes=64;dx=1/nodes;dt=.05
    conductance=math.pi*radius**4/(8*viscosity);pressure=inlet/3;flow=2*conductance*inlet/3
    velocity=radius**2*inlet/(12*viscosity);reynolds=1000*abs(velocity)*2*radius/viscosity
    a=abs(velocity)*dt/dx;d=diffusion*dt/dx**2;z=complex(1-(a+2*d)*(1-math.cos(2*math.pi/nodes)),-velocity*dt/dx*math.sin(2*math.pi/nodes))
    for language,module in [('c',c_model),('phys',phys_model)]:
        output=work/f'{language}-{index}.psrun';options=[]
        for key,value in zip(['inletPressure','viscosity','radius','diffusivity'],[inlet,viscosity,radius,diffusion]):options+=['--param',f'{key}={value}']
        command([runner,module,output,'--steps','200','--dt',str(dt),'--record-scenes',*options])
        rows,scenes,metadata=read(output);assert len(rows)==201 and len(scenes)>1;measurements+=len(rows);simulations.append((rows,scenes))
        assert 'integrator=upwind advection and explicit centered diffusion' in metadata and 'parameter_unit.viscosity=Pa s' in metadata and 'parameter_unit.diffusivity=m^2/s' in metadata
        for i,row in enumerate(rows):
            time,center,reference,error,mass,q,p,re,stability=row
            exact=.2+.1*(-z**i).real;continuous=.2+.1*math.cos(2*math.pi*(.5-velocity*time))*math.exp(-diffusion*(2*math.pi)**2*time)
            expected=[exact,continuous,exact-continuous,.2,flow,pressure,reynolds,a+2*d]
            assert all(near(actual,truth) for actual,truth in zip(row[1:],expected)),(index,i,row,expected)
        for frame in scenes:
            assert struct.unpack_from('<II',frame,12)==(8,4) and struct.unpack_from('<I',frame,24)[0]==65
            scene_time=struct.unpack_from('<d',frame,4)[0];step=round(scene_time/dt);points_at=28+8*8+4*176
            assert len(frame)==points_at+65*24
            for j in range(65):
                x,y,zz=struct.unpack_from('<3d',frame,points_at+j*24)
                expected=.2+.1*(cmath.exp(2j*math.pi*(j%64)/64)*z**step).real
                assert near(x,j/64-.5) and near(y,expected) and zz==0,(index,step,j,x,y,expected)
                assert .1-1e-12<=y<=.3+1e-12
        for name,module_analysis in [('c',c_analysis),('phys',phys_analysis)]:
            prefix=work/f'report-{index}-{language}-{name}';command([analyzer,module_analysis,output,prefix]);command([probe,str(prefix)+'.psreport',output])
            with Path(str(prefix)+'-transport.csv').open(newline='') as f:csv_rows=list(csv.reader(f))
            assert len(csv_rows)==202
            for line,row in zip(csv_rows[1:],rows):assert len(line)==9 and all(near(float(a),b) for a,b in zip(line,row))
    (a_rows,sa),(b_rows,sb)=simulations
    for ra,rb in zip(a_rows,b_rows):assert all(near(x,y) for x,y in zip(ra,rb))
    assert len(sa)==len(sb)
    for x,y in zip(sa,sb):
        assert len(x)==len(y) and x[:28]==y[:28]
        obj=28+8*8
        for i in range(4):
            at=obj+i*176;assert x[at:at+8]==y[at:at+8] and x[at+96:at+176]==y[at+96:at+176]
            for off in range(at+8,at+96,8):assert near(struct.unpack_from('<d',x,off)[0],struct.unpack_from('<d',y,off)[0])
        for off in list(range(28,obj,8))+list(range(obj+4*176,len(x),8)):assert near(struct.unpack_from('<d',x,off)[0],struct.unpack_from('<d',y,off)[0])
for language,module in [('c',c_model),('phys',phys_model)]:
    # Reject instability and out-of-regime pipe use instead of reporting a CFD result.
    command([runner,module,work/f'{language}-unstable.psrun','--steps','1','--dt','1','--param','diffusivity=.0005'],code=5)
    command([runner,module,work/f'{language}-invalid-reynolds.psrun','--steps','1','--dt','.05','--param','viscosity=.001'],code=5)
print(f'Transport tutorial: {measurements} measurements, six hydraulic/Fourier/continuum profiles, all 65 periodic points, mass/positivity and 24 mixed analyses passed')
