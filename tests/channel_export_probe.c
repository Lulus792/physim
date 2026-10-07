#include "physim/data.h"
#include <stdio.h>
int main(int argc,char **argv) {
    if(argc!=3)return 2;
    ps_result result=ps_run_export_csv(argv[1],argv[2]);
    if(result!=PS_OK)fprintf(stderr,"Channel CSV export: %s\n",ps_result_string(result));
    return result==PS_OK?0:1;
}
