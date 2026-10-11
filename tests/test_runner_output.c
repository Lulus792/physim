#include "runner_output.h"
#include "protocol.h"
#include "platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv) {
    if(argc!=2)return 2;ps_binary_stdio();ps_runner_output *out=ps_runner_output_create();if(!out)return 3;
    unsigned char payload[PS_WIRE_MAX];memset(payload,'x',sizeof payload);
    unsigned count=0;bool overflow=!strcmp(argv[1],"overflow"),ok=true;
    if(overflow) {
        while(count<2000 && ps_runner_output_send(out,PS_MSG_ERROR,payload,sizeof payload,false))count++;
        ok=count>=900 && count<1100 && !ps_runner_output_healthy(out);
    } else {
        ok=ps_runner_output_send(out,PS_MSG_ERROR,"before",6,false);
        ps_put_u32(payload,0);ok &= ps_runner_output_send(out,PS_MSG_SNAPSHOT,payload,sizeof payload,true);
        ok &= ps_runner_output_send(out,PS_MSG_ERROR,"barrier",7,false);
        for(count=1;count<=1000 && ok;count++){ps_put_u32(payload,count);ok=ps_runner_output_send(out,PS_MSG_SNAPSHOT,payload,sizeof payload,true);}
        ok &= ps_runner_output_healthy(out);
    }
    FILE *file=fopen("output-ready.tmp","wb");if(!file)_Exit(4);
    if(fprintf(file,"%u %d\n",count,ok)<0 || fclose(file))_Exit(4);
    if(rename("output-ready.tmp","output-ready.txt"))_Exit(4);
    bool drained=ps_runner_output_finish(out,2);
    if(!drained)_Exit(ok?7:4);
    return ok?0:4;
}
