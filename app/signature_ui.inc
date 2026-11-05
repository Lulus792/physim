/* ADR023: an independent, inert call HUD. It never owns editor input, candidate
 * selection or insertion. Current source identity is checked before publication,
 * including the failure-preserving locals cache. */
static void signature_suppression_clear(app *a){
    free(a->signature_suppressed_callee);a->signature_suppressed_callee=NULL;
    a->signature_suppressed_bytes=0;a->signature_suppressed_owner=NULL;
}
static void signature_clear(app *a){
    ps_code_preview_destroy(a->signature_layout);a->signature_layout=NULL;
    ps_signature_help_destroy(&a->signature_help);
    free(a->signature_status);a->signature_status=NULL;
    a->signature_visible=false;signature_suppression_clear(a);
    a->signature_hud_bounds=a->signature_field_bounds=a->signature_status_bounds=nk_rect(0,0,0,0);
    a->signature_body_bounds=nk_rect(0,0,0,0);a->signature_scroll_x=a->signature_scroll_y=0;a->signature_scroll_max=0;
}
static bool signature_eligible(app *a,struct nk_text_edit *edit){
    return edit && completion_target(a)==edit && document_language(a,edit)>=0 && edit->active &&
        edit->select_start==edit->select_end && !a->completion_composing &&
        !(a->show_search && (a->search_keyboard || a->search_focus));
}
static bool signature_current(app *a,struct nk_text_edit *edit){
    ps_signature_help *h=&a->signature_help;
    const char *text=edit?nk_str_get_const(&edit->string):NULL;
    size_t bytes=edit?(size_t)nk_str_len_char(&edit->string):0;
    return signature_eligible(a,edit) && h->owner==edit && h->language==(document_language(a,edit)==1) &&
        h->cursor==editor_search_byte(edit,edit->cursor) && h->bytes==bytes && h->source &&
        !memcmp(h->source,text?text:"",bytes);
}
static void signature_update(app *a,struct nk_text_edit *edit,bool manual){
    a->signature_visible=false;
    if(!signature_eligible(a,edit)){
        if(!edit || completion_target(a)!=a->signature_help.owner || a->completion_composing ||
           (edit && edit->select_start!=edit->select_end))signature_suppression_clear(a);
        return;
    }
    const char *text=nk_str_get_const(&edit->string);if(!text)text="";
    size_t bytes=(size_t)nk_str_len_char(&edit->string),cursor=editor_search_byte(edit,edit->cursor);
    bool same=signature_current(a,edit);
    if(!same){
        /* Row maps borrow the display; discard before a model update can free it. */
        ps_code_preview_destroy(a->signature_layout);a->signature_layout=NULL;a->signature_reveal=true;
        free(a->signature_status);a->signature_status=NULL;
        if(ps_completion_locals_update(&a->completion_locals,edit,text,bytes,cursor,document_language(a,edit)==1)!=PS_OK)return;
        ps_completion_locals *l=&a->completion_locals;
        if(l->owner!=edit || l->bytes!=bytes || l->cursor!=cursor || l->language!=(document_language(a,edit)==1) ||
           !l->source || memcmp(l->source,text,bytes) || ps_signature_help_update(&a->signature_help,l)!=PS_OK ||
           !signature_current(a,edit))return;
    }
    ps_signature_help *h=&a->signature_help;
    if(h->state!=PS_SIGNATURE_READY || !h->display || !h->call.active){signature_suppression_clear(a);return;}
    size_t name_bytes=h->call.callee_end-h->call.callee_start;
    if(manual || a->signature_suppressed_owner!=edit || a->signature_suppressed_language!=h->language || a->signature_suppressed_open!=h->call.open ||
       (a->signature_suppressed_callee && (a->signature_suppressed_bytes!=name_bytes ||
        memcmp(a->signature_suppressed_callee,h->source+h->call.callee_start,name_bytes))))signature_suppression_clear(a);
    if(a->signature_suppressed_owner==edit)return;
    if(!same || !a->signature_status){
        ps_signature_display *d=h->display;char prefix[128];size_t begin=0,length=0;
        if(d->unspecified)snprintf(prefix,sizeof prefix,"Argument %zu · Parameter nicht angegeben",h->call.argument+1);
        else if(!d->count && h->call.at_start && d->variadic_end==d->variadic_begin)snprintf(prefix,sizeof prefix,"Keine Parameter erwartet");
        else if(h->active_parameter!=SIZE_MAX){
            ps_signature_parameter p=d->parameters[h->active_parameter];begin=p.begin;length=p.end-p.begin;
            snprintf(prefix,sizeof prefix,"Parameter %zu von %zu · ",h->active_parameter+1,d->count);
        }else if(d->variadic_end>d->variadic_begin && h->call.argument>=d->count)
            snprintf(prefix,sizeof prefix,"Argument %zu · variabler Parameterbereich",h->call.argument+1);
        else snprintf(prefix,sizeof prefix,"Argument %zu · erwartet %zu Parameter",h->call.argument+1,d->count);
        size_t n=strlen(prefix);char *status=malloc(n+length+1);if(!status)return;
        memcpy(status,prefix,n);memcpy(status+n,d->text+begin,length);status[n+length]=0;
        free(a->signature_status);a->signature_status=status;
    }
    a->signature_visible=true;
    if(manual)a->signature_reveal=true;
}
static bool signature_keyboard_key(app *a,const SDL_KeyboardEvent *key){
    struct nk_text_edit *edit=completion_target(a);
    if(a->signature_visible && signature_current(a,edit) && !(a->completion_active && a->completion_info_visible) &&
       (key->mod&SDL_KMOD_ALT) && !(key->mod&(SDL_KMOD_CTRL|SDL_KMOD_SHIFT|SDL_KMOD_GUI)) &&
       (key->key==SDLK_PAGEUP || key->key==SDLK_PAGEDOWN || key->key==SDLK_HOME || key->key==SDLK_END)){
        float next=(float)a->signature_scroll_y,page=fmaxf(1,a->signature_body_bounds.h*.85f);
        if(key->key==SDLK_HOME)next=0;else if(key->key==SDLK_END)next=a->signature_scroll_max;
        else next+=key->key==SDLK_PAGEUP?-page:page;
        a->signature_scroll_y=(nk_uint)fmaxf(0,fminf(a->signature_scroll_max,next));a->signature_reveal=false;return true;
    }
    if(key->key==SDLK_SPACE && (key->mod&(SDL_KMOD_CTRL|SDL_KMOD_SHIFT))==(SDL_KMOD_CTRL|SDL_KMOD_SHIFT) &&
       !(key->mod&(SDL_KMOD_ALT|SDL_KMOD_GUI)) && signature_eligible(a,edit)){
        if(!key->repeat)signature_update(a,edit,true);return true;
    }
    if(key->key!=SDLK_ESCAPE || a->completion_active || a->completion_pending_edit ||
       !a->signature_visible || !signature_current(a,edit))return false;
    ps_signature_help *h=&a->signature_help;
    signature_suppression_clear(a);
    a->signature_suppressed_owner=edit;a->signature_suppressed_open=h->call.open;
    a->signature_suppressed_language=h->language;
    a->signature_suppressed_bytes=h->call.callee_end-h->call.callee_start;
    a->signature_suppressed_callee=malloc(a->signature_suppressed_bytes+1);
    if(a->signature_suppressed_callee){
        memcpy(a->signature_suppressed_callee,h->source+h->call.callee_start,a->signature_suppressed_bytes);
        a->signature_suppressed_callee[a->signature_suppressed_bytes]=0;
    }
    /* If even the tiny suppression key cannot be allocated, keep this call
     * hidden until manual reopening or leaving it; never show stale data. */
    a->signature_visible=false;return true;
}
static ps_syntax_role signature_role(void *user,size_t offset,size_t bytes){
    ps_signature_display *d=user;
    if(offset==d->name_begin && bytes==d->name_end-d->name_begin)return PS_SYNTAX_FUNCTION;
    for(size_t i=0;i<d->count;i++)if(offset==d->parameters[i].name_begin &&
        bytes==d->parameters[i].name_end-d->parameters[i].name_begin)return PS_SYNTAX_VARIABLE;
    return PS_SYNTAX_TEXT;
}
#include "signature_render.inc"
