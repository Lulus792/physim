/* Remaining inspector B content: one scroll flow, real process/log/run state. */
static struct nk_rect laboratory_tail_note(app *a,const char *text,bool error){
    float f=a->preferences.ui_size/16.f,w=nk_window_get_content_region(a->ui).w,line=(error?16:16.5f)*f,top=(error?8:6)*f;
    float height=line*laboratory_wrapped_lines(&a->workspace_status_font,text,w);
    ps_ui_row_exact_dynamic(a->ui,top+height+(error?0:10*f),1);struct nk_rect r;nk_widget(&r,a->ui);r.y+=top;r.h=height;
    laboratory_wrapped_text(nk_window_get_canvas(a->ui),&a->workspace_status_font,text,r,line,error?UI_COLOR(error):UI_COLOR(muted));ps_ui_accessibility_label(a->ui,r,text);
    return r;
}
static void laboratory_limits(app *a){
    memset(a->laboratory_limit_cards,0,sizeof a->laboratory_limit_cards);memset(a->laboratory_limit_labels,0,sizeof a->laboratory_limit_labels);
    if(!workspace_section_begin(a,"Laufgrenzen",&a->limits_disclosure))return;
    nk_begin_disabled(a->ui,!idle(a));
    for(unsigned i=0;i<2;i++){
        double actual=i?a->runner_wall_seconds:(double)a->runner_memory_mib;
        if(!a->runner_limit_ready[i] || a->runner_limit_shown[i]!=actual){snprintf(a->runner_limit_text[i],64,"%.17g",actual);a->runner_limit_shown[i]=actual;a->runner_limit_ready[i]=true;a->runner_limit_invalid[i]=false;}
        laboratory_field field=laboratory_edit_field(a,i?"Zeitlimit":"Speicherlimit",i?"s":"MiB","",a->runner_limit_text[i],64,nk_filter_default,true,false);
        a->laboratory_limit_cards[i]=field.card;a->laboratory_limit_labels[i]=field.label;a->limit_bounds[i]=field.input;
        if(field.changed){double number;bool valid=ps_parse_input_number(a->runner_limit_text[i],&number) && number>=0 && number<=(i?86400:16384) && (i || floor(number)==number);
            a->runner_limit_invalid[i]=!valid;if(valid){if(i)a->runner_wall_seconds=number;else a->runner_memory_mib=(int)number;a->runner_limit_shown[i]=number;}}
    }
    nk_end_disabled(a->ui);
    laboratory_tail_note(a,"0 = ohne Grenze. Für neue Simulationen und Auswertungen; Zeit zählt einschließlich Pausen. Serien haben ein eigenes Zeitlimit.",false);
    if(a->runner_limit_invalid[0])laboratory_tail_note(a,"Speicherlimit: ganze Zahl von 0 bis 16384 MiB eingeben.",true);
    if(a->runner_limit_invalid[1])laboratory_tail_note(a,"Zeitlimit: endliche Zahl von 0 bis 86400 s eingeben (Komma oder Punkt).",true);
    workspace_section_end(a);
}
static bool laboratory_same_warning(const ps_log_record *left,const ps_log_record *right){return left->level==right->level && left->time_s==right->time_s && !strcmp(left->message,right->message);}
static bool laboratory_warning_recent(app *a){for(unsigned i=0;i<a->live_event_count;i++)if(laboratory_same_warning(&a->live_warning,&a->live_events[i]))return true;return false;}
static void laboratory_log_message(const ps_log_record *record,char text[4*PS_LOG_MESSAGE_MAX+1]){
    size_t used=0;for(size_t i=0;record->message[i];i++){
        if(record->message[i]=='\t'){memcpy(text+used,"    ",4);used+=4;}
        else if(record->message[i]=='\r'){text[used++]='\n';if(record->message[i+1]=='\n')i++;}
        else text[used++]=record->message[i];
    }text[used]=0;
}
static unsigned laboratory_log_layout(struct nk_command_buffer *canvas,const struct nk_user_font *font,const char *text,struct nk_rect r,float line,struct nk_color color){
    unsigned lines=0;int length=(int)strlen(text),at=0;
    do{int end=at;while(end<length && text[end]!='\n')end++;
        if(end==at)lines++;
        while(at<end){int count=ps_ui_text_line(font,text+at,end-at,fmaxf(1,r.w),false);if(count<=0){nk_rune rune;count=nk_utf_decode(text+at,&rune,end-at);}if(count<=0)break;
            if(canvas)nk_draw_text(canvas,nk_rect(r.x,r.y+lines*line+(line-font->height)/2,r.w+1,font->height),text+at,count,font,nk_rgba(0,0,0,0),color);
            lines++;at+=count;while(at<end && text[at]==' ')at++;
        }if(end==length)break;at=end+1;
    }while(at<=length);return lines;
}
static struct nk_rect laboratory_event_card(app *a,const ps_log_record *record,struct nk_rect *heading,struct nk_rect *message){
    struct nk_context *ui=a->ui;float f=a->preferences.ui_size/16.f,w=nk_window_get_content_region(ui).w,inner=fmaxf(1,w-20*f);
    char time[48],title[96],text[4*PS_LOG_MESSAGE_MAX+1];snprintf(time,sizeof time,"%.6g",record->time_s);laboratory_decimal(time);snprintf(title,sizeof title,"%s · %s s",live_event_level(record->level),time);laboratory_log_message(record,text);
    float title_height=14.5f*f*laboratory_wrapped_lines(&a->workspace_status_font,title,inner),message_height=17.4f*f*laboratory_log_layout(NULL,&a->workbench_fonts[2],text,nk_rect(0,0,inner,0),17.4f*f,UI_COLOR(text));
    ps_ui_row_exact_dynamic(ui,20*f+title_height+3*f+message_height,1);struct nk_rect card;nk_widget(&card,ui);
    *heading=nk_rect(card.x+10*f,card.y+10*f,inner,title_height);*message=nk_rect(card.x+10*f,heading->y+title_height+3*f,inner,message_height);
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);nk_fill_rect(canvas,card,7*f,UI_COLOR(window));
    laboratory_wrapped_text(canvas,&a->workspace_status_font,title,*heading,14.5f*f,record->level>=PS_LOG_WARNING?UI_COLOR(error):UI_COLOR(muted));
    laboratory_log_layout(canvas,&a->workbench_fonts[2],text,*message,17.4f*f,UI_COLOR(text));ps_ui_accessibility_label(ui,*heading,title);ps_ui_accessibility_label(ui,*message,text);
    ps_ui_row_exact_dynamic(ui,7*f,1);nk_spacer(ui);return card;
}
static void laboratory_tail_begin(app *a){
    memset(a->laboratory_event_cards,0,sizeof a->laboratory_event_cards);memset(a->laboratory_event_headings,0,sizeof a->laboratory_event_headings);memset(a->laboratory_event_messages,0,sizeof a->laboratory_event_messages);
    a->live_event_bounds=a->live_warning_bounds=a->laboratory_log_button=nk_rect(0,0,0,0);
    a->laboratory_recording_bounds=a->laboratory_recording_name=a->laboratory_recording_meta=a->laboratory_recording_dot=nk_rect(0,0,0,0);
}
static void laboratory_warning(app *a){
    if(!a->live_warning_valid || (a->laboratory_groups[2]==NK_MAXIMIZED && laboratory_warning_recent(a)))return;
    struct nk_rect heading,message;a->live_warning_bounds=laboratory_event_card(a,&a->live_warning,&heading,&message);
}
static void laboratory_events(app *a){
    if(!a->live_event_count)a->live_event_bounds=laboratory_tail_note(a,"Noch keine Experimentmeldungen.",false);
    for(unsigned slot=0;slot<a->live_event_count;slot++){
        const ps_log_record *record=&a->live_events[a->live_event_count-1-slot];
        struct nk_rect card=laboratory_event_card(a,record,&a->laboratory_event_headings[slot],&a->laboratory_event_messages[slot]);a->laboratory_event_cards[slot]=card;
        if(!slot)a->live_event_bounds=card;else a->live_event_bounds.h=card.y+card.h-a->live_event_bounds.y;
        if(a->live_warning_valid && laboratory_same_warning(record,&a->live_warning))a->live_warning_bounds=card;
    }
    if(a->live_event_omitted){char note[128];snprintf(note,sizeof note,"%llu ältere Meldungen in der Protokolldatei.",(unsigned long long)a->live_event_omitted);laboratory_tail_note(a,note,false);}
    float f=a->preferences.ui_size/16.f,w=nk_window_get_content_region(a->ui).w,line=17.4f*f,height=18*f+line*laboratory_wrapped_lines(&a->workbench_fonts[2],"Protokoll öffnen",fmaxf(1,w-18*f));
    ps_ui_row_exact_dynamic(a->ui,10*f,1);nk_spacer(a->ui);ps_ui_row_exact_dynamic(a->ui,height,1);struct nk_rect r;nk_widget(&r,a->ui);a->laboratory_log_button=r;bool focus=false;
    if(ps_ui_button_at_custom_focus(a->ui,r,"Protokoll öffnen",false,&focus)){a->show_log=true;dock_show(a,PS_DOCK_LOG);}
    bool hover=nk_input_is_mouse_hovering_rect(&a->ui->input,r);struct nk_command_buffer *canvas=nk_window_get_canvas(a->ui);nk_fill_rect(canvas,r,6*f,hover?UI_COLOR(selection_active):UI_COLOR(surface));
    const char *label="Protokoll öffnen";int at=0,length=(int)strlen(label);float y=r.y+9*f;
    while(at<length){int count=ps_ui_text_line(&a->workbench_fonts[2],label+at,length-at,fmaxf(1,r.w-18*f),false);if(count<=0)break;float width=a->workbench_fonts[2].width(a->workbench_fonts[2].userdata,a->workbench_fonts[2].height,label+at,count);
        nk_draw_text(canvas,nk_rect(r.x+(r.w-width)/2,y+(line-a->workbench_fonts[2].height)/2,width+1,a->workbench_fonts[2].height),label+at,count,&a->workbench_fonts[2],nk_rgba(0,0,0,0),UI_COLOR(text));y+=line;at+=count;while(at<length && label[at]==' ')at++;}
    if(focus)laboratory_focus_outline(a,r,9);
    laboratory_tail_note(a,"Warnungen und Fehler bleiben auch bei geschlossener Gruppe in der Statuszeile erkennbar.",false);
}
static const char *laboratory_recording_file(const char *path){const char *name=path;for(const char *p=path;*p;p++)if(*p=='/' || *p=='\\')name=p+1;return name;}
static void laboratory_recording(app *a){
    float f=a->preferences.ui_size/16.f,w=nk_window_get_content_region(a->ui).w,inset=*a->last_run?15*f:0,inner=fmaxf(1,w-inset);
    const char *name=*a->last_run?laboratory_recording_file(a->last_run):"Keine Aufzeichnung",*meta=*a->last_run?(a->timeline_scenes?"Messwerte und Szene":"Messwerte · keine Szene"):"Messwerte werden nach dem Start aufgezeichnet.";
    float name_height=15.95f*f*laboratory_wrapped_lines(&a->laboratory_caption_font,name,inner),meta_height=14.5f*f*laboratory_wrapped_lines(&a->workspace_status_font,meta,inner);
    ps_ui_row_exact_dynamic(a->ui,name_height+4*f+meta_height,1);struct nk_rect r;nk_widget(&r,a->ui);a->laboratory_recording_bounds=r;
    a->laboratory_recording_name=nk_rect(r.x+inset,r.y,inner,name_height);a->laboratory_recording_meta=nk_rect(r.x+inset,r.y+name_height+4*f,inner,meta_height);
    struct nk_command_buffer *canvas=nk_window_get_canvas(a->ui);
    if(*a->last_run){a->laboratory_recording_dot=nk_rect(r.x,r.y+(r.h-6*f)/2,6*f,6*f);nk_fill_circle(canvas,a->laboratory_recording_dot,UI_COLOR(accent));}
    laboratory_wrapped_text(canvas,&a->laboratory_caption_font,name,a->laboratory_recording_name,15.95f*f,*a->last_run?UI_COLOR(text):UI_COLOR(muted));laboratory_wrapped_text(canvas,&a->workspace_status_font,meta,a->laboratory_recording_meta,14.5f*f,UI_COLOR(muted));
    ps_ui_accessibility_label(a->ui,a->laboratory_recording_name,name);ps_ui_accessibility_label(a->ui,a->laboratory_recording_meta,meta);
    if(*a->last_run && nk_input_is_mouse_hovering_rect(&a->ui->input,plot_intersection(r,a->ui->current->layout->clip)))nk_tooltip(a->ui,a->last_run);
}
