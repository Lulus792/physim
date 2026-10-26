#include "ui.h"
#include "text_wrap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Wrapped editor %d: %s\n",__LINE__,#x);return 1;}}while(0)
typedef struct {char text[8192];int scalars;} clipboard;
static float width(nk_handle u,float h,const char *s,int n){(void)u;(void)h;return (float)nk_utf_len(s,n);}
static float measure(void *u,const char *s,size_t n){(void)u;return (float)nk_utf_len(s,(int)n);}
static void copy(nk_handle u,const char *s,int scalars) {
    clipboard *clip=u.ptr;size_t at=0;clip->scalars=scalars;
    for(int i=0;i<scalars;i++){uint32_t rune;size_t bytes=ps_code_preview_rune(s+at,strlen(s+at),&rune);if(!bytes)return;at+=bytes;}
    if(at>=sizeof clip->text)return;
    memcpy(clip->text,s,at);clip->text[at]=0;
}
static int frame(struct nk_context *ctx,struct nk_text_edit *edit,ps_text_wrap **layout,
                 const char *source,int mouse,int key,int shift,int field_width,float wheel,struct nk_rect *bounds,int *last_drawn) {
    nk_input_begin(ctx);
    for(int i=0;i<NK_KEY_MAX;i++)nk_input_key(ctx,(enum nk_keys)i,nk_false);
    nk_input_key(ctx,NK_KEY_SHIFT,shift!=0);
    if(key>=0)nk_input_key(ctx,(enum nk_keys)key,nk_true);
    if(mouse>=0 || mouse==-2 || mouse==-3) {
        int x=mouse==2 || mouse==3 || mouse==-2?350:(int)bounds->x+3;
        int y=mouse==2 || mouse==3 || mouse==-2?180:(int)bounds->y+3;
        nk_input_motion(ctx,x,y);
        if(mouse>=0)nk_input_button(ctx,NK_BUTTON_LEFT,x,y,mouse==1 || mouse==2);
    }
    nk_input_scroll(ctx,nk_vec2(0,wheel));
    nk_input_end(ctx);
    if(nk_begin(ctx,"wrapped",nk_rect(0,0,400,200),0)) {
        nk_layout_row_static(ctx,80,field_width,1);*bounds=nk_widget_bounds(ctx);
        ps_text_wrap *next=NULL;
        if(!ps_text_wrap_build(source,strlen(source),bounds->w-11,20.72f,measure,NULL,&next))return 0;
        ps_text_wrap_destroy(*layout);*layout=next;edit->visual=&next->visual;
        nk_edit_buffer(ctx,NK_EDIT_BOX|NK_EDIT_READ_ONLY|NK_EDIT_NO_HORIZONTAL_SCROLL,edit,nk_filter_default);
    }
    nk_end(ctx);
    *last_drawn=0;
    const struct nk_command *command;
    nk_foreach(command,ctx)if(command->type==NK_COMMAND_TEXT) {
        const struct nk_command_text *text=(const struct nk_command_text *)command;
        if(text->length==1 && text->string[0]=='!')*last_drawn=1;
    }
    nk_clear(ctx);return 1;
}
int main(void) {
    struct nk_user_font font={.height=12,.width=width};struct nk_context ctx;
    CHECK(nk_init_default(&ctx,&font));clipboard clip={0};ctx.clip.userdata=nk_handle_ptr(&clip);ctx.clip.copy=copy;
    ctx.style.window.padding=nk_vec2(0,0);ctx.style.window.border=0;
    ctx.style.edit.padding=nk_vec2(0,0);ctx.style.edit.border=0;
    ctx.style.edit.scrollbar_size=nk_vec2(10,10);ctx.style.edit.cursor_size=1;
    char source[8192];for(int i=0;i<6000;i++)source[i]=(char)('A'+i%26);strcpy(source+6000,"\r\nαβ 🌍 e\xcc\x81!");
    struct nk_text_edit edit;nk_textedit_init_fixed(&edit,source,sizeof source);
    edit.string.buffer.allocated=strlen(source);edit.string.len=nk_utf_len(source,(int)strlen(source));
    ps_text_wrap *layout=NULL;struct nk_rect bounds={0};int drawn=0;
    #define FRAME(mouse,key,shift,w) CHECK(frame(&ctx,&edit,&layout,source,mouse,key,shift,w,0,&bounds,&drawn))
    #define WHEEL(pointer,delta) CHECK(frame(&ctx,&edit,&layout,source,pointer,-1,0,51,delta,&bounds,&drawn))
    FRAME(-1,-1,0,91);CHECK(layout->visual.row_count>70 && !drawn);
    FRAME(1,-1,0,91);FRAME(0,-1,0,91);CHECK(edit.active && edit.cursor==3);
    FRAME(-1,NK_KEY_DOWN,0,91);CHECK(edit.cursor==83);
    FRAME(-1,NK_KEY_TEXT_LINE_END,0,91);CHECK(edit.cursor==160 && edit.cursor_at_end_of_line);
    CHECK(nk_text_visual_row_at(&layout->visual,edit.cursor,edit.cursor_at_end_of_line)==1);
    FRAME(-1,NK_KEY_RIGHT,0,91);CHECK(edit.cursor==160 && !edit.cursor_at_end_of_line);
    FRAME(-1,NK_KEY_LEFT,0,91);CHECK(edit.cursor==160 && edit.cursor_at_end_of_line);
    FRAME(-1,NK_KEY_TEXT_LINE_START,0,91);CHECK(edit.cursor==80);
    FRAME(-1,NK_KEY_DOWN,1,91);CHECK(edit.select_start==80 && edit.select_end==160);
    FRAME(-1,NK_KEY_COPY,0,91);CHECK(clip.scalars==80 && !memcmp(clip.text,source+80,80) && !clip.text[80]);
    FRAME(-1,NK_KEY_TEXT_END,0,91);CHECK(edit.cursor==edit.string.len && edit.scrollbar.y>1000 && drawn);
    for(int i=0;i<40;i++) {
        int row=nk_text_visual_row_at(&layout->visual,edit.cursor,edit.cursor_at_end_of_line);
        FRAME(-1,NK_KEY_UP,0,91);
        CHECK(nk_text_visual_row_at(&layout->visual,edit.cursor,edit.cursor_at_end_of_line)==row-1);
    }
    FRAME(-1,NK_KEY_TEXT_END,0,91);
    FRAME(-1,NK_KEY_LEFT,0,91);int cluster_end=edit.cursor;
    FRAME(-1,NK_KEY_LEFT,0,91);CHECK(edit.cursor==cluster_end-2);
    FRAME(-1,NK_KEY_TEXT_END,0,91);
    FRAME(-1,NK_KEY_TEXT_SELECT_ALL,0,91);FRAME(-1,NK_KEY_COPY,0,91);CHECK(!strcmp(clip.text,source));
    int start=edit.select_start,end=edit.select_end;
    FRAME(-1,-1,0,51);CHECK(edit.select_start==start && edit.select_end==end && drawn);
    FRAME(-1,NK_KEY_DEL,0,51);CHECK(!strcmp(clip.text,source));
    FRAME(-2,NK_KEY_SCROLL_START,0,51);CHECK(edit.scrollbar.y==0 && edit.active);
    FRAME(-2,NK_KEY_SCROLL_END,0,51);CHECK(edit.scrollbar.y>1000);
    float scrolled=edit.scrollbar.y;
    WHEEL(-2,-4);CHECK(edit.scrollbar.y==scrolled);
    FRAME(2,-1,0,51);FRAME(3,-1,0,51);CHECK(!edit.active);
    FRAME(-3,NK_KEY_SCROLL_START,0,51);CHECK(edit.scrollbar.y==scrolled && !edit.active);
    WHEEL(-3,3);CHECK(edit.scrollbar.y<scrolled && !edit.active);
    ps_text_wrap_destroy(layout);nk_textedit_free(&edit);nk_free(&ctx);
    puts("Wrapped readonly editor: real draw commands, pointer/vertical navigation, soft End/Home, original UTF8+CRLF copy, reflow selection and visible final glyph passed");return 0;
}
