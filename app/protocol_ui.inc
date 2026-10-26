/* C / Navigator. All content comes from the shared native session history. */
static const char *protocol_origins[]={"Physim","Build","Simulation","Auswertung"};
static const char *protocol_levels[]={"Alle","Debug","Info","Warnungen","Fehler"};
static const char *protocol_outcome_names[]={"Keine Prozessauswirkung übermittelt","Läuft","Erfolgreich beendet","Fehlgeschlagen","Abgebrochen"};
static void protocol_open_from(app *a,const char *opener) {
    ps_protocol_ui *p=&a->protocol_view;
    if(!a->show_log){snprintf(p->return_window,sizeof p->return_window,"%s",a->ui->active?a->ui->active->name_string:"workspace");snprintf(p->return_action,sizeof p->return_action,"%s",opener?opener:"");}
    p->focus_return=false;a->show_log=true;
}
static void protocol_close(app *a) {
    if(a->protocol_view.resize)protocol_resize_abort(a);
    a->show_log=false;a->protocol_view.navigation_request=0;a->protocol_view.focus_return=true;a->protocol_view.return_frames=3;
    const char *target=a->protocol_view.return_window;
    nk_window_set_focus(a->ui,*target && nk_window_find(a->ui,target)?target:"workspace");
}
static void protocol_toggle(app *a,const char *opener) {if(a->show_log)protocol_close(a);else protocol_open_from(a,opener);}
static void protocol_return_focus(app *a) {
    ps_protocol_ui *p=&a->protocol_view;if(!p->focus_return || a->show_log)return;
    if(*p->return_action && nk_sdl_accessibility_request_focus(a->ui,NULL,p->return_action)){p->focus_return=false;return;}
    if(p->return_frames && --p->return_frames)return;
    nk_window_set_focus(a->ui,"workspace");struct nk_text_edit *edit=editor_search_target(a);if(edit)a->completion_focus_edit=edit;p->focus_return=false;
}
static struct nk_color protocol_level_color(ps_log_level level) {
    if(level==PS_LOG_ERROR)return UI_COLOR(error);
    if(level==PS_LOG_WARNING)return ui_palette==&PS_UI_HIGH_CONTRAST?UI_COLOR(accent):ui_palette->text.r<100?nk_rgb(128,85,9):nk_rgb(235,194,121);
    return level==PS_LOG_INFO?UI_COLOR(accent):UI_COLOR(muted);
}
static struct nk_color protocol_selection(void) {
    return UI_COLOR(selection_active);
}
static const char *protocol_level_name(ps_log_level level) {return level==PS_LOG_WARNING?"Warnung":protocol_levels[level];}
static bool protocol_contains(const void *bytes,size_t length,const char *query) {
    if(!*query)return true;const char *text=bytes;
    /* Stored raw bytes have a trailing NUL, and may contain further NULs. */
    while(length) {
        size_t size=0;while(size<length && text[size])size++;
        if(SDL_strcasestr(text,query))return true;
        if(size==length)break;text+=size+1;length-=size+1;
    }
    return false;
}
static bool protocol_matches(const ps_protocol_ui *p,const ps_protocol_message *m) {
    if((p->origin>=0 && (int)m->origin!=p->origin) || (p->level && (int)m->level!=p->level))return false;
    const char *fields[]={m->title,m->context,m->source,m->operation,m->argument,m->project};
    for(unsigned i=0;i<6;i++)if(protocol_contains(fields[i],strlen(fields[i]),p->query))return true;
    return protocol_contains(m->raw,m->raw_size,p->query);
}
static char *protocol_text_copy(const void *raw,size_t size,size_t *length) {
    if(size>(SIZE_MAX-13)/4)return NULL;size_t capacity=4*(size+3)+1;
    char *out=malloc(capacity);ps_protocol_text text={0};
    if(out && !ps_protocol_text_feed(&text,raw,size,true,out,capacity,length)){free(out);out=NULL;}
    return out;
}
static void protocol_preview(const char *text,char out[2064]) {
    size_t size=strlen(text),n=size>500?500:size;
    if(n<size)while(n && ((unsigned char)text[n]&0xc0)==0x80)n--;
    ps_protocol_text state={0};size_t used=0;
    ps_protocol_text_feed(&state,text,n,true,out,2064,&used);
    for(size_t i=0;i<used;i++)if(out[i]=='\r' || out[i]=='\n' || out[i]=='\t')out[i]=' ';
    if(n<size)memcpy(out+used,"…",4);
}
static void protocol_row_name(const ps_protocol_record *record,char name[2300]) {
    char preview[2064];protocol_preview(record->message.title,preview);
    snprintf(name,2300,"Meldung %llu: %s · %s",(unsigned long long)record->id,protocol_level_name(record->message.level),preview);
}
static bool protocol_navigation_key(app *a,const SDL_KeyboardEvent *key) {
    ps_protocol_ui *p=&a->protocol_view;
    if(!a->show_log || a->project_manager || key->mod&(SDL_KMOD_CTRL|SDL_KMOD_ALT|SDL_KMOD_GUI) || key->key==SDLK_TAB){p->navigation_request=0;return false;}
    int delta=key->key==SDLK_UP?-1:key->key==SDLK_DOWN?1:0;
    bool page=key->key==SDLK_PAGEUP || key->key==SDLK_PAGEDOWN,edge=key->key==SDLK_HOME || key->key==SDLK_END;
    if(!delta && !page && !edge)return false;
    const ps_protocol_record *records[PS_PROTOCOL_CAPACITY];ps_protocol_summary summary=ps_protocol_describe(&a->protocol);size_t count=0,index=0;bool owned=p->navigation_request && p->navigation_ui==a->ui;
    for(size_t i=0;i<summary.count;i++) {
        const ps_protocol_record *r=ps_protocol_at(&a->protocol,i);if(!protocol_matches(p,&r->message))continue;
        records[count]=r;if(r->id==p->selected)index=count;
        char name[2300];protocol_row_name(r,name);if(nk_sdl_accessibility_is_focused(a->ui,NULL,name))owned=true;
        count++;
    }
    float f=a->preferences.ui_size/16.f;
    if(!owned || !count) {
        struct nk_window *window=nk_window_find(a->ui,"log");
        if(page && window && a->ui->active==window && !(window->flags&NK_WINDOW_NO_SCROLLBAR) &&
           !nk_sdl_accessibility_is_focused(a->ui,NULL,"Meldung oder Quelle") && !nk_sdl_accessibility_is_focused(a->ui,NULL,"Originalmeldung")) {
            nk_uint step=(nk_uint)fmaxf(32,p->bounds.h*.8f);
            window->scrollbar.y=key->key==SDLK_PAGEUP?(window->scrollbar.y>step?window->scrollbar.y-step:0):window->scrollbar.y>UINT_MAX-step?UINT_MAX:window->scrollbar.y+step;
            return true;
        }
        return false;
    }
    if(page)delta=(key->key==SDLK_PAGEUP?-1:1)*(int)fmaxf(1,floorf(p->panes[1].h/(63*f)));
    int target=key->key==SDLK_HOME?0:key->key==SDLK_END?(int)count-1:(int)index+delta;
    if(target<0)target=0;if(target>=(int)count)target=(int)count-1;
    float top=0,row=0,width=fmaxf(1,p->list_width-42*f);
    if(p->row_count==count && p->row_ids[target]==records[target]->id) {
        top=p->rows[target].y-p->panes[1].y-8*f+p->rendered_scroll_y;row=p->rows[target].h;
    } else for(int i=0;i<=target;i++){char preview[2064];protocol_preview(records[i]->message.title,preview);row=(ps_ui_text_line(&p->fonts[2],preview,(int)strlen(preview),width,false)<(int)strlen(preview)?82:63)*f;if(i<target)top+=row;}
    float viewport=fmaxf(1,p->panes[1].h-16*f);
    if(top<p->scroll_y)p->scroll_y=(nk_uint)top;
    else if(top+row>p->scroll_y+viewport)p->scroll_y=(nk_uint)ceilf(top+row-viewport);
    p->selected=records[target]->id;p->transcript=false;p->follow=false;p->original_id=0;p->navigation_request=p->selected;p->navigation_ui=a->ui;return true;
}
static void protocol_symbol(struct nk_command_buffer *c,struct nk_rect r,ps_log_level level,struct nk_color color) {
    float x=r.x+r.w/2,y=r.y+r.h/2,s=fminf(r.w,r.h)*.32f;
    if(level==PS_LOG_ERROR){nk_stroke_circle(c,nk_rect(x-s,y-s,2*s,2*s),1.5f,color);nk_stroke_line(c,x-s*.5f,y-s*.5f,x+s*.5f,y+s*.5f,1.5f,color);nk_stroke_line(c,x-s*.5f,y+s*.5f,x+s*.5f,y-s*.5f,1.5f,color);}
    else if(level==PS_LOG_WARNING){nk_stroke_triangle(c,x,y-s,x-s,y+s,x+s,y+s,1.3f,color);nk_stroke_line(c,x,y-s*.35f,x,y+s*.3f,1,color);nk_fill_circle(c,nk_rect(x-.7f,y+s*.5f,1.4f,1.4f),color);}
    else if(level==PS_LOG_INFO){nk_stroke_circle(c,nk_rect(x-s,y-s,2*s,2*s),1.3f,color);nk_stroke_line(c,x,y-1,x,y+s*.6f,1.3f,color);nk_fill_circle(c,nk_rect(x-.8f,y-s*.65f,1.6f,1.6f),color);}
    else {nk_stroke_line(c,x,y-s,x+s,y,1.3f,color);nk_stroke_line(c,x+s,y,x,y+s,1.3f,color);nk_stroke_line(c,x,y+s,x-s,y,1.3f,color);nk_stroke_line(c,x-s,y,x,y-s,1.3f,color);}
}
static void protocol_origin_path(struct nk_command_buffer *canvas,struct nk_rect r,const float *points,unsigned count,bool closed,struct nk_color color) {
    float scale=r.w/24,stroke=1.5f*scale;
    for(unsigned i=0;i<count;i++) {
        float x=r.x+points[2*i]*scale,y=r.y+points[2*i+1]*scale;
        if(i || closed){unsigned previous=i?i-1:count-1;nk_stroke_line(canvas,r.x+points[2*previous]*scale,r.y+points[2*previous+1]*scale,x,y,stroke,color);}
        nk_fill_circle(canvas,nk_rect(x-stroke/2,y-stroke/2,stroke,stroke),color);
    }
}
static void protocol_origin_symbol(struct nk_command_buffer *canvas,struct nk_rect r,int origin,struct nk_color color) {
    /* Exact 24-unit paths from the approved log/code/scene SVGs. */
    static const float document[]={4,3,20,3,20,21,4,21},line1[]={8,8,16,8},line2[]={8,12,16,12},line3[]={8,16,13,16};
    static const float code1[]={8,6,2,12,8,18},code2[]={16,6,22,12,16,18},slash[]={13,3,11,21};
    static const float cube[]={12,2,21,7,21,17,12,22,3,17,3,7},fold[]={3,7,12,12,21,7},edge[]={12,12,12,22};
    if(origin==PS_PROTOCOL_BUILD){protocol_origin_path(canvas,r,code1,3,false,color);protocol_origin_path(canvas,r,code2,3,false,color);protocol_origin_path(canvas,r,slash,2,false,color);}
    else if(origin==PS_PROTOCOL_SIMULATION){protocol_origin_path(canvas,r,cube,6,true,color);protocol_origin_path(canvas,r,fold,3,false,color);protocol_origin_path(canvas,r,edge,2,false,color);}
    else {protocol_origin_path(canvas,r,document,4,true,color);protocol_origin_path(canvas,r,line1,2,false,color);protocol_origin_path(canvas,r,line2,2,false,color);protocol_origin_path(canvas,r,line3,2,false,color);}
}
static struct nk_rect protocol_space(struct nk_context *ui,struct nk_rect r) {struct nk_rect result;nk_layout_space_push(ui,r);nk_widget(&result,ui);return result;}
static void protocol_text_at(app *a,const struct nk_user_font *font,struct nk_rect r,const char *text,struct nk_color color) {
    nk_draw_text(nk_window_get_canvas(a->ui),r,text,(int)strlen(text),font,nk_rgba(0,0,0,0),color);
}
static void protocol_wrap(app *a,const char *text,const struct nk_user_font *font,float line,struct nk_color color) {
    float width=fmaxf(1,nk_window_get_content_region(a->ui).w);int at=0,n=(int)strlen(text),lines=0;
    while(at<n){int used=ps_ui_text_line(font,text+at,n-at,width,false);if(used<=0)used=1;at+=used;lines++;}
    nk_layout_row_dynamic(a->ui,fmaxf(line,line*lines),1);struct nk_rect r;nk_widget(&r,a->ui);
    ps_ui_accessibility_label(a->ui,r,text);at=0;
    while(at<n){int used=ps_ui_text_line(font,text+at,n-at,width,false);if(used<=0)used=1;nk_draw_text(nk_window_get_canvas(a->ui),nk_rect(r.x,r.y,width,font->height),text+at,used,font,nk_rgba(0,0,0,0),color);at+=used;r.y+=line;}
}
static void protocol_property(app *a,const char *label,const char *value,float f) {
    const struct nk_user_font *font=&a->protocol_view.fonts[0];float w=fmaxf(1,nk_window_get_content_region(a->ui).w),right=w*.5f;
    int length=(int)strlen(value),at=0,lines=0;
    while(at<length){int n=ps_ui_text_line(font,value+at,length-at,right,false);at+=n>0?n:1;lines++;}
    nk_layout_row_dynamic(a->ui,fmaxf(20*f,18*f*lines),1);struct nk_rect r;nk_widget(&r,a->ui);struct nk_rect label_bounds=r;
    protocol_text_at(a,font,nk_rect(r.x,r.y,right-10*f,font->height),label,muted());at=0;
    while(at<length){int n=ps_ui_text_line(font,value+at,length-at,right,false);if(n<=0)n=1;nk_draw_text(nk_window_get_canvas(a->ui),nk_rect(r.x+r.w-right,r.y,right,font->height),value+at,n,font,nk_rgba(0,0,0,0),UI_COLOR(text));at+=n;r.y+=18*f;}
    char text[2300];snprintf(text,sizeof text,"%s: %s",label,value);ps_ui_accessibility_label(a->ui,label_bounds,text);
}
static bool protocol_disclosure(app *a,const char *text,bool open,float f,struct nk_rect *bounds) {
    nk_layout_row_dynamic(a->ui,28*f,1);struct nk_rect r;nk_widget(&r,a->ui);struct nk_command_buffer *c=nk_window_get_canvas(a->ui);float x=r.x+5*f,y=r.y+12*f;
    if(bounds)*bounds=r;
    if(open){nk_stroke_line(c,x-3*f,y-2*f,x,y+2*f,1,muted());nk_stroke_line(c,x,y+2*f,x+3*f,y-2*f,1,muted());}
    else {nk_stroke_line(c,x-2*f,y-3*f,x+2*f,y,1,muted());nk_stroke_line(c,x+2*f,y,x-2*f,y+3*f,1,muted());}
    protocol_text_at(a,&a->protocol_view.fonts[0],nk_rect(r.x+14*f,r.y+6*f,r.w-14*f,20*f),text,muted());
    char name[128];snprintf(name,sizeof name,"%s %s",text,open?"ausblenden":"anzeigen");return ps_ui_button_at(a->ui,r,name,open);
}
static bool protocol_original(app *a,const ps_protocol_record *selected) {
    ps_protocol_ui *p=&a->protocol_view;const ps_protocol_record *record=selected;
    if(p->transcript && selected->message.transcript_id) {
        record=ps_protocol_find(&a->protocol,selected->message.transcript_id);
        if(!record){snprintf(p->feedback,sizeof p->feedback,"Die vollständige Prozessausgabe ist nicht mehr in der Sitzungshistorie.");record=selected;}
    }
    if(p->original && p->original_id==record->id && p->original_size==record->message.raw_size)return true;
    size_t length=0;char *text=protocol_text_copy(record->message.raw,record->message.raw_size,&length);
    if(!text){snprintf(p->feedback,sizeof p->feedback,"Originaltext konnte nicht dargestellt werden: nicht genügend Speicher.");return false;}
    free(p->original);p->original=text;p->original_id=record->id;p->original_size=record->message.raw_size;p->original_length=length;p->page=0;
    ps_text_wrap_destroy(p->original_layout);p->original_layout=NULL;
    p->original_edit.cursor=p->original_edit.select_start=p->original_edit.select_end=0;
    p->original_edit.scrollbar=nk_vec2(0,0);p->original_edit.visual=NULL;
    p->original_edit.active=false;nk_edit_unfocus(a->ui);return true;
}
typedef struct {char *data;size_t size,capacity;bool failed;} protocol_buffer;
static void protocol_buffer_add(protocol_buffer *b,const char *text,size_t size) {
    if(b->failed)return;
    if(size>32u*1024u*1024u || b->size>32u*1024u*1024u-size){b->failed=true;return;}
    if(b->size+size+1>b->capacity) {
        size_t capacity=b->capacity*2;if(capacity<b->size+size+1)capacity=b->size+size+1;char *next=realloc(b->data,capacity);
        if(!next){b->failed=true;return;}b->data=next;b->capacity=capacity;
    }
    memcpy(b->data+b->size,text,size);b->size+=size;b->data[b->size]=0;
}
static void protocol_buffer_text(protocol_buffer *b,const char *text) {size_t size=0;char *copy=protocol_text_copy(text,strlen(text),&size);if(!copy){b->failed=true;return;}protocol_buffer_add(b,copy,size);free(copy);}
static void protocol_export(app *a) {
    ps_protocol_ui *p=&a->protocol_view;if(p->save)return;
    ps_protocol_summary summary=ps_protocol_describe(&a->protocol);protocol_buffer b={0};char header[256];
    snprintf(header,sizeof header,"Physim · Sitzungsprotokoll\nBis zu 200 Meldungen; %llu ausgelassene Meldungen, %llu ausgelassene Bytes.\nNicht darstellbare Originalbytes erscheinen als \\xHH.\n\n",(unsigned long long)summary.omitted_messages,(unsigned long long)summary.omitted_bytes);
    protocol_buffer_text(&b,header);
    for(size_t i=0;i<summary.count;i++) {
        const ps_protocol_record *r=ps_protocol_at(&a->protocol,i);const ps_protocol_message *m=&r->message;if(!protocol_matches(p,m))continue;
        snprintf(header,sizeof header,"[%llu] %s · %s · %s\n",(unsigned long long)r->id,protocol_level_name(m->level),protocol_origins[m->origin],protocol_outcome_names[m->outcome]);protocol_buffer_text(&b,header);
        protocol_buffer_text(&b,m->title);protocol_buffer_text(&b,"\nKontext: ");protocol_buffer_text(&b,m->context);
        protocol_buffer_text(&b,"\nQuelle: ");protocol_buffer_text(&b,m->source);
        if(m->line){snprintf(header,sizeof header,":%u:%u",m->line,m->column);protocol_buffer_text(&b,header);}
        if(m->has_time){snprintf(header,sizeof header,"\nModellzeit: %.17g s",m->time_s);protocol_buffer_text(&b,header);}
        snprintf(header,sizeof header,"\nOriginal (%zu Bytes, %llu ausgelassen):\n",m->raw_size,(unsigned long long)r->omitted_bytes);protocol_buffer_text(&b,header);
        size_t size=0;char *raw=protocol_text_copy(m->raw,m->raw_size,&size);if(!raw)b.failed=true;else{protocol_buffer_add(&b,raw,size);free(raw);}protocol_buffer_text(&b,"\n\n");
    }
    if(b.failed){free(b.data);snprintf(p->feedback,sizeof p->feedback,"Export konnte nicht vorbereitet werden: Speichergrenze erreicht.");return;}
    ps_graphic_bytes bytes={(unsigned char*)b.data,b.size};p->save=ps_graphic_save_create(&bytes);ps_graphic_bytes_free(&bytes);
    if(!p->save){snprintf(p->feedback,sizeof p->feedback,"Export konnte nicht gestartet werden.");return;}
    if(*p->export_test_path)ps_graphic_save_resolve(p->save,p->export_test_path,false);
    else ps_graphic_save_show_text(p->save,a->window,"Physim-Protokoll.txt");
    snprintf(p->feedback,sizeof p->feedback,"Speicherort für die gefilterten Meldungen wählen …");
}
static void protocol_tick(app *a) {
    ps_protocol_ui *p=&a->protocol_view;if(!p->save)return;
    ps_graphic_save_state state=ps_graphic_save_poll(p->save);
    if(state==PS_GRAPHIC_SAVE_WAITING || state==PS_GRAPHIC_SAVE_WRITING)return;
    snprintf(p->feedback,sizeof p->feedback,"%s",state==PS_GRAPHIC_SAVE_OK?"Gefilterte Meldungen gespeichert.":state==PS_GRAPHIC_SAVE_CANCELLED?"Speichern abgebrochen.":"Speichern fehlgeschlagen. Zielordner und Berechtigungen prüfen.");
    ps_graphic_save_destroy(p->save);p->save=NULL;
}
static void protocol_shutdown(app *a) {
    ps_graphic_save_destroy(a->protocol_view.save);free(a->protocol_view.original);
    ps_text_wrap_destroy(a->protocol_view.original_layout);
}
static float protocol_original_measure(void *user,const char *text,size_t bytes) {
    const struct nk_user_font *font=user;
    return font->width(font->userdata,font->height,text,(int)bytes);
}
static bool protocol_source_path(const ps_protocol_message *m,char path[4096]) {
    if(!m->source[0] || !m->line)return false;
    bool absolute=m->source[0]=='/' || m->source[0]=='\\' || m->source[1]==':';
    if(!absolute && !m->project[0])return false;
    int n=absolute?snprintf(path,4096,"%s",m->source):snprintf(path,4096,"%s/%s",m->project,m->source);
    SDL_PathInfo info;return n>0 && n<4096 && SDL_GetPathInfo(path,&info) && info.type==SDL_PATHTYPE_FILE;
}
static void protocol_jump(app *a,uint64_t id) {
    const ps_protocol_record *record=ps_protocol_find(&a->protocol,id);char path[4096];
    if(!record || !protocol_source_path(&record->message,path)){snprintf(a->protocol_view.feedback,sizeof a->protocol_view.feedback,"Quelldatei ist nicht verfügbar.");return;}
    diagnostic d={0};d.line=record->message.line<=INT_MAX?(int)record->message.line:0;d.column=record->message.column<=INT_MAX?(int)record->message.column:1;
    snprintf(d.source,sizeof d.source,"%s",path);char current[4096];join(current,sizeof current,a->project,experiment_source(a));bool experiment=ps_text_document_same_file(path,current);
    join(current,sizeof current,a->project,analysis_source(a));d.analysis=ps_text_document_same_file(path,current);d.foreign=!experiment && !d.analysis;
    jump_to_diagnostic(a,&d);
}
static void protocol_original_reveal(ps_protocol_ui *p,struct nk_rect clip,ps_ui_text_reveal request) {
    if(!request.requested || !p->original_layout || clip.h<=0)return;
    const struct nk_text_visual *v=&p->original_layout->visual;
    int row=nk_text_visual_row_at(v,request.scalar,p->original_edit.cursor_at_end_of_line);
    float top=p->original_bounds.y+row*v->row_height,bottom=top+v->row_height,delta=0;
    if(request.alignment==PS_UI_TEXT_TOP)delta=top-clip.y;
    else if(request.alignment==PS_UI_TEXT_BOTTOM)delta=bottom-clip.y-clip.h;
    else if(top<clip.y)delta=top-clip.y;
    else if(bottom>clip.y+clip.h)delta=bottom-clip.y-clip.h;
    double target=fmax(0,(double)p->detail_scroll_y+(delta<0?floorf(delta):ceilf(delta)));
    nk_uint next=(nk_uint)fmin(target,(double)UINT_MAX);
    if(next!=p->detail_scroll_y){p->detail_scroll_target=next;p->detail_reveal_pending=true;}
}
static bool protocol_action(app *a,struct nk_rect box,const char *label,bool link,bool enabled,float f) {
    struct nk_context *ui=a->ui;struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    const struct nk_user_font *font=&a->protocol_view.fonts[10];
    bool hover=enabled && nk_input_is_mouse_hovering_rect(&ui->input,ui->current->layout->clip) && nk_input_is_mouse_hovering_rect(&ui->input,box);
    struct nk_color fill=hover?UI_COLOR(selection_active):UI_COLOR(header),text=link?UI_COLOR(accent):UI_COLOR(text),border=UI_COLOR(border);
    if(!enabled){fill.a=text.a=border.a=(nk_byte)(255*.45f);}
    if(!link || hover)nk_fill_rect(canvas,box,7*f,fill);
    if(!link)nk_stroke_rect(canvas,nk_rect(box.x+.5f*f,box.y+.5f*f,box.w-f,box.h-f),6.5f*f,f,border);
    /* Nuklear's standard text button subtracts rounding from content space.
     * CSS rounding does not add padding; keep the full glyph-height rectangle. */
    float width=font->width(font->userdata,font->height,label,(int)strlen(label));
    protocol_text_at(a,font,nk_rect(box.x+(box.w-width)/2,box.y+(box.h-font->height)/2,width+1,font->height),label,text);
    nk_begin_disabled(ui,!enabled);bool pressed=ps_ui_button_at(ui,box,label,false);nk_end_disabled(ui);return pressed;
}
static void protocol_actions(app *a,const ps_protocol_record *record,bool original_ready,float f) {
    struct nk_context *ui=a->ui;ps_protocol_ui *p=&a->protocol_view;
    const char *labels[]={"Quellstelle ansehen →","Meldung kopieren"};char path[4096];
    bool source=protocol_source_path(&record->message,path);
    const struct nk_user_font *font=&p->fonts[10];float width[2],available=nk_window_get_content_region(ui).w;
    /* CSS: 14px font / 1.55 line-height, .48rem .8rem padding, 1px border.
     * The row wraps at intrinsic widths with a 10px gap. */
    for(unsigned i=0;i<2;i++)width[i]=font->width(font->userdata,font->height,labels[i],(int)strlen(labels[i]))+24.4f*f;
    float height=37.14f*f,gap=10*f;bool wrap=source && width[0]+gap+width[1]>available;
    nk_layout_space_begin(ui,NK_STATIC,wrap?2*height+gap:height,source?2:1);
    if(source) {
        p->source=protocol_space(ui,nk_rect(0,0,width[0],height));
        if(protocol_action(a,p->source,labels[0],true,true,f))p->source_request=record->id;
    }
    p->copy=protocol_space(ui,nk_rect(source && !wrap?width[0]+gap:0,wrap?height+gap:0,width[1],height));
    if(protocol_action(a,p->copy,labels[1],false,original_ready,f))snprintf(p->feedback,sizeof p->feedback,"%s",SDL_SetClipboardText(p->original)?"Meldung kopiert.":"Zwischenablage nicht verfügbar. Originaltext auswählen oder TXT speichern.");
    nk_layout_space_end(ui);
}
static void protocol_detail(app *a,const ps_protocol_record *record,float scale) {
    struct nk_context *ui=a->ui;ps_protocol_ui *p=&a->protocol_view;
    if(!record){protocol_wrap(a,"Keine Meldung ausgewählt. Wähle eine Meldung aus der Liste.",&a->protocol_view.fonts[0],20*scale,muted());return;}
    const ps_protocol_message *m=&record->message;char text[2064];
    const struct nk_user_font *small=&p->fonts[1];const char *level=protocol_level_name(m->level);
    float badge=small->width(small->userdata,small->height,level,(int)strlen(level))+14*scale;
    nk_layout_space_begin(ui,NK_STATIC,22*scale,1);struct nk_rect header=protocol_space(ui,nk_rect(0,0,nk_window_get_content_region(ui).w,22*scale));
    protocol_symbol(nk_window_get_canvas(ui),nk_rect(header.x,header.y+1*scale,20*scale,20*scale),m->level,protocol_level_color(m->level));
    struct nk_rect tag=nk_rect(header.x+28*scale,header.y+2*scale,badge,18*scale);nk_stroke_rect(nk_window_get_canvas(ui),tag,5,1,UI_COLOR(border));
    protocol_text_at(a,small,nk_rect(tag.x+7*scale,tag.y+3*scale,badge-10*scale,small->height),level,protocol_level_color(m->level));
    snprintf(text,sizeof text,"%s",protocol_origins[m->origin]);protocol_text_at(a,small,nk_rect(tag.x+tag.w+10*scale,tag.y+3*scale,fmaxf(1,header.w-tag.w-38*scale),small->height),text,muted());
    ps_ui_accessibility_label(ui,header,level);nk_layout_space_end(ui);nk_layout_row_dynamic(ui,10*scale,1);nk_spacer(ui);
    size_t title_length=0;char *title=protocol_text_copy(m->title,strlen(m->title),&title_length);
    if(title){protocol_wrap(a,title,&p->title_font,26*scale,UI_COLOR(text));free(title);}
    const char *outcomes[]={"Die Meldung allein verändert den Laufzustand nicht.","Dieser Prozess läuft.","Dieser Prozess wurde erfolgreich beendet.","Dieser Prozess ist fehlgeschlagen.","Dieser Prozess wurde abgebrochen."};
    ps_protocol_outcome outcome=m->outcome;
    if(outcome==PS_PROTOCOL_NO_OUTCOME && m->transcript_id){const ps_protocol_record *parent=ps_protocol_find(&a->protocol,m->transcript_id);if(parent)outcome=parent->message.outcome;}
    nk_layout_row_dynamic(ui,12*scale,1);nk_spacer(ui);
    float impact_width=fmaxf(1,nk_window_get_content_region(ui).w-26*scale);int at=0,impact_lines=0,len=(int)strlen(outcomes[outcome]);
    while(at<len){int n=ps_ui_text_line(&p->fonts[0],outcomes[outcome]+at,len-at,impact_width,false);at+=n>0?n:1;impact_lines++;}
    nk_layout_row_dynamic(ui,(22+19*impact_lines)*scale,1);struct nk_rect impact;nk_widget(&impact,ui);
    struct nk_color surface=UI_COLOR(header);if(a->preferences.theme!=PS_THEME_HIGH_CONTRAST)surface=m->level==PS_LOG_ERROR?(a->preferences.theme==PS_THEME_LIGHT?nk_rgb(251,238,240):nk_rgb(58,40,45)):m->level==PS_LOG_WARNING?(a->preferences.theme==PS_THEME_LIGHT?nk_rgb(250,242,223):nk_rgb(53,47,36)):surface;
    nk_fill_rect(nk_window_get_canvas(ui),impact,0,surface);nk_fill_rect(nk_window_get_canvas(ui),nk_rect(impact.x,impact.y,2,impact.h),0,protocol_level_color(m->level));at=0;float y=impact.y+11*scale;
    while(at<len){int n=ps_ui_text_line(&p->fonts[0],outcomes[outcome]+at,len-at,impact_width,false);if(n<=0)n=1;nk_draw_text(nk_window_get_canvas(ui),nk_rect(impact.x+13*scale,y,impact_width,p->fonts[0].height),outcomes[outcome]+at,n,&p->fonts[0],nk_rgba(0,0,0,0),UI_COLOR(text));at+=n;y+=19*scale;}
    ps_ui_accessibility_label(ui,impact,outcomes[outcome]);nk_layout_row_dynamic(ui,17*scale,1);nk_spacer(ui);
    if(*m->source){char base[2064];protocol_preview(diagnostic_basename(m->source),base);snprintf(text,sizeof text,"%.2000s:%u:%u",base,m->line,m->column);}
    else protocol_preview(*m->context?diagnostic_basename(m->context):"Nicht übermittelt",text);
    protocol_property(a,"Kontext",text,scale);
    protocol_property(a,"Quellposition",*m->source?text:"Nicht übermittelt",scale);
    if(m->has_time){snprintf(text,sizeof text,"%.17g s",m->time_s);protocol_property(a,"Modellzeit",text,scale);}
    if(m->has_code){snprintf(text,sizeof text,"Fehlercode: %s",ps_result_string(m->code));protocol_wrap(a,text,&a->protocol_view.fonts[1],18*scale,muted());}
    if(*m->operation){protocol_preview(m->operation,text);protocol_wrap(a,text,&a->protocol_view.fonts[1],18*scale,muted());}
    if(*m->argument){protocol_preview(m->argument,text);protocol_wrap(a,text,&a->protocol_view.fonts[1],18*scale,muted());}
    if(record->omitted_bytes){snprintf(text,sizeof text,"Unvollständig: %llu Originalbytes ausgelassen.",(unsigned long long)record->omitted_bytes);protocol_wrap(a,text,&a->protocol_view.fonts[0],19*scale,UI_COLOR(error));}
    if(m->transcript_id){nk_layout_row_dynamic(ui,30*scale,1);p->transcript_toggle=nk_widget_bounds(ui);if(nk_button_label(ui,p->transcript?"Einzelne Diagnose":"Vollständige Prozessausgabe")){p->transcript=!p->transcript;p->original_id=0;}}
    if(protocol_disclosure(a,"Originalmeldung",!p->original_closed,scale,&p->original_toggle))p->original_closed=!p->original_closed;
    bool original_ready=protocol_original(a,record);
    if(original_ready && !p->original_closed) {
        size_t remaining=p->original_length-p->page,n=remaining>12288?12288:remaining;
        if(n<remaining)while(n && ((unsigned char)p->original[p->page+n]&0xc0)==0x80)n--;
        memcpy(p->original_page,p->original+p->page,n);p->original_page[n]=0;
        nk_style_push_font(ui,&a->protocol_view.fonts[6]);
        const struct nk_user_font *font=ui->style.font;
        float width=fmaxf(1,nk_window_get_content_region(ui).w-ui->style.edit.cursor_size);
        float line_height=11.2f*1.85f*scale; /* Approved .8rem at a 14px root. */
        bool same=p->original_layout && p->original_layout->bytes==n && !memcmp(p->original_layout->source,p->original_page,n);
        const struct nk_user_font *cached=&p->original_layout_font;
        bool metrics=same && p->original_layout->width==width && p->original_layout->visual.row_height==line_height &&
            cached->height==font->height && cached->width==font->width && cached->userdata.ptr==font->userdata.ptr;
        if(!metrics) {
            ps_text_wrap *layout=NULL;
            if(ps_text_wrap_build(p->original_page,n,width,line_height,protocol_original_measure,(void*)font,&layout)) {
                ps_text_wrap_destroy(p->original_layout);p->original_layout=layout;p->original_layout_font=*font;
                if(!same){p->original_edit.cursor=p->original_edit.select_start=p->original_edit.select_end=0;p->original_edit.scrollbar=nk_vec2(0,0);}
            } else {
                ps_text_wrap_destroy(p->original_layout);p->original_layout=NULL;p->original_edit.visual=NULL;
            }
        }
        nk_layout_row_dynamic(ui,11.2f*scale,1);nk_spacer(ui); /* pre's 1em block margin */
        if(p->original_layout) {
            const struct nk_text_visual *v=&p->original_layout->visual;
            nk_layout_row_dynamic(ui,v->row_count*v->row_height,1);p->original_bounds=nk_widget_bounds(ui);
            struct nk_rect clip=ui->current->layout->clip,r=p->original_bounds;
            float x=fmaxf(r.x,clip.x),y=fmaxf(r.y,clip.y);
            p->original_viewport=nk_rect(x,y,fmaxf(0,fminf(r.x+r.w,clip.x+clip.w)-x),fmaxf(0,fminf(r.y+r.h,clip.y+clip.h)-y));
            ps_ui_text_reveal request;
            nk_sdl_edit_named_visual_flow(ui,"Originalmeldung",p->original_page,sizeof p->original_page,&p->original_edit,v,&request);
            /* Apply at the next group begin: changing its offset while the
             * current frame is being laid out would split text/action geometry. */
            protocol_original_reveal(p,clip,request);
        } else {
            nk_layout_row_dynamic(ui,40*scale,1);nk_label_wrap(ui,"Originaltext kann nicht dargestellt werden. Meldung kopieren oder TXT speichern.");
        }
        nk_layout_row_dynamic(ui,11.2f*scale,1);nk_spacer(ui);
        nk_style_pop_font(ui);
        if(p->original_length>n) {
            nk_layout_row_dynamic(ui,28*scale,2);nk_begin_disabled(ui,!p->page);
            if(nk_button_label(ui,"Vorheriger Textabschnitt")){p->page=p->page>12288?p->page-12288:0;while(p->page && ((unsigned char)p->original[p->page]&0xc0)==0x80)p->page--;}
            nk_end_disabled(ui);nk_begin_disabled(ui,p->page+n==p->original_length);
            if(nk_button_label(ui,"Nächster Textabschnitt"))p->page+=n;nk_end_disabled(ui);
            snprintf(text,sizeof text,"Textbytes %zu–%zu von %zu; Kopieren und TXT umfassen den ganzen Text.",p->page,p->page+n,p->original_length);protocol_wrap(a,text,&a->protocol_view.fonts[1],18*scale,muted());
        }
        if(p->original_length!=p->original_size)protocol_wrap(a,"Nicht darstellbare Bytes werden als \\xHH angezeigt.",&a->protocol_view.fonts[1],18*scale,muted());
    }
    protocol_actions(a,record,original_ready,scale);
    if(protocol_disclosure(a,"Weitere Angaben",p->details_more,scale,NULL))p->details_more=!p->details_more;
    if(p->details_more) {
        const char *labels[]={"Vollständiger Kontext","Projekt","Übermittelte Quelle"},*values[]={m->context,m->project,m->source};
        for(unsigned i=0;i<3;i++)if(*values[i]){size_t length=0;char *full=protocol_text_copy(values[i],strlen(values[i]),&length);protocol_wrap(a,labels[i],&p->fonts[1],18*scale,muted());if(full){protocol_wrap(a,full,&p->fonts[0],19*scale,UI_COLOR(text));free(full);}}
    }
}
static void protocol_panel(app *a) {
    struct nk_context *ui=a->ui;ps_protocol_ui *p=&a->protocol_view;float f=a->preferences.ui_size/16.f;
    const ps_ui_palette *previous_palette=ui_palette;ps_ui_palette palette=*ui_palette;
    if(a->preferences.theme!=PS_THEME_HIGH_CONTRAST) {
        bool light=a->preferences.theme==PS_THEME_LIGHT;
        palette.window=light?nk_rgb(250,251,253):nk_rgb(32,35,42);
        palette.header=palette.button=light?nk_rgb(240,243,247):nk_rgb(39,43,51);
        palette.edit=light?nk_rgb(255,255,255):nk_rgb(24,27,33);
        palette.border=light?nk_rgb(198,207,221):nk_rgb(60,65,76);
        palette.text=light?nk_rgb(32,44,60):nk_rgb(240,242,247);
        palette.muted=light?nk_rgb(88,104,124):nk_rgb(169,178,193);
        palette.accent=light?nk_rgb(32,92,170):nk_rgb(154,195,255);
        palette.error=light?nk_rgb(169,49,56):nk_rgb(255,170,165);
        palette.selection_active=palette.button_hover=light?nk_rgb(220,233,250):nk_rgb(41,62,91);
        ui_palette=&palette;style(ui);
    }
    const float sizes[]={11.2f,9.8f,11.62f,17.5f,13.3f,10.92f,11.2f,9.8f,10.5f,9.52f,14};
    for(unsigned i=0;i<11;i++){bool bold=(i>=2 && i<=4) || i==7;p->fonts[i]=*(i==6?a->font_code:bold?a->font_title:a->font_ui);p->fonts[i].height=sizes[i]*(i==6?a->workbench_code_em:bold?a->workbench_bold_em:a->workbench_regular_em)*f;}
    ps_protocol_summary summary=ps_protocol_describe(&a->protocol);
    if(!p->initialized){p->initialized=true;p->origin=-1;p->follow=false;if(summary.count)p->last_seen=ps_protocol_at(&a->protocol,summary.count-1)->id;}
    p->title_font=p->fonts[3];
    const ps_protocol_record *records[PS_PROTOCOL_CAPACITY];size_t count=0;
    for(size_t i=0;i<summary.count;i++){const ps_protocol_record *r=ps_protocol_at(&a->protocol,i);if(protocol_matches(p,&r->message))records[count++]=r;}
    const ps_protocol_record *selected=NULL;for(size_t i=0;i<count;i++)if(records[i]->id==p->selected)selected=records[i];
    if(!selected && count){selected=records[0];if(!p->selected || !ps_protocol_find(&a->protocol,p->selected))p->selected=selected->id;}
    if(p->follow && count){selected=records[count-1];p->selected=selected->id;p->last_seen=selected->id;}
    struct nk_rect bounds=a->dock_panels[PS_DOCK_LOG];p->bounds=bounds;p->row_count=0;p->source=p->copy=p->transcript_toggle=p->original_toggle=nk_rect(0,0,0,0);
    float width=bounds.w,inner=fmaxf(1,width-36*f);bool wide=width>1150*f;
    const unsigned order[]={0,PS_LOG_ERROR,PS_LOG_WARNING,PS_LOG_INFO,PS_LOG_DEBUG};
    float filter_widths[5],filter_offsets[5],filter_rows[5],filter_total=0,filter_x=0,filter_y=0;
    for(unsigned i=0;i<5;i++) {
        unsigned level=order[i];char number[24];snprintf(number,sizeof number,"%zu",level?summary.levels[level]:summary.count);
        filter_widths[i]=p->fonts[8].width(p->fonts[8].userdata,p->fonts[8].height,protocol_levels[level],(int)strlen(protocol_levels[level]))+
            p->fonts[9].width(p->fonts[9].userdata,p->fonts[9].height,number,(int)strlen(number))+(25.2f+(level?22.f:0))*f;
        filter_total+=filter_widths[i]+(i?6*f:0);
    }
    bool toolbar_wide=wide && width*.32f+262*f<=width-48*f-filter_total;
    for(unsigned i=0;i<5;i++) {
        if(!toolbar_wide && filter_x>0 && filter_x+filter_widths[i]>inner-ui->style.window.scrollbar_size.x){filter_x=0;filter_y+=38*f;}
        filter_offsets[i]=filter_x;filter_rows[i]=filter_y;filter_x+=filter_widths[i]+6*f;
    }
    float header=toolbar_wide?68*f:136*f+filter_y;
    float body=wide?fmaxf(96*f,bounds.h-header-28*f):fmaxf(240*f,bounds.h-header-28*f);
    bool stacked=!wide,vertical=width<850*f;float content=header+(stacked?body+420*f+(vertical?260*f:0):body)+28*f;
    nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(0,0));nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,0));
    nk_style_push_vec2(ui,&ui->style.window.group_padding,nk_vec2(18*f,18*f));nk_style_push_float(ui,&ui->style.window.rounding,0);
    nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(UI_COLOR(window)));nk_style_push_font(ui,&a->protocol_view.fonts[0]);
    struct nk_window *previous=ui->active;nk_window_show(ui,"log",NK_SHOWN);
    if(nk_begin(ui,"log",bounds,NK_WINDOW_BACKGROUND|(content<=bounds.h+1?NK_WINDOW_NO_SCROLLBAR:0))) {
        /* A stacked layout can have an outer offset larger than this entire
         * panel. Once all three columns fit, no outer scrollbar can recover
         * that offset. Inner text/list scroll positions remain independent. */
        if(content<=bounds.h+1)nk_window_set_scroll(ui,0,0);
        dock_input(a,PS_DOCK_LOG,bounds,previous,true);
        a->panels[PS_DOCK_LOG].restore_scroll=false; /* Each responsive window retains its own outer scroll. */
        if(!a->panel_render && panel_is_detached(a,PS_DOCK_LOG))goto end;
        float available=nk_window_get_content_region(ui).w;
        nk_layout_space_begin(ui,NK_STATIC,header,12);
        struct nk_rect title=protocol_space(ui,nk_rect(18*f,12*f,fmaxf(120*f,available*.30f),22*f));panel_pull_handle(a,PS_DOCK_LOG,title);
        protocol_text_at(a,&p->fonts[4],title,"Protokoll",UI_COLOR(text));
        char text[256];snprintf(text,sizeof text,"%zu Fehler · %zu Warnungen · %zu/%zu sichtbar",summary.levels[PS_LOG_ERROR],summary.levels[PS_LOG_WARNING],count,summary.count);
        protocol_text_at(a,&p->fonts[5],nk_rect(title.x,title.y+23*f,fmaxf(1,title.w),18*f),text,muted());
        float sx=toolbar_wide?available*.32f:18*f,sy=toolbar_wide?18*f:54*f;
        struct nk_rect search=protocol_space(ui,nk_rect(sx+40*f,sy,toolbar_wide?145*f:fmaxf(100*f,available-190*f),32*f));p->search=search;
        protocol_text_at(a,&p->fonts[5],nk_rect(search.x-40*f,search.y+8*f,38*f,18*f),"Suchen",muted());
        nk_layout_space_push(ui,nk_rect(sx+40*f,sy,search.w,search.h));nk_flags edit=nk_sdl_edit_named(ui,"Meldung oder Quelle",NK_EDIT_FIELD,p->query,sizeof p->query,nk_filter_default);
        if(!*p->query && !(edit&NK_EDIT_ACTIVE))protocol_text_at(a,&p->fonts[5],nk_rect(search.x+10*f,search.y+8*f,search.w-18*f,18*f),"Meldung oder Quelle",muted());
        struct nk_rect store=protocol_space(ui,nk_rect(sx+40*f+search.w+6*f,sy,56*f,32*f));p->store=store;
        nk_fill_rect(nk_window_get_canvas(ui),store,6,UI_COLOR(button));nk_stroke_rect(nk_window_get_canvas(ui),store,6,1,UI_COLOR(border));protocol_text_at(a,&p->fonts[5],nk_rect(store.x+10*f,store.y+8*f,store.w-12*f,20*f),"TXT ↓",count && !p->save?UI_COLOR(text):muted());
        nk_begin_disabled(ui,!count || p->save);if(ps_ui_button_at(ui,store,"Gefiltertes Protokoll als TXT speichern",false))protocol_export(a);nk_end_disabled(ui);
        float filters_x=toolbar_wide?available-48*f-filter_total:18*f,filters_y=toolbar_wide?18*f:92*f;
        for(unsigned i=0;i<5;i++) {
            unsigned level=order[i];snprintf(text,sizeof text,"%s %zu",protocol_levels[level],level?summary.levels[level]:summary.count);
            struct nk_rect r=protocol_space(ui,nk_rect(filters_x+filter_offsets[i],filters_y+filter_rows[i],filter_widths[i],32*f));p->levels[i]=r;
            bool active=p->level==(int)level;if(active)nk_fill_rect(nk_window_get_canvas(ui),r,6,UI_COLOR(header));
            if(active)nk_stroke_rect(nk_window_get_canvas(ui),r,6,1,UI_COLOR(border));
            float label_x=r.x+10.1f*f;
            if(level){protocol_symbol(nk_window_get_canvas(ui),nk_rect(label_x,r.y+8*f,22*f,16*f),(ps_log_level)level,protocol_level_color((ps_log_level)level));label_x+=22*f;}
            float label_width=p->fonts[8].width(p->fonts[8].userdata,p->fonts[8].height,protocol_levels[level],(int)strlen(protocol_levels[level]));
            protocol_text_at(a,&p->fonts[8],nk_rect(label_x,r.y+(r.h-p->fonts[8].height)/2,label_width+1,p->fonts[8].height),protocol_levels[level],UI_COLOR(text));
            char number[24];snprintf(number,sizeof number,"%zu",level?summary.levels[level]:summary.count);
            protocol_text_at(a,&p->fonts[9],nk_rect(label_x+label_width+5*f,r.y+(r.h-p->fonts[9].height)/2,r.w-label_width-10*f,p->fonts[9].height),number,muted());
            if(ps_ui_option_at(ui,r,"Protokoll-Schweregrad",text,active)){p->level=(int)level;p->scroll_y=0;}
        }
        p->close=protocol_space(ui,nk_rect(available-34*f,8*f,28*f,28*f));ps_ui_draw_icon(nk_window_get_canvas(ui),PS_ICON_CLOSE,p->close,muted());
        if(ps_ui_button_at(ui,p->close,a->panel_render?"Protokoll zurück in den Arbeitsbereich":"Protokoll schließen",false)) {
            if(a->panel_render)a->panel_return_request|=1u<<PS_DOCK_LOG;else protocol_close(a);
        }
        nk_layout_space_end(ui);workspace_rule(nk_window_get_canvas(ui),nk_rect(bounds.x,bounds.y+header-1,available,1));
        float left=(vertical?available:(wide?190:175)*f),right=310*f,middle=fmaxf(1,available-left-(wide?right:0));
        nk_layout_row_begin(ui,NK_STATIC,vertical?260*f:body,vertical?1:wide?3:2);nk_layout_row_push(ui,left);
        p->panes[0]=nk_widget_bounds(ui);
        if(nk_group_begin(ui,"Protokoll-Herkunft",0)) {
            nk_layout_row_dynamic(ui,20*f,1);nk_label_colored(ui,"HERKUNFT",NK_TEXT_LEFT,muted());nk_layout_row_dynamic(ui,14*f,1);nk_spacer(ui);
            memset(p->origins,0,sizeof p->origins);
            for(unsigned i=0;i<4;i++) {
                int origin=i?(int)i:-1;size_t amount=i?summary.origins[origin]:summary.count;snprintf(text,sizeof text,"%s %zu",i?protocol_origins[origin]:"Alle Bereiche",amount);
                nk_layout_row_dynamic(ui,38*f,1);struct nk_rect r;nk_widget(&r,ui);p->origins[i]=r;
                bool active=p->origin==origin;if(active){nk_fill_rect(nk_window_get_canvas(ui),r,7,protocol_selection());nk_stroke_rect(nk_window_get_canvas(ui),r,7,1,UI_COLOR(border));}
                protocol_origin_symbol(nk_window_get_canvas(ui),nk_rect(r.x+10*f,r.y+10*f,18*f,18*f),origin,UI_COLOR(text));
                protocol_text_at(a,&p->fonts[0],nk_rect(r.x+37*f,r.y+11*f,r.w-59*f,20*f),i?protocol_origins[origin]:"Alle Bereiche",UI_COLOR(text));
                char number[24];snprintf(number,sizeof number,"%zu",amount);float number_width=p->fonts[1].width(p->fonts[1].userdata,p->fonts[1].height,number,(int)strlen(number));
                protocol_text_at(a,&p->fonts[1],nk_rect(r.x+r.w-11*f-number_width,r.y+12*f,number_width+1,18*f),number,muted());
                if(ps_ui_option_at(ui,r,"Protokoll-Herkunft",text,active)){p->origin=origin;p->scroll_y=0;}
            }
            nk_layout_row_dynamic(ui,18*f,1);nk_spacer(ui);protocol_wrap(a,"Fehlertexte bleiben unverändert. Der tatsächliche Laufzustand bestimmt, ob ein Lauf abbricht.",&a->protocol_view.fonts[1],20*f,muted());
            nk_group_end(ui);
        }
        if(vertical){nk_layout_row_end(ui);nk_layout_row_dynamic(ui,body,1);}else nk_layout_row_push(ui,middle);
        nk_style_push_vec2(ui,&ui->style.window.group_padding,nk_vec2(8*f,8*f));
        p->panes[1]=nk_widget_bounds(ui);
        if(p->follow && count) {
            float total=0,rw=fmaxf(1,p->panes[1].w-16*f-42*f);
            for(size_t i=0;i<count;i++){char preview[2064];protocol_preview(records[i]->message.title,preview);total+=(ps_ui_text_line(&a->protocol_view.fonts[2],preview,(int)strlen(preview),rw,false)<(int)strlen(preview)?82:63)*f;}
            p->scroll_y=(nk_uint)fmaxf(0,total-body+16*f);
        }
        p->rendered_scroll_y=p->scroll_y;
        if(nk_group_scrolled_offset_begin(ui,&p->scroll_x,&p->scroll_y,"Protokollmeldungen",0)) {
            p->list_width=nk_window_get_content_region(ui).w;
            if(!count)protocol_wrap(a,summary.count?"Keine Meldungen entsprechen den Filtern.":"Noch keine Meldungen in dieser Sitzung.",&a->protocol_view.fonts[0],22*f,muted());
            for(size_t i=0;i<count;i++) {
                const ps_protocol_record *r=records[i];char preview[2064],context[2064],name[2300];protocol_preview(r->message.title,preview);protocol_preview(r->message.context,context);
                float rw=nk_window_get_content_region(ui).w;int first=ps_ui_text_line(&a->protocol_view.fonts[2],preview,(int)strlen(preview),fmaxf(1,rw-42*f),false);
                bool two=first<(int)strlen(preview);float rh=(two?82:63)*f;
                nk_layout_row_dynamic(ui,rh,1);struct nk_rect row;nk_widget(&row,ui);p->rows[i]=row;p->row_ids[i]=r->id;p->row_count++;
                bool active=selected && r->id==selected->id;if(active){nk_fill_rect(nk_window_get_canvas(ui),row,6,protocol_selection());nk_stroke_rect(nk_window_get_canvas(ui),row,6,1,UI_COLOR(border));}
                protocol_symbol(nk_window_get_canvas(ui),nk_rect(row.x+7*f,row.y+14*f,20*f,20*f),r->message.level,protocol_level_color(r->message.level));
                protocol_text_at(a,&a->protocol_view.fonts[2],nk_rect(row.x+34*f,row.y+12*f,row.w-42*f,20*f),preview,UI_COLOR(text));
                if(two && first>0)protocol_text_at(a,&a->protocol_view.fonts[2],nk_rect(row.x+34*f,row.y+31*f,row.w-42*f,20*f),preview+first,UI_COLOR(text));
                char subtitle[2300];
                if(*r->message.source){protocol_preview(diagnostic_basename(r->message.source),context);snprintf(subtitle,sizeof subtitle,"%s · %s:%u:%u",protocol_level_name(r->message.level),context,r->message.line,r->message.column);}
                else if(r->message.has_time)snprintf(subtitle,sizeof subtitle,"%s · %s · t = %.4g s",protocol_level_name(r->message.level),*context?diagnostic_basename(context):protocol_origins[r->message.origin],r->message.time_s);
                else snprintf(subtitle,sizeof subtitle,"%s · %s",protocol_level_name(r->message.level),*context?diagnostic_basename(context):protocol_origins[r->message.origin]);
                protocol_text_at(a,&a->protocol_view.fonts[1],nk_rect(row.x+34*f,row.y+(two?56:37)*f,row.w-42*f,18*f),subtitle,muted());
                protocol_row_name(r,name);
                if(ps_ui_option_at(ui,row,"Protokollmeldungen",name,active)){p->selected=r->id;p->transcript=false;p->original_id=0;p->follow=false;selected=r;}
            }
            nk_group_end(ui);
        }
        nk_style_pop_vec2(ui);
        if(wide)nk_layout_row_push(ui,right);
        else {if(!vertical)nk_layout_row_end(ui);nk_layout_row_dynamic(ui,420*f,1);}
        nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(UI_COLOR(edit)));
        p->panes[2]=nk_widget_bounds(ui);nk_fill_rect(nk_window_get_canvas(ui),p->panes[2],0,UI_COLOR(edit));
        p->original_viewport=nk_rect(0,0,0,0);
        uint64_t detail_record=selected?selected->id:0;
        if(p->detail_record!=detail_record || p->detail_page!=p->page || p->detail_transcript!=p->transcript) {
            p->detail_record=detail_record;p->detail_page=p->page;p->detail_transcript=p->transcript;
            p->detail_scroll_y=0;p->detail_reveal_pending=false;
        } else if(p->detail_reveal_pending){p->detail_scroll_y=p->detail_scroll_target;p->detail_reveal_pending=false;}
        nk_style_push_vec2(ui,&ui->style.window.group_padding,nk_vec2(20*f,20*f));
        if(nk_group_scrolled_offset_begin(ui,&p->detail_scroll_x,&p->detail_scroll_y,"Protokolldetails",0)){protocol_detail(a,selected,f);nk_group_end(ui);}
        nk_style_pop_vec2(ui);
        nk_style_pop_style_item(ui);if(wide)nk_layout_row_end(ui);
        if(!vertical)workspace_rule(nk_window_get_canvas(ui),nk_rect(p->panes[1].x,p->panes[1].y,1,p->panes[1].h));
        if(wide)workspace_rule(nk_window_get_canvas(ui),nk_rect(p->panes[2].x,p->panes[2].y,1,p->panes[2].h));
        nk_layout_row_dynamic(ui,28*f,1);struct nk_rect footer;nk_widget(&footer,ui);workspace_rule(nk_window_get_canvas(ui),nk_rect(footer.x,footer.y,footer.w,1));
        snprintf(text,sizeof text,"%zu/%zu sichtbar · %llu ausgelassene Meldungen · %llu ausgelassene Bytes",count,summary.count,(unsigned long long)summary.omitted_messages,(unsigned long long)summary.omitted_bytes);
        size_t fresh=0;for(size_t i=0;i<summary.count;i++)if(ps_protocol_at(&a->protocol,i)->id>p->last_seen)fresh++;
        float notice=fresh && !p->follow?230*f:0;
        protocol_text_at(a,&a->protocol_view.fonts[1],nk_rect(footer.x+18*f,footer.y+7*f,fmaxf(1,footer.w-36*f-notice),18*f),*p->feedback?p->feedback:text,muted());
        if(notice){char label[96];snprintf(label,sizeof label,"%zu neue Meldungen · Zum Ende",fresh);if(ps_ui_button_at(ui,nk_rect(footer.x+footer.w-notice,footer.y,notice,footer.h),label,false))p->follow=true;protocol_text_at(a,&a->protocol_view.fonts[1],nk_rect(footer.x+footer.w-notice+5*f,footer.y+7*f,notice-10*f,18*f),label,UI_COLOR(accent));}
        if(p->navigation_request && p->navigation_ui==ui){const ps_protocol_record *target=ps_protocol_find(&a->protocol,p->navigation_request);char name[2300];if(target){protocol_row_name(target,name);if(nk_sdl_accessibility_request_focus(ui,NULL,name))p->navigation_request=0;}else p->navigation_request=0;}
    }
end:
    nk_end(ui);nk_style_pop_font(ui);nk_style_pop_style_item(ui);nk_style_pop_float(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
    ui_palette=previous_palette;style(ui);
    if(p->source_request){uint64_t id=p->source_request;p->source_request=0;protocol_jump(a,id);}
}
static void protocol_separator_view(app *a) {
    struct nk_context *ui=a->ui;bool visible=protocol_visible(a);nk_window_show(ui,"protocol-separator",visible?NK_SHOWN:NK_HIDDEN);if(!visible)return;
    int w,h;SDL_GetWindowSize(a->window,&w,&h);struct nk_rect bounds=protocol_bounds(a,(float)w,(float)h),r=nk_rect(0,bounds.y-4,(float)w,9);a->protocol_view.separator=r;
    nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(0,0));nk_style_push_float(ui,&ui->style.window.rounding,0);
    nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(nk_rgba(0,0,0,0)));struct nk_window *previous=ui->active;
    if(nk_begin(ui,"protocol-separator",r,NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BACKGROUND)) {
        nk_sdl_window_raise(ui,"protocol-separator");
        bool blocked=a->toolbar_menu || a->toolbar_pointer_consumed;
        ps_ui_accessibility_route(ui,!blocked);ui->active=previous;float value=protocol_height_value(a,(float)h);
        if(ps_ui_separator_at(ui,r,"Protokoll-Höhe",150,&value,protocol_max_height(a,(float)h))) {
            if(SDL_getenv("PHYSIM_TEST_TRACE"))fprintf(stderr,"PROTOCOL RANGE APPLY: previous=%u value=%.3f\n",a->preferences.log_height,value);
            a->preferences.log_height=(uint32_t)lroundf(value);
        }
        if(!blocked && nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT) && nk_input_is_mouse_hovering_rect(&ui->input,r))nk_sdl_accessibility_request_focus(ui,NULL,"Protokoll-Höhe");
        bool hover=nk_input_is_mouse_hovering_rect(&ui->input,r);if(hover || nk_sdl_accessibility_is_focused(ui,NULL,"Protokoll-Höhe"))nk_fill_rect(nk_window_get_canvas(ui),nk_rect(r.x,r.y+4,r.w,1),0,UI_COLOR(accent));
        if(hover || a->protocol_view.resize){if(!a->laboratory_cursors[1])a->laboratory_cursors[1]=SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NS_RESIZE);if(a->laboratory_cursors[1]){SDL_SetCursor(a->laboratory_cursors[1]);a->protocol_view.cursor_owned=true;}}
    }nk_end(ui);nk_style_pop_style_item(ui);nk_style_pop_float(ui);nk_style_pop_vec2(ui);nk_sdl_window_raise(ui,"protocol-separator");
}
