/* Real controls and a separately labelled maximum-storage layout stress. */
static struct {
    char path[4096],stress_path[4096];bool stress,tools_probe,failed,tip_end_visible;
    unsigned settle;uint32_t crc;int cursor,start,end;struct nk_vec2 editor_scroll;
    ps_plot_view raw;unsigned scroll;
} component_details;
static uint32_t component_editor_crc(app *a) {
    return ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
}
static bool component_editor_same(app *a) {
    return component_editor_crc(a)==component_details.crc && a->experiment.cursor==component_details.cursor &&
        a->experiment.select_start==component_details.start && a->experiment.select_end==component_details.end &&
        a->experiment.scrollbar.x==component_details.editor_scroll.x && a->experiment.scrollbar.y==component_details.editor_scroll.y;
}
static void component_editor_save(app *a) {
    component_details.crc=component_editor_crc(a);component_details.cursor=a->experiment.cursor;
    component_details.start=a->experiment.select_start;component_details.end=a->experiment.select_end;component_details.editor_scroll=a->experiment.scrollbar;
}
static bool component_report(app *a) {
    if(ps_report_create("Werkzeugprüfung","Native Testdaten",&a->analysis_report)!=PS_OK)return false;
    ps_plot_info info={0};strcpy(info.title,"Werkzeugdiagramm");strcpy(info.x_label,"Zeit");strcpy(info.y_label,"Position");
    ps_report_unit_from(PS_SECOND,&info.x_unit);ps_report_unit_from(PS_METRE,&info.y_unit);
    ps_plot_handle plot;if(ps_report_add_plot(a->analysis_report,&info,&plot)!=PS_OK)return false;
    ps_curve_data *curve=calloc(1,sizeof *curve);if(!curve)return false;
    strcpy(curve->label,"Messreihe");curve->kind=PS_PLOT_LINE;curve->count=curve->source_count=3;
    for(unsigned i=0;i<3;i++){curve->x[i]=i;curve->y[i]=i==1?2:1;}
    bool ok=ps_report_add_curve(a->analysis_report,plot,curve)==PS_OK;free(curve);
    strcpy(info.title,"Diagramm ohne Messpunkte");return ok && ps_report_add_plot(a->analysis_report,&info,&plot)==PS_OK;
}
static bool component_details_start(app *a,const char *root,const char *mode) {
    memset(&component_details,0,sizeof component_details);
    if(strstr(mode,"chart")) {
        if(!analysis_workspace_test_start(a,root,mode) || !component_report(a))return false;
        a->tab=2;a->analysis_tab=0;return true;
    }
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    if(!workbench_test_start(a,root,mode) || !workspace_tree_test_target(a,"physim.project",0))return false;
    snprintf(component_details.path,sizeof component_details.path,"%s",a->workspace_tree_test_path);
    SDL_SetWindowSize(a->window,strstr(mode,"narrow")?1080:1440,strstr(mode,"narrow")?740:1000);return true;
}
static void component_tools_log(app *a,const char *kind,const struct nk_rect *buttons) {
    int w,h;SDL_GetWindowSize(a->window,&w,&h);printf("COMPONENT TOOLS %s size=%u viewport=%d,%d",kind,a->preferences.ui_size,w,h);
    for(unsigned i=0;i<4;i++){struct nk_rect r=i<3?buttons[i]:a->result_export_bounds[2];printf(" rect%u=%.4f,%.4f,%.4f,%.4f",i,r.x,r.y,r.w,r.h);}
    printf("\n");
}
static void component_tip_log(app *a,const char *kind) {
    int w,h;SDL_GetWindowSize(a->window,&w,&h);struct nk_rect r=a->workspace_tip.bounds;
    printf("COMPONENT TIP %s bytes=%zu lines=%u scroll=%u scrollable=%u content=%.4f rect=%.4f,%.4f,%.4f,%.4f viewport=%d,%d\n",
        kind,strlen(a->workspace_tip.text),a->workspace_tip.lines,a->workspace_tip.scroll,a->workspace_tip.scrollable,a->workspace_tip.content_height,r.x,r.y,r.w,r.h,w,h);
}
static void component_details_draw(app *a,const char *mode) {
    if(!component_details.stress && !component_details.tools_probe)return;
    struct nk_context *ui=a->ui;const ps_ui_palette *previous=ui_palette;ui_palette=ps_workbench_palette(previous);
    nk_size text_begin=0;bool check_text=false,check_tip=false;
    if(component_details.stress)workspace_tip_frame_begin(a);
    nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_hide());
    nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(0,0));nk_style_push_float(ui,&ui->style.window.border,0);
    nk_style_push_font(ui,&a->workbench_fonts[2]);
    struct nk_rect box=component_details.stress?nk_rect(0,0,40,40):nk_rect(300,220,180,450);
    if(nk_begin(ui,"component-probe",box,NK_WINDOW_NO_INPUT|NK_WINDOW_NO_SCROLLBAR)) {
        if(component_details.stress) {
            text_begin=ui->memory.allocated;check_tip=true;
            /* The probe container is invisible; its tooltip must use the same
             * opaque workbench material as an actual workspace-tree owner. */
            nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(UI_COLOR(window)));
            workspace_tip_draw(a,component_details.stress_path);nk_style_pop_style_item(ui);
        }
        else {
            analysis_tools_layout p=analysis_tools_measure(a,180,4);text_begin=ui->memory.allocated;check_text=true;
            (void)analysis_tools_draw(a,&p,box.x,box.y,true);
            bool geometry=p.rows>1;
            for(unsigned i=0;i<4;i++) {
                struct nk_rect r=p.buttons[i];geometry=geometry && r.x>=-.001f && r.y>=0 && r.w>0 && r.x+r.w<=180.001f && r.y+r.h<=p.height+.001f;
                for(unsigned j=0;j<i;j++){struct nk_rect q=p.buttons[j];geometry=geometry && !(r.x<q.x+q.w-.001f && r.x+r.w>q.x+.001f && r.y<q.y+q.h-.001f && r.y+r.h>q.y+.001f);}
            }
            if(!geometry){fprintf(stderr,"COMPONENT WRAPPED GEOMETRY FAILED\n");component_details.failed=true;}
            printf("COMPONENT WRAPPED size=%u rows=%u height=%.4f geometry=%d mode=%s\n",a->preferences.ui_size,p.rows,p.height,geometry,mode);
        }
    }
    nk_end(ui);
    if(check_tip) {
        char text[4096]={0};size_t at=0;const struct nk_command *cmd;
        nk_foreach(cmd,ui)if((nk_size)((const char*)cmd-(const char*)ui->memory.memory.ptr)>=text_begin && cmd->type==NK_COMMAND_TEXT) {
            const struct nk_command_text *t=(const struct nk_command_text*)cmd;
            if(at+(size_t)t->length<sizeof text){memcpy(text+at,t->string,(size_t)t->length);at+=(size_t)t->length;}
        }
        component_details.tip_end_visible=strstr(text,"Ende 😀.phys")!=NULL;
    }
    if(check_text) {
        char text[256]={0};size_t at=0;const struct nk_command *cmd;
        nk_foreach(cmd,ui)if((nk_size)((const char*)cmd-(const char*)ui->memory.memory.ptr)>=text_begin && cmd->type==NK_COMMAND_TEXT) {
            const struct nk_command_text *t=(const struct nk_command_text*)cmd;
            if(at+(size_t)t->length<sizeof text){memcpy(text+at,t->string,(size_t)t->length);at+=(size_t)t->length;}
        }
        bool complete=!strcmp(text,"+−Alles zeigenGrafik exportieren…");printf("COMPONENT WRAPPED TEXT complete=%d\n",complete);
        if(!complete){fprintf(stderr,"COMPONENT WRAPPED TEXT LOST: %s\n",text);component_details.failed=true;}
    }
    nk_style_pop_font(ui);nk_style_pop_float(ui);nk_style_pop_vec2(ui);nk_style_pop_style_item(ui);
    if(component_details.stress)workspace_tip_frame_end(a);ui_palette=previous;
}
static void component_chart_frame(app *a,int *stage,int *code,const char **capture) {
    bool ok=true;ps_plot_view *view=&a->data_views[a->plot_channel];
    switch(*stage) {
    case 0:
        if(a->data.thread || !ps_comparison_read(a->comparison).snapshot || a->result_export_bounds[2].w<=0)return;
        component_details.raw=*view;test_plot_motion(a,10,10);break;
    case 1:*capture="tools-normal.png";component_tools_log(a,"raw",a->plot_buttons);break;
    case 2:test_plot_motion(a,a->plot_buttons[0].x+a->plot_buttons[0].w/2,a->plot_buttons[0].y+a->plot_buttons[0].h/2);break;
    case 3:*capture="tools-hover.png";break;
    case 4:ok=nk_sdl_accessibility_request_focus(a->ui,NULL,"Zoom +");break;
    case 5:
        if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"Zoom +") || SDL_GetKeyboardFocus()!=a->window){SDL_RaiseWindow(a->window);return;}
        *capture="tools-focus.png";break;
    case 6:test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 7:if(view->high[0]-view->low[0]>=1)return;ok=view->low[0]>0 && view->high[0]<1;break;
    case 8:*capture="tools-zoom.png";break;
    case 9:test_mouse(a,a->plot_buttons[2],true);break;
    case 10:test_mouse(a,a->plot_buttons[2],false);break;
    case 11:ok=view->low[0]==0 && view->high[0]==1;component_details.raw=*view;a->show_report=true;a->analysis_tab=0;a->result_view=0;break;
    case 12:if(!a->report_views[0].initialized)return;break;
    case 13:*capture="tools-report.png";component_tools_log(a,"report",a->plot_buttons);break;
    case 14:test_plot_motion(a,a->result_export_bounds[2].x+15,a->result_export_bounds[2].y+10);break;
    case 15:*capture="tools-report-hover.png";break;
    case 16:test_mouse(a,a->plot_buttons[0],true);break;
    case 17:test_mouse(a,a->plot_buttons[0],false);break;
    case 18:ok=a->report_views[0].low[0]>0 && view->low[0]==component_details.raw.low[0] && view->high[0]==component_details.raw.high[0];a->result_view=1;break;
    case 19:if(!a->report_views[1].initialized)return;break;
    case 20:*capture="tools-disabled.png";component_tools_log(a,"disabled",a->plot_buttons);break;
    case 21:test_mouse(a,a->result_export_bounds[2],true);break;
    case 22:test_mouse(a,a->result_export_bounds[2],false);break;
    case 23:ok=!a->graphic_export && a->report_views[1].low[0]==0 && a->report_views[1].high[0]==1;component_details.tools_probe=true;break;
    case 24:*capture="tools-wrapped.png";break;
    case 25:component_details.tools_probe=false;printf("COMPONENT CHART SELF-TEST: PASSED\n");a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"COMPONENT CHART FAILED: %d\n",*stage);*code=1;a->quitting=true;}else(*stage)++;
}
static void component_tip_frame(app *a,int *stage,int *code,const char **capture) {
    bool ok=true;int w,h;SDL_GetWindowSize(a->window,&w,&h);
    switch(*stage) {
    case 0:if(a->workspace_tree_test_bounds.w<=0)return;component_editor_save(a);test_plot_motion(a,a->workspace_tree_test_bounds.x+20,a->workspace_tree_test_bounds.y+8);break;
    case 1:if(!a->workspace_tip.visible)return;ok=!strcmp(a->workspace_tip.text,component_details.path);component_tip_log(a,"real");*capture="tip-real.png";break;
    case 2:test_plot_motion(a,a->workspace_tip.bounds.x+a->workspace_tip.bounds.w/2,a->workspace_tip.bounds.y+a->workspace_tip.bounds.h/2);break;
    case 3:ok=a->workspace_tip.visible && !strcmp(a->workspace_tip.text,component_details.path) && component_editor_same(a);*capture="tip-retained.png";break;
    case 4: {
        ps_preferences p=a->preferences;p.ui_size=22;preferences_apply(a,&p);SDL_SetWindowSize(a->window,1080,740);a->workspace_open=false;a->workspace_tip.visible=false;
        memset(component_details.stress_path,'W',sizeof component_details.stress_path-1);component_details.stress_path[4095]=0;
        const char *prefix="D:/Messungen/α/",*suffix="/Ende 😀.phys";memcpy(component_details.stress_path,prefix,strlen(prefix));memcpy(component_details.stress_path+4095-strlen(suffix),suffix,strlen(suffix));
        test_plot_motion(a,1064,720);component_details.stress=true;break;
    }
    case 5:
        if(!a->workspace_tip.visible || a->workspace_tip.viewport_width!=1080)return;
        ok=strlen(a->workspace_tip.text)==4095 && a->workspace_tip.offsets[a->workspace_tip.lines]==4095 && a->workspace_tip.scrollable;
        ok=ok && a->workspace_tip.bounds.x>=8 && a->workspace_tip.bounds.y>=8 && a->workspace_tip.bounds.x+a->workspace_tip.bounds.w<=w-8 && a->workspace_tip.bounds.y+a->workspace_tip.bounds.h<=h-8;
        component_editor_save(a);component_tip_log(a,"maximum-layout-stress");*capture="tip-maximum.png";break;
    case 6:test_plot_motion(a,a->workspace_tip.bounds.x+30,a->workspace_tip.bounds.y+40);break;
    case 7:component_details.scroll=a->workspace_tip.scroll;test_plot_wheel(a,a->workspace_tip.bounds.x+30,a->workspace_tip.bounds.y+40,-8);break;
    case 8:
        if(a->workspace_tip.scroll<=component_details.scroll)return;
        ok=component_editor_same(a) && !strcmp(a->workspace_tip.text,component_details.stress_path);component_tip_log(a,"scrolled");*capture="tip-scrolled.png";break;
    case 9:test_plot_wheel(a,a->workspace_tip.bounds.x+30,a->workspace_tip.bounds.y+40,-1000);break;
    case 10:if(!component_details.tip_end_visible)return;ok=component_editor_same(a);component_tip_log(a,"bottom-end-visible");*capture="tip-bottom.png";break;
    case 11:test_plot_wheel(a,a->workspace_tip.bounds.x+30,a->workspace_tip.bounds.y+40,1000);break;
    case 12:if(a->workspace_tip.scroll)return;*capture="tip-top.png";break;
    case 13:component_details.stress=false;test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    case 14:ok=!a->workspace_tip.visible && component_editor_same(a);*capture="tip-dismissed.png";break;
    case 15:printf("COMPONENT TOOLTIP SELF-TEST: PASSED\n");a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"COMPONENT TOOLTIP FAILED: %d scroll=%u lines=%u visible=%d\n",*stage,a->workspace_tip.scroll,a->workspace_tip.lines,a->workspace_tip.visible);*code=1;a->quitting=true;}else(*stage)++;
}
