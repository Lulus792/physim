/* Presentation of validated analysis outputs. No analysis code executes here. */
static struct nk_color report_color(uint32_t i) {
    return UI_COLOR(curves)[i % 8];
}
static void report_text(app *a, struct nk_rect bounds, const char *text, struct nk_color color) {
    nk_draw_text(nk_window_get_canvas(a->ui), bounds, text, (int)strlen(text), a->font_ui,
                 nk_rgba(0, 0, 0, 0), color);
}
static void report_plot(app *a, uint32_t index, float height) {
    struct nk_context *ui = a->ui;
    ps_plot_info p;
    double b[4];
    if (ps_report_plot_read(a->analysis_report, index, &p) != PS_OK)
        return;
    ps_plot_view *view = &a->report_views[index];
    ps_result bounds_result = ps_report_plot_bounds(a->analysis_report, index, b);
    plot_controls(a, view, bounds_result == PS_OK ? b : NULL);
    nk_layout_row_dynamic(ui, 24, 1);
    char label[256];
    snprintf(label, sizeof label, "%s%s%s%s", p.y_label,p.y_unit.symbol[0]?" [":"",
             p.y_unit.symbol,p.y_unit.symbol[0]?"]":"");
    nk_label_colored(ui, label, NK_TEXT_LEFT, UI_COLOR(muted));
    nk_layout_row_dynamic(ui, height, 1);
    struct nk_rect r;
    nk_widget(&r, ui);
    struct nk_command_buffer *canvas = nk_window_get_canvas(ui);
    nk_fill_rect(canvas, r, 7, UI_COLOR(edit));
    if (bounds_result != PS_OK) {
        report_text(a, nk_rect(r.x + 20, r.y + 20, r.w - 40, 24),
                    "Dieses Diagramm enthält noch keine Kurven.", UI_COLOR(muted));
        return;
    }
    struct nk_rect area = nk_rect(r.x + 86, r.y + 20, r.w - 112, r.h - 64);
    plot_interact(a, view, area, b);
    double displayed[4];
    plot_display_bounds(view, b, displayed);
    double x_offset = ps_plot_axis_offset(displayed[0], displayed[1]);
    double y_offset = ps_plot_axis_offset(displayed[2], displayed[3]);
    if (y_offset) {
        snprintf(label, sizeof label, "Y-Offset: %.17g %s", y_offset, p.y_unit.symbol);
        report_text(a, nk_rect(area.x, r.y + 1, area.w, 18), label, UI_COLOR(muted));
    }
    for (int i = 0; i <= 4; i++) {
        double fraction = i / 4.;
        float x = area.x + (float)fraction * area.w, y = area.y + (1 - (float)fraction) * area.h;
        nk_stroke_line(canvas, area.x, y, area.x + area.w, y, 1, UI_COLOR(plot_grid));
        nk_stroke_line(canvas, x, area.y, x, area.y + area.h, 1, UI_COLOR(plot_grid_minor));
        snprintf(label, sizeof label, "%.5g",
                 plot_tick(displayed[2] - y_offset, displayed[3] - y_offset, fraction));
        report_text(a, nk_rect(r.x + 8, y - 8, 74, 20), label, UI_COLOR(muted));
        snprintf(label, sizeof label, "%.5g",
                 plot_tick(displayed[0] - x_offset, displayed[1] - x_offset, fraction));
        float tw =
            a->font_ui->width(a->font_ui->userdata, a->font_ui->height, label, (int)strlen(label));
        report_text(a, nk_rect(x - tw * .5f, area.y + area.h + 10, tw + 2, 20), label,
                    UI_COLOR(muted));
    }
    const ps_curve_data *c;
    bool reduced = false, sampled_scatter = false;
    struct nk_rect previous_clip = canvas->clip;
    nk_push_scissor(canvas, plot_intersection(area, previous_clip));
    for (uint32_t j = 0; j < p.curves; j++) {
        if (ps_report_curve_view(a->analysis_report, index, j, &c) != PS_OK)
            continue;
        struct nk_color color = report_color(j);
        reduced |= c->kind != PS_PLOT_HISTOGRAM && c->count < c->source_count;
        sampled_scatter |= c->kind == PS_PLOT_SCATTER && c->count < c->source_count;
        double previous_x = 0, previous_y = 0;
        for (uint32_t i = 0; i < c->count; i++) {
            double x = plot_fraction(view, b, 0, c->x[i]), y = plot_fraction(view, b, 1, c->y[i]);
            if (c->kind == PS_PLOT_LINE && c->count > 1) {
                if (i)
                    plot_segment(canvas, area, previous_x, previous_y, x, y, color);
            } else if (c->kind == PS_PLOT_HISTOGRAM) {
                double left = fmax(0, plot_fraction(view, b, 0, c->x[i] - c->bar_width / 2));
                double right = fmin(1, plot_fraction(view, b, 0, c->x[i] + c->bar_width / 2));
                double bottom = fmax(0, plot_fraction(view, b, 1, 0)), top = fmin(1, y);
                if (right > left && top > bottom)
                    nk_fill_rect(canvas,
                                 nk_rect(area.x + (float)left * area.w,
                                         area.y + (1 - (float)top) * area.h,
                                         fmaxf(.5f, (float)(right - left) * area.w - 1),
                                         (float)(top - bottom) * area.h),
                                 1, color);
            } else
                plot_dot(canvas, area, x, y, 2.5f, color);
            previous_x = x;
            previous_y = y;
        }
    }
    nk_push_scissor(canvas, previous_clip);
    nk_layout_row_dynamic(ui, 22, 1);
    snprintf(label, sizeof label, "%s%s%s%s%s%s", p.x_label,p.x_unit.symbol[0]?" [":"",p.x_unit.symbol,
             p.x_unit.symbol[0]?"]":"",
             ps_plot_view_changed(view) ? " · Ausschnitt" : "",
             reduced ? (sampled_scatter ? " · gleichmäßig abgetastete Vorschau"
                                        : " · reduzierte Vorschau, Spitzen bleiben erhalten")
                     : "");
    nk_label_colored(ui, label, NK_TEXT_CENTERED, UI_COLOR(muted));
    if (x_offset) {
        nk_layout_row_dynamic(ui, 20, 1);
        snprintf(label, sizeof label, "X-Offset: %.17g %s · Wert = Offset + Achsenwert", x_offset,
                 p.x_unit.symbol);
        nk_label_colored(ui, label, NK_TEXT_CENTERED, UI_COLOR(muted));
    }
    nk_layout_row_dynamic(ui, 22, 2);
    for (uint32_t j = 0; j < p.curves; j++) {
        if (ps_report_curve_view(a->analysis_report, index, j, &c) != PS_OK)
            continue;
        snprintf(label, sizeof label, "•  %s · %llu Messpunkte", c->label,
                 (unsigned long long)c->source_count);
        nk_label_colored(ui, label, NK_TEXT_LEFT, report_color(j));
    }
}
typedef enum { REPORT_CSV, REPORT_SVG, REPORT_PNG, REPORT_PNG_REGION, REPORT_SVG_REGION } report_format;
static void report_export(app *a, report_format format) {
    uint32_t plots = 0, tables = 0;
    ps_report_describe(a->analysis_report, NULL, NULL, &plots, &tables);
    char path[4096];
    unique_path(a, path, sizeof path,
                format == REPORT_SVG   ? "-diagramm.svg"
                : format == REPORT_SVG_REGION ? "-ausschnitt.svg"
                : format == REPORT_PNG_REGION ? "-ausschnitt.png"
                : format == REPORT_PNG ? "-diagramm.png"
                                       : "-ergebnis.csv");
    ps_result result;
    if ((uint32_t)a->result_view < plots &&
        (format == REPORT_PNG_REGION || format == REPORT_SVG_REGION)) {
        double full[4], region[4];
        result = ps_report_plot_bounds(a->analysis_report, (uint32_t)a->result_view, full);
        if (result == PS_OK) {
            plot_display_bounds(&a->report_views[a->result_view], full, region);
            result = format == REPORT_SVG_REGION ?
                ps_report_export_svg_region(a->analysis_report, (uint32_t)a->result_view, region, path) :
                ps_report_image_region(a->ui, a->font_ui, a->analysis_report,
                         (uint32_t)a->result_view, (unsigned)a->png_scale, region, path);
        }
    } else if ((uint32_t)a->result_view < plots)
        result =
            format == REPORT_PNG ? ps_report_image_scaled(a->ui, a->font_ui, a->analysis_report,
                                                   (uint32_t)a->result_view, (unsigned)a->png_scale, path)
            : format == REPORT_SVG
                ? ps_report_export_svg(a->analysis_report, (uint32_t)a->result_view, path)
                : ps_report_export_plot_csv(a->analysis_report, (uint32_t)a->result_view, path);
    else
        result = format != REPORT_CSV
                     ? PS_INVALID
                     : ps_report_export_table_csv(a->analysis_report,
                                                  (uint32_t)a->result_view - plots, path);
    if (result == PS_OK) {
        snprintf(a->result_export, sizeof a->result_export, "%s", path);
        log_line(a, "Export: %s\n", path);
        status(a, "Export gespeichert. Pfad im Protokoll.");
    } else {
        char error[128];
        snprintf(error, sizeof error, "Export fehlgeschlagen: %s", ps_result_string(result));
        status(a, error);
    }
}
static void report_view(app *a, float height) {
    struct nk_context *ui = a->ui;
    uint32_t plots = 0, tables = 0;
    char title[192], provenance[8192];
    ps_report_describe(a->analysis_report, title, provenance, &plots, &tables);
    nk_layout_row_dynamic(ui, 26, 1);
    nk_label(ui, title, NK_TEXT_LEFT);
    if (!plots && !tables) {
        nk_label(ui, "Die Analyse hat einen leeren Bericht erzeugt.", NK_TEXT_LEFT);
        return;
    }
    char titles[PS_REPORT_MAX_PLOTS + PS_REPORT_MAX_TABLES][224];
    const char *names[PS_REPORT_MAX_PLOTS + PS_REPORT_MAX_TABLES];
    for (uint32_t i = 0; i < plots + tables; i++) {
        if (i < plots) {
            ps_plot_info p;
            ps_report_plot_read(a->analysis_report, i, &p);
            snprintf(titles[i], sizeof titles[i], "Diagramm · %s", p.title);
        } else {
            ps_table_info t;
            ps_report_table_read(a->analysis_report, i - plots, &t);
            snprintf(titles[i], sizeof titles[i], "Tabelle · %s", t.title);
        }
        names[i] = titles[i];
    }
    if (a->result_view < 0 || (uint32_t)a->result_view >= plots + tables)
        a->result_view = 0;
    nk_layout_row_dynamic(ui, 32, 1);
    a->result_view =
        nk_combo(ui, names, (int)(plots + tables), a->result_view, 30, nk_vec2(580, 260));
    nk_layout_row_dynamic(ui, 30, 3);
    bool is_plot = (uint32_t)a->result_view < plots;
    bool cropped = is_plot && ps_plot_view_changed(&a->report_views[a->result_view]);
    a->result_export_bounds[0] = nk_widget_bounds(ui);
    if (nk_button_label(ui, is_plot ? (cropped ? "CSV · Alle Daten" : "CSV · Diagrammdaten")
                                    : "Tabelle als CSV"))
        report_export(a, REPORT_CSV);
    nk_begin_disabled(ui, !is_plot);
    a->result_export_bounds[1] = nk_widget_bounds(ui);
    if (nk_button_label(ui, cropped ? "SVG · Gesamt" : "SVG · Diagramm"))
        report_export(a, REPORT_SVG);
    a->result_export_bounds[2] = nk_widget_bounds(ui);
    if (nk_button_label(ui, cropped ? "PNG · Gesamt" : "PNG · Diagramm"))
        report_export(a, REPORT_PNG);
    nk_end_disabled(ui);
    if (is_plot) {
        const char *sizes[] = {"1200 × 850", "2400 × 1700", "3600 × 2550", "4800 × 3400"};
        float available=nk_window_get_content_region(ui).w;
        bool compact=available<740;
        nk_layout_row_begin(ui,NK_STATIC,26,compact?2:4);
        nk_layout_row_push(ui, 150);
        nk_label(ui, "PNG-Größe (Pixel)", NK_TEXT_LEFT);
        nk_layout_row_push(ui,compact?fmaxf(120,fminf(230,available-158)):230);
        a->png_size_bounds = nk_widget_bounds(ui);
        if (nk_combo_begin_label(ui, sizes[a->png_scale - 1], nk_vec2(230, 145))) {
            nk_layout_row_dynamic(ui, 28, 1);
            for (int i = 0; i < 4; i++) {
                a->png_size_choices[i] = nk_widget_bounds(ui);
                if (nk_combo_item_label(ui, sizes[i], NK_TEXT_LEFT)) a->png_scale = i + 1;
            }
            nk_combo_end(ui);
        }
        if(compact){nk_layout_row_end(ui);nk_layout_row_dynamic(ui,30,2);}
        else nk_layout_row_push(ui,160);
        nk_begin_disabled(ui, !cropped);
        a->png_region_bounds = nk_widget_bounds(ui);
        if (nk_button_label(ui, "PNG · Ausschnitt")) report_export(a, REPORT_PNG_REGION);
        if(!compact)nk_layout_row_push(ui,160);
        a->svg_region_bounds = nk_widget_bounds(ui);
        if (nk_button_label(ui, "SVG · Ausschnitt")) report_export(a, REPORT_SVG_REGION);
        nk_end_disabled(ui);
        if(!compact)nk_layout_row_end(ui);
        ps_plot_info selected;
        ps_report_plot_read(a->analysis_report, (uint32_t)a->result_view, &selected);
        float legend_height = (float)((selected.curves + 1) / 2) * 30;
        report_plot(a, (uint32_t)a->result_view, fmaxf(170, height - 464 - legend_height));
    } else {
        uint32_t index = (uint32_t)a->result_view - plots;
        ps_table_info table;
        ps_report_table_read(a->analysis_report, index, &table);
        nk_layout_row_dynamic(ui, fmaxf(180, height - 295), 1);
        if (nk_group_begin(ui, "report-table", 0)) {
            int column_width = (int)fmaxf(128, (nk_window_get_content_region(ui).w - 24) /
                                                   (float)(table.columns + 1));
            nk_layout_row_static(ui, 54, column_width, (int)table.columns + 1);
            nk_label(ui, "Messreihe", NK_TEXT_LEFT);
            for (uint32_t j = 0; j < table.columns; j++) {
                char label[176];
                snprintf(label, sizeof label, "%s [%s]", table.column[j].label,
                         table.column[j].unit.symbol);
                nk_label_wrap(ui, label);
            }
            nk_layout_row_static(ui, 30, column_width, (int)table.columns + 1);
            for (uint32_t i = 0; i < table.rows; i++) {
                ps_table_row row;
                ps_report_row_read(a->analysis_report, index, i, &row);
                nk_label(ui, row.label, NK_TEXT_LEFT);
                for (uint32_t j = 0; j < table.columns; j++) {
                    char value[64];
                    snprintf(value, sizeof value, "%.9g", row.values[j]);
                    nk_label(ui, value, NK_TEXT_LEFT);
                }
            }
            nk_group_end(ui);
        }
    }
    if (nk_tree_push(ui, NK_TREE_TAB, "Herkunft des Ergebnisses", NK_MINIMIZED)) {
        nk_layout_row_dynamic(ui, 180, 1);
        nk_label_wrap(ui, provenance);
        nk_tree_pop(ui);
    }
}
