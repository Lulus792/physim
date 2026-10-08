"""Eight real mixed runs plus explicit legacy and unsupported method metadata."""
from pathlib import Path
import struct
import subprocess
import sys
import zlib

runner, analyzer, c_model, phys_model, c_doc_model, phys_doc_model, c_analysis, phys_analysis, probe, work = sys.argv[1:]
work=Path(work);work.mkdir(parents=True)

def command(args,success=True):
    r=subprocess.run(list(map(str,args)),capture_output=True,timeout=90)
    assert (r.returncode==0)==success,(args,r.returncode,r.stdout,r.stderr)

def metadata(source,target,change):
    with source.open('rb') as f,target.open('xb') as out:
        out.write(f.read(16))
        while h:=f.read(12):
            kind,size,crc=struct.unpack('<III',h);data=f.read(size)
            assert len(data)==size and zlib.crc32(data)==crc
            if kind==1:data=change(data.decode()).encode()
            out.write(struct.pack('<III',kind,len(data),zlib.crc32(data)));out.write(data)

cases=[(c_model,0,False,'Euler'),(c_model,0,True,'Dormand-Prince 5(4) (adaptive)'),
       (phys_model,1,False,'symplectic Euler'),(phys_model,1,True,'Dormand-Prince 5(4) (adaptive)'),
       (c_model,3,False,'velocity Verlet'),(phys_model,3,True,'Dormand-Prince 5(4) (adaptive)'),
       (c_doc_model,2,False,'RK4'),(phys_doc_model,4,True,'Dormand-Prince 5(4) (adaptive)')]
runs=[];labels=[]
for i,(module,method,adaptive,label) in enumerate(cases):
    run=work/f'run-{i}.psrun';args=[runner,module,run,'--steps','10000','--dt','.005','--until','6','--param',f'integrator={method}']
    if adaptive:args+=['--adaptive','--min-dt','1e-8','--max-dt','.05']
    command(args);runs.append(run);labels.append(f'Run {i+1} / {label}')
legacy=work/'legacy-fixed.psrun'
metadata(runs[0],legacy,lambda s:'\n'.join(line for line in s.split('\n') if not line.startswith('step_mode=')))
unknown=work/'unknown-adaptive.psrun'
metadata(runs[1],unknown,lambda s:s.replace('adaptive_integrator=Dormand-Prince 5(4)','adaptive_integrator=unknown-method'))
missing=work/'missing-adaptive.psrun'
metadata(runs[1],missing,lambda s:'\n'.join(line for line in s.split('\n') if not line.startswith('adaptive_integrator=')))
for language,module in [('c',c_analysis),('phys',phys_analysis)]:
    prefix=work/f'mixed-{language}';command([analyzer,module,'--runs',prefix,*runs]);command([probe,str(prefix)+'.psreport',*labels])
    prefix=work/f'legacy-{language}';command([analyzer,module,legacy,prefix]);command([probe,str(prefix)+'.psreport','Run 1 / Euler'])
    for name,run in [('unknown',unknown),('missing',missing)]:
        prefix=work/f'{name}-{language}';command([analyzer,module,run,prefix],False)
        assert not Path(str(prefix)+'.psreport').exists()
print('Executed-method labels: eight mixed real runs, fixed legacy data, adaptive legacy RK45 and four rejected metadata reports passed')
