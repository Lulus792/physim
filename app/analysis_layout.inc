/* The analysis workspace owns its side visibility and widths for this session.
 * Reuse the existing docking format, while preserving the other workspaces. */
static bool analysis_visible(const app *a) {
    return a->tab==2 && a->analysis_tab==0 && !a->project_manager;
}
static float analysis_side_minimum(const app *a,unsigned panel) {
    return ceilf((panel==PS_DOCK_SIDEBAR?140.f:180.f)*a->preferences.ui_size/16.f);
}
static void analysis_context(app *a) {
    bool active=analysis_visible(a);
    if(active==a->analysis_layout_active)return;
    if(active) {
        a->analysis_previous=laboratory_capture(a);
        if(!a->analysis_layout_initialized) {
            a->analysis_layout=a->analysis_previous;
            a->analysis_layout.dock=ps_dock_fixed_layout(1u<<PS_DOCK_LOG);
            a->analysis_layout.sidebar_width=232;
            a->analysis_layout.inspector_width=242;
            a->analysis_layout.show_log=false;
            a->analysis_layout_initialized=true;
        }
        laboratory_apply(a,&a->analysis_layout);
        if(a->loaded)refresh_library(a);
    } else {
        a->analysis_tab_pointer=0;
        if(a->analysis_plot_resizing)analysis_plot_resize_abort(a);
        a->analysis_plot_hover=false;
        a->analysis_layout=laboratory_capture(a);
        laboratory_apply(a,&a->analysis_previous);
    }
    a->analysis_layout_active=active;
}
static void workspace_context(app *a) {
    /* A context or modal transition ends the pointer transaction before its
     * intermediate dimensions can be captured as that workspace's wish. */
    if(a->dock_resize_transaction && (a->tab!=a->dock_last_tab || analysis_visible(a)!=a->analysis_layout_active || laboratory_visible(a)!=a->laboratory_active || a->project_manager ||
       a->recovery || a->layout_manager || a->channel_unit_manager || a->workspace_manager ||
       a->run_details_manager || a->dialog_pending))dock_resize_abort(a);
    /* Leave a context before entering its neighbour: neither may capture the
     * other's temporary layout as the ordinary workbench state. */
    if(!analysis_visible(a))analysis_context(a);
    laboratory_context(a);
    if(analysis_visible(a))analysis_context(a);
}
