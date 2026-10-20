/* Build-bound editing is a separate transaction from declared runtime parameters. */
static void model_edit_clear(app *a) {
    ps_text_document_destroy(&a->model_edit.source);memset(&a->model_edit,0,sizeof a->model_edit);
}
static void model_edit_format(const ps_model_value *binding,double si,char text[64]) {
    snprintf(text,64,"%.17g",si/binding->scale);
    char *point=strchr(text,'.');
    if(!strchr(text,'e') && !strchr(text,'E') && strlen(text)<59) {
        if(!point)strcat(text,",00");
        else {size_t decimals=strlen(point+1);*point=',';if(decimals<2)strcat(text,"0");}
    } else if(point)*point=',';
}
static bool model_edit_dirty(const ps_model_edit *m) {
    for(unsigned i=0;i<m->bindings.count;i++){double value;ps_model_value baseline=m->bindings.entries[i];baseline.value=m->compiled[i];if(!ps_model_value_parse(&baseline,m->text[i],&value) || value!=m->compiled[i])return true;}
    return false;
}
static bool model_edit_valid(const ps_model_edit *m) {
    if(*m->bindings.error || !m->bindings.count)return false;
    for(unsigned i=0;i<m->bindings.count;i++){double value;if(!ps_model_value_parse(&m->bindings.entries[i],m->text[i],&value))return false;}
    return true;
}
static void model_edit_sync(app *a) {
    ps_model_edit *m=&a->model_edit;
    if(!a->loaded || a->analysis_only)return;
    if(m->loaded && !strcmp(m->project,a->project)) {
        int length=nk_str_len_char(&a->experiment.string);const char *text=nk_str_get_const(&a->experiment.string);
        bool same=length>=0 && (size_t)length==m->source.length && !memcmp(text,m->source.saved,m->source.length);
        if(same || a->dirty || m->building)return;
        if(model_edit_dirty(m)){snprintf(m->error,sizeof m->error,"Quelldefinition geändert. Eingaben bleiben vorgemerkt; Verwerfen lädt danach die gespeicherten Bindungen.");return;}
    }
    model_edit_clear(a);snprintf(m->project,sizeof m->project,"%s",a->project);
    char path[4096];join(path,sizeof path,a->project,experiment_source(a));
    if(ps_text_document_open(&m->source,path)!=PS_DOCUMENT_OK){snprintf(m->error,sizeof m->error,"Modellquelle konnte nicht gelesen werden.");return;}
    m->loaded=true;
    ps_model_values_parse(m->source.saved,m->source.length,a->language_experiment,&m->bindings);
    for(unsigned i=0;i<m->bindings.count;i++) {
        m->compiled[i]=m->bindings.entries[i].value;
        model_edit_format(&m->bindings.entries[i],m->compiled[i],m->text[i]);
    }
}
static bool model_edit_write(app *a,const char values[PS_MODEL_VALUES_MAX][64]);
static void model_edit_discard(app *a) {
    ps_model_edit *m=&a->model_edit;
    bool source_changed=false;char restored[PS_MODEL_VALUES_MAX][64]={{0}};
    for(unsigned i=0;i<m->bindings.count;i++) {
        source_changed=source_changed || m->bindings.entries[i].value!=m->compiled[i];
        snprintf(restored[i],sizeof restored[i],"%.17g",m->compiled[i]/m->bindings.entries[i].scale);
    }
    bool restored_source=!source_changed || m->building || m->stop_build || model_edit_write(a,restored);
    for(unsigned i=0;i<m->bindings.count;i++)model_edit_format(&m->bindings.entries[i],m->compiled[i],m->text[i]);
    m->revision++;if(restored_source)m->error[0]=0;
}
static void model_edit_submitted(app *a) {
    model_edit_sync(a);ps_model_edit *m=&a->model_edit;if(!m->loaded)return;
    /* Freeze only the submitted source, never the currently typed future values. */
    ps_model_values index;const char *source=nk_str_get_const(&a->experiment.string);int n=nk_str_len_char(&a->experiment.string);
    if(n<0 || !ps_model_values_parse(source,(size_t)n,a->language_experiment,&index) || index.count!=m->bindings.count)return;
    for(unsigned i=0;i<index.count;i++) {
        if(strcmp(index.entries[i].id,m->bindings.entries[i].id))return;
        m->submitted[i]=index.entries[i].value;
    }
    char path[4096];join(path,sizeof path,a->project,experiment_source(a));ps_text_document snapshot={0};
    if(ps_text_document_open(&snapshot,path)!=PS_DOCUMENT_OK || snapshot.length!=(size_t)n || memcmp(snapshot.saved,source,(size_t)n)) {
        ps_text_document_destroy(&snapshot);return;
    }
    ps_text_document_destroy(&m->source);m->source=snapshot;m->bindings=index;
    m->building=true;m->ready=false;m->submitted_revision=m->revision;
}
static void model_edit_finish(app *a,bool success) {
    ps_model_edit *m=&a->model_edit;if(!m->building)return;
    if(success) {
        ps_text_document current={0};
        success=ps_text_document_open(&current,m->source.path)==PS_DOCUMENT_OK && current.length==m->source.length &&
            !memcmp(current.saved,m->source.saved,m->source.length) && build_inputs_current(a);
        ps_text_document_destroy(&current);
        if(!success){a->built=false;status(a,"Quelle während Build geändert. Neuer Build erforderlich.");}
    }
    m->building=false;
    if(success) {
        for(unsigned i=0;i<m->bindings.count;i++)m->compiled[i]=m->submitted[i];
        m->has_compiled=true;m->ready=m->build_requested;m->error[0]=0;
    } else if(m->build_requested)snprintf(m->error,sizeof m->error,"Build fehlgeschlagen oder abgebrochen. Vorgemerkte Werte bleiben erhalten; Protokoll prüfen.");
    m->build_requested=false;
}
static bool model_edit_write(app *a,const char values[PS_MODEL_VALUES_MAX][64]) {
    ps_model_edit *m=&a->model_edit;
    if(!idle(a)){snprintf(m->error,sizeof m->error,"Quelle kann erst nach Abschluss der laufenden Aktion zurückgesetzt werden.");return false;}
    if(a->dirty || a->recovery || documents_dirty(a)) {
        snprintf(m->error,sizeof m->error,"Quellenkonflikt: Editoränderungen zuerst speichern und Modellwerte neu laden.");return false;
    }
    const char *source=nk_str_get_const(&a->experiment.string);int n=nk_str_len_char(&a->experiment.string);
    char logical[4096];join(logical,sizeof logical,a->project,experiment_source(a));
    if(n<0 || !ps_text_document_same_file(logical,m->source.path) || (size_t)n!=m->source.length || memcmp(source,m->source.saved,m->source.length)) {
        snprintf(m->error,sizeof m->error,"Quelle wurde zwischenzeitlich geändert. Modellwerte neu laden; Eingaben bleiben erhalten.");return false;
    }
    size_t length;char *replacement=ps_model_values_rewrite(m->source.saved,m->source.length,&m->bindings,values,&length);
    if(!replacement){snprintf(m->error,sizeof m->error,"Modellwerte ungültig oder Quelle zu groß.");return false;}
    ps_model_values next;if(!ps_model_values_parse(replacement,length,a->language_experiment,&next)){free(replacement);return false;}
    struct nk_text_edit edit;nk_textedit_init_default(&edit);
    if(!set_editor_text(&edit,replacement,length)){nk_textedit_free(&edit);free(replacement);return false;}
    ps_document_result result=ps_text_document_save(&m->source,replacement,length);
    if(result!=PS_DOCUMENT_OK) {
        snprintf(m->error,sizeof m->error,"Quelle extern geändert oder Speichern fehlgeschlagen (%u). Datei und Eingaben erhalten.",(unsigned)result);
        nk_textedit_free(&edit);free(replacement);return false;
    }
    nk_textedit_free(&a->experiment);a->experiment=edit;
    free(a->saved_source[0]);a->saved_source[0]=replacement;m->bindings=next;
    invalidate_build(a);m->error[0]=0;return true;
}
static void model_edit_apply_requested(app *a) {
    ps_model_edit *m=&a->model_edit;if(!model_edit_write(a,m->requested))return;
    m->build_requested=true;build_project(a);
    if(!a->job.running)snprintf(m->error,sizeof m->error,"Build konnte nicht gestartet werden. Eingaben bleiben vorgemerkt.");
}
static void model_edit_request(app *a) {
    ps_model_edit *m=&a->model_edit;
    if(!model_edit_valid(m) || m->building || m->stop_build || a->reset_pending)return;
    memcpy(m->requested,m->text,sizeof m->requested);
    if(a->runner.running) {
        if(!a->stop_at && command(a,PS_MSG_STOP)){a->stop_at=ps_clock();m->stop_build=true;}
        return;
    }
    model_edit_apply_requested(a);
}
static void model_edit_tick(app *a) {
    ps_model_edit *m=&a->model_edit;
    if(m->stop_build && idle(a)){m->stop_build=false;model_edit_apply_requested(a);}
    if(m->building && !a->job.running)model_edit_finish(a,a->built);
}
