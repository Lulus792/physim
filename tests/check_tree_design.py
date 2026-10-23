"""Native Messlabor tree layout, keyboard reveal, visibility and shared camera."""
import argparse,json,pathlib,re,sys
from check_display_workflow import Workflow
from check_graphic_raster import png
from check_transport_design import pixels,pixel
from check_inspector_design import rect
TYPES=('Kugel','Linie','Box','Vektor','Punkt','Ebene','Pfad','Beschriftung','Gruppe','Koordinatenrahmen')
def inspect(folder,out,mode):
    records=[]
    for line in out.splitlines():
        if line.startswith('TREE DESIGN kind='):
            r={k:re.search(r'\b'+k+r'=([^ ]+)',line)[1] for k in ('kind','variant','size','preserved','selected','scroll')}
            for k in ('size','preserved','selected','scroll'):r[k]=int(r[k])
            for k in ('header','body','footer','options','left-mode','right-mode'):r[k]=rect(line,k)
            r['rows']=[];r['filters']=[];records.append(r)
        elif line.startswith('TREE ROW '):
            records[-1]['rows'].append(dict(id=int(re.search(r'id=(\d+)',line)[1]),shape=int(re.search(r'shape=(\d+)',line)[1]),row=rect(line,'row'),title=rect(line,'title'),label=bytes.fromhex(re.search(r'label=([0-9a-f]*)',line)[1]).decode('utf-8'),paint=bytes.fromhex(re.search(r'paint=([0-9a-f]*)',line)[1]).decode('utf-8')))
        elif line.startswith('TREE FILTER '):records[-1]['filters'].append(rect(line,'rect'))
    assert len(records)==36,(mode,len(records))
    assert out.count('TREE RESIZE keyboard-home=150 keyboard-end=420 keyboard-enter=190 source-and-run=1')==1
    assert out.count('TREE SEPARATOR menu-blocked=1 keyboard-restored=1')==1
    actions=re.findall(r'TREE ACTIONS variant=(\w+) keyboard-reveal=1 select=1 parent-hide=1 collapse=1 filter-pointer=1 filter-keyboard=1 left-camera=1 right-camera=1 source-and-run=1',out);assert len(actions)==6
    for r in records:
        f=r['size']/16;header,body=r['header'],r['body'];assert r['preserved']==1 and len(r['filters'])==3
        assert abs(header[2]-(190 if r['variant'] in ('dark','light') else 150))<.02 and abs(header[3]-45.4*f)<.02
        im=png(folder/f"tree-{r['variant']}-{r['kind']}.png");light=r['variant'] in ('light','larger','large');contrast=r['variant']=='contrast';text=(255,255,255) if contrast else (32,36,44) if light else (237,238,242);muted=(221,221,221) if contrast else (85,94,108) if light else (166,169,177);accent=(255,225,0) if contrast else (23,86,158) if light else (138,186,255)
        root=next(t for t in r['rows'] if t['id']==100);assert abs(root['row'][0]-header[0]-8)<1.01 and abs(root['row'][2]-header[2]+16)<1.01
        for node in r['rows']:
            row,title=node['row'],node['title']
            if not row[2]:continue
            assert title[0]>=row[0] and title[0]+title[2]<=row[0]+row[2]+.02 and title[1]>=row[1] and title[1]+title[3]<=row[1]+row[3]+.02
            if title[1]>=body[1] and title[1]+title[3]<=body[1]+body[3]:
                expected=node['label'] or f"{TYPES[node['shape']]} · {node['id']}"
                assert ''.join(node['paint'].split())==''.join(expected.split()),('incomplete native label commands',mode,r['variant'],r['kind'],node)
                assert pixels(im,title,muted if r['kind']=='hidden' else text,55),('tree label blank',mode,r['variant'],node)
        if r['kind']=='keyboard':
            node=next(t for t in r['rows'] if t['id']==11);row=node['row'];assert r['selected']==11 and row[1]>=body[1]-.01 and row[1]+row[3]<=body[1]+body[3]+.01,('keyboard selection not revealed',mode,r,node)
            if r['variant'] not in ('dark','light'):assert r['scroll']>0
        if r['kind']=='collapsed':
            assert all(not node['row'][2] for node in r['rows'] if node['id']!=100)
            if r['variant'] in ('dark','light'):assert abs(r['footer'][1]+r['footer'][3]-(body[1]+body[3]))<1.01,('footer not at bottom',mode,r)
        for b in r['filters']:assert abs(b[0]-header[0]-15)<1.01 and abs(b[2]-header[2]+30)<1.01
        if r['kind']=='focus':
            b=r['filters'][1];assert b[1]>=body[1] and b[1]+b[3]<=body[1]+body[3]+1;assert pixels(im,(b[0]-2,b[1]-2,b[2]+4,b[3]+4),accent,2)
        if r['kind']=='camera':
            assert r['left-mode'][0]<header[0]+header[2] and r['right-mode'][0]>header[0]+header[2]
    return dict(records=records,actions=actions)
def main():
    for stream in (sys.stdout,sys.stderr):
        if hasattr(stream,'reconfigure'):stream.reconfigure(encoding='utf-8',errors='replace')
    p=argparse.ArgumentParser();p.add_argument('--app',type=pathlib.Path,required=True);p.add_argument('--root',type=pathlib.Path,required=True);p.add_argument('--work',type=pathlib.Path,required=True);a=p.parse_args();a.work.mkdir(parents=True,exist_ok=True)
    flow=Workflow(a.app,a.work,a.root,False);flow.env['SDL_FORCE_RAISEWINDOW']='1';checks=[]
    for mode in ('system','bundled'):
        if mode=='bundled':flow.env['PHYSIM_TEST_BUNDLED_FONTS']='1'
        else:flow.env.pop('PHYSIM_TEST_BUNDLED_FONTS',None)
        folder=a.work/mode;folder.mkdir();flow.run('--workspace-state-test',folder,'tree-design',timeout=130,marker='TREE DESIGN SELF-TEST: PASSED');checks.append(dict(font_mode=mode,**inspect(folder,flow.steps[-1]['stdout'],mode)))
        assert len(list((folder/'project/runs').glob('*.psrun')))==1 and (folder/'project/main.c').read_bytes()==(a.root/'examples/pendulum/main.c').read_bytes()
    (a.work/'validation.json').write_text(json.dumps(dict(scope=__doc__,checks=checks),ensure_ascii=False,indent=2)+'\n',encoding='utf-8');print('TREE DESIGN: PASSED')
if __name__=='__main__':main()
