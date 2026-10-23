"""Native selected-object header and B property grid with real runner selection."""
import argparse,json,math,pathlib,re,sys
from check_display_workflow import Workflow
from check_graphic_raster import png
from check_transport_design import pixels,pixel

def rect(line,name):return tuple(map(float,re.search(r'\b'+name+r'=([\d.,-]+)',line)[1].split(',')))
def inspect(folder,out,mode):
    records=[]
    for line in out.splitlines():
        if line.startswith('OBJECT DESIGN kind='):
            r={k:re.search(r'\b'+k+r'=([^ ]+)',line)[1] for k in ('kind','variant','size','id','shape','count','preserved','source')}
            for key in ('size','id','shape','count','preserved'):r[key]=int(r[key])
            r['source']=list(map(float,r['source'].split(',')));r['properties']=[]
            for key in ('panel','body','footer','heading','avatar','title'):r[key]=rect(line,key)
            records.append(r)
        elif line.startswith('OBJECT PROPERTY '):
            records[-1]['properties'].append(dict(index=int(re.search(r'index=(\d+)',line)[1]),card=rect(line,'card'),value=rect(line,'value'),present='present=1' in line,text=line.split(' text=',1)[1]))
    assert len(records)==36,(mode,len(records))
    assert out.count('OBJECT ACTIONS hide=1 show-all=1 keyboard-reselect=1 source-and-run=1')==1
    for r in records:
        assert r['preserved']==1 and r['id'] in (3,101,11)
        assert r['count']==(0 if r['kind']=='collapsed' else {3:4,101:0,11:3}[r['id']]) and len(r['properties'])==r['count']
        im=png(folder/f"object-{r['variant']}-{r['id']}-{r['kind']}.png");f=r['size']/16;light=r['variant'] in ('light','larger','large');contrast=r['variant']=='contrast';text=(255,255,255) if contrast else (32,36,44) if light else (237,238,242);accent=(255,225,0) if contrast else (23,86,158) if light else (138,186,255)
        header,body=r['heading'],r['body'];avatar=r['avatar'];title=r['title']
        assert abs(header[0]-r['panel'][0]-17)<1.01 and abs(header[2]-(r['panel'][2]-33))<1.01
        assert abs(avatar[2]-34*f)<.02 and abs(avatar[3]-34*f)<.02
        assert min(abs(title[0]-avatar[0]-44*f),abs(title[0]-avatar[0]))<.02
        if abs(title[0]-avatar[0])<.02:assert abs(title[1]-(avatar[1]+44*f))<.02
        assert abs(title[0]+title[2]-(header[0]+header[2]))<.02
        if r['kind']=='header':
            assert header[1]>=body[1] and header[1]+header[3]<=body[1]+body[3]+1
            assert pixels(im,title,text,45),('selected name missing',mode,r)
            if r['id']==3:assert pixels(im,(avatar[0]+avatar[2]/2-2*f,avatar[1]+avatar[3]/2-2*f,4*f,4*f),accent,2)
        expected=r['source'] if r['id']==3 else r['source'][1:] if r['id']==11 else []
        for p,want in zip(r['properties'],expected):
            c,v=p['card'],p['value'];assert abs(v[0]-c[0]-11*f)<.02 and abs(v[2]-(c[2]-22*f))<.02
            assert abs(v[1]+v[3]-(c[1]+c[3]-10*f))<.02
            fully_visible=v[1]>=body[1] and v[1]+v[3]<=body[1]+body[3]+1
            if fully_visible:
                assert p['present'] and p['text'].endswith('m'),('value/unit commands absent',mode,r,p)
                assert float(p['text'][:-1].strip().replace(',','.'))==float(format(want,'.4g')),(mode,r,p,want)
                assert '.' not in p['text'] and len(pixels(im,v,text,60))>=3,('number ink/localization',mode,r,p)
        if r['count']:
            first=r['properties'][0]['card'];two=r['variant'] in ('dark','light')
            assert abs(first[2]-((header[2]-8*f)/2 if two else header[2]))<.02
            if r['count']>1:
                other=r['properties'][1]['card']
                if two:assert abs(other[0]-(first[0]+first[2]+8*f))<.02 and abs(other[1]-first[1])<.02
                else:assert abs(other[0]-first[0])<.02 and abs(other[1]-(first[1]+first[3]+8*f))<.02
            if r['kind']=='last':
                last=r['properties'][-1]['value'];assert last[1]>=body[1] and last[1]+last[3]<=r['footer'][1]+1
    return records

def main():
    for stream in (sys.stdout,sys.stderr):
        if hasattr(stream,'reconfigure'):stream.reconfigure(encoding='utf-8',errors='replace')
    p=argparse.ArgumentParser();p.add_argument('--app',type=pathlib.Path,required=True);p.add_argument('--root',type=pathlib.Path,required=True);p.add_argument('--work',type=pathlib.Path,required=True);a=p.parse_args();a.work.mkdir(parents=True,exist_ok=True)
    flow=Workflow(a.app,a.work,a.root,False);flow.env['SDL_FORCE_RAISEWINDOW']='1';checks=[]
    for mode in ('system','bundled'):
        if mode=='bundled':flow.env['PHYSIM_TEST_BUNDLED_FONTS']='1'
        else:flow.env.pop('PHYSIM_TEST_BUNDLED_FONTS',None)
        folder=a.work/mode;folder.mkdir();flow.run('--workspace-state-test',folder,'object-design',timeout=130,marker='OBJECT DESIGN SELF-TEST: PASSED')
        checks.append(dict(font_mode=mode,records=inspect(folder,flow.steps[-1]['stdout'],mode)))
        assert len(list((folder/'project/runs').glob('*.psrun')))==1
        assert (folder/'project/main.c').read_bytes()==(a.root/'examples/pendulum/main.c').read_bytes()
    (a.work/'validation.json').write_text(json.dumps(dict(scope=__doc__,checks=checks),ensure_ascii=False,indent=2)+'\n',encoding='utf-8');print('OBJECT DESIGN: PASSED')
if __name__=='__main__':main()
