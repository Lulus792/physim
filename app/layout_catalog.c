#include "layout_catalog.h"
#include "physim/data.h"
#include "text_validation.h"
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_timer.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#define ENTRY_BYTES (PS_LAYOUT_NAME_BYTES+PS_DOCK_WIRE_BYTES+16u)
#define FILE_BYTES (20u+PS_LAYOUT_MAX*ENTRY_BYTES)
static bool name_valid(const char *name) {
    if(!name || !*name || !ps_text_valid(name,PS_LAYOUT_NAME_BYTES,false))return false;
    size_t n=strlen(name);return name[0]!=' ' && name[n-1]!=' ';
}
bool ps_layout_state_valid(const ps_layout_state *s) {
    return s && ps_dock_valid(&s->dock) && s->sidebar_width>=208 && s->sidebar_width<=360 &&
        s->inspector_width>=208 && s->inspector_width<=480 && s->log_height>=150 &&
        s->log_height<=340 && s->show_log<=1;
}
bool ps_layout_catalog_valid(const ps_layout_catalog *c) {
    if(!c || c->count>PS_LAYOUT_MAX)return false;
    for(unsigned i=0;i<c->count;i++) {
        if(!name_valid(c->entries[i].name) || !ps_layout_state_valid(&c->entries[i].state))return false;
        for(unsigned j=0;j<i;j++)if(!strcmp(c->entries[i].name,c->entries[j].name))return false;
    }
    return true;
}
ps_result ps_layout_catalog_put(ps_layout_catalog *c,const char *name,const ps_layout_state *state) {
    if(!ps_layout_catalog_valid(c) || !name_valid(name) || !ps_layout_state_valid(state))return PS_INVALID;
    ps_layout_entry entry={0};memcpy(entry.name,name,strlen(name)+1);entry.state=*state;
    unsigned index=c->count;
    for(unsigned i=0;i<c->count;i++)if(!strcmp(c->entries[i].name,name)){index=i;break;}
    if(index==PS_LAYOUT_MAX)return PS_LIMIT;
    c->entries[index]=entry;if(index==c->count)c->count++;
    return PS_OK;
}
ps_result ps_layout_catalog_remove(ps_layout_catalog *c,uint32_t index) {
    if(!ps_layout_catalog_valid(c) || index>=c->count)return PS_INVALID;
    memmove(c->entries+index,c->entries+index+1,(c->count-index-1)*sizeof c->entries[0]);
    memset(c->entries+--c->count,0,sizeof c->entries[0]);return PS_OK;
}
static uint32_t get32(const unsigned char *p) {
    return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
}
static void put32(unsigned char *p,uint32_t value) {
    for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(value>>(8*i));
}
ps_result ps_layout_catalog_read(const char *path,ps_layout_catalog *out) {
    if(!path || !*path || !out)return PS_INVALID;
    FILE *f=fopen(path,"rb");if(!f)return errno==ENOENT?PS_EOF:PS_IO;
    unsigned char bytes[FILE_BYTES];size_t n=fread(bytes,1,sizeof bytes,f);
    bool ok=n>=20 && fgetc(f)==EOF && !ferror(f);if(fclose(f))ok=false;
    if(!ok || memcmp(bytes,"PSLAYT",6))return PS_CORRUPT;
    if(memcmp(bytes+6,"01",2))return PS_VERSION;
    uint32_t count=get32(bytes+12);size_t payload=4u+(size_t)count*ENTRY_BYTES;
    if(count>PS_LAYOUT_MAX || get32(bytes+8)!=payload || n!=16+payload ||
       get32(bytes+12+payload)!=ps_crc32(bytes,12+payload))return PS_CORRUPT;
    ps_layout_catalog c={0};c.count=count;const unsigned char *at=bytes+16;
    for(unsigned i=0;i<count;i++,at+=ENTRY_BYTES) {
        ps_layout_entry *e=&c.entries[i];memcpy(e->name,at,PS_LAYOUT_NAME_BYTES);
        if(!memchr(e->name,0,sizeof e->name) || !ps_dock_decode(at+PS_LAYOUT_NAME_BYTES,PS_DOCK_WIRE_BYTES,&e->state.dock))return PS_CORRUPT;
        const unsigned char *sizes=at+PS_LAYOUT_NAME_BYTES+PS_DOCK_WIRE_BYTES;
        e->state.sidebar_width=get32(sizes);e->state.inspector_width=get32(sizes+4);
        e->state.log_height=get32(sizes+8);e->state.show_log=get32(sizes+12);
    }
    if(!ps_layout_catalog_valid(&c))return PS_CORRUPT;
    *out=c;return PS_OK;
}
ps_result ps_layout_catalog_write(const char *path,const ps_layout_catalog *c) {
    if(!path || !*path || !ps_layout_catalog_valid(c))return PS_INVALID;
    unsigned char bytes[FILE_BYTES]={0};memcpy(bytes,"PSLAYT01",8);
    size_t payload=4u+c->count*ENTRY_BYTES;put32(bytes+8,(uint32_t)payload);put32(bytes+12,c->count);
    unsigned char *at=bytes+16;
    for(unsigned i=0;i<c->count;i++,at+=ENTRY_BYTES) {
        const ps_layout_entry *e=&c->entries[i];memcpy(at,e->name,strlen(e->name));
        ps_dock_encode(&e->state.dock,at+PS_LAYOUT_NAME_BYTES,PS_DOCK_WIRE_BYTES);
        unsigned char *sizes=at+PS_LAYOUT_NAME_BYTES+PS_DOCK_WIRE_BYTES;
        put32(sizes,e->state.sidebar_width);put32(sizes+4,e->state.inspector_width);
        put32(sizes+8,e->state.log_height);put32(sizes+12,e->state.show_log);
    }
    put32(bytes+12+payload,ps_crc32(bytes,12+payload));
    char temporary[4096];FILE *f=NULL;
    for(unsigned i=0;i<16 && !f;i++) {
        int n=snprintf(temporary,sizeof temporary,"%s.tmp-%llu-%u",path,(unsigned long long)SDL_GetTicksNS(),i);
        if(n<0 || n>=(int)sizeof temporary)return PS_LIMIT;
        f=fopen(temporary,"wbx");if(!f && errno!=EEXIST)return PS_IO;
    }
    if(!f)return PS_IO;
    bool ok=fwrite(bytes,1,16+payload,f)==16+payload;if(fclose(f))ok=false;
    if(ok)ok=SDL_RenamePath(temporary,path);
    if(!ok)SDL_RemovePath(temporary);
    return ok?PS_OK:PS_IO;
}
