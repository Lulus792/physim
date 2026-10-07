#include "ui.h"
#include <stdio.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"a11y UI %d: %s (%s)\n",__LINE__,#x,SDL_GetError());return 1;}}while(0)
static int draw(struct nk_context *ui,bool button,bool disabled) {
 nk_input_begin(ui);nk_input_end(ui);int pressed=0;
 if(nk_begin(ui,"controls",nk_rect(0,0,400,240),NK_WINDOW_NO_SCROLLBAR)) {
  nk_layout_row_dynamic(ui,24,1);nk_label(ui,"Bereit",NK_TEXT_LEFT);
  if(button) {
   if(disabled)nk_widget_disable_begin(ui);
   pressed=nk_button_label(ui,"Öffnen …");
   if(disabled)nk_widget_disable_end(ui);
  }
 }
 nk_end(ui);if(!nk_sdl_render(ui))return -1;return pressed;
}
int main(void) {
 CHECK(SDL_Init(SDL_INIT_VIDEO));
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,4);SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,1);
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
 SDL_Window *window=SDL_CreateWindow("Physim native accessibility UI probe",400,240,SDL_WINDOW_OPENGL);CHECK(window);
 ps_graphics *graphics=ps_graphics_create(window);CHECK(graphics);
 struct nk_context *ui=nk_sdl_init(window,graphics);CHECK(ui);
 struct nk_font_atlas *atlas=nk_sdl_font_stash_begin(ui);struct nk_font *font=nk_font_atlas_add_default(atlas,16,NULL);CHECK(font);
 CHECK(nk_sdl_font_stash_end(ui));nk_style_set_font(ui,&font->handle);
 CHECK(draw(ui,true,false)==0);CHECK(nk_sdl_accessibility_press(ui,"Öffnen …"));
 CHECK(draw(ui,true,false)==1);CHECK(draw(ui,true,false)==0);
 CHECK(!nk_sdl_accessibility_press(ui,"Bereit"));
 CHECK(draw(ui,true,true)==0);CHECK(!nk_sdl_accessibility_press(ui,"Öffnen …"));
 CHECK(draw(ui,true,false)==0);CHECK(nk_sdl_accessibility_press(ui,"Öffnen …"));
 CHECK(draw(ui,false,false)==0);CHECK(!nk_sdl_accessibility_press(ui,"Öffnen …"));
 CHECK(draw(ui,true,false)==0);CHECK(!nk_sdl_accessibility_press(ui,"missing"));
 nk_sdl_shutdown(ui);ps_graphics_destroy(graphics);SDL_DestroyWindow(window);SDL_Quit();
 puts("macOS native UI accessibility: actual rendered Nuklear labels/buttons, native press delivery once, disabled/removed controls and teardown passed");return 0;
}
