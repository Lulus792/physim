#include "model_values.h"
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void space(const char **p,const char *end){while(*p<end && (**p==' ' || **p=='\t' || **p=='\r'))++*p;}
static bool token(const char **p,const char *end,char *out,size_t capacity) {
    space(p,end);const char *start=*p;while(*p<end && !isspace((unsigned char)**p))++*p;
    size_t n=(size_t)(*p-start);if(!n || n>=capacity)return false;memcpy(out,start,n);out[n]=0;return true;
}
static bool identifier(const char *text) {
    if(!isalpha((unsigned char)*text) && *text!='_')return false;
    for(text++;*text;text++)if(!isalnum((unsigned char)*text) && *text!='_')return false;
    return true;
}
static bool number(const char *text,bool comma,double *value) {
    char normalized[64];size_t n=strlen(text);if(!n || n>=sizeof normalized)return false;
    for(size_t i=0;i<=n;i++) {
        char c=text[i];if(c==',' && comma)c='.';
        if(c && !(c>='0' && c<='9') && c!='.' && c!='+' && c!='-' && c!='e' && c!='E')return false;
        normalized[i]=c;
    }
    errno=0;char *end;double x=strtod(normalized,&end);
    if(errno || end==normalized || *end || !isfinite(x))return false;*value=x;return true;
}
bool ps_model_value_parse(const ps_model_value *binding,const char *text,double *si) {
    double value;if(!binding || !text || !si || !number(text,true,&value))return false;
    /* Preserve a source value exactly after a display-unit round trip. */
    double displayed=value;
    value=value==binding->value/binding->scale?binding->value:value*binding->scale;
    if(displayed!=0 && value==0)return false;
    if(!isfinite(value) || value<binding->minimum || value>binding->maximum)return false;
    *si=value;return true;
}
static bool exact(const char **p,const char *end,const char *text) {
    size_t n=strlen(text);if((size_t)(end-*p)<n || memcmp(*p,text,n))return false;*p+=n;return true;
}
static bool declaration(const char *source,const char *start,const char *end,bool physim,ps_model_value *v) {
    const char *p=start;space(&p,end);
    if(!exact(&p,end,physim?"let ":"static const double "))return false;
    space(&p,end);if(!exact(&p,end,v->id))return false;
    space(&p,end);
    if(physim){if(!exact(&p,end,":"))return false;space(&p,end);if(!exact(&p,end,"Float64"))return false;space(&p,end);}
    if(!exact(&p,end,"="))return false;space(&p,end);const char *literal=p;
    while(p<end && (isdigit((unsigned char)*p) || *p=='.' || *p=='+' || *p=='-' || *p=='e' || *p=='E'))p++;
    size_t n=(size_t)(p-literal);char text[64];if(!n || n>=sizeof text)return false;
    memcpy(text,literal,n);text[n]=0;
    if(!number(text,false,&v->value) || v->value<v->minimum || v->value>v->maximum ||
       !isfinite(v->value/v->scale) || (v->value!=0 && v->value/v->scale==0))return false;
    v->offset=(size_t)(literal-source);v->length=n;space(&p,end);
    if(!physim && !exact(&p,end,";"))return false;
    space(&p,end);return p==end;
}
bool ps_model_values_parse(const char *source,size_t length,bool physim,ps_model_values *out) {
    if(!out)return false;memset(out,0,sizeof *out);
    if(!source || length>PS_MODEL_SOURCE_MAX || memchr(source,0,length))goto invalid;
    unsigned depth=0,conditional=0;bool block=false,quote=false,escape=false,triple=false,directive=false;char quote_char=0;
    for(size_t at=0;at<length;) {
        size_t end=at;while(end<length && source[end]!='\n')end++;
        const char *p=source+at,*limit=source+end;space(&p,limit);
        if(!physim && !block && !quote && (directive || (p<limit && *p=='#'))) {
            if(!directive) {
                p++;space(&p,limit);
                if((size_t)(limit-p)>=2 && !memcmp(p,"if",2))conditional++;
                else if((size_t)(limit-p)>=5 && !memcmp(p,"endif",5) && conditional)conditional--;
            }
            const char *tail=limit;while(tail>source+at && isspace((unsigned char)tail[-1]))tail--;
            directive=tail>source+at && tail[-1]=='\\';at=end<length?end+1:length;continue;
        }
        if(!block && !quote && !triple && !depth && !conditional && (!physim || p==source+at) && (size_t)(limit-p)>=22 && !memcmp(p,"// @physim-model-value",22)) {
            if(out->count==PS_MODEL_VALUES_MAX || !exact(&p,limit,"// @physim-model-value-v1 "))goto invalid;
            ps_model_value v={0};char scale[64],lo[64],hi[64];
            if(!token(&p,limit,v.id,sizeof v.id) || !identifier(v.id) ||
               !token(&p,limit,v.unit,sizeof v.unit) || !token(&p,limit,scale,sizeof scale) ||
               !token(&p,limit,lo,sizeof lo) || !token(&p,limit,hi,sizeof hi) ||
               !number(scale,false,&v.scale) || v.scale<=0 || !number(lo,false,&v.minimum) ||
               !number(hi,false,&v.maximum) || v.minimum>v.maximum)goto invalid;
            space(&p,limit);size_t label=(size_t)(limit-p);while(label && isspace((unsigned char)p[label-1]))label--;
            if(!label || label>=sizeof v.label)goto invalid;memcpy(v.label,p,label);v.label[label]=0;
            for(unsigned i=0;i<out->count;i++)if(!strcmp(out->entries[i].id,v.id))goto invalid;
            size_t next=end<length?end+1:length,next_end=next;
            while(next_end<length && source[next_end]!='\n')next_end++;
            if(!declaration(source,source+next,source+next_end,physim,&v))goto invalid;
            out->entries[out->count++]=v;at=next_end<length?next_end+1:length;continue;
        }
        /* Track comments/strings/braces so annotations in a function or string
         * can never grant write access to a source token. */
        for(size_t i=at;i<end;i++) {
            char c=source[i],next=i+1<end?source[i+1]:0;
            if(triple){if(c=='\\' && next)i++;else if(c=='"' && next=='"' && i+2<end && source[i+2]=='"'){triple=false;i+=2;}continue;}
            if(block){if(c=='*' && next=='/'){block=false;i++;}continue;}
            if(quote){if(escape)escape=false;else if(c=='\\')escape=true;else if(c==quote_char)quote=false;continue;}
            if(c=='/' && next=='/')break;
            if(c=='/' && next=='*'){block=true;i++;continue;}
            if(physim && c=='"' && next=='"' && i+2<end && source[i+2]=='"'){triple=true;i+=2;continue;}
            if(c=='"' || c=='\''){quote=true;quote_char=c;continue;}
            if(c=='{')depth++;else if(c=='}' && depth)depth--;
        }
        at=end<length?end+1:length;
    }
    return true;
invalid:
    out->count=0;snprintf(out->error,sizeof out->error,"Modellwert-Bindung ungültig. v1 und direkte numerische Deklaration prüfen.");return false;
}
char *ps_model_values_rewrite(const char *source,size_t length,const ps_model_values *bindings,
                              const char values[PS_MODEL_VALUES_MAX][64],size_t *out_length) {
    if(!source || !bindings || !values || !out_length || length>PS_MODEL_SOURCE_MAX || bindings->count>PS_MODEL_VALUES_MAX)return NULL;
    char literals[PS_MODEL_VALUES_MAX][64];size_t size=length,previous=0;
    for(unsigned i=0;i<bindings->count;i++) {
        const ps_model_value *v=&bindings->entries[i];double si;
        if(v->offset<previous || v->offset>length || v->length>length-v->offset || !ps_model_value_parse(v,values[i],&si))return NULL;
        int n;
        if(si==v->value){if(v->length>=sizeof literals[i])return NULL;memcpy(literals[i],source+v->offset,v->length);literals[i][v->length]=0;n=(int)v->length;}
        else n=snprintf(literals[i],sizeof literals[i],"%.17g",si);
        if(n<=0 || n>=(int)sizeof literals[i])return NULL;
        size=size-v->length+(size_t)n;previous=v->offset+v->length;
    }
    if(size>PS_MODEL_SOURCE_MAX)return NULL;char *result=malloc(size+1);if(!result)return NULL;
    previous=0;size_t used=0;
    for(unsigned i=0;i<bindings->count;i++) {
        const ps_model_value *v=&bindings->entries[i];size_t n=v->offset-previous;
        memcpy(result+used,source+previous,n);used+=n;n=strlen(literals[i]);memcpy(result+used,literals[i],n);used+=n;previous=v->offset+v->length;
    }
    memcpy(result+used,source+previous,length-previous);used+=length-previous;result[used]=0;*out_length=used;return result;
}
