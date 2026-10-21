/* Design H: common header, direct navigation and article-specific context.
 * This file uses the viewer's fonts/GL resources and semantic palette only. */
static const struct nk_user_font *documentation_chrome_font(app *a) {
    const struct nk_user_font *font=a->doc_chrome_fonts[(a->preferences.ui_size-16)/2];
    return font?font:a->doc_font_ui;
}
/* Captions use compact chrome; actionable labels match H's larger text and
 * genuine selected weight. Both roles follow UI size, never reader zoom. */
static const struct nk_user_font *documentation_control_font(app *a,bool selected) {
    unsigned i=(a->preferences.ui_size-16)/2;
    const struct nk_user_font *font=selected?a->doc_bold_fonts[i]:a->doc_ui_fonts[i];
    return font?font:documentation_chrome_font(a);
}
static float documentation_tab_width(app *a,const char *label) {
    size_t n=strlen(label);
    return fmaxf(documentation_text_width(documentation_control_font(a,false),label,n),
        documentation_text_width(documentation_control_font(a,true),label,n))+2;
}
static void documentation_h_label_inset(app *a,const char *text,bool muted,float inset) {
    struct nk_context *ui=a->doc_ui;const struct nk_user_font *font=ui->style.font;
    nk_layout_row_dynamic(ui,documentation_height(font,text,strlen(text),nk_window_get_content_region(ui).w-2*inset)+8,1);
    struct nk_rect r;enum nk_widget_layout_states visible=nk_widget(&r,ui);
    ps_ui_accessibility_label(ui,r,text);
    if(visible==NK_WIDGET_INVALID)return;
    size_t at=0,n=strlen(text);float y=r.y+4;
    do {
        size_t length,used=documentation_line(font,text+at,n-at,r.w-2*inset,&length);
        nk_draw_text(nk_window_get_canvas(ui),nk_rect(r.x+inset,y,r.w-2*inset,font->height+6),text+at,(int)length,
            font,nk_rgba(0,0,0,0),muted?UI_COLOR(muted):UI_COLOR(text));
        at+=used;y+=font->height+6;
    }while(at<n);
}
static void documentation_h_label(app *a,const char *text,bool muted) {
    documentation_h_label_inset(a,text,muted,4);
}
static bool documentation_pane_button(app *a,bool right) {
    struct nk_context *ui=a->doc_ui;struct nk_rect r;
    if(nk_widget(&r,ui)==NK_WIDGET_INVALID)return false;
    bool hidden=right?a->doc_context_hidden:a->doc_navigation_hidden;
    const char *name=right?(hidden?"Modellbereich einblenden (geschlossen)":"Modellbereich ausblenden (geöffnet)"):
        (hidden?"Themennavigation einblenden (geschlossen)":"Themennavigation ausblenden (geöffnet)");
    bool pressed=ps_ui_button_at(ui,r,name,!hidden);
    struct nk_command_buffer *c=nk_window_get_canvas(ui);
    if(nk_input_is_mouse_hovering_rect(&ui->input,r)) {
        nk_fill_rect(c,r,5,UI_COLOR(button_hover));nk_tooltip(ui,name);
    }
    /* Same 24-unit SVG geometry as panelIcon() in the H reference, displayed
     * in a 20 px icon box. Keep the entire widget as the input target. */
    const float scale=20.f/24;
    float x=r.x+(r.w-20)/2,y=r.y+(r.h-20)/2;
    struct nk_color ink=hidden?UI_COLOR(muted):UI_COLOR(doc_text);
    if(!hidden){struct nk_color fill=ink;fill.a=43;
        nk_fill_rect(c,nk_rect(x+(right?15:3)*scale,y+4*scale,6*scale,16*scale),0,fill);}
    nk_stroke_rect(c,nk_rect(x+3*scale,y+4*scale,18*scale,16*scale),2*scale,1.5f*scale,ink);
    float line=x+(right?15:9)*scale;
    nk_stroke_line(c,line,y+4*scale,line,y+20*scale,1.5f*scale,ink);
    if(right)a->doc_context_toggle_bounds=r;else a->doc_navigation_bounds=r;
    documentation_focus_draw(a,right?DOC_CONTEXT:DOC_NAVIGATION,r,false);
    return pressed;
}
/* Keep the segmented appearance while publishing the existing native radio
 * group contract (plain Nuklear selectables have no accessibility adapter). */
static bool documentation_segment(app *a,struct nk_rect r,const char *group,const char *label,bool selected,bool tab) {
    struct nk_context *ui=a->doc_ui;
    bool pressed=ps_ui_option_at(ui,r,group,label,selected);
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    /* H keeps text tabs transparent, including on hover. Only the selected
     * language segment has a surface; keyboard focus has its own outline. */
    if(selected && !tab)nk_fill_rect(canvas,r,4,UI_COLOR(button));
    if(selected && !tab && a->preferences.theme==PS_THEME_HIGH_CONTRAST)
        nk_stroke_rect(canvas,nk_rect(r.x+1,r.y+1,r.w-2,r.h-2),3,2,UI_COLOR(accent));
    const struct nk_user_font *font=documentation_control_font(a,selected);
    float width=documentation_text_width(font,label,strlen(label));
    nk_draw_text(canvas,nk_rect(r.x+(r.w-width)/2,r.y+(r.h-font->height)/2,width+1,font->height),
        label,(int)strlen(label),font,nk_rgba(0,0,0,0),selected?UI_COLOR(doc_text):UI_COLOR(muted));
    if(tab && selected)nk_stroke_line(canvas,r.x,r.y+r.h-1,r.x+r.w,r.y+r.h-1,2,UI_COLOR(accent));
    if(nk_sdl_accessibility_is_focused(ui,group,label))nk_stroke_rect(canvas,nk_rect(r.x+1,r.y+1,r.w-2,r.h-2),3,2,UI_COLOR(accent));
    return pressed;
}
static void documentation_header(app *a) {
    struct nk_context *ui=a->doc_ui;float h=fmaxf(34,ui->style.font->height+12);
    float width=nk_window_get_content_region(ui).w;
    const struct nk_user_font *brand_font=documentation_control_font(a,true);
    const struct nk_user_font *handbook_font=documentation_control_font(a,false);
    float brand_name=documentation_text_width(brand_font,"PHYSIM",6);
    float brand=brand_name+8+documentation_text_width(handbook_font,"Handbuch",8)+2;
    float physim=documentation_tab_width(a,"Physim")+18;
    float cwidth=documentation_tab_width(a,"C")+18;
    float language=cwidth+physim+8;
    float available=width-brand-language-64-5*ui->style.window.spacing.x;
    float search=fminf(310,fmaxf(100,available)),spacer=fmaxf(0,available-search);
    nk_layout_row_begin(ui,NK_STATIC,h,6);
    nk_layout_row_push(ui,32);if(documentation_pane_button(a,false))documentation_navigation_toggle(a);
    nk_layout_row_push(ui,brand);struct nk_rect brand_bounds;nk_widget(&brand_bounds,ui);
    ps_ui_accessibility_label(ui,brand_bounds,"PHYSIM Handbuch");
    nk_draw_text(nk_window_get_canvas(ui),nk_rect(brand_bounds.x,brand_bounds.y+(brand_bounds.h-brand_font->height)/2,brand_name+1,brand_font->height),
        "PHYSIM",6,brand_font,nk_rgba(0,0,0,0),UI_COLOR(doc_text));
    nk_draw_text(nk_window_get_canvas(ui),nk_rect(brand_bounds.x+brand_name+8,brand_bounds.y+(brand_bounds.h-handbook_font->height)/2,brand-brand_name-8,handbook_font->height),
        "Handbuch",8,handbook_font,nk_rgba(0,0,0,0),UI_COLOR(muted));
    nk_layout_row_push(ui,language);struct nk_rect segment;nk_widget(&segment,ui);
    nk_fill_rect(nk_window_get_canvas(ui),segment,5,UI_COLOR(edit));
    nk_stroke_rect(nk_window_get_canvas(ui),segment,5,1,UI_COLOR(divider));
    a->doc_track_bounds[0]=nk_rect(segment.x+3,segment.y+3,cwidth,segment.h-6);
    a->doc_track_bounds[1]=nk_rect(segment.x+5+cwidth,segment.y+3,physim,segment.h-6);
    if(documentation_segment(a,a->doc_track_bounds[0],"Dokumentationssprache","C",a->doc_track!=2,false))documentation_language(a,1);
    documentation_focus_draw(a,DOC_C,a->doc_track_bounds[0],false);
    if(documentation_segment(a,a->doc_track_bounds[1],"Dokumentationssprache","Physim",a->doc_track==2,false))documentation_language(a,2);
    documentation_focus_draw(a,DOC_PHYSIM,a->doc_track_bounds[1],false);
    nk_layout_row_push(ui,spacer);nk_spacing(ui,1);
    nk_layout_row_push(ui,search);
    if(a->doc_keyboard && a->doc_focus!=DOC_FILTER)nk_edit_unfocus(ui);
    if(a->doc_filter_focus || (a->doc_keyboard && a->doc_focus==DOC_FILTER && !ui->current->edit.active)) {
        nk_edit_focus(ui,NK_EDIT_ALWAYS_INSERT_MODE);a->doc_filter_focus=false;
    }
    struct nk_rect bounds=nk_widget_bounds(ui);a->doc_filter_bounds=bounds;
    nk_style_push_font(ui,documentation_control_font(a,false));
    nk_style_push_float(ui,&ui->style.edit.border,1);
    nk_style_push_color(ui,&ui->style.edit.border_color,UI_COLOR(divider));
    nk_flags edit=nk_sdl_edit_named(ui,"Alle Inhalte durchsuchen",NK_EDIT_FIELD,a->doc_filter,sizeof a->doc_filter,nk_filter_default);
    nk_style_pop_color(ui);nk_style_pop_float(ui);nk_style_pop_font(ui);
    if(edit&PS_UI_EDIT_NATIVE_FOCUS){a->doc_focus=DOC_FILTER;a->doc_keyboard=true;}
    a->doc_filter_active=(edit&NK_EDIT_ACTIVE)!=0;
    if(a->doc_filter_active)a->doc_search_active=false;
    documentation_focus_draw(a,DOC_FILTER,bounds,false);
    if(!a->doc_filter[0] && !a->doc_filter_active) {
        const char *placeholder=a->doc_track==2?"Physim-Handbuch durchsuchen …":"C-Handbuch durchsuchen …";
        nk_draw_text(nk_window_get_canvas(ui),nk_rect(bounds.x+8,bounds.y+6,bounds.w-16,bounds.h-12),placeholder,
            (int)strlen(placeholder),documentation_control_font(a,false),nk_rgba(0,0,0,0),UI_COLOR(muted));
    }
    if(strcmp(a->doc_filter,a->doc_last_filter)) {
        documentation_filter(a);a->doc_navigation_hidden=false;a->doc_reset_sidebar=true;
    }
    nk_layout_row_push(ui,32);if(documentation_pane_button(a,true))documentation_context_toggle(a);
    nk_layout_row_end(ui);
}
static void documentation_sections(app *a) {
    if(!a->documentation)return;
    int current=-1;
    for(size_t i=0;i<a->documentation->count;i++) {
        const ps_doc_block *b=&a->documentation->blocks[i];
        if(current<0 && documentation_contents_heading(a,b) && b->level>1)current=(int)i;
        if(documentation_contents_heading(a,b) && b->level>1 && b->hit[2]>0 && b->hit[1]<=a->doc_body_bounds.y+16)current=(int)i;
    }
    struct nk_context *ui=a->doc_ui;
    struct nk_rect content=nk_window_get_content_region(ui);
    float indent=18;
    nk_layout_row_dynamic(ui,30,1);struct nk_rect caption;nk_widget(&caption,ui);
    ps_ui_accessibility_label(ui,caption,"IN DIESEM ARTIKEL");
    nk_draw_text(nk_window_get_canvas(ui),nk_rect(caption.x+indent+6,caption.y+8,caption.w-indent-6,ui->style.font->height),
        "IN DIESEM ARTIKEL",17,ui->style.font,nk_rgba(0,0,0,0),UI_COLOR(muted));
    float top=caption.y,bottom=caption.y+caption.h;bool first=true;
    for(size_t i=0;i<a->documentation->count;i++) {
        ps_doc_block *b=&a->documentation->blocks[i];
        if(!documentation_contents_heading(a,b) || b->level==1)continue;
        const char *label=a->documentation->text+b->text;
        float extra=b->level>2?8:0;
        bool selected=current==(int)i;
        const struct nk_user_font *font=documentation_control_font(a,selected);
        nk_layout_row_begin(ui,NK_STATIC,documentation_height(font,label,strlen(label),content.w-indent-extra-12)+10,2);
        nk_layout_row_push(ui,indent+extra-ui->style.window.spacing.x);nk_spacing(ui,1);
        nk_layout_row_push(ui,content.w-indent-extra);struct nk_rect bounds=nk_widget_bounds(ui);
        if(first){a->doc_first_section_bounds=bounds;first=false;}
        struct nk_rect hit;enum nk_widget_layout_states visible=nk_widget(&hit,ui);
        if(ps_ui_button_at(ui,hit,label,selected))a->doc_jump_block=(int)i;
        if(visible!=NK_WIDGET_INVALID) {
            struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
            if(selected || nk_input_is_mouse_hovering_rect(&ui->input,hit))nk_fill_rect(canvas,hit,0,selected?UI_COLOR(selection_active):UI_COLOR(button_hover));
            if(selected)nk_fill_rect(canvas,nk_rect(hit.x,hit.y,2,hit.h),0,UI_COLOR(accent));
            size_t at=0,n=strlen(label);float y=hit.y+5;
            do {size_t length,used=documentation_line(font,label+at,n-at,hit.w-12,&length);
                nk_draw_text(canvas,nk_rect(hit.x+6,y,hit.w-12,font->height+6),label+at,(int)length,font,
                    nk_rgba(0,0,0,0),selected?UI_COLOR(text):UI_COLOR(muted));
                at+=used;y+=font->height+6;
            }while(at<n);
        }
        documentation_focus_draw(a,DOC_HEADING+(int)i,bounds,false);
        bottom=bounds.y+bounds.h;nk_layout_row_end(ui);
    }
    nk_stroke_line(nk_window_get_canvas(ui),caption.x+11,top,caption.x+11,bottom,1,UI_COLOR(divider));
    nk_layout_row_dynamic(ui,14,1);nk_spacing(ui,1);
}
static void documentation_sidebar_content(app *a) {
    struct nk_context *ui=a->doc_ui;
    a->doc_first_result_bounds=nk_rect(0,0,0,0);
    a->doc_active_topic_bounds=a->doc_first_section_bounds=nk_rect(0,0,0,0);
    if(a->doc_filter[0])documentation_h_label(a,"SUCHERGEBNISSE",true);
    const char *groups[]={a->doc_track==2?"TEIL II · PHYSIM":"TEIL I · C","MECHANIK","NACHSCHLAGEN",
        "APP & LERNEN","FACHTHEMEN","REFERENZ","PROJEKTTECHNIK","QUELLEN"};
    int order[DOCUMENTATION_TOPIC_COUNT];unsigned count=documentation_topic_order(a,order);
    int previous=-1;
        for(unsigned n=0;n<count;n++) {
            int topic=order[n];
            int group=documentation_h_group(topic);
            if(!a->doc_filter[0] && group!=previous) {
                if(previous>=0){nk_layout_row_dynamic(ui,8,1);nk_spacing(ui,1);}
                documentation_h_label_inset(a,groups[group],true,7);previous=group;
            }
            const char *title=documentation_topics[topic].title;
            nk_style_push_font(ui,documentation_control_font(a,topic==a->doc_topic));
            nk_layout_row_dynamic(ui,documentation_height(ui->style.font,title,strlen(title),nk_window_get_content_region(ui).w-20)+14,1);
            struct nk_rect bounds;
            bool pressed=documentation_topic_button(a,title,topic==a->doc_topic,&bounds);
            nk_style_pop_font(ui);
            if(topic==a->doc_topic)a->doc_active_topic_bounds=bounds;
            if(a->doc_filter[0] && n==0)a->doc_first_result_bounds=bounds;
            if(pressed) {
                open_documentation(a,topic);
                if(a->doc_filter[0]){snprintf(a->doc_query,sizeof a->doc_query,"%s",a->doc_filter);a->doc_move_match=true;}
            }
            documentation_focus_draw(a,DOC_TOPIC+topic,bounds,false);
            if(!a->doc_filter[0] && topic==a->doc_topic)documentation_sections(a);
        }
    if(!count)documentation_h_label(a,"Keine Treffer in diesem Sprachlernweg. Anderen Begriff suchen oder die Suche leeren.",true);
}
static void documentation_sidebar(app *a) {
    struct nk_context *ui=a->doc_ui;struct nk_rect content=nk_window_get_content_region(ui),head;
    nk_layout_row_dynamic(ui,46,1);nk_widget(&head,ui);
    ps_ui_accessibility_label(ui,head,"THEMEN & ABSCHNITTE");
    nk_fill_rect(nk_window_get_canvas(ui),head,0,UI_COLOR(header));
    nk_draw_text(nk_window_get_canvas(ui),nk_rect(head.x+12,head.y+(head.h-ui->style.font->height)/2,head.w-24,ui->style.font->height),
        "THEMEN & ABSCHNITTE",19,ui->style.font,nk_rgba(0,0,0,0),UI_COLOR(muted));
    nk_stroke_line(nk_window_get_canvas(ui),head.x,head.y+head.h,head.x+head.w,head.y+head.h,1,UI_COLOR(divider));
    nk_layout_row_dynamic(ui,fmaxf(20,content.h-48),1);
    nk_style_push_vec2(ui,&ui->style.window.group_padding,nk_vec2(15,20));
    if(nk_group_scrolled_offset_begin(ui,&a->doc_navigation_scroll_x,&a->doc_navigation_scroll_y,"navigation-content",0)) {
        documentation_sidebar_content(a);nk_group_scrolled_end(ui);
    }
    nk_style_pop_vec2(ui);
    nk_stroke_line(nk_window_get_canvas(ui),a->doc_left_slot.x+a->doc_left_slot.w-1,a->doc_left_slot.y,
        a->doc_left_slot.x+a->doc_left_slot.w-1,a->doc_left_slot.y+a->doc_left_slot.h,1,UI_COLOR(divider));
}
typedef struct {const char *article,*title,*values,*hint,*source[2];int diagram;} documentation_context;
static const documentation_context documentation_contexts[]={
    {"docs/pendulum-tutorial.md","Ideales Pendel · Vakuum",
     "Masse: 1 kg\nLänge: 1,5 m\nStartwinkel: 0,45 rad\nWinkeltempo: 0 rad/s\nSchwerkraft: 9,80665 m/s²",
     "Ohne Widerstand bleibt die mechanische Energie konstant. Vergleiche den Energiefehler der Integratoren.",
     {"examples/documentation/pendulum_main.c","examples/documentation/pendulum_main.phys"},1},
    {"docs/projectile-tutorial.md","Wurfparabel · Vakuum",
     "Masse: 1 kg\nStart: (−2; 0) m\nGeschwindigkeit: (2; 5) m/s\nSchwerkraft: 9,80665 m/s²",
     "Kein Boden und kein Luftwiderstand. Die mechanische Energie bleibt 14,5 J.",
     {"examples/documentation/vacuum_main.c","examples/language/projectile.phys"},2},
    {"docs/projectile-drag-tutorial.md","Wurf · ruhende Luft",
     "Masse: 1 kg\nStart: (−2; 0) m\nGeschwindigkeit: (2; 5) m/s\nDichte: 1,225 kg/m³\nCd: 0,47 · Fläche: 0,01 m²",
     "Quadratischer Widerstand führt Energie ab. Kein Bodenkontakt, Wind oder Auftrieb.",
     {"examples/projectile/main.c","examples/language/projectile_drag.phys"},2},
    {"docs/spring.md","Feder–Masse–Dämpfer",
     "Masse: 1 kg\nFedersteifigkeit: 16 N/m\nDämpfung: 1,2 N·s/m\nRuhelänge: 1 m\nStartdehnung: 0,35 m\nStarttempo: 0 m/s",
     "Dämpfung variiert zwischen 0 und 64 N·s/m. Vergleiche mechanische und dissipierte Energie.",
     {"examples/spring/main.c","examples/language/spring.phys"},3}
};
static const documentation_context *documentation_context_for(app *a) {
    const char *path=documentation_topics[a->doc_topic].path;
    for(unsigned i=0;i<sizeof documentation_contexts/sizeof documentation_contexts[0];i++)
        if(!strcmp(path,documentation_contexts[i].article))return &documentation_contexts[i];
    return NULL;
}
static const char *documentation_context_code(app *a) {
    const documentation_context *context=documentation_context_for(a);
    if(!context)return NULL;
    if(!a->doc_context_source || a->doc_context_source_topic!=a->doc_topic || a->doc_context_source_track!=a->doc_track) {
        free(a->doc_context_source);char path[4096];size_t size;
        join(path,sizeof path,a->root,context->source[a->doc_track==2]);
        a->doc_context_source=load_utf8_text_limit(path,&size,PS_DOC_MAX_CODE_BYTES);
        a->doc_context_source_topic=a->doc_topic;a->doc_context_source_track=a->doc_track;
    }
    return a->doc_context_source;
}
static void documentation_context_figure(app *a,int kind,struct nk_rect r) {
    struct nk_command_buffer *c=nk_window_get_canvas(a->doc_ui);struct nk_color ink=UI_COLOR(doc_text);
    float x=r.x+r.w*.25f,y=r.y+24,w=r.w*.6f,h=r.h-48;
    if(kind==1) {
        /* Same logical vector geometry as the selected H model.svg. */
        const char *caption="Ideales Pendel · Vakuum";
        const struct nk_user_font *caption_font=a->doc_ui->style.font;
        float caption_height=documentation_height(caption_font,caption,strlen(caption),r.w-8);
        float s=fminf(r.w/220,fmaxf(1,r.h-caption_height-8)/182),ox=r.x+(r.w-220*s)/2,oy=r.y;
        nk_stroke_line(c,ox+44*s,oy+22*s,ox+146*s,oy+22*s,1.5f,UI_COLOR(muted));
        for(int i=0;i<6;i++)nk_stroke_line(c,ox+(54+i*14)*s,oy+22*s,ox+(62+i*14)*s,oy+13*s,1,UI_COLOR(muted));
        for(int i=0;i<17;i++)nk_stroke_line(c,ox+92*s,oy+(24+i*9)*s,ox+92*s,oy+(28+i*9)*s,1,UI_COLOR(muted));
        nk_stroke_line(c,ox+92*s,oy+24*s,ox+155*s,oy+161*s,2.5f,ink);
        nk_fill_circle(c,nk_rect(ox+89*s,oy+21*s,6*s,6*s),ink);
        nk_fill_circle(c,nk_rect(ox+142*s,oy+148*s,26*s,26*s),UI_COLOR(button_hover));
        nk_stroke_circle(c,nk_rect(ox+142*s,oy+148*s,26*s,26*s),1.5f,ink);
        for(int i=0;i<12;i++) {
            float a=i*.43f/12,b=(i+1)*.43f/12;
            nk_stroke_line(c,ox+(92+sinf(a)*49)*s,oy+(24+cosf(a)*49)*s,
                ox+(92+sinf(b)*49)*s,oy+(24+cosf(b)*49)*s,1,UI_COLOR(muted));
        }
        const struct nk_user_font *font=documentation_control_font(a,false);
        const char *labels[]={"θ","L","m"};float lx[]={99,136,176},ly[]={95,95,168};
        for(unsigned i=0;i<3;i++)nk_draw_text(c,nk_rect(ox+lx[i]*s,oy+ly[i]*s-font->height,30,font->height),
            labels[i],(int)strlen(labels[i]),font,nk_rgba(0,0,0,0),ink);
        struct nk_rect caption_bounds=nk_rect(r.x+4,floorf(r.y+r.h-caption_height),r.w-8,ceilf(caption_height));
        size_t at=0,n=strlen(caption);float cy=caption_bounds.y;
        do {
            size_t length,used=documentation_line(caption_font,caption+at,n-at,caption_bounds.w,&length);
            float width=documentation_text_width(caption_font,caption+at,length);
            nk_draw_text(c,nk_rect(caption_bounds.x+(caption_bounds.w-width)/2,cy,width+1,caption_font->height+6),
                caption+at,(int)length,caption_font,nk_rgba(0,0,0,0),UI_COLOR(muted));
            at+=used;cy+=caption_font->height+6;
        }while(at<n);
        if(a->doc_context_caption_test) {
            a->doc_context_caption_bounds=caption_bounds;
        }
    } else if(kind==2) {
        nk_stroke_line(c,x,y+h,x+w,y+h,1,UI_COLOR(muted));
        nk_stroke_line(c,x,y+h,x,y,1,UI_COLOR(muted));
        for(int i=0;i<40;i++) {
            float t=i/40.f,u=(i+1)/40.f;
            nk_stroke_line(c,x+w*t,y+h-(4*t*(1-t))*.8f*h,x+w*u,y+h-(4*u*(1-u))*.8f*h,2,UI_COLOR(accent));
        }
    } else {
        nk_stroke_line(c,x,y,x,y+h,2,ink);
        float prevx=x,prevy=y+h/2;
        for(int i=1;i<=12;i++) {
            float nextx=x+i*w/16,nexty=y+h/2+(i==12?0:(i%2?12:-12));
            nk_stroke_line(c,prevx,prevy,nextx,nexty,2,ink);prevx=nextx;prevy=nexty;
        }
        nk_stroke_rect(c,nk_rect(prevx,prevy-20,w/4,40),3,2,ink);
    }
}
static void documentation_context_content(app *a);
static void documentation_context_view(app *a) {
    struct nk_context *ui=a->doc_ui;float row=fmaxf(46,ui->style.font->height+16);
    float model=documentation_tab_width(a,"Modell");
    float example=documentation_tab_width(a,"Beispiel");
    nk_fill_rect(nk_window_get_canvas(ui),nk_rect(a->doc_right_slot.x,a->doc_right_slot.y,a->doc_right_slot.w,row),0,UI_COLOR(header));
    nk_layout_row_begin(ui,NK_STATIC,row,3);nk_layout_row_push(ui,model);
    struct nk_rect r;nk_widget(&r,ui);
    if(documentation_segment(a,r,"Kontextansicht","Modell",!a->doc_context_example,true))a->doc_context_example=false;
    documentation_focus_draw(a,DOC_MODEL_TAB,r,false);
    nk_layout_row_push(ui,0);nk_spacing(ui,1);nk_layout_row_push(ui,example);nk_widget(&r,ui);
    if(documentation_segment(a,r,"Kontextansicht","Beispiel",a->doc_context_example,true))a->doc_context_example=true;
    documentation_focus_draw(a,DOC_EXAMPLE_TAB,r,false);
    nk_layout_row_end(ui);
    nk_stroke_line(nk_window_get_canvas(ui),a->doc_right_slot.x,r.y+r.h,a->doc_right_slot.x+a->doc_right_slot.w,r.y+r.h,1,UI_COLOR(divider));
    struct nk_rect next=nk_widget_bounds(ui),clip=nk_window_get_content_region(ui);
    nk_layout_row_dynamic(ui,fmaxf(20,clip.y+clip.h-next.y),1);
    unsigned tab=a->doc_context_example?1:0;
    nk_style_push_vec2(ui,&ui->style.window.group_padding,nk_vec2(6,17));
    if(nk_group_scrolled_offset_begin(ui,&a->doc_context_scroll_x[tab],&a->doc_context_scroll_y[tab],"context-content",0)) {
        documentation_context_content(a);nk_group_scrolled_end(ui);
    }
    nk_style_pop_vec2(ui);
}
static void documentation_context_content(app *a) {
    struct nk_context *ui=a->doc_ui;float row=fmaxf(34,ui->style.font->height+16);struct nk_rect r;
    const documentation_context *context=documentation_context_for(a);
    if(!context) {
        documentation_h_label(a,a->doc_context_example?"Kein Beispiel zu diesem Artikel":"Kein Modell zu diesem Artikel",false);
        documentation_h_label(a,"Der vollständige Artikel und seine verknüpften Quellen stehen im Lesebereich.",true);return;
    }
    if(a->doc_context_example)documentation_h_label(a,context->title,false);
    else {
        nk_style_push_font(ui,a->doc_ui_fonts[(a->preferences.ui_size-16)/2]);
        documentation_h_label(a,"Das Versuchsmodell",false);nk_style_pop_font(ui);
    }
    if(a->doc_context_example) {
        const char *code=documentation_context_code(a);
        documentation_h_label(a,context->source[a->doc_track==2],true);
        if(!code){documentation_h_label(a,"Beispielquelle konnte nicht geladen werden.",true);return;}
        nk_layout_row_dynamic(ui,row,1);r=nk_widget_bounds(ui);
        if(nk_button_label(ui,"Code kopieren"))SDL_SetClipboardText(code);
        documentation_focus_draw(a,DOC_CONTEXT_COPY,r,false);
        /* Complete source: wrap at measured UTF-8 boundaries in the side pane. */
        unsigned size=a->preferences.editor_size<16?16:a->preferences.editor_size;
        const struct nk_user_font *font=a->doc_code_fonts[(size-16)/2];
        if(!font)font=ui->style.font;
        nk_style_push_font(ui,font);
        const char *line=code;
        while(*line) {
            const char *end=strchr(line,'\n');size_t n=end?(size_t)(end-line):strlen(line);
            if(n && line[n-1]=='\r')--n; /* Display LF; copy retains the complete source. */
            char text[4096];if(n>=sizeof text)n=sizeof text-1;
            memcpy(text,line,n);text[n]=0;documentation_h_label(a,text,false);
            line=end?end+1:line+strlen(line);
        }
        nk_style_pop_font(ui);
    } else {
        nk_layout_row_dynamic(ui,205,1);r=nk_widget_bounds(ui);nk_widget(&r,ui);
        ps_ui_accessibility_label(ui,r,context->title);
        documentation_context_figure(a,context->diagram,r);
        documentation_h_label(a,context->diagram<3?"Ausgangswerte · Vakuum":"Ausgangswerte",true);
        const char *line=context->values;
        while(*line) {
            const char *end=strchr(line,'\n');size_t length=end?(size_t)(end-line):strlen(line);
            const char *colon=memchr(line,':',length);
            if(colon) {
                char label[128],value[128];size_t n=(size_t)(colon-line),v=length-n-1;
                if(n>=sizeof label)n=sizeof label-1;memcpy(label,line,n);label[n]=0;
                const char *start=colon+1;while(v && *start==' '){start++;v--;}
                if(v>=sizeof value)v=sizeof value-1;memcpy(value,start,v);value[v]=0;
                float column=(nk_window_get_content_region(ui).w-ui->style.window.spacing.x)/2;
                float height=fmaxf(documentation_height(ui->style.font,label,strlen(label),column-10),
                    documentation_height(ui->style.font,value,strlen(value),column-10))+20;
                nk_layout_row_dynamic(ui,height,2);struct nk_rect cell={0};
                const char *cells[]={label,value};
                for(unsigned i=0;i<2;i++) {
                    enum nk_widget_layout_states visible=nk_widget(&cell,ui);
                    ps_ui_accessibility_label(ui,cell,cells[i]);
                    if(visible==NK_WIDGET_INVALID)continue;
                    size_t at=0,length=strlen(cells[i]);float y=cell.y+10;
                    do {size_t drawn,used=documentation_line(ui->style.font,cells[i]+at,length-at,cell.w-10,&drawn);
                        nk_draw_text(nk_window_get_canvas(ui),nk_rect(cell.x+5,y,cell.w-10,ui->style.font->height+6),cells[i]+at,(int)drawn,
                            ui->style.font,nk_rgba(0,0,0,0),UI_COLOR(text));at+=used;y+=ui->style.font->height+6;
                    }while(at<length);
                }
                nk_stroke_line(nk_window_get_canvas(ui),cell.x-cell.w-ui->style.window.spacing.x,cell.y+cell.h,
                    cell.x+cell.w,cell.y+cell.h,1,UI_COLOR(divider));
            }
            line=end?end+1:line+length;
        }
        nk_layout_row_dynamic(ui,14,1);nk_spacing(ui,1);
        float width=nk_window_get_content_region(ui).w-16;
        float notice_height=documentation_height(ui->style.font,context->hint,strlen(context->hint),width)+ui->style.font->height+20;
        nk_layout_row_dynamic(ui,notice_height,1);struct nk_rect notice;nk_widget(&notice,ui);
        ps_ui_accessibility_label(ui,notice,context->hint);
        struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
        nk_fill_rect(canvas,nk_rect(notice.x,notice.y,2,notice.h),0,UI_COLOR(accent));
        nk_draw_text(canvas,nk_rect(notice.x+13,notice.y+8,width,ui->style.font->height),"Worauf achten?",14,
            ui->style.font,nk_rgba(0,0,0,0),UI_COLOR(text));
        size_t at=0,n=strlen(context->hint);float y=notice.y+ui->style.font->height+14;
        do {size_t length,used=documentation_line(ui->style.font,context->hint+at,n-at,width,&length);
            nk_draw_text(canvas,nk_rect(notice.x+13,y,width,ui->style.font->height+6),context->hint+at,(int)length,
                ui->style.font,nk_rgba(0,0,0,0),UI_COLOR(text));at+=used;y+=ui->style.font->height+6;
        }while(at<n);
        documentation_h_label(a,context->article,true);
    }
}
static void documentation_footer(app *a) {
    struct nk_context *ui=a->doc_ui;float h=ui->style.font->height+8;
    float width=nk_window_get_content_region(ui).w;
    char value[32];snprintf(value,sizeof value,"%u px",documentation_font_size(a));
    float action=fmaxf(72,documentation_text_width(ui->style.font,value,strlen(value))+16);
    nk_layout_row_begin(ui,NK_STATIC,h,2);nk_layout_row_push(ui,fmaxf(100,width-action-ui->style.window.spacing.x));
    const char *status=a->doc_track==2?"Teil II · Physim · Offline-Handbuch":"Teil I · C · Offline-Handbuch";
    nk_label(ui,status,NK_TEXT_LEFT);
    nk_layout_row_push(ui,action);struct nk_rect r;nk_widget(&r,ui);a->doc_actions_trigger_bounds=r;
    char accessible[96];snprintf(accessible,sizeof accessible,"Handbuchaktionen · Leseschrift %s",value);
    if(ps_ui_button_at(ui,r,accessible,false))documentation_actions_open(a,r.x,r.y);
    nk_draw_text(nk_window_get_canvas(ui),r,value,(int)strlen(value),ui->style.font,nk_rgba(0,0,0,0),UI_COLOR(muted));
    documentation_focus_draw(a,DOC_ACTIONS,r,false);
    if(nk_input_is_mouse_hovering_rect(&ui->input,r))nk_tooltip(ui,"Handbuchaktionen: Verlauf, Suche und Leseschrift · Umschalt+F10");
    nk_layout_row_end(ui);
}
