/* workbench.css: a 1px rule, 6px padding, two 28px controls and a 3px
 * gap. The hidden context reserves 40px instead of the usual 6px. Use
 * this same header in the main editor and its independent native window. */
static bool workbench_editor_action(app *a,struct nk_rect bounds,ps_ui_icon icon,const char *name,bool enabled) {
    struct nk_context *ui=a->ui;struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    bool interactive=enabled && !(ui->current->layout->flags&NK_WINDOW_ROM) && !ui->current->widgets_disabled;
    struct nk_rect visible=file_tab_clip(bounds,ui->current->layout->clip);
    bool hover=interactive && visible.w>0 && visible.h>0 && nk_input_is_mouse_hovering_rect(&ui->input,visible);
    float scale=a->preferences.ui_size/16.f;
    if(hover)nk_fill_rect(canvas,bounds,6*scale,UI_COLOR(button_hover));
    struct nk_color color=hover?UI_COLOR(text):UI_COLOR(muted);
    if(!enabled)color.a=(nk_byte)(color.a/2);
    workbench_icon(canvas,icon,bounds,color);
    bool focused=false;nk_begin_disabled(ui,!enabled);bool clicked=ps_ui_button_at_custom_focus(ui,bounds,name,false,&focused);nk_end_disabled(ui);
    if(focused || nk_sdl_accessibility_is_focused(ui,NULL,name))nk_stroke_rect(canvas,bounds,6*scale,2,UI_COLOR(accent));
    return clicked;
}
static struct nk_rect workbench_editor_tools(app *a,struct nk_rect header,bool context_hidden,bool reopen) {
    struct nk_context *ui=a->ui;struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    float scale=a->preferences.ui_size/16.f;
    struct nk_rect tabs=header;
    if(reopen) {
        float width=34*scale+1;
        a->workbench_sidebar_bounds=nk_rect(header.x+5*scale,header.y+(header.h-28*scale)/2,24*scale,28*scale);
        if(workbench_side_button(a,a->workbench_sidebar_bounds,false,false,"Workspace ein-/ausblenden")) {
            ps_dock_reveal_outer(&a->preferences.dock,PS_DOCK_SIDEBAR,PS_DOCK_LEFT);a->workbench_sidebar_focus=3;
        }
        workspace_rule(canvas,nk_rect(floorf(header.x+width-1),header.y,1,header.h));
        tabs.x+=width;tabs.w-=width;
    }
    float width=1+(context_hidden?105:71)*scale;
    struct nk_rect tools=nk_rect(header.x+header.w-width,header.y,width,header.h);
    a->workbench_tools_bounds=tools;
    workspace_rule(canvas,nk_rect(floorf(tools.x),tools.y,1,tools.h));
    a->workbench_search_bounds=nk_rect(tools.x+1+6*scale,header.y+(header.h-28*scale)/2,28*scale,28*scale);
    a->workbench_save_bounds=a->workbench_search_bounds;a->workbench_save_bounds.x+=31*scale;
    if(workbench_editor_action(a,a->workbench_search_bounds,PS_ICON_SEARCH,"Dokumentsuche öffnen",true))editor_search_open(a);
    if(workbench_editor_action(a,a->workbench_save_bounds,PS_ICON_SAVE,"Aktive Datei speichern",idle(a)))save_active_document(a);
    if(a->search_icon_focus && nk_sdl_accessibility_request_focus(ui,NULL,"Dokumentsuche öffnen"))a->search_icon_focus=false;
    tabs.w=fmaxf(1,tools.x-tabs.x);return tabs;
}
