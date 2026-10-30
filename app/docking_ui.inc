static const char *dock_names[] = {"Seitenleiste", "Arbeitsbereich", "Protokoll", "Inspektor"};
static const char *dock_windows[] = {"sidebar", "workspace", "log", "inspector"};
static bool workbench_visible(const app *a) {return (a->tab==0 || a->tab==8 || (a->tab==2 && a->analysis_tab==1));}
static bool workspace_shell_visible(const app *a) {
    return (a->tab==0 || a->tab==1 || a->tab==2 || a->tab==8 || browser_workspace_visible(a));
}
enum { WORKSPACE_HEADER_HEIGHT=79, WORKSPACE_PATH_HEIGHT=39,
       WORKSPACE_CONTENT_TOP=118, WORKSPACE_STATUS_HEIGHT=28 };
enum { LABORATORY_CONTENT_TOP=232, LABORATORY_SCENE_MIN=180,
       LABORATORY_MEASURE_MIN=130, LABORATORY_LOG_MIN=68, LABORATORY_TIMELINE_MIN=95 };
static void workspace_rule(struct nk_command_buffer *canvas,struct nk_rect line) {
    struct nk_rect clip=canvas->clip;
    nk_push_scissor(canvas,line);nk_fill_rect(canvas,line,0,UI_COLOR(border));nk_push_scissor(canvas,clip);
}
#include "workbench_header_ui.inc"
#include "protocol_layout.inc"
#include "laboratory_layout.inc"
#include "analysis_layout.inc"
#include "analysis_resize.inc"
static float dock_header_height(const app *a) {return workbench_visible(a)?file_tabs_height(a):DOCK_HEADER_HEIGHT;}
static float dock_panel_header_height(const app *a,unsigned panel) {
    if(a->panel_render)return 0;
    if(panel==PS_DOCK_LOG && protocol_visible(a))return 0;
    if(analysis_visible(a))return 0;
    if(laboratory_visible(a)) {
        if(panel==PS_DOCK_WORKSPACE){uint32_t group=ps_dock_group(&a->preferences.dock,panel);
            return group!=PS_DOCK_NONE && a->preferences.dock.nodes[group].panels!=(1u<<panel)?38:0;}
        if(panel==PS_DOCK_SIDEBAR)return 45.4f*a->preferences.ui_size/16.f;
        if(panel==PS_DOCK_INSPECTOR)return 46.85f*a->preferences.ui_size/16.f;
        return 38;
    }
    return workbench_visible(a) && panel==PS_DOCK_LOG?42:dock_header_height(a);
}
/* The reference uses borders inside adjoining panels, not empty layout gaps. */
static float dock_gap(const app *a) {return workspace_shell_visible(a)?0:8;}
static struct nk_rect dock_root_bounds(app *a,float w,float h) {
    h-=protocol_reserved(a,h);
    if(laboratory_visible(a))return nk_rect(0,LABORATORY_CONTENT_TOP,w,fmaxf(a->show_log?1:180,h-LABORATORY_CONTENT_TOP-WORKSPACE_STATUS_HEIGHT-laboratory_dimension_value(a,1)));
    if(analysis_visible(a))return nk_rect(0,198,w,h-198-WORKSPACE_STATUS_HEIGHT);
    if(workspace_shell_visible(a))return nk_rect(0,WORKSPACE_CONTENT_TOP,w,h-WORKSPACE_CONTENT_TOP-WORKSPACE_STATUS_HEIGHT);
    return nk_rect(8,72,w-16,h-80);
}
static uint32_t dock_mask(const ps_dock_layout *d, uint32_t id) {
    const ps_dock_node *n=&d->nodes[id];
    if(n->kind==PS_DOCK_EMPTY)return 0;
    return n->kind==PS_DOCK_GROUP?n->panels:dock_mask(d,n->first)|dock_mask(d,n->second);
}
static bool dock_single(const ps_dock_layout *d,uint32_t id,uint32_t panel) {
    return d->nodes[id].kind==PS_DOCK_GROUP && d->nodes[id].panels==(1u<<panel);
}
static bool dock_sidebar_split(const ps_dock_layout *d,const ps_dock_node *n) {
    if(n->kind!=PS_DOCK_X || !dock_single(d,n->first,PS_DOCK_SIDEBAR))return false;
    uint32_t mask=dock_mask(d,n->second);
    return mask==2 || mask==6 || mask==10 || mask==14;
}
static bool dock_log_split(const ps_dock_layout *d,const ps_dock_node *n) {
    if(n->kind!=PS_DOCK_Y || !dock_single(d,n->second,PS_DOCK_LOG))return false;
    uint32_t mask=dock_mask(d,n->first);
    return mask==2 || mask==3 || mask==10 || mask==11;
}
static bool dock_context_split(const ps_dock_layout *d,const ps_dock_node *n) {
    return n->kind==PS_DOCK_X && dock_single(d,n->second,PS_DOCK_INSPECTOR) &&
           (dock_mask(d,n->first)&(1u<<PS_DOCK_WORKSPACE))!=0;
}
static float dock_minimum(const app *a,uint32_t id,bool x) {
    const ps_dock_layout *d=&a->preferences.dock;const ps_dock_node *n=&d->nodes[id];
    if(n->kind==PS_DOCK_GROUP) {
        if(protocol_visible(a) && n->panels==(1u<<PS_DOCK_LOG))return 0;
        if(laboratory_visible(a)) {
            if(x) {
                if(n->panels&(1u<<PS_DOCK_WORKSPACE))return 440;
                if(n->panels&(1u<<PS_DOCK_SIDEBAR))return 150;
                if(n->panels&(1u<<PS_DOCK_INSPECTOR))return laboratory_inspector_minimum(a);
            } else {
                if(n->panels&(1u<<PS_DOCK_WORKSPACE))return LABORATORY_SCENE_MIN+1+LABORATORY_MEASURE_MIN;
                if(n->panels&(1u<<PS_DOCK_LOG))return LABORATORY_LOG_MIN;
            }
        }
        if(workbench_visible(a)) {
            if(x) {
                if(n->panels&(1u<<PS_DOCK_WORKSPACE))return 300;
                if(n->panels&(1u<<PS_DOCK_INSPECTOR))return 180;
                if(n->panels&(1u<<PS_DOCK_SIDEBAR))return 140;
            } else {
                if(n->panels&(1u<<PS_DOCK_WORKSPACE))return 280;
                if(n->panels&(1u<<PS_DOCK_LOG))return a->show_log?126:66;
            }
        }
        if(analysis_visible(a) && x) {
            if(n->panels&(1u<<PS_DOCK_WORKSPACE))return 300;
            if(n->panels&(1u<<PS_DOCK_INSPECTOR))return analysis_side_minimum(a,PS_DOCK_INSPECTOR);
            if(n->panels&(1u<<PS_DOCK_SIDEBAR))return analysis_side_minimum(a,PS_DOCK_SIDEBAR);
        }
        return 64;
    }
    if(n->kind!=PS_DOCK_X && n->kind!=PS_DOCK_Y)return 0;
    float one=dock_minimum(a,n->first,x),two=dock_minimum(a,n->second,x);
    return (n->kind==PS_DOCK_X)==x?one+two+dock_gap(a):fmaxf(one,two);
}
static const char *dock_resize_label(app *a,unsigned id,char text[48]) {
    ps_dock_layout *d=&a->preferences.dock;ps_dock_node *n=&d->nodes[id];
    if(dock_sidebar_split(d,n))return laboratory_visible(a)?"Szenenbaum-Breite":analysis_visible(a)?"Messlauflisten-Breite":"Workspace-Breite";
    if(dock_context_split(d,n))return laboratory_visible(a)?"Inspektor-Breite":analysis_visible(a)?"Laufdetails-Breite":"Codekontext-Breite";
    if(dock_log_split(d,n))return "Protokoll-Höhe";
    snprintf(text,48,"Paneltrenner %u",id+1);return text;
}
static float dock_resize_value(app *a,unsigned id,float *minimum,float *maximum,float *standard,bool *reverse) {
    ps_dock_layout *d=&a->preferences.dock;ps_dock_node *n=&d->nodes[id];
    struct nk_rect r=a->dock_rects[id];bool x=n->kind==PS_DOCK_X;
    float extent=fmaxf(1,(x?r.w:r.h)-dock_gap(a)),min_first=dock_minimum(a,n->first,x),min_second=dock_minimum(a,n->second,x);
    *reverse=false;
    if(dock_sidebar_split(d,n)) {*minimum=analysis_visible(a)?min_first:laboratory_visible(a)?150:workbench_visible(a)?140:208;*maximum=fminf(laboratory_visible(a)?420:workbench_visible(a) || analysis_visible(a)?480:360,extent-min_second);*standard=laboratory_visible(a)?190:analysis_visible(a)?232:workbench_visible(a)?218:248;return a->dock_rects[n->first].w;}
    if(dock_context_split(d,n)) {*reverse=true;*minimum=min_second;*maximum=fminf(480,extent-min_first);*standard=analysis_visible(a)?242:workbench_visible(a)?255:280;return a->dock_rects[n->second].w;}
    if(dock_log_split(d,n)) {*reverse=true;*minimum=workbench_visible(a)?126:laboratory_visible(a)?LABORATORY_LOG_MIN:150;*maximum=fminf(workbench_visible(a)?366:340,extent-min_first);*standard=workbench_visible(a)?152:178;return a->dock_rects[n->second].h+dock_gap(a);}
    *minimum=fmaxf(min_first,extent/16);*maximum=fminf(extent-min_second,extent*15/16);*standard=extent*.5f;
    return extent*n->ratio/1024.f;
}
static void dock_resize_set(app *a,unsigned id,float value) {
    float minimum,maximum,standard;bool reverse;
    dock_resize_value(a,id,&minimum,&maximum,&standard,&reverse);
    value=fmaxf(minimum,fminf(fmaxf(minimum,maximum),value));
    ps_dock_layout *d=&a->preferences.dock;ps_dock_node *n=&d->nodes[id];
    if(dock_sidebar_split(d,n))a->preferences.sidebar_width=(uint32_t)lroundf(value);
    else if(dock_context_split(d,n))a->preferences.inspector_width=(uint32_t)lroundf(value);
    else if(dock_log_split(d,n)){a->preferences.log_height=(uint32_t)lroundf(value);a->show_log=true;}
    else {
        struct nk_rect r=a->dock_rects[id];float extent=fmaxf(1,(n->kind==PS_DOCK_X?r.w:r.h)-dock_gap(a));
        n->ratio=(uint32_t)fmaxf(64,fminf(960,value/extent*1024));
    }
}
static void dock_resize_begin(app *a) {
    if(!a->dock_resize_transaction) {
        a->dock_resize_original=a->preferences;a->dock_resize_mouse=a->ui->input.mouse.pos;
        if(a->dock_resize) {
            float minimum,maximum,standard;bool reverse;
            a->dock_resize_initial_value=dock_resize_value(a,(unsigned)a->dock_resize-1,&minimum,&maximum,&standard,&reverse);
        }
        a->dock_resize_transaction=true;a->dock_resize_moved=false;SDL_CaptureMouse(true);
    }
}
static void dock_resize_abort(app *a) {
    if(a->dock_resize_transaction) {
        a->preferences.dock=a->dock_resize_original.dock;
        a->preferences.sidebar_width=a->dock_resize_original.sidebar_width;
        a->preferences.inspector_width=a->dock_resize_original.inspector_width;
        a->preferences.log_height=a->dock_resize_original.log_height;
    }
    a->dock_resize_transaction=false;a->dock_resize=a->dock_float_resize=0;SDL_CaptureMouse(false);
}
static bool dock_resize_key(app *a,const SDL_KeyboardEvent *key) {
    if(key->mod&(SDL_KMOD_CTRL|SDL_KMOD_GUI|SDL_KMOD_ALT) || a->recovery || a->toolbar_menu)return false;
    if(key->key==SDLK_ESCAPE && (a->dock_resize || a->dock_float_resize || a->dock_resize_transaction)) {
        dock_resize_abort(a);return true;
    }
    for(unsigned id=0;id<PS_DOCK_NODES;id++) {
        if(a->dock_splitters[id].w<=0 || (a->preferences.dock.nodes[id].kind!=PS_DOCK_X && a->preferences.dock.nodes[id].kind!=PS_DOCK_Y))continue;
        char label[48];if(!nk_sdl_accessibility_is_focused(a->ui,NULL,dock_resize_label(a,id,label)))continue;
        float minimum,maximum,standard;bool reverse;
        float value=dock_resize_value(a,id,&minimum,&maximum,&standard,&reverse);
        int direction=key->key==SDLK_LEFT || key->key==SDLK_UP?-1:key->key==SDLK_RIGHT || key->key==SDLK_DOWN?1:0;
        if(direction) {
            value+=(reverse?-direction:direction)*(key->mod&SDL_KMOD_SHIFT?40:10);
        } else if(key->key==SDLK_HOME)value=minimum;
        else if(key->key==SDLK_END)value=maximum;
        else if(key->key==SDLK_RETURN || key->key==SDLK_KP_ENTER){value=standard;a->dock_resize_transaction=false;}
        else return false;
        dock_resize_set(a,id,value);return true;
    }
    return false;
}
static void dock_arrange(app *a,uint32_t id,struct nk_rect r) {
    ps_dock_layout *d=&a->preferences.dock;
    ps_dock_node *n=&d->nodes[id]; a->dock_rects[id]=r;
    if(protocol_visible(a)) {
        if(dock_mask(d,id)==(1u<<PS_DOCK_LOG))return;
        if(n->kind==PS_DOCK_X || n->kind==PS_DOCK_Y) {
            if(dock_mask(d,n->first)==(1u<<PS_DOCK_LOG)){dock_arrange(a,n->second,r);return;}
            if(dock_mask(d,n->second)==(1u<<PS_DOCK_LOG)){dock_arrange(a,n->first,r);return;}
        }
    }
    if(n->kind==PS_DOCK_GROUP) {
        for(unsigned p=0;p<PS_DOCK_PANELS;p++) if(n->panels&(1u<<p)) {
            a->dock_panels[p]=r; a->dock_visible[p]=n->active==p;
        }
        return;
    }
    bool x=n->kind==PS_DOCK_X;
    float gap=dock_gap(a),extent=(x?r.w:r.h)-gap,first=extent*n->ratio/1024.f;
    if(dock_sidebar_split(d,n))
        first=(float)a->preferences.sidebar_width;
    if(dock_context_split(d,n))
        first=extent-(float)a->preferences.inspector_width;
    if(dock_log_split(d,n))
        first=extent-(a->show_log&&a->tab!=6?(float)a->preferences.log_height-gap:workbench_visible(a)?66.f:68.f);
    first=fmaxf(dock_minimum(a,n->first,x),fminf(extent-dock_minimum(a,n->second,x),first));
    struct nk_rect one=r,two=r,split=r;
    if(x) { one.w=first; split.x=r.x+first-(workspace_shell_visible(a)?4:0);split.w=workspace_shell_visible(a)?9:8;two.x=r.x+first+gap;two.w=extent-first; }
    else { one.h=first;split.y=r.y+first-(workspace_shell_visible(a)?4:0);split.h=workspace_shell_visible(a)?9:8;two.y=r.y+first+gap;two.h=extent-first; }
    a->dock_splitters[id]=split;dock_arrange(a,n->first,one);dock_arrange(a,n->second,two);
    if(!a->show_log && dock_log_split(d,n))a->dock_splitters[id]=nk_rect(0,0,0,0);
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
static void dock_fixed(app *a);
static void dock_relayout(app *a,float w,float h) {
    dock_fixed(a);
    ps_dock_layout *d=&a->preferences.dock;
    memset(a->dock_visible,0,sizeof a->dock_visible);memset(a->dock_splitters,0,sizeof a->dock_splitters);
    if(d->root!=PS_DOCK_NONE) dock_arrange(a,d->root,dock_root_bounds(a,w,h));
    if(protocol_visible(a)){a->dock_panels[PS_DOCK_LOG]=protocol_bounds(a,w,h);a->dock_visible[PS_DOCK_LOG]=true;}
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
/* Legacy layouts remain readable; only visibility and dimensions participate
 * in the new fixed topology. Never write a migration back merely on open. */
static void dock_fixed(app *a) {
    a->preferences.dock=workbench_visible(a)?ps_dock_fixed_workbench_layout(a->preferences.dock.hidden):ps_dock_fixed_layout(a->preferences.dock.hidden);
}
static void dock_prepare(app *a,float w,float h) {
    workspace_context(a);dock_fixed(a);protocol_resize_input(a,w,h);laboratory_resize_input(a);
    if(!analysis_visible(a) && !a->panels[PS_DOCK_WORKSPACE].detached)file_tabs_prepare(a);
    panel_pull_input(a,w,h);
    ps_dock_layout *d=&a->preferences.dock;
    if(!a->dock_started) { a->dock_last_tab=a->tab;a->dock_started=true; }
    if(a->tab!=a->dock_last_tab) { dock_show(a,PS_DOCK_WORKSPACE);a->dock_last_tab=a->tab; }
    memset(a->dock_visible,0,sizeof a->dock_visible);
    memset(a->dock_splitters,0,sizeof a->dock_splitters);
    if(d->root!=PS_DOCK_NONE) dock_arrange(a,d->root,dock_root_bounds(a,w,h));
    for(unsigned p=0;p<PS_DOCK_PANELS;p++) if(d->floating&(1u<<p)) {
        ps_dock_float old=d->floats[p];
        d->floats[p]=dock_clamp_float(p,nk_rect((float)old.x,(float)old.y,(float)old.w,(float)old.h),w,h);
        ps_dock_float *r=&d->floats[p];
        a->dock_panels[p]=nk_rect((float)r->x,(float)r->y,(float)r->w,(float)r->h);
        a->dock_visible[p]=true;
    }
    struct nk_input *in=&a->ui->input;
    bool down=nk_input_is_mouse_down(in,NK_BUTTON_LEFT), pressed=nk_input_is_mouse_pressed(in,NK_BUTTON_LEFT);
    if(a->toolbar_menu || a->protocol_view.resize || (a->ui->active && a->ui->active->popup.active && a->ui->active->popup.type!=NK_PANEL_TOOLTIP) || a->file_tab_drag) pressed=false;
    /* Fixed slots: only the separators can begin a pointer transaction. */
    a->dock_drag=a->dock_float_resize=0;a->dock_dragging=false;
    if(!a->dock_resize && pressed)for(unsigned i=0;i<PS_DOCK_NODES;i++)
        if(a->dock_splitters[i].w>0 && nk_input_is_mouse_hovering_rect(in,a->dock_splitters[i]))a->dock_resize=(int)i+1;
    if((a->dock_resize || a->dock_float_resize) && pressed)dock_resize_begin(a);
    bool same_splitter_click=a->dock_resize && a->dock_resize_last_click==a->dock_resize;
    if(pressed)a->dock_resize_last_click=a->dock_resize;
    if(a->dock_resize && pressed && same_splitter_click && nk_input_is_mouse_pressed(in,NK_BUTTON_DOUBLE)) {
        unsigned id=(unsigned)a->dock_resize-1;float minimum,maximum,standard;bool reverse;
        dock_resize_value(a,id,&minimum,&maximum,&standard,&reverse);dock_resize_set(a,id,standard);
        a->dock_resize=0;a->dock_resize_transaction=false;SDL_CaptureMouse(false);
    }
    if(a->dock_resize && down) {
        unsigned id=(unsigned)a->dock_resize-1;ps_dock_node *n=&d->nodes[id];
        bool x=n->kind==PS_DOCK_X;
        float delta=x?in->mouse.pos.x-a->dock_resize_mouse.x:in->mouse.pos.y-a->dock_resize_mouse.y;
        float value=a->dock_resize_initial_value+((dock_context_split(d,n) || dock_log_split(d,n))?-delta:delta);
        if(delta!=0)a->dock_resize_moved=true;
        if(a->dock_resize_moved)dock_resize_set(a,id,value);
        if(dock_sidebar_split(d,n))a->panel_drag=1;
        if(dock_log_split(d,n))a->panel_drag=2;
    }
    if(!down) { if(a->dock_resize || a->dock_float_resize){a->dock_resize_transaction=false;SDL_CaptureMouse(false);}a->dock_resize=a->dock_float_resize=a->dock_drag=0;a->dock_dragging=false;a->panel_drag=0; }
    if(a->tab==6 && (a->dock_resize || a->dock_float_resize || a->dock_dragging)) {
        a->settings_defaults=false;
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
    bool popup=a->toolbar_menu || a->toolbar_pointer_consumed || (previous && previous->popup.active && previous->popup.type!=NK_PANEL_TOOLTIP);
    bool completion_pointer=a->completion_active && nk_input_is_mouse_hovering_rect(&ui->input,a->completion_list_bounds);
    bool search_pointer=panel==PS_DOCK_WORKSPACE && a->show_search && editor_search_target(a) && nk_input_is_mouse_hovering_rect(&ui->input,a->search_island_bounds);
    bool mouse=!covered && !popup && !completion_pointer && !search_pointer && nk_input_is_mouse_hovering_rect(&ui->input,r);
    bool enabled=mouse || (body && previous==window && !covered && !popup);
    bool plot_capture=analysis_visible(a) && a->analysis_plot_resizing;
    if(plot_capture)enabled=body && panel==PS_DOCK_WORKSPACE && (!panel_is_detached(a,panel) || a->panel_render) && !covered && !popup;
    if(a->panel_render && (a->recovery || a->layout_manager || a->channel_unit_manager || a->workspace_manager || a->run_details_manager))enabled=false;
    if(a->panel_pull_dragging || a->dock_dragging || a->file_tab_dragging || a->dock_resize || a->dock_float_resize || a->protocol_view.resize)enabled=false;
    /* Background windows otherwise activate even outside their bounds and consume
     * the wheel in nk_end. Keep keyboard focus and route mouse input by stacking. */
    ui->active=body && (mouse || (plot_capture && enabled))?window:previous;
    if(enabled) { window->flags &= ~(nk_flags)NK_WINDOW_ROM;window->layout->flags &= ~(nk_flags)NK_WINDOW_ROM; }
    else { window->flags |= NK_WINDOW_ROM;window->layout->flags |= NK_WINDOW_ROM; }
    window->layout->has_scrolling=mouse && !plot_capture && !a->dock_resize && !a->dock_float_resize;
    ps_ui_accessibility_route(ui,!covered && !popup && !a->dock_resize && !a->dock_float_resize &&
        (!a->panel_render || !(a->recovery || a->layout_manager || a->channel_unit_manager || a->workspace_manager || a->run_details_manager)));
}
static bool dock_begin(app *a,unsigned panel) {
    struct nk_rect r=a->dock_panels[panel];float heading=dock_panel_header_height(a,panel);r.y+=heading;r.h=fmaxf(1,r.h-heading);
    if((workbench_visible(a) || laboratory_visible(a)) && panel==PS_DOCK_INSPECTOR){r.x+=1;r.w=fmaxf(1,r.w-1);}
    nk_window_show(a->ui,dock_windows[panel],a->dock_visible[panel]?NK_SHOWN:NK_HIDDEN);
    struct nk_window *previous=a->ui->active;
    nk_style_push_float(a->ui,&a->ui->style.window.rounding,0);
    nk_flags flags=NK_WINDOW_BACKGROUND|(a->dock_visible[panel]?0:NK_WINDOW_HIDDEN);
    if(workbench_visible(a) && panel==PS_DOCK_INSPECTOR && !workbench_context_needs_scrollbar(a,r))flags|=NK_WINDOW_NO_SCROLLBAR;
    if(analysis_visible(a) && ((panel==PS_DOCK_SIDEBAR && r.h>=420) || (panel==PS_DOCK_WORKSPACE && r.h>=720 && a->preferences.ui_size<=16 && !a->show_report && a->selected_count<=1 && analysis_plot_fits(a,r))))flags|=NK_WINDOW_NO_SCROLLBAR;
    if(laboratory_visible(a) && (panel==PS_DOCK_INSPECTOR || (panel==PS_DOCK_WORKSPACE && (!protocol_visible(a) || r.h>=LABORATORY_SCENE_MIN+LABORATORY_MEASURE_MIN+1))))flags|=NK_WINDOW_NO_SCROLLBAR;
    bool shown=nk_begin(a->ui,dock_windows[panel],r,flags);
    nk_style_pop_float(a->ui);
    if(shown) dock_input(a,panel,r,previous,true);
    if(shown && !a->panel_render && panel_is_detached(a,panel) && !(panel==PS_DOCK_WORKSPACE && laboratory_visible(a))) {
        return false;
    }
    if(!shown)a->ui->active=previous;
    ps_panel_window *p=&a->panels[panel];
    if(shown && p->restore_scroll && p->tab==a->tab && ((bool)a->panel_render==p->detached)) {
        nk_window_set_scroll(a->ui,p->scroll_x,p->scroll_y);p->restore_scroll=false;
    }
    if(shown && workbench_visible(a) && panel==PS_DOCK_INSPECTOR && (flags&NK_WINDOW_NO_SCROLLBAR))nk_window_set_scroll(a->ui,0,0);
    return shown;
}
static void dock_headers(app *a) {
    struct nk_context *ui=a->ui;ps_dock_layout *d=&a->preferences.dock;
    unsigned close=PS_DOCK_NONE;
    if(a->workbench_sidebar_focus && nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT) &&
       !nk_input_is_mouse_hovering_rect(&ui->input,a->workbench_sidebar_bounds))a->workbench_sidebar_focus=0;
    memset(a->dock_headers,0,sizeof a->dock_headers);memset(a->dock_closes,0,sizeof a->dock_closes);memset(a->dock_float_grips,0,sizeof a->dock_float_grips);
    for(unsigned i=0;i<PS_DOCK_NODES+PS_DOCK_PANELS;i++) {
        bool floating=i>=PS_DOCK_NODES;unsigned p=floating?i-PS_DOCK_NODES:0;
        char name[48];snprintf(name,sizeof name,"dock-heading-%u",i);
        bool visible=floating?(d->floating&(1u<<p))!=0:d->nodes[i].kind==PS_DOCK_GROUP;
        if(analysis_visible(a))visible=false;
        if(protocol_visible(a) && (floating?p==PS_DOCK_LOG:d->nodes[i].panels==(1u<<PS_DOCK_LOG)))visible=false;
        if(laboratory_visible(a) && (floating?p==PS_DOCK_WORKSPACE:d->nodes[i].panels==(1u<<PS_DOCK_WORKSPACE)))visible=false;
        nk_window_show(ui,name,visible?NK_SHOWN:NK_HIDDEN);
        if(!visible) continue;
        uint32_t mask=floating?1u<<p:d->nodes[i].panels;
        struct nk_rect r=floating?a->dock_panels[p]:a->dock_rects[i];r.h=dock_panel_header_height(a,floating?p:d->nodes[i].active);
        nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(0,0));
        nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,0));
        nk_style_push_vec2(ui,&ui->style.button.padding,nk_vec2(workbench_visible(a) || laboratory_visible(a)?12:4,2));
        nk_style_push_flags(ui,&ui->style.button.text_alignment,workbench_visible(a) || laboratory_visible(a)?NK_TEXT_LEFT:NK_TEXT_CENTERED);
        nk_style_push_float(ui,&ui->style.window.rounding,0);
        nk_style_push_float(ui,&ui->style.button.rounding,0);
        nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(workbench_visible(a)?UI_COLOR(window):UI_COLOR(header)));
        nk_style_push_style_item(ui,&ui->style.button.normal,nk_style_item_color(workbench_visible(a)?UI_COLOR(window):UI_COLOR(header)));
        struct nk_window *previous=ui->active;
        if(nk_begin(ui,name,r,NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BACKGROUND)) {
            dock_input(a,floating?p:d->nodes[i].active,r,previous,false);
            unsigned count=0;for(unsigned j=0;j<PS_DOCK_PANELS;j++) if(mask&(1u<<j)) count++;
            float close_size=workbench_visible(a) && (floating?p:d->nodes[i].active)==PS_DOCK_SIDEBAR?40:28;
            float close_fraction=workbench_visible(a) && count==1 && (mask&((1u<<PS_DOCK_WORKSPACE)|(1u<<PS_DOCK_LOG)))?0:fminf(.25f,close_size/fmaxf(1,r.w));
            bool laboratory_heading=laboratory_visible(a) && count==1 && (mask&((1u<<PS_DOCK_SIDEBAR)|(1u<<PS_DOCK_INSPECTOR)));
            if(laboratory_heading)close_fraction=0;
            nk_layout_row_begin(ui,NK_DYNAMIC,r.h,(int)count+1);
            for(unsigned j=0;j<PS_DOCK_PANELS;j++) if(mask&(1u<<j)) {
                bool files=!panel_is_detached(a,PS_DOCK_WORKSPACE) && !laboratory_visible(a) && j==PS_DOCK_WORKSPACE && (floating || d->nodes[i].active==j) && (a->loaded || a->document_count);
                float fraction=count==1 || !(mask&(1u<<PS_DOCK_WORKSPACE))?(1-close_fraction)/count:j==PS_DOCK_WORKSPACE?(1-close_fraction)*.68f:(1-close_fraction)*.32f/(count-1);
                nk_layout_row_push(ui,fraction);
                a->dock_headers[j]=nk_widget_bounds(ui);
                panel_pull_handle(a,j,a->dock_headers[j]);
                if(files) {
                    struct nk_rect strip;nk_widget(&strip,ui);
                    a->dock_headers[j]=nk_rect(strip.x,strip.y,workbench_visible(a)?6:24,strip.h);
                    if(!workbench_visible(a)) {ps_ui_draw_icon(nk_window_get_canvas(ui),PS_ICON_DRAG,a->dock_headers[j],UI_COLOR(muted));strip.x+=24;strip.w=fmaxf(1,strip.w-24);}
                    if(editor_search_target(a)) {
                        if(workbench_visible(a))strip=workbench_editor_tools(a,strip,
                            (d->hidden&(1u<<PS_DOCK_INSPECTOR))!=0,(d->hidden&(1u<<PS_DOCK_SIDEBAR))!=0);
                        else {
                            a->workbench_search_bounds=nk_rect(strip.x+strip.w-64,strip.y,32,strip.h);
                            a->workbench_save_bounds=nk_rect(strip.x+strip.w-32,strip.y,32,strip.h);
                            ps_ui_draw_icon(nk_window_get_canvas(ui),PS_ICON_SEARCH,a->workbench_search_bounds,UI_COLOR(muted));
                            ps_ui_draw_icon(nk_window_get_canvas(ui),PS_ICON_SAVE,a->workbench_save_bounds,UI_COLOR(muted));
                            if(ps_ui_button_at(ui,a->workbench_search_bounds,"Dokumentsuche öffnen",false))editor_search_open(a);
                            nk_begin_disabled(ui,!idle(a));
                            if(ps_ui_button_at(ui,a->workbench_save_bounds,"Aktive Datei speichern",false))save_active_document(a);
                            nk_end_disabled(ui);strip.w=fmaxf(1,strip.w-64);
                        }
                    }
                    struct nk_rect pull=strip;
                    if(!workbench_visible(a)){pull.x=a->dock_headers[j].x;pull.w=strip.x+strip.w-pull.x;}
                    panel_pull_handle(a,j,pull);
                    file_tabs_view(a,strip);
                } else {
                    bool active=floating || d->nodes[i].active==j;
                    if(workbench_visible(a) && j==PS_DOCK_LOG && count==1) {
                        struct nk_rect b;nk_widget(&b,ui);struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
                        const struct nk_user_font *font=&a->workbench_fonts[0],*small=&a->workbench_fonts[1];
                        float cy=b.y+b.h/2;
                        if(a->show_log){nk_stroke_line(canvas,b.x+12,cy-2,b.x+15,cy+2,1,UI_COLOR(muted));nk_stroke_line(canvas,b.x+15,cy+2,b.x+18,cy-2,1,UI_COLOR(muted));}
                        else {nk_stroke_line(canvas,b.x+13,cy-3,b.x+17,cy,1,UI_COLOR(muted));nk_stroke_line(canvas,b.x+17,cy,b.x+13,cy+3,1,UI_COLOR(muted));}
                        float title_width=font->width(font->userdata,font->height,"Protokoll",9);
                        nk_draw_text(canvas,nk_rect(b.x+23,cy-font->height/2,title_width+1,font->height),"Protokoll",9,font,nk_rgba(0,0,0,0),UI_COLOR(text));
                        char summary[60];snprintf(summary,sizeof summary,"%d Diagnosen",a->diagnostic_count);
                        const char *profile=a->profile?"Release":"Debug";
                        float profile_width=small->width(small->userdata,small->height,profile,(int)strlen(profile));
                        float summary_x=b.x+23+title_width+13;
                        nk_draw_text(canvas,nk_rect(summary_x,cy-small->height/2,fmaxf(1,b.x+b.w-26-profile_width-summary_x),small->height),summary,(int)strlen(summary),small,nk_rgba(0,0,0,0),UI_COLOR(muted));
                        nk_draw_text(canvas,nk_rect(b.x+b.w-13-profile_width,cy-small->height/2,profile_width+1,small->height),profile,(int)strlen(profile),small,nk_rgba(0,0,0,0),UI_COLOR(muted));
                        if(ps_ui_button_at(ui,nk_rect(b.x+6,b.y+6,23+title_width,b.h-12),"Protokoll ein-/ausklappen",a->show_log))protocol_toggle(a,"Protokoll ein-/ausklappen");
                    } else if(laboratory_heading) {
                        struct nk_rect header;nk_widget(&header,ui);laboratory_panel_heading(a,j,header);
                    } else {
                        bool heading=workbench_visible(a) && (j==PS_DOCK_SIDEBAR || j==PS_DOCK_INSPECTOR);
                        if(heading)nk_style_push_font(ui,&a->workbench_fonts[3]);
                        const char *title=workbench_visible(a)?j==PS_DOCK_SIDEBAR?"Workspace":j==PS_DOCK_INSPECTOR?"CODEKONTEXT":dock_names[j]:laboratory_visible(a) && j==PS_DOCK_SIDEBAR?"Szenenbaum":dock_names[j];
                        if(nk_button_label(ui,title))ps_dock_select(d,j);
                        if(heading)nk_style_pop_font(ui);
                    }
                    if(active && count>1) {
                        struct nk_rect b=a->dock_headers[j];
                        nk_stroke_line(nk_window_get_canvas(ui),b.x,b.y+b.h-1,b.x+b.w,b.y+b.h-1,1,UI_COLOR(accent));
                    }
                }
            }
            unsigned active=floating?p:d->nodes[i].active;
            nk_layout_row_push(ui,close_fraction);
            a->dock_closes[active]=nk_widget_bounds(ui);
            bool context=workbench_visible(a) && (active==PS_DOCK_INSPECTOR || active==PS_DOCK_WORKSPACE);
            if(workbench_visible(a) && active==PS_DOCK_LOG){nk_spacer(ui);a->dock_closes[active]=nk_rect(0,0,0,0);}
            else if(context)nk_spacer(ui);
            else if(laboratory_heading) {nk_spacer(ui);a->dock_closes[active]=a->laboratory_sides[active==PS_DOCK_SIDEBAR?0:1];}
            else if(workbench_visible(a) && active==PS_DOCK_SIDEBAR) {
                struct nk_rect cell;nk_widget(&cell,ui);a->workbench_sidebar_bounds=nk_rect(cell.x+4,cell.y+6,24,28);
                a->dock_closes[active]=a->workbench_sidebar_bounds;
                if(workbench_side_button(a,a->workbench_sidebar_bounds,false,true,"Workspace ein-/ausblenden")) {close=active;a->workbench_sidebar_focus=3;}
            } else if(ps_ui_icon_button(ui,PS_ICON_CLOSE,"Panel schließen"))close=active;
            nk_layout_row_end(ui);
            if(laboratory_visible(a) && active==PS_DOCK_INSPECTOR && a->built && SDL_getenv("PHYSIM_TEST_TRACE")) {
                static bool reported;
                if(!reported){struct nk_rect clip=ui->current->layout->clip;printf("LABORATORY HEADER: r=%.1f,%.1f,%.1f,%.1f clip=%.1f,%.1f,%.1f,%.1f close=%.1f,%.1f,%.1f,%.1f\n",r.x,r.y,r.w,r.h,clip.x,clip.y,clip.w,clip.h,a->dock_closes[active].x,a->dock_closes[active].y,a->dock_closes[active].w,a->dock_closes[active].h);reported=true;}
            }
            if(workbench_visible(a) && active==PS_DOCK_INSPECTOR && count==1) {
                struct nk_text_edit *edit=editor_search_target(a);int language=edit?document_language(a,edit):-1;
                const char *label=language<0?"Text":language?"Physim":"C";const struct nk_user_font *font=&a->workbench_fonts[1];
                float width=fmaxf(18,font->width(font->userdata,font->height,label,(int)strlen(label))+8);
                struct nk_rect badge=nk_rect(r.x+r.w-43-width,r.y+11,width,18);struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
                nk_stroke_rect(canvas,badge,3,1,UI_COLOR(border));
                nk_draw_text(canvas,nk_rect(badge.x+4,badge.y+(badge.h-font->height)/2,badge.w-8,font->height),label,(int)strlen(label),font,nk_rgba(0,0,0,0),UI_COLOR(muted));
                ps_ui_accessibility_label(ui,badge,label);
            }
            if(workbench_visible(a) && (active==PS_DOCK_WORKSPACE || active==PS_DOCK_LOG))
                workspace_rule(nk_window_get_canvas(ui),nk_rect(r.x,r.y+r.h-1,r.w,1));
        }
        nk_end(ui);nk_style_pop_style_item(ui);nk_style_pop_style_item(ui);nk_style_pop_float(ui);nk_style_pop_float(ui);
        nk_style_pop_flags(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
    }
    if(close!=PS_DOCK_NONE) {
        ps_dock_hide(d,close);
        if(laboratory_visible(a) && (close==PS_DOCK_SIDEBAR || close==PS_DOCK_INSPECTOR))a->laboratory_focus_return=close==PS_DOCK_SIDEBAR?1:2;
        if(close==PS_DOCK_LOG) a->show_log=false;
    }
    if(a->workbench_sidebar_focus>1)a->workbench_sidebar_focus--;
    else if(a->workbench_sidebar_focus && nk_sdl_accessibility_request_focus(ui,NULL,"Workspace ein-/ausblenden"))a->workbench_sidebar_focus=0;
    for(unsigned p=0;p<PS_DOCK_PANELS;p++) if(d->floating&(1u<<p)) {
        struct nk_rect r=a->dock_panels[p];a->dock_float_grips[p]=nk_rect(r.x+r.w-18,r.y+r.h-18,18,18);
    }
    nk_window_show(ui,"workbench-context-toggle",workbench_visible(a)?NK_SHOWN:NK_HIDDEN);
    if(workbench_visible(a) && a->dock_visible[PS_DOCK_WORKSPACE]) {
        struct nk_rect anchor=(d->floating&(1u<<PS_DOCK_WORKSPACE))?a->dock_panels[PS_DOCK_WORKSPACE]:
            d->root!=PS_DOCK_NONE?a->dock_rects[d->root]:a->dock_panels[PS_DOCK_WORKSPACE];
        struct nk_rect button=nk_rect(anchor.x+anchor.w-32,a->dock_panels[PS_DOCK_WORKSPACE].y,32,dock_header_height(a));
        a->workbench_context_bounds=nk_rect(button.x,button.y+6,24,28);
        nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(0,0));
        nk_style_push_float(ui,&ui->style.window.rounding,0);
        struct nk_window *previous=ui->active;
        if(nk_begin(ui,"workbench-context-toggle",button,NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BACKGROUND)) {
            nk_sdl_window_raise(ui,"workbench-context-toggle");
            bool blocked=a->toolbar_menu || a->toolbar_pointer_consumed;
            bool pointer=nk_input_is_mouse_hovering_rect(&ui->input,button);
            if(blocked){ui->current->flags|=NK_WINDOW_ROM;ui->current->layout->flags|=NK_WINDOW_ROM;}
            else {ui->current->flags&=~(nk_flags)NK_WINDOW_ROM;ui->current->layout->flags&=~(nk_flags)NK_WINDOW_ROM;}
            ps_ui_accessibility_route(ui,!blocked);ui->active=pointer && !blocked?ui->current:previous;
            bool hidden=(d->hidden&(1u<<PS_DOCK_INSPECTOR))!=0;
            if(workbench_side_button(a,a->workbench_context_bounds,true,!hidden,"Codekontext ein-/ausblenden")) {
                nk_sdl_accessibility_request_focus(ui,NULL,"Codekontext ein-/ausblenden");
                if(hidden)ps_dock_reveal_outer(d,PS_DOCK_INSPECTOR,PS_DOCK_RIGHT);else ps_dock_hide(d,PS_DOCK_INSPECTOR);
            }
        }
        nk_end(ui);nk_style_pop_float(ui);nk_style_pop_vec2(ui);
    }
}
static void dock_order(app *a) {
    ps_dock_layout *d=&a->preferences.dock;char name[48];
    for(unsigned p=0;p<PS_DOCK_PANELS;p++) if(!(d->floating&(1u<<p))) nk_sdl_window_raise(a->ui,dock_windows[p]);
    for(unsigned i=0;i<PS_DOCK_NODES;i++) {
        snprintf(name,sizeof name,"dock-heading-%u",i);nk_sdl_window_raise(a->ui,name);
    }
    if(!(d->floating&(1u<<PS_DOCK_WORKSPACE)))nk_sdl_window_raise(a->ui,"workbench-editor-status");
    for(unsigned pass=0;pass<2;pass++) for(unsigned p=0;p<PS_DOCK_PANELS;p++)
        if((d->floating&(1u<<p)) && ((p==a->dock_front)==(pass==1))) {
            nk_sdl_window_raise(a->ui,dock_windows[p]);
            snprintf(name,sizeof name,"dock-heading-%u",PS_DOCK_NODES+p);nk_sdl_window_raise(a->ui,name);
            if(p==PS_DOCK_WORKSPACE)nk_sdl_window_raise(a->ui,"workbench-editor-status");
        }
    nk_sdl_window_raise(a->ui,"header");
    nk_sdl_window_raise(a->ui,"workbench-context-toggle");
}
static void dock_separator_views(app *a) {
    struct nk_context *ui=a->ui;
    for(unsigned id=0;id<PS_DOCK_NODES;id++) {
        char window[48];snprintf(window,sizeof window,"dock-splitter-%u",id);
        struct nk_rect r=a->dock_splitters[id];unsigned kind=a->preferences.dock.nodes[id].kind;
        bool visible=r.w>0 && r.h>0 && (kind==PS_DOCK_X || kind==PS_DOCK_Y);
        nk_window_show(ui,window,visible?NK_SHOWN:NK_HIDDEN);if(!visible)continue;
        bool vertical=a->preferences.dock.nodes[id].kind==PS_DOCK_X;
        if(!workspace_shell_visible(a)){if(vertical){r.x-=.5f;r.w=9;}else {r.y-=.5f;r.h=9;}}
        nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(0,0));
        nk_style_push_float(ui,&ui->style.window.rounding,0);
        nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(nk_rgba(0,0,0,0)));
        struct nk_window *previous=ui->active;
        if(nk_begin(ui,window,r,NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BACKGROUND)) {
            nk_sdl_window_raise(ui,window);
            unsigned under=dock_hover_float(a);bool blocked=a->toolbar_menu || a->toolbar_pointer_consumed || under!=PS_DOCK_NONE;
            ps_ui_accessibility_route(ui,!blocked);
            if(blocked){ui->current->flags|=NK_WINDOW_ROM;ui->current->layout->flags|=NK_WINDOW_ROM;}
            else {ui->current->flags&=~(nk_flags)NK_WINDOW_ROM;ui->current->layout->flags&=~(nk_flags)NK_WINDOW_ROM;}
            ui->active=previous;
            float minimum,maximum,standard;bool reverse;
            float value=dock_resize_value(a,id,&minimum,&maximum,&standard,&reverse);
            minimum=fminf(value,minimum);maximum=fmaxf(value,maximum);
            char label[48];const char *name=dock_resize_label(a,id,label);
            if(ps_ui_separator_at(ui,r,name,minimum,&value,maximum))dock_resize_set(a,id,value);
            if(nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT) && nk_input_is_mouse_hovering_rect(&ui->input,r) && !blocked)
                nk_sdl_accessibility_request_focus(ui,NULL,name);
            if(nk_sdl_accessibility_is_focused(ui,NULL,name) || (!blocked && nk_input_is_mouse_hovering_rect(&ui->input,r))) {
                struct nk_rect line=vertical?nk_rect(floorf(r.x+r.w/2),r.y,1,r.h):nk_rect(r.x,floorf(r.y+r.h/2),r.w,1);
                nk_fill_rect(nk_window_get_canvas(ui),line,0,UI_COLOR(accent));
            }
        }
        nk_end(ui);nk_style_pop_style_item(ui);nk_style_pop_float(ui);nk_style_pop_vec2(ui);
        nk_sdl_window_raise(ui,window);
    }
}
static void dock_overlay(app *a,float w,float h) {
    struct nk_command_buffer *canvas=nk_window_get_canvas(a->ui);
    struct nk_rect previous=canvas->clip;
    nk_push_scissor(canvas,nk_rect(0,0,w,h));
    for(unsigned i=0;i<PS_DOCK_NODES;i++) if(a->dock_splitters[i].w>0) {
        struct nk_rect r=a->dock_splitters[i];bool x=a->preferences.dock.nodes[i].kind==PS_DOCK_X;
        if(workspace_shell_visible(a)) {
            float position=floorf(x?r.x+r.w/2:r.y+r.h/2);
            if(x && dock_sidebar_split(&a->preferences.dock,&a->preferences.dock.nodes[i]))position-=1;
            nk_fill_rect(canvas,x?nk_rect(position,r.y,1,r.h):nk_rect(r.x,position,r.w,1),0,UI_COLOR(divider));
        } else nk_stroke_line(canvas,r.x+(x?r.w/2:0),r.y+(x?0:r.h/2),r.x+(x?r.w/2:r.w),r.y+(x?r.h:r.h/2),2,UI_COLOR(divider));
    }
    for(unsigned p=0;p<3;p++) if(a->preferences.dock.floating&(1u<<p)) nk_stroke_rect(canvas,a->dock_float_grips[p],2,1,UI_COLOR(accent));
    if(a->dock_dragging) {
        nk_stroke_rect(canvas,a->dock_preview,5,2,UI_COLOR(accent));
        for(unsigned side=0;side<5;side++) if(a->dock_targets[side].w>0)
            nk_fill_rect(canvas,a->dock_targets[side],4,side==a->dock_side?UI_COLOR(primary):UI_COLOR(button));
    }
    nk_push_scissor(canvas,previous);
}
