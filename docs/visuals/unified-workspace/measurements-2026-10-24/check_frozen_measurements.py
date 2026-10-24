"""Verify frozen native measurement evidence and the actual rejected pixel cases."""
import gzip,hashlib,json,pathlib,re,sys
from PIL import Image
HERE=pathlib.Path(__file__).resolve().parent;ROOT=HERE.parents[3];sys.path.insert(0,str(ROOT/'tests'))
from check_graphic_raster import png
from check_transport_design import pixels
from check_inspector_design import rect
def read(name):return json.loads(gzip.decompress((HERE/name).read_bytes()))
def main():
    for name,r in json.loads((HERE/'artifacts.json').read_text(encoding='utf-8')).items():
        raw=(HERE/name).read_bytes();assert len(raw)==r['bytes'] and hashlib.sha256(raw).hexdigest()==r['sha256'],name
    v=json.loads((HERE/'validation.json').read_text(encoding='utf-8'));assert len(v['product_groups'])==6 and all(g['status']=='passed' for g in v['product_groups'])
    assert read('source3.json.gz')['source']==read('source4.json.gz')['source']
    checks=read('linux3-measurement_design-validation.json.gz')['checks'];assert len(checks)==2 and all(len(c['records'])==30 for c in checks)
    before=json.loads((HERE/'rounded-line-before.json').read_text(encoding='utf-8'))
    # This is the RGB-identical BMP conversion, encoded with Pillow's adaptive
    # PNG filters. The native PNG reader deliberately accepts filter zero only.
    old_image=Image.open(HERE/'source1-rounded-line.png').convert('RGB');old=(*old_image.size,old_image.tobytes(),None);columns={}
    assert hashlib.sha256(old[2]).hexdigest()==before['rgb_sha256']
    for x,ys in before['columns'].items():
        actual=[]
        for y in range(105,126):
            r,g,b=old[2][(y*old[0]+int(x))*3:(y*old[0]+int(x))*3+3]
            if b>r+40 and b>g+20:actual.append(y)
        assert actual==ys;columns[x]=len(actual)
    assert min(columns.values())>=6
    current=next(r for r in checks[0]['records'] if r['variant']=='dark' and r['kind']=='normal');im=png(HERE/'linux3-system-dark-normal.png');x,y,w,h=current['metrics'][0]['rule'];bands=[]
    for fraction in (.2,.5,.8):
        xx=int(x+w*fraction);bands.append(sum(any(abs(im[2][(yy*im[0]+xx)*3+i]-(30,31,35)[i])>35 for i in range(3)) for yy in range(int(y)-6,int(y+h)+7)))
    assert all(1<=n<=int(h)+3 for n in bands),bands
    steps=read('linux2-measurement_design-steps.json.gz');out=steps[0]['stdout'];converted=False;old_rect=None
    for line in out.splitlines():
        if line.startswith('MEASUREMENT kind=converted variant=dark '):converted=True
        elif converted and line.startswith('METRIC slot=0 '):old_rect=rect(line,'unit-rect');break
    assert old_rect is not None
    old_unit=len(pixels(png(HERE/'source2-measurement-dark-converted.png'),old_rect,(166,169,177),45))
    actual=next(r for r in checks[0]['records'] if r['variant']=='dark' and r['kind']=='converted')['metrics'][0]
    new_unit=len(pixels(png(HERE/'linux3-system-dark-converted.png'),actual['unit-rect'],(166,169,177),45))
    assert old_unit==0 and new_unit>0 and actual['unit']=='mrad/s'
    frames=json.loads((HERE/'all-frame-hashes.json').read_text(encoding='utf-8'));assert len(frames)==60
    cleanup=json.loads((HERE/'cleanup.json').read_text(encoding='utf-8'));assert sum(c['Images'] for c in cleanup)==440 and sum(c['BytesFreed'] for c in cleanup)==601808746
    print(json.dumps(dict(artifacts='verified',product_groups=6,native_metric_frames=60,old_rule_thickness=columns,new_rule_thickness=bands,old_converted_unit_pixels=old_unit,new_converted_unit_pixels=new_unit,whole_ui_complete=False)))
if __name__=='__main__':main()
