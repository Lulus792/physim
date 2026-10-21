/* A compact preview over the owned reference document. Full reference content
 * stays in F1; only an explicitly marked example is ever inserted. */
static float workbench_api_text_height(const struct nk_user_font *font,const char *text,size_t bytes,float width,float pitch) {
    return workbench_text_height(font,text,bytes,width,pitch,0);
}
static void workbench_api_text(app *a,const struct nk_user_font *font,const char *text,size_t bytes,float pitch,struct nk_color color) {
    struct nk_context *ui=a->ui;float width=nk_window_get_content_region(ui).w;
    ps_ui_row_exact_dynamic(ui,workbench_api_text_height(font,text,bytes,width,pitch),1);
    struct nk_rect r;nk_widget(&r,ui);workbench_text_draw(a,font,r,text,bytes,pitch,0,color);
    ps_ui_accessibility_label(ui,r,text);
}
static void workbench_api_gap(struct nk_context *ui,float height) {
    ps_ui_row_exact_dynamic(ui,height,1);nk_spacer(ui);
}
static float workbench_api_arguments(app *a,float width,bool draw) {
    const ps_doc_api_preview *p=&a->api_reference_preview;ps_document *d=a->api_reference;
    if(p->arguments==SIZE_MAX)return 0;
    const struct nk_user_font *font=&a->workbench_fonts[1];float scale=a->preferences.ui_size/16.f,height=13*scale;
    size_t first=p->arguments,rows=0;
    for(size_t i=first+1;i<a->api_reference_end && rows<2;i++) {
        const ps_doc_block *b=&d->blocks[i];
        if(b->kind!=PS_DOC_TABLE_ROW || b->table_first!=first)break;
        if(b->cell_count!=2)continue;
        const ps_doc_cell *left=&d->cells[b->first_cell],*right=&d->cells[b->first_cell+1];
        float gap=10*scale,one=floorf(fminf(width*.42f,ceilf(font->width(font->userdata,font->height,d->text+left->text,(int)left->length))+1));
        float two=fmaxf(1,floorf(width-one-gap));
        float row=fmaxf(workbench_api_text_height(font,d->text+left->text,left->length,one,13.5f*scale),
                        workbench_api_text_height(font,d->text+right->text,right->length,two,13.5f*scale));
        float gap_before=rows?6*scale:0; height+=gap_before+row;rows++;
        if(draw) {
            if(gap_before)workbench_api_gap(a->ui,gap_before);
            ps_ui_row_exact_begin(a->ui,NK_STATIC,row,2);nk_layout_row_push(a->ui,one);
            struct nk_rect r;nk_widget(&r,a->ui);
            size_t at=0;float y=r.y+(fmaxf(13.5f*scale,font->height)-font->height)/2;
            while(at<left->length) {
                size_t length=0,used=documentation_line(font,d->text+left->text+at,left->length-at,fmaxf(1,one),&length);if(!used)break;
                nk_draw_text(nk_window_get_canvas(a->ui),nk_rect(r.x,y,r.w,font->height),d->text+left->text+at,(int)length,font,nk_rgba(0,0,0,0),muted());
                at+=used;y+=13.5f*scale;
            }
            nk_layout_row_push(a->ui,two);nk_widget(&r,a->ui);
            a->api_card_arguments_bounds=r;
            at=0;y=r.y+(fmaxf(13.5f*scale,font->height)-font->height)/2;
            while(at<right->length) {
                size_t length=0,used=documentation_line(font,d->text+right->text+at,right->length-at,two,&length);if(!used)break;
                float text_width=font->width(font->userdata,font->height,d->text+right->text+at,(int)length);
                nk_draw_text(nk_window_get_canvas(a->ui),nk_rect(r.x+fmaxf(0,r.w-text_width),y,r.w,font->height),d->text+right->text+at,(int)length,font,nk_rgba(0,0,0,0),UI_COLOR(text));
                at+=used;y+=13.5f*scale;
            }
            char reading[512];snprintf(reading,sizeof reading,"%.*s: %.*s",(int)left->length,d->text+left->text,(int)right->length,d->text+right->text);
            ps_ui_accessibility_label(a->ui,r,reading);
            nk_layout_row_end(a->ui);
        }
    }
    return height;
}
static const char *workbench_api_category(const app *a) {
    return !strcmp(a->api_reference_name,"ps_channel_add") || !strcmp(a->api_reference_name,"Channel")?"PHYSIM API / MESSKANÄLE":"PHYSIM API / REFERENZ";
}
static float workbench_api_natural_height(app *a,float width) {
    ps_document *d=a->api_reference;const ps_doc_api_preview *p=&a->api_reference_preview;
    float scale=a->preferences.ui_size/16.f,inner=fmaxf(1,floorf(width-28));
    const struct nk_user_font *label=&a->workbench_fonts[8],*body=&a->workbench_fonts[9],*code=&a->workbench_fonts[7],*name=&a->workbench_fonts[10];
    const ps_doc_block *summary=p->summary==SIZE_MAX?NULL:&d->blocks[p->summary];
    size_t code_index=p->example!=SIZE_MAX?p->example:p->signature;
    const ps_doc_block *sample=code_index==SIZE_MAX?NULL:&d->blocks[code_index];
    float name_h=workbench_api_text_height(name,a->api_reference_name,strlen(a->api_reference_name),inner,14*scale);
    float summary_h=summary?workbench_api_text_height(body,d->text+summary->text,summary->length,inner,17*scale):0;
    float code_h=sample?workbench_api_text_height(code,d->text+sample->text,sample->length,inner,17*scale):0;
    float args_h=workbench_api_arguments(a,inner,false);
    float label_h=workbench_text_height(label,workbench_api_category(a),strlen(workbench_api_category(a)),inner,12*scale,.8f*scale);
    return 28+label_h+10*scale+name_h+(summary?8*scale+summary_h+13*scale:0)+code_h+args_h+7*scale+30*scale;
}
static float workbench_api_card_height(app *a,float width) {
    return fmaxf(236,fminf(320*a->preferences.ui_size/16.f,workbench_api_natural_height(a,width)));
}
static void workbench_api_frame(struct nk_command_buffer *canvas,struct nk_rect r,float radius,struct nk_color face) {
    /* Nuklear stores rectangle coordinates as integers. A half-pixel stroke
     * offset is lost before conversion; nested fills keep a crisp 1 px ring. */
    float x=floorf(r.x),y=floorf(r.y),width=floorf(r.x+r.w)-x,height=floorf(r.y+r.h)-y;
    nk_fill_rect(canvas,nk_rect(x,y,width,height),radius,UI_COLOR(border));
    nk_fill_rect(canvas,nk_rect(x+1,y+1,fmaxf(0,width-2),fmaxf(0,height-2)),radius-1,face);
    /* The rounded fill's antialiasing can also blend a straight edge by one
     * color step. Integer, unrounded strips preserve the CSS border there. */
    float horizontal=fmaxf(0,width-2*radius),vertical=fmaxf(0,height-2*radius);
    nk_fill_rect(canvas,nk_rect(x+radius,y,horizontal,1),0,UI_COLOR(border));
    nk_fill_rect(canvas,nk_rect(x+radius,y+height-1,horizontal,1),0,UI_COLOR(border));
    nk_fill_rect(canvas,nk_rect(x,y+radius,1,vertical),0,UI_COLOR(border));
    nk_fill_rect(canvas,nk_rect(x+width-1,y+radius,1,vertical),0,UI_COLOR(border));
}
static void workbench_api_card(app *a,float requested) {
    struct nk_context *ui=a->ui;ps_document *d=a->api_reference;const ps_doc_api_preview *p=&a->api_reference_preview;
    float scale=a->preferences.ui_size/16.f,width=nk_window_get_content_region(ui).w;
    const struct nk_user_font *label=&a->workbench_fonts[8],*body=&a->workbench_fonts[9],*code=&a->workbench_fonts[7],*name=&a->workbench_fonts[10];
    const ps_doc_block *summary=p->summary==SIZE_MAX?NULL:&d->blocks[p->summary];
    size_t code_index=p->example!=SIZE_MAX?p->example:p->signature;
    const ps_doc_block *sample=code_index==SIZE_MAX?NULL:&d->blocks[code_index];
    float button_h=30*scale,padding=14,natural=workbench_api_natural_height(a,width);
    a->api_card_natural_height=natural;a->api_card_arguments_bounds=nk_rect(0,0,0,0);
    float height=fmaxf(requested,fminf(320*scale,natural));
    nk_layout_space_begin(ui,NK_STATIC,height,1);
    nk_layout_space_push(ui,nk_rect(0,0,width,height));
    struct nk_rect card=nk_widget_bounds(ui);a->api_card_bounds=card;
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);workbench_api_frame(canvas,card,7,UI_COLOR(edit));
    struct nk_rect contents=nk_rect(card.x+padding,card.y+padding,card.w-2*padding,card.h-2*padding-button_h-7*scale);
    a->api_reference_bounds=contents;
    a->api_card_action_bounds=nk_rect(card.x+padding,card.y+card.h-padding-button_h,card.w-2*padding,button_h);
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(10*scale,0));
    nk_style_push_vec2(ui,&ui->style.window.group_padding,nk_vec2(0,0));
    nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(UI_COLOR(edit)));
    nk_layout_space_push(ui,nk_rect(contents.x-card.x,contents.y-card.y,contents.w,contents.h));
    if(nk_group_scrolled_offset_begin(ui,&a->api_reference_scroll_x,&a->api_reference_scroll_y,"workbench-api-preview",natural<=height?NK_WINDOW_NO_SCROLLBAR:0)) {
        float body_start=ui->current->layout->at_y;
        struct nk_rect caption=workbench_text_row(a,label,workbench_api_category(a),12*scale,.8f*scale,muted());a->api_reference_handbook_bounds=caption;
        if(ps_ui_button_at(ui,caption,"API-Referenz im Handbuch öffnen",false))api_reference_handbook(a);
        workbench_api_gap(ui,10*scale);
        workbench_api_text(a,name,a->api_reference_name,strlen(a->api_reference_name),14*scale,UI_COLOR(accent));
        if(summary) {
            workbench_api_gap(ui,8*scale);
            workbench_api_text(a,body,d->text+summary->text,summary->length,17*scale,muted());
            workbench_api_gap(ui,13*scale);
        }
        if(sample)workbench_api_text(a,code,d->text+sample->text,sample->length,17*scale,UI_COLOR(text));
        if(p->arguments!=SIZE_MAX){workbench_api_gap(ui,13*scale);workbench_api_arguments(a,nk_window_get_content_region(ui).w,true);}
        a->api_card_measured_body_height=ui->current->layout->at_y+ui->current->layout->row.height-body_start;
        nk_group_end(ui);
    }
    nk_layout_space_end(ui);nk_style_pop_style_item(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
    struct nk_rect action=a->api_card_action_bounds;struct nk_text_edit *edit=completion_target(a);
    bool example=p->example!=SIZE_MAX;
    const char *text=example?"Beispiel einfügen":"Im Handbuch öffnen";
    bool enabled=!example || (edit && idle(a) && a->api_reference_symbol && document_language(a,edit)==(int)a->api_reference_symbol->language);
    struct nk_rect visible=plot_intersection(action,ui->current->layout->clip);
    bool hover=visible.w>0 && visible.h>0 && nk_input_is_mouse_hovering_rect(&ui->input,visible);
    workbench_api_frame(canvas,action,6,enabled && hover?UI_COLOR(button_hover):UI_COLOR(edit));
    nk_draw_text(canvas,nk_rect(action.x+9,action.y+(action.h-body->height)/2,action.w-26,body->height),text,(int)strlen(text),body,nk_rgba(0,0,0,0),enabled?UI_COLOR(text):muted());
    if(example){float x=action.x+action.w-12,y=action.y+action.h/2;nk_stroke_line(canvas,x+3,y-3,x-3,y+3,1,UI_COLOR(muted));nk_stroke_line(canvas,x-3,y-1,x-3,y+3,1,UI_COLOR(muted));nk_stroke_line(canvas,x-3,y+3,x+1,y+3,1,UI_COLOR(muted));}
    nk_begin_disabled(ui,!enabled);
    if(ps_ui_button_at(ui,action,text,false)) {
        if(example){a->api_example_pending=true;a->api_example_edit=edit;}
        else api_reference_handbook(a);
    }
    nk_end_disabled(ui);
    if(!example)a->api_reference_handbook_bounds=action;
    if(hover)nk_tooltip(ui,example?"Dokumentiertes Beispiel an der Auswahl einfügen. Namen und Kontext gegebenenfalls anpassen.":"Vollständige Referenz im F1-Handbuch öffnen.");
}
static float workbench_api_empty_height(app *a,float width) {
    float scale=a->preferences.ui_size/16.f,inner=fmaxf(1,width-28);
    float label=workbench_text_height(&a->workbench_fonts[8],"PHYSIM API / REFERENZ",21,inner,12*scale,.8f*scale);
    return fmaxf(236,28+label+10*scale+workbench_card_text_height(&a->workbench_fonts[9],"Kein API-Bezeichner ausgewählt.",inner)+
        14*scale+workbench_card_text_height(&a->workbench_fonts[1],"Bezeichner im Code wählen; Umschalt+F1.",inner)+14*scale+30*scale);
}
static void workbench_api_empty(app *a,float height) {
    struct nk_context *ui=a->ui;float scale=a->preferences.ui_size/16.f;
    const struct nk_user_font *label=&a->workbench_fonts[8],*body=&a->workbench_fonts[9],*small=&a->workbench_fonts[1];
    ps_ui_row_exact_dynamic(ui,height,1);struct nk_rect card;nk_widget(&card,ui);
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);workbench_api_frame(canvas,card,7,UI_COLOR(edit));
    float width=card.w-28;struct nk_rect caption=nk_rect(card.x+14,card.y+14,width,workbench_text_height(label,"PHYSIM API / REFERENZ",21,width,12*scale,.8f*scale));
    workbench_text_draw(a,label,caption,"PHYSIM API / REFERENZ",21,12*scale,.8f*scale,muted());
    struct nk_rect message=nk_rect(card.x+14,caption.y+caption.h+10*scale,width,workbench_card_text_height(body,"Kein API-Bezeichner ausgewählt.",width));
    struct nk_rect help=nk_rect(card.x+14,message.y+message.h+14*scale,width,workbench_card_text_height(small,"Bezeichner im Code wählen; Umschalt+F1.",width));
    struct nk_rect action=nk_rect(card.x+14,card.y+card.h-14-30*scale,width,30*scale);
    a->workbench_api_text_bounds[0]=message;a->workbench_api_text_bounds[1]=help;a->workbench_api_text_bounds[2]=action;
    a->api_card_bounds=card;a->api_card_action_bounds=action;
    workbench_card_text(canvas,body,message,"Kein API-Bezeichner ausgewählt.",UI_COLOR(text));
    workbench_card_text(canvas,small,help,"Bezeichner im Code wählen; Umschalt+F1.",muted());
    struct nk_rect visible=plot_intersection(action,ui->current->layout->clip);
    bool hover=visible.w>0 && visible.h>0 && nk_input_is_mouse_hovering_rect(&ui->input,visible);
    workbench_api_frame(canvas,action,6,hover?UI_COLOR(button_hover):UI_COLOR(edit));
    nk_draw_text(canvas,nk_rect(action.x+9,action.y+(action.h-body->height)/2,action.w-18,body->height),"API-Referenz öffnen",20,body,nk_rgba(0,0,0,0),UI_COLOR(text));
    if(ps_ui_button_at(ui,action,"API-Referenz öffnen",false)){struct nk_text_edit *edit=editor_search_target(a);if(edit)api_reference_from_editor(a,edit);}
}
