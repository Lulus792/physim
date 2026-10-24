"""Controlled native curve edge cases above an unchanged real paused project."""
import argparse,json,pathlib,re,sys
from check_display_workflow import Workflow
from check_graphic_raster import png
from check_transport_design import pixels
from check_inspector_design import rect

def inspect(folder,out,mode):
    records=[]
    messages={'empty':'Für eine Kurve sind mindestens zwei gültige Messpunkte nötig.','one':'Für eine Kurve sind mindestens zwei gültige Messpunkte nötig.','missing':'Keine gültigen Messwerte in dieser Vorschau.','unrepresentable':'Anzeigeeinheit nicht darstellbar.'}
    compact=lambda x:''.join(x.split())
    for line in out.splitlines():
        if not line.startswith('CURVE CASE '):continue
        r={k:re.search(r'\b'+k+r'=([^ ]+)',line)[1] for k in ('variant','kind','size','preserved','segments','stroke','header','hover')}
        for k in ('size','preserved','segments','hover'):r[k]=int(r[k])
        r['header']=float(r['header']);r['stroke']=float(r['stroke'])
        for k in ('head','name','unit','frame','area','note','message'):r[k]=rect(line,k)
        for i in range(3):r[f'paint{i}']=bytes.fromhex(re.search(r'\bpaint'+str(i)+r'=([0-9a-f]*)',line)[1]).decode('utf-8')
        records.append(r)
    assert len(records)==48,(mode,len(records))
    for r in records:
        assert r['preserved'] and r['hover']==(r['kind']=='hover');f=r['size']/16
        image=png(folder/f"curve-{r['variant']}-{r['kind']}.png")
        light=r['variant'] in ('light','larger','large');contrast=r['variant']=='contrast'
        ink=(255,255,255) if contrast else (32,36,44) if light else (237,238,242)
        muted=(221,221,221) if contrast else (85,94,108) if light else (166,169,177)
        expected_name='Vollständiger Kanalname mit langen Metadaten' if r['kind']=='long' else 'Messkanal'
        expected_unit='kg·m²/(A·s³) · vollständige Anzeigeeinheit' if r['kind']=='long' else 'm/s'
        assert compact(r['paint0'])==compact(expected_name) and compact(r['paint1'])==compact(expected_unit),(mode,r)
        assert pixels(image,r['name'],ink,40) and pixels(image,r['unit'],muted,40)
        assert abs(r['unit'][0]+r['unit'][2]-r['head'][0]-r['head'][2])<.03
        for key in ('name','unit'):
            b=r[key];h=r['head'];assert b[0]>=h[0]-.03 and b[0]+b[2]<=h[0]+h[2]+.03 and b[1]+b[3]<=h[1]+h[3]+.03
        if r['kind']!='long':
            # Static reference captions have no chevron or filled hover surface.
            x,y,w,h=r['name'];right=r['unit'][0]
            gap=(x+w+2*f,y,min(16*f,right-x-w-4*f),h)
            assert gap[2]>0 and not pixels(image,gap,muted,35),('extra header chrome',mode,r)
            bg=(0,0,0) if contrast else (246,247,249) if light else (30,31,35)
            hx,hy,hw,hh=r['head'];probe=(hx+hw/2,hy+f,1,1)
            assert pixels(image,probe,bg,0),('filled chart hover',mode,r)
        if r['kind'] in messages:
            assert r['segments']==0 and compact(r['paint2'])==compact(messages[r['kind']]),(mode,r)
            assert pixels(image,r['message'],muted,40)
            assert r['message'][1]+r['message'][3]<=r['frame'][1]+r['frame'][3]+.03
        else:
            assert r['segments']==(2 if r['kind']=='gaps' else 5),(mode,r)
            assert abs(r['stroke']-2*f)<.0001,('float stroke rounded',mode,r)
            assert r['area'][3]>=48*f-2 and not r['message'][2]
            color=(140,200,255) if contrast else (23,86,158) if light else (138,186,255)
            assert len(pixels(image,r['area'],color,12))>=20,('curve or palette missing',mode,r)
            if r['kind']=='gaps':
                x,y,w,h=r['area'];assert not pixels(image,(x+w*.43,y+2,w*.14,max(1,h-4)),color,12),('status gap bridged',mode,r)
            if r['kind']=='offset':assert r['note'][3]>0 and r['note'][1]+r['note'][3]<r['area'][1] and pixels(image,r['note'],muted,40)
    return dict(records=records)

def main():
    for s in (sys.stdout,sys.stderr):
        if hasattr(s,'reconfigure'):s.reconfigure(encoding='utf-8',errors='replace')
    p=argparse.ArgumentParser();p.add_argument('--app',type=pathlib.Path,required=True);p.add_argument('--root',type=pathlib.Path,required=True);p.add_argument('--work',type=pathlib.Path,required=True);a=p.parse_args();a.work.mkdir(parents=True,exist_ok=True)
    flow=Workflow(a.app,a.work,a.root,False);flow.env['SDL_FORCE_RAISEWINDOW']='1';checks=[]
    for mode in ('system','bundled'):
        if mode=='bundled':flow.env['PHYSIM_TEST_BUNDLED_FONTS']='1'
        else:flow.env.pop('PHYSIM_TEST_BUNDLED_FONTS',None)
        folder=a.work/mode;folder.mkdir();flow.run('--workspace-state-test',folder,'curve-design',timeout=190,marker='CURVE DESIGN SELF-TEST: PASSED');checks.append(dict(font_mode=mode,**inspect(folder,flow.steps[-1]['stdout'],mode)))
        assert len(list((folder/'project/runs').glob('*.psrun')))==1 and (folder/'project/main.c').read_bytes()==(a.root/'examples/pendulum/main.c').read_bytes()
    (a.work/'validation.json').write_text(json.dumps(dict(scope=__doc__,checks=checks),ensure_ascii=False,indent=2)+'\n',encoding='utf-8');print('CURVE DESIGN: PASSED')
if __name__=='__main__':main()
