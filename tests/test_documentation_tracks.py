"""Check both shipped learning routes and their executable paired model sources."""
import argparse
import json
from pathlib import Path
import re
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--root',type=Path,required=True)
parser.add_argument('--catalog',type=Path,required=True)
args=parser.parse_args();root=args.root.resolve()
catalog=json.loads(args.catalog.read_text(encoding="utf-8"))
registered=set(re.findall(r'\{"[^"]+", "([^"]+)", (?:true|false)\}',(root/'app/documentation_ui.inc').read_text(encoding="utf-8"))) if (root/'app/documentation_ui.inc').exists() else None
models=('documentation_vacuum_source','documentation_projectile_drag_source','documentation_pendulum_source','documentation_collision_source','documentation_spring_source','documentation_monte_carlo_source','documentation_saved_run_source','documentation_material_source')
for language,home,primer in [('c','docs/c-guide.md','docs/c-workflow.md'),('physim','docs/physim-guide.md','docs/physim-workflow.md')]:
    text=(root/home).read_text(encoding="utf-8");links=re.findall(r'\]\(([^)]+)\)',text)
    for label in ('## 1.','## 2.','## 3.','## 4.','## 5.','## 6.','## 7.'):
        assert label in text,(home,label)
    resolved={(Path(home).parent/link).as_posix() for link in links if not link.startswith(('http:','https:'))}
    assert primer in resolved and 'docs/build.md' in resolved and 'docs/troubleshooting.md' in resolved
    assert ('docs/experiment-tutorial.md' if language=='c' else 'docs/language-tutorial.md') in resolved
    assert ('docs/reference/core.md' if language=='c' else 'docs/reference/language-library.md') in resolved
    for path in resolved:
        assert (root/path).is_file(),(home,path)
        if registered is not None:assert path in registered,(home,'offline link not registered',path)
    assert Path(home).name in (root/primer).read_text(encoding="utf-8"),(primer,'return to own route missing')
    for group in models:
        model=catalog[group];assert model['document'] in resolved,(home,group)
        sources=[entry for entry in model['sources'] if entry['language']==language]
        assert len(sources)>=2,(group,language,'experiment/analysis sources missing')
        document=(root/model['document']).read_text(encoding="utf-8")
        for source in sources:
            code=(root/source['path']).read_text(encoding="utf-8").strip()
            assert code in document,(group,source['path'],'published code drift')
for group in ('documentation_c_workflow_source','documentation_physim_workflow_source'):
    item=catalog[group];text=(root/item['document']).read_text(encoding="utf-8")
    for source in item['sources']:assert (root/source['path']).read_text(encoding="utf-8").strip() in text
assert 'c-guide.md' in (root/'docs/guide.md').read_text(encoding="utf-8") and 'physim-guide.md' in (root/'docs/guide.md').read_text(encoding="utf-8")
assert '../c-guide.md' in (root/'docs/reference/core.md').read_text(encoding="utf-8")
assert '../physim-guide.md' in (root/'docs/reference/language-library.md').read_text(encoding="utf-8")
print('Learning routes: seven ordered chapters each, eight shared paired models, complete published sources and offline navigation targets passed')
