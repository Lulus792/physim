/* Native hierarchy with wrapping labels and separate selection/visibility. */
static void scene_tree_icon(struct nk_command_buffer *canvas,const ps_object *o,struct nk_rect r,struct nk_color color) {
    float f=r.w/14,x=r.x+7*f,y=r.y+r.h/2;
    switch(o->shape) {
    case PS_SPHERE:case PS_POINT:nk_fill_circle(canvas,nk_rect(x-3*f,y-3*f,6*f,6*f),color);break;
    case PS_GROUP:case PS_FRAME:
        nk_stroke_line(canvas,x,y-5*f,x+5*f,y,f,color);nk_stroke_line(canvas,x+5*f,y,x,y+5*f,f,color);
        nk_stroke_line(canvas,x,y+5*f,x-5*f,y,f,color);nk_stroke_line(canvas,x-5*f,y,x,y-5*f,f,color);break;
    case PS_LABEL:
        nk_stroke_line(canvas,x-4*f,y-4*f,x+4*f,y-4*f,f,color);nk_stroke_line(canvas,x,y-4*f,x,y+4*f,f,color);break;
    case PS_ARROW:
        nk_stroke_line(canvas,x-4*f,y+4*f,x+4*f,y-4*f,f,color);nk_stroke_line(canvas,x,y-4*f,x+4*f,y-4*f,f,color);
        nk_stroke_line(canvas,x+4*f,y-4*f,x+4*f,y,f,color);break;
    case PS_POLYLINE:
        nk_stroke_curve(canvas,x-5*f,y+1*f,x-2*f,y-5*f,x+2*f,y+5*f,x+5*f,y-1*f,f,color);break;
    default:
        if(o->shape==PS_LINE)nk_stroke_line(canvas,x-4*f,y+4*f,x+4*f,y-4*f,f,color);
        else {nk_stroke_rect(canvas,nk_rect(x-4*f,y-4*f,8*f,8*f),0,f,color);nk_stroke_line(canvas,x,y-4*f,x,y+4*f,f,color);nk_stroke_line(canvas,x-4*f,y,x+4*f,y,f,color);}
        break;
    }
}
static void scene_tree_row(app *a,uint32_t slot,unsigned depth) {
    struct nk_context *ui=a->ui;const ps_scene *scene=display_scene(a);
    const ps_object *o=&scene->objects[slot];
    static const char *names[]={"Kugel","Linie","Box","Vektor","Punkt","Ebene","Pfad","Beschriftung","Gruppe","Koordinatenrahmen"};
    bool children=false;
    if(o->id) for(uint32_t i=0;i<scene->count;i++) children |= scene->objects[i].parent_id==o->id;
    uint32_t bit=UINT32_C(1)<<slot;
    char label[256],name[320];
    if((o->shape==PS_GROUP || o->shape==PS_FRAME) && o->text[0]) snprintf(label,sizeof label,"%s",o->text);
    else if(o->id) snprintf(label,sizeof label,"ID %u · %s%s%s",o->id,names[o->shape],o->text[0]?" · ":"",o->text);
    else snprintf(label,sizeof label,"%u · %s%s%s",slot+1,names[o->shape],o->text[0]?" · ":"",o->text);
    const struct nk_user_font *font=&a->workbench_fonts[2];
    float available=nk_window_get_content_region(ui).w;
    float indent=fminf((float)depth*12,fmaxf(0,fminf(48,available-130)));
    float text_width=fmaxf(16,available-indent-66);
    float text_height=workbench_card_text_height(font,label,text_width);
    nk_layout_row_dynamic(ui,fmaxf(33,text_height+12),1);struct nk_rect row;
    enum nk_widget_layout_states state=nk_widget(&row,ui);
    /* Keep full layout targets for scroll-to-item callers, including rows
     * outside the clip. Pointer/assistive hit targets remain clipped below. */
    a->scene_visibility_bounds[slot]=nk_rect(row.x+row.w-22,row.y,22,row.h);
    if(children)a->scene_expand_bounds[slot]=nk_rect(row.x+indent,row.y,20,row.h);
    if(state!=NK_WIDGET_INVALID) {
        struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
        bool effective=ps_scene_view_visible(&a->scene_view,slot),own_visible=!(a->scene_view.hidden&bit);
        bool selected=a->scene_selected && a->scene_selected_slot==slot;
        struct nk_rect visible=plot_intersection(row,ui->current->layout->clip);
        if(selected || nk_input_is_mouse_hovering_rect(&ui->input,visible))
            nk_fill_rect(canvas,row,5,selected?UI_COLOR(selection_active):UI_COLOR(button_hover));
        struct nk_rect expand=nk_rect(row.x+indent,row.y,20,row.h);
        if(children) {
            float x=expand.x+8,y=expand.y+expand.h/2;struct nk_color color=muted();
            if(a->scene_view.collapsed&bit) {
                nk_stroke_line(canvas,x-2,y-3,x+1,y,1,color);nk_stroke_line(canvas,x+1,y,x-2,y+3,1,color);
            } else {
                nk_stroke_line(canvas,x-3,y-2,x,y+1,1,color);nk_stroke_line(canvas,x,y+1,x+3,y-2,1,color);
            }
            snprintf(name,sizeof name,"Gruppe auf-/zuklappen: %s",label);
            if(ps_ui_button_at(ui,plot_intersection(expand,visible),name,!(a->scene_view.collapsed&bit)))a->scene_view.collapsed^=bit;
        }
        struct nk_rect icon=nk_rect(row.x+indent+20,row.y,14,row.h);
        scene_tree_icon(canvas,o,icon,selected?accent():muted());
        struct nk_rect text=nk_rect(row.x+indent+40,row.y+6,text_width,text_height);
        workbench_card_text(canvas,font,text,label,effective?UI_COLOR(text):muted());
        struct nk_rect choose=plot_intersection(nk_rect(icon.x,row.y,row.w-indent-46,row.h),visible);
        snprintf(name,sizeof name,"Objekt auswählen: %s",label);
        if(ps_ui_button_at(ui,choose,name,selected))scene_select_entry(a,slot);
        struct nk_rect eye=nk_rect(row.x+row.w-22,row.y,22,row.h);
        float x=eye.x+11,y=eye.y+eye.h/2;struct nk_color color=muted();
        nk_stroke_circle(canvas,nk_rect(x-5,y-4,10,8),1,color);
        if(effective)nk_fill_circle(canvas,nk_rect(x-1,y-1,2,2),color);
        else nk_stroke_line(canvas,x-5,y+4,x+5,y-4,1,color);
        if(ps_ui_switch_at(ui,plot_intersection(eye,visible),label,&own_visible)) {
            if(own_visible)a->scene_view.hidden&=~bit;else a->scene_view.hidden|=bit;
        }
    }
    if(children && !(a->scene_view.collapsed&bit))
        for(uint32_t i=0;i<scene->count;i++) if(scene->objects[i].parent_id==o->id) scene_tree_row(a,i,depth+1);
}
static float scene_tree_laboratory_row(app *a,uint32_t slot,unsigned depth);
static float scene_tree_view(app *a) {
    struct nk_context *ui=a->ui;
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(ui->style.window.spacing.x,0));
    memset(a->scene_visibility_bounds,0,sizeof a->scene_visibility_bounds);
    memset(a->scene_expand_bounds,0,sizeof a->scene_expand_bounds);
    memset(a->laboratory_tree_rows,0,sizeof a->laboratory_tree_rows);memset(a->laboratory_tree_titles,0,sizeof a->laboratory_tree_titles);
    const ps_scene *scene=display_scene(a);float height=0;
    for(uint32_t i=0;i<scene->count;i++) if(!scene->objects[i].parent_id) {if(laboratory_visible(a))height+=scene_tree_laboratory_row(a,i,0);else scene_tree_row(a,i,0);}
    nk_style_pop_vec2(ui);return height;
}
static float scene_tree_laboratory_row(app *a,uint32_t slot,unsigned depth) {
    struct nk_context *ui=a->ui;const ps_scene *scene=display_scene(a);const ps_object *object=&scene->objects[slot];
    static const char *types[]={"Kugel","Linie","Box","Vektor","Punkt","Ebene","Pfad","Beschriftung","Gruppe","Koordinatenrahmen"};
    const char *type=object->shape<SDL_arraysize(types)?types[object->shape]:"Objekt";
    bool children=false;for(uint32_t i=0;object->id && i<scene->count;i++)children|=scene->objects[i].parent_id==object->id;
    bool root=!depth && (object->shape==PS_GROUP || object->shape==PS_FRAME);uint32_t bit=UINT32_C(1)<<slot;
    char title[128],label[256],name[320];
    if(*object->text)snprintf(title,sizeof title,"%s",object->text);
    else snprintf(title,sizeof title,"%s · %u",type,object->id?object->id:slot+1);
    if((object->shape==PS_GROUP || object->shape==PS_FRAME) && *object->text)snprintf(label,sizeof label,"%s",object->text);
    else if(object->id)snprintf(label,sizeof label,"ID %u · %s%s%s",object->id,type,*object->text?" · ":"",object->text);
    else snprintf(label,sizeof label,"%u · %s%s%s",slot+1,type,*object->text?" · ":"",object->text);
    float f=a->preferences.ui_size/16.f,available=nk_window_get_content_region(ui).w;
    const struct nk_user_font *font=root?&a->workbench_fonts[2]:&a->laboratory_caption_font;
    float indent=fminf((depth?depth-1:0)*12*f,fmaxf(0,fminf(48*f,available-146*f)));
    bool compact=available-indent-66*f<80*f;float text_width=fmaxf(1,available-indent-(compact?16:66)*f);
    float line=(root?17.4f:15.95f)*f,text_height=line*laboratory_wrapped_lines(font,title,text_width);
    float height=compact?12*f+20*f+4*f+text_height:fmaxf((root?33.4f:33)*f,text_height+(root?16:12)*f);
    nk_layout_row_dynamic(ui,height,1);struct nk_rect row;enum nk_widget_layout_states state=nk_widget(&row,ui);
    struct nk_rect expand=nk_rect(row.x+indent+2*f,row.y,16*f,compact?32*f:row.h);
    struct nk_rect icon=nk_rect(row.x+indent+22*f,row.y+(compact?6*f:0),13*f,compact?20*f:row.h);
    struct nk_rect text=nk_rect(row.x+indent+(compact?8:root?42:44)*f,row.y+(compact?36*f:(row.h-text_height)/2),text_width,text_height);
    struct nk_rect eye=nk_rect(row.x+row.w-22*f,row.y,22*f,compact?32*f:row.h);
    a->laboratory_tree_rows[slot]=row;a->laboratory_tree_titles[slot]=text;a->scene_visibility_bounds[slot]=eye;
    if(children)a->scene_expand_bounds[slot]=expand;
    if(state!=NK_WIDGET_INVALID) {
        struct nk_command_buffer *canvas=nk_window_get_canvas(ui);struct nk_rect visible=plot_intersection(row,ui->current->layout->clip);
        bool effective=ps_scene_view_visible(&a->scene_view,slot),own=!(a->scene_view.hidden&bit),selected=a->scene_selected && a->scene_selected_slot==slot;
        bool enabled=!(ui->current->layout->flags&NK_WINDOW_ROM),hover=enabled && nk_input_is_mouse_hovering_rect(&ui->input,visible);
        if(selected || hover)nk_fill_rect(canvas,row,6*f,selected?UI_COLOR(selection_active):UI_COLOR(button_hover));
        bool expanded_focus=false,selected_focus=false;
        if(children) {
            snprintf(name,sizeof name,"Gruppe auf-/zuklappen: %s",label);
            if(ps_ui_button_at_custom_focus(ui,plot_intersection(expand,visible),name,!(a->scene_view.collapsed&bit),&expanded_focus))a->scene_view.collapsed^=bit;
            float x=expand.x+expand.w/2,y=expand.y+expand.h/2;
            if(a->scene_view.collapsed&bit){nk_stroke_line(canvas,x-2*f,y-3*f,x+f,y,f,muted());nk_stroke_line(canvas,x+f,y,x-2*f,y+3*f,f,muted());}
            else {nk_stroke_line(canvas,x-3*f,y-2*f,x,y+f,f,muted());nk_stroke_line(canvas,x,y+f,x+3*f,y-2*f,f,muted());}
        } else if(depth) {
            float x=expand.x+6*f,y=expand.y+expand.h/2;nk_stroke_line(canvas,x,y-4*f,x,y+2*f,f,muted());nk_stroke_line(canvas,x,y+2*f,x+5*f,y+2*f,f,muted());
        }
        scene_tree_icon(canvas,object,icon,selected?accent():muted());
        laboratory_wrapped_text(canvas,font,title,text,line,effective?UI_COLOR(text):muted());
        struct nk_rect choose=compact?nk_rect(text.x,row.y+32*f,text.w,row.h-32*f):nk_rect(icon.x,row.y,eye.x-icon.x,row.h);
        snprintf(name,sizeof name,"Objekt auswählen: %s",label);
        bool choose_pressed=ps_ui_button_at_custom_focus(ui,plot_intersection(choose,visible),name,selected,&selected_focus);
        bool icon_pressed=compact && enabled && nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT) && nk_input_is_mouse_hovering_rect(&ui->input,plot_intersection(icon,visible));
        if(choose_pressed || icon_pressed)scene_select_entry(a,slot);
        if(selected_focus)nk_stroke_rect(canvas,nk_rect(visible.x+f,visible.y+f,fmaxf(0,visible.w-2*f),fmaxf(0,visible.h-2*f)),5*f,2*f,accent());
        if(expanded_focus)nk_stroke_rect(canvas,nk_rect(expand.x+f,expand.y+f,expand.w-2*f,expand.h-2*f),3*f,2*f,accent());
        bool eye_focus=nk_sdl_accessibility_is_focused(ui,NULL,label);
        if(selected || hover || !effective || eye_focus) {
            float x=eye.x+11*f,y=eye.y+eye.h/2;nk_stroke_circle(canvas,nk_rect(x-5*f,y-4*f,10*f,8*f),f,muted());
            if(effective)nk_fill_circle(canvas,nk_rect(x-f,y-f,2*f,2*f),muted());else nk_stroke_line(canvas,x-5*f,y+4*f,x+5*f,y-4*f,f,muted());
        }
        if(ps_ui_switch_at(ui,plot_intersection(eye,visible),label,&own)){if(own)a->scene_view.hidden&=~bit;else a->scene_view.hidden|=bit;}
        if(!effective && own)ps_ui_accessibility_label(ui,plot_intersection(eye,visible),"Durch eine übergeordnete Gruppe ausgeblendet");
    }
    if(children && !(a->scene_view.collapsed&bit))for(uint32_t i=0;i<scene->count;i++)if(scene->objects[i].parent_id==object->id)height+=scene_tree_laboratory_row(a,i,depth+1);
    return height;
}
