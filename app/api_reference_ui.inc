static bool api_reference_open(app *a,const ps_completion_symbol *symbol) {
    if(!symbol)return false;
    char relative[256],path[4096];
    snprintf(relative,sizeof relative,symbol->language?"docs/reference/language-library.md":"docs/reference/%s.md",symbol->module);
    ps_document *document=NULL;size_t first=0,end=0;ps_result result=PS_IO;
    join(path,sizeof path,a->root,relative);size_t bytes=0;char *source=load_utf8_text(path,&bytes);
    if(source)result=ps_document_parse(source,bytes,false,&document);free(source);
    if(result==PS_OK)result=ps_document_api_section(document,symbol->name,symbol->language,&first,&end);
    if(result!=PS_OK && symbol->language && symbol->kind==PS_COMPLETION_TYPE) {
        free(document);document=NULL;
        snprintf(relative,sizeof relative,"docs/language.md");
        join(path,sizeof path,a->root,relative);source=load_utf8_text(path,&bytes);result=PS_IO;
        if(source)result=ps_document_parse(source,bytes,false,&document);free(source);
        if(result==PS_OK)result=ps_document_api_section(document,"Statische Prüfung 0.1",false,&first,&end);
    }
    if(result!=PS_OK){free(document);status(a,"API-Referenz konnte nicht geladen werden. Bisherige Referenz bleibt erhalten.");return false;}
    free(a->api_reference);a->api_reference=document;a->api_reference_first=first;a->api_reference_end=end;
    snprintf(a->api_reference_name,sizeof a->api_reference_name,"%s",symbol->name);
    snprintf(a->api_reference_path,sizeof a->api_reference_path,"%s",relative);
    a->api_reference_scroll_x=a->api_reference_scroll_y=0;dock_show(a,PS_DOCK_INSPECTOR);
    return true;
}
static void api_reference_from_editor(app *a,struct nk_text_edit *edit) {
    if(a->completion_active && a->completion_edit==edit) {
        const ps_completion_symbol *symbol=ps_completion_at(a->completion_indices[a->completion_selected]);
        if(api_reference_open(a,symbol)){completion_close(a);a->completion_focus_edit=edit;}
        return;
    }
    int kind=document_language(a,edit);if(kind<0)return;
    const char *text=nk_str_get_const(&edit->string);size_t bytes=(size_t)nk_str_len_char(&edit->string),cursor=bytes;
    if(!text)text="";
    if(edit->cursor<nk_str_len(&edit->string)){nk_rune rune;int length;const char *at=nk_str_at_const(&edit->string,edit->cursor,&rune,&length);if(!at)return;cursor=(size_t)(at-text);}
    ps_completion_query query;
    if(ps_completion_prepare(text,bytes,cursor,kind==1,&query)!=PS_OK){status(a,"Kein API-Name an dieser Textposition.");return;}
    size_t indices_count=ps_completion_count(),*indices=malloc(indices_count*sizeof *indices);
    if(!indices){status(a,"Nicht genügend Speicher für die API-Referenz.");return;}
    ps_completion_query whole=query;size_t word=query.end-query.start,count=0;
    if(word<sizeof whole.prefix){memcpy(whole.prefix,text+query.start,word);whole.prefix[word]=0;count=ps_completion_matches(&whole,indices,indices_count);}
    if(!count)count=ps_completion_matches(&query,indices,indices_count);
    if(count==1)api_reference_open(a,ps_completion_at(indices[0]));
    else if(count){completion_open(a,edit);status(a,"API-Typ in den Vorschlägen wählen und Referenz öffnen.");}
    else status(a,"Kein passender API-Name gefunden.");
    free(indices);
}
static void api_reference_view(app *a) {
    struct nk_context *ui=a->ui;
    bool compact=!a->project_manager && (a->tab==0 || a->tab==8);
    const struct nk_user_font *heading=compact?&a->workbench_fonts[2]:a->font_title;
    const struct nk_user_font *body=compact?&a->workbench_fonts[0]:a->font_ui;
    nk_layout_row_dynamic(ui,24,1);nk_label_colored(ui,"API-Referenz",NK_TEXT_LEFT,UI_COLOR(muted));
    float width=fmaxf(40,nk_window_get_content_region(ui).w-16);
    nk_layout_row_dynamic(ui,documentation_height(heading,a->api_reference_name,strlen(a->api_reference_name),width)+12,1);
    nk_style_push_font(ui,heading);nk_label_wrap(ui,a->api_reference_name);nk_style_pop_font(ui);
    const char *close_label="Schließen";
    float button_width=ui->style.font->width(ui->style.font->userdata,ui->style.font->height,close_label,(int)strlen(close_label))+2*ui->style.button.padding.x+2*ui->style.button.border;
    nk_layout_row_dynamic(ui,28,compact && nk_window_get_content_region(ui).w<2*button_width+ui->style.window.spacing.x?1:2);a->api_reference_handbook_bounds=nk_widget_bounds(ui);
    if(nk_button_label(ui,"Handbuch")) {
        int topic=-1;for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++)if(!strcmp(documentation_topics[i].path,a->api_reference_path)){topic=i;break;}
        if(topic>=0){open_documentation(a,topic);if(a->documentation && a->doc_topic==topic)a->doc_jump_block=(int)a->api_reference_first;}
    }
    a->api_reference_close_bounds=nk_widget_bounds(ui);
    if(nk_button_label(ui,"Schließen")){free(a->api_reference);a->api_reference=NULL;return;}
    nk_layout_row_dynamic(ui,12,1);struct nk_rect remaining=nk_widget_bounds(ui),clip=nk_window_get_content_region(ui);
    nk_layout_row_dynamic(ui,fmaxf(40,clip.y+clip.h-remaining.y),1);a->api_reference_bounds=nk_widget_bounds(ui);
    if(nk_group_scrolled_offset_begin(ui,&a->api_reference_scroll_x,&a->api_reference_scroll_y,"api-reference-body",0)) {
        ps_document *d=a->api_reference;width=fmaxf(40,nk_window_get_content_region(ui).w-16);
        for(size_t i=a->api_reference_first;i<a->api_reference_end;i++) {
            const ps_doc_block *block=&d->blocks[i];const char *text=d->text+block->text;
            const struct nk_user_font *font=block->kind==PS_DOC_CODE?a->font_code:block->kind==PS_DOC_HEADING?heading:body;
            float height=documentation_height(font,text,block->length,width)+16;
            nk_layout_row_dynamic(ui,height,1);struct nk_rect bounds;
            if(nk_widget(&bounds,ui)!=NK_WIDGET_INVALID) {
                struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
                if(block->kind==PS_DOC_CODE)nk_fill_rect(canvas,bounds,4,UI_COLOR(edit));
                size_t at=0;float y=bounds.y+8;
                do{size_t length,used=documentation_line(font,text+at,block->length-at,width,&length);
                    nk_draw_text(canvas,nk_rect(bounds.x+8,y,width,font->height+6),text+at,(int)length,font,nk_rgba(0,0,0,0),UI_COLOR(text));
                    at+=used;y+=font->height+6;
                }while(at<block->length);
            }
        }
        nk_group_end(ui);
    }
}
