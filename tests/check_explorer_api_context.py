"""Native Explorer A C-context and actual F1 link, across themes and text sizes."""
import argparse,hashlib,json,os,subprocess
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__)
for name in ('app','work'):p.add_argument('--'+name,type=Path,required=True)
a=p.parse_args();a.app=a.app.resolve();a.work.mkdir(parents=True,exist_ok=True)
binary=hashlib.sha256(a.app.read_bytes()).hexdigest();cases=[]
for mode in ('dark','light','contrast','large'):
 work=a.work/mode;work.mkdir();command=[str(a.app),'--workspace-state-test',str(work),'explorer-a-api-context-'+mode]
 result=subprocess.run(command,capture_output=True,timeout=90,env=dict(os.environ,PHYSIM_TEST_TRACE='1'))
 (work/'stdout.bin').write_bytes(result.stdout);(work/'stderr.bin').write_bytes(result.stderr)
 rows=[json.loads(line) for line in result.stdout.decode('utf8',errors='replace').splitlines() if line.startswith('{"kind":"explorer_api_context"')]
 cases.append(dict(mode=mode,command=command,exit_code=result.returncode,measurements=rows))
 (a.work/'verification.json').write_text(json.dumps(dict(binary_sha256=binary,cases=cases),indent=2)+'\n')
 assert result.returncode==0 and b'EXPLORER API CONTEXT SELF-TEST: PASSED' in result.stdout,(mode,result.stderr.decode('utf8',errors='replace'))
 assert len(rows)==1 and rows[0]['symbol']=='ps_channel_add' and rows[0]['parameter_rows']==3
 assert abs(rows[0]['pitch']-10.08*1.85*rows[0]['scale'])<.05
 assert rows[0]['signature_selection'][1]>rows[0]['signature_selection'][0]
 assert hashlib.sha256(a.app.read_bytes()).hexdigest()==binary
print('Explorer C context: real worker/manual, expand/collapse, wrapped readonly signature/copy/resize, actual F1 section and independent selection passed')
