"""Require immediate in-frame formula texture eviction to fail the real F1 probe."""
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
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--compiler',default='clang')
    parser.add_argument('--sdl',type=Path,required=True)
    parser.add_argument('--build-dir',type=Path,default=build.ROOT/'build/native/Debug-sanitized')
    args=parser.parse_args()
    if sys.platform not in ('linux','darwin'):
        parser.error('Use the actual Linux or macOS graphics environment')
    directory=args.build_dir.resolve();proof=directory.parent/'inline-math-counterexample'
    proof.mkdir()  # Never overwrite earlier evidence.
    (proof/'workflow').mkdir()
    ui=(build.ROOT/'app/documentation_ui.inc').read_text(encoding='utf-8')
    math=(build.ROOT/'app/documentation_math.inc').read_text(encoding='utf-8')
    before='if((!item->texture || item->frame!=cache->frame) && (!slot || item->used<slot->used))slot=item;'
    assert math.count(before)==1
    previous=math.replace(before,'if(!slot || item->used<slot->used)slot=item;')
    eviction='    if(slot->texture)ps_graphics_delete_texture(a->doc_graphics,slot->texture);'
    assert previous.count(eviction)==1
    previous=previous.replace(eviction,
        '    if(slot->texture && slot->frame==cache->frame)printf("INLINE MATH INVALID EVICTION: handle=%u\\n",slot->texture);\n'+eviction)
    (proof/'documentation_ui.inc').write_text(ui,encoding='utf-8')
    (proof/'documentation_math.inc').write_text(previous,encoding='utf-8')
    source=(build.ROOT/'app/main.c').read_text(encoding='utf-8')
    main_source=proof/'main.c'
    main_source.write_text('/* isolated inline cache '+hashlib.sha256(previous.encode()).hexdigest()+' */\n'+source,encoding='utf-8')
    options=argparse.Namespace(build_dir=directory,compiler=args.compiler,sdl=args.sdl,
        config='Debug',no_app=False,sanitizers=True,jobs=4,rebuild=False)
    with build.build_lock(directory):
        builder=build.Builder(options,dict(os.environ))
        libraries=[builder.lib/('libphysim-'+name+'.a') for name in ('project','batch','platform','language','zlib','core')]
        if not all(p.is_file() for p in libraries):raise RuntimeError('Build the current sanitized viewer first')
        sources=[str(main_source) if name=='main' else f'app/{name}.c' for name in build.APP]
        binary=builder.executable('physim-inline-math-counterexample',sources,libraries,sdl=True,
            defines=('Z_PREFIX',f'PS_SOURCE_DIR="{build.ROOT.as_posix()}"'))
    command=[sys.executable,str(build.ROOT/'tests/check_display_workflow.py'),'documentation_inline_math',
        '--app',str(binary),'--root',str(build.ROOT),'--work',str(proof/'workflow')]
    result=subprocess.run(command,cwd=build.ROOT,capture_output=True,timeout=120)
    (proof/'stdout').write_bytes(result.stdout);(proof/'stderr').write_bytes(result.stderr)
    steps_path=proof/'workflow/app-steps.json'
    steps=json.loads(steps_path.read_text(encoding='utf-8')) if steps_path.is_file() else []
    output='\n'.join(s.get('stdout','') for s in steps)
    errors='\n'.join(s.get('stderr','') for s in steps)
    states=re.findall(r'INLINE MATH DRAW: case=(6) item=(\d+) font=16 theme=0 complete=0',output)
    evictions=re.findall(r'INLINE MATH INVALID EVICTION: handle=(\d+)',output)
    render_error='Documentation rendering: OpenGL UI render: 0x502' in errors
    rejected=result.returncode!=0 and bool(evictions) and (
        (bool(states) and 'INLINE MATH SELF-TEST: FAILED' in output) or render_error)
    report=dict(command=command,exit_code=result.returncode,precise_rejection=rejected,
        source_sha256=hashlib.sha256(math.encode()).hexdigest(),
        isolated_previous_cache_sha256=hashlib.sha256(previous.encode()).hexdigest(),
        binary_sha256=hashlib.sha256(binary.read_bytes()).hexdigest(),rejected_states=states,
        invalid_eviction_handles=evictions,actual_invalid_operation=render_error)
    (proof/'report.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    if not rejected:raise RuntimeError('Immediate eviction did not reach and fail the simultaneous formula draw probe')
    print('INLINE MATH COUNTEREXAMPLE: immediate in-frame texture eviction rejected by actual rendering')


if __name__=='__main__':main()
