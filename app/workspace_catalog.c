#include "workspace_catalog.h"
#include "physim/data.h"
#include "text_validation.h"
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_timer.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_BYTES (20u + PS_WORKSPACE_MAX * (PS_WORKSPACE_NAME_BYTES + 4u + PS_WORKSPACE_WIRE_LIMIT))
static bool name_valid(const char *s) {
    if (!s || !*s || !ps_text_valid(s, PS_WORKSPACE_NAME_BYTES, false)) return false;
    return s[0] != ' ' && s[strlen(s)-1] != ' ';
}
bool ps_workspace_catalog_valid(const ps_workspace_catalog *c) {
    if (!c || c->count > PS_WORKSPACE_MAX) return false;
    for (unsigned i=0; i<c->count; i++) {
        if (!name_valid(c->entries[i].name) || !c->entries[i].state.root[0] || !ps_workspace_state_valid(&c->entries[i].state)) return false;
        for (unsigned j=0; j<i; j++) if (!strcmp(c->entries[i].name,c->entries[j].name)) return false;
    }
    return true;
}
ps_result ps_workspace_catalog_put(ps_workspace_catalog *c, const char *name, const ps_workspace_state *s) {
    if (!ps_workspace_catalog_valid(c) || !name_valid(name) || !ps_workspace_state_valid(s) || !s->root[0]) return PS_INVALID;
    unsigned index=c->count;
    for (unsigned i=0; i<c->count; i++) if (!strcmp(name,c->entries[i].name)) { index=i; break; }
    if (index==PS_WORKSPACE_MAX) return PS_LIMIT;
    char owned_name[PS_WORKSPACE_NAME_BYTES]={0};memcpy(owned_name,name,strlen(name));
    /* memmove also permits a state borrowed from this catalog. */
    memmove(&c->entries[index].state,s,sizeof *s);
    memcpy(c->entries[index].name,owned_name,sizeof owned_name);
    if (index==c->count) c->count++;
    return PS_OK;
}
ps_result ps_workspace_catalog_remove(ps_workspace_catalog *c, uint32_t index) {
    if (!ps_workspace_catalog_valid(c) || index>=c->count) return PS_INVALID;
    memmove(c->entries+index,c->entries+index+1,(c->count-index-1)*sizeof c->entries[0]);
    memset(c->entries+--c->count,0,sizeof c->entries[0]);return PS_OK;
}
static void put32(unsigned char *p, uint32_t n) { for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(n>>(8*i)); }
static uint32_t get32(const unsigned char *p) { return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24; }
ps_result ps_workspace_catalog_read(const char *path, ps_workspace_catalog *out) {
    if (!path || !*path || !out) return PS_INVALID;
    FILE *f=fopen(path,"rb");if(!f)return errno==ENOENT?PS_EOF:PS_IO;
    unsigned char *bytes=malloc(MAX_BYTES);ps_workspace_catalog *c=calloc(1,sizeof *c);
    if(!bytes || !c){free(bytes);free(c);fclose(f);return PS_MEMORY;}
    size_t n=fread(bytes,1,MAX_BYTES,f);bool ok=fgetc(f)==EOF && !ferror(f);if(fclose(f))ok=false;
    ps_result result=PS_CORRUPT;
    if(!ok || n<20 || memcmp(bytes,"PSWSET",6))goto done;
    if(memcmp(bytes+6,"01",2)){result=PS_VERSION;goto done;}
    if(get32(bytes+8)!=n || get32(bytes+n-4)!=ps_crc32(bytes,n-4))goto done;
    c->count=get32(bytes+12);if(c->count>PS_WORKSPACE_MAX)goto done;
    size_t at=16;
    for(unsigned i=0;i<c->count;i++) {
        if(n-4-at<PS_WORKSPACE_NAME_BYTES+4)goto done;
        ps_named_workspace_entry *e=&c->entries[i];memcpy(e->name,bytes+at,PS_WORKSPACE_NAME_BYTES);at+=PS_WORKSPACE_NAME_BYTES;
        uint32_t size=get32(bytes+at);at+=4;
        if(size>n-4-at)goto done;
        ps_result r=ps_workspace_state_decode(bytes+at,size,&e->state);
        if(r==PS_MEMORY){result=r;goto done;}
        if(r!=PS_OK)goto done;
        at+=size;
    }
    if(at!=n-4 || !ps_workspace_catalog_valid(c))goto done;
    *out=*c;result=PS_OK;
done:free(bytes);free(c);return result;
}
ps_result ps_workspace_catalog_write(const char *path, const ps_workspace_catalog *c) {
    if(!path || !*path || !ps_workspace_catalog_valid(c))return PS_INVALID;
    unsigned char *bytes=calloc(MAX_BYTES,1);if(!bytes)return PS_MEMORY;
    memcpy(bytes,"PSWSET01",8);put32(bytes+12,c->count);size_t at=16;
    ps_result result=PS_IO;
    for(unsigned i=0;i<c->count;i++) {
        unsigned char *state=NULL;size_t size=0;
        ps_result r=ps_workspace_state_encode(&c->entries[i].state,&state,&size);
        if(r!=PS_OK){result=r;goto done;}
        memcpy(bytes+at,c->entries[i].name,strlen(c->entries[i].name));at+=PS_WORKSPACE_NAME_BYTES;
        put32(bytes+at,(uint32_t)size);at+=4;memcpy(bytes+at,state,size);at+=size;free(state);
    }
    put32(bytes+8,(uint32_t)(at+4));put32(bytes+at,ps_crc32(bytes,at));at+=4;
    char temporary[PS_WORKSPACE_PATH];FILE *f=NULL;
    for(unsigned i=0;i<16 && !f;i++) {
        int n=snprintf(temporary,sizeof temporary,"%s.tmp-%llu-%u",path,(unsigned long long)SDL_GetTicksNS(),i);
        if(n<0 || (size_t)n>=sizeof temporary){result=PS_LIMIT;goto done;}
        f=fopen(temporary,"wbx");if(!f && errno!=EEXIST)goto done;
    }
    if(!f)goto done;
    bool ok=fwrite(bytes,1,at,f)==at;if(fclose(f))ok=false;
    if(ok)ok=SDL_RenamePath(temporary,path);
    if(!ok)SDL_RemovePath(temporary);
    result=ok?PS_OK:PS_IO;
done:free(bytes);return result;
}
