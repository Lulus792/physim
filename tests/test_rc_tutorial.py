"""Independent high precision RC trajectory and integral-of-Joule-power oracle."""
from decimal import Decimal,localcontext
from pathlib import Path
import csv,math,struct,subprocess,sys,zlib
runner,analyzer,c_model,phys_model,c_analysis,phys_analysis,probe,work=sys.argv[1:]
work=Path(work);work.mkdir(parents=True,exist_ok=True)
D=lambda x:Decimal(str(x))
def command(args,code=0):
    p=subprocess.run(list(map(str,args)),capture_output=True,timeout=60)
    assert p.returncode==code,(args,p.returncode,p.stdout,p.stderr)
    return p
names=['voltage','current','power.resistor','energy.capacitor','energy.dissipated','energy.source','energy.balance']
dimensions=[(2,1,-3,-1,0,0,0),(0,0,0,1,0,0,0),(2,1,-3,0,0,0,0)]+[(2,1,-2,0,0,0,0)]*4
def read(path):
    raw=path.read_bytes();assert raw[:16]==b'PSRUN17\n'+struct.pack('<II',1,0x01020304)
    rows=[];scenes=[];metadata='';at=16;footer=False;schema=False
    while at<len(raw):
        kind,size,crc=struct.unpack_from('<III',raw,at);data=raw[at+12:at+12+size];assert len(data)==size and zlib.crc32(data)==crc
        if kind==1:metadata=data.decode()
        if kind==2:
            assert struct.unpack_from('<I',data)[0]==7 and size==4+7*167
            for i,name in enumerate(names):
                offset=4+i*167;assert data[offset:offset+48].split(b'\0')[0].decode()==name
                assert struct.unpack_from('<7b',data,offset+160)==dimensions[i]
            schema=True
        if kind==3:rows.append(struct.unpack('<8d',data))
        if kind==4:assert struct.unpack('<Q',data)[0]==len(rows);footer=True
        if kind==5:scenes.append(data)
        at+=12+size
    assert footer and schema and rows and scenes
    return rows,scenes,metadata
def near(x,y):return math.isfinite(x) and abs(x-y)<=3e-12*max(1,abs(y))
scenarios=[(1000,.002,12,0),(1000,.002,0,12),(1000,.002,-12,12),(1000,.002,12,20),(1000,.002,12,12),(1,.000001,12,0),(1000000,1,-1000,1000)]
measurements=0
for index,(resistance,capacitance,source,initial) in enumerate(scenarios):
    simulations=[]
    for language,module in [('c',c_model),('phys',phys_model)]:
        output=work/f'{language}-{index}.psrun';options=[]
        for key,value in zip(['resistance','capacitance','sourceVoltage','initialVoltage'],[resistance,capacitance,source,initial]):options+=['--param',f'{key}={value}']
        command([runner,module,output,'--steps','200','--dt','.05','--record-scenes',*options])
        rows,scenes,metadata=read(output);assert len(rows)==201 and len(scenes)>1
        simulations.append((rows,scenes));measurements+=len(rows)
        assert 'integrator=exact exponential' in metadata and 'parameter_unit.resistance=ohm' in metadata and 'parameter_unit.capacitance=F' in metadata and 'parameter_unit.sourceVoltage=V' in metadata
        previous_heat=0
        with localcontext() as ctx:
            ctx.prec=65
            r,c,vs,v0=map(D,(resistance,capacitance,source,initial));energy0=c*v0*v0/2
            for row in rows:
                time,voltage,current,power,energy,heat,source_work,balance=row
                decay=(-D(time)/(r*c)).exp();exact_voltage=vs+(v0-vs)*decay;exact_current=(vs-v0)/r*decay
                # Integral_0^t R I^2 ds = C(V0-Vs)^2/2 (1-exp(-2t/RC)).
                exact_heat=c*(v0-vs)**2/2*(1-decay*decay)
                expected=[exact_voltage,exact_current,r*exact_current**2,c*exact_voltage**2/2,exact_heat,vs*c*(exact_voltage-v0),energy0]
                for actual,exact in zip(row[1:],expected):assert near(actual,float(exact)),(index,time,actual,exact)
                assert heat>=previous_heat-3e-9 and power>=0 and min(initial,source)-1e-10<=voltage<=max(initial,source)+1e-10
                previous_heat=heat
        for name,module_analysis in [('c',c_analysis),('phys',phys_analysis)]:
            prefix=work/f'report-{index}-{language}-{name}';command([analyzer,module_analysis,output,prefix]);command([probe,str(prefix)+'.psreport',output])
            with Path(str(prefix)+'-rc.csv').open(newline='') as f:csv_rows=list(csv.reader(f))
            assert len(csv_rows)==202
            for line,row in zip(csv_rows[1:],rows):assert len(line)==8 and all(near(float(a),b) for a,b in zip(line,row))
    (a,sa),(b,sb)=simulations
    assert len(sa)==len(sb)
    for ra,rb in zip(a,b):assert all(near(x,y) for x,y in zip(ra,rb))
    for x,y in zip(sa,sb):
        assert len(x)==len(y) and x[:28]==y[:28] and struct.unpack_from('<II',x,12)==(7,3)
        for offset in range(28,28+7*8,8):assert near(struct.unpack_from('<d',x,offset)[0],struct.unpack_from('<d',y,offset)[0])
        for i in range(3):
            at=28+7*8+i*176;assert x[at:at+8]==y[at:at+8] and x[at+96:at+176]==y[at+96:at+176]
            for field in range(8,96,8):assert near(struct.unpack_from('<d',x,at+field)[0],struct.unpack_from('<d',y,at+field)[0])
for language,module in [('c',c_model),('phys',phys_model)]:
    path=work/f'{language}-large-steps.psrun';command([runner,module,path,'--steps','10','--dt','1','--record-scenes'])
    final=read(path)[0][-1];reference=read(work/f'{language}-0.psrun')[0][-1]
    assert all(near(a,b) for a,b in zip(final,reference))
    command([runner,module,work/f'{language}-invalid.psrun','--steps','1','--dt','.1','--param','resistance=0'],code=5)
print(f'RC tutorial: {measurements} measurements, seven Decimal/Joule-power oracles, 28 mixed analyses, full scenes, SI units and exact composition passed')
