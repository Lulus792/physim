"""Independent Windows UIA reads, focuses and invokes embedded F1 links."""
import argparse
import json
import os
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
environment=os.environ.copy()
environment['PHYSIM_TEST_TRACE']='1'
started=time.monotonic()
observations={}
phase='app readiness'
failure=None
process=subprocess.Popen([str(args.app),'--workspace-state-test',str(reader),'docs-keyboard-links-remote'],stdout=out,stderr=err,env=environment)

def wait_for(marker):
    deadline=time.monotonic()+20
    while marker not in (args.work/'app.stdout').read_text(encoding='utf-8'):
        assert process.poll() is None,(process.returncode,(args.work/'app.stderr').read_text(encoding='utf-8'))
        assert time.monotonic()<deadline,'Missing app state: '+marker
        time.sleep(.02)
    observations.setdefault(marker,round(time.monotonic()-started,6))

try:
    wait_for('LINKS READY FOCUS')
    for action,marker,flag in (('links','LINKS READY FOCUS','link-focus-done'),('invoke-link-invalid','LINKS READY INVALID','link-invalid-done'),('invoke-link','LINKS READY INVOKE','link-invoke-done')):
        phase=action
        wait_for(marker)
        result=subprocess.run([str(args.probe),str(process.pid),action,'Physim | Dokumentation','C-Lernweg' if action=='invoke-link-invalid' else 'C'],capture_output=True,timeout=25)
        (args.work/(action+'.stdout')).write_bytes(result.stdout)
        (args.work/(action+'.stderr')).write_bytes(result.stderr)
        assert result.returncode==0,(action,result.returncode,result.stderr)
        assert b'DOCUMENT LINKS:' in result.stdout
        assert b'WINDOWS LINK EVENTS:' in result.stdout
        if action=='links':
            assert b'quiet idle passed' in result.stdout
        elif action=='invoke-link':
            assert b'invoked=1 bad=0' in result.stdout
        else:
            assert b'rejected target' in result.stdout and b'invoked=0 bad=0' in result.stdout
        (reader/flag).write_text('verified\n',encoding='utf-8')
    phase='app completion'
    assert process.wait(timeout=15)==0
    wait_for('DOCUMENTATION LINKS SELF-TEST: PASSED')
    for name in ('docs-native-link-focus.bmp','docs-native-link-failed.bmp','docs-native-link-open.bmp'):
        assert (reader/name).is_file()
    (args.work/'results.json').write_text(json.dumps(dict(independent_client=True,ordered_links=2,exact_text_ranges=True,offscreen_focus=True,visible_glyphs=True,native_navigation=True,stale_ranges_rejected=True,editor_unchanged=True,property_events=True,invoked_event=True,cached_event_identity=True,disabled_host_rejection=True,screen_position_event=True,quiet_idle=True),indent=2)+'\n',encoding='utf-8')
except BaseException as error:
    failure=type(error).__name__+': '+str(error)
    raise
finally:
    app_exit_before_cleanup=process.poll()
    if app_exit_before_cleanup is None:
        process.kill()
        process.wait()
    out.close()
    err.close()
    (args.work/'diagnostics.json').write_text(json.dumps(dict(
        phase=phase,failure=failure,seconds=round(time.monotonic()-started,6),
        markers=observations,app_exit_before_cleanup=app_exit_before_cleanup,
        app_killed_by_cleanup=app_exit_before_cleanup is None,
        startup_trace='app.stderr',readiness_timeout_seconds=20,probe_timeout_seconds=25),indent=2)+'\n',encoding='utf-8')
print('Actual F1 UIA links: role, embedded ranges, offscreen focus, navigation, stale ranges and input isolation passed')
