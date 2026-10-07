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
    {"C: Laufdatei-Index", "docs/reference/run_index.md", false},
    {"Persistente Kontakte & Warmstart", "docs/contact-world.md", false},
    {"contact_world.h", "include/physim/contact_world.h", true},
    {"C: Kontaktzustand", "docs/reference/contact_world.md", false},
    {"Eigenes Material & Medium", "docs/material-tutorial.md", false},
    {"Pendel & Integratoren", "docs/pendulum-tutorial.md", false},
    {"Elastischer & inelastischer Stoß", "docs/collision-tutorial.md", false},
    {"Monte Carlo: Anfangswerte", "docs/monte-carlo-tutorial.md", false},
    {"Gespeicherten Lauf auswerten", "docs/saved-run-tutorial.md", false},
    {"Projektanforderungen & Abnahme", "docs/project-audit.md", false},
    {"Batch & Monte Carlo in Physim", "docs/batch-language.md", false},
    {"batch.h", "include/physim/batch.h", true},
    {"C: Batch-Hostdienste", "docs/reference/batch.md", false},
    {"Strömung: Rohrnetze & Tracer", "docs/fluid.md", false},
    {"fluid.h", "include/physim/fluid.h", true},
    {"C: Strömung", "docs/reference/fluid.md", false},
    {"Wellen, Saiten & geometrische Optik", "docs/waves-optics.md", false},
    {"waves.h", "include/physim/waves.h", true},
    {"optics.h", "include/physim/optics.h", true},
    {"C: Wellen", "docs/reference/waves.md", false},
    {"C: Optik", "docs/reference/optics.md", false},
    {"Elektromagnetismus & RC", "docs/electromagnetism.md", false},
    {"electromagnetism.h", "include/physim/electromagnetism.h", true},
    {"C: Elektromagnetismus", "docs/reference/electromagnetism.md", false},
    {"Thermodynamik: ideales Gas & Wärme", "docs/thermodynamics.md", false},
    {"thermodynamics.h", "include/physim/thermodynamics.h", true},
    {"C: Thermodynamik", "docs/reference/thermodynamics.md", false},
    {"Teil I – Physim mit C", "docs/c-guide.md", false},
    {"Teil II – Physim-Sprache", "docs/physim-guide.md", false},
    {"C-Einstieg für den Workflow", "docs/c-workflow.md", false},
    {"Physim-Einstieg: Syntax & Werte", "docs/physim-workflow.md", false},
    {"Materialdaten & Gültigkeitsbereiche", "docs/properties.md", false},
    {"properties.h", "include/physim/properties.h", true},
    {"C: Materialeigenschaften", "docs/reference/properties.md", false},
};
#define DOCUMENTATION_MONTE_CARLO_TOPIC 26
#define DOCUMENTATION_MEASUREMENT_HEADER 27
#define DOCUMENTATION_MEASUREMENT_TOPIC 28
#define DOCUMENTATION_TOPIC_COUNT                                                                  \
    ((int)(sizeof documentation_topics / sizeof documentation_topics[0]))
static int documentation_learning_topic(unsigned track) {
    const char *path=track==1?"docs/c-guide.md":"docs/physim-guide.md";
    for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++)
        if(!strcmp(documentation_topics[i].path,path))return i;
    return 1;
}
#define DOCUMENTATION_C_GUIDE_TOPIC documentation_learning_topic(1)
#define DOCUMENTATION_PHYSIM_GUIDE_TOPIC documentation_learning_topic(2)
static char *documentation_index[DOCUMENTATION_TOPIC_COUNT];
static bool documentation_hits[DOCUMENTATION_TOPIC_COUNT];
static int documentation_group(int topic) {
    if(!strcmp(documentation_topics[topic].path,"docs/c-guide.md") ||
       !strcmp(documentation_topics[topic].path,"docs/physim-guide.md") ||
       !strcmp(documentation_topics[topic].path,"docs/c-workflow.md") ||
       !strcmp(documentation_topics[topic].path,"docs/physim-workflow.md"))return 0;
    if (!strncmp(documentation_topics[topic].path, "docs/reference/", 15)) return 2;
    if (documentation_topics[topic].code &&
        !strncmp(documentation_topics[topic].path, "include/", 8)) return 4;
    if (!strcmp(documentation_topics[topic].path, "docs/language-tutorial.md") ||
        !strcmp(documentation_topics[topic].path, "docs/language-analysis-tutorial.md") ||
        !strcmp(documentation_topics[topic].path, "docs/projectile-tutorial.md") ||
        !strcmp(documentation_topics[topic].path, "docs/projectile-drag-tutorial.md") ||
        !strcmp(documentation_topics[topic].path, "docs/pendulum-tutorial.md") ||
        !strcmp(documentation_topics[topic].path, "docs/collision-tutorial.md") ||
        !strcmp(documentation_topics[topic].path, "docs/monte-carlo-tutorial.md") ||
        !strcmp(documentation_topics[topic].path, "docs/saved-run-tutorial.md"))
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
        if(topic==DOCUMENTATION_C_GUIDE_TOPIC)a->doc_track=1;
        else if(topic==DOCUMENTATION_PHYSIM_GUIDE_TOPIC)a->doc_track=2;
        else if(topic==1)a->doc_track=0;
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
    if(topic==DOCUMENTATION_C_GUIDE_TOPIC)a->doc_track=1;
    else if(topic==DOCUMENTATION_PHYSIM_GUIDE_TOPIC)a->doc_track=2;
    else if(topic==1)a->doc_track=0;
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
/* Stable semantic IDs are rebuilt from current content for each key event. */
enum { DOC_C=1,DOC_PHYSIM,DOC_TOPICS,DOC_CONTENTS,DOC_FILTER,DOC_GROUP,DOC_BACK,DOC_HOME,DOC_WORKSPACE,DOC_SEARCH,DOC_PREVIOUS,DOC_NEXT,DOC_BODY,
       DOC_TOPIC=10000,DOC_HEADING=20000,DOC_COPY=30000,DOC_LINK=40000 };
static unsigned documentation_focus_list(app *a,int *entries) {
    unsigned count=0;entries[count++]=DOC_C;entries[count++]=DOC_PHYSIM;
    entries[count++]=DOC_TOPICS;entries[count++]=DOC_CONTENTS;
    if(a->doc_contents && a->documentation) {
        for(size_t i=0;i<a->documentation->count;i++)if(a->documentation->blocks[i].kind==PS_DOC_HEADING && a->documentation->blocks[i].level<=2)entries[count++]=DOC_HEADING+(int)i;
    } else {
        entries[count++]=DOC_FILTER;entries[count++]=DOC_GROUP;
        for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++)if(a->doc_filter[0]?documentation_hits[i] && !documentation_topics[i].code:documentation_group(i)==a->doc_group)entries[count++]=DOC_TOPIC+i;
    }
    if(a->doc_previous>=0)entries[count++]=DOC_BACK;
    entries[count++]=DOC_HOME;entries[count++]=DOC_WORKSPACE;entries[count++]=DOC_SEARCH;
    entries[count++]=DOC_PREVIOUS;entries[count++]=DOC_NEXT;entries[count++]=DOC_BODY;
    if(a->documentation)for(size_t i=0;i<a->documentation->count;i++) {
        ps_doc_kind kind=a->documentation->blocks[i].kind;
        if(kind==PS_DOC_CODE || kind==PS_DOC_LINK)entries[count++]=(kind==PS_DOC_CODE?DOC_COPY:DOC_LINK)+(int)i;
    }
    return count;
}
static bool documentation_keyboard_key(app *a,const SDL_KeyboardEvent *key) {
    if(key->mod&(SDL_KMOD_CTRL|SDL_KMOD_GUI|SDL_KMOD_ALT)) {
        if(key->key==SDLK_F && (key->mod&PS_UI_COMMAND_MOD)) {
            a->doc_keyboard=true;a->doc_focus=DOC_SEARCH;a->doc_focus_reveal=true;
            a->doc_search_focus=true;a->doc_filter_active=false;return true;
        }
        return false;
    }
    if(key->key==SDLK_ESCAPE)return false;
    if(key->key==SDLK_TAB) {
        if(a->doc_keyboard && (a->doc_focus==DOC_SEARCH || a->doc_focus==DOC_FILTER)) {
            bool search=a->doc_focus==DOC_SEARCH;
            ps_ui_flush_edit(a->doc_ui,search?"document":"topics",search?a->doc_query:a->doc_filter,sizeof a->doc_query,a->doc_focus_bounds);
            if(!search && strcmp(a->doc_filter,a->doc_last_filter))documentation_filter(a);
        }
        int entries[2*PS_DOC_MAX_BLOCKS+DOCUMENTATION_TOPIC_COUNT+16];unsigned count=documentation_focus_list(a,entries),selected=count;
        for(unsigned i=0;i<count;i++)if(entries[i]==a->doc_focus){selected=i;break;}
        int direction=key->mod&SDL_KMOD_SHIFT?-1:1;
        selected=!a->doc_keyboard || selected==count?(direction>0?0:count-1):(selected+count+direction)%count;
        a->doc_keyboard=true;a->doc_focus=entries[selected];a->doc_focus_reveal=true;
        a->doc_search_active=a->doc_filter_active=false;
        a->doc_search_focus=a->doc_focus==DOC_SEARCH;a->doc_filter_focus=a->doc_focus==DOC_FILTER;
        if(a->doc_focus>=DOC_COPY)a->doc_jump_block=a->doc_focus%10000;
        return true;
    }
    if(!a->doc_keyboard || a->doc_focus==DOC_SEARCH || a->doc_focus==DOC_FILTER)return false;
    bool activate=key->key==SDLK_RETURN || key->key==SDLK_KP_ENTER || key->key==SDLK_SPACE;
    int direction=key->key==SDLK_LEFT || key->key==SDLK_UP?-1:key->key==SDLK_RIGHT || key->key==SDLK_DOWN?1:0;
    if(activate && key->repeat)return true;
    int focus=a->doc_focus;
    if(focus==DOC_BODY) {
        int w,h;SDL_GetWindowSize(a->doc_window,&w,&h);(void)w;
        double delta=key->key==SDLK_PAGEUP?-h*.7:key->key==SDLK_PAGEDOWN?h*.7:direction*48;
        double y=(double)a->doc_scroll_y+delta;
        if(key->key==SDLK_HOME)y=0;
        if(key->key==SDLK_END && a->documentation) {
            y=0;for(size_t i=0;i<a->documentation->count;i++){const ps_doc_block *b=&a->documentation->blocks[i];y+=b->height+8+(b->kind==PS_DOC_CODE?36:0);}
        }
        a->doc_scroll_y=(nk_uint)fmax(0,fmin(UINT32_MAX,y));return true;
    }
    if(focus==DOC_GROUP && (activate || direction)) {
        if(a->doc_filter[0]){a->doc_filter[0]=0;documentation_filter(a);}
        else a->doc_group=(a->doc_group+(direction<0?4:1))%5;
        return true;
    }
    if(!activate)return true;
    if(focus==DOC_C || focus==DOC_PHYSIM) {
        a->doc_query[0]=a->doc_filter[0]=0;documentation_filter(a);a->doc_contents=false;
        open_documentation(a,focus==DOC_C?DOCUMENTATION_C_GUIDE_TOPIC:DOCUMENTATION_PHYSIM_GUIDE_TOPIC);
    } else if(focus==DOC_TOPICS || focus==DOC_CONTENTS)a->doc_contents=focus==DOC_CONTENTS;
    else if(focus==DOC_BACK && a->doc_previous>=0)open_documentation(a,a->doc_previous);
    else if(focus==DOC_HOME)open_documentation(a,a->doc_track==1?DOCUMENTATION_C_GUIDE_TOPIC:a->doc_track==2?DOCUMENTATION_PHYSIM_GUIDE_TOPIC:1);
    else if(focus==DOC_WORKSPACE)SDL_RaiseWindow(a->window);
    else if(focus==DOC_PREVIOUS || focus==DOC_NEXT){a->doc_match+=focus==DOC_NEXT?1:-1;a->doc_move_match=true;}
    else if(focus>=DOC_LINK) {
        unsigned i=(unsigned)(focus-DOC_LINK);
        if(a->documentation && i<a->documentation->count && a->documentation->blocks[i].kind==PS_DOC_LINK) {
            char target[4096];snprintf(target,sizeof target,"%s",a->documentation->text+a->documentation->blocks[i].target);
            documentation_link(a,target);a->doc_focus=DOC_BODY;a->doc_focus_reveal=true;
        }
    } else if(focus>=DOC_COPY) {
        unsigned i=(unsigned)(focus-DOC_COPY);
        if(a->documentation && i<a->documentation->count && a->documentation->blocks[i].kind==PS_DOC_CODE)SDL_SetClipboardText(a->documentation->text+a->documentation->blocks[i].text);
    } else if(focus>=DOC_HEADING)a->doc_jump_block=focus-DOC_HEADING;
    else if(focus>=DOC_TOPIC) {
        open_documentation(a,focus-DOC_TOPIC);
        if(a->doc_filter[0]){snprintf(a->doc_query,sizeof a->doc_query,"%s",a->doc_filter);a->doc_move_match=true;}
    }
    return true;
}
static void documentation_focus_draw(app *a,int id,struct nk_rect bounds,bool body) {
    struct nk_context *ui=a->doc_ui;
    if(nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT) && nk_input_is_mouse_hovering_rect(&ui->input,bounds)){a->doc_keyboard=false;a->doc_focus=id;}
    if(!a->doc_keyboard || a->doc_focus!=id)return;
    if(id==DOC_BODY){struct nk_rect clip=nk_window_get_content_region(ui);bounds.h=fmaxf(0,fminf(bounds.h,clip.y+clip.h-bounds.y));}
    a->doc_focus_bounds=bounds;
    nk_stroke_rect(nk_window_get_canvas(ui),bounds,3,2,UI_COLOR(accent));
    if(!a->doc_focus_reveal)return;
    struct nk_rect clip=nk_window_get_content_region(ui);float delta=0;
    if(bounds.y<clip.y)delta=bounds.y-clip.y;
    else if(bounds.y+bounds.h>clip.y+clip.h)delta=bounds.y+bounds.h-clip.y-clip.h;
    if(body) {if(delta)a->doc_scroll_y=(nk_uint)fmaxf(0,(float)a->doc_scroll_y+delta);}
    else {nk_uint x,y;nk_window_get_scroll(ui,&x,&y);if(delta)nk_window_set_scroll(ui,x,(nk_uint)fmaxf(0,(float)y+delta));}
    if(!delta)a->doc_focus_reveal=false;
}
static void documentation_sidebar(app *a) {
    struct nk_context *ui = a->doc_ui;
    nk_layout_row_dynamic(ui, 28, 1);
    nk_label_colored(ui, "PHYSIM HANDBUCH", NK_TEXT_LEFT, UI_COLOR(muted));
    nk_layout_row_dynamic(ui,32,1);
    a->doc_track_bounds[0]=nk_widget_bounds(ui);
    bool c_selected=a->doc_track==1;
    if(nk_selectable_label(ui,"Teil I · C",NK_TEXT_LEFT,&c_selected)) {
        a->doc_query[0]=a->doc_filter[0]=0;documentation_filter(a);a->doc_contents=false;
        open_documentation(a,DOCUMENTATION_C_GUIDE_TOPIC);
    }
    documentation_focus_draw(a,DOC_C,a->doc_track_bounds[0],false);
    nk_layout_row_dynamic(ui,32,1);
    a->doc_track_bounds[1]=nk_widget_bounds(ui);
    bool physim_selected=a->doc_track==2;
    if(nk_selectable_label(ui,"Teil II · Physim",NK_TEXT_LEFT,&physim_selected)) {
        a->doc_query[0]=a->doc_filter[0]=0;documentation_filter(a);a->doc_contents=false;
        open_documentation(a,DOCUMENTATION_PHYSIM_GUIDE_TOPIC);
    }
    documentation_focus_draw(a,DOC_PHYSIM,a->doc_track_bounds[1],false);
    nk_layout_row_dynamic(ui, 30, 2);
    struct nk_rect topic_bounds=nk_widget_bounds(ui);
    if (nk_button_label(ui, "Themen"))
        a->doc_contents = false;
    documentation_focus_draw(a,DOC_TOPICS,topic_bounds,false);
    struct nk_rect contents_bounds=nk_widget_bounds(ui);
    if (nk_button_label(ui, "Inhalt"))
        a->doc_contents = true;
    documentation_focus_draw(a,DOC_CONTENTS,contents_bounds,false);
    if (a->doc_contents && a->documentation) {
        a->doc_filter_active = false;
        nk_layout_row_dynamic(ui, 44, 1);
        nk_label_wrap(ui, documentation_topics[a->doc_topic].title);
        for (size_t i = 0; i < a->documentation->count; i++) {
            ps_doc_block *b = &a->documentation->blocks[i];
            if (b->kind != PS_DOC_HEADING || b->level > 2)
                continue;
            nk_layout_row_dynamic(ui, 36, 1);
            struct nk_rect bounds=nk_widget_bounds(ui);
            if (nk_button_label(ui, a->documentation->text + b->text))
                a->doc_jump_block = (int)i;
            documentation_focus_draw(a,DOC_HEADING+(int)i,bounds,false);
        }
        return;
    }
    nk_layout_row_dynamic(ui, 24, 1);
    nk_label(ui, "Alle Inhalte durchsuchen", NK_TEXT_LEFT);
    nk_layout_row_dynamic(ui, 32, 1);
    if(a->doc_keyboard && a->doc_focus!=DOC_FILTER)nk_edit_unfocus(ui);
    if(a->doc_filter_focus || (a->doc_keyboard && a->doc_focus==DOC_FILTER && !ui->current->edit.active)){nk_edit_focus(ui,NK_EDIT_ALWAYS_INSERT_MODE);a->doc_filter_focus=false;}
    struct nk_rect filter_bounds=nk_widget_bounds(ui);
    nk_flags edit = nk_edit_string_zero_terminated(ui, NK_EDIT_FIELD, a->doc_filter,
                                                   sizeof a->doc_filter, nk_filter_default);
    documentation_focus_draw(a,DOC_FILTER,filter_bounds,false);
    a->doc_filter_active = (edit & NK_EDIT_ACTIVE) != 0;
    if (a->doc_filter_active)
        a->doc_search_active = false;
    if (strcmp(a->doc_filter, a->doc_last_filter))
        documentation_filter(a);
    const char *groups[] = {"Lernen & Bedienung", "Fachthemen & Beispiele", "Funktionsreferenz",
                            "Projekt & Technik", "Header (optional)"};
    nk_layout_row_dynamic(ui, 32, 1);
    struct nk_rect group_bounds=nk_widget_bounds(ui);
    if (a->doc_filter[0]) {
        if (nk_button_label(ui, "Suche zurücksetzen")) {
            a->doc_filter[0] = 0;
            documentation_filter(a);
        }
    } else
        a->doc_group = nk_combo(ui, groups, 5, a->doc_group, 30, nk_vec2(260, 190));
    documentation_focus_draw(a,DOC_GROUP,group_bounds,false);
    int found = 0;
    for (int i = 0; i < DOCUMENTATION_TOPIC_COUNT; i++) {
        if (a->doc_filter[0] ? (!documentation_hits[i] || documentation_topics[i].code)
                             : documentation_group(i) != a->doc_group)
            continue;
        found++;
        nk_layout_row_dynamic(ui, 34, 1);
        struct nk_rect bounds=nk_widget_bounds(ui);
        bool selected = a->documentation && a->doc_topic == i;
        if (nk_selectable_label(ui, documentation_topics[i].title, NK_TEXT_LEFT, &selected)) {
            open_documentation(a, i);
            if (a->doc_filter[0]) {
                snprintf(a->doc_query, sizeof a->doc_query, "%s", a->doc_filter);
                a->doc_move_match = true;
            }
        }
        documentation_focus_draw(a,DOC_TOPIC+i,bounds,false);
    }
    if (!found) {
        nk_layout_row_dynamic(ui, 76, 1);
        nk_label_wrap(ui,
                      "Keine Treffer. Suche nach einem Begriff wie Sensor, Export oder ps_scene.");
    }
}
static void documentation_view(app *a, float height) {
    struct nk_context *ui = a->doc_ui;
    bool large=a->preferences.ui_size>=20;
    nk_layout_row_dynamic(ui, 32, large?2:3);
    struct nk_rect back_bounds=nk_widget_bounds(ui);
    nk_begin_disabled(ui, a->doc_previous < 0);
    if (nk_button_label(ui, "Zurück"))
        open_documentation(a, a->doc_previous);
    nk_end_disabled(ui);
    documentation_focus_draw(a,DOC_BACK,back_bounds,false);
    a->doc_home_bounds=nk_widget_bounds(ui);
    if (nk_button_label(ui, "Start"))
        open_documentation(a,a->doc_track==1?DOCUMENTATION_C_GUIDE_TOPIC:
                            a->doc_track==2?DOCUMENTATION_PHYSIM_GUIDE_TOPIC:1);
    documentation_focus_draw(a,DOC_HOME,a->doc_home_bounds,false);
    struct nk_rect workspace_bounds=nk_widget_bounds(ui);
    if (nk_button_label(ui, "Arbeitsbereich"))
        SDL_RaiseWindow(a->window);
    documentation_focus_draw(a,DOC_WORKSPACE,workspace_bounds,false);
    if(large)nk_layout_row_dynamic(ui,32,1);
    else {nk_layout_row_begin(ui, NK_DYNAMIC, 32, 3);nk_layout_row_push(ui,.60f);}
    if(a->doc_keyboard && a->doc_focus!=DOC_SEARCH)nk_edit_unfocus(ui);
    if (a->doc_search_focus || (a->doc_keyboard && a->doc_focus==DOC_SEARCH && !ui->current->edit.active)) {
        nk_edit_focus(ui, NK_EDIT_ALWAYS_INSERT_MODE);
        a->doc_search_focus = false;
    }
    struct nk_rect search_bounds = nk_widget_bounds(ui);
    nk_flags search_state = nk_edit_string_zero_terminated(ui, NK_EDIT_FIELD, a->doc_query,
                                                           sizeof a->doc_query, nk_filter_default);
    documentation_focus_draw(a,DOC_SEARCH,search_bounds,false);
    a->doc_search_active = (search_state & NK_EDIT_ACTIVE) != 0;
    if (!a->doc_query[0] && !(search_state & NK_EDIT_ACTIVE)) {
        const char *placeholder = "Dieses Dokument durchsuchen …";
        nk_draw_text(nk_window_get_canvas(ui),
                     nk_rect(search_bounds.x + 10, search_bounds.y + 8, search_bounds.w - 20, 20),
                     placeholder, (int)strlen(placeholder), a->doc_font_ui, nk_rgba(0, 0, 0, 0),
                     UI_COLOR(muted));
    }
    if(large)nk_layout_row_dynamic(ui,32,2);else nk_layout_row_push(ui,.20f);
    struct nk_rect previous_bounds=nk_widget_bounds(ui);
    if (nk_button_label(ui, "Vorheriger")) {
        a->doc_match--;
        a->doc_move_match = true;
    }
    documentation_focus_draw(a,DOC_PREVIOUS,previous_bounds,false);
    if(!large)nk_layout_row_push(ui,.20f);
    struct nk_rect next_bounds=nk_widget_bounds(ui);
    if (nk_button_label(ui, "Nächster")) {
        a->doc_match++;
        a->doc_move_match = true;
    }
    documentation_focus_draw(a,DOC_NEXT,next_bounds,false);
    if(!large)nk_layout_row_end(ui);
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
    if(large) {nk_style_push_color(ui,&ui->style.text.color,UI_COLOR(muted));nk_label_wrap(ui,detail);nk_style_pop_color(ui);}
    else nk_label_colored(ui, detail, NK_TEXT_LEFT, UI_COLOR(muted));
    nk_layout_row_dynamic(ui,16,1);
    struct nk_rect remaining=nk_widget_bounds(ui),clip=nk_window_get_content_region(ui);
    nk_layout_row_dynamic(ui,fmaxf(20,fminf(height,clip.y+clip.h-remaining.y)),1);
    struct nk_rect body_bounds=nk_widget_bounds(ui);
    documentation_focus_draw(a,DOC_BODY,body_bounds,false);
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
                    struct nk_rect copy_bounds=nk_widget_bounds(ui);
                    if (nk_button_label(ui, "Code kopieren"))
                        SDL_SetClipboardText(text);
                    documentation_focus_draw(a,DOC_COPY+(int)i,copy_bounds,true);
                }
                nk_layout_row_dynamic(ui, b->height, 1);
                if (b->kind == PS_DOC_LINK) {
                    bool external = !strncmp(d->text + b->target, "http", 4);
                    char label[240];
                    snprintf(label, sizeof label, "%s%s",
                             external ? "Im Browser öffnen: " : "Öffnen: ", text);
                    struct nk_rect link_bounds=nk_widget_bounds(ui);
                    if (nk_button_label(ui, label)) {
                        /* Loading another document frees d; stop traversal immediately. */
                        documentation_link(a, d->text + b->target);
                        break;
                    }
                    documentation_focus_draw(a,DOC_LINK+(int)i,link_bounds,true);
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
