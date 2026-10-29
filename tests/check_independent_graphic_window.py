"""Actual export OS window while the main workspace remains interactive."""
import argparse,hashlib,json,os,subprocess
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__)
for name in ('app','work'):p.add_argument('--'+name,type=Path,required=True)
a=p.parse_args();a.app=a.app.resolve();a.work.mkdir(parents=True,exist_ok=True)
project=a.work/'export window ä';assert not project.exists()
environment=dict(os.environ,PHYSIM_TEST_GRAPHIC_WINDOW='1',PHYSIM_TEST_TRACE='1')
environment.pop('PHYSIM_TEST_GRAPHIC_DIALOG',None)
command=[str(a.app),'--plot-test',str(project)]
result=subprocess.run(command,capture_output=True,timeout=150,env=environment,cwd=a.work)
(a.work/'stdout.bin').write_bytes(result.stdout);(a.work/'stderr.bin').write_bytes(result.stderr)
rows=[json.loads(line) for line in result.stdout.decode('utf8',errors='replace').splitlines() if line.startswith('{"kind":"independent_graphic_window"')]
(a.work/'verification.json').write_text(json.dumps(dict(command=command,exit_code=result.returncode,binary_sha256=hashlib.sha256(a.app.read_bytes()).hexdigest(),measurements=rows),indent=2)+'\n')
assert result.returncode==0 and b'INDEPENDENT GRAPHIC WINDOW SELF-TEST: PASSED' in result.stdout,result.stderr.decode('utf8',errors='replace')
assert len(rows)==1 and rows[0]['native_handles_distinct'] and rows[0]['main_window_id']!=rows[0]['export_window_id'] and rows[0]['output_bytes']>0
assert (project/'independent-export.png').is_file()
print('Independent export window: distinct OS handles/UI, main navigation, frozen output, independent resize, Escape, reopen and OS close passed')
