import contextlib
import hashlib
import io
import json
from pathlib import Path
import subprocess
import sys
import types

repo = Path(__file__).resolve().parents[2]
source_base = '58185206114e79c4684914ff6f285517f1a6ee5d'
baseline = repo / 'build/validation/protocol-wrap-2026-10-26/3-isolated/src'
rows = []
for phase in ('before', 'after'):
    sources = {}
    for name in ('generate-reference.py', 'reference_descriptions.py'):
        path = 'tools/' + name
        sources[name] = (subprocess.check_output(['git', 'show', source_base + ':' + path], cwd=repo)
                         if phase == 'before' else (repo / path).read_bytes())
    desc = types.ModuleType('reference_descriptions')
    exec(compile(sources['reference_descriptions.py'], 'reference_descriptions.py', 'exec'), desc.__dict__)
    sys.modules['reference_descriptions'] = desc
    module = types.ModuleType('reference_check')
    module.__file__ = str(repo / 'tools/generate-reference.py')
    exec(compile(sources['generate-reference.py'], module.__file__, 'exec'), module.__dict__)
    module.ROOT = baseline
    sys.argv = ['generate-reference.py', '--check']
    output = io.StringIO()
    code = 0
    with contextlib.redirect_stdout(output), contextlib.redirect_stderr(output):
        try:
            module.main()
        except SystemExit as error:
            code = error.code if isinstance(error.code, int) else 1
            print(error.code)
    rows.append(dict(phase=phase, exit_code=code, output=output.getvalue(),
                     source_sha256={name: hashlib.sha256(raw).hexdigest() for name, raw in sources.items()}))
assert rows[0]['exit_code'] == 1 and 'docs/reference/experiment.md, docs/reference/language-library.md' in rows[0]['output'], rows
assert rows[1]['exit_code'] == 0, rows
result = dict(platform=sys.platform, baseline=str(baseline),
              command='python -B docs/verification/reference-generator-2026-11-04.py',
              mode='Read-only generator --check against the frozen clean unit-focus-compact source tree; before uses pinned 58185206 generator inputs, after uses the two modified generator inputs.',
              phases=rows)
target = repo / 'build/validation/continuation-2026-11-04' / ('reference-' + sys.platform + '.json')
target.parent.mkdir(parents=True, exist_ok=True)
target.write_text(json.dumps(result, ensure_ascii=False, indent=2) + '\n', encoding='utf8')
print(json.dumps(result, ensure_ascii=False))
