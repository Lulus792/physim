/* Actual saved runs, native controls and frozen graphic export; source samples
 * are independent, include gaps and exceed the visible reduction limit. */
static bool comparison_test_start(app *a,const char *root,const char *mode) {
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    join(a->project_input,sizeof a->project_input,root,"project");a->example=0;new_project(a);if(!a->loaded)return false;
    for(unsigned run=0;run<9;run++) {
        ps_context c={0};c.dt_s=.01+run*.001;
        if(run%2 && ps_channel_add(&c,"Energie",PS_JOULE,"")<0)return false;
        if(ps_channel_add(&c,"Position",run==8?PS_SECOND:PS_METRE,"")<0 || ps_channel_add(&c,"Position.status",PS_ONE,"")<0)return false;
        if(!(run%2) && ps_channel_add(&c,"Energie",PS_JOULE,"")<0)return false;
        char name[64];snprintf(name,sizeof name,"runs/Lauf %03u.psrun",run+1);join(path,sizeof path,a->project,name);
        ps_run_writer w;if(ps_run_create(&w,path,&c,"Native comparison fixture")!=PS_OK)return false;
        for(unsigned i=0;i<(run?301+run*5:6001);i++) {
            double t=.25*run+c.dt_s*i,v=.2*sin(t+run*.1)+run*.05,status=i==200?0:i==201?2:1;
            if(!run && i==125)v=2;if(!run && i==127)v=-2;
            double values[3];values[run%2?1:0]=v;values[run%2?2:1]=status;values[run%2?0:2]=t*t;
            if(ps_run_append(&w,t,values)!=PS_OK)return false;
        }
        if(ps_run_close(&w)!=PS_OK)return false;
    }
    join(a->last_run,sizeof a->last_run,a->project,"runs/Lauf 001.psrun");
    for(unsigned run=0;run<8;run++){char name[64];snprintf(name,sizeof name,"runs/Lauf %03u.psrun",run+1);join(path,sizeof path,a->project,name);select_run(a,path,true);}
    request_dataset(a);refresh_library(a);a->tab=2;a->analysis_tab=0;a->show_report=false;
    ps_preferences preferences=a->preferences;preferences.theme=strstr(mode,"light")?PS_THEME_LIGHT:strstr(mode,"contrast")?PS_THEME_HIGH_CONTRAST:PS_THEME_DARK;
    if(strstr(mode,"large"))preferences.ui_size=22;preferences_apply(a,&preferences);
    SDL_SetWindowSize(a->window,strstr(mode,"narrow")?1080:1440,strstr(mode,"narrow")?740:1100);return true;
}
static void comparison_test_frame(app *a,int *stage,int *code,const char **capture) {
    static unsigned settle,scale=1,return_wait,save_tabs;static bool filtered,captured_state;static ps_graphic_axes frozen_axes;static struct nk_rect drag;
    if(settle++<2)return;settle=0;
    ps_comparison_state state=ps_comparison_read(a->comparison);
    if(a->data.thread || a->library_thread || state.loading)return;
    ps_graphic_description d={0};if(state.snapshot)ps_graphic_snapshot_describe(state.snapshot,&d);
    bool ok=true;char path[4096];
    switch(*stage) {
    case 0: {
        if(SDL_GetKeyboardFocus()!=a->window){SDL_RaiseWindow(a->window);return;}
        if(a->plot_area.w<=0 || a->plot_area.h<=0)return; /* actual published draw, not only worker completion */
        const ps_curve_data *curve;ps_graphic_run_info run;
        ok=state.snapshot && d.input_count==8 && d.plot.curves==8 && d.reduced && d.reference==0;
        ok=ok && ps_graphic_snapshot_run(state.snapshot,0,&run)==PS_OK && run.samples==6001 && run.si_statistics.count==5999 && run.si_statistics.min==-2 && run.si_statistics.max==2;
        ok=ok && ps_report_curve_view(ps_graphic_snapshot_report(state.snapshot),0,1,&curve)==PS_OK && fabs(curve->x[1]-.261)<1e-12;
        printf("COMPARISON NATIVE: inputs=%u curves=%u reference=%u full=%llu valid=%llu independent-x1=%.17g\n",d.input_count,d.plot.curves,d.reference,(unsigned long long)run.samples,(unsigned long long)run.si_statistics.count,ok?curve->x[1]:0);
        printf("COMPARISON AREA: %.1f,%.1f,%.1f,%.1f\n",a->plot_area.x,a->plot_area.y,a->plot_area.w,a->plot_area.h);
        for(unsigned i=0;i<8;i++)printf("COMPARISON COLOR: %u %u %u %u\n",i,ui_palette->curves[i].r,ui_palette->curves[i].g,ui_palette->curves[i].b);
        printf("COMPARISON BACKGROUND: %u %u %u\n",UI_COLOR(edit).r,UI_COLOR(edit).g,UI_COLOR(edit).b);
        *capture="comparison-eight.png";break;
    }
    case 1:
        /* Library order follows real filesystem timestamps. Prepare the existing
         * search state rather than requesting an offscreen native action. */
        if(!filtered){snprintf(a->analysis_query,sizeof a->analysis_query,"002.psrun");filtered=true;return;}
        ok=a->selected_count==8 && nk_sdl_accessibility_request_focus(a->ui,NULL,"Als Referenz verwenden: Lauf 002.psrun");break;
    case 2:
        if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"Als Referenz verwenden: Lauf 002.psrun"))return;
        test_window_key_mod(a->window,SDLK_SPACE,0);break;
    case 3: {
        ps_graphic_run_info run;ok=state.snapshot && d.plot.curves==8 && ps_graphic_snapshot_run(state.snapshot,0,&run)==PS_OK && strstr(run.path,"002.psrun");
        *capture="comparison-reference.png";test_mouse(a,a->plot_buttons[0],true);break;
    }
    case 4:test_mouse(a,a->plot_buttons[0],false);break;
    case 5:
        ok=ps_plot_view_changed(&a->data_views[a->plot_channel]);drag=nk_rect(a->plot_area.x+a->plot_area.w*.5f,a->plot_area.y+a->plot_area.h*.5f,0,0);test_mouse(a,drag,true);break;
    case 6:drag.x+=30;drag.y+=20;test_mouse(a,drag,true);break;
    case 7:test_mouse(a,drag,false);break;
    case 8:ok=nk_sdl_accessibility_request_focus(a->ui,NULL,"Grafik exportieren…");break;
    case 9:
        if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"Grafik exportieren…"))return;
        test_window_key_mod(a->window,SDLK_SPACE,0);break;
    case 10: {
        graphic_dialog *dialog=a->graphic_export;ok=dialog && dialog->description.plot.curves==8 && dialog->description.axes.cropped && dialog->description.reference==0;
        if(!ok)break;frozen_axes=dialog->description.axes;
        ps_display_unit cm={.scale=.01,.symbol="cm",.dimension={1}};
        ok=ps_channel_units_put(&a->channel_units,&a->data.channels[a->plot_channel],&cm)==PS_OK;
        while(a->selected_count>1){char remove[4096];snprintf(remove,sizeof remove,"%s",a->selected_runs[a->selected_count-1]);select_run(a,remove,false);}
        *capture="comparison-export.png";break;
    }
    case 11: {
        graphic_dialog *dialog=a->graphic_export;
        ok=dialog && dialog->description.plot.curves==8 && !strcmp(dialog->description.plot.y_unit.symbol,"m") && !memcmp(&dialog->description.axes,&frozen_axes,sizeof frozen_axes);
        ok=ok && state.snapshot && d.plot.curves==1 && !strcmp(d.plot.y_unit.symbol,"cm");
        join(path,sizeof path,a->project,"runs/Lauf 001.psrun");ok=ok && !remove(path);
        ok=ok && nk_sdl_accessibility_request_focus(a->ui,"Dateiformat","SVG");break;
    }
    case 12:
        if(!nk_sdl_accessibility_is_focused(a->ui,"Dateiformat","SVG"))return;
        test_window_key_mod(a->window,SDLK_SPACE,0);break;
    case 13:
        ok=a->graphic_export && a->graphic_export->svg;
        join(a->graphic_export_test_path,sizeof a->graphic_export_test_path,a->project,"comparison-eight.svg");
        if(ok && !nk_sdl_accessibility_is_focused(a->ui,NULL,"SVG speichern…")) {
            if(a->graphic_export->focus_target)return;
            if(save_tabs++<6){test_window_key_mod(a->window,SDLK_TAB,0);return;}
            ok=false;
        }
        break;
    case 14:
        if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"SVG speichern…"))return;
        test_window_key_mod(a->window,SDLK_SPACE,0);break;
    case 15:
        if(a->graphic_export->save)return;
        ok=strstr(a->graphic_export->message,"gespeichert")!=NULL;
        join(path,sizeof path,a->project,"comparison-full.png");
        ok=ok && ps_graphics_image_png(ps_graphic_raster_image(a->graphic_export->raster),path)==PS_OK;
        a->graphic_export->svg=false;a->graphic_export->visible=true;a->graphic_export->scale=scale;a->graphic_export->dirty=true;break;
    case 16: {
        graphic_dialog *dialog=a->graphic_export;ps_graphic_raster_info image;
        ok=dialog && !dialog->dirty && ps_graphic_raster_describe(dialog->raster,&image)==PS_OK && image.width==1200*scale && image.height==850*scale;
        char name[64];snprintf(name,sizeof name,"comparison-scale%u.png",scale);join(path,sizeof path,a->project,name);
        if(ok)ok=ps_graphics_image_png(ps_graphic_raster_image(dialog->raster),path)==PS_OK;
        if(ok && ++scale<=4){dialog->scale=scale;dialog->dirty=true;return;}
        if(ok) {
            ps_graphic_svg *svg=NULL;ok=ps_graphic_figure_svg(ps_graphic_raster_figure(dialog->raster),0,ps_allocator_default(),&svg)==PS_OK;
            if(ok){size_t size;const char *text=ps_graphic_svg_bytes(svg,&size);join(path,sizeof path,a->project,"comparison-cropped.svg");FILE *f=fopen(path,"wb");ok=f && fwrite(text,1,size,f)==size;if(f && fclose(f))ok=false;}
            ps_graphic_svg_destroy(svg);
        }
        test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    }
    case 17:
        if(a->graphic_export || a->graphic_return_ui)return;
        ok=nk_sdl_accessibility_is_focused(a->ui,NULL,"Grafik exportieren…") && state.snapshot && d.plot.curves==1 && !strcmp(d.plot.y_unit.symbol,"cm");
        if(!ok) {
            struct nk_window *w=nk_window_find(a->ui,"workspace");
            fprintf(stderr,"COMPARISON RETURN OBSERVE: focused=%d snapshot=%d unit=%s channel=%d scroll=%u flags=%u\n",nk_sdl_accessibility_is_focused(a->ui,NULL,"Grafik exportieren…"),state.snapshot!=NULL,d.plot.y_unit.symbol,a->plot_channel,w?w->scrollbar.y:0,w?w->flags:0);
            if(return_wait++<4)return;
            break;
        }
        if(!captured_state){*capture="comparison-current.png";captured_state=true;return;}captured_state=false;
        join(path,sizeof path,a->project,"runs/Lauf 009.psrun");select_run(a,path,true);break;
    case 18: {
        ps_graphic_run_info run;ok=state.snapshot && d.input_count==2 && d.plot.curves==1 && ps_graphic_snapshot_run(state.snapshot,1,&run)==PS_OK && run.result==PS_INVALID;
        if(!captured_state){*capture="comparison-partial.png";captured_state=true;return;}captured_state=false;
        while(a->selected_count){char remove[4096];snprintf(remove,sizeof remove,"%s",a->selected_runs[a->selected_count-1]);select_run(a,remove,false);}break;
    }
    case 19:
        ok=!state.snapshot && !state.loading && !nk_sdl_accessibility_request_focus(a->ui,NULL,"Grafik exportieren…");
        if(!captured_state){*capture="comparison-empty.png";captured_state=true;return;}captured_state=false;
        join(path,sizeof path,a->project,"runs/Lauf 002.psrun");select_run(a,path,true);a->plot_channel=0;break;
    case 20:
        ok=state.snapshot && !strcmp(d.plot.y_label,"Energie") && d.plot.curves==1;
        if(!captured_state){*capture="comparison-channel.png";captured_state=true;return;}captured_state=false;
        {char remove[4096];snprintf(remove,sizeof remove,"%s",a->selected_runs[0]);select_run(a,remove,false);}
        join(path,sizeof path,a->project,"runs/Lauf 009.psrun");select_run(a,path,true);
        a->plot_channel=1;break;
    case 21:
        if(!state.snapshot && state.result==PS_INVALID && a->comparison_retry_bounds.w<=0)return;
        ok=!state.snapshot && state.result==PS_INVALID && a->comparison_retry_bounds.w>0;
        *capture="comparison-error.png";a->quitting=true;break;
    }
    if(!ok){*capture="comparison-failed.png";fprintf(stderr,"COMPARISON UI FAILED: stage=%d result=%d inputs=%u curves=%u focus=%u\n",*stage,state.result,d.input_count,d.plot.curves,SDL_GetWindowID(SDL_GetKeyboardFocus()));*code=1;a->quitting=true;}
    if(a->quitting)printf("COMPARISON UI SELF-TEST: %s\n",*code?"FAILED":"PASSED");
    (*stage)++;
}
