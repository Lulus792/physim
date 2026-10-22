/* Native ADR 010 dialog. Owns the opening snapshot, output and source context.
 * OS callbacks receive immutable CPU bytes, never this app or GL owner. */
typedef struct graphic_dialog {
    ps_graphic_snapshot *snapshot;ps_graphic_raster *raster;ps_graphic_save *save;
    SDL_Window *window;ps_graphics *graphics;struct nk_context *ui;
    ps_graphic_description description;struct nk_user_font fonts[8];
    char source[64],directory[4096],message[256],filename[192],saved_path[8192];
    unsigned scale,focus_target,menu_field;float factor,content_height,scale_override;bool svg,visible,appearance,dark,contrast,dirty,close,first;
    struct nk_rect bounds,paper,formats[2],fields[3],choices[3][4],copy,store,exit;
} graphic_dialog;
typedef struct {struct nk_input input;ps_ui_a11y_hook hook;bool held;} graphic_input_guard;
static graphic_input_guard graphic_export_guard(app *a) {
    graphic_input_guard guard={0};if(!a->graphic_export)return guard;
    guard.held=true;guard.input=a->ui->input;
    ps_ui_font_layout *layout=a->ui->userdata.ptr;guard.hook=layout->accessibility;layout->accessibility=NULL;
    memset(&a->ui->input,0,sizeof a->ui->input);a->ui->input.mouse.pos=nk_vec2(-10000,-10000);
    return guard;
}
static void graphic_export_unguard(app *a,graphic_input_guard guard) {
    if(!guard.held)return;
    a->ui->input=guard.input;
    ((ps_ui_font_layout *)a->ui->userdata.ptr)->accessibility=guard.hook;
}
static void graphic_export_close(app *a) {
    graphic_dialog *d=a->graphic_export;if(!d)return;
    ps_graphics_make_current(d->graphics);ps_graphic_raster_destroy(d->raster);
    ps_graphic_snapshot_destroy(d->snapshot);ps_graphic_save_destroy(d->save);
    nk_window_close(d->ui,"graphic-export");nk_window_close(d->ui,"graphic-export-backdrop");
    nk_window_set_focus(d->ui,d->source);
    a->graphic_return_ui=d->ui;a->graphic_return_frames=3;
    a->graphic_export=NULL;free(d);ps_graphics_make_current(a->graphics);
}
static bool graphic_export_report(app *a,uint32_t index) {
    if(a->graphic_export)return false;
    graphic_dialog *d=calloc(1,sizeof *d);if(!d){status(a,"Grafikvorschau: nicht genügend Speicher.");return false;}
    ps_graphic_axes axes={0};ps_result result=ps_report_plot_bounds(a->analysis_report,index,axes.full);
    if(result==PS_OK) {
        plot_display_bounds(&a->report_views[index],axes.full,axes.visible);
        axes.cropped=ps_plot_view_changed(&a->report_views[index]);
        result=ps_graphic_snapshot_from_report(a->analysis_report,index,&axes,PS_GRAPHIC_NO_REFERENCE,
                                             ps_allocator_default(),&d->snapshot);
    }
    if(result!=PS_OK){free(d);status(a,"Dieses Diagramm kann nicht als Grafik dargestellt werden.");return false;}
    ps_graphic_snapshot_describe(d->snapshot,&d->description);
    d->window=a->window;d->graphics=a->graphics;d->ui=a->ui;d->dark=a->preferences.theme!=PS_THEME_LIGHT;
    d->contrast=a->preferences.theme==PS_THEME_HIGH_CONTRAST;
    snprintf(d->source,sizeof d->source,"%s",a->ui->current?a->ui->current->name_string:"workspace");
    snprintf(d->directory,sizeof d->directory,"%s",a->project);d->scale=2;d->dirty=d->first=true;
    snprintf(d->message,sizeof d->message,"PNG oder SVG direkt aus dieser Vorschau speichern.");
    d->focus_target=2;a->graphic_export=d;a->plot_drag=NULL;completion_close(a);toolbar_keyboard_close(a);
    return true;
}
static void graphic_export_tick(app *a) {
    if(a->graphic_return_ui) {
        if(nk_sdl_accessibility_request_focus(a->graphic_return_ui,NULL,"Grafik exportieren…") || !--a->graphic_return_frames)
            a->graphic_return_ui=NULL;
    }
    graphic_dialog *d=a->graphic_export;if(!d)return;
    if(d->close){graphic_export_close(a);return;}
    if(!d->save)return;
    ps_graphic_save_state state=ps_graphic_save_poll(d->save);
    if(state==PS_GRAPHIC_SAVE_WAITING || state==PS_GRAPHIC_SAVE_WRITING)return;
    snprintf(d->message,sizeof d->message,"%s",state==PS_GRAPHIC_SAVE_OK?"Grafik gespeichert.":
        state==PS_GRAPHIC_SAVE_CANCELLED?"Speichern abgebrochen.":"Grafik konnte nicht gespeichert werden. Ziel und Berechtigungen prüfen.");
    if(state==PS_GRAPHIC_SAVE_OK) {
        snprintf(a->result_export,sizeof a->result_export,"%s",ps_graphic_save_path(d->save));
        log_line(a,"Grafik gespeichert: %s\n",a->result_export);
    }
    ps_graphic_save_destroy(d->save);d->save=NULL;
}
static bool graphic_export_event(app *a,const SDL_Event *event,SDL_Window *window) {
    graphic_dialog *d=a->graphic_export;if(!d)return false;
    if(event->type==SDL_EVENT_QUIT || (event->type==SDL_EVENT_WINDOW_CLOSE_REQUESTED && window==a->window)) {
        graphic_export_close(a);return false;
    }
    if(window==d->window && event->type==SDL_EVENT_WINDOW_CLOSE_REQUESTED){d->close=true;return true;}
    bool input=event->type==SDL_EVENT_KEY_DOWN || event->type==SDL_EVENT_KEY_UP ||
        event->type==SDL_EVENT_TEXT_INPUT || event->type==SDL_EVENT_TEXT_EDITING ||
        event->type==SDL_EVENT_MOUSE_MOTION || event->type==SDL_EVENT_MOUSE_BUTTON_DOWN ||
        event->type==SDL_EVENT_MOUSE_BUTTON_UP || event->type==SDL_EVENT_MOUSE_WHEEL;
    if(!input)return false;
    if(window==d->window) {
        struct nk_window *w=nk_window_find(d->ui,"graphic-export");
        if(event->type==SDL_EVENT_KEY_DOWN && event->key.key==SDLK_TAB) {
            const char *names[]={"Grafikexport schließen","PNG","SVG","Zeitbereich","Darstellung","PNG-Größe","Bild kopieren",d->svg?"SVG speichern…":"PNG speichern…"};
            struct nk_rect targets[]={d->exit,d->formats[0],d->formats[1],d->fields[0],d->fields[1],d->fields[2],d->copy,d->store};
            int current=1;
            for(unsigned i=0;i<8;i++)if(nk_sdl_accessibility_is_focused(d->ui,i==1 || i==2?"Dateiformat":NULL,names[i]))current=(int)i;
            if(w && w->popup.active) {
                current=(int)d->menu_field+2;w->popup.active=false;
                if(w->popup.win)w->popup.win->flags|=NK_WINDOW_HIDDEN;
            }
            int direction=event->key.mod&SDL_KMOD_SHIFT?-1:1,index=(current+direction+8)%8;
            if(d->svg && index==5)index=(index+direction+8)%8;
            d->focus_target=(unsigned)index+1;
            if(SDL_getenv("PHYSIM_TEST_TRACE"))fprintf(stderr,"GRAPHIC DIALOG FOCUS: tab=%d target=%s\n",direction,names[index]);
            if(w) {
                struct nk_rect target=targets[index];float low=d->bounds.y+8,high=d->bounds.y+d->bounds.h-14;
                float offset=(float)w->scrollbar.y;
                if(target.y<low)offset+=target.y-low;
                else if(target.y+target.h>high)offset+=target.y+target.h-high;
                w->scrollbar.y=(nk_uint)fmaxf(0,fminf(fmaxf(0,d->content_height-d->bounds.h+16),offset));
            }
            return true;
        }
        if(event->type==SDL_EVENT_KEY_DOWN && event->key.key==SDLK_ESCAPE) {
            if(w && w->popup.active){w->popup.active=false;if(w->popup.win)w->popup.win->flags|=NK_WINDOW_HIDDEN;}
            else d->close=true;
            return true;
        }
        if(event->type==SDL_EVENT_KEY_UP && event->key.key==SDLK_TAB)return true;
        if(event->type==SDL_EVENT_MOUSE_BUTTON_DOWN)d->focus_target=0;
        if(!nk_sdl_accessibility_event(d->ui,event)){SDL_Event copy=*event;nk_sdl_handle_event(d->ui,&copy);}
    }
    return true;
}
static void graphic_export_filename(graphic_dialog *d) {
    char slug[128];size_t n=0;const char *title=d->description.plot.title;
    for(size_t i=0;title[i] && n<sizeof slug-1;i++) {
        unsigned char c=(unsigned char)title[i];
        if(c>='A' && c<='Z')c=(unsigned char)(c-'A'+'a');
        if((c>='a' && c<='z') || (c>='0' && c<='9'))slug[n++]=(char)c;
        else if(n && slug[n-1]!='-')slug[n++]='-';
    }
    while(n && slug[n-1]=='-')n--;
    slug[n]=0;
    snprintf(d->filename,sizeof d->filename,"physim-%s%s.%s",n?slug:"diagramm",d->visible?"-ausschnitt":"",d->svg?"svg":"png");
}
static void graphic_export_range(char out[160],double low,double high) {
    char first[64],last[64];snprintf(first,sizeof first,"%.3f",low);snprintf(last,sizeof last,"%.3f",high);
    if(fmax(fabs(low),fabs(high))>=1e9 || high-low<.001 || !strcmp(first,last)) {
        snprintf(first,sizeof first,"%.17g",low);snprintf(last,sizeof last,"%.17g",high);
    }
    for(char *p=first;*p;p++)if(*p=='.')*p=',';
    for(char *p=last;*p;p++)if(*p=='.')*p=',';
    snprintf(out,160,"%s–%s",first,last);
}
static ps_result graphic_export_bytes(graphic_dialog *d,ps_graphic_bytes *out) {
    if(!d->raster)return PS_INVALID;
    if(d->svg) {
        ps_graphic_svg *svg=NULL;ps_result result=ps_graphic_figure_svg(ps_graphic_raster_figure(d->raster),0,ps_allocator_default(),&svg);
        if(result!=PS_OK)return result;
        size_t size;const char *bytes=ps_graphic_svg_bytes(svg,&size);unsigned char *copy=malloc(size);
        if(copy)memcpy(copy,bytes,size);
        ps_graphic_svg_destroy(svg);
        if(!copy)return PS_MEMORY;
        *out=(ps_graphic_bytes){copy,size};return PS_OK;
    }
    unsigned w,h;size_t stride;const unsigned char *rgb;
    ps_result result=ps_graphics_image_read(ps_graphic_raster_image(d->raster),&w,&h,&stride,&rgb);
    return result==PS_OK?ps_png_encode(rgb,w,h,stride,&out->data,&out->size):result;
}
static void graphic_export_save(app *a,const char *test_path) {
    graphic_dialog *d=a->graphic_export;if(!d || d->save || d->dirty)return;
    ps_graphic_bytes bytes={0};ps_result result=graphic_export_bytes(d,&bytes);
    if(result==PS_OK)d->save=ps_graphic_save_create(&bytes);
    ps_graphic_bytes_free(&bytes);
    if(!d->save){snprintf(d->message,sizeof d->message,"Grafik konnte nicht vorbereitet werden.");return;}
    if(!test_path && a->graphic_export_test_path[0])test_path=a->graphic_export_test_path;
    if(test_path) {
        snprintf(d->saved_path,sizeof d->saved_path,"%s",test_path);
        ps_graphic_save_resolve(d->save,test_path,false);
    } else {
        char path[8192];snprintf(path,sizeof path,"%s/%s",d->directory,d->filename);
        ps_graphic_save_show(d->save,d->window,path,d->svg);d->saved_path[0]=0;
    }
    snprintf(d->message,sizeof d->message,"Speichern…");
}
static struct nk_rect graphic_export_slot(struct nk_context *ui,float x,float y,float w,float h) {
    nk_layout_space_push(ui,nk_rect(x,y,w,h));struct nk_rect r;nk_widget(&r,ui);return r;
}
static void graphic_export_text(graphic_dialog *d,float x,float y,float w,float h,const char *text,unsigned role,struct nk_color color) {
    struct nk_rect r=graphic_export_slot(d->ui,x,y,w,h);
    if(role==0) {
        float at=r.x,spacing=(!strncmp(text,"PHYSIM",6)?1.5f:1.1f)*d->factor;
        for(size_t i=0;text[i];i++) {
            float advance=d->fonts[role].width(d->fonts[role].userdata,d->fonts[role].height,text+i,1);
            nk_draw_text(nk_window_get_canvas(d->ui),nk_rect(at,r.y,advance+1,r.h),text+i,1,&d->fonts[role],nk_rgba(0,0,0,0),color);
            at+=advance+spacing;
        }
    } else nk_draw_text(nk_window_get_canvas(d->ui),r,text,(int)strlen(text),&d->fonts[role],nk_rgba(0,0,0,0),color);
    ps_ui_accessibility_label(d->ui,r,text);
}
static unsigned graphic_export_wrap(graphic_dialog *d,float x,float y,float width,const char *text,unsigned role,float line,bool draw) {
    const struct nk_user_font *font=&d->fonts[role];unsigned lines=0;const char *at=text;
    while(*at) {
        size_t bytes=0,next=0,space=0;
        while(at[next]) {
            next++;while(((unsigned char)at[next]&0xc0)==0x80)next++;
            if(bytes && font->width(font->userdata,font->height,at,(int)next)>width)break;
            bytes=next;if(at[next-1]==' ')space=next;
        }
        if(at[bytes] && role!=6 && space)bytes=space;
        if(!bytes)break;
        if(draw) {
            struct nk_rect r=graphic_export_slot(d->ui,x,y+lines*line,width,line);
            nk_draw_text(nk_window_get_canvas(d->ui),r,at,(int)bytes,font,nk_rgba(0,0,0,0),role==6?UI_COLOR(text):UI_COLOR(muted));
        }
        at+=bytes;while(*at==' ' && role!=6)at++;lines++;
    }
    if(draw && role!=6)ps_ui_accessibility_label(d->ui,nk_layout_space_rect_to_screen(d->ui,nk_rect(x,y,width,lines*line)),text);
    return lines;
}
static bool graphic_export_button(graphic_dialog *d,float x,float y,float w,float h,const char *label,bool primary) {
    struct nk_context *ui=d->ui;
    /* Absolute slots already have measured width and CSS padding. The app's
     * flow-row wrapping adapter may replace row.height for large text; using it
     * here would replace the entire scrolling sheet height with one button. */
    nk_layout_space_push(ui,nk_rect(x,y,w,h));
    nk_style_push_font(ui,&d->fonts[2]);
    struct nk_style_button previous=ui->style.button;
    ui->style.button.padding=nk_vec2(12*d->factor,7*d->factor);
    if(primary) {
        ui->style.button.normal=nk_style_item_color(UI_COLOR(primary));
        ui->style.button.hover=nk_style_item_color(UI_COLOR(primary_hover));
        ui->style.button.active=nk_style_item_color(UI_COLOR(primary_active));
        ui->style.button.text_normal=ui->style.button.text_hover=ui->style.button.text_active=UI_COLOR(primary_text);
    }
    bool clicked=ps_ui_fixed_button_label(ui,label);
    ui->style.button=previous;nk_style_pop_font(ui);return clicked;
}
static bool graphic_export_combo(graphic_dialog *d,unsigned field,float x,float y,float w,float f,
    const char *name,const char *const *items,int count,int *selected,bool disabled) {
    struct nk_context *ui=d->ui;bool changed=false;
    nk_style_push_font(ui,&d->fonts[2]);nk_begin_disabled(ui,disabled);
    nk_layout_space_push(ui,nk_rect(x,y,w,34*f));d->fields[field]=nk_widget_bounds(ui);
    nk_fill_rect(nk_window_get_canvas(ui),d->fields[field],6*f,UI_COLOR(edit));
    nk_stroke_rect(nk_window_get_canvas(ui),d->fields[field],6*f,1,UI_COLOR(border));
    if(ps_ui_combo_begin_named(ui,name,items[*selected],nk_vec2(w,36*f*count+12))) {
        d->menu_field=field+1;
        nk_layout_row_dynamic(ui,34*f,1);
        for(int i=0;i<count;i++) {
            d->choices[field][i]=nk_widget_bounds(ui);
            bool unavailable=field==0 && i==1 && !d->description.axes.cropped;
            nk_begin_disabled(ui,unavailable);
            if(nk_combo_item_label(ui,items[i],NK_TEXT_LEFT)){changed=i!=*selected;*selected=i;}
            nk_end_disabled(ui);
        }
        nk_combo_end(ui);
    }
    nk_end_disabled(ui);nk_style_pop_font(ui);return changed;
}
static void graphic_export_view(app *a,int width,int height) {
    graphic_dialog *d=a->graphic_export;if(!d || d->ui!=a->ui)return;
    if(d->dirty) {
        Uint64 render_started=SDL_GetTicksNS();
        ps_graphic_figure_options options={0};options.dark=d->appearance && d->dark;options.visible=d->visible;
        options.high_contrast=d->appearance && d->contrast;
        ps_graphic_raster *raster=NULL;
        ps_result result=ps_graphic_raster_create(d->graphics,d->snapshot,&options,d->scale,graphic_font_load,a->root,ps_allocator_default(),&raster);
        ps_graphic_raster_destroy(d->raster);d->raster=raster;d->dirty=false;
        if(SDL_getenv("PHYSIM_TEST_TRACE"))fprintf(stderr,"GRAPHIC DIALOG RASTER: scale=%u visible=%u dark=%u seconds=%.6f result=%s\n",d->scale,d->visible,options.dark,(double)(SDL_GetTicksNS()-render_started)/1e9,ps_result_string(result));
        if(result!=PS_OK)snprintf(d->message,sizeof d->message,"Vorschau nicht verfügbar: %s. Größe reduzieren.",ps_result_string(result));
    }
    graphic_export_filename(d);
    struct nk_context *ui=d->ui;const ps_ui_palette *previous=ui_palette;
    ui_palette=a->preferences.theme==PS_THEME_HIGH_CONTRAST?&PS_UI_HIGH_CONTRAST:
        a->preferences.theme==PS_THEME_LIGHT?&PS_UI_LIGHT:&PS_UI_DARK;
    struct nk_style saved_style=ui->style;style(ui);
    float f=d->scale_override>0?d->scale_override:a->preferences.ui_size/16.f;
    d->factor=f;
    const float sizes[]={10,width<=420*f?18:width<=760*f?20:24,12,11,10,9,10,12};
    for(unsigned i=0;i<8;i++) {
        bool strong=i==1 || i==7,mono=i==6;
        d->fonts[i]=*(mono?a->font_code:strong?a->font_title:a->font_ui);
        d->fonts[i].height=sizes[i]*f*(mono?a->workbench_code_em:strong?a->workbench_bold_em:a->workbench_regular_em);
    }
    ui->style.window.padding=nk_vec2(0,0);ui->style.window.spacing=nk_vec2(0,0);
    ui->style.window.border=0;ui->style.window.rounding=14*f;
    ui->style.window.fixed_background=nk_style_item_color(nk_rgba(8,12,20,125));
    if(nk_begin(ui,"graphic-export-backdrop",nk_rect(0,0,(float)width,(float)height),NK_WINDOW_NO_INPUT|NK_WINDOW_NO_SCROLLBAR)){}
    nk_end(ui);nk_sdl_window_raise(ui,"graphic-export-backdrop");
    ui->style.window.fixed_background=nk_style_item_color(UI_COLOR(window));
    float margin=width<=760*f?10:20,w=fminf(1080*f,width-2*margin),outer_w=w;
    bool narrow=width<=760*f,columns=narrow && width>420*f,tiny=width<=420*f;
    float pad=(narrow?18:28)*f,header=0,footer=0,side=0,preview_w=0,paper_w=0,paper_h=0,preview_h=0,body=0,total=0,h=0;
    unsigned header_lines=1,note_lines=1,status_lines=1;
    const char *subtitle="Titel, Einheiten und Legende werden übernommen.";
    char note[512],range[160];const double *axes=d->visible?d->description.axes.visible:d->description.axes.full;
    graphic_export_range(range,axes[0],axes[1]);
    snprintf(note,sizeof note,"%u %s · %s · %s %s · %s",d->description.plot.curves,d->description.plot.curves==1?"Kurve":"Kurven",d->description.plot.y_unit.symbol,
        range,d->description.plot.x_unit.symbol,d->visible?"sichtbarer Ausschnitt":"gesamtes Diagramm");
    for(unsigned pass=0;pass<2;pass++) {
        side=narrow?w:285*f;preview_w=narrow?w:w-side;paper_w=fmaxf(80,preview_w-(narrow?36:56)*f);paper_h=paper_w*17/24;
        if(narrow && paper_h>360*f){paper_h=360*f;paper_w=paper_h*24/17;}
        header_lines=graphic_export_wrap(d,0,0,w-2*pad,subtitle,2,18*f,false);
        note_lines=graphic_export_wrap(d,0,0,preview_w-2*pad,note,3,18*f,false);
        status_lines=graphic_export_wrap(d,0,0,narrow?w-48*f:w*.55f,d->message,3,18*f,false);
        header=((narrow?(tiny?107:109):128)+18*(header_lines-1))*f;
        footer=((narrow?104:68)+18*fmaxf(0,(float)status_lines-(narrow?1:2)))*f;
        preview_h=paper_h+(126+18*(note_lines-1))*f;
        if(!narrow)preview_h=fmaxf(440*f,preview_h);
        unsigned filename_lines=graphic_export_wrap(d,0,0,side-(narrow?36:48)*f,d->filename,6,17*f,false);
        /* Keep the filename rule below the final hint even for the longest
         * generated name at 200% text size in a detached, narrow workspace. */
        float settings_h=fmaxf((columns?403:568)*f,((columns?292:438)+19+57+17*filename_lines)*f);
        body=narrow?preview_h+settings_h:fmaxf(preview_h,settings_h);total=header+body+footer;
        h=fminf(total,height-2*margin);
        if(pass==0 && total>h){w-=ui->style.window.scrollbar_size.x;continue;}
        break;
    }
    d->bounds=nk_rect((width-outer_w)/2,(height-h)/2,outer_w,h);
    d->content_height=total;
    nk_window_show(ui,"graphic-export",NK_SHOWN);
    struct nk_window *modal=nk_window_find(ui,"graphic-export");
    if(modal) {
        /* NO_SCROLLBAR hides scroll handling; it does not reset Nuklear's
         * cached offset. Reflowing a formerly tall sheet must keep its content
         * inside the new range before any widget bounds are constructed. */
        float maximum=total>h?fmaxf(0,total-h+ui->style.window.scrollbar_size.y):0;
        modal->scrollbar.x=0;
        modal->scrollbar.y=(nk_uint)fminf((float)modal->scrollbar.y,maximum);
    }
    /* Moving the passive backdrop above an existing window makes Nuklear mark
     * that window ROM. Restore its real input focus before panel_begin copies
     * those flags into the layout. A live combo keeps its parent read-only. */
    if(modal && !modal->popup.active)nk_window_set_focus(ui,"graphic-export");
    if(nk_begin(ui,"graphic-export",d->bounds,total>h?0:NK_WINDOW_NO_SCROLLBAR)) {
        if(d->first){nk_window_set_focus(ui,"graphic-export");d->first=false;}
        if(d->focus_target) {
            const char *names[]={"Grafikexport schließen","PNG","SVG","Zeitbereich","Darstellung","PNG-Größe","Bild kopieren",d->svg?"SVG speichern…":"PNG speichern…"};
            unsigned index=d->focus_target-1;
            if(nk_sdl_accessibility_is_focused(ui,index==1 || index==2?"Dateiformat":NULL,names[index])) {
                if(SDL_getenv("PHYSIM_TEST_TRACE"))fprintf(stderr,"GRAPHIC DIALOG FOCUS: granted=%s\n",names[index]);
                d->focus_target=0;
            } else nk_sdl_accessibility_request_focus(ui,index==1 || index==2?"Dateiformat":NULL,names[index]);
        }
        nk_sdl_window_raise(ui,"graphic-export");
        nk_layout_space_begin(ui,NK_STATIC,total,100);
        graphic_export_text(d,pad,(narrow?17:24)*f,w-100*f,16*f,"PHYSIM / AUSWERTUNG",0,UI_COLOR(muted));
        graphic_export_text(d,pad,(narrow?43:50)*f,w-85*f,33*f,"Grafik exportieren",1,UI_COLOR(text));
        graphic_export_wrap(d,pad,(narrow?(tiny?77:79):91)*f,w-2*pad,subtitle,2,18*f,true);
        d->exit=graphic_export_slot(ui,w-58*f,24*f,30*f,30*f);
        float cx=d->exit.x+d->exit.w/2,cy=d->exit.y+d->exit.h/2;
        nk_stroke_line(nk_window_get_canvas(ui),cx-5*f,cy-5*f,cx+5*f,cy+5*f,1.5f*f,UI_COLOR(text));
        nk_stroke_line(nk_window_get_canvas(ui),cx+5*f,cy-5*f,cx-5*f,cy+5*f,1.5f*f,UI_COLOR(text));
        if(ps_ui_button_at(ui,d->exit,"Grafikexport schließen",false))d->close=true;
        struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
        struct nk_rect preview=graphic_export_slot(ui,0,header,preview_w,preview_h);
        nk_fill_rect(canvas,preview,0,UI_COLOR(canvas));
        graphic_export_text(d,pad,header+24*f,preview_w-180*f,16*f,"EXPORTVORSCHAU",0,UI_COLOR(muted));
        char dimensions[64];snprintf(dimensions,sizeof dimensions,d->svg?"Skalierbares SVG":"%u × %u px",1200*d->scale,850*d->scale);
        graphic_export_text(d,preview_w-150*f,header+24*f,124*f,16*f,dimensions,4,UI_COLOR(muted));
        d->paper=graphic_export_slot(ui,(preview_w-paper_w)/2,header+56*f,paper_w,paper_h);
        nk_fill_rect(canvas,nk_rect(d->paper.x,d->paper.y+6*f,paper_w,paper_h),0,nk_rgba(0,0,0,25));
        unsigned texture=d->raster?ps_graphic_raster_texture(d->raster):0;
        if(texture){struct nk_image image=nk_image_id((int)texture);nk_draw_image(canvas,d->paper,&image,nk_rgb(255,255,255));}
        else nk_fill_rect(canvas,d->paper,0,UI_COLOR(edit));
        nk_stroke_rect(canvas,d->paper,0,1,UI_COLOR(border));
        graphic_export_wrap(d,pad,header+paper_h+73*f,preview_w-2*pad,note,3,18*f,true);
        graphic_export_text(d,pad,header+paper_h+(97+18*(note_lines-1))*f,preview_w-2*pad,16*f,"Beim Öffnen festgehaltene Diagrammdaten.",4,UI_COLOR(muted));
        float sx=narrow?18*f:preview_w+24*f,sy=narrow?header+preview_h:header,sw=side-(narrow?36:48)*f;
        float field_w=columns?(sw-16*f)/2:sw;
        graphic_export_text(d,sx,sy+23*f,sw,18*f,"Dateiformat",7,UI_COLOR(text));
        nk_begin_disabled(ui,d->save!=NULL);
        for(unsigned i=0;i<2;i++) {
            float card_x=sx+(columns?i*(field_w+16*f):0),card_y=sy+(51+(columns?0:i*67))*f,card_w=columns?field_w:sw;
            d->formats[i]=graphic_export_slot(ui,card_x,card_y,card_w,60*f);
            struct nk_rect card=d->formats[i];bool selected=d->svg==(i==1);
            nk_fill_rect(canvas,card,7*f,selected?UI_COLOR(selection_active):UI_COLOR(edit));
            nk_stroke_rect(canvas,card,7*f,1,selected?UI_COLOR(accent):UI_COLOR(border));
            struct nk_color radio=ui_palette==&PS_UI_DARK?UI_COLOR(primary_text):UI_COLOR(accent);
            nk_stroke_circle(canvas,nk_rect(card.x+12*f,card.y+23*f,14*f,14*f),f,selected?radio:UI_COLOR(muted));
            if(selected)nk_fill_circle(canvas,nk_rect(card.x+16*f,card.y+27*f,6*f,6*f),radio);
            if(ps_ui_option_at(ui,card,"Dateiformat",i?"SVG":"PNG",selected))d->svg=i==1;
            graphic_export_text(d,card_x+36*f,card_y+12*f,card_w-43*f,18*f,i?"SVG":"PNG",7,UI_COLOR(text));
            graphic_export_text(d,card_x+36*f,card_y+35*f,card_w-43*f,16*f,i?"Skalierbare Vektorgrafik":"Dokumente & Präsentationen",4,UI_COLOR(muted));
        }
        nk_end_disabled(ui);
        const char *scope[]={"Gesamtes Diagramm","Sichtbarer Ausschnitt"},*appearance[]={"Hell auf Weiß","Wie in der Auswertung"};
        const char *sizes_text[]={"1200 × 850 px","2400 × 1700 px","3600 × 2550 px","4800 × 3400 px"};
        int scope_index=d->visible,appearance_index=d->appearance,scale_index=(int)d->scale-1;
        float field_y=sy+(columns?131:199)*f;
        graphic_export_text(d,sx,field_y,field_w,18*f,"Zeitbereich",7,UI_COLOR(text));
        if(graphic_export_combo(d,0,sx,field_y+24*f,field_w,f,"Zeitbereich",scope,2,&scope_index,d->save!=NULL)){d->visible=scope_index!=0;d->dirty=true;}
        float ax=columns?sx+field_w+16*f:sx,ay=columns?field_y:field_y+78*f;
        graphic_export_text(d,ax,ay,field_w,18*f,"Darstellung",7,UI_COLOR(text));
        if(graphic_export_combo(d,1,ax,ay+24*f,field_w,f,"Darstellung",appearance,2,&appearance_index,d->save!=NULL)){d->appearance=appearance_index!=0;d->dirty=true;}
        float size_y=field_y+(columns?78:156)*f;
        graphic_export_text(d,sx,size_y,field_w,18*f,"PNG-Größe",7,d->svg?UI_COLOR(muted):UI_COLOR(text));
        if(graphic_export_combo(d,2,sx,size_y+24*f,field_w,f,"PNG-Größe",sizes_text,4,&scale_index,d->svg || d->save!=NULL)){d->scale=(unsigned)scale_index+1;d->dirty=true;}
        graphic_export_text(d,sx,size_y+67*f,field_w,16*f,d->svg?"SVG bleibt ohne feste Pixelgröße.":"Mehr Bildpixel, dieselben Messdaten.",4,UI_COLOR(muted));
        unsigned file_lines=graphic_export_wrap(d,0,0,sw,d->filename,6,17*f,false);
        float file_y=sy+body-(narrow?preview_h:0)-(43+file_lines*17)*f;
        struct nk_rect file_rule=graphic_export_slot(ui,sx,file_y-14*f,sw,1);nk_fill_rect(canvas,file_rule,0,UI_COLOR(border));
        graphic_export_text(d,sx,file_y,sw,14*f,"DATEINAME",5,UI_COLOR(muted));
        graphic_export_wrap(d,sx,file_y+22*f,sw,d->filename,6,17*f,true);
        ps_ui_accessibility_label(ui,nk_layout_space_rect_to_screen(ui,nk_rect(sx,file_y+22*f,sw,file_lines*17*f)),d->filename);
        float bottom=header+body;
        struct nk_rect line=graphic_export_slot(ui,0,header,w,1);nk_fill_rect(canvas,line,0,UI_COLOR(border));
        line=graphic_export_slot(ui,narrow?0:preview_w,narrow?sy:header,narrow?w:1,narrow?1:body);nk_fill_rect(canvas,line,0,UI_COLOR(border));
        line=graphic_export_slot(ui,0,bottom,w,1);nk_fill_rect(canvas,line,0,UI_COLOR(border));
        const char *save_label=d->svg?"SVG speichern…":"PNG speichern…";
        float actions_y=bottom+(narrow?52+18*(status_lines-1):16)*f;
        /* Nuklear's text clamp excludes a glyph at an exactly equal advance.
         * Keep a three-pixel fitting guard beyond the two 12-pixel insets. */
        float button_w=d->fonts[2].width(d->fonts[2].userdata,d->fonts[2].height,"Bild kopieren",13)+27*f;
        float save_w=d->fonts[2].width(d->fonts[2].userdata,d->fonts[2].height,save_label,(int)strlen(save_label))+27*f;
        graphic_export_wrap(d,24*f,bottom+(narrow?14:status_lines>1?16:21)*f,narrow?w-48*f:w*.55f,d->message,3,18*f,true);
        bool unavailable=!d->raster || d->dirty || d->save;nk_begin_disabled(ui,unavailable);
        d->copy=nk_layout_space_rect_to_screen(ui,nk_rect(w-24*f-save_w-8*f-button_w,actions_y,button_w,34*f));
        if(graphic_export_button(d,w-24*f-save_w-8*f-button_w,actions_y,button_w,34*f,"Bild kopieren",false)) {
            ps_result result=ps_graphic_image_clipboard(ps_graphic_raster_image(d->raster));
            snprintf(d->message,sizeof d->message,"%s",result==PS_OK?"Bild in die Zwischenablage kopiert.":"Bild konnte nicht kopiert werden.");
        }
        d->store=nk_layout_space_rect_to_screen(ui,nk_rect(w-24*f-save_w,actions_y,save_w,34*f));
        if(graphic_export_button(d,w-24*f-save_w,actions_y,save_w,34*f,save_label,true))graphic_export_save(a,NULL);
        nk_end_disabled(ui);
        if(d->focus_target) {
            struct nk_rect targets[]={d->exit,d->formats[0],d->formats[1],d->fields[0],d->fields[1],d->fields[2],d->copy,d->store};
            struct nk_rect target=targets[d->focus_target-1];float low=d->bounds.y+8,high=d->bounds.y+d->bounds.h-14;
            float offset=(float)ui->current->scrollbar.y,prior=offset;
            if(target.y<low)offset+=target.y-low;else if(target.y+target.h>high)offset+=target.y+target.h-high;
            ui->current->scrollbar.y=(nk_uint)fmaxf(0,fminf(fmaxf(0,total-d->bounds.h+16),offset));
            if(prior!=ui->current->scrollbar.y && SDL_getenv("PHYSIM_TEST_TRACE"))fprintf(stderr,"GRAPHIC DIALOG FOCUS: reveal=%u scroll=%.1f->%u target=%.1f/%.1f viewport=%.1f/%.1f row=%.1f total=%.1f\n",d->focus_target,prior,ui->current->scrollbar.y,target.y,target.h,low,high,ui->current->layout->row.height,total);
        }
        nk_layout_space_end(ui);
        struct nk_rect previous_clip=canvas->clip;
        nk_push_scissor(canvas,d->bounds);
        nk_stroke_rect(canvas,d->bounds,14*f,1,UI_COLOR(border));
        nk_push_scissor(canvas,previous_clip);
    }
    nk_end(ui);ui->style=saved_style;ui_palette=previous;
}
