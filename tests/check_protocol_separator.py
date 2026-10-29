"""Visible resting protocol boundary, using actual native framebuffer pixels."""
import argparse,hashlib,json,os,subprocess
from pathlib import Path
from check_graphic_raster import png
p=argparse.ArgumentParser(description=__doc__)
for name in ('app','work'):p.add_argument('--'+name,type=Path,required=True)
a=p.parse_args();a.app=a.app.resolve();a.work.mkdir(parents=True,exist_ok=True)
binary=hashlib.sha256(a.app.read_bytes()).hexdigest();cases=[]
for mode,color in [('dark',(59,61,69)),('light',(190,195,206)),('contrast',(255,255,255)),('large',(59,61,69))]:
 work=a.work/mode;work.mkdir();command=[str(a.app),'--workspace-state-test',str(work),'protocol-ui-border-'+mode]
 result=subprocess.run(command,capture_output=True,timeout=60,env=dict(os.environ,PHYSIM_TEST_TRACE='1'))
 (work/'stdout.bin').write_bytes(result.stdout);(work/'stderr.bin').write_bytes(result.stderr)
 assert result.returncode==0,result.stderr.decode('utf8',errors='replace')
 image=work/'protocol-navigator.png';w,h,rgb,_=png(image);assert (w,h)==(1440,1000)
 y=h-28-340;expected=bytes(color)
 def count(row):return sum(rgb[(row*w+x)*3:(row*w+x)*3+3]==expected for x in range(1,w-1))
 counts={str(row):count(row) for row in (y-1,y,y+1)}
 cases.append(dict(mode=mode,command=command,exit_code=result.returncode,image_sha256=hashlib.sha256(image.read_bytes()).hexdigest(),width=w,height=h,separator_y=y,border_rgb=color,matching_pixels=counts,checked_pixels=w-2))
 (a.work/'verification.json').write_text(json.dumps(dict(binary_sha256=binary,cases=cases),indent=2)+'\n')
 assert counts[str(y)]==w-2,('Idle protocol separator missing',mode,counts,w-2)
 assert counts[str(y+1)]<w//2,('Boundary became a broad surface',mode,counts)
 assert hashlib.sha256(a.app.read_bytes()).hexdigest()==binary
print('Protocol separator: resting full-width 1px boundary in dark/light/high contrast and large text passed; actual native pixels checked')
