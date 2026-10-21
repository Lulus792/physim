/* Same production encoder and pixels, only DEFLATE level differs. This manual
 * profile is not an automatic unit test or a general application benchmark. */
#include "../app/png.c"
#include "platform.h"
static unsigned little32(const unsigned char *p) {
    return p[0]|((unsigned)p[1]<<8)|((unsigned)p[2]<<16)|((unsigned)p[3]<<24);
}
int main(int argc,char **argv) {
    if(argc!=3)return 2;
    FILE *input=fopen(argv[1],"rb");if(!input)return 1;
    unsigned char header[16];bool valid=fread(header,1,16,input)==16 && !memcmp(header,"PSRGB1\0",8);
    unsigned width=valid?little32(header+8):0,height=valid?little32(header+12):0;
    if(!width || !height || width>4800 || height>3400){fclose(input);return 1;}
    size_t bytes=(size_t)width*height*3;unsigned char *rgb=malloc(bytes);
    valid=rgb && fread(rgb,1,bytes,input)==bytes && fgetc(input)==EOF;
    fclose(input);if(!valid){free(rgb);return 1;}
    printf("PNG INPUT: width=%u height=%u bytes=%zu crc=%08x\n",width,height,bytes,ps_crc32(rgb,bytes));
    for(unsigned pair=0;pair<3;pair++)for(unsigned step=0;step<2;step++) {
        int level=(step^(pair&1))?1:6;char path[4096];
        int n=snprintf(path,sizeof path,"%s-level%d-%u.png",argv[2],level,pair);
        if(n<0 || (size_t)n>=sizeof path){free(rgb);return 1;}
        double started=ps_clock();ps_result result=write_png(path,rgb,width,height,(size_t)width*3,level);
        double elapsed=ps_clock()-started;
        if(result!=PS_OK){free(rgb);return 1;}
        FILE *file=fopen(path,"rb");if(!file){free(rgb);return 1;}
        bool read=fseek(file,0,SEEK_END)==0;long length=read?ftell(file):-1;fclose(file);
        if(length<=0){free(rgb);return 1;}
        printf("PNG LEVEL: pair=%u level=%d seconds=%.9f bytes=%ld\n",pair,level,elapsed,length);
    }
    free(rgb);return 0;
}
