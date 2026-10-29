/* Separate OS windows and actual routed gestures. This case retains the
 * existing plot setup, then proves main navigation while an export is open. */
static void test_graphic_window_frame(app *a,int *stage,int *code) {
    static unsigned settle;static graphic_dialog *held;static uint32_t crc;static size_t bytes;
    static int return_tab;bool okay=true;graphic_dialog *d=a->graphic_export;
    if(settle++<2)return;settle=0;
    switch(*stage){
    case 4000:{
        if(!d || d->dirty || !d->raster)return;
        okay=d->window!=a->window && d->ui!=a->ui && d->graphics!=a->graphics;
        okay &= SDL_GetWindowID(d->window)!=SDL_GetWindowID(a->window);
#ifdef _WIN32
        void *native=SDL_GetPointerProperty(SDL_GetWindowProperties(d->window),SDL_PROP_WINDOW_WIN32_HWND_POINTER,NULL);
        void *main=SDL_GetPointerProperty(SDL_GetWindowProperties(a->window),SDL_PROP_WINDOW_WIN32_HWND_POINTER,NULL);
        okay &= native && main && native!=main;
#else
        if(!strcmp(SDL_GetCurrentVideoDriver(),"x11")){
            Sint64 native=SDL_GetNumberProperty(SDL_GetWindowProperties(d->window),SDL_PROP_WINDOW_X11_WINDOW_NUMBER,0);
            Sint64 main=SDL_GetNumberProperty(SDL_GetWindowProperties(a->window),SDL_PROP_WINDOW_X11_WINDOW_NUMBER,0);
            okay &= native && main && native!=main;
        }else okay=false; /* This fixture claims only Windows HWND / X11 proof. */
#endif
        ps_graphic_bytes image={0};okay &= graphic_export_bytes(d,&image)==PS_OK;
        if(image.data){crc=ps_crc32(image.data,image.size);bytes=image.size;}ps_graphic_bytes_free(&image);
        held=d;return_tab=a->tab;
        printf("{\"kind\":\"independent_graphic_window\",\"main_window_id\":%u,\"export_window_id\":%u,\"native_handles_distinct\":%s,\"output_crc\":%u,\"output_bytes\":%zu}\n",SDL_GetWindowID(a->window),SDL_GetWindowID(d->window),okay?"true":"false",crc,bytes);
        test_mouse(a,a->navigation_bounds[0],true);break;
    }
    case 4001:test_mouse(a,a->navigation_bounds[0],false);break;
    case 4002:
        okay=d==held && a->tab==0 && d->window!=a->window;
        test_mouse(a,a->navigation_bounds[2],true);break;
    case 4003:test_mouse(a,a->navigation_bounds[2],false);break;
    case 4004:{
        okay=a->tab==return_tab && d==held;
        ps_graphic_bytes image={0};okay &= graphic_export_bytes(d,&image)==PS_OK && image.size==bytes && ps_crc32(image.data,image.size)==crc;ps_graphic_bytes_free(&image);
        int width,height;SDL_GetWindowSize(a->window,&width,&height);
        okay &= SDL_SetWindowSize(d->window,680,740);
        int main_width,main_height;SDL_GetWindowSize(a->window,&main_width,&main_height);
        okay &= width==main_width && height==main_height;
        break;
    }
    case 4005:
        okay=d==held && d->bounds.x==0 && d->bounds.y==0;
        test_window_key_mod(d->window,SDLK_ESCAPE,0);break;
    case 4006:
        okay=!d && SDL_GetWindowID(a->window)!=0 && a->tab==return_tab;
        if(okay)okay=graphic_export_report(a,3);break;
    case 4007:{
        if(!d || d->dirty || !d->raster)return;
        join(d->capture,sizeof d->capture,a->project,"independent-export.png");
        SDL_Event event={0};event.type=SDL_EVENT_WINDOW_CLOSE_REQUESTED;event.window.windowID=SDL_GetWindowID(d->window);SDL_PushEvent(&event);break;
    }
    case 4008:
        okay=!d && SDL_GetWindowID(a->window)!=0;
        puts(okay?"INDEPENDENT GRAPHIC WINDOW SELF-TEST: PASSED":"INDEPENDENT GRAPHIC WINDOW SELF-TEST: FAILED");a->quitting=true;break;
    }
    if(!okay){fprintf(stderr,"Independent graphic window stage%d failed, tab=%d dialog=%p\n",*stage,a->tab,(void*)d);*code=1;a->quitting=true;}
    (*stage)++;
}
