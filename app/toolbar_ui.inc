static size_t workspace_run_count(const app *a) {
    size_t count=0;
    if(a->library)for(size_t i=0;i<a->library->count;i++)count+=a->library->items[i].kind==PS_LIBRARY_RUN;
    return count;
}
static void select_workspace_tab(app *a, int tab) {
    dock_show(a,PS_DOCK_WORKSPACE);
    a->project_manager = false;
    if(tab==0 && a->file_tabs_initialized && file_tab_visible(a,a->file_tab_last_active)){
        file_tab_select(a,a->file_tab_last_active);return;
    }
    if(tab==2)a->analysis_tab=0;
    a->tab = tab == 0 && !a->loaded && a->document_count ? 8 : tab;
}
static int workspace_active_area(const app *a){
    if(a->project_manager)return -1;
    if(workbench_visible(a))return 0;
    return a->tab==1?1:a->tab==2?2:-1;
}
typedef enum {
    TOOLBAR_OPEN, TOOLBAR_ADD_FILE, TOOLBAR_ADD_FOLDER, TOOLBAR_NEW, TOOLBAR_SAVE,
    TOOLBAR_WORKSPACES, TOOLBAR_LOG, TOOLBAR_REFRESH, TOOLBAR_LIBRARY, TOOLBAR_BATCH,
    TOOLBAR_SIDEBAR, TOOLBAR_WORKSPACE, TOOLBAR_RESET_LAYOUT, TOOLBAR_INSPECTOR, TOOLBAR_LAYOUTS,
    TOOLBAR_RELOAD, TOOLBAR_SEARCH, TOOLBAR_BUILD, TOOLBAR_HELP, TOOLBAR_REFERENCE, TOOLBAR_COMPLETION, TOOLBAR_BUILD_SETTINGS,TOOLBAR_SETTINGS,TOOLBAR_REFERENCE_CLOSE
} toolbar_action;
typedef struct { const char *text; toolbar_action action; bool enabled; } toolbar_entry;
/* Bounds keep stable semantic slots; presentation follows the familiar IDE order. */
static const unsigned toolbar_top_order[]={0,4,1,5,2};
static int toolbar_next_top(int current,int direction) {
    for(unsigned i=0;i<SDL_arraysize(toolbar_top_order);i++)
        if(toolbar_top_order[i]==(unsigned)current)return (int)toolbar_top_order[(i+SDL_arraysize(toolbar_top_order)+direction)%SDL_arraysize(toolbar_top_order)];
    return 0;
}
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
            {"Workspaces …",TOOLBAR_WORKSPACES,!a->dialog_pending},
            {"Aktive Datei neu laden",TOOLBAR_RELOAD,a->tab==8 && active_document(a) && !active_document(a)->recovery}
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
    if(menu==3) {
        struct nk_text_edit *edit=completion_target(a);
        /* The menu itself is open, so inspect the selected document directly. */
        if(!edit && !a->project_manager) {
            if(a->tab==8 && active_document(a))edit=&active_document(a)->edit;
            else if(a->loaded && a->tab==0)edit=a->analysis_only?&a->analysis:&a->experiment;
            else if(a->loaded && a->tab==2 && a->analysis_tab==1)edit=&a->analysis;
        }
        bool code=edit && document_language(a,edit)>=0;
        toolbar_entry help[]={
            {"Handbuch\tF1",TOOLBAR_HELP,true},
            {"API-Referenz\tShift+F1",TOOLBAR_REFERENCE,code},
            {"Vervollständigen\tCtrl+Space",TOOLBAR_COMPLETION,code},
            {"API-Referenz schließen",TOOLBAR_REFERENCE_CLOSE,a->api_reference!=NULL}
        };
        memcpy(entries,help,sizeof help);return SDL_arraysize(help);
    }
    if(menu==5) {
        bool editor=!a->project_manager && ((a->loaded && (a->tab==0 || (a->tab==2 && a->analysis_tab==1))) ||
                    (a->tab==8 && active_document(a) && !active_document(a)->recovery));
        entries[0]=(toolbar_entry){"Suchen / Ersetzen\t" PS_UI_SHORTCUT "F",TOOLBAR_SEARCH,editor};
        entries[1]=(toolbar_entry){"Einstellungen …",TOOLBAR_SETTINGS,true};return 2;
    }
    if(menu==6) {
        entries[0]=(toolbar_entry){"Projekt bauen\tF5",TOOLBAR_BUILD,a->loaded && idle(a)};
        entries[1]=(toolbar_entry){"Build-Einstellungen",TOOLBAR_BUILD_SETTINGS,a->loaded};return 2;
    }
    return 0;
}
static void toolbar_dispatch(app *a, toolbar_action action) {
    switch(action) {
    case TOOLBAR_OPEN: choose_workspace_path(a,PS_DIALOG_OPEN_FOLDER);break;
    case TOOLBAR_ADD_FILE: choose_workspace_path(a,PS_DIALOG_ADD_FILE);break;
    case TOOLBAR_ADD_FOLDER: choose_workspace_path(a,PS_DIALOG_ADD_FOLDER);break;
    case TOOLBAR_NEW: open_project_manager(a,"Datei");break;
    case TOOLBAR_SAVE: save_active_document(a);break;
    case TOOLBAR_WORKSPACES: a->workspace_manager=true;break;
    case TOOLBAR_LOG: protocol_toggle(a,"Ansicht");break;
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
    case TOOLBAR_RELOAD: document_request(a,2);break;
    case TOOLBAR_SEARCH: editor_search_open(a);break;
    case TOOLBAR_BUILD: build_project(a);break;
    case TOOLBAR_HELP: open_documentation(a,1);break;
    case TOOLBAR_REFERENCE: {struct nk_text_edit *edit=completion_target(a);if(edit)api_reference_from_editor(a,edit);break;}
    case TOOLBAR_COMPLETION: {struct nk_text_edit *edit=completion_target(a);if(edit)completion_open(a,edit);break;}
    case TOOLBAR_BUILD_SETTINGS: a->project_build_disclosure=NK_MAXIMIZED;dock_show(a,PS_DOCK_SIDEBAR);break;
    case TOOLBAR_SETTINGS:a->project_manager=false;open_settings(a);break;
    case TOOLBAR_REFERENCE_CLOSE:api_reference_close(a);break;
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
            else {
                /* F10 explicitly transfers input from a native control to the
                 * custom menu, like outline/document keyboard navigation. */
                nk_sdl_accessibility_keyboard_begin(a->ui);
                a->toolbar_keyboard=true;a->toolbar_keyboard_top=0;a->toolbar_keyboard_item=-1;a->toolbar_menu=0;
            }
        }
        return true;
    }
    if(a->toolbar_menu && plain && key->key==SDLK_ESCAPE){toolbar_keyboard_close(a);return true;}
    if(!plain && (a->toolbar_menu || a->toolbar_keyboard)){toolbar_keyboard_close(a);return false;}
    if(!a->toolbar_keyboard && a->toolbar_menu && plain &&
       (key->key==SDLK_UP || key->key==SDLK_DOWN || key->key==SDLK_HOME || key->key==SDLK_END)) {
        a->toolbar_keyboard=true;a->toolbar_keyboard_top=a->toolbar_menu-1;a->toolbar_keyboard_item=-1;
    }
    if(!a->toolbar_keyboard)return false;
    if(!plain){toolbar_keyboard_close(a);return false;}
    if(key->repeat)return true;
    if(key->key==SDLK_ESCAPE){toolbar_keyboard_close(a);return true;}
    if(key->key==SDLK_LEFT || key->key==SDLK_RIGHT || key->key==SDLK_TAB) {
        int direction=key->key==SDLK_LEFT || (key->key==SDLK_TAB && (key->mod&SDL_KMOD_SHIFT))?-1:1;
        bool opened=a->toolbar_menu!=0;
        a->toolbar_keyboard_top=toolbar_next_top(a->toolbar_keyboard_top,direction);
        a->toolbar_menu=opened?a->toolbar_keyboard_top+1:0;
        a->toolbar_keyboard_item=-1;
        toolbar_keyboard_move(a,1,true);
    } else if(key->key==SDLK_UP || key->key==SDLK_DOWN || key->key==SDLK_HOME || key->key==SDLK_END) {
        {
            bool opened=a->toolbar_menu!=0;
            a->toolbar_menu=a->toolbar_keyboard_top+1;
            int direction=key->key==SDLK_UP || key->key==SDLK_END?-1:1;
            toolbar_keyboard_move(a,direction,!opened || a->toolbar_keyboard_item<0 || key->key==SDLK_HOME || key->key==SDLK_END);
        }
    } else if(key->key==SDLK_RETURN || key->key==SDLK_KP_ENTER || key->key==SDLK_SPACE) {
        if(!a->toolbar_menu) {
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
static ps_ui_icon toolbar_action_icon(toolbar_action action) {
    switch(action) {
    case TOOLBAR_OPEN:return PS_ICON_FOLDER;
    case TOOLBAR_ADD_FILE:return PS_ICON_FILE_ADD;
    case TOOLBAR_ADD_FOLDER:case TOOLBAR_NEW:return PS_ICON_FOLDER_ADD;
    case TOOLBAR_SAVE:return PS_ICON_SAVE;
    case TOOLBAR_WORKSPACES:case TOOLBAR_LAYOUTS:return PS_ICON_LAYOUT;
    case TOOLBAR_LOG:return PS_ICON_CONSOLE;
    case TOOLBAR_REFRESH:case TOOLBAR_RELOAD:return PS_ICON_REFRESH;
    case TOOLBAR_LIBRARY:return PS_ICON_CHART;
    case TOOLBAR_BATCH:return PS_ICON_DICE;
    case TOOLBAR_SIDEBAR:return PS_ICON_SIDEBAR;
    case TOOLBAR_WORKSPACE:return PS_ICON_WINDOW;
    case TOOLBAR_RESET_LAYOUT:return PS_ICON_LAYOUT_RESET;
    case TOOLBAR_INSPECTOR:return PS_ICON_INSPECTOR;
    case TOOLBAR_SEARCH:return PS_ICON_SEARCH;
    case TOOLBAR_BUILD:return PS_ICON_BUILD;
    case TOOLBAR_HELP:return PS_ICON_BOOK;
    case TOOLBAR_REFERENCE:return PS_ICON_REFERENCE;
    case TOOLBAR_COMPLETION:return PS_ICON_COMPLETION;
    case TOOLBAR_SETTINGS:return PS_ICON_LAYOUT;
    case TOOLBAR_REFERENCE_CLOSE:return PS_ICON_CLOSE;
    }
    return PS_ICON_FILE;
}
static bool toolbar_separator(int menu,unsigned item) {
    return (menu==1 && (item==3 || item==5)) || (menu==2 && (item==2 || item==4 || item==8)) || (menu==3 && item==1);
}
static bool toolbar_menu_item(struct nk_context *ui, const toolbar_entry *entry, bool focused,bool compact) {
    const char *text=entry->text;bool enabled=entry->enabled;
    struct nk_rect bounds;
    enum nk_widget_layout_states state=nk_widget(&bounds,ui);
    bool hover=enabled && state==NK_WIDGET_VALID && nk_input_is_mouse_hovering_rect(&ui->input,bounds);
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    if(hover || focused)nk_fill_rect(canvas,bounds,compact?5:2,compact?UI_COLOR(button):UI_COLOR(selection_active));
    if(focused)nk_stroke_rect(canvas,nk_rect(bounds.x+1,bounds.y+1,bounds.w-2,bounds.h-2),2,1,UI_COLOR(accent));
    const struct nk_user_font *font=ui->style.font;
    const char *shortcut=strchr(text,'\t');
    int length=shortcut?(int)(shortcut-text):(int)strlen(text);
    float y=bounds.y+(bounds.h-font->height)/2;
    struct nk_color icon_color=enabled?UI_COLOR(text):muted();
    if(enabled && entry->action==TOOLBAR_BUILD)icon_color=UI_COLOR(accent);
    if(!compact)ps_ui_draw_icon(canvas,toolbar_action_icon(entry->action),nk_rect(bounds.x+6,bounds.y,24,bounds.h),icon_color);
    float shortcut_width=0;
    if(shortcut)shortcut_width=font->width(font->userdata,font->height,shortcut+1,(int)strlen(shortcut+1))+24;
    float inset=compact?10.f:38.f;
    nk_draw_text(canvas,nk_rect(bounds.x+inset,y,fmaxf(1,bounds.w-inset-12-shortcut_width),font->height),text,length,font,nk_rgba(0,0,0,0),enabled?UI_COLOR(text):muted());
    if(shortcut) {
        ++shortcut;int count=(int)strlen(shortcut);
        float width=font->width(font->userdata,font->height,shortcut,count);
        nk_draw_text(canvas,nk_rect(bounds.x+bounds.w-10-width,y,width+1,font->height),shortcut,count,font,nk_rgba(0,0,0,0),hover || focused?UI_COLOR(text):muted());
    }
    nk_begin_disabled(ui,!enabled);
    bool activated=ps_ui_button_at(ui,bounds,text,focused);
    nk_end_disabled(ui);
    return activated || (hover && nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT));
}
static void toolbar_popup(app *a, int previous) {
    struct nk_context *ui=a->ui;
    if(!a->toolbar_menu && !previous)return;
    int menu=a->toolbar_menu?a->toolbar_menu:previous;
    struct nk_rect anchor=a->toolbar_bounds[menu-1];
    float factor=a->preferences.ui_size/16.f;
    toolbar_entry available_entries[9];unsigned menu_count=toolbar_entries(a,menu,available_entries);
    bool compact=workspace_shell_visible(a);
    unsigned separators=0;float popup_width=(compact?250:240)*factor;
    for(unsigned i=0;i<menu_count;i++) {
        const char *label=available_entries[i].text,*shortcut=strchr(label,'\t');
        int length=shortcut?(int)(shortcut-label):(int)strlen(label);
        float text_width=ui->style.font->width(ui->style.font->userdata,ui->style.font->height,label,length);
        float shortcut_width=shortcut?ui->style.font->width(ui->style.font->userdata,ui->style.font->height,shortcut+1,(int)strlen(shortcut+1))+28:0;
        popup_width=fmaxf(popup_width,text_width+shortcut_width+(compact?24:64));
        if(toolbar_separator(menu,i))separators++;
    }
    float row_height=fmaxf(28,(float)a->preferences.ui_size+10);
    a->toolbar_popup_bounds=nk_rect(anchor.x,anchor.y+anchor.h+1,popup_width,10+menu_count*(row_height+2)+separators*9);
    nk_style_push_vec2(ui,&ui->style.window.popup_padding,nk_vec2(5,5));
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(4,2));
    nk_style_push_float(ui,&ui->style.window.rounding,compact?7:3);
    if(nk_popup_begin(ui,NK_POPUP_STATIC,"application-menu",NK_WINDOW_NO_SCROLLBAR,a->toolbar_popup_bounds)) {
        if(!a->toolbar_menu)nk_popup_close(ui);
        else {
            toolbar_entry entries[9];unsigned count=toolbar_entries(a,menu,entries);
            if(a->toolbar_keyboard && (a->toolbar_keyboard_item<0 || (unsigned)a->toolbar_keyboard_item>=count || !entries[a->toolbar_keyboard_item].enabled))
                toolbar_keyboard_move(a,1,true);
            for(unsigned i=0;i<count;i++) {
                if(toolbar_separator(menu,i)) {
                    nk_layout_row_dynamic(ui,7,1);struct nk_rect separator;nk_widget(&separator,ui);
                    nk_stroke_line(nk_window_get_canvas(ui),separator.x+36,separator.y+separator.h/2,separator.x+separator.w-8,separator.y+separator.h/2,1,UI_COLOR(divider));
                }
                nk_layout_row_dynamic(ui,row_height,1);
                struct nk_rect bounds=nk_widget_bounds(ui);
                if(menu==1 && i<5)a->toolbar_item_bounds[i]=bounds;
                if(menu==1 && i==6)a->document_bounds[1]=bounds;
                if(menu==6 && i==0)a->editor_build_bounds=bounds;
                if(menu==3 && i==1)a->api_reference_button_bounds=bounds;
                if(menu==3 && i==2)a->completion_button_bounds=bounds;
                if(menu==1 && i==5)a->workspace_menu_bounds=bounds;
                if(menu==2 && i==0)a->toolbar_item_bounds[5]=bounds;
                if(menu==2 && i==3)a->batch_navigation_bounds=bounds;
                if(menu==2 && i>=4)a->dock_menu_bounds[i-4]=bounds;
                if(entries[i].action==TOOLBAR_SETTINGS)a->toolbar_settings_bounds=bounds;
                if(entries[i].action==TOOLBAR_REFERENCE_CLOSE)a->api_reference_close_bounds=bounds;
                bool focused=a->toolbar_keyboard && a->toolbar_keyboard_item==(int)i;
                bool clicked=toolbar_menu_item(ui,&entries[i],focused,compact);
                if(entries[i].enabled && clicked) {
                    /* Selecting a menu command must not also hit a control
                     * exposed underneath when the popup closes this frame. */
                    a->toolbar_pointer_consumed=nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT);
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
static void toolbar_theme_view(app *a) {
    struct nk_context *ui=a->ui;struct nk_rect r;nk_widget(&r,ui);a->toolbar_theme_bounds=r;
    bool light=a->preferences.theme==PS_THEME_LIGHT;
    const char *label=light?"Dunkel":"Hell";
    bool hovered=nk_input_is_mouse_hovering_rect(&ui->input,r);
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    if(hovered)nk_fill_rect(canvas,nk_rect(r.x+2,r.y+4,r.w-4,r.h-8),5,UI_COLOR(button));
    float x=r.x+12,y=r.y+r.h/2;struct nk_color color=muted();
    if(light) {
        nk_stroke_arc(canvas,x,y,5,.4f,5.4f,1,color);
        nk_stroke_arc(canvas,x+2,y-2,4,1.5f,4.6f,1,color);
    } else {
        nk_stroke_circle(canvas,nk_rect(x-3,y-3,6,6),1,color);
        for(unsigned i=0;i<8;i++){float angle=(float)i*3.14159265f/4;nk_stroke_line(canvas,x+5*cosf(angle),y+5*sinf(angle),x+7*cosf(angle),y+7*sinf(angle),1,color);}
    }
    const struct nk_user_font *font=ui->style.font;
    nk_draw_text(canvas,nk_rect(r.x+26,r.y+(r.h-font->height)/2,r.w-30,font->height),label,(int)strlen(label),font,nk_rgba(0,0,0,0),color);
    bool activated=ps_ui_button_at(ui,r,"Darstellung umschalten",false);
    ps_ui_accessibility_label(ui,r,light?"Dunkle Darstellung aktivieren":"Helle Darstellung aktivieren");
    if(activated || (hovered && nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT))) {
        a->theme_request=(light?PS_THEME_DARK:PS_THEME_LIGHT)+1;
        a->toolbar_pointer_consumed=true;
        toolbar_keyboard_close(a);
    }
}
static void toolbar_view(app *a) {
    struct nk_context *ui = a->ui;
    /* Caption/header actions remain available when an editor or scene owns
     * Nuklear's widget focus; the completed stack still rejects covered nodes. */
    ps_ui_accessibility_route(ui,true);
    bool compact=workspace_shell_visible(a);
    nk_fill_rect(nk_window_get_canvas(ui), nk_rect(0, 0, nk_window_get_bounds(ui).w, compact?WORKSPACE_HEADER_HEIGHT:32), 0,
                 UI_COLOR(header));
    nk_style_push_vec2(ui, &ui->style.button.padding, nk_vec2(7, 2));
    nk_style_push_style_item(ui, &ui->style.button.normal, nk_style_item_color(UI_COLOR(header)));
    nk_menubar_begin(ui);
    float available = nk_window_get_content_region(ui).w;
    if(compact)nk_style_push_font(ui,&a->workbench_fonts[2]);
    nk_layout_row_begin(ui, NK_STATIC, compact?38:32, 11);
    nk_layout_row_push(ui, compact?52:72);
    if(compact) {
        struct nk_rect mark;nk_widget(&mark,ui);struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
        const struct nk_user_font *font=&a->workbench_fonts[4];
        nk_draw_text(canvas,nk_rect(mark.x+12,mark.y+(mark.h-font->height)/2,22,font->height),"p",1,font,nk_rgba(0,0,0,0),UI_COLOR(text));
        nk_fill_circle(canvas,nk_rect(mark.x+27,mark.y+16,3,3),UI_COLOR(accent));
    } else nk_label(ui, "Physim", NK_TEXT_CENTERED);
    const char *menus[] = {"Datei", "Ansicht", "Hilfe", "Einstellungen", "Bearbeiten", "Build"};
    float factor=a->preferences.ui_size/16.f*(compact?.75f:1.f);
    float menu_width[] = {58*factor,72*factor,54*factor,116*factor,90*factor,54*factor};
    float menu_extent=0;
    for(unsigned i=0;i<SDL_arraysize(menu_width);i++) {
        if(compact)menu_width[i]=ui->style.font->width(ui->style.font->userdata,ui->style.font->height,menus[i],(int)strlen(menus[i]))+24;
    }
    for(unsigned i=0;i<SDL_arraysize(toolbar_top_order);i++)menu_extent+=menu_width[toolbar_top_order[i]];
    a->toolbar_bounds[3]=nk_rect(0,0,0,0);a->toolbar_theme_bounds=nk_rect(0,0,0,0);
    int previous = a->toolbar_popup_menu;
    /* Header actions remain responsive while the popup makes its parent read-only. */
    bool press = nk_input_is_mouse_pressed(&ui->input, NK_BUTTON_LEFT);
    if(press && a->toolbar_menu)a->toolbar_pointer_consumed=true;
    if (press) {
        a->toolbar_keyboard = false;
    }
    int clicked = -1;
    for (unsigned column=0;column<SDL_arraysize(toolbar_top_order);column++) {int i=(int)toolbar_top_order[column];
        if (press && nk_input_is_mouse_hovering_rect(&ui->input, a->toolbar_bounds[i]))
            clicked = i;
    }
    if (clicked >= 0) {
        a->toolbar_keyboard = false;
        a->toolbar_menu = a->toolbar_menu == clicked + 1 ? 0 : clicked + 1;
    } else if (a->toolbar_menu &&
               (nk_input_is_key_pressed(&ui->input, NK_KEY_TEXT_RESET_MODE) ||
                (press && !nk_input_is_mouse_hovering_rect(&ui->input, a->toolbar_popup_bounds))))
        a->toolbar_menu = 0;
    else if (a->toolbar_menu && !press &&
             !a->toolbar_keyboard &&
             (ui->input.mouse.delta.x != 0 || ui->input.mouse.delta.y != 0)) {
        for (unsigned column=0;column<SDL_arraysize(toolbar_top_order);column++) {int i=(int)toolbar_top_order[column];
            if (nk_input_is_mouse_hovering_rect(&ui->input, a->toolbar_bounds[i]))
                a->toolbar_menu = i + 1;
        }
    }
    for (unsigned column = 0; column < SDL_arraysize(toolbar_top_order); column++) {
        int i=(int)toolbar_top_order[column];
        nk_layout_row_push(ui, menu_width[i]);
        struct nk_rect bounds;
        nk_widget(&bounds, ui);
        a->toolbar_bounds[i] = bounds;
        /* This caption is painted manually; publish its existing menu action
         * as well so a modal can return native focus to its actual opener. */
        if(ps_ui_accessibility_button(ui,bounds,menus[i])) {
            a->toolbar_keyboard=false;
            a->toolbar_menu=a->toolbar_menu==i+1?0:i+1;
        }
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
                     nk_rect(bounds.x + (compact?9:(bounds.w - text_width) / 2),
                             bounds.y + (bounds.h - font->height) / 2, text_width + 1,
                             font->height),
                     menus[i], (int)strlen(menus[i]), font, nk_rgba(0, 0, 0, 0), UI_COLOR(text));
    }
    float status_width=compact?0:fminf(175,fmaxf(96,available-72-menu_extent-138-36));
    float theme_width=compact?75*a->preferences.ui_size/16.f:0;
    nk_layout_row_push(ui, fmaxf(20, available - (compact?52:72) - menu_extent - 138 - status_width-theme_width));
    struct nk_rect path_bounds = nk_widget_bounds(ui);
    char caption[256];snprintf(caption,sizeof caption,"%s — Physim",a->loaded?project_name(a):a->workspace_open?workspace_name(a->workspace):"Workspace");
    nk_label_colored(ui, compact?caption:a->workspace_open ? a->workspace : "Kein Ordner geöffnet", compact?NK_TEXT_RIGHT:NK_TEXT_LEFT, muted());
    if (a->workspace_open && !ui->current->popup.active && !a->toolbar_menu &&
        nk_input_is_mouse_hovering_rect(&ui->input, path_bounds))
        nk_tooltip(ui, a->workspace);
    struct nk_rect status_bounds=nk_widget_bounds(ui);
    if(compact){nk_layout_row_push(ui,theme_width);status_bounds=nk_widget_bounds(ui);toolbar_theme_view(a);}
    else {nk_layout_row_push(ui, status_width);nk_label_colored(ui,
                     a->runner.running ? (a->paused ? "Pausiert" : "Simulation läuft")
                     : a->batch_thread ? "Laufserie läuft"
                     : a->job.running  ? "Wird verarbeitet …"
                     : (a->dirty || a->analysis_dirty || a->project_settings_dirty || documents_dirty(a))
                         ? "Ungespeicherte Änderungen"
                         : "Bereit",
                     NK_TEXT_RIGHT, muted());}
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
    if(compact)nk_style_pop_font(ui);
    nk_style_pop_style_item(ui);
    if(compact)nk_style_push_font(ui,&a->workbench_fonts[2]);
    nk_layout_row_dynamic(ui, compact?40:30, 3);
    const char *tabs[] = {"Entwickeln", "Simulieren", "Auswerten"};
    for (int i = 0; i < 3; i++) {
        bool selected = workspace_active_area(a)==i;
        struct nk_rect bounds;
        enum nk_widget_layout_states state = nk_widget(&bounds, ui);
        if(compact) {
            float tab_width=fmaxf(1,(available-32)/3);
            float left=8+i*(tab_width+8);
            bounds.x=roundf(left);bounds.w=roundf(left+tab_width)-bounds.x;
        }
        a->navigation_bounds[i] = bounds;
        bool hovered = !a->toolbar_pointer_consumed && nk_input_is_mouse_hovering_rect(&ui->input, bounds);
        if (state == NK_WIDGET_VALID && !a->toolbar_menu && !a->toolbar_pointer_consumed && hovered &&
            nk_input_is_mouse_pressed(&ui->input, NK_BUTTON_LEFT))
            select_workspace_tab(a, i);
        struct nk_command_buffer *canvas = nk_window_get_canvas(ui);
        struct nk_rect surface = nk_rect(bounds.x + (compact?0:5), bounds.y + 4, bounds.w - (compact?0:10), bounds.h - (compact?10:8));
        if (selected || hovered)
            nk_fill_rect(canvas, surface, compact?5:8,
                         selected ? (compact?UI_COLOR(button):UI_COLOR(button_hover)) : UI_COLOR(button));
        const struct nk_user_font *font = ui->style.font;
        float text_width = font->width(font->userdata, font->height, tabs[i], (int)strlen(tabs[i]));
        size_t runs=compact && i==2?workspace_run_count(a):0;
        const struct nk_user_font *count_font=&a->workspace_status_font;
        char count[24];snprintf(count,sizeof count,"%zu",runs);
        float count_width=runs?count_font->width(count_font->userdata,count_font->height,count,(int)strlen(count)):0;
        float scale=a->preferences.ui_size/16.f;
        /* Reference geometry: caption including its space/margin advances
         * 64 px; the count has 5 px padding and 5.4 px per digit at 10 px.
         * Keep the box stable across platform font advances; draw real text. */
        float badge_width=runs?roundf((10+5.4f*(float)strlen(count))*scale):0;
        float badge_x=roundf(bounds.x+bounds.w/2+(64*scale-badge_width)/2);
        float space=font->width(font->userdata,font->height," ",1);
        float caption_x=runs?badge_x-5*scale-space-text_width:bounds.x+(bounds.w-text_width)/2;
        nk_draw_text(canvas,
                     nk_rect(caption_x,
                             bounds.y + (bounds.h - font->height) / 2, text_width + 1,
                             font->height),
                     tabs[i], (int)strlen(tabs[i]), font, nk_rgba(0, 0, 0, 0),
                     selected ? UI_COLOR(text) : UI_COLOR(muted));
        if(runs) {
            float badge_height=roundf(16*scale);
            struct nk_rect badge=nk_rect(badge_x,roundf(bounds.y+(bounds.h-badge_height)/2),badge_width,badge_height);
            nk_fill_rect(canvas,badge,4,UI_COLOR(selection_active));
            nk_draw_text(canvas,nk_rect(badge.x+(badge.w-count_width)/2,badge.y+(badge.h-count_font->height)/2,count_width+1,count_font->height),count,(int)strlen(count),count_font,nk_rgba(0,0,0,0),UI_COLOR(accent));
            char description[96];snprintf(description,sizeof description,"%zu Laufdateien im Projekt",runs);ps_ui_accessibility_label(ui,badge,description);
            if(!a->toolbar_menu && !a->toolbar_pointer_consumed && nk_input_is_mouse_hovering_rect(&ui->input,badge))nk_tooltip(ui,description);
        }
    }
    if(compact)nk_style_pop_font(ui);
    if(compact)nk_fill_rect(nk_window_get_canvas(ui),nk_rect(0,WORKSPACE_HEADER_HEIGHT-1,available,1),0,UI_COLOR(border));
    nk_style_pop_vec2(ui);
}

/* Product state in the shared reference's breadcrumb strip, never demo data. */
static void workspace_path_view(app *a,float w) {
    struct nk_context *ui=a->ui;
    bool visible=workspace_shell_visible(a);
    nk_window_show(ui,"workspace-path",visible?NK_SHOWN:NK_HIDDEN);
    if(!visible)return;
    nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(14,4));
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(16,0));
    nk_style_push_float(ui,&ui->style.window.rounding,0);
    nk_style_push_font(ui,&a->workbench_fonts[0]);
    struct nk_rect bounds=nk_rect(0,WORKSPACE_HEADER_HEIGHT,w,WORKSPACE_PATH_HEIGHT);
    struct nk_window *previous=ui->active;
    if(nk_begin(ui,"workspace-path",bounds,NK_WINDOW_BACKGROUND|NK_WINDOW_NO_SCROLLBAR)) {
        dock_input(a,PS_DOCK_WORKSPACE,bounds,previous,true);
        const struct nk_user_font *font=ui->style.font;
        const char *name=a->loaded?project_name(a):a->workspace_open?workspace_name(a->workspace):"Workspace";
        int active_area=workspace_active_area(a);
        const char *area=active_area==1?"Messlabor":active_area==2?"Auswertung":"Werkbank";
        float name_width=fminf(w*.34f,font->width(font->userdata,font->height,name,(int)strlen(name)));
        const struct nk_user_font *area_font=&a->workbench_fonts[3];
        float area_width=ceilf(area_font->width(area_font->userdata,area_font->height,area,(int)strlen(area)))+1;
        float action_width=150*a->preferences.ui_size/16.f;
        float state_width=fminf(280*a->preferences.ui_size/16.f,w*.3f);
        name_width=fminf(name_width,fmaxf(40,w-28-area_width-state_width-action_width-85));
        nk_layout_row_begin(ui,NK_STATIC,30,6);
        nk_layout_row_push(ui,name_width);nk_label_colored(ui,name,NK_TEXT_LEFT,muted());
        nk_layout_row_push(ui,5);nk_label_colored(ui,"/",NK_TEXT_LEFT,UI_COLOR(border));
        nk_layout_row_push(ui,area_width);nk_style_push_font(ui,&a->workbench_fonts[3]);nk_label(ui,area,NK_TEXT_LEFT);nk_style_pop_font(ui);
        nk_layout_row_push(ui,fmaxf(0,w-28-name_width-area_width-state_width-action_width-85));nk_spacer(ui);
        const char *state=a->job.running?"Build läuft …":a->runner.running?(a->paused?"Simulation pausiert":"Simulation läuft"):
            !a->loaded?"Ordner öffnen":a->dirty || a->analysis_dirty || documents_dirty(a)?"Ungespeicherte Änderungen":a->built?"Modell bereit":"Build erforderlich";
        nk_layout_row_push(ui,state_width);nk_label_colored(ui,state,NK_TEXT_RIGHT,muted());
        nk_layout_row_push(ui,action_width);
        nk_style_push_style_item(ui,&ui->style.button.normal,nk_style_item_color(UI_COLOR(window)));
        nk_style_push_flags(ui,&ui->style.button.text_alignment,NK_TEXT_RIGHT);
        nk_style_push_color(ui,&ui->style.button.text_normal,UI_COLOR(accent));
        nk_begin_disabled(ui,!a->loaded);
        if(nk_button_label(ui,active_area==1?"Messlauf auswerten →":active_area==2?"Zum Modell →":"Zur Simulation →"))select_workspace_tab(a,active_area==1?2:active_area==2?0:1);
        nk_end_disabled(ui);nk_style_pop_color(ui);nk_style_pop_flags(ui);nk_style_pop_style_item(ui);
        nk_layout_row_end(ui);
        struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
        nk_push_scissor(canvas,bounds);
        nk_fill_rect(canvas,nk_rect(0,WORKSPACE_CONTENT_TOP-1,w,1),0,UI_COLOR(border));
    }nk_end(ui);
    nk_style_pop_font(ui);nk_style_pop_float(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
}
