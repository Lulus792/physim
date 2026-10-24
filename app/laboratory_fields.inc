/* B data-field: caption, real editor, optional action, hint in one scroll flow. */
typedef struct {struct nk_rect card,label,unit,input,action,hint;nk_flags edit;bool changed,clicked;} laboratory_field;
static struct nk_color laboratory_field_color(app *a,struct nk_color c) {
    if(a->ui->current->widgets_disabled){struct nk_color bg=UI_COLOR(window);c.r=(nk_byte)((c.r+bg.r)/2);c.g=(nk_byte)((c.g+bg.g)/2);c.b=(nk_byte)((c.b+bg.b)/2);}
    return c;
}
static struct nk_color laboratory_standard_color(app *a,struct nk_color c) {
    /* CSS fieldset opacity .5 and button:disabled opacity .45 compose. */
    if(a->ui->current->widgets_disabled){struct nk_color bg=UI_COLOR(window);c.r=(nk_byte)roundf(bg.r+(c.r-bg.r)*.225f);c.g=(nk_byte)roundf(bg.g+(c.g-bg.g)*.225f);c.b=(nk_byte)roundf(bg.b+(c.b-bg.b)*.225f);}
    return c;
}
static void laboratory_decimal(char *text){for(char *p=text;*p;p++)if(*p=='.')*p=',';}
static void laboratory_number_text(char *text,int capacity) {
    double value;if(!ps_parse_input_number(text,&value))return;
    /* Short display without changing even a single stored floating-point bit. */
    char candidate[64],best[64]={0};
    for(int digits=1;digits<=17;digits++){
        double parsed;snprintf(candidate,sizeof candidate,"%.*g",digits,value);
        if(ps_parse_finite_number(candidate,NULL,&parsed) && parsed==value && !!signbit(parsed)==!!signbit(value)){
            if(!*best || strlen(candidate)<strlen(best) || (strlen(candidate)==strlen(best) && strchr(best,'e') && !strchr(candidate,'e')))snprintf(best,sizeof best,"%s",candidate);
        }
    }
    if(*best){snprintf(text,(size_t)capacity,"%s",best);laboratory_decimal(text);}
}
static void laboratory_field_message(app *a,const char *text,bool error) {
    if(!*text)return;
    struct nk_context *ui=a->ui;float f=a->preferences.ui_size/16.f,w=nk_window_get_content_region(ui).w;
    float line=16.5f*f,height=line*laboratory_wrapped_lines(&a->workspace_status_font,text,w);
    ps_ui_row_exact_dynamic(ui,height+14*f,1);struct nk_rect r;nk_widget(&r,ui);r.y+=6*f;r.h=height;
    laboratory_wrapped_text(nk_window_get_canvas(ui),&a->workspace_status_font,text,r,line,laboratory_field_color(a,error?UI_COLOR(error):UI_COLOR(muted)));ps_ui_accessibility_label(ui,r,text);
}
static laboratory_field laboratory_edit_field(app *a,const char *label,const char *unit,const char *hint,char *text,int capacity,nk_plugin_filter filter,bool numeric,bool standard) {
    struct nk_context *ui=a->ui;float f=a->preferences.ui_size/16.f,width=nk_window_get_content_region(ui).w,inner=fmaxf(1,width-26*f);
    const struct nk_user_font *caption=&a->laboratory_caption_font,*small=&a->workspace_status_font;
    float label_width=caption->width(caption->userdata,caption->height,label,(int)strlen(label));
    float unit_width=small->width(small->userdata,small->height,unit,(int)strlen(unit));
    bool unit_below=*unit && label_width+4*f+unit_width>inner;
    float label_height=15.95f*f*laboratory_wrapped_lines(caption,label,inner);
    float unit_height=*unit?14.5f*f*laboratory_wrapped_lines(small,unit,inner):0;
    float caption_height=unit_below?label_height+2*f+unit_height:fmaxf(label_height,unit_height);
    float action_width=small->width(small->userdata,small->height,"Standard",8)+16*f;
    bool action_below=standard && inner-action_width-5*f<70*f;
    float hint_height=*hint?16.5f*f*laboratory_wrapped_lines(small,hint,inner):0;
    float content_height=caption_height+5*f+35*f+(action_below?29.5f*f:0)+(hint_height?7*f+hint_height:0);
    laboratory_field field={0};nk_layout_space_begin(ui,NK_STATIC,24*f+content_height,standard?2:1);
    struct nk_vec2 origin=nk_layout_space_to_screen(ui,nk_vec2(0,0));field.card=nk_rect(origin.x,origin.y,width,24*f+content_height);
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    nk_fill_rect(canvas,field.card,8*f,UI_COLOR(window));nk_stroke_rect(canvas,nk_rect(field.card.x+f/2,field.card.y+f/2,field.card.w-f,field.card.h-f),7.5f*f,f,laboratory_field_color(a,UI_COLOR(border)));
    field.label=nk_rect(origin.x+13*f,origin.y+12*f,inner,label_height);
    field.unit=nk_rect(unit_below?origin.x+13*f:origin.x+13*f+label_width+4*f,unit_below?field.label.y+label_height+2*f:field.label.y+(15.95f-14.5f)*f,unit_below?inner:unit_width,unit_height);
    field.input=nk_rect(origin.x+13*f,field.label.y+caption_height+5*f,standard && !action_below?inner-action_width-5*f:inner,35*f);
    char name[256];snprintf(name,sizeof name,"%s%s%s%s",label,*unit?" [":"",unit,*unit?"]":"");ps_ui_accessibility_label(ui,nk_rect(field.label.x,field.label.y,inner,caption_height),name);
    char before[PS_STEP_BREAKPOINT_TEXT];snprintf(before,sizeof before,"%s",text);
    nk_layout_space_push(ui,nk_rect(field.input.x-origin.x,field.input.y-origin.y,field.input.w,field.input.h));
    nk_style_push_font(ui,&a->laboratory_model_font);nk_style_push_float(ui,&ui->style.edit.border,0);nk_style_push_float(ui,&ui->style.edit.color_factor,1);
    nk_style_push_vec2(ui,&ui->style.edit.padding,nk_vec2(0,3*f));
    nk_style_push_style_item(ui,&ui->style.edit.normal,nk_style_item_color(nk_rgba(0,0,0,0)));nk_style_push_style_item(ui,&ui->style.edit.hover,nk_style_item_color(nk_rgba(0,0,0,0)));nk_style_push_style_item(ui,&ui->style.edit.active,nk_style_item_color(nk_rgba(0,0,0,0)));
    struct nk_color ink=laboratory_field_color(a,UI_COLOR(text));nk_style_push_color(ui,&ui->style.edit.text_normal,ink);nk_style_push_color(ui,&ui->style.edit.text_hover,ink);nk_style_push_color(ui,&ui->style.edit.text_active,ink);
    field.edit=nk_sdl_edit_named(ui,name,NK_EDIT_FIELD,text,capacity,filter);
    nk_style_pop_color(ui);nk_style_pop_color(ui);nk_style_pop_color(ui);nk_style_pop_style_item(ui);nk_style_pop_style_item(ui);nk_style_pop_style_item(ui);nk_style_pop_vec2(ui);nk_style_pop_float(ui);nk_style_pop_float(ui);nk_style_pop_font(ui);
    field.changed=strcmp(before,text)!=0;
    if(numeric && !(field.edit&NK_EDIT_ACTIVE))laboratory_number_text(text,capacity);
    bool focused=!ui->current->widgets_disabled && (field.edit&NK_EDIT_ACTIVE);
    laboratory_wrapped_text(canvas,caption,label,field.label,15.95f*f,laboratory_field_color(a,focused?UI_COLOR(accent):UI_COLOR(muted)));
    if(*unit)laboratory_wrapped_text(canvas,small,unit,field.unit,14.5f*f,laboratory_field_color(a,UI_COLOR(muted)));
    if(focused)laboratory_focus_outline(a,field.input,9);
    if(standard) {
        field.action=nk_rect(origin.x+13*f+inner-action_width,action_below?field.input.y+field.input.h+5*f:field.input.y+5.25f*f,action_width,24.5f*f);
        nk_layout_space_push(ui,nk_rect(field.action.x-origin.x,field.action.y-origin.y,field.action.w,field.action.h));struct nk_rect button;nk_widget(&button,ui);
        char control[288];snprintf(control,sizeof control,"Standard: %s",name);bool focus=false;field.clicked=ps_ui_button_at_custom_focus(ui,button,control,false,&focus);
        bool hover=!ui->current->widgets_disabled && nk_input_is_mouse_hovering_rect(&ui->input,button);
        nk_fill_rect(canvas,button,6*f,laboratory_standard_color(a,hover?UI_COLOR(selection_active):UI_COLOR(surface)));
        float text_width=action_width-16*f;laboratory_wrapped_text(canvas,small,"Standard",nk_rect(button.x+(button.w-text_width)/2,button.y+5*f,text_width+1,14.5f*f),14.5f*f,laboratory_standard_color(a,UI_COLOR(text)));
        if(focus)laboratory_focus_outline(a,button,9);
    }
    if(hint_height){field.hint=nk_rect(origin.x+13*f,origin.y+12*f+content_height-hint_height-2*f,inner,hint_height);laboratory_wrapped_text(canvas,small,hint,field.hint,16.5f*f,laboratory_field_color(a,UI_COLOR(muted)));ps_ui_accessibility_label(ui,field.hint,hint);}
    nk_layout_space_end(ui);ps_ui_row_exact_dynamic(ui,10*f,1);nk_spacer(ui);return field;
}
static void laboratory_run_timing(app *a,unsigned slot,unsigned which,const char *label,double *value) {
    char *text=a->adaptive_limit_text[which];double *shown=&a->adaptive_limit_displayed[which];
    if(*shown!=*value && !(isnan(*shown) && isnan(*value))){snprintf(text,64,"%.9g",*value);*shown=*value;}
    laboratory_field field=laboratory_edit_field(a,label,"s","",text,64,nk_filter_default,true,false);
    a->run_field_cards[slot]=field.card;a->run_field_labels[slot]=field.label;a->run_field_inputs[slot]=a->adaptive_limit_bounds[which]=field.input;
    if(field.changed){double parsed;*value=ps_parse_input_number(text,&parsed)?parsed:NAN;*shown=*value;a->project_settings_dirty=true;}
}
static void laboratory_run_settings(app *a) {
    struct nk_context *ui=a->ui;a->breakpoint_input_active=false;
    memset(a->run_field_cards,0,sizeof a->run_field_cards);memset(a->run_field_labels,0,sizeof a->run_field_labels);memset(a->run_field_inputs,0,sizeof a->run_field_inputs);
    if(!workspace_section_begin(a,"Laufeinstellungen",&a->steps_disclosure))return;
    nk_begin_disabled(ui,a->runner.running || a->reset_pending);
    bool previous_adaptive=a->adaptive_steps;
    laboratory_run_timing(a,0,2,"Zeitschritt",&a->dt);
    laboratory_checkbox(a,"Adaptive Schritte",&a->adaptive_steps,0,&a->adaptive_bounds);
    ps_ui_row_exact_dynamic(ui,12*a->preferences.ui_size/16.f,1);nk_spacer(ui);
    if(a->adaptive_steps){laboratory_field_message(a,"Das Modell muss adaptive Schritte unterstützen. Δt ist der Startschritt.",false);laboratory_run_timing(a,1,0,"Minimum",&a->minimum_dt);laboratory_run_timing(a,2,1,"Maximum",&a->maximum_dt);}
    laboratory_field seed=laboratory_edit_field(a,"Zufallsseed","","",a->seed,sizeof a->seed,nk_filter_decimal,false,false);
    a->run_field_cards[3]=seed.card;a->run_field_labels[3]=seed.label;a->run_field_inputs[3]=seed.input;
    laboratory_field points=laboratory_edit_field(a,"Haltepunkte","Schritte","Bis zu 32 verschiedene Schrittzahlen. Leer = aus.",a->step_breakpoints,sizeof a->step_breakpoints,nk_filter_default,false,false);
    a->run_field_cards[4]=points.card;a->run_field_labels[4]=points.label;a->run_field_inputs[4]=a->breakpoint_bounds=points.input;a->breakpoint_input_active=(points.edit&NK_EDIT_ACTIVE)!=0;
    if(seed.changed || points.changed || previous_adaptive!=a->adaptive_steps)a->project_settings_dirty=true;
    nk_end_disabled(ui);
    ps_project_settings bounds={.timestep=a->dt,.adaptive=a->adaptive_steps,.minimum_timestep=a->minimum_dt,.maximum_timestep=a->maximum_dt};
    if(!ps_project_timestep_valid(a->dt) || !ps_project_step_bounds_valid(&bounds))laboratory_field_message(a,a->adaptive_steps?"Positive endliche Zahlen wählen: Minimum ≤ Δt ≤ Maximum ≤ 1 s.":"Zeitschritt: endliche Zahl zwischen 2,225e-308 und 1 s eingeben.",true);
    uint64_t parsed;ps_step_breakpoints breakpoints;
    if(!ps_project_seed_parse(a->seed,&parsed))laboratory_field_message(a,"Seed: ganze Zahl von 0 bis 18446744073709551615 eingeben.",true);
    if(!ps_step_breakpoints_parse(a->step_breakpoints,&breakpoints))laboratory_field_message(a,"Haltepunkte: bis zu 32 verschiedene ganze Zahlen von 0 bis 1000000000 eingeben.",true);
    laboratory_field_message(a,"Die Integration bestimmt das Modell. Diese Einstellungen gelten für neue Läufe; während eines Laufs sind sie gesperrt.",false);
    workspace_section_end(a);
}
