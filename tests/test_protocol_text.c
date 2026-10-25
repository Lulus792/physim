#include "protocol_text.h"
#include "text_validation.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Protocol text line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void) {
    const char valid[]="Model α\t☃\r\nA \xF0\x9F\x8C\x8DZ";
    for(size_t split=0;split<=strlen(valid);split++) {
        ps_protocol_text state={0};char output[256],combined[512];size_t n=0,total=0;
        CHECK(ps_protocol_text_feed(&state,valid,split,false,output,sizeof output,&n));
        CHECK(ps_text_valid(output,sizeof output,true));memcpy(combined,output,n);total+=n;
        CHECK(ps_protocol_text_feed(&state,valid+split,strlen(valid)-split,true,output,sizeof output,&n));
        CHECK(ps_text_valid(output,sizeof output,true));memcpy(combined+total,output,n+1);
        CHECK(!strcmp(combined,valid) && !state.used);
    }
    const unsigned char bad[]={0xe4,'\\',0,0x1b,0x7f,0x80,0xc0,0xaf,0xed,0xa0,0x80,0xf4,0x90,0x80,0x80,0xc2,0x85,0xe2,0x98};
    const char *expected="\\xE4\\\\x00\\x1B\\x7F\\x80\\xC0\\xAF\\xED\\xA0\\x80\\xF4\\x90\\x80\\x80\\xC2\\x85\\xE2\\x98";
    for(size_t split=0;split<=sizeof bad;split++) {
        ps_protocol_text state={0};char output[256],combined[512];size_t n=0,total=0;
        CHECK(ps_protocol_text_feed(&state,bad,split,false,output,sizeof output,&n));
        CHECK(ps_text_valid(output,sizeof output,true));memcpy(combined,output,n);total+=n;
        CHECK(ps_protocol_text_feed(&state,bad+split,sizeof bad-split,true,output,sizeof output,&n));
        CHECK(ps_text_valid(output,sizeof output,true));memcpy(combined+total,output,n+1);
        CHECK(!strcmp(combined,expected) && !state.used);
    }
    ps_protocol_text state={0};char output[256];size_t n=0;
    CHECK(ps_protocol_text_feed(&state,"\xe2",1,false,output,sizeof output,&n) && n==0 && state.used==1);
    ps_protocol_text previous=state;strcpy(output,"unchanged");n=123;
    CHECK(!ps_protocol_text_feed(&state,"x",1,true,output,4,&n));
    CHECK(!memcmp(&state,&previous,sizeof state) && !strcmp(output,"unchanged") && n==123);
    CHECK(ps_protocol_text_feed(&state,NULL,0,true,output,sizeof output,&n) && !strcmp(output,"\\xE2") && n==4 && !state.used);
    CHECK(!ps_protocol_text_feed(&state,NULL,1,true,output,sizeof output,&n));
    puts("Protocol text: every UTF-8 split, invalid/control bytes, incomplete flush and transactional capacity passed");return 0;
}
