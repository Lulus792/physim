static void select_workspace_tab(app *a, int tab) {
    dock_show(a,PS_DOCK_WORKSPACE);
    a->project_manager = false;
    a->tab = tab == 0 && !a->loaded && a->document_count ? 8 : tab;
}
typedef enum {
    TOOLBAR_OPEN, TOOLBAR_ADD_FILE, TOOLBAR_ADD_FOLDER, TOOLBAR_NEW, TOOLBAR_SAVE,
    TOOLBAR_WORKSPACES, TOOLBAR_LOG, TOOLBAR_REFRESH, TOOLBAR_LIBRARY, TOOLBAR_BATCH,
    TOOLBAR_SIDEBAR, TOOLBAR_WORKSPACE, TOOLBAR_RESET_LAYOUT, TOOLBAR_INSPECTOR, TOOLBAR_LAYOUTS
} toolbar_action;
typedef struct { const char *text; toolbar_action action; bool enabled; } toolbar_entry;
/* Shared action availability for mouse, keyboard and drawing. */
static unsigned toolbar_entries(app *a, int menu, toolbar_entry entries[9]) {
    bool choose = !a->dialog_pending && idle(a) && !a->library_thread && !a->recovery;
    if(menu == 1) {
        toolbar_entry file[] = {
            {"Ordner öffnen …",TOOLBAR_OPEN,choose},
            {"Datei hinzufügen …",TOOLBAR_ADD_FILE,choose && a->workspace_open},
            {"Ordner hinzufügen …",TOOLBAR_ADD_FOLDER,choose && a->workspace_open},
            {"Neues Projekt …",TOOLBAR_NEW,choose},
            {"Speichern\t" PS_UI_SHORTCUT "S",TOOLBAR_SAVE,(a->loaded || a->document_count) && idle(a)},
            {"Workspaces …",TOOLBAR_WORKSPACES,!a->dialog_pending}
        };
        memcpy(entries,file,sizeof file);return SDL_arraysize(file);
    }
    if(menu == 2) {
        toolbar_entry view[] = {
            {a->show_log?"Protokoll ausblenden\t" PS_UI_SHORTCUT "L":"Protokoll einblenden\t" PS_UI_SHORTCUT "L",TOOLBAR_LOG,true},
            {"Dateibaum aktualisieren",TOOLBAR_REFRESH,a->workspace_open},
            {"Läufe und Berichte\t" PS_UI_SHORTCUT "4",TOOLBAR_LIBRARY,a->loaded},
            {"Monte Carlo",TOOLBAR_BATCH,a->loaded},
            {a->preferences.dock.hidden&1?"Seitenleiste anzeigen":"Seitenleiste ausblenden",TOOLBAR_SIDEBAR,true},
            {"Arbeitsbereich anzeigen",TOOLBAR_WORKSPACE,true},
            {"Panelanordnung zurücksetzen",TOOLBAR_RESET_LAYOUT,true},
            {a->preferences.dock.hidden&8?"Inspektor anzeigen":"Inspektor ausblenden",TOOLBAR_INSPECTOR,true},
            {"Panelanordnungen …",TOOLBAR_LAYOUTS,true}
        };
        memcpy(entries,view,sizeof view);return SDL_arraysize(view);
    }
    return 0;
}
static void toolbar_dispatch(app *a, toolbar_action action) {
    switch(action) {
    case TOOLBAR_OPEN: choose_workspace_path(a,PS_DIALOG_OPEN_FOLDER);break;
    case TOOLBAR_ADD_FILE: choose_workspace_path(a,PS_DIALOG_ADD_FILE);break;
    case TOOLBAR_ADD_FOLDER: choose_workspace_path(a,PS_DIALOG_ADD_FOLDER);break;
    case TOOLBAR_NEW: a->project_manager=true;a->workspace_preview_path[0]=0;break;
    case TOOLBAR_SAVE: save_active_document(a);break;
    case TOOLBAR_WORKSPACES: a->workspace_manager=true;break;
    case TOOLBAR_LOG: a->show_log=!a->show_log;if(a->show_log)dock_show(a,PS_DOCK_LOG);break;
    case TOOLBAR_REFRESH: refresh_workspace_entries(a);break;
    case TOOLBAR_LIBRARY: a->project_manager=false;open_library(a);break;
    case TOOLBAR_BATCH: a->project_manager=false;a->tab=5;break;
    case TOOLBAR_SIDEBAR:
        if(a->preferences.dock.hidden&1)dock_show(a,PS_DOCK_SIDEBAR);
        else ps_dock_hide(&a->preferences.dock,PS_DOCK_SIDEBAR);break;
    case TOOLBAR_WORKSPACE: dock_show(a,PS_DOCK_WORKSPACE);break;
    case TOOLBAR_RESET_LAYOUT: a->preferences.dock=PS_DOCK_DEFAULT;break;
    case TOOLBAR_INSPECTOR:
        if(a->preferences.dock.hidden&8)dock_show(a,PS_DOCK_INSPECTOR);
        else ps_dock_hide(&a->preferences.dock,PS_DOCK_INSPECTOR);break;
    case TOOLBAR_LAYOUTS: a->layout_manager=true;break;
    }
}
static void toolbar_keyboard_close(app *a) {
    a->toolbar_keyboard=false;
    a->toolbar_keyboard_item=-1;a->toolbar_menu=0;
}
static void toolbar_keyboard_move(app *a, int direction, bool edge) {
    toolbar_entry entries[9];int count=(int)toolbar_entries(a,a->toolbar_menu,entries);
    int current=edge?(direction>0?-1:count):a->toolbar_keyboard_item;
    for(int i=0;i<count;i++) {
        current=(current+direction+count)%count;
        if(entries[current].enabled){a->toolbar_keyboard_item=current;return;}
    }
    a->toolbar_keyboard_item=-1;
}
static bool toolbar_keyboard_key(app *a, const SDL_KeyboardEvent *key) {
    bool plain=!(key->mod&(SDL_KMOD_CTRL|SDL_KMOD_GUI|SDL_KMOD_ALT));
    if(key->key==SDLK_F10 && plain && !(key->mod&SDL_KMOD_SHIFT)) {
        if(!key->repeat) {
            if(a->toolbar_keyboard)toolbar_keyboard_close(a);
            else {a->toolbar_keyboard=true;a->toolbar_keyboard_top=0;a->toolbar_keyboard_item=-1;a->toolbar_menu=0;}
        }
        return true;
    }
    if(!a->toolbar_keyboard)return false;
    if(!plain){toolbar_keyboard_close(a);return false;}
    if(key->repeat)return true;
    if(key->key==SDLK_ESCAPE){toolbar_keyboard_close(a);return true;}
    if(key->key==SDLK_LEFT || key->key==SDLK_RIGHT || key->key==SDLK_TAB) {
        int direction=key->key==SDLK_LEFT || (key->key==SDLK_TAB && (key->mod&SDL_KMOD_SHIFT))?-1:1;
        bool opened=a->toolbar_menu!=0;
        a->toolbar_keyboard_top=(a->toolbar_keyboard_top+direction+4)%4;
        a->toolbar_menu=opened && a->toolbar_keyboard_top<2?a->toolbar_keyboard_top+1:0;
        a->toolbar_keyboard_item=-1;
        toolbar_keyboard_move(a,1,true);
    } else if(key->key==SDLK_UP || key->key==SDLK_DOWN || key->key==SDLK_HOME || key->key==SDLK_END) {
        if(a->toolbar_keyboard_top<2) {
            bool opened=a->toolbar_menu!=0;
            a->toolbar_menu=a->toolbar_keyboard_top+1;
            int direction=key->key==SDLK_UP || key->key==SDLK_END?-1:1;
            toolbar_keyboard_move(a,direction,!opened || key->key==SDLK_HOME || key->key==SDLK_END);
        }
    } else if(key->key==SDLK_RETURN || key->key==SDLK_KP_ENTER || key->key==SDLK_SPACE) {
        if(a->toolbar_keyboard_top>=2) {
            int top=a->toolbar_keyboard_top;toolbar_keyboard_close(a);
            if(top==2)open_documentation(a,1);else {a->project_manager=false;open_settings(a);}
        } else if(!a->toolbar_menu) {
            a->toolbar_menu=a->toolbar_keyboard_top+1;toolbar_keyboard_move(a,1,true);
        } else if(a->toolbar_keyboard_item>=0) {
            toolbar_entry entries[9];unsigned count=toolbar_entries(a,a->toolbar_menu,entries);
            int selected=a->toolbar_keyboard_item;
            if((unsigned)selected<count && entries[selected].enabled) {
                toolbar_action action=entries[selected].action;
                toolbar_keyboard_close(a);
                toolbar_dispatch(a,action);
            }
        }
    }
    return true; /* Menu input must never edit a still-focused document. */
}
static bool toolbar_menu_item(struct nk_context *ui, const char *text, bool enabled, bool focused) {
    struct nk_rect bounds;
    enum nk_widget_layout_states state=nk_widget(&bounds,ui);
    bool hover=enabled && state==NK_WIDGET_VALID && nk_input_is_mouse_hovering_rect(&ui->input,bounds);
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    if(hover || focused)nk_fill_rect(canvas,bounds,5,UI_COLOR(selection_active));
    if(focused)nk_stroke_rect(canvas,nk_rect(bounds.x+1,bounds.y+1,bounds.w-2,bounds.h-2),5,2,UI_COLOR(accent));
    const struct nk_user_font *font=ui->style.font;
    const char *shortcut=strchr(text,'\t');
    int length=shortcut?(int)(shortcut-text):(int)strlen(text);
    float y=bounds.y+(bounds.h-font->height)/2;
    nk_draw_text(canvas,nk_rect(bounds.x+10,y,bounds.w-20,font->height),text,length,font,nk_rgba(0,0,0,0),enabled?UI_COLOR(text):muted());
    if(shortcut) {
        ++shortcut;int count=(int)strlen(shortcut);
        float width=font->width(font->userdata,font->height,shortcut,count);
        nk_draw_text(canvas,nk_rect(bounds.x+bounds.w-10-width,y,width+1,font->height),shortcut,count,font,nk_rgba(0,0,0,0),hover || focused?UI_COLOR(text):muted());
    }
    return hover && nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT);
}
static void toolbar_popup(app *a, int previous) {
    struct nk_context *ui=a->ui;
    if(!a->toolbar_menu && !previous)return;
    int menu=a->toolbar_menu?a->toolbar_menu:previous;
    struct nk_rect anchor=a->toolbar_bounds[menu-1];
    float factor=a->preferences.ui_size/16.f;
    a->toolbar_popup_bounds=nk_rect(anchor.x,anchor.y+anchor.h+2,286*factor,(menu==2?296.f:200.f)+(a->preferences.ui_size-16)*(menu==2?9.f:6.f));
    nk_style_push_vec2(ui,&ui->style.window.popup_padding,nk_vec2(5,5));
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(4,4));
    nk_style_push_float(ui,&ui->style.window.rounding,9);
    if(nk_popup_begin(ui,NK_POPUP_STATIC,"application-menu",NK_WINDOW_NO_SCROLLBAR,a->toolbar_popup_bounds)) {
        if(!a->toolbar_menu)nk_popup_close(ui);
        else {
            toolbar_entry entries[9];unsigned count=toolbar_entries(a,menu,entries);
            if(a->toolbar_keyboard && (a->toolbar_keyboard_item<0 || (unsigned)a->toolbar_keyboard_item>=count || !entries[a->toolbar_keyboard_item].enabled))
                toolbar_keyboard_move(a,1,true);
            for(unsigned i=0;i<count;i++) {
                if(menu==1 && i==3) {
                    nk_layout_row_dynamic(ui,8,1);struct nk_rect separator;nk_widget(&separator,ui);
                    nk_stroke_line(nk_window_get_canvas(ui),separator.x+10,separator.y+separator.h/2,separator.x+separator.w-10,separator.y+separator.h/2,1,UI_COLOR(button));
                }
                nk_layout_row_dynamic(ui,26,1);
                struct nk_rect bounds=nk_widget_bounds(ui);
                if(menu==1 && i<5)a->toolbar_item_bounds[i]=bounds;
                if(menu==1 && i==5)a->workspace_menu_bounds=bounds;
                if(menu==2 && i==0)a->toolbar_item_bounds[5]=bounds;
                if(menu==2 && i==3)a->batch_navigation_bounds=bounds;
                if(menu==2 && i>=4)a->dock_menu_bounds[i-4]=bounds;
                bool focused=a->toolbar_keyboard && a->toolbar_keyboard_item==(int)i;
                bool clicked=toolbar_menu_item(ui,entries[i].text,entries[i].enabled,focused);
                if(entries[i].enabled && clicked) {
                    toolbar_action action=entries[i].action;
                    toolbar_keyboard_close(a);nk_popup_close(ui);toolbar_dispatch(a,action);
                    break;
                }
            }
        }
        nk_popup_end(ui);
    }
    nk_style_pop_float(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
}
static void toolbar_view(app *a) {
    struct nk_context *ui = a->ui;
    nk_fill_rect(nk_window_get_canvas(ui), nk_rect(0, 0, nk_window_get_bounds(ui).w, 32), 0,
                 UI_COLOR(header));
    nk_style_push_vec2(ui, &ui->style.button.padding, nk_vec2(7, 2));
    nk_style_push_style_item(ui, &ui->style.button.normal, nk_style_item_color(UI_COLOR(header)));
    nk_menubar_begin(ui);
    float available = nk_window_get_content_region(ui).w;
    nk_layout_row_begin(ui, NK_STATIC, 32, 10);
    nk_layout_row_push(ui, 72);
    nk_label(ui, "Physim", NK_TEXT_CENTERED);
    const char *menus[] = {"Datei", "Ansicht", "Hilfe", "Einstellungen"};
    float factor=a->preferences.ui_size/16.f;
    const float menu_width[] = {58*factor,72*factor,54*factor,116*factor};
    int previous = a->toolbar_popup_menu;
    /* Header actions remain responsive while the popup makes its parent read-only. */
    bool press = nk_input_is_mouse_pressed(&ui->input, NK_BUTTON_LEFT);
    if (press) {
        a->toolbar_keyboard = false;
    }
    int clicked = -1;
    for (int i = 0; i < 4; i++) {
        if (press && nk_input_is_mouse_hovering_rect(&ui->input, a->toolbar_bounds[i]))
            clicked = i;
    }
    if (clicked >= 0) {
        a->toolbar_keyboard = false;
        if (clicked < 2)
            a->toolbar_menu = a->toolbar_menu == clicked + 1 ? 0 : clicked + 1;
        else {
            a->toolbar_menu = 0;
            if (clicked == 2)
                open_documentation(a, 1);
            else {
                a->project_manager = false;
                open_settings(a);
            }
        }
    } else if (a->toolbar_menu &&
               (nk_input_is_key_pressed(&ui->input, NK_KEY_TEXT_RESET_MODE) ||
                (press && !nk_input_is_mouse_hovering_rect(&ui->input, a->toolbar_popup_bounds))))
        a->toolbar_menu = 0;
    else if (a->toolbar_menu && !press &&
             !a->toolbar_keyboard &&
             (ui->input.mouse.delta.x != 0 || ui->input.mouse.delta.y != 0)) {
        for (int i = 0; i < 2; i++) {
            if (nk_input_is_mouse_hovering_rect(&ui->input, a->toolbar_bounds[i]))
                a->toolbar_menu = i + 1;
        }
    }
    for (int i = 0; i < 4; i++) {
        nk_layout_row_push(ui, menu_width[i]);
        struct nk_rect bounds;
        nk_widget(&bounds, ui);
        a->toolbar_bounds[i] = bounds;
        bool selected = a->toolbar_menu == i + 1 || (!a->toolbar_menu && i == 3 && a->tab == 6);
        bool hover = nk_input_is_mouse_hovering_rect(&ui->input, bounds);
        struct nk_command_buffer *canvas = nk_window_get_canvas(ui);
        if (selected || hover)
            nk_fill_rect(canvas, nk_rect(bounds.x + 2, bounds.y + 3, bounds.w - 4, bounds.h - 6), 6,
                         selected ? UI_COLOR(button_active) : UI_COLOR(button_hover));
        if (a->toolbar_keyboard && a->toolbar_keyboard_top == i)
            nk_stroke_rect(canvas, nk_rect(bounds.x + 2, bounds.y + 3, bounds.w - 4, bounds.h - 6),
                           6, 2, UI_COLOR(accent));
        const struct nk_user_font *font = ui->style.font;
        float text_width =
            font->width(font->userdata, font->height, menus[i], (int)strlen(menus[i]));
        nk_draw_text(canvas,
                     nk_rect(bounds.x + (bounds.w - text_width) / 2,
                             bounds.y + (bounds.h - font->height) / 2, text_width + 1,
                             font->height),
                     menus[i], (int)strlen(menus[i]), font, nk_rgba(0, 0, 0, 0), UI_COLOR(text));
    }
    nk_layout_row_push(ui, fmaxf(100, available - 72 - (58+72+54+116)*factor - 175 - 138));
    struct nk_rect path_bounds = nk_widget_bounds(ui);
    nk_label_colored(ui, a->workspace_open ? a->workspace : "Kein Ordner geöffnet", NK_TEXT_LEFT,
                     muted());
    if (a->workspace_open && !ui->current->popup.active && !a->toolbar_menu &&
        nk_input_is_mouse_hovering_rect(&ui->input, path_bounds))
        nk_tooltip(ui, a->workspace);
    nk_layout_row_push(ui, 175);
    nk_label_colored(ui,
                     a->runner.running ? (a->paused ? "Pausiert" : "Simulation läuft")
                     : a->batch_thread ? "Laufserie läuft"
                     : a->job.running  ? "Wird verarbeitet …"
                     : (a->dirty || a->analysis_dirty || a->project_settings_dirty || documents_dirty(a))
                         ? "Ungespeicherte Änderungen"
                         : "Bereit",
                     NK_TEXT_RIGHT, muted());
    struct nk_rect status_bounds = nk_widget_bounds(ui);
    a->window_drag_bounds =
        nk_rect(path_bounds.x, path_bounds.y, status_bounds.x - path_bounds.x, path_bounds.h);
    for (unsigned i = 0; i < 3; i++) {
        nk_layout_row_push(ui, 46);
        window_control(a, i);
    }
    nk_layout_row_end(ui);
    toolbar_popup(a, previous);
    a->toolbar_popup_menu = a->toolbar_menu;
    nk_menubar_end(ui);
    nk_style_pop_style_item(ui);
    nk_layout_row_dynamic(ui, 30, 3);
    const char *tabs[] = {"Entwickeln", "Simulieren", "Auswerten"};
    for (int i = 0; i < 3; i++) {
        bool selected = !a->project_manager && (a->tab == i || (a->tab == 8 && i == 0));
        struct nk_rect bounds;
        enum nk_widget_layout_states state = nk_widget(&bounds, ui);
        a->navigation_bounds[i] = bounds;
        bool hovered = nk_input_is_mouse_hovering_rect(&ui->input, bounds);
        if (state == NK_WIDGET_VALID && !a->toolbar_menu && hovered &&
            nk_input_is_mouse_pressed(&ui->input, NK_BUTTON_LEFT))
            select_workspace_tab(a, i);
        struct nk_command_buffer *canvas = nk_window_get_canvas(ui);
        struct nk_rect surface = nk_rect(bounds.x + 5, bounds.y + 3, bounds.w - 10, bounds.h - 6);
        if (selected || hovered)
            nk_fill_rect(canvas, surface, 8,
                         selected ? UI_COLOR(button_hover) : UI_COLOR(button));
        const struct nk_user_font *font = ui->style.font;
        float text_width = font->width(font->userdata, font->height, tabs[i], (int)strlen(tabs[i]));
        nk_draw_text(canvas,
                     nk_rect(bounds.x + (bounds.w - text_width) / 2,
                             bounds.y + (bounds.h - font->height) / 2, text_width + 1,
                             font->height),
                     tabs[i], (int)strlen(tabs[i]), font, nk_rgba(0, 0, 0, 0),
                     selected ? UI_COLOR(text) : UI_COLOR(muted));
    }
    nk_style_pop_vec2(ui);
}
