/* Native hierarchy with wrapping labels and separate selection/visibility. */
static void scene_tree_icon(struct nk_command_buffer *canvas,const ps_object *o,struct nk_rect r,struct nk_color color) {
    float x=r.x+7,y=r.y+r.h/2;
    switch(o->shape) {
    case PS_SPHERE:case PS_POINT:nk_fill_circle(canvas,nk_rect(x-3,y-3,6,6),color);break;
    case PS_GROUP:case PS_FRAME:
        nk_stroke_line(canvas,x,y-5,x+5,y,1,color);nk_stroke_line(canvas,x+5,y,x,y+5,1,color);
        nk_stroke_line(canvas,x,y+5,x-5,y,1,color);nk_stroke_line(canvas,x-5,y,x,y-5,1,color);break;
    case PS_LABEL:
        nk_stroke_line(canvas,x-4,y-4,x+4,y-4,1,color);nk_stroke_line(canvas,x,y-4,x,y+4,1,color);break;
    case PS_ARROW:
        nk_stroke_line(canvas,x-4,y+4,x+4,y-4,1,color);nk_stroke_line(canvas,x,y-4,x+4,y-4,1,color);
        nk_stroke_line(canvas,x+4,y-4,x+4,y,1,color);break;
    case PS_POLYLINE:
        nk_stroke_curve(canvas,x-5,y+1,x-2,y-5,x+2,y+5,x+5,y-1,1,color);break;
    default:
        if(o->shape==PS_LINE)nk_stroke_line(canvas,x-4,y+4,x+4,y-4,1,color);
        else {nk_stroke_rect(canvas,nk_rect(x-4,y-4,8,8),0,1,color);nk_stroke_line(canvas,x,y-4,x,y+4,1,color);nk_stroke_line(canvas,x-4,y,x+4,y,1,color);}
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
static void scene_tree_view(app *a) {
    struct nk_context *ui=a->ui;
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(ui->style.window.spacing.x,0));
    memset(a->scene_visibility_bounds,0,sizeof a->scene_visibility_bounds);
    memset(a->scene_expand_bounds,0,sizeof a->scene_expand_bounds);
    const ps_scene *scene=display_scene(a);
    for(uint32_t i=0;i<scene->count;i++) if(!scene->objects[i].parent_id) scene_tree_row(a,i,0);
    nk_style_pop_vec2(ui);
}
