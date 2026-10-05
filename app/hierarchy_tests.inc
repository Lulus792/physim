static bool hierarchy_test_start(app *a,const char *root,const char *mode) {
    if(!strcmp(mode,"hierarchy-v1")) return timeline_test_start(a,root,"v1");
    join(a->project_input,sizeof a->project_input,root,"project");
    a->example=!strcmp(mode,"hierarchy-c")?0:8;new_project(a);
    if(!a->loaded) return false;
    SDL_SetWindowSize(a->window,1080,740);a->preferences.sidebar_width=360;
    ps_dock_hide(&a->preferences.dock,PS_DOCK_LOG);a->scene_disclosure=NK_MAXIMIZED;
    a->dt=.01;a->project_settings_dirty=true;select_build_profile(a,1);return true;
}
static bool hierarchy_test_click(app *a,struct nk_rect r) {
    struct nk_rect panel=a->dock_panels[PS_DOCK_SIDEBAR];
    if(r.y<panel.y+32 || r.y+r.h>panel.y+panel.h-12) {
        test_plot_wheel(a,panel.x+panel.w/2,panel.y+panel.h/2,r.y<panel.y+32?1:-1);return false;
    }
    test_mouse(a,r,true);return true;
}
static void hierarchy_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    if(!strcmp(mode,"hierarchy-v1")) {
        if(*stage>=4 && *stage<=21) {
            bool flat=a->timeline_scenes && a->timeline.latest.scene.count>=3 && !a->built && !a->runner.running;
            for(uint32_t i=0;i<a->timeline.latest.scene.count;i++)
                flat &= !a->timeline.latest.scene.objects[i].parent_id && a->timeline.latest.scene.objects[i].shape<=PS_LABEL;
            if(!flat) { *code=1;a->quitting=true;return; }
        }
        timeline_test_frame(a,"v1",stage,code,capture);return;
    }
    static uint32_t root,sub,body,crc;static double held;
    bool ok=true;
    switch(*stage) {
    case 0: build_project(a);break;
    case 1: if(!idle(a) || a->library_thread) return;ok=a->built;start_run(a);break;
    case 2:
        if(!a->hello || a->simulation_time<.1) return;
        root=sub=body=PS_MAX_OBJECTS;
        for(uint32_t i=0;i<a->scene.count;i++) {
            if(a->scene.objects[i].id==100) root=i;
            if(a->scene.objects[i].id==101) sub=i;
            if(a->scene.objects[i].id==3) body=i;
        }
        ok=root<PS_MAX_OBJECTS && sub<PS_MAX_OBJECTS && body<PS_MAX_OBJECTS &&
           a->scene.objects[body].parent_id==101 && a->scene.objects[sub].parent_id==100;
        test_mouse(a,a->run_control_bounds[1],true);break;
    case 3: test_mouse(a,a->run_control_bounds[1],false);break;
    case 4: {
        if(!a->paused) return;
        held=a->simulation_time;ps_snapshot snapshot={.time=held,.count=a->channel_count,.scene=a->scene,.paused=true};
        memcpy(snapshot.values,a->values,sizeof snapshot.values);crc=timeline_test_crc(&snapshot);
        *capture="hierarchy-expanded.bmp";break;
    }
    case 5: if(!hierarchy_test_click(a,a->scene_expand_bounds[root])) return;break;
    case 6: test_mouse(a,a->scene_expand_bounds[root],false);break;
    case 7:
        ok=(a->scene_view.collapsed&(UINT32_C(1)<<root)) && a->scene_visibility_bounds[body].w==0;
        *capture="hierarchy-collapsed.bmp";
        if(!hierarchy_test_click(a,a->scene_visibility_bounds[root])) return;break;
    case 8: test_mouse(a,a->scene_visibility_bounds[root],false);break;
    case 9:
        for(uint32_t i=0;i<a->scene.count;i++) ok &= !ps_scene_view_visible(&a->scene_view,i);
        if(!hierarchy_test_click(a,a->scene_visibility_bounds[root])) return;break;
    case 10: test_mouse(a,a->scene_visibility_bounds[root],false);break;
    case 11:
        ok=ps_scene_view_visible(&a->scene_view,body) && !a->scene_view.hidden;
        if(!hierarchy_test_click(a,a->scene_expand_bounds[root])) return;break;
    case 12: test_mouse(a,a->scene_expand_bounds[root],false);break;
    case 13:
        ok=!(a->scene_view.collapsed&(UINT32_C(1)<<root)) && a->scene_visibility_bounds[body].w>0;
        if(!hierarchy_test_click(a,a->scene_expand_bounds[sub])) return;break;
    case 14: test_mouse(a,a->scene_expand_bounds[sub],false);break;
    case 15:
        ok=(a->scene_view.collapsed&(UINT32_C(1)<<sub)) && a->scene_visibility_bounds[body].w==0;break;
    case 16: case 17: case 18: test_scene_key(a,SDLK_N,SDL_KMOD_ALT,false);break;
    case 19:
        ok=a->scene_selected && a->scene_selected_id==3 && !(a->scene_view.collapsed&(UINT32_C(1)<<sub));
        if(!hierarchy_test_click(a,a->scene_visibility_bounds[sub])) return;break;
    case 20: test_mouse(a,a->scene_visibility_bounds[sub],false);break;
    case 21:
        ok=!ps_scene_view_visible(&a->scene_view,body) && ps_scene_view_visible(&a->scene_view,0);
        *capture="hierarchy-hidden-subgroup.bmp";
        if(!hierarchy_test_click(a,a->scene_visibility_bounds[sub])) return;break;
    case 22: test_mouse(a,a->scene_visibility_bounds[sub],false);break;
    case 23: test_mouse(a,a->run_control_bounds[4],true);break;
    case 24: test_mouse(a,a->run_control_bounds[4],false);break;
    case 25:
        if(!idle(a) || a->library_thread) return;
        ok=a->data.result==PS_EOF && a->data.total>2 && a->timeline.latest.scene.objects[body].parent_id==101;
        *capture="hierarchy-recorded.bmp";a->quitting=true;break;
    }
    if(*stage>=5 && *stage<=23) {
        ps_snapshot snapshot={.time=a->simulation_time,.count=a->channel_count,.scene=a->scene,.paused=a->paused};
        memcpy(snapshot.values,a->values,sizeof snapshot.values);
        ok &= a->simulation_time==held && timeline_test_crc(&snapshot)==crc;
    }
    if(!ok) { fprintf(stderr,"Hierarchy %s stage %d failed: hidden=%x collapsed=%x time=%.4f %s\n",mode,*stage,a->scene_view.hidden,a->scene_view.collapsed,a->simulation_time,a->status);*code=1;a->quitting=true; }
    if(a->quitting) printf("HIERARCHY %s SELF-TEST: %s\n",mode,*code?"FAILED":"PASSED");
    (*stage)++;
}
