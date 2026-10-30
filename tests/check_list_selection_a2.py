"""A2 list wash, accent/check pixels and preserved native text/status ink."""
import argparse,collections,hashlib,json,os,re,subprocess
from pathlib import Path
from check_graphic_raster import png
p=argparse.ArgumentParser(description=__doc__)
for key in ('app','root','work'):p.add_argument('--'+key,type=Path,required=True)
p.add_argument('--context',choices=('protocol','files','grid'))
p.add_argument('--theme',choices=('dark','light','contrast','large'))
a=p.parse_args();a.app=a.app.resolve();a.work.mkdir(parents=True,exist_ok=True)
css=(a.root/'docs/visuals/selection-options/selection.css').read_text(encoding='utf8')
rgba=re.findall(r'--soft-tint:rgba\((\d+),(\d+),(\d+),([\d.]+)\)',css)
colors={name:(tuple(map(int,row[:3])),float(row[3])) for name,row in zip(('dark','light'),rgba)}
accents=[tuple(bytes.fromhex(value)) for value in re.findall(r'--soft-accent:#([0-9a-f]{6})',css)]
warning=[tuple(bytes.fromhex(value)) for value in re.findall(r'--warn:#([0-9a-f]{6})',css)]
muted=[tuple(bytes.fromhex(value)) for value in re.findall(r'--muted:#([0-9a-f]{6})',css)[:2]]
binary=hashlib.sha256(a.app.read_bytes()).hexdigest();cases=[]
def rgb(data,w,x,y):return tuple(data[(y*w+x)*3:(y*w+x)*3+3])
def inside(r,x,y):return r[0]<=x<r[0]+r[2] and r[1]<=y<r[1]+r[3]
for context in ([a.context] if a.context else ('protocol','files','grid')):
 for theme in ([a.theme] if a.theme else ('dark','light','contrast','large')):
  mode=context+'-'+theme;work=a.work/mode;work.mkdir();cmd=[str(a.app),'--workspace-state-test',str(work),'list-a2-'+mode]
  result=subprocess.run(cmd,capture_output=True,timeout=90,env=dict(os.environ,PHYSIM_TEST_TRACE='1'))
  (work/'stdout.bin').write_bytes(result.stdout);(work/'stderr.bin').write_bytes(result.stderr)
  frames=[json.loads(line) for line in result.stdout.decode('utf8',errors='replace').splitlines() if line.startswith('{"kind":"list_selection_a2"')]
  case=dict(mode=mode,command=cmd,exit_code=result.returncode,frames=frames);cases.append(case)
  (a.work/'verification.json').write_text(json.dumps(dict(binary_sha256=binary,cases=cases),indent=2)+'\n')
  assert result.returncode==0 and b'LIST SELECTION A2 SELF-TEST: PASSED' in result.stdout,(mode,result.stderr.decode('utf8',errors='replace'))
  assert [frame['phase'] for frame in frames]==(['before','after'] if context=='protocol' else ['before','after','focus']);before,after=frames[:2]
  assert before['row']==after['row'],'Selecting a row moved its contents'
  key=lambda t:(t['hex'],tuple(t['box']),tuple(t['clip']),tuple(t['color']))
  assert collections.Counter(map(key,before['texts']))==collections.Counter(map(key,after['texts'])),'Selection changed original text, geometry or ink'
  bw,bh,bdata,bsha=png(work/'list-before.png');w,h,data,asha=png(work/'list-after.png');assert (bw,bh)==(w,h)
  r=after['row'];f=after['scale'];assert r[2]>40*f and r[3]>20*f
  checkbox=(r[0]+13*f,r[1]+13*f if context=='grid' else r[1]+(r[3]-15*f)/2,15*f,15*f)
  border_color=(212,212,212) if theme=='contrast' else muted[theme=='light']
  for frame in (before,after):
   borders=[rect for rect in frame['rectangles'] if max(abs(rect['box'][i]-checkbox[i]) for i in range(4))<.01]
   assert len(borders)==1 and tuple(borders[0]['color'])==(*border_color,255) and abs(borders[0]['width']-f)<.01 and abs(borders[0]['rounding']-3*f)<.01,'A2 check border differs from shared CSS role/geometry'
  mask={(x,y) for y in range(max(0,int(r[1]+7*f)),min(h,int(r[1]+r[3]-7*f))) for x in range(max(0,int(r[0]+7*f)),min(w,int(r[0]+r[2]-7*f))) if not inside((checkbox[0]-2*f,checkbox[1]-2*f,19*f,19*f),x+.5,y+.5)}
  background=collections.Counter(rgb(bdata,w,x,y) for x,y in mask).most_common(1)[0][0]
  tint,alpha=((0,0,110),1.) if theme=='contrast' else colors['light' if theme=='light' else 'dark']
  accent=(255,225,0) if theme=='contrast' else accents[theme=='light']
  wash=tuple(round(background[i]*(1-alpha)+tint[i]*alpha) for i in range(3));flat=0
  for x,y in mask:
   if rgb(bdata,w,x,y)!=background:continue
   actual=rgb(data,w,x,y);assert max(abs(actual[i]-wash[i]) for i in range(3))<=1,('A2 row wash differs',mode,(x,y),actual,wash)
   flat+=1
  assert flat>=40
  edge={(x,y) for y in range(max(0,int(r[1]+8*f)),min(h,int(r[1]+r[3]-8*f))) for x in range(max(0,int(r[0])+1),min(w,int(r[0]+3*f)))}
  assert edge and all(rgb(data,w,x,y)==accent for x,y in edge),('A2 3px row accent missing',mode)
  check_lines=[line for line in after['lines'] if tuple(line['color'])==(*accent,255) and abs(line['width']-1.5*f)<.01 and inside(checkbox,*line['points'][:2]) and inside(checkbox,*line['points'][2:])]
  assert len(check_lines)==2,('A2 selected check missing',mode,len(check_lines))
  check_direction=[accent[i]-wash[i] for i in range(3)];check_norm=sum(v*v for v in check_direction);checks=0
  for y in range(max(0,int(checkbox[1]+2*f)),min(h,int(checkbox[1]+13*f))):
   for x in range(max(0,int(checkbox[0]+2*f)),min(w,int(checkbox[0]+13*f))):
    pixel=rgb(data,w,x,y);coverage=sum((pixel[i]-wash[i])*check_direction[i] for i in range(3))/check_norm
    checks+=.2<=coverage<=1.02 and max(abs(pixel[i]-(wash[i]+coverage*check_direction[i])) for i in range(3))<=3
  assert checks>=4,('A2 selected check missing',mode,checks)
  ink=0
  groups=[(t['box'],t['clip'],t['color'][:3]) for t in before['texts']]
  if context=='protocol':groups.append((r,r,accent if theme=='contrast' else warning[theme=='light']))
  status_ink=0
  for k,(box,clip,foreground) in enumerate(groups):
   direction=[foreground[i]-background[i] for i in range(3)];norm=sum(v*v for v in direction)
   if norm<100:continue
   count=0
   for y in range(max(0,int(box[1])),min(h,int(box[1]+box[3])+1)):
    for x in range(max(0,int(box[0])),min(w,int(box[0]+box[2])+1)):
     if (x,y) not in mask or not inside(clip,x+.5,y+.5):continue
     original=rgb(bdata,w,x,y);coverage=sum((original[i]-background[i])*direction[i] for i in range(3))/norm
     if not .35<=coverage<=1.01 or max(abs(original[i]-(background[i]+coverage*direction[i])) for i in range(3))>2:continue
     expected=[original[i]+(wash[i]-background[i])*(1-coverage) for i in range(3)];actual=rgb(data,w,x,y)
     assert max(abs(actual[i]-expected[i]) for i in range(3))<=4,('A2 changes original row ink',mode,(x,y),actual,expected)
     count+=1
   if context=='protocol' and k==len(groups)-1:status_ink=count
   else:ink+=count
  assert ink>=10 and (context!='protocol' or status_ink>=5),('Missing original text/status ink evidence',mode,ink,status_ink)
  focus_segments=0
  if context!='protocol':
   focus=frames[2];fr=focus['row'];assert fr[2:]==r[2:],'Keyboard focus resized the selected row'
   relative_key=lambda t,row:(t['hex'],tuple(round(t['box'][i]-row[i],3) if i<2 else t['box'][i] for i in range(4)),tuple(t['color']))
   assert collections.Counter(relative_key(t,r) for t in after['texts'])==collections.Counter(relative_key(t,fr) for t in focus['texts']),'Keyboard reveal changed row text, ink or relative layout'
   fw,fh,fdata,fsha=png(work/'list-focus.png');assert (fw,fh)==(w,h)
   focus_color=(255,225,0) if theme=='contrast' else (23,86,158) if theme=='light' else (138,186,255)
   focus_lines=[line for line in focus['lines'] if tuple(line['color'])==(*focus_color,255) and abs(line['width']-2*f)<.01]
   assert len(focus_lines)>=12,'Missing native dashed focus commands in the shared accent role'
   assert all(max(abs(line['points'][2]-line['points'][0]),abs(line['points'][3]-line['points'][1]))<=3*f+.01 for line in focus_lines),'Focus contains a solid edge'
   samples=[]
   direction=[focus_color[i]-wash[i] for i in range(3)];norm=sum(v*v for v in direction)
   for x in range(max(0,int(fr[0]+6*f)),min(w,int(fr[0]+fr[2]-6*f))):
    visible=False
    for y in range(max(0,int(fr[1])),min(h,int(fr[1]+4*f)+1)):
     pixel=rgb(fdata,w,x,y);coverage=sum((pixel[i]-wash[i])*direction[i] for i in range(3))/norm
     visible|=.2<=coverage<=1.02 and max(abs(pixel[i]-(wash[i]+coverage*direction[i])) for i in range(3))<=3
    samples.append(visible)
   focus_segments=sum(value and (i==0 or not samples[i-1]) for i,value in enumerate(samples))
   assert focus_segments>=3 and samples.count(False)>=3,('Keyboard focus must have visible dashed segments and gaps',mode,focus_segments)
   case['focus_sha256']=fsha
   case['keyboard_reveal_offset']=[round(fr[i]-r[i],4) for i in range(2)]
  case.update(image_size=[w,h],background=list(background),wash=list(wash),flat_pixels=flat,original_text_pixels=ink,original_status_pixels=status_ink,edge_pixels=len(edge),check_pixels=checks,focus_segments=focus_segments,before_sha256=bsha,after_sha256=asha)
  assert hashlib.sha256(a.app.read_bytes()).hexdigest()==binary
(a.work/'verification.json').write_text(json.dumps(dict(binary_sha256=binary,cases=cases),indent=2)+'\n')
print('A2 native rows:',len(cases),'cases passed:',', '.join(case['mode'] for case in cases))
