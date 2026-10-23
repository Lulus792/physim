#include "workspace_path_tooltip.inc"
static bool workbench_path_equal(const char *left,const char *right) {
    while(*left && *right) {
        unsigned char a=(unsigned char)*left++,b=(unsigned char)*right++;
#ifdef _WIN32
        if(a=='\\')a='/';if(b=='\\')b='/';
#endif
        if(a!=b)return false;
    }
    return !*left && !*right;
}
static unsigned workbench_tree_rank(const app *a,const ps_workspace_entry *entry) {
    const char *name=workspace_name(entry->path);
    if(!entry->directory) {
        if(!strcmp(name,experiment_source(a)))return 0;
        if(!strcmp(name,analysis_source(a)))return 1;
        if(!strcmp(name,"physim.project"))return 2;
    } else {
        if(!strcmp(name,"assets"))return 3;
        if(!strcmp(name,"runs"))return 4;
        if(!strcmp(name,"build"))return 5;
    }
    return 6;
}
static void workbench_tree_order(app *a) {
    const ps_workspace_tree *tree=&a->workspace_tree;size_t written=0;
    for(size_t root=0;root<tree->count;) {
        size_t end=root+1;while(end<tree->count && tree->entries[end].depth>tree->entries[root].depth)end++;
        a->workbench_tree_order[written++]=(uint16_t)root;
        bool project=a->loaded && workbench_path_equal(tree->entries[root].path,a->project);
        for(unsigned rank=0;rank<7;rank++)for(size_t i=root+1;i<end;) {
            size_t next=i+1;while(next<end && tree->entries[next].depth>tree->entries[i].depth)next++;
            if((project?workbench_tree_rank(a,&tree->entries[i]):6)==rank)
                for(size_t j=i;j<next;j++)a->workbench_tree_order[written++]=(uint16_t)j;
            i=next;
        }
        root=end;
    }
}
static void workspace_tree_view(app *a) {
    struct nk_context *ui = a->ui;
    bool compact=workbench_visible(a);
    if(compact)workbench_tree_order(a);
    char active[4096]={0};
    if(compact) {
        workspace_document *doc=active_document(a);
        if(a->tab==8 && doc)snprintf(active,sizeof active,"%s",doc->path);
        else if(a->loaded && a->tab==0)join(active,sizeof active,a->project,a->analysis_only?analysis_source(a):experiment_source(a));
    }
    if(!workbench_visible(a)) {
    nk_layout_row_dynamic(ui, 28, 1);
    if (nk_button_label(ui, "Dateibaum aktualisieren"))
        refresh_workspace_entries(a);
    }
    if (a->workspace_tree_error[0]) {
        nk_layout_row_dynamic(ui, 64, 1);
        nk_label_wrap(ui, a->workspace_tree_error);
    }
    a->workspace_tree_test_bounds = nk_rect(0, 0, 0, 0);
    const char *tip_path=NULL;
    size_t clicked_index = SIZE_MAX;
    for (size_t display=0;display<a->workspace_tree.count;display++) {
        size_t i=compact?a->workbench_tree_order[display]:display;
        const ps_workspace_entry *entry = &a->workspace_tree.entries[i];
        if (!entry->depth && entry->root) {
            nk_layout_row_dynamic(ui, 22, 1);
            nk_label_colored(ui, "Hinzugefügt", NK_TEXT_LEFT, muted());
        }
        float width=nk_window_get_content_region(ui).w;
        float indent = fminf((float)entry->depth * (compact?12:16), fmaxf(0,width-64));
        nk_layout_row_begin(ui, NK_STATIC, compact?31:27, entry->depth ? 2 : 1);
        if (entry->depth) {
            nk_layout_row_push(ui, indent);
            nk_spacer(ui);
        }
        nk_layout_row_push(ui, fmaxf(24,width-indent-(entry->depth?ui->style.window.spacing.x:0)));
        struct nk_rect bounds = nk_widget_bounds(ui);
        if (!strcmp(entry->path, a->workspace_tree_test_path))
            a->workspace_tree_test_bounds = bounds;
        char label[4200];
        snprintf(label, sizeof label, entry->directory?"        %s":"      %s",
                 workspace_name(entry->path));
        nk_style_push_flags(ui, &ui->style.button.text_alignment, NK_TEXT_LEFT);
        bool selected = !strcmp(entry->path, a->workspace_preview_path);
        bool clicked;
        if(workbench_visible(a)) {
            selected=workbench_path_equal(entry->path,active);
            nk_widget(&bounds,ui);struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
            struct nk_rect row=nk_rect(bounds.x-indent,bounds.y,nk_window_get_bounds(ui).w-16,bounds.h);
            if(selected || (!entry->directory && nk_input_is_mouse_hovering_rect(&ui->input,row)))nk_fill_rect(canvas,row,selected?0:4,selected?UI_COLOR(selection_active):UI_COLOR(button_hover));
            float inset=entry->directory?(entry->depth?30:18):25;
            const char *name=workspace_name(entry->path);
            const struct nk_user_font *font=entry->depth?ui->style.font:&a->workbench_fonts[3];
            nk_draw_text(canvas,nk_rect(bounds.x+inset,bounds.y+(bounds.h-font->height)/2,fmaxf(1,bounds.w-inset-3),font->height),name,(int)strlen(name),font,nk_rgba(0,0,0,0),selected || !entry->depth?UI_COLOR(text):UI_COLOR(muted));
            clicked=ps_ui_button_at(ui,row,name,selected);
        } else clicked=action_button(ui,label,selected);
        nk_style_pop_flags(ui);
        nk_layout_row_end(ui);
        struct nk_command_buffer *guides=nk_window_get_canvas(ui);
        for(unsigned depth=0;depth<entry->depth && (!compact || !selected);depth++) {
            float gx=bounds.x-indent+(float)depth*(compact?12:16)+8;
            nk_stroke_line(guides,gx,bounds.y,gx,bounds.y+bounds.h,1,UI_COLOR(divider));
        }
        if(compact && !entry->directory && !entry->missing)workbench_file_icon(a,guides,entry->path,nk_rect(bounds.x+7,bounds.y,14,bounds.h),UI_COLOR(accent));
        else if(compact && entry->directory && entry->depth && !entry->missing)workbench_icon(guides,PS_ICON_FOLDER,nk_rect(bounds.x+9,bounds.y,14,bounds.h),UI_COLOR(muted));
        else if(!compact || entry->depth || entry->missing)ps_ui_draw_icon(nk_window_get_canvas(ui),entry->missing?PS_ICON_ERROR:entry->directory?PS_ICON_FOLDER:PS_ICON_FILE,
                        nk_rect(bounds.x+(entry->directory?14:4),bounds.y,compact?14:20,bounds.h),entry->missing?UI_COLOR(error):UI_COLOR(muted));
        if (entry->directory) {
            struct nk_command_buffer *canvas = nk_window_get_canvas(ui);
            float x = bounds.x + 8, y = bounds.y + bounds.h * .5f;
            struct nk_color color=UI_COLOR(muted);
            if(compact && entry->depth) {
                x=bounds.x-4;
                /* CSS puts the disclosure on the path guide, with its own
                 * window-colored backing so the guide cannot cross it. */
                nk_fill_rect(canvas,nk_rect(x-5.5f,y-8,11,16),0,UI_COLOR(window));
            }
            if (entry->expanded) {
                nk_stroke_line(canvas,x-2,y-2,x+1,y+1,1,color);
                nk_stroke_line(canvas,x+1,y+1,x+4,y-2,1,color);
            } else {
                nk_stroke_line(canvas,x-1,y-3,x+2,y,1,color);
                nk_stroke_line(canvas,x+2,y,x-1,y+3,1,color);
            }
        }
        struct nk_rect tip_visible=plot_intersection(bounds,ui->current->layout->clip);
        if(tip_visible.w>0 && tip_visible.h>0 && nk_input_is_mouse_hovering_rect(&ui->input,tip_visible))tip_path=entry->path;
        if (clicked)
            clicked_index = i;
        if(compact && !entry->depth && entry->expanded){nk_layout_row_dynamic(ui,6,1);nk_spacer(ui);}
        if (entry->error != PS_OK) {
            nk_layout_row_dynamic(ui, 44, 1);
            nk_label_wrap(
                ui,
                entry->error == PS_LIMIT
                    ? "Anzeigegrenze erreicht. Andere Ordner zuklappen und erneut öffnen."
                : entry->missing
                    ? "Pfad nicht erreichbar."
                    : "Ordnerinhalt nicht vollständig lesbar. Erneut öffnen oder aktualisieren.");
        } else if (entry->expanded && (i + 1 == a->workspace_tree.count ||
                                       a->workspace_tree.entries[i + 1].depth <= entry->depth)) {
            nk_layout_row_dynamic(ui, 22, 1);
            nk_label_colored(ui, "Ordner ist leer", NK_TEXT_LEFT, muted());
        }
    }
    workspace_tip_draw(a,tip_path);
    /* Finish this frame's layout before a mutation replaces paths and indices. */
    if (clicked_index != SIZE_MAX) {
        const ps_workspace_entry *entry = &a->workspace_tree.entries[clicked_index];
        if (entry->directory) {
            ps_result r = ps_workspace_tree_toggle(&a->workspace_tree, clicked_index);
            if (r != PS_OK)
                status(a, "Ordner nicht vollständig geladen. Hinweis im Dateibaum beachten.");
        } else
            preview_workspace_path(a, entry->path);
    }
}
