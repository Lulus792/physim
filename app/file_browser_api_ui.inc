/* Explorer A's C context uses its own worker snapshot and edit state. It never
 * replaces the workbench's API card or reads files during layout/painting. */
static float browser_api_range(app *a,browser_dialog *d,const char *text,size_t bytes,unsigned role,
    float x,float y,float width,float pitch,bool draw,struct nk_color color) {
    const struct nk_user_font *font=&d->fonts[role];size_t at=0;unsigned lines=0;
    do{size_t length,used=documentation_line(font,text+at,bytes-at,fmaxf(1,width),&length);
        if(draw){nk_layout_space_push(a->ui,nk_rect(x,y+lines*pitch,width,pitch));struct nk_rect r;
            if(nk_widget(&r,a->ui)!=NK_WIDGET_INVALID)
                nk_draw_text(nk_window_get_canvas(a->ui),nk_rect(r.x,r.y+(pitch-font->height)/2,width,font->height),text+at,(int)length,font,nk_rgba(0,0,0,0),color);}
        lines++;if(!used)break;at+=used;
    }while(at<bytes);
    return lines*pitch;
}
static browser_api_table_layout *browser_api_table(browser_dialog *d,float width,float f) {
    const struct nk_user_font *font=&d->fonts[9];
    for(unsigned k=0;k<2;k++)if(d->api_tables[k].columns && d->api_tables[k].width==width && !memcmp(&d->api_tables[k].font,font,sizeof *font))return &d->api_tables[k];
    const ps_document *document=d->preview.api.document;size_t first=d->preview.api.preview.arguments,count=document->blocks[first].table_columns;
    if(!count || count>PS_DOC_MAX_CELLS)return NULL;
    float *columns=calloc(count*2,sizeof *columns);if(!columns)return NULL;
    /* CSS auto-table sizing: preserve content preferences and share remaining
     * width proportionally. Body cells can wrap anywhere, header words cannot. */
    for(size_t i=first;i<d->preview.api.end;i++){
        const ps_doc_block *row=&document->blocks[i];if(row->kind!=PS_DOC_TABLE_ROW || row->table_first!=first)break;
        for(size_t c=0;c<row->cell_count;c++){
            const ps_doc_cell *cell=&document->cells[row->first_cell+c];const char *text=document->text+cell->text;
            float preferred=font->width(font->userdata,font->height,text,(int)cell->length),minimum=0;size_t at=0;
            while(at<cell->length){size_t used=0;
                if(i==first && row->level==1){while(at+used<cell->length && text[at+used]!=' ')used++;if(!used)used=1;}
                else {nk_rune rune;int bytes=nk_utf_decode(text+at,&rune,(int)(cell->length-at));used=bytes>0?(size_t)bytes:1;}
                minimum=fmaxf(minimum,font->width(font->userdata,font->height,text+at,(int)used));at+=used;}
            columns[c]=fmaxf(columns[c],minimum+10*f);columns[count+c]=fmaxf(columns[count+c],preferred+10*f);
        }
    }
    float minimum=0,preferred=0;for(size_t c=0;c<count;c++){minimum+=columns[c];preferred+=columns[count+c];}
    for(size_t c=0;c<count;c++){
        if(width>=preferred)columns[c]=preferred>0?width*columns[count+c]/preferred:width/count;
        else if(width>=minimum && preferred>minimum)columns[c]+=(columns[count+c]-columns[c])*(width-minimum)/(preferred-minimum);
        else columns[c]=minimum>0?width*columns[c]/minimum:width/count;
    }
    browser_api_table_layout *cached=&d->api_tables[d->api_table_next++%2];free(cached->columns);
    *cached=(browser_api_table_layout){columns,width,count,*font};return cached;
}
static browser_preview_layout *browser_api_wrap(browser_dialog *d,float width,float f) {
    if(!d->preview.api.document || d->preview.api.preview.signature==SIZE_MAX)return NULL;
    const struct nk_user_font *font=&d->fonts[8];
    for(unsigned k=0;k<2;k++){browser_preview_layout *cached=&d->api_layouts[k];
        if(cached->layout && cached->width==width && !memcmp(&cached->font,font,sizeof *font))return cached;}
    const ps_doc_block *block=&d->preview.api.document->blocks[d->preview.api.preview.signature];ps_text_wrap *layout=NULL;
    if(!ps_text_wrap_build(d->preview.api.document->text+block->text,block->length,fmaxf(1,width),10.08f*1.85f*f,browser_preview_measure,(void*)font,&layout))return NULL;
    browser_preview_layout *cached=&d->api_layouts[d->api_layout_next++%2];
    if(d->edits[4].visual && cached->layout && d->edits[4].visual==&cached->layout->visual)d->edits[4].visual=NULL;
    ps_text_wrap_destroy(cached->layout);*cached=(browser_preview_layout){layout,*font,width,false};return cached;
}
static void browser_api_control(app *a,browser_dialog *d,int id,float y,float width,float height,float f,bool draw) {
    if(!draw){d->controls[id]=nk_rect(18*f,y,width,height);return;}
    const char *name=id==21?"API-Kontext":"Bestehende F1-Referenz ansehen";
    const char *label=id==21?"API-Kontext":"Bestehende F1-Referenz ansehen ↗";
    struct nk_rect r=browser_slot(a,18*f,y,width,height);float left=id==21?14*f:0;
    if(id==21){struct nk_command_buffer *canvas=nk_window_get_canvas(a->ui);float cx=r.x+4*f,cy=r.y+height/2;
        if(d->api_expanded)nk_fill_triangle(canvas,cx-3*f,cy-2*f,cx+4*f,cy-2*f,cx+.5f*f,cy+3*f,UI_COLOR(accent));
        else nk_fill_triangle(canvas,cx-2*f,cy-4*f,cx-2*f,cy+4*f,cx+3*f,cy,UI_COLOR(accent));}
    browser_text(a,d,18*f+left,y+(id==21?7*f:0),width-left,label,id==21?11:2,(id==21?16.38f:16.8f)*f,UI_COLOR(accent),false);
    if(ps_ui_button_at(a->ui,r,name,id==21 && d->api_expanded))browser_action(a,d,id);
    browser_focus_draw(a,d,id,r,name);
}
static float browser_api_content(app *a,browser_dialog *d,float width,float y,float f,bool draw) {
    if(!d->preview.api.symbol)return y;
    y+=18*f;browser_api_control(a,d,21,y,width,30.38f*f,f,draw);y+=30.38f*f;
    if(!d->api_expanded)return y;
    const char *name=d->preview.api.symbol->name;
    y+=8*f;float h=browser_api_range(a,d,name,strlen(name),10,18*f,y,width,17.85f*f,draw,UI_COLOR(text));
    if(draw)ps_ui_accessibility_label(a->ui,nk_rect(d->preview_bounds.x+18*f,d->controls[21].y+30.38f*f+8*f,width,h),name);
    y+=h+8*f;
    ps_document *document=d->preview.api.document;
    if(d->preview.api.status!=PS_OK || !document){
        const char *error="Die lokale API-Referenz konnte nicht geladen werden.";
        h=browser_text_height(d,error,2,width,16.8f*f);
        if(draw)browser_text(a,d,18*f,y,width,error,2,16.8f*f,UI_COLOR(muted),true);
        return y+h;
    }
    const ps_doc_api_preview *preview=&d->preview.api.preview;
    if(preview->signature!=SIZE_MAX){
        browser_preview_layout *cached=browser_api_wrap(d,width,f);y+=18*f;
        const ps_doc_block *block=&document->blocks[preview->signature];
        h=cached?cached->layout->visual.row_count*cached->layout->visual.row_height:16.8f*f;
        if(!draw)d->controls[23]=nk_rect(18*f,y,width,h);
        else if(cached){nk_layout_space_push(a->ui,nk_rect(18*f,y,width,h));struct nk_rect r=nk_widget_bounds(a->ui);
            if(d->keyboard && d->focus==23)nk_edit_focus(a->ui,NK_EDIT_ALWAYS_INSERT_MODE);
            struct nk_style_edit saved=a->ui->style.edit;a->ui->style.edit.padding=nk_vec2(0,0);a->ui->style.edit.border=0;
            a->ui->style.edit.scrollbar_size=nk_vec2(0,0);
            a->ui->style.edit.normal=a->ui->style.edit.hover=a->ui->style.edit.active=nk_style_item_color(nk_rgba(0,0,0,0));
            nk_style_push_font(a->ui,&d->fonts[8]);
            nk_sdl_edit_named_visual(a->ui,"API-Signatur",document->text+block->text,(int)block->length+1,&d->edits[4],&cached->layout->visual);
            nk_style_pop_font(a->ui);a->ui->style.edit=saved;browser_focus_draw(a,d,23,r,"API-Signatur");
        }else browser_text(a,d,18*f,y,width,"Signatur kann nicht dargestellt werden.",2,16.8f*f,UI_COLOR(error),true);
        y+=h+14*f;
    }
    if(preview->summary!=SIZE_MAX){const ps_doc_block *block=&document->blocks[preview->summary];const char *text=document->text+block->text;
        h=browser_api_range(a,d,text,block->length,2,18*f,y,width,16.8f*f,draw,UI_COLOR(text));
        if(draw){nk_uint sx,sy;nk_group_get_scroll(a->ui,"Explorer Vorschau",&sx,&sy);ps_ui_accessibility_label(a->ui,nk_rect(d->preview_bounds.x+18*f,d->preview_bounds.y+y-sy,width,h),text);}
        y+=h+14*f;
    }
    if(preview->arguments!=SIZE_MAX){size_t first=preview->arguments;browser_api_table_layout *table=browser_api_table(d,width,f);
        for(size_t i=first;i<d->preview.api.end;i++){
            const ps_doc_block *row=&document->blocks[i];if(row->kind!=PS_DOC_TABLE_ROW || row->table_first!=first)break;
            size_t columns=document->blocks[first].table_columns;if(!columns)break;
            float height=0;
            for(size_t c=0;c<row->cell_count;c++){const ps_doc_cell *cell=&document->cells[row->first_cell+c];
                float column=table?table->columns[c]:width/columns;
                height=fmaxf(height,browser_api_range(a,d,document->text+cell->text,cell->length,9,0,0,fmaxf(1,column-10*f),15.12f*f,false,UI_COLOR(text)));}
            if(draw){nk_layout_space_push(a->ui,nk_rect(18*f,y,width,height+16*f));struct nk_rect r;
                bool header=i==first && row->level==1;
                if(nk_widget(&r,a->ui)!=NK_WIDGET_INVALID){if(header)nk_fill_rect(nk_window_get_canvas(a->ui),r,0,UI_COLOR(surface));
                    nk_stroke_line(nk_window_get_canvas(a->ui),r.x,r.y+r.h,r.x+width,r.y+r.h,f,UI_COLOR(border));}
                ps_ui_accessibility_label(a->ui,r,document->text+row->text);
                float left=23*f;
                for(size_t c=0;c<row->cell_count;c++){const ps_doc_cell *cell=&document->cells[row->first_cell+c];float column=table?table->columns[c]:width/columns;
                    browser_api_range(a,d,document->text+cell->text,cell->length,9,left,y+8*f,fmaxf(1,column-10*f),15.12f*f,true,header?UI_COLOR(muted):UI_COLOR(text));left+=column;}}
            y+=height+16*f;
        }y+=14*f;
    }
    h=browser_text_height(d,"Bestehende F1-Referenz ansehen ↗",2,width,16.8f*f);
    browser_api_control(a,d,22,y,width,h,f,draw);return y+h;
}
