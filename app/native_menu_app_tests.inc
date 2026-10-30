/* macOS replacements for tests that previously clicked the in-window menus.
 * Commands go through real NSMenu targets and the normal SDL application loop.
 * No production dispatcher is called directly and no old menu is re-enabled. */
#ifdef __APPLE__
#include <objc/message.h>
#include <objc/runtime.h>
extern id const NSEventTrackingRunLoopMode;
#define MT0(T,O,S) ((T(*)(id,SEL))objc_msgSend)((id)(O),sel_registerName(S))
#define MT1(T,O,S,A,V) ((T(*)(id,SEL,A))objc_msgSend)((id)(O),sel_registerName(S),(V))
#define MT2(T,O,S,A,V,B,W) ((T(*)(id,SEL,A,B))objc_msgSend)((id)(O),sel_registerName(S),(V),(W))
#define MT_COMMAND ((uintptr_t)1<<20)
typedef struct {double x,y;} menu_test_point;
static id native_test_string(const char *text){return MT1(id,objc_getClass("NSString"),"stringWithUTF8String:",const char*,text);}
static id native_test_application(void){return MT0(id,objc_getClass("NSApplication"),"sharedApplication");}
static bool native_test_item(app *a,toolbar_action action,id *menu,intptr_t *index){
    if(!a->native_menu_active)return false;
    char title[256]={0};toolbar_entry entries[9];
    for(int group=1;group<=6;group++)for(unsigned i=0,n=toolbar_entries(a,group,entries);i<n;i++)
        if(entries[i].action==action)SDL_strlcpy(title,entries[i].text,sizeof title);
    char *hint=strchr(title,'\t');if(hint)*hint=0;if(!title[0])return false;
    id bar=MT0(id,native_test_application(),"mainMenu");
    for(intptr_t g=0;g<MT0(intptr_t,bar,"numberOfItems");g++){
        id root=MT1(id,bar,"itemAtIndex:",intptr_t,g),group=MT0(id,root,"submenu");
        for(intptr_t i=0;i<MT0(intptr_t,group,"numberOfItems");i++){
            id item=MT1(id,group,"itemAtIndex:",intptr_t,i);const char *text=MT0(const char*,MT0(id,item,"title"),"UTF8String");
            if(text && !strcmp(text,title)){*menu=group;*index=i;return true;}
        }
    }
    return false;
}
static bool native_test_invoke(app *a,toolbar_action action,bool enabled){
    id menu;intptr_t index;if(!native_test_item(a,action,&menu,&index))return false;
    id item=MT1(id,menu,"itemAtIndex:",intptr_t,index);
    if((bool)MT0(BOOL,item,"isEnabled")!=enabled)return false;
    MT1(void,menu,"performActionForItemAtIndex:",intptr_t,index);return true;
}
static bool native_test_key(SDL_Window *window,intptr_t type,const char *text,unsigned short raw,uintptr_t modifiers,BOOL repeat){
    if(SDL_GetKeyboardFocus()!=window)return false;
    id pool=MT0(id,MT0(id,objc_getClass("NSAutoreleasePool"),"alloc"),"init");
    id application=native_test_application(),target=MT0(id,application,"keyWindow");
    id event=((id(*)(id,SEL,intptr_t,menu_test_point,uintptr_t,double,intptr_t,id,id,id,BOOL,unsigned short))objc_msgSend)(
        objc_getClass("NSEvent"),sel_registerName("keyEventWithType:location:modifierFlags:timestamp:windowNumber:context:characters:charactersIgnoringModifiers:isARepeat:keyCode:"),
        type,(menu_test_point){0,0},modifiers,MT0(double,MT0(id,objc_getClass("NSProcessInfo"),"processInfo"),"systemUptime"),
        MT0(intptr_t,target,"windowNumber"),NULL,native_test_string(text),native_test_string(text),repeat,raw);
    if(event)MT2(void,application,"postEvent:atStart:",id,event,BOOL,NO);
    MT0(void,pool,"drain");return event!=NULL;
}
static bool native_test_stroke(SDL_Window *window,const char *text,unsigned short raw,uintptr_t modifiers){
    return native_test_key(window,12,"",55,modifiers,NO) && native_test_key(window,10,text,raw,modifiers,NO) &&
        native_test_key(window,11,text,raw,modifiers,NO) && native_test_key(window,12,"",55,0,NO);
}
/* A real published Edit menu (one Search item), including AppKit's nested
 * tracking loop. The timeout is only an escape hatch; selecting must succeed. */
static bool native_test_track_search(app *a,bool cancel){
    id menu;intptr_t index;if(!native_test_item(a,TOOLBAR_SEARCH,&menu,&index))return false;
    id item=MT1(id,menu,"itemAtIndex:",intptr_t,index);
    if(MT0(intptr_t,menu,"numberOfItems")!=1 || !MT0(BOOL,item,"isEnabled"))return false;
    id pool=MT0(id,MT0(id,objc_getClass("NSAutoreleasePool"),"alloc"),"init");
    id modes=MT1(id,objc_getClass("NSArray"),"arrayWithObject:",id,NSEventTrackingRunLoopMode);
    ((void(*)(id,SEL,SEL,id,double,id))objc_msgSend)(menu,sel_registerName("performSelector:withObject:afterDelay:inModes:"),sel_registerName("cancelTracking"),NULL,2.,modes);
    bool queued=native_test_stroke(a->window,cancel?"x":"\xef\x9c\x81",cancel?7:125,0) &&
        native_test_stroke(a->window,cancel?"\x1b":"\r",cancel?53:36,0);
    id view=MT0(id,MT0(id,native_test_application(),"keyWindow"),"contentView");
    BOOL selected=queued?((BOOL(*)(id,SEL,id,menu_test_point,id))objc_msgSend)(menu,sel_registerName("popUpMenuPositioningItem:atLocation:inView:"),item,(menu_test_point){40,40},view):NO;
    ((void(*)(id,SEL,id,SEL,id))objc_msgSend)(objc_getClass("NSObject"),sel_registerName("cancelPreviousPerformRequestsWithTarget:selector:object:"),menu,sel_registerName("cancelTracking"),NULL);
    MT0(void,pool,"drain");return queued && (bool)selected==!cancel;
}
static bool native_test_shell(app *a){
    if(!a->native_menu_active || !a->native_menu || a->toolbar_menu || a->toolbar_keyboard)return false;
    for(unsigned i=0;i<SDL_arraysize(toolbar_top_order);i++)if(a->toolbar_bounds[toolbar_top_order[i]].w!=0)return false;
    return a->navigation_bounds[0].w>0 && a->navigation_bounds[0].y==a->navigation_bounds[2].y;
}
static bool native_test_focus_wait(app *a,SDL_Window *window,int *stage,double deadline){
    if(SDL_GetKeyboardFocus()==window)return true;
    if(ps_clock()<deadline){--*stage;return false;}
    fprintf(stderr,"Native menu focus timeout: wanted=%u actual=%u\n",SDL_GetWindowID(window),SDL_GetKeyboardFocus()?SDL_GetWindowID(SDL_GetKeyboardFocus()):0);
    (void)a;return true; /* Let the caller's focus assertion fail. */
}
static void native_toolbar_test_frame(app *a,int *stage,int *code,const char **capture){
    static double deadline;static bool log_before;bool ok=true;(void)capture;
    switch((*stage)++){
    case 0:SDL_RaiseWindow(a->window);deadline=ps_clock()+5;break;
    case 1:{
        if(!native_test_focus_wait(a,a->window,stage,deadline))return;
        int width,height;SDL_GetWindowSize(a->window,&width,&height);
        SDL_Point drag={(int)a->window_drag_bounds.x+20,15},control={(int)a->window_control_bounds[1].x+20,15},edge={1,height/2};
        ok=SDL_GetKeyboardFocus()==a->window && native_test_shell(a) &&
            fabsf(a->navigation_bounds[0].w-((float)width-(workspace_shell_visible(a)?32:0))/3)<=1 &&
            window_hit_test(a->window,&drag,a)==SDL_HITTEST_DRAGGABLE && window_hit_test(a->window,&control,a)==SDL_HITTEST_NORMAL &&
            window_hit_test(a->window,&edge,a)==SDL_HITTEST_RESIZE_LEFT && native_test_invoke(a,TOOLBAR_NEW,true);break;
    }
    case 2:ok=a->project_manager && !a->loaded;test_mouse(a,a->navigation_bounds[1],true);break;
    case 3:test_mouse(a,a->navigation_bounds[1],false);break;
    case 4:ok=a->tab==1 && a->project_manager;test_mouse(a,a->navigation_bounds[2],true);break;
    case 5:test_mouse(a,a->navigation_bounds[2],false);break;
    case 6:ok=a->tab==2 && a->project_manager;manager_test_key(a,SDLK_ESCAPE,0);break;
    case 7:ok=!a->project_manager;SDL_RaiseWindow(a->window);deadline=ps_clock()+5;break;
    case 8:if(!native_test_focus_wait(a,a->window,stage,deadline))return;log_before=a->show_log;ok=native_test_invoke(a,TOOLBAR_LOG,true);break;
    case 9:ok=a->show_log!=log_before && native_test_invoke(a,TOOLBAR_LOG,true);break;
    case 10:ok=a->show_log==log_before && native_test_invoke(a,TOOLBAR_SETTINGS,true);break;
    case 11:ok=a->tab==6 && native_test_stroke(a->window,"1",18,MT_COMMAND);break;
    case 12:ok=a->tab==0 && native_test_invoke(a,TOOLBAR_HELP,true);break;
    case 13:ok=a->doc_visible && a->doc_window && a->doc_window!=a->window;documentation_window_hide(a);SDL_RaiseWindow(a->window);deadline=ps_clock()+5;break;
    case 14:if(!native_test_focus_wait(a,a->window,stage,deadline))return;ok=native_test_invoke(a,TOOLBAR_ADD_FILE,false);break;
    case 15:ok=!a->dialog_pending && !a->workspace_open;test_mouse(a,a->window_control_bounds[1],true);deadline=ps_clock()+5;break;
    case 16:test_mouse(a,a->window_control_bounds[1],false);break;
    case 17:
        if(!(SDL_GetWindowFlags(a->window)&SDL_WINDOW_MAXIMIZED) && ps_clock()<deadline){--*stage;return;}
        ok=(SDL_GetWindowFlags(a->window)&SDL_WINDOW_MAXIMIZED)!=0;test_mouse(a,a->window_control_bounds[1],true);deadline=ps_clock()+5;break;
    case 18:test_mouse(a,a->window_control_bounds[1],false);break;
    case 19:
        if((SDL_GetWindowFlags(a->window)&SDL_WINDOW_MAXIMIZED) && ps_clock()<deadline){--*stage;return;}
        ok=!(SDL_GetWindowFlags(a->window)&SDL_WINDOW_MAXIMIZED);test_mouse(a,a->window_control_bounds[0],true);deadline=ps_clock()+5;break;
    case 20:test_mouse(a,a->window_control_bounds[0],false);break;
    case 21:
        if(!(SDL_GetWindowFlags(a->window)&SDL_WINDOW_MINIMIZED) && ps_clock()<deadline){--*stage;return;}
        ok=(SDL_GetWindowFlags(a->window)&SDL_WINDOW_MINIMIZED)!=0;SDL_RestoreWindow(a->window);SDL_RaiseWindow(a->window);deadline=ps_clock()+5;break;
    case 22:
        if(!native_test_focus_wait(a,a->window,stage,deadline))return;
        ok=SDL_GetKeyboardFocus()==a->window && !(SDL_GetWindowFlags(a->window)&SDL_WINDOW_MINIMIZED) && native_test_shell(a);
        test_mouse(a,a->window_control_bounds[2],true);break;
    case 23:test_mouse(a,a->window_control_bounds[2],false);break;
    case 24:break; /* Allow the close event posted by the release frame. */
    case 25:ok=false;break; /* The real close button must already have quit. */
    }
    if(!ok){fprintf(stderr,"Native toolbar failure stage=%d tab=%d manager=%d\n",*stage-1,a->tab,a->project_manager);*code=1;a->quitting=true;}
    if(a->quitting && *stage<24){*code=1;fprintf(stderr,"Native toolbar quit before the close-button stage\n");}
    if(a->quitting)printf("TOOLBAR AND TABS: %s (native AppKit, stage %d)\n",*code?"FAILED":"PASSED",*stage);
}
static void native_keyboard_menu_test_frame(app *a,const char *root,int *stage,int *code,const char **capture){
    static double deadline;static bool log_before;static int length;bool ok=true;(void)capture;
    switch((*stage)++){
    case 0:SDL_RaiseWindow(a->window);deadline=ps_clock()+5;break;
    case 1:
        if(!native_test_focus_wait(a,a->window,stage,deadline))return;
        log_before=a->show_log;ok=SDL_GetKeyboardFocus()==a->window && native_test_shell(a) && native_test_invoke(a,TOOLBAR_SAVE,false);
        ok &= native_test_key(a->window,12,"",55,MT_COMMAND,NO) && native_test_key(a->window,10,"l",37,MT_COMMAND,NO);break;
    case 2:ok=a->show_log!=log_before && !a->document_count;ok &= native_test_key(a->window,10,"l",37,MT_COMMAND,YES);break;
    case 3:ok=a->show_log!=log_before;ok &= native_test_key(a->window,11,"l",37,MT_COMMAND,NO) && native_test_key(a->window,12,"",55,0,NO);break;
    case 4:ok=native_test_stroke(a->window,",",43,MT_COMMAND);break;
    case 5:ok=a->tab==6 && native_test_stroke(a->window,"1",18,MT_COMMAND);break;
    case 6:ok=a->tab==0 && native_test_stroke(a->window,"\xef\x9c\x84",122,0);break;
    case 7:ok=a->doc_visible;documentation_window_hide(a);SDL_RaiseWindow(a->window);deadline=ps_clock()+5;break;
    case 8:if(!native_test_focus_wait(a,a->window,stage,deadline))return;ok=native_test_invoke(a,TOOLBAR_NEW,true);break;
    case 9:ok=a->project_manager;manager_test_key(a,SDLK_ESCAPE,0);break;
    case 10:
        ok=!a->project_manager && document_test_start(a,root);
        if(ok)preview_workspace_path(a,a->workspace_tree_test_path);SDL_RaiseWindow(a->window);deadline=ps_clock()+5;break;
    case 11:
        if((a->library_thread || !idle(a)) && ps_clock()<deadline){--*stage;return;}
        if(!native_test_focus_wait(a,a->window,stage,deadline))return;
        ok=active_document(a) && a->tab==8;test_mouse(a,a->document_bounds[6],true);break;
    case 12:test_mouse(a,a->document_bounds[6],false);break;
    case 13:
        ok=active_document(a) && active_document(a)->edit.active;
        if(ok){length=nk_str_len_char(&active_document(a)->edit.string);ok=native_test_track_search(a,false);}break;
    case 14:
        ok=a->show_search && nk_str_len_char(&active_document(a)->edit.string)==length;
        ok &= native_test_stroke(a->window,"\x1b",53,0);break;
    case 15:
        ok=!a->show_search && nk_str_len_char(&active_document(a)->edit.string)==length && native_test_track_search(a,true);break;
    case 16:ok=!a->show_search && nk_str_len_char(&active_document(a)->edit.string)==length && native_test_stroke(a->window,"z",6,0);break;
    case 17:
        ok=nk_str_len_char(&active_document(a)->edit.string)==length+1 && active_document(a)->dirty && native_test_stroke(a->window,"s",1,MT_COMMAND);break;
    case 18:ok=!active_document(a)->dirty && native_test_stroke(a->window,"q",12,0);break;
    case 19:ok=active_document(a)->dirty && native_test_stroke(a->window,"\xef\x9c\x84",122,0);deadline=ps_clock()+5;break;
    case 20:
        if(!native_test_focus_wait(a,a->doc_window,stage,deadline))return;
        ok=a->doc_visible && native_test_invoke(a,TOOLBAR_SAVE,false) && native_test_stroke(a->doc_window,"s",1,MT_COMMAND);break;
    case 21:
        ok=active_document(a)->dirty && a->doc_visible;documentation_window_hide(a);SDL_RaiseWindow(a->window);deadline=ps_clock()+5;break;
    case 22:if(!native_test_focus_wait(a,a->window,stage,deadline))return;ok=native_test_stroke(a->window,"s",1,MT_COMMAND);break;
    case 23:ok=!active_document(a)->dirty && native_test_shell(a);SDL_HideWindow(a->window);deadline=ps_clock()+5;break;
    case 24:
        if(SDL_GetKeyboardFocus()==a->window && ps_clock()<deadline){--*stage;return;}
        ok=SDL_GetKeyboardFocus()!=a->window && !a->toolbar_keyboard && !a->toolbar_menu;
        SDL_ShowWindow(a->window);SDL_RaiseWindow(a->window);deadline=ps_clock()+5;break;
    case 25:
        if(!native_test_focus_wait(a,a->window,stage,deadline))return;
        ok=SDL_GetKeyboardFocus()==a->window && native_test_shell(a) && !active_document(a)->dirty;
        a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Native keyboard menu failure stage=%d tab=%d search=%d doc=%d\n",*stage-1,a->tab,a->show_search,a->doc_visible);*code=1;a->quitting=true;}
    if(a->quitting && *stage<26){*code=1;fprintf(stderr,"Native keyboard workflow quit before completion\n");}
    if(a->quitting)printf("KEYBOARD MENU SELF-TEST: %s (native AppKit, stage %d)\n",*code?"FAILED":"PASSED",*stage);
}
#undef MT_COMMAND
#undef MT2
#undef MT1
#undef MT0
#endif
