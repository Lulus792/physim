#include "graphic_output.h"
#include "png.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif
void ps_graphic_bytes_free(ps_graphic_bytes *b){if(b){free(b->data);*b=(ps_graphic_bytes){0};}}
static void little32(unsigned char *p,uint32_t x){for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(x>>(8*i));}
ps_result ps_graphic_image_bmp(const ps_graphics_image *image,ps_graphic_bytes *out) {
    unsigned w,h;size_t stride;const unsigned char *rgb;
    if(!out || ps_graphics_image_read(image,&w,&h,&stride,&rgb)!=PS_OK)return PS_INVALID;
    size_t row=((size_t)w*3+3)&~(size_t)3;
    if(h>(UINT32_MAX-54)/row)return PS_LIMIT;
    size_t size=54+row*h;unsigned char *bytes=calloc(1,size);if(!bytes)return PS_MEMORY;
    bytes[0]='B';bytes[1]='M';little32(bytes+2,(uint32_t)size);little32(bytes+10,54);
    little32(bytes+14,40);little32(bytes+18,w);little32(bytes+22,h);bytes[26]=1;bytes[28]=24;
    little32(bytes+34,(uint32_t)(row*h));
    for(unsigned y=0;y<h;y++)for(unsigned x=0;x<w;x++) {
        unsigned char *p=bytes+54+(h-1-y)*row+x*3;const unsigned char *q=rgb+y*stride+x*3;
        p[0]=q[2];p[1]=q[1];p[2]=q[0];
    }
    *out=(ps_graphic_bytes){bytes,size};return PS_OK;
}
typedef struct {ps_graphic_bytes png,bmp;unsigned references;} clipboard_owner;
static const void *SDLCALL clipboard_data(void *user,const char *mime,size_t *size) {
    clipboard_owner *p=user;const ps_graphic_bytes *b=mime && !strcmp(mime,"image/png")?&p->png:
        mime && !strcmp(mime,"image/bmp")?&p->bmp:NULL;
    *size=b?b->size:0;return b?b->data:NULL;
}
static void SDLCALL clipboard_release(void *user) {
    clipboard_owner *p=user;if(--p->references)return;
    ps_graphic_bytes_free(&p->png);ps_graphic_bytes_free(&p->bmp);free(p);
}
ps_result ps_graphic_image_clipboard(const ps_graphics_image *image) {
    if(!SDL_GetCurrentVideoDriver())return PS_INVALID;
    unsigned w,h;size_t stride;const unsigned char *rgb;
    if(ps_graphics_image_read(image,&w,&h,&stride,&rgb)!=PS_OK)return PS_INVALID;
    clipboard_owner *p=calloc(1,sizeof *p);if(!p)return PS_MEMORY;p->references=1;
    ps_result result=ps_png_encode(rgb,w,h,stride,&p->png.data,&p->png.size);
    if(result==PS_OK)result=ps_graphic_image_bmp(image,&p->bmp);
    if(result==PS_OK) {
        const char *types[]={"image/png","image/bmp"};p->references++;
        if(!SDL_SetClipboardData(clipboard_data,clipboard_release,p,types,2)) {
            /* Valid SDL 3.2 providers are registered before platform errors.
             * Clear releases a retained provider; our ref protects both calls. */
            SDL_ClearClipboardData();result=PS_IO;
        }
    }
    clipboard_release(p);return result;
}
struct ps_graphic_save {
    SDL_Mutex *mutex;SDL_AtomicInt references;SDL_Thread *thread;
    ps_graphic_bytes bytes;char *path;bool ready,closed,shown;
    ps_graphic_save_state state;
};
static void save_release(ps_graphic_save *s) {
    if(!SDL_AtomicDecRef(&s->references))return;
    SDL_DestroyMutex(s->mutex);ps_graphic_bytes_free(&s->bytes);SDL_free(s->path);free(s);
}
ps_graphic_save *ps_graphic_save_create(ps_graphic_bytes *bytes) {
    if(!bytes || !bytes->data || !bytes->size || bytes->size>64u*1024u*1024u)return NULL;
    ps_graphic_save *s=calloc(1,sizeof *s);if(!s)return NULL;
    s->mutex=SDL_CreateMutex();if(!s->mutex){free(s);return NULL;}
    SDL_SetAtomicInt(&s->references,1);s->bytes=*bytes;*bytes=(ps_graphic_bytes){0};return s;
}
void ps_graphic_save_resolve(ps_graphic_save *s,const char *path,bool error) {
    SDL_LockMutex(s->mutex);
    if(!s->closed && !s->ready) {
        s->path=path && *path?SDL_strdup(path):NULL;
        s->state=error || (path && *path && !s->path)?PS_GRAPHIC_SAVE_ERROR:
            s->path?PS_GRAPHIC_SAVE_WAITING:PS_GRAPHIC_SAVE_CANCELLED;
        s->ready=true;
    }
    SDL_UnlockMutex(s->mutex);
}
static void SDLCALL save_callback(void *user,const char *const *files,int filter) {
    (void)filter;ps_graphic_save *s=user;
    ps_graphic_save_resolve(s,files?files[0]:NULL,!files);save_release(s);
}
void ps_graphic_save_show(ps_graphic_save *s,SDL_Window *parent,const char *path,bool svg) {
    if(!s || s->shown || s->ready)return;
    s->shown=true;SDL_AtomicIncRef(&s->references);
    static const SDL_DialogFileFilter png_filter={"PNG-Grafik","png"},svg_filter={"SVG-Grafik","svg"};
    SDL_ShowSaveFileDialog(save_callback,s,parent,svg?&svg_filter:&png_filter,1,path);
}
static FILE *exclusive(const char *path) {
#ifdef _WIN32
    int count=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,path,-1,NULL,0);if(!count)return NULL;
    wchar_t *wide=malloc((size_t)count*sizeof *wide);if(!wide)return NULL;
    MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,path,-1,wide,count);
    FILE *f=_wfopen(wide,L"wbx");free(wide);return f;
#else
    return fopen(path,"wbx");
#endif
}
static int SDLCALL write_output(void *user) {
    ps_graphic_save *s=user;size_t length=strlen(s->path);bool ok=false;
    char *temporary=length<=8192?malloc(length+80):NULL;FILE *file=NULL;
    for(unsigned attempt=0;temporary && !file && attempt<16;attempt++) {
        snprintf(temporary,length+80,"%s.physim-%llu-%u.tmp",s->path,(unsigned long long)SDL_GetTicksNS(),attempt);
        file=exclusive(temporary);
    }
    if(file) {
        ok=fwrite(s->bytes.data,1,s->bytes.size,file)==s->bytes.size;
        if(fflush(file))ok=false;
        if(fclose(file))ok=false;
        if(ok)ok=SDL_RenamePath(temporary,s->path);
        if(!ok)SDL_RemovePath(temporary); /* Exclusively created sibling only. */
    }
    free(temporary);SDL_LockMutex(s->mutex);s->state=ok?PS_GRAPHIC_SAVE_OK:PS_GRAPHIC_SAVE_ERROR;
    SDL_UnlockMutex(s->mutex);return 0;
}
ps_graphic_save_state ps_graphic_save_poll(ps_graphic_save *s) {
    SDL_LockMutex(s->mutex);
    if(s->ready && s->path && s->state==PS_GRAPHIC_SAVE_WAITING) {
        s->state=PS_GRAPHIC_SAVE_WRITING;s->thread=SDL_CreateThread(write_output,"graphic-save",s);
        if(!s->thread)s->state=PS_GRAPHIC_SAVE_ERROR;
    }
    ps_graphic_save_state state=s->state;SDL_UnlockMutex(s->mutex);return state;
}
const char *ps_graphic_save_path(ps_graphic_save *s) {return s?s->path:NULL;}
void ps_graphic_save_destroy(ps_graphic_save *s) {
    if(!s)return;
    SDL_LockMutex(s->mutex);s->closed=true;SDL_UnlockMutex(s->mutex);
    if(s->thread)SDL_WaitThread(s->thread,NULL);
    save_release(s);
}
