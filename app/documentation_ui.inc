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
    {"F1-Schriften nach Bedarf", "docs/decisions/005-f1-font-demand.md", false},
    {"F1-Tabellen und native Semantik", "docs/decisions/006-f1-table-semantics.md", false},
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
static int documentation_index_track[DOCUMENTATION_TOPIC_COUNT];
static bool documentation_hits[DOCUMENTATION_TOPIC_COUNT];
/* One language context applies to navigation, search and parsed article code.
 * Common explanations remain verbatim. Untagged/tooling fences remain visible. */
static void documentation_language_source(char *source,int track) {
    if(!source)return;
    char *read=source,*write=source;bool fenced=false,omit=false;unsigned omitted_section=0;
    while(*read) {
        char *end=strchr(read,'\n');size_t n=end?(size_t)(end+1-read):strlen(read);
        if(!fenced && read[0]=='#') {
            unsigned depth=0;while(depth<n && read[depth]=='#')++depth;
            if(omitted_section && depth<=omitted_section)omitted_section=0;
            char heading[256];size_t length=n<sizeof heading-1?n:sizeof heading-1;
            memcpy(heading,read,length);heading[length]=0;
            if((track==2 && strstr(heading,"C-Quellcode")) || (track!=2 && strstr(heading,"Physim-Quellcode")))
                omitted_section=depth;
        }
        if(omitted_section){if(n>=3 && !memcmp(read,"```",3))fenced=!fenced;read+=n;continue;}
        if(n>=3 && !memcmp(read,"```",3)) {
            if(!fenced) {
                const char *tag=read+3;
                bool c=(*tag=='c' && (tag[1]=='\r' || tag[1]=='\n' || tag[1]==0));
                bool physim=!strncmp(tag,"physim",6) || !strncmp(tag,"phys\n",5) || !strncmp(tag,"phys\r",5);
                omit=(track==2 && c) || (track!=2 && physim);fenced=true;
                if(!omit){memmove(write,read,n);write+=n;}
            } else {
                if(!omit){memmove(write,read,n);write+=n;}
                fenced=false;omit=false;
            }
        } else if(!omit){memmove(write,read,n);write+=n;}
        read+=n;
    }
    *write=0;
}
static bool documentation_topic_language(int topic,int track) {
    const char *p=documentation_topics[topic].path;
    bool physim=strstr(p,"/language") || strstr(p,"/physim-") || strstr(p,"batch-language") || strstr(p,".phys");
    bool c=strstr(p,"/c-guide") || strstr(p,"/c-workflow") || strstr(p,"experiment-tutorial") ||
        !strncmp(p,"include/",8) || strstr(p,".c") ||
        (!strncmp(p,"docs/reference/",15) && !strstr(p,"language-library"));
    return track==2?!c:!physim;
}
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
        if(documentation_index_track[i]!=a->doc_track){free(documentation_index[i]);documentation_index[i]=NULL;}
        if (a->doc_filter[0] && !documentation_index[i]) {
            char path[4096];
            size_t length = 0;
            join(path, sizeof path, a->root, documentation_topics[i].path);
            documentation_index[i] = load_utf8_text_limit(path, &length,
                documentation_topics[i].code?PS_DOC_MAX_CODE_BYTES:PS_DOC_MAX_BYTES);
            if(!documentation_topics[i].code)documentation_language_source(documentation_index[i],a->doc_track);
            documentation_index_track[i]=a->doc_track;
        }
        documentation_hits[i] = documentation_topic_language(i,a->doc_track) && (!a->doc_filter[0] ||
                                ps_document_find(documentation_topics[i].title, a->doc_filter) ||
                                ps_document_find(documentation_index[i], a->doc_filter));
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
    int track=topic==DOCUMENTATION_C_GUIDE_TOPIC?1:topic==DOCUMENTATION_PHYSIM_GUIDE_TOPIC?2:(a->doc_track?a->doc_track:1);
    /* Explicit language-specific links update the same global context. */
    if(!documentation_topic_language(topic,track) && documentation_topic_language(topic,track==2?1:2))track=track==2?1:2;
    if (!documentation_window_open(a))
        return;
    if (a->documentation && topic == a->doc_topic && a->doc_loaded_track==track) {
        if(a->doc_track!=track){a->doc_track=track;documentation_filter(a);}
        a->doc_error[0] = 0;
        return;
    }
    char path[4096];
    join(path, sizeof path, a->root, documentation_topics[topic].path);
    size_t length = 0;
    char *source = load_utf8_text_limit(path, &length,documentation_topics[topic].code?PS_DOC_MAX_CODE_BYTES:PS_DOC_MAX_BYTES);
    ps_document *document = NULL;
    ps_result r = PS_IO;
    if(source && !documentation_topics[topic].code)documentation_language_source(source,track);
    if(source)length=strlen(source);
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
    if(a->doc_track!=track){a->doc_track=track;documentation_filter(a);}
    a->doc_loaded_track=track;
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
static void documentation_language(app *a,int track) {
    if(a->doc_track==track && a->doc_topic!=1)return;
    documentation_history_save(a);
    int topic=a->doc_topic;
    const char *p=documentation_topics[topic].path;
    const char *pairs[][2]={{"docs/c-guide.md","docs/physim-guide.md"},
        {"docs/c-workflow.md","docs/physim-workflow.md"},
        {"docs/experiment-tutorial.md","docs/language-tutorial.md"},
        {"docs/api.md","docs/reference/language-library.md"}};
    bool mapped=topic==1;
    if(mapped)topic=documentation_learning_topic(track);
    for(unsigned n=0;n<sizeof pairs/sizeof pairs[0];n++)if(!strcmp(p,pairs[n][0]) || !strcmp(p,pairs[n][1])) {
        for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++)if(!strcmp(documentation_topics[i].path,pairs[n][track==2])){topic=i;mapped=true;break;}
        break;
    }
    if(!mapped && !documentation_topic_language(topic,track))topic=documentation_learning_topic(track);
    int previous_track=a->doc_track,previous_topic=a->doc_topic;
    a->doc_track=track;
    /* Reparse shared content with the new code scope; the reading offset is
     * preserved for shared physics pages and mapped by normal history otherwise. */
    nk_uint x=a->doc_scroll_x,y=a->doc_scroll_y;bool shared=topic==a->doc_topic;
    a->doc_history_navigation=true;open_documentation(a,topic);a->doc_history_navigation=false;
    if(a->doc_error[0]){a->doc_topic=previous_topic;a->doc_track=previous_track;return;}
    if(!a->doc_error[0]){ps_doc_location location=documentation_location(a);ps_doc_history_visit(&a->doc_history,&location);}
    if(shared){a->doc_scroll_x=x;a->doc_scroll_y=y;a->doc_move_match=false;a->doc_history_restore=2;}
    documentation_filter(a);
}
static void documentation_history_go(app *a,int direction) {
    const ps_doc_location *entry=ps_doc_history_peek(&a->doc_history,direction);
    if(!entry)return;
    ps_doc_location target=*entry;documentation_history_save(a);
    if(target.track==1 || target.track==2)a->doc_track=target.track;
    a->doc_history_navigation=true;open_documentation(a,target.topic);a->doc_history_navigation=false;
    if(a->doc_error[0] || a->doc_topic!=target.topic)return;
    ps_doc_history_move(&a->doc_history,direction);
    if(target.track==1 || target.track==2)a->doc_track=target.track;
    a->doc_contents=target.contents;
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
    return a->doc_zoom_size?a->doc_zoom_size:a->preferences.editor_size<16?16:a->preferences.editor_size;
}
static float documentation_control_height(const app *a) {
    return (float)documentation_font_size(a)+16;
}
#include "documentation_typography.inc"
static void documentation_zoom(app *a,int direction) {
    unsigned old=documentation_font_size(a);
    int size=direction?(int)old+direction*2:0;
    unsigned requested=direction?(unsigned)(size<16?16:size>32?32:size):0;
    unsigned now=requested?requested:a->preferences.ui_size;
    if(!documentation_fonts_prepare(a,now,requested?requested:a->preferences.editor_size))return;
    a->doc_zoom_size=requested;
    if(now!=old && !a->doc_query[0] && a->documentation)
        nk_sdl_accessibility_document_preserve_position(a->doc_ui,a->doc_reading_version);
    a->doc_font_ui=a->doc_ui_fonts[(now-16)/2];
    a->doc_font_title=a->doc_title_fonts[(now-16)/2];
    a->doc_font_code=a->doc_code_fonts[(documentation_code_size(a)-16)/2];
    documentation_typography_update(a);
    nk_style_set_font(a->doc_ui,a->doc_font_ui);nk_sdl_set_ui_size(a->doc_ui,a->preferences.ui_size);
    if(a->documentation)a->documentation->layout_width=0;
    a->doc_scroll_y=(nk_uint)((double)a->doc_scroll_y*now/old);
    /* Reflow changes the match's line position, not just its pixel scale. */
    if(now!=old && a->doc_query[0])a->doc_move_match=true;
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
    if(b->kind==PS_DOC_CODE)return documentation_type_font(a,b->code_line_height>0?DOC_TYPE_GRID:DOC_TYPE_CODE);
    if(b->kind==PS_DOC_HEADING) {
        if(b->level==1)return documentation_type_font(a,DOC_TYPE_TITLE);
        return documentation_type_font(a,b->level==2?DOC_TYPE_H2:DOC_TYPE_H3);
    }
    if(b->kind==PS_DOC_TABLE_ROW)return documentation_type_font(a,b->level?DOC_TYPE_TABLE_STRONG:DOC_TYPE_TABLE);
    if(documentation_is_lead(a,b))return documentation_type_font(a,DOC_TYPE_LEAD);
    return documentation_type_font(a,DOC_TYPE_BODY);
}
#include "documentation_math.inc"
static const ps_doc_span *documentation_span_at(const ps_document *d,const ps_doc_block *b,size_t at) {
    size_t lo=b->first_span,hi=lo+b->span_count;
    while(lo<hi){size_t mid=lo+(hi-lo)/2;if(d->spans[mid].text<=at)lo=mid+1;else hi=mid;}
    if(lo>b->first_span){const ps_doc_span *s=&d->spans[lo-1];if(at<s->text+s->length)return s;}
    return NULL;
}
static const struct nk_user_font *documentation_span_font(const app *a,const ps_doc_block *b,const ps_doc_span *s) {
    if(b->kind==PS_DOC_HEADING)return documentation_block_font(a,b);
    if(s && (s->style&PS_DOC_INLINE_CODE))return documentation_type_font(a,b->kind==PS_DOC_TABLE_ROW?DOC_TYPE_TABLE_CODE:
        documentation_is_lead(a,b)?DOC_TYPE_LEAD_CODE:DOC_TYPE_INLINE_CODE);
    if(s && (s->style&PS_DOC_BOLD))return documentation_type_font(a,b->kind==PS_DOC_TABLE_ROW?DOC_TYPE_TABLE_STRONG:
        documentation_is_lead(a,b)?DOC_TYPE_LEAD_STRONG:DOC_TYPE_STRONG);
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
static float documentation_grid_pitch(const app *a,const ps_document *d,const ps_doc_block *b) {
    if(b->kind!=PS_DOC_CODE)return 0;
    bool grid=false;const char *text=d->text+b->text;
    for(size_t at=0;at<b->length;) {
        nk_rune rune;int n=nk_utf_decode(text+at,&rune,(int)(b->length-at));if(n<=0)return 0;
        if(rune>=0x2500 && rune<=0x257f){grid=true;break;}at+=(size_t)n;
    }
    if(!grid)return 0;
    const struct nk_user_font *font=documentation_type_font(a,DOC_TYPE_GRID);
    struct nk_user_font_glyph vertical={0};font->query(font->userdata,font->height,&vertical,0x2502,0);
    /* The first/last antialiased row is not an opaque rule. Let those edge
     * rows overlap; complete glyphs still draw with their measured bounds. */
    return ps_ui_font_has_glyph(font,0x2502) && vertical.height>1?vertical.height-1:0;
}
static float documentation_font_baseline(const struct nk_user_font *font) {
    const struct nk_font *native=font->userdata.ptr;
    return native->info.ascent*font->height/native->info.height;
}
#include "documentation_article_flow.inc"
static const ps_doc_formula_variant *documentation_inline_variant(const app *a,const ps_doc_span *span,float width) {
    if(!span || !span->formula)return NULL;
    const ps_doc_formula *f=span->formula;float zoom=(documentation_font_size(a)/16.f);
    if(f->inline_variant->width*zoom<=width)return f->inline_variant;
    for(size_t i=f->count;i>0;i--)if(f->variants[i-1].width*zoom<=width)return &f->variants[i-1];
    return NULL;
}
static size_t documentation_rich_line_metrics(const app *a,const ps_document *d,const ps_doc_block *b,
    size_t at,float width,size_t *length,float *height,float *baseline) {
    const char *text=d->text+b->text;size_t begin=at,space=SIZE_MAX;
    const struct nk_user_font *base=documentation_block_font(a,b);
    float used=0,h=documentation_block_line_height(a,b),space_height=h;
    float above=documentation_font_baseline(base)+(h-base->height)/2,below=h-above,space_above=above;
    while(at<b->length) {
        if(text[at]=='\r'){*length=at-begin;*height=h;*baseline=above;return at-begin+(at+1<b->length && text[at+1]=='\n'?2:1);}
        if(text[at]=='\n'){*length=at-begin;*height=h;*baseline=above;return at-begin+1;}
        const ps_doc_span *span=documentation_span_at(d,b,b->text+at);
        const struct nk_user_font *font=documentation_span_font(a,b,span);
        float glyph;size_t bytes=documentation_rich_cluster(font,text+at,b->length-at,&glyph);
        const ps_doc_formula_variant *math=documentation_inline_variant(a,span,width);
        if(math) {bytes=span->length;glyph=math->width*(documentation_font_size(a)/16.f);}
        if(at>begin && used+glyph>width) {
            if(space!=SIZE_MAX && space>begin){*length=space-begin;*height=space_height;*baseline=space_above;return space-begin+1;}
            *length=at-begin;*height=h;*baseline=above;return at-begin;
        }
        if(b->code_line_height<=0) {
            float line=documentation_span_line_height(a,b,span);
            float ascent=math?math->baseline*(documentation_font_size(a)/16.f)+3:
                documentation_font_baseline(font)+(line-font->height)/2;
            float descent=math?(math->height-math->baseline)*(documentation_font_size(a)/16.f)+3:line-ascent;
            above=fmaxf(above,ascent);below=fmaxf(below,descent);h=above+below;
        }
        if(!math && (text[at]==' ' || text[at]=='\t')){space=at;space_height=h;space_above=above;}
        used+=glyph;at+=(size_t)bytes;
    }
    *length=at-begin;*height=h;*baseline=above;return at-begin;
}
static size_t documentation_rich_line(const app *a,const ps_document *d,const ps_doc_block *b,
    size_t at,float width,size_t *length,float *height) {
    float baseline;return documentation_rich_line_metrics(a,d,b,at,width,length,height,&baseline);
}
static float documentation_rich_height(const app *a,const ps_document *d,const ps_doc_block *b,float width) {
    size_t at=0;float total=0;
    do {size_t length;float height;size_t used=documentation_rich_line(a,d,b,at,width,&length,&height);
        at+=used;total+=height;}while(at<b->length);
    return total;
}
static float documentation_rich_offset(const app *a,const ps_document *d,const ps_doc_block *b,size_t target,float width) {
    size_t at=0;float y=0;
    while(at<b->length) {
        size_t length;float height;size_t used=documentation_rich_line(a,d,b,at,width,&length,&height);
        if(target<at+used || at+used>=b->length)return y;
        at+=used;y+=height;
    }
    return y;
}
static void documentation_rich_draw(app *a,ps_document *d,const ps_doc_block *b,struct nk_rect bounds) {
    struct nk_context *ui=a->doc_ui;struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    const char *text=d->text+b->text;size_t at=0;float y=bounds.y;
    const char *match=NULL;bool searched=false;
    float grid_offset=0;
    if(b->code_line_height>0) {
        const struct nk_user_font *font=documentation_block_font(a,b);
        struct nk_user_font_glyph cell={0};font->query(font->userdata,font->height,&cell,0x2502,0);
        grid_offset=-cell.offset.y;
    }
    if(b->kind==PS_DOC_CODE && !strncmp(text,"┌──┬──┐",strlen("┌──┬──┐"))) {
        const struct nk_user_font *font=documentation_block_font(a,b);
        struct nk_user_font_glyph cell={0};font->query(font->userdata,font->height,&cell,0x2502,0);
        size_t length;float height;documentation_rich_line(a,d,b,0,bounds.w,&length,&height);
        if(cell.height>0) {
            a->doc_grid_probe[0]=bounds.x+cell.offset.x+cell.width/2;
            a->doc_grid_probe[1]=floorf(bounds.y+grid_offset)+cell.offset.y;
            a->doc_grid_probe[2]=documentation_glyph_width(font," ",1)*6;
            a->doc_grid_probe[3]=height;a->doc_grid_probe[4]=cell.height;
            a->doc_grid_probe[5]=canvas->clip.y+canvas->clip.h;
        }
    }
    do {
        size_t length;float height,baseline;size_t used=documentation_rich_line_metrics(a,d,b,at,bounds.w,&length,&height,&baseline);
        size_t end=at+length,pos=at;float x=bounds.x;
        bool visible=y<canvas->clip.y+canvas->clip.h && y+height>canvas->clip.y;
        if(b->kind==PS_DOC_CODE && y>canvas->clip.y+canvas->clip.h)break;
        while(pos<end) {
            const ps_doc_span *span=documentation_span_at(d,b,b->text+pos);
            const struct nk_user_font *font=documentation_span_font(a,b,span);
            size_t next=span?span->text+span->length-b->text:end;if(next>end)next=end;
            const ps_doc_formula_variant *math=documentation_inline_variant(a,span,bounds.w);
            float zoom=(documentation_font_size(a)/16.f);
            float width=math?math->width*zoom:documentation_rich_width(font,text+pos,next-pos);
            float draw_y=b->code_line_height>0?y+grid_offset:y+baseline-documentation_font_baseline(font);
            struct nk_rect part=math?nk_rect(x,y+baseline-math->baseline*zoom,width,math->height*zoom):
                nk_rect(x,y,width,height);
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
                    if(math)nk_sdl_accessibility_text_box(ui,b->reading_begin+pos,b->reading_begin+next,part);
                    for(size_t glyph=pos;!math && glyph<next;) {
                        nk_rune rune;int bytes=nk_utf_decode(text+glyph,&rune,(int)(next-glyph));if(bytes<=0)bytes=1;
                        float glyph_width=documentation_rich_advance(font,text+glyph,next-glyph,bytes);
                        struct nk_rect box=nk_rect(glyph_x,y,fmaxf(0,glyph_width),height);
                        if(b->code_line_height>0) {
                            struct nk_user_font_glyph ink={0};font->query(font->userdata,font->height,&ink,rune,0);
                            box=ink.width>0 && ink.height>0
                                ? nk_rect(floorf(glyph_x)+ink.offset.x,floorf(draw_y)+ink.offset.y,ink.width,ink.height)
                                : nk_rect(glyph_x,y,glyph_width,height);
                        }
                        nk_sdl_accessibility_text_box(ui,b->reading_begin+glyph,b->reading_begin+glyph+(size_t)bytes,box);
                        glyph_x+=glyph_width;glyph+=(size_t)bytes;
                    }
                }
                if(!searched || (match && match<text+pos)){match=ps_document_find(text+pos,a->doc_query);searched=true;}
                bool found=match && match<text+next;
                if(found)nk_stroke_rect(canvas,part,2,1,UI_COLOR(accent));
                struct nk_color color=link || found?UI_COLOR(accent):b->kind==PS_DOC_CODE?UI_COLOR(code_text):UI_COLOR(doc_text);
                if(math) {
                    unsigned texture=documentation_math_texture_for(a,math);
                    if(texture){struct nk_image image=nk_image_id((int)texture);nk_draw_image(canvas,part,&image,color);}
                    else nk_draw_text(canvas,nk_rect(x,draw_y,width+1,font->height+6),text+pos,(int)(next-pos),font,nk_rgba(0,0,0,0),color);
                }
                size_t chunk=pos;float draw_x=x;
                while(!math && chunk<next) {
                    const char *tab=memchr(text+chunk,'\t',next-chunk);size_t last=tab?(size_t)(tab-text):next;
                    float run_width=documentation_rich_width(font,text+chunk,last-chunk);
                    if(last>chunk)nk_draw_text(canvas,nk_rect(draw_x,draw_y,run_width+1,font->height+6),text+chunk,(int)(last-chunk),font,nk_rgba(0,0,0,0),color);
                    draw_x+=run_width;if(tab)draw_x+=documentation_glyph_width(font,"\t",1);chunk=last+(tab?1:0);
                }
                if(link) {
                    float underline=math?part.y+part.h-2:y+baseline+3;
                    nk_stroke_line(canvas,x,underline,x+width,underline,1,color);
                }
            }
            x+=width;pos=next;
        }
        at+=used;y+=height;
    } while(at<b->length);
}
static void documentation_inline_math_draw_probe(app *a) {
    if(!a->doc_inline_math_test_requested)return;
    a->doc_inline_math_test_requested=false;a->doc_inline_math_test_ok=false;
    ps_document *d=a->documentation;if(!d || a->doc_inline_math_test_block>=d->count || !a->doc_math)return;
    const ps_doc_block *b=&d->blocks[a->doc_inline_math_test_block];unsigned count=0;bool okay=true;
    for(size_t i=0;i<b->span_count;i++) {
        const ps_doc_span *span=&d->spans[b->first_span+i];if(!span->formula)continue;
        const ps_doc_formula_variant *v=documentation_inline_variant(a,span,b->hit[2]-24);unsigned texture=0;
        float zoom=(documentation_font_size(a)/16.f);
        if(v) {
            float scale=zoom*a->doc_raster_density;
            unsigned width=(unsigned)fmin(v->raster_width,ceil(v->width*scale));
            unsigned height=(unsigned)fmin(v->raster_height,ceil(v->height*scale));
            for(size_t j=0;j<a->doc_math->count;j++) {
                const documentation_math_texture *item=&a->doc_math->items[j];
                if(item->variant==v && item->width==width && item->height==height)texture=item->texture;
            }
        }
        const struct nk_command *command;struct nk_rect clip={0},frame={0};unsigned images=0;
        nk_foreach(command,a->doc_ui) {
            if(command->type==NK_COMMAND_SCISSOR) {
                const struct nk_command_scissor *s=(const struct nk_command_scissor *)command;clip=nk_rect(s->x,s->y,s->w,s->h);
            } else if(command->type==NK_COMMAND_IMAGE) {
                const struct nk_command_image *image=(const struct nk_command_image *)command;
                if((unsigned)image->img.handle.id!=texture || image->y<b->hit[1]-1 || image->y>=b->hit[1]+b->hit[3])continue;
                frame=nk_rect(image->x,image->y,image->w,image->h);++images;
                okay &= v && fabsf(frame.w-v->width*zoom)<=1 && fabsf(frame.h-v->height*zoom)<=1 &&
                    frame.x>=clip.x-1 && frame.y>=clip.y-1 && frame.x+frame.w<=clip.x+clip.w+1 && frame.y+frame.h<=clip.y+clip.h+1;
            }
        }
        okay &= images==1 && texture!=0;
        /* The short fixture places prose on the same baseline as its equation. */
        if(v && v->width*zoom<(b->hit[2]-24)*.5f && b->span_count<10) {
            bool aligned=false;
            nk_foreach(command,a->doc_ui)if(command->type==NK_COMMAND_TEXT) {
                const struct nk_command_text *text=(const struct nk_command_text *)command;
                if(text->y<b->hit[1]-1 || text->y>=b->hit[1]+b->hit[3])continue;
                float base=text->y+documentation_font_baseline(text->font);
                if(fabsf(base-(frame.y+v->baseline*zoom))<=1.5f)aligned=true;
            }
            okay &= aligned;
        }
        size_t begin=b->reading_begin+span->text-b->text;
        okay &= nk_sdl_accessibility_document_visible(a->doc_ui,a->doc_reading_version,begin) &&
            nk_sdl_accessibility_document_visible(a->doc_ui,a->doc_reading_version,begin+span->length-1);
        if(span->style&PS_DOC_INLINE_LINK)a->doc_inline_math_test_link=frame;
        printf("INLINE MATH DRAW: case=%u item=%u font=%.0f theme=%u complete=%d frame=%.0f,%.0f,%.0f,%.0f\n",
            a->doc_inline_math_test_case,count++,a->doc_font_ui->height,a->preferences.theme,okay,frame.x,frame.y,frame.w,frame.h);
    }
    a->doc_inline_math_test_ok=okay && count>0;
}
#include "documentation_tables.inc"
/* Layout and native text geometry use the parser's identical cell boundaries. */
static float documentation_table(app *a, const ps_doc_block *block, float width,
                                 const struct nk_rect *bounds) {
    size_t count=block->cell_count;
    if (!count)
        return 24;
    size_t columns=a->documentation->blocks[block->table_first].table_columns;
    float cell_width = width / (float)columns, height = 24;
    for (size_t i = 0; i < count; i++) {
        const ps_doc_cell *range=&a->documentation->cells[block->first_cell+i];
        ps_doc_block cell=*block;cell.text=range->text;cell.length=range->length;
        height =
            fmaxf(height,
                  documentation_rich_height(a,a->documentation,&cell,fmaxf(1,cell_width-16))+25);
    }
    if (!bounds)
        return height;
    struct nk_command_buffer *canvas = nk_window_get_canvas(a->doc_ui);
    nk_fill_rect(canvas, *bounds, 0, block->level ? UI_COLOR(doc_header) : UI_COLOR(doc_body));
    nk_fill_rect(canvas,nk_rect(bounds->x,bounds->y+bounds->h-1,bounds->w,1),0,UI_COLOR(divider));
    for (size_t i = 0; i < count; i++) {
        const ps_doc_cell *range=&a->documentation->cells[block->first_cell+i];
        ps_doc_block cell=*block;cell.text=range->text;cell.length=range->length;
        if(cell.reading_begin!=SIZE_MAX)cell.reading_begin+=cell.text-block->text;
        documentation_rich_draw(a,a->documentation,&cell,nk_rect(bounds->x+(float)i*cell_width+8,
            bounds->y+12,fmaxf(1,cell_width-16),bounds->h-25));
    }
    return height;
}
static bool documentation_link(app *a, const char *target) {
    if (!strncmp(target, "https://", 8) || !strncmp(target, "http://", 7)) {
        if(!SDL_SetClipboardText(target)){status(a,"Quellenadresse konnte nicht kopiert werden.");return false;}
        status(a,"Quellenadresse kopiert. Das Handbuch bleibt in Physim geöffnet.");
        return true;
    }
    char resolved[4096],fragment[4096];
    const char *hash=strchr(target,'#');
    size_t path_length=hash?(size_t)(hash-target):strlen(target);
    if(path_length>=sizeof resolved || (hash && strlen(hash+1)>=sizeof fragment)) {
        status(a,"Dieses Linkziel ist zu lang.");return false;
    }
    if(hash)snprintf(fragment,sizeof fragment,"%s",hash+1);
    const char *base = documentation_topics[a->doc_topic].path;
    const char *slash = strrchr(base, '/');
    int written=path_length?snprintf(resolved,sizeof resolved,"%.*s%.*s",slash?(int)(slash-base+1):0,base,(int)path_length,target):snprintf(resolved,sizeof resolved,"%s",base);
    if(written<0 || (size_t)written>=sizeof resolved){status(a,"Dieses Linkziel ist zu lang.");return false;}
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
                    status(a,a->doc_error);return false;
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
            return a->documentation && !a->doc_error[0];
        }
    }
    snprintf(a->doc_error,sizeof a->doc_error,"Dieses Linkziel ist nicht in der mitgelieferten Dokumentation enthalten.");
    status(a,a->doc_error);
    return false;
}
static bool documentation_activate_link(app *a,size_t index) {
    ps_document *d=a->documentation;if(!d || index>=d->count || d->blocks[index].kind!=PS_DOC_LINK)return false;
    const ps_doc_block *link=&d->blocks[index];uint64_t version=a->doc_reading_version;
    size_t begin=link->reading_begin,end=begin==SIZE_MAX?SIZE_MAX:begin+link->reading_length;
    bool internal=strncmp(d->text+link->target,"http",4)!=0;
    char *target=SDL_strdup(d->text+link->target);if(!target){snprintf(a->doc_error,sizeof a->doc_error,"Link konnte nicht geöffnet werden: %s",SDL_GetError());return false;}
    bool completed=documentation_link(a,target);SDL_free(target);
    if(completed && internal && begin!=SIZE_MAX && end>begin)
        nk_sdl_accessibility_document_link_invoked(a->doc_ui,version,begin,end);
    return completed;
}
/* Stable semantic IDs are rebuilt from current content for each key event. */
enum { DOC_C=1,DOC_PHYSIM,DOC_TOPICS,DOC_CONTENTS,DOC_FILTER,DOC_GROUP,DOC_BACK,DOC_HOME,DOC_WORKSPACE,DOC_SEARCH,DOC_PREVIOUS,DOC_NEXT,DOC_BODY,DOC_FORWARD,DOC_ZOOM_OUT,DOC_ZOOM_RESET,DOC_ZOOM_IN,DOC_NAVIGATION,
       DOC_CONTEXT,DOC_MODEL_TAB,DOC_EXAMPLE_TAB,DOC_CONTEXT_COPY,DOC_ACTIONS,DOC_FIND_CLOSE,
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
static int documentation_h_group(int topic) {
    const char *path=documentation_topics[topic].path;
    if(!strcmp(path,"docs/c-guide.md") || !strcmp(path,"docs/physim-guide.md"))return 0;
    if(!strcmp(path,"docs/pendulum-tutorial.md") || !strcmp(path,"docs/spring.md") ||
       !strcmp(path,"docs/projectile-tutorial.md") || !strcmp(path,"docs/projectile-drag-tutorial.md") ||
       !strcmp(path,"docs/collision-tutorial.md") || !strcmp(path,"docs/mechanics.md") || !strcmp(path,"docs/buoyancy.md"))return 1;
    if(!strcmp(path,"docs/numerics.md") || !strcmp(path,"docs/troubleshooting.md"))return 2;
    return 3+documentation_group(topic);
}
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
        } else if(!documentation_topic_language(i,a->doc_track))continue;
        order[found++]=i;
    }
    if(!a->doc_filter[0]) {
        int grouped[DOCUMENTATION_TOPIC_COUNT];unsigned n=0;
        int active=documentation_h_group(a->doc_topic),groups[8],group_count=0;
        groups[group_count++]=0;
        if(active)groups[group_count++]=active;
        for(int group=1;group<8;group++)if(group!=active)groups[group_count++]=group;
        for(int g=0;g<group_count;g++) {
            int group=groups[g];
            /* The reference presents the pendulum before related mechanical topics. */
            if(group==1)for(unsigned i=0;i<found;i++)
                if(!strcmp(documentation_topics[order[i]].path,"docs/pendulum-tutorial.md"))grouped[n++]=order[i];
            for(unsigned i=0;i<found;i++)if(documentation_h_group(order[i])==group &&
                (group!=1 || strcmp(documentation_topics[order[i]].path,"docs/pendulum-tutorial.md")))grouped[n++]=order[i];
        }
        memcpy(order,grouped,n*sizeof *order);
    }
    return found;
}
static void documentation_finish_edits(app *a) {
    if(a->doc_filter_active)
        ps_ui_flush_edit(a->doc_ui,"documentation-header",a->doc_filter,sizeof a->doc_filter,a->doc_filter_bounds);
    if(a->doc_search_active)
        ps_ui_flush_edit(a->doc_ui,"documentation-find",a->doc_query,sizeof a->doc_query,a->doc_search_bounds);
    if(strcmp(a->doc_filter,a->doc_last_filter))documentation_filter(a);
    for(struct nk_window *window=a->doc_ui->begin;window;window=window->next) {
        window->edit.active=nk_false;window->edit.name=0;
    }
    a->doc_ui->text_edit.active=nk_false;
    a->doc_filter_active=a->doc_filter_focus=a->doc_search_active=a->doc_search_focus=false;
}
static void documentation_navigation_toggle(app *a) {
    documentation_finish_edits(a);
    nk_sdl_accessibility_document_preserve_position(a->doc_ui,a->doc_reading_version);
    a->doc_navigation_hidden=!a->doc_navigation_hidden;
    a->doc_filter_active=a->doc_filter_focus=a->doc_search_active=a->doc_search_focus=false;
    a->doc_keyboard=true;a->doc_focus=DOC_NAVIGATION;a->doc_focus_reveal=true;
    nk_sdl_accessibility_keyboard_begin(a->doc_ui);
    if(a->documentation)a->documentation->layout_width=0;
}
static void documentation_context_toggle(app *a) {
    documentation_finish_edits(a);
    nk_sdl_accessibility_document_preserve_position(a->doc_ui,a->doc_reading_version);
    a->doc_context_hidden=!a->doc_context_hidden;
    a->doc_keyboard=true;a->doc_focus=DOC_CONTEXT;a->doc_focus_reveal=true;
    nk_sdl_accessibility_keyboard_begin(a->doc_ui);
    if(a->documentation)a->documentation->layout_width=0;
}
static const char *documentation_context_code(app *a);
static unsigned documentation_action_ids(app *a,int ids[8]);
static void documentation_actions_open(app *a,float x,float y);
static void documentation_actions_close(app *a);
static unsigned documentation_focus_list(app *a,int *entries) {
    unsigned count=0;
    if(a->doc_actions_open)return documentation_action_ids(a,entries);
    entries[count++]=DOC_NAVIGATION;entries[count++]=DOC_C;entries[count++]=DOC_PHYSIM;
    entries[count++]=DOC_FILTER;entries[count++]=DOC_CONTEXT;
    if(!a->doc_navigation_hidden) {
        int order[DOCUMENTATION_TOPIC_COUNT];unsigned topics=documentation_topic_order(a,order);
        for(unsigned n=0;n<topics;n++) {
            entries[count++]=DOC_TOPIC+order[n];
            if(!a->doc_filter[0] && order[n]==a->doc_topic && a->documentation)
                for(size_t i=0;i<a->documentation->count;i++)
                    if(documentation_contents_heading(a,&a->documentation->blocks[i]) && a->documentation->blocks[i].level>1)
                        entries[count++]=DOC_HEADING+(int)i;
        }
    }
    if(a->doc_find_open){entries[count++]=DOC_SEARCH;entries[count++]=DOC_PREVIOUS;entries[count++]=DOC_NEXT;entries[count++]=DOC_FIND_CLOSE;}
    entries[count++]=DOC_BODY;
    if(a->documentation)for(size_t i=0;i<a->documentation->count;i++) {
        ps_doc_kind kind=a->documentation->blocks[i].kind;
        if(kind==PS_DOC_CODE || kind==PS_DOC_LINK)entries[count++]=(kind==PS_DOC_CODE?DOC_COPY:DOC_LINK)+(int)i;
        if(kind==PS_DOC_IMAGE && documentation_media_kind(a->documentation->text+a->documentation->blocks[i].target)==3)
            for(int part=0;part<11;part++)entries[count++]=DOC_VISUAL+(int)i*16+part;
    }
    if(!a->doc_context_hidden) {
        entries[count++]=DOC_MODEL_TAB;entries[count++]=DOC_EXAMPLE_TAB;
        if(a->doc_context_example && documentation_context_code(a))entries[count++]=DOC_CONTEXT_COPY;
    }
    entries[count++]=DOC_ACTIONS;
    return count;
}
/* Pane navigation stays independent of the number of topics/sections. Tab
 * still traverses the complete catalogue and all actions within each pane. */
static int documentation_focus_region(int focus) {
    if(focus==DOC_NAVIGATION || focus==DOC_C || focus==DOC_PHYSIM || focus==DOC_FILTER || focus==DOC_CONTEXT)return 0;
    if(focus>=DOC_TOPIC && focus<DOC_COPY)return 1;
    if(focus==DOC_MODEL_TAB || focus==DOC_EXAMPLE_TAB || focus==DOC_CONTEXT_COPY)return 3;
    return 2;
}
static void documentation_focus_pane(app *a,int direction) {
    documentation_finish_edits(a);
    int targets[4],regions[4];unsigned count=0;
    targets[count]=DOC_FILTER;regions[count++]=0;
    if(!a->doc_navigation_hidden) {
        int order[DOCUMENTATION_TOPIC_COUNT];unsigned topics=documentation_topic_order(a,order);
        if(topics) {
            int topic=order[0];for(unsigned i=0;i<topics;i++)if(order[i]==a->doc_topic){topic=a->doc_topic;break;}
            targets[count]=DOC_TOPIC+topic;regions[count++]=1;
        }
    }
    targets[count]=a->documentation?DOC_BODY:DOC_ACTIONS;regions[count++]=2;
    if(!a->doc_context_hidden) {
        targets[count]=a->doc_context_example?DOC_EXAMPLE_TAB:DOC_MODEL_TAB;regions[count++]=3;
    }
    unsigned current=count;
    if(a->doc_keyboard)for(unsigned i=0;i<count;i++)if(regions[i]==documentation_focus_region(a->doc_focus)){current=i;break;}
    unsigned next=current==count?(direction>0?0:count-1):(current+count+direction)%count;
    a->doc_keyboard=true;a->doc_focus=targets[next];a->doc_focus_reveal=true;
    a->doc_filter_focus=a->doc_focus==DOC_FILTER;a->doc_search_focus=a->doc_focus==DOC_SEARCH;
    nk_sdl_accessibility_keyboard_begin(a->doc_ui);
}
static bool documentation_keyboard_key(app *a,const SDL_KeyboardEvent *key) {
    if(key->key==SDLK_F6 && !(key->mod&(SDL_KMOD_CTRL|SDL_KMOD_GUI|SDL_KMOD_ALT))) {
        if(!key->repeat)documentation_focus_pane(a,key->mod&SDL_KMOD_SHIFT?-1:1);
        return true;
    }
    if((key->mod&PS_UI_COMMAND_MOD) && (key->mod&SDL_KMOD_SHIFT) && key->key==SDLK_R) {
        if(!key->repeat)documentation_context_toggle(a);return true;
    }
    if((key->mod&PS_UI_COMMAND_MOD) && (key->mod&SDL_KMOD_SHIFT) && key->key==SDLK_B) {
        if(!key->repeat)documentation_navigation_toggle(a);return true;
    }
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
            nk_sdl_accessibility_keyboard_begin(a->doc_ui);
            a->doc_keyboard=true;a->doc_focus=DOC_SEARCH;a->doc_focus_reveal=true;
            a->doc_find_open=true;a->doc_search_focus=true;a->doc_filter_active=false;return true;
        }
        return false;
    }
    if(key->key==SDLK_ESCAPE)return false;
    if(key->key==SDLK_TAB) {
        if(a->doc_keyboard && (a->doc_focus==DOC_SEARCH || a->doc_focus==DOC_FILTER)) {
            bool search=a->doc_focus==DOC_SEARCH;
            ps_ui_flush_edit(a->doc_ui,search?"documentation-find":"documentation-header",search?a->doc_query:a->doc_filter,sizeof a->doc_query,
                search?a->doc_search_bounds:a->doc_filter_bounds);
            if(!search && strcmp(a->doc_filter,a->doc_last_filter))documentation_filter(a);
        }
        int entries[12*PS_DOC_MAX_BLOCKS+DOCUMENTATION_TOPIC_COUNT+32];unsigned count=documentation_focus_list(a,entries),selected=count;
        for(unsigned i=0;i<count;i++)if(entries[i]==a->doc_focus){selected=i;break;}
        int direction=key->mod&SDL_KMOD_SHIFT?-1:1;
        selected=!a->doc_keyboard || selected==count?(direction>0?0:count-1):(selected+count+direction)%count;
        a->doc_keyboard=true;a->doc_focus=entries[selected];a->doc_focus_reveal=true;
        nk_sdl_accessibility_keyboard_begin(a->doc_ui);
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
    if((focus==DOC_MODEL_TAB || focus==DOC_EXAMPLE_TAB) && direction) {
        a->doc_context_example=direction>0;a->doc_focus=direction>0?DOC_EXAMPLE_TAB:DOC_MODEL_TAB;
        a->doc_focus_reveal=true;nk_sdl_accessibility_keyboard_begin(a->doc_ui);return true;
    }
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
        documentation_language(a,focus==DOC_C?1:2);
    } else if(focus==DOC_NAVIGATION)documentation_navigation_toggle(a);
    else if(focus==DOC_CONTEXT)documentation_context_toggle(a);
    else if(focus==DOC_MODEL_TAB || focus==DOC_EXAMPLE_TAB)a->doc_context_example=focus==DOC_EXAMPLE_TAB;
    else if(focus==DOC_CONTEXT_COPY){const char *code=documentation_context_code(a);if(code)SDL_SetClipboardText(code);}
    else if(focus==DOC_ACTIONS)documentation_actions_open(a,a->doc_actions_trigger_bounds.x,a->doc_actions_trigger_bounds.y);
    else if(focus==DOC_FIND_CLOSE){documentation_finish_edits(a);a->doc_find_open=false;a->doc_focus=DOC_BODY;a->doc_focus_reveal=true;}
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
            documentation_activate_link(a,i);a->doc_focus=DOC_BODY;a->doc_focus_reveal=true;
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
    if(!(ui->current->layout->flags&NK_WINDOW_ROM) && nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT) &&
       nk_input_is_mouse_hovering_rect(&ui->input,nk_window_get_content_region(ui)) &&
       nk_input_is_mouse_hovering_rect(&ui->input,bounds)){a->doc_keyboard=false;a->doc_focus=id;}
    if(!a->doc_keyboard || a->doc_focus!=id)return;
    if(id!=DOC_BODY)nk_sdl_accessibility_keyboard_target(ui,bounds);
    if(body)bounds.h=fminf(bounds.h,nk_window_get_content_region(ui).h);
    if(id==DOC_BODY){struct nk_rect clip=nk_window_get_content_region(ui);bounds.h=fmaxf(0,fminf(bounds.h,clip.y+clip.h-bounds.y));}
    bounds.x=truncf(bounds.x);bounds.y=truncf(bounds.y);bounds.w=truncf(bounds.w);bounds.h=truncf(bounds.h);
    a->doc_focus_bounds=bounds;
    nk_stroke_rect(nk_window_get_canvas(ui),bounds,3,2,UI_COLOR(accent));
    if(!a->doc_focus_reveal)return;
    struct nk_rect clip=nk_window_get_content_region(ui);float delta=0;
    /* Match nk_stroke_rect / nk_push_scissor's integer command geometry.
     * Comparing unsnapped CSS fractions can never settle an nk_uint scroll. */
    float top=truncf(bounds.y),bottom=top+truncf(bounds.h);
    float clip_top=truncf(clip.y),clip_bottom=clip_top+truncf(clip.h);
    if(top<clip_top)delta=top-clip_top;
    else if(bottom>clip_bottom)delta=bottom-clip_bottom;
    if(body) {if(delta)a->doc_scroll_adjustment=delta;}
    else if(!strcmp(ui->current->name_string,"topics")) {
        if(delta)a->doc_navigation_scroll_y=(nk_uint)fmaxf(0,(float)a->doc_navigation_scroll_y+delta);
    } else {nk_uint x,y;nk_window_get_scroll(ui,&x,&y);if(delta)nk_window_set_scroll(ui,x,(nk_uint)fmaxf(0,(float)y+delta));}
    if(!delta)a->doc_focus_reveal=false;
}
#include "documentation_media.inc"
static bool documentation_topic_button(app *a,const char *label,bool selected,struct nk_rect *actual) {
    struct nk_context *ui=a->doc_ui;struct nk_rect bounds;
    enum nk_widget_layout_states visible=nk_widget(&bounds,ui);
    /* nk_widget snaps fractional chrome metrics to integer bounds. Use that
     * same rectangle for native focus, rather than the preceding layout peek. */
    *actual=bounds;
    if(visible==NK_WIDGET_INVALID)return false;
    bool pressed=ps_ui_button_at(ui,bounds,label,selected);
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    bool hover=nk_input_is_mouse_hovering_rect(&ui->input,bounds);
    if(selected || hover)nk_fill_rect(canvas,bounds,4,selected?UI_COLOR(selection_active):UI_COLOR(button_hover));
    if(selected)nk_fill_rect(canvas,nk_rect(bounds.x,bounds.y+3,3,bounds.h-6),1,UI_COLOR(accent));
    const struct nk_user_font *font=ui->style.font;
    size_t at=0,n=strlen(label);float y=bounds.y+8;
    do {
        size_t length,used=documentation_line(font,label+at,n-at,bounds.w-20,&length);
        nk_draw_text(canvas,nk_rect(bounds.x+10,y,bounds.w-20,font->height+6),
            label+at,(int)length,font,nk_rgba(0,0,0,0),UI_COLOR(text));
        at+=used;y+=font->height+6;
    } while(at<n);
    return pressed;
}
/* Diagnostic counterprobe reads the actual queued draw payload, independently
 * of the wrapping/height calculation, before the renderer clears the buffer. */
static void documentation_sidebar_draw_probe(app *a) {
    if(!a->doc_sidebar_test_requested)return;
    a->doc_sidebar_test_requested=false;a->doc_sidebar_test_ok=false;a->doc_sidebar_test_lines=0;
    bool control=a->doc_focus==DOC_TOPICS || a->doc_focus==DOC_CONTENTS;
    const char *expected;
    if(control)expected=a->doc_focus==DOC_TOPICS?"Themen":"Inhalt";
    else {
        if(!a->documentation || a->doc_focus<DOC_HEADING || a->doc_focus>=DOC_COPY)return;
        size_t block=(size_t)(a->doc_focus-DOC_HEADING);if(block>=a->documentation->count)return;
        expected=a->documentation->text+a->documentation->blocks[block].text;
    }
    char actual[4096];size_t used=0;struct nk_rect bounds=a->doc_focus_bounds,clip={0};bool fits=true;
    const struct nk_command *command;
    nk_foreach(command,a->doc_ui) {
        if(command->type==NK_COMMAND_SCISSOR) {
            const struct nk_command_scissor *s=(const struct nk_command_scissor *)command;
            clip=nk_rect(s->x,s->y,s->w,s->h);
        } else if(command->type==NK_COMMAND_TEXT) {
            const struct nk_command_text *t=(const struct nk_command_text *)command;
            if(t->x<bounds.x || t->x>=bounds.x+bounds.w || t->y<bounds.y || t->y>=bounds.y+bounds.h)continue;
            float width=t->font->width(t->font->userdata,t->height,t->string,t->length);
            fits &= t->x>=clip.x && t->y>=clip.y && t->x+width<=clip.x+clip.w+1 &&
                t->y+t->height<=clip.y+clip.h+1 && t->x+width<=bounds.x+bounds.w+1 && t->y+t->height<=bounds.y+bounds.h+1;
            for(int i=0;i<t->length;i++)if(t->string[i]!=' ' && t->string[i]!='\n') {
                if(used+1<sizeof actual)actual[used++]=t->string[i];else fits=false;
            }
            a->doc_sidebar_test_lines++;
        }
    }
    actual[used]=0;size_t at=0;
    for(size_t i=0;expected[i];i++)if(expected[i]!=' ' && expected[i]!='\n') {
        if(at>=used || expected[i]!=actual[at++])fits=false;
    }
    a->doc_sidebar_test_ok=fits && at==used && (control?a->doc_sidebar_test_lines==1:a->doc_sidebar_test_lines>=2);
    if(control)printf("DOCUMENTATION SIDEBAR TABS: font=%u label=%s theme=%d lines=%u complete=%d width=%.1f height=%.1f\n",
        documentation_font_size(a),expected,a->preferences.theme,a->doc_sidebar_test_lines,a->doc_sidebar_test_ok,bounds.w,bounds.h);
    else printf("DOCUMENTATION SIDEBAR DRAW: font=%u lines=%u complete=%d width=%.1f height=%.1f\n",
        documentation_font_size(a),a->doc_sidebar_test_lines,a->doc_sidebar_test_ok,bounds.w,bounds.h);
}
static void documentation_detail_draw_probe(app *a) {
    if(!a->doc_detail_test_requested)return;
    a->doc_detail_test_requested=false;a->doc_detail_test_ok=false;
    struct nk_rect bounds=a->doc_detail_test_bounds,clip={0};const struct nk_command *command;
    char actual[384],unit[24];size_t used=0;bool fits=true,unit_seen=false;unsigned lines=0;
    snprintf(unit,sizeof unit,"%u\xC2\xA0px",documentation_font_size(a));
    bool unit_needed=strstr(a->doc_detail_test_text,"\xC2\xA0px")!=NULL;
    nk_foreach(command,a->doc_ui) {
        if(command->type==NK_COMMAND_SCISSOR) {
            const struct nk_command_scissor *s=(const struct nk_command_scissor *)command;clip=nk_rect(s->x,s->y,s->w,s->h);
        } else if(command->type==NK_COMMAND_TEXT) {
            const struct nk_command_text *t=(const struct nk_command_text *)command;
            if(t->x<bounds.x || t->x>=bounds.x+bounds.w || t->y<bounds.y || t->y>=bounds.y+bounds.h)continue;
            fits &= t->x>=clip.x-1 && t->y>=clip.y-1 && t->y+t->height<=fminf(clip.y+clip.h,bounds.y+bounds.h)+1;
            size_t at=0;float x=t->x;
            while(at<(size_t)t->length) {
                nk_rune cp=0,next=0;int bytes=nk_utf_decode(t->string+at,&cp,t->length-(int)at);
                if(bytes<=0){fits=false;break;}
                if(at+(size_t)bytes<(size_t)t->length)nk_utf_decode(t->string+at+bytes,&next,t->length-(int)at-bytes);
                struct nk_user_font_glyph glyph={0};t->font->query(t->font->userdata,t->height,&glyph,cp,next);
                if(glyph.width>0 && glyph.height>0) {
                    float left=x+glyph.offset.x,top=t->y+glyph.offset.y;
                    fits &= left>=fmaxf(bounds.x,clip.x)-1 && top>=fmaxf(bounds.y,clip.y)-1 &&
                        left+glyph.width<=fminf(bounds.x+bounds.w,clip.x+clip.w)+1 &&
                        top+glyph.height<=fminf(bounds.y+bounds.h,clip.y+clip.h)+1;
                }
                x+=glyph.xadvance;at+=(size_t)bytes;
            }
            for(int i=0;i<t->length;i++)if(!isspace((unsigned char)t->string[i])) {
                if(used<sizeof actual)actual[used++]=t->string[i];else fits=false;
            }
            if(strstr(t->string,unit))unit_seen=true;
            ++lines;
        }
    }
    size_t at=0;for(const char *p=a->doc_detail_test_text;*p;p++)if(!isspace((unsigned char)*p)) {
        if(at>=used || actual[at]!=*p)fits=false;
        ++at;
    }
    a->doc_detail_test_ok=fits && at==used && lines>0 && (!unit_needed || unit_seen);
    printf("DOCUMENTATION DETAIL DRAW: case=%u font=%u theme=%u lines=%u complete=%d frame=%.0f,%.0f,%.0f,%.0f\n",
        a->doc_detail_test_case,documentation_font_size(a),a->preferences.theme,lines,a->doc_detail_test_ok,bounds.x,bounds.y,bounds.w,bounds.h);
    if(!a->doc_detail_test_ok)printf("DOCUMENTATION DETAIL REJECT: expected=%s bytes=%zu/%zu fits=%d\n",a->doc_detail_test_text,used,at,fits);
}
#include "documentation_h.inc"
/* Detail text uses the same bounded UTF-8 line breaks for measurement and drawing. */
static struct nk_rect documentation_detail_label(app *a,const char *text) {
    struct nk_context *ui=a->doc_ui;const struct nk_user_font *font=documentation_type_font(a,DOC_TYPE_CRUMB);
    /* Configure once: a second layout call would insert an unused row. */
    ps_ui_row_exact_dynamic(ui,1,1);
    struct nk_rect bounds=nk_widget_bounds(ui);
    float width=fmaxf(1,bounds.w-32),line=.7f*documentation_font_size(a)*1.65f;
    size_t measured=0;unsigned lines=0;
    do {size_t count,used=documentation_line(font,text+measured,strlen(text)-measured,width,&count);measured+=used;++lines;}while(measured<strlen(text));
    float height=lines*line;
    ui->current->layout->row.height=height+ui->style.window.spacing.y;
    enum nk_widget_layout_states visible=nk_widget(&bounds,ui);
    bounds.x+=16;bounds.w=width;
    ps_ui_accessibility_label(ui,bounds,text);
    if(visible==NK_WIDGET_INVALID)return bounds;
    size_t at=0,length=strlen(text);float y=bounds.y+(line-font->height)/2;
    do {
        size_t count,used=documentation_line(font,text+at,length-at,width,&count);
        nk_draw_text(nk_window_get_canvas(ui),nk_rect(bounds.x,y,width+1,font->height),text+at,(int)count,
            font,nk_rgba(0,0,0,0),a->doc_error[0]?UI_COLOR(error):UI_COLOR(muted));
        at+=used;y+=line;
    } while(at<length);
    return bounds;
}
#include "documentation_actions.inc"
static void documentation_code_header(app *a,const char *text,size_t block,float width,float gap) {
    struct nk_context *ui=a->doc_ui;const struct nk_user_font *font=documentation_type_font(a,DOC_TYPE_CODE_HEAD);
    float control=fmaxf(30,.75f*documentation_font_size(a)*1.2f+12),height=control+22;
    ps_ui_row_exact_dynamic(ui,height+gap,1);struct nk_rect outer;
    enum nk_widget_layout_states visible=nk_widget(&outer,ui);
    struct nk_rect frame=nk_rect(outer.x+12,outer.y+gap,width-24,height);
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    nk_fill_rect(canvas,frame,0,UI_COLOR(edit));
    nk_fill_rect(canvas,nk_rect(frame.x,frame.y,frame.w,1),0,UI_COLOR(divider));
    nk_fill_rect(canvas,nk_rect(frame.x,frame.y,1,frame.h),0,UI_COLOR(divider));
    nk_fill_rect(canvas,nk_rect(frame.x+frame.w-1,frame.y,1,frame.h),0,UI_COLOR(divider));
    float copy=fminf(frame.w-22,documentation_text_width(font,"Code kopieren",13)+18);
    struct nk_rect action=nk_rect(frame.x+frame.w-11-copy,frame.y+11,copy,control);
    if(visible==NK_WIDGET_INVALID){documentation_focus_draw(a,DOC_COPY+(int)block,action,true);return;}
    ps_ui_accessibility_label(ui,nk_rect(frame.x+11,frame.y+11,fmaxf(0,frame.w-copy-30),control),"Code");
    nk_draw_text(canvas,nk_rect(frame.x+11,frame.y+(height-font->height)/2,fmaxf(0,frame.w-copy-30),font->height),
        "Code",4,font,nk_rgba(0,0,0,0),UI_COLOR(code_text));
    nk_style_push_font(ui,font);
    bool enabled=!(ui->current->layout->flags&NK_WINDOW_ROM) && !ui->current->widgets_disabled;
    if(ps_ui_accessibility_button(ui,action,"Code kopieren") || (enabled &&
       nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT) &&
       nk_input_is_mouse_hovering_rect(&ui->input,nk_window_get_content_region(ui)) &&
       nk_input_is_mouse_hovering_rect(&ui->input,action)))SDL_SetClipboardText(text);
    nk_fill_rect(canvas,action,5,nk_input_is_mouse_hovering_rect(&ui->input,action)?UI_COLOR(button_hover):UI_COLOR(button));
    float label=documentation_text_width(font,"Code kopieren",13);
    nk_draw_text(canvas,nk_rect(action.x+(action.w-label)/2,action.y+(action.h-font->height)/2,label+1,font->height),
        "Code kopieren",13,font,nk_rgba(0,0,0,0),UI_COLOR(code_text));
    nk_style_pop_font(ui);documentation_focus_draw(a,DOC_COPY+(int)block,action,true);
}
static bool documentation_article_probe(app *a,const char *capture) {
    if(!capture || !a->documentation)return true;
    const char *name=capture,*slash=strrchr(capture,'/'),*backslash=strrchr(capture,'\\');
    if(slash)name=slash+1;if(backslash && backslash+1>name)name=backslash+1;
    unsigned index=0;int consumed=0;
    if(sscanf(name,"docs-h-%u.bmp%n",&index,&consumed)!=1 || name[consumed])return true;
    const ps_doc_block *heading=NULL;
    for(size_t i=0;i<a->documentation->count;i++)if(a->documentation->blocks[i].kind==PS_DOC_HEADING && a->documentation->blocks[i].level==1){heading=&a->documentation->blocks[i];break;}
    if(!heading)return false;
    const struct nk_user_font *font=documentation_block_font(a,heading);
    struct nk_user_font_glyph actual_glyph={0},base_glyph={0};
    font->query(font->userdata,font->height,&actual_glyph,'A','B');
    const documentation_spaced_font *spaced=font->userdata.ptr;
    spaced->base->query(spaced->base->userdata,font->height,&base_glyph,'A','B');
    float tracking=actual_glyph.xadvance-base_glyph.xadvance,step=0,first=-1,title_x=-1,crumb_x=-1;
    char actual[4096];size_t used=0;const struct nk_command *command;
    nk_foreach(command,a->doc_ui)if(command->type==NK_COMMAND_TEXT) {
        const struct nk_command_text *text=(const struct nk_command_text *)command;
        if(text->font==documentation_type_font(a,DOC_TYPE_CRUMB) && crumb_x<0)crumb_x=text->x;
        if(text->font!=font || text->y<heading->hit[1]-font->height || text->y>=heading->hit[1]+heading->hit[3])continue;
        if(title_x<0)title_x=text->x;
        if(first<0)first=text->y;else if(!step && text->y>first)step=text->y-first;
        for(int i=0;i<text->length;i++)if(!isspace((unsigned char)text->string[i]) && used<sizeof actual)actual[used++]=text->string[i];
    }
    size_t expected=0;bool bytes=true;const char *source=a->documentation->text+heading->text;
    for(size_t i=0;i<heading->length;i++)if(!isspace((unsigned char)source[i])) {
        if(expected>=used || actual[expected]!=source[i])bytes=false;++expected;
    }
    bytes=bytes && expected==used;
    ps_doc_block body={0};body.kind=PS_DOC_TEXT;body.text=SIZE_MAX;
    float line=documentation_block_line_height(a,heading),gap=-1;
    for(size_t i=0;i<a->documentation->count;i++)if(documentation_is_lead(a,&a->documentation->blocks[i])) {
        gap=a->documentation->blocks[i].hit[1]-heading->hit[1]-heading->hit[3];break;
    }
    printf("DOCUMENTATION H FLOW: case=%u title-line=%.3f title-step=%.3f body-line=%.3f lead-gap=%.3f inset-delta=%.3f lead-em=%.3f table-em=%.3f tracking=%.3f title-bytes=%d\n",
        index,line,step,documentation_block_line_height(a,&body),gap,crumb_x-title_x,
        documentation_type_font(a,DOC_TYPE_LEAD)->height/a->doc_type_em[DOC_TYPE_LEAD],
        documentation_type_font(a,DOC_TYPE_TABLE)->height/a->doc_type_em[DOC_TYPE_TABLE],tracking,bytes);
    return bytes && fabsf(tracking+.55f)<.001f && (!step || fabsf(step-line)<=1.01f) &&
        fabsf(gap-20)<=1.01f && crumb_x>=0 && title_x>=0 && fabsf(crumb_x-title_x)<=1.01f;
}
static void documentation_view(app *a, float height) {
    struct nk_context *ui = a->doc_ui;
    a->doc_inline_clicked=-1;
    size_t reading_byte=SIZE_MAX;
    bool reading_top=true;
    float section_scroll=-1;
    ps_document *section_document=NULL;
    a->doc_scroll_pending=false;
    /* CSS article padding and the breadcrumb paragraph's top margin. The
     * body group supplies the existing horizontal inset independently. */
    int window_width,window_height;SDL_GetWindowSize(a->doc_window,&window_width,&window_height);(void)window_height;
    ps_ui_row_exact_dynamic(ui,window_width<=900?35:37,1);nk_spacer(ui);
    if(strcmp(a->doc_last_query,a->doc_query)) {
        snprintf(a->doc_last_query,sizeof a->doc_last_query,"%s",a->doc_query);a->doc_match=0;a->doc_move_match=true;
    }
    ps_document *d = a->documentation;
    if(d && nk_sdl_accessibility_document_scroll(ui,a->doc_reading_version,&reading_byte,&reading_top)) {
        /* A queued viewport-preservation request can arrive after a resize.
         * A newer, explicit section activation must keep its target. */
        if(a->doc_jump_block>=0)reading_byte=SIZE_MAX;
        else for(size_t i=0;i<d->count;i++)if(d->blocks[i].inline_parent==SIZE_MAX && d->blocks[i].reading_begin!=SIZE_MAX && d->blocks[i].reading_begin<=reading_byte)
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
    else {
        const char *groups[]={"Lernweg","Mechanik","Nachschlagen","App & Lernen","Fachthemen","Referenz","Projekttechnik","Quellen"};
        snprintf(detail,sizeof detail,"%s / %s / %s",a->doc_track==2?"Teil II · Physim":"Teil I · C",
            groups[documentation_h_group(a->doc_topic)],d?documentation_topics[a->doc_topic].title:"Dokument nicht verfügbar");
    }
    struct nk_rect detail_bounds=documentation_detail_label(a,detail);
    if(a->doc_detail_test_requested) {
        a->doc_detail_test_bounds=detail_bounds;
        snprintf(a->doc_detail_test_text,sizeof a->doc_detail_test_text,"%s",detail);
    }
    /* Peek the next row after the consumed detail row. An unused placeholder
     * row would be advanced by the following layout call and shift the group
     * below this measurement, making its scrollbar taller than its real clip. */
    ps_ui_row_exact_dynamic(ui,21,1);nk_spacer(ui);
    struct nk_rect remaining=nk_widget_bounds(ui),clip=nk_window_get_content_region(ui);
    ps_ui_row_exact_dynamic(ui,fmaxf(20,fminf(height,clip.y+clip.h-remaining.y)),1);
    struct nk_rect body_bounds=nk_widget_bounds(ui);
    body_bounds.x=truncf(body_bounds.x);body_bounds.y=truncf(body_bounds.y);
    body_bounds.w=truncf(body_bounds.w);body_bounds.h=truncf(body_bounds.h);
    a->doc_body_bounds=body_bounds;
    documentation_focus_draw(a,DOC_BODY,body_bounds,false);
    nk_style_push_vec2(ui,&ui->style.window.group_padding,nk_vec2(4,0));
    if (nk_group_scrolled_offset_begin(ui, &a->doc_scroll_x, &a->doc_scroll_y, "document-body",
                                       0)) {
        if (d) {
            nk_sdl_accessibility_document(ui,a->doc_reading_version,documentation_topics[a->doc_topic].title,d->reading,nk_window_get_content_region(ui));
            if(nk_sdl_accessibility_document_focus(ui,a->doc_reading_version,a->doc_keyboard && a->doc_focus==DOC_BODY)) {
                a->doc_keyboard=true;a->doc_focus=DOC_BODY;a->doc_focus_reveal=true;
                a->doc_search_active=a->doc_filter_active=false;
                a->doc_search_focus=a->doc_filter_focus=false;
            }
            for(size_t i=0;i<d->count;i++)d->blocks[i].hit[2]=0;
            float width = fmaxf(40,ui->current->layout->bounds.w);
            /* A dynamic single-column widget uses the panel bounds, while
             * content_region is its clip. Select the same formula variant for
             * measurement and drawing, including near a wrapping threshold. */
            float math_width=ui->current->layout->bounds.w;
            if (d->layout_width != width) {
                float previous=0;
                for (size_t i = 0; i < d->count; i++) {
                    ps_doc_block *b = &d->blocks[i];
                    b->code_line_height=documentation_grid_pitch(a,d,b);
                    float gap=documentation_block_gap(a,b,&previous);
                    b->height =
                        b->kind == PS_DOC_TABLE_ROW
                            ? documentation_table(a, b, width-24, NULL)
                        : b->kind == PS_DOC_MATH && documentation_math_variant(a,d,b,math_width-24)
                            ? documentation_math_variant(a,d,b,math_width-24)->height*((documentation_font_size(a)/16.f))+24
                        : b->kind == PS_DOC_IMAGE && documentation_media_kind(d->text+b->target)==3 ? 900
                        : b->kind == PS_DOC_IMAGE && documentation_media_kind(d->text+b->target) ? (documentation_media_kind(d->text+b->target)==1?320:480)
                        : b->kind == PS_DOC_RULE ? 1
                        : b->kind == PS_DOC_LINK && b->inline_parent!=SIZE_MAX ? 0
                        : b->kind == PS_DOC_LINK
                            ? 32
                            : documentation_rich_height(a,d,b,width-(b->kind==PS_DOC_CODE?54:24))+
                                  (b->kind==PS_DOC_CODE?36:0);
                    if(!(b->kind==PS_DOC_LINK && b->inline_parent!=SIZE_MAX))b->height+=gap;
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
            float previous=0;
            for (size_t i = 0; i < d->count; i++) {
                ps_doc_block *b = &d->blocks[i];
                const char *text = d->text + b->text;
                const struct nk_user_font *font = documentation_block_font(a,b);
                float gap=documentation_block_gap(a,b,&previous);
                if(b->kind==PS_DOC_LINK && b->inline_parent!=SIZE_MAX) {
                    struct nk_rect link_bounds=nk_rect(b->hit[0],b->hit[1],b->hit[2],b->hit[3]);
                    if(link_bounds.w>0) {
                        bool external=!strncmp(d->text+b->target,"http",4);char label[1024];
                        snprintf(label,sizeof label,"%s%s",external?"Quellenadresse kopieren: ":"Öffnen: ",text);
                        if(external && ps_ui_accessibility_button(ui,link_bounds,label))a->doc_inline_clicked=(int)i;
                        documentation_focus_draw(a,DOC_LINK+(int)i,link_bounds,true);
                        if((int)i==target)section_scroll=fmaxf(0,link_bounds.y+(float)submitted_scroll-body_origin);
                    }
                    continue;
                }
                float row_height=b->height;
                if (b->kind == PS_DOC_CODE) {
                    documentation_code_header(a,text,i,width,gap);row_height-=gap;gap=0;
                }
                if(b->kind==PS_DOC_IMAGE && documentation_media_kind(d->text+b->target)==3) {
                    if(gap>0){ps_ui_row_exact_dynamic(ui,gap,1);nk_spacer(ui);}
                    documentation_pendulum(a,(int)i,width,b->height-gap);
                    if((int)i==a->doc_anchor_heading)a->doc_anchor_bounds=a->doc_visual_bounds[0];
                    if((int)i==target)section_scroll=fmaxf(0,a->doc_visual_bounds[0].y+(float)submitted_scroll-body_origin);
                    continue;
                }
                ps_ui_row_exact_dynamic(ui,row_height,1);
                struct nk_rect block_bounds=nk_widget_bounds(ui);
                block_bounds.y+=gap;block_bounds.h-=gap;
                b->hit[0]=block_bounds.x;b->hit[1]=block_bounds.y;b->hit[2]=block_bounds.w;b->hit[3]=block_bounds.h;
                if(b->kind==PS_DOC_TABLE_ROW || b->kind==PS_DOC_CODE){b->hit[0]+=12;b->hit[2]=width-24;}
                if((int)i==a->doc_anchor_heading)a->doc_anchor_bounds=block_bounds;
                if((int)i==target) {
                    float prefix=0;
                    size_t reading_offset=reading_byte!=SIZE_MAX && b->reading_begin!=SIZE_MAX && reading_byte>=b->reading_begin?
                        reading_byte-b->reading_begin:0;
                    if(reading_offset>b->length)reading_offset=b->length;
                    const char *match=reading_byte!=SIZE_MAX?text+reading_offset:ps_document_find(text,a->doc_query);
                    if((!explicit_jump || reading_byte!=SIZE_MAX) && b->kind!=PS_DOC_TABLE_ROW && b->kind!=PS_DOC_MATH && match && match>text) {
                        prefix=documentation_rich_offset(a,d,b,(size_t)(match-text),width-(b->kind==PS_DOC_CODE?54:24));
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
                        documentation_activate_link(a,i);
                        break;
                    }
                    documentation_focus_draw(a,DOC_LINK+(int)i,link_bounds,true);
                } else {
                    struct nk_rect rect;
                    enum nk_widget_layout_states state = nk_widget(&rect, ui);
                    rect.y+=gap;rect.h-=gap;
                    bool link_geometry=(target>=0 && d->blocks[target].inline_parent==i) ||
                        (a->doc_keyboard && a->doc_focus>=DOC_LINK && a->doc_focus<DOC_LINK+(int)d->count &&
                         d->blocks[a->doc_focus-DOC_LINK].inline_parent==i);
                    if (state != NK_WIDGET_INVALID || link_geometry) {
                        struct nk_command_buffer *canvas = nk_window_get_canvas(ui);
                        if(b->kind==PS_DOC_MATH && documentation_math_draw(a,d,b,rect)) {}
                        else if(b->kind==PS_DOC_IMAGE && documentation_media_kind(d->text+b->target))
                            documentation_diagram(a,documentation_media_kind(d->text+b->target),rect);
                        else if (b->kind == PS_DOC_TABLE_ROW) {
                            struct nk_rect table=nk_rect(rect.x+12,rect.y,width-24,rect.h);
                            memcpy(b->hit,&table,sizeof b->hit);documentation_table(a,b,width-24,&table);
                        }
                        else if (b->kind == PS_DOC_RULE)
                            nk_fill_rect(canvas,nk_rect(rect.x+12,rect.y,width-24,1),0,UI_COLOR(divider));
                        else {
                            if (b->kind == PS_DOC_CODE) {
                                struct nk_rect pre=nk_rect(rect.x+12,rect.y,width-24,rect.h);
                                memcpy(b->hit,&pre,sizeof b->hit);nk_fill_rect(canvas,pre,0,UI_COLOR(edit));
                                nk_fill_rect(canvas,nk_rect(pre.x,pre.y,pre.w,1),0,UI_COLOR(divider));
                                nk_fill_rect(canvas,nk_rect(pre.x,pre.y+pre.h-1,pre.w,1),0,UI_COLOR(divider));
                                nk_fill_rect(canvas,nk_rect(pre.x,pre.y,1,pre.h),0,UI_COLOR(divider));
                                nk_fill_rect(canvas,nk_rect(pre.x+pre.w-1,pre.y,1,pre.h),0,UI_COLOR(divider));
                            }
                            documentation_rich_draw(a,d,b,b->kind==PS_DOC_CODE?
                                nk_rect(rect.x+27,rect.y+18,width-54,rect.h-36):nk_rect(rect.x+12,rect.y,width-24,rect.h));
                        }
                    }
                }
            }
            ps_ui_row_exact_dynamic(ui,previous+(window_width<=900?36:40),1);nk_spacer(ui);
            /* Native links retain their document identity outside the viewport.
             * Pointer actions still use the renderer's individual fragments. */
            if(a->documentation==d && !documentation_publish_tables(a,d))
                nk_sdl_accessibility_document_abort(ui);
            if(a->documentation==d)for(size_t i=0;i<d->count;i++) {
                ps_doc_block *b=&d->blocks[i];
                if(b->kind!=PS_DOC_LINK || !strncmp(d->text+b->target,"http",4) || b->reading_begin==SIZE_MAX || !b->reading_length)continue;
                if(nk_sdl_accessibility_document_link_target(ui,d->text+b->text,d->text+b->target,a->doc_reading_version,
                    b->reading_begin,b->reading_begin+b->reading_length,nk_rect(b->hit[0],b->hit[1],b->hit[2],b->hit[3])))a->doc_inline_clicked=(int)i;
                if(a->doc_keyboard && a->doc_focus==DOC_LINK+(int)i)
                    nk_sdl_accessibility_keyboard_target(ui,nk_rect(b->hit[0],b->hit[1],b->hit[2],b->hit[3]));
            }
        }
        nk_group_scrolled_end(ui);
    }
    nk_style_pop_vec2(ui);
    if(a->doc_inline_clicked>=0 && a->documentation==d) {
        documentation_activate_link(a,(size_t)a->doc_inline_clicked);
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
