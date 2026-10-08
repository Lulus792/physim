#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Child usage %d: %s\n",__LINE__,#x);return 1;}}while(0)
static volatile double sink;
static int child(bool held) {
    size_t bytes=(held?64u:32u)*1024u*1024u;
    volatile unsigned char *memory=malloc(bytes);CHECK(memory);
    for(size_t i=0;i<bytes;i+=4096)memory[i]=(unsigned char)i;
    if(held){puts("READY");fflush(stdout);(void)getchar();}
    double start=ps_clock();
    do{for(unsigned i=1;i<10000;i++)sink+=sqrt(i);}while(ps_clock()-start<.06);
    ps_process_usage self;CHECK(ps_process_usage_self(&self));
    printf("USAGE %.9f %.9f %llu\n",self.user_seconds,self.system_seconds,
           (unsigned long long)self.peak_resident_bytes);fflush(stdout);
    free((void*)memory);return 7;
}
static bool drain(ps_process *p,char *text,size_t size,bool ready) {
    size_t used=strlen(text);double deadline=ps_clock()+15;
    while(ps_clock()<deadline) {
        int n=ps_process_read(p,text+used,size-used-1);
        if(n>0){used+=(size_t)n;text[used]=0;if(ready && strstr(text,"READY"))return true;}
        if(!ps_process_poll(p)) {
            while((n=ps_process_read(p,text+used,size-used-1))>0){used+=(size_t)n;text[used]=0;}
            return !ready;
        }
        if(used==size-1)return false;ps_sleep(1);
    }
    return false;
}
int main(int argc,char **argv) {
    if(argc==2)return child(!strcmp(argv[1],"--held"));
    ps_process a={0},b={0};ps_process_usage out={8,9,10},saved=out;ps_process_usage_scope scope=(ps_process_usage_scope)99;
    CHECK(!ps_process_usage_final(NULL,&out,&scope) && !ps_process_usage_final(&a,&out,&scope));
    CHECK(!memcmp(&out,&saved,sizeof out) && scope==99);
    const char *args[]={argv[0],"--child",NULL},*hold[]={argv[0],"--held",NULL};
    CHECK(ps_process_start(&b,hold,NULL));char bt[1024]={0};CHECK(drain(&b,bt,sizeof bt,true));
    CHECK(ps_process_start(&a,args,NULL) && a.pid!=b.pid);
    CHECK(!ps_process_usage_final(&a,&out,&scope) && !memcmp(&out,&saved,sizeof out));
    char at[1024]={0};CHECK(drain(&a,at,sizeof at,false) && a.exit_code==7);
    CHECK(ps_process_usage_final(&a,&out,&scope));
    double user,system;unsigned long long peak;CHECK(sscanf(at,"USAGE %lf %lf %llu",&user,&system,&peak)==3);
    CHECK(out.user_seconds>=user && out.system_seconds>=system && out.peak_resident_bytes>=peak && peak>=32u*1024u*1024u);
    ps_process_usage finished=out;
    CHECK(ps_process_write(&b,"x",1) && drain(&b,bt,sizeof bt,false) && b.exit_code==7);
    CHECK(ps_process_usage_final(&a,&out,&scope) && !memcmp(&out,&finished,sizeof out));
    CHECK(ps_process_usage_final(&b,&out,&scope) && out.peak_resident_bytes>=64u*1024u*1024u);
    ps_process_close(&a);CHECK(ps_process_usage_final(&a,&out,&scope) && !memcmp(&out,&finished,sizeof out));
    ps_process_close(&b);
    CHECK(!ps_process_start(&a,NULL,NULL) && !ps_process_usage_final(&a,&out,&scope));
    CHECK(ps_process_start(&a,hold,NULL));at[0]=0;CHECK(drain(&a,at,sizeof at,true));
    ps_process_kill(&a);CHECK(!a.running && ps_process_usage_final(&a,&out,&scope) && out.peak_resident_bytes>=64u*1024u*1024u);
    ps_process_close(&a);
    CHECK(ps_process_start(&a,hold,NULL));at[0]=0;CHECK(drain(&a,at,sizeof at,true));
    ps_process_close(&a);CHECK(!a.running && ps_process_usage_final(&a,&out,&scope));
    puts("Owned child resources: self-oracle, independent children, repeat/close, failed restart and killed children verified");return 0;
}
