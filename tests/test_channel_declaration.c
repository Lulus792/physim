#include "physim/data.h"
#include "physim/units.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if(!(x)){fprintf(stderr,"Channel declaration %d: %s\n",__LINE__,#x);return 1;} } while(0)
static int rejected(ps_context *c,const char *name,ps_unit unit,const char *description) {
    ps_context saved=*c;
    return ps_channel_add(c,name,unit,description)==-1 && !memcmp(c,&saved,sizeof saved);
}
int main(int argc,char **argv) {
    CHECK(argc==2);ps_context c={0};c.dt_s=.25;c.seed=42;c.values[0]=19;
    ps_unit cm=PS_METRE;cm.scale=.01;cm.symbol="cm";
    CHECK(rejected(&c,"length",cm,"length in centimetres"));
    CHECK(!c.channel_count && c.values[0]==19);
    ps_unit invalid=PS_METRE;double scales[]={0,-1,NAN,INFINITY,2};
    for(unsigned i=0;i<sizeof scales/sizeof *scales;i++){invalid.scale=scales[i];CHECK(rejected(&c,"length",invalid,"test"));}
    invalid=PS_METRE;invalid.symbol=NULL;CHECK(rejected(&c,"length",invalid,"test"));
    CHECK(rejected(&c,NULL,PS_METRE,"test"));CHECK(rejected(&c,"length",PS_METRE,NULL));
    CHECK(rejected(&c,"",PS_METRE,"test"));CHECK(rejected(&c,"bad\xc0\xaf",PS_METRE,"test"));
    CHECK(rejected(&c,"bad\nname",PS_METRE,"test"));
    invalid=PS_METRE;invalid.symbol="bad\xc0\xaf";CHECK(rejected(&c,"length",invalid,"test"));
    char name[49],symbol[17],description[97];memset(name,'n',sizeof name);name[48]=0;
    memset(symbol,'u',sizeof symbol);symbol[16]=0;memset(description,'d',sizeof description);description[96]=0;
    CHECK(rejected(&c,name,PS_METRE,"test"));invalid=PS_METRE;invalid.symbol=symbol;CHECK(rejected(&c,"length",invalid,"test"));
    CHECK(rejected(&c,"length",PS_METRE,description));
    name[47]=0;symbol[15]=0;description[95]=0;invalid.symbol=symbol;
    CHECK(ps_channel_add(&c,name,invalid,description)==0);
    CHECK(!strcmp(c.channels[0].name,name) && !strcmp(c.channels[0].unit,symbol) && !strcmp(c.channels[0].description,description));
    CHECK(rejected(&c,name,PS_METRE,"duplicate"));
    CHECK(ps_channel_add(&c,"length,\"α\"",PS_METRE,"explicit SI\nconverted from cm")==1);
    CHECK(ps_convert(125,cm,PS_METRE,&c.values[1])==PS_OK && c.values[1]==1.25);
    char path[4096],csv[4096];snprintf(path,sizeof path,"%s/canonical.psrun",argv[1]);snprintf(csv,sizeof csv,"%s/canonical.csv",argv[1]);
    ps_run_writer writer;CHECK(ps_run_create(&writer,path,&c,"canonical SI")==PS_OK);
    CHECK(ps_run_append(&writer,0,c.values)==PS_OK && ps_run_close(&writer)==PS_OK);
    ps_run_reader reader;CHECK(ps_run_open(&reader,path)==PS_OK && reader.channels==2);
    CHECK(!strcmp(reader.schema[1].unit,"m") && reader.schema[1].dimension[0]==1);
    double time,values[PS_MAX_CHANNELS];CHECK(ps_run_next(&reader,&time,values)==PS_OK && time==0 && values[1]==1.25);
    CHECK(ps_run_next(&reader,&time,values)==PS_EOF);ps_run_reader_close(&reader);
    CHECK(ps_run_export_csv(path,csv)==PS_OK);FILE *f=fopen(csv,"rb");CHECK(f);char text[1024]={0};CHECK(fread(text,1,sizeof text-1,f)>0 && !fclose(f));
    CHECK(strstr(text,"length,\"\"α\"\" [m]") && strstr(text,"1.25"));
    for(unsigned i=2;i<PS_MAX_CHANNELS;i++){snprintf(name,sizeof name,"channel %u",i);CHECK(ps_channel_add(&c,name,PS_ONE,"")==(int)i);}
    CHECK(rejected(&c,"overflow",PS_ONE,""));
    c.channel_count=PS_MAX_CHANNELS+1;CHECK(rejected(&c,"corrupt count",PS_ONE,""));
    CHECK(ps_channel_add(NULL,"length",PS_METRE,"")==-1);
    ps_context alias={0};strcpy(alias.channels[0].name,"alias");strcpy(alias.channels[0].unit,"m");
    strcpy(alias.channels[0].description,"owned input alias");invalid=PS_METRE;invalid.symbol=alias.channels[0].unit;
    CHECK(ps_channel_add(&alias,alias.channels[0].name,invalid,alias.channels[0].description)==0);
    CHECK(!strcmp(alias.channels[0].name,"alias") && !strcmp(alias.channels[0].description,"owned input alias"));
    alias.channel_count=1;memset(alias.channels[0].name,'x',sizeof alias.channels[0].name);
    CHECK(rejected(&alias,"new",PS_METRE,"malformed previous name"));
    puts("Channel declaration: canonical SI, bounded UTF-8, uniqueness, atomic errors and real run/CSV values passed");return 0;
}
