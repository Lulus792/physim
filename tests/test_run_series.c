#include <run_series.h>
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Series %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void) {
 ps_run_series value,saved;memset(&saved,0x5a,sizeof saved);
 CHECK(ps_series_parse("seeded,18446744073709551615,1,1",UINT64_MAX,&value) && value.base_seed==UINT64_MAX);
 CHECK(ps_series_parse("linear,42,2,3,length,0.5,2.5",43,&value) && value.kind==2 && ps_series_value(&value)==1.5);
 char text[512];CHECK(ps_series_metadata(&value,text,sizeof text)>0 && strstr(text,"series.value=1.5\n"));
 const char *bad[]={"seeded,18446744073709551615,1,2","seeded,42,0,3","seeded,42,4,3","seeded,42,1,0","seeded,42,1,1001","seeded,18446744073709551616,1,1","seeded,42,1,3,extra","linear,42,1,3,x,1,1","linear,42,1,3,x,nan,2","linear,42,1,3,x\nforged=1,1,2","unknown,42,1,3","seeded,+42,1,3"};
 for(unsigned i=0;i<sizeof bad/sizeof *bad;i++){value=saved;CHECK(!ps_series_parse(bad[i],42,&value) && !memcmp(&value,&saved,sizeof value));}
 value=saved;CHECK(!ps_series_parse("seeded,42,2,3",42,&value) && !memcmp(&value,&saved,sizeof value));
 puts("Series provenance: signed/overflow/injection/count/seed mismatch rejection, linear interpolation and atomic parsing passed");return 0;
}
