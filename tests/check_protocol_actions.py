"""Native C detail actions: approved intrinsic widths, wrap, insets and real copy."""
import argparse
import hashlib
import json
import math
import os
from pathlib import Path
import subprocess
from check_protocol_original import final_glyph_pixels

MODES=('dark','light','contrast','large','narrow','nosource')
def require(value,message):
    if not value:raise RuntimeError(message)
def verify(work,mode,code,stdout):
    require(code==0,'Native action process failed')
    require('PROTOCOL ACTIONS '+mode+' SELF-TEST: PASSED' in stdout,'Completion marker missing')
    states=[json.loads(line) for line in stdout.splitlines() if line.startswith('{"kind":"protocol_actions"')]
    require([s['stage'] for s in states]==list(range(3,10)) and all(s['passed'] for s in states),'Incomplete or failed action sequence')
    require(len({s['source_crc'] for s in states})==1,'Editor changed during actions')
    state=next(s for s in states if s['stage']==6);scale=state['scale'];copy=state['copy'];source=state['source'];pane=state['pane']
    close=lambda a,b:math.isclose(a,b,abs_tol=.05)
    require(close(copy[2],state['expected_widths'][1]) and close(copy[3],37.14*scale),'Copy is not intrinsically sized')
    require(copy[0]>=pane[0]+20*scale-.05 and copy[0]+copy[2]<=pane[0]+pane[2]-20*scale+.05,'Copy leaves detail insets')
    ink=None
    if mode.endswith('nosource'):
        require(source[2]==0 and close(copy[0],pane[0]+20*scale),'Unavailable source still has a control')
    else:
        require(close(source[0],pane[0]+20*scale) and close(source[2],state['expected_widths'][0]) and close(source[3],37.14*scale),'Source action has wrong width/height/inset')
        if close(copy[1],source[1]):require(close(copy[0]-source[0]-source[2],10*scale),'Horizontal action gap differs')
        else:require(close(copy[0],source[0]) and close(copy[1]-source[1]-source[3],10*scale),'Wrapped action gap differs')
        arrow=state['arrow'];require(arrow[0]>=source[0] and arrow[0]+arrow[2]<=source[0]+source[2],'Arrow box leaves source action')
        ink=final_glyph_pixels(work/'protocol-actions.png',dict(state,end_box=arrow,viewport=source))['ink_pixels']
    return {'states':states,'arrow_ink_pixels':ink}
def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--app',type=Path,required=True);parser.add_argument('--root',type=Path,required=True);parser.add_argument('--work',type=Path,required=True)
    args=parser.parse_args();args.app=args.app.resolve();args.root=args.root.resolve();args.work=args.work.resolve();args.work.mkdir(parents=True,exist_ok=True)
    sha=hashlib.sha256(args.app.read_bytes()).hexdigest();results=[]
    for suffix in MODES:
        mode='protocol-ui-actions-'+suffix;work=args.work/mode;work.mkdir(exist_ok=False)
        command=[str(args.app),'--workspace-state-test',str(work),mode]
        result=subprocess.run(command,env=dict(os.environ,SDL_FORCE_RAISEWINDOW='1'),capture_output=True,timeout=80)
        (work/'stdout.bin').write_bytes(result.stdout);(work/'stderr.bin').write_bytes(result.stderr)
        record={'mode':mode,'command':command,'exit_code':result.returncode,'passed':False}
        try:record.update(verify(work,mode,result.returncode,result.stdout.decode('utf-8')),passed=True)
        except (RuntimeError,OSError,ValueError) as error:record['error']=str(error)
        results.append(record);(args.work/'verification.json').write_text(json.dumps({'binary_sha256':sha,'cases':results},indent=2)+'\n',encoding='utf-8')
        require(record['passed'],'Native action geometry/copy failed: '+mode+' '+record.get('error',''))
        require((work/'project/main.c').read_bytes()==(args.root/'examples/pendulum/main.c').read_bytes(),'Source file changed')
        require((work/'protocol-actions.png').is_file(),'Visible action-state capture missing')
        require(hashlib.sha256(args.app.read_bytes()).hexdigest()==sha,'App changed during run')
    print('Native protocol actions: six layout/theme states, actual pointer copy and unchanged source passed')
if __name__=='__main__':main()
