from pathlib import Path
from types import SimpleNamespace
import hashlib,os,sys,json
ROOT=Path('/mnt/d/Physim');sys.path.insert(0,str(ROOT/'tools'));import native_tests
names=['documentation_h','workbench_workflow','graphic_dialog']
snapshot=json.loads((ROOT/'build/native/rounded-sections-source4.json').read_text(encoding='utf-8'))
def source_guard():
 for p,h in snapshot['source'].items():assert hashlib.sha256((ROOT/p).read_bytes().replace(b'\r\n',b'\n')).hexdigest()==h,p
 for p,h in snapshot['fonts'].items():assert hashlib.sha256((ROOT/p).read_bytes()).hexdigest()==h,p
source_guard();cases=[c for c in native_tests.catalog() if c.name in names];assert len(cases)==3
for c in cases:
 assert c.integration and c.display and all(a.get('interpreter')=='python' or 'prebuilt' in a for a in c.integration['artifacts'].values()),c.name
class ReadOnlySDK:
 directory=ROOT/'build/native/Design-analysis-linux'
 bin=directory/'bin'
 args=SimpleNamespace(no_app=False,sanitizers=True)
 cc='/usr/bin/cc'
 env=os.environ.copy()
 def executable(self,*a,**k):raise RuntimeError('Read-only SDK cannot compile')
 def benchmark(self,*a,**k):raise RuntimeError('Read-only SDK cannot compile')
builder=ReadOnlySDK();before={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in builder.bin.iterdir() if p.is_file()}
print('PREBUILT ROUNDING: source4 SDK, original H/workbench/graphic cases, compile prohibited, isolated X display',flush=True)
try:native_tests.run_suite(builder,{},ROOT,names,display=True)
finally:
 source_guard();after={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in builder.bin.iterdir() if p.is_file()};assert before==after
 (ROOT/'build/native/rounded-sections-broad-binaries.json').write_text(json.dumps(dict(scope='Read-only source4 SDK; original tracked cases and expectations; all sources/fonts/binaries unchanged; independent X display',cases=names,before=before,after=after),indent=2)+'\n',encoding='utf-8')
