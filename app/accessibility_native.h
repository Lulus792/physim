#ifndef PS_APP_ACCESSIBILITY_NATIVE_H
#define PS_APP_ACCESSIBILITY_NATIVE_H
#include "accessibility.h"
#include <SDL3/SDL.h>
typedef struct ps_a11y_native ps_a11y_native;
/* Native bridges are independent of rendering and model tests. NULL explicitly
 * means this platform bridge is not implemented/available yet. A successful
 * create takes ownership of mutex; destroy invalidates all retained elements
 * before the model can be freed. The model remains caller-owned. */
ps_a11y_native *ps_a11y_native_create(SDL_Window *window,ps_a11y_model *model,SDL_Mutex *mutex);
void ps_a11y_native_publish(ps_a11y_native *bridge,bool changed);
void ps_a11y_native_destroy(ps_a11y_native *bridge);
bool ps_a11y_native_test(SDL_Window *window);
/* Exercise the published native action, never directly enqueue a model press. */
bool ps_a11y_native_press_label(ps_a11y_native *bridge,const char *label);
#endif
