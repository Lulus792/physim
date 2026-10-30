/* Selected Explorer A. UI owns navigation and transferred snapshots; all file
 * operations are submitted to file_browser_worker, never executed in paint. */
typedef struct {ps_text_wrap *layout;struct nk_user_font font;float width;bool scroll;} browser_preview_layout;
typedef struct {float *columns,width;size_t count;struct nk_user_font font;} browser_api_table_layout;
typedef struct browser_dialog {
    SDL_Window *window;ps_graphics *graphics;struct nk_context *ui,*source_ui;
    SDL_WindowID source_id;char capture[4096];bool renew_window,compact;
    ps_browser_worker *worker;ps_browser_listing listing;ps_browser_result preview;
    ps_browser_operation operation;ps_browser_mode filter;int mode,selected,focus,focused_row;
    ps_dialog_ticket ticket;
    bool visible,embedded,failed,grid,hidden,create,path_edit,path_dirty,keyboard,reveal,initial,popup_close,cache_dirty,list_reset,preview_reset,api_expanded;
    int return_tab;
    char location[4096],path_input[4096],query[256],new_name[4096],message[512];
    char reference_root[4096];
    char history[32][4096],places[3][4096],place_names[3][256],opener[96],return_label[128];
    unsigned history_count,return_frames,grid_columns;ps_browser_sort sort;
    size_t filtered[PS_BROWSER_ENTRY_LIMIT],count;float tops[PS_BROWSER_ENTRY_LIMIT+1],cache_width,cache_scale,content_height;
    struct nk_user_font fonts[15];documentation_spaced_font caption_fonts[2];struct nk_rect bounds,list_bounds,preview_bounds,controls[32],selected_bounds;
    struct {int index;struct nk_rect bounds;} rows[64];unsigned row_count;
    struct nk_text_edit edits[5]; /* Location, search, new name, source, API signature. */
    browser_preview_layout text_layouts[2];unsigned text_layout_next;
    browser_preview_layout api_layouts[2];unsigned api_layout_next;
    browser_api_table_layout api_tables[2];unsigned api_table_next;
    struct nk_rect preview_text_bounds,place_ink[3];size_t place_bytes[3];float preview_text_height;char empty_preview[2];
    void (*draw_probe)(struct browser_dialog *); /* Optional native command observation in layout fixtures. */
} browser_dialog;
static void browser_preview_clear(browser_dialog *d) {
    d->edits[3].visual=NULL;
    d->edits[4].visual=NULL;d->api_expanded=false;
    for(unsigned k=0;k<2;k++){ps_text_wrap_destroy(d->text_layouts[k].layout);memset(&d->text_layouts[k],0,sizeof d->text_layouts[k]);}
    for(unsigned k=0;k<2;k++){ps_text_wrap_destroy(d->api_layouts[k].layout);memset(&d->api_layouts[k],0,sizeof d->api_layouts[k]);}
    for(unsigned k=0;k<2;k++){free(d->api_tables[k].columns);memset(&d->api_tables[k],0,sizeof d->api_tables[k]);}
    ps_browser_result_destroy(&d->preview);memset(&d->edits[3],0,2*sizeof d->edits[3]);d->preview_reset=true;
}
static const char *browser_names[]={"Dateiauswahl schließen","Zurück","Übergeordneter Ordner","Ort","Suche im Ordner","Sortieren","Listenansicht","Rasteransicht","Projekte","Geöffnetes Projekt","Messläufe","Dateiliste","Dateivorschau","Ordner öffnen","Versteckte Einträge","Neuer Ordner","Ordnername","Ordner anlegen","Ordneranlage abbrechen","Dateiauswahl abbrechen","Auswahl bestätigen"};
static const char *browser_kind_name(ps_browser_kind kind) {
    static const char *names[]={"Ordner","C","Physim","Messlauf","Grafik","Projektdatei","Textdatei","Datei"};
    return kind<=PS_BROWSER_OTHER?names[kind]:"Datei";
}
static bool browser_active(const app *a){for(unsigned i=1;i<PS_DIALOG_REQUEST_ROUTES;i++)if(a->browser_popups[i] && a->browser_popups[i]->visible)return true;return false;}
static bool browser_raise(app *a,int mode) {
    if(mode<1 || mode>=PS_DIALOG_REQUEST_ROUTES)return false;
    browser_dialog *d=a->browser_popups[mode];
    if(!d || !d->visible || !ps_dialog_request_find(&a->dialog_requests,d->ticket))return false;
    a->browser=d;if(d->window){SDL_ShowWindow(d->window);SDL_RaiseWindow(d->window);}return true;
}
/* Native lifetime ends only at a frame boundary, never from a paint callback.
 * The app's immutable fonts outlive every popup context. */
static void browser_window_destroy(app *a,browser_dialog *d) {
    if(!d || !d->window)return;
    if(d->graphics){ps_graphics_make_current(d->graphics);if(d->ui)nk_sdl_shutdown(d->ui);ps_graphics_destroy(d->graphics);}
    SDL_DestroyWindow(d->window);d->window=NULL;d->graphics=NULL;d->ui=NULL;
    ps_graphics_make_current(a->graphics);
}
static bool browser_window_open(app *a,browser_dialog *d) {
    SDL_Window *source=SDL_GetWindowFromID(d->source_id);if(!source)return false;
    int w,h,x,y;SDL_GetWindowSize(source,&w,&h);SDL_GetWindowPosition(source,&x,&y);float f=a->preferences.ui_size/16.f;
    if(!ps_graphics_make_current(a->graphics) || !SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT,1))goto fail;
    d->window=SDL_CreateWindow(d->mode==PS_DIALOG_MANAGER_PARENT?"Physim | Speicherort auswählen":"Physim | Dateiauswahl",(int)fminf(1180*f,fmaxf(320,w-60)),
        (int)fminf(1000*f,fmaxf(320,h-60)),SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE|SDL_WINDOW_HIGH_PIXEL_DENSITY|SDL_WINDOW_HIDDEN);
    if(d->window)d->graphics=ps_graphics_create(d->window);
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT,0);if(!d->graphics)goto fail;
    SDL_GL_SetSwapInterval(0);d->ui=nk_sdl_init(d->window,d->graphics);if(!d->ui)goto fail;
    nk_sdl_borrow_fonts(d->ui,a->ui);nk_input_begin(d->ui);nk_input_end(d->ui);nk_sdl_accessibility_keyboard_begin(d->ui);
    SDL_SetWindowMinimumSize(d->window,320,320);SDL_SetWindowPosition(d->window,x+30,y+30);
    SDL_ShowWindow(d->window);SDL_RaiseWindow(d->window);ps_graphics_make_current(a->graphics);return true;
fail:
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT,0);browser_window_destroy(a,d);return false;
}
static bool browser_workspace_visible(const app *a){return a->browser_workspace && a->browser_workspace->visible && a->tab==9 &&
    !a->workspace_manager && !a->layout_manager && !a->channel_unit_manager && !a->run_details_manager && !a->recovery && !a->document_pending;}
static bool browser_busy(const browser_dialog *d){return ps_browser_worker_read(d->worker).loading;}
static bool browser_mutating(const browser_dialog *d){return browser_busy(d) && (d->operation==PS_BROWSER_CONFIRM || d->operation==PS_BROWSER_MAKE_DIRECTORY);}
static bool browser_path_field(const browser_dialog *d){return d->path_edit || d->failed || (browser_busy(d) && d->operation==PS_BROWSER_READ_DIRECTORY);}
static bool browser_request(browser_dialog *d,ps_browser_operation operation,const char *path,const char *name) {
    ps_browser_request request={.operation=operation,.mode=d->filter};
    if(SDL_strlcpy(request.path,path,sizeof request.path)>=sizeof request.path ||
       (name && SDL_strlcpy(request.name,name,sizeof request.name)>=sizeof request.name))return false;
    if(operation==PS_BROWSER_READ_TEXT && d->selected>=0 && d->listing.entries[d->selected].kind==PS_BROWSER_C_SOURCE)
        SDL_strlcpy(request.reference_root,d->reference_root,sizeof request.reference_root);
    ps_result result=ps_browser_worker_request(d->worker,&request);
    if(result!=PS_OK){snprintf(d->message,sizeof d->message,"Anfrage fehlgeschlagen: %s",ps_result_string(result));return false;}
    d->operation=operation;d->message[0]=0;return true;
}
static bool browser_navigate(browser_dialog *d,const char *path,bool remember) {
    if(browser_mutating(d))return false;
    char next[4096];
    /* Preserve POSIX link/.. meaning on typed paths; the worker canonicalizes
     * successful directories. Copy before writing an alias of location. */
    if(SDL_strlcpy(next,path,sizeof next)>=sizeof next)return false;
    if(!browser_request(d,PS_BROWSER_READ_DIRECTORY,next,NULL))return false;
    if(remember && *d->listing.directory && !d->failed && strcmp(next,d->listing.directory) &&
       (!d->history_count || strcmp(d->history[d->history_count-1],d->listing.directory))) {
        if(d->history_count==32){memmove(d->history,d->history+1,31*sizeof d->history[0]);d->history_count--;}
        snprintf(d->history[d->history_count++],4096,"%s",d->listing.directory);
    }
    snprintf(d->location,sizeof d->location,"%s",next);snprintf(d->path_input,sizeof d->path_input,"%s",next);
    memset(&d->edits[0],0,sizeof d->edits[0]);memset(&d->edits[1],0,sizeof d->edits[1]);
    d->selected=d->focused_row=-1;d->count=0;d->query[0]=0;d->failed=false;d->cache_dirty=d->list_reset=true;d->path_edit=d->path_dirty=d->create=false;
    browser_preview_clear(d);return true;
}
static void browser_back(browser_dialog *d) {
    if(d->history_count && !browser_mutating(d)){char path[4096];strcpy(path,d->history[d->history_count-1]);
        if(browser_navigate(d,path,false))d->history_count--;}
}
static void browser_up(browser_dialog *d) {
    char parent[4096];if(ps_browser_parent(d->location,parent)==PS_OK)browser_navigate(d,parent,true);
}
static void browser_close(app *a,browser_dialog *d,const char *path) {
    if(!d || !d->visible)return;
    char chosen[4096];if(path)snprintf(chosen,sizeof chosen,"%s",path);
    SDL_Window *source=SDL_GetWindowFromID(d->source_id);
    bool return_focus=!d->embedded && d->window && SDL_GetKeyboardFocus()==d->window && source &&
        !(SDL_GetWindowFlags(source)&(SDL_WINDOW_HIDDEN|SDL_WINDOW_MINIMIZED));
    d->visible=false;d->return_frames=d->embedded?(!path?3:0):(return_focus?3:0);ps_browser_worker_cancel(d->worker);
    if(return_focus)SDL_RaiseWindow(source);
    if(d->embedded){a->tab=d->return_tab;if(path)browser_complete(a,d->ticket,chosen);else ps_dialog_request_cancel(&a->dialog_requests,d->ticket);}
    else browser_complete(a,d->ticket,path?chosen:NULL);
    a->dialog_pending=ps_dialog_requests_pending(&a->dialog_requests);
}
static void browser_context_destroy(app *a,browser_dialog *d) {
    if(!d)return;
    browser_window_destroy(a,d);ps_browser_worker_destroy(d->worker);ps_browser_listing_destroy(&d->listing);
    browser_preview_clear(d);free(d);
}
static void browser_cancel_project_parent(app *a) {
    browser_dialog *d=a->browser_popups[PS_DIALOG_MANAGER_PARENT];if(!d || !d->visible)return;
    browser_close(a,d,NULL);d->return_frames=0;
}
static void browser_cancel_project_context(app *a) {
    for(unsigned i=1;i<PS_DIALOG_REQUEST_ROUTES;i++){browser_dialog *d=a->browser_popups[i];
        if(d && d->visible && i!=PS_DIALOG_MANAGER_PARENT){browser_close(a,d,NULL);d->return_frames=0;}}
}
static void browser_destroy(app *a) {
    for(unsigned i=1;i<PS_DIALOG_REQUEST_ROUTES;i++){browser_context_destroy(a,a->browser_popups[i]);a->browser_popups[i]=NULL;}
    browser_context_destroy(a,a->browser_workspace);a->browser=a->browser_workspace=NULL;
}
static void browser_places(app *a,browser_dialog *d) {
    snprintf(d->places[0],4096,"%s",a->manager_parent);snprintf(d->place_names[0],256,"Projekte");
    snprintf(d->places[1],4096,"%s",a->workspace_open?a->workspace:"");
    const char *leaf=strrchr(a->workspace,'/');snprintf(d->place_names[1],256,"%s",a->workspace_open?(leaf?leaf+1:a->workspace):"Geöffnetes Projekt");
    d->places[2][0]=0;if(a->loaded)join(d->places[2],4096,a->project,"runs");snprintf(d->place_names[2],256,"Messläufe");
}
static bool browser_context_open(app *a,browser_dialog **owner,int mode,bool embedded) {
    browser_dialog *d=*owner;
    if(!d){d=calloc(1,sizeof *d);if(!d)return false;d->worker=ps_browser_worker_create();if(!d->worker){free(d);return false;}*owner=d;}
    SDL_Window *source=mode==PS_DIALOG_MANAGER_PARENT?a->manager_window:a->window;
    d->source_ui=mode==PS_DIALOG_MANAGER_PARENT?a->manager_ui:a->ui;
    if(!source || !d->source_ui)return false;
    d->source_id=SDL_GetWindowID(source);if(embedded)d->ui=a->ui;
    ps_browser_worker_cancel(d->worker);ps_browser_listing_destroy(&d->listing);browser_preview_clear(d);
    d->mode=mode;d->embedded=embedded;d->filter=mode==PS_DIALOG_IMPORT_RUN?PS_BROWSER_RUNS:
        mode==PS_DIALOG_ADD_FILE || mode==PS_DIALOG_BROWSE?PS_BROWSER_FILES:PS_BROWSER_FOLDERS;
    SDL_strlcpy(d->reference_root,a->root,sizeof d->reference_root);
    d->visible=true;d->failed=d->grid=d->hidden=d->create=d->path_edit=false;
    d->query[0]=d->new_name[0]=d->message[0]=0;d->sort=PS_BROWSER_SORT_NAME;d->history_count=0;d->return_frames=0;
    memset(d->edits,0,sizeof d->edits);
    d->keyboard=d->initial=d->reveal=true;d->focus=0;d->cache_dirty=true;d->count=0;
    const char *opener=d->source_ui->current?d->source_ui->current->name_string:d->source_ui->active?d->source_ui->active->name_string:"workspace";
    snprintf(d->opener,sizeof d->opener,"%s",opener);
    snprintf(d->return_label,sizeof d->return_label,"%s",mode==PS_DIALOG_MANAGER_PARENT?"Speicherort auswählen":"Datei");
    browser_places(a,d);
    if(d->ui){nk_sdl_accessibility_keyboard_begin(d->ui);
        struct nk_window *window=nk_window_find(d->ui,"Dateiauswahl");if(window)window->scrollbar=(struct nk_scroll){0,0};}
    const char *location=mode==PS_DIALOG_MANAGER_PARENT?a->manager_parent:a->workspace_open?a->workspace:a->manager_parent;
    if(!browser_navigate(d,*location?location:a->root,false)){d->visible=false;return false;}return true;
}
static bool browser_open(app *a,int mode,ps_dialog_ticket ticket){
    if(mode<1 || mode>=PS_DIALOG_REQUEST_ROUTES)return false;
    browser_dialog *d=a->browser_popups[mode];if(d){d->renew_window=true;ps_dialog_request_cancel(&a->dialog_requests,d->ticket);}
    if(mode==PS_DIALOG_BROWSE && a->browser_workspace && a->browser_workspace->operation==PS_BROWSER_CONFIRM){
        ps_browser_worker_cancel(a->browser_workspace->worker);a->browser_workspace->operation=PS_BROWSER_READ_DIRECTORY;}
    if(!browser_context_open(a,&a->browser_popups[mode],mode,false))return false;
    a->browser=a->browser_popups[mode];
    a->browser->ticket=ticket;return true;
}
static void browser_select(browser_dialog *d,int index);
static bool browser_workspace_open(app *a) {
    browser_dialog *d=a->browser_workspace;int previous=a->tab;
    if(!d){if(!browser_context_open(a,&a->browser_workspace,PS_DIALOG_BROWSE,true))return false;d=a->browser_workspace;}
    else {d->visible=true;d->keyboard=d->reveal=d->initial=true;nk_sdl_accessibility_keyboard_begin(d->ui);
        if(!d->failed && d->operation==PS_BROWSER_READ_DIRECTORY && strcmp(d->location,d->listing.directory))browser_navigate(d,d->location,false);
        else if(d->selected>=0 && !d->preview.text && d->operation==PS_BROWSER_READ_TEXT){int selected=d->selected;d->selected=-1;browser_select(d,selected);}}
    if(previous!=9)d->return_tab=previous;
    browser_places(a,d);a->tab=9;d->return_frames=0;if(!d->focus)d->focus=3;return true;
}
static void browser_context_tick(app *a,browser_dialog *d) {
    if(!d)return;ps_browser_result result={0};
    if(!ps_browser_worker_poll(d->worker,&result))return;
    if(!d->visible){ps_browser_result_destroy(&result);return;}
    if(result.operation==PS_BROWSER_READ_DIRECTORY) {
        if(result.status==PS_OK || result.status==PS_LIMIT || result.status==PS_CORRUPT) {
            ps_browser_listing_destroy(&d->listing);d->listing=result.listing;memset(&result.listing,0,sizeof result.listing);
            snprintf(d->location,4096,"%s",d->listing.directory);
            if(!d->path_edit && !d->path_dirty){snprintf(d->path_input,4096,"%s",d->location);memset(&d->edits[0],0,sizeof d->edits[0]);}
            d->cache_dirty=true;d->failed=false;
            d->list_reset=true;
            if(result.status!=PS_OK)snprintf(d->message,sizeof d->message,"Begrenzte Liste: mindestens %zu Einträge fehlen oder sind nicht lesbar.",d->listing.omitted);
        } else {d->failed=true;snprintf(d->message,sizeof d->message,"Ordner konnte nicht geöffnet werden: %s",ps_result_string(result.status));}
    } else if(result.operation==PS_BROWSER_READ_TEXT) {
        browser_preview_clear(d);d->preview=result;memset(&result,0,sizeof result);
    } else if(result.operation==PS_BROWSER_CONFIRM) {
        if(!ps_dialog_request_find(&a->dialog_requests,d->ticket) || (d->embedded && !browser_workspace_visible(a))){ps_dialog_request_cancel(&a->dialog_requests,d->ticket);a->dialog_pending=ps_dialog_requests_pending(&a->dialog_requests);d->operation=PS_BROWSER_READ_DIRECTORY;}
        else if(result.status==PS_OK && result.selectable)browser_close(a,d,result.path);
        else snprintf(d->message,sizeof d->message,"Die Auswahl ist nicht mehr verfügbar oder passt nicht zum benötigten Dateityp.");
        if(d->embedded && d->visible){ps_dialog_request_cancel(&a->dialog_requests,d->ticket);a->dialog_pending=ps_dialog_requests_pending(&a->dialog_requests);}
    } else if(result.operation==PS_BROWSER_MAKE_DIRECTORY) {
        if(result.status==PS_OK){d->create=false;d->new_name[0]=0;browser_navigate(d,d->location,false);}
        else snprintf(d->message,sizeof d->message,"%s",result.created==PS_BROWSER_COLLISION?"Dieser Name ist bereits vergeben.":result.created==PS_BROWSER_NAME_INVALID?"Einen gültigen einzelnen Ordnernamen eingeben.":"Der Ordner konnte nicht angelegt werden. Prüfe den Speicherort und die Berechtigung.");
    }
    ps_browser_result_destroy(&result);
}
static void browser_tick(app *a){
    for(unsigned i=1;i<PS_DIALOG_REQUEST_ROUTES;i++){
        browser_dialog *d=a->browser_popups[i];if(!d)continue;
        if(d->renew_window){browser_window_destroy(a,d);d->renew_window=false;}
        if(d->visible && !ps_dialog_request_find(&a->dialog_requests,d->ticket))browser_close(a,d,NULL);
        browser_context_tick(a,d);
        if(!d->visible)browser_window_destroy(a,d);
        else if(!d->window && !browser_window_open(a,d)){browser_close(a,d,NULL);status(a,"Dateiauswahlfenster konnte nicht geöffnet werden.");}
        if(d->return_frames){SDL_Window *source=SDL_GetWindowFromID(d->source_id);
            if(!source || SDL_GetKeyboardFocus()!=source || SDL_GetWindowFlags(source)&(SDL_WINDOW_HIDDEN|SDL_WINDOW_MINIMIZED))d->return_frames=0;
            else {nk_window_set_focus(d->source_ui,d->opener);if(nk_sdl_accessibility_request_focus(d->source_ui,NULL,d->return_label) || !--d->return_frames)d->return_frames=0;}}
    }
    browser_context_tick(a,a->browser_workspace);
}
static void browser_select(browser_dialog *d,int index) {
    if(index<0 || (size_t)index>=d->listing.count || browser_mutating(d) || (browser_busy(d) && d->operation==PS_BROWSER_READ_DIRECTORY))return;
    if(d->selected==index)return;
    d->selected=d->focused_row=index;browser_preview_clear(d);
    ps_browser_entry *entry=&d->listing.entries[index];
    if(entry->error==PS_OK && (entry->kind==PS_BROWSER_C_SOURCE || entry->kind==PS_BROWSER_PHYS_SOURCE || entry->kind==PS_BROWSER_PROJECT || entry->kind==PS_BROWSER_TEXT))
        browser_request(d,PS_BROWSER_READ_TEXT,entry->path,NULL);
    else {ps_browser_worker_cancel(d->worker);d->message[0]=0;}
}
static void browser_confirm(app *a,browser_dialog *d) {
    if(d->failed || d->path_dirty || browser_mutating(d) || (browser_busy(d) && d->operation==PS_BROWSER_READ_DIRECTORY))return;
    const char *path=d->selected>=0?d->listing.entries[d->selected].path:d->location;
    if(d->filter!=PS_BROWSER_FOLDERS && d->selected<0)return;
    if(d->filter!=PS_BROWSER_FOLDERS && d->listing.entries[d->selected].kind==PS_BROWSER_DIRECTORY){browser_navigate(d,path,true);return;}
    if(d->embedded){browser_dialog *popup=a->browser_popups[d->mode];if(popup){browser_close(a,popup,NULL);popup->return_frames=0;}
        ps_dialog_request *request=NULL;
        if(ps_dialog_request_begin(&a->dialog_requests,(unsigned)d->mode,&request)!=PS_OK){snprintf(d->message,sizeof d->message,"Auswahl konnte nicht vorbereitet werden.");return;}
        d->ticket=ps_dialog_request_ticket(request);}
    if(!browser_request(d,PS_BROWSER_CONFIRM,path,NULL) && d->embedded)ps_dialog_request_cancel(&a->dialog_requests,d->ticket);
    a->dialog_pending=ps_dialog_requests_pending(&a->dialog_requests);
}
static void browser_focus_set(app *a,browser_dialog *d,int id);
static void browser_action(app *a,browser_dialog *d,int id) {

    if(id==0 || id==19)browser_close(a,d,NULL);
    else if(id==1)browser_back(d);else if(id==2)browser_up(d);
    else if(id==3){if(browser_path_field(d))browser_navigate(d,d->path_input,true);else browser_navigate(d,d->location,false);}
    else if(id==5){d->sort=d->sort==PS_BROWSER_SORT_NAME?PS_BROWSER_SORT_TYPE:PS_BROWSER_SORT_NAME;d->cache_dirty=true;}
    else if(id==6 || id==7){d->grid=id==7;d->cache_dirty=true;}
    else if(id>=8 && id<=10 && *d->places[id-8])browser_navigate(d,d->places[id-8],true);
    else if(id==11 || id==20)browser_confirm(a,d);
    else if(id==13 && d->selected>=0)browser_navigate(d,d->listing.entries[d->selected].path,true);
    else if(id==14){d->hidden=!d->hidden;d->cache_dirty=true;}
    else if(id==15 && !d->failed && !browser_busy(d)){d->create=true;d->new_name[0]=0;memset(&d->edits[2],0,sizeof d->edits[2]);browser_focus_set(a,d,16);}
    else if(id==17){if(!browser_mutating(d))browser_request(d,PS_BROWSER_MAKE_DIRECTORY,d->location,d->new_name);}
    else if(id==18){d->create=false;browser_focus_set(a,d,15);}
    else if(id==21){d->api_expanded=!d->api_expanded;d->reveal=true;}
    else if(id==22 && d->preview.api.status==PS_OK && d->preview.api.symbol){
        char relative[256];snprintf(relative,sizeof relative,"docs/reference/%s.md",d->preview.api.symbol->module);
        for(int topic=0;topic<DOCUMENTATION_TOPIC_COUNT;topic++)if(!strcmp(documentation_topics[topic].path,relative)){
            open_documentation(a,topic);if(a->documentation && a->doc_topic==topic)a->doc_jump_block=(int)d->preview.api.first;break;}
    }
}
static int browser_order(browser_dialog *d,int ids[24]) {
    if(browser_mutating(d)){if(d->embedded){ids[0]=d->focus;return 1;}ids[0]=0;ids[1]=19;return 2;}
    int n=0;if(!d->embedded)ids[n++]=0;if(d->history_count)ids[n++]=1;ids[n++]=2;ids[n++]=3;ids[n++]=4;ids[n++]=5;ids[n++]=6;ids[n++]=7;
    for(int i=0;i<3;i++)if(*d->places[i])ids[n++]=8+i;
    if(d->count && !d->failed)ids[n++]=11;
    if(d->preview.text)ids[n++]=12;
    if(d->preview.api.symbol){ids[n++]=21;if(d->api_expanded && d->preview.api.status==PS_OK){
        if(d->preview.api.preview.signature!=SIZE_MAX)ids[n++]=23;ids[n++]=22;}}
    if(d->selected>=0 && d->listing.entries[d->selected].kind==PS_BROWSER_DIRECTORY)ids[n++]=13;
    ids[n++]=14;if(!d->failed && !browser_busy(d))ids[n++]=15;if(d->create){ids[n++]=16;ids[n++]=17;ids[n++]=18;}if(!d->embedded)ids[n++]=19;
    if(!d->failed && !d->path_dirty && !(browser_busy(d) && d->operation==PS_BROWSER_READ_DIRECTORY) && (d->filter==PS_BROWSER_FOLDERS || d->selected>=0))ids[n++]=20;return n;
}
static void browser_focus_set(app *a,browser_dialog *d,int id) {
    d->focus=id;d->keyboard=d->initial=d->reveal=true;nk_sdl_accessibility_keyboard_begin(d->ui);
}
static void browser_move(app *a,browser_dialog *d,int direction,bool edge) {
    if(!d->count || d->failed || (browser_busy(d) && d->operation==PS_BROWSER_READ_DIRECTORY))return;size_t at=0;
    for(size_t k=0;k<d->count;k++)if((int)d->filtered[k]==d->focused_row)at=k;
    if(edge)at=direction<0?0:d->count-1;
    else if(direction<0)at=at?at-1:0;else if(at+1<d->count)at++;
    browser_select(d,(int)d->filtered[at]);browser_focus_set(a,d,11);
}
static void browser_flush_field(app *a,browser_dialog *d) {
    if(browser_mutating(d) || (d->focus!=3 && d->focus!=4 && d->focus!=16))return;
    char *text=d->focus==3?d->path_input:d->focus==4?d->query:d->new_name,before[4096];strcpy(before,text);
    ps_ui_flush_edit_buffer(d->ui,text,d->focus==4?sizeof d->query:4096,&d->edits[d->focus==3?0:d->focus==4?1:2],d->controls[d->focus]);
    if(d->focus==4)d->cache_dirty=d->list_reset=true;
    if(d->focus==3 && strcmp(before,text))d->path_dirty=d->path_edit=true;
}
static bool browser_event(app *a,const SDL_Event *event,SDL_Window *window) {
    if(!window)return false;
    browser_dialog *d=NULL;
    for(unsigned i=1;i<PS_DIALOG_REQUEST_ROUTES;i++)if(a->browser_popups[i] && a->browser_popups[i]->window==window){d=a->browser_popups[i];break;}
    if(d){if(event->type==SDL_EVENT_WINDOW_CLOSE_REQUESTED){browser_close(a,d,NULL);return true;}if(!d->visible)return true;}
    else if(window==a->window && browser_workspace_visible(a))d=a->browser_workspace;
    if(!d || !d->ui)return false;
    if(event->type==SDL_EVENT_WINDOW_RESIZED && d->keyboard)d->reveal=true;
    bool input=event->type==SDL_EVENT_KEY_DOWN || event->type==SDL_EVENT_KEY_UP || event->type==SDL_EVENT_TEXT_INPUT ||
        event->type==SDL_EVENT_MOUSE_BUTTON_DOWN || event->type==SDL_EVENT_MOUSE_BUTTON_UP || event->type==SDL_EVENT_MOUSE_MOTION || event->type==SDL_EVENT_MOUSE_WHEEL;
    if(!input){if(d->embedded)return false;SDL_Event copy=*event;nk_sdl_handle_event(d->ui,&copy);return true;}
    if(d->embedded){
        if(a->toolbar_menu || a->toolbar_keyboard)return false;
        if(event->type>=SDL_EVENT_MOUSE_MOTION && event->type<=SDL_EVENT_MOUSE_WHEEL){
            float x=event->type==SDL_EVENT_MOUSE_MOTION?event->motion.x:event->type==SDL_EVENT_MOUSE_WHEEL?event->wheel.mouse_x:event->button.x;
            float y=event->type==SDL_EVENT_MOUSE_MOTION?event->motion.y:event->type==SDL_EVENT_MOUSE_WHEEL?event->wheel.mouse_y:event->button.y;
            if((x<d->bounds.x || x>d->bounds.x+d->bounds.w || y<d->bounds.y || y>d->bounds.y+d->bounds.h) &&
               !((d->edits[3].active || d->edits[4].active) && event->type==SDL_EVENT_MOUSE_MOTION)){
                if(event->type==SDL_EVENT_MOUSE_BUTTON_DOWN)d->keyboard=d->initial=false;return false;}
        }
        bool local_shortcut=(event->type==SDL_EVENT_KEY_DOWN || event->type==SDL_EVENT_KEY_UP) &&
            (event->key.mod&PS_UI_COMMAND_MOD && (event->key.key==SDLK_F || event->key.key==SDLK_L));
        if((event->type==SDL_EVENT_KEY_DOWN || event->type==SDL_EVENT_KEY_UP || event->type==SDL_EVENT_TEXT_INPUT) &&
           !local_shortcut && !d->keyboard && d->ui->active!=nk_window_find(d->ui,"Dateien"))return false;
        if(event->type==SDL_EVENT_KEY_DOWN || event->type==SDL_EVENT_KEY_UP){SDL_Keycode key=event->key.key;
            if(key==SDLK_F10 || key==SDLK_F5 || (event->key.mod&SDL_KMOD_ALT && key!=SDLK_LEFT && key!=SDLK_UP) ||
               (event->key.mod&PS_UI_COMMAND_MOD && (key==SDLK_1 || key==SDLK_2 || key==SDLK_3 || key==SDLK_4 || key==SDLK_N || key==SDLK_O || key==SDLK_S)))return false;
        }
    }
    if(event->type==SDL_EVENT_KEY_DOWN) {
        const SDL_KeyboardEvent *key=&event->key;SDL_Keycode k=key->key;
        if(k==SDLK_F1 && !key->repeat){open_documentation(a,a->documentation?a->doc_topic:1);return true;}
        if(k==SDLK_ESCAPE){struct nk_window *w=nk_window_find(d->ui,d->embedded?"Dateien":"Dateiauswahl");
            if(w && w->popup.active)d->popup_close=true;
            else if(browser_mutating(d))browser_close(a,d,NULL);
            else if(d->create)browser_action(a,d,18);
            else if(d->path_edit){strcpy(d->path_input,d->location);d->path_dirty=d->path_edit=false;memset(&d->edits[0],0,sizeof d->edits[0]);browser_focus_set(a,d,3);}
            else browser_close(a,d,NULL);return true;}
        if(browser_mutating(d) && k!=SDLK_TAB){if((k==SDLK_RETURN || k==SDLK_KP_ENTER || k==SDLK_SPACE) && (d->focus==0 || d->focus==19))browser_close(a,d,NULL);return true;}
        if((key->mod&PS_UI_COMMAND_MOD) && (k==SDLK_L || k==SDLK_F)){if(k==SDLK_L)d->path_edit=true;browser_focus_set(a,d,k==SDLK_L?3:4);return true;}
        if((key->mod&SDL_KMOD_ALT) && (k==SDLK_LEFT || k==SDLK_UP)){if(k==SDLK_LEFT)browser_back(d);else browser_up(d);return true;}
        if(k==SDLK_TAB) {
            browser_flush_field(a,d);
            int ids[24],n=browser_order(d,ids),at=-1,direction=key->mod&SDL_KMOD_SHIFT?-1:1;
            for(int i=0;i<n;i++)if(ids[i]==d->focus)at=i;
            if(at<0)at=direction>0?-1:0;
            browser_focus_set(a,d,ids[(at+direction+n)%n]);return true;
        }
        bool field=d->focus==4 || d->focus==16 || (d->focus==3 && browser_path_field(d)) || d->focus==12 || d->focus==23;
        if(!field && (k==SDLK_UP || k==SDLK_DOWN || k==SDLK_LEFT || k==SDLK_RIGHT || k==SDLK_HOME || k==SDLK_END) && d->focus==11){browser_move(a,d,k==SDLK_UP || k==SDLK_LEFT || k==SDLK_HOME?-1:1,k==SDLK_HOME || k==SDLK_END);return true;}
        if((k==SDLK_RETURN || k==SDLK_KP_ENTER || (k==SDLK_SPACE && !field)) && !key->repeat) {
            browser_flush_field(a,d);
            if(d->focus==4){d->cache_dirty=true;browser_focus_set(a,d,11);}
            else if(d->focus==16)browser_action(a,d,17);else if(d->focus!=12 && d->focus!=23)browser_action(a,d,d->focus);
            return true;
        }
    }
    if(event->type==SDL_EVENT_MOUSE_BUTTON_DOWN){d->keyboard=false;d->return_frames=0;}
    if(!nk_sdl_accessibility_event(d->ui,event)){SDL_Event input=*event;nk_sdl_handle_event(d->ui,&input);}return true;
}
static struct nk_rect browser_slot(browser_dialog *d,float x,float y,float w,float h) {
    nk_layout_space_push(d->ui,nk_rect(x,y,w,h));struct nk_rect r;nk_widget(&r,d->ui);return r;
}
static float browser_text_height(browser_dialog *d,const char *text,unsigned role,float width,float pitch) {
    size_t n=strlen(text),at=0;unsigned lines=0;
    while(at<n){int used=ps_ui_text_line(&d->fonts[role],text+at,(int)(n-at),fmaxf(1,width),false);at+=used>0?(size_t)used:1;lines++;}
    return fmaxf(pitch,lines*pitch);
}
static float browser_text(app *a,browser_dialog *d,float x,float y,float w,const char *text,unsigned role,float pitch,struct nk_color color,bool semantic) {
    const struct nk_user_font *font=&d->fonts[role];int n=(int)strlen(text),at=0;
    float h=browser_text_height(d,text,role,w,pitch);struct nk_rect r=browser_slot(d,x,y,w,h);
    if(semantic)ps_ui_accessibility_label(d->ui,r,text);
    float top=r.y+(pitch-font->height)/2;
    while(at<n){int used=ps_ui_text_line(font,text+at,n-at,fmaxf(1,w),false);if(used<=0)used=1;
        nk_draw_text(nk_window_get_canvas(d->ui),nk_rect(r.x,top,w,font->height),text+at,used,font,nk_rgba(0,0,0,0),color);at+=used;top+=pitch;}
    return h;
}
static void browser_centered_text(browser_dialog *d,float x,float y,float width,const char *text,unsigned role,float pitch,struct nk_color color) {
    const struct nk_user_font *font=&d->fonts[role];size_t at=0,n=strlen(text);
    struct nk_rect r=browser_slot(d,x,y,width,browser_text_height(d,text,role,width,pitch));
    while(at<n){int used=ps_ui_text_line(font,text+at,(int)(n-at),fmaxf(1,width),false);if(used<=0)used=1;
        float advance=font->width(font->userdata,font->height,text+at,used);
        nk_draw_text(nk_window_get_canvas(d->ui),nk_rect(r.x+(width-advance)/2,r.y+(pitch-font->height)/2,advance,font->height),text+at,used,font,nk_rgba(0,0,0,0),color);
        at+=used;r.y+=pitch;
    }
}
static float browser_grid_padding(const browser_dialog *d,float f){return (d->compact?8:12)*f;}
static float browser_grid_gap(const browser_dialog *d,float f){return (d->compact?6:9)*f;}
static float browser_grid_width(const browser_dialog *d,float width,float f){return (width-2*browser_grid_padding(d,f)-(d->grid_columns-1)*browser_grid_gap(d,f))/d->grid_columns;}
static float browser_list_start(const browser_dialog *d,float f){return (d->count && !d->compact?32.756f*f:0)+(d->grid?browser_grid_padding(d,f):0);}
static float browser_list_extent(const browser_dialog *d,float f){return browser_list_start(d,f)+d->tops[d->count]+(d->grid?browser_grid_padding(d,f)-(d->count?browser_grid_gap(d,f):0):0);}
static void browser_focus_draw(app *a,browser_dialog *d,int id,struct nk_rect r,const char *name) {
    if(id<32)d->controls[id]=r;
    bool native=nk_sdl_accessibility_is_focus_target(d->ui,NULL,name);
    if(native && !d->keyboard){d->focus=id;d->focused_row=id==11?d->focused_row:-1;}
    if(ps_ui_mouse_pressed_in_rect(&d->ui->input,NK_BUTTON_LEFT,r)){d->focus=id;d->keyboard=false;}
    bool custom=d->keyboard && d->focus==id;
    if(custom && nk_sdl_accessibility_keyboard_target(d->ui,r))d->initial=false;
    float f=a->preferences.ui_size/16.f;
    if(custom || native)nk_stroke_rect(nk_window_get_canvas(d->ui),r,7*f,2*f,UI_COLOR(accent));
}
static void browser_icon(struct nk_command_buffer *canvas,int icon,struct nk_rect r,struct nk_color color) {
    /* Exact 24-unit path coordinates from design-options/options.js. Keep
     * this component scalable; the older shared 14px toolbar icons are fixed. */
    float x=r.x,y=r.y,s=r.w/24,t=1.5f*s;
#define B_LINE(a,b,c,d) nk_stroke_line(canvas,x+(a)*s,y+(b)*s,x+(c)*s,y+(d)*s,t,color)
    if(icon==PS_ICON_FOLDER){float p[]={3,6,10,6,12,9,21,9,21,20,3,20,3,6};for(unsigned k=0;k<7;k++){p[2*k]=x+p[2*k]*s;p[2*k+1]=y+p[2*k+1]*s;}nk_stroke_polyline(canvas,p,7,t,color);}
    else if(icon==PS_ICON_FILE){float p[]={5,2,14,2,19,7,19,22,5,22,5,2};for(unsigned k=0;k<6;k++){p[2*k]=x+p[2*k]*s;p[2*k+1]=y+p[2*k+1]*s;}nk_stroke_polyline(canvas,p,6,t,color);
        B_LINE(14,2,14,8);B_LINE(14,8,19,8);B_LINE(9,13,15,13);B_LINE(9,17,15,17);}
    else if(icon==PS_ICON_CHART){B_LINE(3,3,3,21);B_LINE(3,21,21,21);B_LINE(6,15,10,9);B_LINE(10,9,15,12);B_LINE(15,12,21,4);}
    else if(icon==PS_ICON_CLOSE){B_LINE(5,5,19,19);B_LINE(5,19,19,5);}
    else if(icon==-1){B_LINE(14,5,7,12);B_LINE(7,12,14,19);}
    else if(icon==-2){B_LINE(5,14,12,7);B_LINE(12,7,19,14);}
    else if(icon==-3){nk_stroke_rect(canvas,nk_rect(x+4*s,y+3*s,16*s,18*s),0,t,color);B_LINE(8,8,16,8);B_LINE(8,12,16,12);B_LINE(8,16,13,16);}
    else if(icon==-4)for(int k=0;k<4;k++)nk_stroke_rect(canvas,nk_rect(x+(3+(k%2)*11)*s,y+(3+(k/2)*11)*s,7*s,7*s),0,t,color);
    else if(icon==-5){B_LINE(8,6,2,12);B_LINE(2,12,8,18);B_LINE(16,6,22,12);B_LINE(22,12,16,18);B_LINE(13,3,11,21);}
    else if(icon==-6){B_LINE(12,11,12,17);nk_fill_circle(canvas,nk_rect(x+12*s-t/2,y+7*s-t/2,t,t),color);nk_stroke_circle(canvas,nk_rect(x+2*s,y+2*s,20*s,20*s),t,color);}
    else ps_ui_draw_icon(canvas,(ps_ui_icon)icon,r,color);
#undef B_LINE
}
static bool browser_button(app *a,browser_dialog *d,int id,float x,float y,float w,float h,const char *label,int icon,bool enabled,bool primary,bool selected,bool border) {
    struct nk_context *ui=d->ui;float f=a->preferences.ui_size/16.f;
    struct nk_rect r=browser_slot(d,x,y,w,h);bool hover=enabled && nk_input_is_mouse_hovering_rect(&ui->input,r);
    struct nk_color color=primary?UI_COLOR(primary_text):id==15?UI_COLOR(muted):UI_COLOR(text);
    struct nk_color fill=primary?UI_COLOR(primary):selected?UI_COLOR(selection_active):hover?UI_COLOR(button_hover):border?UI_COLOR(surface):nk_rgba(0,0,0,0),edge=primary?UI_COLOR(primary):UI_COLOR(border);
    if(!enabled){fill.a=(nk_byte)(fill.a*.45f);edge.a=(nk_byte)(edge.a*.45f);color.a=(nk_byte)(color.a*.45f);}
    nk_fill_rect(nk_window_get_canvas(ui),r,7*f,fill);
    bool place=id>=8 && id<=10;float place_padding=(d->compact?6.6f:10.8f)*f;
    if(border || primary || (place && selected))nk_stroke_rect(nk_window_get_canvas(ui),r,7*f,f,edge);
    if(icon!=-99)browser_icon(nk_window_get_canvas(ui),icon,nk_rect(r.x+(label&&*label?(place?place_padding:10*f):(r.w-18*f)/2),r.y+(h-18*f)/2,18*f,18*f),color);
    if(label && *label){unsigned place_role=d->compact?4:3;const struct nk_user_font *font=&d->fonts[id==19 || (id==20 && !d->embedded)?7:id==15?12:id>=8 && id<=10?place_role:2];float tw=font->width(font->userdata,font->height,label,(int)strlen(label));
        if(place){float inset=(d->compact?28.6f:37.8f)*f,width=fmaxf(1,r.w-inset-place_padding),pitch=(d->compact?14.756f:17.36f)*f,th=browser_text_height(d,label,place_role,width,pitch),top=r.y+(r.h-th)/2;
            size_t at=0,n=strlen(label);struct nk_rect ink=nk_rect(r.x+inset,top+(pitch-font->height)/2,0,th-pitch+font->height);
            while(at<n){int used=ps_ui_text_line(font,label+at,(int)(n-at),width,false);if(used<=0)used=1;
                float drawn=font->width(font->userdata,font->height,label+at,used);ink.w=fmaxf(ink.w,drawn);
                nk_draw_text(nk_window_get_canvas(ui),nk_rect(ink.x,top+(pitch-font->height)/2,width,font->height),label+at,used,font,nk_rgba(0,0,0,0),color);at+=used;top+=pitch;}
            d->place_ink[id-8]=ink;d->place_bytes[id-8]=at;
        }else nk_draw_text(nk_window_get_canvas(ui),nk_rect(r.x+(icon!=-99?35*f:fmaxf(4*f,(r.w-tw)/2)),r.y+(h-font->height)/2,r.w-(icon!=-99?39*f:8*f),font->height),label,(int)strlen(label),font,nk_rgba(0,0,0,0),color);}
    const char *name=d->embedded && id==20?"Datei öffnen":id<21?browser_names[id]:label;nk_begin_disabled(ui,!enabled);
    bool pressed=ps_ui_button_at(ui,r,name,selected);nk_end_disabled(ui);browser_focus_draw(a,d,id,r,name);return pressed;
}
static void browser_field(app *a,browser_dialog *d,int id,char *text,int capacity,struct nk_rect slot) {
    struct nk_context *ui=d->ui;nk_layout_space_push(ui,slot);
    if(d->keyboard && d->focus==id && !browser_mutating(d))nk_edit_focus(ui,NK_EDIT_ALWAYS_INSERT_MODE);
    char before[4096];snprintf(before,sizeof before,"%s",text);nk_begin_disabled(ui,browser_mutating(d));
    struct nk_rect r=nk_widget_bounds(ui);nk_style_push_font(ui,&d->fonts[2]);
    nk_sdl_edit_named_buffer(ui,browser_names[id],NK_EDIT_FIELD,text,capacity,&d->edits[id==3?0:id==4?1:2]);nk_style_pop_font(ui);nk_end_disabled(ui);
    if(id==3 && strcmp(before,text))d->path_dirty=d->path_edit=true;
    if(!browser_mutating(d))browser_focus_draw(a,d,id,r,browser_names[id]);else d->controls[id]=r;
}
static void browser_rebuild(browser_dialog *d,float width,float f) {
    if(!d->cache_dirty && d->cache_width==width && d->cache_scale==f)return;
    char selected[4096]="";if(d->selected>=0)strcpy(selected,d->listing.entries[d->selected].path);
    ps_browser_listing_sort(&d->listing,d->sort);d->selected=d->focused_row=-1;d->count=0;
    for(size_t k=0;k<d->listing.count;k++)if(ps_browser_entry_matches(&d->listing.entries[k],d->filter,d->query,d->hidden)) {
        d->filtered[d->count++]=k;if(!strcmp(selected,d->listing.entries[k].path))d->selected=d->focused_row=(int)k;
    }
    if(d->selected<0){browser_preview_clear(d);}
    d->tops[0]=0;
    if(!d->grid)for(size_t k=0;k<d->count;k++) {
        const char *name=ps_browser_entry_name(&d->listing.entries[d->filtered[k]]);
        float text_width=width-(d->compact?51:229)*f;
        float text_h=browser_text_height(d,name,3,fmaxf(30*f,text_width),17.92f*f);
        d->tops[k+1]=d->tops[k]+fmaxf(18*f,text_h)+(d->compact?9+14.756f+26:26)*f;
    }
    if(d->grid){float y=0;for(size_t k=0;k<d->count;k+=d->grid_columns){float h=0;size_t end=k+d->grid_columns;if(end>d->count)end=d->count;
        /* CSS flex column: border, padding, icon + margin, two gaps, name,
         * kind, and the inherited 2px bottom margin outside the card. */
        h=150*f;for(size_t j=k;j<end;j++)h=fmaxf(h,browser_text_height(d,ps_browser_entry_name(&d->listing.entries[d->filtered[j]]),3,
            fmaxf(1,browser_grid_width(d,width,f)-(d->compact?20:26)*f),17.92f*f)+(2+2*(d->compact?15:20)+35+8+18+14.756f)*f);
        for(size_t j=k;j<end;j++)d->tops[j]=y;y+=h+2*f+browser_grid_gap(d,f);d->tops[end]=y;
    }}
    d->cache_dirty=false;d->cache_width=width;d->cache_scale=f;
}
static void browser_size(char text[64],const ps_browser_entry *entry) {
    if(entry->kind==PS_BROWSER_DIRECTORY)snprintf(text,64,"—");
    else if(entry->size<1024)snprintf(text,64,"%llu B",(unsigned long long)entry->size);
    else if(entry->size<1024*1024)snprintf(text,64,"%.1f KiB",(double)entry->size/1024);
    else snprintf(text,64,"%.1f MiB",(double)entry->size/(1024*1024));
}
/* Same breadcrumbs for pointer navigation and the full typed location field.
 * Ancestor splitting is purely lexical on an already resolved directory. */
static float browser_crumbs(app *a,browser_dialog *d,float x,float y,float width,float f,bool draw) {
    char path[4096],prefix[4096];
    if(ps_browser_path(d->location,path)!=PS_OK)return 32*f;
    size_t root=1;
#ifdef _WIN32
    if(path[1]==':')root=3;
    else {const char *share=strchr(path+2,'/');const char *end=share?strchr(share+1,'/'):NULL;root=end?(size_t)(end-path)+1:strlen(path);}
#endif
    size_t at=0,n=strlen(path),part=root;float left=x,top=y;unsigned index=0;
    while(at<n) {
        if(at) {part=strcspn(path+at,"/");if(!part){at++;continue;}}
        char label[4096];memcpy(label,path+at,part);label[part]=0;
        float natural=d->fonts[2].width(d->fonts[2].userdata,d->fonts[2].height,label,(int)part)+14*f;
        float bw=fminf(natural,width);if(left>x && left+bw>x+width){left=x;top+=36*f;}
        float bh=fmaxf(32*f,browser_text_height(d,label,2,bw-14*f,16.275f*f)+10*f);
        size_t end=at+part;memcpy(prefix,path,end);prefix[end]=0;
        if(draw){bool last=end==n || (end+1==n && path[end]=='/');int id=last?3:32+(int)index;
            struct nk_rect r=browser_slot(d,left,top,bw,bh);nk_stroke_rect(nk_window_get_canvas(d->ui),r,5*f,f,UI_COLOR(border));
            browser_text(a,d,left+7*f,top+5*f,bw-14*f,label,2,16.275f*f,UI_COLOR(accent),false);
            if(ps_ui_button_at(d->ui,r,label,false))browser_navigate(d,prefix,true);browser_focus_draw(a,d,id,r,label);}
        left+=bw+8*f;top+=bh>32*f?bh-32*f:0;at=end;if(at<n && path[at]=='/')at++;index++;
    }
    return top-y+32*f;
}
static void browser_list(app *a,browser_dialog *d,float x,float y,float width,float height,float f) {
    struct nk_context *ui=d->ui;
    d->row_count=0;d->selected_bounds=nk_rect(0,0,0,0);
    d->list_bounds=browser_slot(d,x,y,width,height);if(d->list_reset){nk_group_set_scroll(ui,"Explorer Dateiliste",0,0);d->list_reset=false;}
    nk_flags flags=NK_WINDOW_NO_SCROLLBAR;
    if(!d->failed && !(browser_busy(d) && d->operation==PS_BROWSER_READ_DIRECTORY)) {
        browser_rebuild(d,width,f);float content=browser_list_extent(d,f);
        if(content>height){flags=0;browser_rebuild(d,width-ui->style.window.scrollbar_size.x,f);}
    }
    nk_layout_space_push(ui,nk_rect(x,y,width,height));
    if(nk_group_begin(ui,"Explorer Dateiliste",flags)) {
        width=nk_window_get_content_region(ui).w;
        if(d->failed || (browser_busy(d) && d->operation==PS_BROWSER_READ_DIRECTORY)) {
            nk_layout_space_begin(ui,NK_STATIC,90*f,2);browser_text(a,d,18*f,20*f,width-36*f,d->failed?"Ordner nicht verfügbar":"Ordner wird geladen …",5,22.785f*f,d->failed?UI_COLOR(error):UI_COLOR(text),true);nk_layout_space_end(ui);
        } else {
            browser_rebuild(d,width,f);if(d->keyboard && d->focus==11 && d->selected<0 && d->count)browser_select(d,(int)d->filtered[0]);
            nk_uint sx=0,sy=0;nk_group_get_scroll(ui,"Explorer Dateiliste",&sx,&sy);
            float header=browser_list_start(d,f),content=browser_list_extent(d,f);
            if(d->keyboard && d->focus==11 && d->reveal && d->selected>=0)for(size_t k=0;k<d->count;k++)if((int)d->filtered[k]==d->selected) {
                size_t end=d->grid?k-k%d->grid_columns+d->grid_columns:k+1;if(end>d->count)end=d->count;
                float top=header+d->tops[k],bottom=header+d->tops[end];
                if(top<sy)sy=(nk_uint)top;else if(bottom>sy+height)sy=(nk_uint)fmaxf(0,bottom-height+4*f);
                nk_group_set_scroll(ui,"Explorer Dateiliste",0,sy);break;
            }
            nk_layout_space_begin(ui,NK_STATIC,fmaxf(content,height-4*f),(int)(d->count*5+4));
            if(d->count && !d->compact){browser_text(a,d,10*f,9*f,width-175*f,"Name",4,14.756f*f,UI_COLOR(muted),true);
                browser_text(a,d,width-165*f,9*f,90*f,"Art",4,14.756f*f,UI_COLOR(muted),true);browser_text(a,d,width-75*f,9*f,65*f,"Größe",4,14.756f*f,UI_COLOR(muted),true);}
            if(!d->count)browser_text(a,d,18*f,header+20*f,width-36*f,*d->query?"Keine passenden Einträge":"Dieser Ordner ist leer.",3,17.92f*f,UI_COLOR(muted),true);
            for(size_t k=0;k<d->count;k++) {
                size_t end=d->grid?k-k%d->grid_columns+d->grid_columns:k+1;if(end>d->count)end=d->count;
                float top=header+d->tops[k],bottom=header+d->tops[end];if(bottom<sy || top>sy+height)continue;
                ps_browser_entry *entry=&d->listing.entries[d->filtered[k]];bool selected=d->selected==(int)d->filtered[k];
                float rw=d->grid?browser_grid_width(d,width,f):width;
                float rx=d->grid?browser_grid_padding(d,f)+(k%d->grid_columns)*(rw+browser_grid_gap(d,f)):0,rh=bottom-top-2*f-(d->grid?browser_grid_gap(d,f):0);
                struct nk_rect r=browser_slot(d,rx,top,rw,rh);struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
                if(d->row_count<64){d->rows[d->row_count].index=(int)d->filtered[k];d->rows[d->row_count++].bounds=r;}
                bool hover=nk_input_is_mouse_hovering_rect(&ui->input,r);
                nk_fill_rect(canvas,r,6*f,selected?UI_COLOR(selection_active):hover?UI_COLOR(button_hover):d->grid?UI_COLOR(edit):nk_rgba(0,0,0,0));
                if(selected || d->grid)nk_stroke_rect(canvas,r,6*f,f,selected?UI_COLOR(accent):UI_COLOR(border));
                int icon=entry->kind==PS_BROWSER_DIRECTORY?PS_ICON_FOLDER:entry->kind==PS_BROWSER_RUN?PS_ICON_CHART:entry->kind==PS_BROWSER_C_SOURCE || entry->kind==PS_BROWSER_PHYS_SOURCE?-5:PS_ICON_FILE;
                char size[64];browser_size(size,entry);const char *name=ps_browser_entry_name(entry);
                float iw=(d->grid?35:18)*f,tw=fmaxf(1,rw-(d->grid?(d->compact?20:26):d->compact?51:229)*f);
                float th=browser_text_height(d,name,3,tw,17.92f*f),first=fmaxf(18*f,th);
                float icon_y=d->grid?(d->compact?16:21)*f:d->compact?12*f+(first-iw)/2:(rh-iw)/2;
                browser_icon(canvas,icon,nk_rect(r.x+(d->grid?(rw-iw)/2:(d->compact?10:11)*f),r.y+icon_y,iw,iw),UI_COLOR(accent));
                if(d->grid){float inset=(d->compact?10:13)*f,name_y=top+icon_y+iw+17*f;
                    browser_centered_text(d,rx+inset,name_y,tw,name,3,17.92f*f,UI_COLOR(text));
                    browser_centered_text(d,rx+inset,name_y+th+9*f,tw,browser_kind_name(entry->kind),4,14.756f*f,UI_COLOR(muted));}
                else if(d->compact){browser_text(a,d,rx+41*f,top+12*f+(first-th)/2,tw,name,3,17.92f*f,UI_COLOR(text),false);
                    browser_text(a,d,rx+41*f,top+12*f+first+9*f,tw,browser_kind_name(entry->kind),4,14.756f*f,UI_COLOR(muted),false);}
                else {float tw=rw-229*f,th=browser_text_height(d,name,3,fmaxf(30*f,tw),17.92f*f);
                    browser_text(a,d,rx+45*f,top+(rh-th)/2,fmaxf(30*f,tw),name,3,17.92f*f,UI_COLOR(text),false);
                    browser_text(a,d,rx+rw-175*f,top+(rh-14.756f*f)/2,90*f,browser_kind_name(entry->kind),4,14.756f*f,UI_COLOR(muted),false);
                    browser_text(a,d,rx+rw-76*f,top+(rh-14.756f*f)/2,65*f,size,4,14.756f*f,UI_COLOR(muted),false);}
                bool native=false;nk_begin_disabled(ui,browser_mutating(d));
                if(ps_ui_option_at_custom_focus(ui,r,"Dateien",name,selected,&native))browser_select(d,(int)d->filtered[k]);nk_end_disabled(ui);
                if(native && !d->keyboard){d->focus=11;d->focused_row=(int)d->filtered[k];}
                if(ps_ui_mouse_pressed_in_rect(&ui->input,NK_BUTTON_LEFT,r)){d->focus=11;d->focused_row=(int)d->filtered[k];}
                if(native || (d->keyboard && d->focus==11 && selected)) {
                    if(d->keyboard && selected)nk_sdl_accessibility_keyboard_target(ui,r);
                    nk_stroke_rect(canvas,r,6*f,2*f,UI_COLOR(accent));d->controls[11]=r;
                }
                if(selected)d->selected_bounds=r;
                if(entry->kind==PS_BROWSER_DIRECTORY && ps_ui_mouse_pressed_in_rect(&ui->input,NK_BUTTON_DOUBLE,r)){browser_navigate(d,entry->path,true);break;}
            }
            nk_layout_space_end(ui);
        }
        nk_group_end(ui);
    }
}
static float browser_preview_measure(void *user,const char *text,size_t bytes) {
    const struct nk_user_font *font=user;return font->width(font->userdata,font->height,text,(int)bytes);
}
/* Two widths occur when the containing preview acquires a scrollbar. Cache
 * both; otherwise a long preview would rebuild two complete glyph maps/frame. */
static browser_preview_layout *browser_preview_wrap(app *a,browser_dialog *d,float width,float f) {
    if(!d->preview.text)return NULL;
    const struct nk_user_font *font=&d->fonts[6];float pitch=10.5f*1.85f*f;
    for(unsigned k=0;k<2;k++){browser_preview_layout *cached=&d->text_layouts[k];
        if(cached->layout && cached->width==width && cached->layout->visual.row_height==pitch &&
           cached->font.height==font->height && cached->font.width==font->width && cached->font.userdata.ptr==font->userdata.ptr &&
           cached->layout->bytes==d->preview.length && !memcmp(cached->layout->source,d->preview.text,d->preview.length))return cached;}
    float available=fmaxf(1,width-d->ui->style.edit.cursor_size);ps_text_wrap *layout=NULL;
    if(!ps_text_wrap_build(d->preview.text,d->preview.length,available,pitch,browser_preview_measure,(void*)font,&layout))return NULL;
    bool scroll=layout->visual.row_count*pitch>240*f;
    if(scroll){ps_text_wrap *narrow=NULL;available=fmaxf(1,available-d->ui->style.window.scrollbar_size.x);
        if(!ps_text_wrap_build(d->preview.text,d->preview.length,available,pitch,browser_preview_measure,(void*)font,&narrow)){ps_text_wrap_destroy(layout);return NULL;}
        ps_text_wrap_destroy(layout);layout=narrow;}
    browser_preview_layout *cached=&d->text_layouts[d->text_layout_next++%2];
    if(cached->layout && d->edits[3].visual==&cached->layout->visual)d->edits[3].visual=NULL;
    ps_text_wrap_destroy(cached->layout);*cached=(browser_preview_layout){layout,*font,width,scroll};return cached;
}
static float browser_preview_extent(app *a,browser_dialog *d,float width,float f) {
    browser_preview_layout *cached=browser_preview_wrap(a,d,width,f);
    return cached?fminf(240*f,cached->layout->visual.row_count*cached->layout->visual.row_height):
        browser_text_height(d,"Textvorschau kann nicht dargestellt werden.",11,width,16.926f*f);
}
#include "file_browser_api_ui.inc"
static float browser_preview_content(app *a,browser_dialog *d,float width,float f,float *action_y) {
    const ps_browser_entry *entry=d->selected>=0?&d->listing.entries[d->selected]:NULL;
    const char *name=entry?ps_browser_entry_name(entry):d->filter==PS_BROWSER_FOLDERS?"Zielordner wählen":"Datei auswählen";
    float y=(entry?54.7f:73)*f+browser_text_height(d,name,5,width,22.785f*f)+12*f;
    if(entry){char size[64];browser_size(size,entry);
        const char *values[]={browser_kind_name(entry->kind),size,d->listing.directory,entry->error==PS_OK?"Verfügbar bei der Auflistung":"Nicht lesbar"};
        const char *labels[]={"Typ","Größe","Ort","Status"};
        for(unsigned k=0;k<4;k++)y+=browser_text_height(d,labels[k],3,width,17.36f*f)+(2+10)*f+browser_text_height(d,values[k],3,width,17.36f*f);
        *action_y=y+(entry->kind==PS_BROWSER_DIRECTORY?0:8*f);
        if(entry->kind==PS_BROWSER_DIRECTORY)y+=32*f;
        else if(d->preview.text)y+=8*f+browser_preview_extent(a,d,width,f)+(d->preview.truncated?6*f+browser_text_height(d,"Vorschau gekürzt · erste 64 KiB",11,width,16.926f*f):0);
        else y+=8*f+browser_text_height(d,browser_busy(d) && d->operation==PS_BROWSER_READ_TEXT?"Vorschau wird geladen …":"Für diese Datei ist keine Textvorschau verfügbar.",11,width,16.926f*f);
        if(entry->kind!=PS_BROWSER_DIRECTORY)y=browser_api_content(a,d,width,y,f,false);
        if(d->embedded && entry->kind!=PS_BROWSER_DIRECTORY)y+=50*f;
    } else {const char *description=d->filter==PS_BROWSER_FOLDERS?"Wähle einen vorhandenen Ordner als Speicherort.":"Ein Klick zeigt die Vorschau. Ordner öffnen sich per Doppelklick oder über die Aktion unten.";
        y+=browser_text_height(d,description,11,width,16.926f*f);*action_y=0;}
    return y+21*f;
}
static void browser_preview(app *a,browser_dialog *d,float x,float y,float width,float height,float f) {
    struct nk_context *ui=d->ui;
    d->preview_bounds=browser_slot(d,x,y,width,height);nk_fill_rect(nk_window_get_canvas(ui),d->preview_bounds,0,UI_COLOR(edit));
    nk_stroke_line(nk_window_get_canvas(ui),d->preview_bounds.x,d->preview_bounds.y,d->preview_bounds.x,d->preview_bounds.y+height,f,UI_COLOR(border));
    if(y>d->list_bounds.y+1)nk_stroke_line(nk_window_get_canvas(ui),d->preview_bounds.x,d->preview_bounds.y,d->preview_bounds.x+width,d->preview_bounds.y,f,UI_COLOR(border));
    struct nk_style_item background=ui->style.window.fixed_background;ui->style.window.fixed_background=nk_style_item_hide();
    if(d->preview_reset){nk_group_set_scroll(ui,"Explorer Vorschau",0,0);d->preview_reset=false;}
    d->preview_text_bounds=nk_rect(0,0,0,0);d->preview_text_height=0;
    float target_y=0,content=browser_preview_content(a,d,width-36*f,f,&target_y);nk_flags flags=NK_WINDOW_NO_SCROLLBAR;
    if(content>height){flags=0;content=browser_preview_content(a,d,width-36*f-ui->style.window.scrollbar_size.x,f,&target_y);}
    float text_width=width-36*f-(flags==0?ui->style.window.scrollbar_size.x:0);
    if(d->keyboard && d->reveal && (d->focus==12 || d->focus==13 || d->focus>=21 || (d->embedded && d->focus==20))) {
        float focus_y=d->focus==20?content-53*f:d->focus>=21?d->controls[d->focus].y:target_y;
        nk_uint sx,sy;nk_group_get_scroll(ui,"Explorer Vorschau",&sx,&sy);float target_h=d->focus==12?browser_preview_extent(a,d,text_width,f):32*f;
        if(d->focus>=21)target_h=fminf(height,d->controls[d->focus].h);
        if(focus_y<sy)sy=(nk_uint)focus_y;else if(focus_y+target_h>sy+height)sy=(nk_uint)fmaxf(0,focus_y+target_h-height+21*f);
        nk_group_set_scroll(ui,"Explorer Vorschau",0,sy);
    }
    /* Publish fresh target geometry even when the whole preview group is
     * outside the outer viewport and nk_group_begin skips its drawing. A
     * rectangle left over from a previous scroll would falsely finish reveal. */
    if(d->selected>=0){nk_uint sx,sy;nk_group_get_scroll(ui,"Explorer Vorschau",&sx,&sy);
        const ps_browser_entry *entry=&d->listing.entries[d->selected];
        if(entry->kind==PS_BROWSER_DIRECTORY)d->controls[13]=nk_rect(d->preview_bounds.x+18*f,d->preview_bounds.y+target_y-sy,text_width,32*f);
        else if(d->preview.text)d->controls[12]=nk_rect(d->preview_bounds.x+18*f,d->preview_bounds.y+target_y-sy,text_width,browser_preview_extent(a,d,text_width,f));
        if(d->embedded && entry->kind!=PS_BROWSER_DIRECTORY)d->controls[20]=nk_rect(d->preview_bounds.x+18*f,d->preview_bounds.y+content-53*f-sy,text_width,32*f);
        if(d->preview.api.symbol)for(unsigned k=21;k<=23;k++)d->controls[k]=nk_rect(d->preview_bounds.x+18*f,d->preview_bounds.y+d->controls[k].y-sy,d->controls[k].w,d->controls[k].h);
    }
    nk_layout_space_push(ui,nk_rect(x,y,width,height));
    if(nk_group_begin(ui,"Explorer Vorschau",flags)) {
        float w=nk_window_get_content_region(ui).w-36*f,top=21*f;
        const ps_browser_entry *entry=d->selected>=0?&d->listing.entries[d->selected]:NULL;
        const char *name=entry?ps_browser_entry_name(entry):d->filter==PS_BROWSER_FOLDERS?"Zielordner wählen":"Datei auswählen";
        nk_layout_space_begin(ui,NK_STATIC,fmaxf(height-4*f,content),INT_MAX);
        float badge_height=19.92f*f;struct nk_rect icon=browser_slot(d,18*f,top,(entry?18:40)*f,(entry?18:40)*f);
        if(entry){browser_icon(nk_window_get_canvas(ui),entry->kind==PS_BROWSER_DIRECTORY?PS_ICON_FOLDER:entry->kind==PS_BROWSER_RUN?PS_ICON_CHART:entry->kind==PS_BROWSER_C_SOURCE || entry->kind==PS_BROWSER_PHYS_SOURCE?-5:PS_ICON_FILE,icon,UI_COLOR(accent));
            const char *label=browser_kind_name(entry->kind);float bw=d->fonts[0].width(d->fonts[0].userdata,d->fonts[0].height,label,(int)strlen(label))+14.6f*f;
            struct nk_rect badge=browser_slot(d,42*f,top,bw,badge_height);nk_stroke_rect(nk_window_get_canvas(ui),badge,5*f,f,UI_COLOR(border));browser_text(a,d,49.3f*f,top+3.1f*f,bw-14.6f*f,label,0,13.72f*f,UI_COLOR(muted),false);}
        else browser_icon(nk_window_get_canvas(ui),-6,icon,UI_COLOR(accent));
        top+=(entry?33.7f*f:52*f);top+=browser_text(a,d,18*f,top,w,name,5,22.785f*f,UI_COLOR(text),true)+12*f;
        if(!entry)browser_text(a,d,18*f,top,w,d->filter==PS_BROWSER_FOLDERS?"Wähle einen vorhandenen Ordner als Speicherort.":"Ein Klick zeigt die Vorschau. Ordner öffnen sich per Doppelklick oder über die Aktion unten.",11,16.926f*f,UI_COLOR(muted),true);
        else {
            char size[64];browser_size(size,entry);
            const char *labels[]={"Typ","Größe","Ort","Status"};
            const char *values[]={browser_kind_name(entry->kind),size,d->listing.directory,entry->error==PS_OK?"Verfügbar bei der Auflistung":"Nicht lesbar"};
            for(unsigned k=0;k<4;k++){top+=browser_text(a,d,18*f,top,w,labels[k],3,17.36f*f,UI_COLOR(muted),true)+2*f;
                top+=browser_text(a,d,18*f,top,w,values[k],3,17.36f*f,UI_COLOR(text),true)+10*f;}
            if(entry->kind==PS_BROWSER_DIRECTORY){float action_width=fminf(w,d->fonts[2].width(d->fonts[2].userdata,d->fonts[2].height,"Ordner öffnen",(int)strlen("Ordner öffnen"))+24.4f*f);
                if(browser_button(a,d,13,18*f,top,action_width,32*f,"Ordner öffnen",-99,!browser_mutating(d),false,false,true))browser_action(a,d,13);}
            else if(d->preview.text && !strcmp(d->preview.path,entry->path)) {
                browser_preview_layout *cached=browser_preview_wrap(a,d,w,f);float text_height=browser_preview_extent(a,d,w,f);
                d->preview_text_height=text_height;
                if(cached){nk_layout_space_push(ui,nk_rect(18*f,top+8*f,w,text_height));struct nk_rect r=nk_widget_bounds(ui);d->preview_text_bounds=r;
                    if(d->keyboard && d->focus==12)nk_edit_focus(ui,NK_EDIT_ALWAYS_INSERT_MODE);
                    struct nk_style_edit saved_edit=ui->style.edit;ui->style.edit.padding=nk_vec2(0,0);ui->style.edit.border=0;
                    ui->style.edit.scrollbar_size=nk_vec2(cached->scroll?ui->style.window.scrollbar_size.x:0,0);
                    ui->style.edit.normal=ui->style.edit.hover=ui->style.edit.active=nk_style_item_color(nk_rgba(0,0,0,0));
                    nk_style_push_font(ui,&d->fonts[6]);nk_sdl_edit_named_visual(ui,"Dateivorschau",d->preview.length?d->preview.text:d->empty_preview,(int)(d->preview.length?d->preview.length+1:2),&d->edits[3],&cached->layout->visual);nk_style_pop_font(ui);
                    ui->style.edit=saved_edit;browser_focus_draw(a,d,12,r,"Dateivorschau");
                }else browser_text(a,d,18*f,top+8*f,w,"Textvorschau kann nicht dargestellt werden.",11,16.926f*f,UI_COLOR(error),true);
                if(d->preview.truncated)browser_text(a,d,18*f,top+text_height+14*f,w,"Vorschau gekürzt · erste 64 KiB",11,16.926f*f,UI_COLOR(muted),true);
            } else browser_text(a,d,18*f,top+8*f,w,browser_busy(d) && d->operation==PS_BROWSER_READ_TEXT?"Vorschau wird geladen …":"Für diese Datei ist keine Textvorschau verfügbar.",11,16.926f*f,UI_COLOR(muted),true);
            if(entry->kind!=PS_BROWSER_DIRECTORY){float source_end=top+8*f+(d->preview.text?browser_preview_extent(a,d,w,f)+(d->preview.truncated?6*f+browser_text_height(d,"Vorschau gekürzt · erste 64 KiB",11,w,16.926f*f):0):
                browser_text_height(d,browser_busy(d) && d->operation==PS_BROWSER_READ_TEXT?"Vorschau wird geladen …":"Für diese Datei ist keine Textvorschau verfügbar.",11,w,16.926f*f));
                browser_api_content(a,d,w,source_end,f,true);}
            if(d->embedded && entry->kind!=PS_BROWSER_DIRECTORY){
                float action_width=fminf(w,d->fonts[2].width(d->fonts[2].userdata,d->fonts[2].height,"Datei öffnen →",(int)strlen("Datei öffnen →"))+24.4f*f);
                if(browser_button(a,d,20,18*f,content-53*f,action_width,32*f,"Datei öffnen →",-99,!browser_mutating(d) && !d->failed && !d->path_dirty,false,false,true))browser_action(a,d,20);
            }
        }
        nk_layout_space_end(ui);nk_group_end(ui);
    }
    ui->style.window.fixed_background=background;
}
static void browser_view(app *a,browser_dialog *d,int width,int height,struct nk_rect frame) {
    if(!d || !d->ui)return;
    struct nk_context *ui=d->ui;const char *window_name=d->embedded?"Dateien":"Dateiauswahl";
    if(!d->visible){nk_window_close(ui,window_name);
        if(d->return_frames){nk_window_set_focus(ui,d->opener);if(nk_sdl_accessibility_request_focus(ui,NULL,d->return_label) || !--d->return_frames)d->return_frames=0;}return;}
    if(d->keyboard && nk_sdl_accessibility_has_focus(ui) && !nk_sdl_accessibility_keyboard_owned(ui))d->keyboard=false;
    struct nk_style saved=ui->style;const ps_ui_palette *prior=ui_palette;ps_ui_palette palette=*prior;float f=a->preferences.ui_size/16.f;
    bool compact=width<=560*f;if(compact!=d->compact){d->compact=compact;d->cache_dirty=true;}
    bool light=a->preferences.theme==PS_THEME_LIGHT;
    if(a->preferences.theme!=PS_THEME_HIGH_CONTRAST){palette.window=light?nk_rgb(250,251,253):nk_rgb(32,35,42);palette.edit=light?nk_rgb(255,255,255):nk_rgb(24,27,33);
        palette.surface=light?nk_rgb(240,243,247):nk_rgb(39,43,51);palette.border=light?nk_rgb(198,207,221):nk_rgb(60,65,76);
        palette.text=light?nk_rgb(32,44,60):nk_rgb(240,242,247);palette.muted=light?nk_rgb(88,104,124):nk_rgb(169,178,193);
        palette.accent=light?nk_rgb(32,92,170):nk_rgb(154,195,255);palette.selection_active=palette.button_hover=light?nk_rgb(220,233,250):nk_rgb(41,62,91);
        palette.primary=light?nk_rgb(32,93,172):nk_rgb(35,102,196);palette.primary_text=nk_rgb(255,255,255);}
    ui_palette=&palette;style(ui);
    const float sizes[]={9.8f,24.5f,10.5f,11.2f,9.52f,14.7f,10.5f,11.9f,10.08f,10.08f,11.9f,10.92f,10.22f,9.8f,9.8f};
    float previous_height=d->fonts[2].height;
    for(unsigned k=0;k<15;k++){bool strong=k==1 || k==5 || k==10 || k>=13,code=k==6 || k==8;d->fonts[k]=*(code?a->font_code:strong?a->font_title:a->font_ui);d->fonts[k].height=sizes[k]*(code?a->workbench_code_em:strong?a->workbench_bold_em:a->workbench_regular_em)*f;}
    /* Separate tracked headings from ordinary untracked type badges. These
     * persistent wrappers use the same native face for measuring and drawing. */
    for(unsigned k=13;k<15;k++) {
        documentation_spaced_font *caption=&d->caption_fonts[k-13];caption->base=a->font_title;
        caption->native=*(const struct nk_font *)caption->base->userdata.ptr;
        caption->spacing=(k==13?1.7f:1.f)*f;caption->reference_height=d->fonts[k].height;
        d->fonts[k].userdata=nk_handle_ptr(caption);d->fonts[k].width=documentation_spaced_width;
        d->fonts[k].query=documentation_spaced_query;
    }
    if(previous_height && previous_height!=d->fonts[2].height && d->keyboard)d->reveal=true;
    if(compact)d->fonts[1].height=19.6f*a->workbench_bold_em*f;
    nk_style_set_font(ui,&d->fonts[2]);ui->style.window.padding=ui->style.window.spacing=ui->style.window.group_padding=nk_vec2(0,0);ui->style.window.group_border=0;
    const char *title=d->mode==PS_DIALOG_MANAGER_PARENT?"Speicherort auswählen":d->mode==PS_DIALOG_IMPORT_RUN?"Messlauf auswählen":d->filter==PS_BROWSER_FOLDERS?"Ordner auswählen":"Dateien";
    const char *subtitle="Dateien ansehen, Ordner öffnen und einen Ort auswählen.";
    float outer=d->embedded?frame.w:fminf(1180*f,(float)width),w=outer,header=d->embedded?0:123.525f*f,tools=0,body=440*f,places=width<=850*f?140*f:width<=1150*f?150*f:170*f;
    float available=d->embedded?frame.h:(float)height;
    bool narrow=width<=1150*f,tools_wrap=w<760*f;unsigned columns=width<=850*f?2:3;
    if(d->grid_columns!=columns){d->grid_columns=columns;d->cache_dirty=true;}
    float path_width=tools_wrap?w-110*f:w-550*f;
    bool path_field=browser_path_field(d);
    float path_h=path_field?32*f:browser_crumbs(a,d,0,0,path_width,f,false);
    tools=fmaxf(32*f,path_h)+24*f+(tools_wrap?40*f:0);
    const char *confirm_label=d->filter==PS_BROWSER_FOLDERS?"Diesen Ordner wählen":d->filter==PS_BROWSER_RUNS?"Messlauf auswählen":"Öffnen";
    float cancel_w=d->fonts[7].width(d->fonts[7].userdata,d->fonts[7].height,"Abbrechen",9)+24.4f*f;
    float confirm_w=d->fonts[7].width(d->fonts[7].userdata,d->fonts[7].height,confirm_label,(int)strlen(confirm_label))+24.4f*f;
    float form=d->create?64*f:0,footer=54*f,footer_controls=12*f,dialog_footer=72*f,total=0,places_h=0,title_h=0,subtitle_y=0;
    bool footer_wrap=false,footer_actions_wrap=false,dialog_buttons_wrap=false,form_buttons_wrap=false;float dialog_controls=16*f;
    for(unsigned pass=0;pass<2;pass++){
        if(compact && !d->embedded){title_h=browser_text_height(d,title,1,w-96*f,30.38f*f);subtitle_y=18*f+15.19f*f+4*f+title_h+7*f;
            header=subtitle_y+browser_text_height(d,subtitle,3,w-36*f,17.36f*f)+18*f;}
        tools_wrap=w<760*f;path_width=compact?w-92*f:tools_wrap?w-110*f:w-550*f;
        path_h=path_field?32*f:browser_crumbs(a,d,0,0,path_width,f,false);tools=fmaxf(32*f,path_h)+(compact?92:tools_wrap?64:24)*f;
        places_h=0;if(compact){float pw=(w-28*f)/3;
            for(unsigned k=0;k<3;k++)places_h=fmaxf(places_h,fmaxf(32*f,fmaxf(18*f,browser_text_height(d,d->place_names[k],4,fmaxf(1,pw-35.2f*f),14.756f*f))+13.2f*f));
            places_h+=20*f;}
        footer_wrap=w<650*f;footer=footer_wrap?94*f:54*f;
        footer_controls=(footer_wrap?52:12)*f;
        footer_actions_wrap=w<362*f;if(footer_actions_wrap)footer+=40*f;
        form_buttons_wrap=w<290*f;if(d->create)form=(compact?(form_buttons_wrap?138:98):64)*f;
        if(d->embedded){char text[5120];snprintf(text,sizeof text,"%zu Einträge%s · %s%s%s",d->count,d->listing.omitted?" · begrenzt":"",d->location,*d->message?" · ":"",d->message);
            float text_height=browser_text_height(d,text,12,footer_wrap?w-32*f:w*.39f,15.841f*f);
            footer=fmaxf(footer,text_height+(footer_wrap?72:34)*f);if(footer_wrap)footer_controls=text_height+35*f;}
        const char *location=*d->message?d->message:d->path_dirty?"Ort mit Enter öffnen · Escape verwirft die Eingabe":d->selected>=0?d->listing.entries[d->selected].path:d->location;
        dialog_footer=d->embedded?0:fmaxf(72*f,browser_text_height(d,location,2,fmaxf(50*f,w-cancel_w-confirm_w-73*f),16.275f*f)+32*f);
        dialog_buttons_wrap=w<cancel_w+confirm_w+44*f;
        if(compact && !d->embedded){dialog_controls=15*f+browser_text_height(d,location,2,w-36*f,16.275f*f)+10*f;
            dialog_footer=dialog_controls+(dialog_buttons_wrap?77.77f:33.885f)*f+15*f;}
        total=header+tools+places_h+body*(narrow?2:1)+form+footer+dialog_footer;
        if(!pass && total+2>available)w-=ui->style.window.scrollbar_size.x;else break;
    }
    d->content_height=total;float h=available;d->bounds=d->embedded?frame:nk_rect((width-outer)/2,0,outer,h);
    ui->style.window.fixed_background=nk_style_item_color(UI_COLOR(window));ui->style.window.border=0;ui->style.window.rounding=0;
    nk_window_show(ui,window_name,NK_SHOWN);struct nk_window *window=nk_window_find(ui,window_name);
    if(window && total+2<=h)window->scrollbar.y=0;
    bool owns=!d->embedded || (!a->toolbar_menu && !a->toolbar_keyboard &&
        (d->keyboard || (window && ui->active==window) || ps_ui_mouse_pressed_in_rect(&ui->input,NK_BUTTON_LEFT,d->bounds)));
    if(window && !window->popup.active && owns)nk_window_set_focus(ui,window_name);
    if(nk_begin(ui,window_name,d->bounds,total+2>h?0:NK_WINDOW_NO_SCROLLBAR)) {
        if(d->embedded){
            /* Inactive workspace controls remain available to native clients,
             * like the header. Completed-stack occlusion still protects them. */
            ps_ui_accessibility_route(ui,true);
            if(owns){ui->active=ui->current;ui->current->flags&=~(nk_flags)NK_WINDOW_ROM;ui->current->layout->flags&=~(nk_flags)NK_WINDOW_ROM;}
        }
        /* One hot editor per window. Inactive fields must not unfocus the
         * field already visited earlier in this frame (or disable SDL IME). */
        if(d->keyboard)nk_edit_unfocus(ui);
        if(!d->embedded)nk_sdl_window_raise(ui,window_name);w=nk_window_get_content_region(ui).w;
        nk_layout_space_begin(ui,NK_STATIC,total,100);struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
        if(!d->embedded){browser_text(a,d,(compact?18:27)*f,(compact?18:23)*f,w-100*f,"DATEI-EXPLORER",13,15.19f*f,UI_COLOR(muted),true);
        browser_text(a,d,(compact?18:27)*f,(compact?37.19f:42.19f)*f,w-(compact?96:100)*f,title,1,(compact?30.38f:37.975f)*f,UI_COLOR(text),true);
        browser_text(a,d,(compact?18:27)*f,compact?subtitle_y:87.165f*f,w-(compact?36:100)*f,subtitle,3,17.36f*f,UI_COLOR(muted),true);
        if(browser_button(a,d,0,w-59*f,24*f,32*f,32*f,"",PS_ICON_CLOSE,true,false,false,false))browser_action(a,d,0);}
        struct nk_rect toolbar=browser_slot(d,0,header,w,tools);nk_fill_rect(canvas,toolbar,0,UI_COLOR(surface));
        nk_stroke_line(canvas,toolbar.x,toolbar.y,toolbar.x+w,toolbar.y,f,UI_COLOR(border));
        if(browser_button(a,d,1,(compact?9:15)*f,header+(compact?9:12)*f,32*f,32*f,"",-1,d->history_count && !browser_mutating(d),false,false,true))browser_action(a,d,1);
        if(browser_button(a,d,2,(compact?46:55)*f,header+(compact?9:12)*f,32*f,32*f,"",-2,!browser_mutating(d),false,false,true))browser_action(a,d,2);
        if(path_field)browser_field(a,d,3,d->path_input,4096,nk_rect((compact?83:95)*f,header+(compact?9:12)*f,path_width,32*f));else browser_crumbs(a,d,(compact?83:95)*f,header+(compact?9:12)*f,path_width,f,true);
        float ty=header+(compact?9:12)*f+(tools_wrap?path_h+8*f:0),tx=compact?9*f:tools_wrap?15*f:w-440*f;
        browser_text(a,d,tx,ty+8*f,35*f,"Suche",4,15.232f*f,UI_COLOR(muted),true);
        char query[256];strcpy(query,d->query);browser_field(a,d,4,d->query,sizeof d->query,nk_rect(tx+40*f,ty,compact?w-58*f:150*f,32*f));
        if(strcmp(query,d->query)){d->cache_dirty=d->list_reset=true;}
        float sort_y=compact?ty+37*f:ty,sort_x=compact?9*f:tx+198*f;
        browser_text(a,d,sort_x,sort_y+8*f,55*f,"Sortieren",4,15.232f*f,UI_COLOR(muted),true);
        nk_layout_space_push(ui,nk_rect(sort_x+57*f,sort_y,75*f,32*f));struct nk_rect sort=nk_widget_bounds(ui);
        if(ps_ui_combo_begin_named_scaled(ui,"Sortieren",d->sort==PS_BROWSER_SORT_NAME?"Name":"Art",nk_vec2(130*f,90*f),f)) {
            if(d->popup_close){nk_combo_close(ui);d->popup_close=false;}else {ps_ui_row_exact_dynamic(ui,30*f,1);if(nk_button_label(ui,"Name")){d->sort=PS_BROWSER_SORT_NAME;d->cache_dirty=true;nk_combo_close(ui);}if(nk_button_label(ui,"Art")){d->sort=PS_BROWSER_SORT_TYPE;d->cache_dirty=true;nk_combo_close(ui);}}nk_combo_end(ui);}
        browser_focus_draw(a,d,5,sort,"Sortieren");
        if(browser_button(a,d,6,compact?146*f:tx+338*f,sort_y,32*f,32*f,"",-3,true,false,!d->grid,true))browser_action(a,d,6);
        if(browser_button(a,d,7,compact?183*f:tx+378*f,sort_y,32*f,32*f,"",-4,true,false,d->grid,true))browser_action(a,d,7);
        float by=header+tools,preview_w=compact?w:narrow?w-places:270*f,list_w=compact?w:w-places-(narrow?0:preview_w);
        struct nk_rect side=browser_slot(d,0,by,compact?w:places,compact?places_h:body*(narrow?2:1));nk_fill_rect(canvas,side,0,UI_COLOR(surface));
        if(compact)nk_stroke_line(canvas,side.x,side.y+side.h,side.x+side.w,side.y+side.h,f,UI_COLOR(border));
        else {nk_stroke_line(canvas,side.x+side.w,side.y,side.x+side.w,side.y+side.h,f,UI_COLOR(border));
            browser_text(a,d,12*f,by+20*f,places-24*f,"ORTE",14,15.19f*f,UI_COLOR(muted),true);}
        float place_y=by+47*f;
        for(unsigned k=0;k<3;k++){float ph=fmaxf(18*f,browser_text_height(d,d->place_names[k],3,fmaxf(1,places-72.6f*f),17.36f*f))+20.2f*f;
            if(browser_button(a,d,8+(int)k,compact?10*f+k*(w-20*f+4*f)/3:12*f,compact?by+10*f:place_y,compact?(w-28*f)/3:places-24*f,compact?places_h-20*f:ph,d->place_names[k],k==2?PS_ICON_CHART:PS_ICON_FOLDER,*d->places[k] && !browser_mutating(d),false,!strcmp(d->location,d->places[k]),false))browser_action(a,d,8+(int)k);place_y+=ph+4*f;}
        float list_y=by+places_h;browser_list(a,d,compact?0:places,list_y,list_w,body,f);browser_preview(a,d,compact?0:narrow?places:places+list_w,narrow?list_y+body:list_y,preview_w,body,f);
        float fy=list_y+body*(narrow?2:1);
        if(d->create){browser_field(a,d,16,d->new_name,4096,nk_rect(16*f,fy+14*f,compact?w-32*f:w-290*f,34*f));
            if(browser_button(a,d,17,compact?16*f:w-258*f,fy+(compact?56:14)*f,form_buttons_wrap?w-32*f:112*f,34*f,"Ordner anlegen",-99,!browser_mutating(d),true,false,true))browser_action(a,d,17);
            if(browser_button(a,d,18,form_buttons_wrap?16*f:compact?136*f:w-138*f,fy+(form_buttons_wrap?98:compact?56:14)*f,form_buttons_wrap?w-32*f:122*f,34*f,"Abbrechen",-99,!browser_mutating(d),false,false,true))browser_action(a,d,18);fy+=form;}
        struct nk_rect footer_r=browser_slot(d,0,fy,w,footer);nk_stroke_line(canvas,footer_r.x,footer_r.y,footer_r.x+w,footer_r.y,f,UI_COLOR(border));
        char count[128];snprintf(count,sizeof count,"%zu Einträge%s",d->count,d->listing.omitted?" · begrenzt":"");
        if(d->embedded){char footer_text[5120];snprintf(footer_text,sizeof footer_text,"%s · %s%s%s",count,d->location,*d->message?" · ":"",d->message);browser_text(a,d,16*f,fy+17*f,footer_wrap?w-32*f:w*.39f,footer_text,12,15.841f*f,*d->message?UI_COLOR(error):UI_COLOR(muted),true);}
        else browser_text(a,d,16*f,fy+17*f,w*.36f,count,12,15.841f*f,UI_COLOR(muted),true);
        nk_layout_space_push(ui,nk_rect(footer_wrap?16*f:w*.42f,fy+footer_controls,200*f,30*f));nk_bool hidden=d->hidden;struct nk_rect toggle=nk_widget_bounds(ui);
        nk_style_push_font(ui,&d->fonts[12]);
        nk_style_push_color(ui,&ui->style.checkbox.text_normal,UI_COLOR(muted));
        nk_style_push_color(ui,&ui->style.checkbox.text_hover,UI_COLOR(muted));
        nk_style_push_color(ui,&ui->style.checkbox.text_active,UI_COLOR(muted));
        if(ps_ui_checkbox_named(ui,"Versteckte Einträge","Versteckte Einträge",&hidden)){d->hidden=hidden;d->cache_dirty=true;}
        nk_style_pop_color(ui);nk_style_pop_color(ui);nk_style_pop_color(ui);
        nk_style_pop_font(ui);
        browser_focus_draw(a,d,14,toggle,"Versteckte Einträge");
        if(browser_button(a,d,15,footer_actions_wrap?16*f:w-130*f,fy+footer_controls+(footer_actions_wrap?40*f:0),114*f,32*f,"Neuer Ordner",-99,!d->failed && !browser_busy(d),false,false,true))browser_action(a,d,15);
        fy+=footer;if(!d->embedded){struct nk_rect end=browser_slot(d,0,fy,w,dialog_footer);nk_stroke_line(canvas,end.x,end.y,end.x+w,end.y,f,UI_COLOR(border));
        const char *location=*d->message?d->message:d->path_dirty?"Ort mit Enter öffnen · Escape verwirft die Eingabe":d->selected>=0?d->listing.entries[d->selected].path:d->location;
        browser_text(a,d,(compact?18:24)*f,fy+(compact?15:16)*f,compact?w-36*f:fmaxf(50*f,w-cancel_w-confirm_w-73*f),location,2,16.275f*f,*d->message?UI_COLOR(error):UI_COLOR(muted),true);
        if(browser_button(a,d,19,dialog_buttons_wrap?18*f:w-(compact?18:24)*f-confirm_w-8*f-cancel_w,fy+dialog_controls,dialog_buttons_wrap?w-36*f:cancel_w,33.885f*f,"Abbrechen",-99,true,false,false,true))browser_action(a,d,19);
        bool confirm=!d->failed && !d->path_dirty && !browser_mutating(d) && !(browser_busy(d) && d->operation==PS_BROWSER_READ_DIRECTORY) && (d->filter==PS_BROWSER_FOLDERS || d->selected>=0);
        if(browser_button(a,d,20,dialog_buttons_wrap?18*f:w-(compact?18:24)*f-confirm_w,fy+dialog_controls+(dialog_buttons_wrap?43.885f*f:0),dialog_buttons_wrap?w-36*f:confirm_w,33.885f*f,confirm_label,-99,confirm,true,false,true))browser_action(a,d,20);}
        if(d->reveal){struct nk_rect r=d->focus==11?(d->selected_bounds.h>0?d->selected_bounds:d->list_bounds):d->controls[d->focus<32?d->focus:3],clip=nk_window_get_content_region(ui);float delta=0;
            /* The selected row is the focus target, not its fixed-height list.
             * A taller-than-viewport target otherwise alternates top/bottom. */
            r.h=fminf(r.h,clip.h);
            if(r.y<clip.y)delta=r.y-clip.y;else if(r.y+r.h>clip.y+clip.h)delta=r.y+r.h-clip.y-clip.h;
            nk_uint sx,sy;nk_window_get_scroll(ui,&sx,&sy);if(delta)nk_window_set_scroll(ui,sx,(nk_uint)fmaxf(0,ceilf(sy+delta)));else d->reveal=false;}
        nk_layout_space_end(ui);
    }
    nk_end(ui);if(d->draw_probe && !d->embedded)d->draw_probe(d);
    ui->style=saved;ui_palette=prior;
}
static void browser_input(app *a,bool begin) {
    for(unsigned i=1;i<PS_DIALOG_REQUEST_ROUTES;i++){browser_dialog *d=a->browser_popups[i];
        if(d && d->ui){if(begin)nk_input_begin(d->ui);else nk_input_end(d->ui);}}
}
static bool browser_windows_render(app *a) {
    bool okay=true;
    for(unsigned i=1;i<PS_DIALOG_REQUEST_ROUTES;i++){
        browser_dialog *d=a->browser_popups[i];if(!d || !d->visible || !d->window || SDL_GetWindowFlags(d->window)&(SDL_WINDOW_HIDDEN|SDL_WINDOW_MINIMIZED))continue;
        if(!ps_graphics_make_current(d->graphics)){okay=false;break;}
        nk_style_set_font(d->ui,a->font_ui);nk_sdl_set_ui_size(d->ui,a->preferences.ui_size);
        const ps_ui_palette *palette=a->preferences.theme==PS_THEME_LIGHT?&PS_UI_LIGHT:a->preferences.theme==PS_THEME_HIGH_CONTRAST?&PS_UI_HIGH_CONTRAST:&PS_UI_DARK;
        struct nk_color background=a->preferences.theme==PS_THEME_HIGH_CONTRAST?palette->window:a->preferences.theme==PS_THEME_LIGHT?nk_rgb(250,251,253):nk_rgb(32,35,42);
        ps_graphics_background(d->graphics,background.r,background.g,background.b);
        int w,h;SDL_GetWindowSize(d->window,&w,&h);browser_view(a,d,w,h,nk_rect(0,0,0,0));
        nk_sdl_update_TextInput(d->ui);if(!nk_sdl_render(d->ui))okay=false;
        if(*d->capture){if(!ps_graphics_capture(d->graphics,d->capture))okay=false;d->capture[0]=0;}
        if(!ps_graphics_present(d->graphics))okay=false;
    }
    if(!ps_graphics_make_current(a->graphics))okay=false;return okay;
}
