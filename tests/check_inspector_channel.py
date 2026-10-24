"""Native B channel/unit cards, actual viewed-time value and unchanged SI recordings."""
import argparse,json,pathlib,re,sys,math
from check_display_workflow import Workflow
from check_graphic_raster import png
from check_transport_design import pixels
from check_inspector_tail import rect,hextext,inside

def inspect(folder,out):
    records=[]
    for line in out.splitlines():
        if line.startswith('INSPECTOR CHANNEL kind='):
            assert 'preserved=1' in line,line
            records.append(dict(kind=re.search(r'kind=(\w+)',line)[1],variant=re.search(r'variant=(\w+)',line)[1],size=int(re.search(r'size=(\d+)',line)[1]),time=float(re.search(r' time=([^ ]+)',line)[1]),browsing='browsing=1' in line,body=rect(line,'body'),value=rect(line,'value'),hint=rect(line,'hint'),paint=hextext(line,'paint'),font=float(re.search(r'value-font=([^ ]+)',line)[1]),em=float(re.search(r'em=([^ ]+)',line)[1]),fields=[]))
        elif line.startswith('CHANNEL FIELD '):records[-1]['fields'].append(dict(card=rect(line,'card'),label=rect(line,'label'),field=rect(line,'field'),paint=hextext(line,'paint')))
        elif line.startswith('CHANNEL DATA '):records[-1]['data']=dict(selected=int(re.search(r'selected=(\d+)',line)[1]),raw=float(re.search(r'raw=([^ ]+)',line)[1]),scale=float(re.search(r'scale=([^ ]+)',line)[1]),symbol=hextext(line,'symbol'),name=hextext(line,'name'))
    assert len(records)==14,len(records)
    assert 'CHANNEL EMPTY actual=1' in out
    assert 'INSPECTOR CHANNEL ACTIONS keyboard-channel=1 keyboard-unit=1 inline=1 viewed-time=1 custom-escape=1 restore-SI=1 source-and-run=1' in out
    seen_fields=seen_values=0
    for r in records:
        f=r['size']/16;light=r['variant'] in ('light','larger','large');contrast=r['variant']=='contrast'
        ink=(255,255,255) if contrast else (32,36,44) if light else (237,238,242)
        border=(255,255,255) if contrast else (190,195,206) if light else (59,61,69)
        im=png(folder/f"channel-{r['variant']}-{r['kind']}.png")
        assert abs(r['font']/r['em']-24*f)<.002
        assert len(r['fields'])==2
        for i,field in enumerate(r['fields']):
            card,label,control=(field[k] for k in ('card','label','field'))
            assert abs(label[0]-card[0]-13*f)<.03 and abs(label[1]-card[1]-12*f)<.03
            assert abs(control[0]-label[0])<.03 and abs(control[1]-label[1]-label[3]-5*f)<.03
            assert control[3]>=32*f-.03 and abs(card[1]+card[3]-control[1]-control[3]-12*f)<.03
            if inside(card,r['body']):
                assert field['paint']==(r['data']['name'] if i==0 else r['data']['symbol']),(r,field)
                assert pixels(im,control,ink,55),('missing control glyphs',r)
                # A single outer card border; the selector itself has no filled pill.
                assert pixels(im,card,border,6),('missing card border',r)
                seen_fields+=1
        if inside(r['value'],r['body']):
            assert pixels(im,r['value'],ink,50),('missing value glyphs',r)
            value=r['paint'].removesuffix(' '+r['data']['symbol']).replace(',','.')
            converted=r['data']['raw']/r['data']['scale']
            assert math.isclose(float(value),converted,abs_tol=.0051,rel_tol=.0051),r
            assert r['paint'].endswith(' '+r['data']['symbol']),r
            seen_values+=1
    cm,browse=records[:2]
    assert cm['kind']=='centimetres' and cm['data']['scale']==.01 and cm['data']['symbol']=='cm'
    assert browse['kind']=='browse' and browse['browsing'] and browse['time']<cm['time'] and browse['data']['raw']!=cm['data']['raw']
    assert {r['variant'] for r in records if r['kind']=='value'}=={'dark','light','medium','larger','large','contrast'}
    assert seen_fields>=12 and seen_values>=8,(seen_fields,seen_values)
    return dict(records=records,visible_fields=seen_fields,visible_values=seen_values)

def main():
    for s in (sys.stdout,sys.stderr):
        if hasattr(s,'reconfigure'):s.reconfigure(encoding='utf-8',errors='replace')
    p=argparse.ArgumentParser();p.add_argument('--app',type=pathlib.Path,required=True);p.add_argument('--root',type=pathlib.Path,required=True);p.add_argument('--work',type=pathlib.Path,required=True);a=p.parse_args();a.work.mkdir(parents=True,exist_ok=True)
    flow=Workflow(a.app,a.work,a.root,False);flow.env['SDL_FORCE_RAISEWINDOW']='1';checks=[]
    for font in ('system','bundled'):
        if font=='bundled':flow.env['PHYSIM_TEST_BUNDLED_FONTS']='1'
        else:flow.env.pop('PHYSIM_TEST_BUNDLED_FONTS',None)
        folder=a.work/font;folder.mkdir();flow.run('--workspace-state-test',folder,'inspector-channel',timeout=190,marker='INSPECTOR CHANNEL SELF-TEST: PASSED')
        checks.append(dict(font_mode=font,**inspect(folder,flow.steps[-1]['stdout'])))
    (a.work/'validation.json').write_text(json.dumps(dict(scope=__doc__,checks=checks),ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    print('INSPECTOR CHANNEL: PASSED')
if __name__=='__main__':main()
