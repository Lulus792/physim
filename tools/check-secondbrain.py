"""Verify Physim's portable project memory and optionally its real SecondBrain CLI."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess

parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--root',type=Path,default=Path(__file__).resolve().parent.parent)
parser.add_argument('--cli',type=Path)
args=parser.parse_args();root=args.root.resolve();brain=root/'brains/physim'
metadata=json.loads((brain/'brain.json').read_text(encoding='utf-8'))
assert metadata['schema_version']==1 and metadata['template_version']==1 and metadata['id']=='physim'
assert (brain/metadata['project_root']).resolve()==root
for name in ('START','PROJECT','STATE','DECISIONS','QUESTIONS','SOURCES','AGENTS'):
    assert (brain/(name+'.md')).is_file(),name
files=sorted(brain.rglob('*'));before={str(p.relative_to(brain)):hashlib.sha256(p.read_bytes()).hexdigest() for p in files if p.is_file()}
for p in brain.rglob('*.md'):
    text=p.read_text(encoding='utf-8');assert '\0' not in text and text.startswith('# '),p
    for link in re.findall(r'\]\(([^)]+)\)',text):
        if link.startswith(('https://','http://')):continue
        target=(p.parent/link.split('#',1)[0]).resolve()
        assert target.is_relative_to(root) and target.is_file(),(p,link)
if args.cli:
    cli=str(args.cli.resolve());workspace=str(brain.parent)
    def run(*arguments):
        result=subprocess.run([cli,*arguments],check=True,capture_output=True,encoding='utf-8',timeout=30)
        return result.stdout
    assert 'physim\tPhysim' in run('list',workspace)
    context=run('context',workspace,'physim');assert 'Physim Auftrag' in context and 'Physim Stand' in context
    assert 'STATE.md' in run('search',workspace,'physim','Modulanforderungen')
    assert before=={str(p.relative_to(brain)):hashlib.sha256(p.read_bytes()).hexdigest() for p in files if p.is_file()}
    print('Actual SecondBrain CLI: project listing, saved context, search and unchanged notes passed')
print('Portable SecondBrain schema, core notes and original-source links passed')
