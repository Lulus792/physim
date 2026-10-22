#ifndef PHYSIM_GRAPHICS_H
#define PHYSIM_GRAPHICS_H
#include "physim/experiment.h"
#include "physim/memory.h"
#include <SDL3/SDL.h>
struct nk_context;
struct nk_buffer;
struct nk_draw_null_texture;
typedef struct ps_graphics ps_graphics;
typedef struct ps_graphics_image ps_graphics_image;
/* A frame-local command marker for one bounded UI backdrop. Register before
 * submission; ps_ui_draw_backdrop records its isolated rounded command. No CPU
 * pixels are read; unsupported filtering is opaque. */
typedef struct {
    float x,y,w,h;
    uint8_t red,green,blue;
    bool light,opaque;
    uint8_t fallback[3];
    /* Pure Gaussian sigma 3 logical px, without glass tint/saturation/rounding.
     * opaque or unavailable filtering leaves the background unchanged; callers
     * draw their dimming overlay separately. Zero preserves the search glass. */
    bool blur_only;
} ps_ui_backdrop;
void ps_graphics_backdrop(ps_graphics *g,const ps_ui_backdrop *backdrop);
/* Frame-local broad outer shadows. blur is the CSS blur radius (sigma=blur/2).
 * Rounded corner correction supports radius<=.3*sigma; rectangular shadows have
 * no ratio restriction. Register up to four persistent descriptors until render. */
typedef struct {
    float x,y,w,h,radius,offset_x,offset_y,blur;
    uint8_t red,green,blue,alpha;
} ps_ui_shadow;
void ps_graphics_shadows(ps_graphics *g,const ps_ui_shadow *shadows,unsigned count);
/* Last UI submission, including PNG drawing. CPU timings exclude swap/vsync and
 * do not measure GPU completion. Allocation count covers conversion VBO/EBO data. */
typedef struct {
    double conversion_seconds, upload_seconds;
    uint64_t allocations;
    size_t vertex_bytes, index_bytes, retained_bytes;
    unsigned backdrop_passes;
    unsigned shadow_draws;
    size_t backdrop_pixels;
} ps_ui_render_stats;
bool ps_graphics_ui_stats(const ps_graphics *g, ps_ui_render_stats *out);
/* Last successful scene call. CPU wall times: validation/target setup,
 * tessellation/transforms, then GL upload/draw/sort/resolve submission. Excludes
 * GPU completion, UI composition, capture and swap. Failure invalidates stats.
 * Retained bytes count the three dynamic CPU geometry arrays, excluding the
 * graphics struct, driver and GPU allocations. */
typedef struct {
    double setup_seconds, tessellation_seconds, submission_seconds;
    size_t vertices, vertex_bytes, index_bytes, retained_bytes;
} ps_scene_render_stats;
bool ps_graphics_scene_stats(const ps_graphics *g, ps_scene_render_stats *out);
typedef struct {
    float yaw, pitch, distance;
    ps_vec3 target;
    bool orthographic, vectors, grid;
} ps_camera;
extern const ps_camera PS_CAMERA_DEFAULT;
/* All calls belong to the creating thread and its current GL context. */
ps_graphics *ps_graphics_create(SDL_Window *window);
/* UI clear color only; physical scene colors are independent. No GL calls. */
void ps_graphics_background(ps_graphics *g, uint8_t red, uint8_t green, uint8_t blue);
/* Packed RGBA semantic colors for the scene clear and minor grid. No GL calls. */
void ps_graphics_scene_colors(ps_graphics *g, uint32_t background, uint32_t grid);
bool ps_graphics_make_current(ps_graphics *g);
void ps_graphics_destroy(ps_graphics *g);
unsigned ps_graphics_texture(ps_graphics *g, const void *rgba, int w, int h);
/* One coverage byte per pixel; sampling returns white RGB and that alpha. */
unsigned ps_graphics_alpha_texture(ps_graphics *g, const void *coverage, int w, int h);
/* Recorded GL_MAX_TEXTURE_SIZE for this context, zero for a null renderer. */
int ps_graphics_texture_limit(const ps_graphics *g);
void ps_graphics_delete_texture(ps_graphics *g, unsigned texture);
bool ps_graphics_ui(ps_graphics *g, struct nk_context *ui, struct nk_buffer *commands,
                    const struct nk_draw_null_texture *null_texture);
unsigned ps_graphics_scene(ps_graphics *g, const ps_scene *scene, const ps_camera *camera,
                           int width, int height);
/* Nearest triangle in the last successfully rendered scene; normalized top-left
 * coordinates. Returns its scene index, or -1 for background/invalid input.
 * Grid occludes but is not selectable; labels are handled by the UI overlay. */
int ps_graphics_pick(const ps_graphics *g, double x, double y);
/* Actual framebuffer readback: .png is lossless RGB, other suffixes keep BMP. */
bool ps_graphics_capture(ps_graphics *g, const char *path);
/* Standalone UI drawing rendered at twice the logical dimensions, RGB PNG. */
ps_result ps_graphics_ui_png(ps_graphics *g, struct nk_context *ui,
                             const struct nk_draw_null_texture *null_texture, int width, int height,
                             const char *path);
/* Integer raster scale 1..4. Logical dimensions unchanged; output limited to
 * 8192 per axis and the GPU texture limit. Unsupported output size -> PS_LIMIT. */
ps_result ps_graphics_ui_png_scaled(ps_graphics *g, struct nk_context *ui,
                                    const struct nk_draw_null_texture *null_texture, int width,
                                    int height, unsigned scale, const char *path);
/* Owned RGB8, top row first. Drawing coordinates are actual image pixels,
 * independent of window size/DPI. Dimensions 1..8192 and GPU texture limit.
 * The supplied allocator owns the image/pixels, not GL/conversion buffers.
 * Failure leaves *out unchanged. Current GL thread/context required to render. */
ps_result ps_graphics_ui_image(ps_graphics *g,struct nk_context *drawing,
    const struct nk_draw_null_texture *null_texture,unsigned width,unsigned height,
    ps_allocator allocator,ps_graphics_image **out);
ps_result ps_graphics_image_read(const ps_graphics_image *image,unsigned *width,unsigned *height,
    size_t *stride,const unsigned char **pixels);
void ps_graphics_image_destroy(ps_graphics_image *image);
ps_result ps_graphics_image_png(const ps_graphics_image *image,const char *path);
/* Independent texture copy for native previews; delete with this renderer.
 * A failed upload does not invalidate the owned RGB image/save path. */
unsigned ps_graphics_image_texture(ps_graphics *g,const ps_graphics_image *image);
bool ps_graphics_present(ps_graphics *g);
/* Projects to normalized top-left viewport coordinates. False outside the frustum. */
bool ps_graphics_project(const ps_camera *camera, ps_vec3 position, double aspect, float *x,
                         float *y);
/* Exercises actual GPU depth, projection, clipping and framebuffer resize. */
bool ps_graphics_test(ps_graphics *g, const char *capture_path);
#endif
