/* Approved Settings A. Immutable font textures are borrowed from the main UI;
 * the task owns its window, GL context, input and accessibility surface. */
static void channel_unit_open(app *a,const ps_channel *ch);
static void channel_unit_open_from(app *a,const ps_channel *ch,SDL_Window *source,const char *label);
static ps_display_unit channel_display_unit(app *a,const ps_channel *ch);
enum {PS_PROTOCOL_INITIAL_HEIGHT=1,PS_PROTOCOL_INITIAL_OPEN=2};
/* The common log retains its current cross-workspace wish. Explicit starting
 * values wait for Develop, unless a later resize/toggle supersedes them. */
static void settings_protocol_apply(app *a) {
    if(!workbench_visible(a) || a->protocol_view.resize)return;
    if(a->protocol_initial_pending&PS_PROTOCOL_INITIAL_HEIGHT)a->preferences.log_height=a->protocol_initial_height;
    if(a->protocol_initial_pending&PS_PROTOCOL_INITIAL_OPEN){a->show_log=a->protocol_initial_open;a->preferences.view_flags=(a->preferences.view_flags&~PS_VIEW_LOG)|(a->show_log?PS_VIEW_LOG:0);}
    a->protocol_initial_pending=0;
}
static const char *settings_groups[]={"Darstellung","Editor & Sicherung","Simulation","Arbeitsfläche","Anzeigeeinheiten"};
static const char *settings_notes[]={"Theme und lesbare Oberfläche.","Code lesen und Änderungen sichern.","Sichtbarkeit und Projektion.","Persönliche Größen für Entwickeln.","Eigene Aufgabe je Messkanal."};
typedef struct {const char *title,*help,*tags;unsigned group,focus;uint32_t flag;} settings_field;
static const settings_field settings_fields[]={
    {"Darstellung","Für Navigation, Editor, Protokoll und Diagramme. Szenenobjekte behalten ihre Farben.","theme dunkel hell kontrast",0,4,0},
    {"Schriftgröße der Oberfläche","Unabhängig vom Code. Die Vorschau zeigt Menüs und Beschriftungen in der gewählten Größe.","ui schrift font lesbarkeit",0,0,0},
    {"Schriftgröße im Code-Editor","Gilt für die Projekteditoren und das Protokoll. Ein eigener F1-Lesezoom bleibt unabhängig.","font schrift editor",1,5,0},
    {"Automatische Sicherung","Sichert ungespeicherte Entwürfe. Verändert keine Quelldatei und ersetzt bewusstes Speichern nicht.","autosave backup sicherung wiederherstellung",1,6,0},
    {"Vektoren anzeigen","Pfeile sichtbar machen. Messwerte und Berechnung bleiben unverändert.","",2,7,PS_VIEW_VECTORS},
    {"Flugbahnen anzeigen","Aufgezeichnete Pfade in der Szene darstellen.","",2,8,PS_VIEW_PATHS},
    {"Punkte anzeigen","Punktgeometrie ein- oder ausblenden.","",2,9,PS_VIEW_POINTS},
    {"Beschriftungen anzeigen","Objekttexte und Modellbeschriftungen sichtbar machen.","",2,10,PS_VIEW_LABELS},
    {"Raster und Achsen","Räumliche Orientierung ein- oder ausblenden.","",2,11,PS_VIEW_GRID},
    {"Orthografische Ansicht","Parallele Projektion für die Betrachtung. Ändert keine Modellgeometrie.","",2,12,PS_VIEW_ORTHOGRAPHIC},
    {"Protokoll geöffnet","Persönliche Sichtbarkeit. Die gemeinsame Protokollgestaltung C bleibt erhalten.","",3,13,PS_VIEW_LOG},
    {"Seitenleistenbreite","Gespeicherter Größenwert für den dargestellten Bereich Entwickeln.","größe seite panel",3,14,0},
    {"Inspektorbreite","Unabhängig von der linken Seitenleiste. Feste Panelplätze bleiben erhalten.","größe panel",3,15,0},
    {"Protokollhöhe","Gespeicherter Ausgangswert. Der verfügbare Platz im Arbeitsfenster begrenzt die tatsächliche Höhe.","größe log",3,16,0}
};
typedef struct {struct nk_color panel,header,field,button,border,text,muted,accent,primary,on_primary,selected,edge,warn,error;} settings_colors;
static settings_colors settings_theme(unsigned theme) {
    if(theme==PS_THEME_LIGHT)return (settings_colors){nk_rgb(246,247,249),nk_rgb(237,239,243),nk_rgb(255,255,255),nk_rgb(224,227,234),nk_rgb(190,195,206),nk_rgb(32,36,44),nk_rgb(85,94,108),nk_rgb(23,86,158),nk_rgb(21,86,172),nk_rgb(255,255,255),nk_rgba(90,108,130,28),nk_rgb(101,120,141),nk_rgb(128,85,9),nk_rgb(165,41,37)};
    if(theme==PS_THEME_HIGH_CONTRAST)return (settings_colors){nk_rgb(0,0,0),nk_rgb(8,8,8),nk_rgb(0,0,0),nk_rgb(24,24,24),nk_rgb(255,255,255),nk_rgb(255,255,255),nk_rgb(238,238,238),nk_rgb(255,244,79),nk_rgb(255,244,79),nk_rgb(0,0,0),nk_rgba(255,244,79,31),nk_rgb(255,244,79),nk_rgb(255,244,79),nk_rgb(255,180,172)};
    return (settings_colors){nk_rgb(30,31,35),nk_rgb(37,38,43),nk_rgb(25,26,30),nk_rgb(53,54,62),nk_rgb(59,61,69),nk_rgb(237,238,242),nk_rgb(166,169,177),nk_rgb(138,186,255),nk_rgb(25,101,206),nk_rgb(255,255,255),nk_rgba(170,184,202,38),nk_rgb(171,185,202),nk_rgb(235,194,121),nk_rgb(255,170,162)};
}
static ps_preferences settings_current(app *a) {
    preferences_capture(a);ps_preferences current=a->preferences;
    const ps_layout_state *regular=a->analysis_layout_active?&a->analysis_previous:a->laboratory_active?&a->laboratory_previous:NULL;
    if(regular){current.dock=regular->dock;current.sidebar_width=regular->sidebar_width;current.inspector_width=regular->inspector_width;
        }
    if(a->protocol_initial_pending&PS_PROTOCOL_INITIAL_HEIGHT)current.log_height=a->protocol_initial_height;
    if(a->protocol_initial_pending&PS_PROTOCOL_INITIAL_OPEN)current.view_flags=(current.view_flags&~PS_VIEW_LOG)|(a->protocol_initial_open?PS_VIEW_LOG:0);
    return current;
}
static void settings_numbers_read(app *a) {
    for(unsigned i=0;i<3;i++)if(a->settings_edited_numbers&(1u<<i)){
        uint32_t number=0;bool valid=*a->settings_numbers[i]!=0;
        for(const char *p=a->settings_numbers[i];*p;p++){if(*p<'0' || *p>'9' || number>(UINT32_MAX-(unsigned)(*p-'0'))/10){valid=false;break;}number=number*10+(unsigned)(*p-'0');}
        uint32_t *target=i==0?&a->settings_draft.sidebar_width:i==1?&a->settings_draft.inspector_width:&a->settings_draft.log_height;
        if(valid)*target=number;unsigned min=i==0?140:i==1?180:126,max=i==2?500:480;
        if(valid && number>=min && number<=max)a->settings_invalid_numbers&=~(1u<<i);else a->settings_invalid_numbers|=1u<<i;
    }
}
static uint32_t settings_changes(app *a) {
    settings_numbers_read(a);
    ps_preferences current=settings_current(a),merged;
    uint32_t intent=ps_preferences_settings_changes(&a->settings_base,&a->settings_draft);
    if(ps_preferences_settings_merge(&current,&a->settings_base,&a->settings_draft,&merged)==PS_OK)
        intent=ps_preferences_settings_changes(&current,&merged);
    for(unsigned i=0;i<3;i++)if(a->settings_invalid_numbers&(1u<<i))intent|=1u<<(PS_SETTING_SIDEBAR+i);
    return intent;
}
static void settings_numbers_sync(app *a) {
    snprintf(a->settings_numbers[0],16,"%u",a->settings_draft.sidebar_width);
    snprintf(a->settings_numbers[1],16,"%u",a->settings_draft.inspector_width);
    snprintf(a->settings_numbers[2],16,"%u",a->settings_draft.log_height);a->settings_invalid_numbers=a->settings_edited_numbers=0;
}
static void settings_window_destroy(app *a) {
    if(!a->settings_window){a->settings_close_pending=false;return;}
    if(a->settings_graphics){ps_graphics_make_current(a->settings_graphics);if(a->settings_ui)nk_sdl_shutdown(a->settings_ui);ps_graphics_destroy(a->settings_graphics);}
    SDL_DestroyWindow(a->settings_window);
    a->settings_window=NULL;a->settings_ui=NULL;a->settings_graphics=NULL;a->settings_close_pending=false;
    ps_graphics_make_current(a->graphics);
}
static void settings_close(app *a) {
    if(!a->settings_open)return;
    SDL_Window *source=SDL_GetWindowFromID(a->settings_source_id);
    bool return_focus=source && SDL_GetKeyboardFocus()==a->settings_window && !(SDL_GetWindowFlags(source)&(SDL_WINDOW_HIDDEN|SDL_WINDOW_MINIMIZED));
    a->settings_open=false;a->settings_close_pending=true;a->settings_keyboard=false;a->settings_apply_requested=false;
    SDL_HideWindow(a->settings_window);if(return_focus)SDL_RaiseWindow(source);
}
static void open_settings(app *a) {
    if(a->settings_open){SDL_ShowWindow(a->settings_window);SDL_RaiseWindow(a->settings_window);return;}
    if(a->settings_close_pending)settings_window_destroy(a);
    int w,h,x,y;SDL_GetWindowSize(a->window,&w,&h);SDL_GetWindowPosition(a->window,&x,&y);
    a->settings_source_id=SDL_GetWindowID(a->window);
    if(!ps_graphics_make_current(a->graphics) || !SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT,1))goto fail;
    a->settings_window=SDL_CreateWindow("Physim | Einstellungen",(int)fminf(1326,fmaxf(640,w-60)),(int)fminf(820,fmaxf(480,h-60)),SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE|SDL_WINDOW_HIGH_PIXEL_DENSITY|SDL_WINDOW_HIDDEN);
    if(a->settings_window)a->settings_graphics=ps_graphics_create(a->settings_window);
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT,0);
    if(!a->settings_graphics)goto fail;
    SDL_GL_SetSwapInterval(0);a->settings_ui=nk_sdl_init(a->settings_window,a->settings_graphics);if(!a->settings_ui)goto fail;
    nk_sdl_borrow_fonts(a->settings_ui,a->ui);nk_input_begin(a->settings_ui);nk_input_end(a->settings_ui);
    a->settings_base=a->settings_draft=settings_current(a);settings_numbers_sync(a);
    a->settings_previous_tab=a->tab;a->settings_from_analysis=a->analysis_layout_active;a->settings_defaults=false;
    a->settings_search[0]=0;a->settings_group=0;a->settings_review=false;a->settings_open=true;
    a->settings_focus=17;a->settings_keyboard=a->settings_focus_scroll=a->settings_initial_focus=true;
    a->settings_popup_close=false;nk_sdl_accessibility_keyboard_begin(a->settings_ui);
    if(a->preferences_writable)a->preferences_error[0]=0;
    SDL_SetWindowMinimumSize(a->settings_window,360,360);SDL_SetWindowPosition(a->settings_window,x+30,y+30);
    SDL_ShowWindow(a->settings_window);SDL_RaiseWindow(a->settings_window);ps_graphics_make_current(a->graphics);return;
fail:
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT,0);settings_window_destroy(a);
    status(a,"Das Einstellungsfenster konnte nicht geöffnet werden.");
}
static void settings_commit(app *a) {
    if(!a->settings_open)return;
    settings_numbers_read(a);
    if(a->settings_invalid_numbers){snprintf(a->preferences_error,sizeof a->preferences_error,"Ganzzahlige Größen eingeben: Seite 140–480, Inspektor 180–480, Protokoll 126–500 px.");return;}
    ps_preferences current=settings_current(a),next;
    if(ps_preferences_settings_merge(&current,&a->settings_base,&a->settings_draft,&next)!=PS_OK){snprintf(a->preferences_error,sizeof a->preferences_error,"Ungültige Einstellungen. Größen und Werte prüfen; der Entwurf bleibt erhalten.");return;}
    if(!settings_changes(a) && a->preferences_writable)return;
    uint32_t changed=ps_preferences_settings_changes(&current,&next);
    if(changed&(1u<<PS_SETTING_LOG)){if(next.view_flags&PS_VIEW_LOG)ps_dock_reveal(&next.dock,PS_DOCK_LOG,PS_DOCK_BOTTOM);else ps_dock_hide(&next.dock,PS_DOCK_LOG);}
    if(!documentation_fonts_prepare_sizes(a,a->doc_zoom_size?a->doc_zoom_size:next.ui_size,a->doc_zoom_size?a->doc_zoom_size:next.editor_size,next.ui_size,next.editor_size)){
        snprintf(a->preferences_error,sizeof a->preferences_error,"Schriftgröße konnte nicht geladen werden. Einstellungen bleiben erhalten.");return;}
    ps_result result=*a->preferences_path?ps_preferences_write(a->preferences_path,&next):PS_IO;
    if(result!=PS_OK){snprintf(a->preferences_error,sizeof a->preferences_error,"Speichern fehlgeschlagen: %s. Deine bisherigen Einstellungen bleiben erhalten.",ps_result_string(result));return;}
    ps_layout_state session={a->preferences.dock,a->preferences.sidebar_width,a->preferences.inspector_width,a->preferences.log_height,a->show_log};
    bool other=a->laboratory_active || a->analysis_layout_active;
    preferences_apply(a,&next);
    if(other){ps_layout_state *regular=a->analysis_layout_active?&a->analysis_previous:&a->laboratory_previous;
        if(changed&(1u<<PS_SETTING_LOG_HEIGHT)){a->protocol_initial_height=next.log_height;a->protocol_initial_pending|=PS_PROTOCOL_INITIAL_HEIGHT;}
        if(changed&(1u<<PS_SETTING_LOG)){a->protocol_initial_open=(next.view_flags&PS_VIEW_LOG)!=0;a->protocol_initial_pending|=PS_PROTOCOL_INITIAL_OPEN;}
        *regular=(ps_layout_state){next.dock,next.sidebar_width,next.inspector_width,next.log_height,(next.view_flags&PS_VIEW_LOG)!=0};
        a->preferences.dock=session.dock;a->preferences.sidebar_width=session.sidebar_width;a->preferences.inspector_width=session.inspector_width;
        a->preferences.log_height=session.log_height;a->show_log=session.show_log;
        a->preferences.view_flags=(a->preferences.view_flags&~PS_VIEW_LOG)|(session.show_log?PS_VIEW_LOG:0);}
    a->preferences_writable=true;a->preferences_error[0]=0;settings_close(a);status(a,"Einstellungen gespeichert.");
}
static void settings_defaults_draft(app *a) {
    a->settings_defaults=true;ps_preferences_settings_defaults(&a->settings_draft);settings_numbers_sync(a);
}
static bool settings_match(app *a,unsigned index) {
    const settings_field *field=&settings_fields[index];if(!*a->settings_search)return field->group==a->settings_group;
    return SDL_strcasestr(field->title,a->settings_search) || SDL_strcasestr(field->help,a->settings_search) || SDL_strcasestr(field->tags,a->settings_search) || SDL_strcasestr(settings_groups[field->group],a->settings_search);
}
static bool settings_units_match(app *a) {
    return *a->settings_search?SDL_strcasestr("Anzeigeeinheiten Messkanal Einheit meter centimeter kmh",a->settings_search)!=NULL:a->settings_group==4;
}
static const ps_channel *settings_channel(app *a) {
    if(a->channel_count)return &a->live_channels[a->plot_channel>=0 && (uint32_t)a->plot_channel<a->channel_count?(unsigned)a->plot_channel:0];
    if(!a->data.thread && a->data.channel_count)return &a->data.channels[0];
    return NULL;
}
static bool settings_search_no_results(app *a) {
    if(!*a->settings_search || settings_units_match(a))return false;
    for(unsigned i=0;i<14;i++)if(settings_match(a,i))return false;
    return true;
}
static void settings_search_clear(app *a) {
    a->settings_search[0]=0;a->settings_focus=17;
    a->settings_keyboard=a->settings_focus_scroll=true;
    nk_sdl_accessibility_keyboard_begin(a->settings_ui);
}
static int settings_order(app *a,int ids[29]) {
    int n=0;ids[n++]=17;if(*a->settings_search)ids[n++]=26;
    if(!a->settings_review){for(int i=20;i<25;i++)ids[n++]=i;for(unsigned i=0;i<14;i++)if(settings_match(a,i))ids[n++]=settings_fields[i].focus;if(settings_units_match(a) && settings_channel(a))ids[n++]=27;if(settings_search_no_results(a))ids[n++]=28;}
    if(a->settings_review)ids[n++]=25;ids[n++]=3;ids[n++]=2;if(settings_changes(a) || !a->preferences_writable){ids[n++]=18;ids[n++]=1;}ids[n++]=19;return n;
}
static const char *settings_focus_name(int id) {
    static const char *names[]={"Schriftgröße der Oberfläche","Übernehmen","Abbrechen","Standardwerte","Darstellung","Schriftgröße im Code-Editor","Automatische Sicherung","Vektoren anzeigen","Flugbahnen anzeigen","Punkte anzeigen","Beschriftungen anzeigen","Raster und Achsen","Orthografische Ansicht","Protokoll geöffnet","Seitenleistenbreite","Inspektorbreite","Protokollhöhe","Einstellungen suchen","Änderungen prüfen","Einstellungen schließen","Darstellung","Editor & Sicherung","Simulation","Arbeitsfläche","Anzeigeeinheiten","Zurück","Suche löschen","Anzeigeeinheit bearbeiten …","Suche zurücksetzen"};
    return names[id];
}
static void settings_field_flush(app *a) {
    if(a->settings_focus==17)ps_ui_flush_edit(a->settings_ui,"settings-search",a->settings_search,sizeof a->settings_search,a->settings_focus_bounds[17]);
    else if(a->settings_focus>=14 && a->settings_focus<=16){unsigned i=a->settings_focus-14;char before[16];memcpy(before,a->settings_numbers[i],16);
        ps_ui_flush_edit(a->settings_ui,"settings-body",a->settings_numbers[i],16,a->settings_focus_bounds[a->settings_focus]);
        if(strcmp(before,a->settings_numbers[i]))a->settings_edited_numbers|=1u<<i;settings_numbers_read(a);}
}
static bool settings_keyboard_key(app *a,const SDL_KeyboardEvent *key) {
    if(!a->settings_open || key->mod&(SDL_KMOD_CTRL|SDL_KMOD_GUI|SDL_KMOD_ALT))return false;
    if(key->key==SDLK_ESCAPE){struct nk_window *body=nk_window_find(a->settings_ui,"settings-body");if(body && body->popup.active)a->settings_popup_close=true;else if(a->settings_review){a->settings_review=false;a->settings_focus=18;a->settings_keyboard=true;a->settings_focus_scroll=true;}else settings_close(a);return true;}
    if(key->key==SDLK_TAB){settings_field_flush(a);int ids[29],n=settings_order(a,ids),at=-1,d=key->mod&SDL_KMOD_SHIFT?-1:1;for(int i=0;i<n;i++)if(ids[i]==a->settings_focus)at=i;
        a->settings_focus=ids[(at+d+n)%n];a->settings_keyboard=a->settings_focus_scroll=true;nk_sdl_accessibility_keyboard_begin(a->settings_ui);return true;}
    if(!a->settings_keyboard || a->settings_focus==17 || (a->settings_focus>=14 && a->settings_focus<=16))return false;
    int id=a->settings_focus,d=key->key==SDLK_LEFT || key->key==SDLK_UP?-1:key->key==SDLK_RIGHT || key->key==SDLK_DOWN?1:0;
    bool activate=key->key==SDLK_RETURN || key->key==SDLK_KP_ENTER || key->key==SDLK_SPACE;
    if(!activate && !d)return false;if(key->repeat && activate)return true;
    ps_preferences *p=&a->settings_draft;
    if(id==1 && activate)a->settings_apply_requested=true;
    else if((id==2 || id==19) && activate)settings_close(a);
    else if(id==3 && activate)settings_defaults_draft(a);
    else if(id==18 && activate){a->settings_review=!a->settings_review;a->settings_focus_scroll=true;}
    else if(id==25 && activate){a->settings_review=false;a->settings_focus=18;a->settings_focus_scroll=true;}
    else if((id==26 || id==28) && activate)settings_search_clear(a);
    else if(id==27 && activate && settings_channel(a))channel_unit_open_from(a,settings_channel(a),a->settings_window,settings_focus_name(27));
    else if(id>=20 && id<=24 && (activate || d)){a->settings_group=(id-20+(d<0?4:d>0?1:0))%5;a->settings_focus=20+a->settings_group;a->settings_search[0]=0;}
    else if(id==0 || id==5){uint32_t *size=id==0?&p->ui_size:&p->editor_size;unsigned min=id==0?16:12,count=id==0?4:6;*size=min+2*(((*size-min)/2+(d<0?count-1:1))%count);}
    else if(id==4)p->theme=(p->theme+(d<0?2:1))%3;
    else if(id==6){const uint32_t secs[]={10,30,60,120};unsigned at=0;while(at<3 && secs[at]!=p->autosave_seconds)at++;p->autosave_seconds=secs[(at+(d<0?3:1))%4];}
    else if(id>=7 && id<=13 && activate)p->view_flags^=1u<<(id-7);
    else return false;
    return true;
}
static void settings_tick(app *a) {
    if(a->settings_close_pending)settings_window_destroy(a);
    if(a->settings_apply_requested){a->settings_apply_requested=false;settings_field_flush(a);settings_commit(a);}
}
static void settings_activate_focus(app *a) {
    struct nk_context *ui=a->settings_ui;
    if(!a->settings_keyboard && !nk_sdl_accessibility_has_focus(ui))return;
    int id=a->settings_focus;const char *name=id==17 || id==26?"settings-search":id==19?"settings-header":id==1 || id==2 || id==3 || id==18?"settings-footer":"settings-body";
    struct nk_window *owner=nk_window_find(ui,name);if(!owner || owner->popup.active)return;
    /* Like the handbook overlay, raise only the explicitly focused pane.
     * nk_window_set_focus alone leaves its old ROM input flag intact. */
    nk_window_set_focus(ui,name);owner->flags&=~(nk_flags)NK_WINDOW_ROM;
    if(ui->current==owner && owner->layout)owner->layout->flags&=~(nk_flags)NK_WINDOW_ROM;
}
static bool settings_event(app *a,const SDL_Event *event,SDL_Window *window) {
    if(!a->settings_window || window!=a->settings_window)return false;
    if(event->type==SDL_EVENT_WINDOW_CLOSE_REQUESTED){settings_close(a);return true;}
    if(!a->settings_open)return true;
    if(event->type==SDL_EVENT_WINDOW_RESIZED)a->settings_focus_scroll=true;
    if(event->type==SDL_EVENT_KEY_DOWN && settings_keyboard_key(a,&event->key))return true;
    if(nk_sdl_accessibility_event(a->settings_ui,event))return true;
    SDL_Event copy=*event;nk_sdl_handle_event(a->settings_ui,&copy);return true;
}
#include "settings_a_ui.inc"
