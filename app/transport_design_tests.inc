/* Real runner and native keyboard/menu tasks, plus the approved nowrap geometry. */
static struct {unsigned variant,settle;uint32_t source_crc;bool complete;struct nk_rect text[6],icons[4];double held;char run[4096];} transport_design;
static const char *transport_design_variant(void) {return (const char*[]){"dark","light","medium","larger","large","contrast"}[transport_design.variant<6?transport_design.variant:5];}
static const char *transport_design_capture(const char *state) {
    static char path[96];snprintf(path,sizeof path,"transport-%s-%s.png",transport_design_variant(),state);return path;
}
static bool transport_design_start(app *a,const char *root) {
    memset(&transport_design,0,sizeof transport_design);char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    if(!laboratory_test_start(a,root,"laboratory-dark"))return false;
    transport_design.source_crc=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));return true;
}
static bool transport_design_source(app *a) {
    return !a->dirty && transport_design.source_crc==ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string)) &&
        (!transport_design.run[0] || !strcmp(transport_design.run,a->last_run));
}
static void transport_design_draw(app *a) {
    char main[80],speed[80];snprintf(main,sizeof main,"%s %s",a->runner.running && !a->paused?"Ⅱ":"▶",a->runner.running?(a->paused?"Fortsetzen":"Pause"):a->built?"Neuer Lauf":"Build");
    laboratory_speed_text(a->simulation_speed,speed,false);
    const char *expected[]={main,"▸│ Schritt","■ Stop","↺ Zurücksetzen",speed,"Tempo"};bool found[6]={0};
    memset(transport_design.text,0,sizeof transport_design.text);memset(transport_design.icons,0,sizeof transport_design.icons);
    const struct nk_command *cmd;
    nk_foreach(cmd,a->ui)if(cmd->type==NK_COMMAND_TEXT) {
        const struct nk_command_text *text=(const struct nk_command_text*)cmd;
        if(text->font!=&a->workbench_fonts[0] || text->y<WORKSPACE_CONTENT_TOP || text->y+text->height>195)continue;
        for(unsigned i=0;i<6;i++)if(text->length==(int)strlen(expected[i]) && !memcmp(text->string,expected[i],(size_t)text->length)) {
            found[i]=true;transport_design.text[i]=nk_rect(text->x,text->y,text->w,text->height);
            if(i<4) {
                nk_rune rune=0;nk_utf_decode(text->string,&rune,text->length);struct nk_user_font_glyph glyph;
                text->font->query(text->font->userdata,text->height,&glyph,rune,0);
                transport_design.icons[i]=nk_rect(text->x+glyph.offset.x,text->y+glyph.offset.y,glyph.width,glyph.height);
            }
        }
    }
    transport_design.complete=true;for(unsigned i=0;i<6;i++)transport_design.complete=transport_design.complete && found[i];
}
static bool transport_design_geometry(app *a) {
    int w,h;SDL_GetWindowSize(a->window,&w,&h);float f=a->preferences.ui_size/16.f;
    bool ok=transport_design.complete && fabsf(a->workbench_fonts[0].height/a->workbench_regular_em/f-11)<.001f;
    unsigned indices[]={0,2,4,3};float prior=0;
    for(unsigned i=0;i<4;i++) {
        struct nk_rect r=a->run_control_bounds[indices[i]];
        ok=ok && fabsf(r.h-(w<=1150?31:32)*f)<.01f && r.y>=WORKSPACE_CONTENT_TOP && r.y+r.h<=195 && r.x>=prior && r.x+r.w<w-18;
        prior=r.x+r.w;
    }
    struct nk_rect s=a->speed_bounds,c=a->laboratory_speed_chevron,p=a->laboratory_transport_status;
    ok=ok && fabsf(s.h-(w<=1150?31:32)*f)<.01f && fabsf(c.w-8*f)<.01f && fabsf(c.h-8*f)<.01f &&
        c.x>=s.x && c.x+c.w<s.x+s.w && c.y>=s.y && c.y+c.h<=s.y+s.h && p.x>=s.x+s.w && p.x+p.w<=w-17;
    const nk_rune glyphs[]={0x25b6,0x25b8,0x25a0,0x21ba,0x2161,0x25cb,0x25cf};
    for(unsigned i=0;i<SDL_arraysize(glyphs);i++)ok=ok && ps_ui_font_has_glyph(&a->workbench_fonts[0],glyphs[i]);
    return ok && transport_design_source(a);
}
static void transport_design_log(app *a,const char *state) {
    const nk_rune glyphs[]={0x25b6,0x25b8,0x25a0,0x21ba,0x2161,0x25cb,0x25cf};unsigned mask=0;
    for(unsigned i=0;i<SDL_arraysize(glyphs);i++)if(ps_ui_font_has_glyph(&a->workbench_fonts[0],glyphs[i]))mask|=1u<<i;
    printf("TRANSPORT %s variant=%s size=%u complete=%d glyphs=%u source=%d",state,transport_design_variant(),a->preferences.ui_size,transport_design.complete,mask,transport_design_source(a));
    unsigned indices[]={0,2,4,3};
    for(unsigned i=0;i<4;i++){struct nk_rect r=a->run_control_bounds[indices[i]];printf(" rect%u=%.4f,%.4f,%.4f,%.4f",i,r.x,r.y,r.w,r.h);r=transport_design.icons[i];printf(" icon%u=%.4f,%.4f,%.4f,%.4f",i,r.x,r.y,r.w,r.h);}
    struct nk_rect r=a->speed_bounds;printf(" speed=%.4f,%.4f,%.4f,%.4f",r.x,r.y,r.w,r.h);r=a->laboratory_speed_chevron;printf(" arrow=%.4f,%.4f,%.4f,%.4f",r.x,r.y,r.w,r.h);
    r=a->laboratory_transport_status;printf(" status=%.4f,%.4f,%.4f,%.4f value=%.3g paused=%d\n",r.x,r.y,r.w,r.h,a->simulation_speed,a->paused);
}
static bool transport_design_focused(app *a,const char *name) {
    if(nk_sdl_accessibility_is_focused(a->ui,NULL,name) && SDL_GetKeyboardFocus()==a->window)return true;
    SDL_RaiseWindow(a->window);nk_sdl_accessibility_request_focus(a->ui,NULL,name);return false;
}
static bool transport_design_returned(app *a) {
    if(nk_sdl_accessibility_is_focused(a->ui,NULL,"Simulationstempo auswählen") && SDL_GetKeyboardFocus()==a->window)return true;
    SDL_RaiseWindow(a->window);return false;
}
static void transport_design_frame(app *a,int *stage,int *code,const char **capture) {
    bool ok=true;
    switch(*stage) {
    case 0:if(transport_design.settle++<3)return;build_project(a);break;
    case 1:if(!idle(a) || a->library_thread)return;ok=a->built;if(!ok)break;transport_design.settle=0;*stage=100;return;
    case 100:if(transport_design.settle++<2)return;ok=transport_design_geometry(a);transport_design_log(a,"ready");*capture="transport-ready.png";break;
    case 101:start_run(a);snprintf(transport_design.run,sizeof transport_design.run,"%s",a->last_run);*stage=3;return;
    case 3:if(!a->hello || a->simulation_time<.2)return;command(a,PS_MSG_PAUSE);break;
    case 4:if(!a->paused)return;*stage=10;return;
    case 10: {
        const unsigned sizes[]={16,16,18,20,22,22};ps_preferences p=a->preferences;
        p.theme=transport_design.variant==0 || transport_design.variant==2?PS_THEME_DARK:transport_design.variant==5?PS_THEME_HIGH_CONTRAST:PS_THEME_LIGHT;p.ui_size=sizes[transport_design.variant];preferences_apply(a,&p);
        SDL_SetWindowSize(a->window,transport_design.variant<2?1440:1080,transport_design.variant<2?1000:740);transport_design.settle=0;
        test_plot_motion(a,10,10);test_mouse(a,nk_rect(10,10,1,1),true);test_mouse(a,nk_rect(10,10,1,1),false);break;
    }
    case 11:if(transport_design.settle++<3)return;ok=transport_design_geometry(a);break;
    case 12:transport_design_log(a,"normal");*capture=transport_design_capture("normal");test_plot_motion(a,a->run_control_bounds[0].x+20,a->run_control_bounds[0].y+16);break;
    case 13:*capture=transport_design_capture("hover");if(!nk_sdl_accessibility_request_focus(a->ui,NULL,"Fortsetzen"))return;break;
    case 14:if(!transport_design_focused(a,"Fortsetzen"))return;*capture=transport_design_capture("focus");break;
    case 15:test_window_key_mod(a->window,SDLK_RETURN,0);transport_design.settle=0;break;
    case 16:if(a->paused || transport_design.settle++<2)return;break;
    case 17:ok=transport_design_geometry(a);transport_design_log(a,"running");*capture=transport_design_capture("running");if(!nk_sdl_accessibility_request_focus(a->ui,NULL,"Pause"))return;break;
    case 18:if(!transport_design_focused(a,"Pause"))return;test_window_key_mod(a->window,SDLK_RETURN,0);transport_design.settle=0;break;
    case 19:if(!a->paused || transport_design.settle++<2)return;transport_design.held=a->simulation_time;test_mouse(a,a->run_control_bounds[2],true);break;
    case 20:test_mouse(a,a->run_control_bounds[2],false);break;
    case 21:if(a->simulation_time==transport_design.held)return;ok=a->paused && fabs(a->simulation_time-transport_design.held-a->dt)<1e-8;transport_design.held=a->simulation_time;break;
    case 22:test_mouse(a,a->speed_bounds,true);break;
    case 23:test_mouse(a,a->speed_bounds,false);break;
    case 24:*capture=transport_design_capture("menu");test_mouse(a,a->speed_choices[7],true);break;
    case 25:test_mouse(a,a->speed_choices[7],false);transport_design.settle=0;break;
    case 26:if(a->simulation_speed!=0 || transport_design.settle++<2)return;ok=a->paused && a->simulation_time==transport_design.held && transport_design_geometry(a);transport_design_log(a,"offline");*capture=transport_design_capture("offline");break;
    case 27:if(!transport_design_returned(a))return;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 28:if(!nk_sdl_accessibility_request_focus(a->ui,NULL,"1× Echtzeit"))return;break;
    case 29:if(!transport_design_focused(a,"1× Echtzeit"))return;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 30:if(a->simulation_speed!=1)return;ok=a->paused && a->simulation_time==transport_design.held && transport_design_source(a);break;
    case 31:if(!transport_design_returned(a))return;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 32:if(!nk_sdl_accessibility_request_focus(a->ui,NULL,"0,5× Echtzeit"))return;break;
    case 33:if(!transport_design_focused(a,"0,5× Echtzeit"))return;test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    case 34:if(!transport_design_returned(a))return;ok=a->simulation_speed==1 && a->paused && transport_design_source(a);break;
    case 35:
        printf("TRANSPORT ACTIONS variant=%s native-enter=1 step-dt=1 offline=1 native-menu=1 escape-return=1 source=1\n",transport_design_variant());
        if(++transport_design.variant<6){*stage=10;return;}command(a,PS_MSG_STOP);a->stop_at=ps_clock();break;
    case 36:if(!idle(a) || a->library_thread)return;ok=transport_design_source(a);puts("TRANSPORT DESIGN SELF-TEST: PASSED");a->quitting=true;break;
    }
    if(!ok){transport_design_log(a,"failure");fprintf(stderr,"TRANSPORT DESIGN FAILED stage=%d variant=%u status=%s\n",*stage,transport_design.variant,a->status);*code=1;a->quitting=true;}else (*stage)++;
}
