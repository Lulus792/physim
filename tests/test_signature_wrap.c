#include "../app/code_preview.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Signature wrap line %d: %s\n",__LINE__,#x);return 1;}}while(0)
typedef struct {const char *source;size_t bytes,begin,end,calls,reject;} metrics;
static float measure(void *user,const char *text,size_t n){
    metrics *m=user;m->calls++;if(m->reject && m->calls>=m->reject)return NAN;
    uint32_t rune;if(!ps_code_preview_rune(text,n,&rune))return NAN;
    uintptr_t offset=(uintptr_t)text-(uintptr_t)m->source;
    return rune==0x308?0:offset<m->bytes && offset>=m->begin && offset<m->end?2:1;
}
static size_t row_at(const ps_code_preview *p,size_t byte){
    for(size_t i=0;i<p->count;i++)if(byte>=p->lines[i].begin && byte<p->lines[i].end)return i;
    return SIZE_MAX;
}
static int coverage(const ps_code_preview *p,const char *source,size_t bytes){
    CHECK(p->source==source && p->length==bytes && p->count>0);
    size_t previous=0;float maximum=0;
    for(size_t i=0;i<p->count;i++){
        ps_code_preview_line r=p->lines[i];CHECK(r.begin>=previous && r.begin<=r.end && r.end<=bytes && !r.hyphen);
        for(size_t at=previous;at<r.begin;){uint32_t c;size_t n=ps_code_preview_rune(source+at,r.begin-at,&c);CHECK(n && (c=='\r' || c=='\n' || c==0x2028 || c==0x2029));at+=n;}
        if(r.begin<bytes)CHECK(((unsigned char)source[r.begin]&0xc0)!=0x80);
        if(r.end<bytes)CHECK(((unsigned char)source[r.end]&0xc0)!=0x80);
        maximum=fmaxf(maximum,r.width);previous=r.end;
    }
    for(size_t at=previous;at<bytes;){uint32_t c;size_t n=ps_code_preview_rune(source+at,bytes-at,&c);CHECK(n && (c=='\r' || c=='\n' || c==0x2028 || c==0x2029));at+=n;}
    CHECK(maximum==p->maximum);return 0;
}
int main(void){
    const char *operators[]={"->","...","=>","::","&&","||","<=",">=","==","!=","++","--","<<",">>",">>=","<<=","??","?.","**"};
    size_t cases=0;bool old_split=false;
    for(size_t op=0;op<sizeof operators/sizeof *operators;op++)for(unsigned width=1;width<=40;width++){
        char text[128];snprintf(text,sizeof text,"aaaaaa%sReturnType",operators[op]);metrics m={.source=text,.bytes=strlen(text),.begin=SIZE_MAX};ps_code_preview *p=NULL;
        CHECK(ps_code_preview_build_signature(text,m.bytes,(float)width,measure,&m,&p));CHECK(!coverage(p,text,m.bytes));
        size_t begin=6,row=row_at(p,begin);CHECK(row!=SIZE_MAX);
        for(size_t i=1;i<strlen(operators[op]);i++)CHECK(row_at(p,begin+i)==row);
        CHECK(p->maximum<=(float)width || (float)strlen(operators[op])>(float)width);
        CHECK(m.calls<16*m.bytes+10);ps_code_preview_destroy(p);
        if(op==0){m.calls=0;CHECK(ps_code_preview_build_anywhere(text,m.bytes,(float)width,measure,&m,&p));old_split|=width>=2 && row_at(p,6)!=row_at(p,7);ps_code_preview_destroy(p);}
        cases++;
    }
    CHECK(old_split);
    const char *unicode="Gr\xc3\xb6\xc3\x9f" "e(a\xcc\x88: Float64) -> Vec3\r\nletzter\xe2\x80\xa8";
    for(unsigned width=1;width<=32;width++){
        metrics m={.source=unicode,.bytes=strlen(unicode),.begin=SIZE_MAX};ps_code_preview *p=NULL;
        CHECK(ps_code_preview_build_signature(unicode,m.bytes,(float)width,measure,&m,&p));CHECK(!coverage(p,unicode,m.bytes));
        const char *cluster=strstr(unicode,"a\xcc\x88");size_t at=(size_t)(cluster-unicode);CHECK(row_at(p,at)==row_at(p,at+1));
        CHECK(p->lines[p->count-1].begin==m.bytes);ps_code_preview_destroy(p);cases++;
    }
    char long_text[2048]="sixteen(";
    for(unsigned i=0;i<16;i++){char part[80];snprintf(part,sizeof part,"%sparameter_%02u: Float64",i?", ":"",i);strcat(long_text,part);}strcat(long_text,") -> Vec3");
    size_t active=(size_t)(strstr(long_text,"parameter_15")-long_text);
    for(unsigned width=8;width<=80;width++){
        metrics m={.source=long_text,.bytes=strlen(long_text),.begin=active,.end=active+strlen("parameter_15: Float64")};ps_code_preview *p=NULL;
        CHECK(ps_code_preview_build_signature(long_text,m.bytes,(float)width,measure,&m,&p));CHECK(!coverage(p,long_text,m.bytes));CHECK(p->maximum<=width);
        CHECK(row_at(p,active)!=SIZE_MAX && row_at(p,m.bytes-1)!=SIZE_MAX && (width>40 || p->count>5));
        CHECK(m.calls<16*m.bytes+10);ps_code_preview_destroy(p);cases++;
    }
    ps_code_preview sentinel={0},*p=&sentinel;metrics m={.source="abc -> T",.bytes=8,.begin=SIZE_MAX};
    CHECK(!ps_code_preview_build_signature("\xf0\x80\x80\x80",4,10,measure,&m,&p) && p==&sentinel);
    CHECK(!ps_code_preview_build_signature(m.source,m.bytes,NAN,measure,&m,&p) && p==&sentinel);
    CHECK(!ps_code_preview_build_signature(m.source,m.bytes,0,measure,&m,&p) && p==&sentinel);
    m.calls=0;m.reject=7;CHECK(!ps_code_preview_build_signature(m.source,m.bytes,4,measure,&m,&p) && p==&sentinel);
    printf("Signature wrap: %zu operator/Unicode/16-parameter variable-font widths, source coverage, bounded callback work; legacy arrow split rejected; invalid requests preserve output\n",cases);return 0;
}
