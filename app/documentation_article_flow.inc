/* CSS struts and collapsed block margins; code grids retain their ink pitch. */
static float documentation_block_line_height(const app *a,const ps_doc_block *b) {
    float size=(float)documentation_font_size(a);
    if(b->code_line_height>0)return b->code_line_height;
    if(b->kind==PS_DOC_CODE)return .8f*documentation_code_size(a)*1.7f;
    if(b->kind==PS_DOC_TABLE_ROW)return .8125f*size*1.5f;
    if(b->kind==PS_DOC_HEADING)return b->level==1?(size==32?1.3f:1.65f)*size*1.2f:
        b->level==2?1.25f*size*1.4f:1.05f*size*1.65f;
    return documentation_is_lead(a,b)?.94f*size*1.7f:size*1.65f;
}
static float documentation_span_line_height(const app *a,const ps_doc_block *b,const ps_doc_span *span) {
    if(b->kind!=PS_DOC_HEADING && span && (span->style&PS_DOC_INLINE_CODE)) {
        float parent=b->kind==PS_DOC_TABLE_ROW?.8125f:documentation_is_lead(a,b)?.94f:1.f;
        return parent*.9f*documentation_code_size(a)*1.7f;
    }
    return documentation_block_line_height(a,b);
}
static float documentation_block_gap(const app *a,const ps_doc_block *b,float *previous) {
    float top=0,bottom=0;
    if(b->kind==PS_DOC_LINK && b->inline_parent!=SIZE_MAX)return 0;
    if(b->kind==PS_DOC_HEADING) {
        top=b->level==1?0:b->level==2?27:24;
        bottom=b->level==1?20:b->level==2?13:12;
    } else if(b->kind==PS_DOC_TEXT) {top=12;bottom=documentation_is_lead(a,b)?27:12;}
    else if(b->kind==PS_DOC_MATH){top=bottom=16;}
    else if(b->kind==PS_DOC_IMAGE){top=17;bottom=25;}
    else if(b->kind==PS_DOC_RULE){top=bottom=16;}
    float gap=fmaxf(*previous,top);*previous=bottom;return gap;
}
/* Keep a base plus zero-advance marks together when wrapping tracked headings. */
static size_t documentation_rich_cluster(const struct nk_user_font *font,const char *text,size_t count,float *width) {
    nk_rune rune;int used=nk_utf_decode(text,&rune,(int)count);if(used<=0)used=1;
    size_t length=(size_t)used;
    if(font->query==documentation_spaced_query && *text!='\t') {
        const documentation_spaced_font *spaced=font->userdata.ptr;
        while(length<count && text[length]!='\n' && text[length]!='\r' && text[length]!='\t') {
            int next=nk_utf_decode(text+length,&rune,(int)(count-length));if(next<=0)break;
            if(spaced->base->width(spaced->base->userdata,font->height,text+length,next)>0)break;
            length+=(size_t)next;
        }
        *width=font->width(font->userdata,font->height,text,(int)length);
    } else *width=documentation_glyph_width(font,text,used);
    return length;
}
static float documentation_rich_advance(const struct nk_user_font *font,const char *text,size_t count,int bytes) {
    if(font->query!=documentation_spaced_query || *text=='\t')return documentation_glyph_width(font,text,bytes);
    nk_rune rune,next=0;nk_utf_decode(text,&rune,bytes);
    if((size_t)bytes<count)nk_utf_decode(text+bytes,&next,(int)(count-(size_t)bytes));
    struct nk_user_font_glyph glyph={0};font->query(font->userdata,font->height,&glyph,rune,next);
    return glyph.xadvance;
}
/* Add a preceding pair only after accepting the next cluster on this line.
 * A forced wrap must never retain kerning toward the following line. */
static float documentation_rich_join(const struct nk_user_font *font,const ps_document *document,const ps_doc_block *block,
                                    const char *text,size_t begin,size_t at,size_t end) {
    if(font->query!=documentation_spaced_query || at<=begin || at>=end)return 0;
    const ps_doc_span *span=documentation_span_at(document,block,block->text+at-1);
    if(span && span->formula)return 0;
    size_t previous=at-1;while(previous>begin && ((unsigned char)text[previous]&0xc0)==0x80)--previous;
    nk_rune first=0,second=0;
    nk_utf_decode(text+previous,&first,(int)(at-previous));nk_utf_decode(text+at,&second,(int)(end-at));
    const documentation_spaced_font *spaced=font->userdata.ptr;
    return ps_ui_font_pair_advance(spaced->pairs,font->height,first,second);
}
static float documentation_rich_width(const struct nk_user_font *font,const char *text,size_t count) {
    float width=0;
    for(size_t at=0;at<count;) {
        const char *tab=memchr(text+at,'\t',count-at);size_t end=tab?(size_t)(tab-text):count;
        width+=font->width(font->userdata,font->height,text+at,(int)(end-at));
        if(tab)width+=documentation_glyph_width(font,"\t",1);
        at=end+(tab?1:0);
    }
    return width;
}
