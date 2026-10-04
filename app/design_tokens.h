#ifndef PHYSIM_DESIGN_TOKENS_H
#define PHYSIM_DESIGN_TOKENS_H

/* Semantic colors shared by the main workspace and its documentation window. */
typedef struct {
    struct nk_color text, muted, window, header, border, edit;
    struct nk_color button, button_hover, button_active;
    struct nk_color selection_active, toggle, toggle_hover, accent, accent_hover;
    struct nk_color slider, scrollbar, scrollbar_hover, scrollbar_active;
    struct nk_color primary, primary_hover, primary_active;
    struct nk_color divider, error;
    struct nk_color primary_text, surface, canvas, gutter, gutter_text;
    struct nk_color code_text, code_comment, code_string, code_keyword, code_type;
    struct nk_color code_preprocessor, code_number, plot_grid, plot_grid_minor;
    struct nk_color doc_header, doc_body, doc_text;
    struct nk_color curves[8];
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
    PS_RGB(55, 57, 63), PS_RGB(255, 155, 148),
    PS_RGB(237, 238, 242), PS_RGB(48, 49, 54), PS_RGB(20, 21, 24),
    PS_RGB(30, 31, 35), PS_RGB(142, 145, 154), PS_RGB(211, 224, 237),
    PS_RGB(118, 145, 154), PS_RGB(232, 192, 121), PS_RGB(187, 160, 236),
    PS_RGB(100, 170, 255), PS_RGB(192, 157, 230), PS_RGB(119, 191, 246),
    PS_RGB(53, 55, 63), PS_RGB(42, 44, 51), PS_RGB(38, 48, 65),
    PS_RGB(25, 26, 30), PS_RGB(225, 227, 233),
    {PS_RGB(100, 170, 255), PS_RGB(245, 166, 87), PS_RGB(109, 207, 148),
     PS_RGB(193, 151, 240), PS_RGB(242, 140, 178), PS_RGB(89, 205, 220),
     PS_RGB(222, 204, 110), PS_RGB(188, 197, 212)}
};
static const ps_ui_palette PS_UI_LIGHT = {
    .text = PS_RGB(25, 30, 38),
    .muted = PS_RGB(78, 87, 100),
    .window = PS_RGB(246, 247, 250),
    .header = PS_RGB(238, 240, 244),
    .border = PS_RGB(133, 143, 157),
    .edit = PS_RGB(255, 255, 255),
    .button = PS_RGB(229, 232, 238),
    .button_hover = PS_RGB(218, 222, 230),
    .button_active = PS_RGB(203, 210, 222),
    .selection_active = PS_RGB(192, 214, 247),
    .toggle = PS_RGB(198, 205, 215),
    .toggle_hover = PS_RGB(174, 187, 203),
    .accent = PS_RGB(0, 73, 163),
    .accent_hover = PS_RGB(0, 57, 128),
    .slider = PS_RGB(190, 197, 208),
    .scrollbar = PS_RGB(143, 153, 167),
    .scrollbar_hover = PS_RGB(104, 118, 138),
    .scrollbar_active = PS_RGB(78, 91, 112),
    .primary = PS_RGB(0, 78, 164),
    .primary_hover = PS_RGB(0, 61, 137),
    .primary_active = PS_RGB(0, 48, 115),
    .divider = PS_RGB(180, 188, 200),
    .error = PS_RGB(160, 27, 25),
    .primary_text = PS_RGB(255, 255, 255),
    .surface = PS_RGB(229, 232, 238),
    .canvas = PS_RGB(232, 235, 241),
    .gutter = PS_RGB(246, 247, 250),
    .gutter_text = PS_RGB(78, 87, 100),
    .code_text = PS_RGB(25, 30, 38),
    .code_comment = PS_RGB(72, 94, 89),
    .code_string = PS_RGB(123, 61, 0),
    .code_keyword = PS_RGB(0, 73, 163),
    .code_type = PS_RGB(0, 82, 108),
    .code_preprocessor = PS_RGB(90, 51, 144),
    .code_number = PS_RGB(113, 47, 130),
    .plot_grid = PS_RGB(203, 210, 222),
    .plot_grid_minor = PS_RGB(225, 229, 235),
    .doc_header = PS_RGB(219, 230, 247),
    .doc_body = PS_RGB(243, 245, 248),
    .doc_text = PS_RGB(37, 42, 51),
    .curves = {PS_RGB(0, 75, 165), PS_RGB(145, 72, 0), PS_RGB(0, 100, 59), PS_RGB(115, 48, 163), PS_RGB(158, 34, 95), PS_RGB(0, 90, 107), PS_RGB(106, 86, 0), PS_RGB(60, 71, 89)}
};
static const ps_ui_palette PS_UI_HIGH_CONTRAST = {
    .text = PS_RGB(255, 255, 255),
    .muted = PS_RGB(212, 212, 212),
    .window = PS_RGB(0, 0, 0),
    .header = PS_RGB(0, 0, 0),
    .border = PS_RGB(255, 255, 255),
    .edit = PS_RGB(0, 0, 0),
    .button = PS_RGB(24, 24, 24),
    .button_hover = PS_RGB(48, 48, 48),
    .button_active = PS_RGB(56, 56, 56),
    .selection_active = PS_RGB(0, 0, 110),
    .toggle = PS_RGB(90, 90, 90),
    .toggle_hover = PS_RGB(130, 130, 130),
    .accent = PS_RGB(255, 225, 0),
    .accent_hover = PS_RGB(255, 255, 255),
    .slider = PS_RGB(160, 160, 160),
    .scrollbar = PS_RGB(180, 180, 180),
    .scrollbar_hover = PS_RGB(230, 230, 230),
    .scrollbar_active = PS_RGB(255, 255, 255),
    .primary = PS_RGB(255, 225, 0),
    .primary_hover = PS_RGB(255, 255, 255),
    .primary_active = PS_RGB(255, 200, 0),
    .divider = PS_RGB(200, 200, 200),
    .error = PS_RGB(255, 180, 180),
    .primary_text = PS_RGB(0, 0, 0),
    .surface = PS_RGB(24, 24, 24),
    .canvas = PS_RGB(0, 0, 0),
    .gutter = PS_RGB(0, 0, 0),
    .gutter_text = PS_RGB(212, 212, 212),
    .code_text = PS_RGB(255, 255, 255),
    .code_comment = PS_RGB(190, 220, 190),
    .code_string = PS_RGB(255, 220, 130),
    .code_keyword = PS_RGB(255, 225, 0),
    .code_type = PS_RGB(140, 225, 255),
    .code_preprocessor = PS_RGB(235, 185, 255),
    .code_number = PS_RGB(215, 210, 255),
    .plot_grid = PS_RGB(140, 140, 140),
    .plot_grid_minor = PS_RGB(90, 90, 90),
    .doc_header = PS_RGB(0, 0, 110),
    .doc_body = PS_RGB(0, 0, 0),
    .doc_text = PS_RGB(255, 255, 255),
    .curves = {PS_RGB(140, 200, 255), PS_RGB(255, 210, 130), PS_RGB(130, 245, 170), PS_RGB(220, 180, 255), PS_RGB(255, 170, 215), PS_RGB(140, 235, 245), PS_RGB(255, 240, 140), PS_RGB(230, 230, 230)}
};
#undef PS_RGB

#endif
