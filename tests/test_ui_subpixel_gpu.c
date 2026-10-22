#include "ui.h"
#include <math.h>
#include <stdio.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Subpixel GPU %d: %s (%s)\n",__LINE__,#x,SDL_GetError());return 1;}}while(0)
static float width(nk_handle h,float size,const char *s,int n){(void)h;(void)size;(void)s;return (float)n;}
int main(int argc,char **argv) {
    CHECK(argc==2 && SDL_Init(SDL_INIT_VIDEO));
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_Window *window=SDL_CreateWindow("Physim Float GPU probe",160,160,SDL_WINDOW_OPENGL|SDL_WINDOW_HIDDEN);CHECK(window);
    ps_graphics *g=ps_graphics_create(window);CHECK(g);ps_graphics_background(g,0,0,0);
    const unsigned char white[]={255,255,255,255};unsigned texture=ps_graphics_texture(g,white,1,1);CHECK(texture);
    struct nk_draw_null_texture null_texture={nk_handle_id((int)texture),{.5f,.5f}};
    struct nk_user_font font={.height=10,.width=width};struct nk_context ui;CHECK(nk_init_default(&ui,&font));
    ui.style.window.padding=nk_vec2(0,0);ui.style.window.border=0;ui.style.window.fixed_background=nk_style_item_hide();
    CHECK(nk_begin(&ui,"float GPU",nk_rect(0,0,160,160),NK_WINDOW_NO_SCROLLBAR));
    struct nk_command_buffer *c=nk_window_get_canvas(&ui);nk_push_scissor(c,nk_rect(0,0,160,160));
    for(unsigned row=0;row<8;row++) {
        float y=20+row*14+row*.125f;
        nk_stroke_line(c,10.25f,y,130.5f,y,1.5f,nk_rgb(255,255,255));
    }
    nk_stroke_line(c,-.25f,145.75f,40.5f,145.75f,2.5f,nk_rgb(255,255,255));nk_end(&ui);
    ps_graphics_image *image=NULL;CHECK(ps_graphics_ui_image(g,&ui,&null_texture,160,160,ps_allocator_default(),&image)==PS_OK);
    unsigned w,h;size_t stride;const unsigned char *pixels;CHECK(ps_graphics_image_read(image,&w,&h,&stride,&pixels)==PS_OK && w==160 && h==160);
    double maximum=0;unsigned samples=0;
    /* A horizontal AA stroke has a unit-width linear fringe and an opaque
     * core of thickness-1. Sample far from its endpoints; no converter vertices
     * or generated reference image participate in this numeric oracle. */
    for(unsigned row=0;row<9;row++) {
        double y=row<8?20+row*14+row*.125:145.75,thickness=row<8?1.5:2.5;
        for(int py=(int)floor(y)-3;py<=(int)ceil(y)+3;py++) {
            double alpha=fmax(0,fmin(1,(thickness+1)*.5-fabs(py+.5-y)));
            const unsigned char *p=pixels+(size_t)py*stride+20*3;
            double error=fabs(p[0]-255*alpha);maximum=fmax(maximum,error);samples++;
            CHECK(error<=1.1 && p[0]==p[1] && p[1]==p[2]);
        }
    }
    char path[4096];snprintf(path,sizeof path,"%s/float-strokes.png",argv[1]);CHECK(ps_graphics_image_png(image,path)==PS_OK);ps_graphics_image_destroy(image);nk_clear(&ui);
    CHECK(nk_begin(&ui,"float GPU",nk_rect(0,0,160,160),NK_WINDOW_NO_SCROLLBAR));
    c=nk_window_get_canvas(&ui);nk_push_scissor(c,nk_rect(.75f,2.25f,10.875f,20.125f));
    nk_fill_rect(c,nk_rect(0,0,100,100),0,nk_rgb(255,255,255));nk_end(&ui);
    image=NULL;CHECK(ps_graphics_ui_image(g,&ui,&null_texture,160,160,ps_allocator_default(),&image)==PS_OK);
    CHECK(ps_graphics_image_read(image,NULL,NULL,&stride,&pixels)==PS_OK);
    /* Hardware scissors conservatively enclose fractional edges; these are
     * integer clip pixels, not an antialiased fractional coverage mask. */
    for(unsigned y=0;y<160;y++)for(unsigned x=0;x<160;x++) {
        unsigned expected=x<12 && y>=2 && y<23?255:0;const unsigned char *p=pixels+y*stride+x*3;
        CHECK(p[0]==expected && p[1]==expected && p[2]==expected);
    }
    snprintf(path,sizeof path,"%s/float-scissor.png",argv[1]);CHECK(ps_graphics_image_png(image,path)==PS_OK);ps_graphics_image_destroy(image);
    printf("FLOAT GPU: %u stroke samples, max channel error %.6f; fractional clip enclosed at 12x21\n",samples,maximum);
    nk_free(&ui);ps_graphics_delete_texture(g,texture);ps_graphics_destroy(g);SDL_DestroyWindow(window);SDL_Quit();return 0;
}
