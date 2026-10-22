/* Exercise the compact native chooser, its real persistence and recovery path. */
static void analysis_units_test_frame(app *a,int *stage,int *code,const char **capture) {
    static bool settle;static ps_statistics statistics;static ps_plot_view view;
    static uint32_t values_crc,time_crc,file_crc;static char units_path[4096];
    if(settle){settle=false;return;}settle=true;
    if(a->data.thread || a->library_thread || ps_comparison_read(a->comparison).loading)return;
    bool ok=true;const ps_channel *channel=&a->data.channels[0];
    switch(*stage) {
    case 0: {
        if(SDL_GetKeyboardFocus()!=a->window){SDL_RaiseWindow(a->window);return;}
        statistics=a->data.stats[0];
        values_crc=ps_crc32((const unsigned char*)a->data.values[0],a->data.count*sizeof(double));
        time_crc=ps_crc32((const unsigned char*)a->data.time,a->data.count*sizeof(double));
        ok=reset_test_crc(a->data.path,&file_crc);
        snprintf(units_path,sizeof units_path,"%s",a->channel_units_path);
        char label[40];plot_axis_label(label,sizeof label,1.9e-7,-.348,.348);ok=ok && !strcmp(label,"0");
        plot_axis_label(label,sizeof label,1e-12,0,2e-12);ok=ok && !strcmp(label,"1e-12");
        a->tab=2;a->analysis_tab=0;break;
    }
    case 1:view=a->data_views[0];ok=nk_sdl_accessibility_request_focus(a->ui,NULL,"Anzeigeeinheit auswählen");break;
    case 2:if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"Anzeigeeinheit auswählen"))return;test_window_key_mod(a->window,SDLK_SPACE,0);break;
    case 3: {
        bool focused=nk_sdl_accessibility_request_focus(a->ui,NULL,"Grad (°)");
        ok=a->analysis_unit_custom.w>0 && focused;
        printf("ANALYSIS UNIT MENU: custom-item-width=%.1f degree-focus-request=%d\n",a->analysis_unit_custom.w,focused);
        *capture="analysis-unit-menu.bmp";break;
    }
    case 4:if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"Grad (°)"))return;test_window_key_mod(a->window,SDLK_SPACE,0);break;
    case 5: {
        ps_display_unit unit=channel_display_unit(a,channel);ps_channel_units saved;double shown;
        ok=!a->channel_unit_manager && unit.scale==channel_unit_presets[3].scale && !strcmp(unit.symbol,"°") &&
            ps_channel_units_read(units_path,&saved)==PS_OK && saved.count==1;
        ok=ok && ps_display_unit_value(&unit,a->data.values[0][0],&shown)==PS_OK && fabs(shown-.3*180/3.14159265358979323846)<1e-12;
        if(!ok)break;
        /* Closing a popup, publishing the enabled parent and granting its
         * queued focus are separate frames. Wait for the actual return. */
        if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"Anzeigeeinheit auswählen"))return;
        *capture="analysis-unit-degrees.bmp";test_mouse(a,a->channel_unit_buttons[1],true);break;
    }
    case 6:test_mouse(a,a->channel_unit_buttons[1],false);break;
    case 7:ok=nk_sdl_accessibility_request_focus(a->ui,NULL,"Benutzerdefiniert …");break;
    case 8:if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"Benutzerdefiniert …"))return;test_window_key_mod(a->window,SDLK_SPACE,0);break;
    case 9:ok=a->channel_unit_manager;test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    case 10:
        ok=!a->channel_unit_manager && channel_display_unit(a,channel).scale==channel_unit_presets[3].scale;
        /* A directory cannot be replaced by the atomic unit-file writer. */
        snprintf(a->channel_units_path,sizeof a->channel_units_path,"%s",a->project);
        test_mouse(a,a->channel_unit_buttons[1],true);break;
    case 11:test_mouse(a,a->channel_unit_buttons[1],false);break;
    case 12:ok=nk_sdl_accessibility_request_focus(a->ui,NULL,"Radiant (rad)");break;
    case 13:if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"Radiant (rad)"))return;test_window_key_mod(a->window,SDLK_SPACE,0);break;
    case 14: {
        ps_channel_units saved;ok=a->channel_units_error[0] && channel_display_unit(a,channel).scale==channel_unit_presets[3].scale &&
            ps_channel_units_read(units_path,&saved)==PS_OK && saved.count==1 && saved.entries[0].unit.scale==channel_unit_presets[3].scale;
        *capture="analysis-unit-save-error.bmp";snprintf(a->channel_units_path,sizeof a->channel_units_path,"%s",units_path);
        test_mouse(a,a->channel_unit_buttons[1],true);break;
    }
    case 15:test_mouse(a,a->channel_unit_buttons[1],false);break;
    case 16:ok=nk_sdl_accessibility_request_focus(a->ui,NULL,"Radiant (rad)");break;
    case 17:if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"Radiant (rad)"))return;test_window_key_mod(a->window,SDLK_SPACE,0);break;
    case 18: {
        ps_channel_units saved;ok=!a->channel_units_error[0] && !a->channel_units.count && channel_display_unit(a,channel).scale==1 &&
            ps_channel_units_read(units_path,&saved)==PS_OK && saved.count==0;
        *capture="analysis-unit-restored.bmp";a->channel_units_writable=false;
        snprintf(a->channel_units_error,sizeof a->channel_units_error,"Test: geschützte Einheitendatei. Benutzerdefiniert bleibt erreichbar.");
        test_mouse(a,a->channel_unit_buttons[1],true);break;
    }
    case 19:test_mouse(a,a->channel_unit_buttons[1],false);break;
    case 20:ok=a->analysis_unit_custom.w>0 && nk_sdl_accessibility_request_focus(a->ui,NULL,"Benutzerdefiniert …");*capture="analysis-unit-readonly.bmp";break;
    case 21:if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"Benutzerdefiniert …"))return;test_window_key_mod(a->window,SDLK_SPACE,0);break;
    case 22:ok=a->channel_unit_manager && !a->channel_units_writable;test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    case 23: {
        uint32_t after=0;a->channel_units_writable=true;a->channel_units_error[0]=0;
        ok=!a->channel_unit_manager && a->data.total==301 && a->selected_count==1 &&
            !memcmp(&statistics,&a->data.stats[0],sizeof statistics) && !memcmp(&view,&a->data_views[0],sizeof view) &&
            values_crc==ps_crc32((const unsigned char*)a->data.values[0],a->data.count*sizeof(double)) && time_crc==ps_crc32((const unsigned char*)a->data.time,a->data.count*sizeof(double)) &&
            reset_test_crc(a->data.path,&after) && after==file_crc;
        printf("ANALYSIS UNIT INVARIANTS: raw file, times, values, statistics, view and selection unchanged=%d\n",ok);a->quitting=true;break;
    }
    }
    if(!ok){*capture="analysis-unit-failed.bmp";fprintf(stderr,"Analysis unit stage %d failed: %s\n",*stage,a->channel_units_error);*code=1;a->quitting=true;}
    if(a->quitting)printf("ANALYSIS UNIT CHOOSER SELF-TEST: %s\n",*code?"FAILED":"PASSED");
    (*stage)++;
}
