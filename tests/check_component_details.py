"""Native chart tools and path tooltip tasks, with scoped CSS pixel probes.

The 4095-byte path is explicitly a native layout stress, not a claim that a file
with that path was opened on Windows. The first path comes from a real project.
No browser or physical DPI acceptance is inferred from these 1x captures.
"""
import argparse,json,math,pathlib,re
from check_display_workflow import Workflow
from check_graphic_raster import png

def main():
    p=argparse.ArgumentParser();p.add_argument('--app',type=pathlib.Path,required=True);p.add_argument('--root',type=pathlib.Path,required=True)
    p.add_argument('--work',type=pathlib.Path,required=True);p.add_argument('--mode',required=True);args=p.parse_args()
    args.work.mkdir(parents=True,exist_ok=True);flow=Workflow(args.app,args.work,args.root,False);flow.env['SDL_FORCE_RAISEWINDOW']='1'
    mode=args.mode.replace('_','-');records={}
    for kind,marker in [('chart','COMPONENT CHART SELF-TEST: PASSED'),('tip','COMPONENT TOOLTIP SELF-TEST: PASSED')]:
        folder=args.work/kind;folder.mkdir();flow.run('--workspace-state-test',folder,'component-'+kind+'-'+mode,timeout=75,marker=marker)
        records[kind]=flow.steps[-1]['stdout']
    text=records['chart'];rects={};scale=None;viewport=None
    pattern=r'COMPONENT TOOLS (\w+) size=(\d+) viewport=(\d+),(\d+)((?: rect\d=[\d.,-]+){4})'
    for name,size,w,h,tail in re.findall(pattern,text):
        scale=int(size)/16;viewport=int(w),int(h);rects[name]=[tuple(map(float,x.split(','))) for x in re.findall(r'rect\d=([\d.,-]+)',tail)]
    assert set(rects)=={'raw','report','disabled'},rects
    assert 'COMPONENT WRAPPED TEXT complete=1' in text
    assert re.search(r'COMPONENT WRAPPED .*rows=[2-4] .*geometry=1',text)
    for name,boxes in rects.items():
        for x,y,w,h in boxes:assert w>0 and h>=28*scale-.001 and x>=0 and y>=0 and x+w<=viewport[0]+.001
        assert boxes[1][0]-boxes[0][0]-boxes[0][2]>=4*scale-.001
    palette=({'border':(190,195,206),'hover':(224,227,234),'edit':(255,255,255),'accent':(23,86,158)} if 'light' in mode else
             {'border':(59,61,69),'hover':(53,54,62),'edit':(25,26,30),'accent':(138,186,255)})
    if 'contrast' in mode:palette['edit']=(0,0,0)
    checks=[]
    def pixel(image,x,y):
        w,h,rgb,_=image;x=int(math.floor(x));y=int(math.floor(y));assert 0<=x<w and 0<=y<h;at=(y*w+x)*3;return tuple(rgb[at:at+3])
    def load(name):
        im=png(args.work/'chart'/name);assert im[:2]==viewport,'Pixel oracle requires the named 1x client';return im
    normal=load('tools-normal.png');hover=load('tools-hover.png');focus=load('tools-focus.png')
    x,y,w,h=rects['raw'][0];px=x+w/2;py=math.floor(y+.5)
    if 'contrast' not in mode:
        # The rounded one-pixel border has an antialiased inner edge. At a
        # fractional text scale its composited channel can differ by one byte.
        assert max(abs(a-b) for a,b in zip(pixel(normal,px,py),palette['border']))<=1,('border',pixel(normal,px,py),palette)
        assert pixel(normal,px,y+4*scale)==palette['edit'],('transparent control',pixel(normal,px,y+4*scale),palette)
        assert pixel(hover,px,y+4*scale)==palette['hover'],('hover',pixel(hover,px,y+4*scale),palette)
        # Sample the full-coverage pixel on the outside half of the 2px ring.
        assert pixel(focus,px,y-4*scale-.5)==palette['accent'],('outer focus',pixel(focus,px,y-4*scale-.5),palette)
        checks+=['CSS border, transparent interior, hover surface and outer focus color']
    for kind,filename in [('raw','tools-normal.png'),('report','tools-report.png')]:
        im=load(filename);x,y,w,h=rects[kind][3];pad=(7 if viewport[0]<=1200*scale else 9)*scale
        # Arrow tip from the original SVG M10 13V2 in its 20-unit viewBox,
        # displayed at16px, before the text starts. No generated native image
        # is used as the expected icon.
        tip_x=x+pad+1+8*scale;tip_y=y+(h-16*scale)/2+1.6*scale
        ink=max(max(abs(c-b) for c,b in zip(pixel(im,tip_x+dx,tip_y+dy),palette['edit']))
                for dx in (-.6,0,.6) for dy in (-.6,0,.6))
        assert ink>20,(kind,'missing export SVG arrow tip',ink)
    checks+=['export SVG arrow tip in raw and report toolbar']
    tip=records['tip'];entries=re.findall(r'COMPONENT TIP ([\w-]+) bytes=(\d+) lines=(\d+) scroll=(\d+) scrollable=(\d) content=([\d.]+) rect=([\d.,]+) viewport=(\d+),(\d+)',tip)
    assert {x[0] for x in entries}=={'real','maximum-layout-stress','scrolled','bottom-end-visible'}
    for kind,bytes_,lines,scroll,scrollable,content,box,vw,vh in entries:
        x,y,w,h=map(float,box.split(','));assert x>=8 and y>=8 and x+w<=int(vw)-8+.001 and y+h<=int(vh)-8+.001
        assert int(lines)>0
        if kind!='real':assert int(bytes_)==4095 and int(scrollable)==1
        if kind in ('scrolled','bottom-end-visible'):assert int(scroll)>0
    for name in ('tip-real.png','tip-retained.png','tip-maximum.png','tip-scrolled.png','tip-bottom.png','tip-top.png','tip-dismissed.png'):
        assert (args.work/'tip'/name).is_file(),name
    surface=(246,247,249) if 'light' in mode else (0,0,0) if 'contrast' in mode else (30,31,35)
    maximum=png(args.work/'tip'/'tip-maximum.png')
    # Gap below the first text row: the transparent probe container must not
    # leak through the actual popup material and invalidate the visual proof.
    assert pixel(maximum,540,33)==surface,('popup surface',pixel(maximum,540,33),surface)
    checks+=['opaque native tooltip material in maximum-storage probe']
    checks+=['real path, retained hover, complete maximum-storage bytes, visible final suffix, scroll isolation and Escape']
    (args.work/'validation.json').write_text(json.dumps({'mode':mode,'checks':checks,'tools':rects,'tip':entries,'scope':__doc__},indent=2)+'\n',encoding='utf-8')
    print('COMPONENT DETAILS: PASSED',mode)
if __name__=='__main__':main()
