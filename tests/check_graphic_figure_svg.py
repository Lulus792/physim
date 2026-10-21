"""Independently inspect SVG emitted by the real C figure fixture.

This checks XML and reference coordinates, not font rasterization or native PNG.
"""
import argparse,hashlib,json,math,pathlib,xml.etree.ElementTree as ET

def check(work):
    ns={'s':'http://www.w3.org/2000/svg'}
    palette=[['#17569e','#914800','#00643b','#7844a0'],['#8abaff','#f5b46c','#8bcca5','#d0a6ec']]
    results=[]
    for name in ['figure-0-0.svg','figure-0-1.svg','figure-1-0.svg','figure-1-1.svg','figure-extreme.svg']:
        path=work/name;data=path.read_bytes();root=ET.fromstring(data)
        assert root.tag=='{'+ns['s']+'}svg' and root.attrib['viewBox']=='0 0 1200 850'
        assert (root.attrib['width'],root.attrib['height'])==('1200','850')
        assert root.find('s:title',ns).text=='Auslenkung <&> α'
        assert not root.findall('.//s:script',ns) and not root.findall('.//s:image',ns)
        for e in root.iter():
            for key,value in e.attrib.items():
                assert key not in ('href','{http://www.w3.org/1999/xlink}href')
                if key in ('x','y','x1','y1','x2','y2','cx','cy','r','width','height','font-size','letter-spacing','textLength','stroke-width'):
                    assert math.isfinite(float(value)),(name,key,value)
        clip=root.find('.//s:clipPath/s:rect',ns)
        assert [float(clip.attrib[k]) for k in ('x','y','width','height')]==[116,230,1032,470]
        titles=[e for e in root.findall('.//s:text',ns) if e.text=='Auslenkung <&> α']
        assert len(titles)==1
        assert [float(titles[0].attrib[k]) for k in ('x','y','font-size')]==[64,104,34]
        assert titles[0].attrib['font-weight']=='600'
        texts=root.findall('.//s:text',ns)
        assert len([e for e in texts if e.text=='Referenz B · Referenz'])==1
        assert len([e for e in texts if e.text=='Signal [1]'])==1
        assert len([e for e in texts if e.text=='Zeit [s]'])==1
        long=[e for e in texts if e.text=='D'*95]
        assert len(long)==1 and float(long[0].attrib['font-size'])<17
        assert all(float(e.attrib.get('textLength',0))>0 for e in texts)
        if name!='figure-extreme.svg':
            theme,crop=map(int,path.stem.split('-')[1:]);colors=palette[theme]
            paper=root.find('s:g/s:rect',ns)
            assert paper.attrib['fill']==('#1b1c20' if theme else '#ffffff')
            curves=root.find('.//s:g[@clip-path]',ns);assert curves is not None
            line=[e for e in curves if e.tag.endswith('line') and e.attrib['stroke']==colors[1]]
            assert len(line)==1
            assert [float(line[0].attrib[k]) for k in ('x1','x2','y1','y2')]==[116,1148,347.5 if crop else 418,347.5 if crop else 418]
            dashed=[e for e in curves if e.tag.endswith('line') and e.attrib['stroke']==colors[0]]
            assert len(dashed)>5
            for e in dashed:
                x,y,xx,yy=(float(e.attrib[k]) for k in ('x1','y1','x2','y2'))
                assert 116<=x<=1148 and 116<=xx<=1148 and 230<=y<=700 and 230<=yy<=700
                assert math.hypot(xx-x,yy-y)<=3.00001
                if not crop:assert xx<=322.40001 or x>=735.19999
            assert len(curves.findall('s:circle',ns))==(1 if crop else 3)
            assert len(curves.findall('s:rect',ns))==2
            grids=[e for e in root.findall('.//s:line',ns) if e.attrib['stroke']==('#3b3d45' if theme else '#dce1e9')]
            assert len(grids)==11
        results.append({'name':name,'bytes':len(data),'sha256':hashlib.sha256(data).hexdigest(),'xml':True,'reference_geometry':name!='figure-extreme.svg'})
    return results

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--work',type=pathlib.Path,required=True);p.add_argument('--report',type=pathlib.Path,required=True);a=p.parse_args()
    result={'scope':__doc__,'status':'passed','files':check(a.work)}
    a.report.write_text(json.dumps(result,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    print('Graphic figure SVG: 5/5 XML, captured text, themes, reference layout, masks and XY crop passed')
