/* ADR 015/018: copied channel identities and independent drafts outlive source
 * views. No pointer into a runner/data worker or a source window is retained. */
typedef struct {ps_channel channel;ps_display_unit draft;char factor[64];bool si;} channel_unit_edit;
typedef struct channel_unit_task {
    SDL_Window *window;ps_graphics *graphics;struct nk_context *ui;SDL_WindowID source;
    channel_unit_edit edits[PS_CHANNEL_UNIT_MAX];unsigned count,selected,focus,menu_owner,menu_index;
    bool keyboard,first,close,apply,popup_close,focus_scroll,message_scroll,menu_focus,menu_keyboard;struct nk_rect controls[9],body_rect,message_rect;
    struct nk_user_font fonts[6];char capture[4096],message[192],source_label[96];
} channel_unit_task;
enum {UNIT_CHANNEL,UNIT_PRESET,UNIT_SYMBOL,UNIT_FACTOR,UNIT_SI,UNIT_CLOSE,UNIT_APPLY,UNIT_RECOVER,UNIT_EXIT};
static void channel_unit_store(app *a) {
    channel_unit_task *d=a->unit_task;if(!d || !d->count)return;
    channel_unit_edit *e=d->edits+d->selected;e->draft=a->unit_draft;SDL_strlcpy(e->factor,a->channel_unit_scale,sizeof e->factor);
}
static void channel_unit_select(app *a,unsigned index) {
    channel_unit_task *d=a->unit_task;if(!d || index>=d->count)return;
    channel_unit_store(a);d->selected=index;a->unit_channel=d->edits[index].channel;a->unit_draft=d->edits[index].draft;
    SDL_strlcpy(a->channel_unit_scale,d->edits[index].factor,sizeof a->channel_unit_scale);d->message[0]=0;
    if(a->channel_units_writable)a->channel_units_error[0]=0;
}
static unsigned channel_unit_add(app *a,const ps_channel *channel) {
    channel_unit_task *d=a->unit_task;
    for(unsigned i=0;i<d->count;i++)if(!strcmp(d->edits[i].channel.name,channel->name) && !memcmp(d->edits[i].channel.dimension,channel->dimension,7))return i;
    if(d->count==PS_CHANNEL_UNIT_MAX)return d->count;
    unsigned at=d->count++;channel_unit_edit *e=d->edits+at;e->channel=*channel;e->draft=channel_display_unit(a,channel);
    snprintf(e->factor,sizeof e->factor,"%.17g",e->draft.scale);return at;
}
static void channel_unit_restore_context(app *a,SDL_Window *window,SDL_GLContext context) {
    if(window && context)SDL_GL_MakeCurrent(window,context);else ps_graphics_make_current(a->graphics);
}
static void channel_unit_window_destroy(app *a) {
    channel_unit_task *d=a->unit_task;if(!d)return;
    SDL_Window *previous=SDL_GL_GetCurrentWindow();SDL_GLContext context=SDL_GL_GetCurrentContext();if(previous==d->window){previous=NULL;context=NULL;}
    if(d->graphics){ps_graphics_make_current(d->graphics);if(d->ui)nk_sdl_shutdown(d->ui);ps_graphics_destroy(d->graphics);}
    if(d->window)SDL_DestroyWindow(d->window);free(d);a->unit_task=NULL;a->channel_unit_manager=false;channel_unit_restore_context(a,previous,context);
}
static void channel_unit_close(app *a) {
    channel_unit_task *d=a->unit_task;if(!d || d->close)return;
    SDL_Window *source=SDL_GetWindowFromID(d->source);bool focus=source && SDL_GetKeyboardFocus()==d->window && !(SDL_GetWindowFlags(source)&(SDL_WINDOW_HIDDEN|SDL_WINDOW_MINIMIZED));
    d->close=true;a->channel_unit_manager=false;SDL_HideWindow(d->window);
    if(focus){SDL_RaiseWindow(source);struct nk_context *owner=source==a->window?a->ui:source==a->settings_window?a->settings_ui:source==a->doc_window?a->doc_ui:NULL;
        for(unsigned i=0;!owner && i<PS_PANEL_COUNT;i++)if(a->panels[i].window==source)owner=a->panels[i].ui;
        if(owner && *d->source_label)(void)nk_sdl_accessibility_request_focus(owner,NULL,d->source_label);}
}
static void channel_unit_open(app *a,const ps_channel *channel) {
    channel_unit_task *d=a->unit_task;
    SDL_Window *previous=SDL_GL_GetCurrentWindow();SDL_GLContext context=SDL_GL_GetCurrentContext();
    if(d && d->close){bool deleting_current=previous==d->window;channel_unit_window_destroy(a);d=NULL;if(deleting_current){previous=SDL_GL_GetCurrentWindow();context=SDL_GL_GetCurrentContext();}}
    if(d){unsigned index=channel_unit_add(a,channel);if(index<d->count)channel_unit_select(a,index);else snprintf(a->channel_units_error,sizeof a->channel_units_error,"Alle 64 Kanalentwürfe sind belegt. Aufgabe zuerst schließen.");SDL_ShowWindow(d->window);SDL_RaiseWindow(d->window);return;}
    d=calloc(1,sizeof *d);if(!d){status(a,"Nicht genug Speicher für die Einheitenaufgabe.");return;}a->unit_task=d;
    SDL_Window *source=SDL_GetKeyboardFocus();d->source=SDL_GetWindowID(source?source:a->window);
    if(!ps_graphics_make_current(a->graphics) || !SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT,1))goto fail;
    d->window=SDL_CreateWindow("Physim | Anzeigeeinheit",850,(int)ceilf(473.f*a->preferences.ui_size/16.f),SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE|SDL_WINDOW_HIGH_PIXEL_DENSITY|SDL_WINDOW_HIDDEN);
    if(d->window)d->graphics=ps_graphics_create(d->window);SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT,0);if(!d->graphics)goto fail;
    SDL_GL_SetSwapInterval(0);d->ui=nk_sdl_init(d->window,d->graphics);if(!d->ui)goto fail;
    nk_sdl_borrow_fonts(d->ui,a->ui);nk_input_begin(d->ui);nk_input_end(d->ui);
    /* The requested channel is first even when its originating run closes. */
    channel_unit_add(a,channel);
    for(unsigned i=0;i<a->channel_count;i++)channel_unit_add(a,a->live_channels+i);
    if(!a->data.thread)for(unsigned i=0;i<a->data.channel_count;i++)channel_unit_add(a,a->data.channels+i);
    a->unit_channel=d->edits[0].channel;a->unit_draft=d->edits[0].draft;SDL_strlcpy(a->channel_unit_scale,d->edits[0].factor,sizeof a->channel_unit_scale);
    if(a->channel_units_writable)a->channel_units_error[0]=0;
    d->keyboard=d->first=d->focus_scroll=true;d->focus=UNIT_CHANNEL;a->channel_unit_manager=true;nk_sdl_accessibility_keyboard_begin(d->ui);
    SDL_SetWindowMinimumSize(d->window,360,360);SDL_ShowWindow(d->window);SDL_RaiseWindow(d->window);channel_unit_restore_context(a,previous,context);return;
fail:
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT,0);channel_unit_window_destroy(a);channel_unit_restore_context(a,previous,context);status(a,"Die Einheitenaufgabe konnte nicht geöffnet werden.");
}
static void channel_unit_open_from(app *a,const ps_channel *channel,SDL_Window *source,const char *label) {
    SDL_Window *caller=SDL_GL_GetCurrentWindow();SDL_GLContext context=SDL_GL_GetCurrentContext();
    channel_unit_open(a,channel);if(!a->unit_task || a->unit_task->close)return;
    a->unit_task->source=SDL_GetWindowID(source);SDL_strlcpy(a->unit_task->source_label,label,sizeof a->unit_task->source_label);
    if(SDL_getenv("PHYSIM_TEST_TRACE"))fprintf(stderr,"UNIT CALLER CONTEXT: source=%u caller=%u restored=%u same=%d\n",SDL_GetWindowID(source),SDL_GetWindowID(caller),SDL_GetWindowID(SDL_GL_GetCurrentWindow()),SDL_GL_GetCurrentContext()==context);
}
static void channel_unit_si(app *a) {
    channel_unit_task *d=a->unit_task;if(!d)return;
    ps_channel_units empty={0};ps_display_unit si;if(ps_channel_units_get(&empty,&a->unit_channel,&si)!=PS_OK)return;
    a->unit_draft=si;SDL_strlcpy(a->channel_unit_scale,"1",sizeof a->channel_unit_scale);d->edits[d->selected].si=true;
    SDL_strlcpy(d->message,"SI ist nur im Einheitenentwurf gewählt. Mit Einheit anwenden bestätigen.",sizeof d->message);d->message_scroll=true;if(a->channel_units_writable)a->channel_units_error[0]=0;
}
static void channel_unit_apply(app *a) {
    channel_unit_task *d=a->unit_task;if(!d || !a->channel_units_writable)return;d->message_scroll=true;
    ps_display_unit u=a->unit_draft;
    if(!ps_parse_finite_number(a->channel_unit_scale,NULL,&u.scale) || !ps_display_unit_valid(&u)){
        snprintf(a->channel_units_error,sizeof a->channel_units_error,"Symbol: 1–63 UTF-8-Bytes ohne Rand-Leerzeichen oder Steuerzeichen. Faktor: positiv und endlich.");return;}
    bool ok=false;
    if(d->edits[d->selected].si){ps_channel_units next=a->channel_units;
        ok=channel_unit_fits(a,&a->unit_channel,&u) && ps_channel_units_remove(&next,&a->unit_channel)==PS_OK && channel_units_save(a,&next);}
    else ok=channel_unit_commit(a,&a->unit_channel,&u);
    if(ok){a->unit_draft=u;channel_unit_store(a);d->edits[d->selected].si=false;SDL_strlcpy(d->message,"Anzeigeeinheit angewendet. Allgemeine Einstellungen bleiben unverändert.",sizeof d->message);}
}
static const char *channel_unit_focus_name(unsigned id) {
    static const char *names[]={"Messkanal","Vorgabe","Symbol","Faktor · SI pro Anzeigeeinheit","SI wiederherstellen","Schließen","Einheit anwenden","Datei bewusst zurücksetzen","Einheitenaufgabe schließen"};return names[id];
}
static const char *channel_unit_pane(unsigned id) {return id==UNIT_EXIT?"unit-header":id>=UNIT_SI && id<=UNIT_APPLY?"unit-footer":"unit-body";}
static void channel_unit_activate(app *a) {
    channel_unit_task *d=a->unit_task;struct nk_context *ui=d->ui;
    if(!d->keyboard && !nk_sdl_accessibility_has_focus(ui))return;
    struct nk_window *owner=nk_window_find(ui,channel_unit_pane(d->focus));if(!owner || owner->popup.active)return;
    nk_window_set_focus(ui,channel_unit_pane(d->focus));owner->flags&=~(nk_flags)NK_WINDOW_ROM;
    if(ui->current==owner && owner->layout)owner->layout->flags&=~(nk_flags)NK_WINDOW_ROM;
}
static void channel_unit_flush(app *a) {
    channel_unit_task *d=a->unit_task;if(!d)return;
    if(d->focus==UNIT_SYMBOL)ps_ui_flush_edit(d->ui,"unit-body",a->unit_draft.symbol,sizeof a->unit_draft.symbol,d->controls[UNIT_SYMBOL]);
    else if(d->focus==UNIT_FACTOR)ps_ui_flush_edit(d->ui,"unit-body",a->channel_unit_scale,sizeof a->channel_unit_scale,d->controls[UNIT_FACTOR]);
    channel_unit_store(a);
}
static void channel_unit_choose_preset(app *a,unsigned index) {
    const channel_unit_preset *p=channel_unit_presets+index;
    if(memcmp(p->dimension,a->unit_channel.dimension,7))return;
    a->unit_draft.scale=p->scale;SDL_strlcpy(a->unit_draft.symbol,p->symbol,sizeof a->unit_draft.symbol);snprintf(a->channel_unit_scale,sizeof a->channel_unit_scale,"%.17g",p->scale);
    a->unit_task->edits[a->unit_task->selected].si=false;a->unit_task->message[0]=0;
    if(a->channel_units_writable)a->channel_units_error[0]=0;
}
static unsigned channel_unit_menu_count(app *a,unsigned id) {
    if(id==UNIT_CHANNEL)return a->unit_task->count;unsigned count=1;
    for(unsigned i=0;i<sizeof channel_unit_presets/sizeof channel_unit_presets[0];i++)if(!memcmp(channel_unit_presets[i].dimension,a->unit_channel.dimension,7))count++;return count;
}
static void channel_unit_menu_label(app *a,unsigned id,unsigned index,char text[160]) {
    channel_unit_task *d=a->unit_task;if(id==UNIT_CHANNEL){snprintf(text,160,"%s [%s]",d->edits[index].channel.name,d->edits[index].channel.unit);return;}
    for(unsigned i=0;i<sizeof channel_unit_presets/sizeof channel_unit_presets[0];i++)if(!memcmp(channel_unit_presets[i].dimension,a->unit_channel.dimension,7)){if(!index--){SDL_strlcpy(text,channel_unit_presets[i].symbol,160);return;}}
    SDL_strlcpy(text,"Eigene Einheit",160);
}
static void channel_unit_menu_return(app *a,unsigned id) {
    channel_unit_task *d=a->unit_task;if(!d->menu_keyboard)return;d->focus=id;d->keyboard=d->focus_scroll=true;nk_sdl_accessibility_keyboard_begin(d->ui);
}
static bool channel_unit_key(app *a,const SDL_KeyboardEvent *key) {
    channel_unit_task *d=a->unit_task;if(key->mod&(SDL_KMOD_CTRL|SDL_KMOD_GUI|SDL_KMOD_ALT))return false;
    struct nk_window *body=nk_window_find(d->ui,"unit-body");
    if(key->key==SDLK_ESCAPE){if(body && body->popup.active){d->popup_close=true;channel_unit_menu_return(a,d->menu_owner?d->menu_owner-1:d->focus);}else channel_unit_close(a);return true;}
    if(key->key==SDLK_TAB){channel_unit_flush(a);if(body && body->popup.active)d->popup_close=true;unsigned ids[9],n=0;ids[n++]=UNIT_CHANNEL;
        if(a->channel_units_writable){ids[n++]=UNIT_PRESET;ids[n++]=UNIT_SYMBOL;ids[n++]=UNIT_FACTOR;ids[n++]=UNIT_SI;ids[n++]=UNIT_CLOSE;ids[n++]=UNIT_APPLY;}
        else {ids[n++]=UNIT_RECOVER;ids[n++]=UNIT_CLOSE;}ids[n++]=UNIT_EXIT;
        int at=-1,step=key->mod&SDL_KMOD_SHIFT?-1:1;for(unsigned i=0;i<n;i++)if(ids[i]==d->focus)at=(int)i;
        d->focus=ids[(at+step+(int)n)%(int)n];d->keyboard=d->focus_scroll=true;nk_sdl_accessibility_keyboard_begin(d->ui);return true;}
    int direction=key->key==SDLK_LEFT || key->key==SDLK_UP?-1:key->key==SDLK_RIGHT || key->key==SDLK_DOWN?1:0;
    if(body && body->popup.active){if(direction && d->menu_owner){unsigned n=channel_unit_menu_count(a,d->menu_owner-1);d->menu_index=(d->menu_index+n+(direction<0?-1:1))%n;d->menu_focus=true;return true;}return false;}
    if(!d->keyboard || d->focus==UNIT_SYMBOL || d->focus==UNIT_FACTOR)return false;
    bool activate=key->key==SDLK_RETURN || key->key==SDLK_KP_ENTER || key->key==SDLK_SPACE;if(!activate && !direction)return false;
    if(key->repeat && activate)return true;
    if(d->focus==UNIT_CLOSE || d->focus==UNIT_EXIT){if(activate)channel_unit_close(a);return true;}
    if(d->focus==UNIT_CHANNEL){if(direction)channel_unit_select(a,(d->selected+d->count+(direction<0?-1:1))%d->count);else {d->menu_keyboard=true;d->keyboard=false;nk_sdl_accessibility_keyboard_release(d->ui);return false;}return true;}
    if(!a->channel_units_writable){if(d->focus==UNIT_RECOVER && activate){ps_channel_units empty={0};channel_units_save(a,&empty);}return true;}
    if(d->focus==UNIT_PRESET && direction){unsigned matches[32],n=0,at=0;double scale=0;ps_parse_finite_number(a->channel_unit_scale,NULL,&scale);
        for(unsigned i=0;i<sizeof channel_unit_presets/sizeof channel_unit_presets[0];i++)if(!memcmp(channel_unit_presets[i].dimension,a->unit_channel.dimension,7)){if(scale==channel_unit_presets[i].scale && !strcmp(a->unit_draft.symbol,channel_unit_presets[i].symbol))at=n;matches[n++]=i;}
        if(n)channel_unit_choose_preset(a,matches[(at+n+(direction<0?-1:1))%n]);return true;}
    if(d->focus==UNIT_PRESET){d->menu_keyboard=true;d->keyboard=false;nk_sdl_accessibility_keyboard_release(d->ui);return false;}
    if(d->focus==UNIT_SI && activate)channel_unit_si(a);else if(d->focus==UNIT_APPLY && activate)d->apply=true;else return false;return true;
}
static bool channel_unit_event(app *a,const SDL_Event *event,SDL_Window *window) {
    channel_unit_task *d=a->unit_task;if(!d || window!=d->window)return false;
    if(event->type==SDL_EVENT_WINDOW_CLOSE_REQUESTED){channel_unit_close(a);return true;}if(d->close)return true;
    if(event->type==SDL_EVENT_WINDOW_RESIZED){if(d->focus>=UNIT_SI && (*a->channel_units_error || *d->message))d->message_scroll=true;else d->focus_scroll=true;}
    if(event->type==SDL_EVENT_KEY_DOWN && channel_unit_key(a,&event->key))return true;
    if(nk_sdl_accessibility_event(d->ui,event))return true;SDL_Event copy=*event;nk_sdl_handle_event(d->ui,&copy);return true;
}
static void channel_unit_tick(app *a) {
    channel_unit_task *d=a->unit_task;if(!d)return;if(d->close){channel_unit_window_destroy(a);return;}
    if(d->apply){d->apply=false;channel_unit_flush(a);channel_unit_apply(a);}channel_unit_store(a);
}
#include "channel_unit_task_ui.inc"
