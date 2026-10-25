/* Real files, native focus/activation, CSS geometry and a genuinely wide tab. */
static struct {unsigned variant;char long_path[4096],long_name[106];uint32_t source_crc;bool long_text,end_visible;unsigned settle;struct nk_rect header_editor,header_context;} tab_design;
static const char *tab_design_variant(void) {return (const char*[]){"dark","light","large","contrast"}[tab_design.variant<4?tab_design.variant:2];}
static const char *tab_design_capture(const char *state) {
    static char name[96];snprintf(name,sizeof name,"tabs-%s-%s.png",tab_design_variant(),state);return name;
}
static bool tab_design_start(app *a,const char *root) {
    memset(&tab_design,0,sizeof tab_design);char prefs[4096];join(prefs,sizeof prefs,root,"preferences.bin");preferences_start(a,prefs);
    if(!workbench_test_start(a,root,"workbench-dark-unified-visual"))return false;
    memset(tab_design.long_name,'W',100);memcpy(tab_design.long_name+100,".phys",6);
    join(tab_design.long_path,sizeof tab_design.long_path,a->project,tab_design.long_name);
    if(!document_test_write(tab_design.long_path,"// Datei α\n"))return false;
    char path[4096];join(path,sizeof path,a->project,"physim.project");preview_workspace_path(a,path);file_tab_select(a,0);
    tab_design.source_crc=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));return a->document_count==1 && workspace_tree_test_target(a,"runs",0);
}
static bool tab_design_source_same(app *a) {
    return tab_design.source_crc==ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string)) && !a->dirty && !a->analysis_dirty;
}
static void tab_design_draw(app *a) {
    tab_design.long_text=false;tab_design.end_visible=false;const struct nk_command *command;
    nk_foreach(command,a->ui)if(command->type==NK_COMMAND_TEXT) {
        const struct nk_command_text *text=(const struct nk_command_text*)command;
        if(text->font==&a->workbench_fonts[0] && text->y>=a->file_tab_strip.y && text->y+text->height<=a->file_tab_strip.y+a->file_tab_strip.h && text->length==105 && !memcmp(text->string,tab_design.long_name,105)) {
            tab_design.long_text=true;float right=text->x+text->font->width(text->font->userdata,text->height,text->string,text->length);
            tab_design.end_visible=right>=a->file_tab_strip.x && right<=a->file_tab_strip.x+a->file_tab_strip.w-48*a->preferences.ui_size/16.f;
        }
    }
}
static void tab_design_log(app *a,const char *kind) {
    float f=a->preferences.ui_size/16.f;const struct nk_user_font *font=&a->workbench_marker_font;
    printf("TAB DESIGN %s variant=%s size=%u marker_css=%.4f mono=%.4f strip=%.4f,%.4f,%.4f,%.4f",kind,tab_design_variant(),a->preferences.ui_size,font->height/a->workbench_marker_em/f,font->width(font->userdata,font->height,"C",1),a->file_tab_strip.x,a->file_tab_strip.y,a->file_tab_strip.w,a->file_tab_strip.h);
    for(unsigned i=0;i<4;i++){struct nk_rect r=a->file_tab_bounds[i];printf(" rect%u=%.4f,%.4f,%.4f,%.4f",i,r.x,r.y,r.w,r.h);}printf(" folder=%.4f,%.4f,%.4f,%.4f scroll=%.4f complete=%d end=%d\n",a->workspace_tree_test_bounds.x,a->workspace_tree_test_bounds.y,a->workspace_tree_test_bounds.w,a->workspace_tree_test_bounds.h,a->file_tab_scroll,tab_design.long_text,tab_design.end_visible);
}
static bool tab_design_marker(app *a) {
    const struct nk_user_font *font=&a->workbench_marker_font;float advance=font->width(font->userdata,font->height,"C",1);
    for(nk_rune rune=33;rune<=126;rune++){char text[2]={(char)rune,0};if(!ps_ui_font_has_glyph(font,rune) || fabsf(font->width(font->userdata,font->height,text,1)-advance)>.001f)return false;}
    return font->userdata.ptr!=a->font_title->userdata.ptr && font->userdata.ptr!=a->font_code->userdata.ptr && fabsf(font->height/a->workbench_marker_em-12*a->preferences.ui_size/16.f)<.001f;
}
static bool tab_header_geometry(app *a,bool hidden,bool detached) {
    float f=a->preferences.ui_size/16.f;struct nk_rect tools=a->workbench_tools_bounds,search=a->workbench_search_bounds,save=a->workbench_save_bounds;
    float right=detached?tools.x+tools.w:a->dock_panels[PS_DOCK_WORKSPACE].x+a->dock_panels[PS_DOCK_WORKSPACE].w;
    return tab_design_source_same(a) && fabsf(tools.w-(1+(hidden?105:71)*f))<.001f &&
        fabsf(search.w-28*f)<.001f && fabsf(search.h-28*f)<.001f && workbench_test_rect(nk_rect(search.x+31*f,search.y,search.w,search.h),save) &&
        fabsf(search.x-tools.x-1-6*f)<.001f && fabsf(tools.x+tools.w-right)<.001f &&
        fabsf(a->file_tab_strip.x+a->file_tab_strip.w-tools.x)<.001f;
}
static void tab_header_log(app *a,const char *state) {
    printf("HEADER DESIGN state=%s variant=%s size=%u",state,tab_design_variant(),a->preferences.ui_size);
    struct nk_rect boxes[]={a->workbench_tools_bounds,a->workbench_search_bounds,a->workbench_save_bounds,a->file_tab_strip,a->workbench_sidebar_bounds};
    for(unsigned i=0;i<SDL_arraysize(boxes);i++)printf(" box%u=%.4f,%.4f,%.4f,%.4f",i,boxes[i].x,boxes[i].y,boxes[i].w,boxes[i].h);puts("");
}
static const char *tab_header_capture(const char *state) {
    static char path[96];snprintf(path,sizeof path,"header-%s-%s.png",tab_design_variant(),state);return path;
}
static void tab_header_mouse(app *a,bool down,bool save) {
    test_window_mouse(a->panels[PS_DOCK_WORKSPACE].window,save?a->workbench_save_bounds:a->workbench_search_bounds,down);
}
static void tab_design_frame(app *a,int *stage,int *code,const char **capture) {
    bool ok=true;
    switch(*stage) {
    case 0: {
        ps_preferences p=a->preferences;p.theme=tab_design.variant==0?PS_THEME_DARK:tab_design.variant==3?PS_THEME_HIGH_CONTRAST:PS_THEME_LIGHT;p.ui_size=tab_design.variant<2?16:22;preferences_apply(a,&p);
        SDL_SetWindowSize(a->window,tab_design.variant<2?1440:1080,tab_design.variant<2?1000:740);file_tab_select(a,0);test_plot_motion(a,10,10);test_mouse(a,nk_rect(10,10,1,1),true);test_mouse(a,nk_rect(10,10,1,1),false);break;
    }
    case 1:if(fabsf(a->file_tab_bounds[0].h-28*a->preferences.ui_size/16.f)>.001f || a->file_tab_bounds[2].w<=0)return;break;
    case 2: {
        float f=a->preferences.ui_size/16.f;ok=tab_design_marker(a) && tab_design_source_same(a) && fabsf(a->file_tab_bounds[1].x-a->file_tab_bounds[0].x-a->file_tab_bounds[0].w-3*f)<.001f;
        tab_design_log(a,"normal");*capture=tab_design_capture("normal");test_plot_motion(a,a->file_tab_bounds[2].x+10*f,a->file_tab_bounds[2].y+10*f);break;
    }
    case 3:*capture=tab_design_capture("hover");if(!nk_sdl_accessibility_request_focus(a->ui,NULL,"physim.project"))return;break;
    case 4:if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"physim.project") || SDL_GetKeyboardFocus()!=a->window){SDL_RaiseWindow(a->window);return;}*capture=tab_design_capture("focus");break;
    case 5:test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 6:if(file_tab_active(a)!=2)return;ok=tab_design_source_same(a);if(!nk_sdl_accessibility_request_focus(a->ui,NULL,"physim.project schließen"))return;break;
    case 7:if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"physim.project schließen") || SDL_GetKeyboardFocus()!=a->window){SDL_RaiseWindow(a->window);return;}*capture=tab_design_capture("close-focus");break;
    case 8:test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 9: {
        if(a->document_count)return;ok=tab_design_source_same(a) && file_tab_active(a)==0;if(!ok)break;
        char path[4096];join(path,sizeof path,a->project,"physim.project");preview_workspace_path(a,path);file_tab_select(a,0);tab_design.variant++;*stage=tab_design.variant<4?0:30;return;
    }
    case 30:{ps_preferences p=a->preferences;p.theme=PS_THEME_DARK;p.ui_size=22;preferences_apply(a,&p);preview_workspace_path(a,tab_design.long_path);break;}
    case 31:if(a->file_tab_bounds[3].w<=0 || !tab_design.long_text)return;break;
    case 32:ok=tab_design_source_same(a) && a->document_count==2 && !tab_design.end_visible;tab_design_log(a,"long-start");*capture="tabs-long-start.png";test_plot_wheel(a,a->file_tab_strip.x+30,a->file_tab_strip.y+20,-1000);break;
    case 33:if(a->file_tab_closes[3].w<=0 || !tab_design.end_visible)return;break;
    case 34:ok=tab_design_source_same(a);tab_design_log(a,"long-end");*capture="tabs-long-end.png";test_mouse(a,a->file_tab_closes[3],true);break;
    case 35:test_mouse(a,a->file_tab_closes[3],false);break;
    case 36:if(a->document_count!=1)return;ok=tab_design_source_same(a);file_tab_select(a,0);a->dirty=true;test_plot_motion(a,10,10);break;
    case 37:break;
    case 38:ok=ps_ui_font_has_glyph(&a->workbench_tab_dot_font,0x25cf);*capture="tabs-dirty.png";tab_design_log(a,"dirty");break;
    case 39:a->dirty=false;tab_design.variant=0;break;
    case 40: {
        ps_preferences p=a->preferences;p.theme=tab_design.variant==0?PS_THEME_DARK:tab_design.variant==3?PS_THEME_HIGH_CONTRAST:PS_THEME_LIGHT;p.ui_size=tab_design.variant<2?16:22;preferences_apply(a,&p);
        a->find[0]=0;SDL_SetWindowSize(a->window,tab_design.variant<2?1440:1080,tab_design.variant<2?1000:740);tab_design.settle=0;test_plot_motion(a,10,10);break;
    }
    case 41:
        if(tab_design.settle++<3)return;
        ok=tab_header_geometry(a,false,false);tab_header_log(a,"normal");*capture=tab_header_capture("normal");
        tab_design.header_editor=a->source_editor_bounds;tab_design.header_context=a->workbench_context_bounds;
        test_plot_motion(a,a->workbench_search_bounds.x+10,a->workbench_search_bounds.y+10);break;
    case 42:*capture=tab_header_capture("hover");test_mouse(a,a->workbench_search_bounds,true);break;
    case 43:test_mouse(a,a->workbench_search_bounds,false);tab_design.settle=0;break;
    case 44:
        if(tab_design.settle++<3)return;
        ok=a->show_search && a->search_keyboard && workbench_test_rect(tab_design.header_editor,a->source_editor_bounds) && workbench_test_rect(tab_design.header_context,a->workbench_context_bounds);
        {SDL_Event event={0};event.type=SDL_EVENT_TEXT_INPUT;event.text.windowID=SDL_GetWindowID(a->window);event.text.text="α";SDL_PushEvent(&event);}tab_design.settle=0;*stage=441;return;
    case 441:
        if(tab_design.settle++<3)return;
        ok=!strcmp(a->find,"α") && tab_design_source_same(a);*capture=tab_header_capture("search");test_window_key_mod(a->window,SDLK_ESCAPE,0);tab_design.settle=0;*stage=45;if(!ok)break;return;
    case 45:
        if(tab_design.settle++<3)return;
        if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"Dokumentsuche öffnen") || SDL_GetKeyboardFocus()!=a->window){SDL_RaiseWindow(a->window);return;}
        ok=!a->show_search && tab_header_geometry(a,false,false);*capture=tab_header_capture("focus");a->dirty=true;test_mouse(a,a->workbench_save_bounds,true);break;
    case 46:test_mouse(a,a->workbench_save_bounds,false);break;
    case 47:
        ok=tab_design_source_same(a);ps_dock_hide(&a->preferences.dock,PS_DOCK_SIDEBAR);ps_dock_hide(&a->preferences.dock,PS_DOCK_INSPECTOR);tab_design.settle=0;test_plot_motion(a,10,10);break;
    case 48:
        if(tab_design.settle++<3)return;
        ok=tab_header_geometry(a,true,false);tab_header_log(a,"closed");*capture=tab_header_capture("closed");
        test_mouse(a,a->workbench_sidebar_bounds,true);break;
    case 49:test_mouse(a,a->workbench_sidebar_bounds,false);tab_design.settle=0;break;
    case 50:
        if(tab_design.settle++<3)return;
        ok=!(a->preferences.dock.hidden&(1u<<PS_DOCK_SIDEBAR)) && (a->preferences.dock.hidden&(1u<<PS_DOCK_INSPECTOR)) && tab_header_geometry(a,true,false);
        test_mouse(a,a->workbench_context_bounds,true);break;
    case 51:test_mouse(a,a->workbench_context_bounds,false);tab_design.settle=0;break;
    case 52:
        if(tab_design.settle++<3)return;
        ok=!(a->preferences.dock.hidden&(1u<<PS_DOCK_INSPECTOR)) && tab_header_geometry(a,false,false);
        tab_header_log(a,"restored");*capture=tab_header_capture("restored");
        tab_design.variant++;*stage=tab_design.variant<4?40:60;return;
    case 60: {
        ps_preferences p=a->preferences;p.theme=PS_THEME_DARK;p.ui_size=16;preferences_apply(a,&p);tab_design.variant=0;
        SDL_SetWindowSize(a->window,1440,1000);tab_design.settle=0;break;
    }
    case 61:
        if(tab_design.settle++<3)return;
        ok=tab_header_geometry(a,false,false);if(!ok)break;
        tab_header_log(a,"before-detach");*capture="header-before-detach.png";
        *stage=611;return;
    case 611:
        /* Capture the attached header before opening mutates its visibility. */
        ok=panel_window_open(a,PS_DOCK_WORKSPACE);if(ok)SDL_SetWindowSize(a->panels[PS_DOCK_WORKSPACE].window,(int)(a->workbench_tools_bounds.x+a->workbench_tools_bounds.w-a->file_tab_strip.x),740);
        tab_design.settle=0;*stage=62;if(!ok)break;return;
    case 62:
        if(tab_design.settle++<3)return;
        ok=panel_is_detached(a,PS_DOCK_WORKSPACE) && tab_header_geometry(a,false,true) && a->file_tab_strip.x==0 && a->file_tab_strip.y==0;
        tab_header_log(a,"detached");join(a->panels[PS_DOCK_WORKSPACE].capture,sizeof a->panels[PS_DOCK_WORKSPACE].capture,a->project,"header-detached.png");break;
    case 63:
        if(SDL_GetKeyboardFocus()!=a->panels[PS_DOCK_WORKSPACE].window){SDL_RaiseWindow(a->panels[PS_DOCK_WORKSPACE].window);return;}
        tab_header_mouse(a,true,false);break;
    case 64:tab_header_mouse(a,false,false);tab_design.settle=0;break;
    case 65:
        if(tab_design.settle++<3)return;
        ok=a->show_search;join(a->panels[PS_DOCK_WORKSPACE].capture,sizeof a->panels[PS_DOCK_WORKSPACE].capture,a->project,"header-detached-search.png");
        test_window_key_mod(a->panels[PS_DOCK_WORKSPACE].window,SDLK_ESCAPE,0);tab_design.settle=0;break;
    case 66:
        if(tab_design.settle++<3)return;
        if(!nk_sdl_accessibility_is_focused(a->panels[PS_DOCK_WORKSPACE].ui,NULL,"Dokumentsuche öffnen"))return;
        ok=!a->show_search && tab_header_geometry(a,false,true);a->dirty=true;tab_header_mouse(a,true,true);break;
    case 67:tab_header_mouse(a,false,true);break;
    case 68: {
        ok=tab_design_source_same(a);SDL_Event event={0};event.type=SDL_EVENT_WINDOW_CLOSE_REQUESTED;event.window.windowID=SDL_GetWindowID(a->panels[PS_DOCK_WORKSPACE].window);SDL_PushEvent(&event);tab_design.settle=0;break;
    }
    case 69:
        if(tab_design.settle++<3)return;
        ok=!panel_is_detached(a,PS_DOCK_WORKSPACE) && tab_header_geometry(a,false,false);tab_header_log(a,"returned");*capture="header-returned.png";break;
    case 70:puts("FILE TABS DESIGN SELF-TEST: PASSED");a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"FILE TABS DESIGN FAILED stage=%d variant=%u active=%u documents=%u\n",*stage,tab_design.variant,file_tab_active(a),a->document_count);*code=1;a->quitting=true;}else (*stage)++;
}
