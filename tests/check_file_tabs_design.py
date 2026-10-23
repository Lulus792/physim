"""Native workbench tabs: real files, keyboard close, mono markers and overflow.

The original browser PNG supplies the Windows standard pill geometry only;
this is not a full typography, DPI or screenreader acceptance.
"""
import argparse,json,math,os,pathlib,re,sys
from check_display_workflow import Workflow
from check_graphic_raster import png

def main():
    p=argparse.ArgumentParser();p.add_argument('--app',type=pathlib.Path,required=True);p.add_argument('--root',type=pathlib.Path,required=True);p.add_argument('--work',type=pathlib.Path,required=True);args=p.parse_args()
    args.work.mkdir(parents=True,exist_ok=True);flow=Workflow(args.app,args.work,args.root,False);flow.env['SDL_FORCE_RAISEWINDOW']='1';checks=[]
    for kind in ('system','bundled'):
        folder=args.work/kind;folder.mkdir()
        if kind=='bundled':flow.env['PHYSIM_TEST_BUNDLED_FONTS']='1'
        else:flow.env.pop('PHYSIM_TEST_BUNDLED_FONTS',None)
        flow.run('--workspace-state-test',folder,'file-tabs-design',timeout=90,marker='FILE TABS DESIGN SELF-TEST: PASSED')
        output=flow.steps[-1]['stdout'];sources=re.findall(r'Workspace marker font: (.+)',output);assert len(sources)==1,sources
        platform=(pathlib.Path(os.environ.get('WINDIR','C:/Windows'))/'Fonts/consolab.ttf') if sys.platform=='win32' else pathlib.Path('/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf')
        expected=platform.name if kind=='system' and platform.is_file() else 'NotoSansMono-SemiBold.ttf'
        assert sources[0].replace('\\','/').endswith('/'+expected),(sources,expected)
        records=[]
        for line in output.splitlines():
            if not line.startswith('TAB DESIGN '):continue
            record={'line':line,'kind':line.split()[2]};record['variant']=re.search(r'variant=(\w+)',line)[1];record['size']=int(re.search(r'size=(\d+)',line)[1]);record['rects']=[tuple(map(float,m.split(','))) for m in re.findall(r'rect\d=([\d.,-]+)',line)]
            record['scroll']=float(re.search(r'scroll=([\d.]+)',line)[1]);record['complete']=int(re.search(r'complete=(\d)',line)[1]);record['end']=int(re.search(r'end=(\d)',line)[1]);records.append(record)
            record['folder']=tuple(map(float,re.search(r'folder=([\d.,-]+)',line)[1].split(',')))
            assert float(re.search(r'marker_css=([\d.]+)',line)[1])==12
        assert [r['kind'] for r in records]==['normal']*4+['long-start','long-end','dirty'],records
        for r in records[:4]:
            f=r['size']/16;boxes=r['rects'];assert abs(boxes[0][3]-28*f)<.001
            assert abs(boxes[1][0]-boxes[0][0]-boxes[0][2]-3*f)<.001
            normal=png(folder/f"tabs-{r['variant']}-normal.png")
            assert normal[:2]==((1440,1000) if r['size']==16 else (1080,740))
            for state in ('hover','focus','close-focus'):assert (folder/f"tabs-{r['variant']}-{state}.png").is_file()
            if r['variant']!='contrast':
                x,y,w,h=boxes[0];accent=(138,186,255) if r['variant']=='dark' else (23,86,158)
                iw,ih,rgb,_=normal;ink=0
                for yy in range(math.ceil(y),math.floor(y+h)):
                    for xx in range(math.ceil(x+11*f),math.ceil(x+19*f)):
                        color=rgb[(yy*iw+xx)*3:(yy*iw+xx)*3+3]
                        ink+=max(abs(a-b) for a,b in zip(color,accent))<55
                assert ink>=3,('no rendered mono marker',kind,r['variant'],ink)
                fx,fy,fw,fh=r['folder'];cx,cy=fx-4,fy+fh/2;muted=(166,169,177) if r['variant']=='dark' else (85,94,108)
                if r['size']==16:assert abs(cx-16)<.001,('folder path-guide alignment',cx)
                disclosure=0
                for yy in range(math.floor(cy-5),math.ceil(cy+5)):
                    for xx in range(math.floor(cx-5),math.ceil(cx+5)):
                        color=rgb[(yy*iw+xx)*3:(yy*iw+xx)*3+3]
                        disclosure+=max(abs(a-b) for a,b in zip(color,muted))<=70
                assert disclosure>0,('no visible disclosure on path guide',kind,r['variant'])
        start,end=records[4:6];assert start['complete']==1 and start['end']==0 and end['complete']==end['end']==1 and end['scroll']>start['scroll']
        project=folder/'project';assert (project/('W'*100+'.phys')).read_text(encoding='utf-8')=='// Datei α\n'
        assert (project/'main.c').read_bytes()==(args.root/'examples/pendulum/main.c').read_bytes()
        assert (project/'physim.project').is_file()
        if sys.platform=='win32' and kind=='system':
            from PIL import Image
            for theme,color in [('dark',(43,61,89)),('light',(217,230,248))]:
                def box(image):
                    w,h,rgb,_=image;points=[(x,y) for y in range(118,157) for x in range(218,400) if tuple(rgb[(y*w+x)*3:(y*w+x)*3+3])==color]
                    assert points;return min(x for x,y in points),min(y for x,y in points),max(x for x,y in points),max(y for x,y in points)
                image=Image.open(args.root/f'docs/visuals/unified-workspace/develop-{theme}.png').convert('RGB')
                native=box(png(folder/f'tabs-{theme}-normal.png'));reference=box((image.width,image.height,image.tobytes(),None))
                assert max(abs(a-b) for a,b in zip(native,reference))<=1,(theme,'original pill geometry',native,reference)
        checks.append({'font_mode':kind,'marker_source':sources[0],'records':records})
    (args.work/'validation.json').write_text(json.dumps({'checks':checks,'scope':__doc__},indent=2)+'\n',encoding='utf-8');print('FILE TABS DESIGN: PASSED')
if __name__=='__main__':main()
