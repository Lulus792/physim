/* Real raster sizes and stable logical metrics, independent of display hardware. */
#include "ui.h"
#include <math.h>
#include <stdio.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Font density %d: %s\n",__LINE__,#x);return 1;}} while(0)
int main(void) {
 static const float densities[]={1,1.25f,1.5f,2};float reference=0;
 static const nk_rune ascii[]={32,126,0};
 for(size_t i=0;i<sizeof densities/sizeof densities[0];i++) {
  float density=densities[i];struct nk_font_atlas atlas;nk_font_atlas_init_default(&atlas);nk_font_atlas_begin(&atlas);
  struct nk_font_config config=nk_font_config(16*density);config.range=ascii;
  struct nk_font *font=nk_font_atlas_add_default(&atlas,config.size,&config);CHECK(font);
  int w=0,h=0;CHECK(nk_font_atlas_bake_limited(&atlas,&w,&h,NK_FONT_ATLAS_ALPHA8,2048));
  nk_font_atlas_end(&atlas,nk_handle_id(0),NULL);
  CHECK(font->info.height==16*density);
  CHECK(ps_ui_font_logical_scale(&atlas,density) && fabsf(font->handle.height-16)<.001f);
  float width=font->handle.width(font->handle.userdata,font->handle.height,"Reading AaBb 0123",17);
  if(i==0)reference=width;else CHECK(fabsf(width-reference)<.01f);
  struct nk_user_font_glyph glyph={0};font->handle.query(font->handle.userdata,16,&glyph,'B',0);
  const struct nk_font_glyph *raw=nk_font_find_glyph(font,'B');CHECK(raw && raw->source_present);
  CHECK(fabsf(glyph.width-(raw->x1-raw->x0)/density)<.001f && glyph.width>0);
  CHECK(ps_ui_font_logical_scale(&atlas,density) && font->handle.height==16);
  CHECK(!ps_ui_font_logical_scale(&atlas,NAN) && !ps_ui_font_logical_scale(&atlas,0) && font->handle.height==16);
  nk_font_atlas_cleanup(&atlas);CHECK(ps_ui_font_has_glyph(&font->handle,'B'));
  printf("Raster density %.2f: baked %.1f, logical %.1f, width %.3f\n",density,font->info.height,font->handle.height,width);
  nk_font_atlas_clear(&atlas);
 }
 puts("Device raster normalization: 1/1.25/1.5/2, stable logical metrics, repeated mapping and invalid input passed");return 0;
}
