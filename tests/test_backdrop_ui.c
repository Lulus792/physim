#include "ui.h"
#include <SDL3/SDL_opengl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Backdrop line %d: %s (%s)\n",__LINE__,#x,SDL_GetError());return 1;}}while(0)
static void draw(struct nk_context *ui,ps_graphics *g,ps_ui_backdrop *material,bool visible) {
    ps_graphics_shadows(g,NULL,0);
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
    ps_graphics_shadows(g,NULL,0);
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
static double shadow_coverage(const ps_ui_shadow *s,double x,double y) {
    double r=s->radius,cx=s->x+s->w*.5,cy=s->y+s->h*.5;
    double dx=fabs(x-cx),dy=fabs(y-cy);
    if(dx<s->w*.5 && dy<s->h*.5 && (r==0 || dx<s->w*.5-r || dy<s->h*.5-r ||
       hypot(dx-(s->w*.5-r),dy-(s->h*.5-r))<r))return 0;
    x-=s->offset_x;y-=s->offset_y;
    double sigma=s->blur*.5,root=sqrt(2.)*sigma;
    if(!r)return .25*(erf((x-s->x)/root)-erf((x-s->x-s->w)/root))*
        (erf((y-s->y)/root)-erf((y-s->y-s->h)/root));
    /* Integrate the actual rounded mask, independently of the shader's
     * rectangle-minus-corner-moments approximation. */
    double result=0,step=s->h/2000.;
    for(unsigned i=0;i<2000;i++) {
        double at=(i+.5)*step,edge=0;
        if(at<r)edge=r-sqrt(fmax(0,r*r-(r-at)*(r-at)));
        else if(at>s->h-r)edge=r-sqrt(fmax(0,r*r-(at-s->h+r)*(at-s->h+r)));
        double horizontal=.5*(erf((x-s->x-edge)/root)-erf((x-s->x-s->w+edge)/root));
        result+=horizontal*exp(-pow(s->y+at-y,2)/(2*sigma*sigma))*step/(sqrt(2*3.141592653589793)*sigma);
    }
    return result;
}
static void draw_shadows(struct nk_context *ui,ps_graphics *g,const ps_ui_shadow *shadows,unsigned count) {
    nk_input_begin(ui);nk_input_end(ui);ps_graphics_backdrop(g,NULL);ps_graphics_shadows(g,shadows,count);
    if(nk_begin(ui,"backdrop probe",nk_rect(0,0,320,200),NK_WINDOW_NO_SCROLLBAR)) {
        struct nk_command_buffer *c=nk_window_get_canvas(ui);nk_push_scissor(c,nk_rect(0,0,320,200));
        nk_fill_rect(c,nk_rect(0,0,320,200),0,nk_rgb(255,255,255));
        for(unsigned i=0;i<count;i++)ps_ui_draw_shadow(ui,&shadows[i]);
        nk_fill_rect(c,nk_rect(100,70,10,10),0,nk_rgb(240,20,30));
    }
    nk_end(ui);
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
    const ps_ui_shadow shadows[]={
        {.x=40,.y=40,.w=90,.h=70,.radius=14,.offset_y=28,.blur=100,.alpha=119},
        {.x=200,.y=50,.w=60,.h=60,.offset_y=8,.blur=28,.alpha=34}};
    const int xs[]={1,25,39,41,65,85,129,130,150,180,199,201,230,259,260,290,318};
    const int ys[]={1,20,39,41,60,100,110,130,160,198};
    double maximum_error=0;
    for(unsigned pass=0;pass<3;pass++) {
        unsigned count=pass==2?0:pass+1;draw_shadows(ui,g,shadows,count);CHECK(nk_sdl_render(ui));
        ps_ui_render_stats stats;CHECK(ps_graphics_ui_stats(g,&stats));CHECK(stats.shadow_draws==count && stats.backdrop_passes==0);
        for(unsigned ix=0;ix<sizeof xs/sizeof *xs;ix++)for(unsigned iy=0;iy<sizeof ys/sizeof *ys;iy++) {
            int px=(int)((xs[ix]+.5f)*sx),py=(int)((ys[iy]+.5f)*sy);unsigned char pixel[4];
            read_pixels(px,ph-1-py,1,1,GL_RGBA,GL_UNSIGNED_BYTE,pixel);double expected=255;
            for(unsigned i=0;i<count;i++)expected*=1-shadows[i].alpha/255.*shadow_coverage(&shadows[i],(px+.5)/sx,(py+.5)/sy);
            maximum_error=fmax(maximum_error,fabs(pixel[0]-expected));
            CHECK(fabs(pixel[0]-expected)<=1.1 && pixel[0]==pixel[1] && pixel[1]==pixel[2]);
        }
        unsigned char front[4];read_pixels((int)(105.5f*sx),ph-1-(int)(75.5f*sy),1,1,GL_RGBA,GL_UNSIGNED_BYTE,front);
        CHECK(front[0]==240 && front[1]==20 && front[2]==30);
        char path[4096];snprintf(path,sizeof path,"%s/outer-shadows-%u.png",argv[1],pass);CHECK(ps_graphics_capture(g,path));
    }
    printf("GPU SHADOW ORACLE: samples=510 max_channel_error=%.9f\n",maximum_error);
    nk_sdl_shutdown(ui);ps_graphics_destroy(g);SDL_DestroyWindow(window);SDL_Quit();
    puts("GPU backdrop: glass, pure sigma3, independent rounded shadows, fallback and ordering passed");return 0;
}
