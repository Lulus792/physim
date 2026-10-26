"""Independent UIA scroll reveals hidden F1 field text and preserves edit state."""
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
field='Aktuelles Dokument durchsuchen'
value='Anfang θ '+('x'*80)+' Ziel ᵣ'
probes=[]
with (args.work/'app.stdout').open('wb') as out,(args.work/'app.stderr').open('wb') as err:
    process=subprocess.Popen([str(args.app),'--workspace-state-test',str(args.work/'reader'),'docs-keyboard-edit-scroll-remote'],stdout=out,stderr=err)
    def wait(marker):
        deadline=time.monotonic()+20
        while marker not in (args.work/'app.stdout').read_text(encoding='utf-8'):
            assert process.poll() is None,(process.returncode,(args.work/'app.stderr').read_text(encoding='utf-8'))
            assert time.monotonic()<deadline,'Missing UI state: '+marker
            time.sleep(.02)
    def probe(action,text):
        result=subprocess.run([str(args.probe),str(process.pid),action,field,text],capture_output=True,timeout=15)
        probes.append(dict(action=action,exit_code=result.returncode,stdout=result.stdout.decode('utf-8'),stderr=result.stderr.decode('utf-8')))
        assert result.returncode==0,probes[-1]
    try:
        wait('EDIT SCROLL READY VALUE');probe('edit',value)
        (args.work/'reader'/'edit-focus-applied').write_text('ready',encoding='utf-8')
        wait('EDIT SCROLL READY END');probe('scroll-text-start','Ziel ᵣ')
        (args.work/'reader'/'scroll-end-applied').write_text('ready',encoding='utf-8')
        wait('EDIT SCROLL READY START');probe('scroll-text-end','Anfang θ')
        (args.work/'reader'/'scroll-start-applied').write_text('ready',encoding='utf-8')
        assert process.wait(timeout=15)==0
        assert 'DOCUMENTATION EDIT SCROLL SELF-TEST: PASSED' in (args.work/'app.stdout').read_text(encoding='utf-8')
    finally:
        if process.poll() is None:process.kill();process.wait()
        (args.work/'results.json').write_text(json.dumps(dict(value=value,probes=probes,exit_code=process.returncode),ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
print('Actual F1 field scroll: hidden Unicode ranges revealed, value/selection/caret/focus preserved')
