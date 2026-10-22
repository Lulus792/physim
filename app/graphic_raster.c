#include "graphic_raster.h"
#include "ui.h"
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GLYPH_LIMIT 4096u
#define FONT_MEMORY_LIMIT (128u*1024u*1024u)
struct ps_graphic_raster {
    ps_allocator allocator;
    ps_graphics *graphics;
    ps_graphic_figure *figure;
    ps_graphics_image *image;
    unsigned texture;
    ps_graphic_raster_info info;
};
typedef union {ps_memory_alignment align;size_t bytes;} allocation_header;
typedef struct {ps_allocator allocator;size_t live,peak;ps_result failure;} allocation_domain;
typedef struct {struct nk_font *font;ps_ui_font_pairs *pairs;float em,spacing;struct nk_user_font handle;} draw_font;
typedef struct {
    allocation_domain domain;
    struct nk_allocator allocator;
    struct nk_font_atlas atlas;
    struct nk_context drawing;
    struct nk_draw_null_texture null_texture;
    draw_font fonts[2],text_fonts[32];
    unsigned text_count,font_texture,scale;
    nk_rune characters[GLYPH_LIMIT],ranges[GLYPH_LIMIT*2+1];
    unsigned count;
    bool drawing_initialized;
    struct nk_command_buffer *canvas;
    struct nk_draw_list *list;
    const ps_graphic_figure *figure;
    ps_result draw_result;
} raster_work;
static void *allocate(nk_handle user,void *old,nk_size size) {
    (void)old;allocation_domain *d=user.ptr;
    if(size>FONT_MEMORY_LIMIT || size>SIZE_MAX-sizeof(allocation_header) ||
       size+sizeof(allocation_header)>FONT_MEMORY_LIMIT-d->live){d->failure=PS_LIMIT;return NULL;}
    void *memory=NULL;size_t bytes=sizeof(allocation_header)+(size_t)size;
    ps_result r=ps_memory_allocate(d->allocator,bytes,&memory);
    if(r!=PS_OK){d->failure=r;return NULL;}
    allocation_header *h=memory;h->bytes=bytes;d->live+=bytes;if(d->live>d->peak)d->peak=d->live;
    return h+1;
}
static void release(nk_handle user,void *pointer) {
    if(pointer){allocation_domain *d=user.ptr;allocation_header *h=(allocation_header *)pointer-1;
        size_t bytes=h->bytes;d->live-=bytes;ps_memory_free(d->allocator,h,bytes);}
}
static ps_result characters(raster_work *w,const char *text) {
    size_t n=strlen(text);
    for(size_t at=0;at<n;) {
        nk_rune rune;int bytes=nk_utf_decode(text+at,&rune,(int)(n-at));
        if(bytes<=0)return PS_INVALID;
        bool present=false;for(unsigned i=0;i<w->count;i++)if(w->characters[i]==rune){present=true;break;}
        if(!present){if(w->count==GLYPH_LIMIT)return PS_LIMIT;w->characters[w->count++]=rune;}
        at+=(size_t)bytes;
    }
    return PS_OK;
}
static int rune_compare(const void *a,const void *b) {
    nk_rune x=*(const nk_rune *)a,y=*(const nk_rune *)b;return x<y?-1:x>y;
}
static ps_result demand(raster_work *w,const ps_graphic_snapshot *snapshot) {
    ps_graphic_description d;ps_result r=ps_graphic_snapshot_describe(snapshot,&d);if(r!=PS_OK)return r;
    for(nk_rune rune=32;rune<=126;rune++)w->characters[w->count++]=rune;
    const char *fixed="PHYSIM / AUSWERTUNG · Referenz Zeit Gesamtes Diagramm Sichtbarer Ausschnitt Kurve Kurven Vorschau reduziert; mehr Pixel erzeugen keine Messpunkte Achsenoffset Wert = Offset + Achsenwert – …";
    char title[192];r=ps_report_describe(ps_graphic_snapshot_report(snapshot),title,NULL,NULL,NULL);if(r!=PS_OK)return r;
    const char *strings[]={fixed,title,d.plot.title,d.plot.x_label,d.plot.y_label,d.plot.x_unit.symbol,d.plot.y_unit.symbol};
    for(unsigned i=0;i<sizeof strings/sizeof strings[0];i++){r=characters(w,strings[i]);if(r!=PS_OK)return r;}
    for(uint32_t i=0;i<d.plot.curves;i++) {
        const ps_curve_data *c=NULL;r=ps_report_curve_view(ps_graphic_snapshot_report(snapshot),0,i,&c);
        if(r==PS_OK)r=characters(w,c->label);
        if(r!=PS_OK)return r;
    }
    qsort(w->characters,w->count,sizeof w->characters[0],rune_compare);
    unsigned at=0;
    for(unsigned i=0;i<w->count;i++) {
        nk_rune low=w->characters[i],high=low;
        while(i+1<w->count && w->characters[i+1]==high+1)high=w->characters[++i];
        w->ranges[at++]=low;w->ranges[at++]=high;
    }
    w->ranges[at]=0;return PS_OK;
}
static double unit_advance(draw_font *font,nk_rune first,nk_rune second) {
    struct nk_user_font_glyph g={0};font->font->handle.query(font->font->handle.userdata,font->em,&g,first,second);
    return g.xadvance+ps_ui_font_pair_advance(font->pairs,font->em,first,second);
}
static double advance(draw_font *font,const char *text,int length,double size,double spacing) {
    double width=0;
    for(int at=0;at<length;) {
        nk_rune first=0,second=0;int n=nk_utf_decode(text+at,&first,length-at);if(n<=0)return NAN;
        if(at+n<length && nk_utf_decode(text+at+n,&second,length-at-n)<=0)return NAN;
        width+=unit_advance(font,first,second)*size;if(second)width+=spacing;at+=n;
    }
    return width;
}
static double measure(void *user,const char *text,double size,double spacing,bool strong) {
    raster_work *w=user;return advance(&w->fonts[strong?1:0],text,(int)strlen(text),size,spacing);
}
static float font_width(nk_handle user,float height,const char *text,int length) {
    draw_font *font=user.ptr;return (float)advance(font,text,length,height/font->em,font->spacing);
}
static void font_query(nk_handle user,float height,struct nk_user_font_glyph *glyph,nk_rune first,nk_rune second) {
    draw_font *font=user.ptr;
    font->font->handle.query(font->font->handle.userdata,height,glyph,first,second);
    glyph->xadvance=(float)(unit_advance(font,first,second)*(height/font->em)+(second?font->spacing:0));
}
static ps_result draw_command(void *user,const ps_figure_command *c) {
    raster_work *w=user;float scale=(float)w->scale;
    struct nk_color color=nk_rgb((int)(c->color>>16)&255,(int)(c->color>>8)&255,(int)c->color&255);
    struct nk_draw_list *canvas=w->list;
    switch(c->kind) {
    case PS_FIGURE_RECT:nk_draw_list_fill_rect(canvas,nk_rect((float)c->x*scale,(float)c->y*scale,(float)c->w*scale,(float)c->h*scale),color,0);break;
    case PS_FIGURE_LINE:nk_draw_list_stroke_line(canvas,nk_vec2((float)c->x*scale,(float)c->y*scale),nk_vec2((float)c->w*scale,(float)c->h*scale),color,(float)c->stroke*scale);break;
    case PS_FIGURE_CIRCLE:nk_draw_list_fill_circle(canvas,nk_vec2((float)c->x*scale,(float)c->y*scale),(float)c->w*scale,color,32);break;
    case PS_FIGURE_CLIP:ps_ui_draw_list_clip(canvas,nk_rect((float)c->x*scale,(float)c->y*scale,(float)c->w*scale,(float)c->h*scale));break;
    case PS_FIGURE_UNCLIP:ps_ui_draw_list_clip(canvas,nk_rect(0,0,1200*scale,850*scale));break;
    case PS_FIGURE_TEXT: {
        if(w->text_count==32)return PS_LIMIT;
        draw_font *font=&w->text_fonts[w->text_count++];*font=w->fonts[c->strong?1:0];font->spacing=(float)c->spacing*scale;
        font->handle=font->font->handle;float height=(float)c->h*font->em*scale;
        font->handle.userdata=nk_handle_ptr(font);font->handle.height=height; font->handle.width=font_width;font->handle.query=font_query;
        double x=c->x-(c->anchor==PS_FIGURE_END?c->advance:c->anchor==PS_FIGURE_MIDDLE?c->advance/2:0);
        float baseline=font->font->info.ascent*height/font->font->info.height;
        nk_draw_list_add_text(canvas,&font->handle,nk_rect((float)x*scale,(float)c->y*scale-baseline,(float)c->advance*scale+4,height+4),
            c->text,(int)strlen(c->text),height,color);break;
    }
    default:return PS_INVALID;
    }
    return w->domain.failure;
}
static void drawing_callback(void *canvas,nk_command_coord x,nk_command_coord y,nk_command_size width,nk_command_size height,nk_handle user) {
    (void)x;(void)y;(void)width;(void)height;raster_work *w=user.ptr;
    /* Geometry conversion can retry with larger buffers. Start the immutable
     * figure again; never accumulate font slots across those conversion passes. */
    w->list=canvas;w->text_count=0;
    w->draw_result=ps_graphic_figure_draw(w->figure,draw_command,w);
}
void ps_graphic_raster_destroy(ps_graphic_raster *r) {
    if(r){ps_allocator a=r->allocator;if(r->texture)ps_graphics_delete_texture(r->graphics,r->texture);
        ps_graphics_image_destroy(r->image);ps_graphic_figure_destroy(r->figure);ps_memory_free(a,r,sizeof *r);}
}
const ps_graphic_figure *ps_graphic_raster_figure(const ps_graphic_raster *r){return r?r->figure:NULL;}
const ps_graphics_image *ps_graphic_raster_image(const ps_graphic_raster *r){return r?r->image:NULL;}
ps_result ps_graphic_raster_describe(const ps_graphic_raster *r,ps_graphic_raster_info *out) {
    if(!r || !out)return PS_INVALID;
    *out=r->info;return PS_OK;
}
unsigned ps_graphic_raster_texture(ps_graphic_raster *r) {
    if(r && !r->texture)r->texture=ps_graphics_image_texture(r->graphics,r->image);
    return r?r->texture:0;
}
ps_result ps_graphic_raster_create(ps_graphics *graphics,const ps_graphic_snapshot *snapshot,
    const ps_graphic_figure_options *options,unsigned scale,ps_graphic_font_loader loader,void *user,
    ps_allocator allocator,ps_graphic_raster **out) {
    if(!graphics || !snapshot || !loader || !out || scale<1 || scale>4 || !ps_allocator_valid(allocator))return PS_INVALID;
    if(1200*scale>(unsigned)ps_graphics_texture_limit(graphics) || 850*scale>(unsigned)ps_graphics_texture_limit(graphics))return PS_LIMIT;
    Uint64 started=SDL_GetTicksNS(),font_ready=started;
    void *memory=NULL;ps_result result=ps_memory_zero(allocator,1,sizeof(ps_graphic_raster),&memory);if(result!=PS_OK)return result;
    ps_graphic_raster *r=memory;r->allocator=allocator;r->graphics=graphics;r->info.scale=scale;r->info.width=1200*scale;r->info.height=850*scale;
    raster_work *w=NULL;result=ps_memory_zero(allocator,1,sizeof *w,&memory);if(result!=PS_OK){ps_graphic_raster_destroy(r);return result;}
    w=memory;w->scale=scale;w->domain.allocator=allocator;w->allocator=(struct nk_allocator){nk_handle_ptr(&w->domain),allocate,release};
    nk_font_atlas_init(&w->atlas,&w->allocator);nk_font_atlas_begin(&w->atlas);
#define DO(call) do{result=(call);if(result!=PS_OK)goto done;}while(0)
    DO(demand(w,snapshot));
    for(unsigned i=0;i<2;i++) {
        draw_font *f=&w->fonts[i];float height=(i?34.f:19.f)*(float)scale;
        f->font=loader(user,&w->atlas,height,i!=0,w->ranges,r->info.family[i]);
        if(!f->font){result=w->domain.failure!=PS_OK?w->domain.failure:PS_IO;goto done;}
        f->em=ps_ui_font_em_ratio(f->font);f->pairs=ps_ui_font_pairs_create(f->font,&w->atlas.permanent);
        if(!f->pairs){result=w->domain.failure!=PS_OK?w->domain.failure:PS_MEMORY;goto done;}
        struct nk_font_config *config=f->font->config;
        do{config->size*=f->em;config=config->n;}while(config!=f->font->config);
    }
    int width=0,height=0;
    const void *pixels=nk_font_atlas_bake_limited(&w->atlas,&width,&height,NK_FONT_ATLAS_ALPHA8,ps_graphics_texture_limit(graphics));
    if(!pixels){result=w->domain.failure!=PS_OK?w->domain.failure:PS_LIMIT;goto done;}
    for(unsigned i=0;i<2;i++)for(unsigned j=0;j<w->count;j++)if(!ps_ui_font_has_glyph(&w->fonts[i].font->handle,w->characters[j])) {
        SDL_SetError("Export font has no glyph U+%04X",w->characters[j]);result=PS_INVALID;goto done;
    }
    w->font_texture=ps_graphics_alpha_texture(graphics,pixels,width,height);
    if(!w->font_texture){result=PS_IO;goto done;}
    nk_font_atlas_end(&w->atlas,nk_handle_id((int)w->font_texture),&w->null_texture);nk_font_atlas_cleanup(&w->atlas);
    font_ready=SDL_GetTicksNS();r->info.font_seconds=(double)(font_ready-started)/1e9;
    r->info.glyphs=w->count;r->info.atlas_width=(unsigned)width;r->info.atlas_height=(unsigned)height;
    r->info.regular_height=w->fonts[0].font->info.height;r->info.strong_height=w->fonts[1].font->info.height;
    const struct nk_font_glyph *regular=nk_font_find_glyph(w->fonts[0].font,'A'),*strong=nk_font_find_glyph(w->fonts[1].font,'A');
    r->info.regular_glyph_width=regular->x1-regular->x0;r->info.strong_glyph_width=strong->x1-strong->x0;
    ps_graphic_figure_options captured={0};if(options)captured=*options;
    captured.font_family=r->info.family[0];captured.strong_font_family=r->info.family[1];
    DO(ps_graphic_figure_create(snapshot,&captured,measure,w,allocator,&r->figure));
    w->drawing_initialized=nk_init(&w->drawing,&w->allocator,&w->fonts[0].font->handle)!=0;
    if(!w->drawing_initialized || w->domain.failure!=PS_OK){result=w->domain.failure!=PS_OK?w->domain.failure:PS_MEMORY;goto done;}
    w->drawing.style.window.padding=nk_vec2(0,0);w->drawing.style.window.border=0;
    bool visible=nk_begin(&w->drawing,"figure",nk_rect(0,0,(float)r->info.width,(float)r->info.height),NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_NO_INPUT)!=0;
    if(!visible){if(w->drawing.current)nk_end(&w->drawing);result=w->domain.failure!=PS_OK?w->domain.failure:PS_MEMORY;goto done;}
    w->canvas=nk_window_get_canvas(&w->drawing);nk_push_scissor(w->canvas,nk_rect(0,0,(float)r->info.width,(float)r->info.height));
    w->figure=r->figure;w->draw_result=PS_INVALID;
    nk_push_custom(w->canvas,nk_rect(0,0,(float)r->info.width,(float)r->info.height),drawing_callback,nk_handle_ptr(w));nk_end(&w->drawing);
    DO(w->domain.failure);
    DO(ps_graphics_ui_image(graphics,&w->drawing,&w->null_texture,r->info.width,r->info.height,allocator,&r->image));
    DO(w->draw_result);r->info.render_seconds=(double)(SDL_GetTicksNS()-font_ready)/1e9;
done:
    r->info.font_peak_bytes=w->domain.peak;
    if(w->drawing_initialized)nk_free(&w->drawing);
    for(unsigned i=0;i<2;i++)ps_ui_font_pairs_destroy(w->fonts[i].pairs);
    nk_font_atlas_clear(&w->atlas);if(w->font_texture)ps_graphics_delete_texture(graphics,w->font_texture);
    if(w->domain.live){SDL_SetError("Export font domain retains %zu bytes",w->domain.live);result=PS_INVALID;}
    ps_memory_free(allocator,w,sizeof *w);
    r->info.total_seconds=(double)(SDL_GetTicksNS()-started)/1e9;
    if(result!=PS_OK){ps_graphic_raster_destroy(r);return result;}
    *out=r;return PS_OK;
#undef DO
}
