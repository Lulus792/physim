/* Native analysis workspace. Every action uses the existing project, run and
 * report paths; the HTML reference supplies geometry, never measurement data. */
static void analysis_comparison_sync(app *a) {
    if(a->comparison_pending_active[0] && idle(a)) {
        if(a->selected_count) {
            const char *next=selected_run(a,a->comparison_pending_active)>=0?a->comparison_pending_active:a->selected_runs[0];
            snprintf(a->last_run,sizeof a->last_run,"%s",next);timeline_clear(a);a->scene.count=0;a->channel_count=0;a->history_count=0;
            request_dataset(a);
        }
        a->comparison_pending_active[0]=0;
    }
    if(!a->comparison && a->loaded)a->comparison=ps_comparison_create();
    if(!a->comparison)return;
    ps_comparison_key key={.revision=a->comparison_revision}; /* existing core scratch quota */
    snprintf(key.scope,sizeof key.scope,"%s",a->project);
    if(a->loaded && !a->data.thread && a->data.channel_count && a->data.count && a->selected_count) {
        if(a->plot_channel<0 || a->plot_channel>=(int)a->data.channel_count)a->plot_channel=0;
        key.count=a->selected_count;key.reference=0;key.channel=a->data.channels[a->plot_channel];
        ps_display_unit unit=channel_display_unit(a,&key.channel);
        key.unit.scale=unit.scale;memcpy(key.unit.dimension,unit.dimension,7);snprintf(key.unit.symbol,sizeof key.unit.symbol,"%s",unit.symbol);
        /* Use the existing writable per-user/fixture state directory, even when
         * the opened project is read-only or has never been built. */
        const char *path=a->preferences_path,*slash=strrchr(path,'/'),*back=strrchr(path,'\\');
        if(back && (!slash || back>slash))slash=back;
        size_t parent=slash?(size_t)(slash-path)+1:0;
        if(parent+sizeof "physim-comparison"<=sizeof key.scratch_prefix) {
            memcpy(key.scratch_prefix,path,parent);memcpy(key.scratch_prefix+parent,"physim-comparison",sizeof "physim-comparison");
        }
        for(uint32_t i=0;i<key.count;i++)snprintf(key.paths[i],sizeof key.paths[i],"%s",a->selected_runs[i]);
    }
    (void)ps_comparison_request(a->comparison,&key);
    ps_comparison_poll(a->comparison);
}
static void analysis_heading(app *a,const char *text,float height,bool strong) {
    struct nk_context *ui=a->ui;
    nk_layout_row_dynamic(ui,height,1);
    nk_style_push_font(ui,strong?&a->laboratory_title_font:&a->workbench_fonts[0]);
    nk_label_colored(ui,text,NK_TEXT_LEFT,strong?UI_COLOR(text):muted());
    nk_style_pop_font(ui);
}
static void analysis_rule(app *a) {
    nk_layout_row_dynamic(a->ui,1,1);
    struct nk_rect r;nk_widget(&r,a->ui);
    nk_fill_rect(nk_window_get_canvas(a->ui),r,0,UI_COLOR(border));
}
static void analysis_field_background(app *a,struct nk_rect r,bool focused) {
    /* A filled outer contour keeps the straight edges one solid pixel wide;
     * a stroked Nuklear rectangle spreads that edge across adjacent pixels. */
    float inset=focused?2.f:1.f;
    nk_fill_rect(nk_window_get_canvas(a->ui),r,6,focused?UI_COLOR(accent):UI_COLOR(border));
    nk_fill_rect(nk_window_get_canvas(a->ui),nk_rect(r.x+inset,r.y+inset,r.w-2*inset,r.h-2*inset),6-inset,UI_COLOR(edit));
}
static void analysis_combo_style(app *a,const char *name) {
    struct nk_style_combo *combo=&a->ui->style.combo;
    combo->normal=combo->hover=combo->active=nk_style_item_color(nk_rgba(0,0,0,0));
    combo->border=1;combo->border_color=nk_rgba(0,0,0,0);combo->rounding=6;
    combo->button.normal=combo->button.hover=combo->button.active=nk_style_item_color(nk_rgba(0,0,0,0));
    combo->button.border=0;combo->button.padding=nk_vec2(10,10);
    combo->sym_normal=combo->sym_hover=combo->sym_active=NK_SYMBOL_CHEVRON_DOWN;
    analysis_field_background(a,nk_widget_bounds(a->ui),nk_sdl_accessibility_is_focused(a->ui,NULL,name));
    nk_style_push_font(a->ui,&a->workbench_fonts[2]);
}
static bool analysis_unit_preset_matches(const ps_channel *channel,const channel_unit_preset *preset) {
    if(memcmp(channel->dimension,preset->dimension,7))return false;
    const int8_t dimensionless[7]={0};
    if(!memcmp(channel->dimension,dimensionless,7)) {
        bool angle=!strcmp(channel->unit,"rad") || !strcmp(channel->unit,"°") || !strcmp(channel->unit,"deg");
        bool angle_preset=!strcmp(preset->symbol,"rad") || !strcmp(preset->symbol,"°");
        return angle==angle_preset;
    }
    return true;
}
static const char *analysis_unit_label(const char *symbol) {
    return !strcmp(symbol,"rad")?"Radiant (rad)":!strcmp(symbol,"°")?"Grad (°)":symbol;
}
static void analysis_unit_select(app *a,const ps_channel *channel,const ps_display_unit *unit) {
    if(unit->scale==1 && !strcmp(unit->symbol,channel->unit)) {
        ps_channel_units next=a->channel_units;
        if(a->channel_units_writable && ps_channel_units_remove(&next,channel)==PS_OK)(void)channel_units_save(a,&next);
    } else (void)channel_unit_commit(a,channel,unit);
}
static void analysis_unit_menu(app *a,const ps_channel *channel) {
    struct nk_context *ui=a->ui;ps_display_unit unit=channel_display_unit(a,channel);
    const char *label=analysis_unit_label(unit.symbol);struct nk_style_combo combo=ui->style.combo;
    analysis_combo_style(a,"Anzeigeeinheit auswählen");a->analysis_unit_custom=nk_rect(0,0,0,0);
    a->channel_unit_buttons[1]=nk_widget_bounds(ui);
    float menu_width=fmaxf(a->channel_unit_buttons[1].w,ui->style.font->width(ui->style.font->userdata,ui->style.font->height,"Benutzerdefiniert …",(int)strlen("Benutzerdefiniert …"))+32);
    bool menu_open=ps_ui_combo_begin_named(ui,"Anzeigeeinheit auswählen",label,nk_vec2(fmaxf(220,menu_width),260));
    if(menu_open) {
        struct nk_style_button button=ui->style.button;
        ui->style.button.normal=nk_style_item_color(nk_rgba(0,0,0,0));ui->style.button.text_alignment=NK_TEXT_LEFT;
        bool original_offered=false;nk_layout_row_dynamic(ui,30,1);
        nk_begin_disabled(ui,!a->channel_units_writable);
        for(unsigned i=0;i<sizeof channel_unit_presets/sizeof channel_unit_presets[0];i++) {
            const channel_unit_preset *preset=channel_unit_presets+i;
            if(!analysis_unit_preset_matches(channel,preset))continue;
            ps_display_unit choice={.scale=preset->scale};memcpy(choice.dimension,preset->dimension,7);
            snprintf(choice.symbol,sizeof choice.symbol,"%s",preset->symbol);
            original_offered|=choice.scale==1 && !strcmp(choice.symbol,channel->unit);
            if(nk_button_label(ui,analysis_unit_label(choice.symbol))){analysis_unit_select(a,channel,&choice);a->analysis_unit_focus_return=true;nk_combo_close(ui);}
        }
        if(!original_offered) {
            char original[100];snprintf(original,sizeof original,"Gespeicherte Einheit (%s)",channel->unit[0]?channel->unit:"SI");
            if(nk_button_label(ui,original)) {
                ps_channel_units next=a->channel_units;
                if(ps_channel_units_remove(&next,channel)==PS_OK)(void)channel_units_save(a,&next);
                a->analysis_unit_focus_return=true;
                nk_combo_close(ui);
            }
        }
        nk_end_disabled(ui);a->analysis_unit_custom=nk_widget_bounds(ui);
        if(nk_button_label(ui,"Benutzerdefiniert …")){channel_unit_open(a,channel);a->analysis_unit_focus_return=true;nk_combo_close(ui);}
        ui->style.button=button;
        nk_combo_end(ui);
    }
    nk_style_pop_font(ui);ui->style.combo=combo;
    if(!menu_open && a->analysis_unit_focus_return && nk_sdl_accessibility_request_focus(ui,NULL,"Anzeigeeinheit auswählen"))a->analysis_unit_focus_return=false;
}
static void analysis_text(app *a,struct nk_rect r,const char *text,const struct nk_user_font *font,struct nk_color color) {
    size_t length=strlen(text);char shown[1200];bool truncated=font->width(font->userdata,font->height,text,(int)length)>r.w;
    if(truncated) {
        float dots=font->width(font->userdata,font->height,"…",3);
        while(length && font->width(font->userdata,font->height,text,(int)length)+dots>r.w) {
            length--;while(length && ((unsigned char)text[length]&0xc0)==0x80)length--;
        }
    }
    snprintf(shown,sizeof shown,"%.*s%s",(int)length,text,truncated?"…":"");
    nk_draw_text(nk_window_get_canvas(a->ui),r,shown,(int)strlen(shown),font,nk_rgba(0,0,0,0),color);
    ps_ui_accessibility_label(a->ui,r,text);
    if(truncated && !(a->ui->current->layout->flags&NK_WINDOW_ROM) && nk_input_is_mouse_hovering_rect(&a->ui->input,plot_intersection(r,nk_window_get_canvas(a->ui)->clip)))nk_tooltip(a->ui,text);
}
static void analysis_wrapped_text(app *a,struct nk_rect r,const char *text,const struct nk_user_font *font,struct nk_color color) {
    size_t at=0,n=strlen(text);float y=r.y;
    while(at<n) {
        size_t length,used=documentation_line(font,text+at,n-at,r.w,&length);
        nk_draw_text(nk_window_get_canvas(a->ui),nk_rect(r.x,y,r.w,font->height),text+at,(int)length,font,nk_rgba(0,0,0,0),color);
        at+=used;y+=font->height+6;
    }
    ps_ui_accessibility_label(a->ui,r,text);
}
static void analysis_run_caption(const app *a,const ps_library_item *item,const char *path,char *meta,size_t size) {
    if(!a->data.thread && !strcmp(a->data.path,path) && a->data.count)
        snprintf(meta,size,"%.6g s · %llu Messpunkte",a->data.time[a->data.count-1]-a->data.time[0],(unsigned long long)a->data.total);
    else snprintf(meta,size,"%llu Byte · Messdatei",(unsigned long long)item->bytes);
}
static float analysis_run_card_height(const app *a,const char *meta,float width,float *meta_y,float *state_y) {
    const struct nk_user_font *font=&a->laboratory_title_font,*small=&a->workbench_fonts[2];
    *meta_y=fmaxf(40,12+font->height+4);
    *state_y=fmaxf(62,*meta_y+documentation_height(small,meta,strlen(meta),fmaxf(1,width-44)));
    return fmaxf(90,*state_y+small->height+8);
}
static void analysis_side_toggle(app *a,unsigned side) {
    unsigned panel=side?PS_DOCK_INSPECTOR:PS_DOCK_SIDEBAR;
    if(a->preferences.dock.hidden&(1u<<panel))
        ps_dock_reveal_outer(&a->preferences.dock,panel,side?PS_DOCK_RIGHT:PS_DOCK_LEFT);
    else ps_dock_hide(&a->preferences.dock,panel);
    a->analysis_focus_return=side+1;
}
static void analysis_chrome(app *a,float w,float h) {
    struct nk_context *ui=a->ui;bool visible=analysis_visible(a);
    nk_window_show(ui,"analysis-heading",visible?NK_SHOWN:NK_HIDDEN);
    nk_window_show(ui,"analysis-status",visible?NK_SHOWN:NK_HIDDEN);
    if(!visible)return;
    nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(18,22));
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(12,0));
    nk_style_push_float(ui,&ui->style.window.rounding,0);
    if(laboratory_begin(a,"analysis-heading",nk_rect(0,118,w,80),NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BACKGROUND)) {
        float scale=a->preferences.ui_size/16.f;
        float code_width=104*scale,action_width=(a->selected_count>1?145:120)*scale;
        nk_layout_row_begin(ui,NK_STATIC,32,5);
        nk_layout_row_push(ui,32);struct nk_rect left;nk_widget(&left,ui);
        a->analysis_side_buttons[0]=left;
        if(workbench_side_button(a,a->analysis_side_buttons[0],false,!(a->preferences.dock.hidden&1),"Messläufe ein-/ausblenden"))analysis_side_toggle(a,0);
        nk_layout_row_push(ui,fmaxf(130,w-36-48-64-code_width-action_width));
        struct nk_rect title;nk_widget(&title,ui);struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
        const struct nk_user_font *heading=&a->analysis_title_font,*small=&a->workbench_fonts[0];
        nk_draw_text(canvas,nk_rect(title.x,title.y-3,title.w,heading->height),"Auswertung",10,heading,nk_rgba(0,0,0,0),UI_COLOR(text));
        char description[256];snprintf(description,sizeof description,"%s / %u %s",a->loaded?project_name(a):"Kein Projekt",a->selected_count,a->selected_count==1?"Lauf ausgewählt":"Läufe ausgewählt");
        nk_draw_text(canvas,nk_rect(title.x,title.y-3+heading->height+4,title.w,small->height),description,(int)strlen(description),small,nk_rgba(0,0,0,0),muted());
        ps_ui_accessibility_label(ui,title,description);
        nk_layout_row_push(ui,code_width);
        nk_begin_disabled(ui,!a->loaded);
        if(nk_button_label(ui,"Analysecode"))file_tab_select(a,1);
        nk_end_disabled(ui);
        nk_layout_row_push(ui,action_width);
        char label[64];snprintf(label,sizeof label,a->selected_count>1?"%u Läufe vergleichen":"Analyse starten",a->selected_count);
        a->analysis_start_bounds=nk_widget_bounds(ui);
        nk_begin_disabled(ui,!a->loaded || !idle(a) || (!a->selected_count && !a->last_run[0] && !a->analysis_only));
        if(action_button(ui,label,true)){a->show_report=true;analyze(a,false);}
        nk_end_disabled(ui);
        nk_layout_row_push(ui,32);struct nk_rect right;nk_widget(&right,ui);
        a->analysis_side_buttons[1]=right;
        if(workbench_side_button(a,a->analysis_side_buttons[1],true,!(a->preferences.dock.hidden&8),"Laufdetails ein-/ausblenden"))analysis_side_toggle(a,1);
        nk_layout_row_end(ui);
        if(a->analysis_focus_return && nk_sdl_accessibility_request_focus(ui,NULL,a->analysis_focus_return==1?"Messläufe ein-/ausblenden":"Laufdetails ein-/ausblenden"))a->analysis_focus_return=0;
        workspace_rule(canvas,nk_rect(0,197,w,1));
    }nk_end(ui);
    nk_style_pop_float(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
    nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(12,5));
    nk_style_push_float(ui,&ui->style.window.rounding,0);
    nk_style_push_font(ui,&a->workspace_status_font);
    nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(UI_COLOR(header)));
    if(laboratory_begin(a,"analysis-status",nk_rect(0,h-28,w,28),NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BACKGROUND)) {
        nk_layout_row_dynamic(ui,18,a->job.running?2:1);nk_label_colored(ui,a->status,NK_TEXT_LEFT,muted());
        if(a->job.running && nk_button_label(ui,"Abbrechen")){ps_process_kill(&a->job);ps_process_close(&a->job);status(a,"Auswertung abgebrochen.");}
        workspace_rule(nk_window_get_canvas(ui),nk_rect(0,h-28,w,1));
    }nk_end(ui);
    nk_style_pop_style_item(ui);nk_style_pop_font(ui);nk_style_pop_float(ui);nk_style_pop_vec2(ui);
}
static void analysis_runs_view(app *a) {
    struct nk_context *ui=a->ui;
    nk_layout_row_dynamic(ui,29,1);struct nk_rect title;nk_widget(&title,ui);
    struct nk_command_buffer *heading_canvas=nk_window_get_canvas(ui);
    const struct nk_user_font *title_font=&a->laboratory_title_font,*count_font=&a->workbench_fonts[2];
    nk_draw_text(heading_canvas,nk_rect(title.x+6,title.y+(24-title_font->height)/2,title.w-36,title_font->height),"Messläufe",10,title_font,nk_rgba(0,0,0,0),UI_COLOR(text));
    ps_ui_accessibility_label(ui,title,"Messläufe");
    char count_text[24];snprintf(count_text,sizeof count_text,"%zu",workspace_run_count(a));
    float count_width=count_font->width(count_font->userdata,count_font->height,count_text,(int)strlen(count_text));
    struct nk_rect badge=nk_rect(title.x+title.w-6-ceilf(count_width+12),title.y+2,ceilf(count_width+12),18);
    nk_fill_rect(heading_canvas,badge,4,UI_COLOR(button));
    nk_draw_text(heading_canvas,nk_rect(badge.x+(badge.w-count_width)/2,badge.y+(badge.h-count_font->height)/2,count_width+1,count_font->height),count_text,(int)strlen(count_text),count_font,nk_rgba(0,0,0,0),muted());
    char count_description[80];snprintf(count_description,sizeof count_description,"%zu Laufdateien im Projekt",workspace_run_count(a));ps_ui_accessibility_label(ui,badge,count_description);
    if(!a->panel_render)panel_pull_handle(a,PS_DOCK_SIDEBAR,nk_rect(a->dock_panels[PS_DOCK_SIDEBAR].x,a->dock_panels[PS_DOCK_SIDEBAR].y,a->dock_panels[PS_DOCK_SIDEBAR].w,44));
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,ui->style.window.spacing.y));
    nk_layout_row_begin(ui,NK_STATIC,34,3);
    nk_layout_row_push(ui,2);nk_spacer(ui);
    nk_layout_row_push(ui,fmaxf(1,nk_window_get_content_region(ui).w-4));struct nk_rect search=nk_widget_bounds(ui);
    struct nk_style_edit edit=ui->style.edit;
    ui->style.edit.border=1;ui->style.edit.rounding=6;ui->style.edit.border_color=nk_rgba(0,0,0,0);ui->style.edit.padding=nk_vec2(9,6);
    ui->style.edit.normal=ui->style.edit.hover=ui->style.edit.active=nk_style_item_color(nk_rgba(0,0,0,0));
    bool search_focus=nk_sdl_accessibility_is_focused(ui,NULL,"Messläufe durchsuchen") || (nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT) && nk_input_is_mouse_hovering_rect(&ui->input,search));
    analysis_field_background(a,search,search_focus);
    nk_style_push_font(ui,&a->workbench_fonts[2]);
    nk_flags edited=nk_sdl_edit_named(ui,"Messläufe durchsuchen",NK_EDIT_FIELD,a->analysis_query,sizeof a->analysis_query,nk_filter_default);
    ui->style.edit=edit;
    if(!a->analysis_query[0] && !(edited&NK_EDIT_ACTIVE))analysis_text(a,nk_rect(search.x+9,search.y+8,search.w-18,ui->style.font->height),"Läufe durchsuchen …",ui->style.font,muted());
    nk_style_pop_font(ui);
    nk_layout_row_push(ui,2);nk_spacer(ui);nk_layout_row_end(ui);nk_style_pop_vec2(ui);
    nk_layout_row_dynamic(ui,6,1);nk_spacer(ui);
    analysis_heading(a,"IM PROJEKT",18,false);
    char selected[100];snprintf(selected,sizeof selected,"%u von 8 Läufen für die Analyse ausgewählt.",a->selected_count);
    float hint_height=fmaxf(32,documentation_height(ui->style.font,selected,strlen(selected),nk_window_get_content_region(ui).w)+4);
    nk_uint list_scroll_x,list_scroll_y;nk_window_get_scroll(ui,&list_scroll_x,&list_scroll_y);
    float list_top=ui->current->layout->at_y+ui->current->layout->row.height+ui->style.window.spacing.y+(float)list_scroll_y;
    float list_height=fmaxf(80,ui->current->layout->clip.y+ui->current->layout->clip.h-list_top-hint_height-64-3*ui->style.window.spacing.y);
    float cards_height=0;
    if(a->library)for(size_t i=0;i<a->library->count;i++) {
        const ps_library_item *item=&a->library->items[i];char path[4096],meta[160];float meta_y,state_y;
        if(item->kind!=PS_LIBRARY_RUN || (a->analysis_query[0] && !ps_document_find(item->name,a->analysis_query)) || ps_library_path(a->library,i,path,sizeof path)!=PS_OK)continue;
        analysis_run_caption(a,item,path,meta,sizeof meta);
        cards_height+=analysis_run_card_height(a,meta,nk_window_get_content_region(ui).w,&meta_y,&state_y)+ui->style.window.spacing.y;
    }
    float list_space=list_height;list_height=fminf(list_space,fmaxf(80,cards_height));
    nk_layout_row_dynamic(ui,list_height,1);
    memset(a->analysis_run_bounds,0,sizeof a->analysis_run_bounds);
    nk_style_push_vec2(ui,&ui->style.window.group_padding,nk_vec2(0,0));
    if(nk_group_begin(ui,"analysis-run-list",cards_height<=list_height?NK_WINDOW_NO_SCROLLBAR:0)) {
        size_t shown=0;
        if(a->library)for(size_t i=0;i<a->library->count;i++) {
            const ps_library_item *item=&a->library->items[i];
            if(item->kind!=PS_LIBRARY_RUN || (a->analysis_query[0] && !ps_document_find(item->name,a->analysis_query)))continue;
            char path[4096];if(ps_library_path(a->library,i,path,sizeof path)!=PS_OK)continue;
            nk_bool checked=selected_run(a,path)>=0;int selection=selected_run(a,path);
            char meta[160];float meta_y,state_y;analysis_run_caption(a,item,path,meta,sizeof meta);
            float card_height=analysis_run_card_height(a,meta,nk_window_get_content_region(ui).w,&meta_y,&state_y);
            nk_layout_row_dynamic(ui,card_height,1);struct nk_rect card;
            if(nk_widget(&card,ui)==NK_WIDGET_INVALID){shown++;continue;}
            if(shown<8)a->analysis_run_bounds[shown]=card;
            struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
            bool hover=!(ui->current->layout->flags&NK_WINDOW_ROM) && !ui->current->widgets_disabled && nk_input_is_mouse_hovering_rect(&ui->input,card);
            if(checked) {
                struct nk_color bg=UI_COLOR(header),sel=UI_COLOR(selection_active);
                bg.r=(nk_byte)lroundf(.6f*bg.r+.4f*sel.r);bg.g=(nk_byte)lroundf(.6f*bg.g+.4f*sel.g);bg.b=(nk_byte)lroundf(.6f*bg.b+.4f*sel.b);
                nk_fill_rect(canvas,card,7,bg);nk_stroke_rect(canvas,card,7,1,UI_COLOR(accent));
            } else if(hover)nk_fill_rect(canvas,card,7,UI_COLOR(button));
            struct nk_rect checkbox=nk_rect(card.x+10,card.y+16,15,15);
            nk_fill_rect(canvas,checkbox,2,checked?UI_COLOR(primary):UI_COLOR(edit));
            nk_stroke_rect(canvas,checkbox,2,1,checked?UI_COLOR(primary):UI_COLOR(border));
            if(checked){nk_stroke_line(canvas,checkbox.x+3,checkbox.y+7,checkbox.x+6,checkbox.y+11,2,UI_COLOR(primary_text));nk_stroke_line(canvas,checkbox.x+6,checkbox.y+11,checkbox.x+12,checkbox.y+3,2,UI_COLOR(primary_text));}
            char select_name[1200];snprintf(select_name,sizeof select_name,"Messlauf auswählen: %s",item->name);
            if(ps_ui_switch_at(ui,nk_rect(card.x+4,card.y+5,27,32),select_name,&checked))select_run(a,path,checked);
            if(nk_sdl_accessibility_is_focused(ui,NULL,select_name))nk_stroke_rect(canvas,nk_rect(checkbox.x-3,checkbox.y-3,21,21),3,2,UI_COLOR(accent));
            const struct nk_user_font *font=&a->laboratory_title_font,*small=&a->workbench_fonts[2];
            struct nk_rect name=nk_rect(card.x+36,card.y+12,card.w-44,font->height);
            analysis_text(a,name,item->name,font,UI_COLOR(text));
            char accessible[1200];snprintf(accessible,sizeof accessible,"Messlauf anzeigen: %s",item->name);
            nk_begin_disabled(ui,!idle(a));
            if(ps_ui_button_at(ui,nk_rect(card.x+32,card.y,card.w-32,selection>0?state_y:card.h),accessible,!strcmp(a->data.path,path)))open_run_path(a,path);
            nk_end_disabled(ui);
            if(nk_sdl_accessibility_is_focused(ui,NULL,accessible))nk_stroke_rect(canvas,card,7,2,UI_COLOR(accent));
            analysis_wrapped_text(a,nk_rect(card.x+36,card.y+meta_y,card.w-44,state_y-meta_y),meta,small,muted());
            char state[96];snprintf(state,sizeof state,"%s%s",selection==0?"Referenz":checked?"Für Vergleich ausgewählt":"",
                !strcmp(a->data.path,path)?checked?" · Aktiver Lauf":"Aktiver Lauf":"");
            struct nk_rect state_rect=nk_rect(card.x+36,card.y+state_y,card.w-44,small->height);
            const char *reference_label=!strcmp(a->data.path,path)?"Als Referenz · Aktiver Lauf":"Als Referenz verwenden";
            analysis_text(a,state_rect,selection>0?reference_label:state,small,selection>=0?UI_COLOR(accent):muted());
            if(selection>0) {
                char reference[1200];snprintf(reference,sizeof reference,"Als Referenz verwenden: %s",item->name);
                nk_begin_disabled(ui,!idle(a));if(ps_ui_button_at(ui,state_rect,reference,false))reference_run(a,path);nk_end_disabled(ui);
                if(nk_sdl_accessibility_is_focused(ui,NULL,reference))nk_stroke_rect(canvas,state_rect,2,2,UI_COLOR(accent));
            }
            shown++;
        }
        if(!shown){nk_layout_row_dynamic(ui,64,1);nk_label_wrap(ui,a->library_thread?"Messläufe werden geladen …":a->analysis_query[0]?"Keine passenden Messläufe. Die Auswahl bleibt erhalten.":"Noch keine Messläufe. Eine Simulation starten oder einen Messlauf importieren.");}
        nk_group_end(ui);
    }
    nk_style_pop_vec2(ui);
    snprintf(selected,sizeof selected,"%u von 8 Läufen für die Analyse ausgewählt.",a->selected_count);
    nk_layout_row_dynamic(ui,hint_height,1);nk_label_wrap(ui,selected);
    if(list_space-list_height>ui->style.window.spacing.y){nk_layout_row_dynamic(ui,list_space-list_height-ui->style.window.spacing.y,1);nk_spacer(ui);}
    nk_layout_row_dynamic(ui,30,1);if(nk_button_label(ui,"Zur Simulation →"))select_workspace_tab(a,1);
    nk_layout_row_dynamic(ui,30,1);a->import_bounds=nk_widget_bounds(ui);
    nk_style_push_style_item(ui,&ui->style.button.normal,nk_style_item_color(nk_rgba(0,0,0,0)));
    nk_style_push_color(ui,&ui->style.button.text_normal,UI_COLOR(accent));
    nk_begin_disabled(ui,!a->loaded || !idle(a) || a->library_thread || a->dialog_pending);
    if(nk_button_label(ui,"Messlauf importieren …"))choose_workspace_path(a,PS_DIALOG_IMPORT_RUN);
    nk_end_disabled(ui);
    nk_style_pop_color(ui);nk_style_pop_style_item(ui);
}
static void analysis_details_view(app *a) {
    struct nk_context *ui=a->ui;
    analysis_heading(a,"Details",24,true);
    if(!a->panel_render)panel_pull_handle(a,PS_DOCK_INSPECTOR,nk_rect(a->dock_panels[PS_DOCK_INSPECTOR].x,a->dock_panels[PS_DOCK_INSPECTOR].y,a->dock_panels[PS_DOCK_INSPECTOR].w,44));
    analysis_rule(a);analysis_heading(a,"DARSTELLUNG",32,false);
    if(!a->data.thread && a->data.channel_count) {
        if(a->plot_channel<0 || a->plot_channel>=(int)a->data.channel_count)a->plot_channel=0;
        const char *names[PS_MAX_CHANNELS];for(uint32_t i=0;i<a->data.channel_count;i++)names[i]=a->data.channels[i].name;
        analysis_heading(a,"Messkanal",24,false);nk_layout_row_dynamic(ui,34,1);
        struct nk_style_combo combo=ui->style.combo;
        analysis_combo_style(a,names[a->plot_channel]);
        a->plot_channel=nk_combo(ui,names,(int)a->data.channel_count,a->plot_channel,28,nk_vec2(280,240));
        nk_style_pop_font(ui);
        ui->style.combo=combo;
        analysis_heading(a,"Anzeigeeinheit",24,false);nk_layout_row_dynamic(ui,34,1);
        analysis_unit_menu(a,&a->data.channels[a->plot_channel]);
        const char *hint="Die Anzeigeeinheit gilt für Kurve und Statistik. Rohdatenexport behält seine gespeicherten Einheiten.";
        nk_layout_row_dynamic(ui,fmaxf(52,documentation_height(ui->style.font,hint,strlen(hint),nk_window_get_content_region(ui).w)),1);nk_label_wrap(ui,hint);
        if(a->channel_units_error[0]) {
            nk_layout_row_dynamic(ui,documentation_height(ui->style.font,a->channel_units_error,strlen(a->channel_units_error),nk_window_get_content_region(ui).w)+4,1);nk_label_wrap(ui,a->channel_units_error);
        }
    } else {nk_layout_row_dynamic(ui,68,1);nk_label_wrap(ui,a->data.thread?"Messdaten werden geladen …":"Messlauf öffnen, um Kanal und Einheit zu wählen.");}
    analysis_rule(a);analysis_heading(a,"AKTIVER LAUF",34,false);
    nk_layout_row_dynamic(ui,40,1);nk_label_wrap(ui,a->data.path[0]?diagnostic_basename(a->data.path):"Kein Lauf geöffnet");
    if(!a->data.thread && a->data.count) {
        const char *labels[]={"Dauer","Messpunkte","Kanäle","Datenstatus"};char values[4][96];
        snprintf(values[0],sizeof values[0],"%.6g s",a->data.time[a->data.count-1]-a->data.time[0]);
        snprintf(values[1],sizeof values[1],"%llu",(unsigned long long)a->data.total);
        snprintf(values[2],sizeof values[2],"%u",a->data.channel_count);
        snprintf(values[3],sizeof values[3],"%s",a->data.result==PS_RECOVERED?"Rekonstruiert":a->data.result==PS_EOF?"Vollständig":ps_result_string(a->data.result));
        for(unsigned i=0;i<4;i++){nk_layout_row_dynamic(ui,26,2);nk_label_colored(ui,labels[i],NK_TEXT_LEFT,muted());nk_label(ui,values[i],NK_TEXT_RIGHT);}
    }
    nk_layout_row_dynamic(ui,32,1);a->run_details_button=nk_widget_bounds(ui);
    nk_begin_disabled(ui,a->data.thread || !a->data.channel_count || !a->data.path[0]);
    if(nk_button_label(ui,"Modell & Laufdetails …"))(void)run_details_open(a);
    nk_end_disabled(ui);
    analysis_rule(a);analysis_heading(a,"HERKUNFT",34,false);
    analysis_heading(a,"Projekt-Analysecode",24,false);nk_layout_row_dynamic(ui,30,1);nk_label(ui,analysis_source(a),NK_TEXT_LEFT);
    if(nk_tree_state_push(ui,NK_TREE_NODE,"Quelldatei",&a->analysis_source_disclosure)) {
        const char *path=a->show_report && a->loaded_report_path[0]?a->loaded_report_path:a->data.path[0]?a->data.path:"Keine Messdatei geöffnet.";
        nk_layout_row_dynamic(ui,documentation_height(ui->style.font,path,strlen(path),fmaxf(40,nk_window_get_content_region(ui).w))+8,1);
        nk_label_wrap(ui,path);nk_tree_state_pop(ui);
    }
}

static void analysis_workspace_view(app *a,float content) {
    struct nk_context *ui=a->ui;
    analysis_comparison_sync(a);ps_comparison_state comparison=ps_comparison_read(a->comparison);
    if(a->show_report || !comparison.snapshot) {
        if(a->analysis_plot_resizing)analysis_plot_resize_abort(a);
        a->analysis_plot_splitter=nk_rect(0,0,0,0);
        if(!a->show_report){a->plot_area=nk_rect(0,0,0,0);a->active_plot_view=NULL;a->plot_drag=NULL;}
    }
    if(!a->panel_render)panel_pull_handle(a,PS_DOCK_WORKSPACE,nk_rect(a->dock_panels[PS_DOCK_WORKSPACE].x+24,a->dock_panels[PS_DOCK_WORKSPACE].y,a->dock_panels[PS_DOCK_WORKSPACE].w-48,54));
    float tab_scale=a->preferences.ui_size/16.f;
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(20*tab_scale,0));
    nk_layout_row_begin(ui,NK_STATIC,54,3);
    const char *tabs[]={"Messdaten","Analyseergebnis"};float tab_widths[2];
    for(unsigned i=0;i<2;i++)tab_widths[i]=fmaxf((i?116.f:64.f)*tab_scale,ceilf(a->workbench_fonts[2].width(a->workbench_fonts[2].userdata,a->workbench_fonts[2].height,tabs[i],(int)strlen(tabs[i]))+1));
    for(unsigned i=0;i<2;i++) {
        float width=tab_widths[i];nk_layout_row_push(ui,width);
        struct nk_rect tab;nk_widget(&tab,ui);a->analysis_tabs[i]=tab;
        bool active=a->show_report==(i!=0);
        const struct nk_user_font *font=&a->workbench_fonts[2];
        nk_draw_text(nk_window_get_canvas(ui),nk_rect(tab.x,tab.y+(tab.h-font->height)/2,tab.w,font->height),tabs[i],(int)strlen(tabs[i]),font,nk_rgba(0,0,0,0),active?UI_COLOR(text):muted());
        if(ps_ui_option_at(ui,tab,"Auswertungsansicht",tabs[i],active))a->show_report=i!=0;
        if(nk_sdl_accessibility_is_focused(ui,NULL,tabs[i]))nk_stroke_rect(nk_window_get_canvas(ui),tab,3,2,UI_COLOR(accent));
    }
    nk_layout_row_push(ui,fmaxf(20,nk_window_get_content_region(ui).w-tab_widths[0]-tab_widths[1]-2*ui->style.window.spacing.x));
    char total[100];
    if(a->show_report)snprintf(total,sizeof total,"Analysebericht · eigene Eingaben");
    else snprintf(total,sizeof total,"%u %s ausgewählt",a->selected_count,a->selected_count==1?"Lauf":"Läufe");
    nk_label_colored(ui,total,NK_TEXT_RIGHT,muted());nk_layout_row_end(ui);nk_style_pop_vec2(ui);
    analysis_rule(a);
    /* The active rule shares the bottom pixel of the full-width border. */
    struct nk_rect active_tab=a->analysis_tabs[a->show_report?1:0];
    nk_fill_rect(nk_window_get_canvas(ui),nk_rect(active_tab.x,active_tab.y+active_tab.h-1,active_tab.w,2),0,UI_COLOR(accent));
    if (a->report_thread || a->result_error[0]) {
        nk_layout_row_dynamic(ui, 42, 1);
        nk_label_wrap(ui, a->report_thread ? "Analyseergebnis wird geprüft und geladen …"
                                           : a->result_error);
    }
    if (a->show_report) {
        if(!a->analysis_report) {
            nk_layout_row_dynamic(ui,100,1);nk_label_wrap(ui,a->job.running?"Analyse läuft … Vorhandene Messdaten bleiben im Messdaten-Tab erreichbar.":"Noch kein Analysebericht geladen. Analyse starten oder einen bestehenden Bericht in Läufe & Berichte öffnen.");
        } else
        report_view(a, content);
    } else if (comparison.snapshot) {
        ps_graphic_description description;ps_graphic_snapshot_describe(comparison.snapshot,&description);
        ps_display_unit unit={.scale=description.plot.y_unit.scale};memcpy(unit.dimension,description.plot.y_unit.dimension,7);
        snprintf(unit.symbol,sizeof unit.symbol,"%s",description.plot.y_unit.symbol);
        char title[192];
        snprintf(title,sizeof title,"%s [%s]",description.plot.y_label,unit.symbol);
        for(uint32_t i=0;i<description.input_count;i++) {
            ps_graphic_run_info run;ps_graphic_snapshot_run(comparison.snapshot,i,&run);
            if(run.result==PS_OK && run.statistics_result==PS_OK)continue;
            const char *reason=run.result==PS_OK?"Vollständige Statistik nicht verfügbar":run.result==PS_INVALID?"Messkanal fehlt, ist inkompatibel oder hat keine gültigen Werte":ps_result_string(run.result);
            char note[256];snprintf(note,sizeof note,"%s: %s%s",run.label,reason,i==0 && run.result!=PS_OK?" · Referenz nicht verfügbar":"");
            nk_layout_row_dynamic(ui,documentation_height(ui->style.font,note,strlen(note),nk_window_get_content_region(ui).w)+4,1);nk_label_wrap(ui,note);
        }
        /* The reference SVG is 800 × 326 before a manual height is chosen. */
        a->analysis_plot_auto_height=roundf(nk_window_get_content_region(ui).w*326.f/800.f);
        plot_draw(a,NULL,NULL,0,analysis_plot_dimension(a,content),title,false,&a->data_views[a->plot_channel],comparison.snapshot);
        analysis_plot_separator(a);
        const char *stat_note="Gesamter Lauf · unabhängig vom Zoom";
        float heading_width=ceilf(a->laboratory_title_font.width(a->laboratory_title_font.userdata,a->laboratory_title_font.height,"Statistik",9))+4;
        float note_width=nk_window_get_content_region(ui).w-heading_width-ui->style.window.spacing.x;
        bool stack_note=ui->style.font->width(ui->style.font->userdata,ui->style.font->height,stat_note,(int)strlen(stat_note))>note_width;
        nk_layout_row_begin(ui,NK_STATIC,44,stack_note?1:2);nk_layout_row_push(ui,heading_width);
        nk_style_push_font(ui,&a->laboratory_title_font);nk_label(ui,"Statistik",NK_TEXT_LEFT);nk_style_pop_font(ui);
        if(!stack_note){nk_layout_row_push(ui,note_width);nk_label_colored(ui,stat_note,NK_TEXT_RIGHT,muted());}
        nk_layout_row_end(ui);
        if(stack_note){nk_layout_row_dynamic(ui,documentation_height(ui->style.font,stat_note,strlen(stat_note),nk_window_get_content_region(ui).w),1);nk_style_push_color(ui,&ui->style.text.color,muted());nk_label_wrap(ui,stat_note);nk_style_pop_color(ui);}
        nk_layout_row_dynamic(ui,fminf(360,28+39*(float)description.input_count),1);
        nk_style_push_vec2(ui,&ui->style.window.group_padding,nk_vec2(0,0));
        nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,0));
        if(nk_group_begin(ui,"measurement-statistics",description.input_count<=5 && a->preferences.ui_size<=16?NK_WINDOW_NO_SCROLLBAR:0)) {
            nk_layout_row_dynamic(ui,28,5);
            const char *head[] = {"Messlauf", "Mittelwert", "Std.-Abw.", "Minimum", "Maximum"};
            for (int i = 0; i < 5; i++)
                nk_label_colored(ui, head[i], NK_TEXT_LEFT, muted());
            for (uint32_t i = 0; i < description.input_count; i++) {
                ps_graphic_run_info run;ps_graphic_snapshot_run(comparison.snapshot,i,&run);
                nk_layout_row_dynamic(ui,39,5);
                char stat_label[160];snprintf(stat_label,sizeof stat_label,"%s%s",run.label,i==0?" · Referenz":"");
                channel_unit_cell(ui,stat_label);
                double values[] = {run.si_statistics.mean,ps_statistics_stddev(&run.si_statistics),run.si_statistics.min,run.si_statistics.max};
                for (int k = 0; k < 4; k++) {
                    char text[80];
                    double converted;
                    if(ps_display_unit_value(&unit,values[k],&converted)==PS_OK)snprintf(text,sizeof text,"%.8g",converted);
                    else snprintf(text,sizeof text,"nicht darstellbar");
                    if (run.result!=PS_OK || run.statistics_result!=PS_OK || !run.si_statistics.count || (k == 1 && run.si_statistics.count < 2))
                        snprintf(text, sizeof text, "—");
                    channel_unit_cell(ui,text);
                }
            }
            nk_group_end(ui);
        }
        nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
        char statistics_unit[256];snprintf(statistics_unit,sizeof statistics_unit,"%s in %s · vollständige gültige Messwerte je Lauf · Stichprobenstandardabweichung",description.plot.y_label,unit.symbol);
        nk_layout_row_dynamic(ui,documentation_height(ui->style.font,statistics_unit,strlen(statistics_unit),nk_window_get_content_region(ui).w)+4,1);nk_label_colored_wrap(ui,statistics_unit,muted());
    } else {
        nk_layout_row_dynamic(ui, 110, 1);
        nk_label_wrap(ui, !a->selected_count?"Wähle links mindestens einen Messlauf für Kurve und Statistik aus.":comparison.loading?"Vergleichskurven und vollständige Statistiken werden geladen …":comparison.result==PS_MEMORY?"Der Vergleich konnte wegen Speichermangels nicht geladen werden.":comparison.result==PS_LIMIT?"Der Vergleich überschreitet das Ressourcenlimit. Weniger oder kleinere Läufe wählen.":comparison.result!=PS_OK?"Die gewählten Läufe enthalten keine darstellbare gemeinsame Messkurve. Kanal, Einheit und Laufdateien prüfen.":a->data.thread ? "Messdaten werden geladen …"
                          : a->last_run[0]
                              ? "Werte den gespeicherten Lauf aus. Die Analyse erstellt "
                                "Messkurven, Statistik und einen Bericht."
                              : "Hier werden Ergebnisse sichtbar. Starte zuerst eine "
                                "Simulation und zeichne einen Lauf auf.");
        if(!comparison.loading && comparison.result!=PS_OK && a->selected_count) {
            nk_layout_row_dynamic(ui,32,1);a->comparison_retry_bounds=nk_widget_bounds(ui);
            if(nk_button_label(ui,"Vergleich erneut laden"))a->comparison_revision++;
        }
    }
    float bottom=ui->current->layout->clip.y+ui->current->layout->clip.h;
    nk_uint scroll_x,scroll_y;nk_window_get_scroll(ui,&scroll_x,&scroll_y);
    float fill=bottom-36-ui->current->layout->at_y-ui->current->layout->row.height-(float)scroll_y;
    if(fill>0){nk_layout_row_dynamic(ui,fill,1);nk_spacer(ui);}
    nk_layout_row_dynamic(ui,36,2);nk_label_colored(ui,"Rohdaten · gesamter aktiver Lauf",NK_TEXT_LEFT,muted());a->raw_export_bounds=nk_widget_bounds(ui);
    nk_style_push_style_item(ui,&ui->style.button.normal,nk_style_item_color(nk_rgba(0,0,0,0)));
    nk_style_push_color(ui,&ui->style.button.text_normal,UI_COLOR(accent));
    nk_style_push_flags(ui,&ui->style.button.text_alignment,NK_TEXT_RIGHT);
    nk_begin_disabled(ui,!idle(a) || !a->last_run[0]);
    if(nk_button_label(ui,"CSV · Alle Rohdaten ↗"))analyze(a,true);
    nk_end_disabled(ui);
    nk_style_pop_flags(ui);nk_style_pop_color(ui);nk_style_pop_style_item(ui);
}
