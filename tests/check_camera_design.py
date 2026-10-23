"""Native B camera fields, real dropdowns/ranges, preserved runner and source."""
import argparse,json,math,pathlib,re,sys
from check_display_workflow import Workflow
from check_graphic_raster import png
from check_transport_design import pixels
from check_inspector_design import rect

def inspect(folder,out,mode):
    records=[]
    for line in out.splitlines():
        if line.startswith('CAMERA DESIGN kind='):
            r={k:re.search(r'\b'+k+r'=([^ ]+)',line)[1] for k in ('kind','variant','size','preserved','mode','plane','projection','yaw','pitch','zoom')}
            for k in ('size','preserved','mode','plane','projection'):r[k]=int(r[k])
            for k in ('yaw','pitch','zoom'):r[k]=float(r[k])
            for k in ('header','body','save','mode-rect'):r[k]=rect(line,k)
            r['save_paint']=bytes.fromhex(re.search(r'save-paint=([0-9a-f]*)',line)[1]).decode('utf-8')
            r['fields']=[];r['ranges']=[];records.append(r)
        elif line.startswith('CAMERA FIELD '):records[-1]['fields'].append(dict(rect=rect(line,'rect'),paint=bytes.fromhex(re.search(r'paint=([0-9a-f]*)',line)[1]).decode('utf-8')))
        elif line.startswith('CAMERA RANGE '):records[-1]['ranges'].append(dict(rect=rect(line,'rect'),output=rect(line,'output'),paint=bytes.fromhex(re.search(r'paint=([0-9a-f]*)',line)[1]).decode('utf-8')))
    assert len(records)==60,(mode,len(records))
    assert out.count('CAMERA CANCEL disabled-menu-closed=1 stale-option-rejected=1 source-and-run=1')==1
    actions=re.findall(r'CAMERA ACTIONS variant=(\w+) preset-pointer=1 free-escape=1 plane-keyboard=1 disabled-orbit=1 zoom-home-end=1 orbit-pointer=1 orbit-negative-home-end=1 canonical-drag-continued=1 source-and-run=1',out);assert len(actions)==6
    field_heights={}
    for r in records:
        f=r['size']/16;header,body=r['header'],r['body'];assert r['preserved']==1 and len(r['fields'])==2 and len(r['ranges'])==3
        assert abs(header[2]-(320 if r['variant'] in ('dark','light') else 208))<.02
        im=png(folder/f"camera-{r['variant']}-{r['kind']}.png");light=r['variant'] in ('light','larger','large');contrast=r['variant']=='contrast'
        text=(255,255,255) if contrast else (32,36,44) if light else (237,238,242);muted=(221,221,221) if contrast else (96,103,115) if light else (155,158,168);accent=(255,225,0) if contrast else (23,86,158) if light else (138,186,255)
        def visible(b):return b[1]>=body[1] and b[1]+b[3]<=body[1]+body[3]
        for i,field in enumerate(r['fields']):
            b=field['rect'];assert abs(b[0]-header[0]-17)<1.01 and abs(b[0]+b[2]-header[0]-header[2]+16)<1.01
            assert b[3]>=32*f-.01
            key=(r['variant'],i);field_heights.setdefault(key,b[3]);assert abs(field_heights[key]-b[3])<.02,('camera fields move during preset-to-free drag',mode,key,field_heights[key],b[3])
            if visible(b) and r['kind']!='menu':
                value='Eigene Ansicht' if r['kind']=='top' and i==0 else ('XY','YZ','XZ')[r['plane']] if i==1 else None
                if value is not None:assert ''.join(field['paint'].split())==''.join(value.split()),('field text commands',mode,r['variant'],r['kind'],i,field)
                # The plane is visibly disabled before 2D is enabled.
                if i==0 or r['mode']:assert pixels(im,(b[0]+6*f,b[1]+2*f,b[2]-32*f,b[3]-4*f),text,55),('empty field',mode,r)
        if r['kind']=='top':assert visible(r['fields'][0]['rect']) and not r['mode'] and abs(r['yaw']-.31)<1e-6 and abs(r['pitch']+.28)<1e-6
        if r['kind']=='check-focus':
            b=r['mode-rect'];assert visible(b)
            assert pixels(im,(b[0]-4*f,b[1]+5*f,2*f,8*f),accent,3),('checkbox outer focus clipped',mode,r['variant'])
        if r['kind']=='plane':assert r['mode'] and r['plane']==2 and abs(r['pitch']-math.pi/2)<1e-6
        if r['kind'] in ('zoom-max','zoom-min','orbit','orbit-min','orbit-drag'):
            for i,axis in enumerate(r['ranges']):
                b,o=axis['rect'],axis['output'];assert visible(b) and visible(o),('range not fully visible',mode,r['variant'],axis,body)
                assert abs(b[3]-16*f)<.02 and o[0]>=b[0]-.02 and o[0]+o[2]<=b[0]+b[2]+.02
                value=r[('yaw','pitch','zoom')[i]];expected=(f'{value:.1f} m' if i==2 else f'{value:.2f} rad').replace('.',',')
                assert axis['paint']==expected,('numeric output commands',mode,r['variant'],r['kind'],axis,expected)
                assert pixels(im,o,muted,55),('numeric output blank',mode,r['variant'],axis)
            if r['kind'] in ('zoom-max','zoom-min'):
                assert r['mode'] and r['zoom']==(12 if r['kind']=='zoom-max' else 1.5)
                b=r['ranges'][2]['rect'];x=b[0]+b[2]-8*f if r['kind']=='zoom-max' else b[0]+8*f;y=b[1]+b[3]/2
                assert pixels(im,(x-2*f,y-6*f,4*f,2*f),accent,3),('endpoint thumb outside input',mode,r['variant'],r['kind'])
                assert pixels(im,(b[0]-4*f,b[1],2*f,b[3]),accent,3),('range outer focus clipped',mode,r['variant'],r['kind'])
            elif r['kind']=='orbit':assert not r['mode'] and 1.8<r['yaw']<2.5
            elif r['kind']=='orbit-min':assert not r['mode'] and abs(r['yaw']+3.14)<1e-6
            else:assert not r['mode'] and 1.8<r['yaw']<2.5
        if r['kind']=='save':
            assert visible(r['save']) and r['save'][3]>=35.4*f-.02
            assert ''.join(r['save_paint'].split())=='AlsProjektvorgabespeichern'
            assert pixels(im,r['save'],text,55),('save caption blank',mode,r['variant'])
    return dict(records=records,actions=actions)

def main():
    for stream in (sys.stdout,sys.stderr):
        if hasattr(stream,'reconfigure'):stream.reconfigure(encoding='utf-8',errors='replace')
    p=argparse.ArgumentParser();p.add_argument('--app',type=pathlib.Path,required=True);p.add_argument('--root',type=pathlib.Path,required=True);p.add_argument('--work',type=pathlib.Path,required=True);a=p.parse_args();a.work.mkdir(parents=True,exist_ok=True)
    flow=Workflow(a.app,a.work,a.root,False);flow.env['SDL_FORCE_RAISEWINDOW']='1';checks=[]
    for mode in ('system','bundled'):
        if mode=='bundled':flow.env['PHYSIM_TEST_BUNDLED_FONTS']='1'
        else:flow.env.pop('PHYSIM_TEST_BUNDLED_FONTS',None)
        folder=a.work/mode;folder.mkdir();flow.run('--workspace-state-test',folder,'camera-design',timeout=190,marker='CAMERA DESIGN SELF-TEST: PASSED');checks.append(dict(font_mode=mode,**inspect(folder,flow.steps[-1]['stdout'],mode)))
        assert len(list((folder/'project/runs').glob('*.psrun')))==1 and (folder/'project/main.c').read_bytes()==(a.root/'examples/pendulum/main.c').read_bytes()
    (a.work/'validation.json').write_text(json.dumps(dict(scope=__doc__,checks=checks),ensure_ascii=False,indent=2)+'\n',encoding='utf-8');print('CAMERA DESIGN: PASSED')
if __name__=='__main__':main()
