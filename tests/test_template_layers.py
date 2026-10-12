"""Read real template archives independently; verify layer states and numerical non-regression."""
from pathlib import Path
import math
import argparse
import struct
import subprocess
import sys
import zlib
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('runner');parser.add_argument('work',type=Path);parser.add_argument('modules',nargs=16,type=Path)
parser.add_argument('--baseline-sdk',type=Path)
args=parser.parse_args();runner=args.runner;work=args.work;work.mkdir(parents=True,exist_ok=True);modules=args.modules
language_names=('pendulum','projectile_drag','collision','box_contacts','spring','uncertain_projectile','box_collision','buoyancy')
keys=('geometry','laws','medium','losses','numerics','measurement','sampling')
cases=[('pendulum',({'integrator':2}, {'medium':'omitted','losses':'omitted','measurement':'ideal','sampling':'omitted'}),({'integrator':2,'airDensity':1.225,'sensorNoise':.02},{'medium':'active','losses':'active','measurement':'active','sampling':'active'})),
 ('projectile',({}, {'losses':'active'}),({'dragCoefficient':0},{'losses':'omitted'})),
 ('collision',({}, {'medium':'omitted','losses':'ideal'}),({'mediumMode':1,'friction':.3},{'medium':'active','losses':'active'}),({'mediumMode':3,'customDensity':0,'customViscosity':.001,'dragMode':1},{'medium':'active','losses':'active'}),({'mediumMode':3,'customDensity':0,'customViscosity':.001,'dragMode':2},{'medium':'active','losses':'ideal'})),
 ('box_floor',({}, {'medium':'omitted','losses':'active'})),
 ('spring',({}, {'losses':'active'}),({'damping':0},{'losses':'omitted'})),
 ('uncertain_projectile',({}, {'measurement':'active','sampling':'active'})),
 ('box_collision',({}, {'losses':'ideal'}),({'friction':.3},{'losses':'active'})),
 ('buoyancy',({}, {'medium':'ideal','losses':'active'}))]
def archive(path):
 b=path.read_bytes();assert b[:16]==b'PSRUN17\n'+struct.pack('<II',1,0x01020304)
 at=16;meta=None;schema=None;rows=[];footer=False
 while at<len(b):
  kind,n,crc=struct.unpack_from('<III',b,at);p=b[at+12:at+12+n];assert n<=8192 and len(p)==n and zlib.crc32(p)==crc
  if kind==1:
   pairs=[x.split('=',1) for x in p.decode().splitlines() if '=' in x];assert len({k for k,v in pairs})==len(pairs);meta=dict(pairs)
  if kind==2:schema=p
  if kind==3:rows.append(struct.unpack('<'+'d'*(n//8),p))
  if kind==4:assert struct.unpack('<Q',p)[0]==len(rows);footer=True
  at+=12+n
 assert at==len(b) and footer and len(rows)==21
 return meta,schema,rows
count=0
for index,(name,*variants) in enumerate(cases):
 for variant,(parameters,expected) in enumerate(variants):
  results=[]
  for language,module in zip(('c','phys'),modules[2*index:2*index+2]):
   path=work/f'{name}-{variant}-{language}.psrun';cmd=[runner,str(module),str(path),'--steps','20','--dt','.005','--seed','42']
   for k,v in parameters.items():cmd+=['--param',f'{k}={v}']
   p=subprocess.run(cmd,capture_output=True,timeout=15);assert p.returncode==0,(name,parameters,p.stdout,p.stderr)
   meta,schema,rows=archive(path);layer={k:meta['layer.'+k] for k in keys}
   for k,value in layer.items():assert value.split('|',1)[0] in ('active','ideal','omitted') and value.split('|',1)[1]
   for k,value in expected.items():assert layer[k].split('|',1)[0]==value,(name,parameters,k,layer[k])
   if name!='uncertain_projectile' and name!='pendulum':assert layer['sampling'].startswith('omitted|') and 'external' in layer['sampling']
   if args.baseline_sdk:
    suffix='.dll' if sys.platform=='win32' else '.so'
    baseline=args.baseline_sdk/'bin'/((name if language=='c' else 'language-'+language_names[index])+suffix)
    previous=work/f'{name}-{variant}-{language}-before.psrun'
    command=[str(args.baseline_sdk/'bin'/('physim-runner.exe' if sys.platform=='win32' else 'physim-runner')),str(baseline),str(previous),*cmd[3:]]
    p=subprocess.run(command,capture_output=True,timeout=15);assert p.returncode==0,(command,p.stderr)
    _,old_schema,old_rows=archive(previous);assert schema==old_schema and rows==old_rows,(name,language,'changed scientific data')
   results.append((layer,rows));count+=1
  assert results[0][0]==results[1][0],(name,results[0][0],results[1][0])
  for a,b in zip(results[0][1],results[1][1]):assert len(a)==len(b) and all(math.isclose(x,y,rel_tol=2e-10,abs_tol=2e-10) for x,y in zip(a,b)),(name,a,b)
print(f'Template layers: {count} complete CRC-verified C/Physim archives, all eight pairs/seven declarations, zero losses and viscosity-only custom media passed')
