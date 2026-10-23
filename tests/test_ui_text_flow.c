#include "ui.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Text flow %d: %s\n",__LINE__,#x);return 1;}}while(0)
static float width(nk_handle user,float height,const char *text,int length) {
    (void)user;(void)height;float result=0;nk_rune previous=0;
    for(int at=0;at<length;) {
        nk_rune rune;int n=nk_utf_decode(text+at,&rune,length-at);if(!n)return NAN;
        result+=rune==' '?4:rune>127?12:7;if(previous=='A' && rune=='V')result-=3;
        previous=rune;at+=n;
    }return result;
}
static unsigned hook(void *user,const char *window,const char *name,int role,const float bounds[4],bool enabled,bool checked,const char *group,bool activated) {
    (void)window;(void)name;(void)role;(void)bounds;(void)checked;(void)group;(void)activated;
    unsigned *calls=user;(*calls)++;return enabled?4:0;
}
int main(void) {
    struct nk_user_font font={.height=12,.width=width};
    CHECK(ps_ui_text_line(&font,"AVZ",3,11,false)==2);
    CHECK(ps_ui_text_line(&font,"AVZ",3,10,false)==1);
    CHECK(ps_ui_text_line(&font,"A",1,6,false)==0);
    CHECK(!ps_ui_text_line(&font,"A",1,NAN,false) && !ps_ui_text_line(NULL,"A",1,10,false));
    CHECK(ps_ui_text_line(&font,"D:/abcdef",9,42,true)==3);
    const char *text="D:/Messung mit Leerzeichen/αβ/über 🚀/Modell.phys";
    char copy[4096]={0};int length=(int)strlen(text),done=0;unsigned lines=0;
    while(done<length) {
        int n=ps_ui_text_line(&font,text+done,length-done,67,true);CHECK(n>0 && n<=length-done);
        CHECK(width(nk_handle_id(0),12,text+done,n)<=67);
        CHECK(done+n==length || ((unsigned char)text[done+n]&0xc0)!=0x80);
        memcpy(copy+done,text+done,(size_t)n);done+=n;lines++;
    }
    CHECK(lines>3 && !strcmp(copy,text));
    memset(copy,'W',sizeof copy-1);copy[sizeof copy-1]=0;done=0;
    while(done<(int)sizeof copy-1){int n=ps_ui_text_line(&font,copy+done,(int)sizeof copy-1-done,53,true);CHECK(n>0 && n<=7);done+=n;}
    struct nk_context ui;CHECK(nk_init_default(&ui,&font));unsigned calls=0;
    ps_ui_font_layout layout={.magic=PS_UI_LAYOUT_MAGIC,.accessibility=hook,.accessibility_user=&calls,.focus_color={40,80,160,255}};ui.userdata=nk_handle_ptr(&layout);
    CHECK(nk_begin(&ui,"test",nk_rect(0,0,300,200),NK_WINDOW_NO_SCROLLBAR));
    struct nk_command_buffer *c=nk_window_get_canvas(&ui);struct nk_rect box=nk_rect(30,40,100,28);nk_size before=c->end;bool focused=false;
    CHECK(!ps_ui_button_at_custom_focus(&ui,box,"Zoom +",false,&focused) && focused && calls==1 && c->end==before);
    CHECK(!ps_ui_button_at(&ui,box,"Zoom -",false) && calls==2 && c->end>before);
    nk_widget_disable_begin(&ui);focused=true;CHECK(!ps_ui_button_at_custom_focus(&ui,box,"disabled",false,&focused) && !focused);nk_widget_disable_end(&ui);
    focused=true;CHECK(!ps_ui_button_at_custom_focus(&ui,nk_rect(-1000,-1000,1,1),"offscreen",false,&focused) && !focused);
    nk_end(&ui);nk_free(&ui);puts("UTF-8 flow: kerning, preserved path bytes, long tokens and custom focus passed");return 0;
}
