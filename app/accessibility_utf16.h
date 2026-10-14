#ifndef PS_APP_ACCESSIBILITY_UTF16_H
#define PS_APP_ACCESSIBILITY_UTF16_H
#include <stdint.h>
#include "accessibility_text.h"

/* AppKit/NSString ranges count UTF-16 units. Own the UTF-8 and byte map;
 * interior surrogate positions map to their glyph but cannot split a range. */
typedef struct {char *text;size_t *bytes;size_t count;uint64_t version;} ps_a11y_utf16_index;
static inline void ps_a11y_utf16_clear(ps_a11y_utf16_index *index) {
    free(index->text);free(index->bytes);memset(index,0,sizeof *index);
}
static inline bool ps_a11y_utf16_build(ps_a11y_utf16_index *index,const char *text,size_t length,uint64_t version) {
    ps_a11y_text_index scalar={0};
    if(!ps_a11y_text_index_build(&scalar,text,length,version))return false;
    size_t count=scalar.count;for(size_t i=0;i<scalar.count;i++)if(scalar.characters[i]>0xffff)count++;
    ps_a11y_utf16_index next={.text=scalar.text,.count=count,.version=version};
    next.bytes=malloc((count+1)*sizeof *next.bytes);
    if(!next.bytes){ps_a11y_text_index_clear(&scalar);return false;}
    size_t unit=0;for(size_t i=0;i<scalar.count;i++) {
        next.bytes[unit++]=scalar.bytes[i];if(scalar.characters[i]>0xffff)next.bytes[unit++]=scalar.bytes[i];
    }
    next.bytes[unit]=length;scalar.text=NULL;ps_a11y_text_index_clear(&scalar);
    ps_a11y_utf16_clear(index);*index=next;return true;
}
static inline bool ps_a11y_utf16_boundary(const ps_a11y_utf16_index *index,size_t unit) {
    return index && index->bytes && unit<=index->count && (!unit || index->bytes[unit]!=index->bytes[unit-1]);
}
static inline bool ps_a11y_utf16_range(const ps_a11y_utf16_index *index,size_t start,size_t length,size_t *begin,size_t *end) {
    if(!index || !index->bytes || start>index->count || length>index->count-start || !ps_a11y_utf16_boundary(index,start) ||
       !ps_a11y_utf16_boundary(index,start+length))return false;
    *begin=index->bytes[start];*end=index->bytes[start+length];return true;
}
static inline bool ps_a11y_utf16_offset(const ps_a11y_utf16_index *index,size_t byte,size_t *unit) {
    if(!index || !index->bytes)return false;
    size_t lo=0,hi=index->count+1;
    while(lo<hi){size_t mid=lo+(hi-lo)/2;if(index->bytes[mid]<byte)lo=mid+1;else hi=mid;}
    if(lo>index->count || index->bytes[lo]!=byte)return false;
    *unit=lo;return true;
}
#endif
