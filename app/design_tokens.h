#ifndef PHYSIM_DESIGN_TOKENS_H
#define PHYSIM_DESIGN_TOKENS_H

/* Semantic colors for the current dark workspace theme. Keep interaction states
 * here so later themes can replace values without changing workspace logic. */
typedef struct {
    struct nk_color text, muted, window, header, border, edit;
    struct nk_color button, button_hover, button_active;
    struct nk_color selection_active, toggle, toggle_hover, accent, accent_hover;
    struct nk_color slider, scrollbar, scrollbar_hover, scrollbar_active;
    struct nk_color primary, primary_hover, primary_active;
    struct nk_color divider, error;
} ps_ui_palette;

enum {
    PS_UI_RADIUS_PANEL = 10,
    PS_UI_RADIUS_CONTROL = 7,
    PS_UI_RADIUS_FIELD = 6,
    PS_UI_RADIUS_PROGRESS = 4,
    PS_UI_SPACE_SMALL = 8,
    PS_UI_SPACE_MEDIUM = 14,
    PS_UI_SPACE_LARGE = 18
};

#define PS_RGB(r, g, b) {r, g, b, 255}
static const ps_ui_palette PS_UI_DARK = {
    PS_RGB(237, 238, 242), PS_RGB(166, 169, 177), PS_RGB(30, 31, 35),
    PS_RGB(36, 37, 42), PS_RGB(65, 66, 73), PS_RGB(23, 24, 28),
    PS_RGB(53, 54, 60), PS_RGB(67, 69, 77), PS_RGB(78, 81, 92),
    PS_RGB(38, 86, 153), PS_RGB(69, 71, 79), PS_RGB(84, 87, 97),
    PS_RGB(100, 170, 255), PS_RGB(137, 191, 255), PS_RGB(64, 66, 74),
    PS_RGB(83, 85, 93), PS_RGB(112, 114, 122), PS_RGB(140, 142, 150),
    PS_RGB(25, 101, 206), PS_RGB(37, 116, 222), PS_RGB(19, 84, 179),
    PS_RGB(55, 57, 63), PS_RGB(255, 155, 148)
};
#undef PS_RGB

#endif
