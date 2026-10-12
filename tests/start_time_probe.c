#include "physim/data.h"
#include <stdio.h>
#include <string.h>
int main(int argc,char **argv) {
 if(argc!=2)return 2;ps_run_reader r;ps_result status=ps_run_open(&r,argv[1]);
 if(status!=PS_OK){printf("open %d\n",status);return 0;}
 ps_run_start_time out,sentinel;memset(&sentinel,0x5a,sizeof sentinel);out=sentinel;
 status=ps_run_start_time_read(r.metadata,strlen(r.metadata),&out);
 if(status!=PS_OK && memcmp(&out,&sentinel,sizeof out))return 3;
 if(status==PS_OK)printf("time %lld %u\n",(long long)out.unix_seconds,(unsigned)out.nanoseconds);
 else printf("status %d\n",status);
 ps_run_reader_close(&r);return 0;
}
