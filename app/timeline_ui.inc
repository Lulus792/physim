/* Simulation timeline: original CSS heading, readout, range and ruler.
 * Drawing retains app-owned font handles. Pointer seeking follows time, while
 * native arrow adjustment moves exactly one retained recorded state. */
static float timeline_text_width(const struct nk_user_font *font,const char *text) {
    return font->width(font->userdata,font->height,text,(int)strlen(text));
}
static void timeline_decimal(char *text) {for(char *p=text;*p;p++)if(*p=='.')*p=',';}
static void timeline_time_text(double time,char text[80]) {
    snprintf(text,80,fabs(time)<10000?"%.2f":"%.6g",time);timeline_decimal(text);
}
static struct nk_color timeline_color(struct nk_context *ui,struct nk_color color) {
    if(ui->current->widgets_disabled) {
        /* CSS opacity applies to the complete control over the flat window.
         * Premixing opaque endpoints also preserves correct antialiased text. */
        struct nk_color base=UI_COLOR(window);
        color.r=(nk_byte)lroundf(base.r*.55f+color.r*.45f);
        color.g=(nk_byte)lroundf(base.g*.55f+color.g*.45f);
        color.b=(nk_byte)lroundf(base.b*.55f+color.b*.45f);
    }return color;
}
static void timeline_text(struct nk_command_buffer *canvas,const struct nk_user_font *font,struct nk_rect r,const char *text,struct nk_color color) {
    nk_draw_text(canvas,nk_rect(r.x,r.y+(r.h-font->height)/2,r.w+1,font->height),text,(int)strlen(text),font,nk_rgba(0,0,0,0),color);
}
static float timeline_spaced_width(const struct nk_user_font *font,const char *text,float spacing) {
    float width=0;for(const char *p=text;*p;p++)width+=font->width(font->userdata,font->height,p,1)+spacing;return width;
}
static void timeline_spaced_text(struct nk_command_buffer *canvas,const struct nk_user_font *font,float x,float y,const char *text,float spacing,struct nk_color color) {
    for(const char *p=text;*p;p++){float width=font->width(font->userdata,font->height,p,1);nk_draw_text(canvas,nk_rect(x,y,width+1,font->height),p,1,font,nk_rgba(0,0,0,0),color);x+=width+spacing;}
}
static unsigned timeline_history_text(struct nk_command_buffer *canvas,const struct nk_user_font *font,const char *text,struct nk_rect r,float line_height,struct nk_color color) {
    int length=(int)strlen(text),at=0;unsigned lines=0;
    while(at<length) {
        int count=ps_ui_text_line(font,text+at,length-at,r.w,false);if(!count){nk_rune rune;count=nk_utf_decode(text+at,&rune,length-at);}if(count<=0)break;
        if(canvas)nk_draw_text(canvas,nk_rect(r.x,r.y+lines*line_height+(line_height-font->height)/2,r.w+1,font->height),text+at,count,font,nk_rgba(0,0,0,0),color);
        at+=count;while(at<length && text[at]==' ')at++;lines++;
    }return lines?lines:1;
}
static bool timeline_button(app *a,struct nk_rect r,const char *name,const char *caption,bool selected) {
    struct nk_context *ui=a->ui;struct nk_command_buffer *canvas=nk_window_get_canvas(ui);bool focused=false;
    nk_fill_rect(canvas,r,6*a->preferences.ui_size/16.f,timeline_color(ui,selected?UI_COLOR(selection_active):UI_COLOR(surface)));
    if(a->preferences.theme==PS_THEME_HIGH_CONTRAST)nk_stroke_rect(canvas,r,6*a->preferences.ui_size/16.f,a->preferences.ui_size/16.f,timeline_color(ui,UI_COLOR(border)));
    const struct nk_user_font *font=&a->workspace_status_font;float width=timeline_text_width(font,caption);
    timeline_text(canvas,font,nk_rect(r.x+(r.w-width)/2,r.y,width,r.h),caption,timeline_color(ui,selected?UI_COLOR(accent):UI_COLOR(text)));
    bool pressed=ps_ui_button_at_custom_focus(ui,r,name,selected,&focused);
    if(focused){float f=a->preferences.ui_size/16.f;nk_stroke_rect(canvas,nk_rect(r.x-4*f,r.y-4*f,r.w+8*f,r.h+8*f),10*f,2*f,UI_COLOR(accent));}
    return pressed;
}
static void timeline_drag_abort(app *a) {
    if(!a->timeline_dragging)return;
    a->timeline_browsing=a->timeline_drag_browsing;a->timeline_playing=a->timeline_drag_playing;
    a->timeline_view=a->timeline_drag_view;a->timeline_target=a->timeline_drag_target;a->timeline_wall=ps_clock();
    ps_scene_view_sync(&a->scene_view,a->timeline_browsing?&a->timeline_view.scene:&a->scene);
    timeline_release_drag(a);
}
static double timeline_fraction(double time,double start,double end) {
    if(!(end>start))return 0;
    double span=end-start,part=isfinite(span)?(time-start)/span:(time*.5-start*.5)/(end*.5-start*.5);
    return fmax(0,fmin(1,part));
}
static void timeline_seek_pointer(app *a,float x) {
    uint32_t count=ps_timeline_count(&a->timeline);if(count<2)return;
    struct nk_rect r=a->timeline_track_bounds;float f=a->preferences.ui_size/16.f;
    double start=ps_timeline_get(&a->timeline,0)->time,end=a->timeline.latest.time;
    double fraction=fmax(0,fmin(1,(x-r.x-8*f)/fmaxf(1,r.w-16*f)));
    double span=end-start,target=fraction<=0?start:fraction>=1?end:isfinite(span)?start+span*fraction:start*(1-fraction)+end*fraction;
    a->timeline_playing=false;timeline_select(a,ps_timeline_nearest(&a->timeline,target));
}
static bool timeline_controls_event(app *a,const SDL_Event *event,SDL_Window *window) {
    SDL_WindowID id=window?SDL_GetWindowID(window):0;
    if(a->timeline_dragging && (event->type==SDL_EVENT_QUIT || (id==a->timeline_drag_window &&
       (event->type==SDL_EVENT_WINDOW_FOCUS_LOST || event->type==SDL_EVENT_WINDOW_RESIZED || event->type==SDL_EVENT_WINDOW_CLOSE_REQUESTED))))timeline_drag_abort(a);
    if(id && id==a->timeline_control_window && (event->type==SDL_EVENT_WINDOW_FOCUS_LOST || event->type==SDL_EVENT_WINDOW_RESIZED))a->timeline_play_focus_return=false;
    if(id && id==a->timeline_control_window && event->type==SDL_EVENT_WINDOW_RESIZED)a->timeline_control_window=0;
    if(a->timeline_dragging && id==a->timeline_drag_window) {
        if(event->type==SDL_EVENT_KEY_DOWN && event->key.key==SDLK_ESCAPE){timeline_drag_abort(a);return true;}
        if(event->type==SDL_EVENT_KEY_DOWN && (event->key.mod&PS_UI_COMMAND_MOD) && (event->key.key==SDLK_1 || event->key.key==SDLK_3))timeline_drag_abort(a);
        if(event->type==SDL_EVENT_MOUSE_MOTION){struct nk_context *target=window==a->window?a->ui:a->panels[PS_PANEL_TIMELINE].ui;nk_input_motion(target,(int)event->motion.x,(int)event->motion.y);timeline_seek_pointer(a,event->motion.x);return true;}
        if(event->type==SDL_EVENT_MOUSE_BUTTON_UP && event->button.button==SDL_BUTTON_LEFT) {
            timeline_seek_pointer(a,event->button.x);timeline_release_drag(a);return true;
        }
    }
    if(a->tab!=1 || !id || id!=a->timeline_control_window || a->reset_pending || a->reset_starting || a->toolbar_menu || a->recovery)return false;
    struct nk_context *ui=window==a->window && !panel_is_detached(a,PS_PANEL_TIMELINE)?a->ui:
        window==a->panels[PS_PANEL_TIMELINE].window && panel_is_detached(a,PS_PANEL_TIMELINE)?a->panels[PS_PANEL_TIMELINE].ui:NULL;
    uint32_t count=ps_timeline_count(&a->timeline);if(!ui || count<2)return false;
    if(event->type==SDL_EVENT_MOUSE_BUTTON_DOWN || (event->type==SDL_EVENT_KEY_DOWN && (event->key.key==SDLK_TAB || event->key.key==SDLK_ESCAPE)))a->timeline_play_focus_return=false;
    if(event->type==SDL_EVENT_MOUSE_BUTTON_DOWN && event->button.button==SDL_BUTTON_LEFT) {
        struct nk_rect r=a->timeline_bounds[4];float x=event->button.x,y=event->button.y;
        if(x>=r.x && x<r.x+r.w && y>=r.y && y<r.y+r.h) {
            nk_sdl_accessibility_event(ui,event);
            if(!nk_sdl_accessibility_request_focus(ui,NULL,"Position im gespeicherten Lauf"))return false;
            a->timeline_pointer_focus=true;
            a->timeline_drag_browsing=a->timeline_browsing;a->timeline_drag_playing=a->timeline_playing;
            a->timeline_drag_view=a->timeline_view;a->timeline_drag_target=a->timeline_target;
            a->timeline_dragging=SDL_CaptureMouse(true);a->timeline_drag_window=a->timeline_dragging?id:0;timeline_seek_pointer(a,x);return true;
        }
    }
    if(event->type==SDL_EVENT_KEY_DOWN)a->timeline_pointer_focus=false;
    if(event->type==SDL_EVENT_KEY_DOWN && !(event->key.mod&(SDL_KMOD_CTRL|SDL_KMOD_GUI|SDL_KMOD_ALT)) &&
       nk_sdl_accessibility_is_focused(ui,NULL,"Position im gespeicherten Lauf")) {
        if(event->key.key==SDLK_HOME || event->key.key==SDLK_END) {
            a->timeline_playing=false;timeline_select(a,event->key.key==SDLK_HOME?0:count-1);return true;
        }
        if(event->key.key==SDLK_SPACE && !event->key.repeat){timeline_toggle_play(a);return true;}
    }return false;
}
/* Bound the ruler to seven readable 1/2/5-decade marks. Endpoints are the
 * fallback for very small/large or offset intervals that cannot form a step. */
static unsigned timeline_ruler(double start,double end,float width,float factor,const struct nk_user_font *font,double values[7],char labels[7][80]) {
    double span=end-start;
    if(!(end>start)){values[0]=start;snprintf(labels[0],80,"%.6g s",start);timeline_decimal(labels[0]);return 1;}
    for(unsigned desired=7;desired>=2;desired--) {
        double raw=span/(desired+1),scale=isfinite(raw) && raw>0?pow(10,floor(log10(raw))):0;
        double mantissa=scale>0?raw/scale:0,step=(mantissa<=1?1:mantissa<=2?2:mantissa<=5?5:10)*scale;
        unsigned count=0;
        if(isfinite(step) && step>0)for(unsigned attempt=0;attempt<8;attempt++) {
            double first=ceil(start/step)*step,tolerance=8*DBL_EPSILON*fmax(fabs(start),fabs(end));count=0;
            if(!isfinite(first))break;
            for(unsigned i=0;i<8;i++) {
                double t=first+i*step;if(!isfinite(t) || (t>end && t-end>tolerance))break;
                if(t<start && start-t>tolerance)continue;
                if(count<7)values[count]=fmax(start,fmin(end,t));
                count++;
            }
            if(count<=desired)break;
            double power=pow(10,floor(log10(step))),part=step/power;
            step=(part<1.5?2:part<3.5?5:10)*power;
        }
        if(count<2 || count>desired){count=2;values[0]=start;values[1]=end;}
        bool distinct=false;float widest=0;
        const unsigned precisions[]={3,6,9,12,15,17};
        for(unsigned p=0;p<SDL_arraysize(precisions) && !distinct;p++) {
            distinct=true;widest=0;
            for(unsigned i=0;i<count;i++) {
                snprintf(labels[i],80,"%.*g s",(int)precisions[p],values[i]);timeline_decimal(labels[i]);
                if(i && !strcmp(labels[i],labels[i-1]))distinct=false;
                widest=fmaxf(widest,timeline_text_width(font,labels[i]));
            }
        }
        if(desired==2 || width>=count*widest+(count-1)*12*factor)return count;
    }return 0;
}
static void timeline_controls(app *a) {
    struct nk_context *ui=a->ui;struct nk_rect bounds=nk_window_get_bounds(ui);float f=a->preferences.ui_size/16.f;
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);struct nk_rect clip=canvas->clip;nk_push_scissor(canvas,bounds);
    const struct nk_user_font *heading=&a->workbench_fonts[3],*small=&a->workspace_status_font,*tick=&a->workbench_fonts[1],*value=&a->laboratory_timeline_font,*unit=&a->workbench_fonts[0];
    uint32_t count=ps_timeline_count(&a->timeline),selected=a->timeline_browsing?ps_timeline_nearest(&a->timeline,a->timeline_view.time):count?count-1:0;
    bool ready=count && !a->reset_pending && !a->reset_starting;
    const char *play=a->timeline_playing?"Ⅱ Anhalten":"▷ Abspielen";
    float right=bounds.x+bounds.w-18,button_height=24.5f*f,play_width=timeline_text_width(small,play)+22*f,live_width=timeline_text_width(small,"● Live")+22*f;
    float live_x=right-live_width,play_x=live_x-6*f-play_width;
    char history[320];snprintf(history,sizeof history,"%s%s%s",!count?"Noch keine Aufzeichnung":a->timeline_browsing?"Aufzeichnung · Lauf bleibt unverändert":"Aktuelle Szene",
        count && !a->timeline_scenes?" · Messwerte ohne Szene":"",a->timeline.stride>1 || a->history_stride>1 || a->data.total>(uint64_t)a->history_count?" · Vorschau ausgedünnt":"");
    float heading_width=timeline_text_width(heading,"Zeitleiste"),history_x=bounds.x+18+heading_width+14*f;
    struct nk_rect history_rect=nk_rect(history_x,0,fmaxf(1,play_x-14*f-history_x),0);
    unsigned history_lines=timeline_history_text(NULL,small,history,history_rect,14.5f*f,UI_COLOR(muted));
    float header_height=fmaxf(button_height,history_lines*14.5f*f);
    char current[80],last[80],numeral[82],suffix[88],accessible[512];double last_time=count?a->timeline.latest.time:0;
    timeline_time_text(display_time(a),current);timeline_time_text(last_time,last);snprintf(numeral,sizeof numeral,"%s ",current);snprintf(suffix,sizeof suffix,"/ %s s",last);
    float number_width=timeline_spaced_width(value,numeral,-.4f*f),suffix_width=timeline_text_width(unit,suffix);
    float output_width=fmaxf(120*f,number_width+suffix_width),maximum_output=fmaxf(120*f,(bounds.w-36)*.48f);
    bool stacked=output_width>maximum_output;if(stacked)output_width=fmaxf(120*f,fmaxf(number_width,suffix_width));
    output_width=fminf(output_width,fmaxf(1,bounds.w-36-25*f-80*f));
    float track_height=(stacked?fmaxf(33.05f,26.1f+15.95f):33.05f)*f,fit=fminf(1,fmaxf(0,(bounds.h-1-header_height-track_height)/(35*f)));
    float top=bounds.y+fmaxf(6,1+11*f*fit),track_y=top+header_height+10*f*fit;
    a->timeline_heading_bounds=nk_rect(bounds.x+18,top,fmaxf(1,play_x-14*f-bounds.x-18),header_height);
    a->timeline_control_window=SDL_GetWindowID(a->window);
    a->timeline_bounds[0]=a->timeline_bounds[1]=nk_rect(0,0,0,0);
    timeline_text(canvas,heading,nk_rect(bounds.x+18,top,heading_width,header_height),"Zeitleiste",UI_COLOR(text));
    history_rect.y=top+(header_height-14.5f*f*history_lines)/2;
    timeline_history_text(canvas,small,history,history_rect,14.5f*f,UI_COLOR(muted));
    history_rect.h=history_lines*14.5f*f;
    ps_ui_accessibility_label(ui,history_rect,history);panel_pull_handle(a,PS_PANEL_TIMELINE,a->timeline_heading_bounds);
    a->timeline_bounds[3]=nk_rect(play_x,top+(header_height-button_height)/2,play_width,button_height);
    a->timeline_bounds[2]=nk_rect(live_x,top+(header_height-button_height)/2,live_width,button_height);
    bool play_focus=nk_sdl_accessibility_is_focused(ui,NULL,"Abspielen") || nk_sdl_accessibility_is_focused(ui,NULL,"Anhalten");
    if(play_focus && !nk_sdl_accessibility_is_focused(ui,NULL,a->timeline_playing?"Anhalten":"Abspielen"))a->timeline_play_focus_return=true;
    nk_begin_disabled(ui,!ready || count<2);
    if(timeline_button(a,a->timeline_bounds[3],a->timeline_playing?"Anhalten":"Abspielen",play,false)){timeline_toggle_play(a);if(play_focus)a->timeline_play_focus_return=true;}
    nk_end_disabled(ui);
    nk_begin_disabled(ui,!ready);if(timeline_button(a,a->timeline_bounds[2],"Live","● Live",!a->timeline_browsing))timeline_live(a);nk_end_disabled(ui);
    if(a->timeline_play_focus_return && nk_sdl_accessibility_request_focus(ui,NULL,a->timeline_playing?"Anhalten":"Abspielen"))a->timeline_play_focus_return=false;
    timeline_time_text(display_time(a),current);snprintf(numeral,sizeof numeral,"%s ",current);number_width=timeline_spaced_width(value,numeral,-.4f*f);
    a->timeline_readout_bounds=nk_rect(bounds.x+18,track_y,output_width,track_height);
    float number_y=track_y+(stacked?0:(track_height-value->height)/2);
    timeline_spaced_text(canvas,value,bounds.x+18,number_y,numeral,-.4f*f,UI_COLOR(text));
    const struct nk_font *source=value->userdata.ptr;float ascent=source && source->info.height>0?source->info.ascent/source->info.height:.8f;
    float suffix_y=stacked?track_y+26.1f*f:number_y+(value->height-unit->height)*ascent;
    nk_draw_text(canvas,nk_rect(bounds.x+18+(stacked?0:number_width),suffix_y,suffix_width+1,unit->height),suffix,(int)strlen(suffix),unit,nk_rgba(0,0,0,0),UI_COLOR(muted));
    snprintf(accessible,sizeof accessible,"Betrachtete Zeit %.17g s · letzter aufgezeichneter Zeitpunkt %.17g s. Zustand %u/%u · %llu empfangene Vorschauzustände%s%s%s",display_time(a),last_time,selected+(count?1:0),count,(unsigned long long)a->timeline.seen,
        a->timeline.stride>1 || a->history_stride>1?" · Begrenzte Vorschau; vollständige Daten in der Messdatei":"",a->timeline_error[0]?" · ":"",a->timeline_error);
    ps_ui_accessibility_label(ui,a->timeline_readout_bounds,accessible);
    struct nk_rect range=nk_rect(bounds.x+18+output_width+25*f,track_y+(track_height-33.05f*f)/2,fmaxf(1,right-bounds.x-18-output_width-25*f),17*f);
    a->timeline_bounds[4]=a->timeline_track_bounds=range;
    nk_begin_disabled(ui,!ready || count<2);float position=(float)selected;bool range_focus=false;
    if(ps_ui_range_at_custom_focus(ui,range,"Position im gespeicherten Lauf",&position,(float)(count>1?count-1:1),1,&range_focus)) {a->timeline_playing=false;timeline_select(a,(uint32_t)lroundf(fmaxf(0,fminf((float)(count?count-1:0),position))));}
    double start=count?ps_timeline_get(&a->timeline,0)->time:0,end=last_time;
    double fraction=timeline_fraction(display_time(a),start,end);
    float center=range.x+8*f+(range.w-16*f)*(float)fraction,cy=range.y+8.5f*f;
    struct nk_rect bar=nk_rect(range.x,cy-4*f,range.w,8*f);
    struct nk_color outline=a->preferences.theme==PS_THEME_LIGHT?nk_rgb(178,178,178):nk_rgb(96,96,96);
    struct nk_color fill=a->preferences.theme==PS_THEME_LIGHT?nk_rgb(239,239,239):nk_rgb(59,59,59);
    if(a->preferences.theme==PS_THEME_HIGH_CONTRAST){outline=UI_COLOR(border);fill=UI_COLOR(window);}
    nk_fill_rect(canvas,bar,4*f,timeline_color(ui,outline));nk_fill_rect(canvas,nk_rect(bar.x+f,bar.y+f,fmaxf(0,bar.w-2*f),6*f),3*f,timeline_color(ui,fill));
    if(center>bar.x+f)nk_fill_rect(canvas,nk_rect(bar.x+f,bar.y+f,fmaxf(0,center-bar.x-f),6*f),3*f,timeline_color(ui,UI_COLOR(accent)));
    nk_fill_circle(canvas,nk_rect(center-8*f,cy-8*f,16*f,16*f),timeline_color(ui,UI_COLOR(accent)));nk_end_disabled(ui);
    if(!a->timeline_pointer_focus && range_focus)nk_stroke_rect(canvas,nk_rect(range.x-4*f,range.y-4*f,range.w+8*f,range.h+8*f),6*f,2*f,UI_COLOR(accent));
    char labels[7][80];unsigned ticks=count?timeline_ruler(start,end,range.w,f,tick,a->timeline_tick_values,labels):0;
    a->timeline_tick_count=ticks;memset(a->timeline_tick_bounds,0,sizeof a->timeline_tick_bounds);
    for(unsigned i=0;i<ticks;i++) {
        float width=timeline_text_width(tick,labels[i]);double part=timeline_fraction(a->timeline_tick_values[i],start,end);
        if(!isfinite(part))part=i?1:0;
        float x=range.x+(range.w-width)*(float)fmax(0,fmin(1,part));
        a->timeline_tick_bounds[i]=nk_rect(x,range.y+20*f,width,13.05f*f);timeline_text(canvas,tick,a->timeline_tick_bounds[i],labels[i],UI_COLOR(muted));
    }
    nk_fill_rect(canvas,nk_rect(bounds.x,bounds.y,bounds.w,1),0,UI_COLOR(border));nk_push_scissor(canvas,clip);
}
