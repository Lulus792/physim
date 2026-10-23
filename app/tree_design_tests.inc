/* Real pendulum hierarchy: pointer actions, keyboard reveal and shared camera. */
static struct {unsigned settle,steps,recorded;uint32_t root,body;size_t vertices;char paint[PS_MAX_OBJECTS][256];} tree_design;
static bool tree_design_start(app *a,const char *root) {memset(&tree_design,0,sizeof tree_design);return inspector_design_start(a,root);}
static void tree_design_draw(app *a) {
    memset(tree_design.paint,0,sizeof tree_design.paint);const struct nk_command *command;
    nk_foreach(command,a->ui)if(command->type==NK_COMMAND_TEXT) {
        const struct nk_command_text *text=(const struct nk_command_text*)command;
        if(text->font!=&a->laboratory_caption_font && text->font!=&a->workbench_fonts[2])continue;
        for(uint32_t i=0;i<display_scene(a)->count;i++) {
            struct nk_rect r=a->laboratory_tree_titles[i];if(r.w<=0 || text->x<r.x-1 || text->x>=r.x+r.w || text->y<r.y-1 || text->y>=r.y+r.h)continue;
            size_t at=strlen(tree_design.paint[i]),count=NK_MIN((size_t)text->length,sizeof tree_design.paint[i]-at-1);memcpy(tree_design.paint[i]+at,text->string,count);tree_design.paint[i][at+count]=0;
        }
    }
}
static const char *tree_design_capture(const char *kind) {static char file[96];snprintf(file,sizeof file,"tree-%s-%s.png",inspector_design_variant(),kind);return file;}
static void tree_design_hex(const char *text) {for(const unsigned char *at=(const unsigned char*)text;*at;at++)printf("%02x",*at);}
static void tree_design_log(app *a,const char *kind) {
    struct nk_window *window=nk_window_find(a->ui,"sidebar");
    printf("TREE DESIGN kind=%s variant=%s size=%u preserved=%d selected=%u scroll=%u",kind,inspector_design_variant(),a->preferences.ui_size,inspector_design_same(a),a->scene_selected_id,window?window->scrollbar.y:0);
    struct nk_rect b[]={a->dock_headers[PS_DOCK_SIDEBAR],a->laboratory_tree_body,a->laboratory_tree_footer,a->laboratory_tree_options,a->laboratory_tree_camera_mode,a->camera_mode_bounds};const char *names[]={"header","body","footer","options","left-mode","right-mode"};
    for(unsigned i=0;i<SDL_arraysize(b);i++)printf(" %s=%.4f,%.4f,%.4f,%.4f",names[i],b[i].x,b[i].y,b[i].w,b[i].h);
    puts("");
    for(uint32_t i=0;i<display_scene(a)->count;i++) {
        const ps_object *o=&display_scene(a)->objects[i];struct nk_rect r=a->laboratory_tree_rows[i],t=a->laboratory_tree_titles[i];
        printf("TREE ROW id=%u shape=%u row=%.4f,%.4f,%.4f,%.4f title=%.4f,%.4f,%.4f,%.4f label=",o->id,o->shape,r.x,r.y,r.w,r.h,t.x,t.y,t.w,t.h);tree_design_hex(o->text);printf(" paint=");tree_design_hex(tree_design.paint[i]);puts("");
    }
    for(unsigned i=0;i<3;i++){struct nk_rect r=a->laboratory_tree_filters[i];printf("TREE FILTER index=%u rect=%.4f,%.4f,%.4f,%.4f\n",i,r.x,r.y,r.w,r.h);}
}
static void tree_design_record(app *a,const char *kind,unsigned bit,const char **capture) {
    if(tree_design.recorded&bit)return;
    tree_design.recorded|=bit;tree_design_log(a,kind);*capture=tree_design_capture(kind);
}
static bool tree_design_same_vertices(app *a,bool equal) {ps_scene_render_stats stats={0};return ps_graphics_scene_stats(a->graphics,&stats) && (equal?stats.vertices==tree_design.vertices:stats.vertices<tree_design.vertices);}
static void tree_design_frame(app *a,int *stage,int *code,const char **capture) {
    if(*stage<4){inspector_design_frame(a,stage,code,capture);return;}bool ok=true;
    switch(*stage) {
    case 4: {
        inspector_design_frame(a,stage,code,capture);a->preferences.sidebar_width=inspector_design.variant<2?190:150;ps_dock_reveal_outer(&a->preferences.dock,PS_DOCK_SIDEBAR,PS_DOCK_LEFT);
        a->view_disclosure=a->camera_disclosure=NK_MINIMIZED;a->scene_view.hidden=a->scene_view.collapsed=0;a->show_vectors=a->show_paths=a->show_points=a->show_labels=true;a->show_grid=true;a->scene_selected=false;a->laboratory_tree_reveal=false;
        a->yaw=PS_CAMERA_DEFAULT.yaw;a->pitch=PS_CAMERA_DEFAULT.pitch;a->zoom=PS_CAMERA_DEFAULT.distance;a->camera_target=PS_CAMERA_DEFAULT.target;a->orthographic=PS_CAMERA_DEFAULT.orthographic;a->camera_2d=false;a->camera_plane=0;
        struct nk_window *window=nk_window_find(a->ui,"sidebar");if(window)window->scrollbar.y=0;
        tree_design.settle=tree_design.steps=tree_design.recorded=0;tree_design.root=tree_design.body=PS_MAX_OBJECTS;
        for(uint32_t i=0;i<display_scene(a)->count;i++){if(display_scene(a)->objects[i].id==100)tree_design.root=i;if(display_scene(a)->objects[i].id==3)tree_design.body=i;}
        if(!inspector_design.variant)*stage=28;
        return;
    }
    case 5: {
        if(tree_design.settle++<3)return;
        ps_scene_render_stats stats={0};ok=tree_design.root<PS_MAX_OBJECTS && tree_design.body<PS_MAX_OBJECTS && ps_graphics_scene_stats(a->graphics,&stats);tree_design.vertices=stats.vertices;
        tree_design_record(a,"expanded",1,capture);tree_design.settle=0;break;
    }
    case 6:
        if(!a->scene_selected || a->scene_selected_id!=11){ok=tree_design.steps++<PS_MAX_OBJECTS;if(ok){test_window_key_mod(a->window,SDLK_N,SDL_KMOD_ALT);return;}}
        if(tree_design.settle++<3)return;
        ok=ok && a->scene_selected && a->scene_selected_id==11 && inspector_design_same(a);tree_design_record(a,"keyboard",2,capture);break;
    case 7:if(!camera_test_click(a,a->laboratory_tree_titles[tree_design.root]))return;
        break;
    case 8:test_mouse(a,camera_test_pressed,false);tree_design.settle=0;break;
    case 9:
        if(tree_design.settle++<3)return;
        ok=a->scene_selected_id==100 && tree_design_same_vertices(a,true);if(!ok)break;
        if(!camera_test_click(a,a->scene_visibility_bounds[tree_design.root]))return;
        break;
    case 10:test_mouse(a,camera_test_pressed,false);tree_design.settle=0;break;
    case 11:
        if(tree_design.settle++<3)return;
        ok=!scene_entry_enabled(a,tree_design.body) && tree_design_same_vertices(a,false) && inspector_design_same(a);tree_design_record(a,"hidden",4,capture);if(!ok)break;
        if(!camera_test_click(a,a->scene_visibility_bounds[tree_design.root]))return;
        break;
    case 12:test_mouse(a,camera_test_pressed,false);tree_design.settle=0;break;
    case 13:
        if(tree_design.settle++<3)return;
        ok=scene_entry_enabled(a,tree_design.body) && tree_design_same_vertices(a,true);if(!ok)break;
        if(!camera_test_click(a,a->scene_expand_bounds[tree_design.root]))return;
        break;
    case 14:test_mouse(a,camera_test_pressed,false);tree_design.settle=0;break;
    case 15:
        if(tree_design.settle++<3)return;
        ok=!a->scene_visibility_bounds[tree_design.body].w && tree_design_same_vertices(a,true);tree_design_record(a,"collapsed",8,capture);if(!ok)break;
        if(!camera_test_click(a,a->laboratory_tree_filters[1]))return;
        break;
    case 16:test_mouse(a,camera_test_pressed,false);tree_design.settle=0;break;
    case 17:
        if(tree_design.settle++<3)return;
        ok=!a->show_vectors && tree_design_same_vertices(a,false);if(!ok)break;
        if(SDL_GetKeyboardFocus()!=a->window || !nk_sdl_accessibility_is_focused(a->ui,NULL,"Vektoren")){nk_sdl_accessibility_request_focus(a->ui,NULL,"Vektoren");return;}
        tree_design_record(a,"focus",16,capture);test_window_key_mod(a->window,SDLK_RETURN,0);tree_design.settle=0;break;
    case 18:
        if(tree_design.settle++<3)return;
        ok=a->show_vectors && tree_design_same_vertices(a,true);if(!ok)break;
        if(!camera_test_click(a,a->laboratory_tree_options))return;
        break;
    case 19:test_mouse(a,camera_test_pressed,false);tree_design.settle=0;break;
    case 20:
        if(tree_design.settle++<3)return;
        ok=a->view_disclosure==NK_MAXIMIZED && a->camera_disclosure==NK_MAXIMIZED;if(!ok)break;
        if(!camera_test_click(a,a->laboratory_tree_camera_mode))return;
        break;
    case 21:test_mouse(a,camera_test_pressed,false);tree_design.settle=0;break;
    case 22:
        if(tree_design.settle++<3)return;
        ok=a->camera_2d && a->orthographic && inspector_design_same(a);a->laboratory_groups[3]=NK_MAXIMIZED;break;
    case 23:if(!camera_test_click(a,a->camera_mode_bounds))return;
        break;
    case 24:test_mouse(a,camera_test_pressed,false);tree_design.settle=0;break;
    case 25:
        if(tree_design.settle++<3)return;
        ok=!a->camera_2d && inspector_design_same(a);if(!ok)break;
        tree_design_record(a,"camera",32,capture);printf("TREE ACTIONS variant=%s keyboard-reveal=1 select=1 parent-hide=1 collapse=1 filter-pointer=1 filter-keyboard=1 left-camera=1 right-camera=1 source-and-run=1\n",inspector_design_variant());
        if(++inspector_design.variant<6){*stage=4;return;}break;
    case 26:{uint32_t crc=0;ok=timeline_design_run_crc(a->last_run,&crc) && crc==inspector_design.run_crc && inspector_design_same(a);command(a,PS_MSG_STOP);a->stop_at=ps_clock();break;}
    case 27:if(!idle(a) || a->library_thread)return;ok=inspector_design_same(a);if(ok)puts("TREE DESIGN SELF-TEST: PASSED");a->quitting=true;break;
    case 28:
        if(tree_design.settle++<3)return;
        if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"Szenenbaum-Breite")){nk_sdl_accessibility_request_focus(a->ui,NULL,"Szenenbaum-Breite");return;}
        test_window_key_mod(a->window,SDLK_HOME,0);tree_design.settle=0;break;
    case 29:
        if(tree_design.settle++<3)return;
        ok=a->preferences.sidebar_width==150 && fabsf(a->dock_panels[PS_DOCK_SIDEBAR].w-150)<.01f;
        test_window_key_mod(a->window,SDLK_END,0);tree_design.settle=0;break;
    case 30:
        if(tree_design.settle++<3)return;
        ok=a->preferences.sidebar_width==420 && fabsf(a->dock_panels[PS_DOCK_SIDEBAR].w-420)<.01f;
        test_window_key_mod(a->window,SDLK_RETURN,0);tree_design.settle=0;break;
    case 31:
        if(tree_design.settle++<3)return;
        ok=a->preferences.sidebar_width==190 && fabsf(a->dock_panels[PS_DOCK_SIDEBAR].w-190)<.01f && inspector_design_same(a);
        if(ok){puts("TREE RESIZE keyboard-home=150 keyboard-end=420 keyboard-enter=190 source-and-run=1");tree_design.settle=0;test_window_key_mod(a->window,SDLK_F10,0);}
        break;
    case 32:
        if(tree_design.settle++<2)return;
        ok=a->toolbar_keyboard && !a->toolbar_menu;test_window_key_mod(a->window,SDLK_DOWN,0);tree_design.settle=0;break;
    case 33:
        if(tree_design.settle++<3)return;
        ok=a->toolbar_menu==1 && !nk_sdl_accessibility_request_focus(a->ui,NULL,"Szenenbaum-Breite");
        *capture="tree-separator-menu.png";test_window_key_mod(a->window,SDLK_ESCAPE,0);tree_design.settle=0;break;
    case 34:
        if(tree_design.settle++<3)return;
        ok=!a->toolbar_keyboard && !a->toolbar_menu;
        if(ok && !inspector_design_focus(a,"Szenenbaum-Breite"))return;
        if(ok){puts("TREE SEPARATOR menu-blocked=1 keyboard-restored=1");tree_design.settle=0;*stage=5;return;}
        break;
    }
    if(!ok){fprintf(stderr,"TREE DESIGN FAILED stage=%d variant=%u selected=%u\n",*stage,inspector_design.variant,a->scene_selected_id);*code=1;a->quitting=true;}else (*stage)++;
}
