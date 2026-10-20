static float model_edit_banner(app *a) {
    ps_model_edit *m=&a->model_edit;struct nk_context *ui=a->ui;model_edit_sync(a);
    bool dirty=model_edit_dirty(m);
    if(!dirty && !m->building && !m->stop_build && !m->ready && !*m->error)return 0;
    float start=ui->current->layout->at_y;
    laboratory_note(a,m->stop_build?"Lauf wird vor dem Build gestoppt …":m->building?"Modell wird kompiliert …":dirty?"Modellwerte geändert":"Build erfolgreich · Neuer Lauf verwendet die neuen Artefakte");
    if(*m->error) {
        nk_layout_row_dynamic(ui,32,1);
        if(nk_group_begin(ui,"model-value-error",NK_WINDOW_BORDER)){laboratory_note(a,m->error);nk_group_end(ui);}
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
    if(!nk_tree_state_push(ui,NK_TREE_TAB,"Modellwerte",&a->model_disclosure))return;
    if(*m->bindings.error)laboratory_note(a,m->bindings.error);
    else if(!m->bindings.count)laboratory_note(a,"Keine buildgebundenen Modellwerte deklariert. Freigegebene Laufparameter stehen separat darunter.");
    else {
        laboratory_note(a,"Quellwerte vormerken und neu kompilieren. Eingestellte Laufparameter bleiben erhalten und können Quellstandards übersteuern; dort bei Bedarf Standard wählen.");
        for(unsigned i=0;i<m->bindings.count;i++) {
            const ps_model_value *v=&m->bindings.entries[i];char label[192];snprintf(label,sizeof label,"%s / %s",v->label,v->unit);
            nk_layout_row_dynamic(ui,24,1);nk_label_colored(ui,label,NK_TEXT_LEFT,muted());
            char previous[64];memcpy(previous,m->text[i],sizeof previous);
            nk_layout_row_dynamic(ui,46*a->preferences.ui_size/16.f,1);a->model_input_bounds[i]=nk_widget_bounds(ui);
            nk_style_push_font(ui,&a->laboratory_value_font);
            nk_edit_string_zero_terminated(ui,NK_EDIT_FIELD,m->text[i],sizeof m->text[i],nk_filter_default);nk_style_pop_font(ui);
            if(strcmp(previous,m->text[i])){m->revision++;m->error[0]=0;}
            double value;char hint[256];
            bool valid=ps_model_value_parse(v,m->text[i],&value);
            snprintf(hint,sizeof hint,"%s %.6g %s · Bereich %.6g bis %.6g",m->has_compiled?"Kompiliert:":"Quellstand:",m->compiled[i]/v->scale,v->unit,v->minimum/v->scale,v->maximum/v->scale);
            laboratory_note(a,hint);
            if(!valid){nk_style_push_color(ui,&ui->style.text.color,UI_COLOR(error));laboratory_note(a,"Endliche Zahl innerhalb des Bereichs eingeben (Komma oder Punkt).");nk_style_pop_color(ui);}
        }
    }
    nk_tree_state_pop(ui);
}
