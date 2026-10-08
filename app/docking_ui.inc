static const char *dock_names[] = {"Seitenleiste", "Arbeitsbereich", "Protokoll", "Inspektor"};
static const char *dock_windows[] = {"sidebar", "workspace", "log", "inspector"};
static uint32_t dock_mask(const ps_dock_layout *d, uint32_t id) {
    const ps_dock_node *n=&d->nodes[id];
    return n->kind==PS_DOCK_GROUP?n->panels:dock_mask(d,n->first)|dock_mask(d,n->second);
}
static bool dock_single(const ps_dock_layout *d,uint32_t id,uint32_t panel) {
    return d->nodes[id].kind==PS_DOCK_GROUP && d->nodes[id].panels==(1u<<panel);
}
static bool dock_sidebar_split(const ps_dock_layout *d,const ps_dock_node *n) {
    if(n->kind!=PS_DOCK_X || !dock_single(d,n->first,PS_DOCK_SIDEBAR))return false;
    uint32_t mask=dock_mask(d,n->second);
    return mask==2 || mask==10;
}
static bool dock_log_split(const ps_dock_layout *d,const ps_dock_node *n) {
    if(n->kind!=PS_DOCK_Y || !dock_single(d,n->second,PS_DOCK_LOG))return false;
    uint32_t mask=dock_mask(d,n->first);
    return mask==3 || mask==11;
}
static float dock_minimum(const ps_dock_layout *d,uint32_t id,bool x) {
    const ps_dock_node *n=&d->nodes[id];
    if(n->kind==PS_DOCK_GROUP) return 64;
    float one=dock_minimum(d,n->first,x),two=dock_minimum(d,n->second,x);
    return (n->kind==PS_DOCK_X)==x?one+two+8:fmaxf(one,two);
}
static void dock_arrange(app *a,uint32_t id,struct nk_rect r) {
    ps_dock_layout *d=&a->preferences.dock;
    ps_dock_node *n=&d->nodes[id]; a->dock_rects[id]=r;
    if(n->kind==PS_DOCK_GROUP) {
        for(unsigned p=0;p<PS_DOCK_PANELS;p++) if(n->panels&(1u<<p)) {
            a->dock_panels[p]=r; a->dock_visible[p]=n->active==p;
        }
        return;
    }
    bool x=n->kind==PS_DOCK_X;
    float extent=(x?r.w:r.h)-8,first=extent*n->ratio/1024.f;
    if(dock_sidebar_split(d,n))
        first=(float)a->preferences.sidebar_width;
    if(x && dock_single(d,n->first,PS_DOCK_WORKSPACE) && dock_single(d,n->second,PS_DOCK_INSPECTOR))
        first=extent-(float)a->preferences.inspector_width;
    if(dock_log_split(d,n))
        first=extent-(a->show_log&&a->tab!=6?(float)a->preferences.log_height-8:68.f);
    first=fmaxf(dock_minimum(d,n->first,x),fminf(extent-dock_minimum(d,n->second,x),first));
    struct nk_rect one=r,two=r,split=r;
    if(x) { one.w=first; split.x=r.x+first;split.w=8;two.x=split.x+8;two.w=extent-first; }
    else { one.h=first;split.y=r.y+first;split.h=8;two.y=split.y+8;two.h=extent-first; }
    a->dock_splitters[id]=split;dock_arrange(a,n->first,one);dock_arrange(a,n->second,two);
}
static ps_dock_float dock_clamp_float(unsigned panel,struct nk_rect r,float w,float h) {
    float minw=panel==PS_DOCK_WORKSPACE?640:208,minh=panel==PS_DOCK_WORKSPACE?580:120;
    r.w=fmaxf(minw,fminf(w-16,r.w));r.h=fmaxf(minh,fminf(h-80,r.h));
    r.x=fmaxf(8,fminf(w-r.w-8,r.x));r.y=fmaxf(72,fminf(h-r.h-8,r.y));
    return (ps_dock_float){(uint32_t)r.x,(uint32_t)r.y,(uint32_t)r.w,(uint32_t)r.h};
}
static void dock_show(app *a,unsigned panel) {
    if(panel==PS_DOCK_INSPECTOR && (a->preferences.dock.hidden&(1u<<panel)) &&
       ps_dock_group(&a->preferences.dock,PS_DOCK_WORKSPACE)!=PS_DOCK_NONE) {
        ps_dock_move(&a->preferences.dock,panel,PS_DOCK_WORKSPACE,PS_DOCK_RIGHT);return;
    }
    ps_dock_reveal(&a->preferences.dock,panel,panel==PS_DOCK_SIDEBAR?PS_DOCK_LEFT:panel==PS_DOCK_LOG?PS_DOCK_BOTTOM:PS_DOCK_TAB);
}
static void dock_relayout(app *a,float w,float h) {
    ps_dock_layout *d=&a->preferences.dock;
    memset(a->dock_visible,0,sizeof a->dock_visible);memset(a->dock_splitters,0,sizeof a->dock_splitters);
    if(d->root!=PS_DOCK_NONE) dock_arrange(a,d->root,nk_rect(8,72,w-16,h-80));
    for(unsigned p=0;p<PS_DOCK_PANELS;p++) if(d->floating&(1u<<p)) {
        ps_dock_float *r=&d->floats[p];a->dock_panels[p]=nk_rect((float)r->x,(float)r->y,(float)r->w,(float)r->h);a->dock_visible[p]=true;
    }
    a->panel_bounds[0]=a->panel_bounds[1]=nk_rect(0,0,0,0);
    for(unsigned i=0;i<PS_DOCK_NODES;i++) {
        ps_dock_node *n=&d->nodes[i];
        if(dock_sidebar_split(d,n)) a->panel_bounds[0]=a->dock_splitters[i];
        if(dock_log_split(d,n)) a->panel_bounds[1]=a->dock_splitters[i];
    }
}
static void dock_prepare(app *a,float w,float h) {
    ps_dock_layout *d=&a->preferences.dock;
    if(!a->dock_started) { a->dock_last_tab=a->tab;a->dock_started=true; }
    if(a->tab!=a->dock_last_tab) { dock_show(a,PS_DOCK_WORKSPACE);a->dock_last_tab=a->tab; }
    memset(a->dock_visible,0,sizeof a->dock_visible);
    memset(a->dock_splitters,0,sizeof a->dock_splitters);
    if(d->root!=PS_DOCK_NONE) dock_arrange(a,d->root,nk_rect(8,72,w-16,h-80));
    for(unsigned p=0;p<PS_DOCK_PANELS;p++) if(d->floating&(1u<<p)) {
        ps_dock_float old=d->floats[p];
        d->floats[p]=dock_clamp_float(p,nk_rect((float)old.x,(float)old.y,(float)old.w,(float)old.h),w,h);
        ps_dock_float *r=&d->floats[p];
        a->dock_panels[p]=nk_rect((float)r->x,(float)r->y,(float)r->w,(float)r->h);
        a->dock_visible[p]=true;
    }
    struct nk_input *in=&a->ui->input;
    bool down=nk_input_is_mouse_down(in,NK_BUTTON_LEFT), pressed=nk_input_is_mouse_pressed(in,NK_BUTTON_LEFT);
    if(a->toolbar_menu || (a->ui->active && a->ui->active->popup.active)) pressed=false;
    unsigned under=PS_DOCK_NONE;
    for(unsigned p=0;p<PS_DOCK_PANELS;p++)
        if((d->floating&(1u<<p)) && nk_input_is_mouse_hovering_rect(in,a->dock_panels[p])) under=p;
    if((d->floating&(1u<<a->dock_front)) && nk_input_is_mouse_hovering_rect(in,a->dock_panels[a->dock_front])) under=a->dock_front;
    if(pressed) {
        if(under!=PS_DOCK_NONE) a->dock_front=under;
    }
    if(!a->dock_drag && !a->dock_resize && !a->dock_float_resize && pressed) {
        if(under==PS_DOCK_NONE) for(unsigned i=0;i<PS_DOCK_NODES;i++)
            if(a->dock_splitters[i].w>0 && nk_input_is_mouse_hovering_rect(in,a->dock_splitters[i])) a->dock_resize=(int)i+1;
        for(unsigned p=0;p<PS_DOCK_PANELS;p++) {
            if(under!=PS_DOCK_NONE && under!=p) continue;
            if((d->floating&(1u<<p)) && nk_input_is_mouse_hovering_rect(in,a->dock_float_grips[p])) {
                a->dock_float_resize=(int)p+1;a->dock_resize_mouse=in->mouse.pos;a->dock_resize_start=d->floats[p];
            }
            if(nk_input_is_mouse_hovering_rect(in,a->dock_headers[p]) && !(d->hidden&(1u<<p))) {
                a->dock_drag=(int)p+1;a->dock_origin=in->mouse.pos;
                a->dock_grab=nk_vec2(in->mouse.pos.x-a->dock_panels[p].x,in->mouse.pos.y-a->dock_panels[p].y);
            }
        }
    }
    if(a->dock_resize && down) {
        unsigned id=(unsigned)a->dock_resize-1;ps_dock_node *n=&d->nodes[id];struct nk_rect r=a->dock_rects[id];
        bool x=n->kind==PS_DOCK_X;
        float fraction=(x?in->mouse.pos.x-r.x:in->mouse.pos.y-r.y)/(x?r.w:r.h);
        n->ratio=(uint32_t)fmaxf(64,fminf(960,fraction*1024));
        if(dock_sidebar_split(d,n)) {
            a->preferences.sidebar_width=(uint32_t)fmaxf(208,fminf(360,in->mouse.pos.x-r.x-4));a->panel_drag=1;
        }
        if(x && dock_single(d,n->first,PS_DOCK_WORKSPACE) && dock_single(d,n->second,PS_DOCK_INSPECTOR))
            a->preferences.inspector_width=(uint32_t)fmaxf(208,fminf(480,r.x+r.w-in->mouse.pos.x-4));
        if(dock_log_split(d,n)) {
            a->preferences.log_height=(uint32_t)fmaxf(150,fminf(340,h-in->mouse.pos.y-4));a->panel_drag=2;
        }
    }
    if(a->dock_float_resize && down) {
        unsigned p=(unsigned)a->dock_float_resize-1;ps_dock_float old=a->dock_resize_start;
        d->floats[p]=dock_clamp_float(p,nk_rect((float)old.x,(float)old.y,old.w+in->mouse.pos.x-a->dock_resize_mouse.x,old.h+in->mouse.pos.y-a->dock_resize_mouse.y),w,h);
    }
    if(a->dock_drag && down && (fabsf(in->mouse.pos.x-a->dock_origin.x)>5 || fabsf(in->mouse.pos.y-a->dock_origin.y)>5)) a->dock_dragging=true;
    a->dock_target=PS_DOCK_NONE;a->dock_side=PS_DOCK_NONE;
    memset(a->dock_targets,0,sizeof a->dock_targets);
    if(a->dock_dragging) {
        unsigned panel=(unsigned)a->dock_drag-1;
        for(unsigned i=0;i<PS_DOCK_NODES;i++) if(d->nodes[i].kind==PS_DOCK_GROUP && nk_input_is_mouse_hovering_rect(in,a->dock_rects[i])) {
            unsigned target=d->nodes[i].active;
            if(target==panel && d->nodes[i].panels==(1u<<panel)) continue;
            if(target==panel) for(unsigned p=0;p<PS_DOCK_PANELS;p++) if(p!=panel && (d->nodes[i].panels&(1u<<p))) { target=p;break; }
            struct nk_rect r=a->dock_rects[i];float cx=r.x+r.w/2,cy=r.y+r.h/2;
            const float dx[]={0,-42,42,0,0},dy[]={0,0,0,-42,42};
            for(unsigned side=0;side<5;side++) {
                a->dock_targets[side]=nk_rect(cx+dx[side]-16,cy+dy[side]-16,32,32);
                if(nk_input_is_mouse_hovering_rect(in,a->dock_targets[side])) { a->dock_target=target;a->dock_side=side; }
            }
            break;
        }
        ps_dock_float old=d->floats[panel];
        ps_dock_float preview=dock_clamp_float(panel,nk_rect(in->mouse.pos.x-a->dock_grab.x,in->mouse.pos.y-a->dock_grab.y,(float)old.w,(float)old.h),w,h);
        a->dock_preview=nk_rect((float)preview.x,(float)preview.y,(float)preview.w,(float)preview.h);
        if(!down) {
            if(a->dock_side!=PS_DOCK_NONE) ps_dock_move(d,panel,a->dock_target,a->dock_side);
            else if(d->root==PS_DOCK_NONE && nk_input_is_mouse_hovering_rect(in,nk_rect(8,72,w-16,h-80))) ps_dock_move(d,panel,PS_DOCK_NONE,PS_DOCK_TAB);
            else {
                ps_dock_float_panel(d,panel,preview);a->dock_front=panel;
                if(panel==PS_DOCK_LOG) a->show_log=true;
            }
        }
    }
    if(!down) { a->dock_resize=a->dock_float_resize=a->dock_drag=0;a->dock_dragging=false;a->panel_drag=0; }
    if(a->tab==6 && (a->dock_resize || a->dock_float_resize || a->dock_dragging)) {
        a->settings_draft.dock=*d;a->settings_draft.sidebar_width=a->preferences.sidebar_width;a->settings_draft.log_height=a->preferences.log_height;
        a->settings_draft.inspector_width=a->preferences.inspector_width;
    }
    dock_relayout(a,w,h);
}
static unsigned dock_hover_float(app *a) {
    unsigned under=PS_DOCK_NONE;
    for(unsigned p=0;p<PS_DOCK_PANELS;p++)
        if((a->preferences.dock.floating&(1u<<p)) && nk_input_is_mouse_hovering_rect(&a->ui->input,a->dock_panels[p])) under=p;
    if((a->preferences.dock.floating&(1u<<a->dock_front)) &&
       nk_input_is_mouse_hovering_rect(&a->ui->input,a->dock_panels[a->dock_front])) under=a->dock_front;
    return under;
}
static void dock_input(app *a,unsigned panel,struct nk_rect r,struct nk_window *previous,bool body) {
    struct nk_context *ui=a->ui;struct nk_window *window=ui->current;
    unsigned under=dock_hover_float(a);
    bool covered=under!=PS_DOCK_NONE && under!=panel;
    bool popup=a->toolbar_menu || (previous && previous->popup.active);
    bool mouse=!covered && !popup && nk_input_is_mouse_hovering_rect(&ui->input,r);
    bool enabled=mouse || (body && previous==window && !covered && !popup);
    /* Background windows otherwise activate even outside their bounds and consume
     * the wheel in nk_end. Keep keyboard focus and route mouse input by stacking. */
    ui->active=body && mouse?window:previous;
    if(enabled) { window->flags &= ~(nk_flags)NK_WINDOW_ROM;window->layout->flags &= ~(nk_flags)NK_WINDOW_ROM; }
    else { window->flags |= NK_WINDOW_ROM;window->layout->flags |= NK_WINDOW_ROM; }
    window->layout->has_scrolling=mouse;
    ps_ui_accessibility_route(ui,!covered && !popup);
}
static bool dock_begin(app *a,unsigned panel) {
    struct nk_rect r=a->dock_panels[panel];r.y+=24;r.h=fmaxf(1,r.h-24);
    nk_window_show(a->ui,dock_windows[panel],a->dock_visible[panel]?NK_SHOWN:NK_HIDDEN);
    struct nk_window *previous=a->ui->active;
    bool shown=nk_begin(a->ui,dock_windows[panel],r,NK_WINDOW_BACKGROUND|(a->dock_visible[panel]?0:NK_WINDOW_HIDDEN));
    if(shown) dock_input(a,panel,r,previous,true);
    else a->ui->active=previous;
    return shown;
}
static void dock_headers(app *a) {
    struct nk_context *ui=a->ui;ps_dock_layout *d=&a->preferences.dock;
    unsigned close=PS_DOCK_NONE;
    memset(a->dock_headers,0,sizeof a->dock_headers);memset(a->dock_closes,0,sizeof a->dock_closes);memset(a->dock_float_grips,0,sizeof a->dock_float_grips);
    for(unsigned i=0;i<PS_DOCK_NODES+PS_DOCK_PANELS;i++) {
        bool floating=i>=PS_DOCK_NODES;unsigned p=floating?i-PS_DOCK_NODES:0;
        char name[48];snprintf(name,sizeof name,"dock-heading-%u",i);
        bool visible=floating?(d->floating&(1u<<p))!=0:d->nodes[i].kind==PS_DOCK_GROUP;
        nk_window_show(ui,name,visible?NK_SHOWN:NK_HIDDEN);
        if(!visible) continue;
        uint32_t mask=floating?1u<<p:d->nodes[i].panels;
        struct nk_rect r=floating?a->dock_panels[p]:a->dock_rects[i];r.h=24;
        nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(2,0));
        nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(2,0));
        nk_style_push_vec2(ui,&ui->style.button.padding,nk_vec2(4,2));
        struct nk_window *previous=ui->active;
        if(nk_begin(ui,name,r,NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BACKGROUND)) {
            dock_input(a,floating?p:d->nodes[i].active,r,previous,false);
            unsigned count=0;for(unsigned j=0;j<PS_DOCK_PANELS;j++) if(mask&(1u<<j)) count++;
            float close_fraction=fminf(.25f,24.f/fmaxf(1,r.w));
            nk_layout_row_begin(ui,NK_DYNAMIC,24,(int)count+1);
            for(unsigned j=0;j<PS_DOCK_PANELS;j++) if(mask&(1u<<j)) {
                nk_layout_row_push(ui,(1-close_fraction)/count);
                a->dock_headers[j]=nk_widget_bounds(ui);
                if(action_button(ui,dock_names[j],floating || d->nodes[i].active==j)) ps_dock_select(d,j);
            }
            unsigned active=floating?p:d->nodes[i].active;
            nk_layout_row_push(ui,close_fraction);
            a->dock_closes[active]=nk_widget_bounds(ui);
            if(nk_button_label(ui,"×")) {
                close=active;
            }
            nk_layout_row_end(ui);
        }
        nk_end(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
    }
    if(close!=PS_DOCK_NONE) {
        ps_dock_hide(d,close);
        if(close==PS_DOCK_LOG) a->show_log=false;
    }
    for(unsigned p=0;p<PS_DOCK_PANELS;p++) if(d->floating&(1u<<p)) {
        struct nk_rect r=a->dock_panels[p];a->dock_float_grips[p]=nk_rect(r.x+r.w-18,r.y+r.h-18,18,18);
    }
}
static void dock_order(app *a) {
    ps_dock_layout *d=&a->preferences.dock;char name[48];
    for(unsigned p=0;p<PS_DOCK_PANELS;p++) if(!(d->floating&(1u<<p))) nk_sdl_window_raise(a->ui,dock_windows[p]);
    for(unsigned i=0;i<PS_DOCK_NODES;i++) {
        snprintf(name,sizeof name,"dock-heading-%u",i);nk_sdl_window_raise(a->ui,name);
    }
    for(unsigned pass=0;pass<2;pass++) for(unsigned p=0;p<PS_DOCK_PANELS;p++)
        if((d->floating&(1u<<p)) && ((p==a->dock_front)==(pass==1))) {
            nk_sdl_window_raise(a->ui,dock_windows[p]);
            snprintf(name,sizeof name,"dock-heading-%u",PS_DOCK_NODES+p);nk_sdl_window_raise(a->ui,name);
        }
    nk_sdl_window_raise(a->ui,"header");
}
static void dock_overlay(app *a,float w,float h) {
    struct nk_command_buffer *canvas=nk_window_get_canvas(a->ui);
    struct nk_rect previous=canvas->clip;
    nk_push_scissor(canvas,nk_rect(0,0,w,h));
    for(unsigned i=0;i<PS_DOCK_NODES;i++) if(a->dock_splitters[i].w>0) {
        struct nk_rect r=a->dock_splitters[i];bool x=r.w==8;
        nk_stroke_line(canvas,r.x+(x?r.w/2:0),r.y+(x?0:r.h/2),r.x+(x?r.w/2:r.w),r.y+(x?r.h:r.h/2),2,UI_COLOR(divider));
    }
    for(unsigned p=0;p<3;p++) if(a->preferences.dock.floating&(1u<<p)) nk_stroke_rect(canvas,a->dock_float_grips[p],2,1,UI_COLOR(accent));
    if(a->dock_dragging) {
        nk_stroke_rect(canvas,a->dock_preview,5,2,UI_COLOR(accent));
        for(unsigned side=0;side<5;side++) if(a->dock_targets[side].w>0)
            nk_fill_rect(canvas,a->dock_targets[side],4,side==a->dock_side?UI_COLOR(primary):UI_COLOR(button));
    }
    nk_push_scissor(canvas,previous);
}
