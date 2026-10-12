/* Real SDL mouse/key paths; a camera operation must preserve the scene/data. */
static void camera_test_button(app *a,bool down) {
    SDL_Event event={0};event.type=down?SDL_EVENT_MOUSE_BUTTON_DOWN:SDL_EVENT_MOUSE_BUTTON_UP;
    event.button.windowID=SDL_GetWindowID(a->window);event.button.which=PS_TEST_MOUSE_ID;
    event.button.button=SDL_BUTTON_RIGHT;event.button.down=down;
    event.button.x=a->scene_viewport_bounds.x+a->scene_viewport_bounds.w/2;
    event.button.y=a->scene_viewport_bounds.y+a->scene_viewport_bounds.h/2;
    SDL_PushEvent(&event);
}
static struct nk_rect camera_test_pressed;
static bool camera_test_click(app *a,struct nk_rect r) {
    static unsigned settle;
    if(settle){settle--;return false;}
    if(a->ui->input.mouse.scroll_delta.y!=0)return false; /* Render queued scrolling before using its old bounds. */
    struct nk_rect panel=a->dock_panels[PS_DOCK_INSPECTOR],sidebar=a->dock_panels[PS_DOCK_SIDEBAR];
    if(r.x>=sidebar.x && r.x+r.w<=sidebar.x+sidebar.w)panel=sidebar;
    if(r.h<=0) return false;
    if(r.y<panel.y+32 || r.y+r.h>panel.y+panel.h-12) {
        test_plot_wheel(a,panel.x+panel.w/2,panel.y+panel.h/2,r.y<panel.y+32?1:-1);settle=2;return false;
    }
    camera_test_pressed=r;test_mouse(a,r,true);return true;
}
static bool camera_test_depth(app *a,bool equal) {
    ps_camera camera={a->yaw,a->pitch,a->zoom,a->camera_target,a->orthographic,a->show_vectors,a->show_grid};
    ps_vec3 first=ps_v3(1,0,0),second=ps_v3(1,0,.5);
    if(a->camera_plane==1){first=ps_v3(0,0,1);second=ps_v3(.5,0,1);}
    if(a->camera_plane==2){first=ps_v3(1,0,0);second=ps_v3(1,.5,0);}
    float x[2],y[2];double aspect=a->scene_viewport_bounds.w/a->scene_viewport_bounds.h;
    if(!ps_graphics_project(&camera,first,aspect,&x[0],&y[0]) ||
       !ps_graphics_project(&camera,second,aspect,&x[1],&y[1]))return false;
    double difference=fabs(x[0]-x[1])+fabs(y[0]-y[1]);
    return equal?difference<1e-6:difference>1e-4;
}
static void camera_plane_test_frame(app *a,int *stage,int *code,const char **capture) {
    static ps_scene scene;static ps_vec3 target;static double values[PS_MAX_CHANNELS],time;static float zoom;
    bool ok=true;
    switch(*stage) {
    case 0:
        if(a->data.thread || a->library_thread) return;
        a->tab=1;a->scene_disclosure=NK_MINIMIZED;a->view_disclosure=NK_MAXIMIZED;a->camera_disclosure=NK_MAXIMIZED;
        a->preferences.sidebar_width=360;ps_dock_hide(&a->preferences.dock,PS_DOCK_LOG);
        a->scene=(ps_scene){0};
        ps_scene_add_id(&a->scene,1,PS_SPHERE,ps_v3(-.5,0,0),ps_v3(0,0,0),.2,0x53dec2ff);
        ps_scene_add_id(&a->scene,2,PS_SPHERE,ps_v3(.5,0,.5),ps_v3(0,0,0),.2,0xf2c572ff);
        ps_scene_add_id(&a->scene,3,PS_ARROW,ps_v3(0,0,0),ps_v3(0,1,0),.02,0xffffffff);
        timeline_clear(a);timeline_live(a);
        scene=a->scene;memcpy(values,a->values,sizeof values);time=a->simulation_time;break;
    case 1: if(!camera_test_click(a,a->camera_mode_bounds)) return;
        break;
    case 2: test_mouse(a,camera_test_pressed,false);break;
    case 3:
        ok=a->camera_2d && a->camera_plane==0 && a->yaw==0 && a->pitch==0 && a->orthographic && camera_test_depth(a,true);
        *capture="camera-2d-xy-orthographic.bmp";
        if(!camera_test_click(a,a->camera_projection_bounds)) return;
        break;
    case 4:test_mouse(a,camera_test_pressed,false);break;
    case 5:
        ok=a->camera_2d && !a->orthographic && camera_test_depth(a,false);
        *capture="camera-2d-xy-perspective.bmp";target=a->camera_target;
        test_scene_key(a,SDLK_RIGHT,SDL_KMOD_ALT,false);break;
    case 6:
        ok=a->camera_target.x>target.x && a->yaw==0 && a->pitch==0;
        target=a->camera_target;test_scene_key(a,SDLK_UP,SDL_KMOD_ALT|SDL_KMOD_SHIFT,false);break;
    case 7:
        ok=a->camera_target.y>target.y && a->yaw==0 && a->pitch==0;
        test_plot_motion(a,a->scene_viewport_bounds.x+a->scene_viewport_bounds.w/2,
                           a->scene_viewport_bounds.y+a->scene_viewport_bounds.h/2);break;
    case 8: target=a->camera_target;camera_test_button(a,true);break;
    case 9: test_plot_motion(a,a->scene_viewport_bounds.x+a->scene_viewport_bounds.w/2+30,
                              a->scene_viewport_bounds.y+a->scene_viewport_bounds.h/2+20);break;
    case 10: break; /* Mouse movement is consumed by the following render pass. */
    case 11: ok=ps_vlength(ps_vsub(target,a->camera_target))>0 && a->yaw==0 && a->pitch==0;camera_test_button(a,false);break;
    case 12:zoom=a->zoom;test_plot_wheel(a,a->scene_viewport_bounds.x+a->scene_viewport_bounds.w/2,
                                         a->scene_viewport_bounds.y+a->scene_viewport_bounds.h/2,2);break;
    case 13: break; /* Let viewport consume the queued wheel event. */
    case 14:ok=a->zoom<zoom && a->yaw==0 && a->pitch==0;
        if(!camera_test_click(a,a->camera_preset_bounds[2])) return;
        break;
    case 15:test_mouse(a,camera_test_pressed,false);break;
    case 16:ok=a->camera_2d && a->camera_plane==1 && a->yaw==(float)PS_PI/2 && a->pitch==0 && a->orthographic && camera_test_depth(a,true);
        *capture="camera-2d-yz.bmp";target=a->camera_target;test_scene_key(a,SDLK_RIGHT,SDL_KMOD_ALT,false);break;
    case 17:ok=a->camera_target.z<target.z && a->camera_target.x==target.x && a->camera_target.y==target.y;
        if(!camera_test_click(a,a->camera_preset_bounds[3])) return;
        break;
    case 18:test_mouse(a,camera_test_pressed,false);break;
    case 19:ok=a->camera_2d && a->camera_plane==2 && a->yaw==0 && a->pitch==(float)PS_PI/2 && camera_test_depth(a,true);
        *capture="camera-2d-xz.bmp";target=a->camera_target;test_scene_key(a,SDLK_UP,SDL_KMOD_ALT,false);break;
    case 20:ok=a->camera_target.z<target.z && a->camera_target.x==target.x && a->camera_target.y==target.y;
        test_scene_key(a,SDLK_2,SDL_KMOD_ALT,true);break;
    case 21:ok=a->camera_2d;test_scene_key(a,SDLK_2,SDL_KMOD_ALT,false);break;
    case 22:ok=!a->camera_2d;test_scene_key(a,SDLK_2,SDL_KMOD_ALT|SDL_KMOD_CTRL,false);break;
    case 23:ok=!a->camera_2d;test_scene_key(a,SDLK_2,SDL_KMOD_ALT,false);break;
    case 24:ok=a->camera_2d && a->camera_plane==2 && a->pitch==(float)PS_PI/2;test_scene_key(a,SDLK_HOME,SDL_KMOD_ALT,false);break;
    case 25:ok=!a->camera_2d && a->yaw==PS_CAMERA_DEFAULT.yaw && a->pitch==PS_CAMERA_DEFAULT.pitch && a->orthographic==PS_CAMERA_DEFAULT.orthographic;
        a->tab=0;test_scene_key(a,SDLK_2,SDL_KMOD_ALT,false);break;
    case 26:ok=!a->camera_2d;a->tab=2;test_scene_key(a,SDLK_2,SDL_KMOD_ALT,false);break;
    case 27:ok=!a->camera_2d;break;
    default:printf("CAMERA PLANE SELF-TEST: %s\n",*code?"FAILED":"PASSED");a->quitting=true;return;
    }
    if(*stage>1) {
        ps_scene_render_stats stats;
        ok &= ps_graphics_scene_stats(a->graphics,&stats) && stats.vertices>4608 && display_scene(a)->count==3;
    }
    if(*stage>0)ok &= !memcmp(&scene,&a->scene,sizeof scene) && !memcmp(values,a->values,sizeof values) && time==a->simulation_time;
    if(!ok){fprintf(stderr,"Camera plane stage %d failed: mode=%d plane=%u yaw=%.6g pitch=%.6g\n",*stage,a->camera_2d,a->camera_plane,a->yaw,a->pitch);*code=1;a->quitting=true;}
    (*stage)++;
}
