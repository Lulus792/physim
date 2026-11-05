#include "c_bindings.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"C call line %d: %s\n",__LINE__,#x);return 1;}}while(0)
typedef struct {const char *text;bool active,start;size_t argument;} vector;
static const vector cases[]={
 {"void step(int dt){ foo(|",true,true,0},
 {"void step(int dt){ foo(|); int future=1; }",true,true,0},
 {"void step(int dt){ foo(dt,|",true,true,1},
 {"void step(int dt){ foo(inner(1,2),|",true,true,1},
 {"void step(int dt){ foo(inner(1,|",true,true,1},
 {"void step(int dt){ foo(a[1,2],|",true,true,1},
 {"void step(int dt){ foo((a,b),|",true,true,1},
 {"void step(int dt){ foo(flag ? a,b : c,|",true,true,1},
 {"void step(int dt){ foo(flag ? a,|",true,false,0},
 {"void step(int dt){ foo(flag ? other ? a,b:c : d,|",true,true,1},
 {"void step(|",false,false,0},
 {"void step(double dt,|",false,false,0},
 {"int global; void step(int dt){ if(|",false,false,0},
 {"void step(int dt){ while(|",false,false,0},
 {"void step(int dt){ for(|",false,false,0},
 {"void step(int dt){ sizeof(|",false,false,0},
 {"void step(int dt){ (int)(|",false,false,0},
 {"void step(int dt){ (*callback)(|",true,true,0},
 {"void step(int dt){ callbacks[0](|",true,true,0},
 {"void step(int dt){ p->fn(|",true,true,0},
 {"void (*callback)(|",false,false,0},
 {"void step(int dt){ int a[]={1,|",false,false,0},
 {"void step(int dt){ int a[|",false,false,0},
 {"void step(int dt){ foo(/* |",false,false,0},
 {"void step(int dt){ foo(/* comment */ |",true,true,0},
 {"void step(int dt){ foo(// comment\n |",true,true,0},
 {"void step(int dt){ foo(\"parens,(|",false,false,0},
 {"void step(int dt){ foo(\"parens,(\",|",true,true,1},
 {"void step(int dt){ foo('|",false,false,0},
 {"void step(int dt){ foo('x',|",true,true,1},
 {"void step(int dt){ foo(dt);|",false,false,0},
 {"Type function(|",false,false,0},
 {"void step(int dt){ int value=0, function(|",false,false,0},
 {"void step(int dt){ int result=foo(|",true,true,0},
 {"void step(int dt){ factory()(|",true,true,0},
 {"void step(int dt){ foo((struct Point){1,2},|",true,true,1},
 {"void step(int dt){ foo(a[1,|",true,false,0},
 {"void step(int dt){ foo(va|lue); }",true,false,0},
};
int main(void){
    for(size_t i=0;i<sizeof cases/sizeof *cases;i++){
        char text[1024];const char *mark=strchr(cases[i].text,'|');CHECK(mark);
        size_t cursor=(size_t)(mark-cases[i].text),bytes=strlen(cases[i].text)-1;
        memcpy(text,cases[i].text,cursor);memcpy(text+cursor,mark+1,bytes-cursor);text[bytes]=0;
        ps_c_binding bindings[64];ps_c_bindings_result r=ps_c_bindings(text,bytes,cursor,bindings,64,(ps_allocator){0});
        if(r.status!=PS_OK || r.call.active!=cases[i].active || r.call.at_start!=cases[i].start || r.call.argument!=cases[i].argument){
            fprintf(stderr,"C call vector %zu status %u got %u/%u/%zu expected %u/%u/%zu text %s\n",i,(unsigned)r.status,r.call.active,r.call.at_start,r.call.argument,cases[i].active,cases[i].start,cases[i].argument,text);return 1;
        }
        if(r.call.active)CHECK(r.call.open<cursor && text[r.call.open]=='(');
    }
    printf("C call context: %zu original-source call/declaration/literal/nesting vectors passed\n",sizeof cases/sizeof *cases);return 0;
}
