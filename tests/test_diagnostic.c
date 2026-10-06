#include "physim/experiment.h"
#include "physim/data.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Diagnostic line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv) {
    CHECK(argc==2);ps_diagnostic d;ps_diagnostic_clear(&d);CHECK(ps_diagnostic_valid(&d) && d.code==PS_OK);
    CHECK(ps_diagnostic_set(&d,PS_SINGULAR,"linearSolve","matrix","C:\\models α\\main.c",12,7,"Singular matrix α\nChoose independent equations") == PS_OK);
    CHECK(ps_diagnostic_valid(&d));ps_diagnostic before=d;
    CHECK(ps_diagnostic_set(&d,PS_OK,NULL,NULL,NULL,0,0,"bad")==PS_INVALID && !memcmp(&before,&d,sizeof d));
    CHECK(ps_diagnostic_set(&d,PS_EOF,NULL,NULL,NULL,0,0,"bad")==PS_INVALID);
    CHECK(ps_diagnostic_set(&d,PS_IO,"\033control",NULL,NULL,0,0,"bad")==PS_INVALID);
    CHECK(ps_diagnostic_set(&d,PS_IO,NULL,NULL,NULL,1,1,"bad")==PS_INVALID);
    CHECK(ps_diagnostic_set(&d,PS_IO,NULL,NULL,"x",0,1,"bad")==PS_INVALID);
    CHECK(ps_diagnostic_set(&d,PS_IO,NULL,NULL,NULL,0,0,"\xc0\x80")==PS_INVALID);
    char big[PS_DIAGNOSTIC_MESSAGE_MAX+2];memset(big,'x',sizeof big);big[sizeof big-1]=0;
    CHECK(ps_diagnostic_set(&d,PS_IO,NULL,NULL,NULL,0,0,big)==PS_INVALID && !memcmp(&before,&d,sizeof d));
    char formatted[PS_DIAGNOSTIC_WIRE_MAX+256];CHECK(ps_diagnostic_format(&d,formatted,sizeof formatted)==PS_OK);
    CHECK(strstr(formatted,"main.c:12:7: error") && strstr(formatted,"linearSolve/matrix") && strstr(formatted,"matrix α"));
    char short_text[4];CHECK(ps_diagnostic_set(&d,PS_IO,NULL,NULL,NULL,0,0,"ααα")==PS_OK);
    CHECK(ps_diagnostic_format(&d,short_text,sizeof short_text)==PS_LIMIT && short_text[sizeof short_text-1]==0);
    CHECK(ps_diagnostic_format(&d,formatted,sizeof formatted)==PS_OK);
    size_t prefix=(size_t)(strstr(formatted,"α")-formatted);
    CHECK(ps_diagnostic_format(&d,formatted,prefix+2)==PS_LIMIT && strlen(formatted)==prefix);d=before;
    unsigned char bytes[PS_DIAGNOSTIC_WIRE_MAX],damaged[PS_DIAGNOSTIC_WIRE_MAX];
    size_t n=ps_diagnostic_encode(bytes,sizeof bytes,&d);CHECK(n && !ps_diagnostic_encode(bytes,n-1,&d));
    ps_diagnostic read;ps_diagnostic_clear(&read);CHECK(ps_diagnostic_decode(bytes,n,&read)==PS_OK && !memcmp(&read,&d,sizeof d));
    ps_diagnostic saved=read;memcpy(damaged,bytes,n);damaged[n-1]^=1;
    CHECK(ps_diagnostic_decode(damaged,n,&read)==PS_CORRUPT && !memcmp(&read,&saved,sizeof read));
    memcpy(damaged,bytes,n);ps_put_u32(damaged+4,2);CHECK(ps_diagnostic_decode(damaged,n,&read)==PS_VERSION);
    memcpy(damaged,bytes,n);ps_put_u32(damaged+8,PS_RECOVERED);ps_put_u32(damaged+n-4,ps_crc32(damaged,n-4));
    CHECK(ps_diagnostic_decode(damaged,n,&read)==PS_CORRUPT && !memcmp(&read,&saved,sizeof read));
    memcpy(damaged,bytes,n);damaged[36]=0;ps_put_u32(damaged+n-4,ps_crc32(damaged,n-4));CHECK(ps_diagnostic_decode(damaged,n,&read)==PS_CORRUPT);
    for(size_t size=0;size<n;size++)CHECK(ps_diagnostic_decode(bytes,size,&read)!=PS_OK && !memcmp(&read,&saved,sizeof read));
    char path[4096];snprintf(path,sizeof path,"%s/record α.psdiag",argv[1]);
    CHECK(ps_diagnostic_save(path,&d)==PS_OK && ps_diagnostic_load(path,&read)==PS_OK && !memcmp(&read,&d,sizeof d));
    CHECK(ps_diagnostic_save(path,&d)==PS_IO && ps_diagnostic_load(path,&read)==PS_OK && !memcmp(&read,&d,sizeof d));
    ps_context c={0};c.struct_size=sizeof c;c.api_version=PS_API_VERSION;c.time_s=.5;c.values[0]=42;
    CHECK(ps_experiment_diagnostic(&c,&read)==PS_OK && read.code==PS_OK);
    CHECK(ps_experiment_fail(&c,&d)==PS_SINGULAR && c.time_s==.5 && c.values[0]==42 && strstr(c.error,"Singular matrix"));
    CHECK(ps_experiment_diagnostic(&c,&read)==PS_OK && !memcmp(&read,&d,sizeof d));
    ps_context unchanged=c;ps_diagnostic invalid=d;invalid.message[0]=0;
    CHECK(ps_experiment_fail(&c,&invalid)==PS_INVALID && !memcmp(&c,&unchanged,sizeof c));
    c.struct_size=(uint32_t)offsetof(ps_context,diagnostic);
    CHECK(ps_experiment_fail(&c,&d)==PS_SINGULAR && ps_experiment_diagnostic(&c,&read)==PS_VERSION);
    puts("Diagnostics: independent owned UTF-8 records, typed fields, atomic validation, CRC/version roundtrips, exclusive files and ABI-3 context compatibility passed");return 0;
}
