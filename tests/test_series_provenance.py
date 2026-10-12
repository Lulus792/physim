"""Independent wire reference for actual single/seeded/sweep C/Physim runs."""
from pathlib import Path
import struct,subprocess,sys,zlib
runner,batch,c_model,phys_model,forged,work=sys.argv[1:];work=Path(work).resolve();work.mkdir(parents=True,exist_ok=True)
def command(argv,code=0):
 p=subprocess.run(list(map(str,argv)),capture_output=True,timeout=60);assert p.returncode==code,(argv,p.returncode,p.stdout,p.stderr)
def metadata(path):
 b=path.read_bytes();assert b[:16]==b'PSRUN17\n'+struct.pack('<II',1,0x01020304);kind,n,crc=struct.unpack_from('<III',b,16);p=b[28:28+n];assert kind==1 and zlib.crc32(p)==crc
 lines=[x.split('=',1) for x in p.decode().splitlines() if '=' in x];assert len({k for k,v in lines})==len(lines);return dict(lines)
for language,model in (('c',c_model),('phys',phys_model)):
 single=work/f'{language}-single.psrun';command([runner,model,single,'--steps','2']);assert metadata(single)['series.kind']=='single'
 for mode in ('seeded','linear'):
  directory=work/f'{language}-{mode}';args=[batch,runner,model,directory,'angle','3','10','.005','42','--workers','2']
  if mode=='linear':args+=['--sweep','length=0.5:2.5']
  command(args)
  for i in range(1,4):
   m=metadata(directory/f'run-{i:04d}.psrun');assert m['series.kind']==mode and m['series.index']==str(i) and m['series.count']=='3' and m['series.base_seed']=='42' and m['seed']==str(41+i) and m['series.seed_policy']=='base+index-1'
   if mode=='linear':assert m['series.parameter']=='length' and float(m['series.value'])==float(m['parameter.length'])==.5+i-1 and m['parameter_unit.length']=='m'
 for index,info in enumerate(('seeded,42,2,3','seeded,18446744073709551615,1,2','linear,42,1,3,length,0.5,2.5','seeded,42,1,1001','seeded,42,1,3\nseries.kind=linear')):
  path=work/f'{language}-invalid-{index}.psrun';command([runner,model,path,'--steps','1','--seed','42','--series-info',info],5 if index==2 else 2);assert not path.exists()
 path=work/f'{language}-duplicate.psrun';command([runner,model,path,'--series-info','seeded,42,1,3','--series-info','seeded,42,1,3'],2);assert not path.exists()
forged_path=work/'forged.psrun';command([runner,forged,forged_path,'--steps','1'],5);assert not forged_path.exists()
print('Series provenance: fourteen real C/Physim archives, single/seeded/linear modes, exact seeds/indexes/SI study values, duplicate/forged/mismatched/overflow no-file rejection passed')
