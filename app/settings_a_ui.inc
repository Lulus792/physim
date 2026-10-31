/* Logical CSS roles from settings-options/settings.css, scaled by the active
 * UI preference. The draft changes only the separate schematic preview. */
enum { SET_FONT_BODY,SET_FONT_HELP,SET_FONT_TITLE,SET_FONT_LABEL,SET_FONT_NAV,SET_FONT_SMALL,SET_FONT_BUTTON,SET_FONT_PREVIEW,SET_FONT_CODE,SET_FONT_PREVIEW_TITLE,SET_FONT_HEADER };
static struct nk_rect settings_slot(app *a,float x,float y,float w,float h) {
    nk_layout_space_push(a->settings_ui,nk_rect(x,y,fmaxf(1,w),h));struct nk_rect r;nk_widget(&r,a->settings_ui);return r;
}
static float settings_text_height(app *a,unsigned role,const char *text,float width,float pitch) {
    const struct nk_user_font *font=&a->settings_fonts[role];int n=(int)strlen(text),at=0,lines=0;
    while(at<n){int used=ps_ui_text_line(font,text+at,n-at,fmaxf(1,width),false);at+=used>0?used:1;lines++;}return fmaxf(pitch,pitch*lines);
}
static float settings_text(app *a,float x,float y,float w,const char *text,unsigned role,float pitch,struct nk_color color) {
    struct nk_user_font *font=&a->settings_fonts[role];int n=(int)strlen(text),at=0;
    float h=settings_text_height(a,role,text,w,pitch);struct nk_rect r=settings_slot(a,x,y,w,h);
    ps_ui_accessibility_label(a->settings_ui,r,text);float top=r.y+(pitch-font->height)/2;
    while(at<n){int used=ps_ui_text_line(font,text+at,n-at,fmaxf(1,w),false);if(used<=0)used=1;
        nk_draw_text(nk_window_get_canvas(a->settings_ui),nk_rect(r.x,top,w,font->height),text+at,used,font,nk_rgba(0,0,0,0),color);at+=used;top+=pitch;}
    return h;
}
static void settings_focus_draw(app *a,int id,struct nk_rect r,bool native) {
    struct nk_context *ui=a->settings_ui;a->settings_focus_bounds[id]=r;
    if(ps_ui_mouse_pressed(&ui->input,NK_BUTTON_LEFT) && nk_input_is_mouse_hovering_rect(&ui->input,r)){a->settings_focus=id;a->settings_keyboard=false;}
    if(a->settings_initial_focus && id==17 && nk_sdl_accessibility_keyboard_target(ui,r))a->settings_initial_focus=false;
    if(native && !a->settings_keyboard && a->settings_focus!=id){a->settings_focus=id;a->settings_focus_scroll=true;}
    bool custom=a->settings_keyboard && a->settings_focus==id;
    if(!custom && !native)return;
    if(custom)nk_sdl_accessibility_keyboard_target(ui,r);
    nk_stroke_rect(nk_window_get_canvas(ui),r,4,2,UI_COLOR(accent));
    if(a->settings_focus_scroll && !strcmp(ui->current->name_string,"settings-body")){
        struct nk_rect clip=nk_window_get_content_region(ui);float delta=r.y<clip.y?r.y-clip.y:r.y+r.h>clip.y+clip.h?r.y+r.h-clip.y-clip.h:0;
        nk_uint sx,sy;nk_window_get_scroll(ui,&sx,&sy);if(delta)nk_window_set_scroll(ui,sx,(nk_uint)fmaxf(0,ceilf(sy+delta)));else a->settings_focus_scroll=false;
    }
}
static bool settings_button(app *a,int id,const char *label,float x,float y,float w,float h,bool primary,bool enabled,bool selected) {
    struct nk_context *ui=a->settings_ui;settings_colors c=settings_theme(a->preferences.theme);float f=a->preferences.ui_size/16.f;
    struct nk_rect r=settings_slot(a,x,y,w,h);bool hover=enabled && nk_input_is_mouse_hovering_rect(&ui->input,r),native=false;
    if(id==1)a->settings_bounds[0]=r;else if(id==2)a->settings_bounds[4]=r;else if(id==3)a->settings_bounds[5]=r;
    struct nk_color bg=primary?c.primary:selected?c.selected:hover?c.selected:c.button,ink=primary?c.on_primary:c.text;
    if(!enabled){bg=primary?nk_rgb((c.primary.r*45+c.header.r*55)/100,(c.primary.g*45+c.header.g*55)/100,(c.primary.b*45+c.header.b*55)/100):c.header;ink=c.muted;}
    if(id==3 || id==25){bg=id==3?c.header:c.panel;ink=c.accent;}
    nk_fill_rect(nk_window_get_canvas(ui),r,6*f,bg);if(id!=3 && id!=25)nk_stroke_rect(nk_window_get_canvas(ui),r,6*f,f,primary?bg:c.border);
    const struct nk_user_font *font=&a->settings_fonts[SET_FONT_BUTTON];float tw=font->width(font->userdata,font->height,label,(int)strlen(label));
    nk_draw_text(nk_window_get_canvas(ui),nk_rect(r.x+fmaxf(4*f,(r.w-tw)/2),r.y+(r.h-font->height)/2,fmaxf(1,r.w-8*f),font->height),label,(int)strlen(label),font,nk_rgba(0,0,0,0),ink);
    nk_begin_disabled(ui,!enabled);bool pressed=ps_ui_button_at_custom_focus(ui,r,id==25?"Zurück zu den Einstellungen":id==18 && a->settings_review?"Zurück":settings_focus_name(id),selected,&native);nk_end_disabled(ui);
    settings_focus_draw(a,id,r,native);return pressed;
}
static bool settings_edit(app *a,int id,const char *window,char *text,int capacity,float x,float y,float w,float h) {
    struct nk_context *ui=a->settings_ui;nk_layout_space_push(ui,nk_rect(x,y,w,h));struct nk_rect r=nk_widget_bounds(ui);
    bool target=a->settings_keyboard && a->settings_focus==id;
    bool native=nk_sdl_accessibility_is_focused(ui,NULL,settings_focus_name(id));
    if((target || native) && !ui->current->popup.active){nk_window_set_focus(ui,window);ui->current->flags&=~(nk_flags)NK_WINDOW_ROM;ui->current->layout->flags&=~(nk_flags)NK_WINDOW_ROM;}
    if(target && !ui->current->edit.active)nk_edit_focus(ui,NK_EDIT_ALWAYS_INSERT_MODE);
    else if(a->settings_keyboard && id==17 && !target)nk_edit_unfocus(ui);
    if(!a->settings_keyboard && nk_sdl_accessibility_is_focused(ui,NULL,settings_focus_name(id)) && !ui->current->edit.active)nk_edit_focus(ui,NK_EDIT_ALWAYS_INSERT_MODE);
    char before[192];snprintf(before,sizeof before,"%s",text);
    nk_style_push_font(ui,&a->settings_fonts[SET_FONT_BUTTON]);
    if(id==17)nk_style_push_vec2(ui,&ui->style.edit.padding,nk_vec2(28*a->preferences.ui_size/16.f,7*a->preferences.ui_size/16.f));
    nk_sdl_edit_named(ui,settings_focus_name(id),NK_EDIT_FIELD,text,capacity,nk_filter_default);if(id==17)nk_style_pop_vec2(ui);nk_style_pop_font(ui);
    settings_focus_draw(a,id,r,nk_sdl_accessibility_is_focus_target(ui,NULL,settings_focus_name(id)));
    (void)window;return strcmp(before,text)!=0;
}
static void settings_line(app *a,float x,float y,float w,struct nk_color color) {
    struct nk_rect r=settings_slot(a,x,y,w,1);nk_stroke_line(nk_window_get_canvas(a->settings_ui),r.x,r.y,r.x+w,r.y,1,color);
}
static unsigned settings_count(uint32_t mask){unsigned n=0;while(mask){n+=mask&1;mask>>=1;}return n;}
static void settings_value(const ps_preferences *p,unsigned field,char text[96]) {
    static const char *themes[]={"Dunkel","Hell","Hoher Kontrast"};
    if(field==0)snprintf(text,96,"%s",p->theme<3?themes[p->theme]:"Ungültig");
    else if(field==1 || field==2)snprintf(text,96,"%u px",field==1?p->ui_size:p->editor_size);
    else if(field==3)snprintf(text,96,"%u Sekunden",p->autosave_seconds);
    else if(field<=10)snprintf(text,96,"%s",p->view_flags&settings_fields[field].flag?"An":"Aus");
    else snprintf(text,96,"%u px",field==11?p->sidebar_width:field==12?p->inspector_width:p->log_height);
}
static void settings_combo(app *a,unsigned field,float x,float y,float w,float f) {
    struct nk_context *ui=a->settings_ui;int id=settings_fields[field].focus;char label[96];settings_value(&a->settings_draft,field,label);
    nk_layout_space_push(ui,nk_rect(x,y,w,34*f));struct nk_rect r=nk_widget_bounds(ui);nk_style_push_font(ui,&a->settings_fonts[SET_FONT_BUTTON]);
    settings_colors c=settings_theme(a->preferences.theme);nk_fill_rect(nk_window_get_canvas(ui),r,5*f,c.field);nk_stroke_rect(nk_window_get_canvas(ui),r,5*f,f,c.border);
    if(ps_ui_combo_begin_named_scaled(ui,settings_focus_name(id),label,nk_vec2(w,220*f),f)){
        nk_layout_row_dynamic(ui,34*f,1);unsigned count=field==2?6:4;
        for(unsigned i=0;i<count;i++){const uint32_t secs[]={10,30,60,120};uint32_t value=field==3?secs[i]:(field==1?16:12)+i*2;
            snprintf(label,sizeof label,field==3?"%u Sekunden":"%u px",value);
            if(nk_button_label(ui,label)){if(field==1)a->settings_draft.ui_size=value;else if(field==2)a->settings_draft.editor_size=value;else a->settings_draft.autosave_seconds=value;nk_combo_close(ui);}}
        if(a->settings_popup_close)nk_combo_close(ui);nk_combo_end(ui);
    }
    nk_style_pop_font(ui);settings_focus_draw(a,id,r,nk_sdl_accessibility_is_focus_target(ui,NULL,settings_focus_name(id)));
    if(field==1)for(unsigned i=0;i<4;i++)a->ui_size_bounds[i]=r;
    if(field==2)a->settings_bounds[1]=r;
}
static float settings_field_height(app *a,unsigned field,float width,float f) {
    if(field==0)return 147*f;
    bool stack=width<360*f;float label_w=stack?width:fmaxf(1,width-182*f);
    float h=18*f+5*f+settings_text_height(a,SET_FONT_HELP,settings_fields[field].help,label_w,17*f);
    return fmaxf(34*f,h)+(stack?46*f:0)+34*f;
}
static float settings_field_draw(app *a,unsigned field,float x,float y,float width,float f) {
    struct nk_context *ui=a->settings_ui;settings_colors c=settings_theme(a->preferences.theme);const settings_field *item=&settings_fields[field];
    float height=settings_field_height(a,field,width,f);bool stack=width<360*f;float lw=field==0 || stack?width:width-182*f;
    settings_text(a,x,y,lw,item->title,SET_FONT_LABEL,18*f,c.text);
    float hh=settings_text(a,x,y+23*f,lw,item->help,SET_FONT_HELP,17*f,c.muted);
    a->settings_fields[field]=settings_slot(a,x,y,width,height-17*f);
    if(field==0){const char *names[]={"Dunkel","Hell","Hoher Kontrast"};float tile=(width-14*f)/3;
        for(unsigned i=0;i<3;i++){settings_colors t=settings_theme(i);struct nk_rect r=settings_slot(a,x+i*(tile+7*f),y+23*f+hh+16*f,tile,78*f);
            bool focused=false,chosen=a->settings_draft.theme==i;nk_fill_rect(nk_window_get_canvas(ui),r,6*f,chosen?c.selected:c.field);nk_stroke_rect(nk_window_get_canvas(ui),r,6*f,f,chosen?c.edge:c.border);
            struct nk_rect sw=nk_rect(r.x+8*f,r.y+8*f,r.w-16*f,35*f);nk_fill_rect(nk_window_get_canvas(ui),sw,3*f,t.field);nk_fill_rect(nk_window_get_canvas(ui),nk_rect(sw.x,sw.y,sw.w*.25f,sw.h),3*f,t.header);nk_stroke_rect(nk_window_get_canvas(ui),sw,3*f,f,i==2?t.accent:t.border);
            const struct nk_user_font *font=&a->settings_fonts[SET_FONT_SMALL];float tw=font->width(font->userdata,font->height,names[i],(int)strlen(names[i]));
            float tx=r.x+(r.w-tw-18*f)/2;struct nk_rect radio=nk_rect(tx,r.y+54*f,11*f,11*f);nk_stroke_circle(nk_window_get_canvas(ui),radio,f,c.muted);if(chosen)nk_fill_circle(nk_window_get_canvas(ui),nk_rect(radio.x+2*f,radio.y+2*f,7*f,7*f),c.edge);
            nk_draw_text(nk_window_get_canvas(ui),nk_rect(tx+18*f,r.y+53*f,r.w-18*f,font->height),names[i],(int)strlen(names[i]),font,nk_rgba(0,0,0,0),c.text);
            if(ps_ui_option_at_custom_focus(ui,r,"Darstellung",names[i],chosen,&focused))a->settings_draft.theme=i;
            a->theme_bounds[i]=r;if(chosen || focused)settings_focus_draw(a,4,r,focused);}
    }else{float cx=stack?x:x+width-160*f,cy=stack?y+23*f+hh+10*f:y,cw=stack?fminf(190*f,width):160*f;
        if(field<=3)settings_combo(a,field,cx,cy,cw,f);
        else if(field<=10){nk_layout_space_push(ui,nk_rect(cx,cy,cw,34*f));struct nk_rect r=nk_widget_bounds(ui);nk_bool value=(a->settings_draft.view_flags&item->flag)!=0;
            nk_style_push_font(ui,&a->settings_fonts[SET_FONT_BUTTON]);ps_ui_checkbox_named(ui,value?"An":"Aus",item->title,&value);nk_style_pop_font(ui);
            if(value)a->settings_draft.view_flags|=item->flag;else a->settings_draft.view_flags&=~item->flag;
            settings_focus_draw(a,item->focus,r,nk_sdl_accessibility_is_focus_target(ui,NULL,item->title));
            if(field==8)a->settings_bounds[2]=r;if(field==10)a->settings_bounds[3]=r;
        }else{unsigned i=field-11;bool changed=settings_edit(a,14+i,"settings-body",a->settings_numbers[i],16,cx,cy,fminf(105*f,cw),34*f);
            if(changed){a->settings_edited_numbers|=1u<<i;settings_numbers_read(a);}
            settings_text(a,cx+112*f,cy+8*f,30*f,"px",SET_FONT_BUTTON,18*f,c.muted);
        }}
    settings_line(a,x,y+height-17*f,width,c.border);return height;
}
static void settings_preview_draw(app *a,float x,float y,float width,float f,uint32_t changed) {
    settings_colors c=settings_theme(a->preferences.theme),p=settings_theme(a->settings_draft.theme);struct nk_context *ui=a->settings_ui;
    float pad=18*f,inner=fmaxf(1,width-2*pad);x+=pad;y+=24*f;
    settings_text(a,x,y,inner,"LIVE-VORSCHAU",SET_FONT_SMALL,15*f,c.muted);y+=23*f;
    char text[128];snprintf(text,sizeof text,"%s · aktueller Entwurf",a->settings_draft.theme==0?"Dunkel":a->settings_draft.theme==1?"Hell":"Hoher Kontrast");settings_text(a,x,y,inner,text,SET_FONT_SMALL,15*f,c.muted);y+=26*f;
    struct nk_rect mini=settings_slot(a,x,y,inner,360*f);a->settings_preview=mini;struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    nk_fill_rect(canvas,mini,7*f,p.field);nk_stroke_rect(canvas,mini,7*f,f,p.border);
    struct nk_rect bar=settings_slot(a,x,y,inner,32*f);nk_fill_rect(canvas,bar,7*f,p.header);
    settings_text(a,x+11*f,y+8*f,inner*.5f,"p· Physim",SET_FONT_SMALL,15*f,p.text);
    settings_text(a,x+inner*.53f,y+8*f,inner*.44f,a->settings_draft.view_flags&PS_VIEW_ORTHOGRAPHIC?"Orthografisch":"Perspektivisch",SET_FONT_SMALL,15*f,p.text);
    float top=y+46*f;settings_text(a,x+11*f,top,inner-22*f,"Pendelstudie",SET_FONT_PREVIEW_TITLE,26*f,p.text);
    snprintf(text,sizeof text,"Oberfläche %u px",a->settings_draft.ui_size);settings_text(a,x+11*f,top+31*f,inner-22*f,text,SET_FONT_PREVIEW,24*f,p.accent);
    settings_text(a,x+11*f,top+62*f,inner-22*f,"double energy =",SET_FONT_CODE,23*f,p.text);
    settings_text(a,x+11*f,top+85*f,inner-22*f,"  0.5 * mass * speed;",SET_FONT_CODE,23*f,p.text);
    struct nk_rect scene=settings_slot(a,x+11*f,y+162*f,inner-22*f,106*f);nk_stroke_rect(canvas,scene,3*f,f,p.border);
    if(a->settings_draft.view_flags&PS_VIEW_GRID){for(int i=1;i<6;i++)nk_stroke_line(canvas,scene.x+i*scene.w/6,scene.y+6*f,scene.x+i*scene.w/6,scene.y+scene.h-6*f,f,p.border);for(int i=1;i<4;i++)nk_stroke_line(canvas,scene.x+6*f,scene.y+i*scene.h/4,scene.x+scene.w-6*f,scene.y+i*scene.h/4,f,p.border);}
    float px=scene.x+scene.w*.46f,py=scene.y+19*f,mx=px+27*f,my=py+48*f;
    nk_stroke_line(canvas,px,py,mx,my,2*f,p.muted);nk_fill_circle(canvas,nk_rect(mx-11*f,my-11*f,22*f,22*f),p.header);nk_stroke_circle(canvas,nk_rect(mx-11*f,my-11*f,22*f,22*f),f,p.text);
    if(a->settings_draft.view_flags&PS_VIEW_VECTORS){nk_stroke_line(canvas,mx+20*f,my-3*f,mx+20*f,my+24*f,1.5f*f,p.accent);nk_stroke_line(canvas,mx+20*f,my+24*f,mx+17*f,my+18*f,1.5f*f,p.accent);nk_stroke_line(canvas,mx+20*f,my+24*f,mx+23*f,my+18*f,1.5f*f,p.accent);}
    if(a->settings_draft.view_flags&PS_VIEW_LABELS)settings_text(a,x+inner*.65f,y+200*f,inner*.32f,"Pendelmasse",SET_FONT_SMALL,15*f,p.text);
    if(a->settings_draft.view_flags&PS_VIEW_POINTS)nk_fill_circle(canvas,nk_rect(px-2*f,py-2*f,4*f,4*f),p.accent);
    if(a->settings_draft.view_flags&PS_VIEW_PATHS)nk_stroke_curve(canvas,mx-20*f,my+8*f,mx-10*f,my+13*f,mx+5*f,my+12*f,mx+13*f,my+5*f,f,p.accent);
    float space=inner-22*f,gap=5*f,sw=space*fminf(.25f,fmaxf(.1f,a->settings_draft.sidebar_width/2000.f)),iw=space*fminf(.25f,fmaxf(.1f,a->settings_draft.inspector_width/2000.f));
    struct nk_rect left=settings_slot(a,x+11*f,y+276*f,sw,37*f),mid=settings_slot(a,x+11*f+sw+gap,y+276*f,fmaxf(1,space-sw-iw-2*gap),37*f),right=settings_slot(a,x+inner-11*f-iw,y+276*f,iw,37*f);
    nk_fill_rect(canvas,left,2*f,p.header);nk_stroke_rect(canvas,left,2*f,f,p.border);nk_stroke_rect(canvas,mid,2*f,f,p.border);nk_fill_rect(canvas,right,2*f,p.header);nk_stroke_rect(canvas,right,2*f,f,p.border);
    if(a->settings_draft.view_flags&PS_VIEW_LOG){struct nk_rect log=settings_slot(a,x+11*f,y+321*f,inner-22*f,28*f);nk_fill_rect(canvas,log,0,p.header);settings_text(a,x+17*f,y+328*f,inner-34*f,"Protokoll · Bereit",SET_FONT_SMALL,15*f,p.muted);}
    y+=372*f;y+=settings_text(a,x,y,inner,"Schriftgrößen direkt dargestellt; Szene und Größenverhältnisse schematisch. Keine Simulation oder Quelldatei wird verändert.",SET_FONT_SMALL,16*f,c.muted)+18*f;
    settings_line(a,x,y,inner,c.border);y+=19*f;
    if(!changed)settings_text(a,x,y,inner,"Noch keine Änderungen",SET_FONT_LABEL,18*f,c.text);
    else{settings_text(a,x,y,inner,"Noch nicht übernommen",SET_FONT_LABEL,18*f,c.warn);y+=28*f;
        for(unsigned i=0;i<14;i++)if(changed&(1u<<i)){char value[96];settings_value(&a->settings_draft,i,value);snprintf(text,sizeof text,"%s: %s",settings_fields[i].title,value);y+=settings_text(a,x,y,inner,text,SET_FONT_HELP,17*f,c.muted)+7*f;}}
}
static void settings_style(app *a) {
    float f=a->preferences.ui_size/16.f,regular=a->workbench_regular_em>0?a->workbench_regular_em:1,bold=a->workbench_bold_em>0?a->workbench_bold_em:1,code=a->workbench_code_em>0?a->workbench_code_em:1;
    const float sizes[]={14,10.08f,18.2f,12.18f,10.92f,9.52f,11.2f,16,12,18.2f,14};
    for(unsigned i=0;i<11;i++){bool strong=i==2 || i==3 || i==9 || i==10;a->settings_fonts[i]=*(i==8?a->font_code:strong?a->font_title:a->font_ui);a->settings_fonts[i].height=sizes[i]*(i==8?code:strong?bold:regular)*f;}
    a->settings_fonts[SET_FONT_PREVIEW]=*a->ui_fonts[(a->settings_draft.ui_size-16)/2];a->settings_fonts[SET_FONT_PREVIEW].height=a->settings_draft.ui_size*regular;
    a->settings_fonts[SET_FONT_PREVIEW_TITLE]=*a->title_fonts[(a->settings_draft.ui_size-16)/2];a->settings_fonts[SET_FONT_PREVIEW_TITLE].height=a->settings_draft.ui_size*bold;
    a->settings_fonts[SET_FONT_CODE]=*a->code_fonts[(a->settings_draft.editor_size-12)/2];a->settings_fonts[SET_FONT_CODE].height=a->settings_draft.editor_size*code;
    style(a->settings_ui);struct nk_context *ui=a->settings_ui;nk_style_set_font(ui,&a->settings_fonts[SET_FONT_BODY]);nk_sdl_set_ui_size(ui,a->preferences.ui_size);
    ui->style.window.padding=ui->style.window.spacing=nk_vec2(0,0);ui->style.window.min_size=nk_vec2(1,1);ui->style.window.border=0;ui->style.edit.rounding=5*f;ui->style.edit.border=1;ui->style.edit.border_color=UI_COLOR(border);
}
typedef struct {float usable,nav,preview,nav_h,content_x,content_y,content_w,total;bool scroll;} settings_body_layout;
static settings_body_layout settings_body_measure(app *a,float width,float height,float f,uint32_t changes) {
    settings_body_layout r={0};bool narrow=width<700*f;
    for(unsigned pass=0;pass<2;pass++){
        r.usable=width-(pass?a->settings_ui->style.window.scrollbar_size.x:0);
        r.nav=a->settings_review || narrow?0:(r.usable<1200*f?170:190)*f;
        r.preview=!a->settings_review && r.usable>=1000*f?(r.usable<1200*f?255:290)*f:0;
        r.nav_h=narrow && !a->settings_review?ceilf(5.f/fmaxf(1,floorf((r.usable-20*f)/(150*f))))*43*f+20*f:0;
        r.content_x=r.nav+24*f;r.content_y=r.nav_h+24*f;r.content_w=fmaxf(1,r.usable-r.nav-r.preview-48*f);
        float h=104*f;
        if(a->settings_review){
            h=24*f+37*f+settings_text_height(a,SET_FONT_HELP,"Anzeigeeinheiten und unabhängige Fenster-/Layoutänderungen gehören nicht zu diesem Entwurf.",r.content_w,18*f)+18*f+54*f+28*f;
            ps_preferences current=settings_current(a);float col=r.content_w/3;
            for(unsigned i=0;i<14;i++)if(changes&(1u<<i)){
                char old[96],value[96];settings_value(&current,i,old);settings_value(&a->settings_draft,i,value);
                if(i>=11 && a->settings_invalid_numbers&(1u<<(i-11)))snprintf(value,sizeof value,"%s (ungültig)",a->settings_numbers[i-11]);
                float row=settings_text_height(a,SET_FONT_NAV,settings_fields[i].title,col-18*f,17.5f*f);
                row=fmaxf(row,settings_text_height(a,SET_FONT_NAV,old,col-18*f,17.5f*f));row=fmaxf(row,settings_text_height(a,SET_FONT_NAV,value,col-18*f,17.5f*f));h+=row+24*f;
            }
        }else{
            for(unsigned i=0;i<14;i++)if(settings_match(a,i))h+=settings_field_height(a,i,r.content_w,f);
            if(settings_units_match(a))h+=190*f;
        }
        if(*a->preferences_error)h+=settings_text_height(a,SET_FONT_HELP,a->preferences_error,r.content_w,18*f)+18*f;
        float preview_h=(550+settings_count(changes)*44)*f;
        r.total=fmaxf(500*f,r.content_y+h+24*f);
        if(r.preview)r.total=fmaxf(r.total,preview_h);
        else if(!a->settings_review)r.total=fmaxf(r.total,r.content_y+h+preview_h);
        r.scroll=r.total>height;if(!r.scroll || pass)break;
    }
    return r;
}
static void settings_view(app *a,int width,int height) {
    struct nk_context *ui=a->settings_ui;float f=a->preferences.ui_size/16.f;settings_colors c=settings_theme(a->preferences.theme);
    const ps_ui_palette *prior=ui_palette;ps_ui_palette palette=*prior;
    palette.window=c.panel;palette.header=c.header;palette.surface=c.header;palette.edit=c.field;palette.button=c.button;palette.border=c.border;palette.text=c.text;palette.muted=c.muted;palette.accent=c.accent;palette.primary=c.primary;palette.primary_text=c.on_primary;palette.button_hover=c.selected;
    ui_palette=&palette;settings_style(a);settings_activate_focus(a);ps_ui_font_layout *layout=ui->userdata.ptr;bool full=layout->accessibility_full_controls;layout->accessibility_full_controls=true;
    memset(a->settings_focus_bounds,0,sizeof a->settings_focus_bounds);memset(a->settings_fields,0,sizeof a->settings_fields);memset(a->settings_categories,0,sizeof a->settings_categories);
    uint32_t changes=settings_changes(a);unsigned count=settings_count(changes);bool dirty=count || !a->preferences_writable,narrow=width<700*f;
    float w=(float)width,header=70*f,search=63*f,footer_y=0;char badge[96];snprintf(badge,sizeof badge,count==1?"1 Änderung":count?"%u Änderungen":"Keine Änderungen",count);
    float action_widths[3];int action_ids[]={2,18,1};const char *action_names[]={"Abbrechen",a->settings_review?"Zurück":"Änderungen prüfen","Übernehmen"};
    struct nk_rect action_slots[3];float right=w-24*f,top=15*f,row=0;
    for(int i=2;i>=0;i--){const struct nk_user_font *font=&a->settings_fonts[SET_FONT_BUTTON];action_widths[i]=font->width(font->userdata,font->height,action_names[i],(int)strlen(action_names[i]))+24*f;}
    float total_actions=action_widths[0]+action_widths[1]+action_widths[2]+16*f;
    if(narrow || total_actions+350*f>w)top=52*f;
    for(int i=2;i>=0;i--){if(right-action_widths[i]<16*f){right=w-24*f;row+=42*f;}action_slots[i]=nk_rect(right-action_widths[i],top+row,action_widths[i],34*f);right-=action_widths[i]+8*f;}
    float footer=top+row+49*f;footer_y=fmaxf(header+search+1,(float)height-footer);a->settings_frame=nk_rect(0,0,w,(float)height);
    if(nk_begin(ui,"settings-header",nk_rect(0,0,w,header),NK_WINDOW_NO_SCROLLBAR)){nk_layout_space_begin(ui,NK_STATIC,header,20);
        ps_ui_accessibility_route(ui,true);settings_activate_focus(a);
        struct nk_rect header_bg=settings_slot(a,0,0,w,header);nk_fill_rect(nk_window_get_canvas(ui),header_bg,0,c.header);
        struct nk_rect icon=settings_slot(a,23*f,17*f,37*f,37*f);struct nk_command_buffer *canvas=nk_window_get_canvas(ui);nk_fill_rect(canvas,icon,9*f,c.field);nk_stroke_rect(canvas,icon,9*f,f,c.border);nk_stroke_circle(canvas,nk_rect(icon.x+11*f,icon.y+11*f,15*f,15*f),1.5f*f,c.accent);
        for(int i=0;i<8;i++){float t=i*PS_PI/4.f;nk_stroke_line(canvas,icon.x+18.5f*f+cosf(t)*8*f,icon.y+18.5f*f+sinf(t)*8*f,icon.x+18.5f*f+cosf(t)*11*f,icon.y+18.5f*f+sinf(t)*11*f,1.5f*f,c.accent);}
        settings_text(a,72*f,16*f,fmaxf(80*f,w-220*f),"Einstellungen",SET_FONT_HEADER,22*f,c.text);settings_text(a,72*f,41*f,fmaxf(80*f,w-110*f),"Persönlich · gilt für diese Anwendung",SET_FONT_SMALL,15*f,c.muted);
        if(w>520*f){const struct nk_user_font *font=&a->settings_fonts[SET_FONT_SMALL];int len=(int)strlen(badge);float bw=font->width(font->userdata,font->height,badge,len)+22*f;struct nk_rect br=settings_slot(a,w-60*f-bw,23*f,bw,24*f);nk_fill_rect(canvas,br,5*f,c.field);nk_stroke_rect(canvas,br,5*f,f,c.border);ps_ui_accessibility_label(ui,br,badge);
            /* Intrinsic, single-line status: wrapping at the exact glyph width
             * can put a second line outside this compact badge. */
            nk_draw_text(canvas,nk_rect(br.x+10*f,br.y+(br.h-font->height)/2,br.w-18*f,font->height),badge,len,font,nk_rgba(0,0,0,0),count?c.warn:c.muted);}
        struct nk_rect close=settings_slot(a,w-47*f,20*f,26*f,30*f);bool native=false;nk_stroke_line(canvas,close.x+9*f,close.y+10*f,close.x+17*f,close.y+18*f,1.4f*f,c.text);nk_stroke_line(canvas,close.x+17*f,close.y+10*f,close.x+9*f,close.y+18*f,1.4f*f,c.text);
        if(ps_ui_button_at_custom_focus(ui,close,"Einstellungen schließen",false,&native))settings_close(a);settings_focus_draw(a,19,close,native);settings_line(a,0,header-1,w,c.border);nk_layout_space_end(ui);}nk_end(ui);
    if(nk_begin(ui,"settings-search",nk_rect(0,header,w,search),NK_WINDOW_NO_SCROLLBAR)){nk_layout_space_begin(ui,NK_STATIC,search,20);
        ps_ui_accessibility_route(ui,true);settings_activate_focus(a);
        float sw=fminf(520*f,w-46*f-(*a->settings_search?40*f:0));if(settings_edit(a,17,"settings-search",a->settings_search,sizeof a->settings_search,23*f,13*f,sw,36*f))a->settings_review=false;
        struct nk_rect lens=settings_slot(a,32*f,25*f,8*f,8*f);nk_stroke_circle(nk_window_get_canvas(ui),lens,f,c.muted);nk_stroke_line(nk_window_get_canvas(ui),lens.x,lens.y+8*f,lens.x-3*f,lens.y+11*f,f,c.muted);
        if(!*a->settings_search)settings_text(a,51*f,23*f,sw-36*f,"Einstellungen suchen …",SET_FONT_BUTTON,17*f,c.muted);
        if(*a->settings_search && settings_button(a,26,"×",23*f+sw+6*f,13*f,34*f,36*f,false,true,false)){a->settings_search[0]=0;a->settings_focus=17;a->settings_keyboard=true;}
        if(w>900*f)settings_text(a,w-220*f,23*f,197*f,"Darstellung, Schrift, Sicherung …",SET_FONT_SMALL,17*f,c.muted);
        settings_line(a,0,search-1,w,c.border);nk_layout_space_end(ui);}nk_end(ui);
    float body_y=header+search,body_h=fmaxf(1,footer_y-body_y);a->settings_body=nk_rect(0,body_y,w,body_h);
    settings_body_layout body=settings_body_measure(a,w,body_h,f,changes);
    if(nk_begin(ui,"settings-body",a->settings_body,body.scroll?0:NK_WINDOW_NO_SCROLLBAR)){
        ps_ui_accessibility_route(ui,true);settings_activate_focus(a);
        /* One editor owns this pane. Later inactive size fields must not
         * unfocus the selected field or stop SDL's text-input channel. */
        if(a->settings_keyboard)nk_edit_unfocus(ui);
        float usable=body.usable,nav=body.nav,preview=body.preview;
        float content_x=body.content_x,content_y=body.content_y,content_w=body.content_w,total=body.total;
        nk_layout_space_begin(ui,NK_STATIC,total,512);struct nk_rect whole=settings_slot(a,0,0,usable,total);struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
        if(!narrow && !a->settings_review){nk_fill_rect(canvas,nk_rect(whole.x,whole.y,nav,fmaxf(total,body_h)),0,c.field);nk_stroke_line(canvas,whole.x+nav,whole.y,whole.x+nav,whole.y+fmaxf(total,body_h),1,c.border);}
        float nx=12*f,ny=19*f;
        for(unsigned i=0;i<5 && !a->settings_review;i++){float nw=narrow?fmaxf(125*f,(usable-20*f)/fmaxf(1,floorf((usable-20*f)/(150*f)))-4*f):nav-24*f;
            if(narrow && nx+nw>usable-8*f){nx=10*f;ny+=43*f;}
            struct nk_rect r=settings_slot(a,nx,ny,nw,41*f);a->settings_categories[i]=r;bool selected=!a->settings_review && !*a->settings_search && a->settings_group==i,native=false;
            if(selected){nk_fill_rect(canvas,r,5*f,c.selected);nk_fill_rect(canvas,nk_rect(r.x,r.y,3*f,r.h),0,c.edge);}
            const struct nk_user_font *font=&a->settings_fonts[SET_FONT_NAV];float ix=r.x+10*f,iy=r.y+16*f;
            if(i==0){nk_stroke_circle(canvas,nk_rect(ix,iy,8*f,8*f),f,c.muted);nk_fill_rect(canvas,nk_rect(ix+4*f,iy+f,3*f,6*f),0,c.muted);}
            else if(i==2){nk_stroke_line(canvas,ix,iy+7*f,ix+7*f,iy, f,c.muted);nk_stroke_line(canvas,ix+2*f,iy,ix+7*f,iy,f,c.muted);nk_stroke_line(canvas,ix+7*f,iy,ix+7*f,iy+5*f,f,c.muted);}
            else if(i==3){nk_stroke_rect(canvas,nk_rect(ix,iy,8*f,8*f),0,f,c.muted);for(int k=1;k<3;k++)nk_stroke_line(canvas,ix+k*8*f/3,iy,ix+k*8*f/3,iy+8*f,f,c.muted);}
            else{const char *symbol=i==1?"⌘":"m";nk_draw_text(canvas,nk_rect(ix,r.y+(r.h-font->height)/2,18*f,font->height),symbol,(int)strlen(symbol),font,nk_rgba(0,0,0,0),c.muted);}
            nk_draw_text(canvas,nk_rect(r.x+35*f,r.y+(r.h-font->height)/2,r.w-40*f,font->height),settings_groups[i],(int)strlen(settings_groups[i]),font,nk_rgba(0,0,0,0),c.text);
            if(ps_ui_option_at_custom_focus(ui,r,"Kategorien",settings_groups[i],selected,&native)){a->settings_group=i;a->settings_search[0]=0;a->settings_review=false;}
            settings_focus_draw(a,20+i,r,native);if(narrow)nx+=nw+4*f;else ny+=41*f;}
        if(!narrow && !a->settings_review)settings_text(a,20*f,ny+22*f,nav-36*f,"Persönliche Darstellung. Keine Modellparameter oder Laufdaten.",SET_FONT_SMALL,16*f,c.muted);
        if(*a->preferences_error){content_y+=settings_text(a,content_x,content_y,content_w,a->preferences_error,SET_FONT_HELP,18*f,c.error)+18*f;}
        if(a->settings_review){
            settings_text(a,content_x,content_y,content_w,"VOR DEM ÜBERNEHMEN",SET_FONT_SMALL,15*f,c.muted);content_y+=24*f;
            char title[96];snprintf(title,sizeof title,count==1?"1 Änderung prüfen":"%u Änderungen prüfen",count);settings_text(a,content_x,content_y,content_w,title,SET_FONT_TITLE,28*f,c.text);content_y+=37*f;
            content_y+=settings_text(a,content_x,content_y,content_w,"Anzeigeeinheiten und unabhängige Fenster-/Layoutänderungen gehören nicht zu diesem Entwurf.",SET_FONT_HELP,18*f,c.muted)+18*f;
            if(settings_button(a,25,"← Zurück zu den Einstellungen",content_x,content_y,fminf(260*f,content_w),34*f,false,true,false)){a->settings_review=false;a->settings_focus=18;a->settings_focus_scroll=true;}content_y+=54*f;
            float col=content_w/3;const char *heads[]={"Einstellung","Aktuell","Entwurf"};
            for(unsigned j=0;j<3;j++)settings_text(a,content_x+j*col+9*f,content_y,col-18*f,heads[j],SET_FONT_SMALL,15*f,c.muted);
            content_y+=28*f;settings_line(a,content_x,content_y,content_w,c.border);
            ps_preferences current=settings_current(a);
            for(unsigned i=0;i<14;i++)if(changes&(1u<<i)){
                char old[96],value[96];settings_value(&current,i,old);settings_value(&a->settings_draft,i,value);
                if(i>=11 && a->settings_invalid_numbers&(1u<<(i-11)))snprintf(value,sizeof value,"%s (ungültig)",a->settings_numbers[i-11]);
                const char *cells[]={settings_fields[i].title,old,value};float row=0;
                for(unsigned j=0;j<3;j++)row=fmaxf(row,settings_text_height(a,SET_FONT_NAV,cells[j],col-18*f,17.5f*f));row+=24*f;
                struct nk_rect selected=settings_slot(a,content_x+2*col,content_y,col,row);nk_fill_rect(canvas,selected,0,c.selected);
                for(unsigned j=0;j<3;j++)settings_text(a,content_x+j*col+9*f,content_y+12*f,col-18*f,cells[j],SET_FONT_NAV,17.5f*f,c.text);
                content_y+=row;settings_line(a,content_x,content_y,content_w,c.border);
            }
        }
        else{const char *title=*a->settings_search?"Suchergebnisse":settings_groups[a->settings_group];const char *eyebrows[]={"DARSTELLUNG","EDITOR & SICHERUNG","SIMULATION","ARBEITSFLÄCHE","ANZEIGEEINHEITEN"};settings_text(a,content_x,content_y,content_w,*a->settings_search?"SUCHERGEBNISSE":eyebrows[a->settings_group],SET_FONT_SMALL,15*f,c.muted);content_y+=22*f;
            settings_text(a,content_x,content_y,content_w,title,SET_FONT_TITLE,28*f,c.text);content_y+=34*f;
            settings_text(a,content_x,content_y,content_w,*a->settings_search?"Passende Einstellungen in allen Kategorien.":settings_notes[a->settings_group],SET_FONT_HELP,18*f,c.muted);content_y+=40*f;
            unsigned matches=0;for(unsigned i=0;i<14;i++)if(settings_match(a,i)){matches++;content_y+=settings_field_draw(a,i,content_x,content_y,content_w,f);}
            if(settings_units_match(a)){matches++;settings_text(a,content_x,content_y,content_w,"Anzeigeeinheiten für Messkanäle",SET_FONT_LABEL,19*f,c.text);content_y+=29*f;
                content_y+=settings_text(a,content_x,content_y,content_w,"Persönliche Auswahl je Kanal und SI-Dimension. Eigene Aufgabe mit eigenem Anwenden; Rohdaten und CSV bleiben unverändert.",SET_FONT_HELP,18*f,c.muted)+18*f;
                const ps_channel *channel=settings_channel(a);if(channel){char current[160];ps_display_unit unit=channel_display_unit(a,channel);snprintf(current,sizeof current,"%s [%s]",channel->name,unit.symbol);settings_text(a,content_x,content_y,content_w,current,SET_FONT_BUTTON,20*f,c.text);content_y+=28*f;}
                if(settings_button(a,27,"Anzeigeeinheit bearbeiten …",content_x,content_y,fminf(content_w,240*f),34*f,false,channel!=NULL,false))channel_unit_open_from(a,channel,a->settings_window,settings_focus_name(27));
                if(!channel)settings_text(a,content_x,content_y+46*f,content_w,"Noch keine Messkanäle verfügbar. Ein Experiment oder einen gespeicherten Lauf öffnen.",SET_FONT_HELP,18*f,c.muted);}
            if(!matches){settings_text(a,content_x,content_y,content_w,"Keine passenden Einstellungen.",SET_FONT_LABEL,22*f,c.text);content_y+=34*f;
                if(settings_button(a,26,"Suche zurücksetzen",content_x,content_y,fminf(content_w,190*f),34*f,false,true,false))a->settings_search[0]=0;}}
        if(preview){struct nk_rect edge=settings_slot(a,usable-preview,0,1,fmaxf(total,body_h));nk_stroke_line(canvas,edge.x,edge.y,edge.x,edge.y+fmaxf(total,body_h),1,c.border);settings_preview_draw(a,usable-preview,0,preview,f,changes);}
        else if(!a->settings_review)settings_preview_draw(a,nav,content_y+26*f,usable-nav,f,changes);
        nk_layout_space_end(ui);
    }nk_end(ui);
    if(nk_begin(ui,"settings-footer",nk_rect(0,footer_y,w,(float)height-footer_y),NK_WINDOW_NO_SCROLLBAR)){nk_layout_space_begin(ui,NK_STATIC,footer,30);
        ps_ui_accessibility_route(ui,true);settings_activate_focus(a);
        struct nk_rect bg=settings_slot(a,0,0,w,footer);nk_fill_rect(nk_window_get_canvas(ui),bg,0,c.header);settings_line(a,0,0,w,c.border);
        a->settings_bounds[5]=nk_rect(24*f,15*f,100*f,34*f);if(settings_button(a,3,"Standardwerte",24*f,15*f,100*f,34*f,false,true,false))settings_defaults_draft(a);
        if(w>450*f)settings_text(a,137*f,24*f,fmaxf(1,(top>20*f?w-160*f: right-150*f)),dirty?"Änderungen erst nach Übernehmen.":"Keine Änderungen.",SET_FONT_SMALL,17*f,c.muted);
        for(unsigned i=0;i<3;i++){struct nk_rect r=action_slots[i];if(i==0)a->settings_bounds[4]=r;else if(i==2)a->settings_bounds[0]=r;
            if(settings_button(a,action_ids[i],action_names[i],r.x,r.y,r.w,r.h,i==2,i==0 || dirty,false)){
                if(i==0)settings_close(a);else if(i==1){a->settings_review=!a->settings_review;nk_window_set_scroll(ui,0,0);}else a->settings_apply_requested=true;}}
        nk_layout_space_end(ui);}nk_end(ui);
    settings_activate_focus(a);
    a->settings_popup_close=false;layout->accessibility_full_controls=full;ui_palette=prior;
}
static bool settings_window_render(app *a) {
    if(!a->settings_open || !a->settings_window || SDL_GetWindowFlags(a->settings_window)&(SDL_WINDOW_HIDDEN|SDL_WINDOW_MINIMIZED))return true;
    if(!ps_graphics_make_current(a->settings_graphics))return false;
    settings_colors c=settings_theme(a->preferences.theme);ps_graphics_background(a->settings_graphics,c.panel.r,c.panel.g,c.panel.b);
    int width,height;SDL_GetWindowSize(a->settings_window,&width,&height);settings_view(a,width,height);
    nk_sdl_update_TextInput(a->settings_ui);bool ok=nk_sdl_render(a->settings_ui);
    if(*a->settings_capture){ok=ps_graphics_capture(a->settings_graphics,a->settings_capture) && ok;a->settings_capture[0]=0;}
    if(!ps_graphics_present(a->settings_graphics))ok=false;return ps_graphics_make_current(a->graphics) && ok;
}
