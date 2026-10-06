static bool collision_tutorial_start(app *a,const char *root,const char *mode) {
    bool language=!strcmp(mode,"collision-tutorial-phys");char path[4096];
    join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    a->template_analysis_language=language?1:0;a->example=language?18:2;
    join(a->project_input,sizeof a->project_input,root,"project");new_project(a);if(!a->loaded)return false;
    join(path,sizeof path,a->root,language?"examples/documentation/collision_main.phys":"examples/documentation/collision_main.c");
    if(!load_editor(&a->experiment,path))return false;
    a->dirty=true;
    join(path,sizeof path,a->root,language?"examples/documentation/collision_analysis.phys":"examples/documentation/collision_analysis.c");
    if(!load_editor(&a->analysis,path))return false;
    a->analysis_dirty=true;a->dt=.02;a->project_settings_dirty=true;select_build_profile(a,1);
    SDL_SetWindowSize(a->window,1080,740);return true;
}
static void collision_tutorial_frame(app *a,int *stage,int *code,const char **capture) {
    static unsigned steps;static double held;bool ok=true;
    switch(*stage) {
    case 0:steps=0;held=0;build_project(a);break;
    case 1: {
        if(!idle(a) || a->library_thread)return;
        ok=a->built && a->parameters.count==5;unsigned found=0;
        for(unsigned i=0;i<a->parameters.count;i++) {
            if(!strcmp(a->parameters.entries[i].name,"restitution")){strcpy(a->parameters.selected[i],"0");found++;}
            if(!strcmp(a->parameters.entries[i].name,"massA")){strcpy(a->parameters.selected[i],"2");found++;}
        }
        ok &= found==2 && start_run_mode(a,true);a->camera_target=ps_v3(0,.15,0);a->yaw=.4f;a->pitch=.15f;a->zoom=4;break;
    }
    case 2:
        if(!a->hello || !a->history_count)return;
        held=a->simulation_time;steps++;ok=command(a,PS_MSG_STEP);break;
    case 3:
        if(a->simulation_time<=held)return;
        if(steps<100){*stage=2;return;}
        ok=a->paused && a->channel_count==11 && fabs(a->values[2]-.2)<1e-12 && fabs(a->values[3]-.2)<1e-12 &&
            fabs(a->values[4]-.06)<1e-12 && fabs(a->values[5]-.6)<1e-12 && fabs(a->values[6]-.48)<1e-12 &&
            fabs(a->values[7]-.54)<1e-12 && fabs(a->values[9]-4.0/3)<1e-11 && a->values[10]==1 && a->scene.count==10;
        *capture="collision-inelastic-scene.bmp";ok &= command(a,PS_MSG_STOP);break;
    case 4:
        if(a->runner.running || !idle(a))return;
        ok=a->data.result==PS_EOF && a->data.total==101 && a->timeline_scenes;if(ok)analyze(a,false);break;
    case 5: {
        if(!idle(a))return;
        uint32_t plots=0,tables=0;ps_table_row row,event;
        ok=a->analysis_report && ps_report_describe(a->analysis_report,NULL,NULL,&plots,&tables)==PS_OK && plots==4 && tables==2 &&
            ps_report_row_read(a->analysis_report,0,0,&row)==PS_OK && row.values[0]==101 && row.values[1]<1e-12 &&
            fabs(row.values[2]-.48)<1e-12 && row.values[3]<1e-12 && row.values[4]==1 &&
            ps_report_row_read(a->analysis_report,1,0,&event)==PS_OK && fabs(event.values[0]-4.0/3)<1e-11 && fabs(event.values[1]+.8)<1e-12;
        a->tab=2;a->result_view=3;break;
    }
    case 6:*capture="collision-energy-analysis.bmp";a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Collision tutorial stage %d: %.17g %s\n",*stage,a->simulation_time,a->status);*code=1;a->quitting=true;}
    if(a->quitting)printf("COLLISION TUTORIAL SELF-TEST: %s\n",*code?"FAILED":"PASSED");
    (*stage)++;
}
