/* Measurement typography follows metric-grid; data and conversion stay native. */
static struct nk_color laboratory_curve_color(unsigned slot) {
    /* Shared bridge.js accent for blue; simulation.css --vector for amber. */
    if(ui_palette==&PS_UI_HIGH_CONTRAST)return ui_palette->curves[slot?1:0];
    return slot?ui_palette->curves[1]:UI_COLOR(accent);
}
static void laboratory_metric_number(double value,char number[96]) {
    double magnitude=fabs(value);
    snprintf(number,96,magnitude>=1e6 || (magnitude>0 && magnitude<.005)?"%.2e":"%.2f",value==0?0:value);
    timeline_decimal(number);
}
static void laboratory_metric_text(app *a,const ps_channel *ch,unsigned selected,char number[96],char unit[96],char exact[240]) {
    ps_display_unit u=channel_display_unit(a,ch);double converted,value=display_values(a)[selected];
    int mask=a->channel_status[selected];unit[0]=0;
    if(mask>=0 && display_values(a)[mask]!=1)snprintf(number,96,"%s",display_values(a)[mask]==2?"Messung ausgefallen":"Keine neue Messung");
    else if(ps_display_unit_value(&u,value,&converted)!=PS_OK)snprintf(number,96,"nicht darstellbar");
    else {
        /* Keep tiny nonzero readings visible; ordinary values use the two
         * fractional digits of the design. Exact values remain in the tooltip. */
        laboratory_metric_number(converted,number);snprintf(unit,96,"%s",u.symbol);
    }
    char precise[128];channel_display_text(&u,value,precise,sizeof precise);
    snprintf(exact,240,"%s: %s",ch->name,*unit?precise:number);
}
static float laboratory_metric_height(app *a,const char *label,const char *number,const char *unit,float width) {
    float f=a->preferences.ui_size/16.f;const struct nk_user_font *small=&a->workspace_status_font,*units=&a->laboratory_caption_font;
    float labels=14.5f*f*laboratory_wrapped_lines(small,label,width),line=a->laboratory_measurement_font.height/a->workbench_regular_em*1.45f;
    float value=timeline_spaced_width(&a->laboratory_measurement_font,number,-.7f*f),suffix=timeline_text_width(units,unit);
    if(!*unit)line=15.95f*f*laboratory_wrapped_lines(units,number,width);
    else {
        if(value>width)line=33.35f*f*laboratory_wrapped_lines(&a->laboratory_measurement_font,number,width);
        if(value+6*f+suffix>width)line+=15.95f*f*laboratory_wrapped_lines(units,unit,width);
    }
    return labels+2*f+line+8*f+2*f;
}
static void laboratory_metric_draw(app *a,unsigned slot,struct nk_rect card,const char *label,const char *number,const char *unit,const char *exact) {
    float f=a->preferences.ui_size/16.f;struct nk_context *ui=a->ui;struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    const struct nk_user_font *small=&a->workspace_status_font,*units=&a->laboratory_caption_font,*large=&a->laboratory_measurement_font;
    struct nk_rect name=nk_rect(card.x,card.y,card.w,14.5f*f*laboratory_wrapped_lines(small,label,card.w));
    laboratory_wrapped_text(canvas,small,label,name,14.5f*f,UI_COLOR(muted));
    float y=name.y+name.h+2*f,line=large->height/a->workbench_regular_em*1.45f;
    struct nk_rect value=nk_rect(card.x,y,card.w,line),suffix=nk_rect(0,0,0,0);
    if(*unit) {
        float w=timeline_spaced_width(large,number,-.7f*f),u=timeline_text_width(units,unit);
        /* A narrow scientific value wraps by complete text runs instead of
         * shrinking below the selected text size or dropping the unit. */
        if(w>card.w) {
            line=33.35f*f*laboratory_wrapped_lines(large,number,card.w);
            value.h=line;laboratory_wrapped_text(canvas,large,number,value,33.35f*f,UI_COLOR(text));
        } else {value.w=w;timeline_spaced_text(canvas,large,value.x,value.y+(line-large->height)/2,number,-.7f*f,UI_COLOR(text));}
        suffix=nk_rect(w+6*f+u<=card.w?card.x+w+6*f:card.x,w+6*f+u<=card.w?y+line-15.95f*f:y+line,u,15.95f*f);
        if(w+6*f+u<=card.w && large->query && units->query) {
            struct nk_user_font_glyph big={0},small_digit={0};
            large->query(large->userdata,large->height,&big,'0',0);units->query(units->userdata,units->height,&small_digit,'0',0);
            float baseline=y+(line-large->height)/2+big.offset.y+big.height;
            suffix.y=baseline-small_digit.offset.y-small_digit.height-(15.95f*f-units->height)/2;
        }
        if(suffix.w>card.w)suffix.w=card.w;
        suffix.h=15.95f*f*laboratory_wrapped_lines(units,unit,suffix.w);
        laboratory_wrapped_text(canvas,units,unit,suffix,15.95f*f,UI_COLOR(muted));
    } else {
        value.h=15.95f*f*laboratory_wrapped_lines(units,number,card.w);
        laboratory_wrapped_text(canvas,units,number,value,15.95f*f,UI_COLOR(muted));
    }
    struct nk_rect rule=nk_rect(card.x,card.y+card.h-2*f,card.w,2*f);struct nk_color color=laboratory_curve_color(slot);color.a=179;
    /* CSS clamps a capsule radius to half its height. This Nuklear snapshot
     * only clamps to the full height, which makes a nonconvex thin rectangle. */
    nk_fill_rect(canvas,rule,rule.h*.5f,color);
    a->laboratory_metric_cards[slot]=card;a->laboratory_metric_labels[slot]=name;a->laboratory_metric_numbers[slot]=value;a->laboratory_metric_units[slot]=suffix;a->laboratory_metric_lines[slot]=rule;
    snprintf(a->laboratory_metric_number_text[slot],96,"%s",number);snprintf(a->laboratory_metric_unit_text[slot],96,"%s",unit);
    ps_ui_accessibility_label(ui,card,exact);
    struct nk_rect visible=plot_intersection(card,canvas->clip);if(visible.w>0 && visible.h>0 && nk_input_is_mouse_hovering_rect(&ui->input,visible))nk_tooltip(ui,exact);
}
typedef struct {app *a;unsigned slot;bool chart;const char *name,*unit,*caption;} laboratory_channel_paint;
typedef struct {struct nk_rect name,unit;float height;} laboratory_channel_layout;
static laboratory_channel_layout laboratory_channel_measure(app *a,const char *name,const char *unit,struct nk_rect r) {
    float f=a->preferences.ui_size/16.f,line=14.5f*f;const struct nk_user_font *font=&a->workspace_status_font;
    float n=timeline_text_width(font,name),u=timeline_text_width(font,unit);bool stacked=n+u+20*f>r.w;
    float nw=stacked?fmaxf(1,r.w):n,uw=fminf(u,r.w);
    float nh=line*laboratory_wrapped_lines(font,name,nw),uh=line*laboratory_wrapped_lines(font,unit,fmaxf(1,uw));
    laboratory_channel_layout m={.name=nk_rect(r.x,r.y,nw,nh),.unit=nk_rect(r.x+r.w-uw,r.y+(stacked?nh+2*f:0),uw,uh)};
    m.height=(stacked?nh+2*f+uh:fmaxf(nh,uh))+9*f;
    return m;
}
static void laboratory_channel_field(struct nk_context *ui,struct nk_rect r,bool focused,void *user) {
    laboratory_channel_paint *p=user;app *a=p->a;float f=a->preferences.ui_size/16.f;struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    if(!p->chart && nk_input_is_mouse_hovering_rect(&ui->input,r))nk_fill_rect(canvas,r,5*f,UI_COLOR(surface));
    if(p->chart) {
        laboratory_channel_layout m=laboratory_channel_measure(a,p->name,p->unit,r);
        laboratory_wrapped_text(canvas,&a->workspace_status_font,p->name,m.name,14.5f*f,UI_COLOR(text));
        laboratory_wrapped_text(canvas,&a->workspace_status_font,p->unit,m.unit,14.5f*f,UI_COLOR(muted));
        a->laboratory_channel_fields[p->slot]=r;a->laboratory_channel_visible[p->slot]=plot_intersection(r,canvas->clip);a->laboratory_channel_names[p->slot]=m.name;a->laboratory_channel_units[p->slot]=m.unit;
        snprintf(a->laboratory_channel_name_text[p->slot],48,"%s",p->name);snprintf(a->laboratory_channel_unit_text[p->slot],64,"%s",p->unit);
    } else {
        laboratory_wrapped_text(canvas,&a->workspace_status_font,p->caption,nk_rect(r.x+4*f,r.y+4*f,r.w-24*f,r.h-8*f),14.5f*f,UI_COLOR(text));
        float x=r.x+r.w-10*f,y=r.y+r.h/2;
        nk_stroke_line(canvas,x-3*f,y-2*f,x,y+1*f,f,UI_COLOR(muted));nk_stroke_line(canvas,x,y+1*f,x+3*f,y-2*f,f,UI_COLOR(muted));
    }
    if(focused)laboratory_focus_outline(a,r,10);
}
static float laboratory_curve_note(app *a,const double *times,const double *values,int count,bool represented,float width,char note[180]) {
    char offsets[120]="";
    if(count>=2 && times[count-1]>times[0]) {
        double low=INFINITY,high=-INFINITY;for(int i=0;i<count;i++)if(isfinite(values[i])){low=fmin(low,values[i]);high=fmax(high,values[i]);}
        if(isfinite(low)){plot_expand_range(&low,&high,true);double offset=ps_plot_axis_offset(low,high),toffset=ps_plot_axis_offset(times[0],times[count-1]);
            if(offset || toffset)snprintf(offsets,120,"Offset: Zeit %.17g s · Wert %.17g · addieren",toffset,offset);}
    }
    snprintf(note,180,"%s%s%s",represented?"":"Anzeigeeinheit nicht darstellbar.",!represented && *offsets?" ":"",offsets);
    if(!*note)return 0;
    float f=a->preferences.ui_size/16.f;return 14.5f*f*laboratory_wrapped_lines(&a->workspace_status_font,note,width)+8*f;
}
static float laboratory_curve_minimum(app *a,const char *name,const char *unit,const double *times,const double *values,int count,bool represented,float width) {
    char note[180];float f=a->preferences.ui_size/16.f;
    float header=laboratory_channel_measure(a,name,unit,nk_rect(0,0,width,0)).height;
    return fmaxf(156*f,header+96*f+laboratory_curve_note(a,times,values,count,represented,width,note));
}
typedef struct {ps_display_unit unit;double values[PREVIEW];int selected;bool represented;} laboratory_curve_data;
static void laboratory_curve_prepare(app *a,const ps_channel *channels,uint32_t count,unsigned slot,laboratory_curve_data *data) {
    data->selected=-1;data->represented=true;data->unit=(ps_display_unit){.scale=1,.symbol="SI"};
    if(!count)return;
    int selected=a->laboratory_channels[slot];if(selected<0 || (unsigned)selected>=count)selected=slot<count?(int)slot:0;
    data->selected=selected;data->unit=channel_display_unit(a,&channels[selected]);
    data->represented=channel_display_values(&data->unit,a->history_v[selected],a->history_count,data->values);
    int mask=a->channel_status[selected];if(mask>=0)for(int i=0;i<a->history_count;i++)if(a->history_v[mask][i]!=1)data->values[i]=NAN;
}
static void laboratory_curve(app *a,const double *times,const double *values,int count,bool represented,float height,unsigned slot) {
    struct nk_context *ui=a->ui;float f=a->preferences.ui_size/16.f;const struct nk_user_font *font=&a->workspace_status_font;
    ps_ui_row_exact_dynamic(ui,height,1);struct nk_rect r;nk_widget(&r,ui);struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    a->laboratory_curve_frames[slot]=r;a->laboratory_curve_notes[slot]=a->laboratory_curve_messages[slot]=a->laboratory_curve_areas[slot]=nk_rect(0,0,0,0);a->laboratory_curve_message_text[slot][0]=0;
    double low=INFINITY,high=-INFINITY;for(int i=0;i<count;i++)if(isfinite(values[i])){low=fmin(low,values[i]);high=fmax(high,values[i]);}
    if(count<2 || !isfinite(low) || !(times[count-1]>times[0])) {
        const char *message=!represented?"Anzeigeeinheit nicht darstellbar.":count<2?"Für eine Kurve sind mindestens zwei gültige Messpunkte nötig.":!isfinite(low)?"Keine gültigen Messwerte in dieser Vorschau.":"Die Vorschau enthält keine wachsende Zeitspanne.";
        struct nk_rect text=nk_rect(r.x,r.y,r.w,14.5f*f*laboratory_wrapped_lines(font,message,r.w));
        laboratory_wrapped_text(canvas,font,message,text,14.5f*f,UI_COLOR(muted));ps_ui_accessibility_label(ui,text,message);
        a->laboratory_curve_messages[slot]=text;snprintf(a->laboratory_curve_message_text[slot],160,"%s",message);return;
    }
    plot_expand_range(&low,&high,true);double bounds[]={times[0],times[count-1],low,high};
    double offset=ps_plot_axis_offset(low,high),toffset=ps_plot_axis_offset(times[0],times[count-1]);
    char ticks[3][80];float left=0;
    for(unsigned i=0;i<3;i++) {plot_axis_label(ticks[i],80,plot_tick(low-offset,high-offset,1-i*.5),low-offset,high-offset);timeline_decimal(ticks[i]);left=fmaxf(left,timeline_text_width(font,ticks[i]));}
    char note_text[180];float note=laboratory_curve_note(a,times,values,count,represented,r.w,note_text);
    if(note){a->laboratory_curve_notes[slot]=nk_rect(r.x,r.y,r.w,note-8*f);laboratory_wrapped_text(canvas,font,note_text,a->laboratory_curve_notes[slot],14.5f*f,UI_COLOR(muted));}
    struct nk_rect area=nk_rect(r.x+left+8*f,r.y+note+8*f,fmaxf(1,r.w-left-12*f),fmaxf(1,r.h-note-47*f));
    a->laboratory_curve_areas[slot]=area;
    for(unsigned i=0;i<3;i++) {
        float y=area.y+area.h*i/2;nk_stroke_line(canvas,area.x,y,area.x+area.w,y,f,UI_COLOR(border));
        timeline_text(canvas,font,nk_rect(r.x,y-7.25f*f,left,14.5f*f),ticks[i],UI_COLOR(muted));
    }
    struct nk_rect clip=canvas->clip;nk_push_scissor(canvas,plot_intersection(clip,area));
    ps_plot_view fit;ps_plot_view_reset(&fit);
    for(int i=1;i<count;i++)if(isfinite(values[i-1]) && isfinite(values[i])) {
        double x=plot_fraction(&fit,bounds,0,times[i-1]),y=plot_fraction(&fit,bounds,1,values[i-1]),xx=plot_fraction(&fit,bounds,0,times[i]),yy=plot_fraction(&fit,bounds,1,values[i]);
        if(ps_plot_clip_line(&x,&y,&xx,&yy))nk_stroke_line(canvas,area.x+(float)x*area.w,area.y+(1-(float)y)*area.h,area.x+(float)xx*area.w,area.y+(1-(float)yy)*area.h,2*f,laboratory_curve_color(slot));
    }
    double t=ps_report_axis_fraction(display_time(a),times[0],times[count-1]);
    if(t>=0 && t<=1)for(float y=area.y;y<area.y+area.h;y+=6*f)nk_stroke_line(canvas,area.x+(float)t*area.w,y,area.x+(float)t*area.w,fminf(y+3*f,area.y+area.h),f,UI_COLOR(muted));
    nk_push_scissor(canvas,clip);
    char xticks[3][80];float widths[3];
    for(unsigned i=0;i<3;i++){snprintf(xticks[i],80,i==2?"%.4g s":"%.4g",plot_tick(times[0]-toffset,times[count-1]-toffset,i*.5));timeline_decimal(xticks[i]);widths[i]=timeline_text_width(font,xticks[i]);}
    bool middle=widths[0]+widths[1]/2+4*f<=area.w/2 && widths[2]+widths[1]/2+4*f<=area.w/2;
    for(unsigned i=0;i<3;i++) {
        if(i==1 && !middle)continue;
        float x=i==0?area.x:i==2?area.x+area.w-widths[i]:area.x+area.w/2-widths[i]/2;
        timeline_text(canvas,font,nk_rect(x,area.y+area.h+6*f,widths[i],14.5f*f),xticks[i],UI_COLOR(muted));
    }
    char preview[96];snprintf(preview,sizeof preview,"Vorschau · %d Punkte%s",count,a->history_stride>1?" · ausgedünnt":"");
    timeline_text(canvas,&a->laboratory_scene_font,nk_rect(r.x,r.y+r.h-13.05f*f,r.w,13.05f*f),preview,UI_COLOR(muted));
    ps_ui_accessibility_label(ui,r,preview);
}
