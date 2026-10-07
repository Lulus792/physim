"""Discrete eigenmode and continuum/refinement oracle for an actual wave grid."""
from pathlib import Path
import csv,math,struct,subprocess,sys,zlib
runner,analyzer,c_model,phys_model,c_analysis,phys_analysis,probe,work=sys.argv[1:]
work=Path(work);work.mkdir(parents=True,exist_ok=True)
def command(args,code=0):
    p=subprocess.run(list(map(str,args)),capture_output=True,timeout=60)
    assert p.returncode==code,(args,p.returncode,p.stdout,p.stderr)
    return p
names=['displacement.center','reference.center','error.center','energy.discrete','courant','nodes']
dimensions=[(1,0,0,0,0,0,0)]*3+[(2,1,-2,0,0,0,0)]+[(0,0,0,0,0,0,0)]*2
def read(path):
    raw=path.read_bytes();assert raw[:16]==b'PSRUN17\n'+struct.pack('<II',1,0x01020304)
    rows=[];scenes=[];metadata='';at=16;footer=False;schema=False
    while at<len(raw):
        kind,size,crc=struct.unpack_from('<III',raw,at);data=raw[at+12:at+12+size];assert len(data)==size and zlib.crc32(data)==crc
        if kind==1:metadata=data.decode()
        if kind==2:
            assert struct.unpack_from('<I',data)[0]==6 and size==4+6*167
            for i,name in enumerate(names):
                offset=4+i*167;assert data[offset:offset+48].split(b'\0')[0].decode()==name and struct.unpack_from('<7b',data,offset+160)==dimensions[i]
            schema=True
        if kind==3:rows.append(struct.unpack('<7d',data))
        if kind==4:assert struct.unpack('<Q',data)[0]==len(rows);footer=True
        if kind==5:scenes.append(data)
        at+=12+size
    assert footer and schema and rows and scenes
    return rows,scenes,metadata
def near(x,y):return math.isfinite(x) and abs(x-y)<=3e-12*max(1,abs(y))
scenarios=[(65,1,1,1,.005),(65,2,1,1,.005),(65,3,4,1,.005),(33,1,1,1,.005),(65,1,1,1,.0025),(65,1,.25,1,.005)]
measurements=0
for index,(nodes,mode,tension,density,dt) in enumerate(scenarios):
    simulations=[];dx=1/(nodes-1);speed=math.sqrt(tension/density);courant=speed*dt/dx;theta=2*math.asin(courant*math.sin(mode*math.pi/(2*(nodes-1))))
    amplitude=.05
    shape=[0 if j in (0,nodes-1) else amplitude*math.sin(mode*math.pi*j/(nodes-1)) for j in range(nodes)]
    previous=[value*math.cos(theta) for value in shape]
    initial_energy=.5*density*dx*sum(((a-b)/dt)**2 for a,b in zip(shape[1:-1],previous[1:-1]))+.5*tension/dx*sum((shape[j+1]-shape[j])*(previous[j+1]-previous[j]) for j in range(nodes-1))
    for language,module in [('c',c_model),('phys',phys_model)]:
        output=work/f'{language}-{index}.psrun';options=[]
        for key,value in zip(['nodes','mode','tension','linearDensity'],[nodes,mode,tension,density]):options+=['--param',f'{key}={value}']
        command([runner,module,output,'--steps','200','--dt',str(dt),'--record-scenes',*options])
        rows,scenes,metadata=read(output);assert len(rows)==201 and len(scenes)>1;measurements+=len(rows);simulations.append((rows,scenes))
        assert 'integrator=centered second-order leapfrog' in metadata and 'parameter_unit.tension=N' in metadata and 'parameter_unit.linearDensity=kg/m' in metadata
        for i,row in enumerate(rows):
            time,center,reference,error,energy,lam,count=row
            exact=shape[nodes//2]*math.cos(i*theta);continuous=amplitude*math.sin(mode*math.pi*.5)*math.cos(speed*mode*math.pi*time)
            assert near(center,exact) and near(reference,continuous) and near(error,center-reference),(index,i,row,exact)
            assert abs(energy-initial_energy)<3e-13 and lam==courant and count==nodes
        for frame in scenes:
            assert struct.unpack_from('<II',frame,12)==(6,1)
            scene_time=struct.unpack_from('<d',frame,4)[0];step=round(scene_time/dt)
            point_count=struct.unpack_from('<I',frame,24)[0];assert point_count==nodes
            points_at=28+6*8+176
            assert len(frame)==points_at+nodes*24
            for j in range(nodes):
                x,y,z=struct.unpack_from('<3d',frame,points_at+j*24)
                assert near(x,j*dx-.5) and near(y,shape[j]*math.cos(step*theta)) and z==0,(index,step,j,x,y,z)
        for name,module_analysis in [('c',c_analysis),('phys',phys_analysis)]:
            prefix=work/f'report-{index}-{language}-{name}';command([analyzer,module_analysis,output,prefix]);command([probe,str(prefix)+'.psreport',output])
            with Path(str(prefix)+'-string.csv').open(newline='') as f:csv_rows=list(csv.reader(f))
            assert len(csv_rows)==202
            for line,row in zip(csv_rows[1:],rows):assert len(line)==7 and all(near(float(a),b) for a,b in zip(line,row))
    (a,sa),(b,sb)=simulations
    for ra,rb in zip(a,b):assert all(near(x,y) for x,y in zip(ra,rb))
    assert len(sa)==len(sb)
    for x,y in zip(sa,sb):
        assert len(x)==len(y) and x[:28]==y[:28]
        obj=28+6*8;assert x[obj:obj+8]==y[obj:obj+8] and x[obj+96:obj+176]==y[obj+96:obj+176]
        for off in list(range(28,obj,8))+list(range(obj+8,obj+96,8))+list(range(obj+176,len(x),8)):
            assert near(struct.unpack_from('<d',x,off)[0],struct.unpack_from('<d',y,off)[0])
# Refine space and time together at fixed Courant number: second-order dispersion.
for language,module in [('c',c_model),('phys',phys_model)]:
    errors=[]
    for nodes in (17,33,65):
        dt=.4/(nodes-1);steps=(nodes-1);output=work/f'{language}-refine-{nodes}.psrun'
        command([runner,module,output,'--steps',str(steps),'--dt',str(dt),'--param',f'nodes={nodes}','--record-scenes'])
        row=read(output)[0][-1];errors.append(abs(row[1]-.05*math.cos(math.pi*.4)))
    assert 3.9<errors[0]/errors[1]<4.1 and 3.9<errors[1]/errors[2]<4.1,errors
    command([runner,module,work/f'{language}-unstable.psrun','--steps','1','--dt','1'],code=5)
    command([runner,module,work/f'{language}-even-nodes.psrun','--steps','1','--dt','.005','--param','nodes=64'],code=5)
print(f'String tutorial: {measurements} grid measurements, six discrete/continuum oracles, full polylines, second-order refinement, energy and 24 mixed analyses passed')
