/* Real paused C pendulum, camera overlay input and frame/viewport boundaries. */
static struct {unsigned settle,recorded,edge_slot;uint32_t selected;bool selection,edge_ink;float yaw,pitch,zoom;char paint[6][160];struct nk_rect edge_old;} scene_frame_design;
static bool scene_frame_start(app *a,const char *root){memset(&scene_frame_design,0,sizeof scene_frame_design);return inspector_design_start(a,root);}
static bool scene_frame_same(app *a){return inspector_design_same(a) && (!scene_frame_design.selection || (a->scene_selected && a->scene_selected_id==scene_frame_design.selected));}
static bool scene_frame_overlap(struct nk_rect a,struct nk_rect b){return a.w>0 && a.h>0 && b.w>0 && b.h>0 && a.x<b.x+b.w && a.x+a.w>b.x && a.y<b.y+b.h && a.y+a.h>b.y;}
static bool scene_frame_edge_pose(app *a) {
    const ps_scene *scene=display_scene(a);unsigned slot=scene->count;
    for(unsigned i=0;i<scene->count;i++)if(scene->objects[i].shape==PS_LABEL && scene_entry_enabled(a,i) && a->scene_label_bounds[i].w>0){slot=i;break;}
    if(slot==scene->count)return false;
    ps_mat4 transforms[PS_MAX_OBJECTS];ps_vec3 anchor;if(ps_scene_transforms(scene,transforms)!=PS_OK || ps_transform_point(transforms[slot],scene->objects[slot].a,&anchor)!=PS_OK)return false;
    struct nk_rect r=a->scene_viewport_bounds,field=a->laboratory_scene_select,label=a->scene_label_bounds[slot];
    a->camera_2d=false;a->orthographic=true;a->yaw=a->pitch=0;a->zoom=6;a->camera_target=PS_CAMERA_DEFAULT.target;
    ps_vec3 right,up;scene_camera_axes(a,&right,&up);ps_camera camera={a->yaw,a->pitch,a->zoom,a->camera_target,true,a->show_vectors,a->show_grid};
    float x0,y0,xr,yr,xu,yu;double aspect=r.w/r.h;
    if(!ps_graphics_project(&camera,anchor,aspect,&x0,&y0))return false;
    camera.target=ps_vadd(a->camera_target,right);if(!ps_graphics_project(&camera,anchor,aspect,&xr,&yr))return false;
    camera.target=ps_vadd(a->camera_target,up);if(!ps_graphics_project(&camera,anchor,aspect,&xu,&yu))return false;
    double dx=(field.x+4-7-r.x)/r.w-x0,dy=(field.y+4+label.h-r.y)/r.h-y0;
    double ax=xr-x0,ay=yr-y0,bx=xu-x0,by=yu-y0,det=ax*by-ay*bx;if(fabs(det)<1e-10)return false;
    a->camera_target=ps_vadd(a->camera_target,ps_vadd(ps_vscale(right,(dx*by-dy*bx)/det),ps_vscale(up,(ax*dy-ay*dx)/det)));
    scene_frame_design.edge_slot=slot;scene_frame_design.edge_old=nk_rect(field.x+4,field.y+4,label.w,label.h);
    struct nk_rect empty[1]={{0}};return place_label(&scene_frame_design.edge_old,r,empty,0) && scene_frame_overlap(scene_frame_design.edge_old,field);
}
static void scene_frame_draw(app *a) {
    memset(scene_frame_design.paint,0,sizeof scene_frame_design.paint);scene_frame_design.edge_ink=false;const struct nk_command *command;
    const struct nk_rect rects[]={a->laboratory_scene_label,a->laboratory_scene_status,a->laboratory_scene_select,a->laboratory_scene_view,a->laboratory_scene_hint,a->laboratory_scene_reset};
    const struct nk_user_font *fonts[]={&a->laboratory_caption_font,&a->workspace_status_font,&a->workspace_status_font,&a->laboratory_scene_font,&a->laboratory_scene_font,&a->workspace_status_font};
    nk_foreach(command,a->ui)if(command->type==NK_COMMAND_TEXT){const struct nk_command_text *t=(const struct nk_command_text*)command;for(unsigned i=0;i<6;i++)if(t->font==fonts[i] && camera_design_contains(rects[i],t))camera_design_append(scene_frame_design.paint[i],sizeof scene_frame_design.paint[i],t);
        unsigned slot=scene_frame_design.edge_slot;if(slot<display_scene(a)->count){const char *text=display_scene(a)->objects[slot].text;if(t->length==(int)strlen(text) && !memcmp(t->string,text,(size_t)t->length) && camera_design_contains(a->scene_label_bounds[slot],t))scene_frame_design.edge_ink=true;}}
}
static void scene_frame_record(app *a,const char *kind,unsigned bit,const char **capture) {
    if(scene_frame_design.recorded&bit)return;
    scene_frame_design.recorded|=bit;
    printf("SCENE FRAME kind=%s variant=%s size=%u preserved=%d mode=%d projection=%d plane=%u selected=%u yaw=%.9g pitch=%.9g zoom=%.9g",kind,inspector_design_variant(),a->preferences.ui_size,scene_frame_same(a),a->camera_2d,a->orthographic,a->camera_plane,a->scene_selected_id,a->yaw,a->pitch,a->zoom);
    camera_design_rect("frame",a->laboratory_scene_frame);camera_design_rect("viewport",a->scene_viewport_bounds);camera_design_rect("footer",a->laboratory_scene_footer);puts("");
    const struct nk_rect rects[]={a->laboratory_scene_label,a->laboratory_scene_status,a->laboratory_scene_select,a->laboratory_scene_view,a->laboratory_scene_hint,a->laboratory_scene_reset};
    const char *names[]={"label","status","select","view","hint","reset"};
    for(unsigned i=0;i<6;i++){printf("SCENE TEXT role=%s",names[i]);camera_design_rect("rect",rects[i]);printf(" paint=");camera_design_hex(scene_frame_design.paint[i]);puts("");}
    if(!strcmp(kind,"label-edge")){printf("SCENE EDGE slot=%u counterfactual-unseeded=1 glyph=%d color=%08x",scene_frame_design.edge_slot,scene_frame_design.edge_ink,display_scene(a)->objects[scene_frame_design.edge_slot].color);camera_design_rect("old",scene_frame_design.edge_old);camera_design_rect("actual",a->scene_label_bounds[scene_frame_design.edge_slot]);puts("");}
    static char path[100];snprintf(path,sizeof path,"scene-%s-%s.png",inspector_design_variant(),kind);*capture=path;
}
static void scene_frame_click(app *a,struct nk_rect r,bool down){test_mouse(a,r,down);}
static bool scene_frame_default(const app *a){return !a->camera_2d && a->yaw==PS_CAMERA_DEFAULT.yaw && a->pitch==PS_CAMERA_DEFAULT.pitch && a->zoom==PS_CAMERA_DEFAULT.distance && a->orthographic==PS_CAMERA_DEFAULT.orthographic && !memcmp(&a->camera_target,&PS_CAMERA_DEFAULT.target,sizeof a->camera_target);}
static void scene_frame_step(app *a,int *stage,int *code,const char **capture) {
    if(*stage<4){inspector_design_frame(a,stage,code,capture);return;}
    bool ok=true;
    switch(*stage) {
    case 4:
        inspector_design_frame(a,stage,code,capture);ps_dock_hide(&a->preferences.dock,PS_DOCK_SIDEBAR);
        a->camera_2d=false;a->orthographic=false;a->camera_plane=0;a->yaw=.31f;a->pitch=-.28f;a->zoom=6;a->camera_target=PS_CAMERA_DEFAULT.target;
        scene_frame_design.recorded=scene_frame_design.settle=0;scene_frame_design.selection=false;scene_frame_design.edge_slot=PS_MAX_OBJECTS;return;
    case 5:if(scene_frame_design.settle++<3)return;test_window_key_mod(a->window,SDLK_N,SDL_KMOD_ALT);scene_frame_design.settle=0;break;
    case 6:
        if(scene_frame_design.settle++<3)return;
        ok=a->scene_selected;if(!ok)break;scene_frame_design.selection=true;scene_frame_design.selected=a->scene_selected_id;
        scene_frame_record(a,"normal",1,capture);scene_frame_click(a,a->laboratory_scene_select,true);break;
    case 7:scene_frame_click(a,a->laboratory_scene_select,false);scene_frame_design.settle=0;break;
    case 8:
        if(scene_frame_design.settle++<3)return;
        ok=a->laboratory_scene_menu;if(!ok)break;scene_frame_record(a,"menu",2,capture);test_window_key_mod(a->window,SDLK_ESCAPE,0);scene_frame_design.settle=0;break;
    case 9:
        if(scene_frame_design.settle++<3)return;
        ok=!a->laboratory_scene_menu && a->yaw==.31f && a->pitch==-.28f && a->zoom==6;
        if(ok && !inspector_design_focus(a,"Kameraansicht"))return;
        if(ok)test_window_key_mod(a->window,SDLK_RETURN,0);
        scene_frame_design.settle=0;break;
    case 10:
        if(scene_frame_design.settle++<3)return;
        ok=a->laboratory_scene_menu;if(!ok)break;if(!inspector_design_focus(a,"Oben"))return;test_window_key_mod(a->window,SDLK_RETURN,0);scene_frame_design.settle=0;break;
    case 11:
        if(scene_frame_design.settle++<3)return;
        ok=!a->laboratory_scene_menu && !a->camera_2d && a->orthographic && a->yaw==0 && a->pitch==(float)PS_PI/2;
        if(ok && !inspector_design_focus(a,"Kameraansicht"))return;
        scene_frame_record(a,"preset",4,capture);break;
    case 12:test_window_key_mod(a->window,SDLK_TAB,0);scene_frame_design.settle=0;*stage=23;return;
    case 13:
        if(scene_frame_design.settle++<3)return;
        ok=scene_frame_default(a);scene_frame_record(a,"reset",8,capture);scene_frame_design.yaw=a->yaw;scene_frame_design.pitch=a->pitch;camera_test_button(a,true);break;
    case 14:test_plot_motion(a,a->scene_viewport_bounds.x+a->scene_viewport_bounds.w/2+30,a->scene_viewport_bounds.y+a->scene_viewport_bounds.h/2+20);break;
    case 15:camera_test_button(a,false);scene_frame_design.settle=0;break;
    case 16:
        if(scene_frame_design.settle++<3)return;
        ok=a->yaw!=scene_frame_design.yaw && a->pitch!=scene_frame_design.pitch;scene_frame_record(a,"orbit",16,capture);test_window_key_mod(a->window,SDLK_2,SDL_KMOD_ALT);scene_frame_design.settle=0;break;
    case 17:
        if(scene_frame_design.settle++<3)return;
        ok=a->camera_2d;scene_frame_record(a,"2d",32,capture);scene_frame_click(a,a->laboratory_scene_reset,true);break;
    case 18:scene_frame_click(a,a->laboratory_scene_reset,false);scene_frame_design.settle=0;break;
    case 19:
        if(scene_frame_design.settle++<3)return;
        ok=scene_frame_default(a);scene_frame_record(a,"pointer-reset",64,capture);scene_frame_design.zoom=a->zoom;
        {struct nk_rect r=a->laboratory_scene_reset;test_plot_wheel(a,r.x+r.w/2,r.y+r.h/2,2);}scene_frame_design.settle=0;break;
    case 20:
        if(scene_frame_design.settle++<3)return;
        ok=a->zoom==scene_frame_design.zoom;scene_frame_record(a,"footer",128,capture);
        break;
    case 21:ok=scene_frame_edge_pose(a);scene_frame_design.settle=0;break;
    case 22:
        if(scene_frame_design.settle++<3)return;
        {struct nk_rect label=a->scene_label_bounds[scene_frame_design.edge_slot];ok=label.w>0 && scene_frame_design.edge_ink && !scene_frame_overlap(label,a->laboratory_scene_select) && !scene_frame_overlap(label,a->laboratory_scene_label) && !scene_frame_overlap(label,a->laboratory_scene_status);}
        scene_frame_record(a,"label-edge",256,capture);
        printf("SCENE ACTIONS variant=%s pointer-menu-no-pick=1 escape-return=1 native-preset=1 native-reset=1 orbit=1 mode-2d=1 pointer-reset=1 footer-wheel-no-zoom=1 tab-order=1 label-avoidance=1 source-and-run=1\n",inspector_design_variant());
        if(ok){if(++inspector_design.variant<6){*stage=4;return;}*stage=30;return;}
        break;
    case 23:
        if(scene_frame_design.settle++<3)return;
        ok=nk_sdl_accessibility_is_focused(a->ui,NULL,"Kamera auf Standardansicht zurücksetzen");
        if(ok)test_window_key_mod(a->window,SDLK_TAB,SDL_KMOD_SHIFT);
        scene_frame_design.settle=0;break;
    case 24:
        if(scene_frame_design.settle++<3)return;
        ok=nk_sdl_accessibility_is_focused(a->ui,NULL,"Kameraansicht");
        if(ok)test_window_key_mod(a->window,SDLK_TAB,0);
        scene_frame_design.settle=0;break;
    case 25:
        if(scene_frame_design.settle++<3)return;
        ok=nk_sdl_accessibility_is_focused(a->ui,NULL,"Kamera auf Standardansicht zurücksetzen");if(ok){test_window_key_mod(a->window,SDLK_RETURN,0);scene_frame_design.settle=0;*stage=13;return;}
        break;
    case 30:{uint32_t crc=0;ok=timeline_design_run_crc(a->last_run,&crc) && crc==inspector_design.run_crc;command(a,PS_MSG_STOP);a->stop_at=ps_clock();break;}
    case 31:if(!idle(a) || a->library_thread)return;puts("SCENE FRAME SELF-TEST: PASSED");a->quitting=true;break;
    }
    if(!scene_frame_same(a) && *stage<31)ok=false;
    if(!ok){fprintf(stderr,"SCENE FRAME FAILED stage=%d variant=%u selected=%d/%u mode=%d yaw=%.9g pitch=%.9g\n",*stage,inspector_design.variant,a->scene_selected,a->scene_selected_id,a->camera_2d,a->yaw,a->pitch);*code=1;a->quitting=true;}else (*stage)++;
}
