/* Production scene tessellation, upload/draw/resolve, and optional GL timer query.
 * Query collection deliberately serializes this benchmark, never the normal app. */
#include "benchmark_build.h"
#include "benchmark_usage.h"
#include "ui.h"
#include <SDL3/SDL_opengl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    void (APIENTRY *GenQueries)(GLsizei, GLuint *);
    void (APIENTRY *DeleteQueries)(GLsizei, const GLuint *);
    void (APIENTRY *BeginQuery)(GLenum, GLuint);
    void (APIENTRY *EndQuery)(GLenum);
    void (APIENTRY *GetQueryiv)(GLenum, GLenum, GLint *);
    void (APIENTRY *GetQueryObjectiv)(GLuint, GLenum, GLint *);
    void (APIENTRY *GetQueryObjectui64v)(GLuint, GLenum, GLuint64 *);
    void (APIENTRY *Flush)(void);
    GLenum (APIENTRY *GetError)(void);
    GLuint id;
    GLint bits;
} timer;

static bool timer_open(timer *t) {
#define LOAD(name) do { SDL_FunctionPointer address = SDL_GL_GetProcAddress("gl" #name); \
    if (!address) { return false; } memcpy(&t->name, &address, sizeof address); } while (0)
    LOAD(GenQueries); LOAD(DeleteQueries); LOAD(BeginQuery); LOAD(EndQuery);
    LOAD(GetQueryiv); LOAD(GetQueryObjectiv); LOAD(GetQueryObjectui64v);
    LOAD(Flush); LOAD(GetError);
#undef LOAD
    t->GetQueryiv(GL_TIME_ELAPSED, GL_QUERY_COUNTER_BITS, &t->bits);
    if (t->GetError() != GL_NO_ERROR || t->bits < 30 || t->bits > 64) return false;
    t->GenQueries(1, &t->id);
    return t->id && t->GetError() == GL_NO_ERROR;
}

static bool timer_result(timer *t, double started, double *seconds, double *wait) {
    double begin = ps_clock();
    t->Flush();
    GLint available = 0;
    do {
        t->GetQueryObjectiv(t->id, GL_QUERY_RESULT_AVAILABLE, &available);
        if (t->GetError() != GL_NO_ERROR || ps_clock() - begin > 5) return false;
        if (!available) ps_sleep(1);
    } while (!available);
    /* No wrapping query is silently reported as a short duration. */
    if (ps_clock() - started >= ldexp(1.0, t->bits) / 1e9) return false;
    GLuint64 nanoseconds = 0;
    t->GetQueryObjectui64v(t->id, GL_QUERY_RESULT, &nanoseconds);
    if (t->GetError() != GL_NO_ERROR) return false;
    *seconds = (double)nanoseconds / 1e9;
    *wait = ps_clock() - begin;
    return true;
}

static bool fixture(ps_scene *s, unsigned kind, size_t *vertices, size_t *indices) {
    *s = (ps_scene){0}; *indices = 0;
    if (!kind) { *vertices = 0; return true; }
    if (kind == 1) {
        for (unsigned i = 0; i < 32; ++i)
            ps_scene_add(s, PS_SPHERE, ps_v3((i % 8 - 3.5) * .36, (i / 8 - 1.5) * .36, 0),
                         ps_v3(0, 0, 0), .13, 0x54bfe5ff);
        *vertices = 32u * 24u * 16u * 6u;
    } else if (kind == 2) {
        if (ps_scene_frame(s, 100, 0, "Scaled frame", ps_v3(0, 0, 0),
                           ps_quat_axis_angle(ps_v3(0, 1, 0), .2), ps_v3(1.1, .8, 1)) != PS_OK)
            return false;
        for (unsigned i = 0; i < 31; ++i) {
            if (ps_scene_add_id(s, i + 1, PS_BOX, ps_v3((i % 8 - 3.5) * .36, (i / 8 - 1.5) * .36, 0),
                         ps_v3(.26, .26, .26), 0, i & 1 ? 0xee885588 : 0x54bfe5ff) != PS_OK) return false;
            if (ps_scene_set_parent(s, i + 1, 100) != PS_OK) return false;
        }
        *vertices = 31u * 36u; *indices = *vertices * sizeof(uint32_t);
    } else {
        ps_vec3 points[96];
        for (unsigned i = 0; i < 96; ++i)
            points[i] = ps_v3(-1.5 + i * (3.0 / 95), .6 * sin(i * .12), .2 * cos(i * .12));
        if (ps_scene_polyline(s, points, 96, .025, 0x54bfe5ff) != PS_OK) return false;
        ps_scene_add(s, PS_ARROW, ps_v3(-1, -.8, 0), ps_v3(1, -.8, 0), .025, 0xee8855ff);
        *vertices = 95u * 192u + 288u;
    }
    return ps_scene_valid(s);
}

static bool compose(struct nk_context *ui, unsigned texture) {
    nk_input_begin(ui); nk_input_end(ui);
    if (nk_begin(ui, "Scene", nk_rect(0, 0, 640, 480), NK_WINDOW_NO_SCROLLBAR)) {
        nk_layout_row_static(ui, 480, 640, 1);
        nk_image(ui, nk_image_id((int)texture));
    }
    nk_end(ui);
    return nk_sdl_render(ui);
}

int main(int argc, char **argv) {
    bool smoke = false, no_gpu = false;
    if (argc < 2 || argc > 4) return 2;
    for (int i = 2; i < argc; ++i) {
        if (!strcmp(argv[i], "--smoke") && !smoke) smoke = true;
        else if (!strcmp(argv[i], "--no-gpu") && !no_gpu) no_gpu = true;
        else return 2;
    }
    if (!ps_make_directory_exclusive(argv[1])) return 1;
    char path[4096];
    if (snprintf(path, sizeof path, "%s/frames.csv", argv[1]) >= (int)sizeof path) return 2;
    FILE *output = fopen(path, "wbx");
    if (!output) return 1;
    SDL_Window *window = NULL;
    ps_graphics *graphics = NULL;
    struct nk_context *ui = NULL;
    timer query = {0};
    int result = 1;
    SDL_SetMainReady();
    if (!SDL_Init(SDL_INIT_VIDEO)) goto cleanup;
#ifdef __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    window = SDL_CreateWindow("Physim scene benchmark", 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    if (!window || !(graphics = ps_graphics_create(window))) goto cleanup;
    SDL_GL_SetSwapInterval(0);
    if (!(ui = nk_sdl_init(window, graphics))) goto cleanup;
    struct nk_font_atlas *atlas = nk_sdl_font_stash_begin(ui);
    struct nk_font *font = nk_font_atlas_add_default(atlas, 18, NULL);
    if (!font || !nk_sdl_font_stash_end(ui)) goto cleanup;
    nk_style_set_font(ui, &font->handle); ui->style.window.padding = nk_vec2(0, 0);
    bool gpu = !no_gpu && timer_open(&query);
    printf("Compiler: %s | Configuration: %s\nGPU timer: %s | Counter bits: %d\n",
           PS_BENCH_COMPILER, PS_BENCH_CONFIG, gpu ? "available" : "unavailable/disabled", gpu ? query.bits : 0);
    fprintf(output, "workload,frame,total_seconds,setup_seconds,tessellation_seconds,submission_seconds,"
                    "vertices,vertex_bytes,index_bytes,retained_bytes,user_cpu_seconds,system_cpu_seconds,"
                    "peak_resident_bytes,gpu_available,gpu_seconds,query_wait_seconds\n");
    const char *names[] = {"empty", "spheres", "translucent", "path"};
    unsigned frames = smoke ? 6 : 60;
    ps_camera camera = {0, .15f, 4.5f, {0, 0, 0}, false, true, false};
    for (unsigned kind = 0; kind < 4; ++kind) {
        ps_scene scene; size_t vertices, indices;
        if (!fixture(&scene, kind, &vertices, &indices)) { SDL_SetError("Invalid %s reference fixture", names[kind]); goto cleanup; }
        for (unsigned frame = 0; frame < frames + 5; ++frame) {
            SDL_PumpEvents();
            ps_benchmark_mark mark;
            if (!ps_benchmark_begin(&mark)) goto cleanup;
            /* Match the renderer's clock for nested wall intervals. macOS
             * CLOCK_MONOTONIC can be coarser than SDL's nanosecond clock. */
            Uint64 total_started = SDL_GetTicksNS();
            double query_started = ps_clock();
            if (gpu) query.BeginQuery(GL_TIME_ELAPSED, query.id);
            unsigned texture = ps_graphics_scene(graphics, &scene, &camera, 640, 480);
            if (gpu) query.EndQuery(GL_TIME_ELAPSED);
            double total = (double)(SDL_GetTicksNS() - total_started) / 1e9;
            double resource_wall; ps_process_usage usage;
            if (!texture || !ps_benchmark_end(mark, &resource_wall, &usage)) goto cleanup;
            ps_scene_render_stats stats;
            if (!ps_graphics_scene_stats(graphics, &stats)) goto cleanup;
            if (stats.vertices != vertices || stats.vertex_bytes != vertices * 10 * sizeof(float) ||
                stats.index_bytes != indices) {
                SDL_SetError("%s geometry: vertices %zu/%zu, indices %zu/%zu", names[kind],
                             stats.vertices, vertices, stats.index_bytes, indices);
                goto cleanup;
            }
            double gpu_seconds = 0, wait = 0; char gpu_text[64] = "";
            if (gpu) {
                if (!timer_result(&query, query_started, &gpu_seconds, &wait)) goto cleanup;
                snprintf(gpu_text, sizeof gpu_text, "%.9f", gpu_seconds);
            }
            if (frame >= 5)
                fprintf(output, "%s,%u,%.9f,%.9f,%.9f,%.9f,%zu,%zu,%zu,%zu,%.9f,%.9f,%llu,%u,%s,%.9f\n",
                        names[kind], frame - 5, total, stats.setup_seconds,
                        stats.tessellation_seconds, stats.submission_seconds, stats.vertices,
                        stats.vertex_bytes, stats.index_bytes, stats.retained_bytes,
                        usage.user_seconds, usage.system_seconds,
                        (unsigned long long)usage.peak_resident_bytes, gpu ? 1u : 0u, gpu_text, wait);
            if (!compose(ui, texture)) goto cleanup;
            if (frame == 4 || frame == frames + 4) {
                snprintf(path, sizeof path, "%s/%s-%s.bmp", argv[1], names[kind], frame == 4 ? "first" : "last");
                if (!ps_graphics_capture(graphics, path)) goto cleanup;
            }
            if (!ps_graphics_present(graphics)) goto cleanup;
        }
    }
    /* Stale stats must not masquerade as a later failed frame. */
    ps_scene invalid = {0}; invalid.count = PS_MAX_OBJECTS + 1;
    ps_scene_render_stats unchanged = {0}, snapshot = unchanged;
    if (ps_graphics_scene(graphics, &invalid, &camera, 640, 480) ||
        ps_graphics_scene_stats(graphics, &unchanged) || memcmp(&unchanged, &snapshot, sizeof snapshot)) goto cleanup;
    result = 0;
cleanup:
    if (result) fprintf(stderr, "Scene benchmark failed: %s\n", SDL_GetError());
    if (query.id && query.DeleteQueries) query.DeleteQueries(1, &query.id);
    nk_sdl_shutdown(ui); ps_graphics_destroy(graphics); SDL_DestroyWindow(window); SDL_Quit();
    if (fclose(output)) result = 1;
    return result;
}
