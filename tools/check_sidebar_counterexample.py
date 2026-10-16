"""Require the native viewer to reject clipped Design H section labels."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys

import build


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--compiler', default='clang')
    parser.add_argument('--sdl', type=Path, required=True)
    parser.add_argument('--build-dir', type=Path, default=build.ROOT/'build/native/Debug-sanitized')
    parser.add_argument('--proof-dir', type=Path, help='Fresh evidence directory; never overwritten')
    args = parser.parse_args()
    if sys.platform not in ('linux','darwin'):
        parser.error('Use an actual Linux or macOS native font environment')
    directory = args.build_dir.resolve()
    proof = args.proof_dir.resolve() if args.proof_dir else directory.parent/'sidebar-counterexample'
    proof.mkdir()  # Preserve an earlier result instead of overwriting it.
    ui = (build.ROOT/'app/documentation_ui.inc').read_text(encoding='utf-8')
    current = (build.ROOT/'app/documentation_h.inc').read_text(encoding='utf-8')
    source = 'documentation_height(ui->style.font,label,strlen(label),content.w-indent-extra-20)+10'
    if current.count(source)!=1:
        raise RuntimeError('Direct section measurement changed; review this counterexample')
    # Reintroduce the original fault (one-line height for wrapped section text)
    # in H's current indented section renderer. Its draw probe stays unchanged.
    previous = current.replace(source,'ui->style.font->height+10')
    (proof/'documentation_h.inc').write_text(previous, encoding='utf-8')
    (proof/'documentation_ui.inc').write_text(ui, encoding='utf-8')
    # The generated main lives beside the isolated old layout. All other
    # includes and objects remain current, including the independent draw probe.
    original_main = (build.ROOT/'app/main.c').read_text(encoding='utf-8')
    main_source = proof/'main.c'
    main_source.write_text('/* isolated sidebar '+hashlib.sha256(previous.encode()).hexdigest()+' */\n'+original_main,
                           encoding='utf-8')
    options = argparse.Namespace(build_dir=directory, compiler=args.compiler, sdl=args.sdl,
        config='Debug', no_app=False, sanitizers=True, jobs=4, rebuild=False)
    with build.build_lock(directory):
        builder = build.Builder(options, dict(os.environ))
        libraries = [builder.lib/('libphysim-'+name+'.a')
                     for name in ('project','batch','platform','language','zlib','core')]
        if not all(path.is_file() for path in libraries):
            raise RuntimeError('Build and run the current macOS viewer before the counterexample')
        sources = [str(main_source) if name=='main' else f'app/{name}.c' for name in build.APP]
        binary = builder.executable('physim-sidebar-counterexample', sources, libraries, sdl=True,
            defines=('Z_PREFIX', f'PS_SOURCE_DIR="{build.ROOT.as_posix()}"'))
    command = [sys.executable, str(build.ROOT/'tests/check_display_workflow.py'), 'documentation_sidebar',
               '--app', str(binary), '--root', str(build.ROOT), '--work', str(proof/'workflow')]
    result = subprocess.run(command, cwd=build.ROOT, capture_output=True, timeout=120)
    (proof/'stdout').write_bytes(result.stdout)
    (proof/'stderr').write_bytes(result.stderr)
    output = result.stdout.decode('utf-8', 'replace')
    matches = re.findall(r'DOCUMENTATION SIDEBAR DRAW: font=\d+ lines=(\d+) complete=0', output)
    rejected = result.returncode != 0 and any(int(lines)>=1 for lines in matches)
    report = dict(command=command, exit_code=result.returncode, precise_rejection=rejected,
        source_sha256=hashlib.sha256(ui.encode()).hexdigest(),
        isolated_previous_layout_sha256=hashlib.sha256(previous.encode()).hexdigest(),
        binary_sha256=hashlib.sha256(binary.read_bytes()).hexdigest(), clipped_sections=matches)
    (proof/'report.json').write_text(json.dumps(report, indent=2)+'\n', encoding='utf-8')
    if not rejected:
        raise RuntimeError('Counterexample did not reach and reject a clipped direct section label')
    print('SIDEBAR COUNTEREXAMPLE: actual clipped direct section label rejected')


if __name__ == '__main__':
    main()
