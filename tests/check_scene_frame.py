"""Native scene overlay/footer, real camera input and unchanged paused experiment."""
import argparse,json,pathlib,re,sys
from check_display_workflow import Workflow
from check_graphic_raster import png
from check_transport_design import pixels
from check_inspector_design import rect

def inspect(folder,out,mode):
    records=[]
    for line in out.splitlines():
        if line.startswith('SCENE FRAME kind='):
            r={k:re.search(r'\b'+k+r'=([^ ]+)',line)[1] for k in ('kind','variant','size','preserved','mode','projection','plane','selected','yaw','pitch','zoom')}
            for k in ('size','preserved','mode','projection','plane','selected'):r[k]=int(r[k])
            for k in ('yaw','pitch','zoom'):r[k]=float(r[k])
            for k in ('frame','viewport','footer'):r[k]=rect(line,k)
            r['text']={};records.append(r)
        elif line.startswith('SCENE TEXT '):
            role=re.search(r'role=(\w+)',line)[1];paint=bytes.fromhex(re.search(r'paint=([0-9a-f]*)',line)[1]).decode('utf-8')
            records[-1]['text'][role]=dict(rect=rect(line,'rect'),paint=paint)
        elif line.startswith('SCENE EDGE '):
            records[-1]['edge']=dict(old=rect(line,'old'),actual=rect(line,'actual'),glyph=int(re.search(r'glyph=(\d+)',line)[1]),color=int(re.search(r'color=([0-9a-f]+)',line)[1],16),counterfactual_unseeded=True)
    assert len(records)==54,(mode,len(records))
    actions=re.findall(r'SCENE ACTIONS variant=(\w+) pointer-menu-no-pick=1 escape-return=1 native-preset=1 native-reset=1 orbit=1 mode-2d=1 pointer-reset=1 footer-wheel-no-zoom=1 tab-order=1 label-avoidance=1 source-and-run=1',out);assert len(actions)==6
    compact=lambda s:''.join(s.split())
    for r in records:
        f=r['size']/16;frame,view,foot=r['frame'],r['viewport'],r['footer'];assert r['preserved'] and r['selected'] and len(r['text'])==6
        assert frame[3]>=180 and view[3]>=64
        assert abs(frame[0]-view[0])<.02 and abs(frame[1]-view[1])<.02 and abs(frame[2]-view[2])<.02
        assert abs(view[1]+view[3]-foot[1])<.02 and abs(foot[1]+foot[3]-frame[1]-frame[3])<.02
        assert foot[3]>=32*f-.02
        im=png(folder/f"scene-{r['variant']}-{r['kind']}.png");light=r['variant'] in ('light','larger','large');contrast=r['variant']=='contrast'
        text=(255,255,255) if contrast else (32,36,44) if light else (237,238,242);muted=(221,221,221) if contrast else (96,103,115) if light else (155,158,168)
        for role,data in r['text'].items():
            b=data['rect']
            if not b[2]:continue
            assert b[0]>=frame[0]-.02 and b[0]+b[2]<=frame[0]+frame[2]+.02 and b[1]>=frame[1]-.02 and b[1]+b[3]<=frame[1]+frame[3]+.02,(mode,r['variant'],role,b,frame)
            if role in ('view','hint','reset'):assert b[1]>=foot[1] and b[1]+b[3]<=foot[1]+foot[3]+.02
            if r['kind']!='menu' or role not in ('select','reset'):assert pixels(im,b,muted if role in ('view','hint','status') else text,55),('missing ink',mode,r['variant'],r['kind'],role)
        t=r['text']
        if r['kind']=='menu':assert compact(t['reset']['paint']).startswith('Zentrieren') # Popup text may cover the same pixel rectangle.
        else:assert compact(t['reset']['paint'])=='Zentrieren'
        if t['label']['rect'][2]:assert compact(t['label']['paint'])==('2D-Ansicht' if r['mode'] else '3D-Ansicht')
        if t['status']['rect'][2]:assert compact(t['status']['paint'])=='Pausiert'
        if r['kind']!='menu':
            expected='Eigene Ansicht' if r['kind'] in ('normal','orbit') else 'Oben' if r['kind']=='preset' else ('Vorne','Seite','Oben')[r['plane']] if r['kind']=='2d' else 'Vorne' if r['kind']=='label-edge' else 'Perspektive'
            assert compact(t['select']['paint'])==compact(expected),(mode,r['variant'],r['kind'],t['select'])
        expected=f"2D · {('XY','YZ','XZ')[r['plane']]} · Meter" if r['mode'] else 'Oben · Meter' if r['kind']=='preset' else 'Vorne · Meter' if r['kind']=='label-edge' else 'Perspektive · Meter'
        assert compact(t['view']['paint'])==compact(expected),(mode,r['variant'],r['kind'],t['view'])
        if r['variant'] in ('dark','light'):assert t['hint']['rect'][2]>0
        else:assert t['hint']['rect'][2]==0
        if r['kind']=='2d':assert r['mode'] and r['plane']==2
        if r['kind']=='preset':assert r['projection'] and not r['mode']
        if r['kind']=='label-edge':
            e=r['edge'];a=e['actual'];field=t['select']['rect'];old=e['old']
            def overlaps(a,b):return a[0]<b[0]+b[2] and a[0]+a[2]>b[0] and a[1]<b[1]+b[3] and a[1]+a[3]>b[1]
            assert overlaps(old,field) and e['glyph']==1 and a[2]>0
            assert a[0]>=view[0]-.02 and a[1]>=view[1]-.02 and a[0]+a[2]<=view[0]+view[2]+.02 and a[1]+a[3]<=view[1]+view[3]+.02
            assert all(not b['rect'][2] or not overlaps(a,b['rect']) for key,b in t.items() if key in ('label','status','select'))
            color=e['color'];assert pixels(im,a,((color>>24)&255,(color>>16)&255,(color>>8)&255),55),('moved scientific label missing',mode,r['variant'])
    return dict(records=records,actions=actions)

def main():
    for stream in (sys.stdout,sys.stderr):
        if hasattr(stream,'reconfigure'):stream.reconfigure(encoding='utf-8',errors='replace')
    p=argparse.ArgumentParser();p.add_argument('--app',type=pathlib.Path,required=True);p.add_argument('--root',type=pathlib.Path,required=True);p.add_argument('--work',type=pathlib.Path,required=True);a=p.parse_args();a.work.mkdir(parents=True,exist_ok=True)
    flow=Workflow(a.app,a.work,a.root,False);flow.env['SDL_FORCE_RAISEWINDOW']='1';checks=[]
    for mode in ('system','bundled'):
        if mode=='bundled':flow.env['PHYSIM_TEST_BUNDLED_FONTS']='1'
        else:flow.env.pop('PHYSIM_TEST_BUNDLED_FONTS',None)
        folder=a.work/mode;folder.mkdir();flow.run('--workspace-state-test',folder,'scene-frame-design',timeout=190,marker='SCENE FRAME SELF-TEST: PASSED');checks.append(dict(font_mode=mode,**inspect(folder,flow.steps[-1]['stdout'],mode)))
        assert len(list((folder/'project/runs').glob('*.psrun')))==1 and (folder/'project/main.c').read_bytes()==(a.root/'examples/pendulum/main.c').read_bytes()
    (a.work/'validation.json').write_text(json.dumps(dict(scope=__doc__,checks=checks),ensure_ascii=False,indent=2)+'\n',encoding='utf-8');print('SCENE FRAME: PASSED')
if __name__=='__main__':main()
