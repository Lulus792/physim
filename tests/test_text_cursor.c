#include "ui.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Text cursor %d: %s\n",__LINE__,#x);return 1;}}while(0)
static float width(nk_handle user,float height,const char *text,int bytes){(void)user;return height*.5f*nk_utf_len(text,bytes);}
static struct nk_rect frame(struct nk_context *ctx,struct nk_text_edit *edit,nk_flags flags,int mouse,unsigned *count) {
    nk_input_begin(ctx);nk_input_motion(ctx,30,35);if(mouse>=0)nk_input_button(ctx,NK_BUTTON_LEFT,30,35,mouse!=0);nk_input_end(ctx);
    if(nk_begin(ctx,"cursor",nk_rect(0,0,360,180),NK_WINDOW_NO_SCROLLBAR)){
        nk_layout_row_dynamic(ctx,120,1);nk_edit_buffer(ctx,flags,edit,nk_filter_default);
    }nk_end(ctx);
    struct nk_rect found={0};*count=0;const struct nk_command *command;
    nk_foreach(command,ctx)if(command->type==NK_COMMAND_RECT_FILLED){
        const struct nk_command_rect_filled *rect=(const struct nk_command_rect_filled*)command;
        if(rect->color.r==231 && rect->color.g==17 && rect->color.b==43 && rect->color.a==255){(*count)++;found=nk_rect(rect->x,rect->y,rect->w,rect->h);}
    }
    nk_clear(ctx);return found;
}
int main(void) {
    const nk_flags modes[]={NK_EDIT_FIELD,NK_EDIT_BOX,NK_EDIT_BOX|NK_EDIT_READ_ONLY};
    const int positions[]={0,2,5};unsigned checked=0;
    for(unsigned scale=1;scale<=2;scale++)for(unsigned mode=0;mode<3;mode++){
        struct nk_user_font font={0};font.height=12.f*scale;font.width=width;
        struct nk_context ctx;CHECK(nk_init_default(&ctx,&font));
        ctx.style.edit.cursor_size=(float)scale;ctx.style.edit.color_factor=1;
        ctx.style.edit.cursor_normal=ctx.style.edit.cursor_hover=nk_rgba(231,17,43,255);
        struct nk_text_edit edit;nk_textedit_init_default(&edit);edit.mode=NK_TEXT_EDIT_MODE_INSERT;
        nk_textedit_paste(&edit,"abλcd",(int)strlen("abλcd"));unsigned count;
        frame(&ctx,&edit,modes[mode],-1,&count);frame(&ctx,&edit,modes[mode],1,&count);frame(&ctx,&edit,modes[mode],0,&count);CHECK(edit.active);
        for(unsigned at=0;at<3;at++){
            edit.cursor=edit.select_start=edit.select_end=positions[at];
            struct nk_rect r=frame(&ctx,&edit,modes[mode],-1,&count);
            fprintf(stdout,"caret scale=%u mode=%u position=%d rect=%.2f/%.2f count=%u\n",scale,mode,positions[at],r.w,r.h,count);
            CHECK(count==1 && fabsf(r.w-scale)<.01f && fabsf(r.h-font.height)<.01f);
            CHECK(nk_str_len_char(&edit.string)==(int)strlen("abλcd") && !memcmp(nk_str_get_const(&edit.string),"abλcd",strlen("abλcd")));checked++;
        }
        edit.select_start=0;edit.select_end=2;frame(&ctx,&edit,modes[mode],-1,&count);CHECK(!count);
        nk_textedit_free(&edit);nk_free(&ctx);
    }
    printf("Text cursor: %u actual caret rectangles, editable/readonly fields, Unicode, line ends and selection passed\n",checked);return 0;
}
