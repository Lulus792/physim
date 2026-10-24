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
    const char *fixed[]={"Auswahl","Quellcode","","","Optional","Live","","Ansicht",""};
    snprintf(text,64,"%s",slot<9?fixed[slot]:"");
    if(slot==2)snprintf(text,64,"%u",a->parameters.count);
    if(slot==3 && a->adaptive_steps)snprintf(text,64,"Adaptiv");
    if(slot==6)snprintf(text,64,"%u",a->live_event_count);
    if(slot==8)snprintf(text,64,"%s",*a->last_run?".psrun":"Kein Lauf");
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
static const char *const laboratory_section_labels[]={"Objekt","Modellwerte","Laufparameter","Laufeinstellungen","Laufgrenzen","Messkanal & Einheit","Ereignisse","Kamera und Darstellung","Aufzeichnung"};
static float laboratory_section_word_width(const struct nk_user_font *font,const char *text) {
    float width=0;const char *word=text;
    for(const char *p=text;;p++)if(!*p || *p==' ') {
        if(p>word)width=fmaxf(width,font->width(font->userdata,font->height,word,(int)(p-word)));
        if(!*p)break;
        word=p+1;
    }
    return width;
}
static float laboratory_inspector_minimum(const app *a) {
    const struct nk_user_font *font=&a->laboratory_section_font;float word=0,f=a->preferences.ui_size/16.f;
    if(!font->width)return 208;
    for(unsigned i=0;i<9;i++)word=fmaxf(word,laboratory_section_word_width(font,laboratory_section_labels[i]));
    /* Existing unscaled panel insets, scaled disclosure column, whole word
     * plus a small glyph-bearing allowance. Stored wish widths stay intact. */
    return fmaxf(208,ceilf(33+22*f+word));
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
    bool stacked=meta_width>0 && laboratory_section_word_width(font,label)>title_width;
    if(stacked)title_width=fmaxf(1,width-20*f);
    float line_height=18.85f*f;unsigned lines=laboratory_wrapped_lines(font,label,title_width);
    float content_height=lines*line_height+(stacked?18.5f*f:0);
    float height=fmaxf(42*f,20*f+content_height);
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
        float title_y=row.y+(row.h-content_height)/2;
        float x=row.x+6*f,y=stacked?title_y+line_height/2:row.y+row.h/2;
        nk_stroke_line(canvas,x-4*f,y,x+4*f,y,f,muted());
        if(*state!=NK_MAXIMIZED)nk_stroke_line(canvas,x,y-4*f,x,y+4*f,f,muted());
        laboratory_wrapped_text(canvas,font,label,nk_rect(row.x+20*f,title_y,title_width,lines*line_height),line_height,hover?accent():UI_COLOR(text));
        nk_draw_text(canvas,nk_rect(row.x+row.w-meta_width,(stacked?title_y+lines*line_height+4*f+(14.5f*f-small->height)/2:row.y+(row.h-small->height)/2),meta_width+1,small->height),metadata,(int)strlen(metadata),small,nk_rgba(0,0,0,0),muted());
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
static void laboratory_focus_outline(app *a,struct nk_rect r,float radius) {
    struct nk_command_buffer *canvas=nk_window_get_canvas(a->ui);float f=a->preferences.ui_size/16.f;
    struct nk_rect clip=canvas->clip,window=nk_window_get_bounds(a->ui);
    /* The content scissor excludes its horizontal padding. The CSS outline
     * belongs in that padding, while the panel's vertical viewport still clips. */
    float left=fmaxf(window.x,clip.x-8*f),right=fminf(window.x+window.w,clip.x+clip.w+8*f);
    nk_push_scissor(canvas,nk_rect(left,clip.y,fmaxf(0,right-left),clip.h));
    nk_stroke_rect(canvas,nk_rect(r.x-4*f,r.y-4*f,r.w+8*f,r.h+8*f),radius*f,2*f,accent());
    nk_push_scissor(canvas,clip);
}
static bool laboratory_checkbox(app *a,const char *label,bool *value,float inset,struct nk_rect *bounds) {
    struct nk_context *ui=a->ui;float f=a->preferences.ui_size/16.f;const struct nk_user_font *font=&a->laboratory_caption_font;
    float width=fmaxf(1,nk_window_get_content_region(ui).w-2*inset-21*f),line=15.95f*f;
    unsigned lines=laboratory_wrapped_lines(font,label,width);nk_layout_row_dynamic(ui,lines*line+9*f,1);struct nk_rect r;
    enum nk_widget_layout_states visible=nk_widget(&r,ui);r.x+=inset;r.w=fmaxf(1,r.w-2*inset);if(bounds)*bounds=r;
    if(visible==NK_WIDGET_INVALID)return false;
    bool focused=false;bool changed=ps_ui_switch_at_custom_focus(ui,plot_intersection(r,ui->current->layout->clip),label,value,&focused);
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);struct nk_rect box=nk_rect(r.x,r.y+(line-13*f)/2,13*f,13*f);
    nk_fill_rect(canvas,box,2*f,*value?accent():UI_COLOR(window));nk_stroke_rect(canvas,nk_rect(box.x+f/2,box.y+f/2,box.w-f,box.h-f),1.5f*f,f,*value?accent():UI_COLOR(border));
    if(*value){struct nk_color ink=UI_COLOR(primary_text);nk_stroke_line(canvas,box.x+3*f,box.y+6*f,box.x+5.5f*f,box.y+9*f,1.4f*f,ink);nk_stroke_line(canvas,box.x+5.5f*f,box.y+9*f,box.x+10*f,box.y+3.5f*f,1.4f*f,ink);}
    if(focused)laboratory_focus_outline(a,box,6);
    laboratory_wrapped_text(canvas,font,label,nk_rect(r.x+21*f,r.y,width,lines*line),line,UI_COLOR(text));return changed;
}
