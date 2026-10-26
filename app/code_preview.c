#include "code_preview.h"
#include "../third_party/libunibreak-8.0/linebreak.h"
#include "../third_party/libunibreak-8.0/graphemebreak.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
size_t ps_code_preview_rune(const char *s,size_t n,uint32_t *out) {
    if(!s || !n || !out)return 0;
    const unsigned char *p=(const unsigned char *)s;uint32_t c=p[0],minimum=0;size_t bytes=1;
    if(c>=0xc2 && c<=0xdf){bytes=2;minimum=0x80;c&=31;}
    else if(c>=0xe0 && c<=0xef){bytes=3;minimum=0x800;c&=15;}
    else if(c>=0xf0 && c<=0xf4){bytes=4;minimum=0x10000;c&=7;}
    else if(c>=0x80)return 0;
    if(bytes>n)return 0;
    for(size_t i=1;i<bytes;i++){if((p[i]&0xc0)!=0x80)return 0;c=(c<<6)|(p[i]&63);}
    if(c<minimum || c>0x10ffff || (c>=0xd800 && c<=0xdfff))return 0;
    *out=c;return bytes;
}
bool ps_code_preview_invisible(uint32_t c) {return c==0xad || c==0x200b || c==0x200c || c==0x200d || c==0x2060 || c==0xfeff;}
float ps_code_preview_advance(uint32_t c,float x,float tab,float glyph) {
    if(c=='\t')return (floorf(x/tab)+1)*tab;
    return x+(ps_code_preview_invisible(c)?0:glyph);
}
void ps_code_preview_destroy(ps_code_preview *p) {if(p){free(p->lines);free(p);}}
static bool append(ps_code_preview *p,size_t begin,size_t end,float width,bool hyphen) {
    if(!isfinite(width) || width<0)return false;
    if(p->count==p->capacity) {
        size_t capacity=p->capacity?p->capacity*2:32;
        if(capacity<p->capacity || capacity>SIZE_MAX/sizeof *p->lines)return false;
        ps_code_preview_line *lines=realloc(p->lines,capacity*sizeof *lines);if(!lines)return false;
        p->lines=lines;p->capacity=capacity;
    }
    p->lines[p->count++]=(ps_code_preview_line){begin,end,width,hyphen};p->maximum=fmaxf(p->maximum,width);return true;
}
static bool build(const char *source,size_t n,float width,ps_code_measure measure,void *user,ps_code_preview **output,bool anywhere) {
    if(!source || !measure || !output || !isfinite(width) || width<1 || n==SIZE_MAX)return false;
    for(size_t at=0;at<n;) {
        uint32_t c;size_t bytes=ps_code_preview_rune(source+at,n-at,&c);
        if(!bytes || (c<32 && c!='\r' && c!='\n' && c!='\t') || (c>=0x7f && c<=0x9f))return false;
        at+=bytes;
    }
    float space=measure(user," ",1),dash=measure(user,"-",1);
    if(!isfinite(space) || space<=0 || !isfinite(dash) || dash<0 || !isfinite(space*8))return false;
    ps_code_preview *p=calloc(1,sizeof *p);char *breaks=malloc(n?n:1),*clusters=anywhere?malloc(n?n:1):NULL;
    if(!p || !breaks || (anywhere && !clusters)){free(p);free(breaks);free(clusters);return false;}
    p->source=source;p->length=n;p->width=width;p->tab=space*8;
    if(n)set_linebreaks_utf8((const utf8_t *)source,n,"de",breaks);
    if(n && anywhere)set_graphemebreaks_utf8((const utf8_t *)source,n,"de",clusters);
    size_t begin=0;
    while(begin<n) {
        size_t at=begin,allowed=SIZE_MAX,cluster=begin;float x=0,ink=0,allowed_width=0,cluster_ink=0;bool allowed_hyphen=false,finished=false;
        while(at<n) {
            uint32_t c;size_t bytes=ps_code_preview_rune(source+at,n-at,&c);
            if(anywhere && (at==begin || clusters[at-1]==GRAPHEMEBREAK_BREAK)){cluster=at;cluster_ink=ink;}
            if(c=='\r' || c=='\n' || c==0x2028 || c==0x2029) {
                if(!append(p,begin,at,ink,false))goto fail;
                at+=bytes;if(c=='\r' && at<n && source[at]=='\n')at++;
                begin=at;finished=true;break;
            }
            float glyph=ps_code_preview_invisible(c) || c=='\t'?0:measure(user,source+at,bytes);
            if(!isfinite(glyph) || glyph<0)goto fail;
            float next=ps_code_preview_advance(c,x,p->tab,glyph);if(!isfinite(next))goto fail;
            bool whitespace=c==' ' || c=='\t';
            if(!whitespace && next>width && allowed!=SIZE_MAX && allowed>begin && (!anywhere || allowed_width<=width)) {
                if(!append(p,begin,allowed,allowed_width,allowed_hyphen))goto fail;
                begin=allowed;finished=true;break;
            }
            if(!whitespace && next>width && anywhere && cluster>begin) {
                if(!append(p,begin,cluster,cluster_ink,false))goto fail;
                begin=cluster;finished=true;break;
            }
            x=next;if(!whitespace && !ps_code_preview_invisible(c))ink=x;
            at+=bytes;
            if(breaks[at-1]==LINEBREAK_ALLOWBREAK && (!anywhere || clusters[at-1]==GRAPHEMEBREAK_BREAK)) {
                allowed=at;allowed_hyphen=c==0xad;allowed_width=ink+(allowed_hyphen?dash:0);
                if(ink>width) {
                    if(!append(p,begin,at,allowed_width,allowed_hyphen))goto fail;
                    begin=at;finished=true;break;
                }
            }
        }
        if(!finished){if(!append(p,begin,n,ink,false))goto fail;begin=n;}
    }
    if(anywhere && p->count && p->lines[p->count-1].end<n && !append(p,n,n,0,false))goto fail;
    if(!p->count && !append(p,0,0,0,false))goto fail;
    free(breaks);free(clusters);*output=p;return true;
fail:free(breaks);free(clusters);ps_code_preview_destroy(p);return false;
}
bool ps_code_preview_build(const char *source,size_t n,float width,ps_code_measure measure,void *user,ps_code_preview **output) {
    return build(source,n,width,measure,user,output,false);
}
bool ps_code_preview_build_anywhere(const char *source,size_t n,float width,ps_code_measure measure,void *user,ps_code_preview **output) {
    return build(source,n,width,measure,user,output,true);
}
