/* Optional Linux LD_PRELOAD probe for the F1 context-code test. Read boolean
 * representations as bytes so the diagnostic itself cannot trigger bool UB.
 * Forward every event unchanged; only Shift key events are logged. */
#define _GNU_SOURCE
#include <SDL3/SDL.h>
#include <dlfcn.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
static void trace(const char *phase,const SDL_Event *e) {
 if(!e || (e->type!=SDL_EVENT_KEY_DOWN && e->type!=SDL_EVENT_KEY_UP) || (e->key.key!=SDLK_LSHIFT && e->key.key!=SDLK_RSHIFT))return;
 const unsigned char *bytes=(const unsigned char*)e;
 fprintf(stderr,"RAW SHIFT %s type=%u key=%u repeat_offset=%zu repeat=%u down=%u sizeof_key=%zu bytes=",phase,(unsigned)e->type,(unsigned)e->key.key,offsetof(SDL_KeyboardEvent,repeat),bytes[offsetof(SDL_KeyboardEvent,repeat)],bytes[offsetof(SDL_KeyboardEvent,down)],sizeof(SDL_KeyboardEvent));
 for(size_t i=0;i<sizeof(SDL_KeyboardEvent);i++)fprintf(stderr,"%02x",bytes[i]);
 fputc('\n',stderr);
}
bool SDL_PushEvent(SDL_Event *event) {
 bool (*real)(SDL_Event*)=(bool(*)(SDL_Event*))dlsym(RTLD_NEXT,"SDL_PushEvent");if(!real)abort();
 trace("push-before",event);bool result=real(event);trace("push-after",event);return result;
}
bool SDL_PollEvent(SDL_Event *event) {
 bool (*real)(SDL_Event*)=(bool(*)(SDL_Event*))dlsym(RTLD_NEXT,"SDL_PollEvent");if(!real)abort();
 bool result=real(event);if(result)trace("polled",event);return result;
}
