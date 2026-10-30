/* A stable order of keys, independent of editor storage and undo buffers. */
static bool file_tab_visible(const app *a,unsigned key) {
    if(key<2)return a->loaded && !(key==0 && a->analysis_only) && !a->source_tab_closed[key];
    return key-2<a->document_count;
}
static unsigned file_tab_active(const app *a) {
    if(a->tab==8 && a->document_active<a->document_count)return a->document_active+2;
    if(a->loaded && (a->tab==0 || (a->tab==2 && a->analysis_tab==1)))return a->tab==2 || a->analysis_only?1:0;
    return FILE_TABS;
}
static void file_tab_select(app *a,unsigned key) {
    completion_close(a);
    if(key>=2)document_select(a,key-2);
    else {
        a->source_tab_closed[key]=false;a->tab=key==0?0:2;if(key==1)a->analysis_tab=1;
        a->completion_focus_edit=key==0?&a->experiment:&a->analysis;
        join(a->workspace_preview_path,sizeof a->workspace_preview_path,a->project,key?analysis_source(a):experiment_source(a));
    }
    /* An analysis-header action runs after the frame's initial context pass.
     * Restore the workbench before any editor panel measures or paints. */
    workspace_context(a);
    dock_show(a,PS_DOCK_WORKSPACE);
    /* A marked edit alone does not transfer keyboard ownership from the
     * header/search control. Match declaration navigation's window handoff. */
    a->search_keyboard=false;a->search_focus=false;a->search_icon_focus=false;
    nk_sdl_accessibility_keyboard_begin(a->ui);
    a->file_tab_keyboard_edit=a->completion_focus_edit;
    nk_window_set_focus(a->ui,"workspace");
}
static bool file_tabs_keyboard(app *a,const SDL_KeyboardEvent *key) {
    if(a->document_pending || a->recovery || a->toolbar_menu)return false;
    bool cycle=(key->mod&SDL_KMOD_CTRL) && (key->key==SDLK_TAB || key->key==SDLK_PAGEUP || key->key==SDLK_PAGEDOWN);
    if(!cycle)return false;
    if(key->repeat)return true;
    unsigned active=file_tab_active(a),at=FILE_TABS-1;
    for(unsigned i=0;i<FILE_TABS;i++)if(a->file_tab_order[i]==active)at=i;
    int direction=key->key==SDLK_PAGEUP || (key->mod&SDL_KMOD_SHIFT)?-1:1;
    for(unsigned i=0;i<FILE_TABS;i++) {
        at=(at+FILE_TABS+direction)%FILE_TABS;
        if(file_tab_visible(a,a->file_tab_order[at])){file_tab_select(a,a->file_tab_order[at]);return true;}
    }
    return true;
}
static void file_tabs_prepare(app *a) {
    if(!a->file_tabs_initialized) {
        for(unsigned i=0;i<FILE_TABS;i++)a->file_tab_order[i]=i;
        a->file_tabs_initialized=true;
        a->file_tab_last_active=FILE_TABS;
    }
    unsigned active=file_tab_active(a);
    if(active<2)a->source_tab_closed[active]=false;
    struct nk_input *in=&a->ui->input;
    if(!a->file_tab_drag)return;
    bool down=nk_input_is_mouse_down(in,NK_BUTTON_LEFT);
    if(down && (fabsf(in->mouse.pos.x-a->file_tab_origin.x)>6 || fabsf(in->mouse.pos.y-a->file_tab_origin.y)>6))a->file_tab_dragging=true;
    if(a->file_tab_dragging && down && nk_input_is_mouse_hovering_rect(in,a->file_tab_strip)) {
        if(in->mouse.pos.x<a->file_tab_strip.x+24)a->file_tab_scroll-=6;
        if(in->mouse.pos.x>a->file_tab_strip.x+a->file_tab_strip.w-24)a->file_tab_scroll+=6;
        unsigned key=(unsigned)a->file_tab_drag-1;
        for(unsigned target=0;target<FILE_TABS;target++) {
            struct nk_rect b=a->file_tab_bounds[target];
            if(target==key || b.w<=0 || !nk_input_is_mouse_hovering_rect(in,b))continue;
            unsigned from=0,to=0;
            for(unsigned i=0;i<FILE_TABS;i++){if(a->file_tab_order[i]==key)from=i;if(a->file_tab_order[i]==target)to=i;}
            /* Cross the target midpoint in the drag direction before reordering. */
            if((from<to && in->mouse.pos.x<b.x+b.w/2) || (from>to && in->mouse.pos.x>b.x+b.w/2))break;
            if(from<to)memmove(&a->file_tab_order[from],&a->file_tab_order[from+1],(to-from)*sizeof(unsigned));
            else memmove(&a->file_tab_order[to+1],&a->file_tab_order[to],(from-to)*sizeof(unsigned));
            a->file_tab_order[to]=key;break;
        }
    }
    if(!down){a->file_tab_drag=0;a->file_tab_dragging=false;}
}
static void file_tab_close(app *a,unsigned key) {
    unsigned previous=file_tab_active(a);
    if(key>=2) {
        unsigned count=a->document_count;
        document_select(a,key-2);document_request(a,1);
        if(previous!=key && previous<FILE_TABS && count>a->document_count)
            file_tab_select(a,previous>key?previous-1:previous);
        return;
    }
    a->source_tab_closed[key]=true;
    if(previous!=key)return;
    for(unsigned i=0;i<FILE_TABS;i++)if(file_tab_visible(a,a->file_tab_order[i])){file_tab_select(a,a->file_tab_order[i]);return;}
    a->tab=2;a->analysis_tab=0;
}
#include "file_tabs_compact.inc"
static void file_tabs_view(app *a,struct nk_rect bounds) {
    if(workbench_visible(a)){file_tabs_compact_view(a,bounds);return;}
    struct nk_context *ui=a->ui;struct nk_input *in=&ui->input;
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    unsigned active=file_tab_active(a),chosen=FILE_TABS,close=FILE_TABS;
    bool compact=workbench_visible(a);
    memset(a->file_tab_bounds,0,sizeof a->file_tab_bounds);memset(a->file_tab_closes,0,sizeof a->file_tab_closes);
    a->file_tab_strip=bounds;
    float widths[FILE_TABS]={0},total=0;
    char labels[FILE_TABS][256];
    for(unsigned i=0;i<FILE_TABS;i++) {
        unsigned key=a->file_tab_order[i];if(!file_tab_visible(a,key))continue;
        const char *name=key<2?(key?analysis_source(a):experiment_source(a)):workspace_name(a->documents[key-2].path);
        bool dirty=key<2?(key?a->analysis_dirty:a->dirty):a->documents[key-2].dirty;
        snprintf(labels[key],sizeof labels[key],"%s%s",name,dirty?" •":"");
        widths[key]=fminf(220,fmaxf(compact?76:110,ui->style.font->width(ui->style.font->userdata,ui->style.font->height,labels[key],(int)strlen(labels[key]))+(compact?44:52)));
        total+=widths[key];
    }
    bool overflow=total>bounds.w;float strip_width=fmaxf(1,bounds.w-(overflow?48:0));
    if(nk_input_is_mouse_hovering_rect(in,bounds))a->file_tab_scroll-=in->mouse.scroll_delta.y*50;
    a->file_tab_scroll=fmaxf(0,fminf(fmaxf(0,total-strip_width),a->file_tab_scroll));
    /* Selecting via the tree or a shortcut always reveals the active file. */
    float active_x=0;
    for(unsigned i=0;i<FILE_TABS;i++){unsigned key=a->file_tab_order[i];if(key==active)break;active_x+=widths[key];}
    if(!a->file_tab_drag && a->file_tab_last_active!=active && active<FILE_TABS) {
        if(active_x<a->file_tab_scroll)a->file_tab_scroll=active_x;
        if(active_x+widths[active]>a->file_tab_scroll+strip_width)a->file_tab_scroll=active_x+widths[active]-strip_width;
    }
    a->file_tab_last_active=active;
    struct nk_rect old=canvas->clip,strip=nk_rect(bounds.x,bounds.y,strip_width,bounds.h);
    nk_push_scissor(canvas,strip);
    float x=-a->file_tab_scroll;
    for(unsigned i=0;i<FILE_TABS;i++) {
        unsigned key=a->file_tab_order[i];float width=widths[key];if(!width)continue;
        struct nk_rect full=nk_rect(bounds.x+x,bounds.y,width,bounds.h);
        struct nk_rect visible=nk_rect(fmaxf(strip.x,full.x),full.y,fmaxf(0,fminf(strip.x+strip.w,full.x+width)-fmaxf(strip.x,full.x)),full.h);
        a->file_tab_bounds[key]=visible;
        struct nk_rect pill=nk_rect(full.x+5,full.y+6,fmaxf(1,full.w-10),fmaxf(1,full.h-12));
        if(key==active)nk_fill_rect(canvas,compact?pill:full,compact?5:0,compact?UI_COLOR(selection_active):UI_COLOR(window));
        else if(visible.w>0 && nk_input_is_mouse_hovering_rect(in,visible) && !(ui->current->layout->flags&NK_WINDOW_ROM))nk_fill_rect(canvas,compact?pill:full,compact?5:0,UI_COLOR(button_hover));
        if(!compact && key==active)nk_stroke_line(canvas,full.x+1,full.y+full.h-1,full.x+width-1,full.y+full.h-1,1,UI_COLOR(accent));
        const char *icon_name=key<2?(key?analysis_source(a):experiment_source(a)):workspace_name(a->documents[key-2].path);
        if(compact)workbench_file_icon(a,canvas,icon_name,nk_rect(full.x+13,full.y,15,full.h),UI_COLOR(accent));
        else ps_ui_draw_icon(canvas,PS_ICON_FILE,nk_rect(full.x+6,full.y,20,full.h),UI_COLOR(muted));
        nk_draw_text(canvas,nk_rect(full.x+25,full.y+(full.h-ui->style.font->height)/2,width-(compact?29:49),ui->style.font->height),labels[key],(int)strlen(labels[key]),ui->style.font,nk_rgba(0,0,0,0),key==active?UI_COLOR(text):UI_COLOR(muted));
        float close_width=compact?18:24;
        struct nk_rect cb=nk_rect(full.x+width-close_width,full.y,close_width,full.h);
        struct nk_rect close_surface=compact?nk_rect(cb.x,cb.y+6,cb.w,cb.h-12):cb;
        a->file_tab_closes[key]=nk_rect(fmaxf(strip.x,cb.x),cb.y,fmaxf(0,fminf(strip.x+strip.w,cb.x+cb.w)-fmaxf(strip.x,cb.x)),cb.h);
        if(a->file_tab_closes[key].w>0 && nk_input_is_mouse_hovering_rect(in,a->file_tab_closes[key]))nk_fill_rect(canvas,close_surface,3,UI_COLOR(button_hover));
        char close_label[300];snprintf(close_label,sizeof close_label,"%s schließen",labels[key]);
        bool close_focus=nk_sdl_accessibility_is_focused(ui,NULL,close_label);
        if(!compact || close_focus || nk_input_is_mouse_hovering_rect(in,visible))ps_ui_draw_icon(canvas,PS_ICON_CLOSE,cb,UI_COLOR(muted));
        if(close_focus)nk_stroke_rect(canvas,close_surface,4,2,UI_COLOR(accent));
        if(ps_ui_button_at(ui,a->file_tab_closes[key],close_label,false))close=key;
        struct nk_rect select=visible;select.w=fmaxf(0,fminf(select.x+select.w,cb.x)-select.x);
        if(ps_ui_button_at(ui,select,labels[key],key==active)) {
            chosen=key;
            if(nk_input_is_mouse_pressed(in,NK_BUTTON_LEFT)){
                a->file_tab_drag=(int)key+1;a->file_tab_origin=in->mouse.pos;
                memcpy(a->file_tab_drag_order,a->file_tab_order,sizeof a->file_tab_order);
            }
        }
        x+=width;
    }
    nk_push_scissor(canvas,old);
    if(overflow) {
        for(unsigned i=0;i<2;i++) {
            struct nk_rect arrow=nk_rect(bounds.x+strip_width+i*24,bounds.y,24,bounds.h);
            ps_ui_draw_icon(canvas,i?PS_ICON_RIGHT:PS_ICON_LEFT,arrow,UI_COLOR(muted));
            if(ps_ui_button_at(ui,arrow,i?"Weitere Datei-Tabs":"Vorherige Datei-Tabs",false))a->file_tab_scroll+=i?140:-140;
        }
    }
    if(chosen<FILE_TABS)file_tab_select(a,chosen);
    if(close<FILE_TABS)file_tab_close(a,close);
}
