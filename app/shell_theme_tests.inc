/* Actual header actions across all three contexts; no project rebuild is needed. */
static void shell_theme_test_frame(app *a,int *stage,int *code,const char **capture) {
    static bool settle;static unsigned expected;static ps_layout_state layout;
    static ps_statistics statistics;static ps_plot_view view;static uint32_t source_crc,values_crc;
    static double simulation_time;bool ok=true;
    if(settle){settle=false;return;}settle=true;
    if(a->data.thread || a->library_thread)return;
    switch(*stage) {
    case 0:
        if(SDL_GetKeyboardFocus()!=a->window){SDL_RaiseWindow(a->window);return;}
        source_crc=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
        values_crc=ps_crc32((const unsigned char*)a->data.values[0],a->data.count*sizeof(double));
        statistics=a->data.stats[0];simulation_time=a->simulation_time;
        a->experiment.select_start=2;a->experiment.select_end=6;a->experiment.cursor=6;
        break;
    case 1: {
        ok=a->toolbar_theme_bounds.w>0;
#ifdef __APPLE__
        ok=ok && native_test_shell(a);
        if(ok)puts("SHELL THEME NATIVE HEADER: menu=AppKit ui-menus=0 theme-button=1");
#else
        ok=ok && SDL_arraysize(toolbar_top_order)==5 && a->toolbar_bounds[3].w==0;
        for(unsigned i=1;i<SDL_arraysize(toolbar_top_order);i++)ok=ok && a->toolbar_bounds[toolbar_top_order[i]].x>a->toolbar_bounds[toolbar_top_order[i-1]].x;
#endif
        SDL_Point p={(int)(a->toolbar_theme_bounds.x+10),(int)(a->toolbar_theme_bounds.y+15)};
        ok=ok && window_hit_test(a->window,&p,a)==SDL_HITTEST_NORMAL;
        layout=laboratory_capture(a);expected=a->preferences.theme==PS_THEME_LIGHT?PS_THEME_DARK:PS_THEME_LIGHT;
        *capture="shell-before.bmp";test_mouse(a,a->toolbar_theme_bounds,true);break;
    }
    case 2:test_mouse(a,a->toolbar_theme_bounds,false);break;
    case 3:
        ok=a->preferences.theme==expected && a->preferences.sidebar_width==layout.sidebar_width && a->preferences.inspector_width==layout.inspector_width &&
            a->preferences.log_height==layout.log_height && !memcmp(&a->preferences.dock,&layout.dock,sizeof layout.dock) &&
            a->experiment.select_start==2 && a->experiment.select_end==6 && a->experiment.cursor==6;
        *capture="shell-develop-toggled.bmp";select_workspace_tab(a,1);break;
    case 4:layout=laboratory_capture(a);expected=a->preferences.theme==PS_THEME_LIGHT?PS_THEME_DARK:PS_THEME_LIGHT;
        ok=nk_sdl_accessibility_request_focus(a->ui,NULL,"Darstellung umschalten");
        if(!ok) {
            struct nk_window *header=nk_window_find(a->ui,"header");
            fprintf(stderr,"SHELL THEME FOCUS: header-flags=%u target=%.1f/%.1f/%.1f/%.1f\n",header?(unsigned)header->flags:0,a->toolbar_theme_bounds.x,a->toolbar_theme_bounds.y,a->toolbar_theme_bounds.w,a->toolbar_theme_bounds.h);
            if(header)for(struct nk_window *above=header->next;above;above=above->next) {
                struct nk_rect covered=plot_intersection(above->bounds,a->toolbar_theme_bounds);
                if(!(above->flags&(NK_WINDOW_HIDDEN|NK_WINDOW_CLOSED|NK_WINDOW_MINIMIZED)) && covered.w>0 && covered.h>0)fprintf(stderr,"SHELL THEME COVER: %s\n",above->name_string);
            }
        }break;
    case 5:if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"Darstellung umschalten"))return;test_window_key_mod(a->window,SDLK_SPACE,0);break;
    case 6:
        ok=a->preferences.theme==expected && a->laboratory_active && a->preferences.sidebar_width==layout.sidebar_width &&
            a->preferences.inspector_width==layout.inspector_width && a->preferences.log_height==layout.log_height && !memcmp(&a->preferences.dock,&layout.dock,sizeof layout.dock);
        *capture="shell-simulate-toggled.bmp";select_workspace_tab(a,2);break;
    case 7:
        layout=laboratory_capture(a);view=a->data_views[0];expected=a->preferences.theme==PS_THEME_LIGHT?PS_THEME_DARK:PS_THEME_LIGHT;
        open_documentation(a,1);break;
    case 8:
        ok=a->doc_visible && a->doc_ui!=NULL;
        ok=ok && nk_sdl_accessibility_request_focus(a->ui,NULL,"Darstellung umschalten");break;
    case 9:if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"Darstellung umschalten"))return;test_window_key_mod(a->window,SDLK_SPACE,0);break;
    case 10:
        ok=a->preferences.theme==expected && a->analysis_layout_active && a->doc_visible && a->preferences.sidebar_width==layout.sidebar_width &&
            a->preferences.inspector_width==layout.inspector_width && a->preferences.log_height==layout.log_height && !memcmp(&a->preferences.dock,&layout.dock,sizeof layout.dock);
        *capture="shell-analyze-toggled.bmp";break;
    case 11:*capture="docs-shell-theme.bmp";documentation_window_hide(a);break;
    case 12:
        ok=source_crc==ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string)) &&
            values_crc==ps_crc32((const unsigned char*)a->data.values[0],a->data.count*sizeof(double)) &&
            !memcmp(&statistics,&a->data.stats[0],sizeof statistics) && !memcmp(&view,&a->data_views[0],sizeof view) &&
            simulation_time==a->simulation_time && a->selected_count==1;
        printf("SHELL THEME INVARIANTS: source, selection, layout wishes, samples, statistics, view and simulation time unchanged=%d\n",ok);
        a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Shell theme stage %d failed: theme=%u expected=%u\n",*stage,a->preferences.theme,expected);*code=1;a->quitting=true;*capture="shell-theme-failed.bmp";}
    if(a->quitting)printf("SHELL THEME SELF-TEST: %s\n",*code?"FAILED":"PASSED");
    (*stage)++;
}
