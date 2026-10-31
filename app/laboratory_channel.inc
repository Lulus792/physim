/* Inspector B: native choices inside data-field cards; display never writes SI data. */
typedef struct {app *a;const char *text;bool focused;} laboratory_choice_paint;
static const char *const laboratory_choice_names[]={"Messkanal auswählen","Anzeigeeinheit im Inspektor"};
static void laboratory_choice_field(struct nk_context *ui,struct nk_rect r,bool focused,void *user){
    laboratory_choice_paint *p=user;app *a=p->a;float f=a->preferences.ui_size/16.f;p->focused=focused;
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);struct nk_color color=laboratory_camera_color(a,UI_COLOR(text));
    float width=fmaxf(1,r.w-24*f),line=18.85f*f,h=line*laboratory_wrapped_lines(&a->laboratory_control_font,p->text,width);
    laboratory_wrapped_text(canvas,&a->laboratory_control_font,p->text,nk_rect(r.x,r.y+(r.h-h)/2,width,h),line,color);
    float x=r.x+r.w-8*f,y=r.y+r.h/2;nk_stroke_line(canvas,x-4*f,y-2*f,x,y+2*f,1.25f*f,color);nk_stroke_line(canvas,x,y+2*f,x+4*f,y-2*f,1.25f*f,color);
    if(focused)laboratory_focus_outline(a,r,4);
}
static bool laboratory_choice_begin(app *a,unsigned kind,const char *label,const char *text,bool disabled){
    struct nk_context *ui=a->ui;float f=a->preferences.ui_size/16.f,width=nk_window_get_content_region(ui).w,inner=fmaxf(1,width-26*f);
    float caption=15.95f*f*laboratory_wrapped_lines(&a->laboratory_caption_font,label,inner);
    float control=fmaxf(32*f,10*f+18.85f*f*laboratory_wrapped_lines(&a->laboratory_control_font,text,fmaxf(1,inner-24*f)));
    nk_layout_space_begin(ui,NK_STATIC,24*f+caption+5*f+control,1);struct nk_vec2 origin=nk_layout_space_to_screen(ui,nk_vec2(0,0));
    struct nk_rect card=nk_rect(origin.x,origin.y,width,24*f+caption+5*f+control),name=nk_rect(origin.x+13*f,origin.y+12*f,inner,caption),field=nk_rect(name.x,name.y+caption+5*f,inner,control);
    a->inspector_channel_cards[kind]=card;a->inspector_channel_labels[kind]=name;a->inspector_channel_fields[kind]=field;
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);nk_fill_rect(canvas,card,8*f,UI_COLOR(window));nk_stroke_rect(canvas,nk_rect(card.x+f/2,card.y+f/2,card.w-f,card.h-f),7.5f*f,f,UI_COLOR(border));
    nk_layout_space_push(ui,nk_rect(field.x-origin.x,field.y-origin.y,field.w,field.h));
    nk_begin_disabled(ui,disabled);nk_style_push_font(ui,&a->laboratory_control_font);nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(6*f,6*f));nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,0));
    nk_style_push_flags(ui,&ui->style.button.text_alignment,NK_TEXT_LEFT);nk_style_push_style_item(ui,&ui->style.button.normal,nk_style_item_color(UI_COLOR(window)));
    laboratory_choice_paint paint={a,text,false};bool open=ps_ui_combo_begin_named_painted(ui,laboratory_choice_names[kind],text,nk_vec2(fmaxf(inner,220*f),240*f),laboratory_choice_field,&paint);
    /* The popup owns another command buffer; keep the card label in its parent. */
    laboratory_wrapped_text(canvas,&a->laboratory_caption_font,label,name,15.95f*f,paint.focused?UI_COLOR(accent):UI_COLOR(muted));
    if(open)ps_ui_row_exact_dynamic(ui,30*f,1);return open;
}
static void laboratory_choice_end(app *a,unsigned kind,bool open){
    struct nk_context *ui=a->ui;
    if(open){if(nk_input_is_key_pressed(&ui->input,NK_KEY_TEXT_RESET_MODE)){a->inspector_channel_return[kind]=true;nk_combo_close(ui);}nk_combo_end(ui);}
    nk_style_pop_style_item(ui);nk_style_pop_flags(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);nk_style_pop_font(ui);nk_end_disabled(ui);
    nk_layout_space_end(ui);laboratory_camera_space(a,10);
    if(!open && a->inspector_channel_return[kind] && nk_sdl_accessibility_request_focus(ui,NULL,laboratory_choice_names[kind]))a->inspector_channel_return[kind]=false;
}
static void laboratory_channel_inspector(app *a){
    struct nk_context *ui=a->ui;uint32_t count;const ps_channel *channels=laboratory_channels(a,&count);int *selected=&a->laboratory_channels[0];
    if(*selected<0 || (unsigned)*selected>=count)*selected=0;
    bool open=laboratory_choice_begin(a,0,"Kanal",count?channels[*selected].name:"Noch keine Messkanäle",!count);
    if(open)for(unsigned i=0;i<count;i++)if(ps_ui_fixed_button_label(ui,channels[i].name)){*selected=(int)i;a->inspector_channel_return[0]=true;nk_combo_close(ui);}
    laboratory_choice_end(a,0,open);
    const ps_channel *channel=count?&channels[*selected]:NULL;ps_display_unit unit=channel?channel_display_unit(a,channel):(ps_display_unit){0};
    open=laboratory_choice_begin(a,1,"Anzeigeeinheit",channel?unit.symbol:"Keine Einheit",!channel);
    a->channel_unit_buttons[0]=a->inspector_channel_fields[1];a->inspector_channel_custom=nk_rect(0,0,0,0);
    if(open){
        nk_begin_disabled(ui,!a->channel_units_writable);bool original=false,current=false;
        for(unsigned i=0;i<sizeof channel_unit_presets/sizeof channel_unit_presets[0];i++){
            const channel_unit_preset *p=channel_unit_presets+i;if(!analysis_unit_preset_matches(channel,p))continue;
            ps_display_unit choice={.scale=p->scale};memcpy(choice.dimension,p->dimension,7);snprintf(choice.symbol,sizeof choice.symbol,"%s",p->symbol);
            original|=choice.scale==1 && !strcmp(choice.symbol,channel->unit);current|=choice.scale==unit.scale && !strcmp(choice.symbol,unit.symbol);
            if(ps_ui_fixed_button_label(ui,choice.symbol)){analysis_unit_select(a,channel,&choice);a->inspector_channel_return[1]=true;nk_combo_close(ui);}
        }
        if(!original){const char *symbol=*channel->unit?channel->unit:"SI";
            if(ps_ui_fixed_button_label(ui,symbol)){ps_channel_units next=a->channel_units;if(ps_channel_units_remove(&next,channel)==PS_OK)(void)channel_units_save(a,&next);a->inspector_channel_return[1]=true;nk_combo_close(ui);}
            current|=unit.scale==1 && !strcmp(symbol,unit.symbol);
        }
        if(!current && ps_ui_fixed_button_label(ui,unit.symbol)){a->inspector_channel_return[1]=true;nk_combo_close(ui);}
        nk_end_disabled(ui);a->inspector_channel_custom=nk_widget_bounds(ui);
        if(ps_ui_fixed_button_label(ui,"Benutzerdefiniert …")){channel_unit_open_from(a,channel,a->window,laboratory_choice_names[1]);a->inspector_channel_return[1]=false;nk_combo_close(ui);}
    }
    laboratory_choice_end(a,1,open);
    float f=a->preferences.ui_size/16.f,width=nk_window_get_content_region(ui).w;char value[200]="Noch keine Messwerte",precise[240]="Noch keine Messwerte";
    if(channel){char number[96],symbol[96];laboratory_metric_text(a,channel,(unsigned)*selected,number,symbol,precise);snprintf(value,sizeof value,"%s%s%s",number,*symbol?" ":"",symbol);}
    /* Adjacent block margins collapse to 12 px and then 6 px in inspector.css. */
    laboratory_camera_space(a,2);float line=34.8f*f,height=line*laboratory_wrapped_lines(&a->laboratory_inspector_value_font,value,width);
    ps_ui_row_exact_dynamic(ui,height,1);struct nk_rect r;nk_widget(&r,ui);a->inspector_channel_value=r;
    laboratory_wrapped_text(nk_window_get_canvas(ui),&a->laboratory_inspector_value_font,value,r,line,UI_COLOR(text));
    char accessible[256];snprintf(accessible,sizeof accessible,"Aktueller Messwert: %s",value);ps_ui_accessibility_label(ui,r,accessible);
    if(nk_input_is_mouse_hovering_rect(&ui->input,plot_intersection(r,ui->current->layout->clip)))nk_tooltip(ui,precise);
    const char *hint="Folgt der betrachteten Zeit. Rohdaten und Aufzeichnung bleiben unverändert. Die Kurven steuerst du über „Kurven anzeigen“.";
    laboratory_camera_space(a,6);line=17.05f*f;ps_ui_row_exact_dynamic(ui,line*laboratory_wrapped_lines(&a->laboratory_caption_font,hint,width),1);nk_widget(&r,ui);a->inspector_channel_hint=r;
    laboratory_wrapped_text(nk_window_get_canvas(ui),&a->laboratory_caption_font,hint,r,line,UI_COLOR(muted));ps_ui_accessibility_label(ui,r,hint);laboratory_camera_space(a,10);
    if(*a->channel_units_error)laboratory_tail_note(a,a->channel_units_error,true);
}
