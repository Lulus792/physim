#include "ui.h"
#include "text_wrap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Clipped edit %d: %s\n",__LINE__,#x);return 1;}}while(0)
static float width(nk_handle user,float height,const char *text,int bytes) {
 (void)user;(void)height;return (float)nk_utf_len(text,bytes);
}
static float measure(void *user,const char *text,size_t bytes) {(void)user;return (float)nk_utf_len(text,(int)bytes);}
static void frame(struct nk_context *ctx,struct nk_text_edit *edit,const struct nk_text_visual *visual,
                  int x,int y,bool down,struct nk_rect *bounds,struct nk_rect *clip) {
 static nk_uint scroll_x,scroll_y=300;
 nk_input_begin(ctx);nk_input_motion(ctx,x,y);nk_input_button(ctx,NK_BUTTON_LEFT,x,y,down);nk_input_end(ctx);
 if(nk_begin(ctx,"root",nk_rect(0,0,220,220),NK_WINDOW_NO_SCROLLBAR)) {
  nk_layout_space_begin(ctx,NK_STATIC,200,1);nk_layout_space_push(ctx,nk_rect(0,50,180,120));
  if(nk_group_scrolled_offset_begin(ctx,&scroll_x,&scroll_y,"parent",0)) {
   nk_layout_row_dynamic(ctx,visual->row_count*visual->row_height,1);
   *bounds=nk_widget_bounds(ctx);*clip=ctx->current->layout->clip;edit->visual=visual;
   nk_edit_buffer(ctx,NK_EDIT_BOX|NK_EDIT_READ_ONLY|NK_EDIT_NO_HORIZONTAL_SCROLL,edit,nk_filter_default);
   nk_group_end(ctx);
  }
  nk_layout_space_end(ctx);
 }nk_end(ctx);nk_clear(ctx);
}
int main(void) {
 char source[4097];memset(source,'x',sizeof source-1);source[sizeof source-1]=0;
 ps_text_wrap *layout=NULL;CHECK(ps_text_wrap_build(source,strlen(source),80,10,measure,NULL,&layout));
 struct nk_user_font font={0};font.height=10;font.width=width;
 struct nk_context ctx;CHECK(nk_init_default(&ctx,&font));
 ctx.style.edit.padding=nk_vec2(0,0);ctx.style.edit.border=0;ctx.style.edit.scrollbar_size=nk_vec2(0,0);
 struct nk_text_edit edit;nk_textedit_init_fixed(&edit,source,sizeof source);
 edit.string.buffer.allocated=strlen(source);edit.string.len=(int)strlen(source);
 struct nk_rect bounds={0},clip={0};frame(&ctx,&edit,&layout->visual,30,20,false,&bounds,&clip);
 int x=(int)clip.x+20,y=(int)clip.y-10;
 CHECK(y>=bounds.y && y<bounds.y+bounds.h && y<clip.y && clip.h>20);
 frame(&ctx,&edit,&layout->visual,x,y,true,&bounds,&clip);
 CHECK(!edit.active);
 frame(&ctx,&edit,&layout->visual,x,y,false,&bounds,&clip);
 y=(int)clip.y+20;frame(&ctx,&edit,&layout->visual,x,y,true,&bounds,&clip);CHECK(edit.active);
 frame(&ctx,&edit,&layout->visual,x,y,false,&bounds,&clip);CHECK(edit.active);
 CHECK(edit.string.len==4096 && !memcmp(source,layout->source,sizeof source));
 ps_text_wrap_destroy(layout);nk_textedit_free(&edit);nk_free(&ctx);
 puts("Natural readonly edit: clipped pointer cannot activate; visible pointer and original bytes retained");return 0;
}
