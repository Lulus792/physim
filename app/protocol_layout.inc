/* The common log is a presentation of PS_DOCK_LOG, not a second dock graph.
 * Context changes retain its visibility and wishes; ordinary slots stay put. */
static bool protocol_visible(const app *a) {return a->show_log && a->tab!=6 && !a->panel_render;}
static float protocol_max_height(const app *a,float h) {
    float maximum=fminf(500,h*.55f);
    if(laboratory_visible(a)) {
        /* Leave the real scene, measurement and minimum timeline reachable.
         * This limits presentation only; the cross-workspace wish is retained. */
        float scene=laboratory_scene_minimum(a,a->dock_panels[PS_DOCK_WORKSPACE].w);
        maximum=fminf(maximum,h-LABORATORY_CONTENT_TOP-WORKSPACE_STATUS_HEIGHT-scene-LABORATORY_MEASURE_MIN-1-LABORATORY_TIMELINE_MIN);
    }
    return fmaxf(150,maximum);
}
static float protocol_height_value(const app *a,float h) {
    return fminf(protocol_max_height(a,h),fmaxf(150,(float)a->preferences.log_height));
}
static float protocol_reserved(const app *a,float h) {return protocol_visible(a)?protocol_height_value(a,h):0;}
static struct nk_rect protocol_bounds(const app *a,float w,float h) {
    float height=protocol_height_value(a,h),footer=workspace_shell_visible(a)?WORKSPACE_STATUS_HEIGHT:8;
    return nk_rect(0,h-footer-height,w,height);
}
static void protocol_resize_abort(app *a) {
    if(a->protocol_view.resize)a->preferences.log_height=(uint32_t)a->protocol_view.resize_original;
    a->protocol_view.resize=false;a->protocol_view.resize_cancelled=true;SDL_CaptureMouse(false);
}
static bool protocol_resize_key(app *a,const SDL_KeyboardEvent *key) {
    if(SDL_getenv("PHYSIM_TEST_TRACE") && (key->key==SDLK_UP || key->key==SDLK_DOWN || key->key==SDLK_HOME || key->key==SDLK_END))
        fprintf(stderr,"PROTOCOL KEY ROUTE: key=%u mod=%u tab=%d visible=%d focused=%d window-focus=%d height=%u\n",(unsigned)key->key,(unsigned)key->mod,a->tab,protocol_visible(a),nk_sdl_accessibility_is_focused(a->ui,NULL,"Protokoll-Höhe"),SDL_GetKeyboardFocus()==a->window,a->preferences.log_height);
    if(!protocol_visible(a) || a->toolbar_menu || key->mod&(SDL_KMOD_CTRL|SDL_KMOD_ALT|SDL_KMOD_GUI))return false;
    if(key->key==SDLK_ESCAPE && a->protocol_view.resize){protocol_resize_abort(a);return true;}
    if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"Protokoll-Höhe"))return false;
    int w,h;SDL_GetWindowSize(a->window,&w,&h);float value=protocol_height_value(a,(float)h);
    if(key->key==SDLK_UP || key->key==SDLK_DOWN)value+=(key->key==SDLK_UP?1:-1)*(key->mod&SDL_KMOD_SHIFT?40:10);
    else if(key->key==SDLK_HOME)value=150;
    else if(key->key==SDLK_END)value=protocol_max_height(a,(float)h);
    else if(key->key==SDLK_RETURN || key->key==SDLK_KP_ENTER)value=340;
    else return false;
    a->preferences.log_height=(uint32_t)lroundf(fmaxf(150,fminf(protocol_max_height(a,(float)h),value)));
    if(SDL_getenv("PHYSIM_TEST_TRACE"))fprintf(stderr,"PROTOCOL KEY HANDLED: height=%u mod=%u\n",a->preferences.log_height,(unsigned)key->mod);
    return true;
}
static void protocol_resize_input(app *a,float w,float h) {
    ps_protocol_ui *p=&a->protocol_view;struct nk_input *in=&a->ui->input;
    if(p->cursor_owned){SDL_SetCursor(SDL_GetDefaultCursor());p->cursor_owned=false;}
    if(!protocol_visible(a)){if(p->resize)protocol_resize_abort(a);return;}
    struct nk_rect bounds=protocol_bounds(a,w,h);p->separator=nk_rect(0,bounds.y-4,w,9);
    if(SDL_getenv("PHYSIM_TEST_TRACE") && (in->mouse.buttons[NK_BUTTON_LEFT].clicked || nk_input_is_mouse_down(in,NK_BUTTON_LEFT) || p->resize))
        fprintf(stderr,"PROTOCOL POINTER: down=%d pressed=%d hover=%d cancelled=%d menu=%d consumed=%d resize=%d pos=%.2f,%.2f separator=%.2f,%.2f,%.2f,%.2f keyboard=%d mouse=%d\n",
            nk_input_is_mouse_down(in,NK_BUTTON_LEFT),nk_input_is_mouse_pressed(in,NK_BUTTON_LEFT),nk_input_is_mouse_hovering_rect(in,p->separator),
            p->resize_cancelled,a->toolbar_menu,a->toolbar_pointer_consumed,p->resize,in->mouse.pos.x,in->mouse.pos.y,
            p->separator.x,p->separator.y,p->separator.w,p->separator.h,SDL_GetKeyboardFocus()==a->window,SDL_GetMouseFocus()==a->window);
    bool down=nk_input_is_mouse_down(in,NK_BUTTON_LEFT);
    bool pressed=ps_ui_mouse_pressed_in_rect(in,NK_BUTTON_LEFT,p->separator);
    const struct nk_mouse_button *button=&in->mouse.buttons[NK_BUTTON_LEFT];
    if(!down && !pressed) {
        if(p->resize) {
            if(button->clicked && button->releases)a->preferences.log_height=(uint32_t)lroundf(fmaxf(150,fminf(protocol_max_height(a,h),p->resize_effective+p->resize_y-button->release_pos.y)));
            SDL_CaptureMouse(false);
        }
        p->resize=false;p->resize_cancelled=false;return;
    }
    if(p->resize_cancelled)return;
    if(!p->resize && !a->toolbar_menu && !a->toolbar_pointer_consumed && pressed) {
        /* Claim keyboard ownership with the press, before rendering other
         * windows can change the active widget or the pointer's hover area. */
        bool focus_requested=nk_sdl_accessibility_request_focus(a->ui,NULL,"Protokoll-Höhe");
        if(SDL_getenv("PHYSIM_TEST_FOCUS_TRACE"))fprintf(stderr,"FOCUS POINTER REQUEST: accepted=%d keyboard=%d\n",focus_requested,SDL_GetKeyboardFocus()==a->window);
        p->resize=true;p->resize_original=(float)a->preferences.log_height;p->resize_effective=protocol_height_value(a,h);p->resize_y=ps_ui_mouse_press_position(in,NK_BUTTON_LEFT).y;
        if(down)SDL_CaptureMouse(true);
        if(ps_ui_mouse_pressed(in,NK_BUTTON_DOUBLE)){a->preferences.log_height=340;p->resize=false;SDL_CaptureMouse(false);return;}
    }
    if(p->resize) {
        float position=!down && button->releases?button->release_pos.y:in->mouse.pos.y;
        a->preferences.log_height=(uint32_t)lroundf(fmaxf(150,fminf(protocol_max_height(a,h),p->resize_effective+p->resize_y-position)));
        if(!down){p->resize=false;SDL_CaptureMouse(false);}
    }
}
