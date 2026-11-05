#include "c_syntax.h"
#include "test_allocator.h"
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif
#define CHECK(x) do{if(!(x)){fprintf(stderr,"C syntax line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static const char text[]="// Größe\nvoid step(double dt){ double *velocity=0, damping=1; int (*callback)(double value)=0; f(velocity,dt); }";
static int shape(void){
    ps_c_node nodes[512];test_allocator a={0};
    ps_c_syntax_result r=ps_c_syntax(text,strlen(text),nodes,512,test_domain(&a),(ps_c_limits){0});
    CHECK(r.status==PS_OK && !r.has_error && r.count>30 && r.allocations==a.attempts && r.heap_peak==a.peak_bytes);
    CHECK(!strcmp(nodes[0].type,"translation_unit") && nodes[0].parent==SIZE_MAX);
    unsigned decl=0,param=0,call=0,ptr=0;
    for(size_t i=0;i<r.count;i++){
        CHECK(nodes[i].start<=nodes[i].end && nodes[i].end<=strlen(text) && nodes[i].line>=1 && nodes[i].column>=1);
        if(i)CHECK(nodes[i].parent<i);
        if(!strcmp(nodes[i].type,"declaration"))decl++;
        if(!strcmp(nodes[i].type,"parameter_declaration"))param++;
        if(!strcmp(nodes[i].type,"call_expression"))call++;
        if(!strcmp(nodes[i].type,"pointer_declarator"))ptr++;
        if(nodes[i].field && !strcmp(nodes[i].field,"body"))CHECK(!strcmp(nodes[i].type,"compound_statement") && nodes[i].line==2);
    }
    CHECK(decl==2 && param==2 && call==1 && ptr==2);
    CHECK(!a.invalid && !a.live_blocks && !a.live_bytes);
    /* Grammar names/fields remain valid after all private tree storage is gone. */
    CHECK(!strcmp(nodes[0].type,"translation_unit"));
    const char *partial="void step(double dt){ double velocity=1; f(";
    r=ps_c_syntax(partial,strlen(partial),nodes,512,(ps_allocator){0},(ps_c_limits){0});
    CHECK(r.status==PS_OK && r.has_error && r.count>0);
    bool error=false,parameter=false;
    for(size_t i=0;i<r.count;i++){error|=nodes[i].error;parameter|=!strcmp(nodes[i].type,"parameter_declaration");CHECK(nodes[i].end<=strlen(partial));}
    CHECK(error && parameter);return 0;
}
static int failures(void){
    ps_c_node nodes[512];test_allocator probe={0};
    ps_c_syntax_result r=ps_c_syntax(text,strlen(text),nodes,512,test_domain(&probe),(ps_c_limits){0});
    CHECK(r.status==PS_OK && !probe.invalid && !probe.live_blocks);
    size_t allocations=probe.attempts;CHECK(allocations>10);
    for(size_t i=1;i<=allocations;i++){
        test_allocator a={.fail_on=i};
        r=ps_c_syntax(text,strlen(text),nodes,512,test_domain(&a),(ps_c_limits){0});
        CHECK(r.status==PS_MEMORY && !r.count && !a.live_blocks && !a.live_bytes && !a.invalid);
        a.fail_on=0;r=ps_c_syntax(text,strlen(text),nodes,512,test_domain(&a),(ps_c_limits){0});
        CHECK(r.status==PS_OK && !a.live_blocks && !a.invalid);
    }
    printf("C syntax allocation failures: %zu/%zu rejected with exact cleanup and retry\n",allocations,allocations);return 0;
}
static int bounds(void){
    ps_c_node nodes[512];test_allocator a={0};ps_c_syntax_result r;
    r=ps_c_syntax("x",256u*1024u+1,nodes,512,test_domain(&a),(ps_c_limits){0});CHECK(r.status==PS_LIMIT && !a.attempts);
    r=ps_c_syntax("a\0b",3,nodes,512,test_domain(&a),(ps_c_limits){0});CHECK(r.status==PS_INVALID && !a.attempts);
    r=ps_c_syntax(NULL,1,nodes,512,test_domain(&a),(ps_c_limits){0});CHECK(r.status==PS_INVALID && !a.attempts);
    r=ps_c_syntax(text,strlen(text),NULL,1,test_domain(&a),(ps_c_limits){0});CHECK(r.status==PS_INVALID && !a.attempts);
    r=ps_c_syntax(NULL,0,nodes,512,test_domain(&a),(ps_c_limits){0});CHECK(r.status==PS_OK && r.count==1 && !r.has_error && !a.live_blocks);
    r=ps_c_syntax(text,strlen(text),NULL,0,test_domain(&a),(ps_c_limits){0});CHECK(r.status==PS_LIMIT && !r.count && !a.live_blocks);
    r=ps_c_syntax(text,strlen(text),nodes,1,test_domain(&a),(ps_c_limits){0});CHECK(r.status==PS_LIMIT && !r.count && !a.live_blocks);
    r=ps_c_syntax(text,strlen(text),nodes,512,test_domain(&a),(ps_c_limits){.heap_bytes=1});CHECK(r.status==PS_LIMIT && !r.count && !a.live_blocks);
    size_t peak=a.peak_bytes;
    r=ps_c_syntax(text,strlen(text),nodes,512,test_domain(&a),(ps_c_limits){.heap_bytes=peak-1});CHECK(r.status==PS_LIMIT && !r.count && !a.live_blocks && !a.invalid);
    size_t repetitions=4000,bytes=2+repetitions*4;char *source=malloc(bytes+1);CHECK(source);
    memcpy(source,"{\n",2);for(size_t i=0;i<repetitions;i++)memcpy(source+2+i*4,"x++;",4);source[bytes]=0;
    r=ps_c_syntax(source,bytes,nodes,512,(ps_allocator){0},(ps_c_limits){.progress_checks=1});free(source);
    CHECK(r.status==PS_LIMIT && !r.count && r.progress_checks==2);return 0;
}
typedef struct {int failed;bool fault;} job;
static void run_job(job *j){
    for(unsigned i=0;i<40;i++){
        test_allocator a={.fail_on=j->fault?2:0};ps_c_node nodes[512];
        ps_c_syntax_result r=ps_c_syntax(text,strlen(text),nodes,512,test_domain(&a),(ps_c_limits){0});
        if(r.status!=(j->fault?PS_MEMORY:PS_OK) || a.invalid || a.live_blocks || a.live_bytes)j->failed=1;
    }
}
#ifdef _WIN32
static DWORD WINAPI worker(void *p){run_job(p);return 0;}
#else
static void *worker(void *p){run_job(p);return NULL;}
#endif
static int parallel(void){
    job jobs[2]={{.fault=true},{0}};
#ifdef _WIN32
    HANDLE threads[2]={CreateThread(NULL,0,worker,&jobs[0],0,NULL),CreateThread(NULL,0,worker,&jobs[1],0,NULL)};
    CHECK(threads[0] && threads[1]);CHECK(WaitForMultipleObjects(2,threads,TRUE,INFINITE)==WAIT_OBJECT_0);
    CloseHandle(threads[0]);CloseHandle(threads[1]);
#else
    pthread_t threads[2];CHECK(!pthread_create(&threads[0],NULL,worker,&jobs[0]) && !pthread_create(&threads[1],NULL,worker,&jobs[1]));
    CHECK(!pthread_join(threads[0],NULL) && !pthread_join(threads[1],NULL));
#endif
    CHECK(!jobs[0].failed && !jobs[1].failed);return 0;
}
static int malformed(void){
    static const char alphabet[]="int for(){[]}*;,=+\"/ abc012_\n";
    uint32_t seed=0x823a17u;char source[96];ps_c_node nodes[512];
    for(unsigned sample=0;sample<1024;sample++){
        size_t bytes=sample%sizeof source;
        for(size_t i=0;i<bytes;i++){seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;source[i]=alphabet[seed%(sizeof alphabet-1)];}
        ps_c_syntax_result r=ps_c_syntax(source,bytes,nodes,512,(ps_allocator){0},(ps_c_limits){0});
        CHECK(r.status==PS_OK || r.status==PS_LIMIT);
        if(r.status==PS_LIMIT){CHECK(!r.count);continue;}
        CHECK(r.count>0 && (!strcmp(nodes[0].type,"translation_unit") ||
            (!strcmp(nodes[0].type,"ERROR") && nodes[0].error && r.has_error)));
        for(size_t i=0;i<r.count;i++)CHECK(nodes[i].start<=nodes[i].end && nodes[i].end<=bytes && (!i || nodes[i].parent<i));
    }
    puts("C syntax malformed input: 1024 deterministic byte cases bounded");return 0;
}
int main(void){CHECK(!shape() && !failures() && !bounds() && !parallel() && !malformed());puts("C syntax: actual grammar, source spans, incomplete error tree, budgets, cancellation, every allocation failure and parallel domains passed");return 0;}
