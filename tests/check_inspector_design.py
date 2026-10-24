"""Native inspector: final CSS headings/cards, keyboard actions and visible glyphs."""
import argparse,json,pathlib,re,sys,math
from check_display_workflow import Workflow
from check_graphic_raster import png
from check_transport_design import pixels,pixel

def rect(line,name):return tuple(map(float,re.search(r'\b'+name+r'=([\d.,-]+)',line)[1].split(',')))

def rule_coverage(im,r,color,background,blank=False):
    delta=tuple(c-b for c,b in zip(color,background));den=sum(d*d for d in delta);results=[]
    for f in (.25,.5,.75):
        x=r[0]+r[2]*f;weights=[]
        for y in range(math.floor(r[1])-1,math.ceil(r[1]+1)+1):
            actual=pixel(im,x,y);alpha=sum(d*(c-b) for d,c,b in zip(delta,actual,background))/den
            assert -.03<=alpha<=1.03 and max(abs(c-(b+alpha*d)) for c,b,d in zip(actual,background,delta))<2.1
            weights.append((y+.5,max(0,alpha)))
        coverage=sum(w for y,w in weights);results.append(coverage)
        if not blank:
            assert abs(coverage-1)<.08,('one-pixel rule coverage',r,coverage)
            assert abs(sum(y*w for y,w in weights)/coverage-(r[1]+.5))<.08,('rule position',r,weights)
    return results

def inspect_native_case(root,folder,out,mode):
    records=[]
    actions=re.findall(r'INSPECTOR ACTIONS variant=(\w+) enter-expand=1 enter-collapse=1 focus-retained=1 pointer-reveal=1 actual-wheel=(\d) source-and-run=1 scroll=(\d+)',out)
    assert len(actions)==6
    assert all((wheel=='1' and int(scroll)>0) if variant not in ('dark','light') else (wheel=='0' and scroll=='0') for variant,wheel,scroll in actions),actions
    for line in out.splitlines():
        if re.match(r'INSPECTOR DESIGN (?:closed(?:-reveal-[0-8])?|card-[01]) variant=',line):
            record=dict(kind=line.split()[2],variant=re.search(r'variant=(\w+)',line)[1],size=int(re.search(r'size=(\d+)',line)[1]),sections=[],cards=[],titles=[])
            assert 'source=1' in line and 'heading-present=1' in line,line
            for key in ('header','body','footer','title','heading-title','heading-meta'):record[key]=rect(line,key)
            record['title_present']='title-present=1' in line;record['minimum']=float(re.search(r'minimum=([\d.]+)',line)[1]);record['wish']=int(re.search(r'wish=(\d+)',line)[1]);records.append(record)
        elif line.startswith('INSPECTOR SECTION '):records[-1]['sections'].append(dict(slot=int(re.search(r'slot=(\d+)',line)[1]),rect=rect(line,'rect')))
        elif line.startswith('INSPECTOR TITLE '):records[-1]['titles'].append(dict(slot=int(re.search(r'slot=(\d+)',line)[1]),parts=[bytes.fromhex(h).decode('utf-8') for h in re.search(r'parts=([0-9a-f,]*)',line)[1].split(',') if h]))
        elif line.startswith('INSPECTOR CARD '):records[-1]['cards'].append(dict(index=int(re.search(r'index=(\d+)',line)[1]),present='value-present=1' in line,card=rect(line,'card'),input=rect(line,'input')))
    assert all(sum(r['variant']==v and r['kind']==k for r in records)==1 for v in ('dark','light','medium','larger','large','contrast') for k in ('closed','card-0','card-1'));seen={v:set() for v in ('dark','light','medium','larger','large','contrast')}
    for r in records:
        im=png(folder/f"inspector-{r['variant']}-{r['kind']}.png");width,height=im[:2];f=r['size']/16
        light=r['variant'] in ('light','larger','large');contrast=r['variant']=='contrast';text=(255,255,255) if contrast else (32,36,44) if light else (237,238,242);border=(255,255,255) if contrast else (190,195,206) if light else (59,61,69);accent=(255,225,0) if contrast else (23,86,158) if light else (138,186,255)
        header,body,footer=r['header'],r['body'],r['footer']
        assert r['wish']==(320 if r['variant'] in ('dark','light') else 208) and abs(header[2]-max(r['wish'],r['minimum']))<.02
        assert abs(header[3]-46.85*f)<.02 and abs(sum(footer[1::2])-(height-148))<1.01,(mode,r)
        assert abs(body[1]-sum(header[1::2]))<1.01 and abs(sum(body[1::2])-footer[1])<1.01
        assert len(r['sections'])==9
        for key,color in (('heading-title',text),('heading-meta',(221,221,221) if contrast else (96,103,115) if light else (155,158,168))):
            h=r[key];assert h[0]>=header[0] and h[1]>=header[1] and h[0]+h[2]<=header[0]+header[2]+1 and h[1]+h[3]<=header[1]+header[3]+1
            assert len(pixels(im,h,color,45))>=3,('heading text missing',mode,r['variant'],key)
        if r['title_present']:assert len(pixels(im,r['title'],text,35))>=3,('summary covered or blank',mode,r)
        labels=['Objekt','Modellwerte','Laufparameter','Laufeinstellungen','Laufgrenzen','Messkanal und Einheit','Ereignisse','Kamera und Darstellung','Aufzeichnung']
        assert len(r['titles'])==9
        for title in r['titles']:
            row=r['sections'][title['slot']]['rect']
            if row[1]<body[1]-.03 or row[1]+row[3]>body[1]+body[3]+.03:continue
            if r['kind'].startswith('closed'):seen[r['variant']].add(title['slot'])
            label=labels[title['slot']];parts=title['parts'];at=0
            assert ''.join(''.join(parts).split())==''.join(label.split()),('missing title',mode,r['variant'],title)
            for part in parts:
                at=label.index(part,at)+len(part)
                assert at==len(label) or label[at].isspace() or label[at-1].isspace(),('split word',mode,r['variant'],label,parts)
        if r['kind']=='closed':
            first=r['sections'][0]['rect'];assert abs(first[3]-42*f)<.02
            assert abs(first[0]-(header[0]+17))<=1.01 and abs(sum(first[::2])-(sum(header[::2])-16))<=1.01,('content inset',mode,r)
            background=(0,0,0) if contrast else (246,247,249) if light else (30,31,35)
            rule_coverage(im,first,border,background)
            assert max(rule_coverage(im,(first[0],first[1]+5*f,first[2],1),border,background,blank=True))<.02
            focus=png(folder/f"inspector-{r['variant']}-focus.png");assert pixels(focus,(first[0]+f,first[1]+first[3]/2-3*f,4*f,6*f),accent,2)
        if r['kind'].startswith('card-'):
            index=int(r['kind'].split('-')[1]);c=next(c for c in r['cards'] if c['index']==index);card,entry=c['card'],c['input'];assert c['present'],('value command absent',mode,r['variant'],index)
            assert abs(card[0]-(header[0]+17))<=1.01 and abs(card[2]-(header[2]-33))<=1.01,('card width/inset',mode,r['variant'],card,header)
            assert abs(entry[0]-card[0]-12*f)<.02 and abs(entry[2]-(card[2]-24*f))<.02 and abs(entry[3]-35*f)<.02
            assert entry[1]>=body[1] and sum(entry[1::2])<=footer[1]+1
            assert abs(sum(entry[1::2])-(sum(card[1::2])-12*f))<.02,('card row/input mismatch',mode,r['variant'],card,entry)
            assert len(pixels(im,entry,text,65))>=3,('blank native value',mode,r['variant'],index)
            assert len(pixels(im,(card[0]-1,card[1]+15*f,3,card[3]-30*f),border,15))>=3,('missing native card outline',mode,r['variant'],index)
    assert all(slots==set(range(9)) for slots in seen.values()),('incomplete title visibility',seen)
    assert len(list((folder/'project/runs').glob('*.psrun')))==1
    assert (folder/'project/main.c').read_bytes()==(root/'examples/pendulum/main.c').read_bytes()
    return dict(records=records,actions=[dict(variant=v,actual_wheel=w=='1',scroll=int(p)) for v,w,p in actions])

def main():
    for stream in (sys.stdout,sys.stderr):
        if hasattr(stream,'reconfigure'):stream.reconfigure(encoding='utf-8',errors='replace')
    p=argparse.ArgumentParser();p.add_argument('--app',type=pathlib.Path,required=True);p.add_argument('--root',type=pathlib.Path,required=True);p.add_argument('--work',type=pathlib.Path,required=True);a=p.parse_args();a.work.mkdir(parents=True,exist_ok=True)
    css=(a.root/'docs/visuals/simulation-designs/inspector-variants.css').read_text(encoding='utf-8')
    assert '.inspector-group summary{font-size:13px;min-height:42px}' in css
    assert 'font-size:19px;min-height:35px;padding:3px 0;border:0;background:transparent' in css
    # The first native functional suite passed despite this real visible failure.
    old=png(a.root/'docs/visuals/unified-workspace/inspector-tree-2026-10-23/negative-card-clip-origin.png')
    assert not pixels(old,(1156,294,40,20),(237,238,242),30)
    flow=Workflow(a.app,a.work,a.root,False);flow.env['SDL_FORCE_RAISEWINDOW']='1';checks=[]
    for mode in ('system','bundled'):
        if mode=='bundled':flow.env['PHYSIM_TEST_BUNDLED_FONTS']='1'
        else:flow.env.pop('PHYSIM_TEST_BUNDLED_FONTS',None)
        folder=a.work/mode;folder.mkdir();flow.run('--workspace-state-test',folder,'inspector-design',timeout=130,marker='INSPECTOR DESIGN SELF-TEST: PASSED');out=flow.steps[-1]['stdout'];records=[]
        result=inspect_native_case(a.root,folder,out,mode)
        checks.append(dict(font_mode=mode,**result))
    (a.work/'validation.json').write_text(json.dumps(dict(scope=__doc__,checks=checks),ensure_ascii=False,indent=2)+'\n',encoding='utf-8');print('INSPECTOR DESIGN: PASSED')
if __name__=='__main__':main()
