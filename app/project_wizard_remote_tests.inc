/* Independent native clients select the real cards and cancel this sheet.
 * The existing loaded project and its source must remain unchanged. */
static void project_wizard_remote_frame(app *a,const char *root,int *stage,int *code) {
    static unsigned frames,seen;static int previous=-1;static uint32_t source_crc;static char project[4096];
    if(*stage==0) {
        SDL_SetWindowSize(a->window,1080,740);
        source_crc=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
        snprintf(project,sizeof project,"%s",a->project);open_project_manager(a,"Datei");*stage=1;return;
    }
    if(frames++<3)return;
    if(previous<0){int w,h;SDL_GetWindowSize(a->window,&w,&h);printf("{\"kind\":\"wizard_remote_environment\",\"width\":%d,\"height\":%d,\"ui_size\":%u}\nPROJECT WIZARD REMOTE READY\n",w,h,a->preferences.ui_size);fflush(stdout);}
    if(previous!=a->manager_template) {
        previous=a->manager_template;seen|=1u<<(unsigned)previous;
        printf("{\"kind\":\"wizard_remote_selected\",\"template\":%d,\"seen\":%u}\n",previous,seen);fflush(stdout);
    }
    char complete[4096];join(complete,sizeof complete,root,"wizard-reader-complete");
    if(!exists(complete))return;
    uint32_t current=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
    bool ok=!a->project_manager && seen==255 && current==source_crc && !strcmp(project,a->project) && a->loaded;
    printf("{\"kind\":\"wizard_remote_result\",\"passed\":%s,\"seen\":%u,\"source_crc\":%u}\nPROJECT WIZARD REMOTE SELF-TEST: %s\n",ok?"true":"false",seen,(unsigned)current,ok?"PASSED":"FAILED");fflush(stdout);
    *code=ok?0:1;a->quitting=true;(*stage)++;
}
