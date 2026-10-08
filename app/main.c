#include "physim/math.h"
#include "autosave.h"
#include "batch.h"
#include "documentation.h"
#include "library.h"
#include "run_import.h"
#include "physim/analysis.h"
#include "physim/report.h"
#include "platform.h"
#include "profiling.h"
#include "plot_view.h"
#include "parameter_catalog.h"
#include "project_file.h"
#include "report_image.h"
#include "scene_view.h"
#include "preferences.h"
#include "layout_catalog.h"
#include "channel_units.h"
#include "workspace_state.h"
#include "workspace_catalog.h"
#include "workspace_tree.h"
#include "text_document.h"
#include "protocol.h"
#include "pacing.h"
#include "timeline.h"
#include "ui.h"
#include "design_tokens.h"
static const ps_ui_palette *ui_palette = &PS_UI_DARK;
#define UI_COLOR(field) (ui_palette->field)
#include "language/lexer.h"
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifndef _WIN32
#include <sys/stat.h>
#include <unistd.h>
#endif
/* Preserve the surrounding disabled state for nested control groups. */
static bool disabled_stack[16];
static int disabled_depth;
static void nk_begin_disabled(struct nk_context *ctx, bool disabled) {
    disabled_stack[disabled_depth++] = ctx->current->widgets_disabled;
    if (disabled)
        nk_widget_disable_begin(ctx);
}
static void nk_end_disabled(struct nk_context *ctx) {
    bool previous = disabled_stack[--disabled_depth];
    if (previous)
        nk_widget_disable_begin(ctx);
    else
        nk_widget_disable_end(ctx);
}
#ifdef _WIN32
#define MODULE_EXT ".dll"
#define EXE_EXT ".exe"
#else
#define MODULE_EXT ".so"
#define EXE_EXT ""
#endif
#define PREVIEW 2048
#define WORKSPACE_DOCUMENTS PS_WORKSPACE_DOCUMENTS
typedef struct {
    ps_text_document file;
    char path[4096]; /* User-facing path; file.path resolves symbolic links for saving. */
    struct nk_text_edit edit;
    bool dirty;
    char error[192];
    ps_autosave *recovery;
    bool recovery_conflict, draft_blocked, draft_present;
    char *draft_text;
    size_t draft_length;
    double draft_due;
    char draft_error[192];
} workspace_document;
typedef struct {
    char path[4096];
    SDL_AtomicInt done;
    SDL_Thread *thread;
    ps_result result;
    ps_channel channels[PS_MAX_CHANNELS];
    uint32_t channel_count;
    uint64_t total;
    double time[PREVIEW], values[PS_MAX_CHANNELS][PREVIEW];
    int count;
    ps_statistics stats[PS_MAX_CHANNELS];
    char metadata[8192];
    ps_timeline timeline;
    ps_result timeline_result;
    bool recorded_scenes;
} dataset;
typedef struct {
    int line, column;
    bool analysis;
    char text[2048],source[PS_DIAGNOSTIC_SOURCE_MAX+1u];
    ps_result code;
    bool foreign,structured;
} diagnostic;
typedef struct {
    struct nk_context *ui;
    const struct nk_user_font *font_ui, *font_code, *font_title;
    const struct nk_user_font *ui_fonts[4], *title_fonts[4];
    const struct nk_user_font *code_fonts[4];
    ps_preferences preferences, settings_draft;
    char preferences_path[4096], preferences_error[192];
    bool preferences_writable, show_grid;
    int settings_previous_tab, panel_drag;
    int settings_focus;
    struct nk_rect settings_focus_bounds[14];
    bool settings_keyboard, settings_focus_scroll;
    struct nk_rect dock_rects[PS_DOCK_NODES], dock_panels[PS_DOCK_PANELS];
    struct nk_rect dock_headers[PS_DOCK_PANELS], dock_closes[PS_DOCK_PANELS], dock_splitters[PS_DOCK_NODES], dock_float_grips[PS_DOCK_PANELS];
    struct nk_rect dock_targets[5], dock_preview;
    bool dock_visible[PS_DOCK_PANELS], dock_dragging, dock_started;
    unsigned dock_front;
    int dock_resize, dock_float_resize, dock_drag, dock_last_tab;
    struct nk_vec2 dock_origin, dock_grab, dock_resize_mouse;
    ps_dock_float dock_resize_start;
    struct nk_rect dock_menu_bounds[5];
    ps_layout_catalog layouts;
    char layouts_path[4096], layouts_error[192], layout_name[PS_LAYOUT_NAME_BYTES];
    bool layouts_writable, layout_manager;
    int layout_selected;
    struct nk_rect layout_bounds[6], layout_entries[PS_LAYOUT_MAX];
    ps_channel_units channel_units;
    char channel_units_path[4096], channel_units_error[192], channel_unit_scale[64];
    bool channel_units_writable, channel_unit_manager;
    ps_channel unit_channel, live_channels[PS_MAX_CHANNELS], reset_channels[PS_MAX_CHANNELS];
    ps_display_unit unit_draft;
    struct nk_rect channel_unit_buttons[2], channel_unit_bounds[7];
    uint32_t dock_target, dock_side;
    struct nk_rect settings_bounds[6], theme_bounds[PS_THEME_COUNT], panel_bounds[2];
    struct nk_rect ui_size_bounds[4];
    SDL_Window *window;
    ps_graphics *graphics;
    ps_vec3 camera_target;
    struct nk_text_edit experiment, analysis;
    char root[4096], bin[4096], project[4096], project_input[4096], last_run[4096], report[4096];
    char build_directory[4096];
    char workspace[4096], workspace_preview_path[4096];
    workspace_document documents[WORKSPACE_DOCUMENTS];
    unsigned document_count, document_active;
    char document_drafts_directory[4096];
    uint64_t source_revision, build_revision;
    int document_pending; /* 1: close, 2: reload; discard requires an explicit choice. */
    struct nk_rect document_bounds[10], document_list_bounds[WORKSPACE_DOCUMENTS];
    char manager_parent[4096], manager_name[256],manager_error[256];
    bool manager_error_reveal;
    int manager_template, manager_experiment_language;
    int manager_focus;
    bool manager_keyboard,manager_focus_known,manager_focus_reveal;
    struct nk_rect manager_focus_bounds[9];
    char workspace_additions[32][4096];
    unsigned workspace_addition_count;
    ps_workspace_tree workspace_tree;
    char workspace_tree_error[192];
    char workspace_tree_test_path[4096];
    struct nk_rect workspace_tree_test_bounds;
    bool workspace_open, project_manager, dialog_pending;
    ps_workspace_state last_workspace;
    char workspace_state_path[4096], workspace_state_error[192];
    bool workspace_state_writable;
    struct nk_rect workspace_restore_bounds;
    ps_workspace_catalog *workspaces;
    char workspaces_path[4096], workspaces_error[192], workspace_name[PS_WORKSPACE_NAME_BYTES];
    bool workspaces_writable, workspace_manager;
    int workspace_selected;
    struct nk_rect workspace_catalog_bounds[6], workspace_catalog_entries[PS_WORKSPACE_MAX], workspace_menu_bounds;
    enum nk_collapse_states workspace_disclosure;
    char log[65536], status[256], find[128], replace[128];
    int tab, analysis_tab, example, profile, plot_channel;
    bool loaded, built, dirty, analysis_dirty, paused, hello, show_vectors, orthographic, quitting;
    bool analysis_only, manager_analysis_only;
    bool language_experiment;
    bool language_analysis;
    int template_analysis_language;
    bool show_paths, show_points, show_labels;
    bool show_log, show_search;
    char *saved_source[2];
    ps_autosave *recovery;
    bool recovery_conflict, autosave_blocked, autosave_current;
    double autosave_due;
    uint32_t autosave_hash[2], autosave_length[2];
    char autosave_error[192];
    struct nk_rect recovery_bounds[2];
    struct nk_rect navigation_bounds[3];
    struct nk_rect toolbar_bounds[4], toolbar_item_bounds[6];
    int toolbar_menu; /* 0: closed, 1: File, 2: View. One popup supports direct switching. */
    bool toolbar_keyboard;
    int toolbar_keyboard_top, toolbar_keyboard_item;
    int toolbar_popup_menu; /* Previous drawn popup, including keyboard close. */
    struct nk_rect toolbar_popup_bounds;
    struct nk_rect window_control_bounds[3], window_drag_bounds;
    enum nk_collapse_states view_disclosure;
    ps_document *documentation;
    SDL_Window *doc_window;
    ps_graphics *doc_graphics;
    struct nk_context *doc_ui;
    const struct nk_user_font *doc_font_ui, *doc_font_title, *doc_font_code, *doc_code_fonts[4];
    const struct nk_user_font *doc_ui_fonts[4], *doc_title_fonts[4];
    bool doc_visible;
    int doc_topic, doc_match, doc_jump_block;
    nk_uint doc_scroll_x, doc_scroll_y;
    char doc_query[128], doc_last_query[128], doc_error[160];
    bool doc_search_focus, doc_search_active, doc_move_match, doc_reset_sidebar;
    bool doc_keyboard,doc_focus_reveal,doc_filter_focus;
    int doc_focus;
    struct nk_rect doc_focus_bounds;
    char doc_filter[128], doc_last_filter[128];
    bool doc_filter_active, doc_contents;
    int doc_group, doc_previous, doc_track;
    struct nk_rect doc_track_bounds[2],doc_home_bounds;
    struct nk_rect documentation_bounds;
    ps_report *analysis_report, *pending_report;
    SDL_Thread *report_thread;
    SDL_AtomicInt report_done;
    ps_result report_result;
    int result_view;
    bool show_report;
    char result_path[4096], loaded_report_path[4096], result_error[192], result_export[4096];
    struct nk_rect result_export_bounds[3];
    int png_scale;
    struct nk_rect png_region_bounds;
    struct nk_rect svg_region_bounds;
    struct nk_rect png_size_bounds, png_size_choices[4];
    ps_plot_view report_views[PS_REPORT_MAX_PLOTS], data_views[PS_MAX_CHANNELS];
    ps_plot_view *plot_drag, *active_plot_view;
    struct nk_vec2 plot_drag_position;
    struct nk_rect plot_area, plot_buttons[3];
    char plot_data_path[4096];
    ps_library *library, *pending_library;
    SDL_Thread *library_thread;
    SDL_Thread *import_thread;SDL_AtomicInt import_done;ps_result import_result;
    char import_source[4096],import_destination[4096];
    struct nk_rect import_bounds, project_kind_bounds, project_kind_choices[2], project_create_bounds;
    char import_test_path[4096];
    SDL_AtomicInt library_done;
    ps_result library_result;
    char library_directory[4096], library_error[192], library_query[128];
    char selected_runs[PS_ANALYSIS_MAX_INPUTS][4096];
    unsigned selected_count;
    int library_kind;
    bool library_again, library_search_focus, library_search_active;
    struct nk_rect library_open_bounds[8], library_select_bounds[8], library_analyze_bounds, analysis_start_bounds;
    size_t library_visible_indices[8];
    unsigned library_visible_count;
    ps_process job, runner;
    ps_parameter_catalog parameters;
    ps_app_profile *profiler;
    bool profile_job_recorded, profile_runner_recorded;
    double profile_started_at;
    ps_app_profile_frame profile_frame;
    bool project_settings_dirty;
    uint32_t project_format_version;
    ps_text_document project_manifest_snapshot;
    enum nk_collapse_states project_build_disclosure;
    struct nk_rect migration_bounds;
    char parameter_output[8192];
    size_t parameter_output_used;
    bool parameter_output_overflow;
    int runner_memory_mib;
    double runner_wall_seconds, batch_timeout;
    enum nk_collapse_states limits_disclosure;
    struct nk_rect limit_bounds[2];
    SDL_Thread *batch_thread;
    SDL_AtomicInt batch_done, batch_cancel, batch_completed, batch_active;
    ps_batch_options batch_options;
    ps_batch_result batch_result;
    char *batch_source;
    ps_result batch_code;
    int batch_runs, batch_steps, batch_workers;
    double batch_dt;
    bool batch_target,batch_adaptive;
    double batch_end_time,batch_minimum_dt,batch_maximum_dt;
    char batch_timing_text[4][64];
    double batch_timing_displayed[4];
    struct nk_rect batch_target_bounds,batch_adaptive_bounds,batch_timing_bounds[4];
    char batch_seed[32], batch_channel[48], batch_last_report[4096];
    bool batch_sweep;
    bool batch_sweep_ready;
    int batch_sweep_parameter;
    char batch_sweep_start[64], batch_sweep_end[64];
    struct nk_rect batch_start_bounds, batch_cancel_bounds, batch_navigation_bounds, batch_resume_bounds, batch_analyze_bounds;
    char batch_resume_test_path[4096];
    struct nk_rect batch_sweep_bounds;
    struct nk_rect batch_sweep_value_bounds[2];
    struct nk_rect batch_workers_bounds;
    int job_kind;
    diagnostic diagnostics[64];
    int diagnostic_count;
    char diagnostic_line[8192];
    size_t diagnostic_used;
    ps_wire_buffer wire;
    uint32_t command_seq;
    bool reset_pending, start_paused, reset_starting;
    char reset_previous_run[4096], reset_channel_names[PS_MAX_CHANNELS][96];
    uint32_t reset_channel_count;
    struct nk_rect run_control_bounds[6];
    struct nk_rect speed_bounds, speed_choices[8];
    double simulation_speed;
    bool adaptive_steps;
    double minimum_dt,maximum_dt;
    struct nk_rect adaptive_bounds;
    struct nk_rect adaptive_limit_bounds[3];
    char adaptive_limit_text[3][64];
    double adaptive_limit_displayed[3];
    double heartbeat, stop_at, simulation_time, dt;
    char seed[32];
    double values[PS_MAX_CHANNELS];
    uint32_t channel_count;
    char channel_names[PS_MAX_CHANNELS][96];
    int channel_status[PS_MAX_CHANNELS];
    ps_scene scene;
    ps_scene_view scene_view;
    bool scene_selected;
    struct nk_rect scene_expand_bounds[PS_MAX_OBJECTS];
    uint32_t scene_selected_id, scene_selected_slot;
    struct nk_rect scene_viewport_bounds;
    struct nk_rect scene_label_bounds[PS_MAX_OBJECTS];
    struct nk_rect scene_hide_selection_bounds;
    enum nk_collapse_states scene_disclosure;
    enum nk_collapse_states steps_disclosure;
    struct nk_rect scene_visibility_bounds[PS_MAX_OBJECTS];
    double history_t[PREVIEW], history_v[PS_MAX_CHANNELS][PREVIEW];
    int history_count;
    uint64_t history_seen, history_stride;
    float yaw, pitch, zoom;
    ps_timeline timeline;
    ps_snapshot timeline_view;
    bool timeline_browsing, timeline_playing, timeline_scenes;
    double timeline_target, timeline_wall;
    char timeline_error[160];
    struct nk_rect timeline_bounds[5];
    dataset data;
} app;
static const ps_scene *display_scene(const app *a) {
    return a->timeline_browsing ? &a->timeline_view.scene : &a->scene;
}
static double display_time(const app *a) {
    return a->timeline_browsing ? a->timeline_view.time : a->simulation_time;
}
static const double *display_values(const app *a) {
    return a->timeline_browsing ? a->timeline_view.values : a->values;
}
static void timeline_live(app *a) {
    a->timeline_browsing = a->timeline_playing = false;
    ps_scene_view_sync(&a->scene_view, &a->scene);
}
static void timeline_clear(app *a) {
    ps_timeline_clear(&a->timeline);
    a->timeline_browsing = a->timeline_playing = a->timeline_scenes = false;
    a->timeline_error[0] = 0;
}
static void timeline_select(app *a, uint32_t index) {
    const ps_snapshot *snapshot = ps_timeline_get(&a->timeline, index);
    if (!snapshot) return;
    a->timeline_view = *snapshot;
    a->timeline_browsing = true;
    ps_scene_view_sync(&a->scene_view, &a->timeline_view.scene);
}
static void timeline_tick(app *a) {
    if (!a->timeline_playing) return;
    uint32_t count = ps_timeline_count(&a->timeline);
    if (!count) { a->timeline_playing = false; return; }
    double now = ps_clock();
    a->timeline_target += fmin(.25, fmax(0, now - a->timeline_wall));
    a->timeline_wall = now;
    uint32_t index = ps_timeline_nearest(&a->timeline, a->timeline_target);
    /* Playback presents recorded states without stepping the experiment. */
    timeline_select(a, index);
    if (a->timeline_target >= a->timeline.latest.time) a->timeline_playing = false;
}
static void timeline_toggle_play(app *a) {
    if (a->reset_pending || a->reset_starting || ps_timeline_count(&a->timeline) < 2) return;
    if (a->timeline_playing) { a->timeline_playing = false; return; }
    if (!a->timeline_browsing || a->timeline_view.time >= a->timeline.latest.time)
        timeline_select(a, 0);
    a->timeline_target = a->timeline_view.time;
    a->timeline_wall = ps_clock();
    a->timeline_playing = true;
}
static void refresh_library(app *a);
static void open_library(app *a);
static bool action_button(struct nk_context *ui, const char *label, bool primary);
static void open_settings(app *a);
enum { PS_DIALOG_OPEN_FOLDER = 1, PS_DIALOG_ADD_FILE, PS_DIALOG_ADD_FOLDER,
       PS_DIALOG_MANAGER_PARENT, PS_DIALOG_IMPORT_RUN, PS_DIALOG_RESUME_BATCH };
static void choose_workspace_path(app *a, int mode);
static void import_run(app *a,const char *path);
static void resume_batch(app *a,const char *path);
static void analyze(app *a,bool csv);
static void create_managed_project(app *a);
static bool open_workspace_path(app *a, const char *path);
static void restore_workspace(app *a);
static void workspace_catalog_start(app *a);
static void workspace_catalog_store(app *a);
static void workspace_catalog_open(app *a);
static void workspace_catalog_delete(app *a);
static void workspace_catalog_reset(app *a);
static void forget_workspace(app *a);
static const char *experiment_source(const app *a) {
    return a->language_experiment ? "main.phys" : "main.c";
}
static const char *analysis_source(const app *a) {
    return a->language_analysis ? "analysis.phys" : "analysis.c";
}
static void preview_workspace_path(app *a,const char *path);
static const char *diagnostic_basename(const char *path) {
    const char *base=path;for(const char *p=path;*p;p++)if(*p=='/' || *p=='\\')base=p+1;return base;
}
static void structured_diagnostic(app *a,const ps_diagnostic *record,bool analysis) {
    if(!ps_diagnostic_valid(record) || record->code==PS_OK || a->diagnostic_count>=64)return;
    /* Prefer a typed record over a legacy line already received from stderr. */
    for(int i=0;i<a->diagnostic_count;i++)if(a->diagnostics[i].analysis==analysis &&
        !strcmp(diagnostic_basename(a->diagnostics[i].source),diagnostic_basename(record->source)) &&
        (a->diagnostics[i].line==(int)record->line || (!a->diagnostics[i].structured && strstr(a->diagnostics[i].text,"runtime error")))) {
        memmove(a->diagnostics+i,a->diagnostics+i+1,(size_t)(a->diagnostic_count-i-1)*sizeof a->diagnostics[0]);a->diagnostic_count--;break;
    }
    diagnostic *d=&a->diagnostics[a->diagnostic_count++];memset(d,0,sizeof *d);
    d->analysis=analysis;d->structured=true;d->code=record->code;d->line=record->line<=INT_MAX?(int)record->line:0;
    d->column=record->column<=INT_MAX?(int)record->column:1;
    snprintf(d->source,sizeof d->source,"%s",record->source);
    if(*d->source) {
        char actual[4096],expected[4096];
        if(d->source[0]=='/' || d->source[0]=='\\' || (d->source[0] && d->source[1]==':'))snprintf(actual,sizeof actual,"%s",d->source);
        else snprintf(actual,sizeof actual,"%s/%s",a->project,d->source);
        snprintf(expected,sizeof expected,"%s/%s",a->project,analysis?analysis_source(a):experiment_source(a));
        d->foreign=strcmp(actual,expected) && !ps_text_document_same_file(actual,expected);
    }
    ps_diagnostic display=*record;
    snprintf(display.source,sizeof display.source,"%s",diagnostic_basename(record->source));
    for(char *p=display.message;*p;p++)if(*p=='\n' || *p=='\r' || *p=='\t')*p=' ';
    (void)ps_diagnostic_format(&display,d->text,sizeof d->text);a->show_log=true;
}
static void parse_diagnostic(app *a, const char *line) {
    const char *source = experiment_source(a);
    const char *p = strstr(line, source);
    bool analysis = false;
    if (!p) {
        source = analysis_source(a);
        p = strstr(line, source);
        analysis = true;
    }
    if (!p || a->diagnostic_count >= 64)
        return;
    p += strlen(source);
    int row = 0, column = 1;
    if (*p == '(') {
        if (sscanf(p + 1, "%d,%d", &row, &column) < 1)
            return;
    } else if (*p == ':') {
        if (sscanf(p + 1, "%d:%d", &row, &column) < 1)
            return;
    } else
        return;
    if (row <= 0 || (!strstr(p, "error") && !strstr(p, "warning")))
        return;
    diagnostic *d = &a->diagnostics[a->diagnostic_count++];
    a->show_log = true;
    d->line = row;
    d->column = column;
    d->analysis = analysis;d->foreign=false;d->structured=false;d->code=PS_INVALID;
    snprintf(d->source,sizeof d->source,"%s",source);
    snprintf(d->text, sizeof d->text, "%s:%d:%d %s", source, row,
             column, p);
}
static void diagnostic_bytes(app *a, const char *text, size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (text[i] == '\n') {
            a->diagnostic_line[a->diagnostic_used] = 0;
            parse_diagnostic(a, a->diagnostic_line);
            a->diagnostic_used = 0;
        } else if (text[i] != '\r' && a->diagnostic_used + 1 < sizeof a->diagnostic_line)
            a->diagnostic_line[a->diagnostic_used++] = text[i];
    }
}
static void jump_to_diagnostic(app *a, const diagnostic *d) {
    if(d->line<=0 || !d->source[0])return;
    struct nk_text_edit *edit = d->analysis ? &a->analysis : &a->experiment;
    if(d->foreign) {
        char path[4096];
        if(d->source[0]=='/' || d->source[0]=='\\' || (d->source[0] && d->source[1]==':'))snprintf(path,sizeof path,"%s",d->source);
        else snprintf(path,sizeof path,"%s/%s",a->project,d->source);
        preview_workspace_path(a,path);
        if(a->tab!=8 || a->document_active>=a->document_count)return;
        if(!ps_text_document_same_file(path,a->documents[a->document_active].file.path))return;
        edit=&a->documents[a->document_active].edit;
    }
    const char *text = nk_str_get_const(&edit->string);
    int size = nk_str_len_char(&edit->string), at = 0, line = 1;
    while (at < size && line < d->line)
        if (text[at++] == '\n')
            line++;
    int start = at;
    while (at < size && text[at] != '\n')
        at++;
    edit->select_start = nk_utf_len(text, start);
    edit->select_end = nk_utf_len(text, at);
    edit->cursor = edit->select_start;
    edit->scrollbar.y =
        (float)(line > 3 ? line - 3 : 0) * (a->font_code->height + a->ui->style.edit.row_padding);
    edit->scrollbar.x = 0;
    if(d->foreign)return;
    a->tab = d->analysis ? 2 : 0;
    if (d->analysis)
        a->analysis_tab = 1;
}
static void log_line(app *a, const char *format, ...) {
    char text[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(text, sizeof text, format, args);
    va_end(args);
    size_t n = strlen(a->log), m = strlen(text);
    if (n + m + 2 >= sizeof a->log) {
        size_t drop = sizeof a->log / 2;
        while(drop<n && a->log[drop]!='\n')drop++;
        if(drop<n)drop++;
        memmove(a->log, a->log + drop, n - drop + 1);
        n -= drop;
    }
    memcpy(a->log + n, text, m);
    a->log[n + m] = '\n';
    a->log[n + m + 1] = 0;
}
static void status(app *a, const char *s) {
    snprintf(a->status, sizeof a->status, "%s", s);
    log_line(a, "%s", s);
}
static void join(char *out, size_t n, const char *base, const char *leaf) {
    snprintf(out, n, "%s/%s", base, leaf);
}
static bool exists(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f)
        return false;
    fclose(f);
    return true;
}
static bool copy_file_mode(const char *src, const char *dst, const char *mode) {
    FILE *in = fopen(src, "rb");
    if (!in)
        return false;
    FILE *out = fopen(dst, mode);
    if (!out) {
        fclose(in);
        return false;
    }
    char buf[8192];
    size_t n;
    bool ok = true;
    while ((n = fread(buf, 1, sizeof buf, in)) != 0)
        if (fwrite(buf, 1, n, out) != n) {
            ok = false;
            break;
        }
    if (ferror(in))
        ok = false;
    fclose(in);
    if (fclose(out))
        ok = false;
    return ok;
}
static bool copy_file_exclusive(const char *src, const char *dst) {
    return copy_file_mode(src, dst, "wbx");
}
static char *load_utf8_text(const char *path, size_t *length) {
    FILE *f = fopen(path, "rb");
    if (!f)
        return NULL;
    if (fseek(f, 0, SEEK_END)) {
        fclose(f);
        return NULL;
    }
    long size = ftell(f);
    if (size < 0 || size > 256 * 1024) {
        fclose(f);
        return NULL;
    }
    rewind(f);
    char *text = malloc((size_t)size + 1);
    if (!text) {
        fclose(f);
        return NULL;
    }
    size_t n = fread(text, 1, (size_t)size, f);
    bool ok = !ferror(f) && n == (size_t)size;
    fclose(f);
    /* Accept well-formed UTF-8 source text, never silently replace invalid bytes. */
    for (size_t at = 0; ok && at < n;) {
        nk_rune rune;
        char encoded[NK_UTF_SIZE];
        int used = nk_utf_decode(text + at, &rune, (int)(n - at));
        int encoded_size = nk_utf_encode(rune, encoded, NK_UTF_SIZE);
        if (used <= 0 || used != encoded_size || memcmp(text + at, encoded, (size_t)used) ||
            rune == 127 || (rune < 32 && rune != '\n' && rune != '\r' && rune != '\t')) {
            ok = false;
            break;
        }
        at += (size_t)used;
    }
    if (!ok) {
        free(text);
        return NULL;
    }
    text[n] = 0;
    *length = n;
    return text;
}
static bool set_editor_text(struct nk_text_edit *edit, const char *text, size_t n) {
    bool ok = ps_source_text_valid(text, n);
    if (ok) {
        struct nk_text_edit replacement;
        nk_textedit_init_default(&replacement);
        replacement.mode = NK_TEXT_EDIT_MODE_INSERT;
        replacement.single_line = false;
        nk_str_append_text_char(&replacement.string, text, (int)n);
        ok = nk_str_len_char(&replacement.string) == (int)n &&
             (!n || memcmp(nk_str_get_const(&replacement.string), text, n) == 0);
        if (ok) {
            replacement.cursor = 0;
            replacement.select_start = replacement.select_end = 0;
            nk_textedit_free(edit);
            *edit = replacement;
        } else
            nk_textedit_free(&replacement);
    }
    return ok;
}
static bool load_editor(struct nk_text_edit *edit, const char *path) {
    size_t n = 0;
    char *text = load_utf8_text(path, &n);
    bool ok = text && set_editor_text(edit, text, n);
    free(text);
    return ok;
}
static bool save_editor(struct nk_text_edit *edit, const char *path) {
    int n = nk_str_len_char(&edit->string);
    if (n < 0) return false;
    const char *text = nk_str_get_const(&edit->string);
    if (exists(path)) {
        ps_text_document document = {0};
        bool ok = ps_text_document_open(&document, path) == PS_DOCUMENT_OK &&
                  ps_text_document_save(&document, text, (size_t)n) == PS_DOCUMENT_OK;
        ps_text_document_destroy(&document);
        return ok;
    }
    if (!ps_source_text_valid(text, (size_t)n)) return false;
    char temporary[4096];
    if (ps_private_temporary_write(path, text, (size_t)n, NULL, temporary) != PS_OK)
        return false;
    bool ok = SDL_RenamePath(temporary, path);
    if (!ok) SDL_RemovePath(temporary);
    return ok;
}
static bool save_project_settings(app *a) {
    char path[4096];
    join(path, sizeof path, a->project, "physim.project");
    ps_project_settings settings = {.analysis_only=a->analysis_only, .release = a->profile != 0, .timestep = a->dt,
                                   .speed = a->simulation_speed, .parameters = a->parameters,
                                   .adaptive=a->adaptive_steps,.minimum_timestep=a->minimum_dt,
                                   .maximum_timestep=a->maximum_dt};
    if (!ps_project_seed_parse(a->seed, &settings.seed)) return false;
    if(ps_project_settings_save(path,&settings)!=PS_DOCUMENT_OK)return false;
    if(ps_text_document_open(&a->project_manifest_snapshot,path)!=PS_DOCUMENT_OK)return false;
    ps_project_settings verified;
    if(ps_project_settings_read_document(&a->project_manifest_snapshot,&verified)!=PS_DOCUMENT_OK)return false;
    a->project_format_version=verified.format_version;return true;
}
static bool simulation_settings_valid(app *a) {
    uint64_t seed;
    if (!ps_project_timestep_valid(a->dt)) {
        status(a, "Zeitschritt: positiven, endlichen Wert bis 1 Sekunde eingeben.");
        return false;
    }
    if (!ps_project_seed_parse(a->seed, &seed)) {
        status(a, "Zufallsseed: ganze Zahl von 0 bis 18446744073709551615 eingeben.");
        return false;
    }
    if (!ps_speed_valid(a->simulation_speed)) {
        status(a, "Geschwindigkeit: 0 für Offline oder Faktor 0,1 bis 16 wählen.");
        return false;
    }
    ps_project_settings steps={.timestep=a->dt,.adaptive=a->adaptive_steps,
                               .minimum_timestep=a->minimum_dt,.maximum_timestep=a->maximum_dt};
    if(!ps_project_step_bounds_valid(&steps)) {
        status(a,"Adaptive Schritte: 0 < Minimum ≤ Startschritt ≤ Maximum ≤ 1 Sekunde wählen.");
        return false;
    }
    return true;
}
static bool test_editor_failed_import(app *a) {
    char path[4096];
    join(path, sizeof path, a->project, "invalid-import-test.c");
    FILE *f = fopen(path, "wbx");
    if (!f)
        return false;
    const unsigned char invalid[] = {'A', 0xff, 'B'};
    bool ok = fwrite(invalid, 1, sizeof invalid, f) == sizeof invalid;
    if (fclose(f))
        ok = false;
    int length = nk_str_len_char(&a->experiment.string);
    uint32_t before =
        ps_crc32((const unsigned char *)nk_str_get_const(&a->experiment.string), (size_t)length);
    if (ok) {
        bool loaded = load_editor(&a->experiment, path);
        int after_length = nk_str_len_char(&a->experiment.string);
        uint32_t after = ps_crc32((const unsigned char *)nk_str_get_const(&a->experiment.string),
                                  (size_t)after_length);
        ok = !loaded && after_length == length && after == before;
        if (!ok)
            log_line(a, "Import regression: loaded=%d, length=%d/%d, crc=%08x/%08x", loaded, length,
                     after_length, before, after);
    }
    if (remove(path))
        ok = false;
    return ok;
}
/* Exercises the actual primary-editor path using only self-test fixtures. */
static bool test_editor_safe_save(app *a) {
    char source[4096], foreign[4096], candidate[4096], backup[4096];
    join(source,sizeof source,a->project,"safe-save-test.c");
    join(foreign,sizeof foreign,a->project,"safe-save-foreign.txt");
    int n=snprintf(candidate,sizeof candidate,"%s.tmp",source);
    if(n<0 || n>=(int)sizeof candidate)return false;
    n=snprintf(backup,sizeof backup,"%s.bak",source);
    if(n<0 || n>=(int)sizeof backup)return false;
    FILE *f=fopen(foreign,"wbx");if(!f)return false;
    bool ok=fputs("foreign contents",f)>=0;
    if(fclose(f))ok=false;
    struct nk_text_edit edit;nk_textedit_init_default(&edit);
    ok=ok && set_editor_text(&edit,"source one",10) && save_editor(&edit,source);
#ifndef _WIN32
    if(ok)ok=!chmod(source,0700) && !symlink(foreign,candidate);
#else
    if(ok){f=fopen(candidate,"wbx");ok=f && fputs("foreign contents",f)>=0;if(f && fclose(f))ok=false;}
#endif
    ok=ok && set_editor_text(&edit,"source two",10) && save_editor(&edit,source);
    size_t size=0;char *bytes=load_utf8_text(foreign,&size);
    ok=ok && bytes && size==16 && !memcmp(bytes,"foreign contents",16);free(bytes);
    bytes=load_utf8_text(backup,&size);
    ok=ok && bytes && size==10 && !memcmp(bytes,"source one",10);free(bytes);
#ifndef _WIN32
    struct stat x,y;
    ok=ok && !stat(source,&x) && !stat(backup,&y) && (x.st_mode&0777)==0700 && (y.st_mode&0777)==0700;
    ok=ok && !lstat(candidate,&x) && S_ISLNK(x.st_mode);
#endif
    nk_textedit_free(&edit);
    SDL_RemovePath(candidate);SDL_RemovePath(source);SDL_RemovePath(backup);SDL_RemovePath(foreign);
    return ok;
}
// clang-format off
#include "autosave_actions.inc"
// clang-format on
static void save_project(app *a) {
    if (!a->loaded || a->recovery)
        return;
    if (!a->dirty && !a->analysis_dirty && !a->project_settings_dirty)
        return;
    if (!simulation_settings_valid(a)) return;
    char *saved[2] = {copy_editor_text(&a->experiment), copy_editor_text(&a->analysis)};
    if (!saved[0] || !saved[1]) {
        free(saved[0]);
        free(saved[1]);
        status(a, "Speichern fehlgeschlagen: Quelle zu groß oder Speicher erschöpft.");
        return;
    }
    char p[4096];
    join(p, sizeof p, a->project, experiment_source(a));
    bool ok = !a->dirty || (!a->analysis_only && save_editor(&a->experiment, p));
    join(p, sizeof p, a->project, analysis_source(a));
    ok = (!a->analysis_dirty || save_editor(&a->analysis, p)) && ok;
    if (ok && a->project_settings_dirty)
        ok = save_project_settings(a);
    if (ok) {
        a->dirty = a->analysis_dirty = a->project_settings_dirty = false;
        for (unsigned i = 0; i < 2; i++) {
            free(a->saved_source[i]);
            a->saved_source[i] = saved[i];
            saved[i] = NULL;
        }
        a->autosave_current = false;
        a->autosave_due = ps_clock() + a->preferences.autosave_seconds;
        if (!a->autosave_blocked) {
            autosave_path(a, p, sizeof p);
            if (exists(p) && !SDL_RemovePath(p))
                log_line(a, "Alte Autosave-Datei konnte nicht entfernt werden.");
            a->autosave_error[0] = 0;
        }
        status(a, "Projekt gespeichert.");
    } else
        status(a, "Speichern fehlgeschlagen; vorhandene Dateien und Backups pruefen.");
    free(saved[0]);
    free(saved[1]);
}
static void invalidate_build(app *a) {
    ++a->source_revision;
    a->built = false;
}
static void select_build_profile(app *a, int profile) {
    if (!a->loaded || (profile != 0 && profile != 1) || a->profile == profile) return;
    a->profile = profile;
    a->project_settings_dirty = true;
    invalidate_build(a);
}
#include "document_actions.inc"
static bool build_inputs_current(const app *a) {
    return a->source_revision == a->build_revision && !a->dirty && !a->analysis_dirty &&
           !documents_dirty(a);
}
static bool jobs_idle(app *a) {
    return !a->job.running && !a->runner.running && !a->data.thread && !a->report_thread &&
           !a->batch_thread && !a->import_thread;
}
static bool idle(app *a) { return !a->reset_pending && jobs_idle(a); }
static void clear_project(app *a) {
    ps_text_document_destroy(&a->project_manifest_snapshot);a->project_format_version=0;
    ps_report_destroy(a->analysis_report);
    a->analysis_report = NULL;
    ps_library_destroy(a->library);
    a->library = NULL;
    nk_textedit_free(&a->experiment);
    nk_textedit_free(&a->analysis);
    nk_textedit_init_default(&a->experiment);
    nk_textedit_init_default(&a->analysis);
    for (unsigned i = 0; i < 2; i++) {
        free(a->saved_source[i]);
        a->saved_source[i] = NULL;
    }
    a->project[0] = a->last_run[0] = a->report[0] = 0;
    a->loaded = a->built = a->dirty = a->analysis_dirty = false;
    a->project_settings_dirty = false;
    memset(&a->parameters, 0, sizeof a->parameters);
    a->language_experiment = a->language_analysis = a->analysis_only = false;
    a->selected_count = a->channel_count = 0;
    a->history_count = 0;
    timeline_clear(a);
    a->scene.count = 0;
    a->scene_view = (ps_scene_view){0};
    a->scene_selected = false;
    a->loaded_report_path[0] = a->result_error[0] = 0;
    memset(a->live_channels,0,sizeof a->live_channels);
    ps_timeline_destroy(&a->data.timeline);
    memset(&a->data, 0, sizeof a->data);
}
static void refresh_workspace_entries(app *a) {
    a->workspace_tree_error[0] = 0;
    if (!a->workspace_open) {
        ps_workspace_tree_destroy(&a->workspace_tree);
        return;
    }
    const char *roots[33] = {a->workspace};
    for (unsigned i = 0; i < a->workspace_addition_count; i++)
        roots[i + 1] = a->workspace_additions[i];
    ps_result r = ps_workspace_tree_refresh(&a->workspace_tree, roots,
                                           a->workspace_addition_count + 1);
    if (r != PS_OK)
        snprintf(a->workspace_tree_error, sizeof a->workspace_tree_error,
                 "Dateibaum nicht vollständig geladen (%s). Betroffene Ordner sind markiert.",
                 ps_result_string(r));
}
static bool migrate_current_project(app *a) {
    if(!a->loaded || !idle(a) || a->library_thread || a->recovery)return false;
    /* Migrating the manifest must not implicitly save unrelated editor changes. */
    if(a->dirty || a->analysis_dirty || a->project_settings_dirty) {
        status(a,"Vor der Formataktualisierung Änderungen speichern oder verwerfen.");return false;
    }
    char path[4096];join(path,sizeof path,a->project,"physim.project");ps_project_migration report;
    if(!ps_text_document_same_file(path,a->project_manifest_snapshot.path))return false;
    ps_document_result result=ps_project_migrate_document(&a->project_manifest_snapshot,&report);
    if(result!=PS_DOCUMENT_OK){status(a,result==PS_DOCUMENT_CONFLICT?"Projektbeschreibung wurde extern geändert. Projekt erneut öffnen.":"Projektformat konnte nicht aktualisiert werden; vorhandene Dateien bleiben erhalten.");return false;}
    a->project_format_version=report.to_version;
    status(a,report.changed?"Projektformat auf Version 2 aktualisiert. Sicherung: physim.project.bak.":"Projektformat ist bereits aktuell; keine Dateien geändert.");return true;
}
static bool open_project(app *a) {
    bool leaving_manager = a->project_manager;
    if (a->recovery)
        return false;
    if (!idle(a) || a->library_thread) {
        status(a, "Zuerst den laufenden Job beenden.");
        return false;
    }
    if (a->dirty || a->analysis_dirty || a->project_settings_dirty)
        save_project(a);
    if (a->dirty || a->analysis_dirty || a->project_settings_dirty)
        return false;
    if (!documents_save_all(a)) return false;
    char p[4096];
    join(p, sizeof p, a->project_input, "physim.project");
    ps_project_settings project_settings;ps_text_document manifest_snapshot={0};
    if (ps_text_document_open(&manifest_snapshot,p)!=PS_DOCUMENT_OK ||
        ps_project_settings_read_document(&manifest_snapshot,&project_settings)!=PS_DOCUMENT_OK) {
        ps_text_document_destroy(&manifest_snapshot);
        status(a, "Projektdatei fehlt oder ist ungültig. Quellen und Einstellungen bleiben erhalten.");
        return false;
    }
    bool language = project_settings.language_experiment;
    bool analysis_language = project_settings.language_analysis;
    struct nk_text_edit experiment, analysis;
    nk_textedit_init_default(&experiment);
    nk_textedit_init_default(&analysis);
    join(p, sizeof p, a->project_input, language ? "main.phys" : "main.c");
    if (!project_settings.analysis_only && !load_editor(&experiment, p)) {
        nk_textedit_free(&experiment);
        nk_textedit_free(&analysis);
        status(a, "Experimentquelle konnte nicht geladen werden (UTF-8, maximal 256 KiB).");
        ps_text_document_destroy(&manifest_snapshot);return false;
    }
    join(p, sizeof p, a->project_input, analysis_language ? "analysis.phys" : "analysis.c");
    if (!load_editor(&analysis, p)) {
        nk_textedit_free(&experiment);
        nk_textedit_free(&analysis);
        status(a, "Analysequelle konnte nicht geladen werden.");
        ps_text_document_destroy(&manifest_snapshot);return false;
    }
    char *saved[2] = {copy_editor_text(&experiment), copy_editor_text(&analysis)};
    if (!saved[0] || !saved[1]) {
        free(saved[0]);
        free(saved[1]);
        nk_textedit_free(&experiment);
        nk_textedit_free(&analysis);
        status(a, "Projekt konnte nicht geladen werden: Speicher erschöpft.");
        ps_text_document_destroy(&manifest_snapshot);return false;
    }
    documents_clear(a);
    clear_project(a);
    for (unsigned i = 0; i < 2; i++) {
        free(a->saved_source[i]);
        a->saved_source[i] = saved[i];
    }
    nk_textedit_free(&a->experiment);
    nk_textedit_free(&a->analysis);
    a->experiment = experiment;
    a->language_experiment = language;
    a->language_analysis = analysis_language;
    a->analysis_only=project_settings.analysis_only;
    a->project_format_version=project_settings.format_version;
    a->project_manifest_snapshot=manifest_snapshot;
    a->analysis = analysis;
    snprintf(a->project, sizeof a->project, "%s", a->project_input);
    snprintf(a->workspace, sizeof a->workspace, "%s", a->project_input);
    a->workspace_open = true;
    a->workspace_disclosure = NK_MINIMIZED;
    a->workspace_addition_count = 0;
    refresh_workspace_entries(a);
    a->project_manager = false;
    a->workspace_preview_path[0] = 0;
    if (leaving_manager || a->tab == 8)
        a->tab = 0;
    a->loaded = true;
    a->built = false;
    a->parameters = project_settings.parameters;
    a->profile = project_settings.release ? 1 : 0;
    a->dt = project_settings.timestep;
    a->simulation_speed = project_settings.speed;
    a->adaptive_steps=project_settings.adaptive;
    a->minimum_dt=project_settings.minimum_timestep;
    a->maximum_dt=project_settings.maximum_timestep;
    snprintf(a->seed, sizeof a->seed, "%llu", (unsigned long long)project_settings.seed);
    a->project_settings_dirty = false;
    a->batch_sweep = false;
    a->batch_target=a->batch_adaptive=false;
    a->batch_sweep_ready = false;
    a->batch_sweep_parameter = 0;
    a->batch_sweep_start[0] = a->batch_sweep_end[0] = 0;
    a->dirty = a->analysis_dirty = false;
    a->last_run[0] = 0;
    memset(&a->batch_options, 0, sizeof a->batch_options);
    memset(&a->batch_result, 0, sizeof a->batch_result);
    a->batch_last_report[0] = 0;
    SDL_SetAtomicInt(&a->batch_completed, 0);
    ps_report_destroy(a->analysis_report);
    a->analysis_report = NULL;
    a->show_report = false;
    a->result_error[0] = 0;
    ps_timeline_destroy(&a->data.timeline);
    memset(&a->data, 0, sizeof a->data);
    timeline_clear(a);
    a->scene.count = 0;
    a->scene_view = (ps_scene_view){0};
    a->scene_selected = false;
    a->history_count = 0;
    a->channel_count = 0;
    ps_library_destroy(a->library);
    a->library = NULL;
    a->selected_count = 0;
    a->library_query[0] = 0;
    a->library_error[0] = 0;
    refresh_library(a);
    if(a->analysis_only){a->tab=0;a->analysis_tab=1;}
    status(a, a->analysis_only?"Analyseprojekt geladen. Messläufe importieren und Analyse bauen.":"Projekt geladen. Build kompiliert die lokalen Quelldateien.");
    check_recovery(a);
    return true;
}
static void new_project(app *a) {
    if (!documents_save_all(a)) return;
    if (!idle(a) || a->library_thread || a->recovery)
        return;
    char p[4096], src[4096];
    const char *reserved[] = {"main.c", "main.phys", "analysis.c", "analysis.phys", "physim.project",
                              ".physim-autosave"};
    for (size_t i = 0; i < sizeof reserved / sizeof reserved[0]; i++) {
        join(p, sizeof p, a->project_input, reserved[i]);
        if (exists(p)) {
            status(a,
                   "Projektdateien existieren bereits. Oeffnen oder einen neuen Ordner waehlen.");
            return;
        }
    }
    bool language = a->example >= 8 && a->example <= 19;
    join(p, sizeof p, a->project_input, language ? "main.phys" : "main.c");
    if (!ps_make_directory(a->project_input)) {
        status(a, "Projektordner konnte nicht erstellt werden. Elternordner muss existieren.");
        return;
    }
    const char *examples[] = {"pendulum",  "projectile", "collision",
                              "box_floor", "spring",     "uncertain_projectile", "box_collision", "buoyancy"};
    if (language)
        snprintf(src, sizeof src, "%s/examples/language/%s.phys", a->root,
                 a->example == 8 ? "pendulum" : a->example == 9 ? "projectile_drag"
                 : a->example == 10 ? "uncertain_projectile" : a->example == 11 ? "spinning_body"
                 : a->example == 12 ? "box_contacts" : a->example == 13 ? "joint_pendulum"
                 : a->example == 14 ? "coupled_bodies" : a->example == 15 ? "fast_sphere"
                 : a->example == 16 ? "spring" : a->example == 17 ? "buoyancy"
                 : a->example == 18 ? "collision" : "box_collision");
    else
        snprintf(src, sizeof src, "%s/examples/%s/main.c", a->root, examples[a->example]);
    if (!a->manager_analysis_only && !copy_file_exclusive(src, p)) {
        status(a, "Experimentvorlage konnte nicht kopiert werden.");
        return;
    }
    bool analysis_language = a->template_analysis_language == 1;
    join(src, sizeof src, a->root, a->manager_analysis_only
        ? (analysis_language?"examples/analysis_only/analysis.phys":"examples/analysis_only/analysis.c")
        : analysis_language
        ? (a->example == 0 || a->example == 8 ? "examples/documentation/pendulum_analysis.phys"
           : a->example == 10 || a->example == 5 ? "examples/language/analysis_sensors.phys"
           : a->example == 18 || a->example == 2 ? "examples/language/analysis_collision.phys"
           : a->example == 19 || a->example == 6 ? "examples/language/analysis_box_collision.phys"
           : a->example == 17 || a->example == 7 ? "examples/language/analysis_buoyancy.phys"
                                                  : "examples/language/analysis.phys")
        : "examples/pendulum/analysis.c");
    join(p, sizeof p, a->project_input, analysis_language ? "analysis.phys" : "analysis.c");
    if (!copy_file_exclusive(src, p)) {
        status(a, "Analysevorlage fehlt.");
        return;
    }
    join(p, sizeof p, a->project_input, "physim.project");
    FILE *f = fopen(p, "wx");
    if (!f) {
        status(a, "Projektbeschreibung konnte nicht geschrieben werden.");
        return;
    }
    bool written = a->manager_analysis_only
        ? fprintf(f,"physim_project=2\nkind=analysis\nanalysis=%s\nmodules=core,units,data,analysis\nprofile=Debug\n",analysis_language?"analysis.phys":"analysis.c")>0
        : fprintf(f, "physim_project=2\nkind=experiment\nexperiment=%s\nanalysis=%s\nmodules=core,units,mechanics,"
               "data,analysis\nprofile=Debug\nsimulation.dt=0.005\nsimulation.seed=42\n", language ? "main.phys" : "main.c",
               analysis_language ? "analysis.phys" : "analysis.c") > 0;
    if (fclose(f)) written = false;
    if (!written) {
        status(a, "Projektbeschreibung konnte nicht vollständig gespeichert werden.");
        return;
    }
    join(p, sizeof p, a->project_input, "runs");
    ps_make_directory(p);
    open_project(a);
    if (a->loaded && a->example == 4) {
        a->camera_target = ps_v3(-.35, .1, 0);
        a->zoom = 2.5f;
    }
    if (a->loaded && a->example == 7) {
        a->camera_target = ps_v3(0, 0, 0);
        a->zoom = 1.3f;
    }
    if (a->loaded && a->example == 6) {
        a->camera_target = ps_v3(0, -.1, 0);
        a->zoom = 2.8f;
    }
    if (a->loaded && a->example == 13) {
        a->camera_target = ps_v3(0, 1.2, 0);
        a->zoom = 4.5f;
    }
    if (a->loaded && a->example == 14) {
        a->camera_target = ps_v3(0.8, 0.9, 0);
        a->zoom = 4.5f;
    }
    if (a->loaded && a->example == 15) {
        a->camera_target = ps_v3(0, 0.6, 0);
        a->zoom = 4.5f;
    }
    if (a->loaded && a->example == 16) {
        a->camera_target = ps_v3(0, 0, 0);
        a->zoom = 2.8f;
    }
    if (a->loaded && a->example == 17) {
        a->camera_target = ps_v3(0, 0, 0);
        a->zoom = 2.4f;
    }
}
static void build_project(app *a) {
    if (a->recovery)
        return;
    if (!a->loaded || !idle(a))
        return;
    if (!documents_save_all(a)) return;
    save_project(a);
    if (a->dirty || a->analysis_dirty || a->project_settings_dirty)
        return;
    char manifest[4096];join(manifest,sizeof manifest,a->project,"physim.project");
    ps_project_settings current;
    if(ps_project_settings_read(manifest,&current)!=PS_DOCUMENT_OK || current.analysis_only!=a->analysis_only ||
       current.language_analysis!=a->language_analysis || current.language_experiment!=a->language_experiment) {
        invalidate_build(a);status(a,"Projektbeschreibung wurde geändert. Projekt erneut öffnen.");return;
    }
    char builder[4096], compiler[4096];
    join(a->build_directory, sizeof a->build_directory, a->project,
         a->profile ? "build/Release" : "build/Debug");
    join(builder, sizeof builder, a->bin, "physim-build" EXE_EXT);
    join(compiler, sizeof compiler, a->bin, "physimc" EXE_EXT);
    const char *args[] = {builder, "--project", a->project, "--sdk", a->root,
                         "--output", a->build_directory, "--physimc", compiler,
                         "--profile", a->profile ? "Release" : "Debug", NULL};
    a->built = false;
    a->diagnostic_count = 0;
    a->diagnostic_used = 0;
    a->build_revision = a->source_revision;
    if (ps_process_start(&a->job, args, a->project)) {
        a->profile_job_recorded = false;
        a->job_kind = 2;
        status(a,a->analysis_only?"Physim baut die Analyse ...":"Physim baut Experiment und Analyse ...");
    } else
        status(a, "Physim-Build konnte nicht gestartet werden. Installation prüfen.");
}
static bool command(app *a, uint32_t type) {
    unsigned char frame[32], payload[4];
    uint32_t n = 0;
    if (type == PS_MSG_HELLO) {
        ps_put_u32(payload, PS_ABI_VERSION);
        n = 4;
    }
    size_t size = ps_wire_encode(frame, type, a->command_seq++, payload, n);
    return ps_process_write(&a->runner, frame, size);
}
static bool select_simulation_speed(app *a, double speed) {
    if (!a->loaded || !ps_speed_valid(speed) || a->reset_pending || a->reset_starting ||
        (a->runner.running && (!a->hello || a->stop_at)))
        return false;
    if (a->simulation_speed == speed)
        return true;
    if (a->runner.running) {
        unsigned char frame[28], payload[8];
        ps_put_f64(payload, speed);
        size_t size = ps_wire_encode(frame, PS_MSG_SPEED, a->command_seq++, payload, 8);
        if (!ps_process_write(&a->runner, frame, size)) {
            status(a, "Geschwindigkeit konnte nicht geändert werden.");
            return false;
        }
    }
    a->simulation_speed = speed;
    a->project_settings_dirty = true;
    return true;
}
static void unique_path(app *a, char *out, size_t cap, const char *suffix) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char stamp[64];
    strftime(stamp, sizeof stamp, "%Y%m%d-%H%M%S", t);
    snprintf(out, cap, "%s/runs/%s-%llu%s", a->project, stamp, (unsigned long long)SDL_GetTicksNS(),
             suffix);
}
static bool record_limits(const char *prefix, const ps_process_limits *limits) {
    char path[4200];
    int n = snprintf(path, sizeof path, "%s.limits.txt", prefix);
    if (n < 0 || (size_t)n >= sizeof path)
        return false;
    FILE *file = fopen(path, "wbx");
    if (!file)
        return false;
    bool ok = fprintf(file, "physim_limits=1\nrequested_memory_bytes=%llu\nwall_seconds=%.17g\n"
                            "wall_includes_pause=1\nzero_disables_limit=1\n",
                      (unsigned long long)limits->memory_bytes, limits->wall_seconds) >= 0;
    if (fclose(file))
        ok = false;
    if (!ok)
        remove(path);
    return ok;
}
static bool discover_parameters(app *a) {
    char runner[4096], module[4096];
    join(runner, sizeof runner, a->bin, "physim-runner" EXE_EXT);
    join(module, sizeof module, a->build_directory, "experiment" MODULE_EXT);
    const char *args[] = {runner, module, "--describe", NULL};
    ps_process_limits limits = {(uint64_t)a->runner_memory_mib * UINT64_C(1048576), 30};
    a->parameter_output[0] = 0;
    a->parameter_output_used = 0;
    a->parameter_output_overflow = false;
    if (!ps_process_start_limited(&a->job, args, a->project, &limits))
        return false;
    a->profile_job_recorded = false;
    a->job_kind = 5;
    status(a, "Build erfolgreich. Experimentparameter werden gelesen ...");
    return true;
}
static void begin_run_view(app *a) {
    timeline_clear(a);
    a->selected_count = 1;
    snprintf(a->selected_runs[0], sizeof a->selected_runs[0], "%s", a->last_run);
    ps_report_destroy(a->analysis_report);
    a->analysis_report = NULL;
    a->show_report = false;
    a->result_error[0] = 0;
    a->history_count = 0;
    a->history_seen = 0;
    a->history_stride = 1;
    a->scene_view = (ps_scene_view){0};
    a->scene_selected = false;
}
static bool start_run_mode(app *a, bool paused) {
    if(a->analysis_only){status(a,"Analyseprojekte werten gespeicherte Messläufe aus. Zur Auswertung wechseln.");return false;}
    if (a->recovery)
        return false;
    if (!simulation_settings_valid(a)) return false;
    if (a->project_settings_dirty)
        save_project(a);
    if (a->project_settings_dirty)
        return false;
    if (!a->built || !idle(a) || a->dirty) {
        status(a, "Zuerst das gespeicherte Projekt erfolgreich bauen.");
        return false;
    }
    char runner[4096], module[4096], dt[64], speed[64], src[4096], copy[4096], runs[4096], next_run[4096];
    join(runs, sizeof runs, a->project, "runs");
    if (!ps_make_directory(runs)) {
        status(a, "Laufordner konnte nicht angelegt werden.");
        return false;
    }
    join(runner, sizeof runner, a->bin, "physim-runner" EXE_EXT);
    join(module, sizeof module, a->build_directory, "experiment" MODULE_EXT);
    unique_path(a, next_run, sizeof next_run, ".psrun");
    snprintf(dt, sizeof dt, "%.17g", a->dt);
    snprintf(speed, sizeof speed, "%.17g", a->simulation_speed);
    char minimum[64],maximum[64];
    snprintf(minimum,sizeof minimum,"%.17g",a->minimum_dt);
    snprintf(maximum,sizeof maximum,"%.17g",a->maximum_dt);
    char parameter_arguments[PS_MAX_PARAMETERS][128];
    const char *args[18 + 2 * PS_MAX_PARAMETERS] = {
        runner, module, next_run, "--interactive", "--log-events", "--diagnostics", "--dt", dt, "--seed", a->seed,
        "--speed", speed};
    size_t argument_count = 12;
    if(a->adaptive_steps) {
        args[argument_count++]="--adaptive";
        args[argument_count++]="--min-dt";args[argument_count++]=minimum;
        args[argument_count++]="--max-dt";args[argument_count++]=maximum;
    }
    for (uint32_t i = 0; i < a->parameters.count; i++) {
        double selected;
        if (ps_parameter_catalog_value(&a->parameters, i, &selected) != PS_OK) {
            char message[160];
            snprintf(message, sizeof message, "Parameter %s: endlichen Wert innerhalb der Grenzen eingeben.",
                     a->parameters.entries[i].name);
            status(a, message);
            return false;
        }
        snprintf(parameter_arguments[i], sizeof parameter_arguments[i], "%s=%.17g",
                 a->parameters.entries[i].name, selected);
        args[argument_count++] = "--param";
        args[argument_count++] = parameter_arguments[i];
    }
    args[argument_count] = NULL;
    join(src, sizeof src, a->project, experiment_source(a));
    snprintf(copy, sizeof copy, "%s.experiment.%s", next_run,
             a->language_experiment ? "phys" : "c");
    if (!copy_file_exclusive(src, copy)) {
        status(a, "Quellcode-Snapshot konnte nicht gespeichert werden.");
        return false;
    }
    ps_process_limits limits = {(uint64_t)a->runner_memory_mib * UINT64_C(1048576),
                                a->runner_wall_seconds};
    if (!record_limits(next_run, &limits)) {
        status(a, "Laufgrenzen konnten nicht archiviert werden.");
        return false;
    }
    if (ps_process_start_limited(&a->runner, args, a->project, &limits)) {
        a->profile_runner_recorded = false;
        snprintf(a->reset_previous_run, sizeof a->reset_previous_run, "%s", a->last_run);
        snprintf(a->last_run, sizeof a->last_run, "%s", next_run);
        a->start_paused = a->reset_starting = paused;
        /* A reset keeps the displayed state until a valid initial snapshot arrives. */
        if (!paused) {
            begin_run_view(a);
            a->channel_count = 0;
            memset(a->values, 0, sizeof a->values);
            a->simulation_time = 0;
            a->scene.count = 0;
        }
        memset(&a->wire, 0, sizeof a->wire);
        a->command_seq = 0;
        a->paused = true;
        a->hello = false;
        a->heartbeat = ps_clock();
        a->stop_at = 0;
        a->tab = 1;
        status(a, "Runner gestartet; Versionsabgleich ...");
        return true;
    } else
        status(a, "Runner konnte nicht gestartet werden.");
    return false;
}
static void start_run(app *a) { (void)start_run_mode(a, false); }
/* A reset owns the stop/reap/load/relaunch sequence. Other actions remain idle-gated. */
static bool reset_available(app *a) {
    return a->loaded && !a->analysis_only && a->built && build_inputs_current(a) && !a->recovery &&
           !a->reset_pending && !a->reset_starting && !a->job.running && !a->data.thread && !a->report_thread &&
           !a->batch_thread && (!a->runner.running || (a->hello && !a->stop_at));
}
static void reset_run(app *a) {
    if (!reset_available(a))
        return;
    if (a->runner.running) {
        if (!command(a, PS_MSG_STOP)) {
            status(a, "Zurücksetzen fehlgeschlagen: Runner konnte nicht gestoppt werden.");
            return;
        }
        a->stop_at = ps_clock();
    }
    a->reset_pending = true;
    status(a, "Simulation wird zurückgesetzt. Der bisherige Lauf bleibt gespeichert.");
}
static int load_dataset(void *user) {
    dataset *d = user;
    ps_run_reader r;
    d->result = ps_run_open(&r, d->path);
    if (d->result != PS_OK) {
        SDL_SetAtomicInt(&d->done, 1);
        return 0;
    }
    d->channel_count = r.channels;
    memcpy(d->channels, r.schema, sizeof d->channels);
    snprintf(d->metadata, sizeof d->metadata, "%s", r.metadata);
    int mask[PS_MAX_CHANNELS];
    for (uint32_t j = 0; j < r.channels; j++) {
        d->result = ps_channel_status_index(r.schema, r.channels, j, &mask[j]);
        if (d->result != PS_OK) {
            ps_run_reader_close(&r);
            SDL_SetAtomicInt(&d->done, 1);
            return 0;
        }
    }
    double t, v[PS_MAX_CHANNELS];
    uint64_t stride = 1, seen = 0;
    long records_begin = ftell(r.file);
    d->timeline_result = PS_OK;
    while ((d->result = ps_run_next(&r, &t, v)) == PS_OK) {
        bool invalid = false;
        for (uint32_t j = 0; j < r.channels; j++) {
            int s = mask[j];
            if (s >= 0 && v[s] != 0 && v[s] != 1 && v[s] != 2)
                invalid = true;
            if (s < 0 || v[s] == 1)
                ps_statistics_push(&d->stats[j], v[j]);
        }
        if (invalid) {
            d->result = PS_CORRUPT;
            break;
        }
        if (d->timeline_result == PS_OK) {
            ps_snapshot frame = {.time = t, .count = r.channels, .paused = true};
            memcpy(frame.values, v, r.channels * sizeof *v);
            d->timeline_result = ps_timeline_push(&d->timeline, &frame);
        }
        if (seen % stride == 0) {
            if (d->count == PREVIEW) {
                for (int i = 0; i < PREVIEW / 2; i++) {
                    d->time[i] = d->time[i * 2];
                    for (uint32_t j = 0; j < r.channels; j++)
                        d->values[j][i] = d->values[j][i * 2];
                }
                d->count /= 2;
                stride *= 2;
            }
            d->time[d->count] = t;
            for (uint32_t j = 0; j < r.channels; j++)
                d->values[j][d->count] = mask[j] < 0 || v[mask[j]] == 1 ? v[j] : NAN;
            d->count++;
        }
        seen++;
    }
    d->total = seen;
    if (d->result == PS_EOF || d->result == PS_RECOVERED) {
        /* Keep the same open file: a renamed/replaced path must not mix two runs. */
        if (records_begin >= 0 && !fseek(r.file, records_begin, SEEK_SET)) {
            r.samples = 0;
            r.complete = false;
            ps_snapshot frame;
            ps_result next;
            while ((next = ps_run_snapshot_next(&r, &frame)) == PS_OK) {
                if (!d->recorded_scenes) {
                    ps_timeline_clear(&d->timeline);
                    d->recorded_scenes = true;
                }
                next = ps_timeline_push(&d->timeline, &frame);
                if (next != PS_OK) break;
            }
            if (d->recorded_scenes || d->timeline_result == PS_OK) d->timeline_result = next;
        } else d->timeline_result = PS_IO;
    }
    ps_run_reader_close(&r);
    SDL_SetAtomicInt(&d->done, 1);
    return 0;
}
static void request_dataset(app *a) {
    if (a->data.thread || !a->last_run[0])
        return;
    if (strcmp(a->plot_data_path, a->last_run)) {
        memset(a->data_views, 0, sizeof a->data_views);
        snprintf(a->plot_data_path, sizeof a->plot_data_path, "%s", a->last_run);
        a->plot_drag = NULL;
    }
    ps_timeline_destroy(&a->data.timeline);
    memset(&a->data, 0, sizeof a->data);
    snprintf(a->data.path, sizeof a->data.path, "%s", a->last_run);
    SDL_SetAtomicInt(&a->data.done, 0);
    a->data.thread = SDL_CreateThread(load_dataset, "physim-dataset", &a->data);
    if (!a->data.thread)
        status(a, "Datensatz-Worker konnte nicht gestartet werden.");
}
static int load_report(void *user) {
    app *a = user;
    a->report_result = ps_report_load(a->result_path, &a->pending_report);
    SDL_SetAtomicInt(&a->report_done, 1);
    return 0;
}
static void request_report_path(app *a, const char *path) {
    if (a->report_thread)
        return;
    snprintf(a->result_path, sizeof a->result_path, "%s", path);
    a->result_error[0] = 0;
    if (!exists(a->result_path)) {
        snprintf(
            a->result_error, sizeof a->result_error,
            "Diese Analyse hat keinen App-Bericht erzeugt. Exportdateien liegen im runs-Ordner.");
        return;
    }
    SDL_SetAtomicInt(&a->report_done, 0);
    a->report_thread = SDL_CreateThread(load_report, "physim-report", a);
    if (!a->report_thread)
        snprintf(a->result_error, sizeof a->result_error, "Ergebnis konnte nicht geladen werden.");
}
static void request_report(app *a) {
    char path[4096];
    int n = snprintf(path, sizeof path, "%s.psreport", a->report);
    if (n < 0 || (size_t)n >= sizeof path) {
        status(a, "Berichtspfad ist zu lang.");
        return;
    }
    request_report_path(a, path);
}
// clang-format off
#include "library_actions.inc"
#include "batch_actions.inc"
// clang-format on
static void analyze(app *a, bool csv) {
    if (a->recovery)
        return;
    if (!csv && (!a->built || a->analysis_dirty)) {
        status(a, "Analysecode zuerst speichern und bauen.");
        return;
    }
    if ((!a->last_run[0] && (csv || (!a->selected_count && !a->analysis_only))) || !idle(a)) {
        status(a, "Zuerst einen Lauf stoppen und speichern.");
        return;
    }
    char runner[4096], module[4096], source[4096], snapshot_path[4096];
    join(runner, sizeof runner, a->bin, "physim-analysis-runner" EXE_EXT);
    join(module, sizeof module, a->build_directory, "analysis" MODULE_EXT);
    unique_path(a, a->report, sizeof a->report, csv ? ".csv" : "-analysis");
    unsigned count = !csv && a->selected_count ? a->selected_count : 1;
    const char *input = !csv && a->selected_count ? a->selected_runs[0] : a->last_run;
    const char *args[5 + PS_ANALYSIS_MAX_INPUTS] = {runner, csv ? "--csv" : module, input,
                                                    a->report, NULL};
    if(!csv && a->analysis_only && !a->selected_count && !a->last_run[0]) {
        count=0;args[2]="--runs";args[3]=a->report;args[4]=NULL;
    }
    else if (!csv && count > 1) {
        args[2] = "--runs";
        for (unsigned i = 0; i < count; i++)
            args[i + 4] = a->selected_runs[i];
        args[count + 4] = NULL;
    }
    if (!csv) {
        join(source, sizeof source, a->project, analysis_source(a));
        snprintf(snapshot_path, sizeof snapshot_path, "%s.source.%s", a->report,
                 a->language_analysis ? "phys" : "c");
        if (!copy_file_exclusive(source, snapshot_path)) {
            status(a, "Analyse-Quellcode konnte nicht archiviert werden.");
            return;
        }
    }
    ps_process_limits limits = {(uint64_t)a->runner_memory_mib * UINT64_C(1048576),
                                a->runner_wall_seconds};
    if (!record_limits(a->report, &limits)) {
        status(a, "Auswertungsgrenzen konnten nicht archiviert werden.");
        return;
    }
    if (ps_process_start_limited(&a->job, args, a->project, &limits)) {
        a->profile_job_recorded = false;
        if (!csv && strcmp(input, a->last_run)) {
            snprintf(a->last_run, sizeof a->last_run, "%s", input);
            request_dataset(a);
        }
        a->job_kind = csv ? 4 : 3;
        status(a, csv ? "CSV-Export laeuft ..." : "Analyse laeuft im separaten Prozess ...");
    } else
        status(a, "Analyse-Runner konnte nicht gestartet werden.");
}
static void synchronize_live_schema(app *a) {
    for (uint32_t j = 0; j < a->channel_count; j++) {
        a->channel_status[j] = -1;
        (void)ps_channel_status_index(a->live_channels, a->channel_count, j,
                                      &a->channel_status[j]);
    }
}
static void add_history(app *a) {
    if (a->history_count && a->simulation_time <= a->history_t[a->history_count - 1])
        return;
    if (a->history_seen++ % a->history_stride)
        return;
    if (a->history_count == PREVIEW) {
        for (int i = 0; i < PREVIEW / 2; i++) {
            a->history_t[i] = a->history_t[i * 2];
            for (uint32_t j = 0; j < a->channel_count; j++)
                a->history_v[j][i] = a->history_v[j][i * 2];
        }
        a->history_count /= 2;
        a->history_stride *= 2;
    }
    int at = a->history_count++;
    a->history_t[at] = a->simulation_time;
    for (uint32_t j = 0; j < a->channel_count; j++)
        a->history_v[j][at] =
            a->channel_status[j] < 0 || a->values[a->channel_status[j]] == 1 ? a->values[j] : NAN;
}
static int import_worker(void *user) {
    app *a=user;a->import_result=ps_run_import(a->import_source,a->import_destination);
    SDL_SetAtomicInt(&a->import_done,1);return 0;
}
static void import_run(app *a,const char *path) {
    if(!a->loaded || !idle(a) || a->library_thread || a->recovery){status(a,"Zuerst einen laufenden Job beenden und ein Projekt öffnen.");return;}
    if(!path || strlen(path)>=sizeof a->import_source){status(a,"Importpfad ist zu lang.");return;}
    snprintf(a->import_source,sizeof a->import_source,"%s",path);
    unique_path(a,a->import_destination,sizeof a->import_destination,"-import.psrun");
    SDL_SetAtomicInt(&a->import_done,0);a->import_thread=SDL_CreateThread(import_worker,"physim-import",a);
    status(a,a->import_thread?"Messlauf wird kopiert und geprüft …":"Import konnte nicht gestartet werden.");
}
static void profile_process_finished(app *a,ps_process *p,int kind,bool *recorded) {
    if (*recorded || p->running || !p->pid) return;
    *recorded = true;
    if (!a->profiler) return;
    ps_app_profile_process row = {0};
    row.process_id = (uint32_t)p->pid;
    row.time_seconds = ps_clock() - a->profile_started_at;
    row.kind = kind; row.exit_code = p->exit_code; row.timed_out = p->timed_out;
    row.scope = p->usage_scope;
    row.available = ps_process_usage_final(p, &row.usage, &row.scope);
    ps_app_profile_record_process(a->profiler, &row);
}
static void pump(app *a) {
    if(a->import_thread && SDL_GetAtomicInt(&a->import_done)) {
        SDL_WaitThread(a->import_thread,NULL);a->import_thread=NULL;
        if(a->import_result==PS_OK || a->import_result==PS_RECOVERED) {
            snprintf(a->last_run,sizeof a->last_run,"%s",a->import_destination);
            a->selected_count=0;select_run(a,a->last_run,true);refresh_workspace_entries(a);refresh_library(a);
            a->tab=2;a->analysis_tab=0;a->show_report=false;request_dataset(a);
            status(a,a->import_result==PS_RECOVERED?"Unvollständiger Messlauf importiert; gültiger Präfix bleibt erhalten.":"Messlauf und vorhandene Quellsnapshots importiert.");
        } else {char message[160];snprintf(message,sizeof message,"Import fehlgeschlagen (%s). Vorhandene Daten bleiben erhalten.",ps_result_string(a->import_result));status(a,message);}
    }
    pump_batch(a);
    pump_library(a);
    if (a->report_thread && SDL_GetAtomicInt(&a->report_done)) {
        SDL_WaitThread(a->report_thread, NULL);
        a->report_thread = NULL;
        if (a->report_result == PS_OK) {
            ps_report_destroy(a->analysis_report);
            a->analysis_report = a->pending_report;
            a->pending_report = NULL;
            a->result_view = 0;
            memset(a->report_views, 0, sizeof a->report_views);
            a->plot_drag = NULL;
            a->show_report = true;
            snprintf(a->loaded_report_path, sizeof a->loaded_report_path, "%s", a->result_path);
            status(a, "Analyseergebnis geladen. Diagramme und Tabellen sind bereit.");
        } else {
            snprintf(a->result_error, sizeof a->result_error,
                     "Bericht abgewiesen: %s. Ein vorheriges Ergebnis bleibt erhalten.",
                     ps_result_string(a->report_result));
            status(a, a->result_error);
        }
    }
    if (a->job.running) {
        char buf[4096];
        int n;
        for (int i = 0; i < 16 && (n = ps_process_read(&a->job, buf, sizeof buf - 1)) > 0; i++) {
            if (a->profiler) a->profile_frame.job_bytes += (uint64_t)n;
            buf[n] = 0;
            if (a->job_kind == 5) {
                if ((size_t)n < sizeof a->parameter_output - a->parameter_output_used) {
                    memcpy(a->parameter_output + a->parameter_output_used, buf, (size_t)n + 1);
                    a->parameter_output_used += (size_t)n;
                } else
                    a->parameter_output_overflow = true;
            }
            if (a->job_kind == 1 || a->job_kind == 2 || a->job_kind == 3)
                diagnostic_bytes(a, buf, (size_t)n);
            log_line(a, "%s", buf);
        }
        if (!ps_process_poll(&a->job)) {
            profile_process_finished(a, &a->job, a->job_kind, &a->profile_job_recorded);
            int code = a->job.exit_code;
            ps_process_close(&a->job);
            if(code && (a->job_kind==3 || a->job_kind==4)) {
                char path[4096];snprintf(path,sizeof path,"%s.psdiag",a->report);ps_diagnostic record;
                if(ps_diagnostic_load(path,&record)==PS_OK)structured_diagnostic(a,&record,true);
            }
            if (!code && a->job_kind == 2) {
                if (!a->diagnostic_count)
                    a->show_log = false;
                if (!build_inputs_current(a))
                    status(a, "Build fertig; Editor wurde waehrenddessen geaendert. Erneut bauen.");
                else if(a->analysis_only) {
                    a->built=true;status(a,"Analyse erfolgreich gebaut. Gespeicherte Läufe können ausgewertet werden.");
                }
                else if (!discover_parameters(a))
                    status(a, "Build fertig; Parameterabfrage konnte nicht gestartet werden.");
            } else if (a->job_kind == 5) {
                a->built = !code && !a->job.timed_out && !a->parameter_output_overflow &&
                           ps_parameter_catalog_parse(&a->parameters, a->parameter_output) &&
                           build_inputs_current(a);
                if (a->built)
                    a->batch_sweep_ready = false;
                status(a, a->built ? "Build und Parameterabfrage erfolgreich. Experiment bereit."
                          : !build_inputs_current(a)
                              ? "Dateien oder Buildprofil wurden während des Builds geändert. Erneut bauen."
                              : "Parameterabfrage fehlgeschlagen. Details im Protokoll; erneut bauen.");
            } else if (!code && (a->job_kind == 3 || a->job_kind == 4)) {
                refresh_library(a);
                status(a, "Auswertung abgeschlossen; Ergebnisse im runs-Ordner.");
                if (a->job_kind == 3) {
                    request_dataset(a);
                    request_report(a);
                }
            } else if (a->job.timed_out) {
                status(a, "Auswertung nach Zeitlimit beendet. Vorhandene Ergebnisse bleiben erhalten.");
            } else {
                char message[128];
                snprintf(message, sizeof message, "Job beendet (Exit %d). Details im Protokoll.",
                         code);
                status(a, message);
            }
        }
    }
    if (a->runner.running) {
        int got = ps_process_read(&a->runner, a->wire.data + a->wire.used,
                                  sizeof a->wire.data - a->wire.used);
        if (got > 0) {
            a->wire.used += (size_t)got;
            if (a->profiler) a->profile_frame.runner_bytes += (uint64_t)got;
        }
        uint32_t type, n;
        const unsigned char *p;
        int r = 0;
        while ((r = ps_wire_peek(&a->wire, &type, &p, &n)) > 0) {
            a->heartbeat = ps_clock();
            if (type == PS_MSG_HELLO && !a->hello) {
                char hello[8193];
                memcpy(hello, p, n);
                hello[n] = 0;
                char *line = strchr(hello, '\n');
                uint32_t count = 0;
                char names[PS_MAX_CHANNELS][96] = {{0}};
                if (line) {
                    line++;
                    while (*line && count < PS_MAX_CHANNELS) {
                        char *end = strchr(line, '\n');
                        if (!end)
                            break;
                        *end = 0;
                        snprintf(names[count++], 96, "%s", line);
                        line = end + 1;
                    }
                }
                ps_run_reader schema_reader;
                if(ps_run_open(&schema_reader,a->last_run)!=PS_OK) {r=-1;break;}
                bool valid_schema=schema_reader.channels==count;
                for(uint32_t i=0;i<count && valid_schema;i++) {
                    char label[96];snprintf(label,sizeof label,"%s [%s]",schema_reader.schema[i].name,schema_reader.schema[i].unit);
                    valid_schema=!strcmp(label,names[i]);
                }
                if(valid_schema) {
                    memcpy(a->reset_starting?a->reset_channels:a->live_channels,schema_reader.schema,sizeof a->live_channels);
                    memcpy(a->reset_starting?a->reset_channel_names:a->channel_names,names,sizeof names);
                }
                ps_run_reader_close(&schema_reader);
                if(!valid_schema){r=-1;break;}
                if (a->reset_starting)
                    a->reset_channel_count = count;
                else {
                    a->channel_count = count;
                    synchronize_live_schema(a);
                }
                a->hello = true;
                command(a, PS_MSG_HELLO);
                if (!a->start_paused)
                    command(a, PS_MSG_RUN);
                status(a, a->start_paused
                              ? "Runner bereit. Anfangszustand wird geladen."
                              : "Simulation laeuft. Messwerte werden fortlaufend gespeichert.");
            } else if (type == PS_MSG_SNAPSHOT) {
                bool was_paused = a->paused, paused;
                double time, values[PS_MAX_CHANNELS];
                uint32_t count;
                ps_scene scene;
                if (!ps_snapshot_decode(p, n, &time, values, &count, &scene, &paused) ||
                    (a->reset_starting && (time != 0 || !paused || count != a->reset_channel_count))) {
                    r = -1;
                    break;
                }
                a->simulation_time = time;
                memcpy(a->values, values, count * sizeof *values);
                a->channel_count = count;
                a->scene = scene;
                a->paused = paused;
                if (a->reset_starting) {
                    memcpy(a->channel_names, a->reset_channel_names, sizeof a->channel_names);
                    memcpy(a->live_channels,a->reset_channels,sizeof a->live_channels);
                    synchronize_live_schema(a);
                    begin_run_view(a);
                    a->reset_starting = false;
                    a->reset_previous_run[0] = 0;
                    status(a, "Zurückgesetzt. Anfangszustand pausiert; Einzelschritt oder Fortsetzen wählen.");
                } else if (was_paused != a->paused)
                    status(a, a->paused
                                  ? "Simulation pausiert. Einzelschritt oder Fortsetzen wählen."
                                  : "Simulation läuft. Messwerte werden gespeichert.");
                ps_snapshot frame = {.time = time, .count = count, .scene = scene, .paused = paused};
                memcpy(frame.values, values, count * sizeof *values);
                ps_result timeline_result = ps_timeline_push(&a->timeline, &frame);
                a->timeline_scenes = true;
                if (timeline_result != PS_OK)
                    snprintf(a->timeline_error, sizeof a->timeline_error,
                             "Zeitleiste: %s", ps_result_string(timeline_result));
                if (!a->timeline_browsing) ps_scene_view_sync(&a->scene_view, &a->scene);
                add_history(a);
            } else if(type==PS_MSG_LOG) {
                ps_log_record record;
                if(!ps_wire_log_decode(p,n,&record)){r=-1;break;}
                log_line(a,"Experiment [%s, t=%.17g s]: %s",ps_log_level_name(record.level),record.time_s,record.message);
            } else if(type==PS_MSG_DIAGNOSTIC) {
                ps_diagnostic record;if(ps_diagnostic_decode(p,n,&record)!=PS_OK){r=-1;break;}
                structured_diagnostic(a,&record,false);char text[PS_DIAGNOSTIC_WIRE_MAX+256];
                (void)ps_diagnostic_format(&record,text,sizeof text);log_line(a,"Runner: %s",text);
            } else if (type == PS_MSG_ERROR) {
                char error[8193];
                memcpy(error, p, n);
                error[n] = 0;
                parse_diagnostic(a, error);
                log_line(a, "Runner: %s", error);
            } else if (type != PS_MSG_HEARTBEAT && type != PS_MSG_BYE) {
                r = -1;
                break;
            }
            ps_wire_consume(&a->wire, n);
        }
        if (r < 0) {
            ps_process_kill(&a->runner);
            status(a, "Ungueltige Runner-Nachricht; Prozess beendet.");
        }
        if (a->stop_at && ps_clock() - a->stop_at > 1) {
            ps_process_kill(&a->runner);
            status(a,
                   "Runner nach Stop-Timeout beendet. Vollstaendige Messbloecke bleiben lesbar.");
        }
        if (!ps_process_poll(&a->runner)) {
            profile_process_finished(a, &a->runner, 0, &a->profile_runner_recorded);
            int code = a->runner.exit_code;
            ps_process_close(&a->runner);
            a->paused = true;
            a->hello = false;
            char message[200];
            bool failed_reset = a->reset_starting;
            if (failed_reset) {
                snprintf(a->last_run, sizeof a->last_run, "%s", a->reset_previous_run);
                a->reset_previous_run[0] = 0;
                a->reset_starting = false;
                snprintf(message, sizeof message,
                         "Zurücksetzen fehlgeschlagen (Exit %d). Der bisherige Zustand bleibt erhalten.", code);
            } else if (a->runner.timed_out)
                snprintf(message, sizeof message,
                         "Runner nach Zeitlimit beendet. Vollständige Messblöcke bleiben lesbar.");
            else
                snprintf(message, sizeof message,
                         "Runner beendet (Exit %d). Lauf gespeichert bzw. bis zum letzten Block "
                         "rekonstruierbar.", code);
            status(a, message);
            if (!failed_reset)
                request_dataset(a);
            refresh_library(a);
        }
    }
    if (a->data.thread && SDL_GetAtomicInt(&a->data.done)) {
        SDL_WaitThread(a->data.thread, NULL);
        a->data.thread = NULL;
        if (a->data.result == PS_EOF || a->data.result == PS_RECOVERED) {
            ps_timeline_destroy(&a->timeline);
            a->timeline = a->data.timeline;
            memset(&a->data.timeline, 0, sizeof a->data.timeline);
            a->timeline_scenes = a->data.recorded_scenes;
            if (a->data.timeline_result != PS_EOF && a->data.timeline_result != PS_RECOVERED)
                snprintf(a->timeline_error, sizeof a->timeline_error, "Zeitleiste: %s",
                         ps_result_string(a->data.timeline_result));
            else a->timeline_error[0] = 0;
            if (a->timeline.seen) {
                const ps_snapshot *latest = &a->timeline.latest;
                a->simulation_time = latest->time;
                memcpy(a->values, latest->values, sizeof a->values);
                a->channel_count = a->data.channel_count;
                memcpy(a->live_channels,a->data.channels,sizeof a->live_channels);
                a->scene = latest->scene;
                for (uint32_t j = 0; j < a->channel_count; j++)
                    snprintf(a->channel_names[j], sizeof a->channel_names[j], "%s [%s]",
                             a->data.channels[j].name, a->data.channels[j].unit);
                synchronize_live_schema(a);
                if (!a->timeline_browsing) ps_scene_view_sync(&a->scene_view, &a->scene);
            }
            a->history_count = a->data.count;
            memcpy(a->history_t, a->data.time, sizeof a->history_t);
            memcpy(a->history_v, a->data.values, sizeof a->history_v);
            char message[128];
            snprintf(message, sizeof message, "%llu Messpunkte geladen%s.",
                     (unsigned long long)a->data.total,
                     a->data.result == PS_RECOVERED ? " (Lauf rekonstruiert)" : "");
            status(a, message);
        } else {
            a->data.count = 0;
            status(a, "Datensatz konnte nicht vollstaendig gelesen werden.");
        }
    }
    if (a->reset_pending && !a->quitting && jobs_idle(a)) {
        a->reset_pending = false;
        (void)start_run_mode(a, true);
    }
    timeline_tick(a);
}
static bool place_label(struct nk_rect *label, struct nk_rect viewport,
                        const struct nk_rect *placed, unsigned count) {
    if (viewport.h < label->h || viewport.w <= 0)
        return false;
    label->w = fminf(label->w, viewport.w);
    label->x = fmaxf(viewport.x, fminf(label->x, viewport.x + viewport.w - label->w));
    float preferred = label->y;
    for (unsigned attempt = 0; attempt <= 2 * count + 2; attempt++) {
        int row = attempt ? (int)((attempt + 1) / 2) * (attempt % 2 ? -1 : 1) : 0;
        label->y = preferred + (float)row * (label->h + 3);
        if (label->y < viewport.y || label->y + label->h > viewport.y + viewport.h)
            continue;
        bool overlap = false;
        for (unsigned j = 0; j < count; j++) {
            struct nk_rect p = placed[j];
            if (label->x < p.x + p.w + 2 && label->x + label->w + 2 > p.x &&
                label->y < p.y + p.h + 2 && label->y + label->h + 2 > p.y) {
                overlap = true;
                break;
            }
        }
        if (!overlap)
            return true;
    }
    return false;
}
static void scene_selection_sync(app *a) {
    if (a->scene_selected && a->scene_selected_id) {
        a->scene_selected = false;
        for (uint32_t i = 0; i < display_scene(a)->count; i++)
            if (display_scene(a)->objects[i].id == a->scene_selected_id) {
                a->scene_selected = true;
                a->scene_selected_slot = i;
                break;
            }
    }
    if (a->scene_selected_slot >= display_scene(a)->count) a->scene_selected = false;
}
static bool scene_entry_enabled(const app *a, uint32_t i) {
    const ps_object *o = &display_scene(a)->objects[i];
    return o->shape!=PS_GROUP && o->shape!=PS_FRAME && ps_scene_view_visible(&a->scene_view, i) && (o->color & 255) &&
           (o->shape != PS_ARROW || a->show_vectors) &&
           (o->shape != PS_POLYLINE || a->show_paths) &&
           (o->shape != PS_POINT || a->show_points) &&
           (o->shape != PS_LABEL || (a->show_labels && o->text[0]));
}
static void scene_select_entry(app *a,uint32_t slot) {
    const ps_scene *scene=display_scene(a);
    a->scene_selected=slot<scene->count;
    if(!a->scene_selected) return;
    a->scene_selected_slot=slot;a->scene_selected_id=scene->objects[slot].id;
    int parent=ps_scene_parent_index(scene,slot);
    for(unsigned depth=0;parent>=0 && depth<PS_MAX_OBJECTS;depth++) {
        a->scene_view.collapsed &= ~(UINT32_C(1)<<(unsigned)parent);
        parent=ps_scene_parent_index(scene,(uint32_t)parent);
    }
}
static bool scene_shortcut(app *a, const SDL_KeyboardEvent *key) {
    if (a->tab != 1 || a->recovery || !(key->mod & SDL_KMOD_ALT) ||
        (key->mod & (SDL_KMOD_CTRL | SDL_KMOD_GUI)))
        return false;
    bool shift = (key->mod & SDL_KMOD_SHIFT) != 0;
    float dx = key->key == SDLK_RIGHT ? 1.f : key->key == SDLK_LEFT ? -1.f : 0.f;
    float dy = key->key == SDLK_UP ? 1.f : key->key == SDLK_DOWN ? -1.f : 0.f;
    if (dx || dy) {
        if (shift) {
            ps_vec3 right = ps_v3(cos(a->yaw), 0, -sin(a->yaw));
            ps_vec3 up = ps_v3(-sin(a->yaw) * sin(a->pitch), cos(a->pitch),
                               -cos(a->yaw) * sin(a->pitch));
            a->camera_target = ps_vadd(a->camera_target,
                ps_vscale(ps_vadd(ps_vscale(right, dx), ps_vscale(up, dy)), a->zoom * .025));
        } else {
            a->yaw = remainderf(a->yaw + dx * .08f, 2 * (float)PS_PI);
            a->pitch = fmaxf(-(float)PS_PI / 2,
                             fminf((float)PS_PI / 2, a->pitch + dy * .08f));
        }
        return true;
    }
    if (!shift && (key->key == SDLK_PAGEUP || key->key == SDLK_PAGEDOWN)) {
        a->zoom = fmaxf(1.5f, fminf(12, a->zoom * (key->key == SDLK_PAGEUP ? .9f : 1 / .9f)));
        return true;
    }
    if (shift) return false;
    if (key->key == SDLK_HOME) {
        if (!key->repeat) {
            a->yaw = PS_CAMERA_DEFAULT.yaw;
            a->pitch = PS_CAMERA_DEFAULT.pitch;
            a->zoom = PS_CAMERA_DEFAULT.distance;
            a->camera_target = PS_CAMERA_DEFAULT.target;
            a->orthographic = PS_CAMERA_DEFAULT.orthographic;
        }
        return true;
    }
    if (key->key != SDLK_N && key->key != SDLK_P && key->key != SDLK_H)
        return false;
    if (key->repeat) return true;
    ps_scene_view_sync(&a->scene_view, display_scene(a));
    scene_selection_sync(a);
    if (key->key == SDLK_H) {
        if (a->scene_selected) {
            a->scene_view.hidden |= UINT32_C(1) << a->scene_selected_slot;
            a->scene_selected = false;
        }
        return true;
    }
    int direction = key->key == SDLK_N ? 1 : -1;
    int index = a->scene_selected ? (int)a->scene_selected_slot : direction > 0 ? -1 : 0;
    a->scene_selected = false;
    for (uint32_t step = 0; step < display_scene(a)->count; step++) {
        index = (index + direction + (int)display_scene(a)->count) % (int)display_scene(a)->count;
        if (scene_entry_enabled(a, (uint32_t)index) ||
            ((display_scene(a)->objects[index].shape==PS_GROUP || display_scene(a)->objects[index].shape==PS_FRAME) && ps_scene_view_visible(&a->scene_view,(uint32_t)index))) {
            scene_select_entry(a,(uint32_t)index);
            break;
        }
    }
    return true;
}
static void viewport(app *a, float height) {
    memset(a->scene_label_bounds,0,sizeof a->scene_label_bounds);
    struct nk_context *ui = a->ui;
    nk_layout_row_dynamic(ui, height, 1);
    struct nk_rect r;
    enum nk_widget_layout_states widget_state = nk_widget(&r, ui);
    if(ui->current->layout->flags & NK_WINDOW_ROM) widget_state=NK_WIDGET_ROM;
    a->scene_viewport_bounds = r;
    struct nk_input *input = &ui->input;
    if (widget_state == NK_WIDGET_VALID && nk_input_is_mouse_hovering_rect(input, r)) {
        if (nk_input_is_mouse_down(input, NK_BUTTON_RIGHT)) {
            a->yaw -= input->mouse.delta.x * .008f;
            a->pitch += input->mouse.delta.y * .008f;
            a->pitch = fmaxf(-(float)PS_PI / 2, fminf((float)PS_PI / 2, a->pitch));
            a->yaw = remainderf(a->yaw, 2 * (float)PS_PI);
        }
        if (nk_input_is_mouse_down(input, NK_BUTTON_MIDDLE)) {
            double scale = a->zoom / fmaxf(r.h, 1);
            ps_vec3 right = ps_v3(cos(a->yaw), 0, -sin(a->yaw));
            ps_vec3 up =
                ps_v3(-sin(a->yaw) * sin(a->pitch), cos(a->pitch), -cos(a->yaw) * sin(a->pitch));
            a->camera_target =
                ps_vadd(a->camera_target, ps_vadd(ps_vscale(right, -input->mouse.delta.x * scale),
                                                  ps_vscale(up, input->mouse.delta.y * scale)));
        }
        a->zoom = fmaxf(1.5f, fminf(12, a->zoom * expf(-input->mouse.scroll_delta.y * .1f)));
    }
    ps_camera camera = {a->yaw,          a->pitch,        a->zoom, a->camera_target,
                        a->orthographic, a->show_vectors, a->show_grid};
    int w, h, pw, ph;
    SDL_GetWindowSize(a->window, &w, &h);
    SDL_GetWindowSizeInPixels(a->window, &pw, &ph);
    int width = (int)fmaxf(1, r.w * (float)pw / (float)(w ? w : 1));
    int pixels = (int)fmaxf(1, r.h * (float)ph / (float)(h ? h : 1));
    ps_scene visible = *display_scene(a);
    uint32_t visible_slots[PS_MAX_OBJECTS];
    ps_mat4 transforms[PS_MAX_OBJECTS];
    if(ps_scene_transforms(&visible,transforms)!=PS_OK)return;
    for (uint32_t i = 0; i < display_scene(a)->count; i++) {
        const ps_object *o = &display_scene(a)->objects[i];
        (void)o;
        visible_slots[i]=i;
        if(!scene_entry_enabled(a,i))visible.objects[i].color=0;
    }
    unsigned texture = ps_graphics_scene(a->graphics, &visible, &camera, width, pixels);
    if (texture) {
        if (a->profiler) a->profile_frame.has_scene = ps_graphics_scene_stats(a->graphics, &a->profile_frame.scene);
        bool picking = widget_state == NK_WIDGET_VALID && nk_input_is_mouse_hovering_rect(input, r) &&
                       nk_input_is_mouse_pressed(input, NK_BUTTON_LEFT);
        scene_selection_sync(a);
        if (picking) {
            int hit = ps_graphics_pick(a->graphics, (input->mouse.pos.x-r.x)/r.w,
                                       (input->mouse.pos.y-r.y)/r.h);
            a->scene_selected = hit >= 0;
            if (hit >= 0) {
                scene_select_entry(a,visible_slots[hit]);
            }
        }
        struct nk_image image = nk_image_id((int)texture);
        nk_draw_image(nk_window_get_canvas(ui), r, &image, nk_rgb(255, 255, 255));
        struct nk_command_buffer *canvas = nk_window_get_canvas(ui);
        struct nk_rect previous = canvas->clip;
        float left = fmaxf(previous.x, r.x), top = fmaxf(previous.y, r.y);
        float right = fminf(previous.x + previous.w, r.x + r.w),
              bottom = fminf(previous.y + previous.h, r.y + r.h);
        nk_push_scissor(canvas, nk_rect(left, top, fmaxf(0, right - left), fmaxf(0, bottom - top)));
        const struct nk_user_font *font = ui->style.font;
        struct nk_rect placed[PS_MAX_OBJECTS];
        unsigned placed_count = 0;
        for (uint32_t i = 0; i < visible.count; i++) {
            const ps_object *o = &visible.objects[i];
            float x, y;ps_vec3 anchor;
            if(ps_transform_point(transforms[i],o->a,&anchor)!=PS_OK)continue;
            if (o->shape != PS_LABEL || !o->text[0] || !(o->color & 255) ||
                !ps_graphics_project(&camera, anchor, (double)width / pixels, &x, &y))
                continue;
            int length = (int)strlen(o->text);
            float text_width = font->width(font->userdata, font->height, o->text, length);
            struct nk_rect label = nk_rect(r.x + x * r.w + 7, r.y + y * r.h - font->height - 4,
                                           text_width + 8, font->height + 4);
            float preferred_y = label.y;
            if (!place_label(&label, r, placed, placed_count))
                continue;
            placed[placed_count++] = label;
            a->scene_label_bounds[visible_slots[i]] = label;
            if (picking && nk_input_is_mouse_hovering_rect(input, label)) {
                scene_select_entry(a,visible_slots[i]);
            }
            if (fabsf(label.y - preferred_y) > 1)
                nk_stroke_line(canvas, r.x + x * r.w, r.y + y * r.h, label.x,
                               label.y + label.h * .5f, 1, nk_rgba(140, 164, 187, 160));
            nk_fill_rect(canvas, label, 3, nk_rgba(12, 22, 33, 220));
            label.x += 4;
            label.w -= 8;
            label.y += 2;
            uint32_t c = o->color;
            nk_draw_text(canvas, label, o->text, length, font, nk_rgba(0, 0, 0, 0),
                         nk_rgba((int)(c >> 24), (int)((c >> 16) & 255), (int)((c >> 8) & 255),
                                 (int)(c & 255)));
        }
        nk_push_scissor(canvas, previous);
        if (a->timeline_error[0])
            nk_draw_text(canvas, nk_rect(r.x + 10, r.y + 10, r.w - 20, 22), a->timeline_error,
                         (int)strlen(a->timeline_error), font, nk_rgba(0, 0, 0, 0), UI_COLOR(error));
        if (a->timeline.seen && !a->timeline_scenes) {
            const char *message = "Keine Szenenaufzeichnung · Zeitleiste zeigt Messwerte";
            nk_draw_text(canvas, nk_rect(r.x + 10, r.y + (a->timeline_error[0] ? 36 : 10), r.w - 20, 22), message,
                         (int)strlen(message), font, nk_rgba(0, 0, 0, 0), UI_COLOR(muted));
        }
    } else {
        nk_fill_rect(nk_window_get_canvas(ui), r, 0, nk_rgb(70, 20, 25));
        status(a, SDL_GetError());
    }
}
// clang-format off
#include "plot_ui.inc"
// clang-format on
static void plot(app *a, const double *times, const double *values, int count, float height,
                 const char *label, bool scatter, ps_plot_view *view) {
    struct nk_context *ui = a->ui;
    double lo = INFINITY, hi = -INFINITY;
    for (int i = 0; i < count; i++)
        if (isfinite(values[i])) {
            lo = fmin(lo, values[i]);
            hi = fmax(hi, values[i]);
        }
    bool have_values = isfinite(lo);
    if (have_values)
        plot_expand_range(&lo, &hi, true);
    double bounds[] = {count ? times[0] : 0, count ? times[count - 1] : 1, lo, hi};
    nk_layout_row_dynamic(ui, 22, 1);
    nk_label_colored(ui, label, NK_TEXT_LEFT, UI_COLOR(muted));
    if (view)
        plot_controls(a, view, have_values && count > 1 ? bounds : NULL);
    nk_layout_row_dynamic(ui, height, 1);
    struct nk_rect r;
    nk_widget(&r, ui);
    struct nk_command_buffer *canvas = nk_window_get_canvas(ui);
    nk_fill_rect(canvas, r, 6, UI_COLOR(edit));
    if (count < 2)
        return;
    if (!have_values) {
        const char *message = "Keine gültigen Messwerte in dieser Vorschau.";
        nk_draw_text(canvas, nk_rect(r.x + 12, r.y + 12, r.w - 24, 24), message,
                     (int)strlen(message), ui->style.font, UI_COLOR(edit),
                     UI_COLOR(muted));
        return;
    }
    if (!(times[count - 1] > times[0]))
        return;
    struct nk_rect area = nk_rect(r.x + 70, r.y + 20, r.w - 90, r.h - 55);
    ps_plot_view fit;
    ps_plot_view_reset(&fit);
    if (view)
        plot_interact(a, view, area, bounds);
    else
        view = &fit;
    double displayed[4];
    plot_display_bounds(view, bounds, displayed);
    double x_offset = ps_plot_axis_offset(displayed[0], displayed[1]);
    double y_offset = ps_plot_axis_offset(displayed[2], displayed[3]);
    if (x_offset || y_offset) {
        char note[192];
        snprintf(note, sizeof note, "Offset: Zeit %.17g s · Wert %.17g · Achsenwerte addieren",
                 x_offset, y_offset);
        nk_draw_text(canvas, nk_rect(r.x + 8, r.y + 1, r.w - 16, 18), note, (int)strlen(note),
                     ui->style.font, UI_COLOR(edit), UI_COLOR(muted));
    }
    int ticks = area.h < 80 ? 3 : 5;
    for (int i = 0; i < ticks; i++) {
        float y = area.y + area.h * (float)i / (float)(ticks - 1);
        nk_stroke_line(canvas, area.x, y, area.x + area.w, y, 1, UI_COLOR(plot_grid));
        char text[40];
        snprintf(text, sizeof text, "%.4g",
                 plot_tick(displayed[2] - y_offset, displayed[3] - y_offset,
                           1 - (double)i / (ticks - 1)));
        nk_draw_text(canvas, nk_rect(r.x + 4, y - 8, 62, 18), text, (int)strlen(text),
                     ui->style.font, UI_COLOR(edit), UI_COLOR(muted));
    }
    struct nk_rect previous_clip = canvas->clip;
    nk_push_scissor(canvas, plot_intersection(area, previous_clip));
    for (int i = 0; i < count; i++) {
        if (!isfinite(values[i]))
            continue;
        double x2 = plot_fraction(view, bounds, 0, times[i]),
               y2 = plot_fraction(view, bounds, 1, values[i]);
        if (scatter) {
            plot_dot(canvas, area, x2, y2, 2, UI_COLOR(accent));
            continue;
        }
        if (!i || !isfinite(values[i - 1]))
            continue;
        double x1 = plot_fraction(view, bounds, 0, times[i - 1]),
               y1 = plot_fraction(view, bounds, 1, values[i - 1]);
        plot_segment(canvas, area, x1, y1, x2, y2, UI_COLOR(accent));
    }
    if (a->tab == 1 && a->timeline_browsing) {
        double x = plot_fraction(view, bounds, 0, a->timeline_view.time);
        if (x >= 0 && x <= 1) {
            float position = area.x + (float)x * area.w;
            nk_stroke_line(canvas, position, area.y, position, area.y + area.h, 1,
                           UI_COLOR(accent));
        }
    }
    nk_push_scissor(canvas, previous_clip);
    for (int endpoint = 0; endpoint < 2; endpoint++) {
        char text[48];
        snprintf(text, sizeof text, "%.5g s", displayed[endpoint] - x_offset);
        float text_width = ui->style.font->width(ui->style.font->userdata, ui->style.font->height,
                                                 text, (int)strlen(text));
        float x = endpoint ? area.x + area.w - text_width : area.x;
        nk_draw_text(canvas, nk_rect(x, area.y + area.h + 10, text_width + 1, 22), text,
                     (int)strlen(text), ui->style.font, UI_COLOR(edit), UI_COLOR(muted));
    }
}
static void find_text(app *a, struct nk_text_edit *edit, bool replace) {
    const char *text = nk_str_get_const(&edit->string);
    int bytes = nk_str_len_char(&edit->string);
    if (!a->find[0] || !text)
        return;
    char *copy = malloc((size_t)bytes + 1);
    if (!copy)
        return;
    memcpy(copy, text, (size_t)bytes);
    copy[bytes] = 0;
    const char *start = copy;
    int len = 0;
    nk_rune rune;
    const char *cur = nk_str_at_const(&edit->string, edit->cursor, &rune, &len);
    if (cur)
        start = copy + (cur - text);
    char *found = strstr(start, a->find);
    if (!found)
        found = strstr(copy, a->find);
    if (found) {
        int at = nk_utf_len(copy, (int)(found - copy)),
            n = nk_utf_len(a->find, (int)strlen(a->find));
        edit->select_start = at;
        edit->select_end = at + n;
        edit->cursor = at + n;
        unsigned line = 0;
        for (const char *c = copy; c < found; c++)
            if (*c == '\n')
                line++;
        edit->scrollbar.y = (float)line * (a->font_code->height + a->ui->style.edit.row_padding);
        edit->scrollbar.x = 0;
        if (replace) {
            edit->mode = NK_TEXT_EDIT_MODE_INSERT;
            nk_textedit_delete_selection(edit);
            nk_textedit_text(edit, a->replace, (int)strlen(a->replace));
            if (edit == &a->experiment) {
                a->dirty = true;
                invalidate_build(a);
            } else if (edit == &a->analysis) {
                a->analysis_dirty = true;
                invalidate_build(a);
            } else document_changed(a, edit);
        }
    } else
        status(a, "Suchtext nicht gefunden.");
    free(copy);
}
static bool c_keyword(const char *word, size_t length) {
    static const char *keywords[] = {
        "auto",    "break",  "case",     "char",    "const",    "continue",      "default",
        "do",      "double", "else",     "enum",    "extern",   "float",         "for",
        "goto",    "if",     "inline",   "int",     "long",     "register",      "restrict",
        "return",  "short",  "signed",   "sizeof",  "static",   "struct",        "switch",
        "typedef", "union",  "unsigned", "void",    "volatile", "while",         "bool",
        "true",    "false",  "NULL",     "_Atomic", "_Bool",    "_Static_assert"};
    for (size_t i = 0; i < sizeof keywords / sizeof keywords[0]; i++)
        if (strlen(keywords[i]) == length && !memcmp(word, keywords[i], length))
            return true;
    return false;
}
static bool language_keyword(const char *word, size_t length) {
    static const char *keywords[] = {
        "break", "case", "continue", "default", "else", "enum", "false", "for", "func",
        "if", "import", "in", "let", "mutating", "nil", "return", "self", "static",
        "struct", "switch", "true", "var", "while", "Int64", "Float64", "Bool", "String",
        "Void", "Vec2", "Vec3", "Vec4", "Quat", "Unit", "Quantity", "Channel", "Dataset", "Series", "Plot",
        "Distribution", "SensorConfig", "Sensor", "Measurement", "Table", "Body",
        "Contacts", "ContactSolver", "ContactResult", "DistanceJoint", "JointResult", "Submersion",
        "ContactConstraint", "JointConstraint", "ConstraintResult", "Sweep", "Aabb", "CollisionPair", "Mat3", "Mat4", "Bezier3", "Optional"};
    for (size_t i = 0; i < sizeof keywords / sizeof keywords[0]; i++)
        if (strlen(keywords[i]) == length && !memcmp(word, keywords[i], length))
            return true;
    return false;
}
/* Overlay lexical colors on the ordinary Nuklear editor; selection and cursor stay native.
 * Only the visible lines generate draw commands; scanning preserves multiline comments. */
static void syntax_text(app *a, struct nk_text_edit *edit, struct nk_rect bounds) {
    int kind = document_language(a, edit);
    bool language = kind == 1;
    struct nk_context *ui = a->ui;
    const struct nk_user_font *font = ui->style.font;
    struct nk_command_buffer *canvas = nk_window_get_canvas(ui);
    struct nk_rect area =
        nk_rect(bounds.x + ui->style.edit.padding.x + ui->style.edit.border,
                bounds.y + ui->style.edit.padding.y + ui->style.edit.border,
                bounds.w - 2 * (ui->style.edit.padding.x + ui->style.edit.border) -
                    ui->style.edit.scrollbar_size.x,
                bounds.h - 2 * (ui->style.edit.padding.y + ui->style.edit.border));
    struct nk_rect previous_clip = canvas->clip;
    nk_push_scissor(canvas, area);
    const char *text = nk_str_get_const(&edit->string);
    size_t size = (size_t)nk_str_len_char(&edit->string), at = 0;
    float row = font->height + ui->style.edit.row_padding;
    float x = area.x - edit->scrollbar.x, y = area.y - edit->scrollbar.y;
    size_t block_depth = 0;
    bool triple_string = false;
    while (at < size && y < area.y + area.h) {
        if (text[at] == '\n') {
            at++;
            x = area.x - edit->scrollbar.x;
            y += row;
            continue;
        }
        if (text[at] == '\r') {
            at++;
            continue;
        }
        size_t begin = at;
        struct nk_color color = UI_COLOR(code_text);
        if (kind < 0) {
            nk_rune rune;
            int n = nk_utf_decode(text + at, &rune, (int)(size - at));
            at += (size_t)(n > 0 ? n : 1);
        } else if (triple_string ||
            (language && at + 2 < size && text[at] == '"' && text[at + 1] == '"' &&
             text[at + 2] == '"')) {
            color = UI_COLOR(code_string);
            if (!triple_string) {
                at += 3;
                triple_string = true;
            }
            while (at < size && text[at] != '\n' && text[at] != '\r') {
                if (text[at] == '\\' && at + 1 < size && text[at + 1] != '\n' &&
                    text[at + 1] != '\r') {
                    at += 2;
                } else if (at + 2 < size && text[at] == '"' && text[at + 1] == '"' &&
                           text[at + 2] == '"') {
                    at += 3;
                    triple_string = false;
                    break;
                } else {
                    at++;
                }
            }
        } else if (block_depth || (at + 1 < size && text[at] == '/' && text[at + 1] == '*')) {
            color = UI_COLOR(code_comment);
            if (!block_depth) {
                at += 2;
                block_depth = 1;
            }
            while (at < size && text[at] != '\n') {
                if (language && at + 1 < size && text[at] == '/' && text[at + 1] == '*') {
                    block_depth++;
                    at += 2;
                } else if (at + 1 < size && text[at] == '*' && text[at + 1] == '/') {
                    at += 2;
                    if (!--block_depth)
                        break;
                } else
                    at++;
            }
        } else if ((at + 1 < size && text[at] == '/' && text[at + 1] == '/') ||
                   (!language && text[at] == '#')) {
            color = text[at] == '#' ? UI_COLOR(code_preprocessor) : UI_COLOR(code_comment);
            while (at < size && text[at] != '\n' && text[at] != '\r')
                at++;
        } else if (text[at] == '"' || (!language && text[at] == '\'')) {
            color = UI_COLOR(code_string);
            char quote = text[at++];
            while (at < size && text[at] != '\n') {
                if (text[at] == '\\' && at + 1 < size && text[at + 1] != '\n') {
                    at += 2;
                    continue;
                }
                if (text[at++] == quote)
                    break;
            }
        } else if ((language && ps_lang_identifier_width(text + at, size - at, 1)) ||
                   (!language && (isalpha((unsigned char)text[at]) || text[at] == '_'))) {
            if (language) {
                at += ps_lang_identifier_width(text + at, size - at, 1);
                while (at < size) {
                    size_t width = ps_lang_identifier_width(text + at, size - at, 0);
                    if (!width)
                        break;
                    at += width;
                }
            } else {
                at++;
                while (at < size && (isalnum((unsigned char)text[at]) || text[at] == '_'))
                    at++;
            }
            if (language ? language_keyword(text + begin, at - begin)
                         : c_keyword(text + begin, at - begin))
                color = UI_COLOR(code_keyword);
            else if (at - begin >= 3 && text[begin] == 'p' && text[begin + 1] == 's' &&
                     text[begin + 2] == '_')
                color = UI_COLOR(code_type);
        } else if (isdigit((unsigned char)text[at])) {
            color = UI_COLOR(code_number);
            at++;
            while (at < size && (isalnum((unsigned char)text[at]) || text[at] == '.' ||
                                 (language && text[at] == '_')))
                at++;
        } else {
            nk_rune rune;
            int n = nk_utf_decode(text + at, &rune, (int)(size - at));
            at += (size_t)(n > 0 ? n : 1);
        }
        int length = (int)(at - begin);
        float width = font->width(font->userdata, font->height, text + begin, length);
        if (y + row >= area.y && x + width >= area.x && x < area.x + area.w)
            nk_draw_text(canvas,
                         nk_rect(x, y + ui->style.edit.row_padding / 2, width + 1, font->height),
                         text + begin, length, font, nk_rgba(0, 0, 0, 0), color);
        x += width;
    }
    nk_push_scissor(canvas, previous_clip);
}
static void editor(app *a, struct nk_text_edit *edit, float height) {
    struct nk_context *ui = a->ui;
    nk_style_push_font(ui, a->font_code);
    nk_layout_row_begin(ui, NK_STATIC, height, 2);
    nk_layout_row_push(ui, 44);
    struct nk_rect gutter;
    nk_widget(&gutter, ui);
    nk_layout_row_push(ui, nk_window_get_content_region(ui).w - 60);
    int before =
        nk_str_len_char(&edit->string); /* Hash detects replacements of identical length, too. */
    uint32_t hash =
        ps_crc32((const unsigned char *)nk_str_get_const(&edit->string), (size_t)before);
    struct nk_rect editor_bounds = nk_widget_bounds(ui);
    if (active_document(a) && edit == &active_document(a)->edit)
        a->document_bounds[6] = editor_bounds;
    struct nk_color transparent = nk_rgba(0, 0, 0, 0);
    nk_style_push_color(ui, &ui->style.edit.text_normal, transparent);
    nk_style_push_color(ui, &ui->style.edit.text_hover, transparent);
    nk_style_push_color(ui, &ui->style.edit.text_active, transparent);
    nk_style_push_color(ui, &ui->style.edit.selected_text_normal, transparent);
    nk_style_push_color(ui, &ui->style.edit.selected_text_hover, transparent);
    nk_edit_buffer(ui, NK_EDIT_BOX | NK_EDIT_ALLOW_TAB, edit, nk_filter_default);
    for (int i = 0; i < 5; i++)
        nk_style_pop_color(ui);
    syntax_text(a, edit, editor_bounds);
    int after = nk_str_len_char(&edit->string);
    if (before != after ||
        hash != ps_crc32((const unsigned char *)nk_str_get_const(&edit->string), (size_t)after)) {
        if (edit == &a->experiment) {
            a->dirty = true;
            invalidate_build(a);
        } else if (edit == &a->analysis) {
            a->analysis_dirty = true;
            invalidate_build(a);
        } else document_changed(a, edit);
    }
    nk_layout_row_end(ui);
    struct nk_command_buffer *canvas = nk_window_get_canvas(ui);
    nk_push_scissor(canvas, gutter);
    float line_h = ui->style.font->height + ui->style.edit.row_padding;
    int first = (int)(edit->scrollbar.y / line_h);
    for (int i = first; i < first + (int)(height / line_h) + 2; i++) {
        char number[20];
        snprintf(number, sizeof number, "%d", i + 1);
        nk_draw_text(
            canvas,
            nk_rect(gutter.x,
                    gutter.y + (float)i * line_h - edit->scrollbar.y + ui->style.edit.padding.y, 42,
                    line_h),
            number, (int)strlen(number), ui->style.font, UI_COLOR(gutter), UI_COLOR(gutter_text));
    }
    nk_push_scissor(canvas, nk_window_get_content_region(ui));
    nk_style_pop_font(ui);
}
// clang-format off
static bool documentation_window_open(app *a);
#include "documentation_ui.inc"
#include "report_ui.inc"
#include "library_ui.inc"
static void layouts_start(app *a);
static void channel_units_start(app *a);
#include "design_ui.inc"
// clang-format on
static struct nk_font *system_font(struct nk_font_atlas *atlas, float size, bool code, bool title) {
    static const nk_rune ui_ranges[] = {0x20, 0x17f, 0x300, 0x4ff, 0x2000, 0x206f, 0};
    static const nk_rune code_ranges[] = {0x20, 0x17f, 0x300, 0x4ff,
                                          0x1e00, 0x1eff, 0x2000, 0x206f, 0};
    struct nk_font_config config = nk_font_config(size);
    config.range = code ? code_ranges : ui_ranges;
    config.oversample_h = 3;
    config.oversample_v = 2;
    char path[4096];
#ifdef _WIN32
    const char *windows = SDL_getenv("WINDIR");
    snprintf(path, sizeof path, "%s/Fonts/%s", windows ? windows : "C:/Windows",
             code    ? "consola.ttf"
             : title ? "seguisb.ttf"
                     : "segoeui.ttf");
#elif defined(__APPLE__)
    snprintf(path, sizeof path, "/System/Library/Fonts/%s", code ? "Menlo.ttc" : "Helvetica.ttc");
#else
    snprintf(path, sizeof path, "/usr/share/fonts/truetype/dejavu/%s",
             code    ? "DejaVuSansMono.ttf"
             : title ? "DejaVuSans-Bold.ttf"
                     : "DejaVuSans.ttf");
#endif
    struct nk_font *font =
        exists(path) ? nk_font_atlas_add_from_file(atlas, path, size, &config) : NULL;
    return font ? font : nk_font_atlas_add_default(atlas, size, NULL);
}
#include "documentation_window.inc"
static void test_window_key_mod(SDL_Window *window, SDL_Keycode key, SDL_Keymod mod) {
    SDL_Event e = {0};
    e.type = SDL_EVENT_KEY_DOWN;
    e.key.windowID = SDL_GetWindowID(window);
    e.key.key = key;
    e.key.mod = mod;
    e.key.down = true;
    SDL_PushEvent(&e);
    e.type = SDL_EVENT_KEY_UP;
    e.key.down = false;
    SDL_PushEvent(&e);
}
static void test_window_key(SDL_Window *window, SDL_Keycode key) {
    test_window_key_mod(window, key, PS_UI_COMMAND_MOD);
}
static void create_managed_project(app *a) {
    a->manager_error[0]=0;
    if (!idle(a) || a->library_thread || a->recovery) {
        status(a, "Zuerst den laufenden Job beenden.");
        snprintf(a->manager_error,sizeof a->manager_error,"%s",a->status);a->manager_error_reveal=true;
        return;
    }
    if (a->dirty || a->analysis_dirty || a->project_settings_dirty)
        save_project(a);
    if (a->dirty || a->analysis_dirty || a->project_settings_dirty) {
        snprintf(a->manager_error,sizeof a->manager_error,"%s",a->status);a->manager_error_reveal=true;
        return;
    }
    if (!a->manager_name[0] || !strcmp(a->manager_name, ".") ||
        !strcmp(a->manager_name, "..") || strpbrk(a->manager_name, "/\\")) {
        status(a, "Einen gültigen Projektordnernamen eingeben.");
        snprintf(a->manager_error,sizeof a->manager_error,"%s",a->status);a->manager_error_reveal=true;
        return;
    }
    SDL_PathInfo parent;
    if (!SDL_GetPathInfo(a->manager_parent, &parent) ||
        parent.type != SDL_PATHTYPE_DIRECTORY) {
        status(a, "Der Zielordner für das Projekt existiert nicht.");
        snprintf(a->manager_error,sizeof a->manager_error,"%s",a->status);a->manager_error_reveal=true;
        return;
    }
    int n = snprintf(a->project_input, sizeof a->project_input, "%s/%s", a->manager_parent,
                     a->manager_name);
    if (n < 0 || n >= (int)sizeof a->project_input) {
        status(a, "Projektpfad ist zu lang.");
        snprintf(a->manager_error,sizeof a->manager_error,"%s",a->status);a->manager_error_reveal=true;
        return;
    }
    static const int language_templates[8] = {8, 9, 18, 12, 16, 10, 19, 17};
    a->example = a->manager_experiment_language
                     ? language_templates[a->manager_template] : a->manager_template;
    new_project(a);
    if(a->project_manager){snprintf(a->manager_error,sizeof a->manager_error,"%s",a->status);a->manager_error_reveal=true;}
}
static bool open_workspace_path(app *a, const char *path) {
    SDL_PathInfo info;
    if (!path || !SDL_GetPathInfo(path, &info) || info.type != SDL_PATHTYPE_DIRECTORY) {
        status(a, "Ordner konnte nicht geöffnet werden.");
        return false;
    }
    if (!idle(a) || a->library_thread || a->recovery) {
        status(a, "Zuerst den laufenden Job beenden.");
        return false;
    }
    if (a->dirty || a->analysis_dirty || a->project_settings_dirty)
        save_project(a);
    if (a->dirty || a->analysis_dirty || a->project_settings_dirty)
        return false;
    if (strlen(path) >= sizeof a->workspace) {
        status(a, "Ordnerpfad ist zu lang.");
        return false;
    }
    if (!documents_save_all(a)) return false;
    char manifest[4096];
    join(manifest, sizeof manifest, path, "physim.project");
    if (exists(manifest)) {
        char previous[4096];snprintf(previous,sizeof previous,"%s",a->project_input);
        snprintf(a->project_input,sizeof a->project_input,"%s",path);
        if (!open_project(a)) {
            snprintf(a->project_input,sizeof a->project_input,"%s",previous);
            return false;
        }
        a->tab=0;
        return true;
    }
    documents_clear(a);
    clear_project(a);
    snprintf(a->workspace, sizeof a->workspace, "%s", path);
    snprintf(a->project_input, sizeof a->project_input, "%s", path);
    a->workspace_open = true;
    a->workspace_disclosure = NK_MAXIMIZED;
    a->project_manager = false;
    a->tab = 0;
    a->workspace_preview_path[0] = 0;
    a->workspace_addition_count = 0;
    refresh_workspace_entries(a);
    status(a, "Ordner geöffnet. Kein Physim-Projekt erkannt; Dateien können angesehen werden.");
    return true;
}

#include "workspace_actions.inc"
#include "workspace_catalog_actions.inc"

static Uint32 workspace_dialog_event;
static const int dialog_open_folder = PS_DIALOG_OPEN_FOLDER;
static const int dialog_add_file = PS_DIALOG_ADD_FILE;
static const int dialog_add_folder = PS_DIALOG_ADD_FOLDER;
static const int dialog_manager_parent = PS_DIALOG_MANAGER_PARENT;
static const int dialog_import_run=PS_DIALOG_IMPORT_RUN;
static const int dialog_resume_batch=PS_DIALOG_RESUME_BATCH;
static void SDLCALL workspace_dialog_callback(void *userdata, const char *const *filelist,
                                               int filter) {
    (void)filter;
    SDL_Event event = {0};
    event.type = workspace_dialog_event;
    event.user.code = *(const int *)userdata;
    /* Zenity in SDL 3.2.30 can report cancellation as an empty first string. */
    event.user.data1 = filelist && filelist[0] && filelist[0][0] ? SDL_strdup(filelist[0]) : NULL;
    event.user.data2 = filelist ? NULL : (void *)1;
    if (!SDL_PushEvent(&event))
        SDL_free(event.user.data1);
}
static void choose_workspace_path(app *a, int mode) {
    if (a->dialog_pending || workspace_dialog_event == (Uint32)-1)
        return;
    a->dialog_pending = true;
    if(mode==PS_DIALOG_RESUME_BATCH && *a->batch_resume_test_path) {
        const char *files[]={a->batch_resume_test_path,NULL};workspace_dialog_callback((void *)&dialog_resume_batch,files,0);
    }
    else if(mode==PS_DIALOG_IMPORT_RUN && *a->import_test_path) {
        const char *files[]={a->import_test_path,NULL};workspace_dialog_callback((void *)&dialog_import_run,files,0);
    }
    else if(mode==PS_DIALOG_IMPORT_RUN) {
        static const SDL_DialogFileFilter filters[]={{"Physim-Messlauf","psrun"}};
        SDL_ShowOpenFileDialog(workspace_dialog_callback,(void *)&dialog_import_run,a->window,filters,1,NULL,false);
    }
    else if (mode == PS_DIALOG_ADD_FILE)
        SDL_ShowOpenFileDialog(workspace_dialog_callback, (void *)&dialog_add_file, a->window,
                               NULL, 0, a->workspace_open ? a->workspace : NULL, false);
    else {
        const int *dialog_mode = mode == PS_DIALOG_OPEN_FOLDER ? &dialog_open_folder
                                 : mode == PS_DIALOG_RESUME_BATCH ? &dialog_resume_batch
                                 : mode == PS_DIALOG_MANAGER_PARENT ? &dialog_manager_parent
                                                                    : &dialog_add_folder;
        const char *location = mode == PS_DIALOG_MANAGER_PARENT ? a->manager_parent
                               : a->workspace_open ? a->workspace : NULL;
        SDL_ShowOpenFolderDialog(workspace_dialog_callback, (void *)dialog_mode, a->window,
                                 location, false);
    }
}
static void add_workspace_path(app *a, const char *path) {
    SDL_PathInfo info;
    if (!a->workspace_open || !SDL_GetPathInfo(path, &info)) {
        status(a, "Zuerst einen Ordner öffnen; Auswahl konnte nicht gelesen werden.");
        return;
    }
    if (strlen(path) >= sizeof a->workspace_additions[0]) {
        status(a, "Ausgewählter Pfad ist zu lang.");
        return;
    }
    for (unsigned i = 0; i < a->workspace_addition_count; i++)
        if (!strcmp(path, a->workspace_additions[i]))
            return;
    if (a->workspace_addition_count == 32) {
        status(a, "Maximal 32 zusätzliche Dateien und Ordner sind möglich.");
        return;
    }
    snprintf(a->workspace_additions[a->workspace_addition_count++],
             sizeof a->workspace_additions[0], "%s", path);
    refresh_workspace_entries(a);
    status(a, "Datei oder Ordner zum Workspace hinzugefügt.");
}
static void test_key(app *a, SDL_Keycode key) {
    test_window_key(a->window, key);
}
#define PS_TEST_MOUSE_ID ((SDL_MouseID)0x50535445u)
/* Scripted desktop workflows are isolated from native pointer and focus events.
 * Window resizing, minimizing and closing still come from the real desktop. */
static bool test_scripted_external_input(const SDL_Event *e) {
    switch (e->type) {
    case SDL_EVENT_MOUSE_MOTION: return e->motion.which != PS_TEST_MOUSE_ID;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP: return e->button.which != PS_TEST_MOUSE_ID;
    case SDL_EVENT_MOUSE_WHEEL: return e->wheel.which != PS_TEST_MOUSE_ID;
    case SDL_EVENT_WINDOW_FOCUS_LOST: return e->window.data1 != (int)PS_TEST_MOUSE_ID;
    default: return false;
    }
}
static void test_window_mouse(SDL_Window *window,struct nk_rect rect,bool down) {
    float x = rect.x + rect.w * .5f, y = rect.y + rect.h * .5f;
    SDL_Event e = {0};
    e.type = SDL_EVENT_MOUSE_MOTION;
    e.motion.windowID = SDL_GetWindowID(window);
    e.motion.which = PS_TEST_MOUSE_ID;
    e.motion.x = x;
    e.motion.y = y;
    SDL_PushEvent(&e);
    e.type = down ? SDL_EVENT_MOUSE_BUTTON_DOWN : SDL_EVENT_MOUSE_BUTTON_UP;
    e.button.windowID = SDL_GetWindowID(window);
    e.button.which = PS_TEST_MOUSE_ID;
    e.button.x = x;
    e.button.y = y;
    e.button.button = SDL_BUTTON_LEFT;
    e.button.down = down;
    SDL_PushEvent(&e);
}
static void test_mouse(app *a,struct nk_rect rect,bool down) {
    test_window_mouse(a->window,rect,down);
}
// clang-format off
#include "autosave_tests.inc"
#include "batch_tests.inc"
#include "plot_tests.inc"
#include "ui_size_tests.inc"
#include "settings_tests.inc"
#include "workspace_tree_tests.inc"
#include "document_tests.inc"
#include "document_recovery_tests.inc"
#include "toolbar_tests.inc"
#include "keyboard_menu_tests.inc"
#include "documentation_keyboard_tests.inc"
#include "project_manager_keyboard_tests.inc"
#include "project_settings_tests.inc"
#include "project_migration_tests.inc"
#include "reset_tests.inc"
#include "speed_tests.inc"
#include "timeline_tests.inc"
#include "docking_tests.inc"
#include "hierarchy_tests.inc"
#include "inspector_tests.inc"
#include "layout_tests.inc"
#include "workspace_catalog_tests.inc"
#include "pchip_tests.inc"
#include "channel_units_tests.inc"
#include "analysis_project_tests.inc"
#include "batch_resume_tests.inc"
#include "batch_missing_tests.inc"
#include "series_mask_tests.inc"
#include "logging_tests.inc"
#include "frame_tests.inc"
#include "contact_world_tests.inc"
#include "material_tutorial_tests.inc"
#include "spring_tutorial_tests.inc"
#include "pendulum_tutorial_tests.inc"
#include "pendulum_medium_tests.inc"
#include "collision_tutorial_tests.inc"
#include "diagnostic_tests.inc"
#include "adaptive_tests.inc"
#include "series_tests.inc"
#include "monte_carlo_tutorial_tests.inc"
#include "saved_run_tutorial_tests.inc"
#include "native_dialog_tests.inc"
#include "workspace_tests.inc"
// clang-format on
int main(int argc, char **argv) {
    double profile_app_started = ps_clock();
    bool workspace_test = argc > 1 && !strcmp(argv[1], "--workspace-test");
    bool workspace_state_test = argc > 1 && !strcmp(argv[1], "--workspace-state-test");
    if (workspace_state_test && argc != 4)
        return 1;
    bool toolbar_test = workspace_state_test &&
                        (!strcmp(argv[3], "toolbar") || !strcmp(argv[3], "toolbar-noise"));
    if (workspace_test && argc != 3)
        return 2;
    bool syntax_preview_test = argc > 1 && !strcmp(argv[1], "--syntax-preview-test");
    if (syntax_preview_test && argc != 4)
        return 2;
    bool migration_test=argc>1 && !strcmp(argv[1],"--project-migration-test");
    if(migration_test && (argc!=4 || (strcmp(argv[3],"normal") && strcmp(argv[3],"stale") && strcmp(argv[3],"dirty"))))return 2;
    bool settings_test = argc > 1 && !strcmp(argv[1], "--settings-test");
    if (settings_test && (argc != 4 || (strcmp(argv[3], "write") && strcmp(argv[3], "read") &&
        strcmp(argv[3], "keyboard") && strcmp(argv[3], "keyboard-read") &&
        strcmp(argv[3], "checkbox-native") && strcmp(argv[3], "checkbox-remote") &&
        strcmp(argv[3], "options-native") && strcmp(argv[3], "options-remote") &&
        strcmp(argv[3], "focus-native") && strcmp(argv[3], "focus-remote") &&
        strcmp(argv[3], "reset") && strcmp(argv[3], "defaults") && strcmp(argv[3], "corrupt") &&
        strcmp(argv[3], "maxwrite") && strcmp(argv[3], "maxread") &&
        strcmp(argv[3], "theme-light") && strcmp(argv[3], "theme-light-read") &&
        strcmp(argv[3], "theme-contrast") && strcmp(argv[3], "theme-contrast-read") &&
        strcmp(argv[3], "theme-cancel") && strcmp(argv[3], "theme-defaults") &&
        strcmp(argv[3], "theme-dark-read") && strncmp(argv[3],"ui-size-",8))))
        return 2;
    bool plot_noise = argc > 1 && !strcmp(argv[1], "--plot-test-noise");
    bool plot_test = plot_noise || (argc > 1 && !strcmp(argv[1], "--plot-test"));
    if (plot_test && argc != 3)
        return 2;
    bool batch_test = argc > 1 && !strcmp(argv[1], "--batch-test");
    if (batch_test && argc != 3 && argc != 4)
        return 2;
    int test_batch_workers = 2;
    if (batch_test && argc == 4) {
        if (strlen(argv[3]) != 1 || argv[3][0] < '2' || argv[3][0] > '8')
            return 2;
        test_batch_workers = argv[3][0] - '0';
    }
    bool autosave_crash = argc > 1 && !strcmp(argv[1], "--autosave-crash");
    bool recovery_test = argc > 1 && !strcmp(argv[1], "--autosave-recover");
    if ((autosave_crash && argc != 3) || (recovery_test && argc != 4)) {
        fprintf(stderr, "Usage: physim --autosave-crash new-project | "
                        "--autosave-recover project restore|discard|conflict|cancel|corrupt\n");
        return 2;
    }
    const char *recovery_mode = recovery_test ? argv[3] : "";
    if (recovery_test && strcmp(recovery_mode, "restore") && strcmp(recovery_mode, "discard") &&
        strcmp(recovery_mode, "conflict") && strcmp(recovery_mode, "cancel") &&
        strcmp(recovery_mode, "corrupt"))
        return 2;
    bool docs_noise = argc > 1 && !strcmp(argv[1], "--docs-test-noise");
    bool docs_test = docs_noise || (argc > 1 && !strcmp(argv[1], "--docs-test"));
    if (docs_test && argc != 3) {
        fprintf(stderr, "Usage: physim --docs-test output-directory\n");
        return 2;
    }
    int test_example = 0;
    bool test_language_analysis = false;
    if (argc > 1 && !strcmp(argv[1], "--self-test")) {
        if (argc < 3 || argc > 4) {
            fprintf(stderr,
                    "Usage: physim --self-test new-project "
                    "[pendulum|projectile|collision|box_floor|spring|uncertain_projectile|box_collision|buoyancy|"
                    "language_pendulum|language_projectile|language_sensors|language_body|language_contact|language_joint|language_graph|language_sweep|language_spring|language_buoyancy|language_collision|language_box_collision|language_full|language_mixed]\n");
            return 2;
        }
        if (argc == 4) {
            if (!strcmp(argv[3], "projectile"))
                test_example = 1;
            else if (!strcmp(argv[3], "collision"))
                test_example = 2;
            else if (!strcmp(argv[3], "box_floor"))
                test_example = 3;
            else if (!strcmp(argv[3], "spring"))
                test_example = 4;
            else if (!strcmp(argv[3], "uncertain_projectile"))
                test_example = 5;
            else if (!strcmp(argv[3], "box_collision"))
                test_example = 6;
            else if (!strcmp(argv[3], "buoyancy"))
                test_example = 7;
            else if (!strcmp(argv[3], "language_pendulum"))
                test_example = 8;
            else if (!strcmp(argv[3], "language_projectile")) {
                test_example = 9;
                test_language_analysis = true;
            }
            else if (!strcmp(argv[3], "language_sensors")) {
                test_example = 10;
                test_language_analysis = true;
            } else if (!strcmp(argv[3], "language_body")) {
                test_example = 11;
                test_language_analysis = true;
            } else if (!strcmp(argv[3], "language_contact")) {
                test_example = 12;
                test_language_analysis = true;
            } else if (!strcmp(argv[3], "language_full")) {
                test_example = 8;
                test_language_analysis = true;
            } else if (!strcmp(argv[3], "language_joint")) {
                test_example = 13;
                test_language_analysis = true;
            } else if (!strcmp(argv[3], "language_mixed")) {
                test_example = 0;
                test_language_analysis = true;
            } else if (!strcmp(argv[3], "language_graph")) {
                test_example = 14;
                test_language_analysis = true;
            } else if (!strcmp(argv[3], "language_sweep")) {
                test_example = 15;
                test_language_analysis = true;
            } else if (!strcmp(argv[3], "language_spring")) {
                test_example = 16;
                test_language_analysis = true;
            } else if (!strcmp(argv[3], "language_buoyancy")) {
                test_example = 17;
                test_language_analysis = true;
            } else if (!strcmp(argv[3], "language_collision")) {
                test_example = 18;
                test_language_analysis = true;
            } else if (!strcmp(argv[3], "language_box_collision")) {
                test_example = 19;
                test_language_analysis = true;
            } else if (strcmp(argv[3], "pendulum")) {
                fprintf(stderr, "Unknown test example\n");
                return 2;
            }
        }
    }
    bool trace_test = SDL_getenv("PHYSIM_TEST_TRACE") != NULL;
    if (trace_test)
        fprintf(stderr, "APP TEST TRACE: initializing SDL\n");
    SDL_SetMainReady();
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL: %s\n", SDL_GetError());
        return 1;
    }
    if (trace_test) {
        const char *driver = SDL_GetCurrentVideoDriver();
        fprintf(stderr, "APP TEST TRACE: SDL initialized, video driver %s\n", driver ? driver : "unknown");
    }
    workspace_dialog_event = SDL_RegisterEvents(1);
    if (workspace_dialog_event == (Uint32)-1) {
        fprintf(stderr, "Workspace dialog event: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
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
    if (trace_test)
        fprintf(stderr, "APP TEST TRACE: creating window\n");
    SDL_Window *window =
        SDL_CreateWindow("Physim | Experiment Studio", 1440, 940,
                         SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_OPENGL);
    if (trace_test)
        fprintf(stderr, "APP TEST TRACE: window %s, creating OpenGL context\n", window ? "created" : "failed");
    ps_graphics *graphics = window ? ps_graphics_create(window) : NULL;
    if (!graphics) {
        fprintf(stderr, "OpenGL initialization: %s\n", SDL_GetError());
        if (argc < 2 || strncmp(argv[1], "--", 2))
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Physim: Grafikfehler", SDL_GetError(),
                                     window);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    if (argc > 1 && !strcmp(argv[1], "--renderer-test")) {
        bool ok = ps_graphics_test(graphics, argc > 2 ? argv[2] : NULL) &&
                  nk_sdl_test_input(window, graphics);
        if (!ok)
            fprintf(stderr, "GPU TEST: %s\n", SDL_GetError());
        ps_graphics_destroy(graphics);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return ok ? 0 : 1;
    }
    if (trace_test)
        fprintf(stderr, "APP TEST TRACE: window and OpenGL ready\n");
    SDL_SetWindowMinimumSize(window, 1080, 740);
    app *a = calloc(1, sizeof *a);
    if (!a) {
        ps_graphics_destroy(graphics);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    a->window = window;
    if (SDL_SetWindowHitTest(window, window_hit_test, a))
        SDL_SetWindowBordered(window, false);
    else
        fprintf(stderr, "Integrierter Fensterkopf: %s\n", SDL_GetError());
    a->preferences = PS_PREFERENCES_DEFAULT;
    a->doc_jump_block = -1;
    a->doc_previous = -1;
    a->graphics = graphics;
    a->camera_target = PS_CAMERA_DEFAULT.target;
    a->png_scale = 2;
    a->ui = nk_sdl_init(window, graphics);
    if (!a->ui) {
        free(a);
        ps_graphics_destroy(graphics);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    struct nk_font_atlas *atlas = nk_sdl_font_stash_begin(a->ui);
    struct nk_font *ui_fonts[4], *title_fonts[4];
    bool ui_fonts_ok=true;
    for(int i=0;i<4;i++) {
        ui_fonts[i]=system_font(atlas,(float)(16+i*2),false,false);
        title_fonts[i]=system_font(atlas,(float)(23+i*2),false,true);
        ui_fonts_ok &= ui_fonts[i]!=NULL && title_fonts[i]!=NULL;
    }
    struct nk_font *font=ui_fonts[0];
    struct nk_font *code_fonts[4];
    bool code_fonts_ok = true;
    for (int i = 0; i < 4; i++) {
        code_fonts[i] = system_font(atlas, (float)(16 + i * 2), true, false);
        code_fonts_ok &= code_fonts[i] != NULL;
    }
    struct nk_font *title_font = title_fonts[0];
    if (!ui_fonts_ok || !code_fonts_ok || !title_font || !nk_sdl_font_stash_end(a->ui)) {
        fprintf(stderr, "Font initialization: %s\n", SDL_GetError());
        nk_sdl_shutdown(a->ui);
        free(a);
        ps_graphics_destroy(graphics);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    style(a->ui);
    a->font_ui = &font->handle;
    for(int i=0;i<4;i++){a->ui_fonts[i]=&ui_fonts[i]->handle;a->title_fonts[i]=&title_fonts[i]->handle;}
    for (int i = 0; i < 4; i++)
        a->code_fonts[i] = &code_fonts[i]->handle;
    a->font_code = a->code_fonts[0];
    a->font_title = &title_font->handle;
    nk_style_set_font(a->ui, &font->handle);
    nk_textedit_init_default(&a->experiment);
    nk_textedit_init_default(&a->analysis);
    char executable[4096];
    if (!ps_executable_path(executable, sizeof executable))
        snprintf(executable, sizeof executable, "%s", argv[0]);
    char *slash = strrchr(executable, '/'), *back = strrchr(executable, '\\');
    if (back && (!slash || back > slash))
        slash = back;
    if (slash)
        *slash = 0;
    snprintf(a->bin, sizeof a->bin, "%s", executable);
    snprintf(a->root, sizeof a->root, "%s/..", a->bin);
    char check[4096];
    join(check, sizeof check, a->root, "include/physim/core.h");
#ifdef __APPLE__
    if (!exists(check)) {
        snprintf(a->root, sizeof a->root, "%s/../Resources", a->bin);
        join(check, sizeof check, a->root, "include/physim/core.h");
    }
#endif
    if (!exists(check))
        snprintf(a->root, sizeof a->root, "%s", PS_SOURCE_DIR);
    char projects[4096];
#ifdef __APPLE__
    const char *documents = SDL_GetUserFolder(SDL_FOLDER_DOCUMENTS);
    char *fallback = documents ? NULL : SDL_GetPrefPath("Physim", "Physim");
    join(projects, sizeof projects, documents ? documents : fallback ? fallback : ".", "Physim");
    SDL_free(fallback);
#else
    join(projects, sizeof projects, a->root, "projects");
#endif
    ps_make_directory(projects);
    snprintf(a->manager_parent, sizeof a->manager_parent, "%s", projects);
    a->dt = 0.005;
    a->simulation_speed = 1;
    a->minimum_dt=1e-8;a->maximum_dt=.1;
    a->batch_dt = .005;
    a->batch_end_time=1;a->batch_minimum_dt=1e-8;a->batch_maximum_dt=.1;
    a->batch_timeout = 30;
    a->batch_runs = 256;
    a->batch_workers = SDL_GetNumLogicalCPUCores();
    if (a->batch_workers < 1)
        a->batch_workers = 1;
    if (a->batch_workers > 4)
        a->batch_workers = 4;
    a->batch_steps = 200;
    snprintf(a->batch_seed, sizeof a->batch_seed, "42");
    snprintf(a->batch_channel, sizeof a->batch_channel, "position.x");
    snprintf(a->seed, sizeof a->seed, "42");
    a->yaw = PS_CAMERA_DEFAULT.yaw;
    a->pitch = PS_CAMERA_DEFAULT.pitch;
    a->zoom = PS_CAMERA_DEFAULT.distance;
    a->show_vectors = true;
    a->show_grid = true;
    a->show_paths = a->show_points = a->show_labels = true;
    a->history_stride = 1;
    status(a, "Bereit. Ordner öffnen oder ein neues Projekt anlegen.");
    bool smoke = argc > 1 && !strcmp(argv[1], "--smoke");
    bool self_test = migration_test || settings_test || plot_test || batch_test || docs_test || recovery_test ||
                     workspace_test || workspace_state_test || syntax_preview_test ||
                     (argc > 2 && !strcmp(argv[1], "--self-test"));
    if (self_test && SDL_getenv("PHYSIM_TEST_SMALL"))
        SDL_SetWindowSize(window, 1080, 740);
    if (argc < 2 || strncmp(argv[1], "--", 2)) {
        preferences_start(a, NULL);
        workspace_state_start(a, NULL);
        document_drafts_start(a, NULL);
    }
    int test_stage = 0, exit_code = 0;
    double test_started = ps_clock(), paused_time = 0;
    unsigned doc_input_events = 0, doc_input_bytes = 0;
    nk_uint test_doc_scroll = 0;
    SDL_WindowID test_doc_window_id = 0;
    ps_report *test_previous_report = NULL;
    char test_original_run[4096] = {0}, test_original_report[4096] = {0};
    uint64_t test_original_samples = 0;
    int test_library_row = -1;
    if (syntax_preview_test) {
        snprintf(a->project, sizeof a->project, "%s", argv[2]);
        if (!ps_make_directory(a->project) || !load_editor(&a->experiment, argv[3])) {
            exit_code = 1;
            a->quitting = true;
        } else {
            a->loaded = true;
            a->language_experiment = true;
            test_stage = 120;
        }
    } else if (workspace_state_test) {
        if (!workspace_test_start(a, argv[2], argv[3])) {
            exit_code = 1;
            a->quitting = true;
        }
    } else if (workspace_test) {
        if (a->workspace_open || a->loaded || a->project_input[0] ||
            !ps_make_directory(argv[2])) {
            exit_code = 1;
            a->quitting = true;
        }
        test_stage = 100;
    } else if(migration_test) {
        snprintf(a->project_input,sizeof a->project_input,"%s",argv[2]);
        if(!open_project(a)){exit_code=1;a->quitting=true;}
    } else if (settings_test) {
        snprintf(a->project, sizeof a->project, "%s", argv[2]);
        char path[4096];
        join(path, sizeof path, a->project, "preferences.bin");
        if (!strcmp(argv[3], "write") && !ps_make_directory_exclusive(a->project)) {
            exit_code = 1;
            a->quitting = true;
        }
        preferences_start(a, path);
        a->loaded = true;
        const char *sample = "/* Darstellungstest */\ndouble energy(double mass, double speed) {\n"
                             "    return 0.5 * mass * speed * speed;\n}\n";
        nk_str_append_text_char(&a->experiment.string, sample, (int)strlen(sample));
    } else if (autosave_crash) {
        snprintf(a->project_input, sizeof a->project_input, "%s", argv[2]);
        new_project(a);
        bool ok = test_autosave_crash(a);
        fprintf(stdout, "AUTOSAVE ABRUPT EXIT: %s\n", ok ? "PASSED" : "FAILED");
        fflush(stdout);
        _Exit(ok ? 0 : 1);
    } else if (recovery_test) {
        snprintf(a->project_input, sizeof a->project_input, "%s", argv[2]);
        open_project(a);
        bool corrupt = !strcmp(recovery_mode, "corrupt");
        bool conflict = !strcmp(recovery_mode, "conflict");
        if (!a->loaded || (corrupt ? !a->autosave_blocked || a->recovery
                                   : !a->recovery || a->recovery_conflict != conflict)) {
            exit_code = 1;
            a->quitting = true;
        }
        test_stage = 80;
    } else if (plot_test) {
        if (!test_plot_setup(a, argv[2])) {
            exit_code = 1;
            a->quitting = true;
        }
    } else if (docs_test) {
        snprintf(a->project, sizeof a->project, "%s", argv[2]);
        if (!ps_make_directory(a->project)) {
            a->quitting = true;
            exit_code = 1;
        }
        test_stage = 20;
    } else if (batch_test) {
        a->example = 5;
        a->batch_workers = test_batch_workers - 1; /* Incremented through the actual UI control. */
        snprintf(a->project_input, sizeof a->project_input, "%s", argv[2]);
        new_project(a);
        if (!a->loaded) {
            exit_code = 1;
            a->quitting = true;
        } else {
            build_project(a);
            test_stage = 90;
        }
    } else if (self_test) {
        a->example = test_example;
        a->template_analysis_language = test_language_analysis ? 1 : 0;
        if (test_example == 1 || test_example == 5 || test_example == 10) {
            a->camera_target = ps_v3(-.7, .3, 0);
            a->zoom = 4.5f;
        }
        snprintf(a->project_input, sizeof a->project_input, "%s", argv[2]);
        new_project(a);
        if (!a->loaded || nk_str_len_char(&a->experiment.string) < 100 ||
            nk_str_len_char(&a->analysis.string) < 100) {
            fprintf(stderr, "Self-test requires a new writable project directory.\n");
            a->quitting = true;
            exit_code = 1;
        } else if (!test_editor_failed_import(a) || !test_editor_safe_save(a)) {
            status(a, "SELF-TEST: Import oder sicheres Speichern fehlgeschlagen.");
            a->quitting = true;
            exit_code = 1;
        } else {
            const char *failure = a->language_experiment || a->language_analysis
                                      ? "\nlet expectedFailure: Float64 = true\n"
                                      : "\n#error PHYSIM_EXPECTED_BUILD_FAILURE\n";
            struct nk_text_edit *target = test_language_analysis ? &a->analysis : &a->experiment;
            nk_str_append_text_char(&target->string, failure, (int)strlen(failure));
            if (test_language_analysis) a->analysis_dirty = true;
            else a->dirty = true;
            build_project(a);
            test_stage = -1;
        }
    } else if (argc > 1 && !smoke) {
        open_workspace_path(a, argv[1]);
    }
    if (trace_test)
        fprintf(stderr, "APP TEST TRACE: initial stage %d, status %s\n", test_stage, a->status);
    const char *profile_directory = SDL_getenv("PHYSIM_PROFILE_DIR");
    bool profile_requested = profile_directory && *profile_directory;
    a->profile_started_at = profile_app_started;
    if (profile_requested) {
        a->profiler = ps_app_profile_start(profile_directory);
        if (!a->profiler) fprintf(stderr, "App profiling could not start; app continues.\n");
    }
    double profile_ready = 0;
    Uint64 profile_previous = a->profiler ? SDL_GetTicksNS() : 0;
    unsigned frames = 0;
    while (!a->quitting) {
        Uint64 profile_frame_started = a->profiler ? SDL_GetTicksNS() : 0;
        Uint64 profile_phase = profile_frame_started;
        if (a->profiler) {
            a->profile_frame = (ps_app_profile_frame){0};
            a->profile_frame.frame = frames;
        }
        nk_input_begin(a->ui);
        if (a->doc_ui)
            nk_input_begin(a->doc_ui);
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == workspace_dialog_event) {
                a->dialog_pending = false;
                const char *path = (const char *)e.user.data1;
                if (path) {
                    if (e.user.code == PS_DIALOG_OPEN_FOLDER)
                        open_workspace_path(a, path);
                    else if(e.user.code==PS_DIALOG_IMPORT_RUN)import_run(a,path);
                    else if(e.user.code==PS_DIALOG_RESUME_BATCH)resume_batch(a,path);
                    else if (e.user.code == PS_DIALOG_MANAGER_PARENT)
                        snprintf(a->manager_parent, sizeof a->manager_parent, "%s", path);
                    else
                        add_workspace_path(a, path);
                    SDL_free(e.user.data1);
                } else if (e.user.data2) {
                    status(a, "Dateiauswahl fehlgeschlagen.");
                }
                continue;
            }
            if ((plot_test || toolbar_test || settings_test || docs_test ||
                 (workspace_state_test && strncmp(argv[3],"native-dialog",13) &&
                  strcmp(argv[3],"documents-unfiltered"))) && test_scripted_external_input(&e)) continue;
            if (self_test && e.type == SDL_EVENT_TEXT_INPUT) {
                doc_input_events++;
                doc_input_bytes += (unsigned)strlen(e.text.text);
            }
            SDL_Window *event_window = SDL_GetWindowFromEvent(&e);
            if(event_window==a->window && nk_sdl_accessibility_event(a->ui,&e))continue;
            if(event_window==a->doc_window && a->doc_ui && nk_sdl_accessibility_event(a->doc_ui,&e))continue;
            if (event_window == a->window && e.type == SDL_EVENT_WINDOW_FOCUS_LOST)
                { toolbar_keyboard_close(a);a->settings_keyboard=false; }
            if (a->doc_window && event_window == a->doc_window) {
                if (e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
                    documentation_window_hide(a);
                else if (a->doc_visible) {
                    if (e.type == SDL_EVENT_KEY_DOWN && documentation_keyboard_key(a,&e.key))continue;
                    if (e.type == SDL_EVENT_KEY_DOWN && !e.key.repeat) {
                        if (e.key.key == SDLK_F && (e.key.mod & PS_UI_COMMAND_MOD)) {
                            a->doc_search_focus = true;
                            a->doc_filter_active = false;
                        }
                        if (e.key.key == SDLK_ESCAPE) {
                            documentation_window_hide(a);
                            continue;
                        }
                    }
                    nk_sdl_handle_event(a->doc_ui, &e);
                }
                continue;
            }
            if (e.type == SDL_EVENT_QUIT ||
                (e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event_window == a->window)) {
                if (a->dirty || a->analysis_dirty || a->project_settings_dirty)
                    save_project(a);
                if (!a->dirty && !a->analysis_dirty && !a->project_settings_dirty && documents_save_all(a))
                    a->quitting = true;
            }
            if (e.type == SDL_EVENT_KEY_DOWN && event_window == a->window && (a->layout_manager || a->workspace_manager || a->channel_unit_manager) && e.key.key == SDLK_ESCAPE) {
                a->layout_manager = false;
                a->workspace_manager = false;
                a->channel_unit_manager = false;
                continue;
            }
            if (e.type == SDL_EVENT_KEY_DOWN && event_window == a->window && !a->recovery && !a->layout_manager && !a->workspace_manager && !a->channel_unit_manager) {
                if (toolbar_keyboard_key(a, &e.key)) continue;
                if (settings_keyboard_key(a, &e.key)) continue;
                if (project_manager_keyboard_key(a, &e.key)) continue;
                if (a->dock_drag && e.key.key==SDLK_ESCAPE) { a->dock_drag=0;a->dock_dragging=false;continue; }
                if (scene_shortcut(a, &e.key)) continue;
                if (a->tab == 1 && e.key.key == SDLK_SPACE && !e.key.repeat &&
                    !(e.key.mod & (SDL_KMOD_CTRL | SDL_KMOD_GUI | SDL_KMOD_ALT)) &&
                    (!a->ui->active || (!a->ui->active->edit.active &&
                     !a->ui->active->property.active && !a->ui->active->popup.active))) {
                    timeline_toggle_play(a);
                    continue;
                }
                if (!e.key.repeat && (e.key.mod & PS_UI_COMMAND_MOD)) {
                    if (e.key.key >= SDLK_1 && e.key.key <= SDLK_3)
                        select_workspace_tab(a, (int)(e.key.key - SDLK_1));
                    if (e.key.key == SDLK_4)
                        open_library(a);
                    if (e.key.key == SDLK_F) {
                        if (a->tab == 4)
                            a->library_search_focus = true;
                        else
                            a->show_search = !a->show_search;
                    }
                    if (e.key.key == SDLK_L) {
                        a->show_log = !a->show_log;
                        if(a->show_log) dock_show(a,PS_DOCK_LOG);
                    }
                    if (e.key.key == SDLK_COMMA)
                        open_settings(a);
                }
                if (e.key.key == SDLK_S && (e.key.mod & PS_UI_COMMAND_MOD))
                    save_active_document(a);
                if (e.key.key == SDLK_W && (e.key.mod & PS_UI_COMMAND_MOD) && a->tab == 8)
                    document_request(a, 1);
                if (e.key.key == SDLK_F5)
                    build_project(a);
                if (e.key.key == SDLK_F1 && !e.key.repeat)
                    open_documentation(a, a->documentation ? a->doc_topic : 1);
                if (e.key.key == SDLK_F7 && !e.key.repeat)
                    reset_run(a);
                if (e.key.key == SDLK_F6 && !e.key.repeat && !a->reset_pending && !a->reset_starting) {
                    if (!a->runner.running)
                        start_run(a);
                    else if (a->hello && !a->stop_at)
                        command(a, a->paused ? PS_MSG_RUN : PS_MSG_PAUSE);
                }
            }
            if ((a->toolbar_keyboard || (a->tab==6 && a->settings_keyboard)) && e.type == SDL_EVENT_TEXT_INPUT) continue;
            nk_sdl_handle_event(a->ui, &e);
        }
        nk_input_end(a->ui);
        if (a->doc_ui)
            nk_input_end(a->doc_ui);
        if(nk_sdl_accessibility_has_focus(a->ui)){a->settings_keyboard=false;toolbar_keyboard_close(a);}
        if (a->profiler) {
            Uint64 now = SDL_GetTicksNS();
            a->profile_frame.event_seconds = (double)(now - profile_phase) / 1e9;
            profile_phase = now;
        }
        pump(a);
        autosave_tick(a, ps_clock());
        documents_autosave_tick(a, ps_clock());
        const char *capture = NULL;
        int checked_stage = test_stage, previous_exit_code = exit_code;
        if (syntax_preview_test) {
            if (ps_clock() - test_started > 15) {
                exit_code = 1;
                a->quitting = true;
            } else if (test_stage == 120 && frames > 1) {
                capture = "unicode-editor.bmp";
                test_stage = 121;
            } else if (test_stage == 121) {
                printf("UNICODE EDITOR PREVIEW: %s\n", exit_code ? "FAILED" : "PASSED");
                a->quitting = true;
            }
        } else if (workspace_state_test) {
            if (ps_clock() - test_started >
                (!strncmp(argv[3],"docs-keyboard",13)?60:!strncmp(argv[3],"manager-keyboard",16)?900:(!strcmp(argv[3], "documents-build") || !strncmp(argv[3], "project-settings-", 17) ||
                  !strncmp(argv[3], "reset-", 6) || !strncmp(argv[3], "speed-", 6) || !strncmp(argv[3], "timeline-", 9) || !strncmp(argv[3],"adaptive-",9) || !strncmp(argv[3],"series-",7) || !strncmp(argv[3],"inspector-",10) || !strncmp(argv[3],"layouts-",8) || !strncmp(argv[3],"named-",6) || !strncmp(argv[3],"pchip-",6) || !strncmp(argv[3],"units-",6) || !strncmp(argv[3],"analysis-project-",17) || !strncmp(argv[3],"resume-",7) || !strncmp(argv[3],"missing-",8) || !strncmp(argv[3],"mask-",5) || !strncmp(argv[3],"frames-",7) || !strncmp(argv[3],"saved-run-tutorial-",19) || !strncmp(argv[3],"monte-carlo-tutorial-",21) || !strncmp(argv[3],"collision-tutorial-",19) || (!strncmp(argv[3],"pendulum-tutorial-",18) || !strncmp(argv[3],"pendulum-medium-",16)) || !strncmp(argv[3],"spring-tutorial-",16) || !strncmp(argv[3],"material-",9) || !strncmp(argv[3],"contact-world",13) || !strncmp(argv[3],"diagnostic-",11) || !strcmp(argv[3],"logging-c") || !strcmp(argv[3],"logging-phys") || !strcmp(argv[3],"logging-flood") || (!strncmp(argv[3], "dock-", 5) || !strncmp(argv[3], "hierarchy-", 10))
                     ? 120 : !strncmp(argv[3], "native-dialog", 13) ? 180 : 15))) {
                fprintf(stderr, "Workspace self-test timeout: %s after %.3f wall seconds\n",
                        argv[3], ps_clock() - test_started);
                exit_code = 1;
                a->quitting = true;
            } else
                workspace_test_frame(a, argv[3], &test_stage, &exit_code, &capture);
        } else if (workspace_test) {
            if (ps_clock() - test_started > 15) {
                exit_code = 1;
                a->quitting = true;
            } else if (test_stage == 100) {
                open_workspace_path(a, argv[2]);
                if (!a->workspace_open || a->loaded || strcmp(a->workspace, argv[2]))
                    exit_code = 1;
                test_stage = 101;
            } else if (test_stage == 101) {
                char path[4096];
                join(path, sizeof path, argv[2], "notes.txt");
                FILE *file = fopen(path, "w");
                if (!file) {
                    exit_code = 1;
                    a->quitting = true;
                } else {
                    fputs("Workspace text preview\n", file);
                    fclose(file);
                    refresh_workspace_entries(a);
                    add_workspace_path(a, path);
                    preview_workspace_path(a, path);
                    if (a->tab != 8 || !(active_document(a) && strstr(active_document(a)->file.saved, "Workspace text")))
                        exit_code = 1;
                    test_stage = 102;
                }
            } else if (test_stage == 102) {
                a->project_manager = true;
                snprintf(a->manager_parent, sizeof a->manager_parent, "%s", argv[2]);
                snprintf(a->manager_name, sizeof a->manager_name, "created-%u",
                         (unsigned)(ps_clock() * 1000000));
                a->manager_experiment_language = 1;
                a->manager_template = 6;
                a->template_analysis_language = 1;
                test_stage = 103;
            } else if (test_stage == 103) {
                create_managed_project(a);
                char manifest[4096];
                join(manifest, sizeof manifest, a->project, "physim.project");
                if (!a->loaded || a->project_manager || a->tab != 0 || !a->workspace_open ||
                    !exists(manifest) ||
                    !test_source_has(a, "main.phys", "Contacts.boxes") ||
                    !test_source_has(a, "analysis.phys", "Boxpositionen") ||
                    !test_source_has(a, "physim.project", "experiment=main.phys"))
                    exit_code = 1;
                test_stage = 104;
            } else if (test_stage == 104 && idle(a) && !a->library_thread) {
                open_workspace_path(a, argv[2]);
                if (a->loaded || !a->workspace_open || a->project[0] ||
                    strcmp(a->workspace, argv[2]))
                    exit_code = 1;
                test_stage = 105;
            } else if (test_stage == 105) {
                printf("WORKSPACE UI SELF-TEST: %s\n", exit_code ? "FAILED" : "PASSED");
                a->quitting = true;
            }
        } else if(migration_test) {
            if(ps_clock()-test_started>20){exit_code=1;a->quitting=true;}
            else project_migration_test_frame(a,argv[3],&test_stage,&exit_code);
        } else if (settings_test) {
            if (ps_clock() - test_started > 20) {
                exit_code = 1;
                a->quitting = true;
            } else
                test_settings_frame(a, argv[3], &test_stage, &exit_code, &capture);
        } else if (plot_test) {
            if (ps_clock() - test_started > 30) {
                exit_code = 1;
                a->quitting = true;
            } else
                test_plot_frame(a, &test_stage, &exit_code, &capture);
            if (plot_noise) test_pointer_noise(a);
        } else if (batch_test) {
            if (ps_clock() - test_started > 150) {
                fprintf(stderr, "Batch UI timeout, stage %d\n", test_stage);
                exit_code = 1;
                a->quitting = true;
            } else
                test_batch_frame(a, &test_stage, &exit_code, &capture, test_batch_workers);
        } else if (recovery_test) {
            /* Queued SDL events affect widgets in the next draw. Leave a
             * complete frame between actions and observations, including
             * asynchronous native window geometry changes during startup. */
            static bool settle;
            if(settle)settle=false;
            else {
                settle=true;
                if (ps_clock() - test_started > 30) {
                    exit_code = 1;
                    a->quitting = true;
                } else if (test_stage == 80 && frames > 1) {
                    if (!strcmp(recovery_mode, "corrupt")) {
                        char path[4096];
                        autosave_path(a, path, sizeof path);
                        nk_str_append_text_char(&a->experiment.string, "\n/* new draft */\n", 17);
                        a->dirty = true;
                        autosave_tick(a, a->autosave_due + 30);
                        save_project(a);
                        ps_autosave *snapshot = NULL;
                        if (!a->autosave_blocked || a->recovery ||
                            ps_autosave_read(path, &snapshot) != PS_CORRUPT)
                            exit_code = 1;
                        ps_autosave_destroy(snapshot);
                        test_stage = 84;
                    } else {
                        test_key(a, SDLK_S);
                        SDL_Event key = {0};
                        key.type = SDL_EVENT_KEY_DOWN;
                        key.key.windowID = SDL_GetWindowID(window);
                        key.key.key = SDLK_F5;
                        SDL_PushEvent(&key);
                        test_stage = 81;
                    }
                } else if (test_stage == 81) {
                    if (!a->recovery || a->job.running || a->dirty || a->analysis_dirty ||
                        test_source_has(a, "main.c", "autosave main"))
                        exit_code = 1;
                    capture = "recovery.bmp";
                    if (!strcmp(recovery_mode, "cancel")) {
                        SDL_Event quit = {0};
                        quit.type = SDL_EVENT_QUIT;
                        SDL_PushEvent(&quit);
                        test_stage = 84;
                    } else {
                        test_mouse(a, a->recovery_bounds[!strcmp(recovery_mode, "discard")], true);
                        test_stage = 82;
                    }
                } else if (test_stage == 82) {
                    test_mouse(a, a->recovery_bounds[!strcmp(recovery_mode, "discard")], false);
                    test_stage = 83;
                } else if (test_stage == 83) {
                    if (!test_recovery_result(a, strcmp(recovery_mode, "discard") != 0))
                        exit_code = 1;
                    test_stage = 84;
                } else if (test_stage == 84) {
                    printf("RECOVERY SELF-TEST: %s\n", exit_code ? "FAILED" : "PASSED");
                    a->quitting = true;
                }
            }
        } else if (self_test) {
            if (ps_clock() - test_started > 180) {
                status(a, "SELF-TEST: Zeitlimit ueberschritten.");
                exit_code = 1;
                a->quitting = true;
            } else if (test_stage == -1 && !a->job.running) {
                if (a->built || !a->diagnostic_count) {
                    status(a, "SELF-TEST: Compilerfehler wurde nicht erkannt.");
                    exit_code = 1;
                    a->quitting = true;
                } else {
                    jump_to_diagnostic(a, &a->diagnostics[0]);
                    capture = "diagnostics.bmp";
                    test_stage = -2;
                }
            } else if (test_stage == -2) {
                char backup[4096];
                snprintf(backup, sizeof backup, "%s/%s.bak", a->project,
                         test_language_analysis ? analysis_source(a) : experiment_source(a));
                if (!load_editor(test_language_analysis ? &a->analysis : &a->experiment, backup)) {
                    exit_code = 1;
                    a->quitting = true;
                } else {
                    if (test_language_analysis) a->analysis_dirty = true;
                    else a->dirty = true;
                    build_project(a);
                    test_stage = 1;
                }
            } else if (test_stage == 1 && !a->job.running) {
                if (!a->built) {
                    exit_code = 1;
                    a->quitting = true;
                } else {
                    if (test_example == 9) {
                        if (a->parameters.count != 1 ||
                            strcmp(a->parameters.entries[0].name, "dragCoefficient")) {
                            status(a, "SELF-TEST: Experimentparameter fehlen nach dem Build.");
                            exit_code = 1;
                            a->quitting = true;
                        } else {
                            snprintf(a->parameters.selected[0],
                                     sizeof a->parameters.selected[0], "0.6");
                            a->project_settings_dirty = true;
                        }
                    }
                    if (test_example == 18) {
                        bool found_friction = false, found_spin = false;
                        for (uint32_t i = 0; i < a->parameters.count; i++) {
                            if (!strcmp(a->parameters.entries[i].name, "friction")) {
                                snprintf(a->parameters.selected[i],
                                         sizeof a->parameters.selected[i], "0.3");
                                found_friction = true;
                            } else if (!strcmp(a->parameters.entries[i].name, "spin")) {
                                snprintf(a->parameters.selected[i],
                                         sizeof a->parameters.selected[i], "1");
                                found_spin = true;
                            }
                        }
                        if (a->parameters.count != 8 || !found_friction || !found_spin) {
                            status(a, "SELF-TEST: Kugelstossparameter fehlen nach dem Build.");
                            exit_code = 1;
                            a->quitting = true;
                        } else {
                            a->project_settings_dirty = true;
                        }
                    }
                    if (test_example == 19) {
                        bool found_restitution = false;
                        for (uint32_t i = 0; i < a->parameters.count; i++) {
                            if (!strcmp(a->parameters.entries[i].name, "restitution")) {
                                snprintf(a->parameters.selected[i],
                                         sizeof a->parameters.selected[i], "0.5");
                                found_restitution = true;
                            }
                        }
                        if (a->parameters.count != 4 || !found_restitution) {
                            status(a, "SELF-TEST: Boxstossparameter fehlen nach dem Build.");
                            exit_code = 1;
                            a->quitting = true;
                        } else {
                            a->project_settings_dirty = true;
                        }
                    }
                    capture = "editor.bmp";
                    test_stage = 2;
                }
            } else if (test_stage == 2) {
                start_run(a);
                test_stage = 3;
            } else if (test_stage == 3 &&
                       a->simulation_time >= (SDL_getenv("PHYSIM_TEST_LONG")             ? 20
                                              : (test_example == 1 || test_example == 5 || test_example == 10) ? .8
                                              : test_example == 4 || test_example == 7 || test_example == 13 || test_example == 14 || test_example == 16 ? 3.2
                                              : test_example == 3 || test_example == 12  ? 2.5
                                              : test_example == 2 || test_example == 6 || test_example == 18 || test_example == 19 ? 1.5
                                                                                         : .2)) {
                command(a, PS_MSG_PAUSE);
                test_stage = 4;
            } else if (test_stage == 3 && !a->runner.running) {
                exit_code = 1;
                a->quitting = true;
            } else if (test_stage == 4 && a->paused) {
                bool label = false, path = false;
                unsigned velocity_arrows = 0, orientation_lines = 0, contact_points = 0;
                unsigned boxes = 0, planes = 0;
                for (uint32_t i = 0; i < a->scene.count; i++) {
                    label = label || a->scene.objects[i].shape == PS_LABEL;
                    path = path || a->scene.objects[i].shape == PS_POLYLINE;
                    velocity_arrows += a->scene.objects[i].shape == PS_ARROW;
                    orientation_lines += a->scene.objects[i].shape == PS_LINE;
                    contact_points += a->scene.objects[i].shape == PS_POINT;
                    boxes += a->scene.objects[i].shape == PS_BOX;
                    planes += a->scene.objects[i].shape == PS_PLANE;
                }
                if ((!a->language_experiment && !label) ||
                    (test_example == 8 && (a->scene.count != 9 || velocity_arrows != 2 || !label || a->scene.objects[2].parent_id!=101 ||
                                           a->scene.objects[3].shape!=PS_GROUP || a->scene.objects[4].parent_id!=100)) ||
                    (test_example == 9 && (a->scene.count != 5 || !path || !label)) ||
                    (test_example == 10 && (a->scene.count != 9 || contact_points != 2 ||
                                            orientation_lines != 2 || velocity_arrows != 1 ||
                                            !path || !label)) ||
                    (test_example == 11 && (a->scene.count != 3 || boxes != 1 || velocity_arrows != 2)) ||
                    (test_example == 12 && (boxes != 1 || planes != 1 || velocity_arrows != 2 || !contact_points || !label)) ||
                    (test_example == 13 && (boxes != 1 || orientation_lines != 1 || velocity_arrows != 1 || contact_points != 2 || !label)) ||
                    (test_example == 14 && (a->scene.count != 5 || orientation_lines != 1 || planes != 1 || !label)) ||
                    (test_example == 15 && (a->scene.count != 6 || orientation_lines != 1 || planes != 2 || velocity_arrows != 1 || !label)) ||
                    (test_example == 16 && (a->scene.count != 13 || a->scene.point_count != 65 ||
                                            boxes != 3 || orientation_lines != 3 ||
                                            velocity_arrows != 3 || contact_points != 1 || !label)) ||
                    (test_example == 18 && (a->scene.count != 11 || orientation_lines != 2 ||
                                            velocity_arrows != 2 || contact_points != 1 || !label)) ||
                    (test_example == 19 && (boxes != 2 || contact_points != 4 ||
                                            velocity_arrows < 6 || !label)) ||
                    (test_example == 1 && (!path || a->scene.point_count < 3)) ||
                    ((test_example == 7 || test_example == 17) &&
                     (orientation_lines != 18 || contact_points != 1 || velocity_arrows != 3 || !label)) ||
                    (test_example == 2 &&
                     (velocity_arrows < 2 || orientation_lines < 2 || !contact_points)) ||
                    (test_example == 3 && (!boxes || !planes || !contact_points)) ||
                    (test_example == 6 && (boxes != 2 || contact_points != 4 || velocity_arrows < 6)) ||
                    (test_example == 4 &&
                     (!path || a->scene.point_count != 65 || velocity_arrows < 3 || boxes < 3))) {
                    status(a, "SELF-TEST: Erweiterte Szene fehlt im Snapshot.");
                    exit_code = 1;
                    a->quitting = true;
                }
                capture = "simulation.bmp";
                paused_time = a->simulation_time;
                command(a, PS_MSG_STEP);
                test_stage = 5;
            } else if (test_stage == 5 && a->simulation_time > paused_time) {
                if (fabs(a->simulation_time - paused_time - a->dt) > 1e-10) {
                    exit_code = 1;
                    a->quitting = true;
                }
                command(a, PS_MSG_STOP);
                a->stop_at = ps_clock();
                test_stage = 6;
            } else if (test_stage == 6 && idle(a)) {
                if (test_example == 9) {
                    ps_run_reader reader;
                    if (ps_run_open(&reader, a->last_run) != PS_OK)
                        exit_code = 1;
                    else {
                        char selected[96], standard[96];
                        snprintf(selected, sizeof selected, "\nparameter.dragCoefficient=%.17g\n", 0.6);
                        snprintf(standard, sizeof standard,
                                 "\nparameter_default.dragCoefficient=%.17g\n", 0.47);
                        if (!strstr(reader.metadata, selected) ||
                            !strstr(reader.metadata, standard))
                            exit_code = 1;
                        ps_run_reader_close(&reader);
                    }
                }
                if (test_example == 18) {
                    ps_run_reader reader;
                    if (ps_run_open(&reader, a->last_run) != PS_OK)
                        exit_code = 1;
                    else {
                        char friction[96], spin[96];
                        snprintf(friction, sizeof friction, "\nparameter.friction=%.17g\n", 0.3);
                        snprintf(spin, sizeof spin, "\nparameter.spin=%.17g\n", 1.0);
                        if (!strstr(reader.metadata, friction) || !strstr(reader.metadata, spin))
                            exit_code = 1;
                        ps_run_reader_close(&reader);
                    }
                }
                if (test_example == 19) {
                    ps_run_reader reader;
                    if (ps_run_open(&reader, a->last_run) != PS_OK)
                        exit_code = 1;
                    else {
                        if (!strstr(reader.metadata, "\nparameter.restitution=0.5\n") ||
                            !strstr(reader.metadata, "\nparameter_default.restitution=1\n"))
                            exit_code = 1;
                        ps_run_reader_close(&reader);
                    }
                }
                if (a->language_experiment) {
                    char snapshot[4200];
                    snprintf(snapshot, sizeof snapshot, "%s.experiment.phys", a->last_run);
                    size_t length = 0;
                    char *source = load_utf8_text(snapshot, &length);
                    if (!source || strcmp(source, a->saved_source[0]))
                        exit_code = 1;
                    free(source);
                }
                a->tab = 2;
                analyze(a, false);
                test_stage = 7;
            } else if (test_stage == 7 && idle(a) && test_language_analysis) {
                char path[4200];
                uint32_t plots = 0, tables = 0;
                if (test_example == 10 && (a->data.channel_count != 15 ||
                    a->data.stats[7].count == 0 || a->data.stats[7].count >= a->data.total ||
                    a->data.stats[0].count != a->data.total))
                    exit_code = 1;
                if (!a->analysis_report || a->data.total < 10 ||
                    ps_report_describe(a->analysis_report, NULL, NULL, &plots, &tables) != PS_OK ||
                    plots != (test_example == 17 ? 3u : 2u) ||
                    tables != (test_example == 10 || test_example == 8 || test_example == 0 ? 2u : 0u))
                    exit_code = 1;
                bool pendulum_analysis = test_example == 0 || test_example == 8;
                snprintf(path, sizeof path, pendulum_analysis ? "%s.psreport" : "%s-position.svg", a->report);
                if (!exists(path)) exit_code = 1;
                snprintf(path, sizeof path, "%s-%s.csv", a->report,
                         pendulum_analysis ? "pendulum_1" : test_example == 10 ? "sensor"
                         : test_example == 17 ? "position" : "velocity");
                if (!exists(path)) exit_code = 1;
                if (pendulum_analysis) {
                    ps_table_info summary, periods;
                    ps_table_row row;
                    const ps_curve_data *energy;
                    if (ps_report_table_read(a->analysis_report, 0, &summary) != PS_OK ||
                        strcmp(summary.title, "Run comparison") || summary.rows != 1 ||
                        ps_report_row_read(a->analysis_report, 0, 0, &row) != PS_OK ||
                        row.values[0] != (double)a->data.total || row.values[1] < 0 ||
                        ps_report_table_read(a->analysis_report, 1, &periods) != PS_OK ||
                        strcmp(periods.title, "Measured periods") ||
                        periods.rows != (row.values[3] > 0 ? 1u : 0u) ||
                        ps_report_curve_view(a->analysis_report, 1, 0, &energy) != PS_OK ||
                        energy->source_count != a->data.total || fabs(energy->y[0]) > 1e-14)
                        exit_code = 1;
                }
                if (test_example == 10) {
                    snprintf(path, sizeof path, "%s-availability.csv", a->report);
                    if (!exists(path)) exit_code = 1;
                    snprintf(path, sizeof path, "%s-statistics.csv", a->report);
                    if (!exists(path)) exit_code = 1;
                    ps_table_info info;
                    ps_table_row row;
                    if (!a->analysis_report ||
                        ps_report_table_read(a->analysis_report, 0, &info) != PS_OK ||
                        info.rows != 1 ||
                        ps_report_row_read(a->analysis_report, 0, 0, &row) != PS_OK ||
                        row.values[0] != (double)a->data.stats[7].count ||
                        row.values[1] != (double)a->data.total)
                        exit_code = 1;
                }
                snprintf(path, sizeof path, "%s.source.phys", a->report);
                size_t size = 0;
                char *source = load_utf8_text(path, &size);
                if (!source || strcmp(source, a->saved_source[1])) exit_code = 1;
                free(source);
                a->analysis_tab = 0;
                capture = "language-analysis.bmp";
                test_stage = test_example == 10 ? 91 : test_example == 9 ? 80 : 90;
            } else if (test_stage == 80) {
                a->tab = 5;
                test_stage = 81;
            } else if (test_stage == 81) {
                test_mouse(a, a->batch_sweep_bounds, true);
                test_stage = 82;
            } else if (test_stage == 82) {
                test_mouse(a, a->batch_sweep_bounds, false);
                test_stage = 83;
            } else if (test_stage == 83) {
                if (!a->batch_sweep || a->parameters.count != 1 || !a->batch_sweep_ready) {
                    status(a, "SELF-TEST: Parameterstudie wurde nicht aktiviert.");
                    exit_code = 1;
                }
                a->batch_runs = 3;
                a->batch_steps = 1;
                snprintf(a->batch_sweep_start, sizeof a->batch_sweep_start, "0.2");
                snprintf(a->batch_sweep_end, sizeof a->batch_sweep_end, "0.8");
                test_mouse(a, a->batch_start_bounds, true);
                test_stage = 84;
            } else if (test_stage == 84) {
                test_mouse(a, a->batch_start_bounds, false);
                test_stage = 85;
            } else if (test_stage == 85) {
                if (!a->batch_thread && !a->batch_options.runs) {
                    exit_code = 1;
                    a->quitting = true;
                } else if (idle(a) && !a->library_thread) {
                    uint32_t plots = 0, tables = 0;
                    const ps_curve_data *curve = NULL;
                    if (a->batch_code != PS_OK || a->batch_result.completed != 3 ||
                        !a->batch_options.sweep || a->batch_options.sweep_start != 0.2 ||
                        a->batch_options.sweep_end != 0.8 ||
                        !a->analysis_report ||
                        ps_report_describe(a->analysis_report, NULL, NULL, &plots, &tables) != PS_OK ||
                        plots != 1 || tables != 0 ||
                        ps_report_curve_view(a->analysis_report, 0, 0, &curve) != PS_OK ||
                        curve->count != 3 || curve->x[0] != 0.2 || curve->x[2] != 0.8)
                        exit_code = 1;
                    test_stage = 90;
                }
            } else if (test_stage == 91) {
                a->result_view = 3;
                capture = "language-statistics.bmp";
                test_stage = 90;
            } else if (test_stage == 90) {
                open_project(a);
                if (!a->language_analysis || a->language_experiment != (test_example >= 8) ||
                    !test_source_has(a, "analysis.phys", "func analyze():"))
                    exit_code = 1;
                if (test_example == 9) {
                    double saved;
                    if (a->parameters.count != 1 ||
                        ps_parameter_catalog_value(&a->parameters, 0, &saved) != PS_OK ||
                        saved != 0.6 ||
                        !test_source_has(a, "physim.project", "parameter.dragCoefficient="))
                        exit_code = 1;
                }
                if (test_example == 18) {
                    bool found_friction = false, found_spin = false;
                    for (uint32_t i = 0; i < a->parameters.count; i++) {
                        double saved = 0;
                        if (ps_parameter_catalog_value(&a->parameters, i, &saved) != PS_OK)
                            exit_code = 1;
                        if (!strcmp(a->parameters.entries[i].name, "friction"))
                            found_friction = saved == 0.3;
                        if (!strcmp(a->parameters.entries[i].name, "spin"))
                            found_spin = saved == 1;
                    }
                    if (!found_friction || !found_spin ||
                        !test_source_has(a, "physim.project", "parameter.friction=") ||
                        !test_source_has(a, "physim.project", "parameter.spin="))
                        exit_code = 1;
                }
                if (test_example == 19) {
                    bool found_restitution = false;
                    for (uint32_t i = 0; i < a->parameters.count; i++) {
                        double saved = 0;
                        if (ps_parameter_catalog_value(&a->parameters, i, &saved) != PS_OK)
                            exit_code = 1;
                        if (!strcmp(a->parameters.entries[i].name, "restitution"))
                            found_restitution = saved == 0.5;
                    }
                    if (!found_restitution ||
                        !test_source_has(a, "physim.project", "parameter.restitution="))
                        exit_code = 1;
                }
                a->tab = 2;
                a->analysis_tab = 1;
                capture = "language-analysis-editor.bmp";
                test_stage = 19;
            } else if (test_stage == 7 && idle(a)) {
                char report[4096];
                snprintf(report, sizeof report, "%s-plot.svg", a->report);
                if (a->data.total < 10 || a->data.result != PS_EOF || !exists(report))
                    exit_code = 1;
                snprintf(report, sizeof report, "%s-derived.csv", a->report);
                if (!exists(report))
                    exit_code = 1;
                uint32_t plots = 0, tables = 0;
                if (!a->analysis_report ||
                    ps_report_describe(a->analysis_report, NULL, NULL, &plots, &tables) != PS_OK ||
                    plots != (test_example == 0 || test_example == 8 || test_example == 4 || test_example == 5 || test_example == 7 ? 4u : 3u) || !tables)
                    exit_code = 1;
                capture = "analysis.bmp";
                test_stage = 30;
            } else if (test_stage == 30) {
                a->result_view = 1;
                capture = "analysis-scatter.bmp";
                test_stage = 31;
            } else if (test_stage == 31) {
                a->result_view = 2;
                capture = "analysis-histogram.bmp";
                test_stage = 32;
            } else if (test_stage == 32) {
                test_mouse(a, a->result_export_bounds[1], true);
                test_stage = 33;
            } else if (test_stage == 33) {
                test_mouse(a, a->result_export_bounds[1], false);
                test_stage = 34;
            } else if (test_stage == 34) {
                if (!exists(a->result_export) || !strstr(a->result_export, ".svg"))
                    exit_code = 1;
                a->result_view = test_example == 0 || test_example == 8 || test_example == 4 || test_example == 5 || test_example == 7 ? 4 : 3;
                capture = "analysis-table.bmp";
                test_stage = 35;
            } else if (test_stage == 35) {
                test_mouse(a, a->result_export_bounds[0], true);
                test_stage = 36;
            } else if (test_stage == 36) {
                test_mouse(a, a->result_export_bounds[0], false);
                test_stage = 37;
            } else if (test_stage == 37) {
                if (!exists(a->result_export) || !strstr(a->result_export, ".csv"))
                    exit_code = 1;
                a->result_view = test_example == 0 || test_example == 8 || test_example == 4 || test_example == 5 || test_example == 7 ? 5 : 4;
                capture = "analysis-metrics.bmp";
                if (SDL_getenv("PHYSIM_TEST_LONG") && test_example == 0) {
                    ps_table_row metrics;
                    if (ps_report_row_read(a->analysis_report, 1, 0, &metrics) != PS_OK ||
                        metrics.values[0] > 1e-8 || fabs(metrics.values[1] - 2.488805869) > 2e-6)
                        exit_code = 1;
                }
                test_stage = test_example == 0 || test_example == 8 || test_example == 4 || test_example == 7 ? 39 : test_example == 5 ? 41 : 40;
            } else if (test_stage == 41) {
                a->result_view = 3;
                capture = "analysis-sensor.bmp";
                ps_table_row row;
                if (ps_report_row_read(a->analysis_report, 2, 0, &row) != PS_OK ||
                    row.values[0] != (double)a->data.stats[7].count || row.values[1] <= 0 ||
                    row.values[2] <= 0)
                    exit_code = 1;
                test_stage = 42;
            } else if (test_stage == 42) {
                a->show_report = false;
                a->plot_channel = 7;
                capture = "sensor-data.bmp";
                test_stage = 43;
            } else if (test_stage == 43) {
                a->show_report = true;
                a->result_view = 6;
                capture = "sensor-status.bmp";
                test_stage = 44;
            } else if (test_stage == 44) {
                a->result_view = 7;
                capture = "sensor-errors.bmp";
                test_stage = 40;
            } else if (test_stage == 39) {
                a->result_view = 3;
                capture = "analysis-energy.bmp";
                ps_table_row row;
                if (ps_report_row_read(a->analysis_report, 1, 0, &row) != PS_OK ||
                    row.values[0] > (test_example == 8 ? .02 : 1e-7))
                    exit_code = 1;
                if (test_example == 0 || test_example == 8) {
                    const ps_curve_data *energy;
                    ps_plot_info plot;
                    char path[4200];snprintf(path, sizeof path, "%s-energy.csv", a->report);
                    if (!exists(path) ||
                        ps_report_plot_read(a->analysis_report, 3, &plot) != PS_OK ||
                        strcmp(plot.title, "Mechanische Energieänderung") ||
                        plot.y_unit.dimension[0] != 2 || plot.y_unit.dimension[1] != 1 ||
                        plot.y_unit.dimension[2] != -2 ||
                        ps_report_curve_view(a->analysis_report, 3, 0, &energy) != PS_OK ||
                        energy->source_count != a->data.total || fabs(energy->y[0]) > 1e-14)
                        exit_code = 1;
                }
                test_stage = test_example == 0 || test_example == 8 ? 170 : 40;
            } else if (test_stage == 170) {
                test_mouse(a, a->result_export_bounds[1], true);
                test_stage = 171;
            } else if (test_stage == 171) {
                test_mouse(a, a->result_export_bounds[1], false);
                test_stage = 172;
            } else if (test_stage == 172) {
                if (!exists(a->result_export) || !strstr(a->result_export, ".svg"))
                    exit_code = 1;
                test_mouse(a, a->result_export_bounds[2], true);
                test_stage = 173;
            } else if (test_stage == 173) {
                test_mouse(a, a->result_export_bounds[2], false);
                test_stage = 174;
            } else if (test_stage == 174) {
                unsigned char header[24];
                FILE *png = fopen(a->result_export, "rb");
                if (!png || !strstr(a->result_export, ".png") ||
                    fread(header, 1, sizeof header, png) != sizeof header ||
                    memcmp(header, "\x89PNG\r\n\x1a\n", 8) || memcmp(header + 12, "IHDR", 4))
                    exit_code = 1;
                if (png) fclose(png);
                test_stage = 40;
            } else if (test_stage == 40) {
                test_previous_report = a->analysis_report;
                join(a->result_path, sizeof a->result_path, a->project,
                     "invalid-report-test.psreport");
                FILE *invalid = fopen(a->result_path, "wbx");
                if (invalid) {
                    fputs("incomplete report", invalid);
                    fclose(invalid);
                    SDL_SetAtomicInt(&a->report_done, 0);
                    a->report_thread = SDL_CreateThread(load_report, "physim-report-test", a);
                }
                if (!invalid || !a->report_thread)
                    exit_code = 1;
                test_stage = 38;
            } else if (test_stage == 38 && !a->report_thread) {
                if (a->analysis_report != test_previous_report || a->report_result != PS_CORRUPT ||
                    !a->result_error[0])
                    exit_code = 1;
                capture = "analysis-rejected.bmp";
                remove(a->result_path);
                test_stage = 8;
            } else if (test_stage == 8) {
                /* Compiler warnings may keep the log open after a successful build. */
                a->show_log = false;
                a->show_search = false;
                test_key(a, SDLK_1);
                test_key(a, SDLK_F);
                test_key(a, SDLK_L);
                test_stage = 9;
            } else if (test_stage == 9) {
                if (a->tab != 0 || !a->show_search || !a->show_log)
                    exit_code = 1;
                capture = "search.bmp";
                test_key(a, SDLK_L);
                test_mouse(a, a->navigation_bounds[1], true);
                test_stage = 10;
            } else if (test_stage == 10) {
                test_mouse(a, a->navigation_bounds[1], false);
                test_stage = 11;
            } else if (test_stage == 11) {
                if (a->tab != 1 || a->show_log)
                    exit_code = 1;
                a->view_disclosure = NK_MAXIMIZED;
                capture = "inspector.bmp";
                test_stage = 12;
            } else if (test_stage == 12) {
                a->show_log = true;
                capture = "simulation-log.bmp";
                test_stage = 13;
            } else if (test_stage == 13) {
                a->show_log = false;
                test_key(a, SDLK_F1);
                test_stage = 14;
            } else if (test_stage == 14) {
                if (a->tab != 1 || !a->doc_visible || !a->doc_window ||
                    a->doc_window == a->window || !a->documentation || a->doc_topic != 1 ||
                    a->doc_error[0])
                    exit_code = 1;
                capture = "docs-api.bmp";
                test_stage = 15;
            } else if (test_stage == 15) {
                for (int topic = 0; topic < DOCUMENTATION_TOPIC_COUNT; topic++) {
                    open_documentation(a, topic);
                    if (!a->documentation || a->doc_topic != topic || a->doc_error[0])
                        exit_code = 1;
                }
                open_documentation(a, 1);
                documentation_link(a, "language-tutorial.md");
                if (strcmp(documentation_topics[a->doc_topic].path,
                           "docs/language-tutorial.md") || a->doc_error[0])
                    exit_code = 1;
                open_documentation(a, 1);
                documentation_link(a, "measurement.md");
                if (a->doc_topic != DOCUMENTATION_MEASUREMENT_TOPIC || a->doc_error[0])
                    exit_code = 1;
                capture = "docs-measurement.bmp";
                test_stage = 23;
            } else if (test_stage == 23) {
                documentation_link(a, "../include/physim/measurement.h");
                if (a->doc_topic != DOCUMENTATION_MEASUREMENT_HEADER || a->doc_error[0])
                    exit_code = 1;
                capture = "docs-measurement-header.bmp";
                test_stage = 230;
            } else if (test_stage == 230) {
                open_documentation(a, 1);
                documentation_link(a, "reference/math.md");
                if (strcmp(documentation_topics[a->doc_topic].path, "docs/reference/math.md"))
                    exit_code = 1;
                documentation_link(a, "../math.md");
                if (a->doc_topic != 30) exit_code = 1;
                snprintf(a->doc_filter, sizeof a->doc_filter, "ps_sweep_spheres");
                documentation_filter(a);
                int mechanics_topic = -1, collision_reference = -1;
                for (int i = 0; i < DOCUMENTATION_TOPIC_COUNT; ++i) {
                    if (!strcmp(documentation_topics[i].path, "docs/mechanics.md")) mechanics_topic = i;
                    if (!strcmp(documentation_topics[i].path, "docs/reference/collision.md")) collision_reference = i;
                }
                if (mechanics_topic < 0 || collision_reference < 0 ||
                    !documentation_hits[mechanics_topic] || !documentation_hits[collision_reference] ||
                    documentation_hits[1])
                    exit_code = 1;
                open_documentation(a, collision_reference);
                snprintf(a->doc_query, sizeof a->doc_query, "%s", a->doc_filter);
                capture = "docs-global.bmp";
                test_stage = 231;
            } else if (test_stage == 231) {
                if (!a->doc_scroll_y) exit_code = 1;
                a->doc_filter[0] = a->doc_query[0] = 0;
                documentation_filter(a);
                open_documentation(a, 45);
                bool table = false;
                for (size_t i = 0; i < a->documentation->count; i++)
                    if (a->documentation->blocks[i].kind == PS_DOC_TABLE_ROW) {
                        a->doc_jump_block = (int)i;
                        table = true;
                        break;
                    }
                if (!table) exit_code = 1;
                capture = "docs-table.bmp";
                test_stage = 24;
            } else if (test_stage == 24) {
                open_documentation(a, 3);
                test_window_key(a->doc_window, SDLK_F);
                test_stage = 16;
            } else if (test_stage == 16) {
                SDL_Event text_event = {0};
                text_event.type = SDL_EVENT_TEXT_INPUT;
                text_event.text.windowID = SDL_GetWindowID(a->doc_window);
                text_event.text.text = "ps_series_derivative";
                if (!SDL_PushEvent(&text_event)) {
                    fprintf(stderr, "Document text event rejected: %s\n", SDL_GetError());
                    exit_code = 1;
                }
                test_stage = 17;
            } else if (test_stage == 17) {
                test_stage = 18;
            } else if (test_stage == 18) {
                capture = "docs-search.bmp";
                if (!a->documentation || !a->doc_scroll_y ||
                    strcmp(a->doc_query, "ps_series_derivative")) {
                    fprintf(stderr,
                            "Document search failed: events=%u, input bytes=%u, query bytes=%zu, "
                            "scroll=%u\n",
                            doc_input_events, doc_input_bytes, strlen(a->doc_query),
                            a->doc_scroll_y);
                    exit_code = 1;
                }
                test_doc_scroll = a->doc_scroll_y;
                test_doc_window_id = SDL_GetWindowID(a->doc_window);
                /* Documentation shortcuts must not change the main workspace. */
                test_window_key(a->doc_window, SDLK_3);
                test_stage = 25;
            } else if (test_stage == 25) {
                if (a->tab != 1 || !a->doc_visible || a->doc_scroll_y != test_doc_scroll)
                    exit_code = 1;
                test_key(a, SDLK_1);
                SDL_Event close_event = {0};
                close_event.type = SDL_EVENT_WINDOW_CLOSE_REQUESTED;
                close_event.window.windowID = test_doc_window_id;
                SDL_PushEvent(&close_event);
                test_stage = 26;
            } else if (test_stage == 26) {
                if (a->quitting || a->doc_visible || a->tab != 0)
                    exit_code = 1;
                test_key(a, SDLK_F1);
                test_stage = 27;
            } else if (test_stage == 27) {
                if (!a->doc_visible || SDL_GetWindowID(a->doc_window) != test_doc_window_id ||
                    a->tab != 0 || a->doc_topic != 3 || a->doc_scroll_y != test_doc_scroll ||
                    strcmp(a->doc_query, "ps_series_derivative"))
                    exit_code = 1;
                SDL_SetWindowSize(a->doc_window, 760, 540);
                test_stage = 28;
            } else if (test_stage == 28) {
                capture = "docs-small.bmp";
                test_window_key(a->doc_window, SDLK_ESCAPE);
                test_stage = 29;
            } else if (test_stage == 29) {
                if (a->doc_visible || a->quitting)
                    exit_code = 1;
                if (docs_test) {
                    open_documentation(a, 1);
                }
                test_stage = docs_test ? 280 : 50;
            } else if(test_stage==280) {
                test_window_mouse(a->doc_window,a->doc_track_bounds[0],true);test_stage=281;
            } else if(test_stage==281) {
                test_window_mouse(a->doc_window,a->doc_track_bounds[0],false);test_stage=282;
            } else if(test_stage==282) {
                if(a->doc_track!=1 || a->doc_topic!=DOCUMENTATION_C_GUIDE_TOPIC)exit_code=1;
                documentation_link(a,"c-workflow.md");
                if(strcmp(documentation_topics[a->doc_topic].path,"docs/c-workflow.md") || a->doc_track!=1)exit_code=1;
                test_window_mouse(a->doc_window,a->doc_home_bounds,true);test_stage=283;
            } else if(test_stage==283) {
                test_window_mouse(a->doc_window,a->doc_home_bounds,false);test_stage=284;
            } else if(test_stage==284) {
                if(a->doc_topic!=DOCUMENTATION_C_GUIDE_TOPIC || a->doc_track!=1)exit_code=1;
                capture="docs-c-track.bmp";
                test_window_mouse(a->doc_window,a->doc_track_bounds[1],true);test_stage=285;
            } else if(test_stage==285) {
                test_window_mouse(a->doc_window,a->doc_track_bounds[1],false);test_stage=286;
            } else if(test_stage==286) {
                if(a->doc_track!=2 || a->doc_topic!=DOCUMENTATION_PHYSIM_GUIDE_TOPIC)exit_code=1;
                documentation_link(a,"physim-workflow.md");
                if(strcmp(documentation_topics[a->doc_topic].path,"docs/physim-workflow.md") || a->doc_track!=2)exit_code=1;
                documentation_link(a,"reference/language-library.md");
                if(strcmp(documentation_topics[a->doc_topic].path,"docs/reference/language-library.md") || a->doc_track!=2)exit_code=1;
                test_window_mouse(a->doc_window,a->doc_home_bounds,true);test_stage=287;
            } else if(test_stage==287) {
                test_window_mouse(a->doc_window,a->doc_home_bounds,false);test_stage=288;
            } else if(test_stage==288) {
                if(a->doc_topic!=DOCUMENTATION_PHYSIM_GUIDE_TOPIC || a->doc_track!=2)exit_code=1;
                capture="docs-physim-track.bmp";
                /* Closing the main window exits with either learning route open. */
                SDL_Event close_event={0};close_event.type=SDL_EVENT_WINDOW_CLOSE_REQUESTED;
                close_event.window.windowID=SDL_GetWindowID(a->window);SDL_PushEvent(&close_event);
                test_stage=19;
            } else if (test_stage == 50 && idle(a) && !a->library_thread) {
                snprintf(test_original_run, sizeof test_original_run, "%s", a->last_run);
                snprintf(test_original_report, sizeof test_original_report, "%s",
                         a->loaded_report_path);
                test_original_samples = a->data.total;
                a->dt = .01;
                start_run(a);
                test_stage = 51;
            } else if (test_stage == 51 && a->simulation_time >= 1.5) {
                command(a, PS_MSG_STOP);
                a->stop_at = ps_clock();
                test_stage = 52;
            } else if (test_stage == 52 && idle(a) && !a->library_thread) {
                if (!strcmp(a->last_run, test_original_run) || a->data.total < 100)
                    exit_code = 1;
                a->library_kind = 0;
                test_key(a, SDLK_4);
                test_stage = 53;
            } else if (test_stage == 53 && !a->library_thread && a->library_visible_count >= 2) {
                capture = "library-runs.bmp";
                test_library_row = -1;
                for (unsigned i = 0; i < a->library_visible_count; i++) {
                    char path[4096];
                    if (ps_library_path(a->library, a->library_visible_indices[i], path,
                                        sizeof path) == PS_OK &&
                        !strcmp(path, test_original_run))
                        test_library_row = (int)i;
                }
                if (test_library_row < 0) {
                    exit_code = 1;
                    a->quitting = true;
                } else {
                    test_mouse(a, a->library_select_bounds[test_library_row], true);
                    test_stage = 54;
                }
            } else if (test_stage == 54) {
                test_mouse(a, a->library_select_bounds[test_library_row], false);
                test_stage = 55;
            } else if (test_stage == 55) {
                if (a->selected_count != 2)
                    exit_code = 1;
                capture = "library-selected.bmp";
                test_mouse(a, a->library_analyze_bounds, true);
                test_stage = 56;
            } else if (test_stage == 56) {
                test_mouse(a, a->library_analyze_bounds, false);
                test_stage = 57;
            } else if (test_stage == 57 && idle(a) && !a->library_thread) {
                uint32_t plots = 0, tables = 0;
                ps_plot_info plot_info = {0};
                if (ps_report_describe(a->analysis_report, NULL, NULL, &plots, &tables) != PS_OK ||
                    plots != 3 || tables != 2 ||
                    ps_report_plot_read(a->analysis_report, 0, &plot_info) != PS_OK ||
                    plot_info.curves != 2)
                    exit_code = 1;
                capture = "library-comparison.bmp";
                a->result_view = 2;
                test_stage = 72;
            } else if (test_stage == 72) {
                ps_curve_data curve;
                if (ps_report_curve_read(a->analysis_report, 2, 0, &curve) != PS_OK || !curve.count)
                    exit_code = 1;
                capture = "library-difference.bmp";
                test_stage = 58;
            } else if (test_stage == 58) {
                open_project(a);
                test_stage = 59;
            } else if (test_stage == 59 && !a->library_thread) {
                if (a->last_run[0] || a->analysis_report || a->built)
                    exit_code = 1;
                a->library_kind = 1;
                open_library(a);
                test_stage = 60;
            } else if (test_stage == 60 && !a->library_thread && a->library_visible_count >= 2) {
                capture = "library-reports.bmp";
                test_library_row = -1;
                for (unsigned i = 0; i < a->library_visible_count; i++) {
                    char path[4096];
                    if (ps_library_path(a->library, a->library_visible_indices[i], path,
                                        sizeof path) == PS_OK &&
                        !strcmp(path, test_original_report))
                        test_library_row = (int)i;
                }
                if (test_library_row < 0) {
                    exit_code = 1;
                    a->quitting = true;
                } else {
                    test_mouse(a, a->library_open_bounds[test_library_row], true);
                    test_stage = 61;
                }
            } else if (test_stage == 61) {
                test_mouse(a, a->library_open_bounds[test_library_row], false);
                test_stage = 62;
            } else if (test_stage == 62 && idle(a)) {
                uint32_t plots = 0;
                if (strcmp(a->loaded_report_path, test_original_report) ||
                    ps_report_describe(a->analysis_report, NULL, NULL, &plots, NULL) != PS_OK ||
                    plots != (test_example == 0 || test_example == 8 || test_example == 4 || test_example == 5 || test_example == 7 ? 4u : 3u) || a->built)
                    exit_code = 1;
                capture = "library-reopened-report.bmp";
                test_stage = 63;
            } else if (test_stage == 63) {
                a->library_kind = 0;
                open_library(a);
                test_stage = 64;
            } else if (test_stage == 64 && !a->library_thread && a->library_visible_count >= 2) {
                test_library_row = -1;
                for (unsigned i = 0; i < a->library_visible_count; i++) {
                    char path[4096];
                    if (ps_library_path(a->library, a->library_visible_indices[i], path,
                                        sizeof path) == PS_OK &&
                        !strcmp(path, test_original_run))
                        test_library_row = (int)i;
                }
                if (test_library_row < 0) {
                    exit_code = 1;
                    a->quitting = true;
                } else {
                    test_mouse(a, a->library_open_bounds[test_library_row], true);
                    test_stage = 65;
                }
            } else if (test_stage == 65) {
                test_mouse(a, a->library_open_bounds[test_library_row], false);
                test_stage = 66;
            } else if (test_stage == 66 && idle(a)) {
                if (strcmp(a->last_run, test_original_run) ||
                    a->data.total != test_original_samples || a->data.result != PS_EOF ||
                    a->show_report)
                    exit_code = 1;
                capture = "library-reopened-run.bmp";
                test_stage = 67;
            } else if (test_stage == 67) {
                open_library(a);
                test_stage = 68;
            } else if (test_stage == 68 && !a->library_thread) {
                test_key(a, SDLK_F);
                test_stage = 69;
            } else if (test_stage == 69) {
                SDL_Event input = {0};
                input.type = SDL_EVENT_TEXT_INPUT;
                input.text.windowID = SDL_GetWindowID(a->window);
                input.text.text = "nicht-vorhanden";
                SDL_PushEvent(&input);
                test_stage = 70;
            } else if (test_stage == 70) {
                test_stage = 71;
            } else if (test_stage == 71) {
                if (strcmp(a->library_query, "nicht-vorhanden") || a->library_visible_count)
                    exit_code = 1;
                capture = "library-filter.bmp";
                test_stage = 19;
            } else if (test_stage == 20 && frames > 1) {
                test_mouse(a, a->navigation_bounds[1], true);
                test_stage = 21;
            } else if (test_stage == 21) {
                test_mouse(a, a->navigation_bounds[1], false);
                test_stage = 22;
            } else if (test_stage == 22) {
                test_stage = 13;
            } else if (test_stage == 19) {
                if (docs_test && !a->quitting)
                    exit_code = 1;
                printf("APP SELF-TEST: %s\n", exit_code ? "FAILED" : "PASSED");
                a->quitting = true;
            }
        }
        if(docs_noise)test_pointer_noise(a);
        if (trace_test && test_stage != checked_stage)
            fprintf(stderr, "APP TEST TRACE: stage %d -> %d, status %s\n",
                    checked_stage, test_stage, a->status);
        if (exit_code && !previous_exit_code)
            fprintf(stderr, "First workflow failure at stage %d (next %d): %s "
                            "(tab=%d, selected=%u, samples=%llu, report=%d, query=%s)\n",
                    checked_stage, test_stage, a->status, a->tab, a->selected_count,
                    (unsigned long long)a->data.total, a->show_report, a->library_query);
        if (a->profiler) {
            Uint64 now = SDL_GetTicksNS();
            a->profile_frame.work_seconds = (double)(now - profile_phase) / 1e9;
            profile_phase = now;
        }
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        draw_ui(a, w, h);
        preferences_capture(a);
        nk_sdl_update_TextInput(a->ui);
        if (a->profiler) {
            Uint64 now = SDL_GetTicksNS();
            a->profile_frame.ui_seconds = (double)(now - profile_phase) / 1e9;
            profile_phase = now;
        }
        bool profile_rendered = nk_sdl_render(a->ui);
        if (!profile_rendered) {
            fprintf(stderr, "Rendering: %s\n", SDL_GetError());
            exit_code = 1;
            a->quitting = true;
        }
        if (a->profiler) {
            Uint64 now = SDL_GetTicksNS();
            a->profile_frame.render_seconds = (double)(now - profile_phase) / 1e9;
            a->profile_frame.rendered = profile_rendered;
            if (profile_rendered) ps_graphics_ui_stats(graphics, &a->profile_frame.ui);
            profile_phase = now;
        }
        bool doc_capture = capture && (strncmp(capture, "docs-", 5) == 0 ||
                                       strcmp(capture, "batch-documentation.bmp") == 0);
        if (capture && !doc_capture) {
            char path[4096];
            join(path, sizeof path, workspace_state_test ? argv[2] : a->project, capture);
            if (!ps_graphics_capture(graphics, path))
                exit_code = 1;
        }
        if (a->profiler) {
            Uint64 now = SDL_GetTicksNS();
            a->profile_frame.capture_seconds = (double)(now - profile_phase) / 1e9;
            a->profile_frame.captured = capture != NULL;
            profile_phase = now;
        }
        if (!ps_graphics_present(graphics)) {
            exit_code = 1;
            a->quitting = true;
        }
        if (a->profiler) {
            Uint64 now = SDL_GetTicksNS();
            a->profile_frame.present_seconds = (double)(now - profile_phase) / 1e9;
            profile_phase = now;
        }
        char doc_capture_path[4096];
        if (doc_capture)
            join(doc_capture_path, sizeof doc_capture_path, a->project, capture);
        if (!documentation_window_render(a, doc_capture ? doc_capture_path : NULL)) {
            fprintf(stderr, "Documentation rendering: %s\n", SDL_GetError());
            exit_code = 1;
            a->quitting = true;
        }
        if (a->profiler) {
            Uint64 now = SDL_GetTicksNS();
            a->profile_frame.documentation_seconds = (double)(now - profile_phase) / 1e9;
            a->profile_frame.frame_seconds = (double)(now - profile_frame_started) / 1e9;
            a->profile_frame.interval_seconds = (double)(now - profile_previous) / 1e9;
            a->profile_frame.time_seconds = ps_clock() - profile_app_started;
            if (!profile_ready) profile_ready = a->profile_frame.time_seconds;
            a->profile_frame.startup_ready_seconds = profile_ready;
            if (ps_process_usage_self(&a->profile_frame.usage))
                ps_app_profile_record(a->profiler, &a->profile_frame);
            else ps_app_profile_record(a->profiler, NULL);
            profile_previous = now;
        }
        frames++;
        if (smoke && frames >= 8)
            a->quitting = true;
        /* Deterministic slow rendering for the keyboard regression only. */
        SDL_Delay(workspace_state_test && !strcmp(argv[3],"docs-keyboard-delayed")?80:5);
    }
    if (a->dirty || a->analysis_dirty || a->project_settings_dirty)
        save_project(a);
    if (workspace_state_save(a) != PS_OK) {
        fprintf(stderr, "%s\n", a->workspace_state_error);
        exit_code = 1;
    }
    if (a->preferences_writable && a->preferences_path[0]) {
        preferences_capture(a);
        ps_result saved = ps_preferences_write(a->preferences_path, &a->preferences);
        if (saved != PS_OK) {
            fprintf(stderr, "Einstellungen nicht gespeichert: %s\n", ps_result_string(saved));
            exit_code = 1;
        }
    }
    if (a->runner.running) {
        command(a, PS_MSG_STOP);
        double until = ps_clock() + 0.3;
        while (ps_process_poll(&a->runner) && ps_clock() < until)
            ps_sleep(5);
    }
    ps_process_close(&a->runner);
    ps_process_close(&a->job);
    profile_process_finished(a, &a->runner, 0, &a->profile_runner_recorded);
    profile_process_finished(a, &a->job, a->job_kind, &a->profile_job_recorded);
    if (a->batch_thread) {
        SDL_SetAtomicInt(&a->batch_cancel, 1);
        SDL_WaitThread(a->batch_thread, NULL);
    }
    if (batch_test) {
        if (test_stage != 108 || a->batch_code != PS_OK || !a->batch_result.cancelled ||
            a->batch_result.active || !a->batch_result.completed ||
            a->batch_result.completed >= a->batch_options.runs)
            exit_code = 1;
        if (exit_code)
            fprintf(stderr,
                    "Batch UI failed at stage %d, tab=%d, workers=%d, started=%u, completed=%u, "
                    "cancelled=%d, button=(%.1f,%.1f,%.1f,%.1f), status=%s\n",
                    test_stage, a->tab, a->batch_workers, a->batch_result.started,
                    a->batch_result.completed, a->batch_result.cancelled, a->batch_start_bounds.x,
                    a->batch_start_bounds.y, a->batch_start_bounds.w, a->batch_start_bounds.h,
                    a->status);
        printf("BATCH UI SELF-TEST: %s\n", exit_code ? "FAILED" : "PASSED");
    }
    free(a->batch_source);
    if (a->data.thread)
        SDL_WaitThread(a->data.thread, NULL);
    if (a->report_thread)
        SDL_WaitThread(a->report_thread, NULL);
    if (a->import_thread)SDL_WaitThread(a->import_thread,NULL);
    if (a->library_thread)
        SDL_WaitThread(a->library_thread, NULL);
    ps_library_destroy(a->pending_library);
    ps_library_destroy(a->library);
    ps_report_destroy(a->pending_report);
    ps_timeline_destroy(&a->timeline);
    ps_timeline_destroy(&a->data.timeline);
    ps_report_destroy(a->analysis_report);
    if (self_test)
        puts(a->log);
    if (self_test && exit_code)
        fprintf(stderr, "Workflow failed at stage %d: %s (tab=%d, sweep=%d, ready=%d, "
                        "built=%d, idle=%d, runs=%u, button=%.1f/%.1f/%.1f/%.1f)\n",
                test_stage, a->status, a->tab, a->batch_sweep, a->batch_sweep_ready,
                a->built, idle(a), a->batch_options.runs, a->batch_start_bounds.x,
                a->batch_start_bounds.y, a->batch_start_bounds.w, a->batch_start_bounds.h);
    ps_text_document_destroy(&a->project_manifest_snapshot);
    nk_textedit_free(&a->experiment);
    nk_textedit_free(&a->analysis);
    free(a->documentation);
    free(a->saved_source[0]);
    free(a->saved_source[1]);
    free(a->workspaces);
    ps_autosave_destroy(a->recovery);
    ps_workspace_tree_destroy(&a->workspace_tree);
    documents_clear(a);
    documentation_window_destroy(a);
    bool profile_started = a->profiler != NULL;
    ps_app_profile_result profile_result = ps_app_profile_finish(a->profiler);
    if (profile_requested && !profile_started) profile_result.failed = true;
    a->profiler = NULL;
    if (profile_requested)
        fprintf(stderr, "App profiling: %llu frames written, %llu dropped, failed=%u; %llu child records, %llu child records dropped\n",
                (unsigned long long)profile_result.written, (unsigned long long)profile_result.dropped,
                profile_result.failed ? 1u : 0u,
                (unsigned long long)profile_result.process_written,
                (unsigned long long)profile_result.process_dropped);
    nk_sdl_shutdown(a->ui);
    SDL_SetWindowHitTest(window, NULL, NULL);
    free(a);
    ps_graphics_destroy(graphics);
    SDL_DestroyWindow(window);
    SDL_Quit();
    if (self_test)
        printf("APP TEST EXIT: %d\n", exit_code);
    return exit_code;
}
