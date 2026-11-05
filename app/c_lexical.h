#ifndef PHYSIM_C_LEXICAL_H
#define PHYSIM_C_LEXICAL_H
#include "physim/core.h"

/* Only C comment/literal suppression and a cheap scheduling hint. Not a scope,
 * declaration or call parser. Logical characters account for C line splicing
 * before recognizing comments; all returned positions remain original bytes. */
static inline int ps_c_lexical_next(const char *text,size_t cursor,size_t *at){
    while(*at<cursor){
        if(text[*at]=='\\' && *at+1<cursor && text[*at+1]=='\n'){*at+=2;continue;}
        if(text[*at]=='\\' && *at+2<cursor && text[*at+1]=='\r' && text[*at+2]=='\n'){*at+=3;continue;}
        return (unsigned char)text[(*at)++];
    }
    return -1;
}
static inline bool ps_c_lexical_cursor(const char *text,size_t bytes,size_t cursor,int *last_token){
    if((!text && bytes) || cursor>bytes)return false;
    size_t at=0;bool block=false,line=false;int quote=0,last=0,c;
    while((c=ps_c_lexical_next(text,cursor,&at))>=0){
        size_t after=at;int next=ps_c_lexical_next(text,cursor,&after);
        if(line){if(c=='\n')line=false;continue;}
        if(block){if(c=='*' && next=='/'){block=false;at=after;}continue;}
        if(quote){if(c=='\\' && next>=0)at=after;else if(c==quote)quote=0;continue;}
        if(c=='/' && next=='/'){line=true;at=after;continue;}
        if(c=='/' && next=='*'){block=true;at=after;continue;}
        if(c=='"' || c=='\''){quote=c;last=0;continue;}
        if(c!=' ' && c!='\t' && c!='\r' && c!='\n' && c!='\v' && c!='\f')last=c;
    }
    if(last_token)*last_token=last;
    return !block && !line && !quote;
}
#endif
