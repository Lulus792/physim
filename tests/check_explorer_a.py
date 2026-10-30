"""Selected Explorer A geometry and real file/folder/parent-dialog gestures."""
import argparse,hashlib,json,os,re,subprocess
from pathlib import Path
parser=argparse.ArgumentParser(description=__doc__)
for name in ('app','root','work'):parser.add_argument('--'+name,type=Path,required=True)
args=parser.parse_args();args.work.mkdir(parents=True,exist_ok=True)
css=(args.root/'docs/visuals/design-options/options.css').read_text(encoding='utf8')
rules={selector.strip():body for selector,body in re.findall(r'([^{}]+)\{([^{}]*)\}',css[:css.index('@media')])}
def declaration(selector,name):return re.search(r'(?:^|;)'+re.escape(name)+r':([^;]+)',rules[selector])[1].strip()
def px(selector,name):return float(declaration(selector,name)[:-2])
def rem(selector):return float(declaration(selector,'font-size')[:-3])*14
columns=declaration('.browser-body','grid-template-columns').split()
places=float(columns[0][:-2]);preview=float(columns[-1][:-2])
responsive_places={limit:float(re.search(r'@media\(max-width:'+str(limit)+r'px\).*?\.browser-body[^{}]*\{grid-template-columns:(\d+)px',css)[1]) for limit in (1150,850)}
nav_padding=[float(value[:-3])*14 for value in declaration('.navigation-list button','padding').split()]
nav_gap=px('.navigation-list button','gap');nav_icon=px('svg.icon','width')
nav_border=float(declaration('.navigation-list button','border').split()[0][:-2])
nav_line_height=float(re.search(r'/([\d.]+)',declaration('body','font'))[1])
entry_padding=[float(v[:-2]) for v in declaration('.file-entry','padding').split()]
entry_border=float(declaration('.file-entry','border').split()[0][:-2])
entry_line=float(declaration('.file-entry .file-name','line-height'))
entry_margin=px('.file-entry','margin-bottom')
body_height=float(re.search(r'\.browser-main,\.file-preview\{height:(\d+)px',css)[1])
fonts=[rem('.eyebrow'),rem('.dialog-header h2'),10.5,11.2,rem('.file-list-header'),14.7,rem('.file-preview pre'),rem('.dialog-footer button')]
binary=hashlib.sha256(args.app.read_bytes()).hexdigest();cases=[]
for mode in ('dark','light','contrast','large','narrow','large-narrow'):
    work=args.work/mode;work.mkdir();command=[str(args.app),'--workspace-state-test',str(work),'explorer-a-'+mode]
    result=subprocess.run(command,capture_output=True,timeout=90,env=dict(os.environ,PHYSIM_TEST_TRACE='1'))
    (work/'stdout.bin').write_bytes(result.stdout);(work/'stderr.bin').write_bytes(result.stderr)
    text=result.stdout.decode('utf8',errors='replace');rows=[json.loads(line) for line in text.splitlines() if line.startswith('{"kind":"explorer_a"')]
    clients=[json.loads(line) for line in text.splitlines() if line.startswith('{"kind":"explorer_native_client"')]
    place_rows=[json.loads(line) for line in text.splitlines() if line.startswith('{"kind":"explorer_places"')]
    record=dict(mode=mode,command=command,exit_code=result.returncode,measurements=rows,native_clients=clients,places=place_rows);cases.append(record)
    (args.work/'verification.json').write_text(json.dumps(dict(binary_sha256=binary,cases=cases),indent=2)+'\n',encoding='utf8')
    if result.returncode or 'EXPLORER A SELF-TEST: PASSED' not in text:raise RuntimeError('Native explorer workflow failed: '+mode)
    assert len(rows)==1 and rows[0]['passed'];r=rows[0];f=r['scale']
    assert len(clients)==1 and clients[0]['distinct']
    assert len(place_rows)==1 and len(place_rows[0]['items'])==3
    for item in place_rows[0]['items']:
        button,ink=item['button'],item['ink'];pitch=11.2*nav_line_height*f
        assert abs(ink[0]-button[0]-(nav_border+nav_padding[1]+nav_icon+nav_gap)*f)<.05,'Place text inset differs from approved CSS'
        text_height=ink[3]+pitch-place_rows[0]['font_height']
        expected_height=max(nav_icon*f,text_height)+2*(nav_padding[0]+nav_border)*f
        assert abs(button[3]-expected_height)<.05,'Place button height differs from approved CSS'
    assert abs(r['list'][3]-body_height*f)<.05 and abs(r['preview'][3]-body_height*f)<.05
    assert all(abs(a-b)<.02 for a,b in zip(r['fonts'],fonts)),'Native font role differs from reference'
    window_width=clients[0]['window'][0]
    assert r['modal']==[0,0,*clients[0]['window']]
    expected_places=places
    for limit in (1150,850):
        if window_width<=limit*f:expected_places=responsive_places[limit]
    assert abs(r['list'][0]-expected_places*f)<.05,'Places column differs from approved responsive CSS'
    if window_width<=1150*f:
        assert abs(r['preview'][0]-r['list'][0])<.05 and abs(r['preview'][1]-r['list'][1]-body_height*f)<.05
    else:
        assert abs(r['preview'][0]-r['list'][0]-r['list'][2])<.05 and abs(r['preview'][2]-preview*f)<.05
    expected_row=max(nav_icon,rem('.file-entry')*entry_line)+2*(entry_padding[0]+entry_border)+entry_margin
    assert abs(r['row_pitch']-expected_row*f)<.05,'File row height differs from approved CSS'
    assert hashlib.sha256(args.app.read_bytes()).hexdigest()==binary
print('Explorer A: six themes/sizes, list/grid/search, exclusive folder creation, missing path/back, project B parent selection and actual file open passed')
