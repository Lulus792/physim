#include "ui_geometry.h"
#include <math.h>
#include <stdio.h>

/* Analytic rounded-box distance, independent of Nuklear's arc construction. */
static double distance(double x,double y,double w,double h,double radius) {
    double qx=fabs(x-w/2)-(w/2-radius),qy=fabs(y-h/2)-(h/2-radius);
    return hypot(fmax(qx,0),fmax(qy,0))+fmin(fmax(qx,qy),0)-radius;
}
static int probe(float w,float h,float requested,float scale) {
    struct nk_buffer commands,vertices,indices;
    nk_buffer_init_default(&commands);nk_buffer_init_default(&vertices);nk_buffer_init_default(&indices);
    struct nk_draw_null_texture texture={nk_handle_id(1),{.5f,.5f}};
    struct nk_convert_config config;ps_ui_geometry_config(&config,&texture);
    struct nk_draw_list list;nk_draw_list_init(&list);
    nk_draw_list_setup(&list,&config,&commands,&vertices,&indices,NK_ANTI_ALIASING_ON,NK_ANTI_ALIASING_ON);
    w*=scale;h*=scale;requested*=scale;
    nk_draw_list_path_rect_to(&list,nk_vec2(0,0),nk_vec2(w,h),requested);
    const struct nk_vec2 *points=(const struct nk_vec2*)((const unsigned char*)nk_buffer_memory(&commands));
    double radius=fmax(0,fmin(requested,fmin(w,h)/2)),maximum=0,sign=0;int convex=1;
    for(unsigned i=0;i<list.path_count;i++) {
        struct nk_vec2 a=points[i],b=points[(i+1)%list.path_count],c=points[(i+2)%list.path_count];
        double cross=(b.x-a.x)*(c.y-b.y)-(b.y-a.y)*(c.x-b.x);
        if(fabs(cross)>1e-4){if(sign && sign*cross<0)convex=0;sign=cross;}
        maximum=fmax(maximum,fabs(distance(a.x,a.y,w,h,radius)));
        maximum=fmax(maximum,fabs(distance((a.x+b.x)/2,(a.y+b.y)/2,w,h,radius)));
    }
    int ok=list.path_count>=4 && convex && maximum<=.13;
    printf("ROUNDED PATH width=%.3f height=%.3f request=%.3f radius=%.3f points=%u convex=%d error=%.6f pass=%d\n",(double)w,(double)h,(double)requested,radius,list.path_count,convex,maximum,ok);
    nk_buffer_free(&commands);nk_buffer_free(&vertices);nk_buffer_free(&indices);return ok;
}
int main(void) {
    const float scales[]={1,1.125f,1.25f,1.375f,2};
    const float cases[][3]={{36,22,12},{80,2,2},{2,22,12},{100,30,8},{100,30,0},{100,30,-3}};
    unsigned passed=0,total=0;
    for(unsigned i=0;i<sizeof scales/sizeof *scales;i++)for(unsigned j=0;j<sizeof cases/sizeof *cases;j++){
        total++;passed+=(unsigned)probe(cases[j][0],cases[j][1],cases[j][2],scales[i]);
    }
    printf("ROUNDED PATHS: %u/%u passed\n",passed,total);return passed==total?0:1;
}
