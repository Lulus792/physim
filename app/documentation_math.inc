#include "physim/data.h"
#include <zlib.h>
/* Formula outlines are typeset offline by MathJax. Only bounded coverage data,
 * not scripts or SVG documents, is read by the C17 viewer. */
typedef struct {
    const ps_doc_formula_variant *variant;
    unsigned texture,width,height;
    uint64_t used,frame;
} documentation_math_texture;
struct documentation_math_cache {documentation_math_texture *items;size_t count;uint64_t clock,frame;};
static void documentation_math_begin_frame(app *a) {
    if(a->doc_math)++a->doc_math->frame;
}
static const ps_doc_formula_variant *documentation_math_variant(app *a,const ps_document *d,const ps_doc_block *b,float width) {
    const char *tex=d->text+b->target;const ps_doc_formula *formula=ps_document_formula(tex,strlen(tex));
    if(!formula)return NULL;
    float zoom=(documentation_font_size(a)/16.f);
    for(size_t i=formula->count;i>0;i--)if(formula->variants[i-1].width*zoom<=width)return &formula->variants[i-1];
    return NULL;
}
static unsigned char *documentation_math_coverage(const app *a,const ps_doc_formula_variant *v) {
    if(!v->raster_width || !v->raster_height || v->raster_width>8192 || v->raster_height>8192 ||
       (size_t)v->raster_width*v->raster_height>16u*1024u*1024u || !v->packed_size || v->packed_size>8u*1024u*1024u)return NULL;
    char path[4096];join(path,sizeof path,a->root,v->asset);FILE *file=fopen(path,"rb");if(!file)return NULL;
    unsigned char *packed=malloc(v->packed_size),*pixels=malloc((size_t)v->raster_width*v->raster_height);
    bool okay=packed && pixels && fread(packed,1,v->packed_size,file)==v->packed_size && fgetc(file)==EOF && !ferror(file);
    fclose(file);
    if(okay) {
        uLongf count=(uLongf)v->raster_width*v->raster_height;uLong source=(uLong)v->packed_size;
        okay=uncompress2(pixels,&count,packed,&source)==Z_OK &&
            count==(uLongf)v->raster_width*v->raster_height && source==v->packed_size && ps_crc32(pixels,count)==v->crc;
    }
    free(packed);if(!okay){free(pixels);return NULL;}return pixels;
}
/* Area coverage, rather than point sampling, preserves thin fraction/radical
 * rules when the high-resolution source is reduced to the current drawable. */
static unsigned char *documentation_math_reduce(const unsigned char *source,unsigned sw,unsigned sh,unsigned w,unsigned h) {
    if(!w || !h || w>sw || h>sh)return NULL;
    unsigned char *pixels=malloc((size_t)w*h);if(!pixels)return NULL;
    double dx=(double)sw/w,dy=(double)sh/h;
    for(unsigned y=0;y<h;y++)for(unsigned x=0;x<w;x++) {
        double left=x*dx,right=(x+1)*dx,top=y*dy,bottom=(y+1)*dy,sum=0;
        for(unsigned sy=(unsigned)top;sy<sh && sy<ceil(bottom);sy++)for(unsigned sx=(unsigned)left;sx<sw && sx<ceil(right);sx++) {
            double area=(fmin(right,sx+1)-fmax(left,sx))*(fmin(bottom,sy+1)-fmax(top,sy));
            sum+=area*source[(size_t)sy*sw+sx];
        }
        pixels[(size_t)y*w+x]=(unsigned char)fmin(255,fmax(0,floor(sum/(dx*dy)+.5)));
    }
    return pixels;
}
static unsigned documentation_math_texture_for(app *a,const ps_doc_formula_variant *v) {
    if(!a->doc_math)a->doc_math=calloc(1,sizeof *a->doc_math);
    if(!a->doc_math)return 0;
    struct documentation_math_cache *cache=a->doc_math;
    float scale=((documentation_font_size(a)/16.f))*a->doc_raster_density;
    if(!isfinite(scale) || scale<=0 || scale>8)return 0;
    unsigned w=(unsigned)fmin(v->raster_width,ceil(v->width*scale));
    unsigned h=(unsigned)fmin(v->raster_height,ceil(v->height*scale));
    documentation_math_texture *slot=NULL;
    for(size_t i=0;i<cache->count;i++) {
        documentation_math_texture *item=&cache->items[i];
        if(item->variant==v && item->width==w && item->height==h){item->used=++cache->clock;item->frame=cache->frame;return item->texture;}
        if((!item->texture || item->frame!=cache->frame) && (!slot || item->used<slot->used))slot=item;
    }
    /* Submitted commands retain their texture handles until this frame is drawn. */
    if(!slot) {
        if(cache->count>=PS_DOC_MAX_SPANS+PS_DOC_MAX_BLOCKS)return 0;
        size_t count=cache->count+16;
        documentation_math_texture *items=realloc(cache->items,count*sizeof *items);
        if(!items)return 0;
        memset(items+cache->count,0,16*sizeof *items);
        slot=&items[cache->count];cache->items=items;cache->count=count;
    }
    unsigned char *source=documentation_math_coverage(a,v);
    unsigned char *pixels=source?documentation_math_reduce(source,v->raster_width,v->raster_height,w,h):NULL;
    unsigned texture=pixels?ps_graphics_alpha_texture(a->doc_graphics,pixels,(int)w,(int)h):0;
    free(source);free(pixels);
    if(slot->texture)ps_graphics_delete_texture(a->doc_graphics,slot->texture);
    *slot=(documentation_math_texture){v,texture,w,h,++cache->clock,cache->frame};return texture;
}
static void documentation_math_clear(app *a) {
    if(!a->doc_math)return;
    for(size_t i=0;i<a->doc_math->count;i++)if(a->doc_math->items[i].texture)ps_graphics_delete_texture(a->doc_graphics,a->doc_math->items[i].texture);
    free(a->doc_math->items);
    free(a->doc_math);a->doc_math=NULL;
}
static bool documentation_math_draw(app *a,const ps_document *d,const ps_doc_block *b,struct nk_rect bounds) {
    const ps_doc_formula_variant *v=documentation_math_variant(a,d,b,bounds.w-24);if(!v)return false;
    unsigned texture=documentation_math_texture_for(a,v);if(!texture)return false;
    float zoom=(documentation_font_size(a)/16.f);
    struct nk_rect frame=nk_rect(bounds.x+12,bounds.y+12,v->width*zoom,v->height*zoom);
    struct nk_image image=nk_image_id((int)texture);
    nk_draw_image(nk_window_get_canvas(a->doc_ui),frame,&image,UI_COLOR(doc_text));
    if(b->reading_begin!=SIZE_MAX)nk_sdl_accessibility_text_box(a->doc_ui,b->reading_begin,b->reading_begin+b->reading_length,frame);
    return true;
}
static void documentation_math_draw_probe(app *a) {
    if(!a->doc_math_test_requested)return;
    a->doc_math_test_requested=false;a->doc_math_test_ok=false;
    if(!a->documentation || a->doc_math_test_block>=a->documentation->count)return;
    ps_doc_block *b=&a->documentation->blocks[a->doc_math_test_block];
    const ps_doc_formula_variant *v=documentation_math_variant(a,a->documentation,b,b->hit[2]-24);
    if(!v || !a->doc_math)return;
    float zoom=(documentation_font_size(a)/16.f);unsigned texture=0;
    float scale=zoom*a->doc_raster_density;
    unsigned width=(unsigned)fmin(v->raster_width,ceil(v->width*scale));
    unsigned height=(unsigned)fmin(v->raster_height,ceil(v->height*scale));
    for(size_t i=0;i<a->doc_math->count;i++) {
        const documentation_math_texture *item=&a->doc_math->items[i];
        if(item->variant==v && item->width==width && item->height==height)texture=item->texture;
    }
    struct nk_rect clip={0},frame={0};const struct nk_command *command;
    nk_foreach(command,a->doc_ui) {
        if(command->type==NK_COMMAND_SCISSOR) {
            const struct nk_command_scissor *s=(const struct nk_command_scissor *)command;clip=nk_rect(s->x,s->y,s->w,s->h);
        } else if(command->type==NK_COMMAND_IMAGE) {
            const struct nk_command_image *image=(const struct nk_command_image *)command;
            if((unsigned)image->img.handle.id!=texture || fabsf(image->x-(b->hit[0]+12))>1 || fabsf(image->y-(b->hit[1]+12))>1)continue;
            frame=nk_rect(image->x,image->y,image->w,image->h);
            a->doc_math_test_ok=texture && frame.w>0 && frame.h>0 &&
                fabsf(frame.w-v->width*zoom)<=1 && fabsf(frame.h-v->height*zoom)<=1 &&
                frame.x>=b->hit[0]-1 && frame.y>=b->hit[1]-1 &&
                frame.x+frame.w<=b->hit[0]+b->hit[2]+1 && frame.y+frame.h<=b->hit[1]+b->hit[3]+1 &&
                frame.x>=clip.x-1 && frame.y>=clip.y-1 && frame.x+frame.w<=clip.x+clip.w+1 && frame.y+frame.h<=clip.y+clip.h+1;
        }
    }
    printf("FORMULA DRAW: case=%u font=%.0f theme=%u complete=%d frame=%.0f,%.0f,%.0f,%.0f block=%.0f,%.0f,%.0f,%.0f\n",
        a->doc_math_test_case,a->doc_font_ui->height,a->preferences.theme,a->doc_math_test_ok,frame.x,frame.y,frame.w,frame.h,
        b->hit[0],b->hit[1],b->hit[2],b->hit[3]);
}
