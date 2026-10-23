/* Inspector summaries follow the final inspector-variants.css cascade.
 * Content uses ordinary rows, without the stock tree's additional indentation. */
static unsigned laboratory_section_slot(app *a,const enum nk_collapse_states *state) {
    if(state==&a->laboratory_groups[0])return 0;
    if(state==&a->model_disclosure)return 1;
    if(state==&a->parameter_disclosure)return 2;
    if(state==&a->steps_disclosure)return 3;
    if(state==&a->limits_disclosure)return 4;
    for(unsigned i=1;i<5;i++)if(state==&a->laboratory_groups[i])return i+4;
    return 9;
}
static void laboratory_section_metadata(app *a,unsigned slot,char text[64]) {
    const char *fixed[]={"Auswahl","Quellcode","","","Job","Anzeige","","Ansicht",""};
    snprintf(text,64,"%s",slot<9?fixed[slot]:"");
    if(slot==2)snprintf(text,64,"%u",a->parameters.count);
    if(slot==3 && a->adaptive_steps)snprintf(text,64,"Adaptiv");
    if(slot==6)snprintf(text,64,"%u",a->live_event_count);
    if(slot==8)snprintf(text,64,"%s",*a->last_run?"Messlauf":"Kein Lauf");
}
static unsigned laboratory_wrapped_lines(const struct nk_user_font *font,const char *text,float width) {
    int at=0,length=(int)strlen(text);unsigned lines=0;
    while(at<length) {
        int count=ps_ui_text_line(font,text+at,length-at,fmaxf(1,width),false);
        if(count<=0){nk_rune rune;count=nk_utf_decode(text+at,&rune,length-at);}
        if(count<=0)break;
        at+=count;while(at<length && text[at]==' ')at++;lines++;
    }
    return lines?lines:1;
}
static void laboratory_wrapped_text(struct nk_command_buffer *canvas,const struct nk_user_font *font,const char *text,struct nk_rect r,float line_height,struct nk_color color) {
    int at=0,length=(int)strlen(text);float y=r.y;
    while(at<length) {
        int count=ps_ui_text_line(font,text+at,length-at,fmaxf(1,r.w),false);
        if(count<=0){nk_rune rune;count=nk_utf_decode(text+at,&rune,length-at);}
        if(count<=0)break;
        nk_draw_text(canvas,nk_rect(r.x,y+(line_height-font->height)/2,r.w+1,font->height),text+at,count,font,nk_rgba(0,0,0,0),color);
        at+=count;while(at<length && text[at]==' ')at++;y+=line_height;
    }
}
static bool workspace_section_begin(app *a,const char *label,enum nk_collapse_states *state) {
    struct nk_context *ui=a->ui;
    if(!laboratory_visible(a))return nk_tree_state_push(ui,NK_TREE_TAB,label,state)!=0;
    float f=a->preferences.ui_size/16.f;unsigned slot=laboratory_section_slot(a,state);
    const struct nk_user_font *font=&a->laboratory_section_font,*small=&a->workspace_status_font;
    char metadata[64],name[192],description[256];laboratory_section_metadata(a,slot,metadata);
    float width=nk_window_get_content_region(ui).w;
    float meta_width=small->width(small->userdata,small->height,metadata,(int)strlen(metadata));
    float title_width=fmaxf(1,width-20*f-(meta_width?meta_width+8*f:0));
    float line_height=18.85f*f;unsigned lines=laboratory_wrapped_lines(font,label,title_width);
    float height=fmaxf(42*f,20*f+lines*line_height);
    nk_layout_row_dynamic(ui,height,1);struct nk_rect row;
    enum nk_widget_layout_states visible=nk_widget(&row,ui);
    if(slot<9)a->laboratory_section_bounds[slot]=row;
    if(visible!=NK_WIDGET_INVALID) {
        struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
        struct nk_rect target=plot_intersection(row,ui->current->layout->clip);bool focused=false;
        snprintf(name,sizeof name,"%s auf-/zuklappen",label);
        if(ps_ui_button_at_custom_focus(ui,target,name,*state==NK_MAXIMIZED,&focused))*state=*state==NK_MAXIMIZED?NK_MINIMIZED:NK_MAXIMIZED;
        bool hover=!(ui->current->layout->flags&NK_WINDOW_ROM) && nk_input_is_mouse_hovering_rect(&ui->input,target);
        nk_fill_rect(canvas,nk_rect(row.x,row.y,row.w,1),0,UI_COLOR(border));
        float x=row.x+6*f,y=row.y+row.h/2;
        nk_stroke_line(canvas,x-4*f,y,x+4*f,y,f,muted());
        if(*state!=NK_MAXIMIZED)nk_stroke_line(canvas,x,y-4*f,x,y+4*f,f,muted());
        laboratory_wrapped_text(canvas,font,label,nk_rect(row.x+20*f,row.y+(row.h-lines*line_height)/2,title_width,lines*line_height),line_height,hover?accent():UI_COLOR(text));
        nk_draw_text(canvas,nk_rect(row.x+row.w-meta_width,row.y+(row.h-small->height)/2,meta_width+1,small->height),metadata,(int)strlen(metadata),small,nk_rgba(0,0,0,0),muted());
        if(focused)nk_stroke_rect(canvas,nk_rect(target.x+2*f,target.y+2*f,fmaxf(0,target.w-4*f),fmaxf(0,target.h-4*f)),4*f,2*f,accent());
        snprintf(description,sizeof description,"%s: %s%s%s",label,*state==NK_MAXIMIZED?"ausgeklappt":"eingeklappt",*metadata?" · ":"",metadata);
        ps_ui_accessibility_label(ui,target,description);
    }
    return *state==NK_MAXIMIZED;
}
static void workspace_section_end(app *a) {
    if(!laboratory_visible(a)){nk_tree_state_pop(a->ui);return;}
    ps_ui_row_exact_dynamic(a->ui,14*a->preferences.ui_size/16.f,1);nk_spacer(a->ui);
}

static void laboratory_panel_heading(app *a,unsigned panel,struct nk_rect r) {
    struct nk_context *ui=a->ui;struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    float f=a->preferences.ui_size/16.f;bool inspector=panel==PS_DOCK_INSPECTOR;
    const struct nk_user_font *font=inspector?&a->laboratory_section_font:&a->workbench_fonts[2],*small=&a->workspace_status_font;
    const char *title=inspector?"Inspektor":"Szenenbaum";char metadata[64];
    if(inspector)snprintf(metadata,sizeof metadata,"Alle Funktionen");else snprintf(metadata,sizeof metadata,"%u Objekte",display_scene(a)->count);
    float raw=small->width(small->userdata,small->height,metadata,(int)strlen(metadata));
    float left=inspector?17:16,available=fmaxf(1,r.w-left-16);
    float title_raw=font->width(font->userdata,font->height,title,(int)strlen(title));
    float title_line=(inspector?18.85f:17.4f)*f,meta_line=14.5f*f;
    if(title_raw+8*f+raw>available) {
        /* At the minimum pane width, large native fonts need two complete
         * lines. Keep both words intact instead of splitting either column. */
        float y=r.y+(r.h-title_line-4*f-meta_line)/2;
        laboratory_wrapped_text(canvas,font,title,nk_rect(r.x+left,y,available,title_line),title_line,UI_COLOR(text));
        laboratory_wrapped_text(canvas,small,metadata,nk_rect(r.x+left,y+title_line+4*f,available,meta_line),meta_line,muted());
    } else {
        laboratory_wrapped_text(canvas,font,title,nk_rect(r.x+left,r.y+(r.h-title_line)/2,available-raw-8*f,title_line),title_line,UI_COLOR(text));
        laboratory_wrapped_text(canvas,small,metadata,nk_rect(r.x+r.w-16-raw,r.y+(r.h-meta_line)/2,raw,meta_line),meta_line,muted());
    }
    char description[128];snprintf(description,sizeof description,"%s · %s",title,metadata);ps_ui_accessibility_label(ui,r,description);
}
static const char laboratory_footer_text[]="Modell · Lauf · Ansicht";
static float laboratory_inspector_footer_height(app *a,float width) {
    float f=a->preferences.ui_size/16.f;unsigned lines=laboratory_wrapped_lines(&a->workspace_status_font,laboratory_footer_text,fmaxf(1,width-32));
    return fmaxf(37*f,21*f+16*f*lines);
}
static void laboratory_inspector_footer(app *a,struct nk_rect r) {
    struct nk_context *ui=a->ui;struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    const struct nk_user_font *font=&a->workspace_status_font;float f=a->preferences.ui_size/16.f;
    const char *text=laboratory_footer_text;
    nk_fill_rect(canvas,nk_rect(r.x,r.y,r.w,1),0,UI_COLOR(border));
    unsigned lines=laboratory_wrapped_lines(font,text,fmaxf(1,r.w-32));
    laboratory_wrapped_text(canvas,font,text,nk_rect(r.x+16,r.y+(r.h-lines*16*f)/2,fmaxf(1,r.w-32),r.h),16*f,muted());
    ps_ui_accessibility_label(ui,r,text);
}
