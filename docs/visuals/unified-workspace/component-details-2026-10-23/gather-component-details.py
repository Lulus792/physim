import datetime,gzip,hashlib,json,pathlib,re,shutil,subprocess
from PIL import Image
r=pathlib.Path('D:/Physim');out=r/'docs/visuals/unified-workspace/component-details-2026-10-23'
out.mkdir(exist_ok=False)
def sha(p,lf=False):
 b=p.read_bytes();return hashlib.sha256(b.replace(b'\r\n',b'\n') if lf else b).hexdigest()
def read(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def save(name,data):(out/name).write_bytes((json.dumps(data,ensure_ascii=False,indent=2)+'\n').encode('utf-8'))
def copy(p,name):
 shutil.copyfile(p,out/name)
 return {'artifact':name,'source':p.relative_to(r).as_posix(),'bytes':p.stat().st_size,'sha256':sha(p)}
def runpath(log):
 text=(r/'build/native'/log).read_text(encoding='utf-8',errors='replace')
 match=re.findall(r'Results: (.+)',text);assert match,log
 return pathlib.Path(match[-1].strip().replace('/mnt/d/Physim',str(r)))
sources={i:read(r/f'build/native/component-details-source{i}.json') for i in range(1,6)}
for name,h in sources[5]['source'].items():assert sha(r/name,True)==h,name
for name,h in sources[5]['foreign_preserved'].items():assert sha(r/name)==h,name
changed45={n for n,h in sources[4]['source'].items() if sources[5]['source'].get(n)!=h}
assert changed45=={'app/component_details_tests.inc','tests/check_component_details.py'},changed45
for i in sources:copy(r/f'build/native/component-details-source{i}.json',f'source{i}.json')
logs=list((r/'build/native').glob('component-details-*.log'))+list((r/'build/native').glob('component-tip-gdb-source3*.log'))
for p in logs:
 with gzip.open(out/(p.name+'.gz'),'wb') as f:f.write(p.read_bytes())
for p in (r/'build/native').glob('run-component-details-linux-*.sh'):
 raw=p.read_bytes()
 with gzip.open(out/(p.name+'.raw.gz'),'wb') as f:f.write(raw)
 (out/p.name).write_bytes(('\n'.join(raw.decode('utf-8-sig').splitlines())+'\n').encode('utf-8'))
for name in ('component-tip-source3b.gdb','component-tip-source3b.sh'):copy(r/'build/native'/name,name)
copy(pathlib.Path(__file__),pathlib.Path(__file__).name)
final=[];pictures=[];exports={}
runs=[('windows-debug',5,'component-details-windows-5.log','build/native/Design-analysis/bin/physim.exe'),
      ('windows-release',5,'component-details-windows-release-5.log','build/native/Release/bin/physim.exe'),
      ('linux-debug',5,'component-details-linux-5.log','build/native/Design-analysis-linux/bin/physim'),
      ('windows-release-regression',4,'component-details-windows-release-4.log',None),
      ('windows-debug-unit',4,'component-details-windows-unit4.log',None),
      ('windows-release-unit',5,'component-details-windows-release-unit5.log',None),
      ('linux-debug-unit',4,'component-details-linux-4.log',None)]
for label,version,log,binary in runs:
 base=runpath(log)
 if label=='linux-debug-unit':
  matches=re.findall(r'Results: (.+)',(r/'build/native'/log).read_text(encoding='utf-8'));base=pathlib.Path(matches[0].strip().replace('/mnt/d/Physim',str(r)))
 results=read(base/'results.json');assert results and all(x['status']=='passed' for x in results),label
 copy(base/'results.json',label+'-results.json')
 if binary:
  for x in results:
   for step in x['steps']:
    if step.get('target')=='physim':assert step['sha256']==sha(r/binary),(label,x['name'])
 final.append({'label':label,'source_version':version,'directory':base.relative_to(r).as_posix(),'binary_sha256':sha(r/binary) if binary else None,'groups':[{'name':x['name'],'status':x['status'],'seconds':x['seconds']} for x in results]})
 for x in results:
  group=base/x['name'];copy(group/'result.json',label+'-'+x['name']+'.json')
  if not x['name'].startswith('component_details_'):continue
  work=next(group.glob('files*'));short=x['name'].removeprefix('component_details_').replace('_','-')
  copy(work/'app-steps.json',label+'-'+short+'-steps.json');copy(work/'validation.json',label+'-'+short+'-checks.json')
  for image in sorted(work.rglob('*.png')):pictures.append(copy(image,label+'-'+short+'-'+image.name))
 if any(x['name']=='graphic_dialog' for x in results) and label in ('windows-release-regression','linux-debug'):
  # Canonical report exports must remain byte-identical to the previous
  # platform-specific Float milestone, not merely to another new run.
  oldlabel='windows-release' if label.startswith('windows') else 'linux-debug'
  previous=read(r/'docs/visuals/unified-workspace/float-ui-2026-10-23/validation.json')['report_exports_unchanged'][oldlabel]
  assert len(previous)==8,(oldlabel,len(previous))
  checks={}
  for name,olditem in previous.items():
   old=r/olditem['prior'];assert sha(old)==olditem['sha256'],name
   found=list((base/'graphic_dialog').rglob(name));assert len(found)==1,name
   assert found[0].read_bytes()==old.read_bytes(),(label,name)
   checks[name]={'prior':old.relative_to(r).as_posix(),'sha256':sha(found[0]),'byte_equal':True}
  exports[label]=checks
negative=[('windows-unit1','build/native/Design-analysis/test-results/run-ndnr_2jl'),
          ('linux-unit1','build/native/Design-analysis-linux/test-results/run-irrbjer3'),
          ('windows2','build/native/Design-analysis/test-results/run-kffpbmic'),
          ('linux2','build/native/Design-analysis-linux/test-results/run-03yly06o'),
          ('windows3','build/native/Design-analysis/test-results/run-sjyn5ksb'),
          ('linux3','build/native/Design-analysis-linux/test-results/run-s2_dy6o_')]
for label,base in negative:
 p=r/base/'results.json'
 if p.exists():copy(p,'negative-'+label+'-results.json')
for label,base,name in [('disabled-by-dock','build/native/Design-analysis/test-results/run-kffpbmic','tools-normal.png'),('tip-scroll-zero','build/native/Design-analysis/test-results/run-sjyn5ksb','tip-maximum.png'),('transparent-probe','build/native/Design-analysis/test-results/run-yqry_hlk','tip-bottom.png')]:
 p=next((r/base/'component_details_dark').rglob(name));pictures.append(copy(p,'negative-'+label+'.png'))
reference=['docs/visuals/analysis-workspace/analysis.css','docs/visuals/analysis-workspace/graphic-export.css','docs/visuals/analysis-workspace/index.html','docs/visuals/unified-workspace/bridge.js']
keep={'tools-normal.png','tools-hover.png','tools-focus.png','tools-report.png','tools-disabled.png','tools-wrapped.png','tip-real.png','tip-maximum.png','tip-bottom.png','tip-dismissed.png'}
selected=[]
for item in pictures:
 p=out/item['artifact']
 if not p.name.startswith('negative-') and (p.name.startswith('windows-release-regression-') or not any(p.name.endswith('-'+name) for name in keep)):
  assert p.resolve().parent==out.resolve() and p.suffix=='.png';p.unlink();continue
 original=Image.open(r/item['source']);assert original.mode=='RGB'
 assert sha(p)==sha(r/item['source']) and Image.open(p).tobytes()==original.tobytes(),p
 item.update(rgb_sha256=hashlib.sha256(original.tobytes()).hexdigest(),encoding='Original native PNG bytes; complete RGB equality verified');selected.append(item)
pictures=selected
save('validation.json',{'date':datetime.datetime.now().astimezone().isoformat(),'base':sources[5]['base'],'final_source_version':5,'source_normalization':'CRLF -> LF','foreign_preserved':sources[5]['foreign_preserved'],'source4_to5_only_test_changes':sorted(changed45),'final_runs':final,'pictures':pictures,'canonical_exports':exports,'reference_sha256':{n:sha(r/n) for n in reference},'limits':['Scoped native component proof; full UI acceptance remains open','180px component and 4095-byte layout stresses are not window-minimum or filesystem-long-path proofs','No physical DPI/monitor switch, macOS, external screenreader or direct detached tooltip acceptance','Preserved foreign accessibility work is present in tested binaries and excluded from this commit','Source4 functional/pixel successes precede the visual rejection of its transparent stress fixture']})
for p in sorted(out.iterdir()):
 if p.suffix in ('.json','.sh','.py','.gdb') and b'\r' in p.read_bytes():
  raw=p.read_bytes()
  with gzip.open(out/(p.name+'.raw.gz'),'wb') as f:f.write(raw)
  p.write_bytes(raw.replace(b'\r\n',b'\n'))
save('artifacts.json',{p.name:{'bytes':p.stat().st_size,'sha256':sha(p)} for p in sorted(out.iterdir()) if p.is_file() and p.name!='artifacts.json'})
print('Runs',[(x['label'],len(x['groups'])) for x in final]);print('Exports',[(k,len(v)) for k,v in exports.items()]);print('Archive',len(list(out.iterdir())),sum(p.stat().st_size for p in out.iterdir()))
