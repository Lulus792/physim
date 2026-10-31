"""Require the previous native F1 Search handler to fail actual AppKit Cmd+F."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys

import build


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--compiler',default='clang')
    parser.add_argument('--sdl',type=Path,required=True)
    parser.add_argument('--build-dir',type=Path,required=True)
    parser.add_argument('--proof-dir',type=Path)
    args=parser.parse_args()
    if sys.platform!='darwin':
        parser.error('This probe requires actual AppKit execution on macOS')
    directory=args.build_dir.resolve()
    proof=args.proof_dir.resolve() if args.proof_dir else directory/'native-search-counterexample'
    proof.relative_to(directory)
    proof.mkdir();(proof/'workflow').mkdir()
    native=(build.ROOT/'app/native_menu_ui.inc').read_text(encoding='utf8')
    fixed='            documentation_search_open(a);'
    assert native.count(fixed)==1,'Review the native F1 Search handler'
    previous=native.replace(fixed,'            a->doc_search_focus=true;a->doc_filter_active=false;')
    (proof/'native_menu_ui.inc').write_text(previous,encoding='utf8')
    # Quoted includes must follow the copied chain all the way to this override.
    for name in ('design_ui.inc','toolbar_ui.inc'):
        (proof/name).write_bytes((build.ROOT/'app'/name).read_bytes())
    main_source=proof/'main.c'
    main_source.write_text('/* isolated native Search baseline */\n'+(build.ROOT/'app/main.c').read_text(encoding='utf8'),encoding='utf8')
    options=argparse.Namespace(build_dir=directory,compiler=args.compiler,sdl=args.sdl,
        config='Release',no_app=False,sanitizers=False,jobs=4,rebuild=False)
    with build.build_lock(directory):
        builder=build.Builder(options,dict(os.environ))
        libraries=[builder.lib/('libphysim-'+name+'.a') for name in ('project','batch','platform','language','zlib','core')]
        if not all(p.is_file() for p in libraries):
            raise RuntimeError('Build the current Release app before the counterexample')
        sources=([str(main_source) if name=='main' else f'app/{name}.c' for name in build.APP]
                 +build.LINEBREAK+build.GRAPHEME)
        binary=builder.executable('physim-native-search-counterexample',sources,libraries,sdl=True,
            defines=('Z_PREFIX',f'PS_SOURCE_DIR="{build.ROOT.as_posix()}"'))
    command=[sys.executable,str(build.ROOT/'tests/check_display_workflow.py'),'accessibility_edit_macos',
        '--app',str(binary),'--root',str(build.ROOT),'--work',str(proof/'workflow')]
    result=subprocess.run(command,cwd=build.ROOT,capture_output=True,timeout=120)
    (proof/'stdout').write_bytes(result.stdout);(proof/'stderr').write_bytes(result.stderr)
    step_file=proof/'workflow/app-steps.json'
    steps=json.loads(step_file.read_text(encoding='utf8')) if step_file.is_file() else []
    diagnostic='NATIVE F1 SEARCH OPEN: FAILED (Cmd+F did not open the search island)'
    precise=result.returncode!=0 and any(s.get('exit_code')==1 and diagnostic in s.get('stderr','') for s in steps)
    report=dict(command=command,exit_code=result.returncode,precise_rejection=precise,
        source_sha256=hashlib.sha256(native.encode()).hexdigest(),previous_handler_sha256=hashlib.sha256(previous.encode()).hexdigest(),
        fixture_sha256=hashlib.sha256((build.ROOT/'app/documentation_keyboard_tests.inc').read_bytes()).hexdigest(),
        binary_sha256=hashlib.sha256(binary.read_bytes()).hexdigest(),shared_source_unchanged=True,
        scope='Actual AppKit Cmd+F; only the native Search handler is restored to its previous body')
    (proof/'report.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
    if not precise:
        raise RuntimeError('Previous native handler did not reach the specific Cmd+F search-opening failure')
    print('NATIVE F1 SEARCH COUNTEREXAMPLE: previous handler rejected by actual AppKit Cmd+F')


if __name__=='__main__':
    main()
