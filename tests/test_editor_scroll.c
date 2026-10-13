#include "ui.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Editor scroll %d: %s\n",__LINE__,#x);return 1;}} while(0)
static float width(nk_handle user,float height,const char *text,int length) {
 (void)user;(void)height;(void)text;return (float)length*8;
}
int main(void) {
 struct nk_user_font font={0};font.height=16;font.width=width;struct nk_context ctx;CHECK(nk_init_default(&ctx,&font));
 char active[]="ABCD",inactive[64]="0123456789012345678901234567890123456789";int active_length=4,inactive_length=40;
 float offset=0;short origin=0;
 for(int frame=0;frame<3;frame++) {
  nk_input_begin(&ctx);nk_input_end(&ctx);
  if(nk_begin(&ctx,"Fields",nk_rect(0,0,200,150),0)) {
   nk_layout_row_static(&ctx,24,100,1);
   nk_edit_focus(&ctx,NK_EDIT_ALWAYS_INSERT_MODE);
   ctx.current->edit.cursor=1;ctx.current->edit.sel_start=1;ctx.current->edit.sel_end=2;
   nk_flags state=nk_edit_string(&ctx,NK_EDIT_FIELD,active,&active_length,sizeof active,nk_filter_default);
   CHECK(state&NK_EDIT_ACTIVE);
   if(frame==1)offset=80;
   state=nk_edit_string_scrolled(&ctx,NK_EDIT_FIELD,inactive,&inactive_length,sizeof inactive,nk_filter_default,&offset);
   CHECK(state&NK_EDIT_INACTIVE);
   CHECK(ctx.current->edit.active && ctx.current->edit.name==0 && ctx.current->edit.cursor==1 &&
         ctx.current->edit.sel_start==1 && ctx.current->edit.sel_end==2 && ctx.current->edit.scrollbar.x==0);
  }
  nk_end(&ctx);const struct nk_command *command;bool found=false;
  nk_foreach(command,&ctx)if(command->type==NK_COMMAND_TEXT) {
   const struct nk_command_text *text=(const struct nk_command_text*)command;
   if(text->length==40 && !memcmp(text->string,inactive,40)) {
    if(!frame)origin=text->x;else CHECK(text->x==origin-80);found=true;
   }
  }
  CHECK(found && offset==(frame?80:0) && active_length==4 && inactive_length==40 && !strcmp(active,"ABCD"));
  nk_clear(&ctx);
 }
 nk_free(&ctx);puts("Unfocused field scroll: actual draw offset persists, other field focus/cursor/selection and values unchanged");return 0;
}
