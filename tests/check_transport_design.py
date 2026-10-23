"""Native CSS transport geometry, original reference, ink and real runner/menu tasks."""
import argparse,json,math,pathlib,re,sys
from check_display_workflow import Workflow
from check_graphic_raster import png

def pixels(image, rect, target, tolerance=45):
    width,height,rgb,_=image;x,y,w,h=rect;points=[]
    for yy in range(max(0,math.floor(y)),min(height,math.ceil(y+h))):
        for xx in range(max(0,math.floor(x)),min(width,math.ceil(x+w))):
            color=rgb[(yy*width+xx)*3:(yy*width+xx)*3+3]
            if max(abs(a-b) for a,b in zip(color,target))<=tolerance:points.append((xx,yy))
    return points

def pixel(image,x,y):
    width,height,rgb,_=image;x,y=math.floor(x),math.floor(y);assert 0<=x<width and 0<=y<height
    return tuple(rgb[(y*width+x)*3:(y*width+x)*3+3])

def main():
    # Compiler/linker output can contain replacement characters for legacy
    # Windows diagnostics; do not lose the native result while printing it.
    for stream in (sys.stdout,sys.stderr):
        if hasattr(stream,'reconfigure'):stream.reconfigure(encoding='utf-8',errors='replace')
    p=argparse.ArgumentParser();p.add_argument('--app',type=pathlib.Path,required=True);p.add_argument('--root',type=pathlib.Path,required=True);p.add_argument('--work',type=pathlib.Path,required=True);args=p.parse_args()
    args.work.mkdir(parents=True,exist_ok=True);flow=Workflow(args.app,args.work,args.root,False);flow.env['SDL_FORCE_RAISEWINDOW']='1';checks=[]
    css=(args.root/'docs/visuals/simulation-designs/simulation.css').read_text(encoding='utf-8')
    assert '.transport button{font-size:11px;min-height:31px;white-space:nowrap}' in css
    assert 'button{cursor:pointer;padding:7px 11px}' in css and '.transport{gap:4px}' in css and '.transport button{padding:6px 8px}' in css
    for mode in ('system','bundled'):
        folder=args.work/mode;folder.mkdir()
        if mode=='bundled':flow.env['PHYSIM_TEST_BUNDLED_FONTS']='1'
        else:flow.env.pop('PHYSIM_TEST_BUNDLED_FONTS',None)
        flow.run('--workspace-state-test',folder,'transport-design',timeout=190,marker='TRANSPORT DESIGN SELF-TEST: PASSED')
        output=flow.steps[-1]['stdout'];records=[]
        for line in output.splitlines():
            if not re.match(r'TRANSPORT (?:ready|normal|running|offline) ',line):continue
            record=dict(state=line.split()[1],variant=re.search(r'variant=(\w+)',line)[1],size=int(re.search(r'size=(\d+)',line)[1]))
            for key in ('rect','icon'):record[key]=[tuple(map(float,m.split(','))) for m in re.findall(key+r'\d=([\d.,-]+)',line)]
            for key in ('speed','arrow','status'):record[key]=tuple(map(float,re.search(key+r'=([\d.,-]+)',line)[1].split(',')))
            assert 'complete=1' in line and 'glyphs=127' in line and 'source=1' in line,line;records.append(record)
        assert [r['state'] for r in records]==['ready']+['normal','running','offline']*6
        assert len(re.findall(r'TRANSPORT ACTIONS variant=\w+ native-enter=1 step-dt=1 offline=1 native-menu=1 escape-return=1 source=1',output))==6
        for r in records[1:]:
            f=r['size']/16;narrow=r['size']>16;gap=(4 if narrow else 6)*f;height=(31 if narrow else 32)*f
            for box in r['rect']+[r['speed'],r['status']]:
                x,y,w,h=box;assert abs(h-height)<.01 and abs(y+h/2-156.5)<.01,(r,box)
            if not narrow:assert abs(r['rect'][0][0]-(18+190*f+24*f))<.01
            else:assert 18+96*f+14*f-.01<=r['rect'][0][0]<=18+190*f+14*f+.01
            assert abs(r['rect'][1][0]-sum(r['rect'][0][::2])-gap)<.01
            assert abs(r['rect'][2][0]-sum(r['rect'][1][::2])-gap)<.01
            assert abs(r['rect'][3][0]-sum(r['rect'][2][::2])-(2*gap+11*f))<.01
            assert abs(r['arrow'][2]-8*f)<.01 and abs(r['arrow'][3]-8*f)<.01
            image=png(folder/f"transport-{r['variant']}-{r['state']}.png")
            assert image[:2]==((1080,740) if narrow else (1440,1000))
            if r['state']!='normal':continue
            light=r['variant'] in ('light','larger','large');contrast=r['variant']=='contrast'
            primary=(255,225,0) if contrast else (21,86,172) if light else (25,101,206)
            text=(255,255,255) if contrast else (32,36,44) if light else (237,238,242)
            primary_text=(0,0,0) if contrast else (255,255,255)
            ink=[]
            # Small antialiased glyphs need visible coverage, not two nearly
            # opaque pixels. Segoe Symbol reset has 7-8 pixels above half
            # coverage at 11 CSS px, but only one within 55 RGB levels.
            for i,box in enumerate(r['icon']):
                points=pixels(image,box,primary_text if i==0 else text,90);assert len(points)>=2,('no original transport symbol ink',mode,r['variant'],i,box);ink.append(len(points))
            points=pixels(image,r['arrow'],text,55);assert len(points)>=2,('no compact tempo arrow',mode,r['variant'])
            assert max(x for x,y in points)-min(x for x,y in points)<=math.ceil(8*f)+1
            assert max(y for x,y in points)-min(y for x,y in points)<=math.ceil(8*f)+1
            x,y,w,h=r['rect'][0];sample=(x+4*f,y+h/2)
            assert pixel(image,*sample)==primary,('normal primary',mode,r['variant'],pixel(image,*sample),primary)
            hover=png(folder/f"transport-{r['variant']}-hover.png")
            hover_color=(255,255,255) if contrast else tuple(min(255,math.floor(c*1.12+.5)) for c in primary)
            assert pixel(hover,*sample)==hover_color,('CSS brightness hover',r['variant'],pixel(hover,*sample),hover_color)
            focus=png(folder/f"transport-{r['variant']}-focus.png");accent=(255,225,0) if contrast else (23,86,158) if light else (138,186,255)
            assert pixels(focus,(x-5*f,y+h/2-3*f,3*f,6*f),accent,1),('missing outer focus',mode,r['variant'])
            r['symbol_ink']=ink;r['arrow_ink']=len(points)
        sources=re.findall(r'UI primary font: (.+)',output);assert len(sources)==1,sources
        assert len(list((folder/'project/runs').glob('*.psrun')))==1,'Transport actions created an extra run'
        assert (folder/'project/main.c').read_bytes()==(args.root/'examples/pendulum/main.c').read_bytes()
        if mode=='system' and sys.platform=='win32':
            from PIL import Image
            reference=Image.open(args.root/'docs/visuals/unified-workspace/simulate-dark.png').convert('RGB')
            original=(reference.width,reference.height,reference.tobytes(),None);actual=png(folder/'transport-ready.png')
            def box(image):
                points=pixels(image,(200,130,150,55),(25,101,206),0);assert points
                return min(x for x,y in points),min(y for x,y in points),max(x for x,y in points),max(y for x,y in points)
            first,second=box(actual),box(original);assert max(abs(a-b) for a,b in zip(first,second))<=1,('original ready button',first,second)
        checks.append(dict(font_mode=mode,primary_source=sources[0],records=records))
    (args.work/'validation.json').write_text(json.dumps(dict(scope=__doc__,checks=checks),ensure_ascii=False,indent=2)+'\n',encoding='utf-8');print('TRANSPORT DESIGN: PASSED')

if __name__=='__main__':main()
