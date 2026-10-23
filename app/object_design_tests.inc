/* Actual runner, native Alt+N selection, read-only header/property inspection. */
static struct {unsigned selection,settle,steps;bool collapsed,actions_done,text_present[7];char text[7][96];} object_design;
static bool object_design_start(app *a,const char *root) {
    memset(&object_design,0,sizeof object_design);return inspector_design_start(a,root);
}
static uint32_t object_design_target(void) {const uint32_t ids[]={3,101,11};return ids[object_design.selection];}
static void object_design_draw(app *a) {
    memset(object_design.text_present,0,sizeof object_design.text_present);memset(object_design.text,0,sizeof object_design.text);
    const struct nk_command *command;nk_foreach(command,a->ui)if(command->type==NK_COMMAND_TEXT) {
        const struct nk_command_text *text=(const struct nk_command_text*)command;if(text->font!=&a->laboratory_model_font)continue;
        for(unsigned i=0;i<a->laboratory_property_count;i++) {
            struct nk_rect r=a->laboratory_property_value_bounds[i];
            if(text->x>=r.x-1 && text->x<r.x+r.w && text->y>=r.y-1 && text->y<r.y+r.h) {
                object_design.text_present[i]=true;size_t at=strlen(object_design.text[i]),count=NK_MIN((size_t)text->length,sizeof object_design.text[i]-at-1);
                memcpy(object_design.text[i]+at,text->string,count);object_design.text[i][at+count]=0;
            }
        }
    }
}
static const char *object_design_capture(const char *kind) {
    static char name[96];snprintf(name,sizeof name,"object-%s-%u-%s.png",inspector_design_variant(),object_design_target(),kind);return name;
}
static void object_design_log(app *a,const char *kind) {
    const ps_object *o=&display_scene(a)->objects[a->scene_selected_slot];
    printf("OBJECT DESIGN kind=%s variant=%s size=%u id=%u shape=%u count=%u preserved=%d source=%.17g,%.17g,%.17g,%.17g",kind,inspector_design_variant(),a->preferences.ui_size,o->id,o->shape,a->laboratory_property_count,inspector_design_same(a),o->radius,o->a.x,o->a.y,o->a.z);
    const struct nk_rect bounds[]={a->dock_headers[PS_DOCK_INSPECTOR],a->laboratory_inspector_body,a->laboratory_footer_bounds,a->laboratory_object_heading_bounds,a->laboratory_object_avatar_bounds,a->laboratory_object_title_bounds};const char *names[]={"panel","body","footer","heading","avatar","title"};
    for(unsigned i=0;i<SDL_arraysize(bounds);i++)printf(" %s=%.4f,%.4f,%.4f,%.4f",names[i],bounds[i].x,bounds[i].y,bounds[i].w,bounds[i].h);
    puts("");
    for(unsigned i=0;i<a->laboratory_property_count;i++) {
        struct nk_rect c=a->laboratory_property_bounds[i],v=a->laboratory_property_value_bounds[i];
        printf("OBJECT PROPERTY index=%u card=%.4f,%.4f,%.4f,%.4f value=%.4f,%.4f,%.4f,%.4f present=%d text=%s\n",i,c.x,c.y,c.w,c.h,v.x,v.y,v.w,v.h,object_design.text_present[i],object_design.text[i]);
    }
}
static void object_design_frame(app *a,int *stage,int *code,const char **capture) {
    if(*stage<4){inspector_design_frame(a,stage,code,capture);return;}
    bool ok=true;
    switch(*stage) {
    case 4:
        inspector_design_frame(a,stage,code,capture);a->laboratory_groups[0]=NK_MAXIMIZED;object_design.settle=object_design.steps=0;object_design.collapsed=false;return;
    case 5:
        if(SDL_GetKeyboardFocus()!=a->window){SDL_RaiseWindow(a->window);return;}
        if(!a->scene_selected || a->scene_selected_id!=object_design_target()) {
            ok=object_design.steps++<PS_MAX_OBJECTS;
            if(ok){test_window_key_mod(a->window,SDLK_N,SDL_KMOD_ALT);return;}
        }
        if(object_design.settle++<3)return;
        ok=ok && inspector_design_same(a) && a->scene_selected_id==object_design_target();
        if(ok){unsigned expected=object_design.selection==0?4:object_design.selection==1?0:3;ok=a->laboratory_property_count==expected;}
        object_design_log(a,"header");*capture=object_design_capture("header");object_design.settle=0;break;
    case 6:
        if(!a->laboratory_property_count){*stage=9;return;}
        if(!camera_test_click(a,a->laboratory_property_bounds[a->laboratory_property_count-1]))return;
        break;
    case 7:test_mouse(a,camera_test_pressed,false);object_design.settle=0;break;
    case 8:
        if(object_design.settle++<3)return;
        ok=inspector_design_same(a) && object_design.text_present[a->laboratory_property_count-1];
        object_design_log(a,"last");*capture=object_design_capture("last");break;
    case 9:
        if(object_design.selection==0 && !object_design.collapsed) {
            if(!camera_test_click(a,a->laboratory_section_bounds[0]))return;
            *stage=12;return;
        }
        if(object_design.selection==0 && inspector_design.variant==0 && !object_design.actions_done) {
            if(!camera_test_click(a,a->scene_hide_selection_bounds))return;
            *stage=16;return;
        }
        ok=inspector_design_same(a);if(++object_design.selection<3){*stage=4;return;}
        object_design.selection=0;if(++inspector_design.variant<6){*stage=4;return;}break;
    case 10:{uint32_t crc=0;ok=timeline_design_run_crc(a->last_run,&crc) && crc==inspector_design.run_crc && inspector_design_same(a);command(a,PS_MSG_STOP);a->stop_at=ps_clock();break;}
    case 11:if(!idle(a) || a->library_thread)return;ok=inspector_design_same(a);if(ok)puts("OBJECT DESIGN SELF-TEST: PASSED");a->quitting=true;break;
    case 12:test_mouse(a,camera_test_pressed,false);object_design.settle=0;break;
    case 13:
        if(object_design.settle++<3)return;
        ok=a->laboratory_groups[0]==NK_MINIMIZED && !a->laboratory_property_count && a->laboratory_object_heading_bounds.w>0 && a->scene_selected_id==3 && inspector_design_same(a);
        object_design_log(a,"collapsed");*capture=object_design_capture("collapsed");test_mouse(a,a->laboratory_section_bounds[0],true);break;
    case 14:test_mouse(a,a->laboratory_section_bounds[0],false);object_design.settle=0;break;
    case 15:
        if(object_design.settle++<3)return;
        ok=a->laboratory_groups[0]==NK_MAXIMIZED && a->laboratory_property_count==4 && inspector_design_same(a);
        if(ok){object_design.collapsed=true;*stage=9;return;}
        break;
    case 16:test_mouse(a,camera_test_pressed,false);object_design.settle=0;break;
    case 17:
        if(object_design.settle++<3)return;
        ok=!a->scene_selected && (a->scene_view.hidden&(UINT32_C(1)<<a->scene_selected_slot)) && inspector_design_same(a);
        if(!ok)break;
        if(!camera_test_click(a,a->display_all_bounds))return;
        break;
    case 18:test_mouse(a,camera_test_pressed,false);object_design.settle=object_design.steps=0;break;
    case 19:
        if(object_design.settle++<3)return;
        ok=!a->scene_view.hidden && !a->scene_selected && inspector_design_same(a);object_design.settle=0;break;
    case 20:
        if(!a->scene_selected || a->scene_selected_id!=3) {
            ok=object_design.steps++<PS_MAX_OBJECTS;
            if(ok){test_window_key_mod(a->window,SDLK_N,SDL_KMOD_ALT);return;}
        }
        if(object_design.settle++<3)return;
        ok=ok && inspector_design_same(a) && a->scene_selected_id==3 && !a->scene_view.hidden;
        if(ok){puts("OBJECT ACTIONS hide=1 show-all=1 keyboard-reselect=1 source-and-run=1");object_design.actions_done=true;*stage=9;return;}
        break;
    }
    if(!ok){fprintf(stderr,"OBJECT DESIGN FAILED stage=%d variant=%u selection=%u\n",*stage,inspector_design.variant,object_design.selection);*code=1;a->quitting=true;}else (*stage)++;
}
