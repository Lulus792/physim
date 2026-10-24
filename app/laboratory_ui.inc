/* Messlabor: all numbers, scenes and commands come from the native experiment. */
static void laboratory_note(app *a,const char *text) {
    struct nk_context *ui=a->ui;float f=a->preferences.ui_size/16.f;
    const struct nk_user_font *font=&a->laboratory_caption_font;
    float width=fmaxf(1,nk_window_get_content_region(ui).w),line_height=17.05f*f;
    unsigned lines=laboratory_wrapped_lines(font,text,width);
    nk_layout_row_dynamic(ui,16*f+lines*line_height,1);struct nk_rect r;
    if(nk_widget(&r,ui)==NK_WIDGET_INVALID)return;
    struct nk_color color=ui->style.text.color,normal=UI_COLOR(text);if(!memcmp(&color,&normal,sizeof color))color=muted();
    laboratory_wrapped_text(nk_window_get_canvas(ui),font,text,nk_rect(r.x,r.y+6*f,r.w,r.h-16*f),line_height,color);
    ps_ui_accessibility_label(ui,r,text);
}
#include "laboratory_metrics.inc"
#include "laboratory_object.inc"
static const ps_channel *laboratory_channels(app *a,uint32_t *count) {
    if(a->analysis_only){*count=a->data.channel_count;return a->data.channels;}
    *count=a->channel_count;return a->live_channels;
}
static int laboratory_channel_picker(app *a,unsigned slot,bool chart) {
    uint32_t count;const ps_channel *channels=laboratory_channels(a,&count);
    int *chosen=&a->laboratory_channels[slot];
    if(!count){*chosen=0;return -1;}
    if(*chosen<0 || (unsigned)*chosen>=count)*chosen=slot<count?(int)slot:0;
    const char *names[PS_MAX_CHANNELS];char labels[PS_MAX_CHANNELS][160];
    for(unsigned i=0;i<count;i++){ps_display_unit u=channel_display_unit(a,&channels[i]);snprintf(labels[i],sizeof labels[i],"%s [%s]",channels[i].name,u.symbol);names[i]=labels[i];}
    struct nk_context *ui=a->ui;float f=a->preferences.ui_size/16.f;
    ps_display_unit unit=channel_display_unit(a,&channels[*chosen]);
    laboratory_channel_paint paint={a,slot,chart,channels[*chosen].name,unit.symbol,labels[*chosen]};
    float width=nk_window_get_content_region(ui).w;
    unsigned lines=laboratory_wrapped_lines(&a->workspace_status_font,labels[*chosen],fmaxf(1,width-24*f));
    float height=chart?laboratory_channel_measure(a,paint.name,paint.unit,nk_rect(0,0,width,0)).height:(8+14.5f*lines)*f;
    ps_ui_row_exact_dynamic(ui,height,1);
    char control[64];if(chart)snprintf(control,sizeof control,"Messkurve %u: Kanal",slot+1);else snprintf(control,sizeof control,"Messkanal auswählen");
    nk_style_push_font(ui,&a->workspace_status_font);
    if(ps_ui_combo_begin_named_painted(ui,control,labels[*chosen],nk_vec2(fmaxf(width,240*f),240*f),laboratory_channel_field,&paint)) {
        ps_ui_row_exact_dynamic(ui,28*f,1);
        for(unsigned i=0;i<count;i++)if(ps_ui_fixed_button_label(ui,names[i])){*chosen=(int)i;nk_combo_close(ui);}
        nk_combo_end(ui);
    }
    nk_style_pop_font(ui);return *chosen;
}
static void laboratory_measurements(app *a,float height) {
    struct nk_context *ui=a->ui;uint32_t count;const ps_channel *channels=laboratory_channels(a,&count);
    memset(a->laboratory_channel_fields,0,sizeof a->laboratory_channel_fields);
    a->laboratory_curve_channel[0]=a->laboratory_curve_channel[1]=-1;
    float scale=a->preferences.ui_size/16.f,inset=17*scale;
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,0));
    float width=nk_window_get_content_region(ui).w,inner=fmaxf(1,width-2*inset);
    const struct nk_user_font *label_font=&a->laboratory_caption_font,*time_font=&a->workspace_status_font;
    char time[80],heading[160];timeline_time_text(display_time(a),time);snprintf(heading,sizeof heading,"bei t = %s s%s",time,a->timeline_browsing?" · Rückblick":"");
    float title_width=timeline_text_width(label_font,"Messwerte"),time_width=timeline_text_width(time_font,heading),toggle_width=timeline_text_width(label_font,"Kurven anzeigen")+54*scale;
    bool wrap=title_width+time_width+toggle_width+24*scale>inner;
    float header_height=(wrap?64:32)*scale,header_total=12*scale+header_height+14*scale;
    nk_layout_row_dynamic(ui,12*scale,1);nk_spacer(ui);
    nk_layout_row_dynamic(ui,header_height,1);struct nk_rect header;nk_widget(&header,ui);
    a->live_measurement_bounds=nk_rect(header.x,header.y,wrap?header.w:fmaxf(1,header.w-toggle_width-inset),32*scale);
    a->laboratory_measure_title=nk_rect(header.x+inset,header.y,title_width,32*scale);
    a->laboratory_measure_time=nk_rect(header.x+inset+title_width+12*scale,header.y,fmaxf(1,inner-title_width-12*scale-(wrap?0:toggle_width+12*scale)),32*scale);
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    timeline_text(canvas,label_font,a->laboratory_measure_title,"Messwerte",UI_COLOR(text));
    struct nk_rect time_rect=a->laboratory_measure_time;
    float time_height=14.5f*scale*laboratory_wrapped_lines(time_font,heading,time_rect.w);time_rect.y+=(time_rect.h-time_height)/2;time_rect.h=time_height;
    laboratory_wrapped_text(canvas,time_font,heading,time_rect,14.5f*scale,UI_COLOR(muted));
    ps_ui_accessibility_label(ui,a->laboratory_measure_title,"Messwerte");ps_ui_accessibility_label(ui,a->laboratory_measure_time,heading);
    if(!a->panel_render)panel_pull_handle(a,PS_PANEL_MEASURE,a->live_measurement_bounds);
    float sw=36*scale,sh=22*scale;
    a->laboratory_switch=nk_rect(header.x+header.w-inset-toggle_width,header.y+(wrap?32*scale:0),toggle_width,32*scale);
    struct nk_rect r=a->laboratory_switch;
    bool focused=false;bool activated=ps_ui_switch_at_custom_focus(ui,r,"Kurven anzeigen",&a->laboratory_curves,&focused);
    (void)activated;
    if(nk_input_is_mouse_hovering_rect(&ui->input,r))nk_fill_rect(canvas,r,5*scale,UI_COLOR(surface));
    timeline_text(canvas,label_font,nk_rect(r.x+4*scale,r.y,r.w-sw-18*scale,r.h),"Kurven anzeigen",UI_COLOR(text));
    struct nk_rect track=nk_rect(r.x+r.w-sw-4*scale,r.y+(r.h-sh)/2,sw,sh);
    /* simulation.css --axis; the border is quieter than text, stronger than a divider. */
    struct nk_color axis=a->preferences.theme==PS_THEME_HIGH_CONTRAST?UI_COLOR(border):a->preferences.theme==PS_THEME_LIGHT?nk_rgb(107,119,136):nk_rgb(115,122,134);
    nk_fill_rect(canvas,track,12*scale,a->laboratory_curves?UI_COLOR(primary):UI_COLOR(surface));nk_stroke_rect(canvas,track,12*scale,scale,a->laboratory_curves?UI_COLOR(primary):axis);
    nk_fill_circle(canvas,nk_rect(track.x+(a->laboratory_curves?18:4)*scale,track.y+4*scale,14*scale,14*scale),a->laboratory_curves?UI_COLOR(primary_text):UI_COLOR(muted));
    if(focused)laboratory_focus_outline(a,r,10);
    nk_layout_row_dynamic(ui,14*scale,1);nk_spacer(ui);
    unsigned slots=count>1?2:1;float gap=(a->laboratory_scene_host_width<=1150?12:20)*scale;
    bool stacked=slots>1 && (inner-gap)/2<140*scale;float card_width=stacked?inner:(inner-gap*(slots-1))/slots;
    char names[2][96],numbers[2][96],units[2][96],exact[2][240];float cards=0;
    for(unsigned slot=0;slot<slots;slot++) {
        if(!count){snprintf(names[slot],96,"Messkanäle");snprintf(numbers[slot],96,"Noch keine Messwerte");units[slot][0]=0;snprintf(exact[slot],240,"Messkanäle: Noch keine Messwerte");}
        else {int selected=a->laboratory_channels[slot];if(selected<0 || (unsigned)selected>=count)selected=slot<count?(int)slot:0;
            snprintf(names[slot],96,"%s",channels[selected].name);laboratory_metric_text(a,&channels[selected],(unsigned)selected,numbers[slot],units[slot],exact[slot]);}
        cards=fmaxf(cards,laboratory_metric_height(a,names[slot],numbers[slot],units[slot],card_width));
    }
    float card_total=stacked?cards*slots+gap*(slots-1):cards;
    laboratory_curve_data curves[2];float natural[2]={0,0};
    float chart_gap=laboratory_curve_gap(a),ratio=laboratory_dimension_value(a,2),chart_width=fmaxf(1,width-2*inset);
    bool pair=count>1 && chart_width>=430;
    if(a->laboratory_curves)for(unsigned slot=0;slot<slots;slot++) {
        laboratory_curve_prepare(a,channels,count,slot,&curves[slot]);
        float w=pair?(chart_width-chart_gap)*(slot?1-ratio:ratio):chart_width;
        const char *name=curves[slot].selected<0?"Messkanäle":channels[curves[slot].selected].name;
        natural[slot]=laboratory_curve_minimum(a,name,curves[slot].unit.symbol,a->history_t,curves[slot].values,count?a->history_count:0,curves[slot].represented,w);
    }
    float natural_total=pair?fmaxf(natural[0],natural[1]):natural[0]+(slots>1?natural[1]+chart_gap:0);
    float content_minimum=card_total+14*scale+(a->laboratory_curves?natural_total+22*scale:0);
    bool scrollable=content_minimum>fmaxf(32,height-header_total);
    if(!scrollable)a->laboratory_measure_scroll_x=a->laboratory_measure_scroll_y=0;
    nk_layout_row_dynamic(ui,fmaxf(32,height-header_total),1);
    nk_style_push_vec2(ui,&ui->style.window.group_padding,nk_vec2(0,0));
    if(!nk_group_scrolled_offset_begin(ui,&a->laboratory_measure_scroll_x,&a->laboratory_measure_scroll_y,"Messwerte und Kurven",scrollable?0:NK_WINDOW_NO_SCROLLBAR)) {
        nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);return;
    }
    a->laboratory_measure_bounds=nk_window_get_content_region(ui);
    a->laboratory_measure_width=a->laboratory_measure_bounds.w;
    width=nk_window_get_content_region(ui).w;inner=fmaxf(1,width-2*inset);stacked=slots>1 && (inner-gap)/2<140*scale;card_width=stacked?inner:(inner-gap*(slots-1))/slots;
    cards=0;for(unsigned slot=0;slot<slots;slot++)cards=fmaxf(cards,laboratory_metric_height(a,names[slot],numbers[slot],units[slot],card_width));
    card_total=stacked?cards*slots+gap*(slots-1):cards;
    memset(a->laboratory_metric_cards,0,sizeof a->laboratory_metric_cards);
    nk_layout_space_begin(ui,NK_STATIC,card_total+(a->laboratory_curves?0:14*scale),(int)slots);
    for(unsigned slot=0;slot<slots;slot++) {
        nk_layout_space_push(ui,nk_rect(inset+(stacked?0:slot*(card_width+gap)),stacked?slot*(cards+gap):0,card_width,cards));
        struct nk_rect card;nk_widget(&card,ui);laboratory_metric_draw(a,slot,card,names[slot],numbers[slot],units[slot],exact[slot]);
    }
    nk_layout_space_end(ui);
    memset(a->laboratory_channel_fields,0,sizeof a->laboratory_channel_fields);
    if(!a->laboratory_curves){nk_group_end(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);return;}
    nk_layout_row_dynamic(ui,22*scale,1);nk_spacer(ui);
    width=nk_window_get_content_region(ui).w;
    chart_width=fmaxf(1,width-2*inset);pair=count>1 && chart_width>=430;
    for(unsigned slot=0;slot<slots;slot++) {
        float w=pair?(chart_width-chart_gap)*(slot?1-ratio:ratio):chart_width;
        const char *name=curves[slot].selected<0?"Messkanäle":channels[curves[slot].selected].name;
        natural[slot]=laboratory_curve_minimum(a,name,curves[slot].unit.symbol,a->history_t,curves[slot].values,count?a->history_count:0,curves[slot].represented,w);
    }
    natural_total=pair?fmaxf(natural[0],natural[1]):natural[0]+(slots>1?natural[1]+chart_gap:0);
    float plots=fmaxf(natural_total,height-header_total-card_total-22*scale-14*scale);
    if(pair){nk_layout_row_begin(ui,NK_DYNAMIC,plots,5);nk_layout_row_push(ui,inset/width);nk_spacer(ui);}
    nk_style_push_vec2(ui,&ui->style.window.group_padding,nk_vec2(0,0));
    for(unsigned slot=0;slot<(count>1?2u:1u);slot++) {
        if(pair) {
            if(slot){nk_layout_row_push(ui,chart_gap/width);struct nk_rect gap_rect;nk_widget(&gap_rect,ui);a->laboratory_splitters[2]=nk_rect(gap_rect.x+(gap_rect.w-9)/2,gap_rect.y,9,gap_rect.h);}
            nk_layout_row_push(ui,(chart_width-chart_gap)/width*(slot?1-ratio:ratio));
        } else {if(slot){nk_layout_row_dynamic(ui,chart_gap,1);nk_spacer(ui);}nk_layout_row_begin(ui,NK_DYNAMIC,natural[slot]+(plots-natural_total)/slots,3);nk_layout_row_push(ui,inset/width);nk_spacer(ui);nk_layout_row_push(ui,chart_width/width);}
        char group[40];snprintf(group,sizeof group,"laboratory-curve-%u",slot);a->laboratory_plots[slot]=nk_widget_bounds(ui);
        if(nk_group_begin(ui,group,NK_WINDOW_NO_SCROLLBAR)) {
            nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,0));
            int selected=laboratory_channel_picker(a,slot,true);
            if(selected<0)laboratory_note(a,"Noch keine Messkanäle vorhanden.");
            else {
                /* A menu selection takes effect for the next complete frame.
                 * Cards and the just-painted caption still show this frame's
                 * prepared channel; keep its curve bound to the same data. */
                a->laboratory_curve_channel[slot]=curves[slot].selected;
                float header=a->laboratory_channel_fields[slot].h;
                char note[180];float note_height=laboratory_curve_note(a,a->history_t,curves[slot].values,a->history_count,curves[slot].represented,nk_window_get_content_region(ui).w,note);
                laboratory_curve(a,a->history_t,curves[slot].values,a->history_count,curves[slot].represented,fmaxf(96*scale+note_height,nk_window_get_content_region(ui).h-header),slot);
            }
            nk_group_end(ui);
            nk_style_pop_vec2(ui);
        }
        if(!pair){nk_layout_row_push(ui,inset/width);nk_spacer(ui);nk_layout_row_end(ui);}
    }
    nk_style_pop_vec2(ui);
    if(pair){nk_layout_row_push(ui,inset/width);nk_spacer(ui);nk_layout_row_end(ui);}
    nk_group_end(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
}
static void laboratory_workspace(app *a) {
    struct nk_context *ui=a->ui;
    float available=nk_window_get_content_region(ui).h-2*ui->style.window.padding.y;
    if(a->panel_render==PS_DOCK_WORKSPACE+1){viewport(a,fmaxf(64,available));return;}
    float minimum=laboratory_scene_minimum(a,nk_window_get_content_region(ui).w);
    float measure=fmaxf(LABORATORY_MEASURE_MIN,fminf(laboratory_dimension_value(a,0),available-minimum-1));
    float scene=fmaxf(minimum,available-measure-1);
    if(panel_is_detached(a,PS_DOCK_WORKSPACE)) {
        nk_layout_row_dynamic(ui,scene,1);struct nk_rect slot=nk_widget_bounds(ui);
        nk_spacer(ui);
        a->scene_viewport_bounds=slot;
        a->laboratory_scene_frame=slot;
    } else {
        viewport(a,scene);
    }
    a->laboratory_splitters[0]=nk_rect(a->laboratory_scene_frame.x,a->laboratory_scene_frame.y+a->laboratory_scene_frame.h-4,a->laboratory_scene_frame.w,9);
    nk_layout_row_dynamic(ui,1,1);nk_spacer(ui);
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
#include "laboratory_transport.inc"
static void laboratory_chrome(app *a,float w,float h) {
    struct nk_context *ui=a->ui;bool visible=laboratory_visible(a);
    const char *windows[]={"laboratory-transport","laboratory-sections","laboratory-timeline","laboratory-status"};
    for(unsigned i=0;i<4;i++)nk_window_show(ui,windows[i],visible?NK_SHOWN:NK_HIDDEN);
    if(!visible)return;
    nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(18,6));
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(6,4));
    nk_style_push_float(ui,&ui->style.window.rounding,0);
    laboratory_transport(a,w);
    if(laboratory_begin(a,windows[1],nk_rect(0,195,w,37),NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BACKGROUND)) {
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
        workspace_rule(nk_window_get_canvas(ui),nk_rect(0,231,w,1));
    }nk_end(ui);
    float timeline_height=laboratory_dimension_value(a,1);
    struct nk_rect timeline=nk_rect(0,h-WORKSPACE_STATUS_HEIGHT-timeline_height,w,timeline_height);
    a->laboratory_splitters[1]=nk_rect(0,timeline.y-4,w,9);
    nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(0,0));
    if(laboratory_begin(a,windows[2],timeline,NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BACKGROUND)) {
        if(!panel_is_detached(a,PS_PANEL_TIMELINE))timeline_controls(a);
    }nk_end(ui);nk_style_pop_vec2(ui);
    nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(UI_COLOR(header)));
    nk_style_push_font(ui,&a->workspace_status_font);
    nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(12,5));
    if(laboratory_begin(a,windows[3],nk_rect(0,h-WORKSPACE_STATUS_HEIGHT,w,WORKSPACE_STATUS_HEIGHT),NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BACKGROUND)) {
        nk_layout_row_dynamic(ui,18,1);nk_label_colored(ui,a->status,NK_TEXT_LEFT,a->live_warning_valid?UI_COLOR(error):muted());
        workspace_rule(nk_window_get_canvas(ui),nk_rect(0,h-WORKSPACE_STATUS_HEIGHT,w,1));
    }nk_end(ui);
    nk_style_pop_vec2(ui);nk_style_pop_font(ui);nk_style_pop_style_item(ui);
    nk_style_pop_float(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
}
static void laboratory_separators(app *a) {
    struct nk_context *ui=a->ui;
    int cursor=-1;
    for(unsigned i=0;i<3;i++) {
        char window[40];snprintf(window,sizeof window,"laboratory-separator-%u",i);struct nk_rect r=laboratory_handle_bounds(a,i);
        bool visible=laboratory_visible(a) && r.w>0 && r.h>0 && (i!=2 || a->laboratory_curves);
        nk_window_show(ui,window,visible?NK_SHOWN:NK_HIDDEN);if(!visible)continue;
        nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(0,0));nk_style_push_float(ui,&ui->style.window.rounding,0);
        nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(nk_rgba(0,0,0,0)));
        struct nk_window *previous=ui->active;
        if(nk_begin(ui,window,r,NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BACKGROUND)) {
            nk_sdl_window_raise(ui,window);
            bool blocked=a->toolbar_menu || a->toolbar_pointer_consumed;
            ps_ui_accessibility_route(ui,!blocked);ui->active=previous;
            float lo,hi,standard,value=laboratory_dimension_value(a,i);laboratory_dimension_limits(a,i,&lo,&hi,&standard);
            if(ps_ui_separator_step_at(ui,r,laboratory_separator_name(i),lo,&value,hi,i==2?10/fmaxf(1,laboratory_curve_width(a)-laboratory_curve_gap(a)):10))laboratory_dimension_set(a,i,value);
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
