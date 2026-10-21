"""Actual hidden SDL/OpenGL export at four sizes, both themes and font sources.

Checks PNG pixel bytes against C readback CRCs and a native RGB preview round
trip. No browser, focus, native clipboard or physical DPI acceptance is implied.
"""
import argparse,hashlib,json,math,os,pathlib,re,struct,subprocess,time,xml.etree.ElementTree as ET,zlib

def png(path):
    data=path.read_bytes();assert data[:8]==b'\x89PNG\r\n\x1a\n';at=8;compressed=[];width=height=0;ended=False
    while at<len(data):
        assert at+12<=len(data)
        n=struct.unpack_from('>I',data,at)[0];kind=data[at+4:at+8];payload=data[at+8:at+8+n]
        assert at+12+n<=len(data) and zlib.crc32(kind+payload)==struct.unpack_from('>I',data,at+8+n)[0]
        if kind==b'IHDR':
            width,height,depth,color,compression,filtering,interlace=struct.unpack('>IIBBBBB',payload)
            assert 0<width<=4800 and 0<height<=3400 and (depth,color,compression,filtering,interlace)==(8,2,0,0,0)
        elif kind==b'IDAT':compressed.append(payload)
        elif kind==b'IEND':assert not payload and at+12==len(data);ended=True
        at+=12+n
    assert ended and width and height;stride=width*3;expected=(stride+1)*height
    decoder=zlib.decompressobj();raw=decoder.decompress(b''.join(compressed),expected+1)
    assert decoder.eof and not decoder.unused_data and not decoder.unconsumed_tail and len(raw)==expected
    assert all(raw[y*(stride+1)]==0 for y in range(height))
    rgb=b''.join(raw[y*(stride+1)+1:(y+1)*(stride+1)] for y in range(height))
    return width,height,rgb,hashlib.sha256(data).hexdigest()

def line_geometry(svg,width,rgb):
    """Compare actual blue-curve coverage to its independent SVG coordinates.

    Nearly horizontal, non-grid samples avoid slope/grid contamination. RGB
    projection rejects orange overlaps and measures coverage-weighted centers.
    Integer-rounded Nuklear commands lose both position and 2.5px thickness.
    """
    ns={'s':'http://www.w3.org/2000/svg'};group=svg.find('.//s:g[@clip-path]',ns)
    lines=[tuple(float(e.attrib[k]) for k in ('x1','y1','x2','y2')) for e in group
           if e.tag.endswith('line') and e.attrib.get('stroke')=='#17569e']
    delta=(232,169,97);den=sum(v*v for v in delta);samples=[]
    for x in range(118,1146):
        xx=x+.5;matches=[q for q in lines if q[0]<=xx<q[2] and q[2]>q[0]]
        if len(matches)!=1:continue
        a,b,c,d=matches[0];slope=(d-b)/(c-a)
        if abs(slope)>.15:continue
        expected=b+(xx-a)*slope
        if any(abs(expected-(230+i*470/4))<8 for i in range(5)):continue
        weight=weighted=0
        for y in range(max(231,int(expected)-5),min(699,int(expected)+7)):
            p=rgb[(y*width+x)*3:(y*width+x)*3+3]
            alpha=sum(v*(255-color) for v,color in zip(delta,p))/den
            if not .002<alpha<=1.01 or max(abs(color-(255-alpha*v)) for color,v in zip(p,delta))>2:continue
            weight+=alpha;weighted+=(y+.5)*alpha
        if weight>1.8:samples.append({'x':x,'expected':expected,'actual':weighted/weight,'error':weighted/weight-expected,'coverage':weight})
    assert len(samples)>=8,('Too few independent curve samples',len(samples))
    maximum=max(abs(s['error']) for s in samples);coverage=sum(s['coverage'] for s in samples)/len(samples)
    result={'samples':samples,'maximum_center_error_px':maximum,'mean_coverage_px':coverage}
    assert maximum<.2 and 2.3<coverage<2.8,result
    return result

def inspect(directory,stdout):
    pattern=r'GRAPHIC RASTER CASE: name=(\S+) width=(\d+) height=(\d+) scale=(\d+) glyphs=(\d+) atlas=(\d+),(\d+) font=([\d.]+),([\d.]+) peak=(\d+) window=(\d+),(\d+) crc=([0-9a-f]+)'
    cases=re.findall(pattern,stdout);assert len(cases)==12
    assert 'GRAPHIC RASTER SELF-TEST: PASSED' in stdout
    glyphs=re.findall(r'GRAPHIC RASTER GLYPH: name=(\S+) A=([\d.]+),([\d.]+)',stdout);assert len(glyphs)==12
    result=[];full_pixels=None;ns={'s':'http://www.w3.org/2000/svg'}
    for name,ww,hh,ss,gg,aw,ah,fr,fb,peak,window_w,window_h,crc in cases:
        scale=int(ss);dark='-dark-' in name;crop=name.startswith('crop-');marks=name.startswith('marks-')
        w,h,rgb,digest=png(directory/(name+'.png'));assert (w,h)==(int(ww),int(hh))==(1200*scale,850*scale)
        assert zlib.crc32(rgb)==int(crc,16)
        assert 100<=int(gg)<512 and 0<int(aw)<=16384 and 0<int(ah)<=16384 and int(peak)<=128*1024*1024
        assert (int(window_w),int(window_h))==((760,540) if scale%2 else (1600,900))
        paper=(27,28,32) if dark else (255,255,255)
        colors=[(138,186,255),(245,180,108),(139,204,165)] if dark else [(23,86,158),(145,72,0),(0,100,59)]
        def pixel(x,y):
            at=(y*w+x)*3;return tuple(rgb[at:at+3])
        def close(p,color):return max(abs(a-b) for a,b in zip(p,color))<=10
        assert pixel(1190*scale,840*scale)==paper and pixel(1000*scale,40*scale)==paper
        title_ink=sum(max(abs(a-b) for a,b in zip(pixel(x*scale,y*scale),paper))>60 for y in range(68,108) for x in range(62,520))
        assert title_ink>100,(name,'title has no real ink',title_ink)
        if not marks:
            blue=sum(close(pixel(x*scale,y*scale),colors[0]) for y in range(231,700) for x in range(116,1148))
            assert blue>250,(name,'missing plotted reference',blue)
            low,high=(4,12) if crop else (0,20)
            left=math.ceil(116+(4.75-low)/(high-low)*1032)+2;right=math.floor(116+(5.5-low)/(high-low)*1032)-2
            assert not any(close(pixel(x*scale,y*scale),colors[0]) for y in range(231,700) for x in range(left,right)),(name,'mask gap bridged')
        else:
            assert close(pixel(322*scale,600*scale),colors[1]),(name,'missing histogram')
            assert close(pixel(219*scale,582*scale),colors[0]),(name,'missing scatter')
            assert not close(pixel(426*scale,465*scale),colors[0]),(name,'invalid scatter drawn')
            assert close(pixel(529*scale,465*scale),colors[2]),(name,'isolated line point missing')
        assert not any(any(close(pixel(1150*scale,y*scale),color) for color in colors) for y in range(230,700)),(name,'curve outside clip')
        svg=ET.parse(directory/(name+'.svg')).getroot();assert svg.attrib['viewBox']=='0 0 1200 850'
        geometry=line_geometry(svg,w,rgb) if name=='full-light-1' else None
        texts=svg.findall('.//s:text',ns);assert all(e.attrib.get('font-family') for e in texts)
        assert any(e.text==('Anzahl [1]' if marks else 'θ [rad]') for e in texts)
        assert any(e.text==('Position [1]' if marks else 'Zeit [s]') for e in texts)
        if not marks:assert any(e.text=='Lauf 024 · Referenz' for e in texts)
        if not marks:assert any(('4,000–12,000 s' if crop else '0,000–20,000 s') in (e.text or '') for e in texts)
        if name=='full-light-1':full_pixels=rgb
        result.append({'name':name,'size':[w,h],'rgb_crc32':crc,'png_sha256':digest,'title_ink_samples':title_ink,'atlas':[int(aw),int(ah)],'glyphs':int(gg),'font_raster_heights':[float(fr),float(fb)],'font_peak_bytes':int(peak),'window_size':[int(window_w),int(window_h)],'subpixel_geometry':geometry})
    pw,ph,preview,preview_sha=png(directory/'preview-light-1.png')
    assert (pw,ph)==(1200,850) and preview==full_pixels
    return {'cases':result,'preview_rgb_identical':True,'preview_png_sha256':preview_sha,'glyph_probes':glyphs,'font_families':[s.strip() for s in re.findall(r'GRAPHIC RASTER FONT: (.+)',stdout)],'phase_times':re.findall(r'GRAPHIC RASTER TIME: (.+)',stdout)}

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--app',type=pathlib.Path,required=True);p.add_argument('--root',type=pathlib.Path,required=True);p.add_argument('--work',type=pathlib.Path,required=True);a=p.parse_args()
    a.work.mkdir(parents=True,exist_ok=True);steps=[]
    try:
        for bundled in (False,True):
            work=a.work/('bundled' if bundled else 'platform');work.mkdir(exist_ok=False)
            environment=dict(os.environ,PHYSIM_TEST_TRACE='1')
            if bundled:environment['PHYSIM_TEST_BUNDLED_FONTS']='1'
            else:environment.pop('PHYSIM_TEST_BUNDLED_FONTS',None)
            command=[str(a.app.resolve()),'--graphic-raster-test',str(a.root.resolve()),str(work.resolve())]
            start=time.monotonic();completed=subprocess.run(command,cwd=a.root,env=environment,capture_output=True,timeout=120)
            step={'command':command,'seconds':time.monotonic()-start,'exit_code':completed.returncode,'stdout':completed.stdout.decode('utf-8'),'stderr':completed.stderr.decode('utf-8')};steps.append(step)
            assert completed.returncode==0,step['stdout'][-3000:]+step['stderr'][-3000:]
            step['checks']=inspect(work,step['stdout'])
    finally:(a.work/'app-steps.json').write_text(json.dumps(steps,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    print('Graphic raster: 24 real PNG/figure cases, 2 exact RGB texture previews, actual font atlases, masks and atomic failures passed')
if __name__=='__main__':main()
