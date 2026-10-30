/* Seeded native UI fixtures. The runner-based hierarchy suite is separate. */
static struct {
    bool scene,focus_requested,parked,after_parked,menu_prepared;unsigned settle,scroll_attempts;
    char root[4096],target[4096],baseline[4096],folder[4096],focus_name[4200];
    uint32_t scene_crc,source_crc;float yaw,pitch,zoom;ps_vec3 camera;
    Uint64 focus_started;const char *phase;
} tree_a2;
static const char tree_a2_name[]="A2_Originaldatei_mit_vollständigem_Namen_α.txt";
static const char tree_a2_object[]="Kugel mit vollständiger Beschriftung α";
static bool tree_a2_start(app *a,const char *root,const char *mode) {
    memset(&tree_a2,0,sizeof tree_a2);tree_a2.scene=strstr(mode,"-scene-")!=NULL;snprintf(tree_a2.root,sizeof tree_a2.root,"%s",root);
    bool okay=tree_a2.scene?laboratory_test_start(a,root,mode):workbench_test_start(a,root,mode);if(!okay)return false;
    if(strstr(mode,"xxlarge")) {
        /* Deliberate 32px layout stress, beyond the 16..22px Settings range.
         * The normal preference transaction correctly rejects 32; do not
         * silently count the resulting 22px fallback as a 200% observation. */
        a->preferences.ui_size=32;nk_sdl_set_ui_size(a->ui,32);
    }
    a->show_log=false;SDL_SetWindowSize(a->window,1440,1000);
    if(tree_a2.scene) {
        a->preferences.sidebar_width=strstr(mode,"xxlarge")?150:190;
        laboratory_context(a);ps_dock_reveal_outer(&a->preferences.dock,PS_DOCK_SIDEBAR,PS_DOCK_LEFT);
        a->scene=(ps_scene){0};a->scene_view=(ps_scene_view){0};a->scene_selected=false;
        okay=ps_scene_group(&a->scene,100,0,"Versuch α")==PS_OK &&
            ps_scene_add_id(&a->scene,10,PS_SPHERE,ps_v3(0,0,0),ps_v3(0,0,0),.25,UINT32_MAX)==PS_OK &&
            ps_scene_set_parent(&a->scene,10,100)==PS_OK;
        snprintf(a->scene.objects[1].text,sizeof a->scene.objects[1].text,"%s",tree_a2_object);
        a->view_disclosure=a->camera_disclosure=NK_MINIMIZED;
        snprintf(tree_a2.focus_name,sizeof tree_a2.focus_name,"Objekt auswählen: ID 10 · Kugel · %s",tree_a2_object);
    } else {
        snprintf(browser_test_root,sizeof browser_test_root,"%s",a->project);
        okay=browser_fixture_write("00_Basis.txt","Unveränderte Basis α\n") && browser_fixture_write(tree_a2_name,"Unverändertes Ziel Ω\n");
        refresh_workspace_entries(a);snprintf(tree_a2.folder,sizeof tree_a2.folder,"%s",a->workspace_tree.entries[0].path);
        /* SDL's Windows enumeration retains its native path separator. Use
         * the tree's actual identities, as the existing tree workflows do. */
        for(size_t i=0;i<a->workspace_tree.count;i++) {
            const char *name=workspace_name(a->workspace_tree.entries[i].path);
            if(!strcmp(name,tree_a2_name))snprintf(tree_a2.target,sizeof tree_a2.target,"%s",a->workspace_tree.entries[i].path);
            if(!strcmp(name,"00_Basis.txt"))snprintf(tree_a2.baseline,sizeof tree_a2.baseline,"%s",a->workspace_tree.entries[i].path);
        }
        okay &= *tree_a2.target && *tree_a2.baseline;
        preview_workspace_path(a,tree_a2.baseline);snprintf(a->workspace_tree_test_path,sizeof a->workspace_tree_test_path,"%s",tree_a2.target);
        snprintf(tree_a2.focus_name,sizeof tree_a2.focus_name,"%s",tree_a2_name);
    }
    tree_a2.scene_crc=ps_crc32((const unsigned char*)&a->scene,sizeof a->scene);
    tree_a2.source_crc=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
    tree_a2.yaw=a->yaw;tree_a2.pitch=a->pitch;tree_a2.zoom=a->zoom;tree_a2.camera=a->camera_target;
    return okay;
}
static struct nk_rect tree_a2_target(app *a) {return tree_a2.scene?a->laboratory_tree_rows[1]:a->workspace_tree_test_bounds;}
/* Reveal real layout targets using the normal SDL wheel route. Never resize
 * the fixture or select an object merely to make a screenshot pass. */
static bool tree_a2_reveal(app *a,struct nk_rect row,int *code) {
    struct nk_rect clip=tree_a2.scene?a->laboratory_tree_body:a->workspace_tree_test_clip;
    /* Nuklear caps the fractional content extent through an integer scrollbar.
     * Allow that one raster pixel at the row edge; glyph/clip checks stay exact. */
    if(row.h>0 && row.y>=clip.y-1 && row.y+row.h<=clip.y+clip.h+1){tree_a2.scroll_attempts=0;return true;}
    if(row.h<=0 || clip.h<=0 || row.h>clip.h || tree_a2.scroll_attempts++>=32) {
        fprintf(stderr,"TREE A2 reveal failed row=%.1f,%.1f clip=%.1f,%.1f attempts=%u\n",row.y,row.h,clip.y,clip.h,tree_a2.scroll_attempts);
        *code=1;a->quitting=true;return false;
    }
    float x=clip.x+clip.w/2,y=clip.y+clip.h/2;
    test_plot_motion(a,x,y);SDL_Event event;SDL_zero(event);
    event.type=SDL_EVENT_MOUSE_WHEEL;event.wheel.windowID=SDL_GetWindowID(a->window);event.wheel.which=PS_TEST_MOUSE_ID;
    event.wheel.y=row.y<clip.y?2:-2;event.wheel.mouse_x=x;event.wheel.mouse_y=y;SDL_PushEvent(&event);
    return false;
}
static struct nk_rect tree_a2_indicator(app *a,unsigned slot) {
    float f=a->preferences.ui_size/16.f;struct nk_rect row=tree_a2.scene?a->laboratory_tree_rows[slot]:a->workspace_tree_test_bounds;
    bool compact=tree_a2.scene && a->laboratory_tree_titles[slot].y-row.y>30*f;
    return nk_rect(row.x+13*f,row.y+((compact?32*f:row.h)-15*f)/2,15*f,15*f);
}
static bool tree_a2_selected(app *a,bool target) {
    if(tree_a2.scene)return a->scene_selected && a->scene_selected_id==(target?10u:100u);
    workspace_document *doc=active_document(a);return doc && workbench_path_equal(doc->path,target?tree_a2.target:tree_a2.baseline) && !doc->dirty;
}
static bool tree_a2_preserved(app *a) {
    return ps_crc32((const unsigned char*)&a->scene,sizeof a->scene)==tree_a2.scene_crc &&
        ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string))==tree_a2.source_crc &&
        a->yaw==tree_a2.yaw && a->pitch==tree_a2.pitch && a->zoom==tree_a2.zoom && !memcmp(&a->camera_target,&tree_a2.camera,sizeof tree_a2.camera);
}
static void tree_a2_draw(app *a) {
    if(!tree_a2.phase)return;selection_row_probe(a,a->ui,tree_a2.phase,tree_a2_target(a));tree_a2.phase=NULL;
}
static void tree_a2_capture(const char *phase,const char **capture) {
    tree_a2.phase=phase;*capture=!strcmp(phase,"before")?"list-before.png":!strcmp(phase,"after")?"list-after.png":"list-focus.png";
}
static void tree_a2_frame(app *a,int *stage,int *code,const char **capture) {
    if(tree_a2.settle++<3)return;tree_a2.settle=0;bool okay=tree_a2_preserved(a);
    switch(*stage) {
    case 0:
        if(!tree_a2_reveal(a,tree_a2_target(a),code))return;
        if(!tree_a2.parked){test_plot_motion(a,900,90);tree_a2.parked=true;return;}
        tree_a2_capture("before",capture);break;
    case 1:test_window_mouse(a->window,tree_a2_indicator(a,1),true);break;
    case 2:test_window_mouse(a->window,tree_a2_indicator(a,1),false);break;
    case 3:
        if(!tree_a2.after_parked){test_plot_motion(a,900,90);tree_a2.after_parked=true;return;}
        okay&=tree_a2_selected(a,true);tree_a2_capture("after",capture);break;
    case 4:
        if(tree_a2.scene)test_window_mouse(a->window,a->scene_visibility_bounds[1],true);
        else snprintf(a->workspace_tree_test_path,sizeof a->workspace_tree_test_path,"%s",tree_a2.folder);
        break;
    case 5:
        if(!tree_a2.scene && !tree_a2_reveal(a,a->workspace_tree_test_bounds,code))return;
        if(tree_a2.scene)test_window_mouse(a->window,a->scene_visibility_bounds[1],false);else test_window_mouse(a->window,a->workspace_tree_test_bounds,true);break;
    case 6:
        if(tree_a2.scene){okay&=a->scene_view.hidden==2 && tree_a2_selected(a,true);test_window_mouse(a->window,a->scene_visibility_bounds[1],true);}
        else test_window_mouse(a->window,a->workspace_tree_test_bounds,false);
        break;
    case 7:if(tree_a2.scene)test_window_mouse(a->window,a->scene_visibility_bounds[1],false);else okay&=tree_a2_selected(a,true) && !a->workspace_tree.entries[0].expanded;break;
    case 8:
        if(tree_a2.scene && !tree_a2_reveal(a,a->laboratory_tree_rows[0],code))return;
        if(tree_a2.scene){okay&=!a->scene_view.hidden && tree_a2_selected(a,true);test_window_mouse(a->window,a->scene_expand_bounds[0],true);}
        else test_window_mouse(a->window,a->workspace_tree_test_bounds,true);
        break;
    case 9:test_window_mouse(a->window,tree_a2.scene?a->scene_expand_bounds[0]:a->workspace_tree_test_bounds,false);break;
    case 10:
        if(tree_a2.scene){okay&=a->scene_view.collapsed==1 && tree_a2_selected(a,true);test_window_mouse(a->window,a->scene_expand_bounds[0],true);}
        else {okay&=a->workspace_tree.entries[0].expanded && tree_a2_selected(a,true);snprintf(a->workspace_tree_test_path,sizeof a->workspace_tree_test_path,"%s",tree_a2.baseline);}
        break;
    case 11:if(tree_a2.scene)test_window_mouse(a->window,a->scene_expand_bounds[0],false);break;
    case 12:
        if(!tree_a2_reveal(a,tree_a2.scene?a->laboratory_tree_rows[0]:a->workspace_tree_test_bounds,code))return;
        okay&=tree_a2.scene?!a->scene_view.collapsed && a->laboratory_tree_rows[1].h>0:a->workspace_tree_test_bounds.h>0;
        test_window_mouse(a->window,tree_a2_indicator(a,0),true);break;
    case 13:test_window_mouse(a->window,tree_a2_indicator(a,0),false);break;
    case 14:
        if(!tree_a2.menu_prepared) {
            if(!tree_a2_selected(a,false)){*code=1;a->quitting=true;return;}
            if(!tree_a2.scene)snprintf(a->workspace_tree_test_path,sizeof a->workspace_tree_test_path,"%s",tree_a2.target);
            tree_a2.menu_prepared=true;return;
        }
        if(!tree_a2_reveal(a,tree_a2_target(a),code))return;
        a->toolbar_menu=4;break;
    case 15:test_window_mouse(a->window,tree_a2_indicator(a,1),true);break;
    case 16:test_window_mouse(a->window,tree_a2_indicator(a,1),false);break;
    case 17:okay&=!a->toolbar_menu && tree_a2_selected(a,false);test_window_mouse(a->window,tree_a2_indicator(a,1),true);break;
    case 18:test_window_mouse(a->window,tree_a2_indicator(a,1),false);break;
    case 19:
        okay&=tree_a2_selected(a,true);
        test_plot_motion(a,900,90);
#ifdef _WIN32
        *stage=21;break;
#else
        /* Dismiss a path tooltip without inserting a Tab into the open editor. */
        test_window_key_mod(a->window,SDLK_LCTRL,SDL_KMOD_CTRL);break;
#endif
    case 20:
        if(!tree_a2.focus_requested){okay&=nk_sdl_accessibility_request_focus(a->ui,NULL,tree_a2.focus_name);tree_a2.focus_requested=true;tree_a2.focus_started=SDL_GetTicks();}
        if(!nk_sdl_accessibility_is_focused(a->ui,NULL,tree_a2.focus_name)){if(SDL_GetTicks()-tree_a2.focus_started<2000)return;okay=false;}
        okay&=SDL_GetKeyboardFocus()==a->window && tree_a2_selected(a,true);
        tree_a2_capture("focus",capture);break;
    case 21:break;
    case 22:
        okay&=tree_a2_selected(a,true) && (!tree_a2.scene || (!a->scene_view.hidden && !a->scene_view.collapsed));
        printf("TREE SELECTION ACTIONS: scene=%d selected=1 fold=1 visibility=%d menu-close-no-selection=1 source-and-camera=1\n",tree_a2.scene,tree_a2.scene);
        puts("TREE SELECTION A2 SELF-TEST: PASSED");a->quitting=true;break;
    }
    if(!okay){fprintf(stderr,"TREE A2 failed stage=%d scene=%d selected=%u\n",*stage,tree_a2.scene,a->scene_selected_id);*code=1;a->quitting=true;}else (*stage)++;
}
