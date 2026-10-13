"""Independent UIA values and focus drive both real F1 search fields."""
import argparse
import json
from pathlib import Path
import subprocess
import time
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--app',type=Path,required=True)
parser.add_argument('--probe',type=Path,required=True)
parser.add_argument('--work',type=Path,required=True)
args=parser.parse_args();args.work.mkdir(parents=True,exist_ok=True)
out=(args.work/'app.stdout').open('wb');err=(args.work/'app.stderr').open('wb')
process=subprocess.Popen([str(args.app),'--workspace-state-test',str(args.work/'reader'),'docs-keyboard-edit-remote'],stdout=out,stderr=err)
probes=[]
def wait(marker):
    deadline=time.monotonic()+20
    while marker not in (args.work/'app.stdout').read_text(encoding='utf-8'):
        assert process.poll() is None,(process.returncode,(args.work/'app.stderr').read_text(encoding='utf-8'))
        assert time.monotonic()<deadline,'Missing UI state: '+marker
        time.sleep(.02)
try:
    for phase,name in [('SEARCH','Aktuelles Dokument durchsuchen'),('GLOBAL','Alle Inhalte durchsuchen')]:
        wait('EDIT READY '+phase)
        value='ps_rng_seed θ🌍' if phase=='SEARCH' else 'ps_rng_seed'
        result=subprocess.run([str(args.probe),str(process.pid),'edit',name,value],capture_output=True,timeout=15)
        probes.append(dict(field=name,exit_code=result.returncode,stdout=result.stdout.decode('utf-8'),stderr=result.stderr.decode('utf-8')))
        assert result.returncode==0,probes[-1]
        wait('EDIT APPLIED '+phase)
        if phase=='SEARCH':
            result=subprocess.run([str(args.probe),str(process.pid),'select-text',name,'θ🌍'],capture_output=True,timeout=15)
            probes.append(dict(action='select-text',field=name,exit_code=result.returncode,stdout=result.stdout.decode('utf-8'),stderr=result.stderr.decode('utf-8')))
            assert result.returncode==0,probes[-1]
            (args.work/'reader'/'selection-applied').write_text('ready',encoding='utf-8')
    assert process.wait(timeout=15)==0
    assert 'DOCUMENTATION EDIT SELF-TEST: PASSED' in (args.work/'app.stdout').read_text(encoding='utf-8')
finally:
    if process.poll() is None:process.kill();process.wait()
    out.close();err.close()
    (args.work/'results.json').write_text(json.dumps(dict(probes=probes,exit_code=process.returncode),ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
print('Actual F1 native search: named edit values, focus, text input, search results and Tab exit passed')
