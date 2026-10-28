"""Nested Escape and preserved native F1 while project B remains open."""
import argparse,hashlib,json,os,subprocess
from pathlib import Path
parser=argparse.ArgumentParser(description=__doc__)
for name in ('app','work'):parser.add_argument('--'+name,type=Path,required=True)
args=parser.parse_args();args.work.mkdir(parents=True,exist_ok=True);binary=hashlib.sha256(args.app.read_bytes()).hexdigest();cases=[]
for mode in ('manager-design-help','manager-design-help-large'):
    work=args.work/mode;work.mkdir()
    command=[str(args.app),'--workspace-state-test',str(work),mode]
    result=subprocess.run(command,capture_output=True,timeout=110,env=dict(os.environ,PHYSIM_TEST_TRACE='1'))
    (work/'stdout.bin').write_bytes(result.stdout);(work/'stderr.bin').write_bytes(result.stderr)
    passed=result.returncode==0 and b'PROJECT WIZARD HELP SELF-TEST: PASSED' in result.stdout
    cases.append(dict(command=command,exit_code=result.returncode,passed=passed))
    (args.work/'verification.json').write_text(json.dumps(dict(app_sha256=binary,cases=cases),indent=2)+'\n',encoding='utf8')
    assert passed,mode
    assert hashlib.sha256(args.app.read_bytes()).hexdigest()==binary
print('Project B: nested popup Escape, native F1, retained inputs and final dialog Escape passed at both sizes')
