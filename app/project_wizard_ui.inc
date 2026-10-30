/* CSS roles from design-options/options.css; native fields and file actions. */
static struct nk_rect manager_slot(app *a,float x,float y,float w,float h) {
    nk_layout_space_push(a->manager_ui,nk_rect(x,y,w,h));struct nk_rect r;nk_widget(&r,a->manager_ui);return r;
}
static float manager_text_height(app *a,const char *text,unsigned role,float width,float pitch) {
    const struct nk_user_font *font=&a->manager_fonts[role];int n=(int)strlen(text),at=0,lines=0;
    while(at<n){int used=ps_ui_text_line(font,text+at,n-at,fmaxf(1,width),false);at+=used>0?used:1;lines++;}
    return fmaxf(pitch,lines*pitch);
}
static float manager_text(app *a,float x,float y,float w,const char *text,unsigned role,float pitch,struct nk_color color) {
    struct nk_user_font *font=&a->manager_fonts[role];int n=(int)strlen(text),at=0;
    float height=manager_text_height(a,text,role,w,pitch);struct nk_rect r=manager_slot(a,x,y,w,height);
    ps_ui_accessibility_label(a->manager_ui,r,text);float top=r.y+(pitch-font->height)/2;
    if(role==0) {
        float left=r.x,spacing=1.7f*a->preferences.ui_size/16.f;
        while(at<n){nk_rune rune;int used=nk_utf_decode(text+at,&rune,n-at);if(used<=0)break;
            float advance=font->width(font->userdata,font->height,text+at,used);
            nk_draw_text(nk_window_get_canvas(a->manager_ui),nk_rect(left,top,advance+1,font->height),text+at,used,font,nk_rgba(0,0,0,0),color);left+=advance+spacing;at+=used;}
        return height;
    }
    while(at<n){int used=ps_ui_text_line(font,text+at,n-at,fmaxf(1,w),false);if(used<=0)used=1;
        nk_draw_text(nk_window_get_canvas(a->manager_ui),nk_rect(r.x,top,w,font->height),text+at,used,font,nk_rgba(0,0,0,0),color);at+=used;top+=pitch;}
    return height;
}
static const char *manager_button_label(app *a,int id) {
    return id==1?"Auswählen …":id==7 && a->manager_step<2?"Weiter →":project_manager_name(a,id);
}
static float manager_button_width(app *a,int id,float f) {
    const struct nk_user_font *font=&a->manager_fonts[id==1?5:4];const char *label=manager_button_label(a,id);
    return font->width(font->userdata,font->height,label,(int)strlen(label))+24.4f*f;
}
static bool manager_button(app *a,int id,float x,float y,float w,float h,bool primary) {
    struct nk_context *ui=a->manager_ui;struct nk_rect r=manager_slot(a,x,y,w,h);float f=a->preferences.ui_size/16.f;
    bool hover=nk_input_is_mouse_hovering_rect(&ui->input,r);
    nk_fill_rect(nk_window_get_canvas(ui),r,7*f,primary?UI_COLOR(primary):hover?UI_COLOR(button_hover):UI_COLOR(surface));
    nk_stroke_rect(nk_window_get_canvas(ui),r,7*f,f,primary?UI_COLOR(primary):UI_COLOR(border));
    const char *label=manager_button_label(a,id);const struct nk_user_font *font=&a->manager_fonts[id==1?5:4];
    float tw=font->width(font->userdata,font->height,label,(int)strlen(label));
    nk_draw_text(nk_window_get_canvas(ui),nk_rect(r.x+(r.w-tw)/2,r.y+(r.h-font->height)/2,tw+1,font->height),label,(int)strlen(label),font,nk_rgba(0,0,0,0),primary?UI_COLOR(primary_text):UI_COLOR(text));
    bool pressed=ps_ui_button_at(ui,r,project_manager_name(a,id),false);project_manager_focus_draw(a,id,r);return pressed;
}
static void manager_language(app *a,int id,float x,float y,float w,float f) {
    struct nk_context *ui=a->manager_ui;const char *label=project_manager_name(a,id);int *language=id==5?&a->manager_experiment_language:&a->template_analysis_language;
    manager_text(a,x,y,w,label,8,17.794f*f,UI_COLOR(text));
    nk_style_push_font(ui,&a->manager_fonts[8]);
    nk_layout_space_push(ui,nk_rect(x,y+23.794f*f,w,35.194f*f));struct nk_rect r=nk_widget_bounds(ui);
    if(ps_ui_combo_begin_named_scaled(ui,label,*language?"Physim":"C",nk_vec2(w,94*f),f)) {
        if(a->manager_popup_close){nk_combo_close(ui);a->manager_popup_close=false;}
        else {
            ps_ui_row_exact_dynamic(ui,32*f,1);
            if(nk_button_label(ui,"C")){*language=0;nk_combo_close(ui);}
            if(nk_button_label(ui,"Physim")){*language=1;nk_combo_close(ui);}
        }
        nk_combo_end(ui);
    }
    nk_style_pop_font(ui);project_manager_focus_draw(a,id,r);
}
static void manager_cube(struct nk_command_buffer *canvas,struct nk_rect r,struct nk_color color) {
    float f=r.w/24,x=r.x,y=r.y;
    float points[]={x+12*f,y+2*f,x+21*f,y+7*f,x+21*f,y+17*f,x+12*f,y+22*f,x+3*f,y+17*f,x+3*f,y+7*f,x+12*f,y+2*f};
    nk_stroke_polyline(canvas,points,7,1.5f*f,color);
    nk_stroke_line(canvas,x+3*f,y+7*f,x+12*f,y+12*f,1.5f*f,color);nk_stroke_line(canvas,x+12*f,y+12*f,x+21*f,y+7*f,1.5f*f,color);
    nk_stroke_line(canvas,x+12*f,y+12*f,x+12*f,y+22*f,1.5f*f,color);
}
static float manager_analysis_checkbox(app *a,float x,float y,float width,float f) {
    const char *label="Nur Auswertung gespeicherter Läufe";
    float height=manager_text_height(a,label,4,width-28*f,18.445f*f);
    struct nk_context *ui=a->manager_ui;nk_layout_space_push(ui,nk_rect(x,y,width,height));
    struct nk_rect bounds=nk_widget_bounds(ui);a->project_kind_bounds=bounds;
    struct nk_style_toggle saved=ui->style.checkbox;
    ui->style.checkbox.normal=ui->style.checkbox.hover=ui->style.checkbox.active=nk_style_item_hide();
    ui->style.checkbox.cursor_normal=ui->style.checkbox.cursor_hover=nk_style_item_hide();
    ui->style.checkbox.text_normal=ui->style.checkbox.text_hover=ui->style.checkbox.text_active=nk_rgba(0,0,0,0);
    nk_bool only=a->manager_analysis_only;
    if(ps_ui_checkbox_named(ui,label,label,&only))a->manager_analysis_only=only!=0;
    ui->style.checkbox=saved;
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);struct nk_rect box=nk_rect(bounds.x+4*f,bounds.y+(bounds.h-13*f)/2,13*f,13*f);
    nk_fill_rect(canvas,box,2*f,only?UI_COLOR(primary):UI_COLOR(edit));nk_stroke_rect(canvas,box,2*f,f,only?UI_COLOR(primary):UI_COLOR(muted));
    if(only){nk_stroke_line(canvas,box.x+3*f,box.y+6*f,box.x+5.5f*f,box.y+9*f,1.5f*f,UI_COLOR(primary_text));nk_stroke_line(canvas,box.x+5.5f*f,box.y+9*f,box.x+10*f,box.y+3.5f*f,1.5f*f,UI_COLOR(primary_text));}
    struct nk_user_font *font=&a->manager_fonts[4];
    int at=0,length=(int)strlen(label);float top=bounds.y+(18.445f*f-font->height)/2;
    while(at<length){int used=ps_ui_text_line(font,label+at,length-at,fmaxf(1,bounds.w-28*f),false);if(used<=0)used=1;
        nk_draw_text(canvas,nk_rect(bounds.x+28*f,top,bounds.w-28*f,font->height),label+at,used,font,nk_rgba(0,0,0,0),UI_COLOR(text));at+=used;top+=18.445f*f;}
    project_manager_focus_draw(a,3,bounds);
    return height;
}
static float manager_step_layout(app *a,float width,float f,bool compact,struct nk_rect slots[3]) {
    float px=(compact?18:27)*f,py=(compact?12:15)*f,gap=(compact?13:20)*f,pitch=(compact?15.19f:17.36f)*f;
    const char *names[]={"01 · Startpunkt","02 · Projektangaben","03 · Prüfen"};
    const struct nk_user_font *font=&a->manager_fonts[compact?10:6];float x=px,y=py;
    for(unsigned i=0;i<3;i++){
        float w=font->width(font->userdata,font->height,names[i],(int)strlen(names[i]));
        if(x>px && x+w>width-px){x=px;y+=pitch+gap;}
        slots[i]=nk_rect(x,y,w,pitch);x+=w+gap;
    }
    return y+pitch+py+f;
}
static float manager_footer_layout(app *a,float width,float f,bool compact,struct nk_rect slots[3]) {
    const char *note="Vorhandene Projektdateien werden nicht überschrieben.";
    float pad=(compact?18:24)*f,available=fmaxf(1,width-2*pad),x=0,y=0;
    int ids[]={8,9,7};float widths[3],total=0;unsigned count=0,rows=1;
    for(unsigned i=0;i<3;i++){widths[i]=i==1 && !a->manager_step?0:manager_button_width(a,ids[i],f);if(widths[i]){total+=widths[i];count++;}}
    total+=(count-1)*8*f;float group=fminf(total,available),left=width-pad-group;
    float top=compact?15*f+manager_text_height(a,note,5,available,16.275f*f)+17*f:16*f;
    for(unsigned i=0;i<3;i++){
        slots[i]=nk_rect(0,0,0,0);if(!widths[i])continue;
        if(x>0 && x+widths[i]>group+.01f){x=0;y+=33.885f*f+8*f;rows++;}
        slots[i]=nk_rect(left+x,top+y,widths[i],33.885f*f);x+=widths[i]+8*f;
    }
    return compact?top+rows*33.885f*f+(rows-1)*8*f+15*f:66.885f*f+(rows-1)*(33.885f+8)*f;
}
static void project_manager_view(app *a,int width,int height) {
    struct nk_context *ui=a->manager_ui;
    if(!a->project_manager || !ui)return;
    a->manager_error_bounds=a->manager_error_text_bounds=nk_rect(0,0,0,0);
    if(a->manager_keyboard && nk_sdl_accessibility_has_focus(ui) && !nk_sdl_accessibility_keyboard_owned(ui))a->manager_keyboard=false;
    struct nk_style saved=ui->style;const ps_ui_palette *prior=ui_palette;ps_ui_palette palette=*prior;
    ps_ui_font_layout *layout=ui->userdata.ptr;bool previous_full=layout->accessibility_full_controls;layout->accessibility_full_controls=true;
    float f=a->preferences.ui_size/16.f;bool light=a->preferences.theme==PS_THEME_LIGHT;
    if(a->preferences.theme!=PS_THEME_HIGH_CONTRAST) {
        palette.window=light?nk_rgb(250,251,253):nk_rgb(32,35,42);palette.edit=light?nk_rgb(255,255,255):nk_rgb(24,27,33);
        palette.surface=light?nk_rgb(240,243,247):nk_rgb(39,43,51);palette.border=light?nk_rgb(198,207,221):nk_rgb(60,65,76);
        palette.text=light?nk_rgb(32,44,60):nk_rgb(240,242,247);palette.muted=light?nk_rgb(88,104,124):nk_rgb(169,178,193);
        palette.accent=light?nk_rgb(32,92,170):nk_rgb(154,195,255);palette.primary=light?nk_rgb(32,93,172):nk_rgb(35,102,196);
        palette.selection_active=palette.button_hover=light?nk_rgb(220,233,250):nk_rgb(41,62,91);
    }
    ui_palette=&palette;style(ui);
    bool compact=width<=560*f;
    const float sizes[]={9.8f,compact?19.6f:24.5f,14.7f,12.6f,11.9f,10.5f,11.2f,11.2f,11.48f,10.92f,9.8f,11.2f};
    float previous_height=a->manager_fonts[4].height;
    for(unsigned i=0;i<12;i++){bool strong=i<=3 || i==11,code=i==7;a->manager_fonts[i]=*(code?a->font_code:strong?a->font_title:a->font_ui);a->manager_fonts[i].height=sizes[i]*(code?a->workbench_code_em:strong?a->workbench_bold_em:a->workbench_regular_em)*f;}
    if(previous_height>0 && fabsf(previous_height-a->manager_fonts[4].height)>.01f && a->manager_focus_known)a->manager_focus_reveal=true;
    nk_style_set_font(ui,&a->manager_fonts[4]);ui->style.window.padding=ui->style.window.spacing=nk_vec2(0,0);
    ui->style.window.fixed_background=nk_style_item_hide();
    const char *subtitle="Drei kurze Schritte: Startpunkt, Projektangaben, Prüfen. Eingaben bleiben beim Zurückgehen erhalten.";
    char target[4400];snprintf(target,sizeof target,"%s/%s",a->manager_parent,a->manager_name);
    float outer=fmaxf(100,fminf(1080*f,(float)width)),w=outer-2,header=0,subtitle_y=0,steps=0,body=0,total=0,h=0;
    float footer=0,error=0;bool narrow=compact;unsigned columns=narrow?1:2;
    float body_pad=(narrow?20:27)*f,header_pad=(narrow?18:27)*f,header_top=(narrow?18:23)*f;
    float heading_pitch=(narrow?30.38f:37.975f)*f;
    float card_rows[8]={0};struct nk_rect step_slots[3],footer_slots[3];
    a->manager_scrollbar=0;
    for(unsigned pass=0;pass<2;pass++) {
        float inside=fminf(850*f,w)-2*body_pad;
        subtitle_y=header_top+19.19f*f+manager_text_height(a,"Ein neues Projekt.",1,w-2*header_pad-46*f,heading_pitch)+7*f;
        header=subtitle_y+manager_text_height(a,subtitle,6,w-2*header_pad-46*f,17.36f*f)+(narrow?18:19)*f;
        steps=manager_step_layout(a,w,f,narrow,step_slots);footer=manager_footer_layout(a,w,f,narrow,footer_slots);
        error=*a->manager_error?fmaxf(74*f,manager_text_height(a,a->manager_error,4,w-64*f,18.445f*f)+32*f):0;
        body=narrow?0:380*f;
        if(a->manager_step==0){
            body=2*body_pad+manager_text_height(a,"Womit möchtest du beginnen?",2,inside,22.785f*f)+14*f+
                manager_text_height(a,"Nur Auswertung gespeicherter Läufe",4,inside-28*f,18.445f*f)+16*f;
            memset(card_rows,0,sizeof card_rows);float content=(inside-(columns-1)*12*f)/columns-36*f;
            for(unsigned i=0;i<8;i++){
                float height=48.7f*f+manager_text_height(a,manager_templates[i],3,content,19.53f*f)+3*f+
                    manager_text_height(a,manager_descriptions[i],5,content,16.275f*f)+18*f;
                card_rows[i/columns]=fmaxf(card_rows[i/columns],height);
            }
            if(!a->manager_analysis_only)for(unsigned row=0;row<8/columns;row++)body+=card_rows[row]+(row?12*f:0);
            if(!narrow)body=fmaxf(380*f,body);
        }
        if(a->manager_step==1)body=fmaxf(body,2*body_pad+(64.988f+12+64.12f+20+58.988f+(narrow?40.326f:0)+(narrow && !a->manager_analysis_only?78.988f:0))*f+
            manager_text_height(a,"Ein eigener Ordner wird unter dem gewählten Ort angelegt.",10,inside,15.19f*f)+
            manager_text_height(a,"Hier wird dein Projektordner angelegt.",10,inside,15.19f*f));
        if(a->manager_step==2) {
            const char *values[]={a->manager_analysis_only?"Nur Auswertung":"Experiment + Auswertung",a->manager_analysis_only?"Keine":manager_templates[a->manager_template],a->manager_analysis_only?"Keines":a->manager_experiment_language?"Physim":"C",a->template_analysis_language?"Physim":"C"};
            body=2*body_pad+(23.19f+15.215f+12+12+16.926f+7+12+22+30)*f+
                manager_text_height(a,"Dein neuer Arbeitsraum.",2,inside,22.785f*f);
            for(unsigned i=0;i<4;i++)body+=manager_text_height(a,values[i],6,inside*.5f,17.36f*f)+10*f;
            body+=manager_text_height(a,target,7,inside-28*f,20.72f*f);
            body+=manager_text_height(a,a->manager_name,11,inside-30*f,22.4f*f)+(a->manager_analysis_only?3:4)*22.4f*f;
            if(!narrow)body=fmaxf(380*f,body);
        }
        total=header+steps+body+error+footer;h=fminf(total+2,(float)height);
        if(pass==0 && total+2>h){a->manager_scrollbar=ui->style.window.scrollbar_size.x;w-=a->manager_scrollbar;continue;}break;
    }
    a->manager_bounds=nk_rect((width-outer)/2,(height-h)/2,outer,h);
    struct nk_window *window=nk_window_find(ui,"Neues Projekt");
    if(window){window->scrollbar.x=0;window->scrollbar.y=(nk_uint)fminf(window->scrollbar.y,total+2>h?fmaxf(0,total+18*f-h):0);if(!window->popup.active)nk_window_set_focus(ui,"Neues Projekt");}
    ui->style.window.fixed_background=nk_style_item_color(UI_COLOR(window));ui->style.window.rounding=14*f;ui->style.window.border=1;ui->style.window.border_color=UI_COLOR(border);
    nk_window_show(ui,"Neues Projekt",NK_SHOWN);
    if(nk_begin(ui,"Neues Projekt",a->manager_bounds,NK_WINDOW_BORDER|(total+2>h?0:NK_WINDOW_NO_SCROLLBAR))) {
        if(!window)nk_window_set_focus(ui,"Neues Projekt");nk_sdl_window_raise(ui,"Neues Projekt");
        w=nk_window_get_content_region(ui).w;float body_w=fminf(850*f,w),bx=(w-body_w)/2,x=bx+body_pad,y=header+steps+body_pad,inside=body_w-2*body_pad;
        memset(a->manager_focus_bounds,0,sizeof a->manager_focus_bounds);nk_layout_space_begin(ui,NK_STATIC,total,100);
        manager_text(a,header_pad,header_top,w-2*header_pad-46*f,"NEUES PROJEKT",0,15.19f*f,UI_COLOR(muted));
        manager_text(a,header_pad,header_top+19.19f*f,w-2*header_pad-46*f,"Ein neues Projekt.",1,heading_pitch,UI_COLOR(text));
        manager_text(a,header_pad,subtitle_y,w-2*header_pad-46*f,subtitle,6,17.36f*f,UI_COLOR(muted));
        struct nk_rect close=manager_slot(a,w-header_pad-32*f,header_top+f,32*f,32*f);struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
        nk_stroke_line(canvas,close.x+10*f,close.y+10*f,close.x+22*f,close.y+22*f,1.5f*f,UI_COLOR(text));nk_stroke_line(canvas,close.x+22*f,close.y+10*f,close.x+10*f,close.y+22*f,1.5f*f,UI_COLOR(text));
        if(ps_ui_button_at(ui,close,"Projektdialog schließen",false))project_manager_close(a);project_manager_focus_draw(a,10,close);
        struct nk_rect line=manager_slot(a,0,header,w,1);nk_fill_rect(canvas,line,0,UI_COLOR(border));
        const char *names[]={"01 · Startpunkt","02 · Projektangaben","03 · Prüfen"};
        for(unsigned i=0;i<3;i++){
            struct nk_rect slot=step_slots[i];a->manager_steps[i]=manager_slot(a,slot.x,header+slot.y,slot.w,slot.h);
            manager_text(a,slot.x,header+slot.y,slot.w+1,names[i],narrow?10:6,slot.h,i==(unsigned)a->manager_step?UI_COLOR(accent):UI_COLOR(muted));}
        line=manager_slot(a,0,header+steps,w,1);nk_fill_rect(canvas,line,0,UI_COLOR(border));a->manager_body=manager_slot(a,bx,header+steps,body_w,body);
        if(a->manager_step==0) {
            y+=manager_text(a,x,y,inside,"Womit möchtest du beginnen?",2,22.785f*f,UI_COLOR(text))+14*f;
            y+=manager_analysis_checkbox(a,x,y,inside,f)+16*f;
            if(!a->manager_analysis_only)for(unsigned i=0;i<8;i++) {
                float cw=(inside-(columns-1)*12*f)/columns,cx=x+(i%columns)*(cw+12*f),cy=y;
                for(unsigned row=0;row<i/columns;row++)cy+=card_rows[row]+12*f;
                struct nk_rect r=manager_slot(a,cx,cy,cw,card_rows[i/columns]);bool selected=a->manager_template==(int)i,hover=nk_input_is_mouse_hovering_rect(&ui->input,r);
                nk_fill_rect(canvas,r,9*f,selected?UI_COLOR(selection_active):hover?UI_COLOR(button_hover):UI_COLOR(edit));nk_stroke_rect(canvas,r,9*f,f,selected?UI_COLOR(accent):UI_COLOR(border));
                manager_cube(canvas,nk_rect(r.x+18*f,r.y+18*f,18*f,18*f),UI_COLOR(text));
                float title_height=manager_text(a,cx+18*f,cy+48.7f*f,cw-36*f,manager_templates[i],3,19.53f*f,UI_COLOR(text));
                manager_text(a,cx+18*f,cy+48.7f*f+title_height+3*f,cw-36*f,manager_descriptions[i],5,16.275f*f,UI_COLOR(muted));
                bool native_focus=false;
                if(ps_ui_option_at_custom_focus(ui,r,"Vorlage",manager_templates[i],selected,&native_focus))a->manager_template=(int)i;
                if(native_focus && !a->manager_keyboard)nk_stroke_rect(canvas,r,9*f,2*f,UI_COLOR(accent));
                project_manager_focus_draw(a,11+(int)i,r);
            }
        } else if(a->manager_step==1) {
            manager_text(a,x,y,inside,"Projektname",8,17.794f*f,UI_COLOR(text));nk_style_push_font(ui,&a->manager_fonts[8]);
            nk_layout_space_push(ui,nk_rect(x,y+23.794f*f,inside,35.194f*f));project_manager_field(a,2,a->manager_name,sizeof a->manager_name);nk_style_pop_font(ui);
            y+=64.988f*f+manager_text(a,x,y+64.988f*f,inside,"Ein eigener Ordner wird unter dem gewählten Ort angelegt.",10,15.19f*f,UI_COLOR(muted))+12*f;
            manager_text(a,x,y,inside,"Speicherort",8,17.794f*f,UI_COLOR(text));float browse=manager_button_width(a,1,f);
            nk_style_push_font(ui,&a->manager_fonts[9]);nk_layout_space_push(ui,nk_rect(x,y+23.794f*f,narrow?inside:inside-browse-6*f,34.326f*f));project_manager_field(a,0,a->manager_parent,sizeof a->manager_parent);nk_style_pop_font(ui);
            float browse_y=y+(narrow?64.12f:23.794f)*f;
            if(manager_button(a,1,x+inside-browse,browse_y,browse,34.326f*f,false))choose_workspace_path(a,PS_DIALOG_MANAGER_PARENT);
            float hint_y=y+(64.12f+(narrow?40.326f:0))*f;
            y=hint_y+manager_text(a,x,hint_y,inside,"Hier wird dein Projektordner angelegt.",10,15.19f*f,UI_COLOR(muted))+20*f;
            float fw=narrow?inside:(inside-20*f)/2;if(!a->manager_analysis_only){manager_language(a,5,x,y,fw,f);if(narrow)y+=78.988f*f;}manager_language(a,6,a->manager_analysis_only || narrow?x:x+fw+20*f,y,fw,f);
        } else {
            manager_text(a,x,y,inside,"ZUSAMMENFASSUNG",0,15.19f*f,UI_COLOR(muted));y+=23.19f*f;
            y+=manager_text(a,x,y,inside,"Dein neuer Arbeitsraum.",2,22.785f*f,UI_COLOR(text))+15.215f*f;
            const char *labels[]={"Projektart","Vorlage","Experiment","Auswertung"},*values[]={a->manager_analysis_only?"Nur Auswertung":"Experiment + Auswertung",a->manager_analysis_only?"Keine":manager_templates[a->manager_template],a->manager_analysis_only?"Keines":a->manager_experiment_language?"Physim":"C",a->template_analysis_language?"Physim":"C"};
            for(unsigned i=0;i<4;i++){manager_text(a,x,y,inside*.45f,labels[i],6,17.36f*f,UI_COLOR(muted));y+=manager_text(a,x+inside*.5f,y,inside*.5f,values[i],6,17.36f*f,UI_COLOR(text))+10*f;}
            y+=12*f;float ph=manager_text_height(a,target,7,inside-28*f,20.72f*f);
            struct nk_rect preview=manager_slot(a,x,y,inside,12*f+16.926f*f+7*f+ph+12*f);nk_fill_rect(canvas,preview,7*f,UI_COLOR(surface));
            manager_text(a,x+14*f,y+12*f,inside-28*f,"Zielordner",5,16.926f*f,UI_COLOR(muted));manager_text(a,x+14*f,y+(12+16.926f+7)*f,inside-28*f,target,7,20.72f*f,UI_COLOR(accent));y+=preview.h+22*f;
            float name_h=manager_text_height(a,a->manager_name,11,inside-30*f,22.4f*f),tree_h=name_h+(a->manager_analysis_only?3:4)*22.4f*f+30*f;
            struct nk_rect tree=manager_slot(a,x,y,inside,tree_h);nk_fill_rect(canvas,tree,8*f,UI_COLOR(surface));nk_stroke_rect(canvas,tree,8*f,f,UI_COLOR(border));
            manager_text(a,x+15*f,y+15*f,inside-30*f,a->manager_name,11,22.4f*f,UI_COLOR(text));y+=15*f+name_h;
            if(!a->manager_analysis_only){manager_text(a,x+36*f,y,inside-51*f,a->manager_experiment_language?"main.phys":"main.c",6,22.4f*f,UI_COLOR(muted));y+=22.4f*f;}
            manager_text(a,x+36*f,y,inside-51*f,a->template_analysis_language?"analysis.phys":"analysis.c",6,22.4f*f,UI_COLOR(muted));y+=22.4f*f;manager_text(a,x+36*f,y,inside-51*f,"physim.project",6,22.4f*f,UI_COLOR(muted));y+=22.4f*f;manager_text(a,x+36*f,y,inside-51*f,"runs/",6,22.4f*f,UI_COLOR(muted));
        }
        float fy=header+steps+body+error;
        if(*a->manager_error){struct nk_rect er=manager_slot(a,24*f,fy-error+8*f,w-48*f,error-16*f);nk_stroke_rect(canvas,er,7*f,f,UI_COLOR(error));
            float text_height=manager_text(a,32*f,fy-error+16*f,w-64*f,a->manager_error,4,18.445f*f,UI_COLOR(error));
            a->manager_error_bounds=er;a->manager_error_text_bounds=nk_rect(er.x+8*f,er.y+8*f,w-64*f,text_height);
            if(a->manager_error_reveal){nk_window_set_scroll(ui,0,(nk_uint)fmaxf(0,fy-h+20*f));a->manager_error_reveal=false;}}
        line=manager_slot(a,0,fy,w,1);nk_fill_rect(canvas,line,0,UI_COLOR(border));
        manager_text(a,(narrow?18:24)*f,fy+(narrow?15:22)*f,narrow?w-36*f:w*.45f,"Vorhandene Projektdateien werden nicht überschrieben.",5,16.275f*f,UI_COLOR(muted));
        int rendered_step=a->manager_step;struct nk_rect cancel_slot=footer_slots[0],back_slot=footer_slots[1],next_slot=footer_slots[2];
        bool cancel=manager_button(a,8,cancel_slot.x,fy+cancel_slot.y,cancel_slot.w,cancel_slot.h,false);
        bool back=rendered_step && manager_button(a,9,back_slot.x,fy+back_slot.y,back_slot.w,back_slot.h,false);
        a->project_create_bounds=manager_slot(a,next_slot.x,fy+next_slot.y,next_slot.w,next_slot.h);bool next=manager_button(a,7,next_slot.x,fy+next_slot.y,next_slot.w,next_slot.h,true);
        nk_layout_space_end(ui);if(cancel)project_manager_close(a);else if(back)project_manager_step(a,rendered_step-1);else if(next)project_manager_next(a);
    }
    nk_end(ui);ui->style=saved;ui_palette=prior;layout->accessibility_full_controls=previous_full;
}
