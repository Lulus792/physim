#define NK_IMPLEMENTATION
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#include "../third_party/nuklear.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
    /* Project analysis sources exceed 32767 runes. Undo must retain the full
     * cursor/selection offset, for ASCII and multibyte text, not only bytes. */
    const int lengths[]={32766,32767,32768,34869,65535};
    for(unsigned unicode=0;unicode<2;unicode++)for(unsigned i=0;i<sizeof lengths/sizeof lengths[0];i++){
        int count=lengths[i],stride=unicode?2:1;size_t bytes=(size_t)count*stride;
        char *seed=malloc(bytes+1);CHECK(seed);
        for(int j=0;j<count;j++)memcpy(seed+(size_t)j*stride,unicode?"α":"a",(size_t)stride);seed[bytes]=0;
        nk_textedit_init_default(&edit);CHECK(nk_str_append_text_char(&edit.string,seed,(int)bytes));
        nk_textedit_clear_state(&edit,NK_TEXT_EDIT_MULTI_LINE,nk_filter_default);edit.mode=NK_TEXT_EDIT_MODE_INSERT;
        edit.cursor=edit.select_start=edit.select_end=count;
        nk_textedit_text(&edit,"😀",4);CHECK(nk_str_len(&edit.string)==count+1 && edit.cursor==count+1);
        nk_textedit_undo(&edit);CHECK(equals(&edit,seed));CHECK(edit.cursor==count && edit.select_start==count && edit.select_end==count);
        nk_textedit_redo(&edit);CHECK(nk_str_len(&edit.string)==count+1 && edit.cursor==count+1 && edit.select_start==count+1 && edit.select_end==count+1);
        nk_textedit_undo(&edit);CHECK(equals(&edit,seed) && edit.cursor==count);
        nk_textedit_free(&edit);free(seed);
    }
    puts("editor typing undo tests passed");return 0;
}
