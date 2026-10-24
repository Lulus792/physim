"""Local frozen evidence checks; does not start the native app."""
import gzip,hashlib,json,pathlib,sys
HERE=pathlib.Path(__file__).resolve().parent
ROOT=HERE.parents[3]
sys.path.insert(0,str(ROOT/"tests"))
from check_graphic_raster import png
from check_transport_design import pixels

def read(name):
 b=(HERE/name).read_bytes()
 return json.loads(gzip.decompress(b) if name.endswith(".gz") else b)

def main():
 for name,record in read("artifacts.json").items():
  b=(HERE/name).read_bytes()
  assert len(b)==record["bytes"] and hashlib.sha256(b).hexdigest()==record["sha256"],name
 validation=read("validation.json")
 assert len(validation["groups"])==3 and all(g["status"]=="passed" for g in validation["groups"])
 assert validation["whole_ui_complete"] is False and validation["windows"]["build_only"]
 frames=read("all-frame-hashes.json")
 assert sum(k.startswith("measurement_design/") for k in frames)==60
 assert sum(k.startswith("curve_design/") for k in frames)==96
 checks=read("curve_design-validation.json.gz")["checks"]
 assert len(checks)==2 and all(len(c["records"])==48 for c in checks)
 for c in checks:
  for record in c["records"]:
   assert record["preserved"] and record["hover"]==(record["kind"]=="hover")
   if record["segments"]:assert abs(record["stroke"]-2*record["size"]/16)<.001
 before=read("before.json")
 b=(ROOT/before["image"]).read_bytes();assert hashlib.sha256(b).hexdigest()==before["sha256"]
 assert len(pixels(png(ROOT/before["image"]),before["gap"],before["color"],before["tolerance"]))==before["pixels"]>0
 system=next(c["records"] for c in checks if c["font_mode"]=="system")
 for kind in ("empty","hover"):
  record=next(r for r in system if r["variant"]=="dark" and r["kind"]==kind)
  image=png(HERE/f"curve-system-dark-{kind}.png")
  assert not pixels(image,before["gap"],before["color"],before["tolerance"])
  if kind=="hover":
   x,y,w,h=record["head"]
   assert pixels(image,(x+w/2,y+1,1,1),(30,31,35),0)
 print("Frozen captions verified: 3 native groups; 60 measurement and 96 controlled frame hashes; original chevron rejected, final resting/hover captions accepted. Whole UI acceptance remains open.")

if __name__=="__main__":main()
