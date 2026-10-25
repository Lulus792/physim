static bool model_edit_test_start(app *a,const char *root,const char *mode) {
    return laboratory_test_start(a,root,mode);
}
static void model_edit_test_frame(app *a,int *stage,int *code,const char **capture) {
    static double held;static char previous_run[4096];static char *analysis_source_copy;
    static unsigned warmup;bool ok=true;ps_model_edit *m=&a->model_edit;
    static bool settle;if(settle){settle=false;return;}settle=true;
    switch(*stage) {
    case 0:if(warmup++<3)return;build_project(a);break;
    case 1:
        if(!idle(a) || a->library_thread)return;
        ok=a->built && m->bindings.count==2 && m->has_compiled && m->compiled[0]==1.5;start_run(a);break;
    case 2:if(!a->hello || a->simulation_time<.1)return;command(a,PS_MSG_PAUSE);break;
    case 3:
        if(!a->paused)return;held=display_time(a);snprintf(previous_run,sizeof previous_run,"%s",a->last_run);
        a->laboratory_groups[0]=NK_MINIMIZED;a->model_disclosure=NK_MAXIMIZED;a->laboratory_scroll_y=0;break;
    case 4:test_mouse(a,a->model_input_bounds[0],true);break;
    case 5:test_mouse(a,a->model_input_bounds[0],false);break;
    case 6:test_key(a,SDLK_A);break;
    case 7:layout_test_text(a,"1,75");break;
    case 8:
        ok=!strcmp(m->text[0],"1,75") && model_edit_dirty(m) && m->compiled[0]==1.5 &&
            a->runner.running && a->paused && display_time(a)==held && a->built;
        *capture="model-values-staged.bmp";break;
    case 9:test_mouse(a,a->model_build_bounds,true);break;
    case 10:test_mouse(a,a->model_build_bounds,false);break;
    case 11:
        if(!m->building)return;
        ok=!a->runner.running && fabs(m->submitted[0]-1.75)<1e-12;
        strcpy(m->text[0],"2,25");m->revision++;break;
    case 12:
        if(!idle(a) || a->library_thread)return;
        ok=a->built && fabs(m->compiled[0]-1.75)<1e-12 && !strcmp(m->text[0],"2,25") && model_edit_dirty(m) &&
            !strcmp(a->last_run,previous_run) && exists(previous_run);
        *capture="model-values-newer-edit.bmp";model_edit_discard(a);a->model_disclosure=NK_MINIMIZED;a->parameter_disclosure=NK_MAXIMIZED;break;
    case 13:ok=!model_edit_dirty(m);test_mouse(a,a->parameter_default_bounds[0],true);break;
    case 14:test_mouse(a,a->parameter_default_bounds[0],false);break;
    case 15:
        {double value;ok=ps_parameter_catalog_value(&a->parameters,0,&value)==PS_OK && fabs(value-1.75)<1e-12;}
        start_run(a);break;
    case 16:
        if(!a->hello || a->simulation_time<.05)return;
        ok=strcmp(a->last_run,previous_run) && exists(previous_run);
        if(a->scene.count) {
            bool found=false;for(unsigned i=0;i<a->scene.count;i++)if(a->scene.objects[i].id==3){ps_vec3 p=a->scene.objects[i].a;found=true;ok=ok && fabs(sqrt(p.x*p.x+p.y*p.y)-1.75)<1e-6;}ok=ok && found;
        } else ok=false;
        command(a,PS_MSG_STOP);a->stop_at=ps_clock();break;
    case 17:
        if(!idle(a) || a->library_thread)return;
        analysis_source_copy=copy_editor_text(&a->analysis);ok=analysis_source_copy!=NULL;
        nk_str_append_text_char(&a->analysis.string,"\n! syntax_fault\n",16);a->analysis_dirty=true;
        strcpy(m->text[0],"1.9");m->revision++;model_edit_request(a);break;
    case 18:
        if(!idle(a))return;
        ok=!a->built && *m->error && model_edit_dirty(m) && fabs(m->compiled[0]-1.75)<1e-12;
        *capture="model-values-build-error.bmp";
        ok=ok && set_editor_text(&a->analysis,analysis_source_copy,strlen(analysis_source_copy));a->analysis_dirty=true;free(analysis_source_copy);analysis_source_copy=NULL;
        model_edit_request(a);break;
    case 19:
        if(!idle(a))return;
        ok=a->built && fabs(m->compiled[0]-1.9)<1e-12 && !model_edit_dirty(m);
        nk_str_append_text_char(&a->experiment.string,"\n// unsaved\n",12);a->dirty=true;
        strcpy(m->text[0],"2.1");m->revision++;model_edit_request(a);
        ok=ok && *m->error && !a->job.running && a->dirty;
        ok=ok && set_editor_text(&a->experiment,a->saved_source[0],strlen(a->saved_source[0]));a->dirty=false;break;
    case 20: {
        FILE *file=fopen(m->source.path,"ab");ok=file && fputs("\n// external\n",file)>=0;if(file && fclose(file))ok=false;
        strcpy(m->text[0],"2.2");m->revision++;model_edit_request(a);ok=ok && *m->error && !a->job.running;
        file=fopen(m->source.path,"wb");ok=ok && file && fwrite(m->source.saved,1,m->source.length,file)==m->source.length;if(file && fclose(file))ok=false;
        break;
    }
    case 21:model_edit_request(a);ok=a->job.running;if(a->job.running){ps_process_kill(&a->job);protocol_job_cancel(a);ps_process_close(&a->job);}break;
    case 22:
        ok=*m->error && model_edit_dirty(m) && fabs(m->compiled[0]-1.9)<1e-12;
        model_edit_request(a);break;
    case 23:
        if(!idle(a))return;
        ok=a->built && fabs(m->compiled[0]-2.2)<1e-12 && !model_edit_dirty(m) && m->ready;
        *capture="model-values-ready.bmp";a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"MODEL VALUES failed stage %d: text=%s compiled=%.17g built=%d error=%s status=%s\n",*stage,m->text[0],m->compiled[0],a->built,m->error,a->status);*code=1;a->quitting=true;}
    if(a->quitting){free(analysis_source_copy);analysis_source_copy=NULL;printf("MODEL VALUES SELF-TEST: %s\n",*code?"FAILED":"PASSED");}
    (*stage)++;
}
