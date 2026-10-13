/* Embedded font: independent of installed platform fonts and GPU drivers. */
#ifdef _MSC_VER
#pragma warning(push, 0)
#pragma warning(disable : 4116 4701 4706)
#endif
#define NK_IMPLEMENTATION
/* Exercise allocation failure returns without debug assertions terminating
 * before the production cleanup paths can run. */
#define NK_ASSERT(condition) ((void)0)
#include "ui.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#include <stdio.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Font atlas %d: %s\n",__LINE__,#x);return 1;}} while(0)
typedef struct {size_t live,allocations,requests,fail_request,fail_size;} allocations;
static void *allocate(nk_handle user,void *old,nk_size size) {
 (void)old;allocations *a=user.ptr;if(++a->requests==a->fail_request || (a->fail_size && size>=a->fail_size))return NULL;void *memory=malloc(size);
 if(memory){++a->live;++a->allocations;}return memory;
}
static void release(nk_handle user,void *memory) {
 if(memory){allocations *a=user.ptr;--a->live;free(memory);}
}
int main(void) {
 allocations a={0};struct nk_allocator allocator={nk_handle_ptr(&a),allocate,release};struct nk_font_atlas atlas;
 nk_font_atlas_init(&atlas,&allocator);nk_font_atlas_begin(&atlas);
 static const nk_rune range[]={32,126,0};struct nk_font *fonts[12];
 for(int i=0;i<12;i++) {
  struct nk_font_config config=nk_font_config((float)(42+i*2));config.range=range;config.oversample_h=3;config.oversample_v=1;
  fonts[i]=nk_font_atlas_add_default(&atlas,config.size,&config);CHECK(fonts[i]);
 }
 int w=0,h=0;size_t live=a.live;
 CHECK(!nk_font_atlas_bake_limited(&atlas,&w,&h,NK_FONT_ATLAS_RGBA32,64));
 CHECK(w==0 && h==0 && !atlas.pixel && !atlas.glyphs && a.live==live);
 CHECK(!nk_font_atlas_bake_limited(&atlas,&w,&h,NK_FONT_ATLAS_RGBA32,1024));
 CHECK(w==0 && h==0 && !atlas.pixel && !atlas.glyphs && a.live==live);
 /* Includes temporary/glyph storage, both pack attempts, alpha and rasterization. */
 for(size_t offset=1;offset<=8;offset++) {
  a.fail_request=a.requests+offset;
  const void *failed=nk_font_atlas_bake_limited(&atlas,&w,&h,NK_FONT_ATLAS_RGBA32,2048);
  fprintf(stderr,"Failure offset %zu: pixels=%d, requests=%zu, target=%zu\n",offset,failed!=NULL,a.requests,a.fail_request);
  CHECK(!failed);
  CHECK(!atlas.pixel && !atlas.glyphs && a.live==live);
 }
 a.fail_request=0;
 a.fail_size=8u*1024u*1024u; /* RGBA conversion, beyond alpha and glyph storage. */
 CHECK(!nk_font_atlas_bake_limited(&atlas,&w,&h,NK_FONT_ATLAS_RGBA32,2048));
 CHECK(!atlas.pixel && !atlas.glyphs && a.live==live);a.fail_size=0;
 const unsigned char *pixels=nk_font_atlas_bake_limited(&atlas,&w,&h,NK_FONT_ATLAS_RGBA32,2048);
 fprintf(stderr,"Measured atlas: pixels=%d width=%d height=%d live=%zu baseline=%zu\n",pixels!=NULL,w,h,a.live,live);
 CHECK(pixels && w>1024 && w<=2048 && h>0 && h<=2048);
 for(int i=0;i<12;i++)for(nk_rune c=33;c<=126;c++) {
  const struct nk_font_glyph *g=nk_font_find_glyph(fonts[i],c);CHECK(g && g->codepoint==c && g->x1>g->x0 && g->y1>g->y0);
  CHECK(g->u0>=0 && g->u1<=1 && g->v0>=0 && g->v1<=1 && g->u1>g->u0 && g->v1>g->v0);
  bool ink=false;int left=(int)(g->u0*w),right=(int)(g->u1*w),top=(int)(g->v0*h),bottom=(int)(g->v1*h);
  for(int y=top;y<=bottom && y<h && !ink;y++)for(int x=left;x<=right && x<w;x++)if(pixels[((size_t)y*w+x)*4+3]){ink=true;break;}
  CHECK(ink);
 }
 printf("Complete font atlas %dx%d within 2048: adaptive width, every ASCII glyph, pixel ink, failure cleanup and retry passed\n",w,h);
 nk_font_atlas_end(&atlas,nk_handle_id(0),NULL);nk_font_atlas_clear(&atlas);CHECK(a.live==0);
 /* Non-power-of-two caps must bound both rounded output dimensions. */
 nk_font_atlas_init(&atlas,&allocator);nk_font_atlas_begin(&atlas);CHECK(nk_font_atlas_add_default(&atlas,16,NULL));
 CHECK(nk_font_atlas_bake_limited(&atlas,&w,&h,NK_FONT_ATLAS_ALPHA8,700) && w<=700 && h<=700);
 nk_font_atlas_end(&atlas,nk_handle_id(0),NULL);nk_font_atlas_clear(&atlas);CHECK(a.live==0);
 return 0;
}
