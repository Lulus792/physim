"""Cold native project builds of the published C/Physim domain learning paths."""
import argparse
import hashlib
from pathlib import Path
import shutil
import subprocess
import sys

parser = argparse.ArgumentParser(description=__doc__)
for name in ('builder', 'sdk', 'compiler', 'runner', 'analysis-runner', 'probe', 'oracle', 'work'):
    parser.add_argument('--' + name, type=Path, required=True)
parser.add_argument('--domain', choices=('thermal', 'rc', 'string', 'transport', 'property'), required=True)
parser.add_argument('--cc')
args = parser.parse_args()
args.work.mkdir(parents=True, exist_ok=True)
extension = '.dll' if sys.platform == 'win32' else '.so'
products = {}

def checked(command, log, timeout=150):
    result = subprocess.run(list(map(str, command)), capture_output=True, timeout=timeout)
    output = result.stdout + result.stderr
    log.write_bytes(output)
    assert result.returncode == 0, (command, result.returncode, output.decode('utf-8', errors='replace'))
    return output

for language, suffix in (('c', '.c'), ('physim', '.phys')):
    project = args.work / ('Documented ' + args.domain + ' ' + language + ' ä')
    project.mkdir()  # Existing outputs must never hide a cold-build failure.
    for kind, target in (('main', 'main'), ('analysis', 'analysis')):
        shutil.copy2(args.sdk / 'examples/documentation' / (args.domain + '_' + kind + suffix),
                     project / (target + suffix))
    (project / 'physim.project').write_text(
        'physim_project=1\nexperiment=main' + suffix + '\nanalysis=analysis' + suffix + '\n',
        encoding='utf-8')
    original = {p.name: hashlib.sha256(p.read_bytes()).hexdigest() for p in project.iterdir()}
    output = project / 'build/Release'
    command = [args.builder, '--project', project, '--sdk', args.sdk, '--output', output,
               '--physimc', args.compiler, '--profile', 'Release']
    if args.cc:
        command += ['--cc', args.cc]
    checked(command, args.work / (language + '-cold-build.log'))
    for module in ('thermodynamics', 'electromagnetism', 'waves', 'optics', 'fluid', 'properties'):
        assert (output / ('sdk-' + module + '.obj')).is_file(), module
    modules = [output / ('experiment' + extension), output / ('analysis' + extension)]
    before = {p: (p.stat().st_mtime_ns, hashlib.sha256(p.read_bytes()).hexdigest()) for p in modules}
    assert b'Build up to date.' in checked(command, args.work / (language + '-cached-build.log'))
    assert before == {p: (p.stat().st_mtime_ns, hashlib.sha256(p.read_bytes()).hexdigest()) for p in modules}
    assert original == {name: hashlib.sha256((project / name).read_bytes()).hexdigest() for name in original}
    assert {p.name for p in project.iterdir()} == set(original) | {'build'}
    products[language] = modules
    print(args.domain + ' ' + language + ': cold native build and unchanged cached build passed', flush=True)

if args.domain == 'property':
    oracle = [sys.executable, args.oracle, '--runner', args.runner, '--analysis', args.analysis_runner,
              '--c-model', products['c'][0], '--phys-model', products['physim'][0],
              '--c-analysis', products['c'][1], '--phys-analysis', products['physim'][1],
              '--probe', args.probe, '--work', args.work / 'Independent results ä']
else:
    oracle = [sys.executable, args.oracle, args.runner, args.analysis_runner,
              products['c'][0], products['physim'][0], products['c'][1], products['physim'][1],
              args.probe, args.work / 'Independent results ä']
checked(oracle, args.work / 'oracle.log', timeout=180)
print(args.domain + ': native-built C/Physim sources passed the complete independent tutorial oracle', flush=True)
