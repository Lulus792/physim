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
    if(!view->initialized)ps_plot_view_reset(view);
    analysis_tools_layout controls=analysis_tools_measure(a,nk_window_get_content_region(ui).w,4);
    nk_layout_row_dynamic(ui,controls.height+5*a->preferences.ui_size/16.f,1);
    struct nk_rect tools;nk_widget(&tools,ui);
    int action=analysis_tools_draw(a,&controls,tools.x,tools.y,bounds_result==PS_OK);
    if(action==3)graphic_export_report(a,index);
    else if(action>=0){if(action==2)ps_plot_view_reset(view);else ps_plot_view_zoom(view,action==0?2:.5,.5,.5);a->plot_drag=NULL;}
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
                    "Dieses Diagramm enthält keine gültigen Messpunkte.", UI_COLOR(muted));
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
        const uint8_t *mask=NULL;ps_report_curve_mask(a->analysis_report,index,j,&mask);
        struct nk_color color = report_color(j);
        reduced |= c->kind != PS_PLOT_HISTOGRAM && c->count < c->source_count;
        sampled_scatter |= c->kind == PS_PLOT_SCATTER && c->count < c->source_count;
        double previous_x = 0, previous_y = 0;
        for (uint32_t i = 0; i < c->count; i++) {
            if(mask && !(mask[i]&1))continue;
            double x = plot_fraction(view, b, 0, c->x[i]), y = plot_fraction(view, b, 1, c->y[i]);
            if (c->kind == PS_PLOT_LINE && c->count > 1) {
                bool before=i && (!mask || ((mask[i-1]&1) && !(mask[i]&2)));
                bool after=i+1<c->count && (!mask || ((mask[i+1]&1) && !(mask[i+1]&2)));
                if(!before && !after)plot_dot(canvas,area,x,y,2.5f,color);
                if (before)
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
    char titles[PS_REPORT_MAX_PLOTS + PS_REPORT_MAX_TABLES + 1][224];
    const char *names[PS_REPORT_MAX_PLOTS + PS_REPORT_MAX_TABLES + 1];
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
    names[plots + tables] = "Herkunft & Quellen";
    if (a->result_view < 0 || (uint32_t)a->result_view > plots + tables)
        a->result_view = 0;
    nk_layout_row_begin(ui, NK_DYNAMIC, 32, 2);
    nk_layout_row_push(ui, .88f);
    a->result_view =
        nk_combo(ui, names, (int)(plots + tables + 1), a->result_view, 30, nk_vec2(580, 260));
    nk_layout_row_push(ui, .12f);
    a->report_details_bounds = nk_widget_bounds(ui);
    if (action_button(ui, "Details", (uint32_t)a->result_view == plots + tables))
        a->result_view = (int)(plots + tables);
    if (nk_input_is_mouse_hovering_rect(&ui->input, a->report_details_bounds))
        nk_tooltip(ui, "Herkunft, archivierte Quellkopie und Eingabemanifest");
    nk_layout_row_end(ui);
    if ((uint32_t)a->result_view == plots + tables) {
        if (!plots && !tables) {
            nk_layout_row_dynamic(ui, 28, 1);
            nk_label_colored(ui, "Der Bericht enthält keine Diagramme oder Tabellen.", NK_TEXT_LEFT, UI_COLOR(muted));
        }
        report_details_view(a, provenance, height);
        return;
    }
    bool is_plot = (uint32_t)a->result_view < plots;
    /* Data export stays separate from the graphic dialog. */
    nk_layout_row_static(ui,28,172,1);
    a->result_export_bounds[0]=nk_widget_bounds(ui);
    if(nk_button_label(ui,is_plot?"CSV · Diagrammdaten":"Tabelle als CSV"))report_export(a,REPORT_CSV);
    if (is_plot) {
        ps_plot_info selected;
        ps_report_plot_read(a->analysis_report, (uint32_t)a->result_view, &selected);
        float legend_height = (float)((selected.curves + 1) / 2) * 30;
        report_plot(a, (uint32_t)a->result_view, fmaxf(170, height - 348 - legend_height));
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
}
