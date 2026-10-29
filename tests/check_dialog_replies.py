"""App-level request generations across actual project/picker close and reopen."""
import argparse,hashlib,json,os,subprocess
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__)
for name in ('app','work'):p.add_argument('--'+name,type=Path,required=True)
a=p.parse_args();a.app=a.app.resolve();a.work.mkdir(parents=True,exist_ok=True)
fixture=a.work/'reply owner ä';command=[str(a.app),'--workspace-state-test',str(fixture),'explorer-a-replies']
result=subprocess.run(command,capture_output=True,timeout=90,env=dict(os.environ,PHYSIM_TEST_TRACE='1'))
(a.work/'stdout.bin').write_bytes(result.stdout);(a.work/'stderr.bin').write_bytes(result.stderr)
rows=[json.loads(line) for line in result.stdout.decode('utf8',errors='replace').splitlines() if line.startswith('{"kind":"dialog_reply_ownership"')]
changed=[json.loads(line) for line in result.stdout.decode('utf8',errors='replace').splitlines() if line.startswith('{"kind":"dialog_project_change"')]
(a.work/'verification.json').write_text(json.dumps(dict(command=command,exit_code=result.returncode,binary_sha256=hashlib.sha256(a.app.read_bytes()).hexdigest(),measurements=rows,project_changes=changed),indent=2)+'\n')
assert result.returncode==0 and b'DIALOG REPLY OWNERSHIP SELF-TEST: PASSED' in result.stdout,result.stderr.decode('utf8',errors='replace')
assert len(rows)==1 and rows[0]['stale_reply_ignored'] and rows[0]['current_version']>rows[0]['retired_version']
assert len(changed)==1 and changed[0]['retired_routes']==4 and changed[0]['late_callbacks_ignored']
print('Dialog replies: real B/A close/reopen, stale ticket rejection, current selection and actual workspace change with four late callbacks passed')
