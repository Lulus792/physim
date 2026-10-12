#include "physim/data.h"
#include <stdio.h>
#include <string.h>
int main(int argc,char **argv) {
 if(argc!=2)return 2;ps_run_reader reader;ps_result status=ps_run_open(&reader,argv[1]);
 if(status!=PS_OK){printf("open %d\n",status);return 0;}
 ps_run_identity out,sentinel;memset(&sentinel,0x5a,sizeof sentinel);out=sentinel;
 status=ps_run_identity_read(reader.metadata,strlen(reader.metadata),&out);
 if(status!=PS_OK && memcmp(&out,&sentinel,sizeof out))return 3;
 printf("identity %d\n",status);ps_run_reader_close(&reader);return 0;
}
