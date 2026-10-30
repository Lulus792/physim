#include "ui.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Text selection %d: %s\n",__LINE__,#x);return 1;}}while(0)
static float width(nk_handle u,float h,const char *s,int n){(void)u;return h*.5f*nk_utf_len(s,n);}
static void frame(struct nk_context *ui,struct nk_text_edit *edit,nk_flags flags,int mouse) {
    nk_input_begin(ui);nk_input_motion(ui,30,35);if(mouse>=0)nk_input_button(ui,NK_BUTTON_LEFT,30,35,mouse!=0);nk_input_end(ui);
    if(nk_begin(ui,"selection",nk_rect(0,0,360,180),NK_WINDOW_NO_SCROLLBAR)){
        nk_layout_row_dynamic(ui,120,1);nk_edit_buffer(ui,flags,edit,nk_filter_default);
    }nk_end(ui);
}
static bool same(struct nk_color a,struct nk_color b){return !memcmp(&a,&b,sizeof a);}
int main(void) {
    unsigned tested=0;const nk_flags modes[]={NK_EDIT_FIELD,NK_EDIT_BOX,NK_EDIT_BOX|NK_EDIT_READ_ONLY};
    const char *text="Aλbc";struct nk_user_font font={.height=12,.width=width};
    for(unsigned mode=0;mode<3;mode++)for(unsigned preserve=0;preserve<2;preserve++) {
        struct nk_context ui;CHECK(nk_init_default(&ui,&font));
        CHECK(!ui.style.edit.selected_text_preserve && !ui.style.property.edit.selected_text_preserve);
        ui.style.edit.selected_text_preserve=preserve!=0;
        struct nk_color wash=nk_rgba(170,184,202,38),legacy=nk_rgb(19,21,23);
        ui.style.edit.selected_normal=ui.style.edit.selected_hover=wash;
        ui.style.edit.selected_text_normal=ui.style.edit.selected_text_hover=legacy;
        struct nk_text_edit edit;nk_textedit_init_default(&edit);edit.mode=NK_TEXT_EDIT_MODE_INSERT;
        CHECK(nk_textedit_paste(&edit,text,(int)strlen(text)));
        frame(&ui,&edit,modes[mode],-1);nk_clear(&ui);frame(&ui,&edit,modes[mode],1);nk_clear(&ui);frame(&ui,&edit,modes[mode],0);nk_clear(&ui);CHECK(edit.active);
        for(unsigned variant=0;variant<2;variant++) {
            struct nk_color ink=variant?nk_rgb(40,190,70):nk_rgb(220,50,60);
            ui.style.edit.text_normal=ui.style.edit.text_hover=ui.style.edit.text_active=ink;
            edit.select_start=0;edit.cursor=edit.select_end=nk_str_len(&edit.string);
            frame(&ui,&edit,modes[mode],-1);unsigned glyphs=0,fills=0;const struct nk_command *c;
            nk_foreach(c,&ui) {
                if(c->type==NK_COMMAND_TEXT){const struct nk_command_text *t=(const struct nk_command_text *)c;
                    CHECK(same(t->foreground,preserve?ink:legacy));glyphs++;}
                if(c->type==NK_COMMAND_RECT_FILLED){const struct nk_command_rect_filled *r=(const struct nk_command_rect_filled *)c;if(same(r->color,wash))fills++;}
            }
            CHECK(glyphs && fills && nk_str_len_char(&edit.string)==(int)strlen(text) && !memcmp(nk_str_get_const(&edit.string),text,strlen(text)));
            nk_clear(&ui);tested++;
        }
        nk_textedit_free(&edit);nk_free(&ui);
    }
    printf("Text selection: %u editable/readonly/color-change cases preserve original ink on opt-in, retain legacy defaults and original UTF-8\n",tested);return 0;
}
