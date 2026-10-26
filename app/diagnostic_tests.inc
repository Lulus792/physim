/* Drive the visible navigator, including nested and outer scrolling. Data-only
 * source/path tests below deliberately remain separate from this UI proof. */
static struct nk_rect diagnostic_test_intersection(struct nk_rect a,struct nk_rect b) {
    float x=fmaxf(a.x,b.x),y=fmaxf(a.y,b.y);
    return nk_rect(x,y,fmaxf(0,fminf(a.x+a.w,b.x+b.w)-x),fmaxf(0,fminf(a.y+a.h,b.y+b.h)-y));
}
static bool diagnostic_test_contains(struct nk_rect outer,struct nk_rect inner) {
    return inner.w>0 && inner.h>0 && inner.x>=outer.x && inner.y>=outer.y &&
        inner.x+inner.w<=outer.x+outer.w && inner.y+inner.h<=outer.y+outer.h;
}
/* 1 = fully reachable, 0 = wheel sent, -1 = bounded reachability failure. */
static int diagnostic_test_reveal(app *a,struct nk_rect target,struct nk_rect pane,unsigned *attempts) {
    ps_protocol_ui *p=&a->protocol_view;struct nk_rect visible=diagnostic_test_intersection(pane,p->bounds);
    if(diagnostic_test_contains(visible,target)){*attempts=0;return 1;}
    if(++*attempts<=80 && visible.h<60) {
        test_plot_wheel(a,p->bounds.x+8,p->bounds.y+p->bounds.h*.5f,pane.y<p->bounds.y?3:-3);return 0;
    }
    if(*attempts>80 || target.w<=0 || target.h<=0) {
        fprintf(stderr,"DIAGNOSTIC C REACH: target=%.1f,%.1f,%.1f,%.1f pane=%.1f,%.1f,%.1f,%.1f log=%.1f,%.1f,%.1f,%.1f\n",
            target.x,target.y,target.w,target.h,pane.x,pane.y,pane.w,pane.h,p->bounds.x,p->bounds.y,p->bounds.w,p->bounds.h);return -1;
    }
    if(diagnostic_test_contains(pane,target)) {
        /* The eight-pixel margin belongs to the outer log, not a text box. */
        float direction=target.y<p->bounds.y?3:-3;
        test_plot_wheel(a,p->bounds.x+8,p->bounds.y+p->bounds.h*.5f,direction);
    } else {
        /* Stay in group padding so a read-only original does not eat the wheel. */
        test_plot_wheel(a,visible.x+2,visible.y+visible.h*.5f,target.y<pane.y?3:-3);
    }
    return 0;
}
static int diagnostic_test_navigate(app *a,uint64_t id) {
    static unsigned phase,settle,attempts;static int initial_tab;static uint32_t source_crc;
    ps_protocol_ui *p=&a->protocol_view;
    if(settle){--settle;return 0;}
    if(!phase) {
        initial_tab=a->tab;source_crc=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
        protocol_open_from(a,NULL);phase=1;settle=2;return 0;
    }
    if(!ps_protocol_find(&a->protocol,id) || source_crc!=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string)))return -1;
    switch(phase) {
    case 1:test_mouse(a,p->levels[1],true);break;
    case 2:test_mouse(a,p->levels[1],false);break;
    case 3: {
        if(p->level!=PS_LOG_ERROR || *p->query || p->origin!=-1)return -1;
        size_t row=0;while(row<p->row_count && p->row_ids[row]!=id)row++;
        if(row==p->row_count)return -1;
        int reachable=diagnostic_test_reveal(a,p->rows[row],p->panes[1],&attempts);
        if(reachable<=0){settle=2;return reachable;}
        test_mouse(a,p->rows[row],true);break;
    }
    case 4: {
        size_t row=0;while(row<p->row_count && p->row_ids[row]!=id)row++;
        if(row==p->row_count)return -1;test_mouse(a,p->rows[row],false);break;
    }
    case 5: {
        if(p->selected!=id || a->tab!=initial_tab)return -1;
        int reachable=diagnostic_test_reveal(a,p->source,p->panes[2],&attempts);
        if(reachable<=0){settle=2;return reachable;}
        printf("{\"kind\":\"diagnostic_c_navigation\",\"action\":\"selected-without-jump\",\"id\":%llu,\"tab\":%d}\n",(unsigned long long)id,a->tab);
        test_mouse(a,p->source,true);break;
    }
    case 6:test_mouse(a,p->source,false);break;
    case 7:
        printf("{\"kind\":\"diagnostic_c_navigation\",\"action\":\"source-action-released\",\"id\":%llu,\"tab\":%d}\n",(unsigned long long)id,a->tab);return 1;
    }
    phase++;settle=2;return 0;
}
static bool diagnostic_test_original(app *a,uint64_t id,bool transcript,uint32_t original_crc,uint32_t transcript_crc) {
    ps_protocol_ui *p=&a->protocol_view;const ps_protocol_record *selected=ps_protocol_find(&a->protocol,id);
    const ps_protocol_record *record=selected && transcript?ps_protocol_find(&a->protocol,selected->message.transcript_id):selected;
    if(!record || p->selected!=id || p->transcript!=transcript || p->original_closed || !p->original ||
       p->original_id!=record->id || p->original_size!=record->message.raw_size || !strstr(p->original,"compiler navigation sentinel"))return false;
    uint32_t crc=ps_crc32(record->message.raw,record->message.raw_size);
    if(crc!=(transcript?transcript_crc:original_crc))return false;
    printf("{\"kind\":\"diagnostic_c_original\",\"transcript\":%s,\"bytes\":%zu,\"crc32\":%u,\"selected\":%llu}\n",transcript?"true":"false",record->message.raw_size,crc,(unsigned long long)id);return true;
}
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
        static int selected=-1,reported=-1;static unsigned scroll_settle,reveal_attempts;
        static uint32_t original_crc,transcript_crc;
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
                    original_crc=ps_crc32(event->message.raw,event->message.raw_size);
                    const ps_protocol_record *output=ps_protocol_find(&a->protocol,event->message.transcript_id);
                    ok &= output && output->message.raw_size>event->message.raw_size &&
                        strstr(output->message.raw,"compiler navigation sentinel");
                    if(output)transcript_crc=ps_crc32(output->message.raw,output->message.raw_size);
                }
            }
            test_window_key_mod(a->window,SDLK_F6,0);break;
        case 2:
            ok=!a->runner.running && !a->last_run[0] && selected>=0;
            if(ok) {
                int result=diagnostic_test_navigate(a,a->diagnostics[selected].protocol_id);
                if(!result)return;ok=result>0;
            }
            break;
        case 3:break;
        case 4: {
            workspace_document *document=active_document(a);
            ok=a->tab==8 && document && ps_text_document_same_file(path,document->file.path) &&
                document->edit.select_end>document->edit.select_start && strstr(document->file.saved,"#error compiler navigation sentinel");
            ok &= diagnostic_test_original(a,a->diagnostics[selected].protocol_id,false,original_crc,transcript_crc);
            *capture="compiler-helper-jump.png";break;
        }
        case 5: {
            if(scroll_settle){--scroll_settle;return;}
            int result=diagnostic_test_reveal(a,a->protocol_view.transcript_toggle,a->protocol_view.panes[2],&reveal_attempts);
            if(!result){scroll_settle=2;return;}ok=result>0;if(ok)test_mouse(a,a->protocol_view.transcript_toggle,true);break;
        }
        case 6:test_mouse(a,a->protocol_view.transcript_toggle,false);scroll_settle=2;break;
        case 7: {
            if(scroll_settle){--scroll_settle;return;}
            int result=diagnostic_test_reveal(a,a->protocol_view.transcript_toggle,a->protocol_view.panes[2],&reveal_attempts);
            if(!result){scroll_settle=2;return;}ok=result>0;
            ok &= diagnostic_test_original(a,a->diagnostics[selected].protocol_id,true,original_crc,transcript_crc);
            if(SDL_getenv("PHYSIM_TEST_ALL_CAPTURES"))*capture="compiler-helper-output.png";
            if(ok)test_mouse(a,a->protocol_view.transcript_toggle,true);break;
        }
        case 8:test_mouse(a,a->protocol_view.transcript_toggle,false);scroll_settle=2;break;
        case 9: {
            if(scroll_settle){--scroll_settle;return;}
            workspace_document *document=active_document(a);
            ok=document!=NULL && diagnostic_test_original(a,a->diagnostics[selected].protocol_id,false,original_crc,transcript_crc);
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
            if(SDL_getenv("PHYSIM_TEST_ALL_CAPTURES"))*capture="compiler-helper-repaired.png";a->quitting=true;break;
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
        if(ok){int result=diagnostic_test_navigate(a,item->protocol_id);if(!result)return;ok=result>0;}break;
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
