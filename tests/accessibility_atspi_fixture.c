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
} fixture_window;
static bool fallback;
static bool create(fixture_window *f, const char *title) {
    f->window = SDL_CreateWindow(title, 400, 200, SDL_WINDOW_OPENGL);
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
    return true;
}
static bool draw(fixture_window *f, char name) {
    if (!f->window)
        return true;
    if (!ps_graphics_make_current(f->graphics))
        return false;
    nk_input_begin(f->ui);
    nk_input_end(f->ui);
    if (nk_begin(f->ui, "controls", nk_rect(0, 0, 400, 200), NK_WINDOW_NO_SCROLLBAR)) {
        nk_layout_row_dynamic(f->ui, 24, 1);
        nk_label(f->ui, "Bereit 🌍", NK_TEXT_LEFT);
        if (f->visible) {
            if (f->disabled)
                nk_widget_disable_begin(f->ui);
            bool pressed = nk_button_label(f->ui, "Öffnen …");
            nk_layout_row_dynamic(f->ui,24,1);
            bool toggled=nk_checkbox_label(f->ui,"Vektoren",&f->checked);
            if(toggled){f->toggles++;printf("TOGGLE %c %u %u\n",name,f->toggles,f->checked);fflush(stdout);}
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
            if (command == 'c')
                close_window(&b);
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
