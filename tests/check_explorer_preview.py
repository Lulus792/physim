"""Real readonly Explorer preview: natural height, soft wrap, copy and resize."""
import argparse,hashlib,json,os,subprocess
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__)
for name in ('app','work'):p.add_argument('--'+name,type=Path,required=True)
a=p.parse_args();a.app=a.app.resolve();a.work.mkdir(parents=True,exist_ok=True)
binary=hashlib.sha256(a.app.read_bytes()).hexdigest();cases=[]
for size in ('normal','large'):
 work=a.work/size;work.mkdir();command=[str(a.app),'--workspace-state-test',str(work),'explorer-a-preview-flow'+('-large' if size=='large' else '')]
 result=subprocess.run(command,capture_output=True,timeout=90,env=dict(os.environ,PHYSIM_TEST_TRACE='1'))
 (work/'stdout.bin').write_bytes(result.stdout);(work/'stderr.bin').write_bytes(result.stderr)
 rows=[json.loads(line) for line in result.stdout.decode('utf8',errors='replace').splitlines() if line.startswith('{"kind":"explorer_preview_flow"')]
 cases.append(dict(size=size,command=command,exit_code=result.returncode,measurements=rows))
 (a.work/'verification.json').write_text(json.dumps(dict(binary_sha256=binary,cases=cases),indent=2)+'\n')
 assert result.returncode==0 and b'EXPLORER PREVIEW FLOW SELF-TEST: PASSED' in result.stdout,(size,result.stderr.decode('utf8',errors='replace'))
 assert {row['long'] for row in rows}=={False,True}
 for row in rows:
  assert row['horizontal_scroll']==0 and abs(row['pitch']-10.5*1.85*row['scale'])<.05
  assert abs(row['height']-min(240*row['scale'],row['rows']*row['pitch']))<.05
 assert hashlib.sha256(a.app.read_bytes()).hexdigest()==binary
print('Explorer preview: short natural height, capped long wrap, exact Unicode copy, resize selection and readonly input passed')
