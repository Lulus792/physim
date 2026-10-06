typedef struct {
    const char *title, *path;
    bool code;
} documentation_topic;
static const documentation_topic documentation_topics[] = {
    {"Schnellstart", "README.md", false},
    {"Start & Lernpfade", "docs/guide.md", false},
    {"Numerische Verfahren", "docs/numerics.md", false},
    {"Datenreihen & Analyse", "docs/series.md", false},
    {"Bauen & Prüfen", "docs/build.md", false},
    {"Dateiformat", "docs/data-format.md", false},
    {"Designrichtung", "docs/design-direction.md", false},
    {"Projektplan", "Physim_Projektplan.md", false},
    {"Diagramme & Tabellen", "docs/reports.md", false},
    {"Läufe & Berichte", "docs/runs.md", false},
    {"Mechanik & Medien", "docs/mechanics.md", false},
    {"core.h", "include/physim/core.h", true},
    {"experiment.h", "include/physim/experiment.h", true},
    {"numerics.h", "include/physim/numerics.h", true},
    {"units.h", "include/physim/units.h", true},
    {"series.h", "include/physim/series.h", true},
    {"data.h", "include/physim/data.h", true},
    {"analysis.h", "include/physim/analysis.h", true},
    {"report.h", "include/physim/report.h", true},
    {"mechanics.h", "include/physim/mechanics.h", true},
    {"Umsetzungsstand", "docs/status.md", false},
    {"Foundation", "docs/decisions/001-foundation.md", false},
    {"Renderer", "docs/decisions/002-opengl-renderer.md", false},
    {"MIT-Lizenz", "LICENSE", true},
    {"Autosave & Wiederherstellung", "docs/autosave.md", false},
    {"Feder–Masse–Dämpfer", "docs/spring.md", false},
    {"Monte Carlo & Unsicherheit", "docs/monte-carlo.md", false},
    {"measurement.h", "include/physim/measurement.h", true},
    {"Messungen & Sensoren", "docs/measurement.md", false},
    {"Darstellung & Einstellungen", "docs/settings.md", false},
    {"Vektoren & Transformationen", "docs/math.md", false},
    {"math.h", "include/physim/math.h", true},
    {"Speicherverwaltung & Arenen", "docs/memory.md", false},
    {"memory.h", "include/physim/memory.h", true},
    {"Auftrieb", "docs/buoyancy.md", false},
    {"Dynamische Arrays", "docs/array.md", false},
    {"array.h", "include/physim/array.h", true},
    {"String-Views", "docs/string-view.md", false},
    {"string_view.h", "include/physim/string_view.h", true},
    {"Hashmap", "docs/hashmap.md", false},
    {"hashmap.h", "include/physim/hashmap.h", true},
    {"collision.h", "include/physim/collision.h", true},
    {"Physim-Sprache", "docs/language.md", false},
    {"Arbeitsbereich bedienen", "docs/workspace.md", false},
    {"Eigenes Experiment in C", "docs/experiment-tutorial.md", false},
    {"API-Überblick", "docs/api.md", false},
    {"Fehler finden", "docs/troubleshooting.md", false},
    {"Werte, Arrays & Methoden", "docs/language-values.md", false},
    {"Sprachfunktionen A–Z", "docs/reference/language-library.md", false},
    {"C: Grundlagen & Zufall", "docs/reference/core.md", false},
    {"C: Experiment & Szene", "docs/reference/experiment.md", false},
    {"C: Numerik", "docs/reference/numerics.md", false},
    {"C: Einheiten", "docs/reference/units.md", false},
    {"C: Datenreihen", "docs/reference/series.md", false},
    {"C: Dateien & Daten", "docs/reference/data.md", false},
    {"C: Statistik & Analyse", "docs/reference/analysis.md", false},
    {"C: Berichte", "docs/reference/report.md", false},
    {"C: Mechanik", "docs/reference/mechanics.md", false},
    {"C: Kollisionserkennung", "docs/reference/collision.md", false},
    {"C: Messungen", "docs/reference/measurement.md", false},
    {"C: Vektoren & Matrizen", "docs/reference/math.md", false},
    {"C: Speicher", "docs/reference/memory.md", false},
    {"C: Arrays", "docs/reference/array.md", false},
    {"C: String-Views", "docs/reference/string_view.md", false},
    {"C: Hashmap", "docs/reference/hashmap.md", false},
    {"Leistungsmessungen", "docs/performance.md", false},
    {"UI-Rendering", "docs/ui-rendering.md", false},
    {"Fuzzing", "docs/fuzzing.md", false},
    {"CRC-Prüfungen", "docs/crc.md", false},
    {"UI-Zustände", "docs/ui-states.md", false},
    {"Eigenes Experiment in Physim", "docs/language-tutorial.md", false},
    {"Wurfparabel: C & Physim", "docs/projectile-tutorial.md", false},
    {"Wurf mit Luftwiderstand", "docs/projectile-drag-tutorial.md", false},
    {"Mehrlaufanalyse in Physim", "docs/language-analysis-tutorial.md", false},
    {"Plattformprüfung", "docs/platform-validation.md", false},
    {"snapshot.h", "include/physim/snapshot.h", true},
    {"C: Aufgezeichnete Szenen", "docs/reference/snapshot.md", false},
    {"Historische lokale Prüfungen", "docs/status-history.md", false},
    {"Logging", "docs/logging.md", false},
    {"log.h", "include/physim/log.h", true},
    {"C: Logging", "docs/reference/log.md", false},
    {"Lokale Szenenkoordinaten", "docs/scene-frames.md", false},
    {"Strukturierte Diagnosen", "docs/diagnostics.md", false},
    {"diagnostic.h", "include/physim/diagnostic.h", true},
    {"C: Diagnosen", "docs/reference/diagnostic.md", false},
    {"Indizierte Laufdateien", "docs/run-index.md", false},
    {"run_index.h", "include/physim/run_index.h", true},
    {"C: Laufdatei-Index", "docs/reference/run_index.md", false}};
#define DOCUMENTATION_MONTE_CARLO_TOPIC 26
#define DOCUMENTATION_MEASUREMENT_HEADER 27
#define DOCUMENTATION_MEASUREMENT_TOPIC 28
#define DOCUMENTATION_TOPIC_COUNT                                                                  \
    ((int)(sizeof documentation_topics / sizeof documentation_topics[0]))
static char *documentation_index[DOCUMENTATION_TOPIC_COUNT];
static bool documentation_hits[DOCUMENTATION_TOPIC_COUNT];
static int documentation_group(int topic) {
    if (!strncmp(documentation_topics[topic].path, "docs/reference/", 15)) return 2;
    if (documentation_topics[topic].code &&
        !strncmp(documentation_topics[topic].path, "include/", 8)) return 4;
    if (!strcmp(documentation_topics[topic].path, "docs/language-tutorial.md") ||
        !strcmp(documentation_topics[topic].path, "docs/language-analysis-tutorial.md") ||
        !strcmp(documentation_topics[topic].path, "docs/projectile-tutorial.md") ||
        !strcmp(documentation_topics[topic].path, "docs/projectile-drag-tutorial.md"))
        return 0;
    if (topic == 23 || topic >= 65)
        return 3;
    if (documentation_topics[topic].code)
        return 4;
    if (topic >= 48)
        return 2;
    if (topic == 4 || topic == 5 || topic == 6 || topic == 7 || (topic >= 20 && topic <= 22))
        return 3;
    if (topic == 0 || topic == 1 || topic == 24 || topic == 29 || topic == 43 || topic == 44 ||
        topic == 46)
        return 0;
    return 1;
}
static void documentation_filter(app *a) {
    for (int i = 0; i < DOCUMENTATION_TOPIC_COUNT; i++) {
        if (a->doc_filter[0] && !documentation_topics[i].code && !documentation_index[i]) {
            char path[4096];
            size_t length = 0;
            join(path, sizeof path, a->root, documentation_topics[i].path);
            documentation_index[i] = load_utf8_text(path, &length);
        }
        documentation_hits[i] = !a->doc_filter[0] ||
                                ps_document_find(documentation_topics[i].title, a->doc_filter) ||
                                ps_document_find(documentation_index[i], a->doc_filter);
    }
    snprintf(a->doc_last_filter, sizeof a->doc_last_filter, "%s", a->doc_filter);
}
static void open_documentation(app *a, int topic) {
    if (topic < 0 || topic >= DOCUMENTATION_TOPIC_COUNT)
        return;
    if (!documentation_window_open(a))
        return;
    if (a->documentation && topic == a->doc_topic) {
        a->doc_error[0] = 0;
        return;
    }
    char path[4096];
    join(path, sizeof path, a->root, documentation_topics[topic].path);
    size_t length = 0;
    char *source = load_utf8_text(path, &length);
    ps_document *document = NULL;
    ps_result r = PS_IO;
    if (source)
        r = ps_document_parse(source, length, documentation_topics[topic].code, &document);
    free(source);
    if (r != PS_OK) {
        snprintf(a->doc_error, sizeof a->doc_error, "Dokument konnte nicht geladen werden: %s",
                 ps_result_string(r));
        status(a, a->doc_error);
        return;
    }
    a->doc_previous = a->documentation ? a->doc_topic : -1;
    free(a->documentation);
    a->documentation = document;
    a->doc_topic = topic;
    a->doc_error[0] = 0;
    a->doc_scroll_x = a->doc_scroll_y = 0;
    a->doc_match = 0;
    a->doc_move_match = true;
    a->doc_jump_block = -1;
    a->doc_group = documentation_group(topic);
}
static size_t documentation_line(const struct nk_user_font *font, const char *s, size_t n,
                                 float width, size_t *length) {
    size_t at = 0, last_space = SIZE_MAX;
    float used = 0;
    while (at < n) {
        if (s[at] == '\n') {
            *length = at;
            return at + 1;
        }
        nk_rune rune;
        int bytes = nk_utf_decode(s + at, &rune, (int)(n - at));
        if (bytes <= 0)
            bytes = 1;
        float glyph = font->width(font->userdata, font->height, s + at, bytes);
        if (at && used + glyph > width) {
            if (last_space != SIZE_MAX && last_space > 0) {
                *length = last_space;
                return last_space + 1;
            }
            *length = at;
            return at;
        }
        if (s[at] == ' ')
            last_space = at;
        used += glyph;
        at += (size_t)bytes;
    }
    *length = n;
    return n;
}
static float documentation_height(const struct nk_user_font *font, const char *text, size_t n,
                                  float width) {
    size_t at = 0;
    unsigned lines = 0;
    do {
        size_t length;
        size_t used = documentation_line(font, text + at, n - at, width, &length);
        at += used;
        lines++;
    } while (at < n);
    return lines * (font->height + 6);
}
/* Table columns remain real wrapped cells instead of exposing Markdown pipes. */
static int documentation_cells(const char *text, const char **cells, size_t *lengths) {
    int count = 0;
    const char *p = text;
    if (*p == '|')
        p++;
    while (*p && count < 16) {
        const char *end = strchr(p, '|');
        if (!end)
            end = p + strlen(p);
        const char *trim = end;
        while (p < trim && (*p == ' ' || *p == '\t'))
            p++;
        while (trim > p && (trim[-1] == ' ' || trim[-1] == '\t'))
            trim--;
        cells[count] = p;
        lengths[count++] = (size_t)(trim - p);
        p = *end ? end + 1 : end;
    }
    return count;
}
static float documentation_table(app *a, const ps_doc_block *block, const char *text, float width,
                                 const struct nk_rect *bounds) {
    const char *cells[16];
    size_t lengths[16];
    int count = documentation_cells(text, cells, lengths);
    if (!count)
        return 24;
    float cell_width = width / count, height = 24;
    const struct nk_user_font *font = a->doc_font_ui;
    for (int i = 0; i < count; i++)
        height =
            fmaxf(height,
                  documentation_height(font, cells[i], lengths[i], fmaxf(1, cell_width - 20)) + 16);
    if (!bounds)
        return height;
    struct nk_command_buffer *canvas = nk_window_get_canvas(a->doc_ui);
    nk_fill_rect(canvas, *bounds, 4, block->level ? UI_COLOR(doc_header) : UI_COLOR(doc_body));
    for (int i = 0; i < count; i++) {
        size_t at = 0;
        float y = bounds->y + 8;
        do {
            size_t length, used = documentation_line(font, cells[i] + at, lengths[i] - at,
                                                     fmaxf(1, cell_width - 20), &length);
            const char *match = ps_document_find(cells[i] + at, a->doc_query);
            nk_draw_text(
                canvas,
                nk_rect(bounds->x + i * cell_width + 10, y, cell_width - 20, font->height + 6),
                cells[i] + at, (int)length, font, nk_rgba(0, 0, 0, 0),
                match && match < cells[i] + at + length ? UI_COLOR(accent)
                                                        : UI_COLOR(doc_text));
            at += used;
            y += font->height + 6;
        } while (at < lengths[i]);
    }
    return height;
}
static void documentation_link(app *a, const char *target) {
    if (!strncmp(target, "https://", 8) || !strncmp(target, "http://", 7)) {
        SDL_OpenURL(target);
        return;
    }
    char resolved[4096];
    const char *base = documentation_topics[a->doc_topic].path;
    const char *slash = strrchr(base, '/');
    snprintf(resolved, sizeof resolved, "%.*s%s", slash ? (int)(slash - base + 1) : 0, base,
             target);
    /* Resolve the few relative parent components without consulting the filesystem. */
    char *parent;
    while ((parent = strstr(resolved, "/../")) != NULL) {
        char *start = parent;
        while (start > resolved && start[-1] != '/')
            --start;
        memmove(start, parent + 4, strlen(parent + 4) + 1);
    }
    for (int i = 0; i < DOCUMENTATION_TOPIC_COUNT; i++) {
        if (!strcmp(resolved, documentation_topics[i].path)) {
            open_documentation(a, i);
            return;
        }
    }
    status(a, "Dieses Linkziel ist nicht in der mitgelieferten Dokumentation enthalten.");
}
static void documentation_sidebar(app *a) {
    struct nk_context *ui = a->doc_ui;
    nk_layout_row_dynamic(ui, 28, 1);
    nk_label_colored(ui, "PHYSIM HANDBUCH", NK_TEXT_LEFT, UI_COLOR(muted));
    nk_layout_row_dynamic(ui, 30, 2);
    if (nk_button_label(ui, "Themen"))
        a->doc_contents = false;
    if (nk_button_label(ui, "Inhalt"))
        a->doc_contents = true;
    if (a->doc_contents && a->documentation) {
        a->doc_filter_active = false;
        nk_layout_row_dynamic(ui, 44, 1);
        nk_label_wrap(ui, documentation_topics[a->doc_topic].title);
        for (size_t i = 0; i < a->documentation->count; i++) {
            ps_doc_block *b = &a->documentation->blocks[i];
            if (b->kind != PS_DOC_HEADING || b->level > 2)
                continue;
            nk_layout_row_dynamic(ui, 36, 1);
            if (nk_button_label(ui, a->documentation->text + b->text))
                a->doc_jump_block = (int)i;
        }
        return;
    }
    nk_layout_row_dynamic(ui, 24, 1);
    nk_label(ui, "Alle Inhalte durchsuchen", NK_TEXT_LEFT);
    nk_layout_row_dynamic(ui, 32, 1);
    nk_flags edit = nk_edit_string_zero_terminated(ui, NK_EDIT_FIELD, a->doc_filter,
                                                   sizeof a->doc_filter, nk_filter_default);
    a->doc_filter_active = (edit & NK_EDIT_ACTIVE) != 0;
    if (a->doc_filter_active)
        a->doc_search_active = false;
    if (strcmp(a->doc_filter, a->doc_last_filter))
        documentation_filter(a);
    const char *groups[] = {"Lernen & Bedienung", "Fachthemen & Beispiele", "Funktionsreferenz",
                            "Projekt & Technik", "Header (optional)"};
    nk_layout_row_dynamic(ui, 32, 1);
    if (a->doc_filter[0]) {
        if (nk_button_label(ui, "Suche zurücksetzen")) {
            a->doc_filter[0] = 0;
            documentation_filter(a);
        }
    } else
        a->doc_group = nk_combo(ui, groups, 5, a->doc_group, 30, nk_vec2(260, 190));
    int found = 0;
    for (int i = 0; i < DOCUMENTATION_TOPIC_COUNT; i++) {
        if (a->doc_filter[0] ? (!documentation_hits[i] || documentation_topics[i].code)
                             : documentation_group(i) != a->doc_group)
            continue;
        found++;
        nk_layout_row_dynamic(ui, 34, 1);
        bool selected = a->documentation && a->doc_topic == i;
        if (nk_selectable_label(ui, documentation_topics[i].title, NK_TEXT_LEFT, &selected)) {
            open_documentation(a, i);
            if (a->doc_filter[0]) {
                snprintf(a->doc_query, sizeof a->doc_query, "%s", a->doc_filter);
                a->doc_move_match = true;
            }
        }
    }
    if (!found) {
        nk_layout_row_dynamic(ui, 76, 1);
        nk_label_wrap(ui,
                      "Keine Treffer. Suche nach einem Begriff wie Sensor, Export oder ps_scene.");
    }
}
static void documentation_view(app *a, float height) {
    struct nk_context *ui = a->doc_ui;
    nk_layout_row_dynamic(ui, 32, 3);
    nk_begin_disabled(ui, a->doc_previous < 0);
    if (nk_button_label(ui, "Zurück"))
        open_documentation(a, a->doc_previous);
    nk_end_disabled(ui);
    if (nk_button_label(ui, "Start"))
        open_documentation(a, 1);
    if (nk_button_label(ui, "Arbeitsbereich"))
        SDL_RaiseWindow(a->window);
    nk_layout_row_begin(ui, NK_DYNAMIC, 32, 3);
    nk_layout_row_push(ui, .60f);
    if (a->doc_search_focus) {
        nk_edit_focus(ui, NK_EDIT_ALWAYS_INSERT_MODE);
        a->doc_search_focus = false;
    }
    struct nk_rect search_bounds = nk_widget_bounds(ui);
    nk_flags search_state = nk_edit_string_zero_terminated(ui, NK_EDIT_FIELD, a->doc_query,
                                                           sizeof a->doc_query, nk_filter_default);
    a->doc_search_active = (search_state & NK_EDIT_ACTIVE) != 0;
    if (!a->doc_query[0] && !(search_state & NK_EDIT_ACTIVE)) {
        const char *placeholder = "Dieses Dokument durchsuchen …";
        nk_draw_text(nk_window_get_canvas(ui),
                     nk_rect(search_bounds.x + 10, search_bounds.y + 8, search_bounds.w - 20, 20),
                     placeholder, (int)strlen(placeholder), a->doc_font_ui, nk_rgba(0, 0, 0, 0),
                     UI_COLOR(muted));
    }
    nk_layout_row_push(ui, .20f);
    if (nk_button_label(ui, "Vorheriger")) {
        a->doc_match--;
        a->doc_move_match = true;
    }
    nk_layout_row_push(ui, .20f);
    if (nk_button_label(ui, "Nächster")) {
        a->doc_match++;
        a->doc_move_match = true;
    }
    nk_layout_row_end(ui);
    if (strcmp(a->doc_last_query, a->doc_query)) {
        snprintf(a->doc_last_query, sizeof a->doc_last_query, "%s", a->doc_query);
        a->doc_match = 0;
        a->doc_move_match = true;
    }
    ps_document *d = a->documentation;
    int matches[PS_DOC_MAX_BLOCKS], match_count = 0;
    if (d && a->doc_query[0])
        for (size_t i = 0; i < d->count; i++)
            if (ps_document_find(d->text + d->blocks[i].text, a->doc_query))
                matches[match_count++] = (int)i;
    if (match_count)
        a->doc_match = (a->doc_match % match_count + match_count) % match_count;
    else
        a->doc_match = 0;
    nk_layout_row_dynamic(ui, 24, 1);
    char detail[192];
    if (a->doc_error[0])
        snprintf(detail, sizeof detail, "%s", a->doc_error);
    else if (a->doc_query[0])
        snprintf(detail, sizeof detail, "%s · %d Abschnitte mit Treffern%s",
                 d ? documentation_topics[a->doc_topic].title : "", match_count,
                 match_count ? " · blau markiert" : "");
    else
        snprintf(detail, sizeof detail, "%s · offline verfügbar · " PS_UI_SHORTCUT "F sucht in diesem Dokument",
                 d ? documentation_topics[a->doc_topic].title : "Dokument nicht verfügbar");
    nk_label_colored(ui, detail, NK_TEXT_LEFT, UI_COLOR(muted));
    nk_layout_row_dynamic(ui, height - 146, 1);
    if (nk_group_scrolled_offset_begin(ui, &a->doc_scroll_x, &a->doc_scroll_y, "document-body",
                                       0)) {
        if (d) {
            float width = fmaxf(40, nk_window_get_content_region(ui).w - 8);
            if (d->layout_width != width) {
                for (size_t i = 0; i < d->count; i++) {
                    ps_doc_block *b = &d->blocks[i];
                    const struct nk_user_font *font = b->kind == PS_DOC_CODE ? a->doc_font_code
                                                      : b->kind == PS_DOC_HEADING
                                                          ? a->doc_font_title
                                                          : a->doc_font_ui;
                    b->height =
                        b->kind == PS_DOC_TABLE_ROW
                            ? documentation_table(a, b, d->text + b->text, width, NULL)
                        : b->kind == PS_DOC_RULE ? 14
                        : b->kind == PS_DOC_LINK
                            ? 32
                            : documentation_height(font, d->text + b->text, b->length, width - 24) +
                                  16;
                }
                d->layout_width = width;
            }
            int target = a->doc_jump_block >= 0
                             ? a->doc_jump_block
                             : (a->doc_move_match && match_count ? matches[a->doc_match] : -1);
            float offset = 0;
            /* Resolve the scroll position before submitting any widgets. Updating
             * it midway through drawing mixes two offsets in the same frame. */
            for (size_t i = 0; target >= 0 && i < d->count && (int)i <= target; i++) {
                ps_doc_block *b = &d->blocks[i];
                const char *text = d->text + b->text;
                const struct nk_user_font *font = b->kind == PS_DOC_CODE      ? a->doc_font_code
                                                  : b->kind == PS_DOC_HEADING ? a->doc_font_title
                                                                              : a->doc_font_ui;
                if ((int)i == target) {
                    float prefix = 0;
                    const char *match = ps_document_find(text, a->doc_query);
                    if (b->kind != PS_DOC_TABLE_ROW && a->doc_jump_block < 0 && match &&
                        match > text)
                        prefix = fmaxf(0, documentation_height(font, text, (size_t)(match - text),
                                                               width - 24) -
                                              (font->height + 6));
                    a->doc_scroll_y = (nk_uint)(offset + prefix);
                }
                offset += b->height + 8 + (b->kind == PS_DOC_CODE ? 36 : 0);
            }
            for (size_t i = 0; i < d->count; i++) {
                ps_doc_block *b = &d->blocks[i];
                const char *text = d->text + b->text;
                const struct nk_user_font *font = b->kind == PS_DOC_CODE      ? a->doc_font_code
                                                  : b->kind == PS_DOC_HEADING ? a->doc_font_title
                                                                              : a->doc_font_ui;
                if (b->kind == PS_DOC_CODE) {
                    nk_layout_row_dynamic(ui, 28, 2);
                    nk_label_colored(ui, "Code", NK_TEXT_LEFT, UI_COLOR(muted));
                    if (nk_button_label(ui, "Code kopieren"))
                        SDL_SetClipboardText(text);
                }
                nk_layout_row_dynamic(ui, b->height, 1);
                if (b->kind == PS_DOC_LINK) {
                    bool external = !strncmp(d->text + b->target, "http", 4);
                    char label[240];
                    snprintf(label, sizeof label, "%s%s",
                             external ? "Im Browser öffnen: " : "Öffnen: ", text);
                    if (nk_button_label(ui, label)) {
                        /* Loading another document frees d; stop traversal immediately. */
                        documentation_link(a, d->text + b->target);
                        break;
                    }
                } else {
                    struct nk_rect rect;
                    enum nk_widget_layout_states state = nk_widget(&rect, ui);
                    if (state != NK_WIDGET_INVALID) {
                        struct nk_command_buffer *canvas = nk_window_get_canvas(ui);
                        if (b->kind == PS_DOC_TABLE_ROW)
                            documentation_table(a, b, text, width, &rect);
                        else if (b->kind == PS_DOC_RULE)
                            nk_stroke_line(canvas, rect.x, rect.y + 7, rect.x + rect.w, rect.y + 7,
                                           1, UI_COLOR(border));
                        else {
                            if (b->kind == PS_DOC_CODE)
                                nk_fill_rect(canvas, rect, 7, UI_COLOR(edit));
                            size_t at = 0;
                            float y = rect.y + 8;
                            do {
                                size_t length,
                                    used = documentation_line(font, text + at, b->length - at,
                                                              width - 24, &length);
                                const char *match = ps_document_find(text + at, a->doc_query);
                                struct nk_color color = match && match < text + at + length
                                                            ? UI_COLOR(accent)
                                                            : UI_COLOR(doc_text);
                                nk_draw_text(
                                    canvas, nk_rect(rect.x + 12, y, rect.w - 24, font->height + 6),
                                    text + at, (int)length, font, nk_rgba(0, 0, 0, 0), color);
                                at += used;
                                y += font->height + 6;
                            } while (at < b->length);
                        }
                    }
                }
            }
            a->doc_jump_block = -1;
            a->doc_move_match = false;
        }
        nk_group_scrolled_end(ui);
    }
}
