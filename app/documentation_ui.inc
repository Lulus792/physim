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
    {"SI-Einheiten und Konvertierung", "docs/units.md", false},
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
    {"Versionierte Run-Streams", "docs/run-streams.md", false},
    {"run_index.h", "include/physim/run_index.h", true},
    {"C: Laufdatei-Index", "docs/reference/run_index.md", false},
    {"Run-Stream-Referenz", "docs/reference/run_stream.md", false},
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
    {"Reale Gase: Van der Waals", "docs/real-gas.md", false},
    {"Barrierefreiheit", "docs/accessibility.md", false},
    {"Historische Plattformnachweise", "docs/platform-validation-history.md", false},
    {"CFD Teilprojekt", "docs/cfd-project-plan.md", false},
    {"FEM Teilprojekt", "docs/fem-project-plan.md", false},
    {"3D-Szenenmessung", "docs/scene-rendering.md", false},
    {"App-Profilaufzeichnung", "docs/app-profiling.md", false},
    {"Historische Plattformnachweise: Band 2", "docs/platform-validation-history-2.md", false},
    {"SecondBrain Anbindung", "docs/secondbrain.md", false},
    {"Pflege des Physim Projektgedächtnisses", "brains/physim/AGENTS.md", false},
    {"Physim Entscheidungen", "brains/physim/DECISIONS.md", false},
    {"Physim Auftrag", "brains/physim/PROJECT.md", false},
    {"Physim Offene Punkte", "brains/physim/QUESTIONS.md", false},
    {"Physim Quellen", "brains/physim/SOURCES.md", false},
    {"Physim Projektgedächtnis", "brains/physim/START.md", false},
    {"Physim Stand", "brains/physim/STATE.md", false},
    {"Physim Archiv", "brains/physim/archive/README.md", false},
    {"Physim Eingang", "brains/physim/inbox/INBOX.md", false},
    {"Physim Übergabe vom 12 Oktober 2026", "brains/physim/journal/2026-10-12.md", false},
    {"Physim Dokumentation finden", "brains/physim/knowledge/dokumentation.md", false},
    {"Physim Projektanweisungen", "AGENTS.md", true},
    {"SecondBrain Metadaten", "brains/physim/brain.json", true},
    {"Beispielquellen Katalog", "tests/tutorial_sources.json", true},
    {"Physim User Guide", "docs/user-guide.md", false},
    {"Physim Examples", "docs/examples.md", false},
    {"Grundlagen der Physim Dokumentation", "docs/documentation-research.md", false},
    {"Räumliche Modelle verstehen", "docs/spatial-explanations.md", false},
    {"Frühere Physim Einführung", "docs/readme-history.md", false},
    {"Quelle: docs/benchmarks/2026-09-19/baseline/summary.json", "docs/benchmarks/2026-09-19/baseline/summary.json", true},
    {"Quelle: docs/benchmarks/2026-09-19/crc/crc-summary.json", "docs/benchmarks/2026-09-19/crc/crc-summary.json", true},
    {"Quelle: docs/benchmarks/2026-09-19/report-view/summary.json", "docs/benchmarks/2026-09-19/report-view/summary.json", true},
    {"Quelle: docs/benchmarks/2026-09-19/ui-buffers/summary.json", "docs/benchmarks/2026-09-19/ui-buffers/summary.json", true},
    {"Quelle: docs/project-audit.json", "docs/project-audit.json", true},
    {"Beispiel: ccd_events/main.c", "examples/ccd_events/main.c", true},
    {"Beispiel: contact_stack/main.c", "examples/contact_stack/main.c", true},
    {"Beispiel: convex_contacts/main.c", "examples/convex_contacts/main.c", true},
    {"Beispiel: diagnostics/analysis.c", "examples/diagnostics/analysis.c", true},
    {"Beispiel: diagnostics/main.c", "examples/diagnostics/main.c", true},
    {"Beispiel: language/ccd_events.phys", "examples/language/ccd_events.phys", true},
    {"Beispiel: language/contact_stack.phys", "examples/language/contact_stack.phys", true},
    {"Beispiel: language/contact_world.phys", "examples/language/contact_world.phys", true},
    {"Beispiel: language/convex_contacts.phys", "examples/language/convex_contacts.phys", true},
    {"Beispiel: language/diagnostic_analysis.phys", "examples/language/diagnostic_analysis.phys", true},
    {"Beispiel: language/diagnostic_experiment.phys", "examples/language/diagnostic_experiment.phys", true},
    {"Beispiel: language/diagnostic_values.phys", "examples/language/diagnostic_values.phys", true},
    {"Beispiel: language/generics/main.phys", "examples/language/generics/main.phys", true},
    {"Beispiel: language/logging.phys", "examples/language/logging.phys", true},
    {"Beispiel: language/modules/main.phys", "examples/language/modules/main.phys", true},
    {"Beispiel: language/run_index_analysis.phys", "examples/language/run_index_analysis.phys", true},
    {"Beispiel: language/run_index_values.phys", "examples/language/run_index_values.phys", true},
    {"Beispiel: language/scene_frames.phys", "examples/language/scene_frames.phys", true},
    {"Beispiel: logging/main.c", "examples/logging/main.c", true},
    {"Beispiel: pendulum/main.c", "examples/pendulum/main.c", true},
    {"Beispiel: scene_frames/main.c", "examples/scene_frames/main.c", true},
    {"Quelle: third_party/README.md", "third_party/README.md", false},
    {"Quelle: tools/SDK-README.md", "tools/SDK-README.md", false},
    {"Befehlsmenüs und Zustände", "docs/decisions/003-command-menus.md", false},
    {"F1 Handbuch und Abnahme", "docs/decisions/004-f1-handbook.md", false},
    {"Historische Plattformnachweise: Band 3", "docs/platform-validation-history-3.md", false},
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
    if(!strcmp(documentation_topics[topic].path,"docs/user-guide.md") ||
       !strcmp(documentation_topics[topic].path,"docs/examples.md") ||
       !strcmp(documentation_topics[topic].path,"docs/spatial-explanations.md") ||
       !strcmp(documentation_topics[topic].path,"docs/c-guide.md") ||
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
        if (a->doc_filter[0] && !documentation_index[i]) {
            char path[4096];
            size_t length = 0;
            join(path, sizeof path, a->root, documentation_topics[i].path);
            documentation_index[i] = load_utf8_text_limit(path, &length,
                documentation_topics[i].code?PS_DOC_MAX_CODE_BYTES:PS_DOC_MAX_BYTES);
        }
        documentation_hits[i] = !a->doc_filter[0] ||
                                ps_document_find(documentation_topics[i].title, a->doc_filter) ||
                                ps_document_find(documentation_index[i], a->doc_filter);
    }
    snprintf(a->doc_last_filter, sizeof a->doc_last_filter, "%s", a->doc_filter);
}
static ps_doc_location documentation_location(const app *a) {
    ps_doc_location p={.topic=a->doc_topic,.track=a->doc_track,.match=a->doc_match,
        .scroll_x=a->doc_scroll_x,.scroll_y=a->doc_scroll_y,.contents=a->doc_contents};
    snprintf(p.query,sizeof p.query,"%s",a->doc_query);
    snprintf(p.filter,sizeof p.filter,"%s",a->doc_filter);return p;
}
static void documentation_history_save(app *a) {
    if(a->documentation && !a->doc_history_restore) {
        ps_doc_location p=documentation_location(a);ps_doc_history_store(&a->doc_history,&p);
    }
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
    char *source = load_utf8_text_limit(path, &length,documentation_topics[topic].code?PS_DOC_MAX_CODE_BYTES:PS_DOC_MAX_BYTES);
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
    if(!a->doc_history_navigation)documentation_history_save(a);
    free(a->documentation);
    a->documentation = document;
    ++a->doc_reading_version;
    a->doc_topic = topic;
    if(topic==DOCUMENTATION_C_GUIDE_TOPIC)a->doc_track=1;
    else if(topic==DOCUMENTATION_PHYSIM_GUIDE_TOPIC)a->doc_track=2;
    else if(topic==1)a->doc_track=0;
    a->doc_error[0] = 0;
    a->doc_history_restore=0;
    a->doc_scroll_x = a->doc_scroll_y = 0;
    a->doc_match = 0;
    a->doc_move_match = true;
    a->doc_jump_block = -1;
    a->doc_anchor_heading=-1;
    a->doc_group = documentation_group(topic);
    if(!a->doc_history_navigation) {
        ps_doc_location p=documentation_location(a);ps_doc_history_visit(&a->doc_history,&p);
    }
}
static void documentation_history_go(app *a,int direction) {
    const ps_doc_location *entry=ps_doc_history_peek(&a->doc_history,direction);
    if(!entry)return;
    ps_doc_location target=*entry;documentation_history_save(a);
    a->doc_history_navigation=true;open_documentation(a,target.topic);a->doc_history_navigation=false;
    if(a->doc_error[0] || a->doc_topic!=target.topic)return;
    ps_doc_history_move(&a->doc_history,direction);
    a->doc_track=target.track;a->doc_contents=target.contents;
    snprintf(a->doc_query,sizeof a->doc_query,"%s",target.query);
    snprintf(a->doc_last_query,sizeof a->doc_last_query,"%s",target.query);
    a->doc_match=target.match;
    snprintf(a->doc_filter,sizeof a->doc_filter,"%s",target.filter);documentation_filter(a);
    a->doc_move_match=false;a->doc_jump_block=-1;a->doc_history_restore=2;
    a->doc_scroll_x=target.scroll_x;a->doc_scroll_y=target.scroll_y;
}
static unsigned documentation_font_size(const app *a) {
    return a->doc_zoom_size?a->doc_zoom_size:a->preferences.ui_size;
}
static unsigned documentation_code_size(const app *a) {
    return a->doc_zoom_size?a->doc_zoom_size:a->preferences.editor_size;
}
static float documentation_control_height(const app *a) {
    return (float)documentation_font_size(a)+16;
}
static void documentation_zoom(app *a,int direction) {
    unsigned old=documentation_font_size(a);
    int size=direction?(int)old+direction*2:0;
    a->doc_zoom_size=direction?(unsigned)(size<16?16:size>32?32:size):0;
    unsigned now=documentation_font_size(a);
    a->doc_font_ui=a->doc_ui_fonts[(now-16)/2];
    a->doc_font_title=a->doc_title_fonts[(now-16)/2];
    nk_style_set_font(a->doc_ui,a->doc_font_ui);nk_sdl_set_ui_size(a->doc_ui,now);
    if(a->documentation)a->documentation->layout_width=0;
    a->doc_scroll_y=(nk_uint)((double)a->doc_scroll_y*now/old);
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
static const struct nk_user_font *documentation_block_font(const app *a,const ps_doc_block *b) {
    unsigned size=documentation_font_size(a);
    if(b->kind==PS_DOC_CODE)return a->doc_font_code;
    if(b->kind==PS_DOC_HEADING) {
        if(b->level==1)return a->doc_font_title;
        return a->doc_bold_fonts[(size-16)/2+(b->level==2?2:0)];
    }
    if(b->kind==PS_DOC_TABLE_ROW && b->level)return a->doc_bold_fonts[(size-16)/2];
    return a->doc_font_ui;
}
static const ps_doc_span *documentation_span_at(const ps_document *d,const ps_doc_block *b,size_t at) {
    size_t lo=b->first_span,hi=lo+b->span_count;
    while(lo<hi){size_t mid=lo+(hi-lo)/2;if(d->spans[mid].text<=at)lo=mid+1;else hi=mid;}
    if(lo>b->first_span){const ps_doc_span *s=&d->spans[lo-1];if(at<s->text+s->length)return s;}
    return NULL;
}
static const struct nk_user_font *documentation_span_font(const app *a,const ps_doc_block *b,const ps_doc_span *s) {
    if(b->kind==PS_DOC_HEADING)return documentation_block_font(a,b);
    if(s && (s->style&PS_DOC_INLINE_CODE))return a->doc_font_code;
    if(s && (s->style&PS_DOC_BOLD))return a->doc_bold_fonts[(documentation_font_size(a)-16)/2];
    return documentation_block_font(a,b);
}
static float documentation_glyph_width(const struct nk_user_font *font,const char *text,int bytes) {
    return *text=='\t'?font->width(font->userdata,font->height,"    ",4):
        font->width(font->userdata,font->height,text,bytes);
}
static float documentation_text_width(const struct nk_user_font *font,const char *text,size_t n) {
    float width=0;
    for(size_t at=0;at<n;){nk_rune rune;int used=nk_utf_decode(text+at,&rune,(int)(n-at));if(used<=0)used=1;
        width+=documentation_glyph_width(font,text+at,used);at+=(size_t)used;}
    return width;
}
static size_t documentation_rich_line(const app *a,const ps_document *d,const ps_doc_block *b,
    size_t at,float width,size_t *length,float *height) {
    const char *text=d->text+b->text;size_t begin=at,space=SIZE_MAX;
    float used=0,h=documentation_block_font(a,b)->height+6,space_height=h;
    while(at<b->length) {
        if(text[at]=='\r'){*length=at-begin;*height=h;return at-begin+(at+1<b->length && text[at+1]=='\n'?2:1);}
        if(text[at]=='\n'){*length=at-begin;*height=h;return at-begin+1;}
        const ps_doc_span *span=documentation_span_at(d,b,b->text+at);
        const struct nk_user_font *font=documentation_span_font(a,b,span);
        nk_rune rune;int bytes=nk_utf_decode(text+at,&rune,(int)(b->length-at));if(bytes<=0)bytes=1;
        float glyph=documentation_glyph_width(font,text+at,bytes);
        if(at>begin && used+glyph>width) {
            if(space!=SIZE_MAX && space>begin){*length=space-begin;*height=space_height;return space-begin+1;}
            *length=at-begin;*height=h;return at-begin;
        }
        h=fmaxf(h,font->height+6);
        if(text[at]==' ' || text[at]=='\t'){space=at;space_height=h;}
        used+=glyph;at+=(size_t)bytes;
    }
    *length=at-begin;*height=h;return at-begin;
}
static float documentation_rich_height(const app *a,const ps_document *d,const ps_doc_block *b,float width) {
    size_t at=0;float total=0;
    do {size_t length;float height;size_t used=documentation_rich_line(a,d,b,at,width,&length,&height);
        at+=used;total+=height;}while(at<b->length);
    return total;
}
static void documentation_rich_draw(app *a,ps_document *d,const ps_doc_block *b,struct nk_rect bounds) {
    struct nk_context *ui=a->doc_ui;struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    const char *text=d->text+b->text;size_t at=0;float y=bounds.y;
    const char *match=NULL;bool searched=false;
    do {
        size_t length;float height;size_t used=documentation_rich_line(a,d,b,at,bounds.w,&length,&height);
        size_t end=at+length,pos=at;float x=bounds.x;
        bool visible=y<canvas->clip.y+canvas->clip.h && y+height>canvas->clip.y;
        if(b->kind==PS_DOC_CODE && y>canvas->clip.y+canvas->clip.h)break;
        while(pos<end) {
            const ps_doc_span *span=documentation_span_at(d,b,b->text+pos);
            const struct nk_user_font *font=documentation_span_font(a,b,span);
            size_t next=span?span->text+span->length-b->text:end;if(next>end)next=end;
            float width=documentation_text_width(font,text+pos,next-pos);
            struct nk_rect part=nk_rect(x,y,width,font->height+6);
            bool link=span && (span->style&PS_DOC_INLINE_LINK) && span->link<d->count;
            if(link) {
                ps_doc_block *action=&d->blocks[span->link];float *hit=action->hit;
                if(hit[2]<=0){hit[0]=part.x;hit[1]=part.y;hit[2]=part.w;hit[3]=part.h;}
                else {float right=fmaxf(hit[0]+hit[2],part.x+part.w),bottom=fmaxf(hit[1]+hit[3],part.y+part.h);
                    hit[0]=fminf(hit[0],part.x);hit[1]=fminf(hit[1],part.y);hit[2]=right-hit[0];hit[3]=bottom-hit[1];}
                if(visible && nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT) &&
                    nk_input_is_mouse_hovering_rect(&ui->input,part) && nk_input_is_mouse_hovering_rect(&ui->input,canvas->clip))
                    a->doc_inline_clicked=(int)span->link;
            }
            if(visible) {
                if(b->reading_begin!=SIZE_MAX) {
                    float glyph_x=x;
                    for(size_t glyph=pos;glyph<next;) {
                        nk_rune rune;int bytes=nk_utf_decode(text+glyph,&rune,(int)(next-glyph));if(bytes<=0)bytes=1;
                        float glyph_width=documentation_glyph_width(font,text+glyph,bytes);
                        nk_sdl_accessibility_text_box(ui,b->reading_begin+glyph,b->reading_begin+glyph+(size_t)bytes,
                            nk_rect(glyph_x,y,glyph_width,font->height+6));
                        glyph_x+=glyph_width;glyph+=(size_t)bytes;
                    }
                }
                if(!searched || (match && match<text+pos)){match=ps_document_find(text+pos,a->doc_query);searched=true;}
                bool found=match && match<text+next;
                if(span && (span->style&PS_DOC_INLINE_CODE))
                    nk_fill_rect(canvas,nk_rect(x-2,y-2,width+4,font->height+4),3,UI_COLOR(edit));
                if(found)nk_stroke_rect(canvas,part,2,1,UI_COLOR(accent));
                struct nk_color color=link || found?UI_COLOR(accent):UI_COLOR(doc_text);
                size_t chunk=pos;float draw_x=x;
                while(chunk<next) {
                    const char *tab=memchr(text+chunk,'\t',next-chunk);size_t last=tab?(size_t)(tab-text):next;
                    float run_width=documentation_text_width(font,text+chunk,last-chunk);
                    if(last>chunk)nk_draw_text(canvas,nk_rect(draw_x,y,run_width+1,font->height+6),text+chunk,(int)(last-chunk),font,nk_rgba(0,0,0,0),color);
                    draw_x+=run_width;if(tab)draw_x+=documentation_glyph_width(font,"\t",1);chunk=last+(tab?1:0);
                }
                if(link)nk_stroke_line(canvas,x,y+font->height+2,x+width,y+font->height+2,1,color);
            }
            x+=width;pos=next;
        }
        at+=used;y+=height;
    } while(at<b->length);
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
    for (int i = 0; i < count; i++) {
        ps_doc_block cell=*block;cell.text=block->text+(size_t)(cells[i]-text);cell.length=lengths[i];
        height =
            fmaxf(height,
                  documentation_rich_height(a,a->documentation,&cell,fmaxf(1,cell_width-20))+16);
    }
    if (!bounds)
        return height;
    struct nk_command_buffer *canvas = nk_window_get_canvas(a->doc_ui);
    nk_fill_rect(canvas, *bounds, 4, block->level ? UI_COLOR(doc_header) : UI_COLOR(doc_body));
    for (int i = 0; i < count; i++) {
        ps_doc_block cell=*block;cell.text=block->text+(size_t)(cells[i]-text);cell.length=lengths[i];
        if(cell.reading_begin!=SIZE_MAX)cell.reading_begin+=(size_t)(cells[i]-text);
        documentation_rich_draw(a,a->documentation,&cell,nk_rect(bounds->x+i*cell_width+10,
            bounds->y+8,fmaxf(1,cell_width-20),bounds->h-16));
    }
    return height;
}
static void documentation_link(app *a, const char *target) {
    if (!strncmp(target, "https://", 8) || !strncmp(target, "http://", 7)) {
        SDL_SetClipboardText(target);
        status(a,"Quellenadresse kopiert. Das Handbuch bleibt in Physim geöffnet.");
        return;
    }
    char resolved[4096],fragment[4096];
    const char *hash=strchr(target,'#');
    size_t path_length=hash?(size_t)(hash-target):strlen(target);
    if(path_length>=sizeof resolved || (hash && strlen(hash+1)>=sizeof fragment)) {
        status(a,"Dieses Linkziel ist zu lang.");return;
    }
    if(hash)snprintf(fragment,sizeof fragment,"%s",hash+1);
    const char *base = documentation_topics[a->doc_topic].path;
    const char *slash = strrchr(base, '/');
    int written=path_length?snprintf(resolved,sizeof resolved,"%.*s%.*s",slash?(int)(slash-base+1):0,base,(int)path_length,target):snprintf(resolved,sizeof resolved,"%s",base);
    if(written<0 || (size_t)written>=sizeof resolved){status(a,"Dieses Linkziel ist zu lang.");return;}
    /* Resolve the few relative parent components without consulting the filesystem. */
    char *parent;
    while ((parent = strstr(resolved, "/../")) != NULL) {
        char *start = parent;
        while (start > resolved && start[-1] != '/')
            --start;
        memmove(start, parent + 4, strlen(parent + 4) + 1);
    }
    while((parent=strstr(resolved,"/./"))!=NULL)memmove(parent,parent+2,strlen(parent+2)+1);
    while(!strncmp(resolved,"./",2))memmove(resolved,resolved+2,strlen(resolved+2)+1);
    for (int i = 0; i < DOCUMENTATION_TOPIC_COUNT; i++) {
        if (!strcmp(resolved, documentation_topics[i].path)) {
            size_t block=0;
            if(hash && fragment[0]) {
                ps_document *destination=a->documentation;
                bool temporary=i!=a->doc_topic || !destination;
                if(temporary) {
                    char path[4096];size_t length=0;join(path,sizeof path,a->root,documentation_topics[i].path);
                    char *source=load_utf8_text(path,&length);destination=NULL;
                    if(source)ps_document_parse(source,length,documentation_topics[i].code,&destination);
                    free(source);
                }
                ps_result result=destination?ps_document_anchor(destination,fragment,&block):PS_IO;
                if(temporary)free(destination);
                if(result!=PS_OK) {
                    snprintf(a->doc_error,sizeof a->doc_error,"Abschnitt konnte nicht geöffnet werden: %s",ps_result_string(result));
                    status(a,a->doc_error);return;
                }
            }
            bool same=a->documentation && a->doc_topic==i;
            if(same && hash)documentation_history_save(a);
            open_documentation(a, i);
            if(hash && !a->doc_error[0]) {
                if(same){ps_doc_location p=documentation_location(a);ps_doc_history_visit(&a->doc_history,&p);}
                a->doc_jump_block=(int)block;a->doc_move_match=false;
                a->doc_scroll_adjustment=0;
            }
            return;
        }
    }
    status(a, "Dieses Linkziel ist nicht in der mitgelieferten Dokumentation enthalten.");
}
/* Stable semantic IDs are rebuilt from current content for each key event. */
enum { DOC_C=1,DOC_PHYSIM,DOC_TOPICS,DOC_CONTENTS,DOC_FILTER,DOC_GROUP,DOC_BACK,DOC_HOME,DOC_WORKSPACE,DOC_SEARCH,DOC_PREVIOUS,DOC_NEXT,DOC_BODY,DOC_FORWARD,DOC_ZOOM_OUT,DOC_ZOOM_RESET,DOC_ZOOM_IN,
       DOC_TOPIC=10000,DOC_HEADING=20000,DOC_COPY=30000,DOC_LINK=40000,DOC_VISUAL=50000 };
static int documentation_media_kind(const char *target) {
    if(!strcmp(target,"visuals/workflow.svg"))return 1;
    if(!strcmp(target,"visuals/pendulum.svg"))return 2;
    if(!strcmp(target,"visuals/pendulum-3d.svg"))return 3;
    return 0;
}
static bool documentation_contents_heading(const app *a,const ps_doc_block *block) {
    unsigned depth=2;
    if(a->doc_topic>=0 && a->doc_topic<DOCUMENTATION_TOPIC_COUNT &&
       !strncmp(documentation_topics[a->doc_topic].path,"docs/reference/",15))depth=3;
    return block->kind==PS_DOC_HEADING && block->level<=depth;
}
/* Visual and keyboard order share the same task-first catalogue. Search puts
 * title matches before text matches without hiding bundled source pages. */
static unsigned documentation_topic_order(const app *a,int *order) {
    const char *priority[]={"docs/user-guide.md","docs/examples.md","docs/guide.md",
        "docs/c-guide.md","docs/physim-guide.md","docs/troubleshooting.md",
        "docs/spatial-explanations.md","docs/workspace.md"};
    int base[DOCUMENTATION_TOPIC_COUNT],count=0;
    bool placed[DOCUMENTATION_TOPIC_COUNT]={false};
    for(unsigned p=0;p<sizeof priority/sizeof priority[0];p++)
        for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++)if(!strcmp(documentation_topics[i].path,priority[p])){base[count++]=i;placed[i]=true;break;}
    for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++)if(!placed[i])base[count++]=i;
    unsigned found=0;
    for(int pass=0;pass<(a->doc_filter[0]?2:1);pass++)for(int n=0;n<count;n++) {
        int i=base[n];
        if(a->doc_filter[0]) {
            bool title=ps_document_find(documentation_topics[i].title,a->doc_filter)!=NULL;
            if(!documentation_hits[i] || title!=(pass==0))continue;
        } else if(documentation_group(i)!=a->doc_group)continue;
        order[found++]=i;
    }
    return found;
}
static unsigned documentation_focus_list(app *a,int *entries) {
    unsigned count=0;entries[count++]=DOC_C;entries[count++]=DOC_PHYSIM;
    entries[count++]=DOC_TOPICS;entries[count++]=DOC_CONTENTS;
    if(a->doc_contents && a->documentation) {
        for(size_t i=0;i<a->documentation->count;i++)if(documentation_contents_heading(a,&a->documentation->blocks[i]))entries[count++]=DOC_HEADING+(int)i;
    } else {
        entries[count++]=DOC_FILTER;entries[count++]=DOC_GROUP;
        int order[DOCUMENTATION_TOPIC_COUNT];unsigned topics=documentation_topic_order(a,order);
        for(unsigned n=0;n<topics;n++)entries[count++]=DOC_TOPIC+order[n];
    }
    if(ps_doc_history_peek(&a->doc_history,-1))entries[count++]=DOC_BACK;
    if(ps_doc_history_peek(&a->doc_history,1))entries[count++]=DOC_FORWARD;
    entries[count++]=DOC_HOME;entries[count++]=DOC_WORKSPACE;
    if(documentation_font_size(a)>16)entries[count++]=DOC_ZOOM_OUT;
    entries[count++]=DOC_ZOOM_RESET;
    if(documentation_font_size(a)<32)entries[count++]=DOC_ZOOM_IN;
    entries[count++]=DOC_SEARCH;
    entries[count++]=DOC_PREVIOUS;entries[count++]=DOC_NEXT;entries[count++]=DOC_BODY;
    if(a->documentation)for(size_t i=0;i<a->documentation->count;i++) {
        ps_doc_kind kind=a->documentation->blocks[i].kind;
        if(kind==PS_DOC_CODE || kind==PS_DOC_LINK)entries[count++]=(kind==PS_DOC_CODE?DOC_COPY:DOC_LINK)+(int)i;
        if(kind==PS_DOC_IMAGE && documentation_media_kind(a->documentation->text+a->documentation->blocks[i].target)==3)
            for(int part=0;part<11;part++)entries[count++]=DOC_VISUAL+(int)i*16+part;
    }
    return count;
}
static bool documentation_keyboard_key(app *a,const SDL_KeyboardEvent *key) {
    if(key->mod&PS_UI_COMMAND_MOD) {
        int zoom=key->key==SDLK_PLUS || key->key==SDLK_EQUALS || key->key==SDLK_KP_PLUS?1:
            key->key==SDLK_MINUS || key->key==SDLK_KP_MINUS?-1:0;
        if(zoom || key->key==SDLK_0){documentation_zoom(a,zoom);return true;}
    }
    if((key->mod&SDL_KMOD_ALT) && !(key->mod&(SDL_KMOD_CTRL|SDL_KMOD_GUI)) &&
       (key->key==SDLK_LEFT || key->key==SDLK_RIGHT)) {
        if(!key->repeat)documentation_history_go(a,key->key==SDLK_LEFT?-1:1);return true;
    }
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
        int entries[12*PS_DOC_MAX_BLOCKS+DOCUMENTATION_TOPIC_COUNT+16];unsigned count=documentation_focus_list(a,entries),selected=count;
        for(unsigned i=0;i<count;i++)if(entries[i]==a->doc_focus){selected=i;break;}
        int direction=key->mod&SDL_KMOD_SHIFT?-1:1;
        selected=!a->doc_keyboard || selected==count?(direction>0?0:count-1):(selected+count+direction)%count;
        a->doc_keyboard=true;a->doc_focus=entries[selected];a->doc_focus_reveal=true;
        a->doc_search_active=a->doc_filter_active=false;
        a->doc_search_focus=a->doc_focus==DOC_SEARCH;a->doc_filter_focus=a->doc_focus==DOC_FILTER;
        if(a->doc_focus>=DOC_VISUAL)a->doc_jump_block=(a->doc_focus-DOC_VISUAL)/16;
        else if(a->doc_focus>=DOC_COPY)a->doc_jump_block=a->doc_focus%10000;
        return true;
    }
    if(!a->doc_keyboard || a->doc_focus==DOC_SEARCH || a->doc_focus==DOC_FILTER)return false;
    bool activate=key->key==SDLK_RETURN || key->key==SDLK_KP_ENTER || key->key==SDLK_SPACE;
    int direction=key->key==SDLK_LEFT || key->key==SDLK_UP?-1:key->key==SDLK_RIGHT || key->key==SDLK_DOWN?1:0;
    if(activate && key->repeat)return true;
    int focus=a->doc_focus;
    if(focus>=DOC_VISUAL) {
        int part=(focus-DOC_VISUAL)%16;
        float *values[]={&a->doc_theta,&a->doc_length,&a->doc_omega,&a->doc_yaw,&a->doc_pitch,&a->doc_zoom};
        const float minimum[]={-1.2f,.5f,-3,-(float)PS_PI,-(float)PS_PI/2,2};
        const float maximum[]={1.2f,3,3,(float)PS_PI,(float)PS_PI/2,12};
        const float step[]={.01f,.05f,.05f,.08f,.08f,.2f};
        int scalar_direction=key->key==SDLK_LEFT || key->key==SDLK_DOWN?-1:key->key==SDLK_RIGHT || key->key==SDLK_UP?1:0;
        if(part<6 && scalar_direction)*values[part]=fmaxf(minimum[part],fminf(maximum[part],*values[part]+scalar_direction*step[part]));
        else if(activate && part>=6 && part<=8) {
            a->doc_yaw=part==7?.58f:0;a->doc_pitch=part==8?(float)PS_PI/2:part==7?.23f:0;
            a->doc_zoom=5;
        } else if(activate && part==9)a->doc_orthographic=!a->doc_orthographic;
        else if(part==10) {
            if(key->key==SDLK_LEFT || key->key==SDLK_RIGHT)a->doc_yaw=remainderf(a->doc_yaw+direction*.08f,2*(float)PS_PI);
            if(key->key==SDLK_UP || key->key==SDLK_DOWN)a->doc_pitch=fmaxf(-(float)PS_PI/2,fminf((float)PS_PI/2,a->doc_pitch-direction*.08f));
            if(key->key==SDLK_PAGEUP || key->key==SDLK_PAGEDOWN)a->doc_zoom=fmaxf(2,fminf(12,a->doc_zoom+(key->key==SDLK_PAGEUP?-.2f:.2f)));
            if(key->key==SDLK_HOME){a->doc_yaw=.58f;a->doc_pitch=.23f;a->doc_zoom=5;}
        }
        return true;
    }
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
        open_documentation(a,focus==DOC_C?DOCUMENTATION_C_GUIDE_TOPIC:DOCUMENTATION_PHYSIM_GUIDE_TOPIC);
        a->doc_query[0]=a->doc_filter[0]=0;documentation_filter(a);a->doc_contents=false;
    } else if(focus==DOC_TOPICS || focus==DOC_CONTENTS)a->doc_contents=focus==DOC_CONTENTS;
    else if(focus==DOC_BACK)documentation_history_go(a,-1);
    else if(focus==DOC_FORWARD)documentation_history_go(a,1);
    else if(focus==DOC_HOME)open_documentation(a,a->doc_track==1?DOCUMENTATION_C_GUIDE_TOPIC:a->doc_track==2?DOCUMENTATION_PHYSIM_GUIDE_TOPIC:1);
    else if(focus==DOC_WORKSPACE)SDL_RaiseWindow(a->window);
    else if(focus==DOC_ZOOM_OUT || focus==DOC_ZOOM_IN || focus==DOC_ZOOM_RESET)
        documentation_zoom(a,focus==DOC_ZOOM_OUT?-1:focus==DOC_ZOOM_IN?1:0);
    else if(focus==DOC_PREVIOUS || focus==DOC_NEXT){a->doc_match+=focus==DOC_NEXT?1:-1;a->doc_move_match=true;}
    else if(focus>=DOC_LINK) {
        unsigned i=(unsigned)(focus-DOC_LINK);
        if(a->documentation && i<a->documentation->count && a->documentation->blocks[i].kind==PS_DOC_LINK) {
            char target[4096];snprintf(target,sizeof target,"%s",a->documentation->text+a->documentation->blocks[i].target);
            documentation_link(a,target);a->doc_focus=DOC_BODY;a->doc_focus_reveal=true;
        }
    } else if(focus>=DOC_COPY) {
        unsigned i=(unsigned)(focus-DOC_COPY);
        if(a->documentation && i<a->documentation->count && a->documentation->blocks[i].kind==PS_DOC_CODE) {
            if(!SDL_SetClipboardText(a->documentation->text+a->documentation->blocks[i].text))
                snprintf(a->doc_error,sizeof a->doc_error,"Code konnte nicht kopiert werden: %.110s",SDL_GetError());
        }
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
    if(body)bounds.h=fminf(bounds.h,nk_window_get_content_region(ui).h);
    if(id==DOC_BODY){struct nk_rect clip=nk_window_get_content_region(ui);bounds.h=fmaxf(0,fminf(bounds.h,clip.y+clip.h-bounds.y));}
    a->doc_focus_bounds=bounds;
    nk_stroke_rect(nk_window_get_canvas(ui),bounds,3,2,UI_COLOR(accent));
    if(!a->doc_focus_reveal)return;
    struct nk_rect clip=nk_window_get_content_region(ui);float delta=0;
    if(bounds.y<clip.y)delta=bounds.y-clip.y;
    else if(bounds.y+bounds.h>clip.y+clip.h)delta=bounds.y+bounds.h-clip.y-clip.h;
    if(body) {if(delta)a->doc_scroll_adjustment=delta;}
    else {nk_uint x,y;nk_window_get_scroll(ui,&x,&y);if(delta)nk_window_set_scroll(ui,x,(nk_uint)fmaxf(0,(float)y+delta));}
    if(!delta)a->doc_focus_reveal=false;
}
#include "documentation_media.inc"
static bool documentation_topic_button(app *a,const char *label,bool selected) {
    struct nk_context *ui=a->doc_ui;struct nk_rect bounds;
    enum nk_widget_layout_states visible=nk_widget(&bounds,ui);
    if(visible==NK_WIDGET_INVALID)return false;
    bool pressed=ps_ui_button_at(ui,bounds,label,selected);
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    bool hover=nk_input_is_mouse_hovering_rect(&ui->input,bounds);
    if(selected || hover)nk_fill_rect(canvas,bounds,4,selected?UI_COLOR(selection_active):UI_COLOR(button_hover));
    const struct nk_user_font *font=a->doc_font_ui;
    size_t at=0,n=strlen(label);float y=bounds.y+8;
    do {
        size_t length,used=documentation_line(font,label+at,n-at,bounds.w-20,&length);
        nk_draw_text(canvas,nk_rect(bounds.x+10,y,bounds.w-20,font->height+6),
            label+at,(int)length,font,nk_rgba(0,0,0,0),UI_COLOR(text));
        at+=used;y+=font->height+6;
    } while(at<n);
    return pressed;
}
static bool documentation_zoom_button(app *a,int direction) {
    struct nk_context *ui=a->doc_ui;struct nk_rect bounds;
    if(nk_widget(&bounds,ui)==NK_WIDGET_INVALID)return false;
    const char *name=direction<0?"Leseschrift verkleinern":direction>0?"Leseschrift vergrößern":"Standardleseschrift";
    bool pressed=ps_ui_button_at(ui,bounds,name,false);
    struct nk_command_buffer *c=nk_window_get_canvas(ui);
    bool enabled=!ui->current->widgets_disabled;
    bool hover=enabled && nk_input_is_mouse_hovering_rect(&ui->input,bounds);
    nk_fill_rect(c,bounds,7,hover?UI_COLOR(button_hover):UI_COLOR(button));
    struct nk_color color=enabled?UI_COLOR(text):UI_COLOR(muted);
    if(direction) {
        float x=bounds.x+bounds.w/2,y=bounds.y+bounds.h/2;
        nk_stroke_line(c,x-8,y,x+8,y,2,color);
        if(direction>0)nk_stroke_line(c,x,y-8,x,y+8,2,color);
    } else {
        char value[24];snprintf(value,sizeof value,"%u px",documentation_font_size(a));
        float width=a->doc_font_ui->width(a->doc_font_ui->userdata,a->doc_font_ui->height,value,(int)strlen(value));
        nk_draw_text(c,nk_rect(bounds.x+(bounds.w-width)/2,bounds.y+(bounds.h-a->doc_font_ui->height)/2,width+1,a->doc_font_ui->height),
            value,(int)strlen(value),a->doc_font_ui,nk_rgba(0,0,0,0),color);
    }
    if(hover)nk_tooltip(ui,name);
    return pressed;
}
static void documentation_sidebar(app *a) {
    struct nk_context *ui = a->doc_ui;
    nk_layout_row_dynamic(ui,documentation_height(a->doc_font_ui,"PHYSIM HANDBUCH",15,
        nk_window_get_content_region(ui).w-8)+8,1);
    nk_style_push_color(ui,&ui->style.text.color,UI_COLOR(muted));
    nk_label_wrap(ui,"PHYSIM HANDBUCH");nk_style_pop_color(ui);
    nk_layout_row_dynamic(ui,documentation_control_height(a),1);
    a->doc_track_bounds[0]=nk_widget_bounds(ui);
    bool c_selected=a->doc_track==1;
    if(nk_selectable_label(ui,"Teil I · C",NK_TEXT_LEFT,&c_selected)) {
        open_documentation(a,DOCUMENTATION_C_GUIDE_TOPIC);
        a->doc_query[0]=a->doc_filter[0]=0;documentation_filter(a);a->doc_contents=false;
    }
    documentation_focus_draw(a,DOC_C,a->doc_track_bounds[0],false);
    nk_layout_row_dynamic(ui,documentation_control_height(a),1);
    a->doc_track_bounds[1]=nk_widget_bounds(ui);
    bool physim_selected=a->doc_track==2;
    if(nk_selectable_label(ui,"Teil II · Physim",NK_TEXT_LEFT,&physim_selected)) {
        open_documentation(a,DOCUMENTATION_PHYSIM_GUIDE_TOPIC);
        a->doc_query[0]=a->doc_filter[0]=0;documentation_filter(a);a->doc_contents=false;
    }
    documentation_focus_draw(a,DOC_PHYSIM,a->doc_track_bounds[1],false);
    nk_layout_row_dynamic(ui, documentation_control_height(a), 2);
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
            if (!documentation_contents_heading(a,b))
                continue;
            nk_layout_row_dynamic(ui, 36, 1);
            struct nk_rect bounds=nk_widget_bounds(ui);
            if (nk_button_label(ui, a->documentation->text + b->text))
                a->doc_jump_block = (int)i;
            documentation_focus_draw(a,DOC_HEADING+(int)i,bounds,false);
        }
        return;
    }
    nk_layout_row_dynamic(ui, documentation_height(a->doc_font_ui,"Alle Inhalte durchsuchen",24,
        nk_window_get_content_region(ui).w-8)+8,1);
    nk_label_wrap(ui, "Alle Inhalte durchsuchen");
    nk_layout_row_dynamic(ui, documentation_control_height(a), 1);
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
    const char *groups[] = {"App & Lernen", "Fachthemen", "Referenz", "Projekttechnik", "Quellen"};
    nk_layout_row_dynamic(ui, documentation_control_height(a), 1);
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
    int order[DOCUMENTATION_TOPIC_COUNT];unsigned ordered=documentation_topic_order(a,order);
    for (unsigned n = 0; n < ordered; n++) {
        int i=order[n];
        found++;
        const char *title=documentation_topics[i].title;
        nk_layout_row_dynamic(ui,documentation_height(a->doc_font_ui,title,strlen(title),
            nk_window_get_content_region(ui).w-20)+16,1);
        struct nk_rect bounds=nk_widget_bounds(ui);
        bool selected = a->documentation && a->doc_topic == i;
        if (documentation_topic_button(a,title,selected)) {
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
    a->doc_inline_clicked=-1;
    size_t reading_byte=SIZE_MAX;
    bool reading_top=true;
    float section_scroll=-1;
    ps_document *section_document=NULL;
    a->doc_scroll_pending=false;
    bool large=documentation_font_size(a)>=20;
    bool compact=documentation_font_size(a)>=24;
    bool narrow=!compact && (large || nk_window_get_content_region(ui).w<600);
    nk_layout_row_dynamic(ui, documentation_control_height(a), narrow?2:4);
    struct nk_rect back_bounds=nk_widget_bounds(ui);
    nk_begin_disabled(ui, !ps_doc_history_peek(&a->doc_history,-1));
    if (compact?ps_ui_icon_button(ui,PS_ICON_LEFT,"Zurück"):nk_button_label(ui, "Zurück"))
        documentation_history_go(a,-1);
    nk_end_disabled(ui);
    documentation_focus_draw(a,DOC_BACK,back_bounds,false);
    struct nk_rect forward_bounds=nk_widget_bounds(ui);
    nk_begin_disabled(ui,!ps_doc_history_peek(&a->doc_history,1));
    if(compact?ps_ui_icon_button(ui,PS_ICON_RIGHT,"Vorwärts"):nk_button_label(ui,"Vorwärts"))documentation_history_go(a,1);
    nk_end_disabled(ui);documentation_focus_draw(a,DOC_FORWARD,forward_bounds,false);
    if(narrow)nk_layout_row_dynamic(ui,documentation_control_height(a),2);
    a->doc_home_bounds=nk_widget_bounds(ui);
    if (compact?ps_ui_icon_button(ui,PS_ICON_BOOK,"Start"):nk_button_label(ui, "Start"))
        open_documentation(a,a->doc_track==1?DOCUMENTATION_C_GUIDE_TOPIC:
                            a->doc_track==2?DOCUMENTATION_PHYSIM_GUIDE_TOPIC:1);
    documentation_focus_draw(a,DOC_HOME,a->doc_home_bounds,false);
    struct nk_rect workspace_bounds=nk_widget_bounds(ui);
    if (compact?ps_ui_icon_button(ui,PS_ICON_WINDOW,"Arbeitsbereich"):nk_button_label(ui, "Arbeitsbereich"))
        SDL_RaiseWindow(a->window);
    documentation_focus_draw(a,DOC_WORKSPACE,workspace_bounds,false);
    nk_layout_row_dynamic(ui,documentation_control_height(a),3);
    struct nk_rect zoom_out=nk_widget_bounds(ui);
    nk_begin_disabled(ui,documentation_font_size(a)<=16);
    if(documentation_zoom_button(a,-1))documentation_zoom(a,-1);
    nk_end_disabled(ui);documentation_focus_draw(a,DOC_ZOOM_OUT,zoom_out,false);
    struct nk_rect zoom_reset=nk_widget_bounds(ui);
    if(documentation_zoom_button(a,0))documentation_zoom(a,0);
    documentation_focus_draw(a,DOC_ZOOM_RESET,zoom_reset,false);
    struct nk_rect zoom_in=nk_widget_bounds(ui);
    nk_begin_disabled(ui,documentation_font_size(a)>=32);
    if(documentation_zoom_button(a,1))documentation_zoom(a,1);
    nk_end_disabled(ui);documentation_focus_draw(a,DOC_ZOOM_IN,zoom_in,false);
    if(large && !compact)nk_layout_row_dynamic(ui,documentation_control_height(a),1);
    else {nk_layout_row_begin(ui, NK_DYNAMIC, documentation_control_height(a), 3);nk_layout_row_push(ui,.60f);}
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
        const char *placeholder = compact?"Suchen …":"Dieses Dokument durchsuchen …";
        nk_draw_text(nk_window_get_canvas(ui),
                     nk_rect(search_bounds.x + 10, search_bounds.y + 8, search_bounds.w - 20, 20),
                     placeholder, (int)strlen(placeholder), a->doc_font_ui, nk_rgba(0, 0, 0, 0),
                     UI_COLOR(muted));
    }
    if(large && !compact)nk_layout_row_dynamic(ui,documentation_control_height(a),2);else nk_layout_row_push(ui,.20f);
    struct nk_rect previous_bounds=nk_widget_bounds(ui);
    if (compact?ps_ui_icon_button(ui,PS_ICON_LEFT,"Vorheriger Treffer"):nk_button_label(ui, "Vorheriger")) {
        a->doc_match--;
        a->doc_move_match = true;
    }
    documentation_focus_draw(a,DOC_PREVIOUS,previous_bounds,false);
    if(!large || compact)nk_layout_row_push(ui,.20f);
    struct nk_rect next_bounds=nk_widget_bounds(ui);
    if (compact?ps_ui_icon_button(ui,PS_ICON_RIGHT,"Nächster Treffer"):nk_button_label(ui, "Nächster")) {
        a->doc_match++;
        a->doc_move_match = true;
    }
    documentation_focus_draw(a,DOC_NEXT,next_bounds,false);
    if(!large || compact)nk_layout_row_end(ui);
    if (strcmp(a->doc_last_query, a->doc_query)) {
        snprintf(a->doc_last_query, sizeof a->doc_last_query, "%s", a->doc_query);
        a->doc_match = 0;
        a->doc_move_match = true;
    }
    ps_document *d = a->documentation;
    if(d && nk_sdl_accessibility_document_scroll(ui,a->doc_reading_version,&reading_byte,&reading_top)) {
        for(size_t i=0;i<d->count;i++)if(d->blocks[i].reading_begin!=SIZE_MAX && d->blocks[i].reading_begin<=reading_byte)
            a->doc_jump_block=(int)i;
    }
    int matches[PS_DOC_MAX_BLOCKS], match_count = 0;
    if (d && a->doc_query[0])
        for (size_t i = 0; i < d->count; i++)
            if (d->blocks[i].inline_parent==SIZE_MAX && ps_document_find(d->text + d->blocks[i].text, a->doc_query))
                matches[match_count++] = (int)i;
    if (match_count)
        a->doc_match = (a->doc_match % match_count + match_count) % match_count;
    else
        a->doc_match = 0;
    char detail[192];
    if (a->doc_error[0])
        snprintf(detail, sizeof detail, "%s", a->doc_error);
    else if (a->doc_query[0])
        snprintf(detail, sizeof detail, "%s · Treffer %d/%d Abschnitte",
                 d ? documentation_topics[a->doc_topic].title : "",match_count?a->doc_match+1:0,match_count);
    else if(compact)
        snprintf(detail,sizeof detail,"%s · %u px",d?documentation_topics[a->doc_topic].title:"Dokument nicht verfügbar",documentation_font_size(a));
    else
        snprintf(detail, sizeof detail, "%s · offline · %u px · " PS_UI_SHORTCUT "F sucht hier",
                 d ? documentation_topics[a->doc_topic].title : "Dokument nicht verfügbar",documentation_font_size(a));
    nk_layout_row_dynamic(ui,documentation_height(a->doc_font_ui,detail,strlen(detail),
        fmaxf(40,nk_window_get_content_region(ui).w-8))+8,1);
    nk_style_push_color(ui,&ui->style.text.color,a->doc_error[0]?UI_COLOR(error):UI_COLOR(muted));
    nk_label_wrap(ui,detail);nk_style_pop_color(ui);
    nk_layout_row_dynamic(ui,16,1);
    struct nk_rect remaining=nk_widget_bounds(ui),clip=nk_window_get_content_region(ui);
    nk_layout_row_dynamic(ui,fmaxf(20,fminf(height,clip.y+clip.h-remaining.y)),1);
    struct nk_rect body_bounds=nk_widget_bounds(ui);
    a->doc_body_bounds=body_bounds;
    documentation_focus_draw(a,DOC_BODY,body_bounds,false);
    if (nk_group_scrolled_offset_begin(ui, &a->doc_scroll_x, &a->doc_scroll_y, "document-body",
                                       0)) {
        if (d) {
            nk_sdl_accessibility_document(ui,a->doc_reading_version,documentation_topics[a->doc_topic].title,d->reading,nk_window_get_content_region(ui));
            for(size_t i=0;i<d->count;i++)d->blocks[i].hit[2]=0;
            float width = fmaxf(40, nk_window_get_content_region(ui).w - 8);
            if (d->layout_width != width) {
                for (size_t i = 0; i < d->count; i++) {
                    ps_doc_block *b = &d->blocks[i];
                    b->height =
                        b->kind == PS_DOC_TABLE_ROW
                            ? documentation_table(a, b, d->text + b->text, width, NULL)
                        : b->kind == PS_DOC_IMAGE && documentation_media_kind(d->text+b->target)==3 ? 900
                        : b->kind == PS_DOC_IMAGE && documentation_media_kind(d->text+b->target) ? (documentation_media_kind(d->text+b->target)==1?320:480)
                        : b->kind == PS_DOC_RULE ? 14
                        : b->kind == PS_DOC_LINK && b->inline_parent!=SIZE_MAX ? 0
                        : b->kind == PS_DOC_LINK
                            ? 32
                            : documentation_rich_height(a,d,b,width - 24) +
                                  16;
                }
                d->layout_width = width;
            }
            int target = a->doc_jump_block >= 0
                             ? a->doc_jump_block
                             : (a->doc_move_match && match_count ? matches[a->doc_match] : -1);
            bool explicit_jump=a->doc_jump_block>=0;
            if(explicit_jump)a->doc_anchor_heading=target;
            float body_origin=nk_window_get_content_region(ui).y;
            nk_uint submitted_scroll=a->doc_scroll_y;
            section_document=d;
            /* Wrapped buttons and copy rows determine real geometry. Apply a
             * measured target only after this entire group has been submitted. */
            a->doc_jump_block = -1;
            a->doc_move_match = false;
            for (size_t i = 0; i < d->count; i++) {
                ps_doc_block *b = &d->blocks[i];
                const char *text = d->text + b->text;
                const struct nk_user_font *font = documentation_block_font(a,b);
                if(b->kind==PS_DOC_LINK && b->inline_parent!=SIZE_MAX) {
                    struct nk_rect link_bounds=nk_rect(b->hit[0],b->hit[1],b->hit[2],b->hit[3]);
                    if(link_bounds.w>0) {
                        bool external=!strncmp(d->text+b->target,"http",4);char label[1024];
                        snprintf(label,sizeof label,"%s%s",external?"Quellenadresse kopieren: ":"Öffnen: ",text);
                        if(ps_ui_accessibility_button(ui,link_bounds,label))a->doc_inline_clicked=(int)i;
                        documentation_focus_draw(a,DOC_LINK+(int)i,link_bounds,true);
                        if((int)i==target)section_scroll=fmaxf(0,link_bounds.y+(float)submitted_scroll-body_origin);
                    }
                    continue;
                }
                if (b->kind == PS_DOC_CODE) {
                    nk_layout_row_dynamic(ui, 28, 2);
                    nk_label_colored(ui, "Code", NK_TEXT_LEFT, UI_COLOR(muted));
                    struct nk_rect copy_bounds=nk_widget_bounds(ui);
                    if (nk_button_label(ui, "Code kopieren"))
                        SDL_SetClipboardText(text);
                    documentation_focus_draw(a,DOC_COPY+(int)i,copy_bounds,true);
                }
                if(b->kind==PS_DOC_IMAGE && documentation_media_kind(d->text+b->target)==3) {
                    documentation_pendulum(a,(int)i,width,b->height);
                    if((int)i==a->doc_anchor_heading)a->doc_anchor_bounds=a->doc_visual_bounds[0];
                    if((int)i==target)section_scroll=fmaxf(0,a->doc_visual_bounds[0].y+(float)submitted_scroll-body_origin);
                    continue;
                }
                nk_layout_row_dynamic(ui, b->height, 1);
                struct nk_rect block_bounds=nk_widget_bounds(ui);
                b->hit[0]=block_bounds.x;b->hit[1]=block_bounds.y;b->hit[2]=block_bounds.w;b->hit[3]=block_bounds.h;
                if((int)i==a->doc_anchor_heading)a->doc_anchor_bounds=block_bounds;
                if((int)i==target) {
                    float prefix=0;
                    size_t reading_offset=reading_byte!=SIZE_MAX && b->reading_begin!=SIZE_MAX && reading_byte>=b->reading_begin?
                        reading_byte-b->reading_begin:0;
                    if(reading_offset>b->length)reading_offset=b->length;
                    const char *match=reading_byte!=SIZE_MAX?text+reading_offset:ps_document_find(text,a->doc_query);
                    if((!explicit_jump || reading_byte!=SIZE_MAX) && b->kind!=PS_DOC_TABLE_ROW && match && match>text) {
                        ps_doc_block preceding=*b;preceding.length=(size_t)(match-text);
                        prefix=fmaxf(0,documentation_rich_height(a,d,&preceding,width-24)-(font->height+6));
                    }
                    section_scroll=explicit_jump && target==0 && reading_byte==SIZE_MAX?0:fmaxf(0,block_bounds.y+(float)submitted_scroll-body_origin+prefix);
                    if(reading_byte!=SIZE_MAX && !reading_top)
                        section_scroll=fmaxf(0,section_scroll-fmaxf(0,nk_window_get_content_region(ui).h-font->height-22));
                }
                if (b->kind == PS_DOC_LINK) {
                    bool external = !strncmp(d->text + b->target, "http", 4);
                    char label[240];
                    snprintf(label, sizeof label, "%s%s",
                             external ? "Quellenadresse kopieren: " : "Öffnen: ", text);
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
                    bool link_geometry=(target>=0 && d->blocks[target].inline_parent==i) ||
                        (a->doc_keyboard && a->doc_focus>=DOC_LINK && a->doc_focus<DOC_LINK+(int)d->count &&
                         d->blocks[a->doc_focus-DOC_LINK].inline_parent==i);
                    if (state != NK_WIDGET_INVALID || link_geometry) {
                        struct nk_command_buffer *canvas = nk_window_get_canvas(ui);
                        if(b->kind==PS_DOC_IMAGE && documentation_media_kind(d->text+b->target))
                            documentation_diagram(a,documentation_media_kind(d->text+b->target),rect);
                        else if (b->kind == PS_DOC_TABLE_ROW)
                            documentation_table(a, b, text, width, &rect);
                        else if (b->kind == PS_DOC_RULE)
                            nk_stroke_line(canvas, rect.x, rect.y + 7, rect.x + rect.w, rect.y + 7,
                                           1, UI_COLOR(border));
                        else {
                            if (b->kind == PS_DOC_CODE) {
                                float x=fmaxf(rect.x,canvas->clip.x),y=fmaxf(rect.y,canvas->clip.y);
                                float right=fminf(rect.x+rect.w,canvas->clip.x+canvas->clip.w);
                                float bottom=fminf(rect.y+rect.h,canvas->clip.y+canvas->clip.h);
                                if(right>x && bottom>y)nk_fill_rect(canvas,nk_rect(x,y,right-x,bottom-y),7,UI_COLOR(edit));
                            }
                            documentation_rich_draw(a,d,b,nk_rect(rect.x+12,rect.y+8,width-24,rect.h-16));
                        }
                    }
                }
            }
        }
        nk_group_scrolled_end(ui);
    }
    if(a->doc_inline_clicked>=0 && a->documentation==d) {
        char *target=SDL_strdup(d->text+d->blocks[a->doc_inline_clicked].target);
        if(target){documentation_link(a,target);SDL_free(target);}
        else snprintf(a->doc_error,sizeof a->doc_error,"Link konnte nicht geöffnet werden: %s",SDL_GetError());
    }
    if(section_scroll>=0 && a->documentation==section_document && a->doc_jump_block<0) {
        a->doc_scroll_y=(nk_uint)section_scroll;a->doc_scroll_adjustment=0;a->doc_scroll_pending=true;
    }
    if(a->doc_scroll_adjustment) {
        a->doc_scroll_y=(nk_uint)fmaxf(0,(float)a->doc_scroll_y+a->doc_scroll_adjustment);
        a->doc_scroll_adjustment=0;
    }
    if(a->doc_history_restore) {
        const ps_doc_location *p=&a->doc_history.entries[a->doc_history.index];
        a->doc_scroll_x=p->scroll_x;a->doc_scroll_y=p->scroll_y;
        --a->doc_history_restore;
    } else documentation_history_save(a);
}
