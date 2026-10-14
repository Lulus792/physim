/* Real source fonts, overlapping missing glyphs and distinct merge targets. */
#ifdef _MSC_VER
#pragma warning(push, 0)
#pragma warning(disable : 4116 4701 4706)
#endif
#define NK_IMPLEMENTATION
#define NK_ASSERT(condition) ((void)0)
#include "ui.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#include <stdio.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Font fallback %d: %s\n",__LINE__,#x);return 1;}} while(0)
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
 CHECK(argc==2);char path[4096];
 CHECK(snprintf(path,sizeof path,"%s/third_party/fonts/NotoSans.ttf",argv[1])<(int)sizeof path);
 static const nk_rune primary[]={32,126,0x378,0x378,0x3b8,0x3b8,0};
 static const nk_rune extra[]={0x378,0x378,0x3b8,0x3b8,0};
 struct nk_font_atlas atlas;nk_font_atlas_init_default(&atlas);nk_font_atlas_begin(&atlas);
 struct nk_font_config config=nk_font_config(16);config.range=primary;
 struct nk_font *first=nk_font_atlas_add_default(&atlas,16,&config);CHECK(first);
 config.size=32;struct nk_font *second=nk_font_atlas_add_default(&atlas,32,&config);CHECK(second && second!=first);
 config.merge_mode=1;config.range=extra;
 int before=atlas.font_num;
 CHECK(nk_font_atlas_add_from_file(&atlas,path,32,&config)==second);
 CHECK(atlas.font_num==before+1);
 /* The documented merge target is the last handle, never the first one. */
 CHECK(first->config->n==first->config);
 CHECK(second->config->n!=second->config && second->config->n->font==&second->info);
 int w=0,h=0;const unsigned char *pixels=nk_font_atlas_bake_limited(&atlas,&w,&h,NK_FONT_ATLAS_ALPHA8,2048);CHECK(pixels);
 const struct nk_font_glyph *theta=nk_font_find_glyph(second,0x3b8);
 CHECK(theta && theta->codepoint==0x3b8 && theta->x1>theta->x0 && theta->y1>theta->y0);
 /* Prove the resolved glyph belongs to the added source, not the primary's .notdef. */
 CHECK(theta==second->glyphs+(126-32+1)+2+1);
 bool ink=false;
 for(int y=(int)(theta->v0*h);y<(int)(theta->v1*h) && y<h;y++)
  for(int x=(int)(theta->u0*w);x<(int)(theta->u1*w) && x<w;x++)if(pixels[(size_t)y*w+x])ink=true;
 CHECK(ink);
 CHECK(theta->source_present);
 CHECK(nk_font_find_glyph(first,0x3b8)->codepoint!=0x3b8);
 CHECK(nk_font_find_glyph(second,0x378)->codepoint!=0x378);
 CHECK(nk_font_find_glyph(second,32)->codepoint==32 && nk_font_find_glyph(second,32)->source_present);
 nk_font_atlas_end(&atlas,nk_handle_id(0),NULL);
 nk_font_atlas_cleanup(&atlas);
 CHECK(nk_font_find_glyph(second,0x3b8)==theta);
 nk_font_atlas_clear(&atlas);
 /* Allocation failure cannot attach a partially initialized config or handle. */
 memory_state state={0};struct nk_allocator allocator={nk_handle_ptr(&state),allocate,release};
 nk_font_atlas_init(&atlas,&allocator);nk_font_atlas_begin(&atlas);
 struct nk_font *seed=nk_font_atlas_add_default(&atlas,16,NULL);CHECK(seed);
 struct nk_font_config copy=*seed->config;copy.ttf_data_owned_by_atlas=0;
 size_t baseline=state.live;int sources=atlas.font_num;
 for(size_t offset=1;offset<=3;offset++) {
  state.fail=state.requests+offset;CHECK(!nk_font_atlas_add(&atlas,&copy));
  CHECK(state.live==baseline && atlas.font_num==sources && atlas.fonts==seed && !seed->next && !atlas.config->next);
 }
 copy.merge_mode=1;
 for(size_t offset=1;offset<=2;offset++) {
  state.fail=state.requests+offset;CHECK(!nk_font_atlas_add(&atlas,&copy));
  CHECK(state.live==baseline && atlas.font_num==sources && seed->config->n==seed->config);
 }
 state.fail=0;CHECK(nk_font_atlas_add(&atlas,&copy)==seed);
 CHECK(nk_font_atlas_bake_limited(&atlas,&w,&h,NK_FONT_ATLAS_ALPHA8,2048));
 nk_font_atlas_end(&atlas,nk_handle_id(0),NULL);nk_font_atlas_clear(&atlas);CHECK(state.live==0);
 puts("Font fallback: last-handle merge, missing-source overlap, raster ink and post-cleanup lookup passed");
 return 0;
}
