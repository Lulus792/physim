#include "text_wrap.h"
#include "../third_party/libunibreak-8.0/graphemebreak.h"
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

void ps_text_wrap_destroy(ps_text_wrap *p) {
    if(p){free(p->source);free(p->glyphs);free(p->rows);free(p);}
}
bool ps_text_wrap_build(const char *source,size_t n,float width,float height,
                       ps_code_measure measure,void *user,ps_text_wrap **output) {
    if(!source || !output || n>INT_MAX || !isfinite(height) || height<1 ||
       n>(SIZE_MAX/sizeof(struct nk_text_visual_glyph))-1)return false;
    ps_text_wrap *p=calloc(1,sizeof *p);ps_code_preview *lines=NULL;char *breaks=NULL;
    if(!p)return false;
    p->source=malloc(n+1);p->glyphs=calloc(n+1,sizeof *p->glyphs);
    breaks=malloc(n?n:1);
    if(!p->source || !p->glyphs || !breaks)goto fail;
    memcpy(p->source,source,n);p->source[n]=0;p->bytes=n;p->width=width;
    if(!ps_code_preview_build_anywhere(p->source,n,width,measure,user,&lines) ||
       lines->count>INT_MAX || lines->count>SIZE_MAX/sizeof *p->rows || !isfinite(height*(float)lines->count))goto fail;
    p->rows=calloc(lines->count,sizeof *p->rows);if(!p->rows)goto fail;
    if(n)set_graphemebreaks_utf8((const utf8_t *)source,n,NULL,breaks);
    int count=0;
    for(size_t at=0;at<n;) {
        uint32_t rune;size_t bytes=ps_code_preview_rune(source+at,n-at,&rune);
        if(!bytes)goto fail;
        p->glyphs[count].byte=(int)at;
        p->glyphs[count].boundary=(unsigned char)(!at || breaks[at-1]==GRAPHEMEBREAK_BREAK);
        p->glyphs[count].draw=(unsigned char)(rune!='\r' && rune!='\n' && rune!='\t' &&
            rune!=0x2028 && rune!=0x2029 && !ps_code_preview_invisible(rune));
        count++;at+=bytes;
    }
    p->glyphs[count].byte=(int)n;p->glyphs[count].boundary=1;
    int at=0;
    for(size_t i=0;i<lines->count;i++) {
        const ps_code_preview_line *line=&lines->lines[i];
        struct nk_text_visual_row *row=&p->rows[i];
        while(at<count && p->glyphs[at].byte<(int)line->begin)at++;
        row->first=at;
        while(at<count && p->glyphs[at].byte<(int)line->end)at++;
        row->end=at;
        size_t limit=i+1<lines->count?lines->lines[i+1].begin:n;
        while(at<count && p->glyphs[at].byte<(int)limit)at++;
        row->limit=at;row->width=line->width;row->hyphen=(unsigned char)line->hyphen;
        float x=0;
        for(int j=row->first;j<row->limit;j++) {
            struct nk_text_visual_glyph *g=&p->glyphs[j];g->x=x;
            if(j<row->end) {
                uint32_t rune;size_t bytes=ps_code_preview_rune(source+g->byte,n-(size_t)g->byte,&rune);
                float advance=g->draw?measure(user,source+g->byte,bytes):0;
                if(!isfinite(advance) || advance<0)goto fail;
                float next=ps_code_preview_advance(rune,x,lines->tab,advance);
                if(!isfinite(next))goto fail;
                g->advance=next-x;x=next;
            }
        }
        row->edge=x;row->hyphen_x=x;
        if(line->hyphen)row->hyphen_x=line->width-measure(user,"-",1);
        if(row->limit==count)p->glyphs[count].x=x;
    }
    p->visual=(struct nk_text_visual){p->glyphs,p->rows,count,(int)lines->count,height,width};
    free(breaks);ps_code_preview_destroy(lines);*output=p;return true;
fail:
    free(breaks);ps_code_preview_destroy(lines);ps_text_wrap_destroy(p);return false;
}
