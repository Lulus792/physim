/* Actual model rendering at both atlas/export densities, not a text-only bank. */
static void documentation_model_test_frame(app *a,int *stage,int *code,const char **capture) {
    static unsigned sample,frames;static int experiment,analysis;
    static uint32_t experiment_crc,analysis_crc;static double simulation;static char image[64];bool ok=true;
    switch((*stage)++) {
    case 0:
        experiment=a->experiment.string.len;analysis=a->analysis.string.len;
        experiment_crc=ps_crc32((const unsigned char *)nk_str_get_const(&a->experiment.string),(size_t)experiment);
        analysis_crc=ps_crc32((const unsigned char *)nk_str_get_const(&a->analysis.string),(size_t)analysis);
        simulation=a->simulation_time;sample=0;test_window_key_mod(a->window,SDLK_F1,0);break;
    case 1:
        for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++)if(!strcmp(documentation_topics[i].path,"docs/pendulum-tutorial.md")){open_documentation(a,i);break;}
        break;
    case 2: {
        unsigned part=sample%3;ps_preferences p=a->preferences;p.ui_size=part==2?22:16;p.theme=(int)part;
        preferences_apply(a,&p);a->doc_zoom_size=part==2?32:16;a->doc_test_density=sample>=3?2:1;
        ok=documentation_fonts_prepare(a,a->doc_zoom_size,documentation_code_size(a));
        a->doc_font_ui=a->doc_ui_fonts[(a->doc_zoom_size-16)/2];a->doc_font_title=a->doc_title_fonts[(a->doc_zoom_size-16)/2];
        a->doc_context_caption_test=true;a->doc_detail_test_case=sample;
        a->doc_context_example=false;a->doc_context_hidden=false;a->doc_navigation_hidden=false;a->doc_context_scroll_y[0]=0;
        ok=ok && SDL_RestoreWindow(a->doc_window) && SDL_SetWindowSize(a->doc_window,part?760:1080,part?540:780) && SDL_SyncWindow(a->doc_window);
        frames=0;break;
    }
    case 3:
        if(frames++<2 || a->doc_scroll_pending){--*stage;return;}
        ok=a->doc_detail_test_ok && fabsf(a->doc_raster_density-a->doc_test_density)<.01f && a->doc_context_figure_bounds.w>180;
        snprintf(image,sizeof image,"docs-model-%u.%s",sample,sample>=3?"png":"bmp");*capture=image;break;
    case 4:
        ok=a->doc_detail_test_ok && a->experiment.string.len==experiment && a->analysis.string.len==analysis && a->simulation_time==simulation &&
            ps_crc32((const unsigned char *)nk_str_get_const(&a->experiment.string),(size_t)experiment)==experiment_crc &&
            ps_crc32((const unsigned char *)nk_str_get_const(&a->analysis.string),(size_t)analysis)==analysis_crc;
        printf("DOCUMENTATION MODEL RASTER: case=%u ui=%u reader=%u density=%.2f complete=%d\n",sample,a->preferences.ui_size,documentation_font_size(a),a->doc_raster_density,ok);
        if(++sample<6)*stage=2;else {a->doc_test_density=0;a->doc_context_caption_test=false;a->quitting=true;}
        break;
    }
    if(!ok){fprintf(stderr,"Model raster stage=%d case=%u failed\n",*stage-1,sample);*code=1;a->quitting=true;}
    if(a->quitting)printf("DOCUMENTATION MODEL RASTER SELF-TEST: %s\n",*code?"FAILED":"PASSED");
}
