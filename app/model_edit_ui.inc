static float model_edit_banner(app *a) {
    ps_model_edit *m=&a->model_edit;struct nk_context *ui=a->ui;model_edit_sync(a);
    bool dirty=model_edit_dirty(m);
    if(!dirty && !m->building && !m->stop_build && !m->ready && !*m->error)return 0;
    float start=ui->current->layout->at_y;
    laboratory_note(a,m->stop_build?"Lauf wird vor dem Build gestoppt …":m->building?"Modell wird kompiliert …":dirty?"Modellwerte geändert":"Build erfolgreich · Neuer Lauf verwendet die neuen Artefakte");
    if(*m->error) {
        /* One wrapping message in the inspector's scroll flow. An inner
         * scrollable group captured the wheel when it moved under the pointer,
         * stopping the parent before the remaining settings were reachable. */
        nk_style_push_color(ui,&ui->style.text.color,UI_COLOR(error));
        laboratory_note(a,m->error);
        nk_style_pop_color(ui);
    }
    if(m->building) {
        nk_layout_row_dynamic(ui,34,1);
        if(nk_button_label(ui,"Build abbrechen")){ps_process_kill(&a->job);ps_process_close(&a->job);a->built=false;model_edit_finish(a,false);status(a,"Build abgebrochen. Vorgemerkte Modellwerte bleiben erhalten.");}
    }
    nk_layout_row_dynamic(ui,36,1);a->model_build_bounds=nk_widget_bounds(ui);
    nk_begin_disabled(ui,!dirty || !model_edit_valid(m) || m->building || m->stop_build || a->reset_pending || (!a->runner.running && !idle(a)));
    if(action_button(ui,a->runner.running?"Stoppen & neu kompilieren":"Neu kompilieren",true))model_edit_request(a);
    nk_end_disabled(ui);
    nk_layout_row_dynamic(ui,32,1);a->model_discard_bounds=nk_widget_bounds(ui);
    nk_begin_disabled(ui,!dirty || m->stop_build);
    if(nk_button_label(ui,"Verwerfen"))model_edit_discard(a);nk_end_disabled(ui);
    if(m->ready){nk_layout_row_dynamic(ui,34,1);nk_begin_disabled(ui,!a->built || !idle(a));if(nk_button_label(ui,"Neuer Lauf")){start_run(a);m->ready=false;}nk_end_disabled(ui);}
    return ui->current->layout->at_y-start;
}
static void model_edit_fields(app *a) {
    struct nk_context *ui=a->ui;ps_model_edit *m=&a->model_edit;
    memset(a->model_card_bounds,0,sizeof a->model_card_bounds);
    if(!workspace_section_begin(a,"Modellwerte",&a->model_disclosure))return;
    if(*m->bindings.error)laboratory_note(a,m->bindings.error);
    else if(!m->bindings.count)laboratory_note(a,"Keine buildgebundenen Modellwerte deklariert. Freigegebene Laufparameter stehen separat darunter.");
    else {
        laboratory_note(a,"Quellwerte vormerken und neu kompilieren. Eingestellte Laufparameter bleiben erhalten und können Quellstandards übersteuern; dort bei Bedarf Standard wählen.");
        for(unsigned i=0;i<m->bindings.count;i++) {
            const ps_model_value *v=&m->bindings.entries[i];char label[192];snprintf(label,sizeof label,"%s%s%s",v->label,*v->unit?" ":"",v->unit);
            char previous[64];memcpy(previous,m->text[i],sizeof previous);
            float f=a->preferences.ui_size/16.f,width=nk_window_get_content_region(ui).w;
            const struct nk_user_font *caption=&a->laboratory_caption_font;
            float label_height=15.95f*f*laboratory_wrapped_lines(caption,label,fmaxf(1,width-24*f));
            float card_height=24*f+label_height+5*f+35*f;
            nk_layout_space_begin(ui,NK_STATIC,card_height,1);
            struct nk_vec2 origin=nk_layout_space_to_screen(ui,nk_vec2(0,0));
            struct nk_rect card=nk_rect(origin.x,origin.y,width,card_height);
            a->model_card_bounds[i]=card;
            struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
            nk_fill_rect(canvas,card,8*f,UI_COLOR(window));
            nk_stroke_rect(canvas,nk_rect(card.x+f/2,card.y+f/2,fmaxf(0,card.w-f),fmaxf(0,card.h-f)),7.5f*f,f,UI_COLOR(border));
            laboratory_wrapped_text(canvas,caption,label,nk_rect(card.x+12*f,card.y+12*f,fmaxf(1,card.w-24*f),label_height),15.95f*f,muted());
            ps_ui_accessibility_label(ui,nk_rect(card.x+12*f,card.y+12*f,fmaxf(1,card.w-24*f),label_height),label);
            nk_layout_space_push(ui,nk_rect(12*f,12*f+label_height+5*f,fmaxf(1,card.w-24*f),35*f));
            a->model_input_bounds[i]=nk_widget_bounds(ui);
            nk_style_push_font(ui,&a->laboratory_model_font);
            nk_style_push_float(ui,&ui->style.edit.border,0);nk_style_push_vec2(ui,&ui->style.edit.padding,nk_vec2(0,3*f));
            nk_style_push_style_item(ui,&ui->style.edit.normal,nk_style_item_color(nk_rgba(0,0,0,0)));
            nk_style_push_style_item(ui,&ui->style.edit.hover,nk_style_item_color(nk_rgba(0,0,0,0)));
            nk_style_push_style_item(ui,&ui->style.edit.active,nk_style_item_color(nk_rgba(0,0,0,0)));
            nk_edit_string_zero_terminated(ui,NK_EDIT_FIELD,m->text[i],sizeof m->text[i],nk_filter_default);
            nk_style_pop_style_item(ui);nk_style_pop_style_item(ui);nk_style_pop_style_item(ui);nk_style_pop_vec2(ui);nk_style_pop_float(ui);nk_style_pop_font(ui);
            nk_layout_space_end(ui);ps_ui_row_exact_dynamic(ui,10*f,1);nk_spacer(ui);
            if(strcmp(previous,m->text[i])){m->revision++;m->error[0]=0;}
            double value;char hint[256];
            bool valid=ps_model_value_parse(v,m->text[i],&value);
            snprintf(hint,sizeof hint,"%s %.6g %s · Bereich %.6g bis %.6g",m->has_compiled?"Kompiliert:":"Quellstand:",m->compiled[i]/v->scale,v->unit,v->minimum/v->scale,v->maximum/v->scale);
            laboratory_note(a,hint);
            if(!valid){nk_style_push_color(ui,&ui->style.text.color,UI_COLOR(error));laboratory_note(a,"Endliche Zahl innerhalb des Bereichs eingeben (Komma oder Punkt).");nk_style_pop_color(ui);}
        }
    }
    workspace_section_end(a);
}
