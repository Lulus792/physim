#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
/* Tree-sitter uses the libc endian helpers, which glibc hides in strict
 * C/POSIX mode unless its default interfaces are requested before headers. */
#define _DEFAULT_SOURCE 1
#endif
#if !defined(_WIN32) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809L
#endif
#include "c_syntax.h"
#include "autosave.h"
#include <setjmp.h>
#include <string.h>

/* All runtime calls stay in this translation unit. Compile-time hooks avoid
 * ts_set_allocator's process-global mutation and the default abort-on-OOM path. */
static void *ps_c_malloc(size_t bytes);
static void *ps_c_calloc(size_t count,size_t bytes);
static void *ps_c_realloc(void *pointer,size_t bytes);
static void ps_c_free(void *pointer);
#define TREE_SITTER_HIDE_SYMBOLS
#define ts_malloc ps_c_malloc
#define ts_calloc ps_c_calloc
#define ts_realloc ps_c_realloc
#define ts_free ps_c_free
#include "../third_party/tree-sitter-0.27.1/lib/src/lib.c"
#undef ts_malloc
#undef ts_calloc
#undef ts_realloc
#undef ts_free
const TSLanguage *tree_sitter_c(void);

typedef union ps_c_block ps_c_block;
typedef struct {
    ps_allocator allocator;
    ps_c_limits limits;
    ps_c_block *blocks;
    const char *source;
    size_t bytes,live,peak,allocations,checks;
    ps_result error;
    jmp_buf escape;
} ps_c_domain;
union ps_c_block {
    ps_memory_alignment alignment;
    struct {ps_c_block *previous,*next;ps_c_domain *owner;size_t bytes;} data;
};
#ifdef _MSC_VER
static __declspec(thread) ps_c_domain *ps_c_current;
#else
static _Thread_local ps_c_domain *ps_c_current;
#endif
static void ps_c_fail(ps_result error){
    ps_c_current->error=error;longjmp(ps_c_current->escape,1);
}
static void *ps_c_malloc(size_t bytes){
    ps_c_domain *d=ps_c_current;if(!bytes)bytes=1;
    if(bytes>SIZE_MAX-sizeof(ps_c_block))ps_c_fail(PS_LIMIT);
    size_t total=sizeof(ps_c_block)+bytes;
    if(total>d->limits.heap_bytes-d->live)ps_c_fail(PS_LIMIT);
    ps_c_block *block=NULL;ps_result r=ps_memory_allocate(d->allocator,total,(void**)&block);
    if(r!=PS_OK)ps_c_fail(r);
    memset(&block->data,0,sizeof block->data);
    block->data.next=d->blocks;block->data.owner=d;block->data.bytes=bytes;
    if(d->blocks)d->blocks->data.previous=block;
    d->blocks=block;d->live+=total;d->allocations++;if(d->live>d->peak)d->peak=d->live;
    return block+1;
}
static void ps_c_free(void *pointer){
    if(!pointer)return;
    ps_c_block *block=(ps_c_block*)pointer-1;ps_c_domain *d=block->data.owner;
    if(block->data.previous)block->data.previous->data.next=block->data.next;
    else d->blocks=block->data.next;
    if(block->data.next)block->data.next->data.previous=block->data.previous;
    size_t total=sizeof *block+block->data.bytes;d->live-=total;
    ps_memory_free(d->allocator,block,total);
}
static void *ps_c_calloc(size_t count,size_t bytes){
    if(bytes && count>SIZE_MAX/bytes)ps_c_fail(PS_LIMIT);
    size_t total=count*bytes;void *p=ps_c_malloc(total);memset(p,0,total);return p;
}
static void *ps_c_realloc(void *pointer,size_t bytes){
    if(!pointer)return ps_c_malloc(bytes);
    if(!bytes){ps_c_free(pointer);return NULL;}
    ps_c_block *block=(ps_c_block*)pointer-1;size_t old=block->data.bytes;
    void *next=ps_c_malloc(bytes);memcpy(next,pointer,old<bytes?old:bytes);ps_c_free(pointer);return next;
}
static const char *ps_c_read(void *user,uint32_t at,TSPoint point,uint32_t *bytes){
    (void)point;ps_c_domain *d=user;
    if(at>=d->bytes){*bytes=0;return "";}
    *bytes=(uint32_t)(d->bytes-at);return d->source+at;
}
static bool ps_c_progress(TSParseState *state){
    ps_c_domain *d=state->payload;return ++d->checks>d->limits.progress_checks;
}
ps_c_syntax_result ps_c_syntax(const char *source,size_t bytes,ps_c_node *nodes,size_t capacity,
                             ps_allocator allocator,ps_c_limits limits){
    ps_c_syntax_result result={.status=PS_INVALID};
    if((!source && bytes) || (!nodes && capacity))return result;
    if(bytes>PS_SOURCE_MAX_BYTES){result.status=PS_LIMIT;return result;}
    if(bytes && memchr(source,0,bytes))return result;
    if(!allocator.allocate && !allocator.deallocate)allocator=ps_allocator_default();
    if(!ps_allocator_valid(allocator))return result;
    if(!limits.heap_bytes)limits.heap_bytes=32u*1024u*1024u;
    if(!limits.progress_checks)limits.progress_checks=10000;
    if(limits.heap_bytes<sizeof(ps_c_domain)){result.status=PS_LIMIT;return result;}
    ps_c_domain *d=NULL;result.status=ps_memory_zero(allocator,1,sizeof *d,(void**)&d);if(result.status!=PS_OK)return result;
    d->allocator=allocator;d->limits=limits;d->source=source?source:"";d->bytes=bytes;
    d->live=d->peak=sizeof *d;d->allocations=1;
    ps_c_domain *previous=ps_c_current;ps_c_current=d;
    /* Mutable state lives on the heap; no indeterminate modified automatic is
     * inspected after longjmp. All library allocations are linked before use. */
    if(!setjmp(d->escape)){
        TSParser *parser=ts_parser_new();
        if(!ts_parser_set_language(parser,tree_sitter_c()))ps_c_fail(PS_VERSION);
        TSInput input={.payload=d,.read=ps_c_read,.encoding=TSInputEncodingUTF8};
        TSParseOptions options={.payload=d,.progress_callback=ps_c_progress};
        TSTree *tree=ts_parser_parse_with_options(parser,NULL,input,options);
        if(!tree)ps_c_fail(PS_LIMIT);
        TSNode root=ts_tree_root_node(tree);bool has_error=ts_node_has_error(root);
        TSTreeCursor cursor=ts_tree_cursor_new(root);size_t parent=SIZE_MAX,count=0;
        for(;;){
            if(count>=capacity)ps_c_fail(PS_LIMIT);
            TSNode node=ts_tree_cursor_current_node(&cursor);TSPoint start=ts_node_start_point(node);
            size_t at=count++;
            nodes[at]=(ps_c_node){.type=ts_node_type(node),.field=ts_tree_cursor_current_field_name(&cursor),
                .start=ts_node_start_byte(node),.end=ts_node_end_byte(node),.line=(size_t)start.row+1,.column=(size_t)start.column+1,
                .parent=parent,.named=ts_node_is_named(node),.missing=ts_node_is_missing(node),.error=ts_node_is_error(node)};
            if(ts_tree_cursor_goto_first_child(&cursor)){parent=at;continue;}
            bool finished=false;
            while(!ts_tree_cursor_goto_next_sibling(&cursor)){
                if(!ts_tree_cursor_goto_parent(&cursor)){finished=true;break;}
                parent=nodes[parent].parent;
            }
            if(finished)break;
        }
        ts_tree_cursor_delete(&cursor);ts_tree_delete(tree);ts_parser_delete(parser);
        result=(ps_c_syntax_result){.status=PS_OK,.count=count,.has_error=has_error};
    }else result=(ps_c_syntax_result){.status=d->error};
    result.heap_peak=d->peak;result.allocations=d->allocations;result.progress_checks=d->checks;
    while(d->blocks)ps_c_free(d->blocks+1);
    ps_c_current=previous;ps_memory_free(allocator,d,sizeof *d);return result;
}
