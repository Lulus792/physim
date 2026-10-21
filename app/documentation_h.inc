/* Design H: common header, direct navigation and article-specific context.
 * This file uses the viewer's fonts/GL resources and semantic palette only. */
static const struct nk_user_font *documentation_chrome_font(app *a) {
    return documentation_type_font(a,DOC_TYPE_CHROME);
}
/* Captions use compact chrome; actionable labels match H's larger text and
 * genuine selected weight. Both roles follow UI size, never reader zoom. */
static const struct nk_user_font *documentation_control_font(app *a,bool selected) {
    return documentation_type_font(a,selected?DOC_TYPE_CONTROL_STRONG:DOC_TYPE_CONTROL);
}
static float documentation_tab_width(app *a,const char *label,bool selected) {
    size_t n=strlen(label);
    return documentation_text_width(documentation_control_font(a,selected),label,n);
}
/* Snap the two edges together. Truncating origin and size independently can
 * lose almost two pixels and changes both the painted and native hit target. */
static struct nk_rect documentation_h_snap(struct nk_rect r) {
    float x=roundf(r.x),y=roundf(r.y);
    return nk_rect(x,y,roundf(r.x+r.w)-x,roundf(r.y+r.h)-y);
}
static void documentation_side_border(app *a,bool right) {
    struct nk_context *ui=a->doc_ui;struct nk_panel *panel=ui->current->layout;
    struct nk_rect slot=right?a->doc_right_slot:a->doc_left_slot;
    nk_fill_rect(nk_window_get_canvas(ui),nk_rect(right?slot.x:slot.x+slot.w-1,slot.y,1,slot.h),0,UI_COLOR(divider));
    /* CSS border-box: the one-pixel outer rule is outside both the scrolling
     * viewport and its padding, not another pixel of usable content. */
    if(right){panel->bounds.x+=1;panel->at_x+=1;panel->clip.x+=1;}
    panel->bounds.w-=1;panel->clip.w-=1;
    nk_push_scissor(nk_window_get_canvas(ui),panel->clip);
}
static float documentation_h_text_height(const struct nk_user_font *font,const char *text,float width,float line) {
    size_t at=0,n=strlen(text);unsigned lines=0;
    do {size_t length,used=documentation_line(font,text+at,n-at,fmaxf(1,width),&length);at+=used;++lines;}while(at<n);
    return lines*line;
}
static void documentation_h_text_draw(app *a,const struct nk_user_font *font,const char *text,struct nk_rect r,float line,struct nk_color ink) {
    size_t at=0,n=strlen(text);float y=r.y+(line-font->height)/2;
    do {
        size_t length,used=documentation_line(font,text+at,n-at,fmaxf(1,r.w),&length);
        nk_draw_text(nk_window_get_canvas(a->doc_ui),nk_rect(r.x,y,r.w+1,font->height),text+at,(int)length,
            font,nk_rgba(0,0,0,0),ink);
        at+=used;y+=line;
    }while(at<n);
}
static struct nk_rect documentation_h_text_row(app *a,unsigned role,const char *text,float leading,float inset,float top,float bottom,bool muted) {
    struct nk_context *ui=a->doc_ui;const struct nk_user_font *font=documentation_type_font(a,role);
    float line=font->height/a->doc_type_em[role]*leading;
    float height=documentation_h_text_height(font,text,ui->current->layout->bounds.w-2*inset,line);
    ps_ui_row_exact_dynamic(ui,top+height+bottom,1);struct nk_rect r;
    struct nk_rect measured=nk_widget_bounds(ui);
    enum nk_widget_layout_states visible=nk_widget(&r,ui);r=documentation_h_snap(measured);
    struct nk_rect text_bounds=nk_rect(r.x+inset,r.y+top,r.w-2*inset,height);
    ps_ui_accessibility_label(ui,text_bounds,text);
    if(visible!=NK_WIDGET_INVALID)documentation_h_text_draw(a,font,text,text_bounds,line,muted?UI_COLOR(muted):UI_COLOR(doc_text));
    return text_bounds;
}
static void documentation_h_spacer(app *a,float height) {
    if(height>0){ps_ui_row_exact_dynamic(a->doc_ui,height,1);nk_spacer(a->doc_ui);}
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
    if(selected && !tab)nk_fill_rect(canvas,r,5,UI_COLOR(button));
    if(selected && !tab && a->preferences.theme==PS_THEME_HIGH_CONTRAST)
        nk_stroke_rect(canvas,nk_rect(r.x+1,r.y+1,r.w-2,r.h-2),3,2,UI_COLOR(accent));
    const struct nk_user_font *font=documentation_control_font(a,selected);
    float width=documentation_text_width(font,label,strlen(label));
    nk_draw_text(canvas,nk_rect(r.x+(r.w-width)/2,r.y+(r.h-font->height)/2,width+1,font->height),
        label,(int)strlen(label),font,nk_rgba(0,0,0,0),selected?UI_COLOR(doc_text):UI_COLOR(muted));
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
    float physim=documentation_tab_width(a,"Physim",a->doc_track==2)+20;
    float cwidth=documentation_tab_width(a,"C",a->doc_track==1)+20;
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
    float indent=19,ui_scale=a->preferences.ui_size/16.f;
    documentation_h_spacer(a,9);
    ps_ui_row_exact_dynamic(ui,1,1);struct nk_rect caption=nk_widget_bounds(ui);
    const struct nk_user_font *group=documentation_type_font(a,DOC_TYPE_GROUP);
    float caption_height=documentation_h_text_height(group,"IN DIESEM ARTIKEL",content.w-indent-14,15*ui_scale);
    ui->current->layout->row.height=caption_height+13;
    struct nk_rect caption_measured=nk_widget_bounds(ui);
    enum nk_widget_layout_states caption_visible=nk_widget(&caption,ui);
    caption=documentation_h_snap(caption_measured);
    struct nk_rect label=nk_rect(caption.x+indent+7,caption.y,caption.w-indent-14,caption_height);
    a->doc_section_caption_bounds=label;
    ps_ui_accessibility_label(ui,label,"IN DIESEM ARTIKEL");
    if(caption_visible!=NK_WIDGET_INVALID)documentation_h_text_draw(a,group,"IN DIESEM ARTIKEL",label,15*ui_scale,UI_COLOR(muted));
    float top=caption.y,bottom=caption.y+caption.h;bool first=true;
    for(size_t i=0;i<a->documentation->count;i++) {
        ps_doc_block *b=&a->documentation->blocks[i];
        if(!documentation_contents_heading(a,b) || b->level==1)continue;
        const char *label=a->documentation->text+b->text;
        bool selected=current==(int)i;
        const struct nk_user_font *font=documentation_type_font(a,selected?DOC_TYPE_SECTION_STRONG:DOC_TYPE_SECTION);
        float line=12*ui_scale*1.55f;
        ps_ui_row_exact_begin(ui,NK_STATIC,documentation_h_text_height(font,label,content.w-indent-12,line)+14,2);
        nk_layout_row_push(ui,indent-ui->style.window.spacing.x);nk_spacing(ui,1);
        nk_layout_row_push(ui,content.w-indent);struct nk_rect bounds=nk_widget_bounds(ui);
        struct nk_rect hit;enum nk_widget_layout_states visible=nk_widget(&hit,ui);
        hit=documentation_h_snap(bounds);bounds=hit;
        if(first){a->doc_first_section_bounds=hit;first=false;}
        if(ps_ui_button_at(ui,hit,label,selected))a->doc_jump_block=(int)i;
        if(visible!=NK_WIDGET_INVALID) {
            struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
            if(selected || nk_input_is_mouse_hovering_rect(&ui->input,hit))nk_fill_rect(canvas,hit,0,selected?UI_COLOR(selection_active):UI_COLOR(button_hover));
            if(selected)nk_fill_rect(canvas,nk_rect(hit.x,hit.y,2,hit.h),0,UI_COLOR(accent));
            documentation_h_text_draw(a,font,label,nk_rect(hit.x+6,hit.y+7,hit.w-12,hit.h-14),line,
                selected?UI_COLOR(text):UI_COLOR(muted));
        }
        documentation_focus_draw(a,DOC_HEADING+(int)i,bounds,false);
        bottom=bounds.y+bounds.h;nk_layout_row_end(ui);
    }
    nk_stroke_line(nk_window_get_canvas(ui),caption.x+11,top,caption.x+11,bottom,1,UI_COLOR(divider));
    documentation_h_spacer(a,15);
}
static void documentation_sidebar_content(app *a) {
    struct nk_context *ui=a->doc_ui;
    a->doc_first_result_bounds=nk_rect(0,0,0,0);
    a->doc_active_topic_bounds=a->doc_first_section_bounds=nk_rect(0,0,0,0);
    a->doc_nav_group_bounds[0]=a->doc_nav_group_bounds[1]=nk_rect(0,0,0,0);
    a->doc_section_caption_bounds=nk_rect(0,0,0,0);
    if(a->doc_filter[0])documentation_h_text_row(a,DOC_TYPE_GROUP,"SUCHERGEBNISSE",1.5f,7,0,13,true);
    const char *groups[]={a->doc_track==2?"TEIL II · PHYSIM":"TEIL I · C","MECHANIK","NACHSCHLAGEN",
        "APP & LERNEN","FACHTHEMEN","REFERENZ","PROJEKTTECHNIK","QUELLEN"};
    int order[DOCUMENTATION_TOPIC_COUNT];unsigned count=documentation_topic_order(a,order);
    int previous=-1;unsigned group_index=0;
        for(unsigned n=0;n<count;n++) {
            int topic=order[n];
            int group=documentation_h_group(topic);
            if(!a->doc_filter[0] && group!=previous) {
                if(previous>=0)documentation_h_spacer(a,21);
                struct nk_rect caption=documentation_h_text_row(a,DOC_TYPE_GROUP,groups[group],1.5f,7,0,13,true);
                if(group_index<2)a->doc_nav_group_bounds[group_index]=caption;
                ++group_index;previous=group;
            }
            const char *title=documentation_topics[topic].title;
            nk_style_push_font(ui,documentation_type_font(a,topic==a->doc_topic?DOC_TYPE_NAV_STRONG:DOC_TYPE_NAV));
            ps_ui_row_exact_dynamic(ui,documentation_h_text_height(ui->style.font,title,ui->current->layout->bounds.w-20,
                13*a->preferences.ui_size/16.f*1.45f)+18,1);
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
    if(!count)documentation_h_text_row(a,DOC_TYPE_CONTEXT,"Keine Treffer in diesem Sprachlernweg. Anderen Begriff suchen oder die Suche leeren.",1.65f,7,12,12,true);
}
/* F1 has overlay rails: the viewport owns the scrollbar, while its padded
 * content keeps the CSS width. Nuklear normally reserves that rail inside
 * the content. Expand only this group's layout, then reconstruct the full
 * viewport for its native scrollbar/input/clamping at end. */
static bool documentation_scroll_begin(app *a,nk_uint *x,nk_uint *y,const char *name) {
    struct nk_context *ui=a->doc_ui;
    struct nk_rect viewport=nk_widget_bounds(ui);
    float wheel=ui->input.mouse.scroll_delta.y;
    if(wheel && !a->doc_actions_open && nk_input_is_mouse_hovering_rect(&ui->input,viewport) &&
       !(a->doc_find_open && nk_input_is_mouse_hovering_rect(&ui->input,a->doc_find_bounds))) {
        *y=(nk_uint)fmaxf(0,(float)*y-wheel*viewport.h*.1f);
        ui->input.mouse.scroll_delta.y=0;
    }
    nk_style_push_vec2(ui,&ui->style.window.scrollbar_size,nk_vec2(ui->style.window.scrollbar_size.x,0));
    if(!nk_group_scrolled_offset_begin(ui,x,y,name,0)) {nk_style_pop_vec2(ui);return false;}
    struct nk_panel *g=ui->current->layout;
    g->bounds.w+=ui->style.window.scrollbar_size.x;
    g->clip.w=fmaxf(0,fminf(g->bounds.x+g->bounds.w,g->parent->clip.x+g->parent->clip.w)-g->clip.x);
    nk_push_scissor(nk_window_get_canvas(ui),g->clip);
    return true;
}
static void documentation_scroll_end(app *a) {
    struct nk_context *ui=a->doc_ui;struct nk_panel *g=ui->current->layout;
    float padding=ui->style.window.group_padding.x;
    g->bounds.x-=padding;
    g->bounds.w+=2*padding-ui->style.window.scrollbar_size.x;
    nk_style_push_vec2(ui,&ui->style.window.group_padding,nk_vec2(0,ui->style.window.group_padding.y));
    nk_group_scrolled_end(ui);
    nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
}
static void documentation_sidebar(app *a) {
    struct nk_context *ui=a->doc_ui;struct nk_rect content=nk_window_get_content_region(ui),head;
    ps_ui_row_exact_dynamic(ui,46,1);nk_widget(&head,ui);
    ps_ui_accessibility_label(ui,head,"THEMEN & ABSCHNITTE");
    nk_fill_rect(nk_window_get_canvas(ui),head,0,UI_COLOR(header));
    const struct nk_user_font *caption_font=documentation_type_font(a,DOC_TYPE_PANEL_CAPTION);
    nk_draw_text(nk_window_get_canvas(ui),nk_rect(head.x+12,head.y+(head.h-caption_font->height)/2,head.w-24,caption_font->height),
        "THEMEN & ABSCHNITTE",19,caption_font,nk_rgba(0,0,0,0),UI_COLOR(muted));
    nk_fill_rect(nk_window_get_canvas(ui),nk_rect(head.x,head.y+head.h-1,head.w,1),0,UI_COLOR(divider));
    ps_ui_row_exact_dynamic(ui,fmaxf(20,content.h-46),1);
    nk_style_push_vec2(ui,&ui->style.window.group_padding,nk_vec2(15,20));
    if(documentation_scroll_begin(a,&a->doc_navigation_scroll_x,&a->doc_navigation_scroll_y,"navigation-content")) {
        documentation_sidebar_content(a);documentation_scroll_end(a);
    }
    nk_style_pop_vec2(ui);
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
static struct nk_color documentation_context_svg_color(app *a,int red,int green,int blue) {
    if(a->preferences.theme==PS_THEME_HIGH_CONTRAST)return UI_COLOR(doc_text);
    if(a->preferences.theme==PS_THEME_DARK) {
        red=(int)roundf(224.4f-.76f*red);green=(int)roundf(224.4f-.76f*green);blue=(int)roundf(224.4f-.76f*blue);
    }
    return nk_rgb(red,green,blue);
}
static void documentation_context_figure(app *a,int kind,struct nk_rect r) {
    struct nk_command_buffer *c=nk_window_get_canvas(a->doc_ui);struct nk_color ink=UI_COLOR(doc_text);
    float x=r.x+r.w*.25f,y=r.y+24,w=r.w*.6f,h=r.h-48;
    if(kind==1) {
        /* Same logical vector geometry as the selected H model.svg. */
        const char *caption="Ideales Pendel · Vakuum";
        const struct nk_user_font *caption_font=documentation_type_font(a,DOC_TYPE_FIGURE_CAPTION);
        float caption_line=19.5f*a->preferences.ui_size/16.f;
        float caption_height=documentation_h_text_height(caption_font,caption,r.w-8,caption_line);
        float s=fminf(r.w/220,fminf(r.h/210,fmaxf(1,r.h-caption_height)/182)),ox=r.x+(r.w-220*s)/2;
        bool single_caption=caption_height<=caption_line+.01f;
        float oy=r.y+(single_caption?(r.h-210*s)/2:0);
        ink=documentation_context_svg_color(a,50,50,46);
        struct nk_color muted=documentation_context_svg_color(a,98,98,93),guide=documentation_context_svg_color(a,163,163,156);
        nk_stroke_line(c,ox+44*s,oy+22*s,ox+146*s,oy+22*s,1.5f*s,muted);
        for(int i=0;i<6;i++)nk_stroke_line(c,ox+(54+i*14)*s,oy+22*s,ox+(62+i*14)*s,oy+13*s,s,muted);
        for(int i=0;i<17;i++)nk_stroke_line(c,ox+92*s,oy+(24+i*9)*s,ox+92*s,oy+(28+i*9)*s,s,guide);
        nk_stroke_line(c,ox+92*s,oy+24*s,ox+155*s,oy+161*s,2.5f*s,ink);
        nk_fill_circle(c,nk_rect(ox+89*s,oy+21*s,6*s,6*s),ink);
        nk_fill_circle(c,nk_rect(ox+142*s,oy+148*s,26*s,26*s),documentation_context_svg_color(a,180,180,171));
        nk_stroke_circle(c,nk_rect(ox+142*s,oy+148*s,26*s,26*s),1.5f*s,ink);
        for(int i=0;i<12;i++) {
            float a=i*.43f/12,b=(i+1)*.43f/12;
            nk_stroke_line(c,ox+(92+sinf(a)*49)*s,oy+(24+cosf(a)*49)*s,
                ox+(92+sinf(b)*49)*s,oy+(24+cosf(b)*49)*s,s,muted);
        }
        const struct nk_user_font *font=documentation_type_font(a,DOC_TYPE_FIGURE_LABEL);
        const struct nk_font *native=font->userdata.ptr;
        float baseline=(native->info.ascent+.5f)*font->height/native->info.height;
        const char *labels[]={"θ","L","m"};float lx[]={99,136,176},ly[]={95,95,168};
        for(unsigned i=0;i<3;i++)nk_draw_text(c,nk_rect(ox+lx[i]*s,oy+ly[i]*s-baseline,40,font->height),
            labels[i],(int)strlen(labels[i]),font,nk_rgba(0,0,0,0),ink);
        struct nk_rect caption_bounds=nk_rect(r.x+4,floorf(r.y+r.h-caption_height),r.w-8,ceilf(caption_height));
        size_t at=0,n=strlen(caption);float cy=caption_bounds.y+(caption_line-caption_font->height)/2;
        if(single_caption) {
            const struct nk_font *native_caption=caption_font->userdata.ptr;
            cy=oy+203*s-(native_caption->info.ascent+.5f)*caption_font->height/native_caption->info.height;
            caption_bounds.y=floorf(cy);caption_bounds.h=ceilf(caption_font->height+1);
        }
        do {
            size_t length,used=documentation_line(caption_font,caption+at,n-at,caption_bounds.w,&length);
            float width=documentation_text_width(caption_font,caption+at,length);
            nk_draw_text(c,nk_rect(caption_bounds.x+(caption_bounds.w-width)/2,cy,width+1,caption_font->height),
                caption+at,(int)length,caption_font,nk_rgba(0,0,0,0),documentation_context_svg_color(a,101,101,96));
            at+=used;cy+=caption_line;
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
    struct nk_context *ui=a->doc_ui;float row=46;
    float model=documentation_tab_width(a,"Modell",!a->doc_context_example),example=documentation_tab_width(a,"Beispiel",a->doc_context_example);
    nk_fill_rect(nk_window_get_canvas(ui),nk_rect(a->doc_right_slot.x,a->doc_right_slot.y,a->doc_right_slot.w,row),0,UI_COLOR(header));
    ps_ui_row_exact_begin(ui,NK_STATIC,row,4);nk_layout_row_push(ui,4);nk_spacing(ui,1);
    nk_layout_row_push(ui,model);struct nk_rect r;nk_widget(&r,ui);
    struct nk_rect model_bounds=r;
    if(documentation_segment(a,r,"Kontextansicht","Modell",!a->doc_context_example,true))a->doc_context_example=false;
    documentation_focus_draw(a,DOC_MODEL_TAB,r,false);
    nk_layout_row_push(ui,0);nk_spacing(ui,1);nk_layout_row_push(ui,example);nk_widget(&r,ui);
    struct nk_rect example_bounds=r;
    if(documentation_segment(a,r,"Kontextansicht","Beispiel",a->doc_context_example,true))a->doc_context_example=true;
    documentation_focus_draw(a,DOC_EXAMPLE_TAB,r,false);nk_layout_row_end(ui);
    nk_fill_rect(nk_window_get_canvas(ui),nk_rect(a->doc_right_slot.x,r.y+r.h-1,a->doc_right_slot.w,1),0,UI_COLOR(divider));
    struct nk_rect selected=a->doc_context_example?example_bounds:model_bounds;
    nk_fill_rect(nk_window_get_canvas(ui),nk_rect(selected.x,selected.y+selected.h-2,selected.w,2),0,UI_COLOR(accent));
    struct nk_rect next=nk_widget_bounds(ui),clip=nk_window_get_content_region(ui);
    ps_ui_row_exact_dynamic(ui,fmaxf(20,clip.y+clip.h-next.y),1);
    a->doc_context_content_bounds=nk_widget_bounds(ui);
    unsigned tab=a->doc_context_example?1:0;int w,h;SDL_GetWindowSize(a->doc_window,&w,&h);(void)h;
    nk_style_push_vec2(ui,&ui->style.window.group_padding,nk_vec2(w<=900?13:18,21));
    if(documentation_scroll_begin(a,&a->doc_context_scroll_x[tab],&a->doc_context_scroll_y[tab],"context-content")) {
        documentation_context_content(a);documentation_h_spacer(a,30);documentation_scroll_end(a);
    }
    nk_style_pop_vec2(ui);
}
static void documentation_context_content(app *a) {
    struct nk_context *ui=a->doc_ui;float scale=a->preferences.ui_size/16.f;
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    const documentation_context *context=documentation_context_for(a);
    a->doc_context_title_bounds=a->doc_context_figure_bounds=a->doc_context_table_bounds=a->doc_context_table_caption_bounds=
        a->doc_context_notice_bounds=a->doc_context_source_bounds=nk_rect(0,0,0,0);
    a->doc_context_head_bounds=a->doc_context_copy_bounds=a->doc_context_head_label_bounds=nk_rect(0,0,0,0);
    a->doc_context_table_rows=0;
    a->doc_context_title_bounds=documentation_h_text_row(a,DOC_TYPE_CONTEXT_TITLE,
        context?(a->doc_context_example?context->title:"Das Versuchsmodell"):
        (a->doc_context_example?"Kein Beispiel zu diesem Artikel":"Kein Modell zu diesem Artikel"),1.5f,0,0,14,false);
    if(!context) {
        documentation_h_text_row(a,DOC_TYPE_CONTEXT,"Der vollständige Artikel und seine verknüpften Quellen stehen im Lesebereich.",1.7f,0,0,10,true);return;
    }
    if(a->doc_context_example) {
        const char *code=documentation_context_code(a);
        if(!code){documentation_h_text_row(a,DOC_TYPE_CONTEXT,"Beispielquelle konnte nicht geladen werden.",1.7f,0,0,10,true);return;}
        const struct nk_user_font *head_font=documentation_type_font(a,DOC_TYPE_SOURCE);
        const char *language=a->doc_track==2?"Physim-Lernweg":"C-Lernweg";
        float control=fmaxf(30,head_font->height+12),copy=documentation_text_width(head_font,"Kopieren",8)+18;
        float content=ui->current->layout->bounds.w-22;
        bool wrap=documentation_text_width(head_font,language,strlen(language))+copy+8>content;
        float label_height=documentation_h_text_height(head_font,language,wrap?content:content-copy-8,16.5f*scale);
        float head_height=21+(wrap?label_height+8+control:fmaxf(label_height,control));
        ps_ui_row_exact_dynamic(ui,head_height,1);struct nk_rect head_measured=nk_widget_bounds(ui),head;
        enum nk_widget_layout_states visible=nk_widget(&head,ui);head=documentation_h_snap(head_measured);
        a->doc_context_head_bounds=head;
        nk_fill_rect(canvas,head,0,UI_COLOR(edit));nk_stroke_rect(canvas,head,0,1,UI_COLOR(divider));
        struct nk_rect button=nk_rect(wrap?head.x+11:head.x+head.w-copy-11,
            head.y+11+(wrap?label_height+8:0),copy,control);
        button=documentation_h_snap(button);
        a->doc_context_copy_bounds=button;
        struct nk_rect label_bounds=nk_rect(head.x+11,head.y+11+(wrap?0:(control-label_height)/2),wrap?content:content-copy-8,label_height);
        a->doc_context_head_label_bounds=label_bounds;
        if(visible!=NK_WIDGET_INVALID) {
            ps_ui_accessibility_label(ui,label_bounds,language);
            documentation_h_text_draw(a,head_font,language,label_bounds,16.5f*scale,UI_COLOR(code_text));
            if(ps_ui_button_at(ui,button,"Code kopieren",false))SDL_SetClipboardText(code);
            nk_fill_rect(canvas,button,3,nk_input_is_mouse_hovering_rect(&ui->input,button)?UI_COLOR(button_hover):UI_COLOR(button));
            nk_stroke_rect(canvas,button,3,1,UI_COLOR(divider));
            float label=documentation_text_width(head_font,"Kopieren",8);
            nk_draw_text(canvas,nk_rect(button.x+(button.w-label)/2,button.y+(button.h-head_font->height)/2,label+1,head_font->height),
                "Kopieren",8,head_font,nk_rgba(0,0,0,0),UI_COLOR(doc_text));
        }
        documentation_focus_draw(a,DOC_CONTEXT_COPY,button,false);
        /* Keep the complete real source and its original clipboard bytes. The
         * narrow native preview wraps source lines without removing content. */
        const struct nk_user_font *font=documentation_type_font(a,DOC_TYPE_CONTEXT_CODE);
        float line=font->height/a->doc_type_em[DOC_TYPE_CONTEXT_CODE]*1.75f;
        ps_ui_row_exact_dynamic(ui,15,1);struct nk_rect pad;nk_widget(&pad,ui);
        nk_fill_rect(canvas,pad,0,UI_COLOR(edit));
        const char *at=code;
        while(*at) {
            size_t length,used=documentation_line(font,at,strlen(at),ui->current->layout->bounds.w-22,&length);
            char text[4096];size_t n=length<sizeof text-1?length:sizeof text-1;
            memcpy(text,at,n);text[n]=0;
            ps_ui_row_exact_dynamic(ui,line,1);struct nk_rect r;visible=nk_widget(&r,ui);
            nk_fill_rect(canvas,r,0,UI_COLOR(edit));
            nk_fill_rect(canvas,nk_rect(r.x,r.y,1,r.h),0,UI_COLOR(divider));
            nk_fill_rect(canvas,nk_rect(r.x+r.w-1,r.y,1,r.h),0,UI_COLOR(divider));
            ps_ui_accessibility_label(ui,nk_rect(r.x+11,r.y,r.w-22,r.h),text);
            if(visible!=NK_WIDGET_INVALID)documentation_h_text_draw(a,font,text,nk_rect(r.x+11,r.y,r.w-22,r.h),line,UI_COLOR(code_text));
            at+=used;
        }
        ps_ui_row_exact_dynamic(ui,15,1);nk_widget(&pad,ui);nk_fill_rect(canvas,pad,0,UI_COLOR(edit));
        nk_fill_rect(canvas,nk_rect(pad.x,pad.y+pad.h-1,pad.w,1),0,UI_COLOR(divider));
        a->doc_context_source_bounds=documentation_h_text_row(a,DOC_TYPE_SOURCE,context->source[a->doc_track==2],1.7f,0,10,10,true);
        return;
    }
    int window_width,window_height;SDL_GetWindowSize(a->doc_window,&window_width,&window_height);(void)window_height;
    documentation_h_spacer(a,5);
    ps_ui_row_exact_dynamic(ui,window_width<=900?180:205,1);
    a->doc_context_figure_bounds=documentation_h_snap(nk_widget_bounds(ui));struct nk_rect figure_widget;nk_widget(&figure_widget,ui);
    ps_ui_accessibility_label(ui,a->doc_context_figure_bounds,context->title);
    documentation_context_figure(a,context->diagram,a->doc_context_figure_bounds);
    /* The reference's inline SVG also occupies the anonymous line's descent
     * below its margin box: 4 px at the context's 14 px base text size. */
    documentation_h_spacer(a,14+4*scale);
    a->doc_context_table_caption_bounds=documentation_h_text_row(a,DOC_TYPE_SOURCE,context->diagram<3?"Ausgangswerte · Vakuum":"Ausgangswerte",1.5f,0,0,8,true);
    const struct nk_user_font *labels=documentation_type_font(a,DOC_TYPE_CONTEXT_STRONG),*values=documentation_type_font(a,DOC_TYPE_CONTEXT);
    const char *at=context->values;float intrinsic[2]={1,1},width=ui->current->layout->bounds.w;
    /* Two plain columns: share available width in proportion to their actual
     * maximum-content widths, including CSS cell padding. No fixed 50% split. */
    while(*at) {
        const char *end=strchr(at,'\n');size_t n=end?(size_t)(end-at):strlen(at);const char *colon=memchr(at,':',n);
        if(colon){size_t left=(size_t)(colon-at),right=n-left-1;const char *value=colon+1;while(right && *value==' '){value++;right--;}
            intrinsic[0]=fmaxf(intrinsic[0],documentation_text_width(labels,at,left)+10);
            intrinsic[1]=fmaxf(intrinsic[1],documentation_text_width(values,value,right)+10);}
        at=end?end+1:at+n;
    }
    float column=width*intrinsic[0]/(intrinsic[0]+intrinsic[1]);a->doc_context_table_column=column;
    at=context->values;float line=18*scale;
    while(*at) {
        const char *end=strchr(at,'\n');size_t length=end?(size_t)(end-at):strlen(at);const char *colon=memchr(at,':',length);
        if(colon) {
            char text[2][128];size_t n=(size_t)(colon-at),v=length-n-1;const char *value=colon+1;while(v && *value==' '){value++;v--;}
            if(n>=sizeof text[0])n=sizeof text[0]-1;if(v>=sizeof text[1])v=sizeof text[1]-1;
            memcpy(text[0],at,n);text[0][n]=0;memcpy(text[1],value,v);text[1][v]=0;
            float height=fmaxf(documentation_h_text_height(labels,text[0],column-10,line),documentation_h_text_height(values,text[1],width-column-10,line))+21;
            ps_ui_row_exact_dynamic(ui,height,1);struct nk_rect measured=nk_widget_bounds(ui),row;
            enum nk_widget_layout_states visible=nk_widget(&row,ui);row=documentation_h_snap(measured);
            if(!a->doc_context_table_rows)a->doc_context_table_bounds=row;
            a->doc_context_table_bounds.h=row.y+row.h-a->doc_context_table_bounds.y;++a->doc_context_table_rows;
            struct nk_rect cells[]={nk_rect(row.x+5,row.y+10,column-10,row.h-21),nk_rect(row.x+column+5,row.y+10,row.w-column-10,row.h-21)};
            for(unsigned i=0;i<2;i++)ps_ui_accessibility_label(ui,cells[i],text[i]);
            if(visible!=NK_WIDGET_INVALID) {
                nk_fill_rect(canvas,nk_rect(row.x,row.y,column,row.h-1),0,UI_COLOR(header));
                documentation_h_text_draw(a,labels,text[0],cells[0],line,UI_COLOR(doc_text));
                documentation_h_text_draw(a,values,text[1],cells[1],line,UI_COLOR(doc_text));
                nk_fill_rect(canvas,nk_rect(row.x,row.y+row.h-1,row.w,1),0,UI_COLOR(divider));
            }
        }
        at=end?end+1:at+length;
    }
    documentation_h_spacer(a,22);
    float inset=16,notice_line=20.4f*scale;
    const struct nk_user_font *font=documentation_type_font(a,DOC_TYPE_CONTEXT);
    float notice_height=14+notice_line+documentation_h_text_height(font,context->hint,width-inset,notice_line);
    ps_ui_row_exact_dynamic(ui,notice_height,1);struct nk_rect notice;enum nk_widget_layout_states visible=nk_widget(&notice,ui);
    a->doc_context_notice_bounds=notice;
    ps_ui_accessibility_label(ui,notice,"Worauf achten?");ps_ui_accessibility_label(ui,notice,context->hint);
    if(visible!=NK_WIDGET_INVALID) {
        nk_fill_rect(canvas,nk_rect(notice.x,notice.y,3,notice.h),0,UI_COLOR(accent));
        documentation_h_text_draw(a,labels,"Worauf achten?",nk_rect(notice.x+inset,notice.y+7,width-inset,notice_line),notice_line,UI_COLOR(doc_text));
        documentation_h_text_draw(a,font,context->hint,nk_rect(notice.x+inset,notice.y+7+notice_line,width-inset,notice.h-notice_line-14),notice_line,UI_COLOR(doc_text));
    }
    a->doc_context_source_bounds=documentation_h_text_row(a,DOC_TYPE_SOURCE,context->article,1.7f,0,22,10,true);
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
