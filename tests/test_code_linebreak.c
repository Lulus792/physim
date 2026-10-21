#include "../third_party/libunibreak-8.0/linebreak.h"
#include "../third_party/zlib-1.3.2/zlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Linebreak conformance %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv) {
    CHECK(argc==2);FILE *file=fopen(argv[1],"rb");CHECK(file);CHECK(!fseek(file,0,SEEK_END));long size=ftell(file);CHECK(size>0 && size<2*1024*1024);rewind(file);
    unsigned char *packed=malloc((size_t)size);char *data=malloc(4*1024*1024+1);CHECK(packed && data);
    CHECK(fread(packed,1,(size_t)size,file)==(size_t)size);CHECK(!fclose(file));
    z_stream z={0};z.next_in=packed;z.avail_in=(uInt)size;z.next_out=(Bytef *)data;z.avail_out=4*1024*1024;
    CHECK(inflateInit2(&z,31)==Z_OK);CHECK(inflate(&z,Z_FINISH)==Z_STREAM_END && z.avail_in==0);size_t length=z.total_out;CHECK(inflateEnd(&z)==Z_OK);data[length]=0;free(packed);
    size_t count=0;char *line=data;
    while(*line) {
        char *end=strchr(line,'\n');if(!end)end=line+strlen(line);char saved=*end;*end=0;
        char *comment=strchr(line,'#');if(comment)*comment=0;
        utf32_t code[1024];char expected[1024],actual[1024];size_t n=0;char *at=line;
        while(*at==' ' || *at=='\t')at++;
        if(!strncmp(at,"\xc3\xb7",2) || !strncmp(at,"\xc3\x97",2)) {
            at+=2;
            while(*at) {
                char *after;unsigned long value=strtoul(at,&after,16);if(after==at)break;CHECK(n<1024 && value<=0x10ffff);code[n]=(utf32_t)value;at=after;
                while(*at==' ' || *at=='\t')at++;
                CHECK(!strncmp(at,"\xc3\xb7",2) || !strncmp(at,"\xc3\x97",2));expected[n++]=at[1]==(char)0xb7;at+=2;
            }
            /* Same strict CJ mode as upstream's Unicode conformance runner. */
            CHECK(n>0);set_linebreaks_utf32(code,n,"-strict",actual);
            for(size_t i=0;i+1<n;i++)if((actual[i]==LINEBREAK_ALLOWBREAK || actual[i]==LINEBREAK_MUSTBREAK)!=expected[i]) {
                fprintf(stderr,"UAX14 case=%zu boundary=%zu U+%04X/U+%04X actual=%d expected=%d\n",count,i,code[i],code[i+1],actual[i],expected[i]);free(data);return 1;
            }
            CHECK(expected[n-1]);++count;
        }
        *end=saved;if(!saved)break;line=end+1;
    }
    free(data);CHECK(count>10000);printf("Unicode17 UAX14 conformance: %zu complete cases, zero skipped\n",count);return 0;
}
