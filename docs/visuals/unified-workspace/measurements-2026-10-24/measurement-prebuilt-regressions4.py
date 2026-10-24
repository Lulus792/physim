from pathlib import Path
from types import SimpleNamespace
import hashlib,os,sys,json
ROOT=Path('/mnt/d/Physim');sys.path.insert(0,str(ROOT/'tools'));import native_tests
names=['docking_workflow','inspector_workflow','channel_units_workflow','laboratory_workflow','scene_frame_design']
snapshot=json.loads((ROOT/'build/native/measurement-source3.json').read_text(encoding='utf-8'))
def source_guard():
 for p,h in snapshot['source'].items():assert hashlib.sha256((ROOT/p).read_bytes().replace(b'\r\n',b'\n')).hexdigest()==h,p
source_guard()
cases=[c for c in native_tests.catalog() if c.name in names];assert len(cases)==5
for c in cases:
 assert c.integration and c.display
 assert all(a.get('interpreter')=='python' or 'prebuilt' in a for a in c.integration['artifacts'].values()),c.name
class ReadOnlySDK:
 directory=ROOT/'build/native/Design-analysis-linux'
 bin=directory/'bin'
 args=SimpleNamespace(no_app=False,sanitizers=True)
 cc='/usr/bin/cc'
 env=os.environ.copy()
 def executable(self,*a,**k):raise RuntimeError('Read-only SDK cannot compile')
 def benchmark(self,*a,**k):raise RuntimeError('Read-only SDK cannot compile')
builder=ReadOnlySDK();before={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in builder.bin.iterdir() if p.is_file()}
print('PREBUILT MEASUREMENT4: unchanged source3 SDK; tracked cases and unchanged expectations; compile prohibited',flush=True)
try:native_tests.run_suite(builder,{},ROOT,names,display=True)
finally:
 source_guard();after={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in builder.bin.iterdir() if p.is_file()};assert before==after
 (ROOT/'build/native/measurement-prebuilt4-binaries.json').write_text(json.dumps(dict(scope='Read-only existing source3 SDK, five original native cases, unchanged sources and binaries; independent X display',before=before,after=after),indent=2)+'\n',encoding='utf-8')
