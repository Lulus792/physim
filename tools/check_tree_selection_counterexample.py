"""Build native A2 tree defects separately; preserve the normal EXE/PDB bytes."""
import argparse,json,os,subprocess,sys
from pathlib import Path
from check_project_window_counterexample import archive,digest,replace_once

p=argparse.ArgumentParser(description=__doc__)
for key in ('source-root','build-dir','sdl','proof-dir','report'):p.add_argument('--'+key,type=Path,required=True)
a=p.parse_args()
if sys.platform!='win32':p.error('Requires a completed Windows Release build')
source,directory,proof=(value.resolve() for value in (a.source_root,a.build_dir,a.proof_dir))
if directory not in proof.parents:p.error('Keep generated counter sources inside the build directory')
sys.path.insert(0,str(source/'tools'));import build
options=argparse.Namespace(build_dir=directory,config='Release',compiler=None,no_app=False,sdl=a.sdl.resolve(),sanitizers=False,jobs=4,rebuild=False)
with build.build_lock(directory):
    proof.mkdir();normal=directory/'bin/physim.exe';normal_files=[normal,normal.with_suffix('.pdb')]
    # The campaign retains the normal runtime in its separate checkpoint.
    # Hash it before/after instead of writing another identical production ZIP.
    production={path.name:digest(path) for path in normal_files}
    originals=[source/name for name in ('app/main.c','app/design_ui.inc','app/scene_tree_ui.inc','app/selection_ui.h','app/tree_selection_tests.inc','tests/check_list_selection_a2.py','tests/check_tree_selection_a2.py','docs/visuals/selection-options/selection.css')]
    main,design,scene,selection=[path.read_text(encoding='utf8') for path in originals[:4]]
    declaration='static bool tree_counter_fault(const char *name){const char *fault=SDL_getenv("PHYSIM_TREE_A2_COUNTER");return fault && !strcmp(fault,name);}\n'
    selection=replace_once(selection,'enum { PS_SELECTION_COLUMN = 29 };','#include <string.h>\n'+declaration+'enum { PS_SELECTION_COLUMN = 29 };')
    selection=replace_once(selection,'nk_fill_rect(canvas,row,rounding,palette->text_selection);','struct nk_color wash=palette->text_selection;if(tree_counter_fault("opaque"))wash.a=255;nk_fill_rect(canvas,row,rounding,wash);')
    selection=replace_once(selection,'if(selected) {','if(selected && !tree_counter_fault("check")) {')
    selection=replace_once(selection,'return ps_ui_mouse_pressed_in_rect(&ui->input,NK_BUTTON_LEFT,ui->current->layout->clip) &&','return !tree_counter_fault("select") && ps_ui_mouse_pressed_in_rect(&ui->input,NK_BUTTON_LEFT,ui->current->layout->clip) &&')
    scene=replace_once(scene,'laboratory_wrapped_text(canvas,font,title,text,line,effective?UI_COLOR(text):muted());','laboratory_wrapped_text(canvas,font,title,text,line,selected && tree_counter_fault("ink")?nk_rgb(255,255,255):effective?UI_COLOR(text):muted());')
    scene=replace_once(scene,'if(ps_ui_switch_at(ui,plot_intersection(eye,visible),label,&own))','if(ps_ui_switch_at(ui,plot_intersection(eye,visible),label,&own) && !tree_counter_fault("visibility"))')
    scene=replace_once(scene,'a->scene_view.collapsed^=bit;','if(!tree_counter_fault("fold"))a->scene_view.collapsed^=bit;')
    generated=[proof/name for name in ('counter_selection.h','counter_scene.inc','counter_design.inc','counter_main.c')]
    design=replace_once(design,'#include "scene_tree_ui.inc"','#include "'+generated[1].as_posix()+'"')
    main=replace_once(replace_once(main,'#include "selection_ui.h"','#include "'+generated[0].as_posix()+'"'),'#include "design_ui.inc"','#include "'+generated[2].as_posix()+'"')
    for path,text in zip(generated,(selection,scene,design,main)):path.write_text(text,encoding='utf8')
    helpers=[Path(__file__).resolve(),Path(__file__).with_name('check_project_window_counterexample.py').resolve()]
    sources=archive(proof/'source.zip',originals+generated+helpers)
    builder=build.Builder(options,build.compiler_environment());libraries=[builder.lib/('physim-'+name+'.lib') for name in ('project','batch','platform','language','zlib','core')]
    assert all(path.is_file() for path in libraries),'Complete the normal Release build first'
    inputs=[str(generated[3]) if name=='main' else 'app/'+name+'.c' for name in build.APP]+build.LINEBREAK+build.GRAPHEME
    app=builder.executable('physim-tree-a2-counter',inputs,libraries,sdl=True,defines=('Z_PREFIX','PS_SOURCE_DIR="'+source.as_posix()+'"'))
    expected={'opaque':('A2 row wash differs',0),'check':('A2 selected check missing',0),'ink':('Selection changed original text',0),
              'select':('TREE A2 failed stage=3',1),'visibility':('TREE A2 failed stage=6',1),'fold':('TREE A2 failed stage=10',1)}
    cases=[]
    for fault in (*expected,'positive'):
        command=[sys.executable,str(source/'tests/check_list_selection_a2.py'),'--app',str(normal if fault=='positive' else app),'--root',str(source),'--work',str(proof/fault),'--context','scene','--theme','dark']
        environment=dict(builder.env);environment.pop('PHYSIM_TREE_A2_COUNTER',None)
        if fault!='positive':environment['PHYSIM_TREE_A2_COUNTER']=fault
        result=subprocess.run(command,cwd=source,capture_output=True,timeout=120,env=environment);log=proof/(fault+'.log');log.write_bytes(result.stdout+result.stderr)
        report=json.loads((proof/fault/'verification.json').read_text(encoding='utf8'));assert len(report['cases'])==1
        if fault=='positive':assert result.returncode==0,result.stderr[-3000:]
        else:
            message,native_exit=expected[fault]
            assert result.returncode!=0 and message.encode() in result.stderr,result.stderr[-3000:]
            assert report['cases'][0]['exit_code']==native_exit,report['cases'][0]
        cases.append(dict(variant=fault,command=command,exit_code=result.returncode,native_exit_code=report['cases'][0]['exit_code'],binary_sha256=digest(normal if fault=='positive' else app),log=str(log),log_sha256=digest(log),expected_failure=expected.get(fault)))
    for path in normal_files:assert digest(path)==production[path.name],'Normal runtime changed'
    runtime=archive(proof/'runtime.zip',[app,app.with_suffix('.pdb')])
    with a.report.open('x',encoding='utf8') as stream:json.dump(dict(source_archive=str(proof/'source.zip'),source_members=sources,production_sha256=production,runtime_members=runtime,cases=cases,production_binary_unchanged=True,all_archive_bytes_read_back=True),stream,indent=2)
print('TREE A2 COUNTEREXAMPLES: opacity/check/ink/selection/visibility/fold rejected; normal scene passed')
