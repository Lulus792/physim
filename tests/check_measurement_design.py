"""Native measurement typography and real channel/scale input; paused data preserved."""
import argparse,json,pathlib,re,sys
from check_display_workflow import Workflow
from check_graphic_raster import png
from check_transport_design import pixels
from check_inspector_design import rect

def inspect(folder,out,mode):
    records=[]
    for line in out.splitlines():
        if line.startswith('MEASUREMENT kind='):
            r={k:re.search(r'\b'+k+r'=([^ ]+)',line)[1] for k in ('kind','variant','size','preserved','curves','scroll')}
            for k in ('size','preserved','curves','scroll'):r[k]=int(r[k])
            for k in ('title','time','switch'):r[k]=rect(line,k)
            r['metrics']=[];r['heads']=[];records.append(r)
        elif line.startswith('METRIC slot='):
            m={k:float(re.search(r'\b'+k+r'=([^ ]+)',line)[1]) for k in ('slot','channel','raw','scale','value-font','unit-font')}
            for k in ('number','unit','paint0','paint1','paint2'):m[k]=bytes.fromhex(re.search(r'\b'+k+r'=([0-9a-f]*)',line)[1]).decode('utf-8')
            for k in ('card','label','value','unit-rect','rule'):m[k]=rect(line,k)
            records[-1]['metrics'].append(m)
        elif line.startswith('CHANNEL HEAD '):
            h={k:rect(line,k) for k in ('field','visible','name','unit')};h['rendered']=int(re.search(r'\brendered=(-?\d+)',line)[1])
            for k in ('name-text','unit-text','paint0','paint1'):h[k]=bytes.fromhex(re.search(r'\b'+k+r'=([0-9a-f]*)',line)[1]).decode('utf-8')
            records[-1]['heads'].append(h)
    assert len(records)==30,(mode,len(records))
    for r in records:
        assert r['preserved'] and len(r['metrics'])==2
        im=png(folder/f"measurement-{r['variant']}-{r['kind']}.png")
        light=r['variant'] in ('light','larger','large');contrast=r['variant']=='contrast'
        ink=(255,255,255) if contrast else (32,36,44) if light else (237,238,242)
        muted=(221,221,221) if contrast else (96,103,115) if light else (155,158,168)
        assert pixels(im,r['title'],ink,40) and pixels(im,r['time'],muted,40)
        for m in r['metrics']:
            assert m['value-font']>1.7*m['unit-font']
            expected=format(m['raw']/m['scale'],'.2f').replace('.',',')
            assert m['number']==expected,(r['variant'],r['kind'],m['number'],expected)
            compact=lambda x:''.join(x.split())
            assert m['unit']
            if r['kind'] in ('normal','restored','converted'):
                assert compact(m['paint1'])==compact(m['number']) and compact(m['paint2'])==compact(m['unit']),(mode,r['variant'],r['kind'],m)
                assert m['paint0']
            for role in ('label','value','unit-rect'):
                b=m[role];card=m['card'];assert b[0]>=card[0]-.02 and b[1]>=card[1]-.02 and b[0]+b[2]<=card[0]+card[2]+.02 and b[1]+b[3]<=card[1]+card[3]+.02
                if r['kind'] in ('normal','restored','converted'):assert pixels(im,b,ink if role=='value' else muted,45),('missing actual metric',mode,r['variant'],role)
            if r['kind'] in ('normal','restored','converted'):
                # A nonconvex rounded rectangle produced a 6–10 px wedge for a
                # requested 2px rule in source1. Check actual interior pixels.
                x,y,w,h=m['rule'];background=(246,247,249) if light else (0,0,0) if contrast else (30,31,35)
                bands=[]
                for fraction in (.2,.5,.8):
                    xx=int(x+w*fraction)
                    ys=[yy for yy in range(max(0,int(y)-6),min(im[1],int(y+h)+7)) if any(abs(im[2][(yy*im[0]+xx)*3+i]-background[i])>35 for i in range(3))]
                    bands.append(len(ys))
                assert all(1<=n<=int(h)+3 for n in bands),('nonflat metric rule',r['variant'],r['kind'],m['slot'],bands,h)
                if m['slot']==0 and not contrast:
                    # Common shell accent, alpha .7 over the native window.
                    target=(23,86,158) if light else (138,186,255)
                    expected_color=tuple(round(background[i]*(1-179/255)+target[i]*179/255) for i in range(3))
                    assert pixels(im,m['rule'],expected_color,2),('legacy channel palette',mode,r['variant'],r['kind'])
        if r['curves']:
            for h in r['heads']:
                if not h['field'][2]:continue
                fully_visible=h['visible'][2]>=h['field'][2]-.03 and h['visible'][3]>=h['field'][3]-.03
                if r['kind'] in ('curves','channel'):assert fully_visible,('focused channel clipped',mode,r['variant'],h)
                if fully_visible:assert ''.join(h['paint0'].split())==''.join(h['name-text'].split()) and ''.join(h['paint1'].split())==''.join(h['unit-text'].split())
                if r['kind'] in ('curves','channel'):
                    assert pixels(im,h['name'],ink,45) and pixels(im,h['unit'],muted,45)
                assert h['name'][0]==h['field'][0] and abs(h['unit'][0]+h['unit'][2]-h['field'][0]-h['field'][2])<.03
        if r['kind']=='channel':assert r['metrics'][0]['channel']==1
        if r['kind']=='converted':assert all(m['unit']=='mrad/s' for m in r['metrics'])
        if r['kind'] in ('normal','restored'):assert not r['curves']
        else:assert r['curves']
    return dict(records=records)

def main():
    for s in (sys.stdout,sys.stderr):
        if hasattr(s,'reconfigure'):s.reconfigure(encoding='utf-8',errors='replace')
    p=argparse.ArgumentParser();p.add_argument('--app',type=pathlib.Path,required=True);p.add_argument('--root',type=pathlib.Path,required=True);p.add_argument('--work',type=pathlib.Path,required=True);a=p.parse_args();a.work.mkdir(parents=True,exist_ok=True)
    flow=Workflow(a.app,a.work,a.root,False);checks=[];flow.env['SDL_FORCE_RAISEWINDOW']='1'
    for mode in ('system','bundled'):
        if mode=='bundled':flow.env['PHYSIM_TEST_BUNDLED_FONTS']='1'
        else:flow.env.pop('PHYSIM_TEST_BUNDLED_FONTS',None)
        folder=a.work/mode;folder.mkdir();flow.run('--workspace-state-test',folder,'measurement-design',timeout=190,marker='MEASUREMENT DESIGN SELF-TEST: PASSED');assert 'MEASUREMENT FORMATS: zero=1 negative-zero=1 tiny=1 large=1 negative=1' in flow.steps[-1]['stdout'];checks.append(dict(font_mode=mode,**inspect(folder,flow.steps[-1]['stdout'],mode)))
        assert len(list((folder/'project/runs').glob('*.psrun')))==1 and (folder/'project/main.c').read_bytes()==(a.root/'examples/pendulum/main.c').read_bytes()
    (a.work/'validation.json').write_text(json.dumps(dict(scope=__doc__,checks=checks),ensure_ascii=False,indent=2)+'\n',encoding='utf-8');print('MEASUREMENT DESIGN: PASSED')
if __name__=='__main__':main()
