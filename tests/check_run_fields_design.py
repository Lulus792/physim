"""Native B parameter/settings cards, locale input and invalid-run guards."""
import argparse,json,pathlib,re,sys
from check_display_workflow import Workflow
from check_graphic_raster import png
from check_transport_design import pixels
from check_inspector_design import rect

def inspect(folder,out):
    records=[]
    for line in out.splitlines():
        if line.startswith('RUN FIELDS kind='):
            r=dict(kind=re.search(r'kind=([\w-]+)',line)[1],variant=re.search(r'variant=(\w+)',line)[1],size=int(re.search(r'size=(\d+)',line)[1]),disabled='disabled=1' in line,body=rect(line,'body'),fields=[])
            assert 'preserved=1' in line,line
            records.append(r)
        elif line.startswith('RUN FIELD index='):
            records[-1]['fields'].append(dict(index=int(re.search(r'index=(\d+)',line)[1]),card=rect(line,'card'),input=rect(line,'input'),label=rect(line,'label'),paint_width=float(re.search(r'paint-width=([\d.]+)',line)[1]),paint=bytes.fromhex(re.search(r'paint=([0-9a-f]*)',line)[1]).decode('utf-8')))
    assert len(records)==50,len(records)
    assert 'RUN FIELDS ACTIONS comma=1 point=1 zero-blocked=1 seed-overflow-blocked=1 source-and-run=1' in out
    visible=0
    for r in records:
        image=png(folder/f"run-fields-{r['variant']}-{r['kind']}.png");f=r['size']/16
        contrast=r['variant']=='contrast';light=r['variant'] in ('light','larger','large')
        ink=(255,255,255) if contrast else (32,36,44) if light else (237,238,242)
        bg=(0,0,0) if contrast else (246,247,249) if light else (30,31,35)
        if r['disabled']:ink=tuple((a+b)//2 for a,b in zip(ink,bg))
        shown=0
        for field in r['fields']:
            card,entry,label=field['card'],field['input'],field['label']
            assert abs(entry[0]-card[0]-13*f)<.03 and abs(entry[3]-35*f)<.03,(r,field)
            assert abs(label[0]-card[0]-13*f)<.03 and abs(label[1]-card[1]-12*f)<.03
            assert entry[0]+entry[2]<=card[0]+card[2]-13*f+.03
            assert label[1]+label[3]<=entry[1]+.03 and entry[1]+entry[3]<=card[1]+card[3]-12*f+.03
            body=r['body']
            if entry[1]>=body[1]+1 and entry[1]+entry[3]<=body[1]+body[3]-1:
                glyphs=pixels(image,entry,ink,65)
                empty_breakpoints=field['index']==4 and 'parameters' not in r['kind'] and 'invalid' not in r['kind']
                if empty_breakpoints:assert not field['paint'] and not glyphs,('empty breakpoint field has extra ink',r,field)
                else:
                    assert field['paint'],('missing native value command',r,field)
                    assert len(glyphs)>=3,('blank native input',r,field)
                    if field['paint_width']+3<entry[2]:assert max(x for x,y in glyphs)<=entry[0]+field['paint_width']+3,('stale or extra native glyphs',r,field)
                visible+=1;shown+=1
        assert shown,('no visible field',r)
    assert len(list((folder/'project/runs').glob('*.psrun')))==1
    return dict(records=records,visible_inputs=visible,actions=dict(comma=True,point=True,zero_blocked=True,seed_overflow_blocked=True,source_and_run_preserved=True))

def main():
    for stream in (sys.stdout,sys.stderr):
        if hasattr(stream,'reconfigure'):stream.reconfigure(encoding='utf-8',errors='replace')
    p=argparse.ArgumentParser();p.add_argument('--app',type=pathlib.Path,required=True);p.add_argument('--root',type=pathlib.Path,required=True);p.add_argument('--work',type=pathlib.Path,required=True);a=p.parse_args();a.work.mkdir(parents=True,exist_ok=True)
    flow=Workflow(a.app,a.work,a.root,False);flow.env['SDL_FORCE_RAISEWINDOW']='1';checks=[]
    for mode in ('system','bundled'):
        if mode=='bundled':flow.env['PHYSIM_TEST_BUNDLED_FONTS']='1'
        else:flow.env.pop('PHYSIM_TEST_BUNDLED_FONTS',None)
        folder=a.work/mode;folder.mkdir();flow.run('--workspace-state-test',folder,'run-fields-design',timeout=190,marker='RUN FIELDS DESIGN SELF-TEST: PASSED')
        assert (folder/'project/main.c').read_bytes()==(a.root/'examples/pendulum/main.c').read_bytes()
        checks.append(dict(font_mode=mode,**inspect(folder,flow.steps[-1]['stdout'])))
    (a.work/'validation.json').write_text(json.dumps(dict(scope=__doc__,checks=checks),ensure_ascii=False,indent=2)+'\n',encoding='utf-8');print('RUN FIELDS DESIGN: PASSED')
if __name__=='__main__':main()
