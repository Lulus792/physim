/* Text editing and saved synthetic run/report fixtures; no physics claim. */
static struct {unsigned variant,settle;bool wait,only,status;uint32_t experiment,analysis,values,times,runs[2],report_crc;ps_report *report;ps_layout_state layout;char selected[2][4096],report_path[4096];} analysis_editor_test;
static uint32_t analysis_editor_crc(const struct nk_text_edit *edit){return ps_crc32((const unsigned char*)nk_str_get_const(&edit->string),(size_t)nk_str_len_char(&edit->string));}
static bool analysis_editor_start(app *a,const char *root,const char *mode){
    memset(&analysis_editor_test,0,sizeof analysis_editor_test);analysis_editor_test.only=strstr(mode,"only")!=NULL;
    a->template_analysis_language=strstr(mode,"phys")?1:0;a->manager_analysis_only=analysis_editor_test.only;
    if(!analysis_workspace_test_start(a,root,"analysis-layout-dark"))return false;
    char path[4096];join(path,sizeof path,a->project,"runs/Lauf 002.psrun");select_run(a,path,true);
    if(!component_report(a))return false;
    join(analysis_editor_test.report_path,sizeof analysis_editor_test.report_path,a->project,"runs/editor-fixture.psreport");
    if(ps_report_save(a->analysis_report,analysis_editor_test.report_path)!=PS_OK)return false;
    snprintf(a->loaded_report_path,sizeof a->loaded_report_path,"%s",analysis_editor_test.report_path);
    a->tab=2;a->analysis_tab=0;a->show_report=true;return true;
}
static bool analysis_editor_preserved(app *a){
    uint32_t crc;if(a->analysis_report!=analysis_editor_test.report || !a->show_report || a->selected_count!=2 || a->data.thread)return false;
    for(unsigned i=0;i<2;i++)if(strcmp(a->selected_runs[i],analysis_editor_test.selected[i]) || !reset_test_crc(a->selected_runs[i],&crc) || crc!=analysis_editor_test.runs[i])return false;
    return analysis_editor_crc(&a->experiment)==analysis_editor_test.experiment && analysis_editor_crc(&a->analysis)==analysis_editor_test.analysis &&
        ps_crc32((const unsigned char*)a->data.values[0],(size_t)a->data.count*sizeof(double))==analysis_editor_test.values &&
        ps_crc32((const unsigned char*)a->data.time,(size_t)a->data.count*sizeof(double))==analysis_editor_test.times &&
        !strcmp(a->loaded_report_path,analysis_editor_test.report_path) && reset_test_crc(a->loaded_report_path,&crc) && crc==analysis_editor_test.report_crc;
}
static bool analysis_editor_layout_same(app *a){
    ps_layout_state current=laboratory_capture(a),*saved=&analysis_editor_test.layout;
    return !memcmp(&current.dock,&saved->dock,sizeof current.dock) && current.sidebar_width==saved->sidebar_width && current.inspector_width==saved->inspector_width && current.log_height==saved->log_height && current.show_log==saved->show_log;
}
static const char *analysis_editor_variant(void){const char *v[]={"dark","light","large","contrast"};return v[analysis_editor_test.variant];}
static void analysis_editor_draw(app *a){
    analysis_editor_test.status=false;const struct nk_command *command;char prefix[128];snprintf(prefix,sizeof prefix,"%s    Zeile ",analysis_source(a));size_t length=strlen(prefix);
    nk_foreach(command,a->ui)if(command->type==NK_COMMAND_TEXT){const struct nk_command_text *t=(const struct nk_command_text*)command;if(t->font==&a->workbench_fonts[1] && t->length>=(int)length && !memcmp(t->string,prefix,length))analysis_editor_test.status=true;}
}
static void analysis_editor_record(app *a,const char *kind,const char **capture){
    static char file[96];snprintf(file,sizeof file,"analysis-editor-%s-%s.png",analysis_editor_variant(),kind);*capture=file;
    printf("ANALYSIS EDITOR kind=%s variant=%s size=%u area=%d active=%u target=%d preserved=%d only=%d status-name=%d source-font=%.4f",kind,analysis_editor_variant(),a->preferences.ui_size,workspace_active_area(a),file_tab_active(a),editor_search_target(a)==&a->analysis,analysis_editor_preserved(a),a->analysis_only,analysis_editor_test.status,a->workbench_fonts[0].height);
    camera_design_rect("strip",a->file_tab_strip);camera_design_rect("tab",a->file_tab_bounds[1]);camera_design_rect("tree",a->workspace_tree_test_bounds);camera_design_rect("editor",a->source_editor_bounds);camera_design_rect("frame",a->source_editor_frame);camera_design_rect("search",a->search_island_bounds);camera_design_rect("nav",a->navigation_bounds[0]);camera_design_rect("context",a->dock_panels[PS_DOCK_INSPECTOR]);puts("");
}
static void analysis_editor_step(app *a,int *stage,int *code,const char **capture){
    bool ok=true;if(analysis_editor_test.wait){analysis_editor_test.wait=false;return;}analysis_editor_test.wait=true;
    if(a->data.thread || a->library_thread || ps_comparison_read(a->comparison).loading)return;
    switch(*stage){
    case 0:if(SDL_GetKeyboardFocus()!=a->window){SDL_RaiseWindow(a->window);return;}
        ok=a->selected_count==2 && a->analysis_report && workspace_tree_test_target(a,analysis_source(a),0);analysis_editor_test.report=a->analysis_report;analysis_editor_test.layout=laboratory_capture(a);
        analysis_editor_test.experiment=analysis_editor_crc(&a->experiment);analysis_editor_test.analysis=analysis_editor_crc(&a->analysis);
        analysis_editor_test.values=ps_crc32((const unsigned char*)a->data.values[0],(size_t)a->data.count*sizeof(double));analysis_editor_test.times=ps_crc32((const unsigned char*)a->data.time,(size_t)a->data.count*sizeof(double));
        for(unsigned i=0;i<2;i++){snprintf(analysis_editor_test.selected[i],4096,"%s",a->selected_runs[i]);ok &= reset_test_crc(a->selected_runs[i],&analysis_editor_test.runs[i]);}ok &= reset_test_crc(a->loaded_report_path,&analysis_editor_test.report_crc);break;
    case 1:if(!inspector_design_focus(a,"Analysecode"))return;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 2:if(a->completion_focus_edit || !a->analysis.active)return;ok=workbench_visible(a) && workspace_active_area(a)==0 && editor_search_target(a)==&a->analysis && file_tab_active(a)==1 && !a->analysis_layout_active && analysis_editor_preserved(a);break;
    case 3: {
        ps_preferences p=a->preferences;p.theme=analysis_editor_test.variant==0?PS_THEME_DARK:analysis_editor_test.variant==3?PS_THEME_HIGH_CONTRAST:PS_THEME_LIGHT;p.ui_size=analysis_editor_test.variant<2?16:22;preferences_apply(a,&p);
        SDL_SetWindowSize(a->window,analysis_editor_test.variant<2?1440:1080,analysis_editor_test.variant<2?1000:740);analysis_editor_test.settle=0;break;}
    case 4:if(analysis_editor_test.settle++<3)return;ok=analysis_editor_preserved(a) && (analysis_editor_test.only?a->file_tab_bounds[0].w==0:a->file_tab_bounds[0].w>0);analysis_editor_record(a,"workbench",capture);test_window_key_mod(a->window,SDLK_F,PS_UI_COMMAND_MOD);break;
    case 5:if(!a->show_search || !a->search_island_bounds.w)return;ok=analysis_editor_preserved(a);analysis_editor_record(a,"search",capture);test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    case 6:if(a->show_search || a->search_icon_focus)return;ok=analysis_editor_preserved(a);if(!analysis_editor_test.only && ++analysis_editor_test.variant<4){*stage=3;return;}test_window_key_mod(a->window,SDLK_3,PS_UI_COMMAND_MOD);break;
    case 7:ok=analysis_visible(a) && workspace_active_area(a)==2 && a->analysis_layout_active && analysis_editor_preserved(a) && analysis_editor_layout_same(a);
        if(!ok)break;
        *capture="analysis-editor-returned.png";test_window_key_mod(a->window,SDLK_1,PS_UI_COMMAND_MOD);break;
    case 8:if(a->completion_focus_edit || !a->analysis.active)return;ok=workbench_visible(a) && file_tab_active(a)==1 && analysis_editor_preserved(a);test_window_key_mod(a->window,SDLK_END,PS_UI_COMMAND_MOD);break;
    case 9:layout_test_text(a,"α");break;
    case 10:ok=a->analysis_dirty && analysis_editor_crc(&a->analysis)!=analysis_editor_test.analysis && analysis_editor_crc(&a->experiment)==analysis_editor_test.experiment;test_window_key_mod(a->window,SDLK_Z,PS_UI_COMMAND_MOD);break;
    case 11:ok=analysis_editor_crc(&a->analysis)==analysis_editor_test.analysis && a->analysis.cursor==nk_str_len(&a->analysis.string) && a->analysis.select_start==a->analysis.cursor && a->analysis.select_end==a->analysis.cursor;test_window_key_mod(a->window,SDLK_S,PS_UI_COMMAND_MOD);break;
    case 12:ok=!a->analysis_dirty && analysis_editor_preserved(a);preferences_capture(a);ok &= a->preferences.workspace==0;
        if(!analysis_editor_test.only){file_tab_select(a,0);analysis_editor_test.settle=0;*stage=15;return;}break;
    case 15:if(a->completion_focus_edit || analysis_editor_test.settle++<2)return;ok=workbench_visible(a) && editor_search_target(a)==&a->experiment && workspace_active_area(a)==0;
        {char path[4096];join(path,sizeof path,a->project,analysis_source(a));preview_workspace_path(a,path);}*stage=16;return;
    case 16:ok=workbench_visible(a) && editor_search_target(a)==&a->analysis && file_tab_active(a)==1 && analysis_editor_preserved(a);*stage=13;return;
    case 13:ok=!a->analysis_dirty && analysis_editor_preserved(a);if(!ok)break;file_tab_select(a,1);*stage=18;return;
    case 18:if(a->completion_focus_edit || !a->analysis.active)return;ok=a->file_tab_keyboard_edit==&a->analysis && nk_sdl_accessibility_request_focus(a->ui,NULL,"Darstellung umschalten");analysis_editor_test.settle=0;*stage=19;return;
    case 19:if(analysis_editor_test.settle++<3)return;ok=!a->file_tab_keyboard_edit && nk_sdl_accessibility_is_focused(a->ui,NULL,"Darstellung umschalten");if(!ok)break;test_window_key_mod(a->window,SDLK_F10,0);*stage=20;return;
    case 20:ok=a->toolbar_keyboard && !a->file_tab_keyboard_edit;if(!ok)break;test_window_key_mod(a->window,SDLK_ESCAPE,0);*stage=21;return;
    case 21:ok=!a->toolbar_keyboard && !a->toolbar_menu && analysis_editor_preserved(a);if(!ok)break;puts("ANALYSIS EDITOR ACTIONS code-button=1 primary-return=1 active-file=1 search=1 undo-save=1 data-report-source=1 area-persistence=1 native-focus-yield=1 menu=1");puts("ANALYSIS EDITOR SELF-TEST: PASSED");a->quitting=true;break;
    default:ok=false;break;
    }
    if(!ok){fprintf(stderr,"ANALYSIS EDITOR FAILED stage=%d variant=%u area=%d tab=%d/%d active=%u context=%d target=%d preserved=%d status=%s\n",*stage,analysis_editor_test.variant,workspace_active_area(a),a->tab,a->analysis_tab,file_tab_active(a),a->analysis_layout_active,editor_search_target(a)==&a->analysis,analysis_editor_preserved(a),a->status);*code=1;a->quitting=true;}else (*stage)++;
}
