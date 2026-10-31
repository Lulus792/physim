/* Actual app transaction, file replacement and workspace ownership. This does
 * not accept the still-outstanding Settings A window's design or keyboard order. */
static void test_settings_merge_frame(app *a,int *stage,int *code) {
    bool ok=true;
    ps_preferences readback;
    static ps_preferences saved,active,draft;
    static ps_layout_state analysis,laboratory;
    static char path[4096];
    switch(*stage) {
    case 0:
        SDL_SetWindowSize(a->window,1200,800);SDL_SyncWindow(a->window);
        SDL_strlcpy(path,a->preferences_path,sizeof path);
        open_settings(a);
        a->settings_draft.theme=PS_THEME_LIGHT;a->settings_draft.editor_size=20;
        /* Changes owned by the primary window after opening. */
        a->preferences.sidebar_width=333;a->preferences.inspector_width=366;a->preferences.log_height=430;
        a->show_paths=false;a->show_grid=false;a->view_disclosure=NK_MAXIMIZED;
        ok=ps_dock_hide(&a->preferences.dock,PS_DOCK_INSPECTOR);
        settings_commit(a);
        ok=ok && a->tab==0 && a->preferences.theme==PS_THEME_LIGHT && a->preferences.editor_size==20 &&
            a->preferences.sidebar_width==333 && a->preferences.inspector_width==366 && a->preferences.log_height==430 &&
            !a->show_paths && !a->show_grid && a->view_disclosure==NK_MAXIMIZED &&
            (a->preferences.dock.hidden&(1u<<PS_DOCK_INSPECTOR)) &&
            ps_preferences_read(path,&saved)==PS_OK && !memcmp(&saved,&a->preferences,sizeof saved);
        printf("SETTINGS MERGE: stale appearance draft preserves geometry, disclosure, topology and visibility=%d\n",ok);
        break;
    case 1:
        /* Enter both real workspace contexts and give each its own sizes. */
        a->tab=2;a->analysis_tab=0;workspace_context(a);
        a->preferences.sidebar_width=292;a->preferences.inspector_width=311;
        a->tab=1;workspace_context(a);
        a->preferences.sidebar_width=277;a->preferences.inspector_width=321;
        open_settings(a);
        analysis=a->analysis_layout;laboratory=a->laboratory_layout;
        ok=a->analysis_layout_initialized && a->laboratory_initialized &&
            a->settings_draft.sidebar_width==333 && a->settings_draft.inspector_width==366;
        settings_defaults_draft(a);draft=a->settings_draft;
        preferences_capture(a);active=a->preferences;
        ok=ok && a->preferences.theme==PS_THEME_LIGHT && a->settings_draft.theme==PS_THEME_DARK &&
            !memcmp(&draft.dock,&active.dock,sizeof draft.dock);
        /* A real failed replacement, followed by retry with the same draft. */
        SDL_strlcpy(a->preferences_path,a->project,sizeof a->preferences_path);
        settings_commit(a);
        ok=ok && a->tab==6 && *a->preferences_error && !memcmp(&a->preferences,&active,sizeof active) &&
            !memcmp(&a->settings_draft,&draft,sizeof draft) && ps_preferences_read(path,&readback)==PS_OK &&
            !memcmp(&readback,&saved,sizeof saved) && !memcmp(&analysis,&a->analysis_layout,sizeof analysis) &&
            !memcmp(&laboratory,&a->laboratory_layout,sizeof laboratory);
        printf("SETTINGS MERGE: defaults draft and failed save preserve active values, file and both sessions=%d\n",ok);
        SDL_strlcpy(a->preferences_path,path,sizeof a->preferences_path);
        if(ok)settings_commit(a);
        workspace_context(a);
        ok=ok && a->tab==1 && a->preferences.theme==PS_THEME_DARK && a->preferences.editor_size==12 &&
            a->laboratory_initialized && a->analysis_layout_initialized &&
            a->preferences.sidebar_width==277 && a->preferences.inspector_width==321 &&
            !memcmp(&analysis,&a->analysis_layout,sizeof analysis) &&
            ps_preferences_read(path,&saved)==PS_OK && saved.sidebar_width==218 && saved.inspector_width==255 && saved.log_height==340;
        printf("SETTINGS MERGE: retry saves Develop defaults and preserves Simulieren/Auswerten sessions=%d\n",ok);
        break;
    case 2:
        a->tab=2;workspace_context(a);
        ok=a->preferences.sidebar_width==292 && a->preferences.inspector_width==311;
        a->tab=0;workspace_context(a);
        ok=ok && a->preferences.sidebar_width==218 && a->preferences.inspector_width==255;
        open_settings(a);preferences_capture(a);active=a->preferences;
        a->settings_draft.sidebar_width=139;settings_commit(a);
        ok=ok && a->tab==6 && *a->preferences_error && a->settings_draft.sidebar_width==139 &&
            !memcmp(&a->preferences,&active,sizeof active);
        ok=ok && ps_preferences_read(path,&readback)==PS_OK && !memcmp(&readback,&saved,sizeof saved);
        test_window_key_mod(a->window,SDLK_ESCAPE,0);
        break;
    case 3:
        ok=a->tab==0;
        printf("SETTINGS MERGE: invalid range preserves draft, active state and file; Escape cancels=%d\n",ok);
        a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Settings merge failed at stage %d: %s\n",*stage,a->preferences_error);*code=1;a->quitting=true;}
    if(a->quitting)printf("SETTINGS MERGE SELF-TEST: %s\n",*code?"FAILED":"PASSED");
    (*stage)++;
}
