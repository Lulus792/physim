"""Native B window ownership and real gestures; no physical focus claim."""
import argparse,hashlib,json,os,subprocess
from pathlib import Path
from check_graphic_raster import png
p=argparse.ArgumentParser(description=__doc__)
for name in ('app','work'):p.add_argument('--'+name,type=Path,required=True)
a=p.parse_args();a.app=a.app.resolve();a.work.mkdir(parents=True,exist_ok=True);binary=hashlib.sha256(a.app.read_bytes()).hexdigest();cases=[]
for mode in ('dark','light','contrast','large','small','large-small'):
 work=a.work/mode;work.mkdir();command=[str(a.app),'--workspace-state-test',str(work),'project-window-'+mode]
 result=subprocess.run(command,capture_output=True,timeout=90,env=dict(os.environ,PHYSIM_TEST_TRACE='1'))
 (work/'stdout.bin').write_bytes(result.stdout);(work/'stderr.bin').write_bytes(result.stderr)
 rows=[json.loads(line) for line in result.stdout.decode('utf8',errors='replace').splitlines() if line.startswith('{"kind":"independent_project_window"')]
 changes=[json.loads(line) for line in result.stdout.decode('utf8',errors='replace').splitlines() if line.startswith('{"kind":"project_window_domain_changes"')]
 errors=[json.loads(line) for line in result.stdout.decode('utf8',errors='replace').splitlines() if line.startswith('{"kind":"project_window_error"')]
 cases.append(dict(mode=mode,command=command,exit_code=result.returncode,measurements=rows,domain_changes=changes,creation_errors=errors))
 (a.work/'verification.json').write_text(json.dumps(dict(binary_sha256=binary,cases=cases),indent=2)+'\n')
 assert result.returncode==0 and b'INDEPENDENT PROJECT WINDOW SELF-TEST: PASSED' in result.stdout,result.stderr.decode('utf8',errors='replace')
 assert len(rows)==1 and rows[0]['native_handles_distinct'] and rows[0]['main_id']!=rows[0]['project_id']
 assert len(changes)==1 and changes[0]['loaded_project_and_bare_workspace_preserve_form'] and changes[0]['template']==2 and changes[0]['step']==1
 assert changes[0]['live_font_reflow_preserves_focus']
 assert changes[0]['experiment_language']==changes[0]['analysis_language']==1
 assert len(errors)==1 and errors[0]['passed']
 panel,ink=errors[0]['panel'],errors[0]['text']
 assert ink[0]>=panel[0] and ink[1]>=panel[1] and ink[0]+ink[2]<=panel[0]+panel[2]+.1 and ink[1]+ink[3]<=panel[1]+panel[3]+.1
 w,h,_,_=png(work/'project-window.png');assert (w,h)==(320 if 'small' in mode else 680,740)
 if 'small' in mode:assert png(work/'project-window-start.png')[:2]==(320,740)
 if mode=='large-small':assert png(work/'project-window-error.png')[:2]==(320,740)
 assert hashlib.sha256(a.app.read_bytes()).hexdigest()==binary
print('Native B window: distinct OS/UI/GL owners, main navigation/project/workspace changes, retained name/template/languages/reinvoke, own resize/Escape/OS close, child retirement and shutdown passed')
