from pathlib import Path
from types import SimpleNamespace
import hashlib,os,sys,json
ROOT=Path('/mnt/d/Physim');sys.path.insert(0,str(ROOT/'tools'));import native_tests
names=['timeline_workflow','camera_plane_16','camera_plane_22','project_display_16','project_display_22','scene_visibility_16','scene_visibility_22','laboratory_workflow','tree_design','camera_design']
cases=[c for c in native_tests.catalog() if c.name in names];assert len(cases)==10
for c in cases:
 assert c.integration and c.display
 assert all(a.get('interpreter')=='python' or 'prebuilt' in a for a in c.integration['artifacts'].values()),c.name
class ReadOnlySDK:
 directory=ROOT/'build/native/Design-analysis-linux'
 bin=directory/'bin'
 args=SimpleNamespace(no_app=False,sanitizers=True)
 cc='/usr/bin/cc'
 env=os.environ.copy()
 def executable(self,*a,**k):raise RuntimeError('Read-only SDK provider cannot compile')
 def benchmark(self,*a,**k):raise RuntimeError('Read-only SDK provider cannot compile')
builder=ReadOnlySDK();before={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in builder.bin.iterdir() if p.is_file()}
print('PREBUILT SOURCE6: unchanged source5 SDK, tracked catalog and unchanged expectations; compile methods prohibited',flush=True)
try:native_tests.run_suite(builder,{},ROOT,names,display=True)
finally:
 after={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in builder.bin.iterdir() if p.is_file()};assert before==after,'Prebuilt SDK changed during runtime checks'
 (ROOT/'build/native/scene-prebuilt6-binaries.json').write_text(json.dumps(dict(scope='Read-only SDK provider; ten original tracked native cases and expectations; independent X display; binary directory unchanged',before=before,after=after),indent=2)+'\n',encoding='utf-8')
