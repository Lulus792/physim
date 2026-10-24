/* Actual pendulum runner plus native inspector input and independent raster logs. */
static struct {
    char parts[9][8][96];unsigned part_count[9];
    unsigned variant,settle,card,summary_seen;uint32_t source,scene,run_crc;double held;
    struct nk_rect title,heading_title,heading_meta;bool title_present,heading_present[2],value_present[PS_MODEL_VALUES_MAX];
} inspector_design;
static const char *inspector_design_variant(void) {
    const char *names[]={"dark","light","medium","larger","large","contrast"};return names[inspector_design.variant<6?inspector_design.variant:0];
}
static uint32_t inspector_design_scene(app *a) {
    ps_snapshot view={0};view.time=display_time(a);view.count=a->channel_count;
    memcpy(view.values,display_values(a),sizeof view.values);view.scene=*display_scene(a);return timeline_test_crc(&view);
}
static bool inspector_design_same(app *a) {
    return !a->dirty && a->paused && a->simulation_time==inspector_design.held &&
        inspector_design.source==ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string)) && inspector_design.scene==inspector_design_scene(a);
}
static bool inspector_design_start(app *a,const char *root) {
    memset(&inspector_design,0,sizeof inspector_design);
    bool ok=laboratory_test_start(a,root,"laboratory-dark");if(ok)join(a->preferences_path,sizeof a->preferences_path,root,"preferences.bin");return ok;
}
static void inspector_design_draw(app *a) {
    memset(inspector_design.parts,0,sizeof inspector_design.parts);memset(inspector_design.part_count,0,sizeof inspector_design.part_count);
    inspector_design.title_present=false;inspector_design.title=nk_rect(0,0,0,0);memset(inspector_design.value_present,0,sizeof inspector_design.value_present);
    memset(inspector_design.heading_present,0,sizeof inspector_design.heading_present);
    const struct nk_command *command;
    nk_foreach(command,a->ui)if(command->type==NK_COMMAND_TEXT) {
        const struct nk_command_text *text=(const struct nk_command_text*)command;
        if(text->font==&a->laboratory_section_font)for(unsigned i=0;i<9;i++) {
            struct nk_rect r=a->laboratory_section_bounds[i];
            if(r.w>0 && text->x>=r.x && text->x<r.x+r.w && text->y>=r.y && text->y+text->height<=r.y+r.h+1) {
                unsigned j=inspector_design.part_count[i]++;if(j<8)snprintf(inspector_design.parts[i][j],96,"%.*s",text->length,text->string);
            }
        }
        struct nk_rect header=a->dock_headers[PS_DOCK_INSPECTOR];
        if(text->x>=header.x && text->y>=header.y && text->y+text->height<=header.y+header.h+1) {
            if(text->font==&a->laboratory_section_font && text->length==9 && !memcmp(text->string,"Inspektor",9)) {
                inspector_design.heading_present[0]=true;inspector_design.heading_title=nk_rect(text->x,text->y,text->w,text->height);
            }
            if(text->font==&a->workspace_status_font && text->length==15 && !memcmp(text->string,"Alle Funktionen",15)) {
                inspector_design.heading_present[1]=true;inspector_design.heading_meta=nk_rect(text->x,text->y,text->w,text->height);
            }
        }
        if(text->font==&a->laboratory_section_font && text->length==6 && !memcmp(text->string,"Objekt",6)) {
            inspector_design.title_present=true;inspector_design.title=nk_rect(text->x,text->y,text->w,text->height);
        }
        if(text->font==&a->laboratory_model_font)for(unsigned i=0;i<a->model_edit.bindings.count;i++) {
            const char *value=a->model_edit.text[i];struct nk_rect r=a->model_input_bounds[i];
            if(text->x>=r.x-1 && text->x<r.x+r.w && text->y>=r.y-1 && text->y<r.y+r.h && text->length==(int)strlen(value) && !memcmp(text->string,value,(size_t)text->length))inspector_design.value_present[i]=true;
        }
    }
}
static const char *inspector_design_capture(const char *kind) {
    static char file[96];snprintf(file,sizeof file,"inspector-%s-%s.png",inspector_design_variant(),kind);return file;
}
static bool inspector_design_focus(app *a,const char *name) {
    if(SDL_GetKeyboardFocus()==a->window && nk_sdl_accessibility_is_focused(a->ui,NULL,name))return true;
    SDL_RaiseWindow(a->window);nk_sdl_accessibility_request_focus(a->ui,NULL,name);return false;
}
static void inspector_design_log(app *a,const char *kind) {
    printf("INSPECTOR DESIGN %s variant=%s size=%u source=%d title-present=%d heading-present=%d time=%.17g",kind,inspector_design_variant(),a->preferences.ui_size,inspector_design_same(a),inspector_design.title_present,inspector_design.heading_present[0] && inspector_design.heading_present[1],display_time(a));
    const struct nk_rect r[]={a->dock_headers[PS_DOCK_INSPECTOR],a->laboratory_inspector_body,a->laboratory_footer_bounds,inspector_design.title,inspector_design.heading_title,inspector_design.heading_meta};const char *names[]={"header","body","footer","title","heading-title","heading-meta"};
    for(unsigned i=0;i<6;i++)printf(" %s=%.4f,%.4f,%.4f,%.4f",names[i],r[i].x,r[i].y,r[i].w,r[i].h);
    printf(" minimum=%.4f wish=%u",laboratory_inspector_minimum(a),a->preferences.inspector_width);puts("");
    for(unsigned i=0;i<9;i++){struct nk_rect b=a->laboratory_section_bounds[i];printf("INSPECTOR SECTION slot=%u rect=%.4f,%.4f,%.4f,%.4f\n",i,b.x,b.y,b.w,b.h);}
    for(unsigned i=0;i<9;i++) {
        struct nk_rect row=a->laboratory_section_bounds[i],body=a->laboratory_inspector_body;
        if(!strncmp(kind,"closed",6) && row.y>=body.y-.03f && row.y+row.h<=body.y+body.h+.03f)inspector_design.summary_seen|=1u<<i;
        printf("INSPECTOR TITLE slot=%u parts=",i);
        for(unsigned j=0;j<inspector_design.part_count[i] && j<8;j++) {if(j)putchar(',');for(const unsigned char *p=(const unsigned char*)inspector_design.parts[i][j];*p;p++)printf("%02x",*p);}
        puts("");
    }
    if(a->model_disclosure==NK_MAXIMIZED)for(unsigned i=0;i<a->model_edit.bindings.count;i++) {
        struct nk_rect card=a->model_card_bounds[i],input=a->model_input_bounds[i];
        printf("INSPECTOR CARD index=%u value-present=%d card=%.4f,%.4f,%.4f,%.4f input=%.4f,%.4f,%.4f,%.4f\n",i,inspector_design.value_present[i],card.x,card.y,card.w,card.h,input.x,input.y,input.w,input.h);
    }
}
static void inspector_design_frame(app *a,int *stage,int *code,const char **capture) {
    bool ok=true;const char *object="Objekt auf-/zuklappen";
    switch(*stage) {
    case 0:if(inspector_design.settle++<3)return;if(SDL_GetKeyboardFocus()!=a->window){SDL_RaiseWindow(a->window);return;}build_project(a);break;
    case 1:if(!idle(a) || a->library_thread)return;ok=a->built && a->model_edit.bindings.count==2;if(ok)start_run(a);break;
    case 2:if(!a->hello || a->simulation_time<.2)return;command(a,PS_MSG_PAUSE);inspector_design.settle=0;break;
    case 3:
        if(!a->paused || inspector_design.settle++<2)return;
        inspector_design.source=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));inspector_design.held=a->simulation_time;inspector_design.scene=inspector_design_scene(a);
        ok=timeline_design_run_crc(a->last_run,&inspector_design.run_crc);break;
    case 4: {
        const unsigned sizes[]={16,16,18,20,22,22};ps_preferences p=a->preferences;p.ui_size=sizes[inspector_design.variant];p.theme=inspector_design.variant==0 || inspector_design.variant==2?PS_THEME_DARK:inspector_design.variant==5?PS_THEME_HIGH_CONTRAST:PS_THEME_LIGHT;p.inspector_width=inspector_design.variant<2?320:208;preferences_apply(a,&p);
        SDL_SetWindowSize(a->window,inspector_design.variant<2?1440:1080,inspector_design.variant<2?1000:740);
        for(unsigned i=0;i<5;i++)a->laboratory_groups[i]=NK_MINIMIZED;
        a->model_disclosure=a->parameter_disclosure=a->steps_disclosure=a->limits_disclosure=NK_MINIMIZED;a->laboratory_scroll_x=a->laboratory_scroll_y=0;inspector_design.summary_seen=0;inspector_design.settle=0;break;
    }
    case 5:if(inspector_design.settle++<3)return;ok=inspector_design_same(a) && inspector_design.title_present;inspector_design_log(a,"closed");*capture=inspector_design_capture("closed");inspector_design.settle=0;*stage=16;return;
    case 16:
        if(inspector_design.settle++<2)return;
        if(inspector_design.summary_seen==511){*stage=17;return;}
        {unsigned slot=0;while(inspector_design.summary_seen&(1u<<slot))slot++;
        struct nk_rect r=a->laboratory_section_bounds[slot],body=a->laboratory_inspector_body;
        if(r.y<body.y || r.y+r.h>body.y+body.h){test_plot_wheel(a,body.x+body.w/2,body.y+body.h/2,r.y<body.y?1:-1);inspector_design.settle=0;return;}
        char kind[40];snprintf(kind,sizeof kind,"closed-reveal-%u",slot);inspector_design_log(a,kind);*capture=inspector_design_capture(kind);inspector_design.settle=0;return;}
    case 17:
        if(inspector_design.settle++<2)return;
        if(a->laboratory_scroll_y){struct nk_rect r=a->laboratory_inspector_body;test_plot_wheel(a,r.x+r.w/2,r.y+r.h/2,1);inspector_design.settle=0;return;}
        *stage=6;return;
    case 6:if(!inspector_design_focus(a,object))return;*capture=inspector_design_capture("focus");test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 7:if(a->laboratory_groups[0]!=NK_MAXIMIZED)return;ok=inspector_design_same(a) && inspector_design_focus(a,object);*capture=inspector_design_capture("opened");test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 8:if(a->laboratory_groups[0]!=NK_MINIMIZED)return;ok=nk_sdl_accessibility_is_focused(a->ui,NULL,object) && inspector_design_same(a);test_mouse(a,a->laboratory_section_bounds[1],true);break;
    case 9:test_mouse(a,a->laboratory_section_bounds[1],false);inspector_design.settle=0;inspector_design.card=0;break;
    case 10:
        if(inspector_design.settle++<2)return;
        ok=a->model_disclosure==NK_MAXIMIZED && inspector_design_same(a);
        if(!ok)break;
        if(!camera_test_click(a,a->model_card_bounds[inspector_design.card]))return;
        break;
    case 11:test_mouse(a,camera_test_pressed,false);inspector_design.settle=0;break;
    case 12: {
        if(inspector_design.settle++<2)return;
        ok=inspector_design_same(a) && inspector_design.value_present[inspector_design.card];char kind[32];snprintf(kind,sizeof kind,"card-%u",inspector_design.card);inspector_design_log(a,kind);*capture=inspector_design_capture(kind);
        if(++inspector_design.card<2){*stage=10;return;}break;
    }
    case 13: {
        bool scrolled=a->laboratory_scroll_y>0;ok=inspector_design.variant<2 || scrolled;
        printf("INSPECTOR ACTIONS variant=%s enter-expand=1 enter-collapse=1 focus-retained=1 pointer-reveal=1 actual-wheel=%u source-and-run=1 scroll=%u\n",inspector_design_variant(),scrolled?1u:0u,a->laboratory_scroll_y);
        if(++inspector_design.variant<6){*stage=4;return;}break;
    }
    case 14:{uint32_t crc=0;ok=timeline_design_run_crc(a->last_run,&crc) && crc==inspector_design.run_crc && inspector_design_same(a);command(a,PS_MSG_STOP);a->stop_at=ps_clock();break;}
    case 15:if(!idle(a) || a->library_thread)return;ok=inspector_design_same(a);if(ok)puts("INSPECTOR DESIGN SELF-TEST: PASSED");a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"INSPECTOR DESIGN FAILED stage=%d variant=%u\n",*stage,inspector_design.variant);*code=1;a->quitting=true;}else (*stage)++;
}
