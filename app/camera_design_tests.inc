/* Actual paused runner, six camera layouts and real native input. */
static struct {unsigned settle,recorded;char fields[2][128],outputs[3][64],save[128];struct nk_rect drag;} camera_design;
static bool camera_design_start(app *a,const char *root) {memset(&camera_design,0,sizeof camera_design);return inspector_design_start(a,root);}
static void camera_design_append(char *out,size_t capacity,const struct nk_command_text *text) {size_t n=strlen(out),count=NK_MIN((size_t)text->length,capacity-n-1);memcpy(out+n,text->string,count);out[n+count]=0;}
static bool camera_design_contains(struct nk_rect r,const struct nk_command_text *text) {return r.w>0 && text->x>=r.x-1 && text->x<r.x+r.w && text->y>=r.y-1 && text->y<r.y+r.h;}
static void camera_design_draw(app *a) {
    memset(camera_design.fields,0,sizeof camera_design.fields);memset(camera_design.outputs,0,sizeof camera_design.outputs);memset(camera_design.save,0,sizeof camera_design.save);const struct nk_command *command;
    nk_foreach(command,a->ui)if(command->type==NK_COMMAND_TEXT) {
        const struct nk_command_text *text=(const struct nk_command_text*)command;
        for(unsigned i=0;i<2;i++)if(text->font==&a->laboratory_control_font && camera_design_contains(a->laboratory_camera_fields[1][i],text))camera_design_append(camera_design.fields[i],sizeof camera_design.fields[i],text);
        for(unsigned i=0;i<3;i++)if(text->font==&a->workspace_status_font && camera_design_contains(a->laboratory_camera_outputs[1][i],text))camera_design_append(camera_design.outputs[i],sizeof camera_design.outputs[i],text);
        if(text->font==&a->workbench_fonts[2] && camera_design_contains(a->display_save_bounds,text))camera_design_append(camera_design.save,sizeof camera_design.save,text);
    }
}
static bool camera_design_reveal(app *a,struct nk_rect r) {
    static unsigned settle;if(settle){settle--;return false;}if(a->ui->input.mouse.scroll_delta.y!=0 || r.w<=0 || r.h<=0)return false;
    struct nk_rect body=a->laboratory_inspector_body;
    if(r.y<body.y+8 || r.y+r.h>body.y+body.h-12){test_plot_wheel(a,body.x+body.w/2,body.y+body.h/2,r.y<body.y+8?1:-1);settle=2;return false;}
    return true;
}
static struct nk_rect camera_design_range_area(app *a) {
    float f=a->preferences.ui_size/16.f;struct nk_rect first=a->laboratory_camera_sliders[1][0],last=a->laboratory_camera_sliders[1][2];
    return nk_rect(first.x,first.y-18.95f*f,first.w,last.y+last.h-first.y+18.95f*f);
}
static void camera_design_mouse(app *a,float fraction,bool down) {
    struct nk_rect r=camera_design.drag;float f=a->preferences.ui_size/16.f;r.x+=8*f+(r.w-16*f)*fraction-.5f;r.w=1;test_mouse(a,r,down);
}
static void camera_design_motion(app *a,float fraction) {
    struct nk_rect r=camera_design.drag;float f=a->preferences.ui_size/16.f;SDL_Event e={0};e.type=SDL_EVENT_MOUSE_MOTION;e.motion.windowID=SDL_GetWindowID(a->window);e.motion.which=PS_TEST_MOUSE_ID;e.motion.x=r.x+8*f+(r.w-16*f)*fraction;e.motion.y=r.y+r.h/2;SDL_PushEvent(&e);
}
static void camera_design_hex(const char *text) {for(const unsigned char *p=(const unsigned char*)text;*p;p++)printf("%02x",*p);}
static void camera_design_rect(const char *label,struct nk_rect r) {printf(" %s=%.4f,%.4f,%.4f,%.4f",label,r.x,r.y,r.w,r.h);}
static void camera_design_record(app *a,const char *kind,unsigned bit,const char **capture) {
    if(camera_design.recorded&bit)return;
    camera_design.recorded|=bit;
    printf("CAMERA DESIGN kind=%s variant=%s size=%u preserved=%d mode=%d plane=%u projection=%d yaw=%.9g pitch=%.9g zoom=%.9g",kind,inspector_design_variant(),a->preferences.ui_size,inspector_design_same(a),a->camera_2d,a->camera_plane,a->orthographic,a->yaw,a->pitch,a->zoom);
    camera_design_rect("header",a->dock_headers[PS_DOCK_INSPECTOR]);camera_design_rect("body",a->laboratory_inspector_body);camera_design_rect("mode-rect",a->camera_mode_bounds);camera_design_rect("save",a->display_save_bounds);printf(" save-paint=");camera_design_hex(camera_design.save);puts("");
    for(unsigned i=0;i<2;i++){printf("CAMERA FIELD index=%u",i);camera_design_rect("rect",a->laboratory_camera_fields[1][i]);printf(" paint=");camera_design_hex(camera_design.fields[i]);puts("");}
    for(unsigned i=0;i<3;i++){printf("CAMERA RANGE index=%u",i);camera_design_rect("rect",a->laboratory_camera_sliders[1][i]);camera_design_rect("output",a->laboratory_camera_outputs[1][i]);printf(" paint=");camera_design_hex(camera_design.outputs[i]);puts("");}
    static char file[96];snprintf(file,sizeof file,"camera-%s-%s.png",inspector_design_variant(),kind);*capture=file;
}
static void camera_design_frame(app *a,int *stage,int *code,const char **capture) {
    if(*stage<4){inspector_design_frame(a,stage,code,capture);return;}bool ok=true;
    switch(*stage) {
    case 4:
        inspector_design_frame(a,stage,code,capture);a->laboratory_groups[3]=NK_MAXIMIZED;
        ps_dock_hide(&a->preferences.dock,PS_DOCK_SIDEBAR);a->view_disclosure=a->camera_disclosure=NK_MINIMIZED;
        a->camera_2d=false;a->camera_plane=0;a->orthographic=false;a->yaw=.31f;a->pitch=-.28f;a->zoom=6;a->camera_target=PS_CAMERA_DEFAULT.target;
        camera_design.settle=camera_design.recorded=0;return;
    case 5: {
        if(camera_design.settle++<3)return;
        struct nk_rect r=a->laboratory_section_bounds[7],field=a->laboratory_camera_fields[1][0];r.h=field.y+field.h-r.y;
        if(r.h>a->laboratory_inspector_body.h-20) {
            if(camera_design.settle==4)printf("CAMERA TOP-SCROLL combined-height=%.4f available-height=%.4f field-height=%.4f variant=%s\n",r.h,a->laboratory_inspector_body.h-20,field.h,inspector_design_variant());
            r=field;
        }
        if(!camera_design_reveal(a,r))return;
        camera_design_record(a,"top",1,capture);break;
    }
    case 6:if(!camera_test_click(a,a->laboratory_camera_fields[1][0]))return;
        break;
    case 7:test_mouse(a,camera_test_pressed,false);camera_design.settle=0;break;
    case 8:
        if(camera_design.settle++<3)return;
        ok=a->laboratory_camera_open[1][0];if(!ok)break;camera_design_record(a,"menu",2,capture);test_window_key_mod(a->window,SDLK_ESCAPE,0);camera_design.settle=0;break;
    case 9:
        if(camera_design.settle++<3)return;
        ok=!a->laboratory_camera_open[1][0] && a->yaw==.31f && a->pitch==-.28f && a->zoom==6 && !a->camera_2d;
        if(ok && !inspector_design_focus(a,laboratory_camera_names[1][0]))return;
        break;
    case 10:if(!camera_test_preset_click(a,2))return;
        break;
    case 11:test_mouse(a,camera_test_pressed,false);camera_design.settle=0;break;
    case 12:
        if(camera_design.settle++<3)return;
        ok=!a->camera_2d && a->yaw==(float)PS_PI/2 && a->pitch==0 && a->orthographic;if(!ok)break;
        if(!camera_test_click(a,a->camera_mode_bounds))return;
        break;
    case 13:test_mouse(a,camera_test_pressed,false);camera_design.settle=0;break;
    case 14:
        if(camera_design.settle++<3)return;
        ok=a->camera_2d && a->camera_plane==1;if(!ok)break;
        if(!camera_test_click(a,a->laboratory_camera_fields[1][1]))return;
        break;
    case 15:test_mouse(a,camera_test_pressed,false);camera_design.settle=0;break;
    case 16:
        if(camera_design.settle++<3)return;
        ok=a->laboratory_camera_open[1][1];if(!ok)break;
        if(!inspector_design_focus(a,"XZ"))return;
        test_window_key_mod(a->window,SDLK_RETURN,0);camera_design.settle=0;break;
    case 17:
        if(camera_design.settle++<3)return;
        ok=a->camera_plane==2 && a->camera_2d && a->yaw==0 && a->pitch==(float)PS_PI/2 && a->orthographic;
        if(ok && !inspector_design_focus(a,laboratory_camera_names[1][1]))return;
        camera_design_record(a,"plane",4,capture);if(ok){*stage=!inspector_design.variant?40:55;return;}break;
    case 18:
        if(!camera_design_reveal(a,camera_design_range_area(a)))return;
        ok=!nk_sdl_accessibility_request_focus(a->ui,NULL,"Kamera seitlich (rad)") && !nk_sdl_accessibility_request_focus(a->ui,NULL,"Kamera hoch (rad)");break;
    case 19:if(!inspector_design_focus(a,"Kameraabstand (m)"))return;test_window_key_mod(a->window,SDLK_END,0);camera_design.settle=0;break;
    case 20:
        if(camera_design.settle++<3)return;
        ok=fabsf(a->zoom-12)<.0001f;camera_design_record(a,"zoom-max",8,capture);test_window_key_mod(a->window,SDLK_HOME,0);camera_design.settle=0;break;
    case 21:
        if(camera_design.settle++<3)return;
        ok=fabsf(a->zoom-1.5f)<.0001f;camera_design_record(a,"zoom-min",16,capture);break;
    case 22:if(!camera_test_click(a,a->camera_mode_bounds))return;
        break;
    case 23:test_mouse(a,camera_test_pressed,false);camera_design.settle=0;break;
    case 24:
        if(camera_design.settle++<3)return;
        ok=!a->camera_2d;if(!ok)break;if(!camera_design_reveal(a,camera_design_range_area(a)))return;
        break;
    case 25:camera_design.drag=a->laboratory_camera_sliders[1][0];camera_design_mouse(a,.5f,true);break;
    case 26:camera_design_motion(a,.65f);camera_design.settle=0;*stage=56;return;
    case 27:
        if(camera_design.settle++<3)return;
        if(!camera_design_reveal(a,camera_design_range_area(a)))return;
        ok=a->yaw>1.8f && a->yaw<2.5f && a->pitch==(float)PS_PI/2;camera_design_record(a,"orbit",32,capture);if(ok){*stage=45;return;}break;
    case 28:if(!camera_design_reveal(a,a->display_save_bounds))return;
        break;
    case 29:
        camera_design_record(a,"save",64,capture);printf("CAMERA ACTIONS variant=%s preset-pointer=1 free-escape=1 plane-keyboard=1 disabled-orbit=1 zoom-home-end=1 orbit-pointer=1 orbit-negative-home-end=1 canonical-drag-continued=1 source-and-run=1\n",inspector_design_variant());
        if(++inspector_design.variant<6){*stage=4;return;}break;
    case 30:{uint32_t crc=0;ok=timeline_design_run_crc(a->last_run,&crc) && crc==inspector_design.run_crc;command(a,PS_MSG_STOP);a->stop_at=ps_clock();break;}
    case 31:if(!idle(a) || a->library_thread)return;if(ok)puts("CAMERA DESIGN SELF-TEST: PASSED");a->quitting=true;break;
    case 40:if(!camera_test_click(a,a->laboratory_camera_fields[1][1]))return;
        break;
    case 41:test_mouse(a,camera_test_pressed,false);camera_design.settle=0;break;
    case 42:
        if(camera_design.settle++<3)return;
        ok=a->laboratory_camera_open[1][1];if(!ok)break;test_window_key_mod(a->window,SDLK_2,SDL_KMOD_ALT);camera_design.settle=0;break;
    case 43:
        if(camera_design.settle++<3)return;
        ok=!a->camera_2d && !a->laboratory_camera_open[1][1] && !nk_sdl_accessibility_request_focus(a->ui,NULL,"XZ") && a->camera_plane==2;
        if(!ok)break;
        test_window_key_mod(a->window,SDLK_2,SDL_KMOD_ALT);camera_design.settle=0;break;
    case 44:
        if(camera_design.settle++<3)return;
        ok=a->camera_2d && a->camera_plane==2;
        if(ok){puts("CAMERA CANCEL disabled-menu-closed=1 stale-option-rejected=1 source-and-run=1");*stage=55;return;}
        break;
    case 45:if(!inspector_design_focus(a,"Kamera seitlich (rad)"))return;test_window_key_mod(a->window,SDLK_END,0);camera_design.settle=0;break;
    case 46:
        if(camera_design.settle++<3)return;
        ok=fabsf(a->yaw-3.14f)<.0001f;test_window_key_mod(a->window,SDLK_HOME,0);camera_design.settle=0;break;
    case 47:
        if(camera_design.settle++<3)return;
        if(!camera_design_reveal(a,camera_design_range_area(a)))return;
        ok=fabsf(a->yaw+3.14f)<.0001f;camera_design_record(a,"orbit-min",128,capture);
        if(ok){*stage=50;return;}
        break;
    case 50:camera_design.drag=a->laboratory_camera_sliders[1][0];camera_design_mouse(a,.55f,true);camera_design.settle=0;break;
    case 51:
        if(camera_design.settle++<1)return;
        ok=a->yaw>.1f && a->yaw<.6f;if(!ok)break;camera_design_motion(a,.85f);break;
    case 52:camera_design_mouse(a,.85f,false);camera_design.settle=0;break;
    case 53:
        if(camera_design.settle++<3)return;
        if(!camera_design_reveal(a,camera_design_range_area(a)))return;
        ok=a->yaw>1.8f && a->yaw<2.5f;camera_design_record(a,"orbit-drag",256,capture);
        if(ok){*stage=28;return;}
        break;
    case 55:
        if(!inspector_design_focus(a,"2D-Modus"))return;
        camera_design_record(a,"check-focus",512,capture);*stage=18;return;
    case 56:
        if(camera_design.settle++<1)return;
        ok=a->yaw>.6f && a->yaw<1.3f;
        if(ok){camera_design_motion(a,.85f);camera_design.settle=0;*stage=57;return;}
        break;
    case 57:
        if(camera_design.settle++<1)return;
        ok=a->yaw>1.8f && a->yaw<2.5f;
        if(ok){camera_design_mouse(a,.85f,false);camera_design.settle=0;*stage=27;return;}
        break;
    }
    if(!inspector_design_same(a))ok=false;
    if(!ok){fprintf(stderr,"CAMERA DESIGN FAILED stage=%d variant=%u mode=%d plane=%u yaw=%.9g pitch=%.9g zoom=%.9g\n",*stage,inspector_design.variant,a->camera_2d,a->camera_plane,a->yaw,a->pitch,a->zoom);*code=1;a->quitting=true;}else (*stage)++;
}
