#include "ui.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Pointer control %d: %s\n",__LINE__,#x);return 1;}}while(0)
typedef struct {enum nk_buttons button;int x,y;bool down;} event;
static float width(nk_handle user,float height,const char *text,int bytes){(void)user;(void)height;(void)text;return bytes*8.f;}
enum {CLIPPED=1,DISABLED=2,CONSUMED=4};
static bool frame(struct nk_context *ctx,const event *events,size_t count,unsigned flags) {
    nk_input_begin(ctx);
    for(size_t i=0;i<count;i++){nk_input_motion(ctx,events[i].x,events[i].y);nk_input_button(ctx,events[i].button,events[i].x,events[i].y,events[i].down);}
    nk_input_end(ctx);bool activated=false;
    if(flags&CONSUMED)ctx->input.mouse.buttons[NK_BUTTON_LEFT].clicked=0;
    if(nk_begin(ctx,"root",nk_rect(0,0,180,140),NK_WINDOW_NO_SCROLLBAR)) {
        if(flags&CLIPPED)ctx->current->layout->clip=nk_rect(60,30,60,30);
        if(flags&DISABLED)nk_widget_disable_begin(ctx);
        activated=ps_ui_button_at(ctx,nk_rect(20,30,100,30),"Probe",false);
        if(flags&DISABLED)nk_widget_disable_end(ctx);
    }
    nk_end(ctx);nk_clear(ctx);return activated;
}
static void edit_frame(struct nk_context *ctx,struct nk_text_edit *edit,const event *events,size_t count) {
    nk_input_begin(ctx);for(size_t i=0;i<count;i++){nk_input_motion(ctx,events[i].x,events[i].y);nk_input_button(ctx,events[i].button,events[i].x,events[i].y,events[i].down);}nk_input_end(ctx);
    if(nk_begin(ctx,"root",nk_rect(0,0,180,140),NK_WINDOW_NO_SCROLLBAR)) {
        nk_layout_space_begin(ctx,NK_STATIC,100,1);nk_layout_space_push(ctx,nk_rect(20,30,100,30));
        nk_edit_buffer(ctx,NK_EDIT_FIELD,edit,nk_filter_default);nk_layout_space_end(ctx);
    }nk_end(ctx);
    if(nk_begin(ctx,"other",nk_rect(100,90,180,40),NK_WINDOW_NO_SCROLLBAR))nk_layout_row_dynamic(ctx,10,1);
    nk_end(ctx);nk_clear(ctx);
}
static bool combo_frame(struct nk_context *ctx,const event *events,size_t count,struct nk_rect *bounds) {
    nk_input_begin(ctx);
    for(size_t i=0;i<count;i++){nk_input_motion(ctx,events[i].x,events[i].y);nk_input_button(ctx,events[i].button,events[i].x,events[i].y,events[i].down);}
    nk_input_end(ctx);bool open=false;
    if(nk_begin(ctx,"combo-root",nk_rect(0,0,240,200),NK_WINDOW_NO_SCROLLBAR)) {
        nk_layout_row_dynamic(ctx,30,1);*bounds=nk_widget_bounds(ctx);
        open=ps_ui_combo_begin_named_scaled(ctx,"Auswahl","Option",nk_vec2(180,100),1);
        if(open){nk_layout_row_dynamic(ctx,30,1);nk_label(ctx,"Menüinhalt",NK_TEXT_LEFT);nk_combo_end(ctx);}
    }nk_end(ctx);nk_clear(ctx);return open;
}
int main(void) {
    struct nk_user_font font={0};font.height=14;font.width=width;struct nk_context ctx;CHECK(nk_init_default(&ctx,&font));
    CHECK(!frame(&ctx,NULL,0,0));
    const event tap[]={{NK_BUTTON_LEFT,40,40,true},{NK_BUTTON_LEFT,40,40,false}};
    bool clicked=frame(&ctx,tap,2,0);
    printf("Rapid same-frame control: transitions=%u down=%d activated=%d\n",ctx.input.mouse.buttons[NK_BUTTON_LEFT].clicked,ctx.input.mouse.buttons[NK_BUTTON_LEFT].down,clicked);
    CHECK(clicked);CHECK(!frame(&ctx,NULL,0,0));CHECK(!frame(&ctx,tap,2,CONSUMED));
    CHECK(frame(&ctx,tap,1,0));CHECK(!frame(&ctx,NULL,0,0));CHECK(!frame(&ctx,tap+1,1,0));
    const event entered[]={{NK_BUTTON_LEFT,4,4,true},{NK_BUTTON_LEFT,40,40,false}};
    CHECK(!frame(&ctx,entered,2,0));
    const event left[]={{NK_BUTTON_LEFT,40,40,true},{NK_BUTTON_LEFT,160,100,false}};
    CHECK(frame(&ctx,left,2,0));
    const event clip[]={{NK_BUTTON_LEFT,40,40,true},{NK_BUTTON_LEFT,80,40,false}};
    CHECK(!frame(&ctx,clip,2,CLIPPED));CHECK(!frame(&ctx,tap,2,DISABLED));
    const event right[]={{NK_BUTTON_RIGHT,40,40,true},{NK_BUTTON_RIGHT,40,40,false}};
    CHECK(!frame(&ctx,right,2,0));
    const event mixed[]={{NK_BUTTON_LEFT,40,40,true},{NK_BUTTON_LEFT,40,40,false},{NK_BUTTON_RIGHT,160,100,true},{NK_BUTTON_RIGHT,160,100,false}};
    CHECK(frame(&ctx,mixed,4,0));CHECK(!frame(&ctx,NULL,0,0));
    nk_free(&ctx);
    struct nk_context combo;CHECK(nk_init_default(&combo,&font));struct nk_rect combo_bounds;
    CHECK(!combo_frame(&combo,NULL,0,&combo_bounds));
    int combo_x=(int)(combo_bounds.x+combo_bounds.w/2),combo_y=(int)(combo_bounds.y+combo_bounds.h/2);
    const event combo_tap[]={{NK_BUTTON_LEFT,combo_x,combo_y,true},{NK_BUTTON_LEFT,combo_x,combo_y,false}};
    CHECK(combo_frame(&combo,combo_tap,2,&combo_bounds));
    CHECK(combo_frame(&combo,NULL,0,&combo_bounds));nk_free(&combo);
    struct nk_context fast,slow;CHECK(nk_init_default(&fast,&font));CHECK(nk_init_default(&slow,&font));
    struct nk_text_edit fast_edit,slow_edit;char fast_text[64]="0123456789abcdef",slow_text[64]="0123456789abcdef";
    nk_textedit_init_fixed(&fast_edit,fast_text,sizeof fast_text);nk_textedit_init_fixed(&slow_edit,slow_text,sizeof slow_text);
    fast_edit.string.buffer.allocated=slow_edit.string.buffer.allocated=16;fast_edit.string.len=slow_edit.string.len=16;
    edit_frame(&fast,&fast_edit,NULL,0);edit_frame(&slow,&slow_edit,NULL,0);
    nk_window_set_focus(&fast,"other");nk_window_set_focus(&slow,"other");
    const event caret[]={{NK_BUTTON_LEFT,80,50,true},{NK_BUTTON_LEFT,80,50,false}};
    edit_frame(&fast,&fast_edit,caret,2);edit_frame(&slow,&slow_edit,caret,1);edit_frame(&slow,&slow_edit,caret+1,1);
    CHECK(fast_edit.active && slow_edit.active && fast_edit.cursor==slow_edit.cursor && fast_edit.cursor>0);
    const event drag[]={{NK_BUTTON_LEFT,40,50,true},{NK_BUTTON_LEFT,104,50,false}};
    const event move[]={{NK_BUTTON_LEFT,104,50,true}};
    edit_frame(&fast,&fast_edit,drag,2);edit_frame(&slow,&slow_edit,drag,1);
    edit_frame(&slow,&slow_edit,move,1);edit_frame(&slow,&slow_edit,drag+1,1);
    CHECK(fast_edit.cursor==slow_edit.cursor && fast_edit.select_start==slow_edit.select_start && fast_edit.select_end==slow_edit.select_end);
    CHECK(fast_edit.select_start!=fast_edit.select_end);
    CHECK(!strcmp(fast_text,slow_text));nk_textedit_free(&fast_edit);nk_textedit_free(&slow_edit);nk_free(&fast);nk_free(&slow);
    puts("Shared press controls: rapid tap/combo, held state, press origin, consumed/clipped/disabled input, button isolation and fast/slow edit caret/selection passed");return 0;
}
