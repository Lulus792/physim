"""Retain real Windows F1 UIA objects through hide/reopen and minimize/restore."""
import argparse
import json
from pathlib import Path
import subprocess
import time

parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--app',type=Path,required=True)
parser.add_argument('--probe',type=Path,required=True)
parser.add_argument('--work',type=Path,required=True)
args=parser.parse_args()
args.work.mkdir(parents=True,exist_ok=True)
reader=args.work/'reader'
out=(args.work/'app.stdout').open('wb')
err=(args.work/'app.stderr').open('wb')
process=subprocess.Popen([str(args.app),'--workspace-state-test',str(reader),'docs-keyboard-lifecycle-remote'],stdout=out,stderr=err)
try:
    deadline=time.monotonic()+20
    while 'LIFECYCLE READY' not in (args.work/'app.stdout').read_text(encoding='utf-8'):
        assert process.poll() is None,(process.returncode,(args.work/'app.stderr').read_text(encoding='utf-8'))
        assert time.monotonic()<deadline,'Lifecycle app did not become ready'
        time.sleep(.02)
    result=subprocess.run([str(args.probe),str(process.pid),'lifecycle','Physim | Dokumentation',str(reader)],capture_output=True,timeout=50)
    (args.work/'probe.stdout').write_bytes(result.stdout)
    (args.work/'probe.stderr').write_bytes(result.stderr)
    assert result.returncode==0,(result.returncode,result.stdout,result.stderr)
    assert b'DOCUMENT LIFECYCLE:' in result.stdout
    assert process.wait(timeout=15)==0
    assert 'DOCUMENTATION LIFECYCLE SELF-TEST: PASSED' in (args.work/'app.stdout').read_text(encoding='utf-8')
    for name in ('docs-lifecycle-reopened.bmp','docs-lifecycle-restored.bmp'):
        assert (reader/name).is_file()
    (args.work/'results.json').write_text(json.dumps(dict(independent_client=True,retained_objects=True,hide_reopen=True,minimize_restore=True,hidden_geometry_empty=True,hidden_focus_absent=True,hidden_actions_rejected=True,content_preserved=True,editor_unchanged=True),indent=2)+'\n',encoding='utf-8')
finally:
    if process.poll() is None:
        process.kill()
        process.wait()
    out.close()
    err.close()
print('Actual F1 UIA lifecycle: hidden/minimized text, focus, actions, events and restored content passed')
