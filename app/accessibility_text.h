#ifndef PS_APP_ACCESSIBILITY_TEXT_H
#define PS_APP_ACCESSIBILITY_TEXT_H
#include "../src/text_validation.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* AT-SPI offsets count Unicode scalar values, not UTF-8 bytes or UTF-16 units.
 * This immutable index owns its text; replacing it is transactional. */
typedef struct {
    char *text;size_t *bytes;uint32_t *characters;size_t count;uint64_t version;
} ps_a11y_text_index;
static inline void ps_a11y_text_index_clear(ps_a11y_text_index *index) {
    free(index->text);free(index->bytes);free(index->characters);memset(index,0,sizeof *index);
}
static inline bool ps_a11y_text_index_build(ps_a11y_text_index *index,const char *text,size_t length,uint64_t version) {
    if(!index || !text || !version || length>INT_MAX || length>SIZE_MAX/sizeof(size_t)-1 ||
       length>SIZE_MAX/sizeof(uint32_t)-1 || !ps_text_valid(text,length+1,true) || strlen(text)!=length)return false;
    ps_a11y_text_index next={0};next.version=version;
    next.text=malloc(length+1);next.bytes=malloc((length+1)*sizeof *next.bytes);
    next.characters=malloc((length+1)*sizeof *next.characters);
    if(!next.text || !next.bytes || !next.characters){ps_a11y_text_index_clear(&next);return false;}
    memcpy(next.text,text,length+1);
    for(size_t at=0;at<length;) {
        next.bytes[next.count]=at;uint32_t c=(unsigned char)text[at++];unsigned extra=0;
        if(c>=0xf0){c&=7;extra=3;}else if(c>=0xe0){c&=15;extra=2;}else if(c>=0xc0){c&=31;extra=1;}
        while(extra--)c=(c<<6)|((unsigned char)text[at++]&63);
        next.characters[next.count++]=c;
    }
    next.bytes[next.count]=length;ps_a11y_text_index_clear(index);*index=next;return true;
}
static inline bool ps_a11y_text_byte_offset(const ps_a11y_text_index *index,size_t byte,int *offset) {
    size_t left=0,right=index->count+1;
    while(left<right){size_t mid=left+(right-left)/2;if(index->bytes[mid]<byte)left=mid+1;else right=mid;}
    if(left>index->count || index->bytes[left]!=byte)return false;
    *offset=(int)left;return true;
}
static inline bool ps_a11y_text_range(const ps_a11y_text_index *index,int start,int end,size_t *begin,size_t *finish) {
    if(end==-1)end=(int)index->count;
    if(start<0 || end<start || (size_t)end>index->count)return false;
    *begin=index->bytes[start];*finish=index->bytes[end];return true;
}
static inline bool ps_a11y_text_space(uint32_t c) {
    return c==' ' || c=='\t' || c=='\r' || c=='\n' || c==0xa0 || c==0x1680 ||
        (c>=0x2000 && c<=0x200a) || c==0x2028 || c==0x2029 || c==0x202f || c==0x205f || c==0x3000;
}
static inline bool ps_a11y_text_word(uint32_t c) {
    return !ps_a11y_text_space(c) && (c>=128 || (c>='a' && c<='z') ||
        (c>='A' && c<='Z') || (c>='0' && c<='9') || c=='_');
}
/* Logical newline boundaries are available independently of viewport geometry.
 * Word/sentence segmentation is deliberately bounded to the bundled alphabet;
 * complex-script word breaking requires a language-aware text shaper. */
static inline bool ps_a11y_text_boundary(const ps_a11y_text_index *index,size_t at,unsigned type) {
    if(!at || at==index->count)return true;
    uint32_t previous=index->characters[at-1],current=index->characters[at];
    if(type==0)return true;
    if(type==1)return ps_a11y_text_word(current) && !ps_a11y_text_word(previous);
    if(type==2)return ps_a11y_text_word(previous) && !ps_a11y_text_word(current);
    if(type==5)return previous=='\n' || previous==0x2028 || previous==0x2029;
    if(type==6)return current=='\n' || current==0x2028 || current==0x2029;
    if(type==3) {
        if(ps_a11y_text_space(current))return false;
        size_t before=at;while(before && ps_a11y_text_space(index->characters[before-1]))before--;
        if(!before)return true;
        uint32_t c=index->characters[before-1];return c=='.' || c=='!' || c=='?' || previous=='\n';
    }
    return type==4 && (previous=='.' || previous=='!' || previous=='?' || previous=='\n');
}
static inline bool ps_a11y_text_span(const ps_a11y_text_index *index,int offset,unsigned type,int direction,int *start,int *end) {
    if(offset<0 || (size_t)offset>index->count || type>6 || direction<-1 || direction>1)return false;
    size_t begin=0,finish=index->count;
    for(size_t at=0;at<=index->count;at++)if(ps_a11y_text_boundary(index,at,type)) {
        if(at<=(size_t)offset)begin=at;
        else {finish=at;break;}
    }
    if(direction<0) {
        finish=begin;begin=0;
        for(size_t at=0;at<finish;at++)if(ps_a11y_text_boundary(index,at,type))begin=at;
    } else if(direction>0) {
        begin=finish;finish=index->count;
        for(size_t at=begin+1;at<=index->count;at++)if(ps_a11y_text_boundary(index,at,type)){finish=at;break;}
    }
    *start=(int)begin;*end=(int)finish;return true;
}
#endif
