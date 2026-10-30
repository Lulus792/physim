"""Reject native picker input/spacing regressions with unchanged positive fixtures."""
import argparse,json,subprocess,sys
from pathlib import Path
from check_project_window_counterexample import archive,digest,replace_once

def main():
    p=argparse.ArgumentParser(description=__doc__)
    for name in ('source-root','build-dir','sdl','proof-dir'):p.add_argument('--'+name,type=Path,required=True)
    p.add_argument('--fault',choices=('main-input','places-padding'),default='main-input')
    p.add_argument('--report',type=Path);a=p.parse_args()
    if sys.platform!='win32':p.error('Requires a completed Windows Release build')
    source,directory,proof=(value.resolve() for value in (a.source_root,a.build_dir,a.proof_dir))
    if directory not in proof.parents:p.error('Keep generated sources inside the chosen build folder')
    sys.path.insert(0,str(source/'tools'));import build
    options=argparse.Namespace(build_dir=directory,config='Release',compiler=None,no_app=False,
        sdl=a.sdl.resolve(),sanitizers=False,jobs=4,rebuild=False)
    with build.build_lock(directory):
        proof.mkdir();normal=directory/'bin/physim.exe';normal_files=[normal,normal.with_suffix('.pdb')]
        production=archive(proof/'production.zip',normal_files)
        originals=[source/name for name in ('app/file_browser_ui.inc','app/file_browser_api_ui.inc',
            'app/design_ui.inc','app/main.c','app/file_browser_window_tests.inc','tests/check_independent_picker_windows.py',
            'app/file_browser_tests.inc','tests/check_explorer_a.py','docs/visuals/design-options/options.css')]
        if a.fault=='main-input':
            browser=replace_once(originals[0].read_text(encoding='utf8'),
                'static bool browser_event(app *a,const SDL_Event *event,SDL_Window *window) {',
                'static bool browser_event(app *a,const SDL_Event *event,SDL_Window *window) {\n    if(window==a->window && browser_active(a))return true;')
            checker='tests/check_independent_picker_windows.py';expected=b'Independent picker stage10 failed'
        else:
            browser=replace_once(originals[0].read_text(encoding='utf8'),
                'inset=(d->compact?28.6f:37.8f)*f','inset=(d->compact?28.6f:35.f)*f')
            checker='tests/check_explorer_a.py';expected=b'Place text inset differs from approved CSS'
        picker=proof/'counter_picker.inc';picker.write_text(browser,encoding='utf8')
        design=proof/'counter_design.inc';design.write_text(replace_once(originals[2].read_text(encoding='utf8'),
            '#include "file_browser_ui.inc"','#include "'+picker.as_posix()+'"'),encoding='utf8')
        main_source=proof/'counter_main.c';main_source.write_text(replace_once(originals[3].read_text(encoding='utf8'),
            '#include "design_ui.inc"','#include "'+design.as_posix()+'"'),encoding='utf8')
        helpers=[Path(__file__).resolve(),Path(__file__).with_name('check_project_window_counterexample.py').resolve()]
        sources=archive(proof/'source.zip',[picker,design,main_source,*originals,*helpers])
        builder=build.Builder(options,build.compiler_environment())
        libraries=[builder.lib/('physim-'+name+'.lib') for name in ('project','batch','platform','language','zlib','core')]
        if not all(path.is_file() for path in libraries):raise RuntimeError('Complete the normal Release build first')
        inputs=[str(main_source) if name=='main' else 'app/'+name+'.c' for name in build.APP]+build.LINEBREAK+build.GRAPHEME
        counter=builder.executable('physim-picker-'+a.fault+'-counter',inputs,libraries,sdl=True,
            defines=('Z_PREFIX','PS_SOURCE_DIR="'+source.as_posix()+'"'))
        cases=[]
        for variant,app in (('counter',counter),('positive',normal)):
            command=[sys.executable,str(source/checker),'--app',str(app),'--work',str(proof/variant)]
            if a.fault=='places-padding':command+=['--root',str(source)]
            result=subprocess.run(command,cwd=source,capture_output=True,timeout=600,env=builder.env)
            log=proof/(variant+'.log');log.write_bytes(result.stdout+result.stderr)
            cases.append(dict(variant=variant,command=command,exit_code=result.returncode,
                binary_sha256=digest(app),log=str(log),log_sha256=digest(log)))
            if variant=='counter':assert result.returncode!=0 and expected in result.stderr,result.stderr[-3000:]
            else:assert result.returncode==0,result.stderr[-3000:]
        for path in normal_files:assert digest(path)==production[path.name],'Production runtime changed'
        runtime=archive(proof/'runtime.zip',[counter,counter.with_suffix('.pdb')])
        report=a.report.resolve() if a.report else proof/'report.json'
        with report.open('x',encoding='utf8') as stream:json.dump(dict(invocation=sys.argv,fault=a.fault,expected_failure=expected.decode('utf8'),
            source_archive=str(proof/'source.zip'),source_members=sources,production_members=production,runtime_members=runtime,
            cases=cases,production_binary_unchanged=True,all_archive_bytes_read_back=True),stream,indent=2)
    print('PICKER WINDOW COUNTEREXAMPLE: '+a.fault+' rejected; unchanged production passed')

if __name__=='__main__':main()
