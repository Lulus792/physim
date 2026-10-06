"""Independent archived fixtures, cross-language reports, reconstruction and immutable inputs."""
from pathlib import Path
import csv,hashlib,math,struct,subprocess,sys,zlib
runner,analysis,c_model,phys_model,c_analysis,phys_analysis,probe,work=sys.argv[1:]
work=Path(work)
def command(args):
    p=subprocess.run(list(map(str,args)),capture_output=True,timeout=90)
    assert p.returncode==0,(args,p.returncode,p.stderr)
    return p

def chunk(kind,data):return struct.pack('<III',kind,len(data),zlib.crc32(data))+data
def fixture(path,times,positions,complete=True,name='position.x',dimension=(1,0,0,0,0,0,0),symbol='m'):
    schema=name.encode().ljust(48,b'\0')+symbol.encode().ljust(16,b'\0')+b'Independent archived fixture'.ljust(96,b'\0')+struct.pack('<7b',*dimension)
    data=b'PSRUN17\n'+struct.pack('<II',1,0x01020304)+chunk(1,b'model=independent archived fixture\n')+chunk(2,struct.pack('<I',1)+schema)
    for t,x in zip(times,positions):data+=chunk(3,struct.pack('<2d',t,x))
    if complete:data+=chunk(4,struct.pack('<Q',len(times)))
    path.write_bytes(data)

def near(a,b):return math.isfinite(a) and math.isfinite(b) and abs(a-b)<2e-11*max(1,abs(a),abs(b))
cases={}
for language,module in [('c',c_model),('phys',phys_model)]:
    path=work/f'uniform-{language}.psrun';command([runner,module,path,'--steps','32','--dt','.0625','--seed','42'])
    cases[language]=(path,[i/16 for i in range(33)],[1.5*i/16 for i in range(33)],False)
for name,times,positions,recovered,symbol in [
    ('irregular',[0,.125,.3,.9,1.5],[-2+3*t+t*t for t in [0,.125,.3,.9,1.5]],False,'m'),
    ('shifted',[10,10.5,11.75,12],[-4,-3,-.5,0],False,'m'),
    ('two',[2,5],[4,-2],False,'m'),
    ('recovered',[0,.25,.5],[0,.375,.75],True,'m'),
    ('centimetre-symbol',[0,.5,1],[0,.75,1.5],False,'cm'),
    ('large',[i/256 for i in range(2049)],[math.sin(i/256)+i/512 for i in range(2049)],False,'m')]:
    path=work/f'{name}.psrun';fixture(path,times,positions,not recovered,symbol=symbol);cases[name]=(path,times,positions,recovered)
for name,(path,times,positions,recovered) in cases.items():
    original=hashlib.sha256(path.read_bytes()).digest()
    expected_v=[(positions[min(i+1,len(times)-1)]-positions[max(i-1,0)])/(times[min(i+1,len(times)-1)]-times[max(i-1,0)]) for i in range(len(times))]
    reconstructed=[positions[0]]
    for i in range(1,len(times)):reconstructed.append(reconstructed[-1]+.5*(expected_v[i-1]+expected_v[i])*(times[i]-times[i-1]))
    csvs=[]
    for language,module in [('c',c_analysis),('phys',phys_analysis)]:
        prefix=work/f'{name}-{language}';result=command([analysis,module,path,prefix])
        command([probe,str(prefix)+'.psreport',path])
        with Path(str(prefix)+'-motion.csv').open(newline='') as f:rows=list(csv.reader(f))
        assert len(rows)==len(times)+1 and len(rows[0])==5
        expected=list(zip(times,positions,expected_v,reconstructed,[a-b for a,b in zip(reconstructed,positions)]))
        for row,values in zip(rows[1:],expected):assert all(near(float(a),b) for a,b in zip(row,values)),(name,language,row,values)
        csvs.append(rows)
        assert hashlib.sha256(path.read_bytes()).digest()==original
    for a,b in zip(csvs[0][1:],csvs[1][1:]):assert all(near(float(x),float(y)) for x,y in zip(a,b))
for name,times,positions,channel,dim in [
    ('empty',[],[],'position.x',(1,0,0,0,0,0,0)),
    ('singleton',[0],[1],'position.x',(1,0,0,0,0,0,0)),
    ('duplicate',[0,0],[1,2],'position.x',(1,0,0,0,0,0,0)),
    ('missing',[0,1],[1,2],'other',(1,0,0,0,0,0,0)),
    ('dimension',[0,1],[1,2],'position.x',(0,1,0,0,0,0,0))]:
    path=work/f'{name}.psrun';fixture(path,times,positions,name=channel,dimension=dim);original=path.read_bytes()
    for language,module in [('c',c_analysis),('phys',phys_analysis)]:
        prefix=work/f'{name}-{language}';p=subprocess.run([analysis,module,str(path),str(prefix)],capture_output=True,timeout=30)
        assert p.returncode!=0 and not Path(str(prefix)+'.psreport').exists() and path.read_bytes()==original,(name,language)
for language,module in [('c',c_analysis),('phys',phys_analysis)]:
    for count in (0,2):
        prefix=work/f'selection-{count}-{language}'
        args=[analysis,module,'--runs',str(prefix)]+[str(cases['c'][0])]*count
        result=subprocess.run(args,capture_output=True,timeout=30)
        assert result.returncode!=0 and not Path(str(prefix)+'.psreport').exists()
print('Saved run tutorial: eight archived scenarios, sixteen mixed reports, independent reconstruction, recovered prefixes, complete CSV and rejected invalid inputs passed')
