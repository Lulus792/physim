#include "accessibility_native.h"
#include <stdio.h>
int main(void) {
 if(!SDL_Init(SDL_INIT_VIDEO))return 1;
 SDL_Window *window=SDL_CreateWindow("Physim accessibility native probe",400,200,0);
 if(!window){SDL_Quit();return 1;}
 bool passed=ps_a11y_native_test(window);SDL_DestroyWindow(window);SDL_Quit();
 if(passed)puts("macOS native accessibility: UTF-8 role/label, press, real model delivery and retained-element invalidation passed");
 return passed?0:1;
}
