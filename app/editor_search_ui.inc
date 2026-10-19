/* One nonmodal island shared by every real text editor. No layout rows are
 * inserted in the document. The window bounds are exactly its hit region. */
typedef enum { SEARCH_LENS, SEARCH_EXPAND, SEARCH_COLLAPSE, SEARCH_PREVIOUS,
               SEARCH_NEXT, SEARCH_CLOSE, SEARCH_REPLACE } search_icon;
/* Float geometry preserves the reference's 1.5 px strokes; ordinary Nuklear
 * line commands quantize their thickness to an unsigned short. */
static void search_line(struct nk_draw_list *c,float x0,float y0,float x1,float y1,float t,struct nk_color color) {
    nk_draw_list_stroke_line(c,nk_vec2(x0,y0),nk_vec2(x1,y1),color,t);
    nk_draw_list_fill_circle(c,nk_vec2(x0,y0),t*.5f,color,12);
    nk_draw_list_fill_circle(c,nk_vec2(x1,y1),t*.5f,color,12);
}
static void search_icon_render(void *canvas,short bx,short by,unsigned short bw,unsigned short bh,nk_handle data) {
    struct nk_draw_list *c=canvas;unsigned packed=(unsigned)data.id;
    search_icon icon=(search_icon)(packed&15);
    struct nk_color color=nk_rgb((packed>>4)&255,(packed>>12)&255,(packed>>20)&255);
    float x=bx+(bw-16)*.5f,y=by+(bh-16)*.5f,t=1.5f;
    if(icon==SEARCH_LENS) {
        nk_draw_list_stroke_circle(c,nk_vec2(x+7,y+7),4.5f,color,32,t);
        search_line(c,x+10.4f,y+10.4f,x+14,y+14,t,color);
    } else if(icon==SEARCH_CLOSE) {
        search_line(c,x+4.5f,y+4.5f,x+11.5f,y+11.5f,t,color);
        search_line(c,x+11.5f,y+4.5f,x+4.5f,y+11.5f,t,color);
    } else if(icon==SEARCH_REPLACE) {
        search_line(c,x+3,y+5,x+12,y+5,t,color);
        search_line(c,x+12,y+5,x+10,y+3,t,color);search_line(c,x+12,y+5,x+10,y+7,t,color);
        search_line(c,x+13,y+11,x+4,y+11,t,color);
        search_line(c,x+4,y+11,x+6,y+9,t,color);search_line(c,x+4,y+11,x+6,y+13,t,color);
    } else {
        float p[6];
        if(icon==SEARCH_EXPAND){float v[]={6,4,10,8,6,12};memcpy(p,v,sizeof p);}
        else if(icon==SEARCH_PREVIOUS){float v[]={4,10,8,6,12,10};memcpy(p,v,sizeof p);}
        else {float v[]={4,6,8,10,12,6};memcpy(p,v,sizeof p);}
        search_line(c,x+p[0],y+p[1],x+p[2],y+p[3],t,color);
        search_line(c,x+p[2],y+p[3],x+p[4],y+p[5],t,color);
    }
}
static void search_draw_icon(struct nk_command_buffer *c,search_icon icon,struct nk_rect b,struct nk_color color) {
    unsigned packed=(unsigned)icon|((unsigned)color.r<<4)|((unsigned)color.g<<12)|((unsigned)color.b<<20);
    nk_push_custom(c,b,search_icon_render,nk_handle_id((int)packed));
}
static bool search_icon_at(app *a,search_icon icon,struct nk_rect b,const char *label,struct nk_color text,struct nk_color fill,bool surface) {
    struct nk_context *ui=a->ui;struct nk_command_buffer *c=nk_window_get_canvas(ui);
    bool hover=nk_input_is_mouse_hovering_rect(&ui->input,b);
    if(hover)nk_fill_rect(c,b,surface?9:8,nk_input_is_mouse_down(&ui->input,NK_BUTTON_LEFT)?UI_COLOR(button_active):surface?fill:UI_COLOR(button_hover));
    search_draw_icon(c,icon,b,text);
    bool clicked=ps_ui_button_at(ui,b,label,false);
    if(clicked || nk_sdl_accessibility_is_focused(ui,NULL,label))a->search_keyboard=true;
    if(hover && !nk_input_is_mouse_down(&ui->input,NK_BUTTON_LEFT))nk_tooltip(ui,label);
    if(clicked && (icon==SEARCH_PREVIOUS || icon==SEARCH_NEXT) && nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT))nk_sdl_accessibility_request_focus(ui,NULL,label);
    return clicked;
}
static void editor_search_close(app *a) {
    a->show_search=false;a->search_keyboard=false;a->search_focus=false;a->search_icon_focus=true;
}
static bool editor_search_key(app *a,const SDL_KeyboardEvent *key) {
    struct nk_text_edit *edit=editor_search_target(a);
    if(!edit || a->toolbar_menu)return false;
    if(key->key==SDLK_F && (key->mod&PS_UI_COMMAND_MOD)) {
        if(!key->repeat)editor_search_open(a);
        return true;
    }
    if(!a->show_search || !(a->search_keyboard || a->search_focus))return false;
    if(key->key==SDLK_ESCAPE){editor_search_close(a);return true;}
    bool editing=a->ui->active && !strcmp(a->ui->active->name_string,"editor-search") && a->ui->active->edit.active;
    if(key->key==SDLK_TAB && editing) {
        const char *next=a->search_field==2?(key->mod&SDL_KMOD_SHIFT?"Dokumentsuche schließen":"Treffer ersetzen"):
            key->mod&SDL_KMOD_SHIFT?(a->search_replace_open?"Ersetzung einklappen":"Ersetzung aufklappen"):"Vorheriger Treffer";
        nk_sdl_accessibility_request_focus(a->ui,NULL,next);return true;
    }
    if(editing && (key->key==SDLK_RETURN || key->key==SDLK_KP_ENTER)) {
        /* The SDL text event may precede Enter in the same frame. */
        ps_ui_flush_edit(a->ui,"editor-search",a->search_field==2?a->replace:a->find,sizeof a->find,
                         a->search_field==2?a->search_replace_field_bounds:a->search_query_bounds);
        find_text_direction(a,edit,false,(key->mod&SDL_KMOD_SHIFT)!=0);
        return true;
    }
    return false;
}
static void editor_search_view(app *a) {
    struct nk_context *ui=a->ui;
    struct nk_text_edit *edit=editor_search_target(a);
    bool visible=a->show_search && edit && edit==a->search_edit && a->dock_visible[PS_DOCK_WORKSPACE];
    ps_graphics_backdrop(a->graphics,NULL);
    nk_window_show(ui,"editor-search",visible?NK_SHOWN:NK_HIDDEN);
    if(!visible){a->search_island_bounds=nk_rect(0,0,0,0);return;}
    /* Hover routing may temporarily make the code pane current. Keyboard
     * ownership changes on an explicit click/focus action, not mouse motion. */
    a->search_keyboard=a->search_keyboard || a->search_focus;
    struct nk_rect source=a->source_editor_frame;
    float row=fmaxf(30,ui->style.font->height+10),width=fminf(540,source.w-16);
    float footer_height=fmaxf(17,ui->style.font->height+2);
    float height=14+row+5+footer_height+(a->search_replace_open?row+5:0);
    struct nk_rect island=nk_rect(source.x+source.w-8-width,source.y+4,width,height);
    a->search_island_bounds=island;
    bool light=a->preferences.theme==PS_THEME_LIGHT,contrast=a->preferences.theme==PS_THEME_HIGH_CONTRAST;
    bool opaque=contrast || nk_sdl_reduce_transparency(ui);
    struct nk_color text=opaque?UI_COLOR(text):light?nk_rgb(24,42,57):nk_rgb(237,244,252);
    struct nk_color muted_color=opaque?UI_COLOR(muted):light?nk_rgb(52,75,96):nk_rgb(207,223,239);
    struct nk_color field=opaque?UI_COLOR(edit):light?nk_rgba(244,250,255,173):nk_rgba(21,31,46,173);
    struct nk_color button=opaque?UI_COLOR(button):light?nk_rgba(239,247,255,219):nk_rgba(31,45,61,230);
    struct nk_color footer=opaque?UI_COLOR(window):light?nk_rgba(245,250,255,235):nk_rgba(18,28,41,230);
    struct nk_color edge=opaque?UI_COLOR(border):light?nk_rgba(141,171,204,153):nk_rgba(196,220,247,102);
    nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(7,7));
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(5,5));
    nk_style_push_vec2(ui,&ui->style.edit.padding,nk_vec2(8,4));
    nk_style_push_float(ui,&ui->style.window.rounding,16);
    nk_style_push_float(ui,&ui->style.edit.rounding,9);
    nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(nk_rgba(0,0,0,0)));
    nk_style_push_style_item(ui,&ui->style.edit.normal,nk_style_item_color(field));
    nk_style_push_style_item(ui,&ui->style.edit.hover,nk_style_item_color(field));
    nk_style_push_style_item(ui,&ui->style.edit.active,nk_style_item_color(field));
    nk_style_push_color(ui,&ui->style.edit.text_normal,text);
    nk_style_push_color(ui,&ui->style.edit.text_hover,text);
    nk_style_push_color(ui,&ui->style.edit.text_active,text);
    struct nk_window *previous=ui->active;
    if(nk_begin(ui,"editor-search",island,NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BACKGROUND)) {
        nk_sdl_window_raise(ui,"editor-search");
        struct nk_command_buffer *c=nk_window_get_canvas(ui);
        bool pointer=nk_input_is_mouse_hovering_rect(&ui->input,island);
        if(nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT) && !pointer) {
            a->search_keyboard=false;nk_edit_unfocus(ui);
        }
        bool blocked=a->toolbar_menu || a->toolbar_pointer_consumed || a->completion_active;
        bool owns=pointer || a->search_keyboard || a->search_focus;
        if(blocked || !owns){ui->current->flags|=NK_WINDOW_ROM;ui->current->layout->flags|=NK_WINDOW_ROM;}
        else {ui->current->flags&=~(nk_flags)NK_WINDOW_ROM;ui->current->layout->flags&=~(nk_flags)NK_WINDOW_ROM;}
        ps_ui_accessibility_route(ui,!blocked);
        if(!blocked && (a->search_keyboard || a->search_focus || (pointer && nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT))))ui->active=ui->current;
        else ui->active=previous;
        struct nk_rect clip=c->clip;
        nk_push_scissor(c,nk_rect(island.x-22,island.y-15,island.w+44,island.h+44));
        for(int ring=22;ring>0;--ring) {
            float spread=(float)ring;
            int alpha=(int)lroundf(26*erfcf(spread/15.55635f));
            nk_stroke_rect(c,nk_rect(island.x-spread,island.y+7-spread,island.w+2*spread,island.h+2*spread),16+spread,1,nk_rgba(0,0,0,alpha));
        }
        nk_push_scissor(c,island);
        a->search_material=(ps_ui_backdrop){island.x,island.y,island.w,island.h,
            contrast?UI_COLOR(window).r:light?232:36,contrast?UI_COLOR(window).g:light?243:53,
            contrast?UI_COLOR(window).b:light?255:73,light,opaque,
            {UI_COLOR(window).r,UI_COLOR(window).g,UI_COLOR(window).b}};
        ps_graphics_backdrop(a->graphics,&a->search_material);
        ps_ui_draw_backdrop(ui,&a->search_material);
        nk_stroke_rect(c,island,16,1,edge);
        nk_stroke_line(c,island.x+16,island.y+1,island.x+island.w-16,island.y+1,1,nk_rgba(255,255,255,24));
        nk_push_scissor(c,clip);
        float x=island.x+7,y=island.y+7,inner=width-14;
        float small=width<330?25:28,nav=width<330?25:27;
        float query_w=fmaxf(32,inner-small*2-nav*2-15);
        a->search_expand_bounds=nk_rect(x,y,small,row);
        if(search_icon_at(a,a->search_replace_open?SEARCH_COLLAPSE:SEARCH_EXPAND,a->search_expand_bounds,
                          a->search_replace_open?"Ersetzung einklappen":"Ersetzung aufklappen",text,button,true)) {
            a->search_replace_open=!a->search_replace_open;a->search_focus=true;
        }
        x+=small+5;
        a->search_query_bounds=nk_rect(x,y,query_w,row);
        nk_fill_rect(c,a->search_query_bounds,9,field);
        nk_stroke_rect(c,a->search_query_bounds,9,1,edge);
        search_draw_icon(c,SEARCH_LENS,nk_rect(x+5,y,20,row),text);
        a->search_query_bounds.x+=28;a->search_query_bounds.w=fmaxf(4,query_w-28);
        nk_layout_space_begin(ui,NK_STATIC,height-14,4);
        nk_layout_space_push(ui,nk_rect(x-island.x-7+28,0,fmaxf(4,query_w-28),row));
        nk_style_push_style_item(ui,&ui->style.edit.normal,nk_style_item_color(nk_rgba(0,0,0,0)));
        nk_style_push_style_item(ui,&ui->style.edit.hover,nk_style_item_color(nk_rgba(0,0,0,0)));
        nk_style_push_style_item(ui,&ui->style.edit.active,nk_style_item_color(nk_rgba(0,0,0,0)));
        if(a->search_focus){nk_edit_focus(ui,NK_EDIT_ALWAYS_INSERT_MODE);a->search_focus=false;}
        nk_flags state=nk_sdl_edit_named(ui,"Dokument durchsuchen",NK_EDIT_FIELD,a->find,sizeof a->find,nk_filter_default);
        nk_style_pop_style_item(ui);nk_style_pop_style_item(ui);nk_style_pop_style_item(ui);
        if((state&NK_EDIT_ACTIVE) && ui->active==ui->current){a->search_keyboard=true;a->search_field=1;}
        x+=query_w+5;
        struct nk_rect navigation=nk_rect(x,y,nav*2,row);
        nk_fill_rect(c,navigation,9,button);
        nk_stroke_rect(c,navigation,9,1,edge);
        nk_stroke_line(c,x+nav,y+1,x+nav,y+row-1,1,edge);
        a->search_previous_bounds=nk_rect(x,y,nav,row);
        if(search_icon_at(a,SEARCH_PREVIOUS,a->search_previous_bounds,"Vorheriger Treffer",text,button,false))find_text_direction(a,edit,false,true);
        a->search_bounds=nk_rect(x+nav,y,nav,row);
        if(search_icon_at(a,SEARCH_NEXT,a->search_bounds,"Nächster Treffer",text,button,false))find_text(a,edit,false);
        x+=nav*2+5;
        a->search_close_bounds=nk_rect(x,y,small,row);
        if(search_icon_at(a,SEARCH_CLOSE,a->search_close_bounds,"Dokumentsuche schließen",text,button,true))editor_search_close(a);
        if(a->search_replace_open) {
            y+=row+5;float action=fminf(130,inner*.4f);
            nk_layout_space_push(ui,nk_rect(small+5,row+5,inner-small-10-action,row));
            a->search_replace_field_bounds=nk_widget_bounds(ui);
            state=nk_sdl_edit_named(ui,"Ersetzen durch",NK_EDIT_FIELD,a->replace,sizeof a->replace,nk_filter_default);
            if((state&NK_EDIT_ACTIVE) && ui->active==ui->current){a->search_keyboard=true;a->search_field=2;}
            a->replace_bounds=nk_rect(island.x+7+inner-action,y,action,row);
            nk_fill_rect(c,a->replace_bounds,9,button);
            search_draw_icon(c,SEARCH_REPLACE,nk_rect(a->replace_bounds.x+5,y,20,row),text);
            nk_draw_text(c,nk_rect(a->replace_bounds.x+28,y,action-30,row),"Ersetzen",8,ui->style.font,nk_rgba(0,0,0,0),text);
            if(ps_ui_button_at(ui,a->replace_bounds,"Treffer ersetzen",false)){a->search_keyboard=true;find_text(a,edit,true);}
            if(nk_sdl_accessibility_is_focused(ui,NULL,"Treffer ersetzen"))a->search_keyboard=true;
        } else a->replace_bounds=nk_rect(0,0,0,0);
        nk_layout_space_end(ui);
        ps_editor_search_result result;size_t cursor=editor_search_byte(edit,edit->select_start);
        ps_editor_search(nk_str_get_const(&edit->string),(size_t)nk_str_len_char(&edit->string),a->find,strlen(a->find),cursor,false,&result);
        char count[96];snprintf(count,sizeof count,a->find[0]?"%zu von %zu Treffern":"Dokument durchsuchen",result.count?result.ordinal:0,result.count);
        struct nk_rect status_rect=nk_rect(island.x+7,island.y+height-7-footer_height,inner,footer_height);
        const char *hint="Aktuelle Datei · Escape schließt";
        float hint_width=ui->style.font->width(ui->style.font->userdata,ui->style.font->height,hint,(int)strlen(hint));
        float count_width=ui->style.font->width(ui->style.font->userdata,ui->style.font->height,count,(int)strlen(count));
        nk_fill_rect(c,nk_rect(status_rect.x,status_rect.y,fminf(inner,count_width+10),status_rect.h),6,footer);
        nk_draw_text(c,nk_rect(status_rect.x+5,status_rect.y,fminf(inner-10,count_width),status_rect.h),count,(int)strlen(count),ui->style.font,nk_rgba(0,0,0,0),muted_color);
        if(hint_width+count_width+24<inner) {
            nk_fill_rect(c,nk_rect(status_rect.x+inner-hint_width-10,status_rect.y,hint_width+10,status_rect.h),6,footer);
            nk_draw_text(c,nk_rect(status_rect.x+inner-hint_width-5,status_rect.y,hint_width,status_rect.h),hint,(int)strlen(hint),ui->style.font,nk_rgba(0,0,0,0),muted_color);
        }
        ps_ui_accessibility_label(ui,status_rect,count);
    }
    nk_end(ui);
    for(int i=0;i<3;i++)nk_style_pop_color(ui);
    for(int i=0;i<4;i++)nk_style_pop_style_item(ui);
    for(int i=0;i<2;i++)nk_style_pop_float(ui);
    for(int i=0;i<3;i++)nk_style_pop_vec2(ui);
    nk_sdl_window_raise(ui,"editor-search");
}
