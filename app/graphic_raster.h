#ifndef PS_GRAPHIC_RASTER_H
#define PS_GRAPHIC_RASTER_H
#include "graphic_figure.h"
#include "graphics.h"
struct nk_font_atlas;
struct nk_font;
/* Use the application's native/offline font provider. The demand ranges live
 * through atlas destruction. Return the actual primary family in family[96].
 * No source/user pointer survives creation. Height is stb ascent/descent height;
 * the raster builder adjusts every merged source to the actual CSS em. */
typedef struct nk_font *(*ps_graphic_font_loader)(void *user,struct nk_font_atlas *atlas,
    float height,bool strong,const unsigned *ranges,char family[96]);
typedef struct ps_graphic_raster ps_graphic_raster;
typedef struct {
    unsigned scale,width,height,glyphs,atlas_width,atlas_height;
    float regular_height,strong_height;
    float regular_glyph_width,strong_glyph_width;
    size_t font_peak_bytes;
    double font_seconds,render_seconds,total_seconds;
    char family[2][96];
} ps_graphic_raster_info;
/* Creates the canonical figure using real font metrics, bakes a dedicated
 * demand atlas at scale 1..4, renders and owns RGB pixels. No viewport/UI-font
 * dependency. All GL resources belong to graphics' current thread/context.
 * The font bank is freed before return; figure/pixels own their contents.
 * Any failure preserves *out. Snapshot is borrowed only until return. */
ps_result ps_graphic_raster_create(ps_graphics *graphics,const ps_graphic_snapshot *snapshot,
    const ps_graphic_figure_options *options,unsigned scale,ps_graphic_font_loader loader,void *user,
    ps_allocator allocator,ps_graphic_raster **out);
void ps_graphic_raster_destroy(ps_graphic_raster *raster);
const ps_graphic_figure *ps_graphic_raster_figure(const ps_graphic_raster *raster);
const ps_graphics_image *ps_graphic_raster_image(const ps_graphic_raster *raster);
ps_result ps_graphic_raster_describe(const ps_graphic_raster *raster,ps_graphic_raster_info *out);
/* Lazy independent RGB texture; a failure preserves image and PNG/SVG paths.
 * Borrowed texture belongs to raster, destroyed before graphics. */
unsigned ps_graphic_raster_texture(ps_graphic_raster *raster);
#endif
