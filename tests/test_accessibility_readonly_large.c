#include "accessibility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Large readonly %d: %s\n",__LINE__,#x);return 1;}}while(0)

/* Faults affect this subject only, including its snapshot release paths. */
static size_t live_allocations;
static size_t live_bytes,peak_bytes;
static struct {void *pointer;size_t bytes;} allocations[64];
static bool allocation_error;
static bool fail_next_allocation;
static void *subject_malloc(size_t bytes) {
    if(fail_next_allocation){fail_next_allocation=false;return NULL;}
    void *result=malloc(bytes);
    if(result){
        size_t i=0;while(i<64 && allocations[i].pointer)i++;
        if(i==64){free(result);allocation_error=true;return NULL;}
        allocations[i].pointer=result;allocations[i].bytes=bytes;live_allocations++;live_bytes+=bytes;
        if(live_bytes>peak_bytes)peak_bytes=live_bytes;
    }return result;
}
static void subject_free(void *pointer) {
    if(pointer){
        size_t i=0;while(i<64 && allocations[i].pointer!=pointer)i++;
        if(i==64)allocation_error=true;
        else{memset(pointer,0xdd,allocations[i].bytes);live_allocations--;live_bytes-=allocations[i].bytes;allocations[i].pointer=NULL;}
    }free(pointer);
}
#define malloc subject_malloc
#define free subject_free
#include "../app/accessibility.c"
#undef free
#undef malloc

#ifdef PS_A11Y_READONLY_BYTES
#define TEXT(node) ps_a11y_node_text(node)
#else
#define TEXT(node) ((node)->text_value)
#endif

int main(void) {
    const char pattern[]="A\tθ🌍\r\n",suffix[]="END ᵣ";
    size_t repeats=8192,length=repeats*(sizeof pattern-1)+sizeof suffix-1;
    char *text=malloc(length+1);ps_a11y_model *model=malloc(sizeof *model);CHECK(text && model);
    for(size_t i=0;i<repeats;i++)memcpy(text+i*(sizeof pattern-1),pattern,sizeof pattern-1);
    memcpy(text+repeats*(sizeof pattern-1),suffix,sizeof suffix);
    ps_a11y_init(model);float bounds[]={0,0,400,200};ps_a11y_begin(model);
    ps_a11y_record_edit_mode(model,"log","Originalmeldung",bounds,true,true,true,text,length+1);
    printf("Complete readonly: bytes=%zu nodes=%zu fixed-inline-bytes=%u\n",length,model->draft_count,(unsigned)PS_A11Y_EDIT_BYTES);
    CHECK(model->draft_count==1);CHECK(!strcmp(TEXT(&model->draft[0]),text));CHECK(ps_a11y_publish(model));
    uint64_t id=model->nodes[0].id,version=model->nodes[0].text_version;
    CHECK(id && version && model->nodes[0].text_read_only && !strcmp(TEXT(&model->nodes[0]),text));
    CHECK(!ps_a11y_set_text(model,id,"replace"));CHECK(!ps_a11y_edit_replace(model,id,version,"replace"));
    size_t last=length-(sizeof suffix-1),start=0,end=0;bool top=true;
    CHECK(ps_a11y_edit_select(model,id,version,last,length));
    ps_a11y_begin(model);ps_a11y_record_edit_mode(model,"log","Originalmeldung",bounds,true,true,true,text,length+1);
    CHECK(ps_a11y_edit_take_selection(model,0,&start,&end) && start==last && end==length);
    CHECK(ps_a11y_edit_update(model,0,text,start,end));CHECK(ps_a11y_publish(model));
    CHECK(ps_a11y_edit_scroll(model,id,version,last,length,false));
    ps_a11y_begin(model);ps_a11y_record_edit_mode(model,"log","Originalmeldung",bounds,true,true,true,text,length+1);
    CHECK(ps_a11y_edit_take_scroll(model,0,&start,&end,&top) && start==last && end==length && !top);
    CHECK(ps_a11y_edit_box(model,0,last,length,bounds));CHECK(ps_a11y_publish(model));
    CHECK(!ps_a11y_edit_select(model,id,version,length-1,length)); /* inside U+1D63 */
#ifdef PS_A11Y_READONLY_BYTES
    const char *published=TEXT(&model->nodes[0]);size_t live=live_allocations;
    ps_a11y_begin(model);ps_a11y_record_edit_mode(model,"log","Originalmeldung",bounds,true,true,true,text,length+1);
    CHECK(TEXT(&model->draft[0])==published && live_allocations==live);ps_a11y_publish(model);
    text[0]='B';CHECK(TEXT(&model->nodes[0])[0]=='A');
    fail_next_allocation=true;ps_a11y_begin(model);
    ps_a11y_record_edit_mode(model,"log","Originalmeldung",bounds,true,true,true,text,length+1);
    CHECK(!fail_next_allocation && !ps_a11y_publish(model));
    CHECK(TEXT(&model->nodes[0])==published && model->nodes[0].text_version==version);
    ps_a11y_begin(model);ps_a11y_record_edit_mode(model,"log","Originalmeldung",bounds,true,true,true,text,length+1);CHECK(ps_a11y_publish(model));
    CHECK(model->nodes[0].id==id && model->nodes[0].text_version!=version && TEXT(&model->nodes[0])[0]=='B');
    CHECK(!ps_a11y_edit_select(model,id,version,last,length));
    CHECK(!ps_a11y_edit_scroll(model,id,version,last,length,true));
    size_t maximum=PS_A11Y_READONLY_BYTES-1;char *large=malloc(maximum+2);CHECK(large);
    memset(large,'Z',maximum+1);large[maximum+1]=0;
    ps_a11y_begin(model);
    CHECK(!ps_a11y_record_edit_mode(model,"log","Originalmeldung",bounds,true,true,true,large,maximum+2));
    CHECK(model->draft_count==0 && model->count==1 && !strcmp(TEXT(&model->nodes[0]),text));
    large[maximum]=0;
    ps_a11y_begin(model);ps_a11y_record_edit_mode(model,"log","Originalmeldung",bounds,true,true,true,text,length+1);
    ps_a11y_record_edit_mode(model,"log","Zusatz",bounds,true,true,true,large,maximum+1);
    CHECK(!ps_a11y_publish(model) && model->count==1 && !strcmp(TEXT(&model->nodes[0]),text));
    ps_a11y_begin(model);ps_a11y_record_edit_mode(model,"log","Originalmeldung",bounds,true,true,true,large,maximum+1);
    CHECK(ps_a11y_publish(model) && ps_a11y_node_text_size(&model->nodes[0])==maximum && !strcmp(TEXT(&model->nodes[0]),large));
    char small[]="short θ";
    ps_a11y_begin(model);ps_a11y_record_edit_mode(model,"log","Originalmeldung",bounds,true,true,true,small,sizeof small);
    CHECK(ps_a11y_publish(model) && !model->nodes[0].text_snapshot && !strcmp(TEXT(&model->nodes[0]),small));
    /* A source slice may belong to this draft's only snapshot reference. */
    ps_a11y_begin(model);ps_a11y_record_edit_mode(model,"log","Alias",bounds,true,true,true,text,length+1);
    const char *part=TEXT(&model->draft[0])+last;
    CHECK(ps_a11y_edit_update(model,0,part,0,strlen(part)));
    CHECK(!model->draft[0].text_snapshot && !strcmp(TEXT(&model->draft[0]),suffix));
    part=TEXT(&model->draft[0])+4;CHECK(ps_a11y_edit_update(model,0,part,0,strlen(part)));
    CHECK(!strcmp(TEXT(&model->draft[0]),"ᵣ"));CHECK(ps_a11y_publish(model));
    ps_a11y_begin(model);ps_a11y_record_edit_mode(model,"log","Originalmeldung",bounds,true,true,true,text,length+1);CHECK(ps_a11y_publish(model));free(large);
    ps_a11y_node retained=model->nodes[0];ps_a11y_node_text_retain(&retained);
    ps_a11y_begin(model);CHECK(ps_a11y_publish(model));CHECK(!ps_a11y_find(model,id));
    ps_a11y_destroy(model);CHECK(!strcmp(TEXT(&retained),text));ps_a11y_node_text_release(&retained);
#else
    ps_a11y_destroy(model);
#endif
    CHECK(!allocation_error && live_allocations==0 && live_bytes==0);
    printf("Snapshot allocation peak=%zu bytes; model=%zu bytes; all owned allocations released\n",peak_bytes,sizeof *model);
    free(model);free(text);
    puts("Large readonly snapshot: full text, Unicode ranges, immutable publication, retained lifetime and OOM rollback passed");return 0;
}
