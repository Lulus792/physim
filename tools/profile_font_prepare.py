"""Profile the exact application font setup; preserve complete atlas fingerprints.

Runs three serial CPU-only atlases, or three alternating before/after pairs
with --compare-backend-source. The older backend must contain the original
font_source_contains helper. Use fresh labels to preserve reports. Times are
measurements, never CI pass thresholds or a substitute for real F1 startup.
"""
from pathlib import Path
import argparse,hashlib,json,os,re,statistics,sys
root=Path(__file__).resolve().parents[1];sys.path.insert(0,str(root/'tools'))
import build
p=argparse.ArgumentParser();p.add_argument('--label',required=True);p.add_argument('--sdl',type=Path,required=True)
p.add_argument('--compiler',required=True);p.add_argument('--backend-source',type=Path,default=root/'app/ui_backend.c')
p.add_argument('--compare-backend-source',type=Path,help='Alternate an earlier fallback implementation with the selected backend in one process')
a=p.parse_args()
if not re.fullmatch(r'[A-Za-z0-9_-]+',a.label):p.error('--label must contain only letters, digits, underscores or hyphens')
backend=a.backend_source.resolve()
directory=root/'build/native'/('font-prepare-'+a.label);directory.mkdir(parents=True,exist_ok=True)
if (directory/'report.json').exists():p.error('This label already has a report; choose a fresh label to preserve it')
s=(root/'app/main.c').read_text(encoding='utf-8');start=s.index('static struct nk_font *system_font_sampling(')
end=s.index('static struct nk_font *system_font(',start)
setup=s[start:end]
source=directory/'font_prepare_profile.c'
paired=a.compare_backend_source is not None
backend_for_build=backend
macros='''#define PROFILE_RUNS 3
#define PROFILE_SELECT(run) ((void)(run))
#define PROFILE_LABEL(run) "after"
#define PROFILE_SUFFIX(run) ""
#define PROFILE_SAVE(run) (!(run))
'''
if paired:
 previous=a.compare_backend_source.resolve().read_text(encoding='utf-8')
 first=previous.index('static bool font_source_contains(')
 last=previous.index('#include <math.h>',first)
 prior=previous[first:last].replace('font_source_contains','profile_before_contains').replace('ps_ui_font_add_fallback','profile_before_add_fallback')
 backend_for_build=directory/'profile_backend.c'
 backend_for_build.write_text(backend.read_text(encoding='utf-8')+'\n'+prior+'''
static bool profile_before_mode;
void profile_select_before(bool before){profile_before_mode=before;}
bool profile_add_fallback(struct nk_font_atlas *atlas,struct nk_font *target,const char *path,const nk_rune *ranges) {
 return profile_before_mode?profile_before_add_fallback(atlas,target,path,ranges):ps_ui_font_add_fallback(atlas,target,path,ranges);
}
''',encoding='utf-8',newline='\n')
 macros='''extern void profile_select_before(bool before);
extern bool profile_add_fallback(struct nk_font_atlas *,struct nk_font *,const char *,const nk_rune *);
#define ps_ui_font_add_fallback profile_add_fallback
#define PROFILE_RUNS 6
#define PROFILE_SELECT(run) profile_select_before(((run)&1)==0)
#define PROFILE_LABEL(run) (((run)&1)?"after":"before")
#define PROFILE_SUFFIX(run) (((run)&1)?"-after":"-before")
#define PROFILE_SAVE(run) ((run)<2)
'''
source.write_text('''#include "ui.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
static bool exists(const char *path){struct stat value;return stat(path,&value)==0;}
'''+macros+setup+'''
int main(int argc,char **argv) {
 if(argc!=3)return 2;
 for(unsigned run=0;run<PROFILE_RUNS;run++) {
  PROFILE_SELECT(run);
  struct nk_font_atlas atlas;nk_font_atlas_init_default(&atlas);nk_font_atlas_begin(&atlas);
  Uint64 begin=SDL_GetTicksNS();
  const float sizes[]={32,46,32,40,32};
  for(unsigned role=0;role<5;role++)if(!system_font_sampling(&atlas,argv[1],sizes[role],role==4,role==1 || role==2 || role==3,2,1))return 3;
  Uint64 sources=SDL_GetTicksNS();int width=0,height=0;
  const unsigned char *pixels=nk_font_atlas_bake_limited(&atlas,&width,&height,NK_FONT_ATLAS_ALPHA8,8192);
  Uint64 baked=SDL_GetTicksNS();if(!pixels)return 4;
  if(PROFILE_SAVE(run)) {
   char path[4096];
   if(snprintf(path,sizeof path,"%s/atlas%s.a8",argv[2],PROFILE_SUFFIX(run))>=(int)sizeof path)return 5;
   FILE *capture=fopen(path,"wb");if(!capture)return 5;
   size_t bytes=(size_t)width*(size_t)height;
   bool saved=fwrite(pixels,1,bytes,capture)==bytes;if(fclose(capture))saved=false;
   if(!saved)return 5;
  }
  uint64_t hash=14695981039346656037ull;
  for(size_t i=0;i<(size_t)width*(size_t)height;i++){hash^=pixels[i];hash*=1099511628211ull;}
  printf("PROFILE run=%u variant=%s sources=%.9f bake=%.9f fonts=%d glyphs=%d width=%d height=%d hash=%016llx\\n",run,PROFILE_LABEL(run),(double)(sources-begin)/1e9,(double)(baked-sources)/1e9,atlas.font_num,atlas.glyph_count,width,height,(unsigned long long)hash);
  nk_font_atlas_end(&atlas,nk_handle_id(0),NULL);nk_font_atlas_clear(&atlas);
 }
 return 0;
}
''',encoding='utf-8',newline='\n')
args=argparse.Namespace(build_dir=directory,compiler=a.compiler,config='Debug',sdl=a.sdl,no_app=False,sanitizers=True,jobs=8,rebuild=False)
env=build.compiler_environment() if os.name=='nt' else dict(os.environ)
with build.build_lock(directory):
 b=build.Builder(args,env)
 binary=b.executable('font-prepare-profile',[str(source),str(backend_for_build)],[],sdl=True)
 result=build.subprocess.run([str(binary),str(root),str(directory)],env=dict(b.env,ASAN_OPTIONS='detect_leaks=0'),capture_output=True,timeout=180)
 (directory/'stdout').write_bytes(result.stdout);(directory/'stderr').write_bytes(result.stderr)
 print(result.stdout.decode('utf-8','replace'));print('EXIT',result.returncode)
 if result.returncode:print(result.stderr.decode('utf-8','replace')[-1500:])
 if result.returncode:raise SystemExit(result.returncode)
 rows=[]
 for line in result.stdout.decode('utf-8','replace').splitlines():
  if not line.startswith('PROFILE '):continue
  fields=dict(pair.split('=',1) for pair in line.split()[1:])
  rows.append({key:float(value) if key in ('sources','bake') else value if key in ('hash','variant') else int(value) for key,value in fields.items()})
 if len(rows)!=(6 if paired else 3):raise RuntimeError('Incomplete profile iterations')
 identity={(row['fonts'],row['glyphs'],row['width'],row['height'],row['hash']) for row in rows}
 if len(identity)!=1:raise RuntimeError('Atlas contents changed between identical iterations')
 candidate=[row for row in rows if row['variant']=='after']
 atlas=directory/('atlas-after.a8' if paired else 'atlas.a8')
 report=dict(label=a.label,backend_source=str(backend),backend_sha256=hashlib.sha256(backend.read_bytes()).hexdigest(),
  font_setup_sha256=hashlib.sha256(setup.encode('utf-8')).hexdigest(),compiler=b.cc,
  atlas_sha256=hashlib.sha256(atlas.read_bytes()).hexdigest(),
  bundled_fonts=os.environ.get('PHYSIM_TEST_BUNDLED_FONTS')=='1',
  workload='Five initial F1 variants at density 2, full production ranges and sampling; CPU-only source preparation and atlas bake',
  runs=rows,median_sources_seconds=statistics.median(row['sources'] for row in candidate),
  median_bake_seconds=statistics.median(row['bake'] for row in candidate))
 if paired:
  if (directory/'atlas-before.a8').read_bytes()!=atlas.read_bytes():raise RuntimeError('Complete atlas bytes differ between implementations')
  report.update(compared_backend_source=str(a.compare_backend_source.resolve()),
   compared_backend_sha256=hashlib.sha256(a.compare_backend_source.resolve().read_bytes()).hexdigest(),
   paired_before_median_seconds=statistics.median(row['sources'] for row in rows if row['variant']=='before'),
   complete_atlas_bytes_identical=True)
 (directory/'report.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
 print('Report:',directory/'report.json')

