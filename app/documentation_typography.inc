/* H's CSS em sizes differ from stb's ascent/descent pixel heights. Keep the
 * atlas banks unchanged and give each drawing role a persistent handle; draw
 * commands retain these addresses until the native frame has been rendered. */
enum {
    DOC_TYPE_BODY,DOC_TYPE_STRONG,DOC_TYPE_TITLE,DOC_TYPE_H2,DOC_TYPE_H3,
    DOC_TYPE_CODE,DOC_TYPE_INLINE_CODE,DOC_TYPE_CHROME,DOC_TYPE_CONTROL,
    DOC_TYPE_CONTROL_STRONG,DOC_TYPE_NAV,DOC_TYPE_NAV_STRONG,DOC_TYPE_SECTION,
    DOC_TYPE_SECTION_STRONG,DOC_TYPE_GROUP,DOC_TYPE_CONTEXT_TITLE,
    DOC_TYPE_CONTEXT,DOC_TYPE_SOURCE,DOC_TYPE_CONTEXT_CODE,DOC_TYPE_CRUMB,DOC_TYPE_GRID,
    DOC_TYPE_COUNT
};
_Static_assert(DOC_TYPE_COUNT==21,"Update the persistent H font role bank");
static const struct nk_user_font *documentation_type_font(const app *a,unsigned role) {
    return a->doc_type_fonts[role].userdata.ptr?&a->doc_type_fonts[role]:a->doc_font_ui;
}
static void documentation_typography_update(app *a) {
    unsigned reader=(documentation_font_size(a)-16)/2,chrome=(a->preferences.ui_size-16)/2;
    unsigned code=(documentation_code_size(a)-16)/2;
    float reading=(float)documentation_font_size(a),ui=a->preferences.ui_size/16.f;
    const float sizes[DOC_TYPE_COUNT]={reading,reading,(reading==32?1.3f:1.65f)*reading,1.25f*reading,1.05f*reading,
        .8f*documentation_code_size(a),.9f*documentation_code_size(a),11*ui,12*ui,12*ui,
        13*ui,13*ui,12*ui,12*ui,10*ui,15*ui,12*ui,11*ui,
        .75f*(a->preferences.editor_size<16?16:a->preferences.editor_size),.7f*reading,.8f*documentation_code_size(a)};
    const struct nk_user_font *sources[DOC_TYPE_COUNT]={
        a->doc_ui_fonts[reader],a->doc_bold_fonts[reader],a->doc_title_fonts[reader],
        a->doc_bold_fonts[reader+2],a->doc_bold_fonts[reader],a->doc_code_fonts[code],a->doc_code_fonts[code],
        a->doc_chrome_fonts[chrome],a->doc_ui_fonts[chrome],a->doc_bold_fonts[chrome],
        a->doc_ui_fonts[chrome],a->doc_bold_fonts[chrome],a->doc_ui_fonts[chrome],a->doc_bold_fonts[chrome],
        a->doc_chrome_fonts[chrome],a->doc_bold_fonts[chrome],a->doc_ui_fonts[chrome],a->doc_chrome_fonts[chrome],
        a->doc_code_fonts[((a->preferences.editor_size<16?16:a->preferences.editor_size)-16)/2],a->doc_ui_fonts[reader],a->doc_code_fonts[code]};
    const float em[DOC_TYPE_COUNT]={
        a->doc_font_em[0][reader],a->doc_font_em[2][reader],a->doc_font_em[1][reader],
        a->doc_font_em[2][reader+2],a->doc_font_em[2][reader],a->doc_font_em[3][code],a->doc_font_em[3][code],
        a->doc_font_em[4][chrome],a->doc_font_em[0][chrome],a->doc_font_em[2][chrome],
        a->doc_font_em[0][chrome],a->doc_font_em[2][chrome],a->doc_font_em[0][chrome],a->doc_font_em[2][chrome],
        a->doc_font_em[4][chrome],a->doc_font_em[2][chrome],a->doc_font_em[0][chrome],a->doc_font_em[4][chrome],
        a->doc_font_em[3][((a->preferences.editor_size<16?16:a->preferences.editor_size)-16)/2],a->doc_font_em[0][reader],a->doc_font_em[3][code]};
    for(unsigned i=0;i<DOC_TYPE_COUNT;i++) {
        const struct nk_user_font *source=sources[i];
        if(!source)continue;
        /* The source TTF blobs have already been released after baking. These
         * metrics were retained while each candidate's data was still alive. */
        a->doc_type_em[i]=em[i];
        a->doc_type_fonts[i]=*source;a->doc_type_fonts[i].height=sizes[i]*a->doc_type_em[i];
    }
    /* nk_draw_text stores integer logical positions. Use an integer glyph
     * pitch too, so successive rows cannot acquire rounding gaps. Adjust the
     * handle itself: measurement and drawing keep the same complete glyph. */
    struct nk_user_font *grid=&a->doc_type_fonts[DOC_TYPE_GRID];
    if(grid->query && ps_ui_font_has_glyph(grid,0x2502)) {
        struct nk_user_font_glyph vertical={0};grid->query(grid->userdata,grid->height,&vertical,0x2502,0);
        if(vertical.height>=1)grid->height*=floorf(vertical.height+.0001f)/vertical.height;
    }
}
/* Read the queued drawing payload, not only the configured roles. */
static bool documentation_typography_probe(app *a,const char *capture) {
    if(!capture)return true;
    const char *name=capture,*slash=strrchr(capture,'/'),*backslash=strrchr(capture,'\\');
    if(slash)name=slash+1;if(backslash && backslash+1>name)name=backslash+1;
    unsigned index=0;int consumed=0;
    if(sscanf(name,"docs-h-%u.bmp%n",&index,&consumed)!=1 || name[consumed])return true;
    unsigned title=0,body=0;const struct nk_command *command;
    nk_foreach(command,a->doc_ui)if(command->type==NK_COMMAND_TEXT) {
        const struct nk_command_text *text=(const struct nk_command_text *)command;
        if(text->font==documentation_type_font(a,DOC_TYPE_TITLE) && text->length>0)++title;
        if(text->font==documentation_type_font(a,DOC_TYPE_BODY) && text->length>0)++body;
    }
    const unsigned roles[]={DOC_TYPE_BODY,DOC_TYPE_TITLE,DOC_TYPE_CONTROL,DOC_TYPE_NAV};float em[4];
    for(unsigned i=0;i<4;i++) {
        const struct nk_user_font *font=documentation_type_font(a,roles[i]);
        em[i]=font->height/a->doc_type_em[roles[i]];
    }
    printf("DOCUMENTATION H TYPE: case=%u reader=%u ui=%u body-em=%.3f title-em=%.3f control-em=%.3f nav-em=%.3f title-draws=%u body-draws=%u\n",
        index,documentation_font_size(a),a->preferences.ui_size,em[0],em[1],em[2],em[3],title,body);
    printf("DOCUMENTATION H METRIC: case=%u primary-em=%.6f body-height=%.3f title-height=%.3f\n",index,
        a->doc_type_em[DOC_TYPE_BODY],documentation_type_font(a,DOC_TYPE_BODY)->height,documentation_type_font(a,DOC_TYPE_TITLE)->height);
    return title>0;
}
