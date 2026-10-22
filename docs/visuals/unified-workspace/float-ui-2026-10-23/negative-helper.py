import sys,pathlib,types,subprocess,json,hashlib
root=pathlib.Path('D:/Physim');sys.path.insert(0,str(root/'tools'));import build
out=root/'build/native/float-ui-negative';out.mkdir(exist_ok=True);src=out/'mutant';src.mkdir(exist_ok=True)
for name in ('ui.h','ui_backend.c'):(src/name).write_bytes((root/'app'/name).read_bytes())
s=(root/'third_party/nuklear.h').read_text();start=s.index('NK_API void\nnk_stroke_line(');end=s.index('NK_API void',start+12);body=s[start:end];assert body.count('(nk_command_coord)')==4 and body.count('(nk_command_size)')==1
body=body.replace('(nk_command_coord)','(short)').replace('(nk_command_size)','(unsigned short)');s=s[:start]+body+s[end:];(src/'nuklear.h').write_text(s)
args=types.SimpleNamespace(build_dir=out,compiler=None,no_app=False,sdl=None,sanitizers=True,config='Debug',jobs=4,rebuild=False)
b=build.Builder(args,build.compiler_environment())
libraries=[root/'build/native/Design-analysis/lib'/f'physim-{name}.lib' for name in ('platform','zlib','core')]
program=b.executable('physim-test-ui-subpixel-negative',['tests/test_ui_subpixel_gpu.c',str(src/'ui_backend.c'),'app/ui_geometry.c','app/graphics.c','app/png.c'],libraries,sdl=True,defines=('Z_PREFIX',))
result=subprocess.run([str(program),str(out)],env=b.env,cwd=out,capture_output=True,text=True)
print(result.stdout);print(result.stderr);print('NEGATIVE_EXIT',result.returncode)
assert result.returncode==1 and 'Subpixel GPU' in result.stderr
(out/'result.json').write_text(json.dumps({'mutation':'Only nk_stroke_line emission casts restored to short/ushort; command ABI remains Float','exit_code':result.returncode,'stdout':result.stdout,'stderr':result.stderr,'mutant_header_sha256':hashlib.sha256((src/'nuklear.h').read_bytes()).hexdigest()},indent=2)+'\n')
