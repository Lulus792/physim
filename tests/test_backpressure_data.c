#include "physim/data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv) {
    if(argc!=4)return 2;unsigned expected=(unsigned)strtoul(argv[2],NULL,10);ps_run_reader r;
    if(ps_run_open(&r,argv[1])!=PS_OK)return 3;
    bool ok=r.channels==2;double t,v[PS_MAX_CHANNELS];unsigned count=0;ps_result result;
    while((result=ps_run_next(&r,&t,v))==PS_OK){ok &= t==count*.001 && v[0]==count && v[1]==2*count;count++;}
    ok &= (!strcmp(argv[3],"recovered")?count>0:count==expected+1) && result==(!strcmp(argv[3],"closed")?PS_EOF:PS_RECOVERED);
    ps_run_reader_close(&r);if(!ok)fprintf(stderr,"Backpressure data: count=%u expected=%u result=%d\n",count,expected+1,result);
    return ok?0:1;
}
