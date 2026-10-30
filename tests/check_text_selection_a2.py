"""A2 on real code, search and readonly text: actual pixels, UTF-8 and clipboard."""
import argparse,collections,hashlib,json,os,re,subprocess
from pathlib import Path
from check_graphic_raster import png
p=argparse.ArgumentParser(description=__doc__)
for key in ('app','root','work'):p.add_argument('--'+key,type=Path,required=True)
p.add_argument('--context',choices=('code','field','readonly'));p.add_argument('--theme',choices=('dark','light','contrast','large'))
a=p.parse_args();a.app=a.app.resolve();a.work.mkdir(parents=True,exist_ok=True)
css=(a.root/'docs/visuals/selection-options/APPROVED.md').read_text(encoding='utf8')
rgba=re.findall(r'`rgba\((\d+),(\d+),(\d+),([.\d]+)\)`',css)
assert len(rgba)>=2
colors={name:([int(v) for v in values[:3]],float(values[3])) for name,values in zip(('dark','light'),rgba[:2])}
binary=hashlib.sha256(a.app.read_bytes()).hexdigest();cases=[]
def inside(r,x,y):return r[0]<=x<r[0]+r[2] and r[1]<=y<r[1]+r[3]
def rgb(data,width,x,y):return tuple(data[(y*width+x)*3:(y*width+x)*3+3])
for context in ([a.context] if a.context else ('code','field','readonly')):
 for theme in ([a.theme] if a.theme else ('dark','light','contrast','large')):
  mode=context+'-'+theme;work=a.work/mode;work.mkdir();cmd=[str(a.app),'--workspace-state-test',str(work),'selection-a2-'+mode]
  result=subprocess.run(cmd,capture_output=True,timeout=90,env=dict(os.environ,PHYSIM_TEST_TRACE='1'))
  (work/'stdout.bin').write_bytes(result.stdout);(work/'stderr.bin').write_bytes(result.stderr)
  frames=[json.loads(s) for s in result.stdout.decode('utf8',errors='replace').splitlines() if s.startswith('{"kind":"text_selection_a2"')]
  case=dict(mode=mode,command=cmd,exit_code=result.returncode,frames=frames);cases.append(case)
  (a.work/'verification.json').write_text(json.dumps(dict(binary_sha256=binary,cases=cases),indent=2)+'\n')
  assert result.returncode==0 and b'TEXT SELECTION A2 SELF-TEST: PASSED' in result.stdout,(mode,result.stderr.decode('utf8',errors='replace'))
  assert [f['phase'] for f in frames]==['before','after'];before,after=frames
  assert before['crc']==after['crc'] and before['bytes']==after['bytes'] and abs(after['selection'][1]-after['selection'][0])>0,'Selection changed original bytes'
  bw,bh,bdata,bsha=png(work/'a2-before.png');w,h,data,asha=png(work/'a2-after.png');assert (bw,bh)==(w,h)
  assert before['field']==after['field'],'Selection moved the field'
  assert after['fills'],'No actual selection rectangles'
  mask=set()
  for fill in after['fills']:
   r=fill['box'];clip=fill['clip']
   for y in range(max(0,int(r[1]+2)),min(h,int(r[1]+r[3]-2))):
    for x in range(max(0,int(r[0]+1)),min(w,int(r[0]+r[2]-1))):
     if inside(clip,x+.5,y+.5):mask.add((x,y))
  assert mask,'No visible selected interior'
  background=collections.Counter(rgb(bdata,w,x,y) for x,y in mask).most_common(1)[0][0]
  tint,alpha=([0,0,110],1.) if theme=='contrast' else colors['light' if theme=='light' else 'dark']
  wash=tuple(round(background[i]*(1-alpha)+tint[i]*alpha) for i in range(3));flat=0
  for x,y in mask:
   if rgb(bdata,w,x,y)!=background:continue
   actual=rgb(data,w,x,y)
   assert max(abs(actual[i]-wash[i]) for i in range(3))<=1,('A2 text selection background differs',mode,(x,y),actual,wash,background)
   flat+=1
  assert flat>=40,('Too few background pixels',mode,flat)
  ink=0
  for t in before['texts']:
   foreground=t['color'];direction=[foreground[i]-background[i] for i in range(3)];norm=sum(v*v for v in direction)
   if foreground[3]!=255 or norm<100:continue
   r=t['box'];clip=t['clip']
   for y in range(max(0,int(r[1])),min(h,int(r[1]+r[3])+1)):
    for x in range(max(0,int(r[0])),min(w,int(r[0]+r[2])+1)):
     if (x,y) not in mask or not inside(clip,x+.5,y+.5):continue
     original=rgb(bdata,w,x,y);coverage=sum((original[i]-background[i])*direction[i] for i in range(3))/norm
     if not .35<=coverage<=1.01 or max(abs(original[i]-(background[i]+coverage*direction[i])) for i in range(3))>2:continue
     expected=[original[i]+(wash[i]-background[i])*(1-coverage) for i in range(3)];actual=rgb(data,w,x,y)
     assert max(abs(actual[i]-expected[i]) for i in range(3))<=4,('A2 selected ink differs from original',mode,(x,y),actual,expected,t['hex'])
     ink+=1
  assert ink>=10,('Too few original ink pixels',mode,ink)
  text_key=lambda t:(t['hex'],tuple(t['box']),tuple(t['color']))
  assert collections.Counter(map(text_key,before['texts']))==collections.Counter(map(text_key,after['texts'])),'Selected text commands change original ink or glyph layout'
  if context=='code':assert len({tuple(t['color']) for t in before['texts']})>=4,'Syntax colors not exercised'
  case.update(background=list(background),expected_wash=list(wash),background_pixels=flat,ink_pixels=ink,before_sha256=bsha,after_sha256=asha)
  assert hashlib.sha256(a.app.read_bytes()).hexdigest()==binary
(a.work/'verification.json').write_text(json.dumps(dict(binary_sha256=binary,cases=cases),indent=2)+'\n')
print('A2 text selection: actual code/search/readonly pixels, original ink/UTF-8, syntax colors and real clipboard passed')
