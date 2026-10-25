static bool diagnostic_test_start(app *a,const char *root,const char *mode) {
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    join(a->project_input,sizeof a->project_input,root,"project");
    bool analysis=strstr(mode,"analysis")!=NULL,language=strstr(mode,"phys")!=NULL;
    a->manager_analysis_only=analysis;a->template_analysis_language=language;a->example=language?10:0;
    new_project(a);if(!a->loaded)return false;
    if(!strcmp(mode,"diagnostic-build-helper")) {
        join(path,sizeof path,a->project,"include");if(!ps_make_directory(path))return false;
        join(path,sizeof path,a->project,"include/helper α.h");
        FILE *file=fopen(path,"wb");if(!file)return false;
        bool ok=fputs("/* helper source */\n#error compiler navigation sentinel\n",file)>=0;
        ok &= fclose(file)==0;if(!ok)return false;
        const char *include="\n#include \"include/helper α.h\"\n";
        nk_str_append_text_char(&a->experiment.string,include,(int)strlen(include));
        a->dirty=true;select_build_profile(a,1);SDL_SetWindowSize(a->window,1080,740);return true;
    }
    join(path,sizeof path,a->root,analysis?(language?"examples/language/diagnostic_analysis.phys":"examples/diagnostics/analysis.c"):
        (language?"examples/language/diagnostic_experiment.phys":"examples/diagnostics/main.c"));
    if(!load_editor(analysis?&a->analysis:&a->experiment,path))return false;
    if(analysis)a->analysis_dirty=true;else a->dirty=true;
    select_build_profile(a,1);SDL_SetWindowSize(a->window,1080,740);return true;
}
static void diagnostic_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    bool analysis=strstr(mode,"analysis")!=NULL,ok=true;
    if(!strcmp(mode,"diagnostic-build-helper")) {
        static int selected=-1,reported=-1;static unsigned scroll_settle;
        if(reported!=*stage){fprintf(stderr,"Compiler helper stage %d: %s\n",*stage,a->status);reported=*stage;}
        char path[4096];join(path,sizeof path,a->project,"include/helper α.h");
        switch(*stage) {
        case 0:test_window_key_mod(a->window,SDLK_F5,0);break;
        case 1:
            if(a->job.running)return;
            ok=!a->built && !a->runner.running;
            for(int i=0;i<a->diagnostic_count;i++)
                if(a->diagnostics[i].foreign && a->diagnostics[i].line==2 && strstr(a->diagnostics[i].text,"compiler navigation sentinel"))selected=i;
            ok &= selected>=0;
            if(selected>=0) {
                const ps_protocol_record *event=ps_protocol_find(&a->protocol,a->diagnostics[selected].protocol_id);
                ok &= event && event->message.origin==PS_PROTOCOL_BUILD && event->message.level==PS_LOG_ERROR &&
                    event->message.line==2 && strstr(event->message.source,"helper α.h") &&
                    strstr(event->message.raw,"compiler navigation sentinel");
                if(event) {
                    const ps_protocol_record *output=ps_protocol_find(&a->protocol,event->message.transcript_id);
                    ok &= output && output->message.raw_size>event->message.raw_size &&
                        strstr(output->message.raw,"compiler navigation sentinel");
                }
            }
            test_window_key_mod(a->window,SDLK_F6,0);break;
        case 2:
            if(scroll_settle){--scroll_settle;return;}
            ok=!a->runner.running && !a->last_run[0] && selected>=0 && a->diagnostic_bounds[selected].w>0;
            if(ok) {
                struct nk_rect view=workbench_visible(a)?a->diagnostic_view_bounds:a->dock_panels[PS_DOCK_LOG],row=a->diagnostic_bounds[selected];
                if(row.y+row.h>view.y+view.h || row.y<view.y) {
                    test_plot_wheel(a,view.x+view.w*.5f,view.y+view.h*.5f,row.y+row.h>view.y+view.h?-1:1);
                    scroll_settle=2;return;
                }
            }
            if(ok)test_mouse(a,a->diagnostic_bounds[selected],true);break;
        case 3:test_mouse(a,a->diagnostic_bounds[selected],false);break;
        case 4: {
            workspace_document *document=active_document(a);
            ok=a->tab==8 && document && ps_text_document_same_file(path,document->file.path) &&
                document->edit.select_end>document->edit.select_start && strstr(document->file.saved,"#error compiler navigation sentinel");
            *capture="compiler-helper-jump.bmp";break;
        }
        case 5:test_mouse(a,a->diagnostic_mode_bounds,true);break;
        case 6:test_mouse(a,a->diagnostic_mode_bounds,false);break;
        case 7:
            ok=a->diagnostic_raw && strstr(a->log,"compiler navigation sentinel") && a->diagnostic_count>0;
            *capture="compiler-helper-output.bmp";
            test_mouse(a,a->diagnostic_mode_bounds,true);break;
        case 8:test_mouse(a,a->diagnostic_mode_bounds,false);break;
        case 9: {
            workspace_document *document=active_document(a);
            ok=document!=NULL && !a->diagnostic_raw;
            if(ok) {
                document->edit.mode=NK_TEXT_EDIT_MODE_INSERT;
                nk_textedit_delete_selection(&document->edit);
                nk_textedit_text(&document->edit,"/* repaired helper */",21);
                document_changed(a,&document->edit);test_window_key_mod(a->window,SDLK_F5,0);
            }
            break;
        }
        case 10:
            if(a->job.running)return;
            ok=a->built && !a->dirty && !a->analysis_dirty && !documents_dirty(a);
            if(ok)test_window_key_mod(a->window,SDLK_F6,0);break;
        case 11:
            if(!a->hello || a->simulation_time<.01)return;
            test_window_key_mod(a->window,SDLK_F9,0);break;
        case 12: {
            if(!idle(a))return;
            ps_run_reader run;
            ok=a->last_run[0] && ps_run_open(&run,a->last_run)==PS_OK;
            if(ok) {
                double time,values[PS_MAX_CHANNELS];unsigned rows=0;ps_result result;
                while((result=ps_run_next(&run,&time,values))==PS_OK)rows++;
                ok=result==PS_EOF && rows>=3;ps_run_reader_close(&run);
            }
            *capture="compiler-helper-repaired.bmp";a->quitting=true;break;
        }
        }
        if(!ok){fprintf(stderr,"Compiler helper stage %d: diagnostics=%d selected=%d %s\n",*stage,a->diagnostic_count,selected,a->status);*code=1;a->quitting=true;}
        if(a->quitting)printf("COMPILER HELPER SELF-TEST: %s\n",*code?"FAILED":"PASSED");
        (*stage)++;return;
    }
    if(!strcmp(mode,"diagnostic-foreign")) {
        char path[4096];join(path,sizeof path,a->project,"helper α.c");
        if(*stage==0) {
            FILE *file=fopen(path,"wb");ok=file!=NULL;
            if(file){const char *text="// helper\n// failing α expression\n";ok=fwrite(text,1,strlen(text),file)==strlen(text);ok &= fclose(file)==0;}
            ps_diagnostic record;ok &= ps_diagnostic_set(&record,PS_INVALID,"step","value","helper α.c",2,4,"Invalid α") == PS_OK;
            if(ok){structured_diagnostic(a,&record,false);ok=a->diagnostic_count==1 && a->diagnostics[0].foreign;if(ok)jump_to_diagnostic(a,&a->diagnostics[0]);}
            if(ok) {
                char nested[4096];join(nested,sizeof nested,a->project,"nested");ok=ps_make_directory(nested);
                join(nested,sizeof nested,a->project,"nested/main.c");FILE *file=fopen(nested,"wb");ok &= file!=NULL;
                if(file){ok &= fputs("// nested\n// diagnostic source\n",file)>=0;ok &= fclose(file)==0;}
                parse_diagnostic(a,"nested/main.c:2:4: warning: nested source",PS_PROTOCOL_BUILD);
                ok &= a->diagnostic_count==2 && a->diagnostics[1].foreign;
                const ps_protocol_record *warning=ps_protocol_find(&a->protocol,a->diagnostics[1].protocol_id);
                ok &= warning && warning->message.level==PS_LOG_WARNING && warning->message.origin==PS_PROTOCOL_BUILD &&
                    !strcmp(warning->message.raw,"nested/main.c:2:4: warning: nested source");
                ok &= ps_diagnostic_set(&record,PS_INVALID,"step","value","main.c",2,4,"Different main source") == PS_OK;
                if(ok)structured_diagnostic(a,&record,false);
                ok &= a->diagnostic_count==3 && a->diagnostics[1].foreign && !a->diagnostics[2].foreign;
            }
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
        int selected=-1;unsigned structured=0;
        for(int i=0;i<a->diagnostic_count;i++)if(a->diagnostics[i].structured){selected=i;++structured;}
        ok=structured==1 && selected>=0;
        diagnostic *item=ok?&a->diagnostics[selected]:NULL;
        ok=ok && item->code==(analysis?PS_INVALID:PS_SINGULAR) && item->line>0 && !item->foreign && strstr(item->text,"α");
        char path[4096];snprintf(path,sizeof path,"%s.psdiag",analysis?a->report:a->last_run);ps_diagnostic record={0};
        ok=ok && ps_diagnostic_load(path,&record)==PS_OK && record.line==(uint32_t)item->line;
        if(ok) {
            const ps_protocol_record *event=ps_protocol_find(&a->protocol,item->protocol_id);
            ok=event && event->message.origin==(analysis?PS_PROTOCOL_ANALYSIS:PS_PROTOCOL_SIMULATION) &&
                event->message.has_code && event->message.code==record.code && !event->message.has_time &&
                !strcmp(event->message.operation,record.operation) && !strcmp(event->message.argument,record.argument) &&
                !strcmp(event->message.source,record.source) && event->message.line==record.line && event->message.column==record.column &&
                event->message.raw_size==strlen(record.message) && !memcmp(event->message.raw,record.message,strlen(record.message));
        }
        if(ok)jump_to_diagnostic(a,item);break;
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
