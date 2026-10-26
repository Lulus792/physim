"""Real outline hover remains visible; clipped rows cannot publish tooltips."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--app',type=Path,required=True)
    parser.add_argument('--work',type=Path,required=True)
    args=parser.parse_args();args.work.mkdir(parents=True,exist_ok=True)
    digest=hashlib.sha256(args.app.read_bytes()).hexdigest();results=[]
    for mode in ('outline-hover-dark','outline-hover-light','outline-hover-large'):
        work=args.work/mode;work.mkdir(exist_ok=False)
        command=[str(args.app),'--workspace-state-test',str(work),mode]
        tested=subprocess.run(command,capture_output=True,timeout=60)
        (work/'stdout.bin').write_bytes(tested.stdout);(work/'stderr.bin').write_bytes(tested.stderr)
        output=tested.stdout.decode('utf-8')
        states=[json.loads(line) for line in output.splitlines() if line.startswith('{"kind":"outline_hover"')]
        passed=(tested.returncode==0 and 'OUTLINE HOVER SELF-TEST: PASSED' in output and
                [s['stage'] for s in states]==[1,2,3] and all(s['passed'] for s in states) and
                states[1]['visible_commands']>states[1]['visible_base']>0 and states[2]['hidden_commands']==0 and
                len({s['source_crc'] for s in states})==1)
        results.append({'command':command,'exit_code':tested.returncode,'passed':passed,'states':states})
        (args.work/'verification.json').write_text(json.dumps({'app_sha256':digest,'cases':results},indent=2)+'\n',encoding='utf-8')
        if not passed:raise RuntimeError('Native outline hover failed: '+mode)
        assert hashlib.sha256(args.app.read_bytes()).hexdigest()==digest
    print('Native outline hover: visible tooltip retained, clipped row over protocol rejected, source intact; 3 modes passed')


if __name__=='__main__':main()
