static struct nk_rect unit_slot(channel_unit_task *d,float x,float y,float w,float h) {
    nk_layout_space_push(d->ui,nk_rect(x,y,fmaxf(1,w),h));struct nk_rect r;nk_widget(&r,d->ui);return r;
}
static float unit_text_height(channel_unit_task *d,const char *text,unsigned role,float width,float pitch) {
    const struct nk_user_font *font=d->fonts+role;int n=(int)strlen(text),at=0,lines=0;
    while(at<n){int take=ps_ui_text_line(font,text+at,n-at,fmaxf(1,width),false);at+=take>0?take:1;lines++;}return fmaxf(pitch,lines*pitch);
}
static float unit_text(channel_unit_task *d,float x,float y,float width,const char *text,unsigned role,float pitch,struct nk_color color) {
    const struct nk_user_font *font=d->fonts+role;int n=(int)strlen(text),at=0,lines=0;
    while(at<n){int take=ps_ui_text_line(font,text+at,n-at,fmaxf(1,width),false);at+=take>0?take:1;lines++;}
    float h=fmaxf(pitch,lines*pitch);struct nk_rect r=unit_slot(d,x,y,width,h);ps_ui_accessibility_label(d->ui,r,text);
    float top=r.y+(pitch-font->height)/2;
    for(at=0;at<n;){int take=ps_ui_text_line(font,text+at,n-at,fmaxf(1,width),false);if(take<=0)take=1;
        nk_draw_text(nk_window_get_canvas(d->ui),nk_rect(r.x,top,width,font->height),text+at,take,font,nk_rgba(0,0,0,0),color);at+=take;top+=pitch;}
    return h;
}
static void unit_focus(app *a,unsigned id,struct nk_rect r,bool native) {
    channel_unit_task *d=a->unit_task;struct nk_context *ui=d->ui;d->controls[id]=r;
    if(ps_ui_mouse_pressed(&ui->input,NK_BUTTON_LEFT) && nk_input_is_mouse_hovering_rect(&ui->input,r)){d->focus=id;d->keyboard=d->focus_scroll=false;}
    if(native && !d->keyboard && d->focus!=id){d->focus=id;d->focus_scroll=true;}
    bool custom=d->keyboard && d->focus==id;
    if(d->first && id==UNIT_CHANNEL && nk_sdl_accessibility_keyboard_target(ui,r))d->first=false;
    if(!native && !custom)return;if(custom)nk_sdl_accessibility_keyboard_target(ui,r);
    nk_stroke_rect(nk_window_get_canvas(ui),r,4,2,UI_COLOR(accent));
    if((custom || native) && d->focus_scroll && !strcmp(ui->current->name_string,"unit-body")){
        struct nk_rect clip=nk_window_get_content_region(ui);float delta=r.y<clip.y?r.y-clip.y:r.y+r.h>clip.y+clip.h?r.y+r.h-clip.y-clip.h:0;
        nk_uint sx,sy;nk_window_get_scroll(ui,&sx,&sy);if(delta)nk_window_set_scroll(ui,sx,(nk_uint)fmaxf(0,ceilf(sy+delta)));else d->focus_scroll=false;}
}
static bool unit_button(app *a,unsigned id,const char *label,struct nk_rect pos,float f,bool primary,bool enabled) {
    channel_unit_task *d=a->unit_task;settings_colors c=settings_theme(a->preferences.theme);struct nk_rect r=unit_slot(d,pos.x,pos.y,pos.w,pos.h);bool native=false;
    struct nk_color bg=primary?c.primary:c.button,ink=primary?c.on_primary:c.text;
    if(!enabled){bg=nk_rgb((bg.r*45+c.header.r*55)/100,(bg.g*45+c.header.g*55)/100,(bg.b*45+c.header.b*55)/100);ink=nk_rgb((ink.r*45+c.header.r*55)/100,(ink.g*45+c.header.g*55)/100,(ink.b*45+c.header.b*55)/100);}
    nk_fill_rect(nk_window_get_canvas(d->ui),r,6*f,bg);nk_stroke_rect(nk_window_get_canvas(d->ui),r,6*f,f,primary?bg:c.border);
    const struct nk_user_font *font=d->fonts;float tw=font->width(font->userdata,font->height,label,(int)strlen(label));
    nk_draw_text(nk_window_get_canvas(d->ui),nk_rect(r.x+fmaxf(6*f,(r.w-tw)/2),r.y+(r.h-font->height)/2,r.w-12*f,font->height),label,(int)strlen(label),font,nk_rgba(0,0,0,0),ink);
    nk_begin_disabled(d->ui,!enabled);bool clicked=ps_ui_button_at_custom_focus(d->ui,r,channel_unit_focus_name(id),false,&native);nk_end_disabled(d->ui);unit_focus(a,id,r,native);return clicked;
}
static void unit_edit(app *a,unsigned id,char *text,int capacity,float x,float y,float w,float f) {
    channel_unit_task *d=a->unit_task;struct nk_context *ui=d->ui;nk_layout_space_push(ui,nk_rect(x,y,w,34*f));struct nk_rect r=nk_widget_bounds(ui);
    bool target=d->keyboard && d->focus==id,native=nk_sdl_accessibility_is_focused(ui,NULL,channel_unit_focus_name(id));
    if((target || native) && !ui->current->popup.active){nk_window_set_focus(ui,"unit-body");ui->current->flags&=~(nk_flags)NK_WINDOW_ROM;ui->current->layout->flags&=~(nk_flags)NK_WINDOW_ROM;}
    if((target || native) && !ui->current->edit.active)nk_edit_focus(ui,NK_EDIT_ALWAYS_INSERT_MODE);
    char before[64];SDL_strlcpy(before,text,sizeof before);nk_sdl_edit_named(ui,channel_unit_focus_name(id),NK_EDIT_FIELD,text,capacity,nk_filter_default);
    if(strcmp(before,text)){d->edits[d->selected].si=false;d->message[0]=0;if(a->channel_units_writable)a->channel_units_error[0]=0;}
    unit_focus(a,id,r,nk_sdl_accessibility_is_focus_target(ui,NULL,channel_unit_focus_name(id)));
}
static void unit_combo(app *a,unsigned id,float x,float y,float w,float f) {
    channel_unit_task *d=a->unit_task;struct nk_context *ui=d->ui;char label[160];unsigned selected=id==UNIT_CHANNEL?d->selected:channel_unit_menu_count(a,id)-1;
    if(id==UNIT_CHANNEL)snprintf(label,sizeof label,"%s [%s]",a->unit_channel.name,a->unit_channel.unit);
    else {double scale=0;bool match=false;unsigned index=0;ps_parse_finite_number(a->channel_unit_scale,NULL,&scale);
        for(unsigned i=0;i<sizeof channel_unit_presets/sizeof channel_unit_presets[0];i++)if(!memcmp(channel_unit_presets[i].dimension,a->unit_channel.dimension,7)){
            if(channel_unit_presets[i].scale==scale && !strcmp(channel_unit_presets[i].symbol,a->unit_draft.symbol)){match=true;selected=index;}index++;}
        snprintf(label,sizeof label,"%s",match?a->unit_draft.symbol:"Eigene Einheit");}
    nk_layout_space_push(ui,nk_rect(x,y,w,34*f));struct nk_rect r=nk_widget_bounds(ui);
    settings_colors c=settings_theme(a->preferences.theme);nk_fill_rect(nk_window_get_canvas(ui),r,5*f,c.field);nk_stroke_rect(nk_window_get_canvas(ui),r,5*f,f,c.border);
    bool open=ps_ui_combo_begin_named_scaled(ui,channel_unit_focus_name(id),label,nk_vec2(w,220*f),f);
    if(open){
        if(d->menu_owner!=id+1){d->menu_owner=id+1;d->menu_index=selected;d->menu_focus=d->menu_keyboard;}
        nk_layout_row_dynamic(ui,32*f,1);
        if(id==UNIT_CHANNEL){for(unsigned i=0;i<d->count;i++){snprintf(label,sizeof label,"%s [%s]",d->edits[i].channel.name,d->edits[i].channel.unit);if(nk_button_label(ui,label)){channel_unit_select(a,i);channel_unit_menu_return(a,id);nk_combo_close(ui);}}}
        else {for(unsigned i=0;i<sizeof channel_unit_presets/sizeof channel_unit_presets[0];i++)if(!memcmp(channel_unit_presets[i].dimension,a->unit_channel.dimension,7)){
            if(nk_button_label(ui,channel_unit_presets[i].symbol)){channel_unit_choose_preset(a,i);channel_unit_menu_return(a,id);nk_combo_close(ui);}}
            if(nk_button_label(ui,"Eigene Einheit")){channel_unit_menu_return(a,id);nk_combo_close(ui);}}
        if(d->menu_focus){channel_unit_menu_label(a,id,d->menu_index,label);if(nk_sdl_accessibility_request_focus(ui,NULL,label))d->menu_focus=false;}
        if(d->popup_close)nk_combo_close(ui);nk_combo_end(ui);}
    else if(d->menu_owner==id+1){d->menu_owner=0;d->menu_focus=false;}
    unit_focus(a,id,r,nk_sdl_accessibility_is_focus_target(ui,NULL,channel_unit_focus_name(id)));
}
static bool channel_unit_sample(app *a,double *out) {
    for(unsigned i=0;i<a->channel_count;i++)if(!strcmp(a->live_channels[i].name,a->unit_channel.name) && !memcmp(a->live_channels[i].dimension,a->unit_channel.dimension,7)){
        const double *shown=display_values(a);int mask=a->channel_status[i];if((mask<0 || shown[mask]==1) && isfinite(shown[i])){*out=shown[i];return true;}
        for(int j=a->history_count-1;j>=0;j--)if(isfinite(a->history_v[i][j])){*out=a->history_v[i][j];return true;}}
    if(!a->data.thread)for(unsigned i=0;i<a->data.channel_count;i++)if(!strcmp(a->data.channels[i].name,a->unit_channel.name) && !memcmp(a->data.channels[i].dimension,a->unit_channel.dimension,7)){
        for(int j=0;j<a->data.count;j++)if(isfinite(a->data.values[i][j])){*out=a->data.values[i][j];return true;}}
    return false;
}
static float unit_preview_text(channel_unit_task *d,bool draw,float x,float y,float width,const char *text,unsigned role,float pitch,struct nk_color color) {
    return draw?unit_text(d,x,y,width,text,role,pitch,color):unit_text_height(d,text,role,width,pitch);
}
static float unit_preview(app *a,float x,float y,float w,float f,bool draw) {
    channel_unit_task *d=a->unit_task;settings_colors c=settings_theme(a->preferences.theme);float start=y;char text[192];
    y+=unit_preview_text(d,draw,x,y,w,"ANZEIGEVORSCHAU",3,16*f,c.muted)+15*f;double sample;ps_display_unit u=a->unit_draft;
    bool present=channel_unit_sample(a,&sample),valid=ps_parse_finite_number(a->channel_unit_scale,NULL,&u.scale) && ps_display_unit_valid(&u);double converted;
    if(!present)SDL_strlcpy(text,"Keine Messwerte",sizeof text);else if(valid && ps_display_unit_value(&u,sample,&converted)==PS_OK)snprintf(text,sizeof text,"%.8g %s",converted,u.symbol);else SDL_strlcpy(text,"Nicht darstellbar",sizeof text);
    y+=unit_preview_text(d,draw,x,y,w,text,4,32*f,c.text)+15*f;
    if(present)snprintf(text,sizeof text,"Aus %.8g %s. Faktor = SI-Einheiten pro Anzeigeeinheit.",sample,a->unit_channel.unit);else SDL_strlcpy(text,"Vorschau verfügbar, sobald dieser Kanal Messwerte enthält.",sizeof text);
    y+=unit_preview_text(d,draw,x,y,w,text,3,18*f,c.muted)+12*f;
    y+=unit_preview_text(d,draw,x,y,w,"Nur Anzeige dieses Messkanals. Rohdaten, Modell und CSV bleiben unverändert. Berichte verwenden die im Analysecode festgelegten Einheiten.",3,18*f,c.muted);
    return y-start;
}
static bool channel_unit_window_render(app *a) {
    channel_unit_task *d=a->unit_task;if(!d || d->close || SDL_GetWindowFlags(d->window)&(SDL_WINDOW_HIDDEN|SDL_WINDOW_MINIMIZED))return true;
    if(!ps_graphics_make_current(d->graphics))return false;float f=a->preferences.ui_size/16.f;
    const ps_ui_palette *prior=ui_palette;settings_colors c=settings_theme(a->preferences.theme);ps_ui_palette palette=*prior;
    palette.window=c.panel;palette.header=c.header;palette.surface=c.header;palette.edit=c.field;palette.button=c.button;palette.border=c.border;palette.text=c.text;palette.muted=c.muted;palette.accent=c.accent;palette.primary=c.primary;palette.primary_text=c.on_primary;palette.button_hover=c.selected;ui_palette=&palette;
    for(unsigned i=0;i<6;i++){bool bold=i==2 || i==4;float em=bold?a->workbench_bold_em:a->workbench_regular_em;const float sizes[]={11.9f,10.5f,14,10.22f,25.2f,14};d->fonts[i]=*(bold?a->font_title:a->font_ui);d->fonts[i].height=sizes[i]*f*(em>0?em:1);}
    style(d->ui);nk_style_set_font(d->ui,d->fonts);nk_sdl_set_ui_size(d->ui,a->preferences.ui_size);d->ui->style.window.padding=d->ui->style.window.spacing=nk_vec2(0,0);d->ui->style.window.border=0;d->ui->style.window.min_size=nk_vec2(1,1);d->ui->style.edit.rounding=5*f;d->ui->style.edit.border=1;
    ps_ui_font_layout *layout=d->ui->userdata.ptr;bool full=layout->accessibility_full_controls;layout->accessibility_full_controls=true;channel_unit_activate(a);
    int ww,hh;SDL_GetWindowSize(d->window,&ww,&hh);float w=(float)ww,h=(float)hh,header=74*f;struct nk_context *ui=d->ui;
    const char *actions[]={"SI wiederherstellen","Schließen","Einheit anwenden"};unsigned ids[]={UNIT_SI,UNIT_CLOSE,UNIT_APPLY};struct nk_rect slots[3];float total=14*f,widths[3];
    for(unsigned i=0;i<3;i++){widths[i]=d->fonts[0].width(d->fonts[0].userdata,d->fonts[0].height,actions[i],(int)strlen(actions[i]))+24*f;total+=widths[i];}
    bool wide=w>total+280*f;float x=wide?w-total-18*f:18*f,row=wide?13*f:40*f;
    for(unsigned i=0;i<3;i++){if(!wide && x+widths[i]>w-18*f && x>18*f){x=18*f;row+=42*f;}slots[i]=nk_rect(x,row,widths[i],34*f);x+=widths[i]+7*f;}
    float footer=row+48*f,footer_y=fmaxf(header+1,h-footer),body_h=fmaxf(1,footer_y-header);
    memset(d->controls,0,sizeof d->controls);
    if(nk_begin(ui,"unit-header",nk_rect(0,0,w,header),NK_WINDOW_NO_SCROLLBAR)){
        ps_ui_accessibility_route(ui,true);channel_unit_activate(a);nk_layout_space_begin(ui,NK_STATIC,header,12);struct nk_rect bg=unit_slot(d,0,0,w,header);nk_fill_rect(nk_window_get_canvas(ui),bg,0,c.header);
        struct nk_rect icon=unit_slot(d,18*f,17*f,37*f,37*f);nk_fill_rect(nk_window_get_canvas(ui),icon,9*f,c.field);nk_stroke_rect(nk_window_get_canvas(ui),icon,9*f,f,c.border);
        nk_draw_text(nk_window_get_canvas(ui),nk_rect(icon.x+11*f,icon.y+8*f,20*f,22*f),"m",1,d->fonts+5,nk_rgba(0,0,0,0),c.accent);
        unit_text(d,68*f,16*f,w-125*f,"Anzeigeeinheit",2,22*f,c.text);unit_text(d,68*f,41*f,w-120*f,"Eigene Aufgabe · Echte Messkanäle",3,15*f,c.muted);
        struct nk_rect close=unit_slot(d,w-45*f,20*f,26*f,30*f);bool native=false;struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
        nk_stroke_line(canvas,close.x+8*f,close.y+9*f,close.x+17*f,close.y+18*f,1.4f*f,c.text);nk_stroke_line(canvas,close.x+17*f,close.y+9*f,close.x+8*f,close.y+18*f,1.4f*f,c.text);
        if(ps_ui_button_at_custom_focus(ui,close,channel_unit_focus_name(UNIT_EXIT),false,&native))channel_unit_close(a);unit_focus(a,UNIT_EXIT,close,native);struct nk_rect line=unit_slot(d,0,header-1,w,1);nk_stroke_line(canvas,line.x,line.y,line.x+w,line.y,1,c.border);nk_layout_space_end(ui);
    }nk_end(ui);
    bool stack=w<700*f;float content_h=0,available=w,left=0;const char *message=*a->channel_units_error?a->channel_units_error:d->message;
    for(unsigned pass=0;pass<2;pass++){
        left=stack?available-44*f:available-309*f;float fields=306*f+(a->channel_units_writable?0:48*f);
        float preview=unit_preview(a,0,0,stack?available-44*f:220*f,f,false);
        content_h=stack?fields+20*f+preview:fmaxf(fields,22*f+preview);
        if(*message)content_h+=38*f+unit_text_height(d,message,3,available-68*f,18*f);content_h+=22*f;
        if(content_h<=body_h || pass)break;available=w-ui->style.window.scrollbar_size.x;
    }
    d->body_rect=nk_rect(0,header,w,body_h);d->message_rect=nk_rect(0,0,0,0);
    if(nk_begin(ui,"unit-body",d->body_rect,content_h>body_h?0:NK_WINDOW_NO_SCROLLBAR)){
        ps_ui_accessibility_route(ui,true);channel_unit_activate(a);if(d->keyboard)nk_edit_unfocus(ui);
        nk_layout_space_begin(ui,NK_STATIC,content_h,128);float y=22*f;const char *labels[]={"Messkanal","Vorgabe","Symbol","Faktor · SI pro Anzeigeeinheit"};
        for(unsigned i=0;i<4;i++){nk_style_push_font(ui,d->fonts+1);unit_text(d,22*f,y,left,labels[i],1,18*f,c.text);nk_style_pop_font(ui);y+=23*f;
            nk_begin_disabled(ui,i!=0 && !a->channel_units_writable);if(i<2)unit_combo(a,i,22*f,y,left,f);else unit_edit(a,i,i==2?a->unit_draft.symbol:a->channel_unit_scale,64,22*f,y,left,f);nk_end_disabled(ui);y+=48*f;}
        if(!a->channel_units_writable){if(unit_button(a,UNIT_RECOVER,"Datei bewusst zurücksetzen",nk_rect(22*f,y,fminf(270*f,left),34*f),f,false,*a->channel_units_path!=0)){ps_channel_units empty={0};channel_units_save(a,&empty);}y+=48*f;}
        if(stack){struct nk_rect line=unit_slot(d,22*f,y,available-44*f,1);nk_stroke_line(nk_window_get_canvas(ui),line.x,line.y,line.x+line.w,line.y,1,c.border);y+=20*f;y+=unit_preview(a,22*f,y,available-44*f,f,true);}
        else {float preview=unit_preview(a,available-242*f,22*f,220*f,f,true);struct nk_rect edge=unit_slot(d,available-262*f,22*f,1,fmaxf(y-22*f,preview));nk_stroke_line(nk_window_get_canvas(ui),edge.x,edge.y,edge.x,edge.y+edge.h,1,c.border);y=fmaxf(y,22*f+preview);}
        if(*message){y+=18*f;float mh=unit_text_height(d,message,3,available-68*f,18*f)+20*f;struct nk_rect box=unit_slot(d,22*f,y,available-44*f,mh);d->message_rect=box;
            struct nk_color bg=*a->channel_units_error?(a->preferences.theme==PS_THEME_LIGHT?nk_rgb(251,238,240):a->preferences.theme==PS_THEME_HIGH_CONTRAST?nk_rgb(32,11,11):nk_rgb(53,38,42)):c.field;
            nk_fill_rect(nk_window_get_canvas(ui),box,6*f,bg);nk_stroke_rect(nk_window_get_canvas(ui),box,6*f,f,*a->channel_units_error?c.error:c.border);unit_text(d,34*f,y+10*f,available-68*f,message,3,18*f,*a->channel_units_error?c.error:c.accent);
            if(d->message_scroll){struct nk_rect clip=nk_window_get_content_region(ui);
                /* A tall message cannot fit as a whole: reveal its beginning
                 * once, then let the user scroll instead of alternating ends. */
                float delta=box.h>clip.h?box.y-clip.y:box.y<clip.y?box.y-clip.y:box.y+box.h>clip.y+clip.h?box.y+box.h-clip.y-clip.h:0;
                if(fabsf(delta)<1)delta=0;nk_uint sx,sy;nk_window_get_scroll(ui,&sx,&sy);if(delta)nk_window_set_scroll(ui,sx,(nk_uint)fmaxf(0,ceilf(sy+delta)));else d->message_scroll=false;}}
        nk_layout_space_end(ui);
    }nk_end(ui);
    if(nk_begin(ui,"unit-footer",nk_rect(0,footer_y,w,h-footer_y),NK_WINDOW_NO_SCROLLBAR)){
        ps_ui_accessibility_route(ui,true);channel_unit_activate(a);nk_layout_space_begin(ui,NK_STATIC,footer,20);struct nk_rect bg=unit_slot(d,0,0,w,footer);nk_fill_rect(nk_window_get_canvas(ui),bg,0,c.header);nk_stroke_line(nk_window_get_canvas(ui),bg.x,bg.y,bg.x+w,bg.y,1,c.border);
        unit_text(d,18*f,wide?22*f:11*f,wide?220*f:w-36*f,"Unabhängig vom allgemeinen Entwurf.",3,17*f,c.muted);
        for(unsigned i=0;i<3;i++)if(unit_button(a,ids[i],actions[i],slots[i],f,i==2,i==1 || a->channel_units_writable)){
            if(i==0)channel_unit_si(a);else if(i==1)channel_unit_close(a);else d->apply=true;}
        nk_layout_space_end(ui);
    }nk_end(ui);
    a->channel_unit_bounds[0]=d->controls[UNIT_PRESET];a->channel_unit_bounds[1]=d->controls[UNIT_SYMBOL];a->channel_unit_bounds[2]=d->controls[UNIT_FACTOR];a->channel_unit_bounds[3]=d->controls[UNIT_APPLY];a->channel_unit_bounds[4]=d->controls[UNIT_SI];a->channel_unit_bounds[5]=d->controls[UNIT_RECOVER];a->channel_unit_bounds[6]=d->controls[UNIT_CLOSE];
    channel_unit_activate(a);d->popup_close=false;layout->accessibility_full_controls=full;ui_palette=prior;
    ps_graphics_background(d->graphics,c.panel.r,c.panel.g,c.panel.b);nk_sdl_update_TextInput(ui);bool ok=nk_sdl_render(ui);
    if(*d->capture){ok=ps_graphics_capture(d->graphics,d->capture) && ok;d->capture[0]=0;}if(!ps_graphics_present(d->graphics))ok=false;return ps_graphics_make_current(a->graphics) && ok;
}
