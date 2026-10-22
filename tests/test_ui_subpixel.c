#include "ui_geometry.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"UI subpixel %d: %s\n",__LINE__,#x);return 1;}}while(0)
static float width(nk_handle user,float height,const char *text,int length){(void)user;(void)height;(void)text;return length*3.25f;}
static void glyph(nk_handle user,float height,struct nk_user_font_glyph *out,nk_rune rune,nk_rune next) {
    (void)user;(void)height;(void)rune;(void)next;
    *out=(struct nk_user_font_glyph){.width=3.75f,.height=8.25f,.xadvance=4.125f,.offset={.125f,.375f},.uv={{0,0},{1,1}}};
}
typedef struct {float box[4];unsigned calls;} callback_state;
#ifdef NK_INCLUDE_FLOAT_COMMANDS
static void callback(void *canvas,float x,float y,float w,float h,nk_handle data) {
#else
static void callback(void *canvas,short x,short y,unsigned short w,unsigned short h,nk_handle data) {
#endif
    callback_state *s=data.ptr;s->calls++;s->box[0]=x;s->box[1]=y;s->box[2]=w;s->box[3]=h;
    nk_draw_list_fill_rect(canvas,nk_rect(x,y,w,h),nk_rgb(15,25,35),0);
}
int main(void) {
    struct nk_user_font font={.height=9.5f,.width=width,.query=glyph,.texture={.id=1}};
    struct nk_context ctx;CHECK(nk_init_default(&ctx,&font));
    ctx.style.window.padding=nk_vec2(0,0);ctx.style.window.border=0;
    ctx.style.window.fixed_background=nk_style_item_hide();
    callback_state custom={0};struct nk_rect measured={0},peek={0};
    const float points[]={11.125f,25.625f,21.375f,35.875f,31.625f,25.125f};
    if(nk_begin(&ctx,"precision",nk_rect(0,0,800,600),NK_WINDOW_NO_SCROLLBAR)) {
        struct nk_command_buffer *c=nk_window_get_canvas(&ctx);nk_push_scissor(c,nk_rect(.25f,.75f,400.5f,300.25f));
        nk_stroke_line(c,10.25f,20.75f,40.5f,20.75f,1.5f,nk_rgb(200,20,30));
        nk_stroke_curve(c,10.25f,50.75f,20.125f,60.375f,30.625f,60.875f,40.5f,50.75f,1.75f,nk_rgb(20,200,30));
        nk_stroke_rect(c,nk_rect(10.25f,70.75f,40.5f,20.25f),4.5f,1.25f,nk_rgb(20,30,200));
        nk_fill_rect(c,nk_rect(60.25f,70.75f,40.5f,20.25f),4.75f,nk_rgb(50,70,90));
        nk_fill_rect_multi_color(c,nk_rect(110.25f,70.75f,40.5f,20.25f),nk_rgb(1,2,3),nk_rgb(4,5,6),nk_rgb(7,8,9),nk_rgb(10,11,12));
        nk_stroke_circle(c,nk_rect(10.25f,100.75f,15.5f,15.5f),1.75f,nk_rgb(60,80,100));
        nk_fill_circle(c,nk_rect(30.25f,100.75f,15.5f,15.5f),nk_rgb(60,80,100));
        nk_stroke_arc(c,65.25f,110.75f,6.5f,0,1.5f,1.25f,nk_rgb(40,60,80));
        nk_fill_arc(c,85.25f,110.75f,6.5f,0,1.5f,nk_rgb(40,60,80));
        nk_stroke_triangle(c,points[0],points[1],points[2],points[3],points[4],points[5],1.5f,nk_rgb(100,90,80));
        nk_fill_triangle(c,points[0],points[1],points[2],points[3],points[4],points[5],nk_rgb(100,90,80));
        nk_stroke_polygon(c,points,3,1.5f,nk_rgb(100,90,80));nk_fill_polygon(c,points,3,nk_rgb(100,90,80));nk_stroke_polyline(c,points,3,1.5f,nk_rgb(100,90,80));
        struct nk_image image=nk_image_id(7);nk_draw_image(c,nk_rect(120.25f,110.75f,20.5f,15.25f),&image,nk_rgb(255,255,255));
        nk_draw_text(c,nk_rect(10.25f,150.75f,20.5f,12.25f),"AB",2,&font,nk_rgba(0,0,0,0),nk_rgb(255,255,255));
        nk_push_custom(c,nk_rect(10.25f,180.75f,20.5f,12.25f),callback,nk_handle_ptr(&custom));
        nk_layout_space_begin(&ctx,NK_STATIC,30.5f,1);nk_layout_space_push(&ctx,nk_rect(11.25f,2.75f,21.5f,10.25f));
        peek=nk_widget_bounds(&ctx);nk_widget(&measured,&ctx);nk_layout_space_end(&ctx);
    }nk_end(&ctx);
    const struct nk_command *cmd;unsigned types=0;bool found=false;
    nk_foreach(cmd,&ctx) {
        if(cmd->type==NK_COMMAND_LINE) {
            const struct nk_command_line *c=(const struct nk_command_line*)cmd;
            printf("ACTUAL LINE: begin=%.3f,%.3f end=%.3f,%.3f thickness=%.3f\n",(double)c->begin.x,(double)c->begin.y,(double)c->end.x,(double)c->end.y,(double)c->line_thickness);
            CHECK(c->begin.x==10.25f && c->begin.y==20.75f && c->end.x==40.5f && c->line_thickness==1.5f);found=true;
        }
        if(cmd->type==NK_COMMAND_CURVE){const struct nk_command_curve *c=(const struct nk_command_curve*)cmd;CHECK(c->ctrl[0].x==20.125f && c->ctrl[1].y==60.875f && c->line_thickness==1.75f);}
        if(cmd->type==NK_COMMAND_RECT){const struct nk_command_rect *c=(const struct nk_command_rect*)cmd;CHECK(c->x==10.25f && c->w==40.5f && c->rounding==4.5f && c->line_thickness==1.25f);}
        if(cmd->type==NK_COMMAND_RECT_FILLED){const struct nk_command_rect_filled *c=(const struct nk_command_rect_filled*)cmd;CHECK(c->x==60.25f && c->w==40.5f && c->rounding==4.75f);}
        if(cmd->type==NK_COMMAND_RECT_MULTI_COLOR){const struct nk_command_rect_multi_color *c=(const struct nk_command_rect_multi_color*)cmd;CHECK(c->x==110.25f && c->w==40.5f);}
        if(cmd->type==NK_COMMAND_CIRCLE){const struct nk_command_circle *c=(const struct nk_command_circle*)cmd;CHECK(c->x==10.25f && c->h==15.5f && c->line_thickness==1.75f);}
        if(cmd->type==NK_COMMAND_CIRCLE_FILLED){const struct nk_command_circle_filled *c=(const struct nk_command_circle_filled*)cmd;CHECK(c->x==30.25f && c->h==15.5f);}
        if(cmd->type==NK_COMMAND_ARC){const struct nk_command_arc *c=(const struct nk_command_arc*)cmd;CHECK(c->cx==65.25f && c->cy==110.75f && c->r==6.5f && c->line_thickness==1.25f);}
        if(cmd->type==NK_COMMAND_ARC_FILLED){const struct nk_command_arc_filled *c=(const struct nk_command_arc_filled*)cmd;CHECK(c->cx==85.25f && c->r==6.5f);}
        if(cmd->type==NK_COMMAND_TRIANGLE){const struct nk_command_triangle *c=(const struct nk_command_triangle*)cmd;CHECK(c->a.x==points[0] && c->b.y==points[3] && c->line_thickness==1.5f);}
        if(cmd->type==NK_COMMAND_TRIANGLE_FILLED){const struct nk_command_triangle_filled *c=(const struct nk_command_triangle_filled*)cmd;CHECK(c->c.x==points[4]);}
        if(cmd->type==NK_COMMAND_POLYGON){const struct nk_command_polygon *c=(const struct nk_command_polygon*)cmd;CHECK(c->point_count==3 && c->points[2].y==points[5] && c->line_thickness==1.5f);}
        if(cmd->type==NK_COMMAND_POLYGON_FILLED){const struct nk_command_polygon_filled *c=(const struct nk_command_polygon_filled*)cmd;CHECK(c->point_count==3 && c->points[2].y==points[5]);}
        if(cmd->type==NK_COMMAND_POLYLINE){const struct nk_command_polyline *c=(const struct nk_command_polyline*)cmd;CHECK(c->point_count==3 && c->points[2].y==points[5] && c->line_thickness==1.5f);}
        if(cmd->type==NK_COMMAND_IMAGE){const struct nk_command_image *c=(const struct nk_command_image*)cmd;CHECK(c->x==120.25f && c->w==20.5f);}
        if(cmd->type==NK_COMMAND_TEXT){const struct nk_command_text *c=(const struct nk_command_text*)cmd;CHECK(c->x==10.25f && c->y==150.75f && c->w==20.5f && c->h==12.25f && !strcmp(c->string,"AB"));}
        types|=1u<<cmd->type;
    }
    unsigned expected=0;for(unsigned type=NK_COMMAND_SCISSOR;type<=NK_COMMAND_CUSTOM;type++)expected|=1u<<type;
    CHECK((types&expected)==expected);
    CHECK(found && measured.x==peek.x && measured.y==peek.y && measured.w==21.5f && measured.h==10.25f);
    {
        int scissor[4]={-1,-2,-3,-4};
        CHECK(ps_ui_geometry_scissor(nk_rect(.75f,2.25f,10.875f,20.125f),100,100,100,100,scissor));
        CHECK(scissor[0]==0 && scissor[1]==77 && scissor[2]==12 && scissor[3]==21);
        CHECK(ps_ui_geometry_scissor(nk_rect(.75f,2.25f,10.875f,20.125f),100,100,150,125,scissor));
        CHECK(scissor[0]==1 && scissor[1]==97 && scissor[2]==17 && scissor[3]==26);
        CHECK(ps_ui_geometry_scissor(nk_rect(-1e30f,-1e30f,2e30f,2e30f),100,100,100,100,scissor));
        CHECK(scissor[0]==0 && scissor[1]==0 && scissor[2]==100 && scissor[3]==100);
        int unchanged[4];memcpy(unchanged,scissor,sizeof scissor);
        CHECK(!ps_ui_geometry_scissor(nk_rect(NAN,0,10,10),100,100,100,100,scissor) && !memcmp(unchanged,scissor,sizeof scissor));
        CHECK(!ps_ui_geometry_scissor(nk_rect(500,0,10,10),100,100,100,100,scissor) && !memcmp(unchanged,scissor,sizeof scissor));
    }
    ps_ui_geometry geometry;CHECK(ps_ui_geometry_init(&geometry,ps_allocator_default(),0)==PS_OK);
    struct nk_buffer commands;nk_buffer_init_default(&commands);struct nk_draw_null_texture texture={nk_handle_id(1),{.5f,.5f}};
    CHECK(ps_ui_geometry_convert(&geometry,&ctx,&commands,&texture)==PS_OK);
    CHECK(custom.calls>=1 && custom.box[0]==10.25f && custom.box[1]==180.75f && custom.box[2]==20.5f && custom.box[3]==12.25f);
    bool fractional_text=false;const ps_ui_vertex *vertices=geometry.vertices.memory.ptr;
    for(size_t i=0;i<geometry.vertices.allocated/sizeof *vertices;i++)if(vertices[i].p[0]==10.375f && vertices[i].p[1]==151.125f)fractional_text=true;
    CHECK(fractional_text);
    const struct nk_draw_command *draw=nk__draw_begin(&ctx,&commands);CHECK(draw && draw->clip_rect.x==.25f && draw->clip_rect.y==.75f && draw->clip_rect.w==400.5f);
    printf("FLOAT COMMANDS: types=0x%x logical widget=%.3f,%.3f %.3fx%.3f; text vertex 10.375,151.125 and custom rectangle preserved\n",types,measured.x,measured.y,measured.w,measured.h);
    ps_ui_geometry_destroy(&geometry);nk_buffer_free(&commands);nk_clear(&ctx);
    CHECK(nk_begin(&ctx,"precision",nk_rect(0,0,800,600),NK_WINDOW_NO_SCROLLBAR));
    struct nk_command_buffer *c=nk_window_get_canvas(&ctx);nk_push_scissor(c,nk_rect(-10,-10,100,100));
    nk_stroke_line(c,-.25f,-.75f,40.5f,20.75f,1.5f,nk_rgb(200,20,30));
    nk_size end=c->end;
    const int invalid_counts[]={-1,0,65536,65537};
    for(unsigned i=0;i<sizeof invalid_counts/sizeof *invalid_counts;i++) {
        nk_stroke_polygon(c,points,invalid_counts[i],1,nk_rgb(1,2,3));
        nk_fill_polygon(c,points,invalid_counts[i],nk_rgb(1,2,3));
        nk_stroke_polyline(c,points,invalid_counts[i],1,nk_rgb(1,2,3));CHECK(c->end==end);
    }
    nk_stroke_polygon(c,NULL,3,1,nk_rgb(1,2,3));nk_fill_polygon(c,NULL,3,nk_rgb(1,2,3));
    nk_stroke_polyline(c,NULL,3,1,nk_rgb(1,2,3));CHECK(c->end==end);nk_end(&ctx);
    found=false;nk_foreach(cmd,&ctx)if(cmd->type==NK_COMMAND_LINE) {
        const struct nk_command_line *line=(const struct nk_command_line*)cmd;
        CHECK(line->begin.x==-.25f && line->begin.y==-.75f);found=true;
    }CHECK(found);nk_free(&ctx);return 0;
}
