"""Independent Decimal exponential oracle for coupled temperatures and ideal gas."""
from decimal import Decimal,localcontext
from pathlib import Path
import csv,math,struct,subprocess,sys,zlib
runner,analyzer,c_model,phys_model,c_analysis,phys_analysis,probe,work=sys.argv[1:]
work=Path(work);work.mkdir(parents=True,exist_ok=True)
D=lambda x:Decimal(str(x))
R=Decimal('8.31446261815324')
def command(args,code=0):
    p=subprocess.run(list(map(str,args)),capture_output=True,timeout=60)
    assert p.returncode==code,(args,p.returncode,p.stdout,p.stderr)
    return p
names=['temperature.a','temperature.b','heat.power','gas.pressure','gas.energy','energy.balance']
dimensions=[(0,0,0,0,1,0,0)]*2+[(2,1,-3,0,0,0,0),(-1,1,-2,0,0,0,0),(2,1,-2,0,0,0,0),(2,1,-2,0,0,0,0)]
def read(path):
    raw=path.read_bytes();assert raw[:16]==b'PSRUN17\n'+struct.pack('<II',1,0x01020304)
    rows=[];scenes=[];metadata='';at=16;footer=False;schema=False
    while at<len(raw):
        kind,size,crc=struct.unpack_from('<III',raw,at);data=raw[at+12:at+12+size];assert len(data)==size and zlib.crc32(data)==crc
        if kind==1:metadata=data.decode()
        if kind==2:
            assert struct.unpack_from('<I',data)[0]==6 and size==4+6*167
            for i,name in enumerate(names):
                offset=4+i*167;assert data[offset:offset+48].split(b'\0')[0].decode()==name
                assert struct.unpack_from('<7b',data,offset+160)==dimensions[i]
            schema=True
        if kind==3:rows.append(struct.unpack('<7d',data))
        if kind==4:assert struct.unpack('<Q',data)[0]==len(rows);footer=True
        if kind==5:scenes.append(data)
        at+=12+size
    assert footer and schema and rows and scenes
    return rows,scenes,metadata
def near(x,y):return math.isfinite(x) and abs(x-y)<=3e-12*max(1,abs(y))
scenarios=[(100,400,300,300,5),(100,300,300,400,5),(100,400,300,300,0),(100,350,300,350,5),(1,1000,10000,1,10000),(10000,1,1,1000,.001)]
measurements=0
for index,(ca,ta,cb,tb,g) in enumerate(scenarios):
    simulations=[]
    for language,module in [('c',c_model),('phys',phys_model)]:
        output=work/f'{language}-{index}.psrun'
        options=[]
        for key,value in zip(['capacityA','temperatureA','capacityB','temperatureB','conductance'],[ca,ta,cb,tb,g]):options+=['--param',f'{key}={value}']
        command([runner,module,output,'--steps','200','--dt','.15','--record-scenes',*options])
        rows,scenes,metadata=read(output);assert len(rows)==201 and len(scenes)>1
        simulations.append((rows,scenes));measurements+=len(rows)
        assert 'integrator=exact exponential' in metadata and 'parameter_unit.temperatureA=K' in metadata and 'parameter_unit.capacityA=J/K' in metadata
        with localcontext() as ctx:
            ctx.prec=65
            equilibrium=(D(ca)*D(ta)+D(cb)*D(tb))/(D(ca)+D(cb));rate=D(g)*(1/D(ca)+1/D(cb))
            for row in rows:
                time,a,b,power,pressure,energy,balance=row
                decay=(-rate*D(time)).exp();exact_a=equilibrium+(D(ta)-equilibrium)*decay;exact_b=equilibrium+(D(tb)-equilibrium)*decay
                expected=[exact_a,exact_b,D(g)*(exact_a-exact_b),D(ca)/D('12.5')*R*exact_a/D('.1'),D(ca)*exact_a,D(ca)*D(ta)+D(cb)*D(tb)]
                for actual,exact in zip(row[1:],expected):assert near(actual,float(exact)),(index,time,actual,exact)
                assert min(ta,tb)-1e-10<=a<=max(ta,tb)+1e-10 and min(ta,tb)-1e-10<=b<=max(ta,tb)+1e-10
        for name,module_analysis in [('c',c_analysis),('phys',phys_analysis)]:
            prefix=work/f'report-{index}-{language}-{name}';command([analyzer,module_analysis,output,prefix]);command([probe,str(prefix)+'.psreport',output])
            with Path(str(prefix)+'-thermal.csv').open(newline='') as f:csv_rows=list(csv.reader(f))
            assert len(csv_rows)==202
            for line,row in zip(csv_rows[1:],rows):assert all(near(float(a),b) for a,b in zip(line,row)) and len(line)==7
    (a,sa),(b,sb)=simulations
    assert len(sa)==len(sb)
    for ra,rb in zip(a,b):assert all(near(x,y) for x,y in zip(ra,rb))
    # Snapshot metadata and all object geometry/color fields agree independently.
    for x,y in zip(sa,sb):
        assert len(x)==len(y) and x[:28]==y[:28]
        assert struct.unpack_from('<II',x,12)==(6,3)
        for offset in range(28,28+6*8,8):assert near(struct.unpack_from('<d',x,offset)[0],struct.unpack_from('<d',y,offset)[0])
        for i in range(3):
            at=28+6*8+i*176;assert x[at:at+8]==y[at:at+8] and x[at+96:at+176]==y[at+96:at+176]
            for field in range(8,96,8):assert near(struct.unpack_from('<d',x,at+field)[0],struct.unpack_from('<d',y,at+field)[0])
# Exponential semigroup: 30 one-second runner steps equal 200 smaller steps.
for language,module in [('c',c_model),('phys',phys_model)]:
    path=work/f'{language}-single.psrun';command([runner,module,path,'--steps','30','--dt','1','--record-scenes'])
    final=read(path)[0][-1];reference=read(work/f'{language}-0.psrun')[0][-1]
    assert all(near(a,b) for a,b in zip(final,reference))
    command([runner,module,work/f'{language}-invalid.psrun','--steps','1','--dt','.1','--param','temperatureA=0'],code=5)
print(f'Thermal tutorial: {measurements} measurements, six Decimal oracles, 24 mixed analyses, full scene parity, SI metadata and exact time composition passed')
