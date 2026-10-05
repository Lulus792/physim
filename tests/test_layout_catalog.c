#include "layout_catalog.h"
#include "physim/data.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Layout catalog line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static bool write_bytes(const char *path,const void *bytes,size_t n) {
    FILE *f=fopen(path,"wb");if(!f)return false;
    bool ok=fwrite(bytes,1,n,f)==n;return !fclose(f) && ok;
}
static void put32(unsigned char *p,uint32_t n) {for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(n>>(8*i));}
static void crc(unsigned char *p,size_t n) {put32(p+n-4,ps_crc32(p,n-4));}
int main(int argc,char **argv) {
    CHECK(argc==2);char path[4096],bad[4096],directory[4096];
    snprintf(path,sizeof path,"%s/layouts.bin",argv[1]);snprintf(bad,sizeof bad,"%s/bad.bin",argv[1]);
    snprintf(directory,sizeof directory,"%s/target",argv[1]);remove(path);
    ps_layout_catalog c={0},out={0};ps_layout_state s={PS_DOCK_DEFAULT,280,320,220,1};
    CHECK(ps_layout_state_valid(&s) && ps_layout_catalog_valid(&c));
    CHECK(ps_layout_catalog_read(path,&out)==PS_EOF && !memcmp(&c,&out,sizeof c));
    CHECK(ps_layout_catalog_write(path,&c)==PS_OK && ps_layout_catalog_read(path,&out)==PS_OK && !memcmp(&c,&out,sizeof c));
    CHECK(ps_layout_catalog_put(&c,"Programmieren ä",&s)==PS_OK);
    CHECK(ps_dock_move(&s.dock,0,1,PS_DOCK_TAB));CHECK(ps_dock_select(&s.dock,1));
    CHECK(ps_dock_float_panel(&s.dock,3,(ps_dock_float){500,120,320,480}));
    CHECK(ps_dock_hide(&s.dock,2));s.show_log=0;
    CHECK(ps_layout_catalog_put(&c,"Simulation",&s)==PS_OK);
    CHECK(ps_layout_catalog_write(path,&c)==PS_OK && ps_layout_catalog_read(path,&out)==PS_OK && !memcmp(&c,&out,sizeof c));
    s.inspector_width=480;
    CHECK(ps_layout_catalog_put(&c,"Programmieren ä",&s)==PS_OK && c.count==2 && c.entries[0].state.inspector_width==480);
    CHECK(ps_layout_catalog_put(&c,c.entries[0].name,&c.entries[0].state)==PS_OK && c.count==2);
    ps_layout_catalog saved=c;
    const char *invalid[]={""," leading","trailing ","line\nbreak","bad\x7f","bad\xc0\xaf","bad\xed\xa0\x80"};
    for(unsigned i=0;i<sizeof invalid/sizeof invalid[0];i++)
        CHECK(ps_layout_catalog_put(&c,invalid[i],&s)==PS_INVALID && !memcmp(&c,&saved,sizeof c));
    char long_name[65];memset(long_name,'a',64);long_name[64]=0;
    CHECK(ps_layout_catalog_put(&c,long_name,&s)==PS_INVALID);long_name[63]=0;
    CHECK(ps_layout_catalog_put(&c,long_name,&s)==PS_OK);
    for(unsigned i=c.count;i<PS_LAYOUT_MAX;i++){char name[16];snprintf(name,sizeof name,"Layout %u",i);CHECK(ps_layout_catalog_put(&c,name,&s)==PS_OK);}
    saved=c;CHECK(ps_layout_catalog_put(&c,"Ninth",&s)==PS_LIMIT && !memcmp(&c,&saved,sizeof c));
    CHECK(ps_layout_catalog_write(path,&c)==PS_OK && ps_layout_catalog_read(path,&out)==PS_OK && !memcmp(&c,&out,sizeof c));
    CHECK(ps_layout_catalog_remove(&c,PS_LAYOUT_MAX)==PS_INVALID && !memcmp(&c,&saved,sizeof c));
    CHECK(ps_layout_catalog_remove(&c,0)==PS_OK && c.count==7 && !strcmp(c.entries[0].name,"Simulation"));
    CHECK(ps_layout_catalog_write(path,&c)==PS_OK && ps_layout_catalog_read(path,&out)==PS_OK && !memcmp(&c,&out,sizeof c));
    saved=out;
    unsigned char bytes[2612],changed[2613];FILE *f=fopen(path,"rb");CHECK(f);
    size_t n=fread(bytes,1,sizeof bytes,f);CHECK(!fclose(f) && n==20+7*324);
    for(size_t i=0;i<n;i++) {
        CHECK(write_bytes(bad,bytes,i) && ps_layout_catalog_read(bad,&out)!=PS_OK && !memcmp(&out,&saved,sizeof out));
        memcpy(changed,bytes,n);changed[i]^=0x40;
        CHECK(write_bytes(bad,changed,n) && ps_layout_catalog_read(bad,&out)!=PS_OK && !memcmp(&out,&saved,sizeof out));
    }
    memcpy(changed,bytes,n);changed[n]=0;
    CHECK(write_bytes(bad,changed,n+1) && ps_layout_catalog_read(bad,&out)==PS_CORRUPT);
    memcpy(changed,bytes,n);changed[7]='2';crc(changed,n);
    CHECK(write_bytes(bad,changed,n) && ps_layout_catalog_read(bad,&out)==PS_VERSION && !memcmp(&out,&saved,sizeof out));
    /* Correct CRC cannot admit invalid count, names, graph, active tab or dimensions. */
    const size_t offsets[]={12,16,16+64+6*24+4,16+64+5*4,16+64+244,16+64+244+4,16+64+244+8,16+64+244+12};
    const uint32_t values[]={9,0,6,3,207,481,149,2};
    for(unsigned i=0;i<sizeof offsets/sizeof offsets[0];i++) {
        memcpy(changed,bytes,n);put32(changed+offsets[i],values[i]);crc(changed,n);
        CHECK(write_bytes(bad,changed,n) && ps_layout_catalog_read(bad,&out)==PS_CORRUPT && !memcmp(&out,&saved,sizeof out));
    }
    memcpy(changed,bytes,n);memcpy(changed+16+324,changed+16,64);crc(changed,n);
    CHECK(write_bytes(bad,changed,n) && ps_layout_catalog_read(bad,&out)==PS_CORRUPT);
    memcpy(changed,bytes,n);memset(changed+16,'a',64);crc(changed,n);
    CHECK(write_bytes(bad,changed,n) && ps_layout_catalog_read(bad,&out)==PS_CORRUPT);
    /* Invalid saves and a rename failure preserve original file and catalog. */
    c.count=9;CHECK(ps_layout_catalog_write(path,&c)==PS_INVALID);c=saved;
    CHECK(ps_layout_catalog_read(path,&out)==PS_OK && !memcmp(&out,&saved,sizeof out));
    CHECK(SDL_CreateDirectory(directory) && ps_layout_catalog_write(directory,&c)==PS_IO);
    CHECK(ps_layout_catalog_write("/no-such-physim-directory/layouts.bin",&c)==PS_IO);
    CHECK(ps_layout_catalog_read(path,&out)==PS_OK && !memcmp(&out,&saved,sizeof out));
    unsigned char dock[PS_DOCK_WIRE_BYTES];ps_dock_layout d=s.dock,unchanged=d;
    CHECK(ps_dock_encode(&d,dock,sizeof dock) && ps_dock_decode(dock,sizeof dock,&d) && !memcmp(&d,&unchanged,sizeof d));
    put32(dock+6*24+4,6);
    CHECK(!ps_dock_decode(dock,sizeof dock,&d) && !memcmp(&d,&unchanged,sizeof d));
    CHECK(!ps_dock_decode(dock,sizeof dock-1,&d) && !ps_dock_encode(&d,dock,sizeof dock-1));
    puts("Named layouts: full graph, UTF-8, replacement, limits, deletion, roundtrip, mutations, versions and failure preservation passed");
    return 0;
}
