#include "../app/code_preview.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Code preview %d: %s\n",__LINE__,#x);return 1;}}while(0)
static float mono(void *user,const char *s,size_t n) {(void)user;(void)s;(void)n;return 1;}
static float invalid(void *user,const char *s,size_t n) {(void)user;(void)s;(void)n;return NAN;}
static bool line(ps_code_preview *p,size_t i,const char *text) {
    return i<p->count && p->lines[i].end-p->lines[i].begin==strlen(text) && !memcmp(p->source+p->lines[i].begin,text,strlen(text));
}
int main(void) {
    ps_code_preview *p=NULL;
    const char *s="aa  bb cc";CHECK(ps_code_preview_build(s,strlen(s),5,mono,NULL,&p));
    CHECK(p->count==2 && line(p,0,"aa  ") && line(p,1,"bb cc") && p->maximum==5);ps_code_preview_destroy(p);
    s="  identifier_without_any_break θθθ";CHECK(ps_code_preview_build(s,strlen(s),8,mono,NULL,&p));
    CHECK(p->maximum>8 && line(p,1,"identifier_without_any_break ") && line(p,2,"θθθ"));ps_code_preview_destroy(p);
    s="a\tbb";CHECK(ps_code_preview_build(s,strlen(s),10,mono,NULL,&p));
    CHECK(p->count==1 && p->maximum==10 && p->tab==8);ps_code_preview_destroy(p);
    s="x\r\n\r\ny\n";CHECK(ps_code_preview_build(s,strlen(s),10,mono,NULL,&p));
    CHECK(p->count==3 && line(p,0,"x") && line(p,1,"") && line(p,2,"y"));ps_code_preview_destroy(p);
    s="a\xc2\xa0" "b c";CHECK(ps_code_preview_build(s,strlen(s),2,mono,NULL,&p));
    CHECK(p->count==2 && line(p,0,"a\xc2\xa0" "b ") && p->lines[0].width==3);ps_code_preview_destroy(p);
    s="ab\xc2\xad" "cd";CHECK(ps_code_preview_build(s,strlen(s),3,mono,NULL,&p));
    CHECK(p->count==2 && line(p,0,"ab\xc2\xad") && p->lines[0].hyphen && p->lines[0].width==3 && line(p,1,"cd"));ps_code_preview_destroy(p);
    s="ab\xe2\x80\x8b" "cd";CHECK(ps_code_preview_build(s,strlen(s),2,mono,NULL,&p));
    CHECK(p->count==2 && line(p,0,"ab\xe2\x80\x8b") && !p->lines[0].hyphen);ps_code_preview_destroy(p);
    s="a        ";CHECK(ps_code_preview_build(s,strlen(s),2,mono,NULL,&p));
    CHECK(p->count==1 && p->maximum==1 && line(p,0,s));ps_code_preview_destroy(p);
    char long_token[6001];memset(long_token,'x',6000);long_token[6000]=0;
    CHECK(ps_code_preview_build(long_token,6000,30,mono,NULL,&p));CHECK(p->count==1 && p->maximum==6000 && p->lines[0].end==6000);
    ps_code_preview *prior=p;
    CHECK(!ps_code_preview_build("\xf0\x80\x80\x80",4,10,mono,NULL,&p) && p==prior);
    CHECK(!ps_code_preview_build("a\0b",3,10,mono,NULL,&p) && p==prior);
    CHECK(!ps_code_preview_build("x",1,10,invalid,NULL,&p) && p==prior);
    CHECK(!ps_code_preview_build("x",1,NAN,mono,NULL,&p) && p==prior);ps_code_preview_destroy(p);
    CHECK(ps_code_preview_build("",0,1,mono,NULL,&p));CHECK(p->count==1 && p->maximum==0);ps_code_preview_destroy(p);
    s="analysis.phys";CHECK(ps_code_preview_build_filename(s,strlen(s),10,mono,NULL,&p));
    CHECK(p->count==2 && line(p,0,"analysi") && line(p,1,"s.phys") && p->maximum<=10);ps_code_preview_destroy(p);
    s="completion-note.phys";CHECK(ps_code_preview_build_filename(s,strlen(s),17,mono,NULL,&p));
    CHECK(p->count==2 && line(p,0,"completion-") && line(p,1,"note.phys"));ps_code_preview_destroy(p);
    const char *names[]={"completion-note.phys","very_long_header.h","archive.tar.gz","e\xcc\x81.phys",".gitignore","ending.","x.an_extension_wider_than_the_panel","my project.phys"};
    for(size_t k=0;k<sizeof names/sizeof *names;k++)for(unsigned width=6;width<=24;width++){
        s=names[k];size_t n=strlen(s),next=0;CHECK(ps_code_preview_build_filename(s,n,(float)width,mono,NULL,&p));
        CHECK(p->source==s && p->length==n && p->maximum<=width);
        for(size_t i=0;i<p->count;i++){
            CHECK(p->lines[i].begin==next && p->lines[i].end>next && !p->lines[i].hyphen);
            next=p->lines[i].end;CHECK(next==n || ((unsigned char)s[next]&0xc0)!=0x80);
            /* The decomposed e/acute is one grapheme, not a legal row cut. */
            if(k==3)CHECK(next!=1);
        }
        CHECK(next==n);
        const char *dot=strrchr(s,'.');
        if(dot && dot!=s && dot[1] && strlen(dot)<=width){
            bool intact=false;
            for(size_t i=0;i<p->count;i++)if(p->lines[i].begin<=(size_t)(dot-s) && p->lines[i].end==n)intact=true;
            CHECK(intact);
        }
        ps_code_preview_destroy(p);
    }
    CHECK(ps_code_preview_build_filename("analysis.phys",13,30,mono,NULL,&p));
    prior=p;CHECK(!ps_code_preview_build_filename("bad\0.phys",9,10,mono,NULL,&p) && p==prior);ps_code_preview_destroy(p);
    puts("Filename wrap: fitting suffixes, dotfiles, oversized extensions, combining graphemes and exact source spans passed");
    puts("Code preview: Unicode opportunities, intact long tokens/UTF8, whitespace, tabs, forced breaks, soft hyphen, ZWSP, byte spans and failure preservation passed");return 0;
}
