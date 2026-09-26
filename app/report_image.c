#include "report_image.h"
#include "plot_view.h"
#include "ui.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
    struct nk_command_buffer *canvas;
    const struct nk_user_font *font;
    struct nk_user_font fitted[64];
    unsigned count;
} image_drawing;
static void text(image_drawing *drawing, struct nk_rect rect, const char *label, float size,
                 struct nk_color color, bool centered) {
    const struct nk_user_font *font = drawing->font;
    if (drawing->count >= 64)
        return;
    struct nk_user_font *fitted = &drawing->fitted[drawing->count++];
    *fitted = *font;
    int length = (int)strlen(label);
    float width = font->width(font->userdata, size, label, length);
    fitted->height = width > rect.w ? size * rect.w / width : size;
    width = font->width(font->userdata, fitted->height, label, length);
    if (centered)
        rect.x += (rect.w - width) / 2;
    nk_draw_text(drawing->canvas, rect, label, length, fitted, nk_rgb(255, 255, 255), color);
}
static struct nk_vec2 point(struct nk_rect area, const double bounds[4], double x, double y) {
    double fx = ps_report_axis_fraction(x, bounds[0], bounds[1]);
    double fy = ps_report_axis_fraction(y, bounds[2], bounds[3]);
    /* Offscreen marks must remain representable before geometric clipping. */
    fx = fmax(-1e6, fmin(1e6, fx));
    fy = fmax(-1e6, fmin(1e6, fy));
    return nk_vec2(area.x + area.w * (float)fx, area.y + area.h * (1 - (float)fy));
}
ps_result ps_report_image(struct nk_context *ui, const struct nk_user_font *font,
                          const ps_report *report, uint32_t plot, const char *path) {
    return ps_report_image_scaled(ui, font, report, plot, 2, path);
}
ps_result ps_report_image_scaled(struct nk_context *ui, const struct nk_user_font *font,
                                 const ps_report *report, uint32_t plot, unsigned scale,
                                 const char *path) {
    return ps_report_image_region(ui, font, report, plot, scale, NULL, path);
}
ps_result ps_report_image_region(struct nk_context *ui, const struct nk_user_font *font,
                                 const ps_report *report, uint32_t plot, unsigned scale,
                                 const double region[4], const char *path) {
    ps_plot_info info;
    double bounds[4];
    if (!ui || !font || !path || scale < 1 || scale > 4 ||
        ps_report_plot_read(report, plot, &info) != PS_OK ||
        ps_report_plot_bounds(report, plot, bounds) != PS_OK)
        return PS_INVALID;
    if (region) {
        for (unsigned i = 0; i < 4; i++)
            if (!isfinite(region[i]))
                return PS_INVALID;
        if (!(region[0] < region[1] && region[2] < region[3]))
            return PS_INVALID;
        memcpy(bounds, region, sizeof bounds);
    }
    ps_curve_data *curve = malloc(sizeof *curve);
    if (!curve)
        return PS_MEMORY;
    struct nk_context drawing;
    if (!nk_init_default(&drawing, font)) {
        free(curve);
        return PS_MEMORY;
    }
    /* Opaque paper, generous margins, up to eight two-column legend entries. */
    const struct nk_color ink = nk_rgb(37, 42, 51), muted = nk_rgb(82, 90, 103);
    drawing.style.window.padding = nk_vec2(0, 0);
    drawing.style.window.fixed_background = nk_style_item_color(nk_rgb(255, 255, 255));
    nk_begin(&drawing, "figure", nk_rect(0, 0, 1200, 850), NK_WINDOW_NO_SCROLLBAR);
    struct nk_command_buffer *canvas = nk_window_get_canvas(&drawing);
    image_drawing draw = {0};
    draw.canvas = canvas;
    draw.font = font;
    nk_push_scissor(canvas, nk_rect(0, 0, 1200, 850));
    nk_fill_rect(canvas, nk_rect(0, 0, 1200, 850), 0, nk_rgb(255, 255, 255));
    const struct nk_rect area = {118, 150, 1020, 430};
    char label[384];
    text(&draw, nk_rect(60, 32, 1080, 38), info.title, 28, ink, false);
    snprintf(label, sizeof label, "%s [%s]", info.y_label, info.y_unit.symbol);
    text(&draw, nk_rect(118, 92, 1020, 24), label, 18, ink, false);
    double xo = ps_plot_axis_offset(bounds[0], bounds[1]);
    double yo = ps_plot_axis_offset(bounds[2], bounds[3]);
    if (yo) {
        snprintf(label, sizeof label, "Y-Offset: %.17g %s · Wert = Offset + Achsenwert", yo,
                 info.y_unit.symbol);
        text(&draw, nk_rect(118, 121, 1020, 22), label, 15, muted, false);
    }
    for (unsigned i = 0; i <= 4; i++) {
        double f = i / 4.;
        float x = area.x + (float)f * area.w, y = area.y + (1 - (float)f) * area.h;
        nk_stroke_line(canvas, x, area.y, x, area.y + area.h, 1, nk_rgb(226, 229, 235));
        nk_stroke_line(canvas, area.x, y, area.x + area.w, y, 1, nk_rgb(226, 229, 235));
        double value = ps_plot_axis_value(bounds[2] - yo, bounds[3] - yo, f);
        if (bounds[2] <= 0 && bounds[3] >= 0 &&
            fabs(value) < fmax(fabs(bounds[2]), fabs(bounds[3])) * 1e-12)
            value = 0;
        snprintf(label, sizeof label, "%.6g", value);
        text(&draw, nk_rect(16, y - 10, 92, 24), label, 16, muted, false);
        value = ps_plot_axis_value(bounds[0] - xo, bounds[1] - xo, f);
        if (bounds[0] <= 0 && bounds[1] >= 0 &&
            fabs(value) < fmax(fabs(bounds[0]), fabs(bounds[1])) * 1e-12)
            value = 0;
        snprintf(label, sizeof label, "%.6g", value);
        text(&draw, nk_rect(x - 60, 592, 120, 24), label, 16, muted, true);
    }
    static const unsigned char palette[][3] = {{23, 109, 209}, {188, 75, 18}, {23, 132, 80},
                                               {141, 74, 184}, {162, 37, 86}, {20, 127, 139},
                                               {117, 99, 19},  {83, 97, 115}};
    bool reduced = false;
    for (uint32_t c = 0; c < info.curves; c++) {
        ps_report_curve_read(report, plot, c, curve);
        struct nk_color color = nk_rgb(palette[c][0], palette[c][1], palette[c][2]);
        reduced |= curve->kind != PS_PLOT_HISTOGRAM && curve->count < curve->source_count;
        /* One pixel of room for marks exactly at a bound. */
        nk_push_scissor(canvas, nk_rect(area.x - 3, area.y - 3, area.w + 6, area.h + 6));
        struct nk_vec2 previous = {0};
        for (uint32_t i = 0; i < curve->count; i++) {
            struct nk_vec2 p = point(area, bounds, curve->x[i], curve->y[i]);
            if (curve->kind == PS_PLOT_LINE && curve->count > 1) {
                if (i && region) {
                    double x0 = ps_report_axis_fraction(curve->x[i - 1], bounds[0], bounds[1]);
                    double y0 = ps_report_axis_fraction(curve->y[i - 1], bounds[2], bounds[3]);
                    double x1 = ps_report_axis_fraction(curve->x[i], bounds[0], bounds[1]);
                    double y1 = ps_report_axis_fraction(curve->y[i], bounds[2], bounds[3]);
                    if (ps_plot_clip_line(&x0, &y0, &x1, &y1))
                        nk_stroke_line(canvas, area.x + (float)x0 * area.w,
                                       area.y + (1 - (float)y0) * area.h,
                                       area.x + (float)x1 * area.w,
                                       area.y + (1 - (float)y1) * area.h, 2, color);
                } else if (i)
                    nk_stroke_line(canvas, previous.x, previous.y, p.x, p.y, 2, color);
            } else if (curve->kind == PS_PLOT_HISTOGRAM) {
                struct nk_vec2 left = point(area, bounds, curve->x[i] - curve->bar_width / 2, 0);
                struct nk_vec2 right = point(area, bounds, curve->x[i] + curve->bar_width / 2, 0);
                if (region) {
                    left.x = fmaxf(area.x, fminf(area.x + area.w, left.x));
                    right.x = fmaxf(area.x, fminf(area.x + area.w, right.x));
                    p.y = fmaxf(area.y, fminf(area.y + area.h, p.y));
                    left.y = fmaxf(area.y, fminf(area.y + area.h, left.y));
                    if (right.x <= left.x || left.y <= p.y)
                        continue;
                }
                nk_fill_rect(
                    canvas,
                    nk_rect(left.x, p.y, fmaxf(.5f, right.x - left.x - 1), fmaxf(0, left.y - p.y)),
                    0, color);
            } else if (!region || (p.x >= area.x && p.x <= area.x + area.w && p.y >= area.y &&
                                   p.y <= area.y + area.h))
                nk_fill_circle(canvas, nk_rect(p.x - 3, p.y - 3, 6, 6), color);
            previous = p;
        }
        nk_push_scissor(canvas, nk_rect(0, 0, 1200, 850));
        float x = 60 + (float)(c % 2) * 550, y = 686 + (float)(c / 2) * 30;
        if (curve->kind == PS_PLOT_LINE)
            nk_stroke_line(canvas, x, y + 10, x + 24, y + 10, 3, color);
        else if (curve->kind == PS_PLOT_SCATTER)
            nk_fill_circle(canvas, nk_rect(x + 9, y + 7, 6, 6), color);
        else
            nk_fill_rect(canvas, nk_rect(x + 6, y + 3, 14, 14), 0, color);
        snprintf(label, sizeof label, "%s · %llu Messpunkte", curve->label,
                 (unsigned long long)curve->source_count);
        text(&draw, nk_rect(x + 34, y, 495, 24), label, 16, ink, false);
    }
    snprintf(label, sizeof label, "%s [%s]", info.x_label, info.x_unit.symbol);
    text(&draw, nk_rect(118, 626, 1020, 24), label, 18, ink, true);
    if (xo) {
        snprintf(label, sizeof label, "X-Offset: %.17g %s · Wert = Offset + Achsenwert", xo,
                 info.x_unit.symbol);
        text(&draw, nk_rect(118, 654, 1020, 22), label, 15, muted, true);
    }
    if (reduced)
        text(&draw, nk_rect(60, 814, 1080, 22),
             "Reduzierte Berichtsdaten · Gezeigt wird die im Bericht gespeicherte Vorschau.", 14,
             muted, false);
    nk_end(&drawing);
    ps_result result = nk_sdl_export_png_scaled(ui, &drawing, 1200, 850, scale, path);
    nk_free(&drawing);
    free(curve);
    return result;
}
