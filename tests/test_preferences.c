#include "physim/data.h"
#include "preferences.h"
#include "ui.h"
#include "design_tokens.h"
#include <math.h>
#include <SDL3/SDL.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Preferences line %d: %s\n", __LINE__, #x);                            \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static bool bytes_write(const char *path, const void *data, size_t size) {
    FILE *f = fopen(path, "wb");
    if (!f)
        return false;
    bool ok = fwrite(data, 1, size, f) == size;
    return !fclose(f) && ok;
}
static void put32(unsigned char *p, uint32_t v) {
    for (unsigned i = 0; i < 4; i++)
        p[i] = (unsigned char)(v >> (8 * i));
}
static double channel(unsigned value) {
    double s = value / 255.0;
    return s <= 0.04045 ? s / 12.92 : pow((s + 0.055) / 1.055, 2.4);
}
static double luminance(struct nk_color c) {
    return .2126 * channel(c.r) + .7152 * channel(c.g) + .0722 * channel(c.b);
}
static bool readable(struct nk_color foreground, struct nk_color background, double minimum) {
    double a = luminance(foreground), b = luminance(background);
    return (fmax(a, b) + .05) / (fmin(a, b) + .05) >= minimum;
}
static int contrast(void) {
    const ps_ui_palette *palettes[] = {&PS_UI_LIGHT, &PS_UI_HIGH_CONTRAST};
    for (unsigned i = 0; i < 2; i++) {
        const ps_ui_palette *p = palettes[i];
        double minimum = i ? 7 : 4.5;
        CHECK(readable(p->text, p->window, minimum));
        CHECK(readable(p->text, p->header, minimum));
        CHECK(readable(p->text, p->edit, minimum));
        CHECK(readable(p->text, p->button_active, minimum));
        CHECK(readable(p->muted, p->window, minimum));
        CHECK(readable(p->muted, p->header, minimum));
        CHECK(readable(p->gutter_text, p->gutter, minimum));
        CHECK(readable(p->doc_text, p->doc_body, minimum));
        CHECK(readable(p->doc_text, p->doc_header, minimum));
        CHECK(readable(p->primary_text, p->primary, minimum));
        CHECK(readable(p->primary_text, p->primary_hover, minimum));
        CHECK(readable(p->primary_text, p->primary_active, minimum));
        struct nk_color syntax[] = {p->code_text, p->code_comment, p->code_string, p->code_keyword,
            p->code_type, p->code_preprocessor, p->code_number, p->accent, p->error};
        for (unsigned j = 0; j < sizeof syntax / sizeof syntax[0]; j++)
            CHECK(readable(syntax[j], p->edit, minimum));
        for (unsigned j = 0; j < 8; j++)
            CHECK(readable(p->curves[j], p->edit, minimum));
    }
    return 0;
}
int main(int argc, char **argv) {
    CHECK(argc == 2);
    CHECK(contrast() == 0);
    char path[4096], bad[4096];
    snprintf(path, sizeof path, "%s/preferences-unit.bin", argv[1]);
    snprintf(bad, sizeof bad, "%s/preferences-corrupt.bin", argv[1]);
    remove(path);
    remove(bad);
    ps_preferences p = PS_PREFERENCES_DEFAULT, out = p;
    CHECK(ps_preferences_valid(&p) && ps_preferences_read(path, &out) == PS_EOF);
    CHECK(ps_preferences_write(path, &p) == PS_OK);
    for (unsigned theme = 0; theme < PS_THEME_COUNT; theme++) {
        p.theme = theme;
        CHECK(ps_preferences_write(path, &p) == PS_OK && ps_preferences_read(path, &out) == PS_OK);
        CHECK(!memcmp(&p, &out, sizeof p));
    }
    p.editor_size = 22;
    p.autosave_seconds = 120;
    p.width = 1200;
    p.height = 800;
    p.maximized = 1;
    p.sidebar_width = 360;
    p.log_height = 340;
    p.view_flags = 127;
    p.workspace = 2;
    p.inspector_open = 1;
    CHECK(ps_preferences_write(path, &p) == PS_OK && ps_preferences_read(path, &out) == PS_OK);
    CHECK(!memcmp(&p, &out, sizeof p));
    ps_preferences invalid = p;
    invalid.editor_size = 17;
    CHECK(ps_preferences_write(path, &invalid) == PS_INVALID);
    CHECK(ps_preferences_read(path, &out) == PS_OK && !memcmp(&p, &out, sizeof p));
    invalid = p;
    invalid.autosave_seconds = 0;
    CHECK(!ps_preferences_valid(&invalid));
    invalid = p;
    invalid.view_flags = 128;
    CHECK(!ps_preferences_valid(&invalid));
    invalid = p;
    invalid.width = 1079;
    CHECK(!ps_preferences_valid(&invalid));
    invalid = p;
    invalid.workspace = 3;
    CHECK(!ps_preferences_valid(&invalid));
    invalid = p;
    invalid.theme = PS_THEME_COUNT;
    CHECK(ps_preferences_write(path, &invalid) == PS_INVALID);
    CHECK(ps_preferences_read(path, &out) == PS_OK && !memcmp(&p, &out, sizeof p));
    unsigned char bytes[309] = {0};
    FILE *f = fopen(path, "rb");
    CHECK(f && fread(bytes, 1, 308, f) == 308 && !fclose(f));
    for (size_t n = 0; n < 308; n++) {
        CHECK(bytes_write(bad, bytes, n));
        CHECK(ps_preferences_read(bad, &out) == PS_CORRUPT && !memcmp(&p, &out, sizeof p));
    }
    CHECK(bytes_write(bad, bytes, 309) && ps_preferences_read(bad, &out) == PS_CORRUPT);
    for (unsigned i = 0; i < 308; i++) {
        bytes[i] ^= 1;
        CHECK(bytes_write(bad, bytes, 308));
        CHECK(ps_preferences_read(bad, &out) != PS_OK && !memcmp(&p, &out, sizeof p));
        bytes[i] ^= 1;
    }
    put32(bytes + 32, 17);
    put32(bytes + 304, ps_crc32(bytes, 304));
    CHECK(bytes_write(bad, bytes, 308) && ps_preferences_read(bad, &out) == PS_CORRUPT);
    put32(bytes + 32, p.editor_size);
    put32(bytes + 52, PS_THEME_COUNT);
    put32(bytes + 304, ps_crc32(bytes, 304));
    CHECK(bytes_write(bad, bytes, 308) && ps_preferences_read(bad, &out) == PS_CORRUPT);
    CHECK(!memcmp(&p, &out, sizeof p));
    /* Correct-CRC invalid graphs must also preserve the previous settings. */
    put32(bytes + 52, p.theme);
    put32(bytes + 60 + 5 * 24 + 4, 5); /* split references itself */
    put32(bytes + 304, ps_crc32(bytes, 304));
    CHECK(bytes_write(bad, bytes, 308) && ps_preferences_read(bad, &out) == PS_CORRUPT);
    put32(bytes + 60 + 5 * 24 + 4, 0);
    put32(bytes + 52, p.theme);
    memcpy(bytes + 6, "02", 2); put32(bytes + 8, 44);
    put32(bytes + 56, ps_crc32(bytes, 56));
    CHECK(bytes_write(bad, bytes, 60) && ps_preferences_read(bad, &out) == PS_OK);
    CHECK(!memcmp(&p, &out, sizeof p));
    /* Version 1 fixture retains all earlier settings and defaults to the dark theme. */
    memcpy(bytes + 6, "01", 2);
    put32(bytes + 8, 40);
    put32(bytes + 52, ps_crc32(bytes, 52));
    CHECK(bytes_write(bad, bytes, 56) && ps_preferences_read(bad, &out) == PS_OK);
    ps_preferences legacy = p;
    legacy.theme = PS_THEME_DARK;
    CHECK(!memcmp(&legacy, &out, sizeof out));
    CHECK(ps_preferences_write(bad, &out) == PS_OK && ps_preferences_read(bad, &out) == PS_OK);
    CHECK(!memcmp(&legacy, &out, sizeof out));
    /* A genuine five-node version-3 layout migrates without moving its panels. */
    unsigned char v3[240]={0};memcpy(v3,"PSPREF03",8);put32(v3+8,224);
    memcpy(v3+12,bytes+12,44);put32(v3+52,p.theme);
    ps_dock_node old_nodes[5]={{PS_DOCK_GROUP,0,0,0,1,0},{PS_DOCK_GROUP,0,0,0,2,1},
        {PS_DOCK_GROUP,0,0,0,4,2},{PS_DOCK_X,0,1,190,0,0},{PS_DOCK_Y,3,2,800,0,0}};
    unsigned char *old_at=v3+56;
    for(unsigned i=0;i<5;i++,old_at+=24) {
        const ps_dock_node *node=&old_nodes[i];
        uint32_t fields[]={node->kind,node->first,node->second,node->ratio,node->panels,node->active};
        for(unsigned j=0;j<6;j++)put32(old_at+4*j,fields[j]);
    }
    put32(old_at,4);old_at+=12;
    for(unsigned i=0;i<3;i++,old_at+=16) {
        const ps_dock_float *r=&p.dock.floats[i];
        put32(old_at,r->x);put32(old_at+4,r->y);put32(old_at+8,r->w);put32(old_at+12,r->h);
    }
    put32(v3+236,ps_crc32(v3,236));
    CHECK(bytes_write(bad,v3,sizeof v3) && ps_preferences_read(bad,&out)==PS_OK);
    CHECK(out.dock.root==4 && out.dock.hidden==8 && out.inspector_width==280 &&
          !memcmp(out.dock.nodes,old_nodes,sizeof old_nodes));
    CHECK(ps_preferences_write(bad,&out)==PS_OK && ps_preferences_read(bad,&legacy)==PS_OK &&
          !memcmp(&legacy,&out,sizeof out));
    ps_preferences untouched=out;
    put32(v3+56+3*24+4,3);put32(v3+236,ps_crc32(v3,236));
    CHECK(bytes_write(bad,v3,sizeof v3) && ps_preferences_read(bad,&out)==PS_CORRUPT &&
          !memcmp(&out,&untouched,sizeof out));
    /* Return the snapshot expected by the independent failed-write checks. */
    out=p;
    /* Rename failure: a directory cannot be replaced with a settings file. */
    char directory[4096];
    snprintf(directory, sizeof directory, "%s/preferences-target", argv[1]);
    CHECK(SDL_CreateDirectory(directory));
    CHECK(ps_preferences_write(directory, &p) == PS_IO);
    CHECK(ps_preferences_read(path, &out) == PS_OK && !memcmp(&p, &out, sizeof p));
    ps_preferences arranged=p;
    CHECK(ps_dock_move(&arranged.dock,PS_DOCK_SIDEBAR,PS_DOCK_WORKSPACE,PS_DOCK_TAB));
    CHECK(ps_dock_float_panel(&arranged.dock,PS_DOCK_LOG,(ps_dock_float){300,100,520,300}));
    CHECK(ps_preferences_write(bad,&arranged)==PS_OK && ps_preferences_read(bad,&out)==PS_OK);
    CHECK(!memcmp(&arranged,&out,sizeof out));
    puts("Preferences: themes, contrast, v1/v2 migration, docked/floating roundtrip, replacement, bounds, CRC, truncations and failure "
         "preservation passed");
    return 0;
}
