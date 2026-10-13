"""Independent Windows UIA TextPattern reads an entire real F1 source page."""
import argparse
import json
from pathlib import Path
import subprocess
import time

parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--app',type=Path,required=True)
parser.add_argument('--probe',type=Path,required=True)
parser.add_argument('--root',type=Path,required=True)
parser.add_argument('--work',type=Path,required=True)
args=parser.parse_args()
args.work.mkdir(parents=True,exist_ok=True)
out=(args.work/'app.stdout').open('wb')
err=(args.work/'app.stderr').open('wb')
process=subprocess.Popen([str(args.app),'--workspace-state-test',str(args.work/'reader'),'docs-keyboard-reading-remote'],stdout=out,stderr=err)
try:
    deadline=time.monotonic()+20
    while 'READING READY' not in (args.work/'app.stdout').read_text(encoding='utf-8'):
        assert process.poll() is None, (process.returncode,(args.work/'app.stderr').read_text(encoding='utf-8'))
        assert time.monotonic()<deadline,'Reader did not become ready'
        time.sleep(.02)
    transcript=args.work/'uia-document.txt'
    result=subprocess.run([str(args.probe),str(process.pid),'read','Physim | Dokumentation',str(transcript)],capture_output=True,timeout=20)
    (args.work/'probe.stdout').write_bytes(result.stdout)
    (args.work/'probe.stderr').write_bytes(result.stderr)
    assert result.returncode==0,(result.returncode,result.stderr)
    expected=(args.root/'docs/project-audit.json').read_bytes()+b'\n\n'
    actual=transcript.read_bytes()
    assert len(expected)>256*1024 and actual==expected,'Native reader lost or changed document bytes'
    assert process.wait(timeout=15)==0
    stdout=(args.work/'app.stdout').read_text(encoding='utf-8')
    assert 'READING END' in stdout and 'DOCUMENTATION READING SELF-TEST: PASSED' in stdout
    assert (args.work/'reader/docs-reading-end.bmp').is_file()
    (args.work/'results.json').write_text(json.dumps(dict(bytes=len(actual),independent_client=True,complete_source=True,actual_scroll=True),indent=2)+'\n',encoding='utf-8')
finally:
    if process.poll() is None:
        process.kill()
        process.wait()
    out.close()
    err.close()
print('Actual F1 UIA reading: full source, marshalled range clone/compare, visible geometry and native scrolling passed')
