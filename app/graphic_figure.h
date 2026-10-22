#ifndef PS_GRAPHIC_FIGURE_H
#define PS_GRAPHIC_FIGURE_H
#include "graphic_snapshot.h"

#define PS_GRAPHIC_FIGURE_WIDTH 1200u
#define PS_GRAPHIC_FIGURE_HEIGHT 850u
#define PS_GRAPHIC_FIGURE_COMMAND_LIMIT 262144u
#define PS_GRAPHIC_FIGURE_SVG_LIMIT (32u*1024u*1024u)
typedef struct ps_graphic_figure ps_graphic_figure;
typedef enum {PS_FIGURE_RECT=1,PS_FIGURE_LINE,PS_FIGURE_CIRCLE,PS_FIGURE_TEXT,
              PS_FIGURE_CLIP,PS_FIGURE_UNCLIP} ps_figure_kind;
typedef enum {PS_FIGURE_START,PS_FIGURE_MIDDLE,PS_FIGURE_END} ps_figure_anchor;
/* Canonical top-left coordinates. For text, x/y is the baseline anchor, w the
 * available width, h the CSS em size; spacing is additional letter spacing.
 * Line: x/y -> w/h. Circle: x/y center, w radius. Clip/rect: x/y/w/h.
 * Colors are opaque 0xRRGGBB. Curve marks are already geometrically clipped;
 * the plot clip also clips stroke/mark extents. Text pointers belong to figure. */
typedef struct {
    ps_figure_kind kind;
    unsigned color;
    double x,y,w,h,stroke,spacing,advance;
    const char *text;
    ps_figure_anchor anchor;
    bool strong;
    int curve; /* -1 for layout; actual report curve index otherwise */
} ps_figure_command;
typedef struct {
    bool dark,visible;
    /* Optional actual primary font families, copied at construction (95 bytes).
     * SVG retains their identity; installed viewer fonts still govern glyphs. */
    const char *font_family,*strong_font_family;
    bool high_contrast; /* Captured high-contrast appearance; overrides dark. */
} ps_graphic_figure_options;
/* Actual font metrics in canonical CSS em units, including letter spacing.
 * Called only while creating the figure. No callback/user pointer survives.
 * Must return a finite nonnegative width and scale proportionally with size.
 * This makes text fitting part of the common scene rather than each adapter. */
typedef double (*ps_figure_measure)(void *user,const char *text,double size,double spacing,bool strong);
/* Captures a complete immutable vector figure; does not retain the snapshot.
 * NULL options = light/full. Visible requires a captured nontrivial crop.
 * Unrepresentable geometry/resources -> PS_LIMIT; output survives failure.
 * Text is complete UTF-8, fitted once using the supplied actual font metrics.
 * Consumers use the captured h/spacing/advance without further fitting. */
ps_result ps_graphic_figure_create(const ps_graphic_snapshot *snapshot,
    const ps_graphic_figure_options *options,ps_figure_measure measure,void *user,
    ps_allocator allocator,ps_graphic_figure **out);
void ps_graphic_figure_destroy(ps_graphic_figure *figure);
size_t ps_graphic_figure_count(const ps_graphic_figure *figure);
ps_result ps_graphic_figure_command(const ps_graphic_figure *figure,size_t index,ps_figure_command *out);
/* Calls in order. A sink failure is returned immediately; a partial drawing is
 * not a successful figure and must be discarded by the caller. */
typedef ps_result (*ps_figure_sink)(void *user,const ps_figure_command *command);
ps_result ps_graphic_figure_draw(const ps_graphic_figure *figure,ps_figure_sink sink,void *user);
/* UTF-8 scalable SVG, same geometry/dashes/clips as the canonical commands.
 * No font dependency is embedded; font rasterization/shaping remains a consumer
 * property. All text is XML-escaped, with no external URLs/scripts/resources.
 * Borrowed bytes are NUL terminated; length excludes NUL. */
typedef struct ps_graphic_svg ps_graphic_svg;
/* byte_limit includes the terminating NUL; zero uses the 32 MiB hard limit.
 * A caller may supply a smaller budget; exceeding either limit -> PS_LIMIT. */
ps_result ps_graphic_figure_svg(const ps_graphic_figure *figure,size_t byte_limit,
    ps_allocator allocator,ps_graphic_svg **out);
const char *ps_graphic_svg_bytes(const ps_graphic_svg *svg,size_t *length);
void ps_graphic_svg_destroy(ps_graphic_svg *svg);
#endif
