/* Secondary handbook commands stay outside H's standard reading layout. */
static const int documentation_actions[]={DOC_BACK,DOC_FORWARD,DOC_HOME,DOC_WORKSPACE,DOC_SEARCH,DOC_ZOOM_OUT,DOC_ZOOM_RESET,DOC_ZOOM_IN};
static void documentation_overlay_focus(struct nk_context *ui,const char *name) {
    struct nk_window *owner=nk_window_find(ui,name);
    if(!owner)return;
    /* set_focus raises the window but does not clear its previous ROM flag.
     * Only this explicitly active overlay is made interactive. */
    nk_window_set_focus(ui,name);owner->flags&=~(nk_flags)NK_WINDOW_ROM;
}
static bool documentation_action_enabled(app *a,int id) {
    return id==DOC_BACK?ps_doc_history_peek(&a->doc_history,-1)!=NULL:
        id==DOC_FORWARD?ps_doc_history_peek(&a->doc_history,1)!=NULL:
        id==DOC_ZOOM_OUT?documentation_font_size(a)>16:id==DOC_ZOOM_IN?documentation_font_size(a)<32:true;
}
static unsigned documentation_action_ids(app *a,int ids[8]) {
    unsigned count=0;for(unsigned i=0;i<SDL_arraysize(documentation_actions);i++)
        if(documentation_action_enabled(a,documentation_actions[i]))ids[count++]=documentation_actions[i];
    return count;
}
static void documentation_actions_close(app *a) {
    a->doc_actions_open=false;a->doc_keyboard=a->doc_actions_return_keyboard;
    a->doc_focus=a->doc_actions_return_focus;a->doc_focus_reveal=a->doc_keyboard;
    nk_window_show(a->doc_ui,"documentation-actions",NK_HIDDEN);
    nk_window_set_focus(a->doc_ui,"document");
}
static void documentation_actions_open(app *a,float x,float y) {
    if(!a->doc_actions_open) {
        a->doc_actions_return_focus=a->doc_focus;
        a->doc_actions_return_keyboard=a->doc_keyboard;
    }
    documentation_finish_edits(a);
    a->doc_actions_position=nk_vec2(x,y);a->doc_actions_open=true;
    a->doc_actions_wait_release=true;
    int ids[8];unsigned count=documentation_action_ids(a,ids);
    a->doc_focus=count?ids[0]:DOC_HOME;a->doc_keyboard=a->doc_focus_reveal=true;
    nk_sdl_accessibility_keyboard_begin(a->doc_ui);
}
static void documentation_action_activate(app *a,int id) {
    documentation_actions_close(a);
    a->doc_keyboard=true;a->doc_focus=DOC_BODY;a->doc_focus_reveal=true;
    if(id==DOC_BACK || id==DOC_FORWARD)documentation_history_go(a,id==DOC_BACK?-1:1);
    else if(id==DOC_HOME)open_documentation(a,documentation_learning_topic(a->doc_track==2?2:1));
    else if(id==DOC_WORKSPACE)SDL_RaiseWindow(a->window);
    else if(id==DOC_SEARCH){a->doc_find_open=true;a->doc_search_focus=true;a->doc_focus=DOC_SEARCH;}
    else documentation_zoom(a,id==DOC_ZOOM_OUT?-1:id==DOC_ZOOM_IN?1:0);
}
static void documentation_actions_tick(app *a) {
    int id=a->doc_action_pending;a->doc_action_pending=0;
    if(id && a->doc_visible && documentation_action_enabled(a,id))documentation_action_activate(a,id);
}
static bool documentation_point_in(struct nk_rect r,float x,float y) {
    return r.w>0 && r.h>0 && x>=r.x && x<r.x+r.w && y>=r.y && y<r.y+r.h;
}
static bool documentation_actions_event(app *a,const SDL_Event *event) {
    if(!a->doc_visible)return false;
    if(event->type==SDL_EVENT_WINDOW_FOCUS_LOST || event->type==SDL_EVENT_WINDOW_RESIZED) {
        if(a->doc_actions_open)documentation_actions_close(a);
        return false;
    }
    if(event->type==SDL_EVENT_MOUSE_BUTTON_DOWN) {
        float x=event->button.x,y=event->button.y;
        if(event->button.button==SDL_BUTTON_RIGHT &&
           (documentation_point_in(a->doc_body_bounds,x,y) || documentation_point_in(a->doc_actions_trigger_bounds,x,y)) &&
           !(a->doc_find_open && documentation_point_in(a->doc_find_bounds,x,y))) {
            documentation_actions_open(a,x,y);return true;
        }
        if(a->doc_actions_open && !documentation_point_in(a->doc_actions_bounds,x,y)) {
            documentation_actions_close(a);return true;
        }
    }
    if(a->doc_actions_open && event->type==SDL_EVENT_MOUSE_WHEEL)return true;
    if(event->type!=SDL_EVENT_KEY_DOWN)return false;
    const SDL_KeyboardEvent *key=&event->key;
    if(key->key==SDLK_APPLICATION || (key->key==SDLK_F10 && (key->mod&SDL_KMOD_SHIFT))) {
        if(!key->repeat) {
            if(a->doc_actions_open)documentation_actions_close(a);
            else documentation_actions_open(a,a->doc_body_bounds.x+12,a->doc_body_bounds.y+12);
        }
        return true;
    }
    if(!a->doc_actions_open)return false;
    if(key->key==SDLK_ESCAPE){documentation_actions_close(a);return true;}
    if(key->mod&(SDL_KMOD_CTRL|SDL_KMOD_GUI|SDL_KMOD_ALT)) {documentation_actions_close(a);return false;}
    int ids[8];unsigned count=documentation_action_ids(a,ids),current=0;
    for(unsigned i=0;i<count;i++)if(ids[i]==a->doc_focus)current=i;
    int step=key->key==SDLK_UP?-1:key->key==SDLK_DOWN?1:key->key==SDLK_TAB?(key->mod&SDL_KMOD_SHIFT?-1:1):0;
    if(step || key->key==SDLK_HOME || key->key==SDLK_END) {
        if(count)a->doc_focus=ids[key->key==SDLK_HOME?0:key->key==SDLK_END?count-1:(current+count+step)%count];
        a->doc_keyboard=a->doc_focus_reveal=true;nk_sdl_accessibility_keyboard_begin(a->doc_ui);return true;
    }
    if((key->key==SDLK_RETURN || key->key==SDLK_KP_ENTER || key->key==SDLK_SPACE) && !key->repeat && documentation_action_enabled(a,a->doc_focus))
        documentation_action_activate(a,a->doc_focus);
    return true;
}
static void documentation_find_view(app *a) {
    struct nk_context *ui=a->doc_ui;
    nk_window_show(ui,"documentation-find",a->doc_find_open?NK_SHOWN:NK_HIDDEN);
    if(!a->doc_find_open){a->doc_search_bounds=a->doc_find_bounds=nk_rect(0,0,0,0);return;}
    const struct nk_user_font *font=documentation_chrome_font(a);
    float control=fmaxf(32,font->height+12),width=fminf(520,a->doc_article_slot.w-16);
    a->doc_find_bounds=nk_rect(a->doc_article_slot.x+a->doc_article_slot.w-width-8,a->doc_article_slot.y+8,width,control+16);
    nk_style_push_font(ui,font);nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(8,8));
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(4,4));
    nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(UI_COLOR(header)));
    if(!a->doc_actions_open && (a->doc_search_focus || a->doc_search_active || (a->doc_keyboard && a->doc_focus==DOC_SEARCH)))
        documentation_overlay_focus(ui,"documentation-find");
    if(nk_begin(ui,"documentation-find",a->doc_find_bounds,NK_WINDOW_BORDER|NK_WINDOW_NO_SCROLLBAR)) {
        ps_ui_accessibility_route(ui,true);
        float available=nk_window_get_content_region(ui).w;
        ps_ui_row_exact_begin(ui,NK_STATIC,control,4);
        nk_layout_row_push(ui,fmaxf(20,available-96-3*ui->style.window.spacing.x));
        if(a->doc_keyboard && a->doc_focus!=DOC_SEARCH)nk_edit_unfocus(ui);
        if(a->doc_search_focus || (a->doc_keyboard && a->doc_focus==DOC_SEARCH && !ui->current->edit.active)) {
            nk_edit_focus(ui,NK_EDIT_ALWAYS_INSERT_MODE);a->doc_search_focus=false;
        }
        struct nk_rect r=nk_widget_bounds(ui);a->doc_search_bounds=r;
        nk_flags edit=nk_sdl_edit_named(ui,"Aktuelles Dokument durchsuchen",NK_EDIT_FIELD,a->doc_query,sizeof a->doc_query,nk_filter_default);
        if(edit&PS_UI_EDIT_NATIVE_FOCUS){a->doc_focus=DOC_SEARCH;a->doc_keyboard=true;}
        a->doc_search_active=(edit&NK_EDIT_ACTIVE)!=0;if(a->doc_search_active)a->doc_filter_active=false;
        documentation_focus_draw(a,DOC_SEARCH,r,false);
        const int ids[]={DOC_PREVIOUS,DOC_NEXT,DOC_FIND_CLOSE};
        const char *labels[]={"Vorheriger Treffer","Nächster Treffer","Artikelsuche schließen"};
        const ps_ui_icon icons[]={PS_ICON_LEFT,PS_ICON_RIGHT,PS_ICON_CLOSE};
        for(unsigned i=0;i<3;i++) {
            nk_layout_row_push(ui,32);r=nk_widget_bounds(ui);
            if(ps_ui_icon_button(ui,icons[i],labels[i])) {
                if(i<2){a->doc_match+=i?1:-1;a->doc_move_match=true;}
                else {documentation_finish_edits(a);a->doc_find_open=false;a->doc_keyboard=true;a->doc_focus=DOC_BODY;a->doc_focus_reveal=true;}
            }
            documentation_focus_draw(a,ids[i],r,false);
        }
        nk_layout_row_end(ui);
    }nk_end(ui);
    nk_style_pop_style_item(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);nk_style_pop_font(ui);
}
static void documentation_actions_view(app *a) {
    struct nk_context *ui=a->doc_ui;
    nk_window_show(ui,"documentation-actions",a->doc_actions_open?NK_SHOWN:NK_HIDDEN);
    if(!a->doc_actions_open)return;
    const char *labels[]={"Zurück","Vorwärts","Start","Arbeitsbereich","Aktuelles Dokument durchsuchen","Leseschrift verkleinern","Standardleseschrift","Leseschrift vergrößern"};
    const char *shortcuts[]={"Alt+←","Alt+→","","","" PS_UI_SHORTCUT "F","" PS_UI_SHORTCUT "−","" PS_UI_SHORTCUT "0","" PS_UI_SHORTCUT "+"};
    const struct nk_user_font *font=documentation_chrome_font(a);float row=fmaxf(28,font->height+10),width=340;
    for(unsigned i=0;i<SDL_arraysize(labels);i++)width=fmaxf(width,documentation_text_width(font,labels[i],strlen(labels[i]))+documentation_text_width(font,shortcuts[i],strlen(shortcuts[i]))+40);
    int w,h;SDL_GetWindowSize(a->doc_window,&w,&h);width=fminf(width,w-16);
    float height=16+SDL_arraysize(labels)*(row+2);
    a->doc_actions_bounds=nk_rect(fmaxf(8,fminf(w-width-8,a->doc_actions_position.x)),fmaxf(8,fminf(h-height-8,a->doc_actions_position.y)),width,height);
    nk_style_push_font(ui,font);nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(8,8));
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(4,2));
    nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(UI_COLOR(window)));
    documentation_overlay_focus(ui,"documentation-actions");
    if(nk_begin(ui,"documentation-actions",a->doc_actions_bounds,NK_WINDOW_BORDER|NK_WINDOW_NO_SCROLLBAR)) {
        ps_ui_accessibility_route(ui,true);
        for(unsigned i=0;i<SDL_arraysize(labels);i++) {
            int id=documentation_actions[i];bool enabled=documentation_action_enabled(a,id);
            ps_ui_row_exact_dynamic(ui,row,1);struct nk_rect r;nk_widget(&r,ui);
            if(id==DOC_HOME)a->doc_home_bounds=r;
            bool focused=a->doc_keyboard && a->doc_focus==id,hover=nk_input_is_mouse_hovering_rect(&ui->input,r);
            if(enabled && (hover || focused))nk_fill_rect(nk_window_get_canvas(ui),r,4,UI_COLOR(button_hover));
            nk_begin_disabled(ui,!enabled);bool pressed=ps_ui_button_at(ui,r,labels[i],false);nk_end_disabled(ui);
            if(id==DOC_HOME && SDL_getenv("PHYSIM_TEST_TRACE") &&
               (nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT) || nk_input_is_mouse_released(&ui->input,NK_BUTTON_LEFT)))
                fprintf(stderr,"DOCUMENTATION MENU POINTER: pos=%.1f,%.1f start=%.1f,%.1f,%.1f,%.1f hover=%d pressed=%d wait-release=%d flags=%u layout=%u active=%s\n",
                    ui->input.mouse.pos.x,ui->input.mouse.pos.y,r.x,r.y,r.w,r.h,hover,pressed,a->doc_actions_wait_release,
                    (unsigned)ui->current->flags,(unsigned)ui->current->layout->flags,ui->active?ui->active->name_string:"none");
            float shortcut_width=documentation_text_width(font,shortcuts[i],strlen(shortcuts[i]));
            struct nk_color color=enabled?UI_COLOR(text):UI_COLOR(muted);
            float y=r.y+(r.h-font->height)/2;
            nk_draw_text(nk_window_get_canvas(ui),nk_rect(r.x+6,y,r.w-shortcut_width-24,font->height),labels[i],(int)strlen(labels[i]),font,nk_rgba(0,0,0,0),color);
            nk_draw_text(nk_window_get_canvas(ui),nk_rect(r.x+r.w-shortcut_width-6,y,shortcut_width+1,font->height),shortcuts[i],(int)strlen(shortcuts[i]),font,nk_rgba(0,0,0,0),UI_COLOR(muted));
            documentation_focus_draw(a,id,r,false);
            if(enabled && pressed && !a->doc_actions_wait_release) {
                /* Font demand can append an atlas and reset Nuklear's style
                 * stack. Execute commands before the next input/build frame. */
                a->doc_action_pending=id;documentation_actions_close(a);break;
            }
        }
    }nk_end(ui);
    nk_style_pop_style_item(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);nk_style_pop_font(ui);
    if(!nk_input_is_mouse_down(&ui->input,NK_BUTTON_LEFT) && !nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT) &&
       !nk_input_is_mouse_released(&ui->input,NK_BUTTON_LEFT))a->doc_actions_wait_release=false;
    if(a->doc_actions_open)documentation_overlay_focus(ui,"documentation-actions");
}
