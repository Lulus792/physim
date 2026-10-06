static bool contact_world_test_start(app *a,const char *root,bool language) {
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    join(a->project_input,sizeof a->project_input,root,"project");a->example=language?10:0;new_project(a);if(!a->loaded)return false;
    join(path,sizeof path,a->root,language?"examples/language/contact_stack.phys":"examples/contact_stack/main.c");if(!load_editor(&a->experiment,path))return false;
    a->dirty=true;a->dt=.005;a->project_settings_dirty=true;select_build_profile(a,1);
    SDL_SetWindowSize(a->window,1080,740);return true;
}
static void contact_world_test_frame(app *a,int *stage,int *code,const char **capture) {
    static unsigned steps;static double held;bool ok=true;
    switch(*stage) {
    case 0:steps=0;held=0;build_project(a);break;
    case 1:
        if(!idle(a) || a->library_thread)return;
        ok=a->built && start_run_mode(a,true);a->camera_target=ps_v3(0,1.6,0);a->yaw=.5f;a->pitch=.2f;a->zoom=7;break;
    case 2:
        if(!a->hello || !a->history_count)return;
        held=a->simulation_time;steps++;ok=command(a,PS_MSG_STEP);break;
    case 3:
        if(a->simulation_time<=held)return;
        if(steps<40){*stage=2;return;}
        ok=a->paused && a->channel_count==11 && a->values[4]>0 && a->values[5]>0 && a->values[6]>0 &&
            a->values[0]>.45 && a->values[1]>3.4 && a->scene.count>5;
        *capture="contact-stack.bmp";ok &= command(a,PS_MSG_STOP);break;
    case 4:
        if(a->runner.running || !idle(a))return;
        ok=a->data.result==PS_EOF && a->data.total==41 && a->timeline_scenes && a->timeline.latest.scene.count>5;
        a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Contact world stage %d: t=%.17g %s\n",*stage,a->simulation_time,a->status);*code=1;a->quitting=true;}
    if(a->quitting)printf("CONTACT WORLD SELF-TEST: %s\n",*code?"FAILED":"PASSED");
    (*stage)++;
}
