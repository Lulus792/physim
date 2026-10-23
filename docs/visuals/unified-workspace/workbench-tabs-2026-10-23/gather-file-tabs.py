import gzip,hashlib,json,pathlib,re,shutil
from PIL import Image
r=pathlib.Path('D:/Physim');out=r/'docs/visuals/unified-workspace/workbench-tabs-2026-10-23'
out.mkdir(exist_ok=False)
def sha(p,lf=False):
 b=p.read_bytes();return hashlib.sha256(b.replace(b'\r\n',b'\n') if lf else b).hexdigest()
def read(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def save(name,data):(out/name).write_bytes((json.dumps(data,ensure_ascii=False,indent=2)+'\n').encode('utf-8'))
def cp(p,name):
 raw=p.read_bytes();(out/name).write_bytes(raw);return {'artifact':name,'source':str(p.relative_to(r)).replace('\\','/'),'sha256':sha(p),'bytes':len(raw)}
def gz(p,name):
 with gzip.open(out/name,'wb') as stream:stream.write(p.read_bytes())
def runpath(log):
 text=(r/'build/native'/log).read_text(encoding='utf-8',errors='replace');matches=re.findall(r'Results: (.+)',text);assert matches,log
 return pathlib.Path(matches[-1].strip().replace('/mnt/d/Physim',str(r)))
def picture(p,name):
 if p.suffix=='.png':return cp(p,name)
 image=Image.open(p).convert('RGB');image.save(out/name)
 actual=Image.open(out/name).convert('RGB');assert image.size==actual.size and image.tobytes()==actual.tobytes()
 return dict(artifact=name,source=str(p.relative_to(r)).replace('\\','/'),source_sha256=sha(p),sha256=sha(out/name),conversion='RGB bytes and dimensions equal')
# Run mapping and final evidence are appended after the frozen jobs terminate.
sources={i:read(r/f'build/native/file-tabs-source{i}.json') for i in range(1,7)}
for name,h in sources[6]['source'].items():assert sha(r/name,True)==h,name
for name,h in sources[6]['fonts'].items():assert sha(r/name)==h,name
for name,h in sources[6]['foreign_preserved'].items():assert sha(r/name)==h,name
for i in sources:save(f'source{i}.json',sources[i])
for p in (r/'build/native').glob('file-tabs-*.log'):gz(p,p.name+'.gz')
for name in ('file-tabs-font-fetch.json','file-tabs-sdk4-proof.json','file-tabs-compact-counterexample.json'):save(name,read(r/'build/native'/name))
for p in list((r/'build/native').glob('run-file-tabs-*.sh'))+[r/'build/native/probe-file-tabs-source4-ext4.py']:
 raw=p.read_bytes();gz(p,p.name+'.raw.gz');(out/p.name).write_bytes(('\n'.join(raw.decode('utf-8-sig').splitlines())+'\n').encode('utf-8'))
for name in ('tab-panel-source2.gdb','tab-panel-source2.sh','tab-panel-gdb-source2.log'):
 p=r/'build/native'/name
 if p.is_file():gz(p,name+'.gz')
runs=[('windows-debug-source1',1,'file-tabs-windows-1.log'),('linux-source1',1,'file-tabs-linux-1.log'),('windows-debug-source2',2,'file-tabs-windows-2.log'),('windows-release-source2',2,'file-tabs-windows-release-2.log'),('linux-source2',2,'file-tabs-linux-2.log'),('linux-context-source2',2,'file-tabs-linux-context2.log'),('windows-panel-source3',3,'file-tabs-windows-panel3.log'),('windows-release-source4',4,'file-tabs-windows-4.log'),('windows-debug-source4',4,'file-tabs-windows-debug4.log'),('linux-source4',4,'file-tabs-linux-4.log'),('windows-release-source5',5,'file-tabs-windows-5.log'),('linux-source5',5,'file-tabs-linux-5.log'),('windows-release-source6',6,'file-tabs-windows-6.log'),('linux-source6',6,'file-tabs-linux-6.log'),('windows-graphic-source6',6,'file-tabs-windows-graphic6-correct.log'),('linux-graphic-source6',6,'file-tabs-linux-graphic6.log')]
records=[];bases={};pictures=[]
for label,version,log in runs:
 base=runpath(log);bases[label]=base;results=read(base/'results.json');save(label+'-results.json',results)
 binary_hashes={step['sha256'] for result in results for step in result.get('steps',[]) if step.get('target')=='physim'}
 if version==6:
  binary=r/('build/native/Release/bin/physim.exe' if label.startswith('windows') else 'build/native/Design-analysis-linux/bin/physim')
  assert binary_hashes=={sha(binary)},(label,binary_hashes)
  assert all(x['status']=='passed' for x in results),label
 for result in results:
  for p in (base/result['name']).rglob('*.json'):
   if p.name in ('app-steps.json','validation.json','result.json','math-inventory.json'):
    name=label+'--'+str(p.relative_to(base)).replace('\\','__').replace('/','__').replace(' ','_')+'.gz';gz(p,name)
 records.append(dict(label=label,source_version=version,directory=str(base.relative_to(r)).replace('\\','/'),binary_sha256=sorted(binary_hashes),groups=[{k:x[k] for k in ('name','status','seconds')} for x in results]))
for case in ('documentation_math','file_tabs_workflow'):
 base=r/'build/native/Design-analysis-linux/test-results/file-tabs-source4-ext4'/case
 result=read(base/'result.json');assert result['status']=='passed';save('linux-source4-ext4-'+case+'.json',result)
 for p in (base/'files ä').rglob('app-steps.json'):gz(p,'linux-source4-ext4-'+case+'-app-steps.json.gz')
 if case=='documentation_math':save('linux-source4-math-inventory.json',read(base/'files ä/math-inventory.json'))
 records.append(dict(label='linux-source4-ext4-'+case,source_version=4,status=result['status'],seconds=result['seconds'],binary_sha256=result['binary']['sha256']))
# The tab images remain the exact native PNG bytes.
for label in ('windows-release-source6','linux-source6'):
 base=bases[label]/'file_tabs_design'/'files ä';short='windows' if label.startswith('windows') else 'linux'
 selected=['dark-normal','light-normal','large-normal','contrast-normal','long-end','dirty']
 if short=='windows':selected+=['dark-hover','dark-focus','dark-close-focus','long-start']
 for state in selected:pictures.append(picture(base/'system'/('tabs-'+state+'.png'),short+'-'+state+'.png'))
 for state in ('dark-normal','large-normal'):pictures.append(picture(base/'bundled'/('tabs-'+state+'.png'),short+'-bundled-'+state+'.png'))
 for variant in ('c-contrast-large-narrow','phys-light-large-narrow'):
  folder=bases[label]/'inspector_workflow'/'files ä'/'workflow ä'/variant
  for state in ('start','end'):
   file=folder/('panel-compact-log-'+state+'.png')
   if file.is_file():pictures.append(picture(file,short+'-'+variant+'-log-'+state+'.png'))
  if variant.startswith('c-'):pictures.append(picture(folder/'panels-reattached.bmp',short+'-panels-returned.png'))
for label,relative,name in [('windows-debug-source1','file_tabs_design/files ä/system/tabs-large-normal.png','negative-source1-large.png'),('windows-panel-source3','inspector_workflow/files ä/workflow ä/c-contrast-large-narrow/panel-before-measure-pull.png','negative-source3-measure.png'),('windows-release-source5','inspector_workflow/files ä/workflow ä/c-contrast-large-narrow/panel-compact-log-end.png','negative-source5-log-text.png')]:
 pictures.append(picture(bases[label]/relative,name))
save('pictures.json',pictures)
prior=read(r/'docs/visuals/unified-workspace/component-details-2026-10-23/validation.json')['canonical_exports'];exports={}
for label,prior_label in [('windows-graphic-source6','windows-release-regression'),('linux-graphic-source6','linux-debug')]:
 exports[label]={}
 for name,old in prior[prior_label].items():
  path=bases[label]/'graphic_dialog'/'files ä'/'dialog ä'/name;assert path.read_bytes()==(r/old['prior']).read_bytes()
  exports[label][name]=dict(source=str(path.relative_to(r)).replace('\\','/'),prior=old['prior'],sha256=sha(path),byte_equal=True)
save('exports.json',exports)
save('validation.json',dict(base=sources[6]['base'],final_source=6,runs=records,limits=['Only Windows original first-pill geometry is compared to browser PNGs; no full glyph/raster identity claim','System and bundled text roles and actual focus tasks are native; no physical monitor or external screenreader proof','Source4 ext4 checks keep original 75/65-second outer and 45/15-second inner deadlines; only working data moved','Source5 old checker passed despite blank compact diagnostic; independent final raster check rejects that image','SDK source4 validates the new font/license and installed bundled startup; it is not the final source6 application','Tested native binaries include preserved preexisting local accessibility work; its 12 raw hashes are recorded and those changes are excluded from this commit'],canonical_exports='exports.json',sdk_proof='file-tabs-sdk4-proof.json',raster_counterexample='file-tabs-compact-counterexample.json'))
cp(pathlib.Path(__file__),'gather-file-tabs.py')
cp(r/'build/native/verify-file-tabs-negative.py','verify-negative.py')
# JSON/Markdown/scripts are LF in Git; original logs/step files are preserved as gzip.
artifacts={p.name:dict(bytes=p.stat().st_size,sha256=sha(p)) for p in sorted(out.iterdir()) if p.is_file()}
save('artifacts.json',artifacts)
print(json.dumps(dict(directory=str(out),files=len(artifacts)+1,pictures=len(pictures),bytes=sum(p.stat().st_size for p in out.iterdir() if p.is_file())),ensure_ascii=False))
