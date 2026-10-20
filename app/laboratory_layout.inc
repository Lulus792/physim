/* Session-owned use of the existing docking engine. No new preference wire format. */
static bool laboratory_visible(const app *a) {return a->tab==1 && !a->project_manager;}
static ps_layout_state laboratory_capture(const app *a) {
    return (ps_layout_state){a->preferences.dock,a->preferences.sidebar_width,
        a->preferences.inspector_width,a->preferences.log_height,a->show_log};
}
static void laboratory_apply(app *a,const ps_layout_state *s) {
    a->preferences.dock=s->dock;a->preferences.sidebar_width=s->sidebar_width;
    a->preferences.inspector_width=s->inspector_width;a->preferences.log_height=s->log_height;a->show_log=s->show_log;
    a->dock_resize=a->dock_float_resize=a->dock_drag=0;a->dock_dragging=false;
}
static void laboratory_context(app *a) {
    bool active=laboratory_visible(a);
    if(active==a->laboratory_active)return;
    if(active) {
        a->laboratory_previous=laboratory_capture(a);
        if(!a->laboratory_initialized) {
            a->laboratory_layout=a->laboratory_previous;
            /* Both established fixed defaults are standard layouts. Switching
             * from the workbench must still enter Messlabor's intended state. */
            ps_dock_layout workbench=ps_dock_fixed_workbench_layout(0);
            if(!memcmp(&a->preferences.dock,&PS_DOCK_DEFAULT,sizeof PS_DOCK_DEFAULT) || !memcmp(&a->preferences.dock,&workbench,sizeof workbench)) {
                ps_dock_hide(&a->laboratory_layout.dock,PS_DOCK_SIDEBAR);
                ps_dock_hide(&a->laboratory_layout.dock,PS_DOCK_LOG);
                a->laboratory_layout.show_log=false;a->laboratory_layout.inspector_width=320;
            }
            float scale=a->preferences.ui_size/16.f;
            a->laboratory_measure_height=330*scale;a->laboratory_closed_height=118*scale;a->laboratory_timeline_height=110*scale;
            a->laboratory_curve_ratio=.5f;a->laboratory_channels[0]=0;a->laboratory_channels[1]=1;
            a->laboratory_groups[0]=NK_MAXIMIZED;
            a->model_disclosure=NK_MAXIMIZED;
            a->laboratory_initialized=true;
        }
        laboratory_apply(a,&a->laboratory_layout);
    } else {
        a->laboratory_layout=laboratory_capture(a);
        laboratory_apply(a,&a->laboratory_previous);
    }
    a->laboratory_active=active;
}
static const char *laboratory_separator_name(unsigned i) {
    const char *names[]={"Szene und Messbereich", "Zeitleistenhöhe", "Breitenverhältnis der Kurven"};return names[i];
}
static float *laboratory_dimension(app *a,unsigned i) {
    return i==0?(a->laboratory_curves?&a->laboratory_measure_height:&a->laboratory_closed_height):i==1?&a->laboratory_timeline_height:&a->laboratory_curve_ratio;
}
static void laboratory_dimension_limits(app *a,unsigned i,float *minimum,float *maximum,float *standard) {
    int w,h;SDL_GetWindowSize(a->window,&w,&h);float scale=a->preferences.ui_size/16.f;
    *minimum=i==2?.2f:(i==0?(a->laboratory_curves?200:118):84)*scale;
    *maximum=i==2?.8f:fmaxf(*minimum,(float)h*(i==0?.48f:.28f));
    if(i==2){float width=a->dock_panels[PS_DOCK_WORKSPACE].w;*minimum=fminf(.5f,fmaxf(.25f,180/fmaxf(1,width-9)));*maximum=1-*minimum;}
    if(i==0 && a->dock_panels[PS_DOCK_WORKSPACE].h>0)
        *maximum=fminf(*maximum,fmaxf(*minimum,a->dock_panels[PS_DOCK_WORKSPACE].h-70));
    *standard=i==2?.5f:(i==0?(a->laboratory_curves?330:118):110)*scale;
}
static void laboratory_dimension_set(app *a,unsigned i,float value) {
    float lo,hi,standard;laboratory_dimension_limits(a,i,&lo,&hi,&standard);
    *laboratory_dimension(a,i)=fmaxf(lo,fminf(hi,value));
}
static float laboratory_dimension_value(app *a,unsigned i) {
    float lo,hi,standard;laboratory_dimension_limits(a,i,&lo,&hi,&standard);
    return fmaxf(lo,fminf(hi,*laboratory_dimension(a,i)));
}
static void laboratory_resize_abort(app *a) {
    if(a->laboratory_resize)*laboratory_dimension(a,(unsigned)a->laboratory_resize-1)=a->laboratory_resize_original;
    a->laboratory_resize=0;a->laboratory_resize_cancelled=true;SDL_CaptureMouse(false);
}
static void laboratory_dimension_reset(app *a,unsigned i) {
    float lo,hi,standard;laboratory_dimension_limits(a,i,&lo,&hi,&standard);laboratory_dimension_set(a,i,standard);
    if(i==0){float scale=a->preferences.ui_size/16.f;a->laboratory_measure_height=330*scale;a->laboratory_closed_height=118*scale;}
}
static bool laboratory_resize_key(app *a,const SDL_KeyboardEvent *key) {
    if(!laboratory_visible(a) || a->toolbar_menu || key->mod&(SDL_KMOD_CTRL|SDL_KMOD_ALT|SDL_KMOD_GUI))return false;
    if(key->key==SDLK_ESCAPE && a->laboratory_resize) {
        laboratory_resize_abort(a);return true;
    }
    for(unsigned i=0;i<3;i++)if(nk_sdl_accessibility_is_focused(a->ui,NULL,laboratory_separator_name(i))) {
        int dir=i==2?(key->key==SDLK_LEFT?-1:key->key==SDLK_RIGHT?1:0):(key->key==SDLK_UP?-1:key->key==SDLK_DOWN?1:0);
        float lo,hi,standard;laboratory_dimension_limits(a,i,&lo,&hi,&standard);
        float step=key->mod&SDL_KMOD_SHIFT?40:10;
        if(dir)laboratory_dimension_set(a,i,laboratory_dimension_value(a,i)+(i==2?dir*step/fmaxf(1,a->dock_panels[PS_DOCK_WORKSPACE].w-9):-dir*step));
        else if(key->key==SDLK_HOME)laboratory_dimension_set(a,i,lo);
        else if(key->key==SDLK_END)laboratory_dimension_set(a,i,hi);
        else if(key->key==SDLK_RETURN || key->key==SDLK_KP_ENTER)laboratory_dimension_reset(a,i);
        else return false;
        return true;
    }
    return false;
}
static void laboratory_resize_input(app *a) {
    if(!laboratory_visible(a)){a->laboratory_resize=0;return;}
    struct nk_input *in=&a->ui->input;
    if(!nk_input_is_mouse_down(in,NK_BUTTON_LEFT)){a->laboratory_resize=0;a->laboratory_resize_cancelled=false;if(SDL_GetMouseFocus()==a->window)SDL_CaptureMouse(false);return;}
    if(a->laboratory_resize_cancelled)return;
    if(!a->toolbar_menu && !a->toolbar_pointer_consumed && nk_input_is_mouse_pressed(in,NK_BUTTON_LEFT)) {
        for(unsigned i=0;i<3;i++)if(a->laboratory_splitters[i].w>0 && nk_input_is_mouse_hovering_rect(in,a->laboratory_splitters[i])) {
            a->laboratory_resize=(int)i+1;SDL_CaptureMouse(true);
            a->laboratory_resize_original=*laboratory_dimension(a,i);
            if(nk_input_is_mouse_pressed(in,NK_BUTTON_DOUBLE)){laboratory_dimension_reset(a,i);a->laboratory_resize=0;}
            break;
        }
    }
    if(a->laboratory_resize) {
        unsigned i=(unsigned)a->laboratory_resize-1;struct nk_rect r=a->laboratory_splitters[i];
        float value=i==2?(in->mouse.pos.x-a->dock_panels[PS_DOCK_WORKSPACE].x)/fmaxf(1,a->dock_panels[PS_DOCK_WORKSPACE].w):
            laboratory_dimension_value(a,i)+r.y-in->mouse.pos.y;
        laboratory_dimension_set(a,i,value);
    }
}
