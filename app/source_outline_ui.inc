/* The index borrows source only while building. The snapshot owns exact bytes
 * for navigation and unchanged-frame comparison; names are never guessed from
 * the saved file or a previous successful build. */
static void workbench_outline_refresh(app *a,struct nk_text_edit *edit,int language) {
    size_t bytes=edit?(size_t)nk_str_len_char(&edit->string):0;
    const char *source=edit?nk_str_get_const(&edit->string):NULL;
    bool document_changed=a->outline_editor!=edit || a->outline_language!=language;
    a->outline_reset_scroll|=document_changed;
    if(!document_changed && a->outline_cached && a->outline_bytes==bytes &&
       (!bytes || (a->outline_source && !memcmp(a->outline_source,source,bytes))))return;
    a->outline_editor=edit;a->outline_language=language;a->outline_cached=false;
    memset(&a->outline,0,sizeof a->outline);
    if(!edit || language<0){a->outline_bytes=0;return;}
    if(bytes>PS_OUTLINE_SOURCE_LIMIT){a->outline.status=PS_OUTLINE_LIMITED;return;}
    char *snapshot=realloc(a->outline_source,bytes+1);
    if(!snapshot){a->outline.status=PS_OUTLINE_NO_MEMORY;return;}
    a->outline_source=snapshot;a->outline_bytes=bytes;
    if(bytes)memcpy(snapshot,source,bytes);snapshot[bytes]=0;
    ps_source_outline_build(snapshot,bytes,language!=0,&a->outline);a->outline_cached=true;
}
static void workbench_outline_jump(app *a,size_t index) {
    struct nk_text_edit *edit=a->outline_editor;
    if(!edit || index>=a->outline.count)return;
    /* Revalidate bytes at activation, including edits made earlier this frame. */
    size_t bytes=(size_t)nk_str_len_char(&edit->string);const char *source=nk_str_get_const(&edit->string);
    if(bytes!=a->outline_bytes || (bytes && memcmp(source,a->outline_source,bytes)))return;
    ps_outline_symbol symbol=a->outline.symbols[index];
    edit->select_start=nk_utf_len(source,(int)symbol.offset);
    edit->select_end=nk_utf_len(source,(int)(symbol.offset+symbol.length));edit->cursor=edit->select_start;
    edit->scrollbar.x=0;edit->scrollbar.y=(float)(symbol.line>3?symbol.line-3:0)*(a->font_code->height+editor_row_padding(a));
    completion_close(a);a->search_keyboard=false;a->search_focus=false;
    nk_sdl_accessibility_keyboard_begin(a->ui);
    a->completion_focus_edit=edit;a->outline_keyboard=true;nk_window_set_focus(a->ui,"workspace");
}
static void workbench_outline_view(app *a,struct nk_text_edit *edit,int language) {
    struct nk_context *ui=a->ui;
    workbench_outline_refresh(a,edit,language);memset(a->outline_bounds,0,sizeof a->outline_bounds);
    nk_style_push_vec2(ui,&ui->style.window.group_padding,nk_vec2(0,2));
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,0));
    nk_layout_row_dynamic(ui,110,1);
    if(a->outline_reset_scroll){nk_group_set_scroll(ui,"Deklarationen im aktiven Dokument",0,0);a->outline_reset_scroll=false;}
    size_t selected=SIZE_MAX;
    if(nk_group_begin(ui,"Deklarationen im aktiven Dokument",0)) {
        if(a->outline.status!=PS_OUTLINE_OK || !a->outline.count) {
            const char *message=language<0?"Keine Quelltextgliederung für diesen Dateityp.":
                a->outline.status==PS_OUTLINE_NO_MEMORY?"Gliederung benötigt mehr Speicher.":
                a->outline.status==PS_OUTLINE_LIMITED?"Anzeigegrenze der Gliederung erreicht.":
                a->outline.status==PS_OUTLINE_INCOMPLETE?"Unvollständiger Quelltext; Gliederung vorläufig.":
                a->outline.status==PS_OUTLINE_INVALID?"Quelltext kann nicht gelesen werden.":"Keine Deklarationen auf Dateiebene.";
            nk_layout_row_dynamic(ui,38,1);nk_label_wrap(ui,message);
        }
        for(size_t i=0;i<a->outline.count;i++) {
            ps_outline_symbol symbol=a->outline.symbols[i];
            nk_layout_row_dynamic(ui,31,1);struct nk_rect bounds;nk_widget(&bounds,ui);a->outline_bounds[i]=bounds;
            struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
            if(nk_input_is_mouse_hovering_rect(&ui->input,bounds))nk_fill_rect(canvas,bounds,4,UI_COLOR(button_hover));
            ps_ui_draw_icon(canvas,symbol.kind==PS_OUTLINE_FUNCTION?PS_ICON_FUNCTION:symbol.kind==PS_OUTLINE_TYPE?PS_ICON_TYPE:PS_ICON_VALUE,
                            nk_rect(bounds.x,bounds.y,16,bounds.h),UI_COLOR(accent));
            const struct nk_user_font *font=ui->style.font;
            nk_draw_text(canvas,nk_rect(bounds.x+23,bounds.y+(bounds.h-font->height)/2,fmaxf(1,bounds.w-23),font->height),
                         a->outline_source+symbol.offset,(int)symbol.length,font,nk_rgba(0,0,0,0),UI_COLOR(text));
            char *end=a->outline_source+symbol.offset+symbol.length,saved=*end;*end=0;
            if(ps_ui_button_at(ui,bounds,a->outline_source+symbol.offset,false))selected=i;
            if(nk_input_is_mouse_hovering_rect(&ui->input,bounds)) {
                char location[96];snprintf(location,sizeof location,"%s · Zeile %zu",symbol.kind==PS_OUTLINE_FUNCTION?"Funktion":symbol.kind==PS_OUTLINE_TYPE?"Typ":"Bindung",symbol.line);
                nk_tooltip(ui,location);
            }
            *end=saved;
        }
        nk_group_end(ui);
    }
    nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
    if(selected!=SIZE_MAX)workbench_outline_jump(a,selected);
}
