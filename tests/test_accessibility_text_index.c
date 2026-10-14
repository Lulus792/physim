#include "../app/accessibility_text.h"
#include <assert.h>
#include <stdio.h>
int main(void) {
    ps_a11y_text_index index={0};const char *source="A θ🌍 é. Next!\nLast\n";
    assert(ps_a11y_text_index_build(&index,source,strlen(source),1));
    assert(index.count==20 && index.characters[2]==0x3b8 && index.characters[3]==0x1f30d && index.characters[6]==0x301);
    size_t begin,end;int offset,start,finish;
    assert(ps_a11y_text_range(&index,2,4,&begin,&end));assert(end-begin==6 && !memcmp(index.text+begin,"θ🌍",6));
    assert(ps_a11y_text_byte_offset(&index,begin,&offset) && offset==2);
    assert(!ps_a11y_text_byte_offset(&index,begin+1,&offset));
    assert(ps_a11y_text_range(&index,0,-1,&begin,&end) && end==strlen(source));
    assert(!ps_a11y_text_range(&index,-1,1,&begin,&end) && !ps_a11y_text_range(&index,1,0,&begin,&end));
    assert(!ps_a11y_text_range(&index,0,21,&begin,&end));
    assert(ps_a11y_text_span(&index,3,0,0,&start,&finish) && start==3 && finish==4);
    assert(ps_a11y_text_span(&index,2,1,0,&start,&finish) && start==2 && finish==5);
    assert(ps_a11y_text_span(&index,9,3,0,&start,&finish) && start==9 && finish==15);
    assert(ps_a11y_text_span(&index,3,5,0,&start,&finish) && start==0 && finish==15);
    assert(ps_a11y_text_span(&index,3,5,1,&start,&finish) && start==15 && finish==20);
    assert(ps_a11y_text_span(&index,0,5,-1,&start,&finish) && start==0 && finish==0);
    assert(!ps_a11y_text_index_build(&index,"\xc0\xaf",2,2));assert(index.version==1 && !strcmp(index.text,source));
    assert(ps_a11y_text_index_build(&index,"",0,2) && index.count==0);
    assert(ps_a11y_text_span(&index,0,0,0,&start,&finish) && start==0 && finish==0);
    ps_a11y_text_index_clear(&index);assert(!index.text && !index.bytes && !index.characters);
    puts("AT-SPI text index: Unicode scalars, exact ranges, boundaries, replacement and malformed input passed");return 0;
}
