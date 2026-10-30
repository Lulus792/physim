#ifndef PHYSIM_SELECTION_UI_H
#define PHYSIM_SELECTION_UI_H
#include "design_tokens.h"
#include <math.h>

/* A2 adds a 20px selection column and 9px gap to existing file/status icons.
 * Keep original content ink and navigation highlights independent. */
enum { PS_SELECTION_COLUMN = 29 };
static inline void ps_ui_selection_row(struct nk_command_buffer *canvas,struct nk_rect row,float rounding,float scale,const ps_ui_palette *palette) {
    nk_fill_rect(canvas,row,rounding,palette->text_selection);
    nk_fill_rect(canvas,nk_rect(row.x,row.y,3*scale,row.h),0,palette->selection_accent);
}
static inline void ps_ui_selection_check(struct nk_command_buffer *canvas,struct nk_rect box,float scale,bool selected,const ps_ui_palette *palette) {
    nk_stroke_rect(canvas,box,3*scale,scale,palette->selection_border);
    if(selected) {
        nk_stroke_line(canvas,box.x+3*scale,box.y+7*scale,box.x+6*scale,box.y+10*scale,1.5f*scale,palette->selection_accent);
        nk_stroke_line(canvas,box.x+6*scale,box.y+10*scale,box.x+12*scale,box.y+4*scale,1.5f*scale,palette->selection_accent);
    }
}
static inline void ps_ui_selection_focus(struct nk_command_buffer *canvas,struct nk_rect row,float scale,const ps_ui_palette *palette) {
    if(scale<=0 || row.w<=4*scale || row.h<=4*scale)return;
    struct nk_rect r=nk_rect(row.x+2*scale,row.y+2*scale,row.w-4*scale,row.h-4*scale);
    for(float x=0;x<r.w;x+=6*scale) {
        float end=fminf(x+3*scale,r.w);
        nk_stroke_line(canvas,r.x+x,r.y,r.x+end,r.y,2*scale,palette->selection_focus);
        nk_stroke_line(canvas,r.x+x,r.y+r.h,r.x+end,r.y+r.h,2*scale,palette->selection_focus);
    }
    for(float y=0;y<r.h;y+=6*scale) {
        float end=fminf(y+3*scale,r.h);
        nk_stroke_line(canvas,r.x,r.y+y,r.x,r.y+end,2*scale,palette->selection_focus);
        nk_stroke_line(canvas,r.x+r.w,r.y+y,r.x+r.w,r.y+end,2*scale,palette->selection_focus);
    }
}
#endif
