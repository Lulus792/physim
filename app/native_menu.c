#include "native_menu.h"
#include <stdlib.h>
#include <string.h>
#ifdef __APPLE__
#include <objc/message.h>
#include <objc/runtime.h>
#include <stdint.h>
extern id const NSMenuDidBeginTrackingNotification;
extern id const NSMenuDidEndTrackingNotification;

/* Typed Objective-C sends, as in the existing C17 accessibility adapter. */
#define N0(T,O,S) ((T(*)(id,SEL))objc_msgSend)((id)(O),sel_registerName(S))
#define N1(T,O,S,A,V) ((T(*)(id,SEL,A))objc_msgSend)((id)(O),sel_registerName(S),(V))
#define N2(T,O,S,A,V,B,W) ((T(*)(id,SEL,A,B))objc_msgSend)((id)(O),sel_registerName(S),(V),(W))
static id menu_string(const char *text) {return N1(id,objc_getClass("NSString"),"stringWithUTF8String:",const char*,text);}
static void menu_release(id value) {if(value)N0(void,value,"release");}
static id menu_new(const char *name) {return N0(id,N0(id,objc_getClass(name),"alloc"),"init");}

typedef struct {
    id item;unsigned command;SDL_Keycode key;SDL_Keymod modifiers;bool enabled;
    char title[256];
} menu_slot;
struct ps_native_menu {
    id bar,receiver,roots[PS_NATIVE_MENU_GROUPS],groups[PS_NATIVE_MENU_GROUPS];
    id settings,settings_target,settings_title,settings_key,old_help;
    SEL settings_action;intptr_t settings_modifiers,settings_tag;BOOL settings_enabled;
    size_t counts[PS_NATIVE_MENU_GROUPS];bool application[PS_NATIVE_MENU_GROUPS],help[PS_NATIVE_MENU_GROUPS];
    size_t group_count,slot_count;menu_slot slots[PS_NATIVE_MENU_ITEMS];
    SDL_WindowID window;Uint32 event_type;unsigned tracking;bool tracking_marker;
    SDL_KeyboardEvent consumed[PS_NATIVE_MENU_ITEMS];size_t consumed_count;
};
static Class menu_receiver_class;static Ivar menu_owner_ivar;
static id menu_key_string(SDL_Keycode key);
static ps_native_menu *menu_owner(id receiver) {ps_native_menu *m=NULL;memcpy(&m,(char*)receiver+ivar_getOffset(menu_owner_ivar),sizeof m);return m;}
static void menu_set_owner(id receiver,ps_native_menu *m) {memcpy((char*)receiver+ivar_getOffset(menu_owner_ivar),&m,sizeof m);}

/* AppKit's nested tracking loop blocks our SDL consumer. A queue marker owns
 * only input queued during tracking; earlier editor input and app commands stay
 * in their original order. No uptime/SDL timestamp conversion is involved. */
typedef struct {ps_native_menu *menu;bool inside;} menu_tracking_filter;
static bool SDLCALL menu_remove_tracking_input(void *data,SDL_Event *event) {
    menu_tracking_filter *filter=data;
    if(event->type==filter->menu->event_type+1){filter->inside=true;return false;}
    bool input=event->type==SDL_EVENT_KEY_DOWN || event->type==SDL_EVENT_KEY_UP ||
        event->type==SDL_EVENT_TEXT_INPUT || event->type==SDL_EVENT_TEXT_EDITING ||
        event->type==SDL_EVENT_MOUSE_BUTTON_DOWN || event->type==SDL_EVENT_MOUSE_BUTTON_UP || event->type==SDL_EVENT_MOUSE_WHEEL;
    if(filter->inside && input){ps_native_menu_consumes_key(filter->menu,event);return false;}
    return true;
}
static void menu_begin_tracking(id receiver,SEL selector,id notification) {
    (void)selector;(void)notification;ps_native_menu *m=menu_owner(receiver);if(!m || m->tracking++)return;
    SDL_Event marker;SDL_zero(marker);marker.type=m->event_type+1;m->tracking_marker=SDL_PushEvent(&marker);
}
static void menu_end_tracking(id receiver,SEL selector,id notification) {
    (void)selector;(void)notification;ps_native_menu *m=menu_owner(receiver);if(!m || !m->tracking || --m->tracking)return;
    if(m->tracking_marker){menu_tracking_filter filter={m,false};SDL_FilterEvents(menu_remove_tracking_input,&filter);m->tracking_marker=false;}
}

typedef struct {Uint16 raw;SDL_WindowID window;bool found;SDL_KeyboardEvent key;} menu_key_probe;
static bool SDLCALL menu_observe_key(void *data,SDL_Event *event) {
    menu_key_probe *p=data;
    if(event->type==SDL_EVENT_KEY_DOWN && event->key.windowID==p->window && event->key.raw==p->raw){p->key=event->key;p->found=true;}
    return true; /* Observation preserves order and every queued event. */
}
static void menu_invoke(id receiver,SEL selector,id sender) {
    (void)selector;ps_native_menu *m=menu_owner(receiver);if(!m || !SDL_IsMainThread())return;
    intptr_t index=N0(intptr_t,sender,"tag");
    if(index<0 || (size_t)index>=m->slot_count)return;menu_slot *slot=&m->slots[index];
    if(slot->item!=sender || !slot->enabled)return;
    SDL_Window *focused=SDL_GetKeyboardFocus();SDL_WindowID window=focused?SDL_GetWindowID(focused):0;
    if(window!=m->window)return;
    id application=N0(id,objc_getClass("NSApplication"),"sharedApplication");
    id event=N0(id,application,"currentEvent");bool keyboard=event && N0(intptr_t,event,"type")==10; /* NSEventTypeKeyDown */
    menu_key_probe probe={0};
    id characters=keyboard?N0(id,event,"charactersIgnoringModifiers"):NULL;
    bool equivalent=characters && slot->key && N1(BOOL,N0(id,characters,"lowercaseString"),"isEqualToString:",id,menu_key_string(slot->key));
    if(equivalent) {
        probe.raw=N0(unsigned short,event,"keyCode");probe.window=window;SDL_FilterEvents(menu_observe_key,&probe);
        /* A full suppression table falls back to the existing SDL shortcut.
         * Never post a second command without being able to own its key. */
        if(probe.found && m->consumed_count==PS_NATIVE_MENU_ITEMS)return;
        if(N0(BOOL,event,"isARepeat")) {
            if(probe.found)m->consumed[m->consumed_count++]=probe.key;
            return;
        }
    }
    SDL_Event action;SDL_zero(action);action.type=m->event_type;action.user.windowID=window;action.user.code=(Sint32)slot->command;action.user.data1=(void*)(uintptr_t)equivalent;
    if(SDL_PushEvent(&action) && probe.found)m->consumed[m->consumed_count++]=probe.key;
}
static bool menu_class_prepare(void) {
    if(menu_receiver_class)return true;
    Class c=objc_allocateClassPair(objc_getClass("NSObject"),"PhysimMainMenuReceiver",0);if(!c)return false;
    uint8_t alignment=0;while(((size_t)1<<alignment)<_Alignof(void*))alignment++;
    if(!class_addIvar(c,"_physimMenu",sizeof(void*),alignment,"^v") ||
       !class_addMethod(c,sel_registerName("physimMenuInvoke:"),(IMP)menu_invoke,"v@:@") ||
       !class_addMethod(c,sel_registerName("physimMenuBegin:"),(IMP)menu_begin_tracking,"v@:@") ||
       !class_addMethod(c,sel_registerName("physimMenuEnd:"),(IMP)menu_end_tracking,"v@:@")){objc_disposeClassPair(c);return false;}
    objc_registerClassPair(c);menu_receiver_class=c;menu_owner_ivar=class_getInstanceVariable(c,"_physimMenu");return menu_owner_ivar!=NULL;
}
static id menu_make_item(const char *title) {
    id item=N0(id,objc_getClass("NSMenuItem"),"alloc");
    return ((id(*)(id,SEL,id,SEL,id))objc_msgSend)(item,sel_registerName("initWithTitle:action:keyEquivalent:"),menu_string(title),NULL,menu_string(""));
}
static id menu_make_group(const char *title) {
    id menu=N1(id,N0(id,objc_getClass("NSMenu"),"alloc"),"initWithTitle:",id,menu_string(title));
    N1(void,menu,"setAutoenablesItems:",BOOL,NO);return menu;
}
static id menu_key_string(SDL_Keycode key) {
    if((key>=SDLK_F1 && key<=SDLK_F12) || (key>=SDLK_F13 && key<=SDLK_F24)) {
        unsigned short code=(unsigned short)(key<=SDLK_F12?0xf704+(key-SDLK_F1):0xf710+(key-SDLK_F13));
        return N2(id,objc_getClass("NSString"),"stringWithCharacters:length:",const unsigned short*,&code,uintptr_t,1);
    }
    char text[2]={key>=32 && key<127?(char)key:0,0};return menu_string(text);
}
static uintptr_t menu_modifiers(SDL_Keymod modifiers) {
    return ((modifiers&SDL_KMOD_SHIFT)?(UINT32_C(1)<<17):0)|((modifiers&SDL_KMOD_CTRL)?(UINT32_C(1)<<18):0)|
           ((modifiers&SDL_KMOD_ALT)?(UINT32_C(1)<<19):0)|((modifiers&SDL_KMOD_GUI)?(UINT32_C(1)<<20):0);
}
ps_native_menu *ps_native_menu_create(void) {
    if(!SDL_IsMainThread() || !menu_class_prepare())return NULL;
    id application=N0(id,objc_getClass("NSApplication"),"sharedApplication"),bar=N0(id,application,"mainMenu");
    if(!bar || N0(intptr_t,bar,"numberOfItems")<1)return NULL;
    ps_native_menu *m=calloc(1,sizeof *m);if(!m)return NULL;
    m->event_type=SDL_RegisterEvents(2);if(!m->event_type){free(m);return NULL;}
    m->bar=N0(id,bar,"retain");m->receiver=menu_new("PhysimMainMenuReceiver");
    if(!m->receiver){menu_release(m->bar);free(m);return NULL;}
    menu_set_owner(m->receiver,m);m->old_help=N0(id,N0(id,application,"helpMenu"),"retain");
    id center=N0(id,objc_getClass("NSNotificationCenter"),"defaultCenter");
    ((void(*)(id,SEL,id,SEL,id,id))objc_msgSend)(center,sel_registerName("addObserver:selector:name:object:"),m->receiver,sel_registerName("physimMenuBegin:"),NSMenuDidBeginTrackingNotification,NULL);
    ((void(*)(id,SEL,id,SEL,id,id))objc_msgSend)(center,sel_registerName("addObserver:selector:name:object:"),m->receiver,sel_registerName("physimMenuEnd:"),NSMenuDidEndTrackingNotification,NULL);
    return m;
}
static void menu_clear(ps_native_menu *m) {
    for(size_t i=0;i<m->group_count;i++)if(m->roots[i]){N1(void,m->bar,"removeItem:",id,m->roots[i]);menu_release(m->roots[i]);}
    memset(m->roots,0,sizeof m->roots);memset(m->groups,0,sizeof m->groups);m->group_count=m->slot_count=0;
}
bool ps_native_menu_publish(ps_native_menu *m,const ps_native_menu_group *groups,size_t count,SDL_WindowID window) {
    if(!m || !SDL_IsMainThread() || !groups || !count || count>PS_NATIVE_MENU_GROUPS || !groups[0].application)return false;
    size_t total=0;
    for(size_t g=0;g<count;g++) {
        if(!groups[g].title || !groups[g].items || (g && groups[g].application) || (groups[g].application && groups[g].count!=1))return false;
        if(groups[g].count>PS_NATIVE_MENU_ITEMS-total)return false;total+=groups[g].count;
        for(size_t i=0;i<groups[g].count;i++)if(!groups[g].items[i].title || strlen(groups[g].items[i].title)>=256)return false;
    }
    id pool=menu_new("NSAutoreleasePool"),application=N0(id,objc_getClass("NSApplication"),"sharedApplication");
    if(!pool)return false;
    for(size_t g=0;g<count;g++) {
        if(strlen(groups[g].title)>=64 || !menu_string(groups[g].title)){N0(void,pool,"drain");return false;}
        for(size_t i=0;i<groups[g].count;i++)if(!menu_string(groups[g].items[i].title)){N0(void,pool,"drain");return false;}
    }
    bool initialize=!m->group_count;
    if(!initialize && (m->group_count!=count || m->slot_count!=total)){N0(void,pool,"drain");return false;}
    if(!initialize)for(size_t g=0;g<count;g++)if(m->counts[g]!=groups[g].count || m->application[g]!=groups[g].application || m->help[g]!=groups[g].help){N0(void,pool,"drain");return false;}
    size_t slot=0;
    for(size_t g=0;g<count;g++) {
        const ps_native_menu_group *group=&groups[g];id menu=m->groups[g];
        if(initialize) {
            m->group_count=g+1; /* Partial construction is owned by destroy. */
            m->counts[g]=group->count;m->application[g]=group->application;m->help[g]=group->help;
            if(group->application) {
                id root=N1(id,m->bar,"itemAtIndex:",intptr_t,0);menu=N0(id,root,"submenu");
            } else {
                menu=menu_make_group(group->title);id root=menu_make_item(group->title);
                if(!menu || !root){menu_release(menu);menu_release(root);N0(void,pool,"drain");return false;}
                N1(void,root,"setSubmenu:",id,menu);menu_release(menu);
                m->roots[g]=root;
                if(group->help)N1(void,m->bar,"addItem:",id,root);
                else N2(void,m->bar,"insertItem:atIndex:",id,root,intptr_t,(intptr_t)g);
                if(group->help)N1(void,application,"setHelpMenu:",id,menu);
            }
            m->groups[g]=menu;
        }
        for(size_t i=0;i<group->count;i++,slot++) {
            const ps_native_menu_item *input=&group->items[i];menu_slot *saved=&m->slots[slot];id item=saved->item;
            if(initialize) {
                if(group->application) {
                    for(intptr_t n=0;n<N0(intptr_t,menu,"numberOfItems");n++) {
                        id candidate=N1(id,menu,"itemAtIndex:",intptr_t,n);
                        if(!N0(SEL,candidate,"action") && N1(BOOL,N0(id,candidate,"keyEquivalent"),"isEqualToString:",id,menu_string(","))){item=candidate;break;}
                    }
                    if(!item){N0(void,pool,"drain");return false;}
                    m->settings=N0(id,item,"retain");m->settings_target=N0(id,N0(id,item,"target"),"retain");m->settings_action=N0(SEL,item,"action");
                    m->settings_title=N0(id,N0(id,item,"title"),"retain");m->settings_modifiers=N0(intptr_t,item,"keyEquivalentModifierMask");m->settings_enabled=N0(BOOL,item,"isEnabled");
                    m->settings_key=N0(id,N0(id,item,"keyEquivalent"),"retain");m->settings_tag=N0(intptr_t,item,"tag");
                } else {
                    if(input->separator_before)N1(void,menu,"addItem:",id,N0(id,objc_getClass("NSMenuItem"),"separatorItem"));
                    item=menu_make_item(input->title);if(!item){N0(void,pool,"drain");return false;}
                    N1(void,menu,"addItem:",id,item);menu_release(item);
                }
                saved->item=item;m->slot_count=slot+1;
                N1(void,item,"setTarget:",id,m->receiver);N1(void,item,"setAction:",SEL,sel_registerName("physimMenuInvoke:"));N1(void,item,"setTag:",intptr_t,(intptr_t)slot);
            }
            if(initialize || strcmp(saved->title,input->title)){N1(void,item,"setTitle:",id,menu_string(input->title));SDL_strlcpy(saved->title,input->title,sizeof saved->title);}
            if(initialize || saved->key!=input->key)N1(void,item,"setKeyEquivalent:",id,menu_key_string(input->key));
            if(initialize || saved->modifiers!=input->modifiers)N1(void,item,"setKeyEquivalentModifierMask:",uintptr_t,menu_modifiers(input->modifiers));
            N1(void,item,"setEnabled:",BOOL,input->enabled?YES:NO);
            saved->command=input->command;saved->enabled=input->enabled;saved->key=input->key;saved->modifiers=input->modifiers;
        }
    }
    m->group_count=count;m->slot_count=total;m->window=window;N0(void,pool,"drain");return true;
}
bool ps_native_menu_event(const ps_native_menu *m,const SDL_Event *e,unsigned *command,SDL_WindowID *window,bool *shortcut) {
    if(!m || !e || e->type!=m->event_type)return false;
    if(command)*command=(unsigned)e->user.code;if(window)*window=e->user.windowID;if(shortcut)*shortcut=e->user.data1!=NULL;return true;
}
bool ps_native_menu_consumes_key(ps_native_menu *m,const SDL_Event *event) {
    if(!m || !event || event->type!=SDL_EVENT_KEY_DOWN)return false;
    for(size_t i=0;i<m->consumed_count;i++) {
        SDL_KeyboardEvent *key=&m->consumed[i];
        if(key->timestamp==event->key.timestamp && key->windowID==event->key.windowID && key->raw==event->key.raw && key->key==event->key.key) {
            memmove(key,key+1,(m->consumed_count-i-1)*sizeof *key);m->consumed_count--;return true;
        }
    }
    return false;
}
void ps_native_menu_destroy(ps_native_menu *m) {
    if(!m || !SDL_IsMainThread())return;menu_set_owner(m->receiver,NULL);
    N1(void,N0(id,objc_getClass("NSNotificationCenter"),"defaultCenter"),"removeObserver:",id,m->receiver);
    for(size_t i=0;i<m->slot_count;i++)if(m->slots[i].item!=m->settings) {
        N1(void,m->slots[i].item,"setTarget:",id,NULL);N1(void,m->slots[i].item,"setAction:",SEL,NULL);
    }
    if(m->settings) {
        N1(void,m->settings,"setTarget:",id,m->settings_target);N1(void,m->settings,"setAction:",SEL,m->settings_action);
        N1(void,m->settings,"setTitle:",id,m->settings_title);N1(void,m->settings,"setKeyEquivalentModifierMask:",intptr_t,m->settings_modifiers);N1(void,m->settings,"setEnabled:",BOOL,m->settings_enabled);
        N1(void,m->settings,"setKeyEquivalent:",id,m->settings_key);N1(void,m->settings,"setTag:",intptr_t,m->settings_tag);
    }
    id application=N0(id,objc_getClass("NSApplication"),"sharedApplication");
    for(size_t g=0;g<m->group_count;g++)if(m->help[g] && N0(id,application,"helpMenu")==m->groups[g])N1(void,application,"setHelpMenu:",id,m->old_help);
    menu_clear(m);menu_release(m->old_help);menu_release(m->settings_key);menu_release(m->settings_title);menu_release(m->settings_target);menu_release(m->settings);menu_release(m->receiver);menu_release(m->bar);free(m);
}
#else
struct ps_native_menu {unsigned unused;};
ps_native_menu *ps_native_menu_create(void) {return NULL;}
bool ps_native_menu_publish(ps_native_menu *m,const ps_native_menu_group *g,size_t n,SDL_WindowID w){(void)m;(void)g;(void)n;(void)w;return false;}
bool ps_native_menu_event(const ps_native_menu *m,const SDL_Event *e,unsigned *c,SDL_WindowID *w,bool *s){(void)m;(void)e;(void)c;(void)w;(void)s;return false;}
bool ps_native_menu_consumes_key(ps_native_menu *m,const SDL_Event *e){(void)m;(void)e;return false;}
void ps_native_menu_destroy(ps_native_menu *m) {(void)m;}
#endif
