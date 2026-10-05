#include "workspace_catalog.h"
#include "physim/data.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Workspace catalog line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static bool write_bytes(const char *p,const void *b,size_t n){FILE *f=fopen(p,"wb");if(!f)return false;bool ok=fwrite(b,1,n,f)==n;return !fclose(f) && ok;}
static void put32(unsigned char *p,uint32_t n){for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(n>>(8*i));}
static void crc(unsigned char *b,size_t n){put32(b+n-4,ps_crc32(b,n-4));}
int main(int argc,char **argv) {
    CHECK(argc==2);char path[4096],bad[4096],dir[4096];
    snprintf(path,sizeof path,"%s/workspaces.bin",argv[1]);snprintf(bad,sizeof bad,"%s/corrupt.bin",argv[1]);snprintf(dir,sizeof dir,"%s/target",argv[1]);remove(path);
    ps_workspace_catalog *c=calloc(1,sizeof *c),*out=calloc(1,sizeof *out),*saved=calloc(1,sizeof *saved);
    ps_workspace_state *s=calloc(1,sizeof *s);CHECK(c && out && saved && s);
    CHECK(ps_workspace_catalog_valid(c) && ps_workspace_catalog_read(path,out)==PS_EOF && !memcmp(c,out,sizeof *c));
    CHECK(ps_workspace_catalog_write(path,c)==PS_OK && ps_workspace_catalog_read(path,out)==PS_OK && !memcmp(c,out,sizeof *c));
    CHECK(ps_workspace_catalog_put(c,"Empty",s)==PS_INVALID);
    CHECK(ps_workspace_absolute("Experiment ä",s->root)==PS_OK);
    CHECK(ps_workspace_absolute("Notes ä.txt",s->documents[0].path)==PS_OK);s->document_count=1;s->view=3;
    s->documents[0].editor=(ps_workspace_editor){20,2,19,32,450};s->experiment=(ps_workspace_editor){42,21,42,16,320};
    CHECK(ps_workspace_absolute("Missing path",s->additions[0])==PS_OK);s->count=1;
    CHECK(ps_workspace_catalog_put(c,"Versuch ä",s)==PS_OK);
    CHECK(ps_workspace_absolute("Andere Dateien",s->root)==PS_OK);
    CHECK(ps_workspace_catalog_put(c,"Dateien",s)==PS_OK);
    CHECK(ps_workspace_catalog_write(path,c)==PS_OK && ps_workspace_catalog_read(path,out)==PS_OK && !memcmp(c,out,sizeof *c));
    s->experiment.cursor=55;CHECK(ps_workspace_catalog_put(c,"Versuch ä",s)==PS_OK && c->count==2 && c->entries[0].state.experiment.cursor==55);
    CHECK(ps_workspace_catalog_put(c,c->entries[0].name,&c->entries[0].state)==PS_OK);
    *saved=*c;
    const char *invalid[]={""," leading","trailing ","control\n","\xc0\xaf","\xed\xa0\x80"};
    for(unsigned i=0;i<sizeof invalid/sizeof invalid[0];i++)CHECK(ps_workspace_catalog_put(c,invalid[i],s)==PS_INVALID && !memcmp(c,saved,sizeof *c));
    char name[65];memset(name,'a',64);name[64]=0;CHECK(ps_workspace_catalog_put(c,name,s)==PS_INVALID);name[63]=0;
    CHECK(ps_workspace_catalog_put(c,name,s)==PS_OK);
    for(unsigned i=c->count;i<PS_WORKSPACE_MAX;i++){snprintf(name,sizeof name,"Workspace %u",i);CHECK(ps_workspace_catalog_put(c,name,s)==PS_OK);}
    *saved=*c;CHECK(ps_workspace_catalog_put(c,"Ninth",s)==PS_LIMIT && !memcmp(c,saved,sizeof *c));
    CHECK(ps_workspace_catalog_write(path,c)==PS_OK && ps_workspace_catalog_read(path,out)==PS_OK && !memcmp(c,out,sizeof *c));
    memcpy(c->entries[1].name,c->entries[0].name,PS_WORKSPACE_NAME_BYTES);
    CHECK(!ps_workspace_catalog_valid(c) && ps_workspace_catalog_write(path,c)==PS_INVALID);
    CHECK(ps_workspace_catalog_read(path,out)==PS_OK && !memcmp(out,saved,sizeof *out));*c=*saved;
    CHECK(ps_workspace_catalog_remove(c,8)==PS_INVALID && !memcmp(c,saved,sizeof *c));
    CHECK(ps_workspace_catalog_remove(c,0)==PS_OK && c->count==7 && !strcmp(c->entries[0].name,"Dateien"));
    /* One entry keeps exhaustive mutations small while checking nested views. */
    c->count=1;memset(c->entries+1,0,7*sizeof c->entries[0]);
    CHECK(ps_workspace_catalog_write(path,c)==PS_OK && ps_workspace_catalog_read(path,out)==PS_OK && !memcmp(c,out,sizeof *c));*saved=*out;
    unsigned char bytes[16384],changed[16385];FILE *f=fopen(path,"rb");CHECK(f);size_t n=fread(bytes,1,sizeof bytes,f);CHECK(!fclose(f) && n<sizeof bytes);
    for(size_t i=0;i<n;i++) {
        CHECK(write_bytes(bad,bytes,i) && ps_workspace_catalog_read(bad,out)!=PS_OK && !memcmp(out,saved,sizeof *out));
        memcpy(changed,bytes,n);changed[i]^=0x40;
        CHECK(write_bytes(bad,changed,n) && ps_workspace_catalog_read(bad,out)!=PS_OK && !memcmp(out,saved,sizeof *out));
    }
    memcpy(changed,bytes,n);changed[n]=0;CHECK(write_bytes(bad,changed,n+1) && ps_workspace_catalog_read(bad,out)==PS_CORRUPT);
    memcpy(changed,bytes,n);changed[7]='2';crc(changed,n);CHECK(write_bytes(bad,changed,n) && ps_workspace_catalog_read(bad,out)==PS_VERSION);
    const size_t offsets[]={12,16,80,84+12};const uint32_t values[]={9,0,UINT32_MAX,33};
    for(unsigned i=0;i<4;i++){memcpy(changed,bytes,n);put32(changed+offsets[i],values[i]);crc(changed,n);CHECK(write_bytes(bad,changed,n) && ps_workspace_catalog_read(bad,out)==PS_CORRUPT && !memcmp(out,saved,sizeof *out));}
    /* An invalid nested state is rejected even with both CRCs repaired. */
    size_t nested=n-88;memcpy(changed,bytes,n);put32(changed+84+16,0);crc(changed+84,nested);crc(changed,n);
    CHECK(write_bytes(bad,changed,n) && ps_workspace_catalog_read(bad,out)==PS_CORRUPT);
    CHECK(SDL_CreateDirectory(dir) && ps_workspace_catalog_write(dir,c)==PS_IO);
    c->count=9;CHECK(ps_workspace_catalog_write(path,c)==PS_INVALID);*c=*saved;
    CHECK(ps_workspace_catalog_read(path,out)==PS_OK && !memcmp(out,saved,sizeof *out));
    /* Maximum path/document counts exercise the bounded encoder, not C stack copies. */
    s->count=PS_WORKSPACE_ADDITIONS;s->document_count=PS_WORKSPACE_DOCUMENTS;s->active_document=15;
    memset(s->additions,0,sizeof s->additions);memset(s->documents,0,sizeof s->documents);
    for(unsigned i=0;i<s->count;i++){snprintf(name,sizeof name,"folder-%u",i);CHECK(ps_workspace_absolute(name,s->additions[i])==PS_OK);}
    for(unsigned i=0;i<s->document_count;i++){snprintf(name,sizeof name,"document-%u",i);CHECK(ps_workspace_absolute(name,s->documents[i].path)==PS_OK);}
    unsigned char *encoded=NULL;size_t size=0;CHECK(ps_workspace_state_encode(s,&encoded,&size)==PS_OK && size<=PS_WORKSPACE_WIRE_LIMIT);
    CHECK(ps_workspace_state_decode(encoded,size,&out->entries[0].state)==PS_OK && !memcmp(s,&out->entries[0].state,sizeof *s));free(encoded);
    memset(s->root,'a',PS_WORKSPACE_PATH-1);s->root[0]='/';s->root[PS_WORKSPACE_PATH-1]=0;
#ifdef _WIN32
    s->root[0]='C';s->root[1]=':';s->root[2]='/';
#endif
    for(unsigned i=0;i<s->count;i++)memcpy(s->additions[i],s->root,PS_WORKSPACE_PATH);
    for(unsigned i=0;i<s->document_count;i++)memcpy(s->documents[i].path,s->root,PS_WORKSPACE_PATH);
    memset(c,0,sizeof *c);
    for(unsigned i=0;i<PS_WORKSPACE_MAX;i++){snprintf(name,sizeof name,"Long paths %u",i);CHECK(ps_workspace_catalog_put(c,name,s)==PS_OK);}
    CHECK(ps_workspace_catalog_write(path,c)==PS_OK && ps_workspace_catalog_read(path,out)==PS_OK && !memcmp(c,out,sizeof *c));
    free(s);free(saved);free(out);free(c);puts("Named workspaces: UTF-8, snapshots, replacement, limits, deletion, CRC, versions and transactional failures passed");return 0;
}
