#include "physim/data.h"
#include <stdio.h>
#include <string.h>
int main(int argc,char **argv) {
    if(argc!=3)return 2;ps_run_reader r;ps_result result=ps_run_open(&r,argv[1]);
    if(result!=PS_OK){printf("open %d\n",result);return 0;}
    unsigned count=0;ps_log_record event;
    if(argv[2][0]=='e')for(;;) {
        memset(&event,0x5a,sizeof event);ps_log_record saved=event;
        result=ps_run_event_next(&r,&event);
        if(result!=PS_OK){if(memcmp(&event,&saved,sizeof event))return 3;break;}
        count++;
    }
    else {double t,v[PS_MAX_CHANNELS];while((result=ps_run_next(&r,&t,v))==PS_OK)count++;}
    printf("status %d count %u samples %llu complete %u\n",result,count,(unsigned long long)r.samples,r.complete);
    ps_run_reader_close(&r);return 0;
}
