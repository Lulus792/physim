#include "protocol_text.h"
#include <stdint.h>
#include <string.h>

static void escaped(char *out,size_t *at,unsigned char byte) {
    const char hex[]="0123456789ABCDEF";
    out[(*at)++]='\\';out[(*at)++]='x';out[(*at)++]=hex[byte>>4];out[(*at)++]=hex[byte&15];
}
static unsigned sequence_size(unsigned char byte) {
    return byte>=0xc2 && byte<=0xdf?2:byte>=0xe0 && byte<=0xef?3:byte>=0xf0 && byte<=0xf4?4:0;
}
bool ps_protocol_text_feed(ps_protocol_text *state,const void *bytes,size_t size,
                           bool final,char *out,size_t capacity,size_t *written) {
    if(!state || !out || !written || (size && !bytes) || state->used>3 ||
       size>(SIZE_MAX-13)/4 || capacity<4*(size+3)+1)return false;
    const unsigned char *input=bytes;size_t at=0,i=0;
    while(i<size) {
        unsigned char byte=input[i];
        if(state->used) {
            if((byte&0xc0)!=0x80) {
                for(unsigned j=0;j<state->used;j++)escaped(out,&at,state->pending[j]);
                state->used=0;continue; /* Reconsider the current byte as a new character. */
            }
            state->pending[state->used++]=byte;i++;
            unsigned count=sequence_size(state->pending[0]);
            if(state->used<count)continue;
            uint32_t code=state->pending[0]&(count==2?31:count==3?15:7);
            for(unsigned j=1;j<count;j++)code=(code<<6)|(state->pending[j]&63);
            bool valid=code>=(count==2?0x80u:count==3?0x800u:0x10000u) && code<=0x10ffff &&
                !(code>=0xd800 && code<=0xdfff) && !(code>=0x7f && code<=0x9f);
            if(valid){memcpy(out+at,state->pending,count);at+=count;}
            else for(unsigned j=0;j<count;j++)escaped(out,&at,state->pending[j]);
            state->used=0;
        } else if(sequence_size(byte)) {state->pending[0]=byte;state->used=1;i++;}
        else {
            if((byte>=32 && byte<127) || byte=='\n' || byte=='\r' || byte=='\t')out[at++]=(char)byte;
            else escaped(out,&at,byte);
            i++;
        }
    }
    if(final){for(unsigned j=0;j<state->used;j++)escaped(out,&at,state->pending[j]);state->used=0;}
    out[at]=0;*written=at;return true;
}
