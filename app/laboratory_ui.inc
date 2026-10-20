/* Messlabor: all numbers, scenes and commands come from the native experiment. */
static void laboratory_note(app *a,const char *text) {
    float width=fmaxf(40,nk_window_get_content_region(a->ui).w-16);
    nk_layout_row_dynamic(a->ui,fmaxf(26,documentation_height(a->ui->style.font,text,strlen(text),width)+8),1);
    nk_label_wrap(a->ui,text);
}
static void laboratory_value(app *a,const char *label,const char *value) {
    struct nk_context *ui=a->ui;struct nk_rect r;nk_widget(&r,ui);
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    nk_fill_rect(canvas,r,7,UI_COLOR(edit));
    if(a->preferences.theme==PS_THEME_HIGH_CONTRAST)nk_stroke_rect(canvas,r,7,1,UI_COLOR(border));
    const struct nk_user_font *small=&a->workbench_fonts[0],*number=&a->laboratory_value_font;
    nk_draw_text(canvas,nk_rect(r.x+10,r.y+8,r.w-20,small->height),label,(int)strlen(label),small,nk_rgba(0,0,0,0),muted());
    float width=number->width(number->userdata,number->height,value,(int)strlen(value));
    const struct nk_user_font *font=number;
    if(width>r.w-20 && a->laboratory_card_font_count<8) {
        struct nk_user_font *fit=&a->laboratory_card_fonts[a->laboratory_card_font_count++];*fit=*number;
        fit->height=fmaxf(ui->style.font->height,number->height*(r.w-20)/fmaxf(1,width));font=fit;
    }
    nk_draw_text(canvas,nk_rect(r.x+10,r.y+28,r.w-20,font->height+4),value,(int)strlen(value),font,nk_rgba(0,0,0,0),UI_COLOR(text));
    char accessible[320];snprintf(accessible,sizeof accessible,"%s: %s",label,value);ps_ui_accessibility_label(ui,r,accessible);
    if(nk_input_is_mouse_hovering_rect(&ui->input,r))nk_tooltip(ui,accessible);
}
static void laboratory_object(app *a) {
    struct nk_context *ui=a->ui;scene_selection_sync(a);
    if(!nk_tree_state_push(ui,NK_TREE_TAB,"Objekt",&a->laboratory_groups[0]))return;
    const ps_scene *scene=display_scene(a);
    if(!a->scene_selected || a->scene_selected_slot>=scene->count)laboratory_note(a,"Wähle ein Objekt in der Szene oder im Szenenbaum.");
    else {
        const ps_object *o=&scene->objects[a->scene_selected_slot];char value[128];
        static const char *shapes[]={"Kugel","Linie","Box","Vektor","Punkt","Ebene","Pfad","Beschriftung","Gruppe","Koordinatenrahmen"};
        if(*o->text)laboratory_note(a,o->text);
        snprintf(value,sizeof value,"%s · ID %u",o->shape<10?shapes[o->shape]:"Objekt",o->id);laboratory_note(a,value);
        if(o->shape==PS_SPHERE){nk_layout_row_dynamic(ui,70,1);snprintf(value,sizeof value,"%.4g m",o->radius);laboratory_value(a,"Radius / m",value);}
        if(o->shape!=PS_GROUP) {
            ps_vec3 world,local=o->shape==PS_FRAME?ps_v3(0,0,0):o->a;
            if(ps_scene_world_point(scene,a->scene_selected_slot,local,&world)==PS_OK) {
                const double v[]={world.x,world.y,world.z};const char *names[]={"Position x / m","Position y / m","Position z / m"};
                int columns=nk_window_get_content_region(ui).w>=260*a->preferences.ui_size/16.f?2:1;
                nk_layout_row_dynamic(ui,76*a->preferences.ui_size/16.f,columns);
                for(unsigned i=0;i<3;i++){snprintf(value,sizeof value,"%.4g m",v[i]);laboratory_value(a,names[i],value);}
                if(columns==2)nk_spacer(ui);
                snprintf(value,sizeof value,"Lokal / m: %.6g / %.6g / %.6g",o->a.x,o->a.y,o->a.z);laboratory_note(a,value);
                if(o->shape==PS_FRAME){snprintf(value,sizeof value,"Skalierung: %.6g / %.6g / %.6g",o->b.x,o->b.y,o->b.z);laboratory_note(a,value);}
            }
        }
        nk_layout_row_dynamic(ui,34,1);a->scene_hide_selection_bounds=nk_widget_bounds(ui);
        if(nk_button_label(ui,"Auswahl ausblenden")){a->scene_view.hidden|=UINT32_C(1)<<a->scene_selected_slot;a->scene_selected=false;}
    }
    nk_layout_row_dynamic(ui,30,1);a->display_all_bounds=nk_widget_bounds(ui);
    if(nk_button_label(ui,"Alle Objekte einblenden"))a->scene_view.hidden=0;
    nk_tree_state_pop(ui);
}
static const ps_channel *laboratory_channels(app *a,uint32_t *count) {
    if(a->analysis_only){*count=a->data.channel_count;return a->data.channels;}
    *count=a->channel_count;return a->live_channels;
}
static int laboratory_channel_picker(app *a,unsigned slot) {
    uint32_t count;const ps_channel *channels=laboratory_channels(a,&count);
    int *chosen=&a->laboratory_channels[slot];
    if(!count){*chosen=0;return -1;}
    if(*chosen<0 || (unsigned)*chosen>=count)*chosen=slot<count?(int)slot:0;
    const char *names[PS_MAX_CHANNELS];char labels[PS_MAX_CHANNELS][160];
    for(unsigned i=0;i<count;i++){ps_display_unit u=channel_display_unit(a,&channels[i]);snprintf(labels[i],sizeof labels[i],"%s [%s]",channels[i].name,u.symbol);names[i]=labels[i];}
    nk_layout_row_dynamic(a->ui,30,1);
    *chosen=nk_combo(a->ui,names,(int)count,*chosen,28,nk_vec2(300,240));return *chosen;
}
static void laboratory_measurements(app *a,float height) {
    struct nk_context *ui=a->ui;uint32_t count;const ps_channel *channels=laboratory_channels(a,&count);
    float scale=a->preferences.ui_size/16.f;
    nk_layout_row_dynamic(ui,36,2);char heading[96];snprintf(heading,sizeof heading,"Messwerte · %.6g s%s",display_time(a),a->timeline_browsing?" · Rückblick":"");
    a->live_measurement_bounds=nk_widget_bounds(ui);nk_label(ui,heading,NK_TEXT_LEFT);
    if(!a->panel_render)panel_pull_handle(a,PS_PANEL_MEASURE,a->live_measurement_bounds);
    struct nk_rect cell;nk_widget(&cell,ui);float sw=40,sh=22;
    /* Keep the target clear of the adjacent dock separator's pointer margin. */
    a->laboratory_switch=nk_rect(cell.x+fmaxf(0,cell.w-170*scale-12),cell.y, fminf(cell.w-12,170*scale),cell.h);
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);struct nk_rect r=a->laboratory_switch;
    nk_draw_text(canvas,nk_rect(r.x+4,r.y+(r.h-ui->style.font->height)/2,r.w-sw-12,ui->style.font->height),"Kurven anzeigen",15,ui->style.font,nk_rgba(0,0,0,0),UI_COLOR(text));
    struct nk_rect track=nk_rect(r.x+r.w-sw-4,r.y+(r.h-sh)/2,sw,sh);
    nk_fill_rect(canvas,track,sh/2,a->laboratory_curves?UI_COLOR(primary):UI_COLOR(surface));nk_stroke_rect(canvas,track,sh/2,1,UI_COLOR(muted));
    nk_fill_circle(canvas,nk_rect(track.x+(a->laboratory_curves?sw-sh+3:3),track.y+3,sh-6,sh-6),a->laboratory_curves?UI_COLOR(primary_text):UI_COLOR(muted));
    ps_ui_switch_at(ui,r,"Kurven anzeigen",&a->laboratory_curves);
    float cards=fmaxf(74,a->laboratory_value_font.height+40);
    nk_layout_row_dynamic(ui,cards,count>1?2:1);
    if(!count)laboratory_value(a,"Messkanäle","Noch keine Messwerte");
    else for(unsigned slot=0;slot<(count>1?2u:1u);slot++) {
        int selected=a->laboratory_channels[slot];if(selected<0 || (unsigned)selected>=count)selected=slot<count?(int)slot:0;
        const ps_channel *ch=&channels[selected];ps_display_unit u=channel_display_unit(a,ch);char value[96];
        channel_display_text(&u,display_values(a)[selected],value,sizeof value);
        int mask=a->channel_status[selected];
        if(mask>=0 && display_values(a)[mask]!=1)snprintf(value,sizeof value,"%s",display_values(a)[mask]==2?"Messung ausgefallen":"Keine neue Messung");
        laboratory_value(a,ch->name,value);
    }
    if(!a->laboratory_curves)return;
    float width=nk_window_get_content_region(ui).w;
    bool pair=count>1 && width>=430;
    float ratio=laboratory_dimension_value(a,2);
    float plots=fmaxf(80,height-36-cards);
    if(pair)nk_layout_row_begin(ui,NK_DYNAMIC,plots,3);
    else nk_layout_row_dynamic(ui,plots/(count>1?2:1),1);
    nk_style_push_vec2(ui,&ui->style.window.group_padding,nk_vec2(8,2));
    for(unsigned slot=0;slot<(count>1?2u:1u);slot++) {
        if(pair) {
            if(slot){nk_layout_row_push(ui,9/width);nk_widget(&a->laboratory_splitters[2],ui);}
            nk_layout_row_push(ui,(width-9)/width*(slot?1-ratio:ratio));
        }
        char group[40];snprintf(group,sizeof group,"laboratory-curve-%u",slot);a->laboratory_plots[slot]=nk_widget_bounds(ui);
        if(nk_group_begin(ui,group,NK_WINDOW_NO_SCROLLBAR)) {
            nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(8,2));
            int selected=laboratory_channel_picker(a,slot);
            if(selected<0)laboratory_note(a,"Messkurven erscheinen nach dem ersten Messpunkt.");
            else {
                double values[PREVIEW];ps_display_unit unit=channel_display_unit(a,&channels[selected]);
                bool represented=channel_display_values(&unit,a->history_v[selected],a->history_count,values);
                int mask=a->channel_status[selected];
                if(mask>=0)for(int i=0;i<a->history_count;i++)if(a->history_v[mask][i]!=1)values[i]=NAN;
                char title[240];snprintf(title,sizeof title,"%s [%s]%s",channels[selected].name,unit.symbol,represented?"":" · nicht darstellbar");
                const ps_ui_palette *previous=ui_palette;ps_ui_palette curve=*ui_palette;curve.accent=curve.curves[slot];ui_palette=&curve;
                nk_style_push_font(ui,&a->workbench_fonts[0]);
                plot(a,a->history_t,values,a->history_count,fmaxf(40,nk_window_get_content_region(ui).h-62),title,false,NULL);
                nk_style_pop_font(ui);
                ui_palette=previous;
            }
            nk_group_end(ui);
            nk_style_pop_vec2(ui);
        }
    }
    nk_style_pop_vec2(ui);
    if(pair)nk_layout_row_end(ui);
}
static void laboratory_workspace(app *a) {
    struct nk_context *ui=a->ui;
    float available=nk_window_get_content_region(ui).h-2*ui->style.window.padding.y;
    if(a->panel_render==PS_DOCK_WORKSPACE+1){viewport(a,fmaxf(64,available));return;}
    float measure=fminf(laboratory_dimension_value(a,0),available-73);
    float scene=fmaxf(64,available-measure-9);
    if(panel_is_detached(a,PS_DOCK_WORKSPACE)) {
        nk_layout_row_dynamic(ui,scene,1);struct nk_rect slot=nk_widget_bounds(ui);
        nk_spacer(ui);
        a->scene_viewport_bounds=slot;
    } else {
        viewport(a,scene);
    }
    a->laboratory_splitters[0]=nk_rect(a->scene_viewport_bounds.x,a->scene_viewport_bounds.y+a->scene_viewport_bounds.h,a->scene_viewport_bounds.w,9);
    nk_layout_row_dynamic(ui,9,1);nk_spacer(ui);
    if(panel_is_detached(a,PS_PANEL_MEASURE)) {
        nk_layout_row_dynamic(ui,measure,1);nk_spacer(ui);
    } else laboratory_measurements(a,measure);
}
static bool laboratory_begin(app *a,const char *name,struct nk_rect bounds,nk_flags flags) {
    struct nk_window *previous=a->ui->active;
    bool open=nk_begin(a->ui,name,bounds,flags);
    if(open)dock_input(a,PS_DOCK_WORKSPACE,bounds,previous,true);
    return open;
}
static void laboratory_chrome(app *a,float w,float h) {
    struct nk_context *ui=a->ui;bool visible=laboratory_visible(a);
    const char *windows[]={"laboratory-transport","laboratory-sections","laboratory-timeline","laboratory-status"};
    for(unsigned i=0;i<4;i++)nk_window_show(ui,windows[i],visible?NK_SHOWN:NK_HIDDEN);
    if(!visible)return;
    nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(16,10));
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(6,4));
    nk_style_push_float(ui,&ui->style.window.rounding,0);
    if(laboratory_begin(a,windows[0],nk_rect(0,72,w,80),NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BACKGROUND)) {
        float factor=a->preferences.ui_size/16.f;
        nk_layout_row_begin(ui,NK_STATIC,36,6);
        float project_width=fminf(190*factor,w*.18f);
        nk_layout_row_push(ui,project_width);nk_label(ui,a->loaded?project_name(a):"Simulation",NK_TEXT_LEFT);
        nk_layout_row_push(ui,110*factor);a->run_control_bounds[0]=a->run_control_bounds[1]=nk_widget_bounds(ui);
        bool ready=runner_control_ready(a);
        nk_begin_disabled(ui,!a->loaded || a->analysis_only || (a->runner.running?!ready:!idle(a)));
        const char *main_action=a->runner.running?(a->paused?"Fortsetzen":"Pause"):a->built?"Neuer Lauf":"Build";
        if(action_button(ui,main_action,true)){if(a->runner.running)command(a,a->paused?PS_MSG_RUN:PS_MSG_PAUSE);else if(a->built)start_run(a);else build_project(a);}nk_end_disabled(ui);
        nk_layout_row_push(ui,76*factor);a->run_control_bounds[2]=nk_widget_bounds(ui);nk_begin_disabled(ui,!ready || !a->paused);
        if(nk_button_label(ui,"Schritt"))command(a,PS_MSG_STEP);nk_end_disabled(ui);
        nk_layout_row_push(ui,76*factor);a->run_control_bounds[4]=nk_widget_bounds(ui);nk_begin_disabled(ui,!a->runner.running || a->stop_at!=0);
        if(nk_button_label(ui,"Stoppen")){command(a,PS_MSG_STOP);a->stop_at=ps_clock();}nk_end_disabled(ui);
        nk_layout_row_push(ui,124*factor);a->run_control_bounds[3]=nk_widget_bounds(ui);nk_begin_disabled(ui,!reset_available(a));
        if(nk_button_label(ui,a->reset_pending?"Zurücksetzen …":"Zurücksetzen"))reset_run(a);nk_end_disabled(ui);
        float speed_width=fminf(180*factor,fmaxf(90,w-668*factor));
        nk_layout_row_push(ui,speed_width);a->speed_bounds=nk_widget_bounds(ui);
        char speed[80];snprintf(speed,sizeof speed,a->simulation_speed?"Tempo · %.3g×":"Offline · maximal",a->simulation_speed);
        nk_begin_disabled(ui,a->reset_pending || a->reset_starting || (a->runner.running && (!a->hello || a->stop_at)));
        if(nk_combo_begin_label(ui,speed,nk_vec2(260,290))) {
            const double speeds[]={.25,.5,1,2,4,8,16,0};nk_layout_row_dynamic(ui,28,1);
            for(unsigned i=0;i<8;i++){char text[64];snprintf(text,sizeof text,speeds[i]?"%.3g× Echtzeit":"Offline · maximal",speeds[i]);a->speed_choices[i]=nk_widget_bounds(ui);if(nk_button_label(ui,text)){select_simulation_speed(a,speeds[i]);nk_combo_close(ui);}}
            nk_combo_end(ui);
        }nk_end_disabled(ui);
        nk_layout_row_end(ui);
        nk_layout_row_dynamic(ui,19,1);
        const char *state=a->analysis_only?"Gespeicherte Szene · Wiedergabe":a->runner.running?(!a->hello?"Initialisierung …":a->paused?"Pausiert":"Läuft"):a->job.running?"Build / Modellabfrage …":a->built?"Modell bereit":"Build erforderlich";
        nk_label_colored(ui,state,NK_TEXT_LEFT,muted());
    }nk_end(ui);
    if(laboratory_begin(a,windows[1],nk_rect(0,152,w,38),NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BACKGROUND)) {
        nk_style_push_style_item(ui,&ui->style.button.normal,nk_style_item_color(UI_COLOR(window)));
        nk_style_push_flags(ui,&ui->style.button.text_alignment,NK_TEXT_LEFT);
        nk_layout_row_begin(ui,NK_DYNAMIC,24,3);nk_layout_row_push(ui,.18f);
        a->laboratory_sides[0]=nk_widget_bounds(ui);
        if(nk_button_label(ui,"Szenenbaum")){bool hidden=(a->preferences.dock.hidden&1)!=0;if(hidden)ps_dock_reveal_outer(&a->preferences.dock,0,PS_DOCK_LEFT);else ps_dock_hide(&a->preferences.dock,0);a->laboratory_focus_return=1;}
        nk_layout_row_push(ui,.64f);panel_pull_handle(a,PS_DOCK_WORKSPACE,nk_widget_bounds(ui));nk_label_colored(ui,"SZENE / MESSLABOR",NK_TEXT_CENTERED,muted());
        nk_layout_row_push(ui,.18f);nk_style_pop_flags(ui);nk_style_push_flags(ui,&ui->style.button.text_alignment,NK_TEXT_RIGHT);
        a->laboratory_sides[1]=nk_widget_bounds(ui);
        if(nk_button_label(ui,"Inspektor")){bool hidden=(a->preferences.dock.hidden&8)!=0;if(hidden)ps_dock_reveal_outer(&a->preferences.dock,3,PS_DOCK_RIGHT);else ps_dock_hide(&a->preferences.dock,3);a->laboratory_focus_return=2;}
        nk_layout_row_end(ui);nk_style_pop_flags(ui);nk_style_pop_style_item(ui);
        if(a->laboratory_focus_return && nk_sdl_accessibility_request_focus(ui,NULL,a->laboratory_focus_return==1?"Szenenbaum":"Inspektor"))a->laboratory_focus_return=0;
    }nk_end(ui);
    float timeline_height=laboratory_dimension_value(a,1);
    struct nk_rect timeline=nk_rect(0,h-32-timeline_height,w,timeline_height);
    a->laboratory_splitters[1]=nk_rect(0,timeline.y-9,w,9);
    if(laboratory_begin(a,windows[2],timeline,NK_WINDOW_BACKGROUND)) {
        if(panel_is_detached(a,PS_PANEL_TIMELINE)){}
        else {
        char text[160];snprintf(text,sizeof text,"Zeitleiste · %.6g s · %s%s",display_time(a),a->timeline_browsing?"Aufzeichnung":"Aktuelle Szene",a->timeline_scenes?"":" · Messwerte ohne Szene");
        nk_layout_row_dynamic(ui,24,1);struct nk_rect title=nk_widget_bounds(ui);nk_label(ui,text,NK_TEXT_LEFT);
        panel_pull_handle(a,PS_PANEL_TIMELINE,title);timeline_controls(a);
        if(a->history_stride>1 || a->data.total>(uint64_t)a->history_count){nk_layout_row_dynamic(ui,24,1);nk_label_colored(ui,"Begrenzte, ausgedünnte Vorschau · vollständige Daten in der Messdatei",NK_TEXT_LEFT,muted());}
        }
    }nk_end(ui);
    if(laboratory_begin(a,windows[3],nk_rect(0,h-32,w,32),NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BACKGROUND)) {
        nk_layout_row_dynamic(ui,22,1);nk_label_colored(ui,a->status,NK_TEXT_LEFT,a->live_warning_valid?UI_COLOR(error):muted());
    }nk_end(ui);
    nk_style_pop_float(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
}
static void laboratory_separators(app *a) {
    struct nk_context *ui=a->ui;
    int cursor=-1;
    for(unsigned i=0;i<3;i++) {
        char window[40];snprintf(window,sizeof window,"laboratory-separator-%u",i);struct nk_rect r=a->laboratory_splitters[i];
        bool visible=laboratory_visible(a) && r.w>0 && (i!=2 || a->laboratory_curves);
        nk_window_show(ui,window,visible?NK_SHOWN:NK_HIDDEN);if(!visible)continue;
        nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(0,0));nk_style_push_float(ui,&ui->style.window.rounding,0);
        nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(nk_rgba(0,0,0,0)));
        struct nk_window *previous=ui->active;
        if(nk_begin(ui,window,r,NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BACKGROUND)) {
            nk_sdl_window_raise(ui,window);
            bool blocked=a->toolbar_menu || a->toolbar_pointer_consumed;
            ps_ui_accessibility_route(ui,!blocked);ui->active=previous;
            float lo,hi,standard,value=laboratory_dimension_value(a,i);laboratory_dimension_limits(a,i,&lo,&hi,&standard);
            if(ps_ui_separator_step_at(ui,r,laboratory_separator_name(i),lo,&value,hi,i==2?10/fmaxf(1,a->dock_panels[PS_DOCK_WORKSPACE].w-9):10))laboratory_dimension_set(a,i,value);
            if(!blocked && nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT) && nk_input_is_mouse_hovering_rect(&ui->input,r))nk_sdl_accessibility_request_focus(ui,NULL,laboratory_separator_name(i));
            bool focused=nk_sdl_accessibility_is_focused(ui,NULL,laboratory_separator_name(i));
            bool hover=nk_input_is_mouse_hovering_rect(&ui->input,r);
            if(!blocked && (hover || a->laboratory_resize==(int)i+1))cursor=i==2?0:1;
            nk_stroke_line(nk_window_get_canvas(ui),r.x+(i==2?r.w/2:0),r.y+(i==2?0:r.h/2),
                r.x+(i==2?r.w/2:r.w),r.y+(i==2?r.h:r.h/2),1,focused || hover?accent():UI_COLOR(border));
        }nk_end(ui);nk_style_pop_style_item(ui);nk_style_pop_float(ui);nk_style_pop_vec2(ui);nk_sdl_window_raise(ui,window);
    }
    if(laboratory_visible(a))for(unsigned i=0;i<PS_DOCK_NODES;i++)if(a->dock_splitters[i].w>0 &&
        nk_input_is_mouse_hovering_rect(&ui->input,a->dock_splitters[i]))cursor=a->preferences.dock.nodes[i].kind==PS_DOCK_X?0:1;
    if(SDL_GetMouseFocus()==a->window) {
        if(cursor>=0){if(!a->laboratory_cursors[cursor])a->laboratory_cursors[cursor]=SDL_CreateSystemCursor(cursor?SDL_SYSTEM_CURSOR_NS_RESIZE:SDL_SYSTEM_CURSOR_EW_RESIZE);
            if(a->laboratory_cursors[cursor])SDL_SetCursor(a->laboratory_cursors[cursor]);}
        else if(a->laboratory_cursors[0] || a->laboratory_cursors[1])SDL_SetCursor(SDL_GetDefaultCursor());
    }
}
