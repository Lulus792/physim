"""Native timeline fidelity: real adaptive times, keyboard, cancellation and popout."""
import argparse,json,math,pathlib,re,sys
from check_display_workflow import Workflow
from check_graphic_raster import png
from check_transport_design import pixels,pixel

def rect(line,name):return tuple(map(float,re.search(r'\b'+name+r'=([\d.,-]+)',line)[1].split(',')))
def main():
    for stream in (sys.stdout,sys.stderr):
        if hasattr(stream,'reconfigure'):stream.reconfigure(encoding='utf-8',errors='replace')
    p=argparse.ArgumentParser();p.add_argument('--app',type=pathlib.Path,required=True);p.add_argument('--root',type=pathlib.Path,required=True);p.add_argument('--work',type=pathlib.Path,required=True);args=p.parse_args()
    args.work.mkdir(parents=True,exist_ok=True);flow=Workflow(args.app,args.work,args.root,False);flow.env['SDL_FORCE_RAISEWINDOW']='1';checks=[]
    css=(args.root/'docs/visuals/simulation-designs/simulation.css').read_text(encoding='utf-8')
    for rule in ('.timeline-controls{margin-left:auto;display:flex;gap:6px}','.timeline-track{display:flex;gap:25px;align-items:center;margin-top:10px}', '.timeline output{min-width:120px;font-size:18px;letter-spacing:-.4px;font-variant-numeric:tabular-nums}'):
        assert rule in css,rule
    for mode in ('system','bundled'):
        folder=args.work/mode;folder.mkdir()
        if mode=='bundled':flow.env['PHYSIM_TEST_BUNDLED_FONTS']='1'
        else:flow.env.pop('PHYSIM_TEST_BUNDLED_FONTS',None)
        flow.run('--workspace-state-test',folder,'timeline-design',timeout=190,marker='TIMELINE DESIGN SELF-TEST: PASSED')
        output=flow.steps[-1]['stdout'];records=[]
        sample=[float(t) for t in re.findall(r'TIMELINE SAMPLE index=\d+ time=([^\s]+)',output)];assert len(sample)>=10 and all(a<b for a,b in zip(sample,sample[1:]))
        hit=re.search(r'TIMELINE NONUNIFORM: count=(\d+) fraction=([^\s]+) expected=(\d+) old-index=(\d+) start=([^\s]+) end=([^\s]+)',output);assert hit
        count,fraction,expected,old,first,last=hit.groups();count,expected,old=int(count),int(expected),int(old);fraction,first,last=map(float,(fraction,first,last));target=first+(last-first)*fraction
        correct=min(range(count),key=lambda i:abs(sample[i]-target));assert count==len(sample) and correct==expected and old==math.floor(fraction*(count-1)+.5) and old!=correct
        batch=re.search(r'TIMELINE POINTER BATCH: wanted=(\d+) actual=(\d+) time=([^\s]+) drag=(\d+)',output);assert batch
        wanted,actual,time,drag=batch.groups();assert int(wanted)==int(actual)==correct and float(time)==sample[correct] and drag=='0'
        assert 'TIMELINE INVARIANTS: live-run-unchanged=1 source=1 snapshot=1 escape=1 resize=1 actual-focus-loss=1 popout=1 return=1' in output
        assert len(re.findall(r'TIMELINE ACTIONS variant=\w+ native-enter=1 automatic-return=1 time-seek=1 preserved=1',output))==6
        for line in output.splitlines():
            if re.match(r'TIMELINE DESIGN (?:normal|playing|browse|minimum|detached|returned) variant=',line):
                state=line.split()[2];record=dict(state=state,variant=re.search(r'variant=([\w-]+)',line)[1],size=int(re.search(r'size=(\d+)',line)[1]),time=float(re.search(r'\btime=([^\s]+)',line)[1]),end=float(re.search(r'\bend=([^\s]+)',line)[1]),ticks=[])
                assert 'complete=1' in line and 'source=1' in line,line
                for key in ('heading','play','live','readout','range'):record[key]=rect(line,key)
                record['browsing']='browsing=1' in line;record['playing']='playing=1' in line;records.append(record)
            elif line.startswith('TIMELINE TICK '):
                records[-1]['ticks'].append(dict(value=float(re.search(r'value=([^\s]+)',line)[1]),rect=rect(line,'rect'),text=line.split(' text=',1)[1]))
        assert len(records)==24,[r['variant'] for r in records]
        for r in records:
            name=f"timeline-{r['variant']}-{'normal' if r['state'] in ('minimum','detached','returned') else r['state']}.png";im=png(folder/name);width,height=im[:2];f=r['size']/16
            assert abs(sum(r['live'][::2])-(width-18))<.01
            assert abs(sum(r['play'][::2])+6*f-r['live'][0])<.01
            assert abs(r['play'][3]-24.5*f)<.01 and abs(r['live'][3]-24.5*f)<.01
            assert abs(r['range'][0]-(r['readout'][0]+r['readout'][2]+25*f))<.01 and abs(r['range'][3]-17*f)<.01
            assert 2<=len(r['ticks'])<=7
            previous=-math.inf
            for t in r['ticks']:
                assert first-1e-12<=t['value']<=last+1e-12 and t['value']>previous;previous=t['value']
                assert '.' not in t['text'] and t['text'].endswith(' s')
                # Published label text is the real numeric ruler value, rounded for display.
                value=float(t['text'][:-2].replace(',','.'));assert math.isclose(value,t['value'],rel_tol=.005,abs_tol=1e-12)
                x,y,w,h=t['rect'];assert x>=r['range'][0]-.01 and x+w<=sum(r['range'][::2])+1.01 and y+h<=height+.01
            light=r['variant'] in ('light','larger','large','minimum','detached-min','detached-browse','detached-playing','detached-wide','returned');contrast=r['variant']=='contrast';text=(255,255,255) if contrast else (32,36,44) if light else (237,238,242);muted=(255,255,255) if contrast else (84,94,111) if light else (161,165,175);accent=(255,225,0) if contrast else (23,86,158) if light else (138,186,255)
            assert len(pixels(im,r['readout'],text,70))>=8,('blank timeline value',mode,r['variant'])
            assert len(pixels(im,r['range'],accent,2))>=10,('missing range',mode,r['variant'])
            for t in r['ticks']:assert len(pixels(im,t['rect'],muted,95))>=2,('blank tick',mode,r['variant'],t)
            # A hover hint must not cover the native ruler or its labels.
            x,y,w,h=r['ticks'][-1]['rect'];background=(0,0,0) if contrast else (246,247,249) if light else (30,31,35)
            assert pixel(im,x-4*f,y+h/2)==background,('ruler obscured by hover popup',mode,r['variant'],pixel(im,x-4*f,y+h/2),background)
            if r['state']=='browse' or r['variant']=='detached-browse':
                x,y,w,h=r['range'];assert pixel(im,x+20*f,y+f)==background,('unexpected pointer focus rectangle',mode,r['variant'])
            if r['variant']=='minimum':
                rows={y for x,y in pixels(im,(2,height-28-95-4,width-4,9),accent,1)};assert rows=={height-28-95},('separator focus must be a single line',rows)
            if r['state']!='normal':continue
            focus=png(folder/f"timeline-{r['variant']}-focus.png");x,y,w,h=r['play'];assert pixels(focus,(x-5*f,y+h/2-3*f,3*f,6*f),accent,1),('missing play focus',mode,r['variant'])
            rf=png(folder/f"timeline-{r['variant']}-range-focus.png");x,y,w,h=r['range'];assert pixels(rf,(x-5*f,y+h/2-3*f,3*f,6*f),accent,1),('missing keyboard range focus',mode,r['variant'])
        assert len(list((folder/'project/runs').glob('*.psrun')))==1
        assert (folder/'project/main.c').read_bytes()==(args.root/'examples/pendulum/main.c').read_bytes()
        assert 'simulation.steps=adaptive' in (folder/'project/physim.project').read_text(encoding='utf-8')
        if mode=='system' and sys.platform=='win32':
            from PIL import Image
            ref=Image.open(args.root/'docs/visuals/unified-workspace/simulate-dark.png').convert('RGB');reference=(ref.width,ref.height,ref.tobytes(),None);native=png(folder/'timeline-dark-normal.png')
            def box(im,color,area):
                points=pixels(im,area,color,0);assert points
                return min(x for x,y in points),min(y for x,y in points),max(x for x,y in points),max(y for x,y in points)
            for color,area in [((48,49,54),(1250,855,123,42)),((43,61,89),(1370,855,55,42)),((138,186,255),(155,896,1273,24))]:
                a,b=box(native,color,area),box(reference,color,area);assert max(abs(x-y) for x,y in zip(a,b))<=1,('original timeline rectangle',color,a,b)
        checks.append(dict(font_mode=mode,nonuniform=dict(fraction=fraction,expected=expected,old_index=old,times=sample),records=records))
    (args.work/'validation.json').write_text(json.dumps(dict(scope=__doc__,checks=checks),ensure_ascii=False,indent=2)+'\n',encoding='utf-8');print('TIMELINE DESIGN: PASSED')
if __name__=='__main__':main()
