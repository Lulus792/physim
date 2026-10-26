"""Replay one actual native action run; malformed geometry and blank arrow fail."""
import argparse
import copy
import hashlib
import json
from pathlib import Path
from check_protocol_actions import verify

def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--work',type=Path,required=True);args=parser.parse_args()
    mode='protocol-ui-actions-dark';raw=(args.work/'stdout.bin').read_bytes();control=verify(args.work,mode,0,raw.decode('utf-8'))
    states=control['states'];results=[]
    for name in ('missing_copy_step','stretched_copy','changed_source_crc','blank_arrow_region'):
        changed=copy.deepcopy(states);state=next(s for s in changed if s['stage']==6)
        if name=='missing_copy_step':changed=[s for s in changed if s['stage']!=9]
        elif name=='stretched_copy':state['copy'][2]+=20
        elif name=='changed_source_crc':changed[-1]['source_crc']^=1
        else:state['arrow'][0]=state['source'][0]+1;state['arrow'][2]=6
        text='\n'.join(json.dumps(s,separators=(',',':')) for s in changed)+'\nPROTOCOL ACTIONS '+mode+' SELF-TEST: PASSED\n'
        try:verify(args.work,mode,0,text)
        except RuntimeError as error:results.append({'mutation':name,'rejected':True,'reason':str(error)})
        else:raise RuntimeError('Accepted broken counterexample: '+name)
    path=Path(__file__).with_name('check_protocol_actions.py')
    report={'verifier_sha256':hashlib.sha256(path.read_bytes()).hexdigest(),'input_stdout_sha256':hashlib.sha256(raw).hexdigest(),
        'control_arrow_ink_pixels':control['arrow_ink_pixels'],'counterexamples':results,'new_images':0,'native_runs':0}
    (args.work/'counterexamples.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    print('Actual action state accepted; missing copy, stretched width, changed source and blank arrow rejected')
if __name__=='__main__':main()
