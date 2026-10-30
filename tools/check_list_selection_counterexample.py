"""Actual native A2 row counterimages without replacing the verified app."""
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
 proof.mkdir();normal=directory/'bin/physim.exe';normal_files=[normal,normal.with_suffix('.pdb')];production=archive(proof/'production.zip',normal_files)
 originals=[source/name for name in ('app/main.c','app/design_ui.inc','app/protocol_ui.inc','app/selection_ui.h','app/design_tokens.h','app/list_selection_tests.inc','tests/check_list_selection_a2.py','docs/visuals/selection-options/selection.css')]
 main,design,protocol,selection=[path.read_text(encoding='utf8') for path in originals[:4]]
 declaration='static bool list_counter_fault(const char *name){const char *fault=SDL_getenv("PHYSIM_LIST_A2_COUNTER");return fault && !strcmp(fault,name);}\n'
 selection=replace_once(selection,'enum { PS_SELECTION_COLUMN = 29 };','#include <string.h>\n'+declaration+'enum { PS_SELECTION_COLUMN = 29 };')
 selection=replace_once(selection,'nk_fill_rect(canvas,row,rounding,palette->text_selection);','struct nk_color wash=palette->text_selection;if(list_counter_fault("opaque"))wash.a=255;nk_fill_rect(canvas,row,rounding,wash);')
 selection=replace_once(selection,'nk_fill_rect(canvas,nk_rect(row.x,row.y,3*scale,row.h),0,palette->selection_accent);','if(!list_counter_fault("edge"))nk_fill_rect(canvas,nk_rect(row.x,row.y,3*scale,row.h),0,palette->selection_accent);')
 selection=replace_once(selection,'if(selected) {','if(selected && !list_counter_fault("check")) {')
 selection=replace_once(selection,'if(scale<=0 || row.w<=4*scale || row.h<=4*scale)return;','if(list_counter_fault("focus")){nk_stroke_rect(canvas,row,0,2*scale,palette->selection_focus);return;}\n    if(scale<=0 || row.w<=4*scale || row.h<=4*scale)return;')
 protocol=replace_once(protocol,'protocol_level_color(r->message.level));','active && list_counter_fault("status")?UI_COLOR(text):protocol_level_color(r->message.level));')
 for text in ('preview','preview+first'):
  protocol=replace_once(protocol,text+',UI_COLOR(text));',text+',active && list_counter_fault("ink")?nk_rgb(255,255,255):UI_COLOR(text));')
 generated=[proof/name for name in ('counter_selection.h','counter_protocol.inc','counter_design.inc','counter_main.c')]
 design=replace_once(design,'#include "protocol_ui.inc"','#include "'+generated[1].as_posix()+'"')
 main=replace_once(replace_once(main,'#include "selection_ui.h"','#include "'+generated[0].as_posix()+'"'),'#include "design_ui.inc"','#include "'+generated[2].as_posix()+'"')
 for path,text in zip(generated,(selection,protocol,design,main)):path.write_text(text,encoding='utf8')
 helpers=[Path(__file__).resolve(),Path(__file__).with_name('check_project_window_counterexample.py').resolve()]
 sources=archive(proof/'source.zip',originals+generated+helpers)
 builder=build.Builder(options,build.compiler_environment());libraries=[builder.lib/('physim-'+name+'.lib') for name in ('project','batch','platform','language','zlib','core')]
 assert all(path.is_file() for path in libraries),'Complete the normal Release build first'
 inputs=[str(generated[3]) if name=='main' else 'app/'+name+'.c' for name in build.APP]+build.LINEBREAK+build.GRAPHEME
 app=builder.executable('physim-list-a2-counter',inputs,libraries,sdl=True,defines=('Z_PREFIX','PS_SOURCE_DIR="'+source.as_posix()+'"'))
 cases=[]
 expected={'opaque':'A2 row wash differs','edge':'A2 3px row accent missing','check':'A2 selected check missing','status':'A2 changes original row ink','ink':'Selection changed original text','focus':'Missing native dashed focus commands'}
 for fault in (*expected,'positive'):
  command=[sys.executable,str(source/'tests/check_list_selection_a2.py'),'--app',str(normal if fault=='positive' else app),'--root',str(source),'--work',str(proof/fault)]
  environment=dict(builder.env);environment.pop('PHYSIM_LIST_A2_COUNTER',None)
  if fault!='positive':command+=['--context','files' if fault=='focus' else 'protocol','--theme','dark'];environment['PHYSIM_LIST_A2_COUNTER']=fault
  result=subprocess.run(command,cwd=source,capture_output=True,timeout=1200,env=environment);log=proof/(fault+'.log');log.write_bytes(result.stdout+result.stderr)
  if fault=='positive':assert result.returncode==0,result.stderr[-3000:]
  else:assert result.returncode!=0 and expected[fault].encode() in result.stderr,result.stderr[-3000:]
  cases.append(dict(variant=fault,command=command,exit_code=result.returncode,binary_sha256=digest(normal if fault=='positive' else app),log=str(log),log_sha256=digest(log),expected_failure=expected.get(fault)))
 for path in normal_files:assert digest(path)==production[path.name],'Normal runtime changed'
 runtime=archive(proof/'runtime.zip',[app,app.with_suffix('.pdb')])
 with a.report.open('x',encoding='utf8') as stream:json.dump(dict(source_archive=str(proof/'source.zip'),source_members=sources,production_members=production,runtime_members=runtime,cases=cases,production_binary_unchanged=True,all_archive_bytes_read_back=True),stream,indent=2)
print('A2 LIST COUNTEREXAMPLES: opacity/edge/check/status/text/focus rejected; unchanged normal app passed')
