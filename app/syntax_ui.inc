/* One active palette and token stream for editable code and inert signatures.
 * Glyph painting does not alter the editor's text, selection, caret or Undo. */
static struct nk_color syntax_color(ps_syntax_role role){
    switch(role){
    case PS_SYNTAX_COMMENT:return UI_COLOR(code_comment);
    case PS_SYNTAX_STRING:return UI_COLOR(code_string);
    case PS_SYNTAX_KEYWORD:return UI_COLOR(code_keyword);
    case PS_SYNTAX_TYPE:return UI_COLOR(code_type);
    case PS_SYNTAX_PREPROCESSOR:return UI_COLOR(code_preprocessor);
    case PS_SYNTAX_NUMBER:return UI_COLOR(code_number);
    case PS_SYNTAX_FUNCTION:return UI_COLOR(code_function);
    case PS_SYNTAX_VARIABLE:return UI_COLOR(code_variable);
    case PS_SYNTAX_CONTROL:return UI_COLOR(code_control);
    default:return UI_COLOR(code_text);
    }
}
typedef struct {ps_syntax_scan scan;ps_syntax_token token;bool pending;} syntax_cursor;
static void syntax_draw_range(struct nk_command_buffer *canvas,const struct nk_user_font *font,syntax_cursor *cursor,
                              size_t begin,size_t end,float x,float y){
    while(cursor->pending || (cursor->pending=ps_syntax_next(&cursor->scan,&cursor->token))){
        ps_syntax_token token=cursor->token;
        if(token.begin>=end)break;
        size_t first=token.begin>begin?token.begin:begin,last=token.end<end?token.end:end;
        if(last>first){
            const char *text=cursor->scan.source+first;int n=(int)(last-first);
            float width=font->width(font->userdata,font->height,text,n);
            nk_draw_text(canvas,nk_rect(x,y,width+1,font->height),text,n,font,nk_rgba(0,0,0,0),syntax_color(token.role));
            x+=width;
        }
        if(token.end>end)break;
        cursor->pending=false;
    }
}
static ps_syntax_role syntax_symbol_kind(const ps_completion_symbol *symbol){
    return symbol->kind==PS_COMPLETION_FUNCTION?PS_SYNTAX_FUNCTION:
        symbol->kind==PS_COMPLETION_TYPE?PS_SYNTAX_TYPE:PS_SYNTAX_VARIABLE;
}
static ps_syntax_role syntax_outline_role(void *user,size_t offset,size_t bytes){
    app *a=user;
    if(!a->outline_cached || offset>a->outline_bytes || bytes>a->outline_bytes-offset)return PS_SYNTAX_TEXT;
    for(size_t i=0;i<a->outline.count;i++){
        ps_outline_symbol symbol=a->outline.symbols[i];
        if(symbol.length==bytes && !memcmp(a->outline_source+symbol.offset,a->outline_source+offset,bytes))
            return symbol.kind==PS_OUTLINE_TYPE?PS_SYNTAX_TYPE:symbol.kind==PS_OUTLINE_FUNCTION?PS_SYNTAX_FUNCTION:PS_SYNTAX_VARIABLE;
    }
    return PS_SYNTAX_TEXT;
}
typedef struct {app *a;const char *source;bool locals_current;} syntax_editor_context;
static ps_syntax_role syntax_editor_role(void *user,size_t offset,size_t bytes){
    syntax_editor_context *c=user;app *a=c->a;const ps_completion_locals *p=&a->completion_locals;
    /* Only revision-bound declaration spans and the current query are known to
     * this completion snapshot. Never apply the caret's scope to other blocks
     * or to a same-spelled type annotation elsewhere in the file. */
    if(c->locals_current){
        if(!a->completion_query.member && offset==a->completion_query.start &&
           ps_completion_locals_find(p,c->source+offset,bytes))return PS_SYNTAX_VARIABLE;
        for(size_t i=0;i<p->count;i++)if(p->entries[i].declaration.kind!=PS_LANG_EDITOR_SELF &&
            p->entries[i].declaration.name.offset==offset && p->entries[i].declaration.name.length==bytes)return PS_SYNTAX_VARIABLE;
    }
    return syntax_outline_role(a,offset,bytes);
}
