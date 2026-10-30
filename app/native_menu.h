#ifndef PHYSIM_NATIVE_MENU_H
#define PHYSIM_NATIVE_MENU_H
#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Main-thread adapter. Items remain app commands; AppKit owns presentation.
 * The application revalidates a returned command against its current model. */
enum { PS_NATIVE_MENU_GROUPS=6,PS_NATIVE_MENU_ITEMS=64 };
typedef struct {
    unsigned command;
    const char *title;
    SDL_Keycode key;
    SDL_Keymod modifiers;
    bool enabled,separator_before;
} ps_native_menu_item;
typedef struct {
    const char *title;
    const ps_native_menu_item *items;
    size_t count;
    bool application; /* Bind SDL's existing Preferences item. */
    bool help;
} ps_native_menu_group;
typedef struct ps_native_menu ps_native_menu;

ps_native_menu *ps_native_menu_create(void);
bool ps_native_menu_publish(ps_native_menu *,const ps_native_menu_group *,size_t,SDL_WindowID);
bool ps_native_menu_event(const ps_native_menu *,const SDL_Event *,unsigned *,SDL_WindowID *,bool *shortcut);
/* Call before ordinary keyboard routing, including for auxiliary windows. */
bool ps_native_menu_consumes_key(ps_native_menu *,const SDL_Event *);
void ps_native_menu_destroy(ps_native_menu *);
#endif
