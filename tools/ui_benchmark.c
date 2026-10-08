/* Real SDL/Nuklear/OpenGL UI workloads. No physics or source projects are run. */
#include "benchmark_build.h"
#include "benchmark_usage.h"
#include "platform.h"
#include "ui.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool resize_window(SDL_Window *window, int width, int height) {
    /* SDL_SetWindowSize is asynchronous on X11/Wayland. Event pumping alone
     * need not finish the change before rendering/capturing the next frame. */
    if (!SDL_SetWindowSize(window, width, height) || !SDL_SyncWindow(window))
        return false;
    SDL_PumpEvents();
    int actual_width = 0, actual_height = 0;
    if (!SDL_GetWindowSize(window, &actual_width, &actual_height))
        return false;
    if (actual_width != width || actual_height != height)
        return SDL_SetError("Resize requested %dx%d but window is %dx%d", width, height,
                            actual_width, actual_height);
    return true;
}

static void draw(struct nk_context *ui, unsigned kind) {
    nk_input_begin(ui);
    nk_input_end(ui);
    if (nk_begin(ui, "Reference", nk_rect(0, 0, 1080, 740), NK_WINDOW_NO_SCROLLBAR)) {
        if (kind == 1) {
            for (unsigned row = 0; row < 18; row++) {
                nk_layout_row_dynamic(ui, 32, 6);
                for (unsigned col = 0; col < 6; col++) {
                    char label[32];
                    snprintf(label, sizeof label, "Value %u.%u", row, col);
                    nk_button_label(ui, label);
                }
            }
        } else if (kind == 2) {
            nk_layout_row_dynamic(ui, 26, 1);
            nk_label(ui, "Eight curves / 2048 points each", NK_TEXT_LEFT);
            struct nk_command_buffer *canvas = nk_window_get_canvas(ui);
            nk_push_scissor(canvas, nk_rect(24, 48, 1032, 640));
            for (unsigned c = 0; c < 8; c++) {
                struct nk_color color = nk_rgb(60 + c * 20, 170, 250 - c * 20);
                float previous_x = 24, previous_y = 340;
                for (unsigned i = 0; i < 2048; i++) {
                    float x = 24 + i * (1032.f / 2047);
                    float y = 340 + 260 * (float)sin(i * .012 + c * .35);
                    if (i)
                        nk_stroke_line(canvas, previous_x, previous_y, x, y, 1, color);
                    previous_x = x;
                    previous_y = y;
                }
            }
        }
    }
    nk_end(ui);
}

int main(int argc, char **argv) {
    bool smoke = argc == 3 && !strcmp(argv[2], "--smoke");
    if (argc != 2 && !smoke) {
        fprintf(stderr, "Usage: physim-ui-benchmark new-output-directory [--smoke]\n");
        return 2;
    }
    if (!ps_make_directory_exclusive(argv[1]))
        return 1;
    char path[4096];
    int n = snprintf(path, sizeof path, "%s/frames.csv", argv[1]);
    if (n < 0 || n >= (int)sizeof path)
        return 2;
    FILE *output = fopen(path, "wbx");
    if (!output)
        return 1;
    printf("Compiler: %s | Configuration: %s\n", PS_BENCH_COMPILER, PS_BENCH_CONFIG);
    SDL_SetMainReady();
    SDL_Window *window = NULL;
    ps_graphics *graphics = NULL;
    struct nk_context *ui = NULL;
    int result = 1;
    if (!SDL_Init(SDL_INIT_VIDEO))
        goto cleanup;
#ifdef __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    window = SDL_CreateWindow("Physim render benchmark", 1080, 740,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    if (!window)
        goto cleanup;
    graphics = ps_graphics_create(window);
    if (!graphics)
        goto cleanup;
    SDL_GL_SetSwapInterval(0);
    ui = nk_sdl_init(window, graphics);
    if (!ui)
        goto cleanup;
    struct nk_font_atlas *atlas = nk_sdl_font_stash_begin(ui);
    struct nk_font *font = nk_font_atlas_add_default(atlas, 18, NULL);
    if (!font || !nk_sdl_font_stash_end(ui))
        goto cleanup;
    nk_style_set_font(ui, &font->handle);
    fprintf(output, "workload,frame,total_seconds,conversion_seconds,upload_seconds,allocations,"
                    "vertex_bytes,index_bytes,retained_bytes,construction_seconds,present_seconds,frame_seconds,"
                    "user_cpu_seconds,system_cpu_seconds,peak_resident_bytes\n");
    const char *names[] = {"empty", "dense", "plot"};
    unsigned frames = smoke ? 6u : 300u;
    for (unsigned kind = 0; kind < 3; kind++) {
        /* Warm-up is explicit, excluded from steady-state timings. */
        for (unsigned frame = 0; frame < frames + 10; frame++) {
            SDL_PumpEvents();
            ps_ui_render_stats before, after;
            if (!ps_graphics_ui_stats(graphics, &before))
                goto cleanup;
            Uint64 construction_start = SDL_GetTicksNS();
            draw(ui, kind);
            double construction = (double)(SDL_GetTicksNS() - construction_start) / 1e9;
            ps_benchmark_mark start;
            if (!ps_benchmark_begin(&start)) goto cleanup;
            if (!nk_sdl_render(ui))
                goto cleanup;
            double elapsed;
            ps_process_usage usage;
            if (!ps_benchmark_end(start, &elapsed, &usage)) goto cleanup;
            if (!ps_graphics_ui_stats(graphics, &after))
                goto cleanup;
            if (frame >= 10 && after.allocations != before.allocations) {
                SDL_SetError("Steady UI frame allocated conversion storage");
                goto cleanup;
            }
            if (frame == 9) {
                snprintf(path, sizeof path, "%s/%s-%s.bmp", argv[1], names[kind],
                         "first");
                if (!ps_graphics_capture(graphics, path))
                    goto cleanup;
            }
            /* The submission is timed without swap, which drains/queues GPU work. */
            Uint64 present_start = SDL_GetTicksNS();
            if (!ps_graphics_present(graphics))
                goto cleanup;
            double present = (double)(SDL_GetTicksNS() - present_start) / 1e9;
            if (frame >= 10)
                fprintf(output, "%s,%u,%.9f,%.9f,%.9f,%llu,%zu,%zu,%zu,%.9f,%.9f,%.9f,%.9f,%.9f,%llu\n",
                        names[kind], frame - 10, elapsed, after.conversion_seconds,
                        after.upload_seconds,
                        (unsigned long long)(after.allocations - before.allocations),
                        after.vertex_bytes, after.index_bytes, after.retained_bytes,
                        construction, present, construction + elapsed + present,
                        usage.user_seconds, usage.system_seconds,
                        (unsigned long long)usage.peak_resident_bytes);
            if (frame == frames + 9) {
                /* Readback drains GPU work: use an additional untimed frame,
                 * keeping it out of measured submission/present intervals. */
                draw(ui, kind);
                if (!nk_sdl_render(ui)) goto cleanup;
                snprintf(path, sizeof path, "%s/%s-last.bmp", argv[1], names[kind]);
                if (!ps_graphics_capture(graphics, path) || !ps_graphics_present(graphics))
                    goto cleanup;
            }
        }
    }
    /* A second context (PNG export) and resize must not leave stale geometry in
     * the next ordinary window frame, which shares the retained buffers. */
    struct nk_context export_ui;
    if (!nk_init_default(&export_ui, &font->handle))
        goto cleanup;
    draw(&export_ui, 2);
    snprintf(path, sizeof path, "%s/plot.png", argv[1]);
    ps_result exported = nk_sdl_export_png(ui, &export_ui, 1080, 740, path);
    nk_free(&export_ui);
    if (exported != PS_OK)
        goto cleanup;
    draw(ui, 0);
    if (!nk_sdl_render(ui))
        goto cleanup;
    snprintf(path, sizeof path, "%s/empty-after-export.bmp", argv[1]);
    if (!ps_graphics_capture(graphics, path))
        goto cleanup;
    if (!resize_window(window, 640, 480))
        goto cleanup;
    draw(ui, 1);
    if (!nk_sdl_render(ui) || !ps_graphics_present(graphics))
        goto cleanup;
    if (!resize_window(window, 1080, 740))
        goto cleanup;
    draw(ui, 0);
    if (!nk_sdl_render(ui))
        goto cleanup;
    snprintf(path, sizeof path, "%s/empty-restored.bmp", argv[1]);
    if (!ps_graphics_capture(graphics, path))
        goto cleanup;
    result = 0;
cleanup:
    if (result)
        fprintf(stderr, "UI benchmark failed: %s\n", SDL_GetError());
    nk_sdl_shutdown(ui);
    ps_graphics_destroy(graphics);
    SDL_DestroyWindow(window);
    SDL_Quit();
    if (fclose(output))
        result = 1;
    return result;
}
