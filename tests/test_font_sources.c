/* The application's real fallback builder, including ownership on failures. */
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Font sources %d: %s\n",__LINE__,#x);return 1;}} while(0)
typedef struct {size_t live,requests,fail;} memory_state;
static void *allocate(nk_handle user,void *old,nk_size size) {
 (void)old;memory_state *state=user.ptr;
 if(++state->requests==state->fail)return NULL;
 void *p=malloc(size);if(p)state->live++;return p;
}
static void release(nk_handle user,void *p) {
 if(p){memory_state *state=user.ptr;state->live--;free(p);}
}
int main(int argc,char **argv) {
 CHECK(argc==2);
 static const nk_rune primary[]={32,126,0x378,0x378,0x3b8,0x3b8,0};
 static const nk_rune ranges[]={32,126,0x378,0x378,0x3b8,0x3b8,0x1d40,0x1d40,0x1d65,0x1d65,
  0x222b,0x222b,0x2318,0x2318,0x2500,0x2500,0x2502,0x2502,0x250c,0x250c,0x2510,0x2510,0x2514,0x2514,
  0x2518,0x2518,0x251c,0x251c,0x252c,0x252c,0x2534,0x2534,0x2603,0x2603,
  0x2c7c,0x2c7c,0x1f30d,0x1f30d,0x1f600,0x1f600,0};
 static const char *const files[]={"NotoSans.ttf","NotoSansMono.ttf","NotoSansMath-Regular.ttf","NotoSansSymbols2-Regular.ttf","NotoEmoji.ttf"};
 static const nk_rune required[]={32,0x3b8,0x1d40,0x1d65,0x222b,0x2318,0x2500,0x2502,0x250c,0x2510,0x2514,
  0x2518,0x251c,0x252c,0x2534,0x2603,0x2c7c,0x1f30d,0x1f600};
 for(int size=16;size<=32;size+=16) {
  memory_state state={0};struct nk_allocator allocator={nk_handle_ptr(&state),allocate,release};
  struct nk_font_atlas atlas;nk_font_atlas_init(&atlas,&allocator);nk_font_atlas_begin(&atlas);
  struct nk_font_config config=nk_font_config((float)size);config.range=primary;
  struct nk_font *first=nk_font_atlas_add_default(&atlas,(float)size,&config);CHECK(first);
  struct nk_font *target=nk_font_atlas_add_default(&atlas,(float)size,&config);CHECK(target);
  char path[4096];CHECK(snprintf(path,sizeof path,"%s/third_party/fonts/NotoSans.ttf",argv[1])<(int)sizeof path);
  size_t baseline=state.live;int count=atlas.font_num;
  CHECK(!ps_ui_font_add_fallback(&atlas,first,path,ranges));
  CHECK(!ps_ui_font_add_fallback(&atlas,target,"missing-font.ttf",ranges));
  CHECK(state.live==baseline && atlas.font_num==count && target->config->n==target->config);
  /* Source bytes, range storage and config: each failure is atomic and freed. */
  for(size_t offset=1;offset<=3;offset++) {
   state.fail=state.requests+offset;
   CHECK(!ps_ui_font_add_fallback(&atlas,target,path,ranges));
   CHECK(state.live==baseline && atlas.font_num==count && target->config->n==target->config);
  }
  state.fail=0;
  for(size_t i=0;i<sizeof files/sizeof files[0];i++) {
   CHECK(snprintf(path,sizeof path,"%s/third_party/fonts/%s",argv[1],files[i])<(int)sizeof path);
   CHECK(ps_ui_font_add_fallback(&atlas,target,path,ranges));
  }
  CHECK(first->config->n==first->config);
  /* Each successful retry adds only previously missing source glyphs. */
  const struct nk_font_config *source=target->config->n;
  while(source!=target->config) {
   for(const nk_rune *r=source->range;*r;r+=2)CHECK(r[0]>126);
   CHECK(source->range_owned_by_atlas);source=source->n;
  }
  int w=0,h=0;const unsigned char *pixels=nk_font_atlas_bake_limited(&atlas,&w,&h,NK_FONT_ATLAS_ALPHA8,2048);CHECK(pixels);
  for(size_t i=0;i<sizeof required/sizeof required[0];i++) {
   const struct nk_font_glyph *g=nk_font_find_glyph(target,required[i]);
   CHECK(g && g->codepoint==required[i] && g->source_present && ps_ui_font_has_glyph(&target->handle,required[i]));
   if(required[i]==32)continue;
   CHECK(g->x1>g->x0 && g->y1>g->y0);
   bool ink=false;
   for(int y=(int)(g->v0*h);y<(int)(g->v1*h) && y<h;y++)
    for(int x=(int)(g->u0*w);x<(int)(g->u1*w) && x<w;x++)if(pixels[(size_t)y*w+x])ink=true;
   CHECK(ink);
  }
  CHECK(!ps_ui_font_has_glyph(&first->handle,0x3b8) && !ps_ui_font_has_glyph(&target->handle,0x378));
  nk_font_atlas_end(&atlas,nk_handle_id(0),NULL);nk_font_atlas_cleanup(&atlas);
  CHECK(ps_ui_font_has_glyph(&target->handle,0x1f30d));
  nk_font_atlas_clear(&atlas);CHECK(state.live==0);
 }
 /* Strong emphasis needs distinct real raster coverage, not merely a second handle. */
 memory_state state={0};struct nk_allocator allocator={nk_handle_ptr(&state),allocate,release};
 struct nk_font_atlas atlas;nk_font_atlas_init(&atlas,&allocator);nk_font_atlas_begin(&atlas);
 static const nk_rune latin[]={32,126,0};struct nk_font_config config=nk_font_config(32);config.range=latin;
 const char *styles[]={"NotoSans.ttf","NotoSans-Bold.ttf"};struct nk_font *style_fonts[2];char path[4096];
 for(int i=0;i<2;i++) {
  CHECK(snprintf(path,sizeof path,"%s/third_party/fonts/%s",argv[1],styles[i])<(int)sizeof path);
  style_fonts[i]=nk_font_atlas_add_from_file(&atlas,path,32,&config);CHECK(style_fonts[i]);
 }
 int w=0,h=0;const unsigned char *pixels=nk_font_atlas_bake_limited(&atlas,&w,&h,NK_FONT_ATLAS_ALPHA8,2048);CHECK(pixels);
 size_t coverage[2]={0};
 for(int i=0;i<2;i++)for(nk_rune c='A';c<='Z';c++) {
  const struct nk_font_glyph *g=nk_font_find_glyph(style_fonts[i],c);CHECK(g && g->codepoint==c && g->source_present);
  for(int y=(int)(g->v0*h);y<(int)(g->v1*h) && y<h;y++)
   for(int x=(int)(g->u0*w);x<(int)(g->u1*w) && x<w;x++)coverage[i]+=pixels[(size_t)y*w+x];
 }
 fprintf(stderr,"Actual Latin raster coverage: regular=%zu strong=%zu\n",coverage[0],coverage[1]);
 CHECK(coverage[0]>0 && (double)coverage[1]>(double)coverage[0]*1.2);
 nk_font_atlas_end(&atlas,nk_handle_id(0),NULL);nk_font_atlas_cleanup(&atlas);
 CHECK(ps_ui_font_has_glyph(&style_fonts[1]->handle,'B'));
 nk_font_atlas_clear(&atlas);CHECK(state.live==0);
 puts("Offline font sources: missing glyphs, 16/32 raster ink, real strong coverage, failed-add ownership, retry and cleanup passed");return 0;
}
