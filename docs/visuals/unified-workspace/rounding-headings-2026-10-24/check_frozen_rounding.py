"""Verify frozen native paths, whole-word headings and original GPU pixels."""
import gzip,hashlib,json,math,pathlib,re,sys
HERE=pathlib.Path(__file__).resolve().parent
ROOT=HERE.parents[3]
sys.path.insert(0,str(ROOT/"tests"))
from check_graphic_raster import png

def read(name):
 b=(HERE/name).read_bytes()
 return json.loads(gzip.decompress(b) if name.endswith(".gz") else b)

def wrong_pixels(image,density):
 width,height,rgb,_=image;assert width==height==160*density
 shapes=((20,15,80,12,30),(20,47,80,12,6),(20,79,12,22,12),(20,115,80,12,0))
 bad=0
 for x,y,w,h,r in shapes:
  x*=density;y*=density;w*=density;h*=density;r=min(r*density,min(w,h)/2)
  for py in range(int(y)-3,int(y+h)+3):
   for px in range(int(x)-3,int(x+w)+3):
    qx=abs(px+.5-x-w/2)-(w/2-r);qy=abs(py+.5-y-h/2)-(h/2-r)
    distance=math.hypot(max(qx,0),max(qy,0))+min(max(qx,qy),0)-r
    color=rgb[(py*width+px)*3:(py*width+px)*3+3]
    bad+=int((distance < -1.5 and min(color)<250) or (distance>1.5 and max(color)>5))
 return bad

def main():
 artifacts=read("artifacts.json")
 for name,record in artifacts.items():
  b=(HERE/name).read_bytes();assert len(b)==record["bytes"] and hashlib.sha256(b).hexdigest()==record["sha256"],name
 v=read("validation.json");groups=v["groups"]
 assert len(groups)==17 and all(g["status"]=="passed" for g in groups)
 assert len({g["name"] for g in groups if g["platform"]=="linux"})==13
 assert len({g["name"] for g in groups if g["platform"]=="windows"})==4
 assert not v["whole_ui_complete"] and not v["windows_gui"]
 for label in ("linux4","windows4"):
  out=read(label+"-ui_rounded-result.json.gz")["stdout"]
  records=re.findall(r"ROUNDED PATH width=.*?convex=(\d) error=([\d.]+) pass=(\d)",out)
  assert len(records)==30 and all(c==p=="1" and float(e)<=.13 for c,e,p in records)
 assert "ROUNDED PATHS: 5/30 passed" in read("negative-path-results.json.gz")[0]["stdout"]
 for label in ("partial-linux3","partial-windows3"):
  case=next(g for g in read(label+"-results.json.gz") if g["name"]=="ui_rounded")
  assert "ROUNDED PATHS: 20/30 passed" in case["stdout"]
 failures=read("broad4-0-results.json.gz")
 assert [(g["name"],g["status"]) for g in failures if g["status"]!="passed"]==[("documentation_h","failed")]
 assert read("h-ext4-0-results.json.gz")[0]["status"]=="passed"
 old_errors=wrong_pixels(png(HERE/"negative-rounded-gpu.png"),1);assert old_errors>0
 for prefix in ("gpu-rounded-","windows-rounded-"):
  for density in (1,2):assert wrong_pixels(png(HERE/f"{prefix}{density}.png"),density)==0
 labels=['Objekt','Modellwerte','Laufparameter','Laufeinstellungen','Laufgrenzen','Messkanal und Einheit','Ereignisse','Kamera und Darstellung','Aufzeichnung']
 checks=read("linux4-inspector_design-work-validation.json.gz")["checks"];assert len(checks)==2
 total=0
 for check in checks:
  seen={name:set() for name in ('dark','light','medium','larger','large','contrast')}
  for record in check["records"]:
   assert abs(record['header'][2]-max(record['minimum'],record['wish']))<.02
   if not record['kind'].startswith('closed'):continue
   body=record['body']
   for title in record['titles']:
    row=record['sections'][title['slot']]['rect']
    if row[1]<body[1]-.03 or row[1]+row[3]>body[1]+body[3]+.03:continue
    label=labels[title['slot']];at=0;parts=title['parts']
    assert ''.join(''.join(parts).split())==''.join(label.split())
    for part in parts:
     at=label.index(part,at)+len(part)
     assert at==len(label) or label[at].isspace() or label[at-1].isspace()
    seen[record['variant']].add(title['slot'])
  assert all(slots==set(range(9)) for slots in seen.values());total+=sum(map(len,seen.values()))
 assert total==108
 for label in ('windows','linux'):
  assert b'Native test runner: expected exits, diagnostics, timeouts, launch/build failures and complete reports passed' in gzip.decompress((HERE/f'catalog-{label}.log.gz').read_bytes())
 print(f"Verified {len(artifacts)} artifacts; 13 Linux + 4 Windows groups; 30 path cases per platform; 108 complete title/variant combinations. Original GPU image rejects {old_errors} far-field pixels; final 1x/2x Windows/Linux images pass. Whole UI acceptance remains open.")

if __name__=='__main__':main()
