#include "channel_units.h"
#include "physim/data.h"
#include <SDL3/SDL.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Channel units line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static bool write_bytes(const char *path,const void *bytes,size_t n) {
    FILE *f=fopen(path,"wb");if(!f)return false;bool ok=fwrite(bytes,1,n,f)==n;return !fclose(f) && ok;
}
int main(int argc,char **argv) {
    CHECK(argc==2);char path[4096],bad[4096],directory[4096];
    snprintf(path,sizeof path,"%s/units ä.bin",argv[1]);snprintf(bad,sizeof bad,"%s/bad.bin",argv[1]);
    snprintf(directory,sizeof directory,"%s/directory",argv[1]);
    ps_channel ch={.name="position.x",.unit="m",.dimension={1}};
    ps_display_unit cm={.dimension={1},.scale=.01,.symbol="cm"},u;
    ps_channel_units c={0},out={0};double value=99;
    CHECK(ps_channel_units_read(path,&out)==PS_EOF && !out.count);
    CHECK(ps_channel_units_get(&c,&ch,&u)==PS_OK && u.scale==1 && !strcmp(u.symbol,"m"));
    CHECK(ps_display_unit_value(&cm,1.5,&value)==PS_OK && value==150);
    CHECK(ps_display_unit_value(&cm,-0.0,&value)==PS_OK && signbit(value));
    u=cm;u.scale=DBL_TRUE_MIN;
    CHECK(ps_display_unit_value(&u,DBL_TRUE_MIN,&value)==PS_OK && value==1);
    value=99;CHECK(ps_display_unit_value(&u,DBL_MAX,&value)==PS_NUMERIC && value==99);
    u.scale=DBL_MAX;CHECK(ps_display_unit_value(&u,DBL_TRUE_MIN,&value)==PS_NUMERIC && value==99);
    u.scale=0;CHECK(ps_display_unit_value(&u,1,&value)==PS_INVALID && value==99);
    /* Names are supplied by the schema; preserve spaces and unnamed channels. */
    ps_channel unusual=ch;strcpy(unusual.name," position ä ");
    CHECK(ps_channel_units_put(&c,&unusual,&cm)==PS_OK && ps_channel_units_get(&c,&unusual,&u)==PS_OK && u.scale==.01);
    CHECK(ps_channel_units_remove(&c,&unusual)==PS_OK && !c.count);
    unusual.name[0]=0;CHECK(ps_channel_units_put(&c,&unusual,&cm)==PS_OK);
    CHECK(ps_channel_units_remove(&c,&unusual)==PS_OK && !c.count);
    strcpy(unusual.name,"bad\xc0\xaf");CHECK(ps_channel_units_put(&c,&unusual,&cm)==PS_INVALID && !c.count);
    CHECK(ps_channel_units_put(&c,&ch,&cm)==PS_OK && c.count==1);
    CHECK(ps_channel_units_put(&c,&ch,&c.entries[0].unit)==PS_OK && c.count==1);
    ps_channel other=ch;other.dimension[0]=0;other.dimension[2]=1;strcpy(other.unit,"s");
    CHECK(ps_channel_units_get(&c,&other,&u)==PS_OK && u.scale==1 && !strcmp(u.symbol,"s"));
    CHECK(ps_channel_units_put(&c,&other,&cm)==PS_INVALID && c.count==1);
    u.scale=.001;strcpy(u.symbol,"ms");CHECK(ps_channel_units_put(&c,&other,&u)==PS_OK && c.count==2);
    CHECK(ps_channel_units_write(path,&c)==PS_OK && ps_channel_units_read(path,&out)==PS_OK && !memcmp(&c,&out,sizeof c));
    ps_channel_units saved=c;unsigned char bytes[8148],changed[8149];FILE *f=fopen(path,"rb");CHECK(f);
    size_t n=fread(bytes,1,sizeof bytes,f);CHECK(!fclose(f) && n==274);
    for(size_t i=0;i<n;i++) {
        CHECK(write_bytes(bad,bytes,i) && ps_channel_units_read(bad,&out)!=PS_OK && !memcmp(&saved,&out,sizeof out));
        memcpy(changed,bytes,n);changed[i]^=64;
        CHECK(write_bytes(bad,changed,n) && ps_channel_units_read(bad,&out)!=PS_OK && !memcmp(&saved,&out,sizeof out));
    }
    memcpy(changed,bytes,n);changed[7]='2';CHECK(write_bytes(bad,changed,n) && ps_channel_units_read(bad,&out)==PS_VERSION);
    memcpy(changed,bytes,n);changed[n]=0;CHECK(write_bytes(bad,changed,n+1) && ps_channel_units_read(bad,&out)==PS_CORRUPT);
    /* CRC-valid duplicates still fail semantic validation. */
    memcpy(changed,bytes,n);memcpy(changed+16+127,changed+16,127);uint32_t crc=ps_crc32(changed,n-4);
    for(unsigned i=0;i<4;i++)changed[n-4+i]=(unsigned char)(crc>>(i*8));
    CHECK(write_bytes(bad,changed,n) && ps_channel_units_read(bad,&out)==PS_CORRUPT);
    for(unsigned i=2;i<64;i++){snprintf(ch.name,sizeof ch.name,"channel %u",i);CHECK(ps_channel_units_put(&c,&ch,&cm)==PS_OK);}
    saved=c;strcpy(ch.name,"overflow");CHECK(ps_channel_units_put(&c,&ch,&cm)==PS_LIMIT && !memcmp(&saved,&c,sizeof c));
    strcpy(ch.name,"channel 2");cm.scale=.001;strcpy(cm.symbol,"mm ä");
    CHECK(ps_channel_units_put(&c,&ch,&cm)==PS_OK && c.count==64);
    CHECK(ps_channel_units_write(path,&c)==PS_OK && ps_channel_units_read(path,&out)==PS_OK && !memcmp(&c,&out,sizeof c));
    CHECK(ps_channel_units_remove(&c,&ch)==PS_OK && c.count==63);
    saved=c;strcpy(cm.symbol,"bad\nunit");CHECK(ps_channel_units_put(&c,&ch,&cm)==PS_INVALID && !memcmp(&saved,&c,sizeof c));
    CHECK(SDL_CreateDirectory(directory) && ps_channel_units_write(directory,&c)==PS_IO);
    CHECK(ps_channel_units_write("/missing-physim-directory/units.bin",&c)==PS_IO);
    CHECK(ps_channel_units_read(path,&out)==PS_OK && out.count==64);
    puts("Channel display units: dimensions, conversion limits, persistence, mutations and failure preservation passed");return 0;
}
