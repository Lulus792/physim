/* Real files, native focus/activation, CSS geometry and a genuinely wide tab. */
static struct {unsigned variant;char long_path[4096],long_name[106];uint32_t source_crc;bool long_text,end_visible;} tab_design;
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
    case 39:puts("FILE TABS DESIGN SELF-TEST: PASSED");a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"FILE TABS DESIGN FAILED stage=%d variant=%u active=%u documents=%u\n",*stage,tab_design.variant,file_tab_active(a),a->document_count);*code=1;a->quitting=true;}else (*stage)++;
}
