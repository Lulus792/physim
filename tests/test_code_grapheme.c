#include "../third_party/libunibreak-8.0/graphemebreak.h"
#include "../third_party/zlib-1.3.2/zlib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static size_t utf8(unsigned long c,unsigned char *out) {
    if(c<0x80){out[0]=(unsigned char)c;return 1;}
    if(c<0x800){out[0]=(unsigned char)(0xc0|(c>>6));out[1]=(unsigned char)(0x80|(c&63));return 2;}
    if(c>=0xd800 && c<=0xdfff)return 0;
    if(c<0x10000){out[0]=(unsigned char)(0xe0|(c>>12));out[1]=(unsigned char)(0x80|((c>>6)&63));out[2]=(unsigned char)(0x80|(c&63));return 3;}
    if(c>0x10ffff)return 0;
    out[0]=(unsigned char)(0xf0|(c>>18));out[1]=(unsigned char)(0x80|((c>>12)&63));out[2]=(unsigned char)(0x80|((c>>6)&63));out[3]=(unsigned char)(0x80|(c&63));return 4;
}
int main(int argc,char **argv) {
    if(argc!=2)return 2;
    FILE *file=fopen(argv[1],"rb");if(!file)return 2;
    if(fseek(file,0,SEEK_END)){fclose(file);return 2;}long packed_size=ftell(file);rewind(file);
    if(packed_size<=0 || packed_size>1024*1024){fclose(file);return 2;}
    unsigned char *packed=malloc((size_t)packed_size);char *data=malloc(1024*1024+1);
    if(!packed || !data){free(packed);free(data);fclose(file);return 2;}
    size_t got=fread(packed,1,(size_t)packed_size,file);int closed=fclose(file);
    if(got!=(size_t)packed_size || closed){free(packed);free(data);return 2;}
    z_stream z={0};z.next_in=packed;z.avail_in=(uInt)packed_size;z.next_out=(Bytef *)data;z.avail_out=1024*1024;
    if(inflateInit2(&z,31)!=Z_OK){free(packed);free(data);return 2;}
    int inflated=inflate(&z,Z_FINISH);size_t length=z.total_out;bool complete=inflated==Z_STREAM_END && !z.avail_in;
    int ended=inflateEnd(&z);free(packed);if(!complete || ended!=Z_OK){free(data);return 2;}data[length]=0;
    unsigned rows=0,number=0;char *line=data;
    while(*line) {
        char *next=strchr(line,'\n');if(next)*next++=0;
        number++;char *comment=strchr(line,'#');if(comment)*comment=0;
        unsigned char text[2048];char expected[2048],actual[2048];size_t size=0;
        memset(expected,GRAPHEMEBREAK_INSIDEACHAR,sizeof expected);
        char *at=line;
        while(*at) {
            if(*at==' ' || *at=='\t' || *at=='\r' || *at=='\n'){at++;continue;}
            if(!strncmp(at,"\xc3\xb7",2) || !strncmp(at,"\xc3\x97",2)) {
                if(size)expected[size-1]=(unsigned char)at[1]==0xb7?GRAPHEMEBREAK_BREAK:GRAPHEMEBREAK_NOBREAK;
                at+=2;continue;
            }
            char *end=NULL;unsigned long c=strtoul(at,&end,16);
            if(end==at || size>sizeof text-4){free(data);return 2;}
            size_t n=utf8(c,text+size);if(!n){free(data);return 2;}size+=n;at=end;
        }
        if(size) {
            set_graphemebreaks_utf8(text,size,NULL,actual);
            if(memcmp(actual,expected,size)) {fprintf(stderr,"Grapheme conformance line %u failed\n",number);free(data);return 1;}
            rows++;
        }
        if(!next)break;
        line=next;
    }
    free(data);if(rows<700)return 2;
    printf("Unicode grapheme conformance: %u complete upstream cases passed\n",rows);return 0;
}
