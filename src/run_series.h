#ifndef PS_RUN_SERIES_H
#define PS_RUN_SERIES_H
#include "physim/core.h"
#include "number_parse.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
typedef struct {unsigned kind,index,count;uint64_t base_seed;char parameter[48];double start,end;} ps_run_series;
static inline bool ps_series_uint(const char *text,uint64_t maximum,uint64_t *out) {
    if(!text || !*text)return false;uint64_t value=0;
    for(const char *p=text;*p;p++){if(*p<'0' || *p>'9')return false;unsigned digit=(unsigned)(*p-'0');if(digit>maximum || value>(maximum-digit)/10)return false;value=value*10+digit;}
    *out=value;return true;
}
static inline bool ps_series_parse(const char *text,uint64_t seed,ps_run_series *out) {
    if(!text || !out)return false;size_t length=0;while(length<512 && text[length])length++;if(length==512)return false;
    char copy[512];memcpy(copy,text,length+1);char *fields[7];unsigned count=0;char *p=copy;
    for(;;){if(count==7)return false;fields[count++]=p;char *comma=strchr(p,',');if(!comma)break;*comma=0;p=comma+1;}
    ps_run_series value={0};
    if(count==4 && !strcmp(fields[0],"seeded"))value.kind=1;
    else if(count==7 && !strcmp(fields[0],"linear"))value.kind=2;
    else return false;
    uint64_t index,runs;
    if(!ps_series_uint(fields[1],UINT64_MAX,&value.base_seed) || !ps_series_uint(fields[2],1000,&index) || !ps_series_uint(fields[3],1000,&runs) || !index || !runs || index>runs || value.base_seed>UINT64_MAX-(runs-1) || seed!=value.base_seed+index-1)return false;
    value.index=(unsigned)index;value.count=(unsigned)runs;
    if(value.kind==2) {
        const char *name=fields[4];size_t n=strlen(name);
        if(n==0 || n>=sizeof value.parameter || !((name[0]>='A' && name[0]<='Z') || (name[0]>='a' && name[0]<='z') || name[0]=='_'))return false;
        for(size_t i=1;i<n;i++)if(!((name[i]>='A' && name[i]<='Z') || (name[i]>='a' && name[i]<='z') || (name[i]>='0' && name[i]<='9') || name[i]=='_' || name[i]=='.' || name[i]=='-'))return false;
        if(runs<2 || !ps_parse_finite_number(fields[5],NULL,&value.start) || !ps_parse_finite_number(fields[6],NULL,&value.end) || value.start==value.end)return false;
        memcpy(value.parameter,name,n+1);
    }
    *out=value;return true;
}
static inline double ps_series_value(const ps_run_series *series) {
    if(series->index==1)return series->start;if(series->index==series->count)return series->end;
    double fraction=(double)(series->index-1)/(series->count-1);return (1-fraction)*series->start+fraction*series->end;
}
static inline int ps_series_metadata(const ps_run_series *series,char *out,size_t capacity) {
    if(!series->kind)return snprintf(out,capacity,"series.kind=single\n");
    int n=snprintf(out,capacity,"series.kind=%s\nseries.base_seed=%llu\nseries.index=%u\nseries.count=%u\nseries.seed_policy=base+index-1\n",series->kind==2?"linear":"seeded",(unsigned long long)series->base_seed,series->index,series->count);
    if(n<0 || (size_t)n>=capacity)return -1;
    if(series->kind==2){int added=snprintf(out+n,capacity-(size_t)n,"series.parameter=%s\nseries.start=%.17g\nseries.end=%.17g\nseries.value=%.17g\n",series->parameter,series->start,series->end,ps_series_value(series));if(added<0 || (size_t)added>=capacity-(size_t)n)return -1;n+=added;}
    return n;
}
#endif
