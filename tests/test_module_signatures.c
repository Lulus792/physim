#include "language/editor_signatures.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Module signatures line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static ps_lang_node nodes[8192];static ps_lang_editor_function functions[64];static ps_lang_editor_parameter parameters[128];
static ps_lang_editor_signatures_result collect(const char *text){return ps_lang_editor_module_signatures(text,strlen(text),nodes,8192,functions,64,parameters,128);}
static int vectors(void){
    static const struct {const char *text,*names,*owners;ps_lang_editor_status status;} cases[]={
      {"func step():\n    later(\nfunc later(x: Float64):\n    return\n","step,later",",",PS_LANG_EDITOR_PREFIX},
      {"func step():\n    later(,)\nfunc later(x: Float64):\n    return\n","step,later",",",PS_LANG_EDITOR_PREFIX},
      {"let broken: = 1\nfunc later(x: Int64):\n    return\n","later","",PS_LANG_EDITOR_PREFIX},
      {"struct Particle:\n    func step():\n        self.next(\n    func next(x: Int64):\n        return\n    mutating func set(x: Float64):\n        return\n    static func make():\n        return\nfunc after():\n    return\n","step,next,set,make,after","Particle,Particle,Particle,Particle,",PS_LANG_EDITOR_PREFIX},
      {"enum Mode:\n    case idle\n    func step():\n        self.next(,)\n    func next(x: Int64):\n        return\n","step,next","Mode,Mode",PS_LANG_EDITOR_PREFIX},
      {"func outer():\n    func local(x: Int64):\n        return\n    local(\nfunc later():\n    return\n","outer,later",",",PS_LANG_EDITOR_PREFIX},
      {"// func fake():\nlet text = \"func fake():\"\nfunc real():\n    return\n","real","",PS_LANG_EDITOR_OK},
      {"struct Particle:\n    func broken(x:):\n        return\n    func real():\n        return\n","real","Particle",PS_LANG_EDITOR_PREFIX},
      {"func first(): bad(\nfunc second():\n    return\n","first,second",",",PS_LANG_EDITOR_PREFIX},
      {"func first():\n    call(\nfunc generic<T>(value: [T]?) -> T:\n    return value[0]\n","first,generic",",",PS_LANG_EDITOR_PREFIX},
      {"func first():\n    let text = \"open\nfunc hidden():\n    return\n","","",PS_LANG_EDITOR_INVALID},
      {"func first():\n    return\nfunc bad(x:","first","",PS_LANG_EDITOR_PREFIX},
      {"func first():\n    func local():\n        return\n    return\n","first","",PS_LANG_EDITOR_OK},
    };
    for(size_t k=0;k<sizeof cases/sizeof *cases;k++){
        ps_lang_editor_signatures_result r=collect(cases[k].text);char names[512]={0},owners[512]={0};
        for(size_t i=0;i<r.function_count;i++){
            ps_lang_editor_function f=functions[i];CHECK(!f.scope_depth && !f.scope_start && !f.visible_from && f.scope_end==strlen(cases[k].text));
            if(i){strcat(names,",");strcat(owners,",");CHECK(functions[i-1].name_start<f.name_start);}
            strncat(names,cases[k].text+f.name_start,f.name_end-f.name_start);
            strncat(owners,cases[k].text+f.owner_start,f.owner_end-f.owner_start);
            CHECK((f.method!=0)==(f.owner_end>f.owner_start));
        }
        if(r.status!=cases[k].status || strcmp(names,cases[k].names) || strcmp(owners,cases[k].owners)){
            fprintf(stderr,"Module vector %zu status=%d names=%s owners=%s diagnostic=%s\n",k,r.status,names,owners,r.diagnostic.error?r.diagnostic.error:"");return 1;
        }
    }
    printf("Module signatures: %zu actual parser recovery/header/namespace vectors passed\n",sizeof cases/sizeof *cases);return 0;
}
static int bounds(void){
    const char *text="func first():\n    later(\nfunc later(x: Int64, y: String):\n    return\n";
    ps_lang_editor_signatures_result r=ps_lang_editor_module_signatures(text,strlen(text),nodes,8192,NULL,0,NULL,0);CHECK(r.status==PS_LANG_EDITOR_PREFIX && r.function_count==2 && r.parameter_count==2);
    r=ps_lang_editor_module_signatures(text,strlen(text),nodes,8192,functions,1,parameters,1);CHECK(r.status==PS_LANG_EDITOR_LIMIT && r.function_count==2 && r.parameter_count==2);
    int fits=0;
    for(size_t capacity=2;capacity<50;capacity++){
        r=ps_lang_editor_module_signatures(text,strlen(text),nodes,capacity,functions,64,parameters,128);
        if(r.status==PS_LANG_EDITOR_LIMIT){CHECK(!fits && !r.function_count && !r.parameter_count);}
        else{CHECK(r.status==PS_LANG_EDITOR_PREFIX && r.function_count==2 && r.parameter_count==2);fits=1;}
    }
    CHECK(fits);
    r=ps_lang_editor_signatures(text,strlen(text),nodes,8192,functions,64,parameters,128);CHECK(r.status==PS_LANG_EDITOR_INVALID && !r.function_count);
    size_t compared=0;
    for(size_t size=0;size<=strlen(text);size++){
        r=ps_lang_editor_module_signatures(text,size,nodes,8192,functions,64,parameters,128);
        CHECK(r.status==PS_LANG_EDITOR_OK || r.status==PS_LANG_EDITOR_PREFIX);
        for(size_t i=0;i<r.function_count;i++){
            ps_lang_editor_function f=functions[i];CHECK(f.name_start<f.name_end && f.name_end<=f.header_end && f.header_end<size && !f.scope_depth);
            CHECK(f.parameter_first<=r.parameter_count && f.parameter_count<=r.parameter_count-f.parameter_first);compared++;
        }
    }
    printf("Module signatures: %zu original-span prefix/header comparisons passed\n",compared);return 0;
}
static int namespace_headers(void){
    const char *bad[]={"struct Broken(","enum Broken(","import broken extra"};
    for(size_t i=0;i<3;i++){
        char text[512];snprintf(text,sizeof text,"func prior():\n    return\n%s\nfunc later():\n    return\n",bad[i]);
        ps_lang_editor_signatures_result r=collect(text);CHECK(r.status==PS_LANG_EDITOR_PREFIX && r.function_count==2);
        CHECK(functions[0].name_end-functions[0].name_start==5 && !memcmp(text+functions[0].name_start,"prior",5));
        CHECK(functions[1].name_end-functions[1].name_start==5 && !memcmp(text+functions[1].name_start,"later",5));
    }
    puts("Module signatures: 3 malformed namespace headers resume with bounded forward progress");return 0;
}
int main(int argc,char **argv){if(argc==2 && !strcmp(argv[1],"namespace"))return namespace_headers();CHECK(!vectors());CHECK(!bounds());CHECK(!namespace_headers());return 0;}
