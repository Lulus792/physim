/* Linux-only isolated production pair-query profile. Link ui_backend.c with
 * --gc-sections; no SDL window or GPU is created. Output float bytes separately
 * from seven fixed query batches so before/after equality can be checked. */
#define _POSIX_C_SOURCE 200809L
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
static void *allocate(nk_handle user,void *old,nk_size size) {(void)user;(void)old;return malloc(size);}
static void release(nk_handle user,void *data) {(void)user;free(data);}
static double now(void) {struct timespec t;if(clock_gettime(CLOCK_MONOTONIC,&t))exit(9);return t.tv_sec+t.tv_nsec*1e-9;}
int main(int argc,char **argv) {
 if(argc!=3)return 2;
 FILE *file=fopen(argv[1],"rb");if(!file)return 3;
 if(fseek(file,0,SEEK_END))return 4;long length=ftell(file);
 if(length<12 || length>16*1024*1024 || fseek(file,0,SEEK_SET))return 4;
 unsigned char *bytes=malloc((size_t)length);if(!bytes)return 5;
 if(fread(bytes,1,(size_t)length,file)!=(size_t)length)return 6;fclose(file);
 struct nk_font_config cfg=nk_font_config(32);cfg.ttf_blob=bytes;cfg.ttf_size=(nk_size)length;
 struct nk_font font={0};font.config=&cfg;float em=ps_ui_font_em_ratio(&font);
 struct nk_allocator allocator={nk_handle_id(0),allocate,release};
 ps_ui_font_pairs *pairs=ps_ui_font_pairs_create(&font,&allocator);free(bytes);if(!pairs)return 7;
 const char *text="Pendel und Vergleich von Integratoren";
 const float heights[]={16,26.4f*em,41.6f*em,64};float samples[4*36];unsigned n=0;
 for(unsigned s=0;s<4;s++)for(unsigned i=0;i<36;i++)samples[n++]=ps_ui_font_pair_advance(pairs,heights[s],text[i],text[i+1]);
 file=fopen(argv[2],"wb");if(!file)return 8;
 if(fwrite(samples,sizeof *samples,n,file)!=n || fclose(file))return 8;
 volatile double sink=0;
 for(unsigned batch=0;batch<7;batch++) {
  double begin=now();
  for(unsigned repeat=0;repeat<1000;repeat++)for(unsigned s=0;s<4;s++)for(unsigned i=0;i<36;i++)
   sink+=ps_ui_font_pair_advance(pairs,heights[s],text[i],text[i+1]);
  printf("PAIR PROFILE: batch=%u seconds=%.9f calls=144000 sum=%.9f\n",batch,now()-begin,sink);
 }
 ps_ui_font_pairs_destroy(pairs);return 0;
}
