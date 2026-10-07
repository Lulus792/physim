"""The repository and native project builders must expose the same Core modules."""
import ast
from pathlib import Path
import re

root = Path(__file__).resolve().parent.parent
tree = ast.parse((root / 'tools/build.py').read_text(encoding='utf-8'))
assignment = next(node for node in tree.body if isinstance(node, ast.Assign)
                  and any(isinstance(target, ast.Name) and target.id == 'CORE' for target in node.targets))
assert isinstance(assignment.value, ast.Call) and assignment.value.func.attr == 'split'
core = ast.literal_eval(assignment.value.func.value).split()
text = (root / 'app/build_main.c').read_text(encoding='utf-8')
catalog = re.search(r'SDK_MODULES\[\]\s*=\s*\{([^}]+)\}', text).group(1)
native = re.findall(r'"([a-z_]+)"', catalog)
assert len(core) == len(set(core)) and core == native, (core, native)
assert 'SDK_OBJECT_COUNT = SDL_arraysize(SDK_MODULES)' in text
assert all((root / 'src' / (name + '.c')).is_file() for name in core)
print('Repository and native project builders expose the same', len(core), 'Core modules')
