#include "../app/text_wrap.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Visual wrap %d: %s\n",__LINE__,#x);return 1;}}while(0)
static float mono(void *user,const char *text,size_t bytes){(void)user;(void)text;(void)bytes;return 1;}
int main(void) {
    ps_text_wrap *p=NULL;const char *text="abcdefghij\r\ne\xcc\x81X\n";
    CHECK(ps_text_wrap_build(text,strlen(text),4,20,mono,NULL,&p));
    CHECK(!strcmp(p->source,text) && p->bytes==strlen(text));
    const struct nk_text_visual *v=&p->visual;
    CHECK(v->row_count==5 && v->glyph_count==16);
    CHECK(v->rows[0].first==0 && v->rows[0].end==4 && v->rows[0].limit==4);
    CHECK(v->rows[2].first==8 && v->rows[2].end==10 && v->rows[2].limit==12);
    CHECK(v->rows[3].first==12 && v->rows[3].end==15 && v->rows[3].limit==16);
    CHECK(v->rows[4].first==16 && v->rows[4].end==16 && v->glyphs[16].x==0);
    CHECK(!v->glyphs[11].boundary && !v->glyphs[13].boundary);
    CHECK(v->glyphs[14].byte==15 && v->glyphs[15].byte==16);
    CHECK(nk_text_visual_row_at(v,4,0)==1 && nk_text_visual_row_at(v,4,1)==0);
    CHECK(nk_text_visual_x(v,4,0)==4 && nk_text_visual_x(v,4,1)==0);
    int affinity=0;
    CHECK(nk_text_visual_hit(v,3.9f,1,&affinity)==4 && affinity);
    CHECK(nk_text_visual_hit(v,.9f,61,&affinity)==12 && !affinity);
    CHECK(nk_text_visual_hit(v,1.1f,61,&affinity)==14);
    CHECK(nk_text_visual_hit(v,100,10000,&affinity)==16);
    ps_text_wrap_destroy(p);
    CHECK(ps_text_wrap_build("a\tbb",4,4,20,mono,NULL,&p));
    CHECK(p->visual.row_count==2 && p->glyphs[1].x==1 && p->glyphs[1].advance==7);
    ps_text_wrap_destroy(p);
    CHECK(ps_text_wrap_build("",0,1,20,mono,NULL,&p));
    ps_text_wrap *prior=p;
    CHECK(!ps_text_wrap_build("bad\0text",8,4,20,mono,NULL,&p) && p==prior);
    CHECK(!ps_text_wrap_build("x",1,4,NAN,mono,NULL,&p) && p==prior);
    ps_text_wrap_destroy(p);
    puts("Visual map: exact bytes, soft/hard rows, CRLF/combining caret stops, hit affinity, tabs and failed replacement passed");return 0;
}
