/* A generated TTC uses tracked fonts, not any installed platform collection. */
#include "ui.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Font face %d: %s\n",__LINE__,#x);return 1;}}while(0)
typedef struct {size_t live,requests,fail;} memory_state;
static void *allocate(nk_handle u,void *old,nk_size n) {
 (void)old;memory_state *m=u.ptr;if(++m->requests==m->fail)return NULL;
 void *p=malloc(n);if(p)m->live++;return p;
}
static void release(nk_handle u,void *p) {if(p){memory_state *m=u.ptr;m->live--;free(p);}}
static unsigned be32(const unsigned char *p) {return ((unsigned)p[0]<<24)|((unsigned)p[1]<<16)|((unsigned)p[2]<<8)|p[3];}
static void put32(unsigned char *p,unsigned n) {p[0]=(unsigned char)(n>>24);p[1]=(unsigned char)(n>>16);p[2]=(unsigned char)(n>>8);p[3]=(unsigned char)n;}
static unsigned char *load(const char *path,size_t *size) {
 FILE *f=fopen(path,"rb");if(!f)return NULL;
 if(fseek(f,0,SEEK_END)){fclose(f);return NULL;}long n=ftell(f);
 if(n<12 || n>16*1024*1024 || fseek(f,0,SEEK_SET)){fclose(f);return NULL;}
 unsigned char *p=malloc((size_t)n);
 if(!p || fread(p,1,(size_t)n,f)!=(size_t)n){free(p);fclose(f);return NULL;}
 fclose(f);*size=(size_t)n;return p;
}
static bool write_collection(const char *root,const char *path) {
 const char *files[]={"NotoEmoji.ttf","NotoSans-Bold.ttf","NotoSans-Bold.ttf"};
 unsigned char *font[3]={0};size_t size[3]={0},offset[3]={0},total=24;bool ok=true;
 for(unsigned i=0;i<3;i++) {
  char source[4096];if(snprintf(source,sizeof source,"%s/third_party/fonts/%s",root,files[i])>=(int)sizeof source){ok=false;break;}
  font[i]=load(source,&size[i]);if(!font[i]){ok=false;break;}
  offset[i]=total;total+=(size[i]+3)&~(size_t)3;
 }
 unsigned char *ttc=ok?calloc(1,total):NULL;if(!ttc)ok=false;
 if(ok) {
  memcpy(ttc,"ttcf",4);put32(ttc+4,0x10000);put32(ttc+8,3);
  for(unsigned i=0;i<3 && ok;i++) {
   put32(ttc+12+i*4,(unsigned)offset[i]);memcpy(ttc+offset[i],font[i],size[i]);
   unsigned tables=((unsigned)font[i][4]<<8)|font[i][5];
   if(tables>(size[i]-12)/16){ok=false;break;}
   for(unsigned t=0;t<tables;t++) {
    unsigned char *record=ttc+offset[i]+12+t*16;unsigned original=be32(record+8),length=be32(record+12);
    if(original>size[i] || length>size[i]-original){ok=false;break;}
    put32(record+8,original+(unsigned)offset[i]);
    if(i==1 && !memcmp(record,"head",4)) {
     if(length<46){ok=false;break;}ttc[offset[i]+original+44]=0;ttc[offset[i]+original+45]=3; /* bold italic */
    }
   }
  }
 }
 if(ok){FILE *f=fopen(path,"wb");if(!f)ok=false;else{ok=fwrite(ttc,1,total,f)==total;if(fclose(f))ok=false;}}
 for(unsigned i=0;i<3;i++)free(font[i]);free(ttc);return ok;
}
static size_t coverage(const unsigned char *pixels,int w,int h,const struct nk_font *font) {
 size_t sum=0;for(nk_rune c='A';c<='Z';c++) {
  const struct nk_font_glyph *g=nk_font_find_glyph(font,c);
  if(!g || !g->source_present || g->codepoint!=c)return 0;
  for(int y=(int)(g->v0*h);y<(int)(g->v1*h) && y<h;y++)
   for(int x=(int)(g->u0*w);x<(int)(g->u1*w) && x<w;x++)sum+=pixels[(size_t)y*w+x];
 }return sum;
}
int main(int argc,char **argv) {
 CHECK(argc==3 || (argc==4 && !strcmp(argv[3],"--wrong-face")));
 bool wrong_face=argc==4;char collection[4096],regular[4096],bold[4096];
 CHECK(snprintf(collection,sizeof collection,"%s/fixture.ttc",argv[2])<(int)sizeof collection && write_collection(argv[1],collection));
 CHECK(snprintf(regular,sizeof regular,"%s/third_party/fonts/NotoSans.ttf",argv[1])<(int)sizeof regular);
 CHECK(snprintf(bold,sizeof bold,"%s/third_party/fonts/NotoSans-Bold.ttf",argv[1])<(int)sizeof bold);
 memory_state m={0};struct nk_allocator allocator={nk_handle_ptr(&m),allocate,release};
 struct nk_font_atlas atlas;nk_font_atlas_init(&atlas,&allocator);nk_font_atlas_begin(&atlas);
 static const nk_rune ascii[]={32,126,0};struct nk_font_config config=nk_font_config(32);config.range=ascii;
 CHECK(!ps_ui_font_add_file_face(&atlas,regular,32,&config,true) && !atlas.fonts && !m.live);
 CHECK(!ps_ui_font_add_file_face(&atlas,bold,32,&config,false) && !atlas.fonts && !m.live);
 for(size_t failure=1;failure<=3;failure++) {
  m.fail=m.requests+failure;CHECK(!ps_ui_font_add_file_face(&atlas,collection,32,&config,true));
  CHECK(!atlas.fonts && !atlas.font_num && !m.live);
 }m.fail=0;
 struct nk_font *plain=ps_ui_font_add_file_face(&atlas,regular,32,&config,false);CHECK(plain && plain->config->ttf_index==0);
 struct nk_font *emoji=ps_ui_font_add_file_face(&atlas,collection,32,&config,false);CHECK(emoji && emoji->config->ttf_index==0);
 struct nk_font *reference=ps_ui_font_add_file_face(&atlas,bold,32,&config,true);CHECK(reference);
 struct nk_font *selected=ps_ui_font_add_file_face(&atlas,collection,32,&config,true);CHECK(selected && selected->config->ttf_index==2);
 size_t before=m.live;int fonts=atlas.font_num;
 CHECK(ps_ui_font_add_fallback(&atlas,selected,regular,ascii));
 CHECK(atlas.font_num==fonts && m.live==before && selected->config->n==selected->config);
#ifdef __APPLE__
 struct nk_font *mac_plain=ps_ui_font_add_file_face(&atlas,"/System/Library/Fonts/Helvetica.ttc",32,&config,false);
 struct nk_font *mac_bold=ps_ui_font_add_file_face(&atlas,"/System/Library/Fonts/Helvetica.ttc",32,&config,true);
 struct nk_font *mac_code=ps_ui_font_add_file_face(&atlas,"/System/Library/Fonts/Menlo.ttc",32,&config,false);
 CHECK(mac_plain && mac_bold && mac_code && mac_plain->config->ttf_index!=mac_bold->config->ttf_index);
 printf("Actual macOS faces: Helvetica normal=%d bold=%d, Menlo=%d\n",mac_plain->config->ttf_index,mac_bold->config->ttf_index,mac_code->config->ttf_index);
 before=m.live;
#endif
 int w=0,h=0;selected->config->ttf_index=-1;
 CHECK(!nk_font_atlas_bake_limited(&atlas,&w,&h,NK_FONT_ATLAS_ALPHA8,4096) && m.live==before);
 selected->config->ttf_index=99;
 CHECK(!nk_font_atlas_bake_limited(&atlas,&w,&h,NK_FONT_ATLAS_ALPHA8,4096) && m.live==before);
 selected->config->ttf_index=wrong_face?0:2;
 const unsigned char *pixels=nk_font_atlas_bake_limited(&atlas,&w,&h,NK_FONT_ATLAS_ALPHA8,4096);CHECK(pixels);
 CHECK(!ps_ui_font_has_glyph(&emoji->handle,'A') && ps_ui_font_has_glyph(&selected->handle,'A'));
 float normal=plain->handle.width(plain->handle.userdata,32,"AaBb",4);
 float strong=selected->handle.width(selected->handle.userdata,32,"AaBb",4);
 CHECK(fabsf(normal-strong)>.2f && strong==reference->handle.width(reference->handle.userdata,32,"AaBb",4));
 size_t normal_ink=coverage(pixels,w,h,plain),bold_ink=coverage(pixels,w,h,selected);
 CHECK(normal_ink && bold_ink>normal_ink*1.2 && bold_ink==coverage(pixels,w,h,reference));
#ifdef __APPLE__
 float mac_normal_width=mac_plain->handle.width(mac_plain->handle.userdata,32,"AaBb",4);
 float mac_bold_width=mac_bold->handle.width(mac_bold->handle.userdata,32,"AaBb",4);
 CHECK(fabsf(mac_normal_width-mac_bold_width)>.2f);
 CHECK(coverage(pixels,w,h,mac_bold)>coverage(pixels,w,h,mac_plain)*1.1);
 CHECK(ps_ui_font_has_glyph(&mac_code->handle,'A'));
#endif
 nk_font_atlas_end(&atlas,nk_handle_id(0),NULL);nk_font_atlas_cleanup(&atlas);
 CHECK(selected->config->ttf_index==2 && ps_ui_font_has_glyph(&selected->handle,'A'));
 nk_font_atlas_clear(&atlas);CHECK(!m.live);
 puts("Font faces: selected TTC index 2, italic rejection, selected cmap fallback, raster identity, invalid indices, allocation rollback and cleanup passed");return 0;
}
