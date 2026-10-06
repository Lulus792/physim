static bool diagnostic_test_start(app *a,const char *root,const char *mode) {
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    join(a->project_input,sizeof a->project_input,root,"project");
    bool analysis=strstr(mode,"analysis")!=NULL,language=strstr(mode,"phys")!=NULL;
    a->manager_analysis_only=analysis;a->template_analysis_language=language;a->example=language?10:0;
    new_project(a);if(!a->loaded)return false;
    join(path,sizeof path,a->root,analysis?(language?"examples/language/diagnostic_analysis.phys":"examples/diagnostics/analysis.c"):
        (language?"examples/language/diagnostic_experiment.phys":"examples/diagnostics/main.c"));
    if(!load_editor(analysis?&a->analysis:&a->experiment,path))return false;
    if(analysis)a->analysis_dirty=true;else a->dirty=true;
    select_build_profile(a,1);SDL_SetWindowSize(a->window,1080,740);return true;
}
static void diagnostic_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    bool analysis=strstr(mode,"analysis")!=NULL,ok=true;
    if(!strcmp(mode,"diagnostic-foreign")) {
        char path[4096];join(path,sizeof path,a->project,"helper α.c");
        if(*stage==0) {
            FILE *file=fopen(path,"wb");ok=file!=NULL;
            if(file){const char *text="// helper\n// failing α expression\n";ok=fwrite(text,1,strlen(text),file)==strlen(text);ok &= fclose(file)==0;}
            ps_diagnostic record;ok &= ps_diagnostic_set(&record,PS_INVALID,"step","value","helper α.c",2,4,"Invalid α") == PS_OK;
            if(ok){structured_diagnostic(a,&record,false);ok=a->diagnostic_count==1 && a->diagnostics[0].foreign;if(ok)jump_to_diagnostic(a,&a->diagnostics[0]);}
        } else {
            ok=a->tab==8 && a->document_count==1 && ps_text_document_same_file(path,a->documents[0].file.path) &&
                a->documents[0].edit.select_end>a->documents[0].edit.select_start;
            *capture="diagnostic-source.bmp";a->quitting=true;
        }
        if(!ok){*code=1;a->quitting=true;}
        if(a->quitting)printf("DIAGNOSTIC %s SELF-TEST: %s\n",mode,*code?"FAILED":"PASSED");
        (*stage)++;return;
    }
    switch(*stage) {
    case 0:build_project(a);break;
    case 1:
        if(!idle(a) || a->library_thread)return;
        ok=a->built;if(analysis)analyze(a,false);else ok &= start_run_mode(a,true);break;
    case 2:
        if(analysis){if(!idle(a))return;}
        else {if(!a->hello || !a->history_count)return;ok=a->simulation_time==0 && a->values[0]==1;ok &= command(a,PS_MSG_STEP);}
        break;
    case 3: {
        if(a->runner.running || !idle(a))return;
        ok=a->diagnostic_count==1 && a->diagnostics[0].structured && a->diagnostics[0].code==(analysis?PS_INVALID:PS_SINGULAR) &&
            a->diagnostics[0].line>0 && !a->diagnostics[0].foreign && strstr(a->diagnostics[0].text,"α");
        char path[4096];snprintf(path,sizeof path,"%s.psdiag",analysis?a->report:a->last_run);ps_diagnostic record;
        ok &= ps_diagnostic_load(path,&record)==PS_OK && record.line==(uint32_t)a->diagnostics[0].line;
        if(ok)jump_to_diagnostic(a,&a->diagnostics[0]);break;
    }
    case 4: {
        struct nk_text_edit *edit=analysis?&a->analysis:&a->experiment;
        ok=a->tab==(analysis?2:0) && (!analysis || a->analysis_tab==1) && edit->select_end>edit->select_start;
        *capture="diagnostic-source.bmp";a->quitting=true;break;
    }
    }
    if(!ok){fprintf(stderr,"Diagnostic %s stage %d: count=%d %s\n",mode,*stage,a->diagnostic_count,a->status);*code=1;a->quitting=true;}
    if(a->quitting)printf("DIAGNOSTIC %s SELF-TEST: %s\n",mode,*code?"FAILED":"PASSED");
    (*stage)++;
}
