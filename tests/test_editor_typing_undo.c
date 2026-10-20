#define NK_IMPLEMENTATION
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#include "../third_party/nuklear.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#define CHECK(v) do {if(!(v)){fprintf(stderr,"Typing undo %d: %s\n",__LINE__,#v);return 1;}}while(0)
static bool equals(struct nk_text_edit *edit,const char *value) {
    size_t length=strlen(value);return (size_t)nk_str_len_char(&edit->string)==length && !memcmp(nk_str_get_const(&edit->string),value,length);
}
int main(void) {
    const char *seeds[]={"abc","α😀z"};
    const char *changed[]={"aXbc","αX😀z"};
    for(unsigned i=0;i<2;i++) {
        struct nk_text_edit edit;nk_textedit_init_default(&edit);edit.mode=NK_TEXT_EDIT_MODE_INSERT;
        nk_textedit_text(&edit,seeds[i],(int)strlen(seeds[i]));
        nk_textedit_clear_state(&edit,NK_TEXT_EDIT_MULTI_LINE,nk_filter_default);edit.mode=NK_TEXT_EDIT_MODE_INSERT;
        edit.cursor=edit.select_start=edit.select_end=1;
        nk_textedit_text(&edit,"X",1);CHECK(equals(&edit,changed[i]));
        nk_textedit_undo(&edit);CHECK(equals(&edit,seeds[i]));
        nk_textedit_redo(&edit);CHECK(equals(&edit,changed[i]));nk_textedit_free(&edit);
    }
    struct nk_text_edit edit;nk_textedit_init_default(&edit);edit.mode=NK_TEXT_EDIT_MODE_INSERT;
    nk_textedit_text(&edit,"abc",3);nk_textedit_clear_state(&edit,NK_TEXT_EDIT_MULTI_LINE,nk_filter_default);
    edit.mode=NK_TEXT_EDIT_MODE_REPLACE;edit.cursor=edit.select_start=edit.select_end=1;
    nk_textedit_text(&edit,"X",1);CHECK(equals(&edit,"aXc"));nk_textedit_undo(&edit);CHECK(equals(&edit,"abc"));
    nk_textedit_redo(&edit);CHECK(equals(&edit,"aXc"));nk_textedit_free(&edit);
    puts("editor typing undo tests passed");return 0;
}
