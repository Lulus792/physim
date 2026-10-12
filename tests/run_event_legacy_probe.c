/* Uses only the pre-event public API; can be linked to an older Core archive. */
#include "physim/data.h"
#include <stdio.h>
int main(int argc,char **argv) {
    if(argc!=2)return 2;ps_run_reader r;
    if(ps_run_open(&r,argv[1])!=PS_OK)return 3;
    unsigned rows=0;double time,values[PS_MAX_CHANNELS];ps_result status;
    while((status=ps_run_next(&r,&time,values))==PS_OK) {
        if(rows || time!=0 || values[0]!=7)return 4;
        rows++;
    }
    int ok=status==PS_EOF && rows==1 && r.complete && r.samples==1;
    ps_run_reader_close(&r);
    if(!ok)return 5;puts("Pre-event API: new event chunks skipped, measurement/footer preserved");return 0;
}
