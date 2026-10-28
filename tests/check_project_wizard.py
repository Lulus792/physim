"""Selected project B: native card geometry, next/back and retained inputs."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess

def require(condition,message):
    if not condition:raise RuntimeError(message)

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    for name in ('app','root','work'):parser.add_argument('--'+name,type=Path,required=True)
    args=parser.parse_args();args.work.mkdir(parents=True,exist_ok=True)
    css=(args.root/'docs/visuals/design-options/options.css').read_text(encoding='utf8')
    body=re.search(r'\.wizard-body\{([^}]+)\}',css)[1]
    maximum=float(re.search(r'max-width:(\d+)px',body)[1])
    rules={selector.strip():body for selector,body in re.findall(r'([^{}]+)\{([^{}]*)\}',css[:css.index('@media')])}
    def declaration(selector,name):
        return re.search(r'(?:^|;)'+re.escape(name)+r':([^;]+)',rules[selector])[1].strip()
    def rem(selector):return float(declaration(selector,'font-size')[:-3])*14
    role_sizes=[rem('.eyebrow'),rem('.dialog-header h2'),rem('h3'),rem('.template-card strong'),rem('.dialog-footer button'),
                rem('.template-card small'),rem('.project-tree'),rem('pre'),rem('.field'),rem('.location-field input'),rem('.field small'),rem('.project-tree')]
    line=float(declaration('body','font').split('/')[1].split()[0]);padding=declaration('button','padding').split()
    py,px=(float(value[:-3])*14 for value in padding)
    minimum=float(declaration('button','min-height')[:-2]);button_height=max(minimum,role_sizes[4]*line+2*py+2)
    card_gap=float(declaration('.template-cards','gap')[:-2])
    binary=hashlib.sha256(args.app.read_bytes()).hexdigest();cases=[]
    for mode in ('dark','light','contrast','large','narrow','large-narrow'):
        work=args.work/mode;work.mkdir()
        command=[str(args.app),'--workspace-state-test',str(work),'manager-design-'+mode]
        result=subprocess.run(command,capture_output=True,timeout=50,env=dict(os.environ,PHYSIM_TEST_TRACE='1'))
        (work/'stdout.bin').write_bytes(result.stdout);(work/'stderr.bin').write_bytes(result.stderr)
        text=result.stdout.decode('utf8',errors='replace')
        rows=[json.loads(line) for line in text.splitlines() if line.startswith('{"kind":"project_wizard"')]
        controls=[json.loads(line) for line in text.splitlines() if line.startswith('{"kind":"project_wizard_controls"')]
        record=dict(mode=mode,exit_code=result.returncode,command=command,measurements=rows,controls=controls)
        cases.append(record)
        (args.work/'verification.json').write_text(json.dumps(dict(binary_sha256=binary,cases=cases),indent=2)+'\n',encoding='utf8')
        require(result.returncode==0 and 'PROJECT WIZARD SELF-TEST: PASSED' in text,'Native project wizard failed: '+mode)
        require(len(rows)==1 and rows[0]['passed'],'Missing native card geometry')
        row=rows[0];expected=min(maximum*row['scale'],row['modal'][2]-2-row['scrollbar'])
        require(abs(row['body'][2]-expected)<2,'Body width differs from selected CSS maximum')
        require(len(controls)==1,'Missing measured controls');control=controls[0];scale=row['scale']
        require(len(control['font_roles'])==len(role_sizes) and all(abs(a-b)<.02 for a,b in zip(control['font_roles'],role_sizes)),'Font role differs from reference CSS')
        require(abs(control['next'][3]-button_height*scale)<.05,'Footer button height differs from CSS')
        require(abs(control['next'][2]-control['next_text_width']-(2*px+2)*scale)<.05,'Footer button padding differs from CSS')
        require(abs(control['second_card'][0]-row['card'][0]-row['card'][2]-card_gap*scale)<.05,'Template card gap differs from CSS')
        require(hashlib.sha256(args.app.read_bytes()).hexdigest()==binary,'Program changed during test')
    print('Native project B: six themes/widths, cards, actual next/back and retained inputs passed')

if __name__=='__main__':main()
