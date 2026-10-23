/* The workbench reference: one 5px strip inset, 3px gaps, natural tab widths.
 * The native close target occupies the existing right padding and the gap;
 * it does not reserve a blank extra column in every resting tab. */
static float file_tabs_height(const app *a) {return fmaxf(40,40*a->preferences.ui_size/16.f);}
static float file_tab_marker_width(app *a,const char *name) {
    const char *ext=strrchr(name,'.');const struct nk_user_font *font=&a->workbench_marker_font;
    if(ext && (!strcmp(ext,".c") || !strcmp(ext,".h") || !strcmp(ext,".phys")))
        return font->width(font->userdata,font->height,!strcmp(ext,".phys")?"P":"C",1);
    return (ext && !strcmp(ext,".project")?8:14)*a->preferences.ui_size/16.f;
}
static struct nk_rect file_tab_clip(struct nk_rect r,struct nk_rect clip) {
    float x=fmaxf(r.x,clip.x),y=fmaxf(r.y,clip.y);
    return nk_rect(x,y,fmaxf(0,fminf(r.x+r.w,clip.x+clip.w)-x),fmaxf(0,fminf(r.y+r.h,clip.y+clip.h)-y));
}
static void file_tabs_compact_view(app *a,struct nk_rect bounds) {
    struct nk_context *ui=a->ui;struct nk_input *in=&ui->input;struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    const struct nk_user_font *font=&a->workbench_fonts[0],*dot=&a->workbench_tab_dot_font;
    float f=a->preferences.ui_size/16.f;
    float space=font->width(font->userdata,font->height," ",1),dot_width=dot->width(dot->userdata,dot->height,"●",3);
    float widths[FILE_TABS]={0},total=10*f;const char *names[FILE_TABS]={0};bool dirty[FILE_TABS]={0};unsigned count=0;
    unsigned active=file_tab_active(a),chosen=FILE_TABS,close=FILE_TABS;
    memset(a->file_tab_bounds,0,sizeof a->file_tab_bounds);memset(a->file_tab_closes,0,sizeof a->file_tab_closes);a->file_tab_strip=bounds;
    for(unsigned i=0;i<FILE_TABS;i++) {
        unsigned key=a->file_tab_order[i];if(!file_tab_visible(a,key))continue;
        names[key]=key<2?(key?analysis_source(a):experiment_source(a)):workspace_name(a->documents[key-2].path);
        dirty[key]=key<2?(key?a->analysis_dirty:a->dirty):a->documents[key-2].dirty;
        widths[key]=22*f+file_tab_marker_width(a,names[key])+space+font->width(font->userdata,font->height,names[key],(int)strlen(names[key]))+(dirty[key]?8*f+dot_width:0);
        total+=widths[key]+(count++?3*f:0);
    }
    bool overflow=total>bounds.w;float viewport=fmaxf(1,bounds.w-(overflow?48*f:0));
    bool enabled=!(ui->current->layout->flags&NK_WINDOW_ROM) && !ui->current->widgets_disabled;
    if(enabled && nk_input_is_mouse_hovering_rect(in,bounds)) {a->file_tab_scroll-=in->mouse.scroll_delta.y*50*f;in->mouse.scroll_delta.y=0;}
    if(!a->file_tab_drag && active<FILE_TABS && a->file_tab_last_active!=active) {
        float left=5*f;for(unsigned i=0;i<FILE_TABS && a->file_tab_order[i]!=active;i++)if(widths[a->file_tab_order[i]])left+=widths[a->file_tab_order[i]]+3*f;
        if(widths[active]>viewport-10*f)a->file_tab_scroll=left-5*f;
        else if(left<a->file_tab_scroll+5*f)a->file_tab_scroll=left-5*f;
        else if(left+widths[active]>a->file_tab_scroll+viewport-5*f)a->file_tab_scroll=left+widths[active]-viewport+5*f;
    }
    a->file_tab_scroll=fmaxf(0,fminf(fmaxf(0,total-viewport),a->file_tab_scroll));a->file_tab_last_active=active;
    struct nk_rect old=canvas->clip,strip=file_tab_clip(nk_rect(bounds.x,bounds.y,viewport,bounds.h),old);nk_push_scissor(canvas,strip);
    float x=bounds.x+5*f-a->file_tab_scroll,h=28*f;
    for(unsigned i=0;i<FILE_TABS;i++) {
        unsigned key=a->file_tab_order[i];if(!names[key])continue;
        struct nk_rect pill=nk_rect(x,bounds.y+(bounds.h-h)/2,widths[key],h),visible=file_tab_clip(pill,strip);
        a->file_tab_bounds[key]=visible;bool hover=enabled && visible.w>0 && nk_input_is_mouse_hovering_rect(in,visible);
        if(key==active || hover)nk_fill_rect(canvas,pill,5*f,key==active?UI_COLOR(selection_active):UI_COLOR(button_hover));
        float marker=file_tab_marker_width(a,names[key]),text_x=pill.x+11*f+marker+space;
        const char *ext=strrchr(names[key],'.');bool project=ext && !strcmp(ext,".project");
        workbench_file_icon(a,canvas,names[key],nk_rect(pill.x+11*f,pill.y,marker,pill.h),project?(key==active?UI_COLOR(text):UI_COLOR(muted)):UI_COLOR(accent));
        float text_width=font->width(font->userdata,font->height,names[key],(int)strlen(names[key]));
        nk_draw_text(canvas,nk_rect(text_x,pill.y+(pill.h-font->height)/2,text_width+1,font->height),names[key],(int)strlen(names[key]),font,nk_rgba(0,0,0,0),key==active?UI_COLOR(text):UI_COLOR(muted));
        if(dirty[key])nk_draw_text(canvas,nk_rect(text_x+text_width+8*f,pill.y+(pill.h-dot->height)/2,dot_width+1,dot->height),"●",3,dot,nk_rgba(0,0,0,0),UI_COLOR(accent));
        struct nk_rect cb=file_tab_clip(nk_rect(pill.x+pill.w-11*f,pill.y,13*f,pill.h),strip);a->file_tab_closes[key]=cb;
        char label[4101],close_label[4120];snprintf(label,sizeof label,"%s%s",names[key],dirty[key]?" •":"");snprintf(close_label,sizeof close_label,"%s schließen",label);
        bool close_focus=false,focus=false;
        if(ps_ui_button_at_custom_focus(ui,cb,close_label,false,&close_focus))close=key;
        bool close_hover=enabled && cb.w>0 && nk_input_is_mouse_hovering_rect(in,cb);
        if(close_hover)nk_fill_rect(canvas,nk_rect(cb.x,pill.y,cb.w,pill.h),3*f,UI_COLOR(button_hover));
        if(hover || close_focus) {
            float cx=pill.x+pill.w-4.5f*f,cy=pill.y+pill.h/2;
            nk_stroke_line(canvas,cx-2.5f*f,cy-2.5f*f,cx+2.5f*f,cy+2.5f*f,f,UI_COLOR(muted));nk_stroke_line(canvas,cx-2.5f*f,cy+2.5f*f,cx+2.5f*f,cy-2.5f*f,f,UI_COLOR(muted));
        }
        if(close_focus)nk_stroke_rect(canvas,nk_rect(cb.x+f,cb.y+2*f,fmaxf(1,cb.w-2*f),cb.h-4*f),3*f,2*f,UI_COLOR(accent));
        struct nk_rect select=visible;select.w=fmaxf(0,fminf(visible.x+visible.w,pill.x+pill.w-11*f)-visible.x);
        if(ps_ui_button_at_custom_focus(ui,select,label,key==active,&focus)) {
            chosen=key;if(nk_input_is_mouse_pressed(in,NK_BUTTON_LEFT)) {a->file_tab_drag=(int)key+1;a->file_tab_origin=in->mouse.pos;memcpy(a->file_tab_drag_order,a->file_tab_order,sizeof a->file_tab_order);}
        }
        if(focus)nk_stroke_rect(canvas,nk_rect(pill.x+2*f,pill.y+2*f,pill.w-4*f,pill.h-4*f),3*f,2*f,UI_COLOR(accent));
        x+=widths[key]+3*f;
    }
    nk_push_scissor(canvas,old);
    if(overflow)for(unsigned i=0;i<2;i++) {
        struct nk_rect arrow=nk_rect(bounds.x+viewport+i*24*f,bounds.y,24*f,bounds.h);
        float cx=arrow.x+arrow.w/2,cy=arrow.y+arrow.h/2,sign=i?1.f:-1.f;
        nk_stroke_line(canvas,cx-sign*2*f,cy-4*f,cx+sign*2*f,cy,1.25f*f,UI_COLOR(muted));nk_stroke_line(canvas,cx+sign*2*f,cy,cx-sign*2*f,cy+4*f,1.25f*f,UI_COLOR(muted));
        if(ps_ui_button_at(ui,arrow,i?"Weitere Datei-Tabs":"Vorherige Datei-Tabs",false))a->file_tab_scroll+=i?140*f:-140*f;
    }
    if(chosen<FILE_TABS)file_tab_select(a,chosen);if(close<FILE_TABS)file_tab_close(a,close);
}
