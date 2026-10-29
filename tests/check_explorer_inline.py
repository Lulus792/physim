"""Free Explorer A occupies the workspace, retaining state across a real picker."""
import argparse,hashlib,json,os,subprocess
from pathlib import Path
parser=argparse.ArgumentParser(description=__doc__)
for name in ('app','work'):parser.add_argument('--'+name,type=Path,required=True)
args=parser.parse_args();args.app=args.app.resolve();args.work=args.work.resolve();args.work.mkdir(parents=True,exist_ok=True)
binary=hashlib.sha256(args.app.read_bytes()).hexdigest();cases=[]
for mode in ('dark','light','contrast','large','narrow','large-narrow','hidden'):
    work=args.work/mode;work.mkdir()
    command=[str(args.app),'--workspace-state-test',str(work),'explorer-a-inline-'+mode]
    result=subprocess.run(command,capture_output=True,timeout=90,env=dict(os.environ,PHYSIM_TEST_TRACE='1'))
    (work/'stdout.bin').write_bytes(result.stdout);(work/'stderr.bin').write_bytes(result.stderr)
    output=result.stdout.decode('utf8',errors='replace');rows=[json.loads(line) for line in output.splitlines() if line.startswith('{"kind":"explorer_inline"')]
    cases.append(dict(mode=mode,command=command,exit_code=result.returncode,measurements=rows))
    (args.work/'verification.json').write_text(json.dumps(dict(binary_sha256=binary,cases=cases),indent=2)+'\n',encoding='utf8')
    assert result.returncode==0 and 'EXPLORER INLINE SELF-TEST: PASSED' in output,'Inline native workflow failed: '+mode
    assert len(rows)==1;row=rows[0];width=1080 if 'narrow' in mode else 1440
    assert row['frame'][:3]==[0,118,width] and row['frame'][3]==(740 if 'narrow' in mode else 1000)-146
    assert abs(row['body']-440*row['scale'])<.05
    if width<1150*row['scale']:
        assert abs(row['preview'][0]-row['list'][0])<.05 and abs(row['preview'][1]-row['list'][1]-row['body'])<.05
    else:assert abs(row['preview'][0]-row['list'][0]-row['list'][2])<.05 and abs(row['preview'][2]-270*row['scale'])<.05
    assert hashlib.sha256(args.app.read_bytes()).hexdigest()==binary
print('Inline Explorer A: six sizes/themes plus hidden-pane preference; global navigation, retained dirty editor/undo, independent nested picker, log bounds and actual source reopen passed')
