"""Verify camera proof hashes and endpoint rule against actual old/new pixels."""
import gzip,hashlib,json,pathlib,sys
ROOT=pathlib.Path(__file__).resolve().parents[4]
HERE=pathlib.Path(__file__).resolve().parent
sys.path.insert(0,str(ROOT/'tests'))
from check_graphic_raster import png
from check_transport_design import pixels

def endpoint(image,b,f,maximum):
    x=b[0]+b[2]-8*f if maximum else b[0]+8*f
    y=b[1]+b[3]/2
    return len(pixels(image,(x-2*f,y-6*f,4*f,2*f),(138,186,255),3))

def main():
    for name,record in json.loads((HERE/'artifacts.json').read_text(encoding='utf-8')).items():
        raw=(HERE/name).read_bytes()
        assert len(raw)==record['bytes'] and hashlib.sha256(raw).hexdigest()==record['sha256'],name
    v=json.loads((HERE/'validation.json').read_text(encoding='utf-8'));stage=v['final_source']
    checks=json.loads(gzip.decompress((HERE/f'linux{stage}-camera_design-validation.json.gz').read_bytes()))['checks']
    record=next(r for r in checks[0]['records'] if r['variant']=='dark' and r['kind']=='zoom-max')
    before=json.loads((HERE/'controlled-range.json').read_text(encoding='utf-8'))
    old=endpoint(png(HERE/'source2-controlled-zoom-max.png'),before['range_rect'],1,True)
    new=endpoint(png(HERE/f'linux{stage}-system-dark-zoom-max.png'),record['ranges'][2]['rect'],1,True)
    assert old==0 and new>0,('Endpoint rule must reject original cursor and accept corrected cursor',old,new)
    assert record['zoom']==12 and before['application_exit_code']==1
    old_focus=json.loads((HERE/'clipped-focus.json').read_text(encoding='utf-8'))['range_rect']
    def focus_pixels(im,b):return len(pixels(im,(b[0]-4,b[1],2,b[3]),(138,186,255),3))
    old_outline=focus_pixels(png(HERE/'source4-clipped-focus.png'),old_focus)
    new_outline=focus_pixels(png(HERE/f'linux{stage}-system-dark-zoom-max.png'),record['ranges'][2]['rect'])
    assert old_outline==0 and new_outline>0,(old_outline,new_outline)
    for check in checks:
        for r in check['records']:
            if r['variant'] in ('dark','light'):
                assert all(abs(field['rect'][3]-32*r['size']/16)<.02 for field in r['fields']),('wide CSS field height changed',check['font_mode'],r['kind'])
    previous=json.loads(gzip.decompress((HERE/'linux6-camera_design-validation.json.gz').read_bytes()))['checks'][0]['records']
    def heights(records):return [r['fields'][0]['rect'][3] for r in records if r['variant']=='large' and r['kind'] in ('top','plane')]
    old_heights=heights(previous);new_heights=heights(checks[0]['records'])
    assert len(old_heights)==len(new_heights)==2 and old_heights[0]>old_heights[1]+20 and abs(new_heights[0]-new_heights[1])<.02,(old_heights,new_heights)
    print(json.dumps(dict(artifacts='verified',old_endpoint_pixels=old,new_endpoint_pixels=new,old_focus_pixels=old_outline,new_focus_pixels=new_outline,old_keyboard_pass=False,old_field_heights=old_heights,new_field_heights=new_heights,old_continued_drag_measured=False,final_native_groups=len(v['linux']['groups']),product_groups=len(v['product_groups']),whole_ui_complete=False)))
if __name__=='__main__':main()
