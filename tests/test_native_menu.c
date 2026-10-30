#include "native_menu.h"
#include <stdio.h>
#ifdef __APPLE__
#include <objc/message.h>
#include <objc/runtime.h>
extern id const NSMenuDidBeginTrackingNotification;
extern id const NSMenuDidEndTrackingNotification;
#define M0(T,O,S) ((T(*)(id,SEL))objc_msgSend)((id)(O),sel_registerName(S))
#define M1(T,O,S,A,V) ((T(*)(id,SEL,A))objc_msgSend)((id)(O),sel_registerName(S),(V))
#define M2(T,O,S,A,V,B,W) ((T(*)(id,SEL,A,B))objc_msgSend)((id)(O),sel_registerName(S),(V),(W))
#define REQUIRE(X) do {if(!(X)){fprintf(stderr,"native_menu:%d: %s (%s)\n",__LINE__,#X,SDL_GetError());return 1;}} while(0)
static id string(const char *s){return M1(id,objc_getClass("NSString"),"stringWithUTF8String:",const char*,s);}
static bool title(id object,const char *expected){return M1(BOOL,M0(id,object,"title"),"isEqualToString:",id,string(expected));}
static bool focus(SDL_Window *window){
    SDL_RaiseWindow(window);
    Uint64 end=SDL_GetTicks()+5000;
    while(SDL_GetKeyboardFocus()!=window && SDL_GetTicks()<end){SDL_PumpEvents();SDL_Delay(10);}
    return SDL_GetKeyboardFocus()==window;
}
static unsigned commands(ps_native_menu *menu,unsigned expected,SDL_WindowID owner,unsigned *consumed,bool *shortcut){
    SDL_Event event;unsigned count=0;
    while(SDL_PollEvent(&event)){
        if(ps_native_menu_consumes_key(menu,&event)){if(consumed)(*consumed)++;continue;}
        unsigned command;SDL_WindowID window;bool key;
        if(ps_native_menu_event(menu,&event,&command,&window,&key)){
            if(command!=expected || window!=owner){fprintf(stderr,"Wrong native command %u/%u\n",command,window);return 999;}
            if(shortcut)*shortcut=key;count++;
        }
    }
    return count;
}
static void invoke(id menu,intptr_t index){M1(void,menu,"performActionForItemAtIndex:",intptr_t,index);}
static void notification(id name){
    M2(void,M0(id,objc_getClass("NSNotificationCenter"),"defaultCenter"),"postNotificationName:object:",id,name,id,NULL);
}
typedef struct {double x,y;} point;
static void post_key(id application,id window,intptr_t type,const char *characters,unsigned short raw,uintptr_t modifiers,BOOL repeat){
    id event=((id(*)(id,SEL,intptr_t,point,uintptr_t,double,intptr_t,id,id,id,BOOL,unsigned short))objc_msgSend)(
        objc_getClass("NSEvent"),sel_registerName("keyEventWithType:location:modifierFlags:timestamp:windowNumber:context:characters:charactersIgnoringModifiers:isARepeat:keyCode:"),
        type,(point){0,0},modifiers,M0(double,M0(id,objc_getClass("NSProcessInfo"),"processInfo"),"systemUptime"),M0(intptr_t,window,"windowNumber"),NULL,string(characters),string(characters),repeat,raw);
    M2(void,application,"postEvent:atStart:",id,event,BOOL,NO);
}
int main(void){
    REQUIRE(SDL_Init(SDL_INIT_VIDEO));
    SDL_Window *window=SDL_CreateWindow("Physim native menu contract",640,400,0);REQUIRE(window);REQUIRE(focus(window));
    id application=M0(id,objc_getClass("NSApplication"),"sharedApplication"),bar=M0(id,application,"mainMenu");
    id app_root=M1(id,bar,"itemAtIndex:",intptr_t,0),app_menu=M0(id,app_root,"submenu"),settings=NULL;
    intptr_t original_count=M0(intptr_t,bar,"numberOfItems");id original_last=M1(id,bar,"itemAtIndex:",intptr_t,original_count-1);
    for(intptr_t n=0;n<M0(intptr_t,app_menu,"numberOfItems");n++){
        id item=M1(id,app_menu,"itemAtIndex:",intptr_t,n);
        if(!M0(SEL,item,"action") && M1(BOOL,M0(id,item,"keyEquivalent"),"isEqualToString:",id,string(","))){settings=item;break;}
    }
    REQUIRE(settings);id settings_title=M0(id,M0(id,settings,"title"),"retain"),old_help=M0(id,application,"helpMenu");
    intptr_t old_tag=M0(intptr_t,settings,"tag");BOOL old_enabled=M0(BOOL,settings,"isEnabled");
    ps_native_menu_item items[]={
        {100,"Einstellungen …",SDLK_COMMA,SDL_KMOD_GUI,true,false},
        {101,"Speichern",SDLK_S,SDL_KMOD_GUI,true,false},{102,"Nicht verfügbar",0,0,false,true},
        {103,"Suchen",SDLK_F,SDL_KMOD_GUI,true,false},{104,"Protokoll",SDLK_L,SDL_KMOD_GUI,true,false},
        {105,"Bauen",SDLK_F5,0,true,false},{106,"Handbuch",SDLK_F1,0,true,false}
    };
    ps_native_menu_group groups[]={
        {"Physim",items,1,true,false},{"Datei",items+1,2,false,false},{"Bearbeiten",items+3,1,false,false},
        {"Ansicht",items+4,1,false,false},{"Build",items+5,1,false,false},{"Hilfe",items+6,1,false,true}
    };
    ps_native_menu *menu=ps_native_menu_create();REQUIRE(menu);
    REQUIRE(ps_native_menu_publish(menu,groups,6,SDL_GetWindowID(window)));
    REQUIRE(M0(intptr_t,bar,"numberOfItems")==original_count+5);
    REQUIRE(M1(id,bar,"itemAtIndex:",intptr_t,0)==app_root);
    REQUIRE(M1(id,bar,"itemAtIndex:",intptr_t,original_count+3)==original_last);
    const char *names[]={"Datei","Bearbeiten","Ansicht","Build"};
    for(intptr_t n=1;n<=4;n++)REQUIRE(title(M1(id,bar,"itemAtIndex:",intptr_t,n),names[n-1]));
    id file=M0(id,M1(id,bar,"itemAtIndex:",intptr_t,1),"submenu"),save=M1(id,file,"itemAtIndex:",intptr_t,0);
    REQUIRE(title(settings,"Einstellungen …"));REQUIRE(M0(BOOL,settings,"isEnabled"));
    REQUIRE(M0(intptr_t,file,"numberOfItems")==3);REQUIRE(M0(BOOL,M1(id,file,"itemAtIndex:",intptr_t,1),"isSeparatorItem"));
    REQUIRE(!M0(BOOL,M1(id,file,"itemAtIndex:",intptr_t,2),"isEnabled"));
    REQUIRE(title(M0(id,application,"helpMenu"),"Hilfe"));
    commands(menu,0,0,NULL,NULL);invoke(file,0);REQUIRE(commands(menu,101,SDL_GetWindowID(window),NULL,NULL)==1);
    invoke(file,2);REQUIRE(commands(menu,0,0,NULL,NULL)==0);
    items[1].enabled=false;REQUIRE(ps_native_menu_publish(menu,groups,6,SDL_GetWindowID(window)));
    REQUIRE(!M0(BOOL,save,"isEnabled"));invoke(file,0);REQUIRE(commands(menu,0,0,NULL,NULL)==0);
    items[1].enabled=true;items[1].title="Sichern";REQUIRE(ps_native_menu_publish(menu,groups,6,SDL_GetWindowID(window)));REQUIRE(title(save,"Sichern"));
    SDL_Window *other=SDL_CreateWindow("Physim other task window",300,200,0);REQUIRE(other);REQUIRE(focus(other));
    invoke(file,0);REQUIRE(commands(menu,0,0,NULL,NULL)==0); /* stale publication */
    REQUIRE(ps_native_menu_publish(menu,groups,6,SDL_GetWindowID(other)));invoke(file,0);REQUIRE(commands(menu,101,SDL_GetWindowID(other),NULL,NULL)==1);
    REQUIRE(focus(window));REQUIRE(ps_native_menu_publish(menu,groups,6,SDL_GetWindowID(window)));
    REQUIRE(SDL_StartTextInput(window));SDL_PumpEvents();commands(menu,0,0,NULL,NULL);
    id cocoa_window=M0(id,application,"keyWindow");REQUIRE(cocoa_window);
    post_key(application,cocoa_window,10,"s",1,(uintptr_t)1<<20,NO);SDL_PumpEvents();
    unsigned consumed=0;bool shortcut=false;REQUIRE(commands(menu,101,SDL_GetWindowID(window),&consumed,&shortcut)==1);REQUIRE(consumed==1 && shortcut);
    post_key(application,cocoa_window,10,"s",1,(uintptr_t)1<<20,YES);SDL_PumpEvents();consumed=0;
    REQUIRE(commands(menu,0,0,&consumed,NULL)==0 && consumed==1);
    post_key(application,cocoa_window,11,"s",1,(uintptr_t)1<<20,NO);SDL_PumpEvents();commands(menu,0,0,NULL,NULL);
    SDL_StopTextInput(window);
    /* Native notification receiver: earlier editor input must survive, menu
     * navigation/text must not reach any editor, and commands stay ordered. */
    SDL_Event key;SDL_zero(key);key.type=SDL_EVENT_KEY_DOWN;key.key.windowID=SDL_GetWindowID(window);key.key.key=SDLK_A;REQUIRE(SDL_PushEvent(&key));
    notification(NSMenuDidBeginTrackingNotification);key.key.key=SDLK_DOWN;REQUIRE(SDL_PushEvent(&key));
    SDL_Event text;SDL_zero(text);text.type=SDL_EVENT_TEXT_INPUT;text.text.windowID=SDL_GetWindowID(window);text.text.text="x";REQUIRE(SDL_PushEvent(&text));
    invoke(file,0);notification(NSMenuDidEndTrackingNotification);
    SDL_Event event;unsigned before=0,nav=0,actions=0;
    while(SDL_PollEvent(&event)){
        if(event.type==SDL_EVENT_KEY_DOWN){if(event.key.key==SDLK_A)before++;else nav++;}
        if(event.type==SDL_EVENT_TEXT_INPUT)nav++;
        unsigned c;SDL_WindowID w;if(ps_native_menu_event(menu,&event,&c,&w,NULL)){REQUIRE(c==101);actions++;}
    }
    REQUIRE(before==1 && nav==0 && actions==1);
    ps_native_menu_group invalid[6];SDL_memcpy(invalid,groups,sizeof groups);invalid[1].count=1;invalid[2].count=2;
    REQUIRE(!ps_native_menu_publish(menu,invalid,6,SDL_GetWindowID(window)));REQUIRE(title(save,"Sichern"));
    id held=M0(id,save,"retain");ps_native_menu_destroy(menu);
    REQUIRE(M0(intptr_t,bar,"numberOfItems")==original_count);REQUIRE(M0(id,application,"helpMenu")==old_help);
    REQUIRE(M1(BOOL,M0(id,settings,"title"),"isEqualToString:",id,settings_title));REQUIRE(M0(intptr_t,settings,"tag")==old_tag);
    REQUIRE(M0(BOOL,settings,"isEnabled")==old_enabled && !M0(SEL,settings,"action"));
    REQUIRE(!M0(id,held,"target") && !M0(SEL,held,"action"));M0(void,held,"release");M0(void,settings_title,"release");
    SDL_DestroyWindow(other);SDL_DestroyWindow(window);SDL_Quit();
    puts("Native AppKit menu: structure, state, commands, keyboard ownership, tracking boundary, window switch and retirement passed");return 0;
}
#else
int main(void){
    if(ps_native_menu_create()!=NULL)return 1;
    SDL_Event event;SDL_zero(event);event.type=SDL_EVENT_KEY_DOWN;
    if(ps_native_menu_publish(NULL,NULL,0,0) || ps_native_menu_event(NULL,&event,NULL,NULL,NULL) || ps_native_menu_consumes_key(NULL,&event))return 1;
    ps_native_menu_destroy(NULL);puts("Non-macOS native menu fallback passed (no AppKit acceptance)");return 0;
}
#endif
