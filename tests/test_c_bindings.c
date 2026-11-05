#include "c_bindings.h"
#include "test_allocator.h"
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"C bindings line %d: %s\n",__LINE__,#x);return 1;}}while(0)
typedef struct {const char *source,*names,*types;} example;
static const example examples[]={
    {"void step(double dt){ double velocity=1; f(|", "dt,velocity", "double|double"},
    {"void step(double dt){ double velocity=1; f(ve|", "dt,velocity", "double|double"},
    {"int global=0; void other(int foreign){int hidden;} void step(double dt){ int velocity=1; f(|); int future=1; }", "global,dt,velocity", "int|double|int"},
    {"void step(int value){ {double value=1; f(|);} }", "value,value", "int|double"},
    {"void step(int dt){ {int gone=1;} int present=1; f(|", "dt,present", "int|int"},
    {"typedef double Velocity; void step(double dt){ const Velocity *p=0, q=1; int (*callback)(double x)=0; f(|", "dt,p,q,callback", "double|const Velocity *|const Velocity|int (*)(double x)"},
    {"int (*factory(int arg))(double) { int value=1; f(|", "arg,value", "int|int"},
    {"void step(int (*callback)(double nested), int array[10]){ (*callback)(|", "callback,array", "int (*)(double nested)|int [10]"},
    {"void step(double dt){ for(int i=0;i<10;i++){ int a=1,b=2; f(|", "dt,i,a,b", "double|int|int|int"},
    {"void step(double dt){ for(int i=0; f(|);i++){} }", "dt,i", "double|int"},
    {"void step(double dt){ for(int i=0;i<10;i++) f(|", "dt,i", "double|int"},
    {"void step(double dt){ for(int i=0;i<10;i++) f(i); f(|", "dt", "double"},
    {"void step(double dt){ for(int i=0;i<10;i++) if(i) f(i); else f(i); f(|", "dt", "double"},
    {"void step(double dt){ for(int i=0;i<10;i++){f(i);} f(|", "dt", "double"},
    {"void step(double dt){ int first=1, second=f(|", "dt,first", "double|int"},
    {"void step(double dt){ int first=1, second=f(|); }", "dt,first", "double|int"},
    {"void step(double dt){ int first, second=f(|", "dt,first", "double|int"},
    {"void step(double dt){ int current=f(|", "dt", "double"},
    {"void step(double dt){ int data[3]={1,2,3}; struct Point {int field;}; struct Point p; f(|", "dt,data,p", "double|int [3]|struct Point"},
    {"void prototype(int foreign); int (*callback)(int nested); void step(int here){ f(|", "callback,here", "int (*)(int nested)|int"},
    {"void step(int dt){ /* int fake; { } */ const char *message=\"int fake; } {\"; f(|", "dt,message", "int|const char *"},
    {"void step(int dt){ int Größe=1; f(|", "dt,Größe", "int|int"},
    {"void step(int dt){ if(dt){int nested=1; f(|", "dt,nested", "int|int"},
    {"void step(int dt){ int a=1; } f(|", "", ""},
    {"int global=1; |void step(int dt){ int future=1; }", "global", "int"},
    {"void step(int dt){ int **pointer=0; double (*matrix)[3]=0; f(|", "dt,pointer,matrix", "int|int **|double (*)[3]"},
    {"void step(int dt){ for(int i=0;i<3;i++)|", "dt,i", "int|int"},
    {"void step(int dt){ for(int i=0;i<3;i++) for(int j=0;j<2;j++) f(|", "dt,i,j", "int|int|int"},
    {"void step(int dt){ for(int i=0;i<3;i++) if(dt) f(dt); else f(|", "dt,i", "int|int"},
    {"int step(arg) int arg; { int value=1; f(|", "arg,value", "int|int"},
    {"int step(arg) int arg; { int value=1; f(|); }", "arg,value", "int|int"},
    {"int step(arg) int arg; { int value=1; f(); } void next(void){f(|", "", ""},
    {"int *step(int dt){ int value=1; f(|", "dt,value", "int|int"},
    {"const int * const step(int dt){ int value=1; f(|", "dt,value", "int|int"},
    {"void step(int callback(double nested), int dt){ f(|", "callback,dt", "int (double nested)|int"},
};
static int vectors(void){
    for(size_t k=0;k<sizeof examples/sizeof *examples;k++){
        char source[1024],names[512]={0},types[1024]={0};const char *mark=strchr(examples[k].source,'|');CHECK(mark);
        size_t cursor=(size_t)(mark-examples[k].source),bytes=strlen(examples[k].source)-1;
        memcpy(source,examples[k].source,cursor);memcpy(source+cursor,mark+1,bytes-cursor);source[bytes]=0;
        test_allocator a={0};ps_c_binding bindings[64];
        ps_c_bindings_result r=ps_c_bindings(source,bytes,cursor,bindings,64,test_domain(&a));
        if(r.status!=PS_OK){fprintf(stderr,"vector %zu status %u source %s\n",k,(unsigned)r.status,source);return 1;}
        for(size_t i=0;i<r.count;i++){
            ps_c_binding b=bindings[i];CHECK(b.name_start<b.name_end && b.name_end<=cursor && b.line && b.column);
            if(i){strcat(names,",");strcat(types,"|");CHECK(bindings[i-1].name_start<b.name_start);}
            strncat(names,source+b.name_start,b.name_end-b.name_start);
            char type[256];size_t need=ps_c_binding_type(source,bytes,b,type,sizeof type);CHECK(need>0 && need<=sizeof type && strlen(type)+1==need);
            CHECK(ps_c_binding_type(source,bytes,b,NULL,0)==need);strcat(types,type);
            char tiny[2]={'x','x'};CHECK(ps_c_binding_type(source,bytes,b,tiny,sizeof tiny)==need && !tiny[1]);
        }
        if(strcmp(names,examples[k].names)||strcmp(types,examples[k].types)){
            fprintf(stderr,"vector %zu: got names '%s' types '%s'; expected '%s' '%s'\n",k,names,types,examples[k].names,examples[k].types);return 1;
        }
        CHECK(!a.invalid && !a.live_bytes && !a.live_blocks);
    }
    printf("C bindings scopes: %zu original-source vectors passed\n",sizeof examples/sizeof *examples);return 0;
}
static int failures(void){
    const char *source="void step(double dt){ double velocity=1; f(";size_t bytes=strlen(source);
    ps_c_binding output[64];test_allocator probe={0};
    ps_c_bindings_result r=ps_c_bindings(source,bytes,bytes,output,64,test_domain(&probe));CHECK(r.status==PS_OK && r.count==2);
    size_t attempts=probe.attempts;CHECK(attempts>10 && !probe.live_bytes && !probe.invalid);
    for(size_t n=1;n<=attempts;n++){
        test_allocator a={.fail_on=n};r=ps_c_bindings(source,bytes,bytes,output,64,test_domain(&a));
        CHECK(r.status==PS_MEMORY && !r.count && !a.invalid && !a.live_blocks && !a.live_bytes);
        a.fail_on=0;r=ps_c_bindings(source,bytes,bytes,output,64,test_domain(&a));
        CHECK(r.status==PS_OK && r.count==2 && !a.invalid && !a.live_blocks && !a.live_bytes);
    }
    printf("C bindings allocation failures: %zu/%zu cleanup and retry passed\n",attempts,attempts);return 0;
}
static int bounds(void){
    ps_c_binding b[4];test_allocator a={0};ps_allocator alloc=test_domain(&a);
    CHECK(ps_c_bindings(NULL,1,0,b,4,alloc).status==PS_INVALID);
    CHECK(ps_c_bindings("x",262145,0,b,4,alloc).status==PS_LIMIT);
    CHECK(ps_c_bindings("x",1,2,b,4,alloc).status==PS_INVALID);
    CHECK(ps_c_bindings("a\0b",3,3,b,4,alloc).status==PS_INVALID);
    CHECK(ps_c_bindings("é",2,1,b,4,alloc).status==PS_INVALID);
    CHECK(ps_c_bindings("",0,0,NULL,1,alloc).status==PS_INVALID && !a.attempts);
    CHECK(ps_c_bindings(NULL,0,0,NULL,0,alloc).status==PS_OK);
    CHECK(ps_c_bindings("int a;",6,6,NULL,0,alloc).status==PS_LIMIT);
    CHECK(!a.invalid && !a.live_blocks && !a.live_bytes);
    CHECK(!ps_c_binding_type("x",1,(ps_c_binding){0},NULL,0));return 0;
}
static int prefixes(void){
    const char *source="int global=1; void foreign(int other){int hidden=0;} void step(int dt){ int velocity=1; { double nested=2; f(nested); } for(int i=0;i<3;i++){ int inner=3; f(inner); } f(velocity); }";
    size_t bytes=strlen(source);const char *names[]={"global","other","hidden","dt","velocity","nested","i","inner"};
    size_t begin[8],end[8];
    begin[0]=(size_t)(strchr(source,';')-source)+1;end[0]=bytes+1;
    begin[1]=begin[2]=(size_t)(strchr(strstr(source,"void foreign"),'{')-source)+1;
    begin[2]=(size_t)(strchr(strstr(source,"int hidden"),';')-source)+1;
    end[1]=end[2]=(size_t)(strchr(strstr(source,"void foreign"),'}')-source);
    begin[3]=(size_t)(strchr(strstr(source,"void step"),'{')-source)+1;end[3]=bytes-1;
    begin[4]=(size_t)(strchr(strstr(source,"int velocity"),';')-source)+1;end[4]=end[3];
    begin[5]=(size_t)(strchr(strstr(source,"double nested"),';')-source)+1;end[5]=(size_t)(strchr(strstr(source,"double nested"),'}')-source);
    begin[6]=(size_t)(strchr(strstr(source,"for(int i"),';')-source)+1;end[6]=(size_t)(strchr(strstr(source,"for(int i"),'}')-source);
    begin[7]=(size_t)(strchr(strstr(source,"int inner"),';')-source)+1;end[7]=end[6];
    for(size_t cursor=0;cursor<=bytes;cursor++){
        for(unsigned mode=0;mode<2;mode++){
            ps_c_binding b[32];ps_c_bindings_result r=ps_c_bindings(source,mode?cursor:bytes,cursor,b,32,(ps_allocator){0});
            if(r.status!=PS_OK){fprintf(stderr,"prefix cursor %zu mode %u status %u\n",cursor,mode,(unsigned)r.status);return 1;}
            size_t expected=0;
            for(size_t n=0;n<8;n++){
                bool visible=cursor>=begin[n] && cursor<=end[n];expected+=visible;bool found=false;
                for(size_t j=0;j<r.count;j++)if(b[j].name_end-b[j].name_start==strlen(names[n]) && !memcmp(source+b[j].name_start,names[n],strlen(names[n])))found=true;
                if(visible!=found){fprintf(stderr,"prefix cursor %zu mode %u name %s expected %u found %u\n",cursor,mode,names[n],visible,found);return 1;}
            }
            CHECK(r.count==expected);
        }
    }
    printf("C bindings prefixes: %zu cursor/full-document pairs checked against explicit scope intervals\n",bytes+1);return 0;
}
static int malformed(void){
    const char alphabet[]="int for(){[]}*;,=+\"/ abc012_\n";uint32_t seed=0x519f2u;char source[96];ps_c_binding b[64];
    for(unsigned sample=0;sample<1024;sample++){
        size_t bytes=sample%sizeof source;
        for(size_t i=0;i<bytes;i++){seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;source[i]=alphabet[seed%(sizeof alphabet-1)];}
        test_allocator a={0};ps_c_bindings_result r=ps_c_bindings(source,bytes,bytes,b,64,test_domain(&a));
        CHECK(r.status==PS_OK || r.status==PS_INVALID || r.status==PS_LIMIT);
        CHECK(!a.invalid && !a.live_blocks && !a.live_bytes);
        if(r.status!=PS_OK){CHECK(!r.count);continue;}
        for(size_t i=0;i<r.count;i++){
            char type[256];CHECK(b[i].name_start<b[i].name_end && b[i].name_end<=bytes);
            CHECK(ps_c_binding_type(source,bytes,b[i],type,sizeof type)>0);
        }
    }
    puts("C bindings malformed input: 1024 deterministic byte cases bounded");return 0;
}
int main(void){CHECK(!vectors() && !failures() && !bounds() && !prefixes() && !malformed());puts("C bindings: actual declarators, recovery scopes, written types, bounds and allocation failure passed");return 0;}
