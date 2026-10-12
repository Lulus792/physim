/* Real visibility widgets plus submitted geometry and picking, without editing
 * scene objects, recorded samples or simulation time. */
static int visibility_test_pick(app *a,uint32_t slot) {
    const ps_object *o=&display_scene(a)->objects[slot];
    ps_camera camera={a->yaw,a->pitch,a->zoom,a->camera_target,a->orthographic,a->show_vectors,a->show_grid};
    float x,y;
    if(!ps_graphics_project(&camera,o->a,(double)a->scene_viewport_bounds.w/a->scene_viewport_bounds.h,&x,&y))return -2;
    return ps_graphics_pick(a->graphics,x,y);
}
static void scene_visibility_test_frame(app *a,int *stage,int *code,const char **capture) {
    static ps_scene original;static double values[PS_MAX_CHANNELS],time;static size_t vertices,hidden_vertices;
    bool ok=true;ps_scene_render_stats stats={0};
    if(*stage==0) {
        if(a->data.thread || a->library_thread)return;
        timeline_clear(a);timeline_live(a);a->tab=1;
        a->scene_disclosure=a->view_disclosure=NK_MAXIMIZED;a->camera_disclosure=NK_MINIMIZED;
        a->preferences.sidebar_width=360;ps_dock_hide(&a->preferences.dock,PS_DOCK_LOG);
        a->yaw=a->pitch=0;a->zoom=6;a->camera_target=ps_v3(0,0,0);a->orthographic=true;a->show_grid=false;
        a->show_vectors=a->show_paths=a->show_points=a->show_labels=true;
        a->scene=(ps_scene){0};
        ok=ps_scene_add_id(&a->scene,1,PS_SPHERE,ps_v3(-1,0,0),ps_v3(0,0,0),.2,0x53dec2ff)==PS_OK &&
           ps_scene_add_id(&a->scene,2,PS_ARROW,ps_v3(0,0,0),ps_v3(.4,.4,0),.04,0xf2c572ff)==PS_OK;
        ps_vec3 points[]={ps_v3(-1,-.7,0),ps_v3(0,-.4,0),ps_v3(1,-.7,0)};
        ok &= ps_scene_polyline_id(&a->scene,3,points,3,.03,0xffffffff)==PS_OK &&
            ps_scene_add_id(&a->scene,4,PS_POINT,ps_v3(1,.2,0),ps_v3(0,0,0),.08,0xffffffff)==PS_OK &&
            ps_scene_label_id(&a->scene,5,ps_v3(0,.9,0),"Messpunkt α",0xffffffff)==PS_OK;
        ps_scene_view_sync(&a->scene_view,&a->scene);
        original=a->scene;memcpy(values,a->values,sizeof values);time=a->simulation_time;
    } else if(*stage==1) {
        ok=ps_graphics_scene_stats(a->graphics,&stats) && stats.vertices>0 && visibility_test_pick(a,0)==0;
        vertices=stats.vertices;*capture="visibility-all.bmp";
    } else if(*stage<26) {
        unsigned control=(unsigned)(*stage-2)/6,step=(unsigned)(*stage-2)%6;
        struct nk_rect bounds[]={a->display_vector_bounds,a->display_path_bounds,a->display_points_bounds,a->display_labels_bounds};
        bool enabled[]={a->show_vectors,a->show_paths,a->show_points,a->show_labels};
        if(step==0 || step==3) {
            if(step==3) {
                ok=!enabled[control] && !scene_entry_enabled(a,control+1) && scene_entry_enabled(a,0);
                ok &= ps_graphics_scene_stats(a->graphics,&stats) &&
                    (control==3?stats.vertices==vertices:stats.vertices<vertices);
                if(control==3)ok &= a->scene_label_bounds[4].w==0;
                static const char *names[]={"visibility-no-vectors.bmp","visibility-no-paths.bmp","visibility-no-points.bmp","visibility-no-labels.bmp"};
                *capture=names[control];
            }
            if(!camera_test_click(a,bounds[control]))return;
        } else if(step==1 || step==4)test_mouse(a,camera_test_pressed,false);
        /* Steps 2/5 let the real widget release reach the renderer. */
    } else switch(*stage) {
    case 26:
        ok=a->show_vectors && a->show_paths && a->show_points && a->show_labels &&
            ps_graphics_scene_stats(a->graphics,&stats) && stats.vertices==vertices;
        if(!camera_test_click(a,a->scene_visibility_bounds[0]))return;break;
    case 27:test_mouse(a,camera_test_pressed,false);break;
    case 28:break;
    case 29:
        ok=!scene_entry_enabled(a,0) && visibility_test_pick(a,0)==-1 && ps_graphics_scene_stats(a->graphics,&stats) && stats.vertices<vertices;
        hidden_vertices=stats.vertices;*capture="visibility-hidden-object.bmp";break;
    case 30: {
        ps_object object=a->scene.objects[0];a->scene.objects[0]=a->scene.objects[3];a->scene.objects[3]=object;
        ps_scene_view_sync(&a->scene_view,&a->scene);original=a->scene;break;
    }
    case 31:break;
    case 32:
        ok=a->scene.objects[3].id==1 && a->scene_view.hidden==8 && !scene_entry_enabled(a,3) && scene_entry_enabled(a,0) &&
            ps_graphics_scene_stats(a->graphics,&stats) && stats.vertices==hidden_vertices && visibility_test_pick(a,3)==-1;
        if(!camera_test_click(a,a->display_all_bounds))return;break;
    case 33:test_mouse(a,camera_test_pressed,false);break;
    case 34:break;
    case 35:ok=!a->scene_view.hidden && visibility_test_pick(a,3)==3 && ps_graphics_scene_stats(a->graphics,&stats) && stats.vertices==vertices;
        test_scene_key(a,SDLK_N,SDL_KMOD_ALT,false);break;
    case 36:ok=a->scene_selected && a->scene_selected_id==4;test_scene_key(a,SDLK_H,SDL_KMOD_ALT,false);break;
    case 37:break;
    case 38:ok=a->scene_view.hidden==1 && !scene_entry_enabled(a,0) && !a->scene_selected;
        if(!camera_test_click(a,a->display_all_bounds))return;break;
    case 39:test_mouse(a,camera_test_pressed,false);break;
    case 40:break;
    case 41:ok=!a->scene_view.hidden && ps_graphics_scene_stats(a->graphics,&stats) && stats.vertices==vertices;
        *capture="visibility-restored.bmp";break;
    default:printf("SCENE VISIBILITY SELF-TEST: %s\n",*code?"FAILED":"PASSED");a->quitting=true;return;
    }
    if(*stage>0)ok &= !memcmp(&original,&a->scene,sizeof original) && !memcmp(values,a->values,sizeof values) && time==a->simulation_time;
    if(!ok){fprintf(stderr,"Visibility stage %d failed: hidden=%x vertices=%zu baseline=%zu\n",*stage,a->scene_view.hidden,stats.vertices,vertices);*code=1;a->quitting=true;}
    (*stage)++;
}
