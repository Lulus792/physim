/* Real GL ownership and retained text commands across font additions. */
#include "ui.h"
#include <SDL3/SDL_opengl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Font bank %d: %s (%s)\n",__LINE__,#x,SDL_GetError());return 1;}}while(0)
typedef struct {size_t live;bool fail;} memory_state;
static void *allocate(nk_handle user,void *old,nk_size size) {
    (void)old;memory_state *m=user.ptr;if(m->fail)return NULL;
    void *p=malloc(size);if(p)m->live++;return p;
}
static void release(nk_handle user,void *p) {
    if(p){memory_state *m=user.ptr;m->live--;free(p);}
}
static struct nk_font *candidate(struct nk_font_atlas *atlas,memory_state *memory,float size) {
    struct nk_allocator allocator={nk_handle_ptr(memory),allocate,release};
    nk_font_atlas_init(atlas,&allocator);nk_font_atlas_begin(atlas);
    static const nk_rune ascii[]={32,126,0};
    struct nk_font_config config=nk_font_config(size);config.range=ascii;
    return nk_font_atlas_add_default(atlas,size,&config);
}
static bool text_begin(struct nk_context *ui) {
    nk_input_begin(ui);nk_input_end(ui);
    if(!nk_begin(ui,"font bank",nk_rect(10,10,280,100),NK_WINDOW_NO_SCROLLBAR))return false;
    nk_layout_row_dynamic(ui,44,1);nk_label(ui,"Retained AaBb 0123",NK_TEXT_LEFT);return true;
}
static unsigned char *capture(ps_graphics *g,const char *directory,const char *name,size_t *length) {
    char path[4096];if(snprintf(path,sizeof path,"%s/%s.bmp",directory,name)>=(int)sizeof path)return NULL;
    if(!ps_graphics_capture(g,path))return NULL;
    FILE *f=fopen(path,"rb");if(!f)return NULL;
    if(fseek(f,0,SEEK_END)){fclose(f);return NULL;}
    long count=ftell(f);if(count<54 || count>4*1024*1024 || fseek(f,0,SEEK_SET)){fclose(f);return NULL;}
    unsigned char *bytes=malloc((size_t)count);
    if(!bytes || fread(bytes,1,(size_t)count,f)!=(size_t)count){free(bytes);fclose(f);return NULL;}
    fclose(f);*length=(size_t)count;return bytes;
}
static unsigned le32(const unsigned char *p) {return p[0]|((unsigned)p[1]<<8)|((unsigned)p[2]<<16)|((unsigned)p[3]<<24);}
static size_t ink(const unsigned char *bmp,size_t length) {
    if(length<54 || memcmp(bmp,"BM",2))return 0;
    unsigned offset=le32(bmp+10),width=le32(bmp+18),height=le32(bmp+22),bits=bmp[28]|((unsigned)bmp[29]<<8);
    if((bits!=24 && bits!=32) || !width || width>4096 || !height || height>4096)return 0;
    size_t stride=((size_t)width*(bits/8)+3)&~(size_t)3,total=0;
    if(offset>length || stride*height>length-offset)return 0;
    for(unsigned y=0;y<height;y++)for(unsigned x=0;x<width;x++) {
        const unsigned char *p=bmp+offset+y*stride+x*(bits/8);
        if(p[0]>120 && p[1]>120 && p[2]>120)total++;
    }
    return total;
}
int main(int argc,char **argv) {
    CHECK(argc==2 && SDL_Init(SDL_INIT_VIDEO));
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,4);SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_Window *window=SDL_CreateWindow("Physim font bank probe",320,160,SDL_WINDOW_OPENGL);CHECK(window);
    ps_graphics *g=ps_graphics_create(window);CHECK(g);
    struct nk_context *ui=nk_sdl_init(window,g);CHECK(ui);
    GLboolean (APIENTRY *is_texture)(GLuint)=(GLboolean (APIENTRY *)(GLuint))SDL_GL_GetProcAddress("glIsTexture");CHECK(is_texture);
    void (APIENTRY *enable)(GLenum)=(void (APIENTRY *)(GLenum))SDL_GL_GetProcAddress("glEnable");CHECK(enable);
    void (APIENTRY *get_integer)(GLenum,GLint *)=(void (APIENTRY *)(GLenum,GLint *))SDL_GL_GetProcAddress("glGetIntegerv");CHECK(get_integer);
    struct nk_font_atlas *base=nk_sdl_font_stash_begin(ui);
    struct nk_font *old=nk_font_atlas_add_default(base,16,NULL);CHECK(old && nk_sdl_font_stash_end(ui));
    nk_style_set_font(ui,&old->handle);unsigned original=(unsigned)old->texture.id;CHECK(is_texture(original));
    CHECK(text_begin(ui));nk_end(ui);CHECK(nk_sdl_render(ui));
    size_t first_length=0,second_length=0;unsigned char *first=capture(g,argv[1],"font-before",&first_length);
    CHECK(first && ink(first,first_length)>30);
    memory_state memory={0};struct nk_font_atlas extra;
    struct nk_font *added=candidate(&extra,&memory,32);CHECK(added);
    CHECK(!nk_sdl_font_stash_append(ui,&extra,NAN) && extra.fonts==added);
    CHECK(!nk_sdl_font_stash_append(ui,&extra,2) && extra.fonts==added);
    memory.fail=true;CHECK(!nk_sdl_font_stash_append(ui,&extra,1));memory.fail=false;
    CHECK(extra.fonts==added && ui->style.font==&old->handle && is_texture(original));
    enable(0); /* Inject a GL error into the upload's existing failure path. */
    CHECK(!nk_sdl_font_stash_append(ui,&extra,1) && extra.pixel && extra.glyphs);
    GLint binding=-1;get_integer(GL_TEXTURE_BINDING_2D,&binding);CHECK(binding==0);
    CHECK(ui->style.font==&old->handle && is_texture(original));
    size_t failed_live=memory.live;
    CHECK(!nk_sdl_font_stash_append(ui,&extra,1) && memory.live==failed_live);
    nk_font_atlas_clear(&extra);CHECK(memory.live==0);
    added=candidate(&extra,&memory,32);CHECK(added);
    CHECK(text_begin(ui)); /* Already recorded commands still reference old. */
    CHECK(nk_sdl_font_stash_append(ui,&extra,1) && !extra.fonts && !extra.temporary.alloc);
    CHECK(ui->style.font==&old->handle && ps_ui_font_has_glyph(&old->handle,'A'));
    CHECK(added->handle.height==32 && ps_ui_font_has_glyph(&added->handle,'B'));
    unsigned textures[PS_UI_FONT_APPEND_LIMIT];textures[0]=(unsigned)added->texture.id;
    CHECK(is_texture(original) && is_texture(textures[0]));
    nk_end(ui);CHECK(nk_sdl_render(ui));
    unsigned char *second=capture(g,argv[1],"font-after",&second_length);
    CHECK(second && first_length==second_length && !memcmp(first,second,first_length));
    free(first);free(second);
    nk_style_set_font(ui,&added->handle);CHECK(text_begin(ui));nk_end(ui);CHECK(nk_sdl_render(ui));
    second=capture(g,argv[1],"font-added",&second_length);CHECK(second && ink(second,second_length)>100);free(second);
    for(unsigned i=1;i<PS_UI_FONT_APPEND_LIMIT;i++) {
        struct nk_font *f=candidate(&extra,&memory,16);CHECK(f && nk_sdl_font_stash_append(ui,&extra,1));
        textures[i]=(unsigned)f->texture.id;CHECK(is_texture(textures[i]));
    }
    CHECK(candidate(&extra,&memory,16));size_t live=memory.live;
    CHECK(!nk_sdl_font_stash_append(ui,&extra,1) && extra.fonts && !extra.glyphs && memory.live==live);
    nk_font_atlas_clear(&extra);CHECK(memory.live>0);
    memory_state replacement_memory={0};struct nk_font_atlas replacement;
    struct nk_font *next=candidate(&replacement,&replacement_memory,32);CHECK(next);
    replacement_memory.fail=true;CHECK(!nk_sdl_font_stash_replace(ui,&replacement,2,next));replacement_memory.fail=false;
    CHECK(ui->style.font==&added->handle && is_texture(original) && is_texture(textures[0]));
    CHECK(nk_sdl_font_stash_replace(ui,&replacement,2,next) && memory.live==0);
    CHECK(next->handle.height==16 && next->info.height==32 && !is_texture(original));
    for(unsigned i=0;i<PS_UI_FONT_APPEND_LIMIT;i++)CHECK(!is_texture(textures[i]));
    struct nk_font *last=candidate(&extra,&memory,36);CHECK(last && nk_sdl_font_stash_append(ui,&extra,2));
    CHECK(last->handle.height==18 && last->info.height==36);
    unsigned final_texture=(unsigned)last->texture.id,replacement_texture=(unsigned)next->texture.id;
    CHECK(text_begin(ui));nk_end(ui);CHECK(nk_sdl_render(ui));
    nk_sdl_shutdown(ui);CHECK(memory.live==0 && replacement_memory.live==0);
    CHECK(!is_texture(final_texture) && !is_texture(replacement_texture));
    ps_graphics_destroy(g);SDL_DestroyWindow(window);SDL_Quit();
    puts("Font bank: retained command pixels, append/retry, capacity, atomic density replacement and CPU/GPU teardown passed");return 0;
}
