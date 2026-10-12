"""Actual CLI identity assignment preserves project artifacts and archive lineage."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import struct
import subprocess
import zlib
p=argparse.ArgumentParser()
for name in ('builder','sdk','compiler','runner','work'):p.add_argument('--'+name,type=Path,required=True)
p.add_argument('--cc');a=p.parse_args();a.work.mkdir(parents=True,exist_ok=True)
def command(args):
    r=subprocess.run(list(map(str,args)),capture_output=True,timeout=180)
    assert r.returncode==0,(args,r.stdout,r.stderr)
    return r.stdout
checks=[]
for lang in ('c','phys'):
    root=a.work/('Legacy identity '+lang+' α');root.mkdir();(root/'runs').mkdir();(root/'build').mkdir()
    source='main.c' if lang=='c' else 'main.phys';analysis='analysis.c' if lang=='c' else 'analysis.phys'
    shutil.copy2(a.sdk/('examples/pendulum/main.c' if lang=='c' else 'examples/language/pendulum.phys'),root/source)
    shutil.copy2(a.sdk/('examples/pendulum/analysis.c' if lang=='c' else 'examples/language/analysis.phys'),root/analysis)
    (root/'runs/retained.psrun').write_bytes(b'original run bytes')
    (root/'build/retained.bin').write_bytes(b'original build bytes')
    manifest=root/'physim.project';original=f'physim_project=1\r\n# preserved α\r\nexperiment={source}\r\nanalysis={analysis}\r\nprofile=Release\r\nextension.keep=owner'.encode();manifest.write_bytes(original)
    protected={q.relative_to(root).as_posix():hashlib.sha256(q.read_bytes()).hexdigest() for q in root.rglob('*') if q.is_file() and q!=manifest}
    stdout=command([a.builder,'--assign-project-ids','--project',root]);assert b'assigned;' in stdout
    raw=manifest.read_bytes();assert raw.startswith(original) and (root/'physim.project.bak').read_bytes()==original
    fields=dict(l.split('=',1) for l in raw.decode().splitlines() if '=' in l)
    project,experiment=fields['project.id'],fields['experiment.id'];assert re.fullmatch('[0-9a-f]{32}',project) and re.fullmatch('[0-9a-f]{32}',experiment)
    for name,digest in protected.items():assert hashlib.sha256((root/name).read_bytes()).hexdigest()==digest
    stdout=command([a.builder,'--project',root,'--assign-project-ids']);assert b'already assigned; no files written' in stdout
    assert manifest.read_bytes()==raw and (root/'physim.project.bak').read_bytes()==original
    build=[a.builder,'--project',root,'--sdk',a.sdk,'--output',root/'build/Release','--physimc',a.compiler,'--profile','Release']
    if a.cc:build+=['--cc',a.cc]
    command(build)
    archive=root/'runs/identity.psrun';module=root/'build/Release'/('experiment.dll' if os.name=='nt' else 'experiment.so')
    command([a.runner,module,archive,'--steps','3','--project-id',project,'--experiment-id',experiment])
    data=archive.read_bytes();kind,n,crc=struct.unpack_from('<III',data,16);payload=data[28:28+n];assert kind==1 and zlib.crc32(payload)==crc
    lines=payload.decode().splitlines();assert lines.count('project_id='+project)==1 and lines.count('experiment_id='+experiment)==1
    assert manifest.read_bytes()==raw and (root/'physim.project.bak').read_bytes()==original
    for name,digest in protected.items():assert hashlib.sha256((root/name).read_bytes()).hexdigest()==digest
    checks.append(dict(language=lang,project_id=project,experiment_id=experiment))
(a.work/'results.json').write_text(json.dumps(checks,indent=2)+'\n')
print('Legacy identities: actual C/Physim assignment/no-op, preserved source/run/build bytes, subsequent builds and matching archive IDs passed')
