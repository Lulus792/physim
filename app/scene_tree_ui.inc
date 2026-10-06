/* Frame ancestors define coordinates; ordinary groups only organize nodes. */
static void scene_tree_row(app *a,uint32_t slot,unsigned depth) {
    struct nk_context *ui=a->ui;const ps_scene *scene=display_scene(a);
    const ps_object *o=&scene->objects[slot];
    static const char *names[]={"Kugel","Linie","Box","Vektor","Punkt","Ebene","Pfad","Beschriftung","Gruppe","Koordinatenrahmen"};
    bool children=false;
    if(o->id) for(uint32_t i=0;i<scene->count;i++) children |= scene->objects[i].parent_id==o->id;
    uint32_t bit=UINT32_C(1)<<slot;
    char label[112];
    if((o->shape==PS_GROUP || o->shape==PS_FRAME) && o->text[0]) snprintf(label,sizeof label,"%s",o->text);
    else if(o->id) snprintf(label,sizeof label,"ID %u · %s%s%s",o->id,names[o->shape],o->text[0]?" · ":"",o->text);
    else snprintf(label,sizeof label,"%u · %s%s%s",slot+1,names[o->shape],o->text[0]?" · ":"",o->text);
    bool own_visible=!(a->scene_view.hidden&bit);
    bool effective=ps_scene_view_visible(&a->scene_view,slot);
    if(depth || children) {
        float indent=fminf((float)depth*12,72),available=nk_window_get_content_region(ui).w;
        nk_layout_row_begin(ui,NK_STATIC,26,3);
        nk_layout_row_push(ui,indent);nk_spacer(ui);
        nk_layout_row_push(ui,20);
        if(children) {
            a->scene_expand_bounds[slot]=nk_widget_bounds(ui);
            nk_style_push_vec2(ui,&ui->style.button.padding,nk_vec2(1,1));
            nk_style_push_vec2(ui,&ui->style.button.image_padding,nk_vec2(1,1));
            if(nk_button_symbol(ui,a->scene_view.collapsed&bit?NK_SYMBOL_TRIANGLE_RIGHT:NK_SYMBOL_TRIANGLE_DOWN))
                a->scene_view.collapsed ^= bit;
            nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
        } else nk_spacer(ui);
        nk_layout_row_push(ui,fmaxf(16,available-indent-28));
    } else nk_layout_row_dynamic(ui,26,1);
    a->scene_visibility_bounds[slot]=nk_widget_bounds(ui);
    if(!effective) nk_style_push_color(ui,&ui->style.checkbox.text_normal,muted());
    if(nk_checkbox_label(ui,label,&own_visible)) {
        if(own_visible) a->scene_view.hidden &= ~bit;
        else a->scene_view.hidden |= bit;
    }
    if(!effective) nk_style_pop_color(ui);
    if(depth || children) nk_layout_row_end(ui);
    if(children && !(a->scene_view.collapsed&bit))
        for(uint32_t i=0;i<scene->count;i++) if(scene->objects[i].parent_id==o->id) scene_tree_row(a,i,depth+1);
}
static void scene_tree_view(app *a) {
    memset(a->scene_visibility_bounds,0,sizeof a->scene_visibility_bounds);
    memset(a->scene_expand_bounds,0,sizeof a->scene_expand_bounds);
    const ps_scene *scene=display_scene(a);
    for(uint32_t i=0;i<scene->count;i++) if(!scene->objects[i].parent_id) scene_tree_row(a,i,0);
}
