/* Measurement typography follows metric-grid; data and conversion stay native. */
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
    struct nk_rect rule=nk_rect(card.x,card.y+card.h-2*f,card.w,2*f);struct nk_color color=ui_palette->curves[slot%8];color.a=179;
    /* CSS clamps a capsule radius to half its height. This Nuklear snapshot
     * only clamps to the full height, which makes a nonconvex thin rectangle. */
    nk_fill_rect(canvas,rule,floorf(rule.h*.5f),color);
    a->laboratory_metric_cards[slot]=card;a->laboratory_metric_labels[slot]=name;a->laboratory_metric_numbers[slot]=value;a->laboratory_metric_units[slot]=suffix;a->laboratory_metric_lines[slot]=rule;
    snprintf(a->laboratory_metric_number_text[slot],96,"%s",number);snprintf(a->laboratory_metric_unit_text[slot],96,"%s",unit);
    ps_ui_accessibility_label(ui,card,exact);
    struct nk_rect visible=plot_intersection(card,canvas->clip);if(visible.w>0 && visible.h>0 && nk_input_is_mouse_hovering_rect(&ui->input,visible))nk_tooltip(ui,exact);
}
static void laboratory_channel_field(struct nk_context *ui,struct nk_rect r,bool focused,void *user) {
    app *a=user;float f=a->preferences.ui_size/16.f;struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    const char *name=a->laboratory_channel_caption;
    if(nk_input_is_mouse_hovering_rect(&ui->input,r))nk_fill_rect(canvas,r,5*f,UI_COLOR(surface));
    laboratory_wrapped_text(canvas,&a->workspace_status_font,name,nk_rect(r.x+4*f,r.y+4*f,r.w-24*f,r.h-8*f),14.5f*f,UI_COLOR(text));
    float x=r.x+r.w-10*f,y=r.y+r.h/2;nk_stroke_line(canvas,x-3*f,y-2*f,x,y+1*f,f,UI_COLOR(muted));nk_stroke_line(canvas,x,y+1*f,x+3*f,y-2*f,f,UI_COLOR(muted));
    if(focused)laboratory_focus_outline(a,r,10);
}
static void laboratory_curve(app *a,const double *times,const double *values,int count,float height,unsigned slot) {
    struct nk_context *ui=a->ui;float f=a->preferences.ui_size/16.f;const struct nk_user_font *font=&a->workspace_status_font;
    nk_layout_row_dynamic(ui,height,1);struct nk_rect r;nk_widget(&r,ui);struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    double low=INFINITY,high=-INFINITY;for(int i=0;i<count;i++)if(isfinite(values[i])){low=fmin(low,values[i]);high=fmax(high,values[i]);}
    if(count<2 || !isfinite(low) || !(times[count-1]>times[0])) {
        const char *message=count<2?"Messkurven erscheinen nach dem ersten Messpunkt.":"Keine gültigen Messwerte in dieser Vorschau.";
        laboratory_wrapped_text(canvas,font,message,r,14.5f*f,UI_COLOR(muted));ps_ui_accessibility_label(ui,r,message);return;
    }
    plot_expand_range(&low,&high,true);double bounds[]={times[0],times[count-1],low,high};
    double offset=ps_plot_axis_offset(low,high),toffset=ps_plot_axis_offset(times[0],times[count-1]);
    char ticks[3][80];float left=0;
    for(unsigned i=0;i<3;i++) {plot_axis_label(ticks[i],80,plot_tick(low-offset,high-offset,1-i*.5),low-offset,high-offset);timeline_decimal(ticks[i]);left=fmaxf(left,timeline_text_width(font,ticks[i]));}
    float note=(offset || toffset)?29*f:0;
    if(note){char text[180];snprintf(text,sizeof text,"Offset: Zeit %.17g s · Wert %.17g · addieren",toffset,offset);laboratory_wrapped_text(canvas,font,text,nk_rect(r.x,r.y,r.w,note),14.5f*f,UI_COLOR(muted));}
    struct nk_rect area=nk_rect(r.x+left+8*f,r.y+note+8*f,fmaxf(1,r.w-left-12*f),fmaxf(1,r.h-note-47*f));
    a->laboratory_curve_areas[slot]=area;
    for(unsigned i=0;i<3;i++) {
        float y=area.y+area.h*i/2;nk_stroke_line(canvas,area.x,y,area.x+area.w,y,f,UI_COLOR(plot_grid));
        timeline_text(canvas,font,nk_rect(r.x,y-7.25f*f,left,14.5f*f),ticks[i],UI_COLOR(muted));
    }
    struct nk_rect clip=canvas->clip;nk_push_scissor(canvas,plot_intersection(clip,area));
    ps_plot_view fit;ps_plot_view_reset(&fit);
    for(int i=1;i<count;i++)if(isfinite(values[i-1]) && isfinite(values[i]))plot_segment(canvas,area,plot_fraction(&fit,bounds,0,times[i-1]),plot_fraction(&fit,bounds,1,values[i-1]),plot_fraction(&fit,bounds,0,times[i]),plot_fraction(&fit,bounds,1,values[i]),ui_palette->curves[slot]);
    double t=ps_report_axis_fraction(display_time(a),times[0],times[count-1]);
    if(t>=0 && t<=1)for(float y=area.y;y<area.y+area.h;y+=6*f)nk_stroke_line(canvas,area.x+(float)t*area.w,y,area.x+(float)t*area.w,fminf(y+3*f,area.y+area.h),f,UI_COLOR(muted));
    nk_push_scissor(canvas,clip);
    for(unsigned i=0;i<3;i++) {
        char text[80];snprintf(text,80,i==2?"%.4g s":"%.4g",plot_tick(times[0]-toffset,times[count-1]-toffset,i*.5));timeline_decimal(text);
        float width=timeline_text_width(font,text),x=i==0?area.x:i==2?area.x+area.w-width:area.x+area.w/2-width/2;
        timeline_text(canvas,font,nk_rect(x,area.y+area.h+6*f,width,14.5f*f),text,UI_COLOR(muted));
    }
    char preview[96];snprintf(preview,sizeof preview,"Vorschau · %d Punkte%s",count,a->history_stride>1?" · ausgedünnt":"");
    timeline_text(canvas,&a->laboratory_scene_font,nk_rect(r.x,r.y+r.h-13.05f*f,r.w,13.05f*f),preview,UI_COLOR(muted));
    ps_ui_accessibility_label(ui,r,preview);
}
