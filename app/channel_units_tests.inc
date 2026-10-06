static bool channel_units_test_start(app *a,const char *root,const char *mode) {
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    join(a->project_input,sizeof a->project_input,root,"project");SDL_SetWindowSize(a->window,1080,740);
    if(strcmp(mode,"units-c") && strcmp(mode,"units-phys")){open_project(a);return a->loaded;}
    a->example=!strcmp(mode,"units-c")?0:8;new_project(a);a->dt=.01;a->project_settings_dirty=true;select_build_profile(a,1);return a->loaded;
}
static void channel_units_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static bool settle;static double started;static uint32_t source;static unsigned selected;static ps_plot_view view;
    if(settle){settle=false;return;}settle=true;bool ok=true;
    bool running=!strcmp(mode,"units-c") || !strcmp(mode,"units-phys");
    if(!running) {
        ps_channel ch={.name="position.x",.unit="m",.dimension={1}};
        switch(*stage) {
        case 0:
            ok=!strcmp(mode,"units-read")?a->channel_units_writable && a->channel_units.count==1:!a->channel_units_writable && *a->channel_units_error;
            channel_unit_open(a,&ch);break;
        case 1:
            *capture="units-reopened.bmp";
            if(!strcmp(mode,"units-read")) {
                ok=a->unit_draft.scale==.01 && !strcmp(a->unit_draft.symbol,"cm");a->quitting=true;
            } else if(!strcmp(mode,"units-corrupt"))a->quitting=true;
            else test_mouse(a,a->channel_unit_bounds[5],true);
            break;
        case 2:test_mouse(a,a->channel_unit_bounds[5],false);break;
        case 3:ok=a->channel_units_writable && !a->channel_units.count && !*a->channel_units_error;a->quitting=true;break;
        }
    } else switch(*stage) {
    case 0:build_project(a);break;
    case 1:if(!idle(a) || a->library_thread)return;ok=a->built;start_run(a);break;
    case 2:
        if(!a->hello || a->simulation_time<.2)return;
        selected=a->channel_count;
        for(unsigned i=0;i<a->channel_count;i++)if(!strcmp(a->live_channels[i].name,"position.x"))selected=i;
        ok=selected<a->channel_count && a->live_channels[selected].dimension[0]==1;
        if(!ok)break;
        a->plot_channel=(int)selected;started=a->simulation_time;
        source=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));break;
    case 3:test_mouse(a,a->channel_unit_buttons[0],true);break;
    case 4:test_mouse(a,a->channel_unit_buttons[0],false);break;
    case 5:ok=a->channel_unit_manager;test_mouse(a,a->channel_unit_bounds[1],true);break;
    case 6:test_mouse(a,a->channel_unit_bounds[1],false);break;
    case 7:test_key(a,SDLK_A);break;
    case 8:layout_test_text(a,"cm");break;
    case 9:test_mouse(a,a->channel_unit_bounds[2],true);break;
    case 10:test_mouse(a,a->channel_unit_bounds[2],false);break;
    case 11:test_key(a,SDLK_A);break;
    case 12:layout_test_text(a,"0");break;
    case 13:test_mouse(a,a->channel_unit_bounds[3],true);break;
    case 14:test_mouse(a,a->channel_unit_bounds[3],false);break;
    case 15:ok=a->channel_unit_manager && !a->channel_units.count && *a->channel_units_error;test_mouse(a,a->channel_unit_bounds[2],true);break;
    case 16:test_mouse(a,a->channel_unit_bounds[2],false);break;
    case 17:test_key(a,SDLK_A);break;
    case 18:layout_test_text(a,"0.01");break;
    case 19:*capture="units-selection.bmp";test_window_key_mod(a->window,SDLK_F7,0);test_mouse(a,a->channel_unit_bounds[3],true);break;
    case 20:test_mouse(a,a->channel_unit_bounds[3],false);break;
    case 21: {
        ps_display_unit u=channel_display_unit(a,&a->live_channels[selected]);double converted;
        ok=!a->channel_unit_manager && a->channel_units.count==1 && u.scale==.01 && !strcmp(u.symbol,"cm") &&
            a->runner.running && !a->reset_pending && a->simulation_time>started &&
            source==ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
        double shown=a->values[selected];ok &= ps_display_unit_value(&u,shown,&converted)==PS_OK && converted==shown/.01;
        double gaps[]={1,NAN,2},displayed[3];ok &= channel_display_values(&u,gaps,3,displayed) && displayed[0]==100 && isnan(displayed[1]) && displayed[2]==200;
        *capture="units-live.bmp";command(a,PS_MSG_STOP);a->stop_at=ps_clock();break;
    }
    case 22:if(!idle(a) || a->library_thread)return;ok=a->data.total>2 && a->data.result==PS_EOF;a->tab=2;a->show_report=false;a->show_log=false;break;
    case 23: {
        ps_display_unit u=channel_display_unit(a,&a->data.channels[selected]);double mean;
        ok=u.scale==.01 && ps_display_unit_value(&u,a->data.stats[selected].mean,&mean)==PS_OK && mean==a->data.stats[selected].mean/.01;
        *capture="units-statistics.bmp";test_mouse(a,a->channel_unit_buttons[1],true);break;
    }
    case 24:test_mouse(a,a->channel_unit_buttons[1],false);break;
    case 25:ok=a->channel_unit_manager;ps_plot_view_zoom(&a->data_views[selected],.5,.5,.5);view=a->data_views[selected];test_mouse(a,a->channel_unit_bounds[4],true);break;
    case 26:test_mouse(a,a->channel_unit_bounds[4],false);break;
    case 27:
        ok=!a->channel_unit_manager && !a->channel_units.count && !memcmp(&view,&a->data_views[selected],sizeof view);
        channel_unit_open(a,&a->data.channels[selected]);strcpy(a->unit_draft.symbol,"cm");strcpy(a->channel_unit_scale,"1e-320");channel_unit_apply(a);
        ok &= a->channel_unit_manager && !a->channel_units.count && *a->channel_units_error;
        strcpy(a->channel_unit_scale,"0.01");channel_unit_apply(a);ok &= !a->channel_unit_manager && a->channel_units.count==1;
        channel_unit_open(a,&a->data.channels[selected]);strcpy(a->unit_draft.symbol,"cancelled");test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    case 28:ok=!a->channel_unit_manager && !strcmp(a->channel_units.entries[0].unit.symbol,"cm");
        {struct nk_window *window=nk_window_find(a->ui,dock_windows[PS_DOCK_WORKSPACE]);
         if(window)window->scrollbar.y=220;else ok=false;}break;
    case 29:*capture="units-statistics-detail.bmp";a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Channel units GUI stage %d failed: %s / %s\n",*stage,a->status,a->channel_units_error);*code=1;a->quitting=true;}
    if(a->quitting)printf("CHANNEL UNITS %s SELF-TEST: %s\n",mode,*code?"FAILED":"PASSED");
    (*stage)++;
}
