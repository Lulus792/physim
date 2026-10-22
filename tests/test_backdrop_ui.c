#include "ui.h"
#include <SDL3/SDL_opengl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Backdrop line %d: %s (%s)\n",__LINE__,#x,SDL_GetError());return 1;}}while(0)
static void draw(struct nk_context *ui,ps_graphics *g,ps_ui_backdrop *material,bool visible) {
    nk_input_begin(ui);nk_input_end(ui);
    ps_graphics_backdrop(g,visible?material:NULL);
    if(nk_begin(ui,"backdrop probe",nk_rect(0,0,320,200),NK_WINDOW_NO_SCROLLBAR)) {
        struct nk_command_buffer *c=nk_window_get_canvas(ui);
        nk_push_scissor(c,nk_rect(0,0,320,200));
        for(int x=0;x<320;x++)nk_fill_rect(c,nk_rect((float)x,0,1,200),0,x%2?nk_rgb(255,255,255):nk_rgb(0,0,0));
        if(visible) {
            ps_ui_draw_backdrop(ui,material);
        }
        /* A later ordinary command must use the ordinary UI program again. */
        nk_fill_rect(c,nk_rect(100,70,10,10),0,nk_rgb(240,20,30));
    }
    nk_end(ui);
}
static void draw_plain(struct nk_context *ui,ps_graphics *g,ps_ui_backdrop *material) {
    nk_input_begin(ui);nk_input_end(ui);ps_graphics_backdrop(g,material);
    if(nk_begin(ui,"backdrop probe",nk_rect(0,0,320,200),NK_WINDOW_NO_SCROLLBAR)) {
        struct nk_command_buffer *c=nk_window_get_canvas(ui);nk_push_scissor(c,nk_rect(0,0,320,200));
        for(unsigned y=0;y<2;y++)for(unsigned x=0;x<2;x++)
            nk_fill_rect(c,nk_rect(160.f*x,100.f*y,160,100),0,nk_rgb(x?200:20,y?220:40,73));
        ps_ui_draw_backdrop(ui,material);
        nk_fill_rect(c,nk_rect(100,70,10,10),0,nk_rgb(240,20,30));
    }
    nk_end(ui);
}
/* Independent normalized Gaussian, derived from sigma rather than shader
 * coefficients. Each channel has its own step edge; blue must remain untinted. */
static double gaussian_step(int offset) {
    double total=0,white=0;
    for(int i=-9;i<=9;i++) {
        double weight=exp(-i*i/(2.*3*3));total+=weight;if(offset+i>=0)white+=weight;
    }
    return white/total;
}
int main(int argc,char **argv) {
    CHECK(argc==2 && SDL_Init(SDL_INIT_VIDEO));
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_Window *window=SDL_CreateWindow("Physim GPU backdrop probe",320,200,SDL_WINDOW_OPENGL);CHECK(window);
    ps_graphics *g=ps_graphics_create(window);CHECK(g);
    struct nk_context *ui=nk_sdl_init(window,g);CHECK(ui);
    struct nk_font_atlas *atlas=nk_sdl_font_stash_begin(ui);struct nk_font *font=nk_font_atlas_add_default(atlas,16,NULL);CHECK(font);CHECK(nk_sdl_font_stash_end(ui));nk_style_set_font(ui,&font->handle);
    void (APIENTRY *read_pixels)(GLint,GLint,GLsizei,GLsizei,GLenum,GLenum,void*)=
        (void (APIENTRY *)(GLint,GLint,GLsizei,GLsizei,GLenum,GLenum,void*))SDL_GL_GetProcAddress("glReadPixels");CHECK(read_pixels);
    ps_ui_backdrop material={60,40,140,60,36,53,73,false,false,{36,53,73},false};
    int pw,ph;CHECK(SDL_GetWindowSizeInPixels(window,&pw,&ph));
    float sx=pw/320.f,sy=ph/200.f;
    for(unsigned pass=0;pass<4;pass++) {
        material.opaque=pass==1;draw(ui,g,&material,pass!=2);CHECK(nk_sdl_render(ui));
        unsigned char pixel[4],neighbor[4],front[4];
        read_pixels((int)(90.5f*sx),ph-1-(int)(60.5f*sy),1,1,GL_RGBA,GL_UNSIGNED_BYTE,pixel);
        read_pixels((int)(91.5f*sx),ph-1-(int)(60.5f*sy),1,1,GL_RGBA,GL_UNSIGNED_BYTE,neighbor);
        read_pixels((int)(105.5f*sx),ph-1-(int)(75.5f*sy),1,1,GL_RGBA,GL_UNSIGNED_BYTE,front);
        CHECK(front[0]==240 && front[1]==20 && front[2]==30);
        ps_ui_render_stats stats;CHECK(ps_graphics_ui_stats(g,&stats));
        if(pass==0 || pass==3) {
            CHECK(stats.backdrop_passes==2 && stats.backdrop_pixels<(size_t)pw*(size_t)ph);
            CHECK(pixel[0]>65 && pixel[0]<160 && abs((int)pixel[0]-(int)neighbor[0])<8);
        } else if(pass==1)CHECK(stats.backdrop_passes==0 && pixel[0]==36 && pixel[1]==53 && pixel[2]==73);
        else CHECK(stats.backdrop_passes==0 && stats.backdrop_pixels==0 && pixel[0]==0 && neighbor[0]==255);
        char path[4096];snprintf(path,sizeof path,"%s/backdrop-%u.bmp",argv[1],pass);CHECK(ps_graphics_capture(g,path));
    }
    for(unsigned scale=2;scale<=3;scale++) {
        char path[4096];snprintf(path,sizeof path,"%s/backdrop-scale-%u.png",argv[1],scale);
        material.opaque=false;draw(ui,g,&material,true);
        CHECK(nk_sdl_export_png_scaled(ui,ui,320,200,scale,path)==PS_OK);
        ps_ui_render_stats stats;CHECK(ps_graphics_ui_stats(g,&stats));
        CHECK(stats.backdrop_passes==2 && stats.backdrop_pixels==148u*68u*scale*scale);
        FILE *file=fopen(path,"rb");CHECK(file);unsigned char header[24];CHECK(fread(header,1,sizeof header,file)==sizeof header);CHECK(!fclose(file));
        unsigned width=(unsigned)header[16]<<24|(unsigned)header[17]<<16|(unsigned)header[18]<<8|header[19];
        unsigned height=(unsigned)header[20]<<24|(unsigned)header[21]<<16|(unsigned)header[22]<<8|header[23];
        CHECK(width==320u*scale && height==200u*scale);CHECK(nk_sdl_render(ui));
    }
    material=(ps_ui_backdrop){.w=320,.h=200,.blur_only=true};
    for(unsigned pass=0;pass<2;pass++) {
        material.opaque=pass!=0;draw_plain(ui,g,&material);CHECK(nk_sdl_render(ui));
        ps_ui_render_stats stats;CHECK(ps_graphics_ui_stats(g,&stats));
        CHECK(stats.backdrop_passes==(pass?0u:2u));
        CHECK(stats.backdrop_pixels==(pass?0:(size_t)pw*ph));
        for(int offset=-10;offset<=10;offset++) {
            unsigned char horizontal[4],vertical[4];
            read_pixels((int)((160+offset+.5f)*sx),ph-1-(int)(60.5f*sy),1,1,GL_RGBA,GL_UNSIGNED_BYTE,horizontal);
            read_pixels((int)(50.5f*sx),ph-1-(int)((100+offset+.5f)*sy),1,1,GL_RGBA,GL_UNSIGNED_BYTE,vertical);
            double expected=pass?(offset>=0?1:0):gaussian_step(offset);
            CHECK(fabs(horizontal[0]-(20+180*expected))<=1.1 && horizontal[1]==40 && horizontal[2]==73);
            CHECK(fabs(vertical[1]-(40+180*expected))<=1.1 && vertical[0]==20 && vertical[2]==73);
        }
        unsigned char corner[4],front[4];
        read_pixels((int)(1.5f*sx),ph-1-(int)(1.5f*sy),1,1,GL_RGBA,GL_UNSIGNED_BYTE,corner);
        read_pixels((int)(105.5f*sx),ph-1-(int)(75.5f*sy),1,1,GL_RGBA,GL_UNSIGNED_BYTE,front);
        CHECK(corner[0]==20 && corner[1]==40 && corner[2]==73);
        CHECK(front[0]==240 && front[1]==20 && front[2]==30);
        char path[4096];snprintf(path,sizeof path,"%s/pure-blur-%u.bmp",argv[1],pass);CHECK(ps_graphics_capture(g,path));
    }
    nk_sdl_shutdown(ui);ps_graphics_destroy(g);SDL_DestroyWindow(window);SDL_Quit();
    puts("GPU backdrop: glass and pure sigma3 Gaussian pixels, fallback, order, close and reopen passed");return 0;
}
