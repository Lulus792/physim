static uint64_t batch_resume_file_hash(const char *path) {
    FILE *f=fopen(path,"rb");if(!f)return 0;
    uint64_t hash=UINT64_C(14695981039346656037);unsigned char bytes[32768];size_t n;
    while((n=fread(bytes,1,sizeof bytes,f))!=0)for(size_t i=0;i<n;i++){hash^=bytes[i];hash*=UINT64_C(1099511628211);}
    if(ferror(f))hash=0;
    if(fclose(f))hash=0;
    return hash;
}
static bool batch_resume_test_start(app *a,const char *root,const char *mode) {
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    join(a->project_input,sizeof a->project_input,root,"project");a->example=!strcmp(mode,"resume-phys")?8:0;
    new_project(a);select_build_profile(a,1);SDL_SetWindowSize(a->window,1080,740);return a->loaded;
}
static void batch_resume_test_frame(app *a,int *stage,int *code,const char **capture) {
    static bool settle;static uint32_t reused;static uint32_t source_hash;static uint64_t old_hashes[64],artifacts[6];
    static const char *names[]={"completed.csv","endpoints.csv","series.txt","status.txt","resume.bin","experiment" MODULE_EXT};
    if(settle){settle=false;return;}settle=true;bool ok=true;
    switch(*stage) {
    case 0:build_project(a);break;
    case 1:if(!idle(a) || a->library_thread)return;ok=a->built;a->tab=5;a->batch_runs=64;a->batch_steps=2000;a->batch_workers=4;a->batch_dt=.01;strcpy(a->batch_channel,"position.x");break;
    case 2:test_mouse(a,a->batch_start_bounds,true);break;
    case 3:test_mouse(a,a->batch_start_bounds,false);break;
    case 4:
        if(SDL_GetAtomicInt(&a->batch_completed)<2)return;
        ok=a->batch_thread!=NULL;test_mouse(a,a->batch_cancel_bounds,true);break;
    case 5:test_mouse(a,a->batch_cancel_bounds,false);break;
    case 6:
        if(!idle(a) || a->library_thread)return;
        reused=a->batch_result.completed;ok=a->batch_result.cancelled && reused>0 && reused<64;
        snprintf(a->batch_resume_test_path,sizeof a->batch_resume_test_path,"%s",a->batch_options.directory);
        nk_str_append_text_char(&a->experiment.string,"\n/* preserve current edits */\n",(int)strlen("\n/* preserve current edits */\n"));a->dirty=true;invalidate_build(a);
        for(unsigned i=0;i<64;i++){char path[4096];snprintf(path,sizeof path,"%s/run-%04u.psrun",a->batch_resume_test_path,i+1);old_hashes[i]=batch_resume_file_hash(path);}
        for(unsigned i=0;i<6;i++){char path[4096];join(path,sizeof path,a->batch_resume_test_path,names[i]);artifacts[i]=batch_resume_file_hash(path);}
        source_hash=ps_crc32((const unsigned char *)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
        *capture="resume-cancelled.bmp";break;
    case 7:test_mouse(a,a->batch_resume_bounds,true);break;
    case 8:test_mouse(a,a->batch_resume_bounds,false);break;
    case 9:
        if(!idle(a) || a->library_thread)return;
        ok=a->batch_code==PS_OK && !a->batch_result.cancelled && a->batch_result.completed==64 &&
           a->batch_result.reused==reused && a->batch_result.started==64-reused && a->analysis_report &&
           a->dirty && source_hash==ps_crc32((const unsigned char *)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
        for(unsigned i=0;i<64;i++){char path[4096];snprintf(path,sizeof path,"%s/run-%04u.psrun",a->batch_resume_test_path,i+1);ok &= old_hashes[i]==batch_resume_file_hash(path);}
        for(unsigned i=0;i<6;i++){char path[4096];join(path,sizeof path,a->batch_resume_test_path,names[i]);ok &= artifacts[i]==batch_resume_file_hash(path);}
        *capture="resume-result.bmp";a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Resume UI stage %d failed: %s (%s)\n",*stage,a->status,a->batch_result.error);*code=1;a->quitting=true;}
    if(a->quitting)printf("BATCH RESUME SELF-TEST: %s\n",*code?"FAILED":"PASSED");
    (*stage)++;
}
