"""Native B limit fields, actual event cards and recording status."""
import argparse,json,pathlib,sys,re,struct,zlib
from check_display_workflow import Workflow
from check_graphic_raster import png
from check_transport_design import pixels,pixel

def hextext(line,key):return bytes.fromhex(re.search(r'\b'+key+r'=([0-9a-f]*)',line)[1]).decode('utf-8')
def rect(line,key):return tuple(map(float,re.search(r'(?:^|\s)'+re.escape(key)+r'=([\d.,-]+)',line)[1].split(',')))
def inside(r,body):return r[2]>0 and r[3]>0 and r[1]>=body[1]+1 and r[1]+r[3]<=body[1]+body[3]-1
def inspect(folder,out,require_empty=True):
    records=[];empty=[]
    for line in out.splitlines():
        if line.startswith('INSPECTOR TAIL kind='):
            assert 'preserved=1' in line and 'count=3' in line and 'status-warning=1' in line,line
            records.append(dict(kind=re.search(r'kind=([\w-]+)',line)[1],variant=re.search(r'variant=(\w+)',line)[1],size=int(re.search(r'size=(\d+)',line)[1]),body=rect(line,'body'),warning=rect(line,'warning'),button=rect(line,'log-button'),limits=[],events=[]))
        elif line.startswith('TAIL LIMIT '):records[-1]['limits'].append(dict(index=int(re.search(r'index=(\d+)',line)[1]),card=rect(line,'card'),label=rect(line,'label'),input=rect(line,'input'),paint=hextext(line,'paint')))
        elif line.startswith('TAIL EVENT '):records[-1]['events'].append(dict(index=int(re.search(r'index=(\d+)',line)[1]),card=rect(line,'card'),heading=rect(line,'heading'),message=rect(line,'message'),paint=hextext(line,'paint')))
        elif line.startswith('TAIL RECORDING '):records[-1]['recording']=dict(row=rect(line,'row'),name=rect(line,'name'),meta=rect(line,'meta'),dot=rect(line,'dot'),name_text=hextext(line,'name-text'),meta_text=hextext(line,'meta-text'),scenes='scenes=1' in line)
        elif line.startswith('TAIL EMPTY '):
            assert 'events=0' in line and 'run-empty=1' in line;kind=re.search(r'kind=(\w+)',line)[1];target=rect(line,'target');assert inside(target,rect(line,'body'))
            image=png(folder/f'tail-empty-{kind}.png');assert pixels(image,target,(166,169,177),55)
            if kind=='recording':assert hextext(line,'name-text')=='Keine Aufzeichnung' and hextext(line,'meta-text')=='Messwerte werden nach dem Start aufgezeichnet.'
            empty.append(dict(kind=kind,target=target))
    assert len(records)==30,len(records)
    if require_empty:assert {r['kind'] for r in empty}=={'events','recording'}
    assert 'INSPECTOR TAIL ACTIONS fraction-blocked=1 negative-blocked=1 comma=1 build-and-analysis-guard=1 keyboard-log=1 source-and-run=1 cross-view=1' in out
    expected=['Eine lange Warnung mit echten Unicode-Zeichen α und mehreren Worten zur Prüfung.\nTabs:\t☃ Ende','Reset "quoted"\t☃','Created: α']
    visible=0
    for r in records:
        f=r['size']/16;image=png(folder/f"tail-{r['variant']}-{r['kind']}.png");light=r['variant'] in ('light','larger','large');contrast=r['variant']=='contrast'
        ink=(255,255,255) if contrast else (32,36,44) if light else (237,238,242);bg=(0,0,0) if contrast else (246,247,249) if light else (30,31,35);accent=(255,225,0) if contrast else (23,86,158) if light else (138,186,255)
        shown=0
        if r['kind'].startswith('limits'):
            dim=tuple((a+b)//2 for a,b in zip(ink,bg))
            for field in r['limits']:
                card,label,entry=field['card'],field['label'],field['input'];assert card[2]>0
                assert abs(entry[0]-card[0]-13*f)<.03 and abs(entry[3]-35*f)<.03
                assert abs(label[1]-card[1]-12*f)<.03 and abs(label[0]-card[0]-13*f)<.03
                if inside(entry,r['body']):assert field['paint']=='0' and len(pixels(image,entry,dim,45))>=3;shown+=1
        elif r['kind'].startswith('events'):
            assert len(r['events'])==3 and r['warning']==r['events'][0]['card']
            for event in r['events']:
                card,heading,message=event['card'],event['heading'],event['message'];assert card[2]>0
                assert abs(heading[0]-card[0]-10*f)<.03 and abs(heading[1]-card[1]-10*f)<.03
                assert abs(message[1]-heading[1]-heading[3]-3*f)<.03 and abs(message[1]+message[3]-(card[1]+card[3]-10*f))<.03
                if inside(card,r['body']):
                    assert ''.join(event['paint'].split())==''.join(expected[event['index']].split()),('lost event text',r,event)
                    assert len(pixels(image,message,ink,50))>=3
                    assert max(abs(a-b) for a,b in zip(pixel(image,card[0]+3*f,card[1]+card[3]/2),bg))<=2
                    shown+=1
        else:
            recording=r['recording'];row,name,meta,dot=(recording[k] for k in ('row','name','meta','dot'));assert inside(row,r['body'])
            assert abs(dot[2]-6*f)<.03 and abs(dot[3]-6*f)<.03 and abs(name[0]-row[0]-15*f)<.03
            assert abs(meta[1]-name[1]-name[3]-4*f)<.03
            assert recording['name_text']==next((folder/'project/runs').glob('*.psrun')).name
            assert recording['meta_text']==('Messwerte und Szene' if recording['scenes'] else 'Messwerte · keine Szene')
            assert pixels(image,name,ink,50) and pixels(image,meta,(221,221,221) if contrast else (85,94,108) if light else (166,169,177),50)
            assert max(abs(a-b) for a,b in zip(pixel(image,dot[0]+dot[2]/2,dot[1]+dot[3]/2),accent))<=2
            shown+=1
        assert shown,('no fully visible target',r);visible+=shown
    runs=list((folder/'project/runs').glob('*.psrun'));assert len(runs)==1
    raw=runs[0].read_bytes();at=16;values=[]
    while at<len(raw):
        kind,length,crc=struct.unpack_from('<III',raw,at);payload=raw[at+12:at+12+length];assert len(payload)==length and zlib.crc32(payload)==crc
        if kind==3:values.append(struct.unpack('<dd',payload))
        at+=12+length
    assert values==[(0,142),(.25,142.25)],values
    return dict(records=records,empty=empty,visible_targets=visible,actual_samples=values)

def main():
    for s in (sys.stdout,sys.stderr):
        if hasattr(s,'reconfigure'):s.reconfigure(encoding='utf-8',errors='replace')
    p=argparse.ArgumentParser();p.add_argument('--app',type=pathlib.Path,required=True);p.add_argument('--root',type=pathlib.Path,required=True);p.add_argument('--work',type=pathlib.Path,required=True);p.add_argument('--baseline',action='store_true');a=p.parse_args();a.work.mkdir(parents=True,exist_ok=True)
    flow=Workflow(a.app,a.work,a.root,False);flow.env['SDL_FORCE_RAISEWINDOW']='1'
    checks=[]
    for font in (('system',) if a.baseline else ('system','bundled')):
        if font=='bundled':flow.env['PHYSIM_TEST_BUNDLED_FONTS']='1'
        else:flow.env.pop('PHYSIM_TEST_BUNDLED_FONTS',None)
        folder=a.work/font;folder.mkdir();flow.run('--workspace-state-test',folder,'inspector-tail-baseline' if a.baseline else 'inspector-tail-design',timeout=190,marker='INSPECTOR TAIL SELF-TEST: PASSED')
        if not a.baseline:
            assert (folder/'project/main.c').read_bytes()==(a.root/'tests/fixtures/inspector_tail.c').read_bytes()
            checks.append(dict(font_mode=font,**inspect(folder,flow.steps[-1]['stdout'])))
    if not a.baseline:(a.work/'validation.json').write_text(json.dumps(dict(scope=__doc__,checks=checks),ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    print('INSPECTOR TAIL BASELINE CAPTURED' if a.baseline else 'INSPECTOR TAIL: PASSED')
if __name__=='__main__':main()
