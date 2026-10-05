static bool docking_test_start(app *a,const char *root,const char *mode) {
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    if(!strcmp(mode,"dock-corrupt")) return !a->preferences_writable && a->preferences_error[0];
    if(!strcmp(mode,"dock-read")) return a->preferences_writable && (a->preferences.dock.floating&4);
    SDL_SetWindowSize(a->window,1080,740);
    join(a->project_input,sizeof a->project_input,root,"project");
    a->example=!strcmp(mode,"dock-live-c")?5:!strcmp(mode,"dock-live-phys")?10:0;new_project(a);
    if(!a->loaded) return false;
    if(!strncmp(mode,"dock-live-",10)) { a->dt=.01;a->project_settings_dirty=true;select_build_profile(a,1); }
    return true;
}
static void docking_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static struct nk_rect drag;static uint32_t source;static int reported=-1;
    static bool started;static double held_time;
    bool read=!strcmp(mode,"dock-read"),ok=true;
    bool live=!strncmp(mode,"dock-live-",10);
    ps_dock_layout *d=&a->preferences.dock;
    if(!strcmp(mode,"dock-corrupt")) { a->quitting=true;printf("DOCK %s SELF-TEST: PASSED\n",mode);return; }
    if(reported!=*stage) { fprintf(stderr,"Dock %s stage %d: root=%u float=%u hidden=%u\n",mode,*stage,d->root,d->floating,d->hidden);
        fprintf(stderr,"Dock drag=%d active=%d target=%u side=%u mouse=%.1f/%.1f header=%.1f/%.1f/%.1f/%.1f\n",a->dock_drag,a->dock_dragging,a->dock_target,a->dock_side,a->ui->input.mouse.pos.x,a->ui->input.mouse.pos.y,a->dock_headers[0].x,a->dock_headers[0].y,a->dock_headers[0].w,a->dock_headers[0].h);reported=*stage; }
    if(a->dock_dragging && !nk_input_is_mouse_down(&a->ui->input,NK_BUTTON_LEFT)) return;
    if(*stage==21 && a->dock_headers[0].w<=0) return;
    switch(*stage) {
    case 0:
        if(read) {
            uint32_t group=ps_dock_group(d,PS_DOCK_WORKSPACE);
            ok=group!=PS_DOCK_NONE && group==ps_dock_group(d,PS_DOCK_SIDEBAR) && d->nodes[group].active==PS_DOCK_WORKSPACE &&
               d->floats[PS_DOCK_LOG].w==520 && d->floats[PS_DOCK_LOG].h==300;
            *capture="dock-restored.bmp";*stage=39;
        } else {
            if(live) {
                if(!started) { build_project(a);started=true;return; }
                if(!idle(a) && !a->runner.running) return;
                if(!a->runner.running) {
                    if(!a->built) { *code=1;a->quitting=true;return; }
                    start_run(a);return;
                }
                if(!a->hello || a->simulation_time<.2) return;
                held_time=a->simulation_time;
            }
            nk_str_append_text_char(&a->experiment.string,"\n/* unsaved panel test */\n",26);a->dirty=true;
            source=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
            *stage=59;
        }
        break;
    case 1: drag=a->dock_headers[0];test_mouse(a,drag,true);break;
    case 2: {
        struct nk_rect r=a->dock_panels[1];test_plot_motion(a,r.x+r.w/2,r.y+r.h/2);break;
    }
    case 3: {
        struct nk_rect r=a->dock_panels[1];test_mouse(a,nk_rect(r.x+r.w/2,r.y+r.h/2,1,1),false);break;
    }
    case 4:
        ok=ps_dock_group(d,1)!=PS_DOCK_NONE && ps_dock_group(d,0)==ps_dock_group(d,1) && d->nodes[ps_dock_group(d,1)].active==0;
        *capture="dock-tabs.bmp";test_mouse(a,a->dock_headers[1],true);break;
    case 5: test_mouse(a,a->dock_headers[1],false);break;
    case 6:
        ok=ps_dock_group(d,1)!=PS_DOCK_NONE && d->nodes[ps_dock_group(d,1)].active==1;test_mouse(a,a->dock_headers[0],true);break;
    case 7: {
        struct nk_rect r=a->dock_panels[1];test_plot_motion(a,r.x+r.w/2+42,r.y+r.h/2);break;
    }
    case 8: {
        struct nk_rect r=a->dock_panels[1];test_mouse(a,nk_rect(r.x+r.w/2+42,r.y+r.h/2,1,1),false);break;
    }
    case 9:
        ok=ps_dock_group(d,0)!=ps_dock_group(d,1) && a->dock_panels[0].x>a->dock_panels[1].x;
        *capture="dock-right.bmp";test_mouse(a,a->dock_headers[2],true);break;
    case 10: test_plot_motion(a,970,100);break;
    case 11: test_mouse(a,nk_rect(970,100,1,1),false);break;
    case 12:
        ok=(d->floating&4)!=0;drag=a->dock_float_grips[2];
        *capture="dock-floating.bmp";test_mouse(a,drag,true);break;
    case 13: test_plot_motion(a,drag.x+drag.w/2-80,drag.y+drag.h/2+60);break;
    case 14: test_mouse(a,nk_rect(drag.x+drag.w/2-80,drag.y+drag.h/2+60,1,1),false);break;
    case 15:
        ok=d->floats[2].w==520 && d->floats[2].h==300;
        test_mouse(a,a->dock_closes[0],true);break;
    case 16: test_mouse(a,a->dock_closes[0],false);break;
    case 17:
        ok=(d->hidden&1)!=0;test_mouse(a,a->toolbar_bounds[1],true);break;
    case 18: test_mouse(a,a->toolbar_bounds[1],false);break;
    case 19: test_mouse(a,a->dock_menu_bounds[0],true);break;
    case 20: test_mouse(a,a->dock_menu_bounds[0],false);break;
    case 21:
        ok=!(d->hidden&1);test_mouse(a,a->dock_headers[0],true);break;
    case 22: {
        struct nk_rect r=a->dock_panels[1];test_plot_motion(a,r.x+r.w/2,r.y+r.h/2);break;
    }
    case 23: {
        struct nk_rect r=a->dock_panels[1];test_mouse(a,nk_rect(r.x+r.w/2,r.y+r.h/2,1,1),false);break;
    }
    case 24: test_mouse(a,a->dock_headers[1],true);break;
    case 25: test_mouse(a,a->dock_headers[1],false);break;
    case 26:
        ok=ps_dock_valid(d) && ps_dock_group(d,1)!=PS_DOCK_NONE && ps_dock_group(d,0)==ps_dock_group(d,1) &&
           d->nodes[ps_dock_group(d,1)].active==1 && (d->floating&4) && a->dirty &&
           source==ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
        *capture="dock-saved-layout.bmp";
        if(live) {
            ok &= a->runner.running && !a->paused && a->simulation_time>held_time;
            test_mouse(a,a->dock_closes[2],true);
        } else a->quitting=true;
        break;
    case 27: test_mouse(a,a->dock_closes[2],false);break;
    case 28:
        ok=(d->hidden&4)!=0;
        test_mouse(a,a->run_control_bounds[4],true);break;
    case 29: test_mouse(a,a->run_control_bounds[4],false);break;
    case 30:
        if(!idle(a) || a->library_thread) return;
        ok=a->built && a->simulation_time>held_time && a->data.result==PS_EOF && a->data.total>2;
        open_settings(a);*stage=79;break;
    case 80: test_mouse(a,a->settings_bounds[3],true);break;
    case 81: test_mouse(a,a->settings_bounds[3],false);break;
    case 82: test_mouse(a,a->settings_bounds[0],true);break;
    case 83: test_mouse(a,a->settings_bounds[0],false);break;
    case 84: {
        ok=a->show_log && !(d->hidden&4) && ps_dock_group(d,2)!=PS_DOCK_NONE &&
           ps_dock_group(d,0)==ps_dock_group(d,1) && a->dirty &&
           source==ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
        ps_preferences saved;
        ok &= ps_preferences_read(a->preferences_path,&saved)==PS_OK && !memcmp(&saved.dock,d,sizeof *d);
        *capture="dock-settings-restored.bmp";a->quitting=true;break;
    }
    case 40: test_mouse(a,a->dock_headers[2],true);break;
    case 41: {
        struct nk_rect r=a->dock_panels[1];test_plot_motion(a,r.x+r.w/2,r.y+r.h/2+42);break;
    }
    case 42: {
        struct nk_rect r=a->dock_panels[1];test_mouse(a,nk_rect(r.x+r.w/2,r.y+r.h/2+42,1,1),false);break;
    }
    case 43:
        ok=!d->floating && ps_dock_group(d,2)!=PS_DOCK_NONE;
        test_mouse(a,a->toolbar_bounds[1],true);break;
    case 44: test_mouse(a,a->toolbar_bounds[1],false);break;
    case 45: test_mouse(a,a->dock_menu_bounds[2],true);break;
    case 46: test_mouse(a,a->dock_menu_bounds[2],false);break;
    case 47:
        ok=!memcmp(d,&PS_DOCK_DEFAULT,sizeof *d);*capture="dock-reset.bmp";
        test_mouse(a,a->dock_headers[0],true);break;
    case 48: {
        struct nk_rect r=a->dock_panels[2];test_plot_motion(a,r.x+r.w/2,r.y+r.h/2+42);break;
    }
    case 49: {
        struct nk_rect r=a->dock_panels[2];test_mouse(a,nk_rect(r.x+r.w/2,r.y+r.h/2+42,1,1),false);break;
    }
    case 50:
        ok=d->nodes[d->root].kind==PS_DOCK_Y && d->nodes[d->nodes[d->root].second].kind==PS_DOCK_Y;
        drag=a->dock_splitters[d->root];test_mouse(a,drag,true);break;
    case 51: test_plot_motion(a,drag.x+drag.w/2,730);break;
    case 52: test_mouse(a,nk_rect(drag.x+drag.w/2,730,1,1),false);break;
    case 53:
        ok=ps_dock_valid(d) && a->dock_panels[0].h>=64 && a->dock_panels[1].h>=64 && a->dock_panels[2].h>=64;
        *capture="dock-minimum-splits.bmp";a->quitting=true;break;
    case 60: test_mouse(a,a->dock_headers[0],true);break;
    case 61: test_plot_motion(a,950,110);break;
    case 62: test_window_key_mod(a->window,SDLK_ESCAPE,SDL_KMOD_NONE);break;
    case 63: test_mouse(a,nk_rect(950,110,1,1),false);break;
    case 64:
        ok=!memcmp(d,&PS_DOCK_DEFAULT,sizeof *d) && !a->dock_dragging;
        *stage=0;break;
    }
    if(!ok) { fprintf(stderr,"Dock failure stage %d source=%u widths=%u/%u root=%u float=%u hidden=%u\n",*stage,source,d->floats[2].w,d->floats[2].h,d->root,d->floating,d->hidden);*code=1;a->quitting=true; }
    if(a->quitting) printf("DOCK %s SELF-TEST: %s\n",mode,*code?"FAILED":"PASSED");
    (*stage)++;
}
