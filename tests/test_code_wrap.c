#include "../app/code_preview.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Code wrap %d: %s\n",__LINE__,#x);return 1;}}while(0)
static float mono(void *user,const char *text,size_t bytes){(void)user;(void)text;(void)bytes;return 1;}
static float bad(void *user,const char *text,size_t bytes){(void)user;(void)text;(void)bytes;return NAN;}
static bool line(const ps_code_preview *p,size_t index,const char *text) {
    return index<p->count && p->lines[index].end-p->lines[index].begin==strlen(text) &&
        !memcmp(p->source+p->lines[index].begin,text,strlen(text));
}
int main(void) {
    ps_code_preview *p=NULL;char token[6001];memset(token,'x',6000);token[6000]=0;
    CHECK(ps_code_preview_build_anywhere(token,6000,31,mono,NULL,&p));
    CHECK(p->count==194 && p->maximum==31 && p->source==token);
    size_t at=0;
    for(size_t i=0;i<p->count;i++) {
        CHECK(p->lines[i].begin==at && p->lines[i].end>at && p->lines[i].width<=31);
        at=p->lines[i].end;
    }
    CHECK(at==6000 && strlen(token)==6000);for(size_t i=0;i<6000;i++)CHECK(token[i]=='x');
    ps_code_preview_destroy(p);
    const char *source="ab cd";CHECK(ps_code_preview_build_anywhere(source,5,3,mono,NULL,&p));
    CHECK(p->count==2 && line(p,0,"ab ") && line(p,1,"cd"));ps_code_preview_destroy(p);
    source="αβγδε";CHECK(ps_code_preview_build_anywhere(source,strlen(source),2,mono,NULL,&p));
    CHECK(p->count==3 && line(p,0,"αβ") && line(p,1,"γδ") && line(p,2,"ε"));ps_code_preview_destroy(p);
    /* Combining marks, emoji ZWJ, RI flags and Indic conjuncts are indivisible.
     * A cluster wider than the viewport is reported, never split or hidden. */
    const char *clusters[]={"e\xcc\x81","\xf0\x9f\x91\xa9\xe2\x80\x8d\xf0\x9f\x92\xbb",
        "\xf0\x9f\x87\xa9\xf0\x9f\x87\xaa","\xe0\xa4\x95\xe0\xa5\x8d\xe0\xa4\xb7"};
    for(size_t i=0;i<sizeof clusters/sizeof *clusters;i++) {
        char text[100];snprintf(text,sizeof text,"%sX",clusters[i]);
        CHECK(ps_code_preview_build_anywhere(text,strlen(text),1,mono,NULL,&p));
        CHECK(p->count==2 && line(p,0,clusters[i]) && line(p,1,"X"));ps_code_preview_destroy(p);
    }
    source="a\tbb";CHECK(ps_code_preview_build_anywhere(source,4,4,mono,NULL,&p));
    CHECK(p->count==2 && line(p,0,"a\t") && line(p,1,"bb") && p->tab==8);ps_code_preview_destroy(p);
    source="abc\xc2\xad" "x";CHECK(ps_code_preview_build_anywhere(source,strlen(source),3,mono,NULL,&p));
    CHECK(p->count==2 && p->maximum==3 && !p->lines[0].hyphen);ps_code_preview_destroy(p);
    source="ab\xc2\xad" "xy";CHECK(ps_code_preview_build_anywhere(source,strlen(source),3,mono,NULL,&p));
    CHECK(p->count==2 && p->maximum==3 && p->lines[0].hyphen);ps_code_preview_destroy(p);
    source="a\xc2\xad" "b";CHECK(ps_code_preview_build_anywhere(source,strlen(source),1,mono,NULL,&p));
    CHECK(p->count==2 && p->maximum==1 && !p->lines[0].hyphen);ps_code_preview_destroy(p);
    source="x\r\n\r\ny\n";CHECK(ps_code_preview_build_anywhere(source,strlen(source),4,mono,NULL,&p));
    CHECK(p->count==4 && line(p,0,"x") && line(p,1,"") && line(p,2,"y") && line(p,3,""));
    CHECK(p->lines[1].begin==3 && p->lines[2].begin==5 && p->lines[3].begin==7);ps_code_preview_destroy(p);
    source="a        ";CHECK(ps_code_preview_build_anywhere(source,strlen(source),2,mono,NULL,&p));
    CHECK(p->count==1 && line(p,0,source) && p->maximum==1);ps_code_preview_destroy(p);
    CHECK(ps_code_preview_build_anywhere("",0,1,mono,NULL,&p));CHECK(p->count==1 && p->lines[0].end==0);
    ps_code_preview *prior=p;
    CHECK(!ps_code_preview_build_anywhere("\xf0\x80\x80\x80",4,10,mono,NULL,&p) && p==prior);
    CHECK(!ps_code_preview_build_anywhere("a\0b",3,10,mono,NULL,&p) && p==prior);
    CHECK(!ps_code_preview_build_anywhere("x",1,10,bad,NULL,&p) && p==prior);
    CHECK(!ps_code_preview_build_anywhere("x",1,INFINITY,mono,NULL,&p) && p==prior);
    CHECK(!ps_code_preview_build_anywhere("x",1,0,mono,NULL,&p) && p==prior);ps_code_preview_destroy(p);
    puts("Anywhere wrap: long tokens, UTF-8/graphemes, preferred opportunities, tabs, CRLF/trailing caret row and failure preservation passed");return 0;
}
