/* Real paused samples, unit conversion, native switch and channel-menu input. */
static struct {unsigned settle;char paint[2][3][240];ps_channel_units units;int channels[2];} measurement_design;
static bool measurement_design_start(app *a,const char *root) {
    memset(&measurement_design,0,sizeof measurement_design);
    const double input[]={0,-0.,1e-8,1e9,-.03};const char *expected[]={"0,00","0,00","1,00e-08","1,00e+09","-0,03"};
    for(unsigned i=0;i<5;i++){char number[96];laboratory_metric_number(input[i],number);if(strcmp(number,expected[i]))return false;}
    puts("MEASUREMENT FORMATS: zero=1 negative-zero=1 tiny=1 large=1 negative=1");
    return inspector_design_start(a,root);
}
static void measurement_design_draw(app *a) {
    memset(measurement_design.paint,0,sizeof measurement_design.paint);const struct nk_command *cmd;
    nk_foreach(cmd,a->ui)if(cmd->type==NK_COMMAND_TEXT) {
        const struct nk_command_text *t=(const struct nk_command_text*)cmd;
        for(unsigned i=0;i<2;i++) {
            struct nk_rect r[]={a->laboratory_metric_labels[i],a->laboratory_metric_numbers[i],a->laboratory_metric_units[i]};
            for(unsigned j=0;j<3;j++)if(r[j].w>0 && camera_design_contains(r[j],t))camera_design_append(measurement_design.paint[i][j],240,t);
        }
    }
}
static void measurement_design_record(app *a,const char *kind,const char **capture) {
    static char path[100];snprintf(path,sizeof path,"measurement-%s-%s.png",inspector_design_variant(),kind);*capture=path;
    printf("MEASUREMENT kind=%s variant=%s size=%u preserved=%d curves=%d scroll=%u",kind,inspector_design_variant(),a->preferences.ui_size,inspector_design_same(a),a->laboratory_curves,a->laboratory_measure_scroll_y);
    camera_design_rect("title",a->laboratory_measure_title);camera_design_rect("time",a->laboratory_measure_time);camera_design_rect("switch",a->laboratory_switch);puts("");
    for(unsigned i=0;i<2;i++) {
        ps_display_unit u=channel_display_unit(a,&a->live_channels[a->laboratory_channels[i]]);
        printf("METRIC slot=%u channel=%d raw=%.17g scale=%.17g number=",i,a->laboratory_channels[i],display_values(a)[a->laboratory_channels[i]],u.scale);camera_design_hex(a->laboratory_metric_number_text[i]);printf(" unit=");camera_design_hex(a->laboratory_metric_unit_text[i]);
        camera_design_rect("card",a->laboratory_metric_cards[i]);camera_design_rect("label",a->laboratory_metric_labels[i]);camera_design_rect("value",a->laboratory_metric_numbers[i]);camera_design_rect("unit-rect",a->laboratory_metric_units[i]);camera_design_rect("rule",a->laboratory_metric_lines[i]);
        printf(" value-font=%.4f unit-font=%.4f",a->laboratory_measurement_font.height,a->laboratory_caption_font.height);
        for(unsigned j=0;j<3;j++){printf(" paint%u=",j);camera_design_hex(measurement_design.paint[i][j]);}puts("");
    }
}
static void measurement_design_step(app *a,int *stage,int *code,const char **capture) {
    if(*stage<4){inspector_design_frame(a,stage,code,capture);return;}
    bool ok=true;
    switch(*stage) {
    case 4:
        inspector_design_frame(a,stage,code,capture);ps_dock_hide(&a->preferences.dock,PS_DOCK_SIDEBAR);
        a->laboratory_curves=false;a->laboratory_closed_height=240;a->laboratory_measure_height=420;measurement_design.units=a->channel_units;memcpy(measurement_design.channels,a->laboratory_channels,sizeof measurement_design.channels);measurement_design.settle=0;return;
    case 5:
        if(measurement_design.settle++<3)return;
        if(!inspector_design_focus(a,"Kurven anzeigen"))return;
        measurement_design_record(a,"normal",capture);
        test_window_key_mod(a->window,SDLK_RETURN,0);measurement_design.settle=0;break;
    case 6:
        if(measurement_design.settle++<3)return;
        ok=a->laboratory_curves && nk_sdl_accessibility_is_focused(a->ui,NULL,"Kurven anzeigen");
        if(!ok)break;
        if(inspector_design.variant>=2)test_window_key_mod(a->window,SDLK_PAGEDOWN,0);
        measurement_design.settle=0;*stage=13;return;
    case 13:
        if(measurement_design.settle++<3)return;
        if(!inspector_design_focus(a,"Messkurve 1: Kanal"))return;
        measurement_design_record(a,"curves",capture);
        test_window_key_mod(a->window,SDLK_RETURN,0);measurement_design.settle=0;*stage=7;return;
    case 7:
        if(measurement_design.settle++<3)return;
        {ps_display_unit u=channel_display_unit(a,&a->live_channels[1]);char caption[160];snprintf(caption,sizeof caption,"%s [%s]",a->live_channels[1].name,u.symbol);
        if(!inspector_design_focus(a,caption))return;
        test_window_key_mod(a->window,SDLK_RETURN,0);}measurement_design.settle=0;break;
    case 8:
        if(measurement_design.settle++<3)return;
        ok=a->laboratory_channels[0]==1 && a->laboratory_channels[1]==measurement_design.channels[1];
        measurement_design_record(a,"channel",capture);
        if(ok){*stage=14;return;}
        break;
    case 14:
        {ps_display_unit u=channel_display_unit(a,&a->live_channels[1]);u.scale=.001;snprintf(u.symbol,sizeof u.symbol,"mrad/s");ok=ok && ps_channel_units_put(&a->channel_units,&a->live_channels[1],&u)==PS_OK;}
        measurement_design.settle=0;if(ok){*stage=9;return;}break;
    case 9:
        if(measurement_design.settle++<3)return;
        if(!inspector_design_focus(a,"Kurven anzeigen"))return;
        if(a->laboratory_measure_scroll_y){test_window_key_mod(a->window,SDLK_PAGEUP,0);measurement_design.settle=0;return;}
        ok=!strcmp(a->laboratory_metric_unit_text[0],"mrad/s");measurement_design_record(a,"converted",capture);
        if(ok){*stage=15;return;}
        break;
    case 15:
        a->channel_units=measurement_design.units;memcpy(a->laboratory_channels,measurement_design.channels,sizeof measurement_design.channels);
        test_window_key_mod(a->window,SDLK_RETURN,0);measurement_design.settle=0;*stage=10;return;
    case 10:
        if(measurement_design.settle++<3)return;
        ok=!a->laboratory_curves && inspector_design_same(a);
        measurement_design_record(a,"restored",capture);
        if(++inspector_design.variant<6){*stage=4;return;}break;
    case 11:{uint32_t crc;ok=timeline_design_run_crc(a->last_run,&crc) && crc==inspector_design.run_crc;command(a,PS_MSG_STOP);a->stop_at=ps_clock();break;}
    case 12:if(!idle(a) || a->library_thread)return;puts("MEASUREMENT DESIGN SELF-TEST: PASSED");a->quitting=true;break;
    }
    if(!inspector_design_same(a))ok=false;
    if(!ok){fprintf(stderr,"MEASUREMENT DESIGN FAILED stage=%d variant=%u\n",*stage,inspector_design.variant);*code=1;a->quitting=true;}else (*stage)++;
}
