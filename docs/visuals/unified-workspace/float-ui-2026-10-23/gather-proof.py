import pathlib,json,hashlib,shutil,re,gzip,datetime
from PIL import Image
r=pathlib.Path('D:/Physim');out=r/'docs/visuals/unified-workspace/float-ui-2026-10-23'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def copy(p,name):shutil.copy2(p,out/name);return {'artifact':name,'source':str(p.relative_to(r)).replace('\\','/'),'bytes':p.stat().st_size,'sha256':sha(p)}
def jsonwrite(name,m):(out/name).write_text(json.dumps(m,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
def runpath(log):
 s=(r/'build/native'/log).read_text(encoding='utf-8',errors='replace');match=re.findall(r'Results: (.+)',s);assert match,log
 p=match[-1].strip().replace('/mnt/d/Physim',str(r));return pathlib.Path(p)
final=[];images=[];exports={}
for label,log,binary in [('windows-debug','float-ui-windows-final4.log','build/native/Design-analysis/bin/physim.exe'),('windows-release','float-ui-windows-release-final4.log','build/native/Release/bin/physim.exe'),('linux-debug','float-ui-linux-final4.log','build/native/Design-analysis-linux/bin/physim')]:
 p=runpath(log);results=json.loads((p/'results.json').read_text());assert results and all(x['status']=='passed' for x in results),(label,results)
 copy(p/'results.json',label+'-final4.json')
 final.append({'label':label,'source_version':4,'directory':str(p.relative_to(r)).replace('\\','/'),'binary_sha256':sha(r/binary),'groups':[{'name':x['name'],'status':x['status'],'seconds':x['seconds']} for x in results]})
 group=p/'graphic_dialog';copy(group/'result.json',label+'-dialog4.json')
 step=next(group.rglob('app-steps.json'));copy(step,label+'-dialog4-steps.json')
 folder=next(group.rglob('graphic-dialog-reference-light.png')).parent
 for theme in ('dark','light'):images.append(copy(folder/('graphic-dialog-reference-'+theme+'.png'),label+'-export-'+theme+'-v4.png'))
 for name in ('graphic-dialog-480-footer','graphic-dialog-680-default','graphic-dialog-800-centered'):images.append(copy(folder/(name+'.png'),label+'-'+name+'-v4.png'))
 baseline=next(x for x in json.loads((r/'docs/visuals/unified-workspace/analysis-comparison-2026-10-22/validation.json').read_text())['final_runs'] if x['label']==label)['report_exports_unchanged'];checks={}
 for name,item in baseline.items():
  f=next(group.rglob(name));old=r/item['prior'];assert f.read_bytes()==old.read_bytes(),(label,name)
  checks[name]={'prior':item['prior'],'sha256':sha(f),'byte_equal':True}
 exports[label]=checks
 if (p/'comparison_light_large_narrow').exists():
  group=p/'comparison_light_large_narrow';copy(group/'result.json',label+'-comparison4.json')
  for name in ('comparison-eight','comparison-reference','comparison-export'):images.append(copy(next(group.rglob(name+'.png')),label+'-'+name+'-v4.png'))
for version in range(1,5):copy(r/f'build/native/float-ui-source{version}.json',f'source{version}.json')
copy(r/'build/native/Design-analysis-linux/test-results/run-j4iaid5m/results.json','linux-final3.json')
for label,base in [('linux',r/'build/native/Design-analysis-linux/test-results/run-kpt06t9s'),('windows',r/'build/native/Release/test-results/run-is_wqmg8')]:
 names=('docs-code-0-start.bmp',) if label=='linux' else ('hierarchy-expanded.bmp','workspace-tree-open.bmp')
 for name in names:
  paths=list(base.rglob(name))
  if paths:
   p=paths[0];dest=out/(label+'-'+p.stem+'-v2.png');im=Image.open(p).convert('RGB');im.save(dest);assert Image.open(dest).convert('RGB').tobytes()==im.tobytes();images.append({'artifact':dest.name,'source':str(p.relative_to(r)).replace('\\','/'),'source_sha256':sha(p),'sha256':sha(dest),'conversion':'BMP -> PNG, complete RGB equality verified'})
related=[]
base=r/'build/native/Design-analysis-linux/test-results/run-kpt06t9s/workbench_workflow'
for theme in ('dark','light'):
 p=next(p for p in base.rglob('workbench-'+theme+'.bmp') if p.parent.name==theme+'-unified-visual');related.append((p,'linux-develop-'+theme+'.png'))
base=r/'build/native/Design-analysis/test-results/run-rv3u3ph7'
for theme in ('dark','light'):
 p=next(p for p in (base/'laboratory_workflow').rglob('laboratory-initial.bmp') if p.parent.name==theme);related.append((p,'windows-simulate-'+theme+'.png'))
related.append((next((base/'documentation_context_code').rglob('docs-code-0-start.bmp')),'windows-f1-code.png'))
for p,name in related:
 dest=out/name;im=Image.open(p).convert('RGB');assert Image.open(dest).convert('RGB').tobytes()==im.tobytes();images.append({'artifact':name,'source':str(p.relative_to(r)).replace('\\','/'),'source_sha256':sha(p),'sha256':sha(dest),'conversion':'BMP -> PNG, complete RGB equality verified'})
copy(r/'build/native/gather-float-ui-proof.py','gather-proof.py')
for p in sorted((r/'build/native').glob('float-ui-*.log')):
 if p.name.endswith('wm.log'):continue
 with gzip.open(out/(p.name+'.gz'),'wb') as f:f.write(p.read_bytes())
for name in ('run-float-ui-linux-2.sh','run-float-ui-linux-context-2.sh','run-float-ui-linux-final3.sh','run-float-ui-linux-final4.sh'):copy(r/'build/native'/name,name)
fonts=json.loads((r/'docs/visuals/unified-workspace/analysis-comparison-2026-10-22/validation.json').read_text())['font_assets'];assert all(sha(r/k)==v for k,v in fonts.items())
m=json.loads((out/'source4.json').read_text());assert all(hashlib.sha256((r/k).read_bytes().replace(b'\r\n',b'\n')).hexdigest()==v for k,v in m['source'].items())
assert all(sha(r/k)==v for k,v in m['foreign_preserved'].items())
jsonwrite('validation.json',{'date':datetime.datetime.now().astimezone().isoformat(),'base':m['base'],'final_source_version':4,'source_normalization':'CRLF -> LF','font_assets':fonts,'final_runs':final,'report_exports_unchanged':exports,'images':images,'foreign_preserved':m['foreign_preserved'],'related_passed_evidence':{'windows-unit1.json':10,'linux-unit1.json':10,'windows-unit2.json':1,'linux-unit2.json':1,'windows-gpu2.json':2,'linux-gpu2.json':1,'windows-release-unit2.json':10,'windows-release-context2.json':5,'windows-final3.json':2,'linux-final3.json':3},'partial_or_failed_evidence':{'windows-context2.json':'3/5: host focus in workbench, real field contour failure in export','linux-context2.json':'4/5: same field contour failure; final CR filter excluded backdrop, not counted'},'negative_controls':['before.log: actual old pipeline loses fractional coordinates and width','negative.json: only emission casts restored; actual GPU test rejects','counter-field-border.json: original six reference colors differ before painted-contour alignment'],'review':['All app TUs use ui.h flag; both custom callbacks updated; standalone ABI is separate and tested','Variable point lists use actual point size; count and pointer guarded','Scissor clamps finite doubles before integer conversion and leaves output intact on invalid input','Only painted contour coordinates snap; format hit targets and radio positions retained after review','No source/header edit while any corresponding live build or test used that frozen version','Foreign twelve files remain byte-identical; not staged'],'limits':['No full design/pixel identity acceptance','Physical DPI and monitor switching not exercised; arithmetic and raster probes are scoped','macOS and external assistive acceptance remain open','App builds include preserved foreign accessibility work','Earlier failed preflight helpers are not GPU/test success','Existing protected raw captures and dependencies retained']})
# Text copies use repository LF. Preserve scripts byte-for-byte as executed too,
# including the trailing CR which explains the two omitted filter groups.
for p in sorted(out.iterdir()):
 if p.suffix=='.sh':
  raw=p.read_bytes()
  with gzip.open(out/(p.name+'.raw.gz'),'wb') as f:f.write(raw)
for p in sorted(out.iterdir()):
 if p.suffix in ('.json','.py','.patch','.log','.sh'):
  text=p.read_bytes().decode('utf-8').replace('\r\n','\n');text=text.rstrip()+'\n' if p.suffix=='.py' else text;p.write_bytes(text.encode('utf-8'))
jsonwrite('artifacts.json',{p.name:{'bytes':p.stat().st_size,'sha256':sha(p)} for p in sorted(out.iterdir()) if p.is_file() and p.name!='artifacts.json'})
print('Final groups',[(x['label'],len(x['groups'])) for x in final]);print('Exports',sum(len(v) for v in exports.values()));print('Archive files',len(list(out.iterdir())),'bytes',sum(p.stat().st_size for p in out.iterdir() if p.is_file()))
