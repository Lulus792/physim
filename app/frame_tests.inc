static bool frame_test_start(app *a,const char *root,const char *mode) {
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    join(a->project_input,sizeof a->project_input,root,"project");
    if(!strcmp(mode,"frames-read")){open_project(a);return a->loaded;}
    bool language=!strcmp(mode,"frames-phys");a->example=language?10:0;new_project(a);if(!a->loaded)return false;
    join(path,sizeof path,a->root,language?"examples/language/scene_frames.phys":"examples/scene_frames/main.c");
    if(!load_editor(&a->experiment,path))return false;
    a->dirty=true;a->dt=.01;a->project_settings_dirty=true;select_build_profile(a,1);
    SDL_SetWindowSize(a->window,1080,740);return true;
}
static void frame_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    bool read=!strcmp(mode,"frames-read"),ok=true;static double held;
    ps_vec3 world;ps_camera camera={a->yaw,a->pitch,a->zoom,a->camera_target,a->orthographic,a->show_vectors,a->show_grid};
    switch(*stage) {
    case 0:
        if(read){char path[4096];join(path,sizeof path,a->project,"saved-frame-path.txt");size_t n=0;char *text=SDL_LoadFile(path,&n);
            ok=text && n<sizeof a->last_run;if(ok){snprintf(a->last_run,sizeof a->last_run,"%s",text);request_dataset(a);}SDL_free(text);}
        else build_project(a);break;
    case 1:
        if(!idle(a) || a->library_thread)return;
        if(read){ok=a->timeline_scenes && a->timeline.latest.scene.count==11;a->tab=1;held=a->timeline.latest.time;}
        else ok=a->built && start_run_mode(a,true);
        a->yaw=0;a->pitch=.3f;a->zoom=3;a->camera_target=ps_v3(.3,.025,0);a->show_grid=false;break;
    case 2: {
        if(!read && (!a->hello || !a->history_count))return;
        const ps_scene *scene=display_scene(a);
        ok=scene->count==11 && ps_scene_world_point(scene,3,scene->objects[3].a,&world)==PS_OK &&
            fabs(world.x-(.4-.75*(read?held:0)))<1e-12 && fabs(world.y-.025)<1e-12;
        if(!ok)break;
        float x,y;ok=ps_graphics_project(&camera,world,(double)a->scene_viewport_bounds.w/a->scene_viewport_bounds.h,&x,&y);
        test_mouse(a,nk_rect(a->scene_viewport_bounds.x+x*a->scene_viewport_bounds.w,a->scene_viewport_bounds.y+y*a->scene_viewport_bounds.h,1,1),true);break;
    }
    case 3:{struct nk_rect r=a->scene_viewport_bounds;test_mouse(a,nk_rect(r.x+r.w/2,r.y+r.h/2,1,1),false);break;}
    case 4:
        ok=a->scene_selected && a->scene_selected_id==1 && a->scene_label_bounds[5].w>0 &&
            nk_font_find_glyph((const struct nk_font*)a->font_ui->userdata.ptr,0x3b1)->codepoint==0x3b1;
        *capture=read?"frames-reopened.bmp":"frames-world.bmp";
        test_mouse(a,a->scene_label_bounds[5],true);break;
    case 5:test_mouse(a,a->scene_label_bounds[5],false);break;
    case 6:
        ok=a->scene_selected && a->scene_selected_id==3;
        a->scene_view.hidden |= UINT32_C(1)<<2;break;
    case 7:
        for(uint32_t i=3;i<11;i++)ok &= !ps_scene_view_visible(&a->scene_view,i);
        ok &= a->scene_label_bounds[5].w==0;*capture="frames-hidden.bmp";a->scene_view.hidden=0;break;
    case 8:
        if(read){a->quitting=true;break;}
        test_mouse(a,a->run_control_bounds[2],true);break;
    case 9:test_mouse(a,a->run_control_bounds[2],false);break;
    case 10:
        if(a->simulation_time==0)return;
        ok=a->paused && a->simulation_time==.01 && a->values[0]==.01 &&
            ps_scene_world_point(&a->scene,3,a->scene.objects[3].a,&world)==PS_OK && fabs(world.x-.3925)<1e-12;
        *capture="frames-stepped.bmp";ok &= command(a,PS_MSG_STOP);break;
    case 11:
        if(a->runner.running || !idle(a))return;
        ok=a->data.result==PS_EOF && a->data.total==2 && a->timeline.latest.scene.objects[0].shape==PS_FRAME;
        char path[4096];join(path,sizeof path,a->project,"saved-frame-path.txt");FILE *f=fopen(path,"wb");
        ok &= f && fputs(a->last_run,f)>=0;if(f)ok &= fclose(f)==0;a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Frames %s stage %d: selected=%u, t=%.17g %s\n",mode,*stage,a->scene_selected_id,a->simulation_time,a->status);*code=1;a->quitting=true;}
    if(a->quitting)printf("FRAMES %s SELF-TEST: %s\n",mode,*code?"FAILED":"PASSED");
    (*stage)++;
}
