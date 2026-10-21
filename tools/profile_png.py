"""Paired same-process DEFLATE 6/1 profile on actual native PNG input pixels.

Uses existing sanitized or normal native unit libraries. Times include encoding
and writing/closing a new file, without claiming durable storage or UI latency.
"""
import argparse,hashlib,json,pathlib,re,statistics,struct,subprocess,sys
from types import SimpleNamespace
import build
sys.dont_write_bytecode=True
sys.path.insert(0,str(build.ROOT/'tests'))
from check_graphic_raster import png

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--input',type=pathlib.Path,action='append',required=True);p.add_argument('--build-dir',type=pathlib.Path,required=True);p.add_argument('--work',type=pathlib.Path,required=True);p.add_argument('--config',choices=['Debug','Release'],default='Debug');p.add_argument('--sanitizers',action='store_true');a=p.parse_args()
    a.work.mkdir(parents=True,exist_ok=False)
    args=SimpleNamespace(build_dir=a.build_dir,compiler=None,no_app=True,config=a.config,jobs=4,sanitizers=a.sanitizers,benchmarks=False)
    builder=build.Builder(args,build.compiler_environment());ext='.lib' if builder.msvc else '.a'
    libraries=[builder.lib/('physim-'+name+ext if builder.msvc else 'libphysim-'+name+ext) for name in ('zlib','platform','core')]
    assert all(x.is_file() for x in libraries)
    source_hash=hashlib.sha256((build.ROOT/'app/png.c').read_bytes()).hexdigest()
    program=builder.executable('physim-png-level-profile',['tests/benchmark_png_levels.c'],libraries,
        defines=('Z_PREFIX','PS_PNG_PROFILE_SOURCE_'+source_hash[:16]))
    reports=[]
    for index,path in enumerate(a.input):
        width,height,rgb,digest=png(path);raw=a.work/f'input-{index}.rgb';raw.write_bytes(b'PSRGB1\0\0'+struct.pack('<II',width,height)+rgb)
        prefix=a.work/f'output-{index}';command=[str(program),str(raw.resolve()),str(prefix.resolve())]
        r=subprocess.run(command,env=builder.env,capture_output=True,timeout=180)
        report={'input':str(path),'input_sha256':digest,'size':[width,height],'rgb_sha256':hashlib.sha256(rgb).hexdigest(),'command':command,'exit_code':r.returncode,'stdout':r.stdout.decode('utf-8'),'stderr':r.stderr.decode('utf-8')};reports.append(report)
        assert r.returncode==0,report
        samples=re.findall(r'PNG LEVEL: pair=(\d+) level=(\d+) seconds=([\d.]+) bytes=(\d+)',report['stdout']);assert len(samples)==6
        report['samples']=[]
        for pair,level,seconds,length in samples:
            output=pathlib.Path(str(prefix)+f'-level{level}-{pair}.png');w,h,actual,sha=png(output)
            assert (w,h)==(width,height) and actual==rgb and output.stat().st_size==int(length)
            report['samples'].append({'pair':int(pair),'level':int(level),'seconds':float(seconds),'bytes':int(length),'path':str(output),'sha256':sha,'rgb_identical':True})
        report['medians']={level:statistics.median(x['seconds'] for x in report['samples'] if x['level']==int(level)) for level in ('1','6')}
        report['level1_over_level6']=report['medians']['1']/report['medians']['6']
        print(path.name,report['medians'],'ratio',report['level1_over_level6'],flush=True)
    (a.work/'profile.json').write_text(json.dumps({'scope':__doc__,'config':a.config,'sanitizers':a.sanitizers,'png_source_sha256':source_hash,'program_sha256':hashlib.sha256(program.read_bytes()).hexdigest(),'reports':reports},ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
if __name__=='__main__':main()
