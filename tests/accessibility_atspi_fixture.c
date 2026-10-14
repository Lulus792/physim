#include "ui.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
typedef struct {
    SDL_Window *window;
    ps_graphics *graphics;
    struct nk_context *ui;
    bool visible, disabled, checked;
    unsigned presses,toggles,options[2];
    float scalar;
    unsigned document_state;uint64_t document_version;
    bool edit_visible,edit_disabled;char edit_value[128];
} fixture_window;
static bool fallback;
static bool create(fixture_window *f, const char *title) {
    f->window = SDL_CreateWindow(title, 400, 260, SDL_WINDOW_OPENGL);
    if (!f->window) {
        fprintf(stderr, "create window: %s\n", SDL_GetError());
        return false;
    }
    f->graphics = ps_graphics_create(f->window);
    if (!f->graphics) {
        fprintf(stderr, "create graphics: %s\n", SDL_GetError());
        return false;
    }
    f->ui = nk_sdl_init(f->window, f->graphics);
    if (!f->ui || nk_sdl_accessibility_available(f->ui) == fallback) {
        fprintf(stderr, "create UI/native provider: %s\n", SDL_GetError());
        return false;
    }
    struct nk_font_atlas *atlas = nk_sdl_font_stash_begin(f->ui);
    struct nk_font *font = nk_font_atlas_add_default(atlas, 16, NULL);
    if (!font || !nk_sdl_font_stash_end(f->ui))
        return false;
    nk_style_set_font(f->ui, &font->handle);
    f->visible = true;
    f->scalar=.5f;
    return true;
}
static bool draw(fixture_window *f, char name) {
    if (!f->window)
        return true;
    if (!ps_graphics_make_current(f->graphics))
        return false;
    nk_input_begin(f->ui);
    nk_input_end(f->ui);
    if (nk_begin(f->ui, "controls", nk_rect(0, 0, 400, 260), NK_WINDOW_NO_SCROLLBAR)) {
        nk_layout_row_dynamic(f->ui, 24, 1);
        nk_label(f->ui, "Bereit 🌍", NK_TEXT_LEFT);
        if (f->visible) {
            if (f->disabled)
                nk_widget_disable_begin(f->ui);
            bool pressed = nk_button_label(f->ui, "Öffnen …");
            nk_layout_row_dynamic(f->ui,24,1);
            bool toggled=nk_checkbox_label(f->ui,"Vektoren",&f->checked);
            if(toggled){f->toggles++;printf("TOGGLE %c %u %u\n",name,f->toggles,f->checked);fflush(stdout);}
            nk_layout_row_dynamic(f->ui,24,1);
            if(ps_ui_slider_named(f->ui,"Winkel θ",-1,&f->scalar,1,.25f)){printf("VALUE %c %.2f\n",name,(double)f->scalar);fflush(stdout);}
            const char *groups[]={"Darstellung","Code"};
            for(unsigned g=0;g<2;g++) {
                unsigned before=f->options[g];nk_layout_row_dynamic(f->ui,24,2);
                if(ps_ui_option_label(f->ui,groups[g],"16 px",f->options[g]==0))f->options[g]=0;
                if(ps_ui_option_label(f->ui,groups[g],"22 px",f->options[g]==1))f->options[g]=1;
                if(before!=f->options[g]){printf("CHOICE %c %u %u\n",name,g,f->options[g]);fflush(stdout);}
            }
            if (f->disabled)
                nk_widget_disable_end(f->ui);
            if (pressed) {
                f->presses++;
                f->disabled = true;
                printf("PRESS %c %u\n", name, f->presses);
                fflush(stdout);
            }
        }
        if(f->edit_visible && f->visible) {
            nk_layout_row_dynamic(f->ui,24,1);
            if(f->edit_disabled)nk_widget_disable_begin(f->ui);
            nk_sdl_edit_named(f->ui,"Suchfeld θ",NK_EDIT_FIELD,f->edit_value,sizeof f->edit_value,nk_filter_default);
            if(f->edit_disabled)nk_widget_disable_end(f->ui);
        }
        if(f->document_state) {
            const char *source=f->document_state==1?"A θ🌍 é. Next!\nLast\n":"Neu β😀\n";
            const char *title=f->document_state==1?"Lesetext θ🌍":"Neuer Lesetext β😀";
            nk_layout_row_dynamic(f->ui,40,1);struct nk_rect view;
            if(nk_widget(&view,f->ui)!=NK_WIDGET_INVALID) {
                size_t byte;bool top;
                if(nk_sdl_accessibility_document_scroll(f->ui,f->document_version,&byte,&top)) {
                    printf("TEXT SCROLL %c %zu\n",name,byte);fflush(stdout);
                }
                if(!nk_sdl_accessibility_document(f->ui,f->document_version,title,source,view))return false;
                float x=view.x,y=view.y;const struct nk_user_font *font=f->ui->style.font;
                for(size_t at=0;source[at];) {
                    nk_rune rune;int n=nk_utf_decode(source+at,&rune,(int)strlen(source+at));if(n<=0)return false;
                    if(rune=='\n'){x=view.x;y+=font->height;at+=(size_t)n;continue;}
                    float width=font->width(font->userdata,font->height,source+at,n);
                    struct nk_rect box=nk_rect(x,y,width,font->height);
                    nk_draw_text(nk_window_get_canvas(f->ui),box,source+at,n,font,nk_rgba(0,0,0,0),nk_rgb(230,230,230));
                    if(box.x>=view.x && box.y>=view.y && box.x+box.w<=view.x+view.w && box.y+box.h<=view.y+view.h)
                        nk_sdl_accessibility_text_box(f->ui,at,at+(size_t)n,box);
                    x+=width;at+=(size_t)n;
                }
            }
        }
    }
    nk_end(f->ui);
    return nk_sdl_render(f->ui) && ps_graphics_present(f->graphics);
}
static void close_window(fixture_window *f) {
    if (f->graphics)
        ps_graphics_make_current(f->graphics);
    if (f->ui)
        nk_sdl_shutdown(f->ui);
    if (f->graphics)
        ps_graphics_destroy(f->graphics);
    if (f->window)
        SDL_DestroyWindow(f->window);
    memset(f, 0, sizeof *f);
}
int main(int argc, char **argv) {
    fallback = argc == 2 && !strcmp(argv[1], "--fallback");
    SDL_SetMainReady();
    if (!SDL_Init(SDL_INIT_VIDEO))
        return 1;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    fixture_window a = {0}, b = {0};
    int result = 1;
    if (!create(&a, "Physim AT-SPI A") || !create(&b, "Physim AT-SPI B"))
        goto done;
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
    puts("READY");
    fflush(stdout);
    Uint64 start = SDL_GetTicks();
    while (SDL_GetTicks() - start < 60000) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if(a.ui)nk_sdl_accessibility_event(a.ui,&e);
            if(b.ui)nk_sdl_accessibility_event(b.ui,&e);
        }
        char command;
        ssize_t n = read(STDIN_FILENO, &command, 1);
        if (n == 1) {
            if (command == 'q') {
                result = 0;
                break;
            }
            if (command == 'r')
                a.visible = false;
            if (command == 'e') {
                a.visible = true;
                a.disabled = false;
            }
            if (command == 'm')
                SDL_SetWindowPosition(a.window, 300, 100);
            if (command == 'h')
                SDL_HideWindow(a.window);
            if (command == 's')
                SDL_ShowWindow(a.window);
            if(command=='k') {
                SDL_Window *focused=SDL_GetKeyboardFocus();SDL_Event key;SDL_zero(key);key.type=SDL_EVENT_KEY_DOWN;
                key.key.windowID=focused?SDL_GetWindowID(focused):0;key.key.key=SDLK_SPACE;SDL_PushEvent(&key);
            }
            if (command == 'c')
                close_window(&b);
            if(command=='d' || command=='u'){a.document_state=command=='d'?1:2;++a.document_version;}
            if(command=='x')a.document_state=0;
            if(command=='f'){a.edit_visible=b.edit_visible=true;}
            if(command=='j')a.edit_disabled=true;
            if(command=='l')a.edit_disabled=false;
            if(command=='z')a.edit_visible=false;
        }
        if (!draw(&a, 'A') || !draw(&b, 'B'))
            goto done;
        SDL_Delay(20);
    }
done:
    close_window(&b);
    close_window(&a);
    SDL_Quit();
    if (result)
        fprintf(stderr, "AT-SPI fixture failed: %s\n", SDL_GetError());
    return result;
}
