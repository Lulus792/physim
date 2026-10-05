static bool inspector_test_start(app *a,const char *root,const char *mode) {
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    join(a->project_input,sizeof a->project_input,root,"project");
    if(!strcmp(mode,"inspector-read")) {open_project(a);return a->loaded && !a->built;}
    a->example=!strcmp(mode,"inspector-c")?0:8;new_project(a);
    if(!a->loaded)return false;
    SDL_SetWindowSize(a->window,1080,740);ps_dock_hide(&a->preferences.dock,PS_DOCK_LOG);
    a->scene_disclosure=NK_MAXIMIZED;a->dt=.01;a->project_settings_dirty=true;
    select_build_profile(a,1);return true;
}
static void inspector_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static uint32_t source,body;static double started;
    static struct nk_rect grip;
    bool ok=true;ps_dock_layout *d=&a->preferences.dock;
    if(a->dock_dragging && !nk_input_is_mouse_down(&a->ui->input,NK_BUTTON_LEFT))return;
    if(!strcmp(mode,"inspector-read")) {
        ok=(d->floating&8) && d->floats[3].w==320 && d->floats[3].h==480 && !a->runner.running;
        *capture="inspector-reopened.bmp";a->quitting=true;
    } else switch(*stage) {
    case 0:build_project(a);break;
    case 1:
        if(!idle(a) || a->library_thread)return;
        ok=a->built;start_run(a);break;
    case 2:
        if(!a->hello || a->simulation_time<.2)return;
        started=a->simulation_time;
        nk_str_append_text_char(&a->experiment.string,"\n/* inspector keeps source */\n",(int)sizeof "\n/* inspector keeps source */\n"-1);a->dirty=true;
        source=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
        ok=a->dock_visible[0] && a->dock_visible[1] && a->dock_visible[3] && ps_dock_group(d,0)!=ps_dock_group(d,3);
        *capture="inspector-docked.bmp";test_mouse(a,a->dock_headers[3],true);break;
    case 3: {
        struct nk_rect r=a->dock_panels[1];test_plot_motion(a,r.x+r.w-10,r.y+90);break;
    }
    case 4: {
        struct nk_rect r=a->dock_panels[1];test_mouse(a,nk_rect(r.x+r.w-10,r.y+90,1,1),false);break;
    }
    case 5:
        ok=(d->floating&8) && a->runner.running && !a->paused && a->simulation_time>started;
        body=PS_MAX_OBJECTS;
        for(unsigned i=0;i<a->scene.count;i++)if(a->scene.objects[i].id==3)body=i;
        ok &= body<PS_MAX_OBJECTS;
        if(!ok)break;
        *capture="inspector-floating.bmp";
        if(!hierarchy_test_click(a,a->scene_visibility_bounds[body]))return;
        break;
    case 6:test_mouse(a,a->scene_visibility_bounds[body],false);break;
    case 7:
        ok=(a->scene_view.hidden&(1u<<body)) && a->runner.running && !a->paused;
        test_mouse(a,a->dock_closes[3],true);break;
    case 8:test_mouse(a,a->dock_closes[3],false);break;
    case 9:
        ok=(d->hidden&8) && !(d->floating&8) && a->dock_visible[0] && a->dock_visible[1];
        *capture="inspector-hidden.bmp";test_mouse(a,a->toolbar_bounds[1],true);break;
    case 10:test_mouse(a,a->toolbar_bounds[1],false);break;
    case 11:test_mouse(a,a->dock_menu_bounds[3],true);break;
    case 12:test_mouse(a,a->dock_menu_bounds[3],false);break;
    case 13:
        ok=!(d->hidden&8) && a->dock_visible[3];test_mouse(a,a->dock_headers[3],true);break;
    case 14: {
        struct nk_rect r=a->dock_panels[0];test_plot_motion(a,r.x+r.w/2,r.y+r.h/2);break;
    }
    case 15: {
        struct nk_rect r=a->dock_panels[0];test_mouse(a,nk_rect(r.x+r.w/2,r.y+r.h/2,1,1),false);break;
    }
    case 16:
        ok=ps_dock_group(d,3)==ps_dock_group(d,0) && a->dock_visible[3] && !a->dock_visible[0];
        *capture="inspector-tabs.bmp";test_mouse(a,a->dock_headers[0],true);break;
    case 17:test_mouse(a,a->dock_headers[0],false);break;
    case 18:
        if(d->nodes[ps_dock_group(d,0)].active==0 && !a->dock_visible[0])return;
        ok=a->dock_visible[0] && !a->dock_visible[3];test_mouse(a,a->dock_headers[3],true);break;
    case 19: {
        struct nk_rect r=a->dock_panels[1];test_plot_motion(a,r.x+r.w-10,r.y+90);break;
    }
    case 20: {
        struct nk_rect r=a->dock_panels[1];test_mouse(a,nk_rect(r.x+r.w-10,r.y+90,1,1),false);break;
    }
    case 21:
        ok=(d->floating&8) && a->runner.running && !a->paused;
        grip=a->dock_float_grips[3];test_mouse(a,grip,true);break;
    case 22:test_plot_motion(a,grip.x+grip.w/2+40,grip.y+grip.h/2-80);break;
    case 23:test_mouse(a,nk_rect(grip.x+grip.w/2+40,grip.y+grip.h/2-80,1,1),false);break;
    case 24:
        ok=d->floats[3].w==320 && d->floats[3].h==480 && a->dirty && a->simulation_time>started &&
           source==ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
        *capture="inspector-resized.bmp";command(a,PS_MSG_STOP);a->stop_at=ps_clock();break;
    case 25:
        if(!idle(a) || a->library_thread)return;
        ok=a->data.result==PS_EOF && a->data.total>2;
        preferences_capture(a);ok &= ps_preferences_write(a->preferences_path,&a->preferences)==PS_OK;
        a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Inspector failed at stage %d: floating=%u hidden=%u time=%.17g\n",*stage,d->floating,d->hidden,a->simulation_time);*code=1;a->quitting=true;}
    if(a->quitting)printf("INSPECTOR %s SELF-TEST: %s\n",mode,*code?"FAILED":"PASSED");
    (*stage)++;
}
