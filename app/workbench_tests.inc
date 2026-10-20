static bool workbench_test_start(app *a,const char *root,const char *mode) {
    join(a->project_input,sizeof a->project_input,root,"project");a->example=0;new_project(a);
    if(!a->loaded)return false;
    ps_preferences p=a->preferences;p.theme=strstr(mode,"light")?PS_THEME_LIGHT:PS_THEME_DARK;
    if(strstr(mode,"contrast"))p.theme=PS_THEME_HIGH_CONTRAST;
    if(strstr(mode,"large")){p.ui_size=22;p.editor_size=22;}
    preferences_apply(a,&p);
    SDL_SetWindowSize(a->window,strstr(mode,"narrow")?1080:strstr(mode,"large")?1600:strstr(mode,"unified")?1440:1204,strstr(mode,"narrow")?740:strstr(mode,"large") || strstr(mode,"unified")?1000:866);
    ps_dock_move(&a->preferences.dock,PS_DOCK_LOG,PS_DOCK_WORKSPACE,PS_DOCK_BOTTOM);
    a->show_log=true;refresh_workspace_entries(a);return true;
}
static bool workbench_test_rect(struct nk_rect a,struct nk_rect b) {
    return a.x==b.x && a.y==b.y && a.w==b.w && a.h==b.h;
}
static bool workbench_test_separator_focus(app *a,const char *name) {
    if(nk_sdl_accessibility_request_focus(a->ui,NULL,name))return true;
    static unsigned logged;
    if(logged++<2)for(unsigned id=0;id<PS_DOCK_NODES;id++) {
        if(a->dock_splitters[id].w<=0)continue;
        char label[48];if(strcmp(dock_resize_label(a,id,label),name))continue;
        char window[48];snprintf(window,sizeof window,"dock-splitter-%u",id);
        struct nk_window *owner=nk_window_find(a->ui,window);
        fprintf(stderr,"WORKBENCH SEPARATOR WAIT: %s owner=%s seq=%u/current=%u\n",name,window,owner?owner->seq:0,a->ui->seq);
        if(owner)for(struct nk_window *above=owner->next;above;above=above->next) {
            struct nk_rect cover=plot_intersection(above->bounds,a->dock_splitters[id]);
            if(!(above->flags&(NK_WINDOW_HIDDEN|NK_WINDOW_MINIMIZED|NK_WINDOW_CLOSED)) && cover.w>0 && cover.h>0)
                fprintf(stderr,"WORKBENCH SEPARATOR COVER: %s seq=%u rect=%.1f/%.1f/%.1f/%.1f\n",above->name_string,above->seq,above->bounds.x,above->bounds.y,above->bounds.w,above->bounds.h);
        }
    }
    return false;
}
static void workbench_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static struct nk_rect editor_rect,context_rect;static struct nk_vec2 scroll;
    static unsigned side,warmup,mouse_wait,wheel_wait,close_wait;static int selected_start,selected_end,selected_cursor;
    static double focus_retry;bool ok=true;
    struct nk_text_edit *edit=&a->experiment;
    switch(*stage) {
    case 0:
        if(a->library_thread || a->source_editor_bounds.w<=0)return;
        ok=a->preferences.sidebar_width==218 && a->preferences.inspector_width==255 && a->preferences.log_height==152;
        if(!ok)break;
        /* SDL test events do not activate the Windows host. Begin the focus
         * task only after activation is actually observed, as for separators. */
        if(SDL_GetKeyboardFocus()!=a->window) {
            if(ps_clock()>=focus_retry){SDL_RaiseWindow(a->window);focus_retry=ps_clock()+.25;}
            return;
        }
        if(!warmup) {
            const struct nk_user_font *font=&a->workbench_fonts[0];
            printf("WORKBENCH TYPOGRAPHY: ui=%.3f em=%.6f role=%.3f Datei-width=%.3f code=%.3f\n",a->font_ui->height,a->workbench_regular_em,font->height,font->width(font->userdata,font->height,"Datei",5),a->font_code->height);
        }
        if(warmup++<3)return;
        editor_rect=a->source_editor_bounds;context_rect=a->workbench_context_bounds;
        edit->select_start=2;edit->select_end=6;edit->cursor=6;
        edit->scrollbar=nk_vec2(32,32);scroll=edit->scrollbar;
        test_window_key_mod(a->window,SDLK_F,PS_UI_COMMAND_MOD);break;
    case 1:
        if(a->show_search && a->search_query_bounds.w<=0)return;
        ok=a->show_search && a->search_query_bounds.w>0 && workbench_test_rect(editor_rect,a->source_editor_bounds);
        ok=ok && a->search_island_bounds.w<=540 && a->search_island_bounds.w<=a->source_editor_frame.w-16;
        ok=ok && a->search_island_bounds.y==a->source_editor_frame.y+4;
        ok=ok && a->search_island_bounds.x+a->search_island_bounds.w==a->source_editor_frame.x+a->source_editor_frame.w-8;
        if(strstr(mode,"opaque") || strstr(mode,"contrast"))ok=ok && a->search_material.opaque;
        ok=ok && edit->select_start==2 && edit->select_end==6 && edit->scrollbar.x==scroll.x && edit->scrollbar.y==scroll.y;
        test_window_key_mod(a->window,SDLK_F,PS_UI_COMMAND_MOD);break;
    case 2:
        ok=a->show_search && workbench_test_rect(editor_rect,a->source_editor_bounds);
        strcpy(a->find,"length_m");test_mouse(a,a->search_expand_bounds,true);break;
    case 3:test_mouse(a,a->search_expand_bounds,false);break;
    case 4:
        ok=a->search_replace_open && workbench_test_rect(editor_rect,a->source_editor_bounds);
        ok=ok && edit->select_start==2 && edit->select_end==6 && edit->scrollbar.x==scroll.x && edit->scrollbar.y==scroll.y;
        test_plot_motion(a,a->source_editor_bounds.x+20,a->source_editor_bounds.y+200);
        editor_search_open(a);test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 5:
        ok=edit->select_end>edit->select_start;
        *capture=strstr(mode,"light")?"workbench-light-expanded.bmp":"workbench-dark-expanded.bmp";
        test_window_key_mod(a->window,SDLK_RETURN,SDL_KMOD_SHIFT);break;
    case 6:
        selected_start=edit->select_start;selected_end=edit->select_end;selected_cursor=edit->cursor;scroll=edit->scrollbar;
        test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    case 7:
        if(close_wait++<2)return;
        ok=!a->show_search && workbench_test_rect(editor_rect,a->source_editor_bounds);
        ok=ok && edit->select_start==selected_start && edit->select_end==selected_end && edit->cursor==selected_cursor;
        ok=ok && edit->scrollbar.x==scroll.x && edit->scrollbar.y==scroll.y;
        if(ok && !nk_sdl_accessibility_is_focused(a->ui,NULL,"Dokumentsuche öffnen")) {
            /* Keep all geometry/selection assertions while waiting for the
             * existing product focus request; never inject that request here. */
            if(ps_clock()>=focus_retry){SDL_RaiseWindow(a->window);focus_retry=ps_clock()+.25;}
            return;
        }
        ok=ok && nk_sdl_accessibility_is_focused(a->ui,NULL,"Dokumentsuche öffnen");
        if(!ok){fprintf(stderr,"Workbench close: visible=%d selection=%d..%d expected=%d..%d cursor=%d/%d scroll=%.1f/%.1f expected=%.1f/%.1f icon-focus=%d keyboard-host=%d\n",a->show_search,edit->select_start,edit->select_end,selected_start,selected_end,edit->cursor,selected_cursor,edit->scrollbar.x,edit->scrollbar.y,scroll.x,scroll.y,nk_sdl_accessibility_is_focused(a->ui,NULL,"Dokumentsuche öffnen"),SDL_GetKeyboardFocus()==a->window);
            for(struct nk_window *win=a->ui->begin;win;win=win->next) {
                struct nk_rect cover=win->bounds,r=a->workbench_search_bounds;
                if(!(win->flags&(NK_WINDOW_HIDDEN|NK_WINDOW_MINIMIZED|NK_WINDOW_CLOSED)) && cover.x<r.x+r.w && cover.x+cover.w>r.x && cover.y<r.y+r.h && cover.y+cover.h>r.y)
                    fprintf(stderr,"WORKBENCH FOCUS COVER: %s %.1f/%.1f/%.1f/%.1f target %.1f/%.1f/%.1f/%.1f\n",win->name_string,cover.x,cover.y,cover.w,cover.h,r.x,r.y,r.w,r.h);
            }break;}
        edit->scrollbar=nk_vec2(0,0);edit->cursor=edit->select_start=edit->select_end=0;
        editor_search_open(a);a->search_replace_open=false;break;
    case 8:
        if(!a->api_reference) {
            const struct nk_rect *r=a->workbench_api_text_bounds;
            ok=r[0].h>0 && r[1].h>0 && r[2].h>0 && r[0].y+r[0].h+13<=r[1].y && r[1].y+r[1].h+13<=r[2].y;
            printf("WORKBENCH API EMPTY: message-bottom=%.3f hint=%.3f/%.3f action-top=%.3f disjoint=%d\n",r[0].y+r[0].h,r[1].y,r[1].y+r[1].h,r[2].y,ok);
        }
        if(a->preferences.ui_size==16 && !strstr(mode,"narrow")) {
            struct nk_rect pane=a->dock_panels[PS_DOCK_INSPECTOR],environment=a->workbench_environment_bounds;
            ok=ok && environment.h>0 && environment.y>=pane.y && environment.y+environment.h<=pane.y+pane.h-4;
            printf("WORKBENCH ENVIRONMENT: %.1f/%.1f within %.1f/%.1f visible=%d\n",environment.y,environment.y+environment.h,pane.y,pane.y+pane.h,ok);
        }
        *capture=strstr(mode,"light")?"workbench-light.bmp":"workbench-dark.bmp";
        if(!ok)break;
        if(strstr(mode,"visual"))a->quitting=true;
        else {wheel_wait=0;test_plot_wheel(a,a->source_editor_bounds.x+20,a->source_editor_bounds.y+200,-2);*stage=50;return;}
        break;
    case 50:
        if(!wheel_wait++)return;
        ok=edit->scrollbar.y>0 && a->search_keyboard;
        if(ok){edit->scrollbar=nk_vec2(0,0);*stage=9;return;}
        break;
    case 9:case 12:case 15:case 18:
        test_mouse(a,a->workbench_context_bounds,true);break;
    case 10:case 13:case 16:case 19:
        test_mouse(a,a->workbench_context_bounds,false);break;
    case 11:case 14:case 17:case 20:
        ok=workbench_test_rect(context_rect,a->workbench_context_bounds);
        ok=ok && a->workbench_context_bounds.x>a->workbench_save_bounds.x && a->workbench_save_bounds.x>a->workbench_search_bounds.x;
        ok=ok && ((a->preferences.dock.hidden&(1u<<PS_DOCK_INSPECTOR))!=0)==((side%2)==0);
        if(side==0)*capture=strstr(mode,"light")?"workbench-light-right-hidden.bmp":"workbench-dark-right-hidden.bmp";
        if(side==2)*capture=strstr(mode,"light")?"workbench-light-both-hidden.bmp":"workbench-dark-both-hidden.bmp";
        if(++side==2){test_mouse(a,a->workbench_sidebar_bounds,true);*stage=51;return;}
        break;
    case 51:test_mouse(a,a->workbench_sidebar_bounds,false);*stage=15;return;
    case 21:
        *capture=strstr(mode,"light")?"workbench-light-sides-hidden.bmp":"workbench-dark-sides-hidden.bmp";
        test_mouse(a,a->workbench_sidebar_bounds,true);break;
    case 22:test_mouse(a,a->workbench_sidebar_bounds,false);break;
    case 23:
        ok=!(a->preferences.dock.hidden&(1u<<PS_DOCK_SIDEBAR)) && a->preferences.sidebar_width==218;
        if(ok && (a->workbench_sidebar_focus || !nk_sdl_accessibility_is_focused(a->ui,NULL,"Workspace ein-/ausblenden")))return;
        if(ok && !nk_sdl_accessibility_request_focus(a->ui,NULL,"Workspace-Breite"))return;
        break;
    case 24:
        if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"Workspace-Breite")) {
            /* Scripted SDL pointer events do not activate the real OS host.
             * Wait for actual published host focus, including a delayed raise. */
            if(ps_clock()>=focus_retry){SDL_RaiseWindow(a->window);focus_retry=ps_clock()+.25;}
            return;
        }
        test_window_key_mod(a->window,SDLK_RIGHT,0);break;
    case 25:ok=a->preferences.sidebar_width==228;test_window_key_mod(a->window,SDLK_RIGHT,SDL_KMOD_SHIFT);break;
    case 26:ok=a->preferences.sidebar_width==268;test_window_key_mod(a->window,SDLK_HOME,0);break;
    case 27:ok=a->preferences.sidebar_width==140;test_window_key_mod(a->window,SDLK_END,0);break;
    case 28:ok=a->preferences.sidebar_width==480;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 29:ok=a->preferences.sidebar_width==218;test_window_key_mod(a->window,SDLK_LEFT,0);break;
    case 30:ok=a->preferences.sidebar_width==208;test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    case 31:
        /* Escape cancels a pointer transaction; a completed keyboard step stays. */
        ok=a->preferences.sidebar_width==208 && !a->dock_resize_transaction;
        if(ok)ok=nk_sdl_accessibility_request_focus(a->ui,NULL,"Workspace-Breite");
        if(!ok)break;*stage=60;return;
    case 60:
        if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"Workspace-Breite"))return;
        test_window_key_mod(a->window,SDLK_RETURN,0);*stage=61;return;
    case 61:
        ok=a->preferences.sidebar_width==218;
        if(ok && a->dock_panels[PS_DOCK_SIDEBAR].w!=218)return;
        if(!ok)break;
        for(unsigned id=0;id<PS_DOCK_NODES;id++)if(dock_sidebar_split(&a->preferences.dock,&a->preferences.dock.nodes[id])) {
            struct nk_rect r=a->dock_splitters[id];fprintf(stderr,"WORKBENCH DRAG: id=%u rect=%.1f/%.1f/%.1f/%.1f width=%u\n",id,r.x,r.y,r.w,r.h,a->preferences.sidebar_width);test_mouse(a,r,true);
        }
        *stage=32;return;
    case 32:
        if(!a->dock_resize && !mouse_wait++)return;
        ok=a->dock_resize!=0;
        test_plot_motion(a,310,a->source_editor_bounds.y+120);break;
    case 33:test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    case 34:
        ok=a->preferences.sidebar_width==218 && !a->dock_resize;
        test_mouse(a,nk_rect(310,a->source_editor_bounds.y+120,1,1),false);break;
    case 35:
        ok=workbench_test_rect(context_rect,a->workbench_context_bounds);
        a->preferences.sidebar_width=280;break;
    case 36:
        if(a->dock_panels[PS_DOCK_SIDEBAR].w!=280)return;
        mouse_wait=0;
        for(unsigned id=0;id<PS_DOCK_NODES;id++)if(dock_sidebar_split(&a->preferences.dock,&a->preferences.dock.nodes[id]))
            test_window_mouse_count(a->window,a->dock_splitters[id],true,2);
        break;
    case 37:
        if(!mouse_wait++)return; /* Docking consumes the mouse during drawing. */
        ok=a->preferences.sidebar_width==218 && !a->dock_resize;
        for(unsigned id=0;id<PS_DOCK_NODES;id++)if(dock_sidebar_split(&a->preferences.dock,&a->preferences.dock.nodes[id]))test_mouse(a,a->dock_splitters[id],false);
        break;
    case 38:editor_search_open(a);a->layout_manager=true;break;
    case 39:
        ok=editor_search_target(a)==NULL;
        test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    case 40:ok=!a->layout_manager && a->show_search;test_window_key_mod(a->window,SDLK_ESCAPE,0);*stage=70;return;
    case 70:
        if(a->show_search || a->search_icon_focus)return;
        /* The closed island and modal disappear from the published stack at
         * frame completion. Wait for the real target, within the native cap. */
        if(!workbench_test_separator_focus(a,"Codekontext-Breite"))return;
        break;
    case 71:
        if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"Codekontext-Breite"))return;
        test_window_key_mod(a->window,SDLK_HOME,0);break;
    case 72:ok=a->preferences.inspector_width==180;test_window_key_mod(a->window,SDLK_LEFT,0);break;
    case 73:ok=a->preferences.inspector_width==190;test_window_key_mod(a->window,SDLK_LEFT,SDL_KMOD_SHIFT);break;
    case 74:ok=a->preferences.inspector_width==230;test_window_key_mod(a->window,SDLK_END,0);break;
    case 75:ok=a->preferences.inspector_width==480;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 76:
        ok=a->preferences.inspector_width==255;
        if(ok && a->dock_panels[PS_DOCK_INSPECTOR].w!=255)return;
        if(!ok)break;
        if(!workbench_test_separator_focus(a,"Protokoll-Höhe"))return;
        break;
    case 77:
        if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"Protokoll-Höhe"))return;
        test_window_key_mod(a->window,SDLK_HOME,0);break;
    case 78:ok=a->preferences.log_height==126;test_window_key_mod(a->window,SDLK_UP,0);break;
    case 79:ok=a->preferences.log_height==136;test_window_key_mod(a->window,SDLK_UP,SDL_KMOD_SHIFT);break;
    case 80:ok=a->preferences.log_height==176;test_window_key_mod(a->window,SDLK_END,0);break;
    case 81:ok=a->preferences.log_height==366;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 82:
        ok=a->preferences.log_height==152;
        if(ok && a->dock_panels[PS_DOCK_LOG].h!=152)return;
        printf("WORKBENCH GEOMETRY: workspace=%.1f context=%.1f log=%.1f body=%.1f editor-height=%.1f\n",a->dock_panels[PS_DOCK_SIDEBAR].w,a->dock_panels[PS_DOCK_INSPECTOR].w,a->dock_panels[PS_DOCK_LOG].h,a->dock_panels[PS_DOCK_LOG].h-42-24,a->dock_panels[PS_DOCK_WORKSPACE].h);
        *capture="workbench-reset-geometry.bmp";a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"WORKBENCH TEST failed stage %d: edit %.1f/%.1f/%.1f/%.1f expected %.1f/%.1f/%.1f/%.1f, query %.1f show %d, scroll %.1f/%.1f selection %d..%d\n",*stage,a->source_editor_bounds.x,a->source_editor_bounds.y,a->source_editor_bounds.w,a->source_editor_bounds.h,editor_rect.x,editor_rect.y,editor_rect.w,editor_rect.h,a->search_query_bounds.w,a->show_search,edit->scrollbar.x,edit->scrollbar.y,edit->select_start,edit->select_end);*capture="workbench-failure.bmp";*code=1;a->quitting=true;}
    if(a->quitting)printf("WORKBENCH SELF-TEST: %s\n",*code?"FAILED":"PASSED");
    ++*stage;
}
