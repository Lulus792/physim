"""Real C/Physim analyses start, pause and resume the same archived experiments."""
import argparse
import csv
import hashlib
from pathlib import Path
import struct
import subprocess
import zlib

parser=argparse.ArgumentParser(description=__doc__)
for name in ('compiler','analysis-runner','runner','batch','c-analysis','phys-analysis','c-model','phys-model','c-source','phys-source','probe','legacy','work'):
    parser.add_argument('--'+name,type=Path,required=True)
args=parser.parse_args();work=args.work.resolve();work.mkdir(parents=True,exist_ok=True)

def command(argv,code=0):
    p=subprocess.run(list(map(str,argv)),capture_output=True,timeout=90)
    assert p.returncode==code,(argv,p.returncode,p.stdout,p.stderr)
    return p

# Host operations fail before C emission outside analysis modules; constructor
# arguments also remain statically checked.
constructor='Batch("/module","/new","position",1,1,0.01,42,1)'
for name,mode,source,message in (
        ('standalone','--emit-c','let result = '+constructor+'.run()\n',b'Host API requires'),
        ('experiment','--emit-experiment','func create():\n    let result = '+constructor+'.run()\nfunc reset():\n    return\nfunc step(dt: Float64):\n    return\nfunc scene():\n    return\n',b'Host API is unavailable'),
        ('wrongtype','--emit-analysis','func analyze():\n    let result = '+constructor+'.limits(1,true)\n',b'type')):
    source_path=work/(name+'.phys');source_path.write_text(source)
    p=command([args.compiler,mode,source_path],code=1)
    assert message.lower() in p.stderr.lower() and str(source_path).encode() in p.stderr,p.stderr

def archive(path):
    raw=path.read_bytes();assert raw[:16]==b'PSRUN17\n'+struct.pack('<II',1,0x01020304)
    at=16;rows=[];schema=None;footer=False
    while at<len(raw):
        kind,size,crc=struct.unpack_from('<III',raw,at);data=raw[at+12:at+12+size];assert len(data)==size and zlib.crc32(data)==crc
        if kind==2:
            count=struct.unpack_from('<I',data)[0];assert count==5 and size==4+167*count
            assert data[4:52].split(b'\0')[0]==b'position' and struct.unpack_from('<7b',data,164)==(1,0,0,0,0,0,0)
            schema=data
        if kind==3:rows.append(struct.unpack('<6d',data))
        if kind==4:assert struct.unpack('<Q',data)[0]==len(rows);footer=True
        at+=12+size
    assert footer and schema and rows
    return rows

def endpoint_rows(directory):
    with (directory/'endpoints.csv').open(newline='') as f:return list(csv.DictReader(f))

def assert_model(directory,mode):
    rows=endpoint_rows(directory);assert len(rows)==(3 if mode=='sweep' else 6)
    for i,row in enumerate(rows):
        samples=archive(directory/f'run-{i+1:04d}.psrun')
        velocity=(.2,1.6,3)[i] if mode=='sweep' else 1.4
        start=samples[0][1];assert .4<=start<1.4
        previous=-1
        for t,position,endpoint,interval,actual_velocity,offset in samples:
            assert t>previous and abs(position-(start+velocity*t))<2e-12 and abs(endpoint-t)<2e-16,(mode,i,t,position,endpoint,velocity)
            assert actual_velocity==velocity and offset==.4
            if t:assert interval>0 and abs(interval-(t-previous))<2e-16
            previous=t
        assert samples[-1][0]==.7 or (mode=='fixed' and abs(samples[-1][0]-.7)<1e-15)
        assert float(row['value'])==samples[-1][1]
        if mode=='fixed':assert len(samples)==8
        if mode in ('adaptive','sweep'):assert len(samples)<101
    return rows

# The old ABI-3 diagnostic tail remains selected even without the new host tail.
legacy_prefix=work/'old-tail'
command([args.analysis_runner,args.legacy,'--runs',legacy_prefix])
assert legacy_prefix.read_text()=='old diagnostic tail selected\n'

outputs={}
for model_name,model,source in [('c',args.c_model,args.c_source),('phys',args.phys_model,args.phys_source)]:
    for mode in ('fixed','target','adaptive','sweep','error','partial','resume','existing'):
        marker=work/('mode-'+mode);marker.touch(exist_ok=True)
        for language,analysis in [('c',args.c_analysis),('phys',args.phys_analysis)]:
            prefix=work/(model_name+'-'+mode+'-'+language)
            if mode=='resume':
                prior=outputs[(model_name,'partial',language)]
                input_path=prior/'series.txt';held={p.name:hashlib.sha256(p.read_bytes()).digest() for p in prior.glob('run-*.psrun')}
                before={p.name:hashlib.sha256(p.read_bytes()).digest() for p in prior.iterdir() if p.is_file()}
            elif mode=='existing':
                prior=outputs[(model_name,'target',language)]
                input_path=prior/('experiment.dll' if model.suffix=='.dll' else 'experiment.so')
                before={p.name:hashlib.sha256(p.read_bytes()).digest() for p in prior.iterdir() if p.is_file()}
            else:input_path=model
            command([args.analysis_runner,analysis,'--runs',prefix,input_path,marker,source])
            directory=Path(str(prefix)+('-paused' if mode=='partial' else '-series'));outputs[(model_name,mode,language)]=directory
            if mode in ('error','existing'):
                assert not (directory/'summary.psreport').exists()
                counts=[0,0 if mode=='existing' else 4,0,0,0,2]
            elif mode=='partial':
                assert not (directory/'summary.psreport').exists()
                assert len(endpoint_rows(directory))==2
                counts=[2,2,0,2,1,0]
            else:
                rows=assert_model(directory,mode)
                assert (directory/'summary.psreport').is_file()
                n=len(rows);counts=[n,0 if mode=='resume' else n,2 if mode=='resume' else 0,n,0,0]
                if mode=='resume':counts[1]=4
            command([args.probe,Path(str(prefix)+'.psreport'),*counts])
            if mode=='existing':
                assert all(hashlib.sha256((prior/name).read_bytes()).digest()==digest for name,digest in before.items())
            else:
                archived=directory/('experiment.phys' if source.suffix=='.phys' else 'experiment.c')
                assert archived.read_bytes()==source.read_bytes()
            if mode=='resume':
                assert all(hashlib.sha256((prior/name).read_bytes()).digest()==digest for name,digest in before.items())
                assert all(hashlib.sha256((directory/name).read_bytes()).digest()==digest for name,digest in held.items())
                assert (directory/'endpoints.csv').read_bytes()==(outputs[(model_name,'target',language)]/'endpoints.csv').read_bytes()
                for name in held:assert not (directory/('work-'+name[4:8])).exists()
        # Both analysis languages must produce identical numerical archives/CSV.
        left=outputs[(model_name,mode,'c')];right=outputs[(model_name,mode,'phys')]
        if mode not in ('error','existing'):
            assert (left/'endpoints.csv').read_bytes()==(right/'endpoints.csv').read_bytes()
            for file in left.glob('run-*.psrun'):
                if file.name in {p.name for p in right.glob('run-*.psrun')}:assert archive(file)==archive(right/file.name)
    for mode in ('fixed','target','adaptive','sweep'):
        destination=work/(model_name+'-'+mode+'-cli')
        command_line=[args.batch,args.runner,model,destination,'position',3 if mode=='sweep' else 6,
                      7 if mode=='fixed' else 100,'.1','42',source,'--workers','4','--timeout','15','--param','offset=.4']
        command_line+=['--sweep','velocity=.2:3'] if mode=='sweep' else ['--param','velocity=1.4']
        if mode!='fixed':command_line+=['--until','.7']
        if mode in ('adaptive','sweep'):command_line+=['--adaptive','--min-dt','.02','--max-dt','.3']
        command(command_line);assert_model(destination,mode)
        reference=outputs[(model_name,mode,'phys')]
        assert (destination/'endpoints.csv').read_bytes()==(reference/'endpoints.csv').read_bytes()
        for file in destination.glob('run-*.psrun'):assert archive(file)==archive(reference/file.name)

# C and Physim experiment sources agree on every time and every measured value.
for mode in ('fixed','target','adaptive','sweep','partial','resume'):
    for language in ('c','phys'):
        a=outputs[('c',mode,language)];b=outputs[('phys',mode,language)]
        assert (a/'endpoints.csv').read_bytes()==(b/'endpoints.csv').read_bytes()
        for p in a.glob('run-*.psrun'):
            if (b/p.name).exists():assert archive(p)==archive(b/p.name)
print('Batch binding: 32 mixed C/Physim starts plus eight CLI references, fixed/target/adaptive/sweep/error, checkpoint pause/resume, exclusive output, byte-preserved archives and legacy ABI-3 diagnostics passed')
