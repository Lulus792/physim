"""Check frozen scene-frame evidence, label counterexample and real window traces."""
import gzip,hashlib,json,pathlib,re,sys
HERE=pathlib.Path(__file__).resolve().parent
ROOT=HERE.parents[3]
sys.path.insert(0,str(ROOT/'tests'))
from check_graphic_raster import png
from check_transport_design import pixels

def read(name):return json.loads(gzip.decompress((HERE/name).read_bytes()))
def overlaps(a,b):return a[0]<b[0]+b[2] and a[0]+a[2]>b[0] and a[1]<b[1]+b[3] and a[1]+a[3]>b[1]
def main():
    for name,r in json.loads((HERE/'artifacts.json').read_text(encoding='utf-8')).items():
        b=(HERE/name).read_bytes();assert len(b)==r['bytes'] and hashlib.sha256(b).hexdigest()==r['sha256'],name
    v=json.loads((HERE/'validation.json').read_text(encoding='utf-8'))
    assert len(v['product_groups'])==13 and all(g['status']=='passed' for g in v['product_groups'])
    assert read('source5.json.gz')['source']==read('source6.json.gz')['source']
    records=read('linux5-scene_frame_design-validation.json.gz')['checks']
    edges=0
    for check in records:
        assert len(check['records'])==54 and len(check['actions'])==6
        for r in check['records']:
            if r['kind']!='label-edge':continue
            e=r['edge'];old,new=e['old'],e['actual']
            assert e['glyph']==1 and overlaps(old,r['text']['select']['rect'])
            assert all(not t['rect'][2] or not overlaps(new,t['rect']) for key,t in r['text'].items() if key in ('label','status','select'))
            assert abs(new[0]-old[0])<.02 and abs(new[1]-old[1]-2*(new[3]+3))<.03,(old,new)
            edges+=1
    assert edges==12
    normal=next(r for r in records[0]['records'] if r['variant']=='dark' and r['kind']=='normal')
    region=normal['text']['view']['rect'];color=(155,158,168)
    before=len(pixels(png(HERE/'before-no-scene-controls.png'),region,color,3))
    after=len(pixels(png(HERE/'linux5-system-dark-normal.png'),region,color,3))
    assert before==0 and after>0,(before,after)
    count=0
    for group in ('docking_workflow','inspector_workflow'):
        steps=read(f'linux5-{group}-steps.json.gz')
        for step in steps:
            out=step.get('stdout','')
            if 'PANEL SCENE STYLE:' not in out:continue
            assert len(re.findall(r'PANEL SCENE STYLE: .*retained=1',out))==1
            windows=re.findall(r'PANEL (\d+): window=(\d+) .*outside=1',out)
            assert len(windows)==6 and len({w[1] for w in windows})==6
            count+=1
    assert count==4,count
    print(json.dumps(dict(artifacts='verified',groups=13,scene_records=108,label_edge_cases=edges,same_width_window_cases=count,old_footer_ink=before,new_footer_ink=after,whole_ui_complete=False)))
if __name__=='__main__':main()
