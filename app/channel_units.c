#include "channel_units.h"
#include "physim/data.h"
#include "physim/units.h"
#include "text_validation.h"
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_timer.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define ENTRY_BYTES 127u
#define FILE_BYTES (20u+PS_CHANNEL_UNIT_MAX*ENTRY_BYTES)
static bool text_valid(const char *s,size_t capacity) {
    return s && *s && ps_text_valid(s,capacity,false) && s[0]!=' ' && s[strlen(s)-1]!=' ';
}
static bool channel_valid(const ps_channel *c) {
    return c && ps_text_valid(c->name,sizeof c->name,false) && ps_text_valid(c->unit,sizeof c->unit,false);
}
bool ps_display_unit_valid(const ps_display_unit *u) {
    return u && isfinite(u->scale) && u->scale>0 && text_valid(u->symbol,sizeof u->symbol);
}
bool ps_channel_units_valid(const ps_channel_units *c) {
    if(!c || c->count>PS_CHANNEL_UNIT_MAX)return false;
    for(unsigned i=0;i<c->count;i++) {
        if(!ps_text_valid(c->entries[i].name,sizeof c->entries[i].name,false) || !ps_display_unit_valid(&c->entries[i].unit))return false;
        for(unsigned j=0;j<i;j++)if(!strcmp(c->entries[i].name,c->entries[j].name) &&
            !memcmp(c->entries[i].unit.dimension,c->entries[j].unit.dimension,7))return false;
    }
    return true;
}
static unsigned find(const ps_channel_units *c,const ps_channel *ch) {
    for(unsigned i=0;i<c->count;i++)if(!strcmp(c->entries[i].name,ch->name) &&
        !memcmp(c->entries[i].unit.dimension,ch->dimension,7))return i;
    return c->count;
}
ps_result ps_channel_units_get(const ps_channel_units *c,const ps_channel *ch,ps_display_unit *out) {
    if(!ps_channel_units_valid(c) || !channel_valid(ch) || !out)return PS_INVALID;
    unsigned at=find(c,ch);ps_display_unit unit={0};
    if(at<c->count)unit=c->entries[at].unit;
    else {
        memcpy(unit.dimension,ch->dimension,7);unit.scale=1;
        if(*ch->unit)snprintf(unit.symbol,sizeof unit.symbol,"%s",ch->unit);
        else {
            ps_unit si={{0},1,NULL};memcpy(si.dimension,ch->dimension,7);
            ps_result r=ps_unit_format_dimension(si,unit.symbol,sizeof unit.symbol);if(r!=PS_OK)return r;
        }
    }
    *out=unit;return PS_OK;
}
ps_result ps_channel_units_put(ps_channel_units *c,const ps_channel *ch,const ps_display_unit *u) {
    if(!ps_channel_units_valid(c) || !channel_valid(ch) || !ps_display_unit_valid(u) || memcmp(ch->dimension,u->dimension,7))return PS_INVALID;
    ps_channel_unit_entry entry={0};snprintf(entry.name,sizeof entry.name,"%s",ch->name);entry.unit=*u;
    unsigned at=find(c,ch);if(at==PS_CHANNEL_UNIT_MAX)return PS_LIMIT;
    c->entries[at]=entry;if(at==c->count)c->count++;return PS_OK;
}
ps_result ps_channel_units_remove(ps_channel_units *c,const ps_channel *ch) {
    if(!ps_channel_units_valid(c) || !channel_valid(ch))return PS_INVALID;
    unsigned at=find(c,ch);if(at==c->count)return PS_OK;
    memmove(c->entries+at,c->entries+at+1,(c->count-at-1)*sizeof c->entries[0]);
    memset(c->entries+--c->count,0,sizeof c->entries[0]);return PS_OK;
}
ps_result ps_display_unit_value(const ps_display_unit *u,double si,double *out) {
    if(!ps_display_unit_valid(u) || !isfinite(si) || !out)return PS_INVALID;
    double value=si/u->scale;
    if(!isfinite(value) || (si!=0 && value==0))return PS_NUMERIC;
    *out=value;return PS_OK;
}
static void put32(unsigned char *p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(v>>(8*i));}
static uint32_t get32(const unsigned char *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static void put64(unsigned char *p,double value){uint64_t v;memcpy(&v,&value,8);for(unsigned i=0;i<8;i++)p[i]=(unsigned char)(v>>(8*i));}
static double get64(const unsigned char *p){uint64_t v=0;for(unsigned i=0;i<8;i++)v|=(uint64_t)p[i]<<(8*i);double value;memcpy(&value,&v,8);return value;}
ps_result ps_channel_units_read(const char *path,ps_channel_units *out) {
    if(!path || !*path || !out)return PS_INVALID;
    FILE *f=fopen(path,"rb");if(!f)return errno==ENOENT?PS_EOF:PS_IO;
    unsigned char bytes[FILE_BYTES];size_t n=fread(bytes,1,sizeof bytes,f);
    bool ok=n>=20 && fgetc(f)==EOF && !ferror(f);if(fclose(f))ok=false;
    if(!ok || memcmp(bytes,"PSCUNI",6))return PS_CORRUPT;
    if(memcmp(bytes+6,"01",2))return PS_VERSION;
    uint32_t count=get32(bytes+12);
    if(count>PS_CHANNEL_UNIT_MAX || get32(bytes+8)!=n || n!=20u+count*ENTRY_BYTES || get32(bytes+n-4)!=ps_crc32(bytes,n-4))return PS_CORRUPT;
    ps_channel_units c={0};c.count=count;
    for(unsigned i=0;i<count;i++) {
        const unsigned char *at=bytes+16+i*ENTRY_BYTES;
        memcpy(c.entries[i].name,at,48);memcpy(c.entries[i].unit.dimension,at+48,7);
        memcpy(c.entries[i].unit.symbol,at+55,64);c.entries[i].unit.scale=get64(at+119);
    }
    if(!ps_channel_units_valid(&c))return PS_CORRUPT;
    *out=c;return PS_OK;
}
ps_result ps_channel_units_write(const char *path,const ps_channel_units *c) {
    if(!path || !*path || !ps_channel_units_valid(c))return PS_INVALID;
    unsigned char bytes[FILE_BYTES]={0};memcpy(bytes,"PSCUNI01",8);
    size_t size=20u+c->count*ENTRY_BYTES;put32(bytes+8,(uint32_t)size);put32(bytes+12,c->count);
    for(unsigned i=0;i<c->count;i++) {
        unsigned char *at=bytes+16+i*ENTRY_BYTES;
        memcpy(at,c->entries[i].name,strlen(c->entries[i].name));memcpy(at+48,c->entries[i].unit.dimension,7);
        memcpy(at+55,c->entries[i].unit.symbol,strlen(c->entries[i].unit.symbol));put64(at+119,c->entries[i].unit.scale);
    }
    put32(bytes+size-4,ps_crc32(bytes,size-4));char temporary[4096];FILE *f=NULL;
    for(unsigned i=0;i<16 && !f;i++) {
        int n=snprintf(temporary,sizeof temporary,"%s.tmp-%llu-%u",path,(unsigned long long)SDL_GetTicksNS(),i);
        if(n<0 || (size_t)n>=sizeof temporary)return PS_LIMIT;
        f=fopen(temporary,"wbx");if(!f && errno!=EEXIST)return PS_IO;
    }
    if(!f)return PS_IO;
    bool ok=fwrite(bytes,1,size,f)==size;if(fclose(f))ok=false;
    if(ok)ok=SDL_RenamePath(temporary,path);
    if(!ok)SDL_RemovePath(temporary);
    return ok?PS_OK:PS_IO;
}
