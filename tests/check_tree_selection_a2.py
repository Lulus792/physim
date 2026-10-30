"""Actual A2 Workspace and scene-tree pixels plus independent tree actions."""
import argparse
import hashlib
import json
import subprocess
import sys
from pathlib import Path

p=argparse.ArgumentParser(description=__doc__)
for name in ('app','root','work'):p.add_argument('--'+name,type=Path,required=True)
a=p.parse_args();a.app=a.app.resolve();a.work.mkdir(parents=True,exist_ok=True)
binary=hashlib.sha256(a.app.read_bytes()).hexdigest();cases=[]
for context in ('workspace','scene'):
    work=a.work/context
    command=[sys.executable,str(a.root/'tests/check_list_selection_a2.py'),
             '--app',str(a.app),'--root',str(a.root),'--work',str(work),'--context',context]
    result=subprocess.run(command,capture_output=True,timeout=600)
    (a.work/(context+'-stdout.bin')).write_bytes(result.stdout)
    (a.work/(context+'-stderr.bin')).write_bytes(result.stderr)
    assert result.returncode==0,(context,result.stdout.decode('utf8',errors='replace'),result.stderr.decode('utf8',errors='replace'))
    report=json.loads((work/'verification.json').read_text(encoding='utf8'))
    assert report['binary_sha256']==binary and len(report['cases'])==5,context
    cases.extend(report['cases'])
assert hashlib.sha256(a.app.read_bytes()).hexdigest()==binary
(a.work/'verification.json').write_text(json.dumps(dict(binary_sha256=binary,cases=cases),indent=2)+'\n',encoding='utf8')
print('Tree A2:',len(cases),'native states passed')
