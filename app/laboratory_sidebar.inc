/* Messlabor sidebar: actual tree, quiet display filters and shared advanced state. */
static const char *laboratory_tree_note[2]={"Auswahl und Sichtbarkeit","gelten nur für die Ansicht."};
static void laboratory_tree_checkbox(app *a,const char *label,bool *value,unsigned slot) {
    laboratory_checkbox(a,label,value,7,&a->laboratory_tree_filters[slot]);struct nk_rect r=a->laboratory_tree_filters[slot];
    if(slot==0)a->display_path_bounds=r;else if(slot==1)a->display_vector_bounds=r;
}
static void laboratory_scene_sidebar(app *a) {
    struct nk_context *ui=a->ui;float f=a->preferences.ui_size/16.f;scene_selection_sync(a);
    struct nk_panel *layout=ui->current->layout;struct nk_rect saved_bounds=layout->bounds,saved_clip=layout->clip;
    /* Match the reference's eight-pixel side padding while the native scrollbar
     * overlays the outer padding, using its existing wheel/capture behavior. */
    layout->bounds.w+=ui->style.window.scrollbar_size.x;layout->clip.w+=ui->style.window.scrollbar_size.x;nk_push_scissor(nk_window_get_canvas(ui),layout->clip);
    a->laboratory_tree_body=layout->clip;struct nk_rect content=nk_window_get_content_region(ui);float tree_height=scene_tree_view(a);
    if(!display_scene(a)->count) {
        const char *empty="Noch keine Szeneneinträge.";float h=15.95f*f*laboratory_wrapped_lines(&a->laboratory_caption_font,empty,content.w);
        nk_layout_row_dynamic(ui,h+16*f,1);struct nk_rect row;if(nk_widget(&row,ui)!=NK_WIDGET_INVALID){laboratory_wrapped_text(nk_window_get_canvas(ui),&a->laboratory_caption_font,empty,nk_rect(row.x,row.y+8*f,row.w,h),15.95f*f,muted());ps_ui_accessibility_label(ui,row,empty);}tree_height=h+16*f;
    }
    bool advanced=a->view_disclosure==NK_MAXIMIZED || a->camera_disclosure==NK_MAXIMIZED;
    float footer_width=fmaxf(1,content.w-14),note_height=0;
    for(unsigned i=0;i<2;i++)note_height+=16*f*laboratory_wrapped_lines(&a->workspace_status_font,laboratory_tree_note[i],footer_width);
    float footer_height=148.9f*f+note_height;
    if(a->scene_view.hidden)footer_height+=34.5f*f;
    float remaining=content.h-tree_height-footer_height;
    if(!advanced && remaining>0){ps_ui_row_exact_dynamic(ui,remaining,1);nk_spacer(ui);}
    ps_ui_row_exact_dynamic(ui,22*f,1);nk_spacer(ui);
    nk_layout_row_dynamic(ui,25.05f*f,1);struct nk_rect heading;enum nk_widget_layout_states visible=nk_widget(&heading,ui);heading.x+=7;heading.w=fmaxf(1,heading.w-14);a->laboratory_tree_options=heading;
    a->laboratory_tree_footer=nk_rect(content.x,heading.y-22*f,content.w,footer_height);
    if(visible!=NK_WIDGET_INVALID) {
        struct nk_command_buffer *canvas=nk_window_get_canvas(ui);bool focused=false;
        if(ps_ui_button_at_custom_focus(ui,plot_intersection(heading,layout->clip),"Darstellung und weitere Kameraoptionen",advanced,&focused)) {
            a->view_disclosure=a->camera_disclosure=advanced?NK_MINIMIZED:NK_MAXIMIZED;advanced=!advanced;
        }
        const char *text="DARSTELLUNG";const struct nk_user_font *font=&a->workbench_fonts[1];float x=heading.x;
        bool hover=!(layout->flags&NK_WINDOW_ROM) && nk_input_is_mouse_hovering_rect(&ui->input,plot_intersection(heading,layout->clip));
        for(const char *at=text;*at;at++){float w=font->width(font->userdata,font->height,at,1);nk_draw_text(canvas,nk_rect(x,heading.y+(13.05f*f-font->height)/2,w+1,font->height),at,1,font,nk_rgba(0,0,0,0),hover || focused?accent():UI_COLOR(muted));x+=w+1.2f*f;}
        if(focused)nk_stroke_rect(canvas,nk_rect(heading.x+f,heading.y+f,heading.w-2*f,heading.h-2*f),3*f,2*f,accent());
        ps_ui_accessibility_label(ui,heading,advanced?"Weitere Darstellungs- und Kameraoptionen: ausgeklappt":"Weitere Darstellungs- und Kameraoptionen: eingeklappt");
    }
    laboratory_tree_checkbox(a,"Bahnkurve",&a->show_paths,0);laboratory_tree_checkbox(a,"Vektoren",&a->show_vectors,1);laboratory_tree_checkbox(a,"Raster",&a->show_grid,2);
    if(a->scene_view.hidden || advanced) {
        struct nk_rect before=layout->bounds;float before_x=layout->at_x;layout->at_x+=7;layout->bounds.x+=7;layout->bounds.w=fmaxf(1,layout->bounds.w-14);
        if(laboratory_object_action(a,"Alle Objekte einblenden",true,&a->display_all_bounds))a->scene_view.hidden=0;
        layout->bounds=before;layout->at_x=before_x;
    }
    if(advanced) {
        struct nk_rect before=layout->bounds;float before_x=layout->at_x;layout->at_x+=7;layout->bounds.x+=7;layout->bounds.w=fmaxf(1,layout->bounds.w-14);
        scene_camera_controls_view(a,true);a->laboratory_tree_camera_mode=a->camera_mode_bounds;layout->bounds=before;layout->at_x=before_x;
    }
    ps_ui_row_exact_dynamic(ui,11*f,1);nk_spacer(ui);
    for(unsigned i=0;i<2;i++) {
        float h=16*f*laboratory_wrapped_lines(&a->workspace_status_font,laboratory_tree_note[i],footer_width);
        nk_layout_row_dynamic(ui,h,1);struct nk_rect r;if(nk_widget(&r,ui)!=NK_WIDGET_INVALID){r.x+=7;r.w=fmaxf(1,r.w-14);laboratory_wrapped_text(nk_window_get_canvas(ui),&a->workspace_status_font,laboratory_tree_note[i],r,16*f,muted());ps_ui_accessibility_label(ui,r,laboratory_tree_note[i]);}
    }
    ps_ui_row_exact_dynamic(ui,16*f,1);nk_spacer(ui);
    if(a->laboratory_tree_reveal) {
        if(!a->scene_selected)a->laboratory_tree_reveal=false;
        else {
            struct nk_rect row=a->laboratory_tree_rows[a->scene_selected_slot];
            if(row.w>0) {
                float delta=row.y<layout->clip.y?row.y-layout->clip.y:row.y+row.h>layout->clip.y+layout->clip.h?row.y+row.h-layout->clip.y-layout->clip.h:0;
                if(row.h>layout->clip.h)delta=row.y-layout->clip.y;
                float next=(float)*layout->offset_y+delta;*layout->offset_y=(nk_uint)fmaxf(0,delta>0?ceilf(next):floorf(next));a->laboratory_tree_reveal=false;
            }
        }
    }
    layout->bounds=saved_bounds;layout->clip=saved_clip;layout->max_x=fminf(layout->max_x,saved_bounds.x+saved_bounds.w);nk_push_scissor(nk_window_get_canvas(ui),saved_clip);
    nk_window_set_scroll(ui,0,(nk_uint)*layout->offset_y);
}
