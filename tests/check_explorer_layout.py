"""Approved CSS versus actual native text commands, card layout and raster ink."""
import argparse,hashlib,json,os,re,subprocess
from pathlib import Path
from check_graphic_raster import png
p=argparse.ArgumentParser(description=__doc__)
for key in ('app','root','work'):p.add_argument('--'+key,type=Path,required=True)
p.add_argument('--mode',choices=('dark','light','contrast','large','compact','large-compact'))
a=p.parse_args();a.work.mkdir(parents=True,exist_ok=True);a.app=a.app.resolve()
css=(a.root/'docs/visuals/design-options/options.css').read_text(encoding='utf8')
base={s.strip():b for s,b in re.findall(r'([^{}]+)\{([^{}]*)\}',css[:css.index('@media')])}
compact_rules={s.strip():b for s,b in re.findall(r'([^{}]+)\{([^{}]*)\}',css[css.index('@media(max-width:560px)'):])}
def val(selector,key):return re.search(r'(?:^|;)'+re.escape(key)+r':([^;]+)',base[selector])[1]
def compact_val(selector,key):return re.search(r'(?:^|;)'+re.escape(key)+r':([^;]+)',compact_rules[selector])[1]
def number(selector,key):return float(val(selector,key).removesuffix('px'))
def rem(selector):return float(val(selector,'font-size').removesuffix('rem'))*14
line=float(re.search(r'/([\d.]+)',val('body','font'))[1]);name_pitch=rem('.file-entry')*float(val('.file-entry .file-name','line-height'))
small=rem('.file-entry small');small_pitch=small*line
border=float(val('.file-entry','border').split()[0][:-2]);margin=number('.file-entry','margin-bottom');gap=number('.file-entry','gap')
icon=number('svg.icon','width');grid_icon=number('.file-grid .file-entry .icon','width');icon_margin=number('.file-grid .file-entry .icon','margin-bottom')
minimum=number('.file-grid .file-entry','min-height');header_h=small_pitch+2*float(val('.file-list-header','padding').split()[0][:-2])
binary=hashlib.sha256(a.app.read_bytes()).hexdigest();cases=[]
def near(x,y,message):assert abs(x-y)<.08,(message,x,y)
def inside(r,x,y):return r[0]-.1<=x<=r[0]+r[2]+.1 and r[1]-.1<=y<=r[1]+r[3]+.1
for mode in ([a.mode] if a.mode else ('dark','light','contrast','large','compact','large-compact')):
    work=a.work/mode;work.mkdir();cmd=[str(a.app),'--workspace-state-test',str(work),'explorer-a-layout-'+mode]
    result=subprocess.run(cmd,capture_output=True,timeout=90,env=dict(os.environ,PHYSIM_TEST_TRACE='1'))
    (work/'stdout.bin').write_bytes(result.stdout);(work/'stderr.bin').write_bytes(result.stderr)
    frames=[json.loads(s) for s in result.stdout.decode('utf8',errors='replace').splitlines() if s.startswith('{"kind":"explorer_layout"')]
    cases.append(dict(mode=mode,command=cmd,exit_code=result.returncode,frames=frames))
    (a.work/'verification.json').write_text(json.dumps(dict(binary_sha256=binary,cases=cases),indent=2)+'\n')
    assert result.returncode==0 and b'EXPLORER LAYOUT SELF-TEST: PASSED' in result.stdout,(mode,result.stderr.decode('utf8',errors='replace'))
    assert [f['phase'] for f in frames]==['list','preview','grid','footer']
    for frame in frames:
        for t in frame['texts']:t['text']=bytes.fromhex(t['hex']).decode('utf8')
    listing,preview,grid,footer=frames;f=grid['scale'];compact=grid['compact'];em=grid['regular_em']
    assert compact==('compact' in mode)
    if compact:assert grid['window'][0]==320,'Minimum native client width not exercised'
    for frame in (listing,grid):
        headers=[t for t in frame['texts'] if t['text']=='Name' and inside(frame['list'],t['box'][0],t['box'][1])]
        assert len(headers)==(0 if compact else 1),'Native grid/list header differs from CSS'
        if headers:near(headers[0]['box'][0]-frame['list'][0],10*f,'Header inset')
    for row in listing['rows']:
        names=[t for t in listing['texts'] if t['role']==3 and inside(row['bounds'],t['box'][0],t['box'][1])]
        if not names:continue
        if ''.join(t['text'] for t in names)!=row['name'] and not row['name'].startswith('000_'):continue
        assert ''.join(t['text'] for t in names)==row['name'],'List filename glyph commands are incomplete'
        n=len(names);r=row['bounds'];near(r[0],listing['list'][0],'List has extra side inset')
        rule=compact_val if compact else val;pad_y,pad_x=[float(v[:-2]) for v in rule('.file-entry','padding').split()]
        icon_column=float(rule('.file-entry','grid-template-columns').split()[0][:-2])
        near(names[0]['box'][0]-r[0],(border+pad_x+icon_column+gap)*f,'Filename column')
        near(r[2],listing['content_width'],'File row width')
        height=max(icon,name_pitch*n)+(gap+small_pitch if compact else 0)+2*(pad_y+border)
        near(r[3],height*f,'File row content/padding')
    rule=compact_val if compact else val
    grid_pad=float(rule('.file-grid','padding')[:-2])*f;grid_gap=float(rule('.file-grid','gap')[:-2])*f
    py,px=tuple(float(v[:-2]) for v in rule('.file-grid .file-entry','padding').split())
    long_rows=0;pixel_rows=0;grid_heights={};centering_errors=[]
    image=work/'explorer-layout-grid.png';raster=png(image) if image.exists() else None
    for row in grid['rows']:
        r=row['bounds'];texts=[t for t in grid['texts'] if inside(r,t['box'][0],t['box'][1])]
        names=[t for t in texts if t['role']==3];kinds=[t for t in texts if t['role']==4]
        if not names:continue
        if ''.join(t['text'] for t in names)!=row['name'] and not row['name'].startswith('000_'):continue
        assert ''.join(t['text'] for t in names)==row['name'],'Filename glyph commands are incomplete'
        assert len(kinds)==1,'Grid kind missing'
        near(r[2],(grid['content_width']-2*grid_pad-(grid['columns']-1)*grid_gap)/grid['columns'],'Grid card width')
        column=round((r[0]-grid['list'][0]-grid_pad)/(r[2]+grid_gap))
        near(r[0]-grid['list'][0],grid_pad+column*(r[2]+grid_gap),'Grid padding/gap')
        key=round(r[1],3);intrinsic=(2*border+2*py+grid_icon+icon_margin+2*gap+len(names)*name_pitch+small_pitch)*f
        old=grid_heights.get(key,(0,r[3]));grid_heights[key]=(max(old[0],minimum*f,intrinsic),r[3])
        for t in names+kinds:
            if abs(t['box'][0]+t['advance']/2-r[0]-r[2]/2)>=.08:centering_errors.append(t['text'])
            near(t['box'][3],(rem('.file-entry') if t['role']==3 else small)*em*f,'Grid font role')
        near(names[0]['box'][1]-r[1],(border+py+grid_icon+icon_margin+gap)*f+(name_pitch*f-names[0]['box'][3])/2,'Grid text top')
        near(kinds[0]['box'][1]-names[0]['box'][1],len(names)*name_pitch*f+gap*f+(names[0]['box'][3]-kinds[0]['box'][3]+(small_pitch-name_pitch)*f)/2,'Grid kind flow')
        for before,after in zip(names,names[1:]):near(after['box'][1]-before['box'][1],name_pitch*f,'Filename line pitch')
        if row['name'].startswith('000_'):
            long_rows+=1;assert len(names)>1
            near(r[1]-grid['list'][1],(0 if compact else header_h)*f+grid_pad,'Grid header and top padding')
        if raster:
            w,h,rgb,_=raster
            for t in names:
                box=t['box'];clip=t['clip'];points=[]
                if box[1]<max(0,clip[1]) or box[1]+box[3]>min(h,clip[1]+clip[3]):continue
                for y in range(max(0,int(box[1])),min(h,int(box[1]+box[3])+1)):
                    for x in range(max(0,int(r[0]+(px+border)*f)),min(w,int(r[0]+r[2]-(px+border)*f))):
                        if not inside(clip,x+.5,y+.5):continue
                        pixel=rgb[(y*w+x)*3:(y*w+x)*3+3]
                        if max(abs(pixel[i]-t['color'][i]) for i in range(3))<50:points.append((x,y))
                assert points,'No native filename pixels'
                center=(min(x for x,y in points)+max(x for x,y in points)+1)/2
                assert abs(center-(r[0]+r[2]/2))<=3*f,('Grid raster is not centered',mode,t['text'],center,r)
                pixel_rows+=1
    assert long_rows==1,'Wrapped long card was not observed'
    assert not centering_errors,('Grid text is not centered',centering_errors)
    for expected,actual in grid_heights.values():near(actual,expected,'Grid card height is not driven by full flex content')
    if raster:assert pixel_rows>=1;cases[-1]['raster_sha256']=hashlib.sha256(image.read_bytes()).hexdigest()
    props=[t for t in preview['texts'] if t['text'] in ('Typ','Größe','Ort','Status') and inside(preview['preview'],t['box'][0],t['box'][1])]
    assert props,'Preview labels not observed'
    for t in props:
        assert t['role']==3;near(t['box'][3],rem('.property-list>div')*em*f,'Property label font')
        following=preview['texts'][preview['texts'].index(t)+1]
        assert following['role']==3,'Property value font differs from the label'
        near(following['box'][1]-t['box'][1],(rem('.property-list>div')*line+2)*f,'Property label/value flow')
    counts=[t for t in footer['texts'] if re.match(r'\d+ Einträge',t['text']) and t['role']==12]
    assert counts,'Footer text missing'
    for t in counts:near(t['box'][3],rem('.browser-footer')*em*f,'Footer font')
    if mode!='contrast':
        color=val(':root[data-theme=light]' if mode=='light' else ':root','--muted').lstrip('#')
        expected=[int(color[i:i+2],16) for i in (0,2,4)]
        assert all(t['color']==expected for t in footer['texts'] if t['role']==12),'Footer controls do not inherit the approved muted color'
    assert hashlib.sha256(a.app.read_bytes()).hexdigest()==binary
(a.work/'verification.json').write_text(json.dumps(dict(binary_sha256=binary,cases=cases),indent=2)+'\n')
print('Explorer layout: approved CSS rows/grid/metadata/fonts and actual centered native raster passed')
