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
static float workbench_outline_height(const app *a) {
    float scale=a->preferences.ui_size/16.f;
    return 3*(18*scale+fmaxf(13*scale,a->workbench_fonts[11].height)+fmaxf(12*scale,a->workbench_fonts[1].height))+6*scale;
}
static void workbench_outline_view(app *a,struct nk_text_edit *edit,int language) {
    struct nk_context *ui=a->ui;
    workbench_outline_refresh(a,edit,language);memset(a->outline_bounds,0,sizeof a->outline_bounds);
    float scale=a->preferences.ui_size/16.f,width=fmaxf(1,nk_window_get_content_region(ui).w-20*scale),total=0;
    const struct nk_user_font *font=&a->workbench_fonts[11],*small=&a->workbench_fonts[1];
    const char *message=a->outline.status==PS_OUTLINE_OK && a->outline.count?NULL:
        language<0?"Keine Quelltextgliederung für diesen Dateityp.":a->outline.status==PS_OUTLINE_NO_MEMORY?"Gliederung benötigt mehr Speicher.":
        a->outline.status==PS_OUTLINE_LIMITED?"Anzeigegrenze der Gliederung erreicht.":a->outline.status==PS_OUTLINE_INCOMPLETE?"Unvollständiger Quelltext; Gliederung vorläufig.":
        a->outline.status==PS_OUTLINE_INVALID?"Quelltext kann nicht gelesen werden.":"Keine Deklarationen auf Dateiebene.";
    if(message)total=workbench_text_height(&a->workbench_fonts[9],message,strlen(message),width+20*scale,17*scale,0)+3*scale;
    for(size_t i=0;i<a->outline.count;i++) {
        const ps_outline_symbol *symbol=&a->outline.symbols[i];
        char detail[96];snprintf(detail,sizeof detail,"%s · Zeile %zu",symbol->kind==PS_OUTLINE_FUNCTION?"Funktion":symbol->kind==PS_OUTLINE_TYPE?"Typ":"Bindung",symbol->line);
        total+=10*scale+workbench_text_height(font,a->outline_source+symbol->offset,symbol->length,width,13*scale,0)+8*scale+
            workbench_text_height(small,detail,strlen(detail),width,12*scale,0)+(i?3*scale:0);
    }
    ps_ui_row_exact_dynamic(ui,workbench_outline_height(a),1);
    nk_style_push_vec2(ui,&ui->style.window.group_padding,nk_vec2(0,0));
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,3*scale));
    if(a->outline_reset_scroll || total<=workbench_outline_height(a)){nk_group_set_scroll(ui,"Deklarationen im aktiven Dokument",0,0);a->outline_reset_scroll=false;}
    size_t selected=SIZE_MAX;
    if(nk_group_begin(ui,"Deklarationen im aktiven Dokument",total<=workbench_outline_height(a)?NK_WINDOW_NO_SCROLLBAR:0)) {
        if(message)workbench_text_row(a,&a->workbench_fonts[9],message,17*scale,0,UI_COLOR(text));
        for(size_t i=0;i<a->outline.count;i++) {
            ps_outline_symbol symbol=a->outline.symbols[i];
            float text_width=fmaxf(1,nk_window_get_content_region(ui).w-20*scale);
            float name_height=workbench_text_height(font,a->outline_source+symbol.offset,symbol.length,text_width,13*scale,0);
            char detail[96];snprintf(detail,sizeof detail,"%s · Zeile %zu",symbol.kind==PS_OUTLINE_FUNCTION?"Funktion":symbol.kind==PS_OUTLINE_TYPE?"Typ":"Bindung",symbol.line);
            float detail_height=workbench_text_height(small,detail,strlen(detail),text_width,12*scale,0);
            ps_ui_row_exact_dynamic(ui,10*scale+name_height+8*scale+detail_height,1);struct nk_rect bounds;nk_widget(&bounds,ui);a->outline_bounds[i]=bounds;
            struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
            if(nk_input_is_mouse_hovering_rect(&ui->input,bounds))nk_fill_rect(canvas,bounds,4,UI_COLOR(button_hover));
            const char *icon=symbol.kind==PS_OUTLINE_FUNCTION?"ƒ":symbol.kind==PS_OUTLINE_TYPE?"T":"≡";
            workbench_text_draw(a,font,nk_rect(bounds.x,bounds.y+5*scale,12*scale,13*scale),icon,strlen(icon),13*scale,0,UI_COLOR(accent));
            workbench_text_draw(a,font,nk_rect(bounds.x+20*scale,bounds.y+5*scale,text_width,name_height),a->outline_source+symbol.offset,symbol.length,13*scale,0,UI_COLOR(text));
            workbench_text_draw(a,small,nk_rect(bounds.x+20*scale,bounds.y+5*scale+name_height+8*scale,text_width,detail_height),detail,strlen(detail),12*scale,0,muted());
            ps_ui_accessibility_label(ui,nk_rect(bounds.x+20*scale,bounds.y+5*scale+name_height+8*scale,text_width,detail_height),detail);
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
