#include "../app/accessibility_utf16.h"
#include <stdio.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"UTF-16 index line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void) {
 ps_a11y_utf16_index index={0};size_t begin,end,unit;
 const char *source="A θ🌍 é\n";
 CHECK(ps_a11y_utf16_build(&index,source,strlen(source),1) && index.count==9);
 CHECK(ps_a11y_utf16_range(&index,2,3,&begin,&end) && begin==2 && end==8);
 CHECK(!ps_a11y_utf16_range(&index,3,1,&begin,&end) && !ps_a11y_utf16_range(&index,4,1,&begin,&end));
 CHECK(ps_a11y_utf16_range(&index,3,2,&begin,&end) && begin==4 && end==8);
 CHECK(ps_a11y_utf16_offset(&index,4,&unit) && unit==3);
 CHECK(!ps_a11y_utf16_offset(&index,5,&unit));
 CHECK(!ps_a11y_utf16_range(&index,SIZE_MAX,1,&begin,&end) && !ps_a11y_utf16_range(&index,1,SIZE_MAX,&begin,&end));
 CHECK(ps_a11y_utf16_range(&index,9,0,&begin,&end) && begin==strlen(source));
 CHECK(!ps_a11y_utf16_build(&index,"\xf0\x80\x80\xaf",4,2) && index.version==1 && !strcmp(index.text,source));
 CHECK(ps_a11y_utf16_build(&index,"",0,2) && index.count==0 && ps_a11y_utf16_range(&index,0,0,&begin,&end));
 ps_a11y_utf16_clear(&index);ps_a11y_utf16_clear(&index);
 CHECK(!ps_a11y_utf16_range(&index,0,0,&begin,&end) && !ps_a11y_utf16_offset(&index,0,&unit));
 puts("UTF-16 text index: owned UTF-8, supplementary units, exact boundaries, overflow, failed replacement and cleanup passed");return 0;
}
