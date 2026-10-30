"""Actual A2 opacity/ink counterimages with unchanged positive native fixtures."""
import argparse,json,subprocess,sys
from pathlib import Path
from check_project_window_counterexample import archive,digest,replace_once
p=argparse.ArgumentParser(description=__doc__)
for key in ('source-root','build-dir','sdl','proof-dir','report'):p.add_argument('--'+key,type=Path,required=True)
a=p.parse_args()
if sys.platform!='win32':p.error('Requires completed Windows Release build')
source,directory,proof=(v.resolve() for v in (a.source_root,a.build_dir,a.proof_dir))
if directory not in proof.parents:p.error('Keep generated sources under the chosen build directory')
sys.path.insert(0,str(source/'tools'));import build
options=argparse.Namespace(build_dir=directory,config='Release',compiler=None,no_app=False,sdl=a.sdl.resolve(),sanitizers=False,jobs=4,rebuild=False)
with build.build_lock(directory):
 proof.mkdir();normal=directory/'bin/physim.exe';normal_files=[normal,normal.with_suffix('.pdb')];production=archive(proof/'production.zip',normal_files)
 originals=[source/name for name in ('app/design_tokens.h','app/design_ui.inc','app/main.c','third_party/nuklear.h','app/text_selection_tests.inc','tests/check_text_selection_a2.py','tests/test_text_selection.c','docs/visuals/selection-options/APPROVED.md')]
 tokens,design,main=[p.read_text(encoding='utf8') for p in originals[:3]];generated=[];mains={}
 for fault in ('opaque','ink'):
  header=proof/(fault+'_tokens.h');header.write_text(replace_once(tokens,'.text_selection = {170, 184, 202, 38}','.text_selection = {170, 184, 202, 255}') if fault=='opaque' else tokens,encoding='utf8')
  ui=proof/(fault+'_design.inc');ui.write_text(replace_once(design,'text_styles[i]->selected_text_preserve=nk_true;','text_styles[i]->selected_text_preserve=nk_false;') if fault=='ink' else design,encoding='utf8')
  path=proof/(fault+'_main.c');path.write_text(replace_once(replace_once(main,'#include "design_tokens.h"','#include "'+header.as_posix()+'"'),'#include "design_ui.inc"','#include "'+ui.as_posix()+'"'),encoding='utf8');mains[fault]=path;generated.extend((header,ui,path))
 helpers=[Path(__file__).resolve(),Path(__file__).with_name('check_project_window_counterexample.py').resolve()]
 sources=archive(proof/'source.zip',originals+generated+helpers);builder=build.Builder(options,build.compiler_environment())
 libraries=[builder.lib/('physim-'+name+'.lib') for name in ('project','batch','platform','language','zlib','core')]
 assert all(p.is_file() for p in libraries),'Complete normal build first'
 cases=[];counter_files=[]
 for fault in ('opaque','ink','positive'):
  if fault=='positive':app=normal
  else:
   inputs=[str(mains[fault]) if name=='main' else 'app/'+name+'.c' for name in build.APP]+build.LINEBREAK+build.GRAPHEME
   app=builder.executable('physim-a2-'+fault+'-counter',inputs,libraries,sdl=True,defines=('Z_PREFIX','PS_SOURCE_DIR="'+source.as_posix()+'"'));counter_files.extend((app,app.with_suffix('.pdb')))
  command=[sys.executable,str(source/'tests/check_text_selection_a2.py'),'--app',str(app),'--root',str(source),'--work',str(proof/fault)]
  if fault!='positive':command+=['--context','code' if fault=='opaque' else 'field','--theme','dark']
  result=subprocess.run(command,cwd=source,capture_output=True,timeout=1200,env=builder.env);log=proof/(fault+'.log');log.write_bytes(result.stdout+result.stderr)
  expected='A2 text selection background differs' if fault=='opaque' else 'A2 selected ink differs from original'
  if fault=='positive':assert result.returncode==0,result.stderr[-3000:]
  else:assert result.returncode!=0 and expected.encode() in result.stderr,result.stderr[-3000:]
  cases.append(dict(variant=fault,command=command,exit_code=result.returncode,binary_sha256=digest(app),log=str(log),log_sha256=digest(log),expected_failure=None if fault=='positive' else expected))
 for path in normal_files:assert digest(path)==production[path.name],'Production runtime changed'
 runtime=archive(proof/'runtime.zip',counter_files)
 with a.report.open('x',encoding='utf8') as stream:json.dump(dict(source_archive=str(proof/'source.zip'),source_members=sources,production_members=production,runtime_members=runtime,cases=cases,production_binary_unchanged=True,all_archive_bytes_read_back=True),stream,indent=2)
print('A2 TEXT COUNTEREXAMPLES: actual opaque wash and changed ink rejected; unchanged production passed')
