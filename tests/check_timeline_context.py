"""Context disposal must release an actual timeline drag before old Escape input."""
import argparse,json,pathlib,sys
from check_display_workflow import Workflow

def main():
    for stream in (sys.stdout,sys.stderr):
        if hasattr(stream,'reconfigure'):stream.reconfigure(encoding='utf-8',errors='replace')
    p=argparse.ArgumentParser();p.add_argument('--app',type=pathlib.Path,required=True);p.add_argument('--root',type=pathlib.Path,required=True);p.add_argument('--work',type=pathlib.Path,required=True)
    p.add_argument('--without-native-bridge',action='store_true');a=p.parse_args()
    a.work.mkdir(parents=True,exist_ok=True);flow=Workflow(a.app,a.work,a.root,False);flow.env['SDL_FORCE_RAISEWINDOW']='1'
    if a.without_native_bridge:
        assert sys.platform=='linux','NO_AT_BRIDGE is an AT-SPI-specific counterprobe'
        flow.env['NO_AT_BRIDGE']='1'
    flow.run('--workspace-state-test',a.work/'context','timeline-context',timeout=45,marker='TIMELINE CONTEXT SELF-TEST: PASSED')
    out=flow.steps[-1]['stdout'];assert 'TIMELINE CONTEXT AFTER OPEN: dragging=0 browsing=0 count=0' in out
    if a.without_native_bridge:assert 'TIMELINE CONTEXT BEFORE OPEN: native=0 dragging=1 browsing=1 count=4' in out
    assert 'TIMELINE CONTEXT FINAL: dragging=0 browsing=0 count=0 time=0' in out
    assert (a.work/'context/project/main.c').read_bytes()==(a.root/'examples/pendulum/main.c').read_bytes()
    assert not list((a.work/'context/project/runs').glob('*.psrun'))
    (a.work/'validation.json').write_text(json.dumps(dict(scope=__doc__,synthetic_states=True,actual_pointer=True,actual_project_reopen=True,stale_escape_rejected=True,native_bridge_deliberately_disabled=a.without_native_bridge),indent=2)+'\n',encoding='utf-8');print('TIMELINE CONTEXT: PASSED')
if __name__=='__main__':main()
