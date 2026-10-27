"""Native readonly original: wrap, final-glyph pixels, clipboard and reflow."""
import argparse
from collections import Counter
import hashlib
import json
import math
import os
from pathlib import Path
import struct
import subprocess
import sys
import zlib

MODES=('protocol-ui-wrap-dark','protocol-ui-wrap-light','protocol-ui-wrap-contrast',
       'protocol-ui-wrap-large','protocol-ui-wrap-narrow')
SUFFIX="\r\n\tOriginal α e\u0301 🌍\r\nHIER IST DAS ECHTE ENDE!".encode('utf-8')
ORIGINAL=bytes(ord('A')+i%26 for i in range(64*1024-len(SUFFIX)))+SUFFIX
def require(ok,message):
    if not ok:raise RuntimeError(message)

def final_glyph_pixels(path,state):
    data=path.read_bytes();require(data[:8]==b'\x89PNG\r\n\x1a\n','Invalid final-state PNG')
    at=8;compressed=[];width=height=0
    while at<len(data):
        require(at+12<=len(data),'Incomplete PNG chunk')
        length=struct.unpack_from('>I',data,at)[0];kind=data[at+4:at+8];payload=data[at+8:at+8+length]
        require(at+12+length<=len(data),'Incomplete PNG payload')
        require(zlib.crc32(kind+payload)==struct.unpack_from('>I',data,at+8+length)[0],'PNG checksum mismatch')
        if kind==b'IHDR':
            width,height,depth,color,compression,filtering,interlace=struct.unpack('>IIBBBBB',payload)
            require((depth,color,compression,filtering,interlace)==(8,2,0,0,0),'Unexpected native PNG encoding')
        elif kind==b'IDAT':compressed.append(payload)
        at+=length+12
    require(width>0 and height>0,'PNG size missing')
    pixels=zlib.decompress(b''.join(compressed));stride=width*3+1
    require(len(pixels)==height*stride and all(pixels[y*stride]==0 for y in range(height)),'Unexpected native scanlines')
    density=width/state['window'][0]
    require(abs(height/state['window'][1]-density)<.01,'Capture density mismatch')
    x,y,w,h=state['end_box'];vx,vy,vw,vh=state['viewport']
    require(w>0 and h>0 and x>=vx and y>=vy and x+w<=vx+vw and y+h<=vy+vh,'Final glyph is outside original viewport')
    left,top=math.floor(x*density),math.floor(y*density)
    right,bottom=math.ceil((x+w)*density),math.ceil((y+h)*density)
    require(0<=left<right<=width and 0<=top<bottom<=height,'Final glyph outside image')
    samples=[pixels[row*stride+1+column*3:row*stride+4+column*3]
             for row in range(top,bottom) for column in range(left,right)]
    spread=max(max(color[channel] for color in samples)-min(color[channel] for color in samples) for channel in range(3))
    background=Counter(samples).most_common(1)[0][0]
    ink=sum(max(abs(pixel[c]-background[c]) for c in range(3))>=45 for pixel in samples)
    require(spread>=60 and ink>=2,'Final glyph region has no visible ink')
    return {'density':density,'crop':[left,top,right,bottom],'ink_pixels':ink,'channel_spread':spread}

def verify(work,mode,code,stdout,root):
    require(code==0,f'Native app exited {code}: {mode}')
    require(f'PROTOCOL ORIGINAL {mode} SELF-TEST: PASSED' in stdout,'Native completion missing')
    states=[json.loads(line) for line in stdout.splitlines() if line.startswith('{"kind":"protocol_original_step"')]
    require([s['stage'] for s in states]==list(range(3,14)),'Incomplete native action sequence')
    require(all(s['passed'] for s in states),'Native original action failed')
    require(len({s['source_crc'] for s in states})==1,'Source editor changed')
    require(all(s['original_bytes']==len(ORIGINAL) and s['original_crc']==zlib.crc32(ORIGINAL) for s in states),'Original bytes changed')
    steps={s['stage']:s for s in states};scalars=len(ORIGINAL.decode('utf-8'))
    require(all('field' in s and 'row_height' in s and 'detail_scroll_y' in s for s in states),'Missing natural-flow geometry')
    require(all(math.isclose(s['field'][3],s['rows']*s['row_height'],abs_tol=.02) for s in states),'Original does not use natural text height')
    require(steps[6]['cursor']==scalars and steps[6]['rows']>10 and steps[6]['scroll_y']==0 and steps[6]['detail_scroll_y']>0,'End navigation did not reveal the wrapped end through its parent')
    require(steps[9]['copy_equal'],'Native SDL clipboard did not contain exact original')
    require(steps[11]['selection']==[0,scalars],'Resize lost original selection')
    original_width=steps[6]['window'][0]
    require(steps[11]['window'][0]==(original_width-160 if original_width>=1240 else original_width+160),'Resize did not occur')
    require((work/'project/main.c').read_bytes()==(root/'examples/pendulum/main.c').read_bytes(),'Saved source changed')
    image=work/'protocol-original-end.png'
    ink=final_glyph_pixels(image,steps[6])
    return {'mode':mode,'steps':len(states),'original_bytes':len(ORIGINAL),'scalars':scalars,
            'rows':steps[6]['rows'],'end_pixels':ink,'image_sha256':hashlib.sha256(image.read_bytes()).hexdigest()}

def main():
    for stream in (sys.stdout,sys.stderr):
        if hasattr(stream,'reconfigure'):stream.reconfigure(encoding='utf-8',errors='replace')
    parser=argparse.ArgumentParser(description=__doc__)
    for name in ('app','work','root'):parser.add_argument('--'+name,type=Path,required=True)
    args=parser.parse_args();args.app=args.app.resolve();args.work=args.work.resolve();args.root=args.root.resolve()
    args.work.mkdir(parents=True,exist_ok=True);program_hash=hashlib.sha256(args.app.read_bytes()).hexdigest()
    results=[]
    for mode in MODES:
        work=args.work/mode/'workflow ä';work.mkdir(parents=True,exist_ok=False)
        environment=dict(os.environ,SDL_FORCE_RAISEWINDOW='1',PHYSIM_TEST_TRACE='1')
        environment.pop('PHYSIM_TEST_ALL_CAPTURES',None)
        command=[str(args.app),'--workspace-state-test',str(work),mode]
        try:
            tested=subprocess.run(command,cwd=args.work,env=environment,capture_output=True,timeout=90)
            code,stdout,stderr=tested.returncode,tested.stdout,tested.stderr
        except subprocess.TimeoutExpired as error:
            code,stdout,stderr=None,error.stdout or b'',error.stderr or b''
        (work/'stdout.bin').write_bytes(stdout);(work/'stderr.bin').write_bytes(stderr)
        record={'command':command,'exitcode':code,'passed':False,
                'stdout_sha256':hashlib.sha256(stdout).hexdigest(),'stderr_sha256':hashlib.sha256(stderr).hexdigest()}
        try:record.update(verify(work,mode,code,stdout.decode('utf-8',errors='backslashreplace'),args.root),passed=True)
        except (RuntimeError,OSError,ValueError) as error:record['error']=str(error)
        results.append(record)
        (args.work/'verification.json').write_text(json.dumps({'binary_sha256':program_hash,'cases':results},indent=2)+'\n',encoding='utf-8')
        require(record['passed'],record.get('error','Native original failed'))
        require(hashlib.sha256(args.app.read_bytes()).hexdigest()==program_hash,'Native binary changed')
    print('Native protocol original: 5 themes/sizes, final glyph ink, exact UTF8/CRLF clipboard, reflow selection and readonly integrity passed')

if __name__=='__main__':main()
