/* Selected-object header and read-only B property cards. Scene values stay SI. */
static const char *laboratory_shape_name(uint32_t shape) {
    static const char *names[]={"Kugel","Linie","Box","Vektor","Punkt","Ebene","Pfad","Beschriftung","Gruppe","Koordinatenrahmen"};
    return shape<SDL_arraysize(names)?names[shape]:"Objekt";
}
static void laboratory_object_avatar(app *a,const ps_object *object,struct nk_rect r) {
    struct nk_command_buffer *canvas=nk_window_get_canvas(a->ui);float f=a->preferences.ui_size/16.f;
    struct nk_color color=accent();float x=r.x+r.w/2,y=r.y+r.h/2;
    nk_fill_rect(canvas,r,7*f,UI_COLOR(selection_active));
    switch(object->shape) {
    case PS_SPHERE:case PS_POINT:nk_fill_circle(canvas,nk_rect(x-5*f,y-5*f,10*f,10*f),color);break;
    case PS_GROUP:case PS_FRAME:
        nk_stroke_line(canvas,x,y-6*f,x+6*f,y,f,color);nk_stroke_line(canvas,x+6*f,y,x,y+6*f,f,color);
        nk_stroke_line(canvas,x,y+6*f,x-6*f,y,f,color);nk_stroke_line(canvas,x-6*f,y,x,y-6*f,f,color);break;
    case PS_LABEL:
        nk_stroke_line(canvas,x-5*f,y-5*f,x+5*f,y-5*f,f,color);nk_stroke_line(canvas,x,y-5*f,x,y+5*f,f,color);break;
    case PS_ARROW:
        nk_stroke_line(canvas,x-5*f,y+5*f,x+5*f,y-5*f,f,color);nk_stroke_line(canvas,x,y-5*f,x+5*f,y-5*f,f,color);
        nk_stroke_line(canvas,x+5*f,y-5*f,x+5*f,y,f,color);break;
    case PS_POLYLINE:nk_stroke_curve(canvas,x-6*f,y+f,x-2*f,y-6*f,x+2*f,y+6*f,x+6*f,y-f,f,color);break;
    case PS_LINE:nk_stroke_line(canvas,x-5*f,y+5*f,x+5*f,y-5*f,f,color);break;
    default:nk_stroke_rect(canvas,nk_rect(x-5*f,y-5*f,10*f,10*f),f,f,color);break;
    }
}
static void laboratory_object_heading(app *a,const ps_object *object) {
    struct nk_context *ui=a->ui;float f=a->preferences.ui_size/16.f;
    const struct nk_user_font *title=&a->laboratory_title_font,*small=&a->laboratory_caption_font;
    const char *name=*object->text?object->text:laboratory_shape_name(object->shape);char metadata[96];
    if(object->id)snprintf(metadata,sizeof metadata,"%s · ID %u",laboratory_shape_name(object->shape),object->id);
    else snprintf(metadata,sizeof metadata,"%s · Eintrag %u",laboratory_shape_name(object->shape),a->scene_selected_slot+1);
    float available=nk_window_get_content_region(ui).w,inline_width=fmaxf(1,available-44*f),longest=0;
    for(const char *at=name;*at;) {
        const char *end=strchr(at,' ');if(!end)end=at+strlen(at);
        longest=fmaxf(longest,title->width(title->userdata,title->height,at,(int)(end-at)));at=*end?end+1:end;
    }
    bool stacked=longest>inline_width;float width=stacked?available:inline_width,title_line=20.3f*f,meta_line=15.95f*f;
    float title_height=title_line*laboratory_wrapped_lines(title,name,width),meta_height=meta_line*laboratory_wrapped_lines(small,metadata,width);
    float content=stacked?44*f+title_height+3*f+meta_height:fmaxf(34*f,title_height+3*f+meta_height);
    nk_layout_row_dynamic(ui,5*f+content+16*f,1);struct nk_rect row;enum nk_widget_layout_states state=nk_widget(&row,ui);
    a->laboratory_object_heading_bounds=row;
    a->laboratory_object_avatar_bounds=nk_rect(row.x,row.y+5*f+(stacked?0:(content-34*f)/2),34*f,34*f);
    a->laboratory_object_title_bounds=nk_rect(row.x+(stacked?0:44*f),row.y+5*f+(stacked?44*f:0),width,title_height);
    if(state==NK_WIDGET_INVALID)return;
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);laboratory_object_avatar(a,object,a->laboratory_object_avatar_bounds);
    laboratory_wrapped_text(canvas,title,name,a->laboratory_object_title_bounds,title_line,UI_COLOR(text));
    struct nk_rect meta=nk_rect(a->laboratory_object_title_bounds.x,a->laboratory_object_title_bounds.y+title_height+3*f,width,meta_height);
    laboratory_wrapped_text(canvas,small,metadata,meta,meta_line,muted());
    char description[256];snprintf(description,sizeof description,"%s · %s",name,metadata);ps_ui_accessibility_label(ui,row,description);
}
typedef struct {const char *label,*unit;double value;char text[64];} laboratory_property;
static void laboratory_property_format(laboratory_property *property) {
    char number[48];snprintf(number,sizeof number,"%.4g",property->value);char *point=strchr(number,'.');if(point)*point=',';
    snprintf(property->text,sizeof property->text,"%s%s%s",number,*property->unit?" ":"",property->unit);
}
static void laboratory_property_text(app *a,const laboratory_property *property,struct nk_rect r,float line_height) {
    const struct nk_user_font *font=&a->laboratory_model_font;struct nk_command_buffer *canvas=nk_window_get_canvas(a->ui);
    int at=0,length=(int)strlen(property->text),unit=*property->unit?length-(int)strlen(property->unit):length;
    while(at<length) {
        float unit_gap=4*a->preferences.ui_size/16.f;
        int count=ps_ui_text_line(font,property->text+at,length-at,fmaxf(1,r.w-unit_gap),false);
        if(count<=0){nk_rune rune;count=nk_utf_decode(property->text+at,&rune,length-at);}if(count<=0)break;
        int number=unit>at?NK_MIN(count,unit-at):0;float x=r.x;
        if(number){nk_draw_text(canvas,nk_rect(x,r.y+(line_height-font->height)/2,r.w+1,font->height),property->text+at,number,font,nk_rgba(0,0,0,0),UI_COLOR(text));x+=font->width(font->userdata,font->height,property->text+at,number);}
        if(count>number)nk_draw_text(canvas,nk_rect(x+unit_gap,r.y+(line_height-font->height)/2,fmaxf(1,r.x+r.w-x-unit_gap+1),font->height),property->text+at+number,count-number,font,nk_rgba(0,0,0,0),muted());
        at+=count;while(at<length && property->text[at]==' ')at++;r.y+=line_height;
    }
}
static void laboratory_object_properties(app *a,const ps_object *object) {
    struct nk_context *ui=a->ui;laboratory_property properties[7];unsigned count=0;
    if(object->shape==PS_SPHERE)properties[count++]=(laboratory_property){"Radius","m",object->radius,{0}};
    if(object->shape!=PS_GROUP) {
        ps_vec3 world,local=object->shape==PS_FRAME?ps_v3(0,0,0):object->a;
        if(ps_scene_world_point(display_scene(a),a->scene_selected_slot,local,&world)==PS_OK) {
            properties[count++]=(laboratory_property){"Position x","m",world.x,{0}};
            properties[count++]=(laboratory_property){"Position y","m",world.y,{0}};
            properties[count++]=(laboratory_property){"Position z","m",world.z,{0}};
        }
        if(object->shape==PS_FRAME) {
            properties[count++]=(laboratory_property){"Skalierung x","",object->b.x,{0}};
            properties[count++]=(laboratory_property){"Skalierung y","",object->b.y,{0}};
            properties[count++]=(laboratory_property){"Skalierung z","",object->b.z,{0}};
        }
    }
    a->laboratory_property_count=count;float f=a->preferences.ui_size/16.f,available=nk_window_get_content_region(ui).w;
    unsigned columns=available>=252*f?2:1;float gap=8*f,width=(available-(columns-1)*gap)/columns;
    const struct nk_user_font *small=&a->workspace_status_font;float label_line=14.5f*f,value_line=27.55f*f;
    for(unsigned i=0;i<count;i++)laboratory_property_format(&properties[i]);
    for(unsigned first=0;first<count;first+=columns) {
        float height=0;for(unsigned j=first;j<NK_MIN(count,first+columns);j++) {
            float h=20*f+label_line*laboratory_wrapped_lines(small,properties[j].label,fmaxf(1,width-22*f))+5*f+value_line*laboratory_wrapped_lines(&a->laboratory_model_font,properties[j].text,fmaxf(1,width-26*f));height=fmaxf(height,h);
        }
        nk_layout_row_dynamic(ui,height,1);struct nk_rect row;enum nk_widget_layout_states visible=nk_widget(&row,ui);
        for(unsigned j=first;j<NK_MIN(count,first+columns);j++) {
            struct nk_rect card=nk_rect(row.x+(j-first)*(width+gap),row.y,width,height);a->laboratory_property_bounds[j]=card;
            float label_height=label_line*laboratory_wrapped_lines(small,properties[j].label,fmaxf(1,width-22*f));
            struct nk_rect number=nk_rect(card.x+11*f,card.y+10*f+label_height+5*f,fmaxf(1,width-22*f),height-20*f-label_height-5*f);a->laboratory_property_value_bounds[j]=number;
            if(visible!=NK_WIDGET_INVALID) {
                struct nk_command_buffer *canvas=nk_window_get_canvas(ui);nk_fill_rect(canvas,card,7*f,UI_COLOR(window));
                if(a->preferences.theme==PS_THEME_HIGH_CONTRAST)nk_stroke_rect(canvas,nk_rect(card.x+f/2,card.y+f/2,card.w-f,card.h-f),6.5f*f,f,UI_COLOR(border));
                laboratory_wrapped_text(canvas,small,properties[j].label,nk_rect(card.x+11*f,card.y+10*f,number.w,label_height),label_line,muted());
                laboratory_property_text(a,&properties[j],number,value_line);
                char description[192];snprintf(description,sizeof description,"%s: %s",properties[j].label,properties[j].text);ps_ui_accessibility_label(ui,card,description);
            }
        }
        ps_ui_row_exact_dynamic(ui,8*f,1);nk_spacer(ui);
    }
}
static void laboratory_object_coordinate(app *a,const char *label,ps_vec3 value) {
    struct nk_context *ui=a->ui;float f=a->preferences.ui_size/16.f,width=nk_window_get_content_region(ui).w;
    const struct nk_user_font *small=&a->workspace_status_font,*font=&a->laboratory_coordinate_font;
    char text[160];snprintf(text,sizeof text,"%.6g / %.6g / %.6g",value.x,value.y,value.z);for(char *at=text;*at;at++)if(*at=='.')*at=',';
    float label_height=16*f*laboratory_wrapped_lines(small,label,width),value_height=20.3f*f*laboratory_wrapped_lines(font,text,width);
    nk_layout_row_dynamic(ui,8*f+label_height+3*f+value_height+10*f,1);struct nk_rect r;
    if(nk_widget(&r,ui)==NK_WIDGET_INVALID)return;
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    laboratory_wrapped_text(canvas,small,label,nk_rect(r.x,r.y+8*f,r.w,label_height),16*f,muted());
    laboratory_wrapped_text(canvas,font,text,nk_rect(r.x,r.y+8*f+label_height+3*f,r.w,value_height),20.3f*f,UI_COLOR(text));
    char description[256];snprintf(description,sizeof description,"%s: %s",label,text);ps_ui_accessibility_label(ui,r,description);
}
static bool laboratory_object_action(app *a,const char *label,bool secondary,struct nk_rect *bounds) {
    struct nk_context *ui=a->ui;float f=a->preferences.ui_size/16.f;
    const struct nk_user_font *font=secondary?&a->workspace_status_font:&a->laboratory_caption_font;
    float line=(secondary?14.5f:15.95f)*f,padding=(secondary?7:8)*f,width=nk_window_get_content_region(ui).w;
    unsigned lines=laboratory_wrapped_lines(font,label,fmaxf(1,width-2*padding));
    ps_ui_row_exact_dynamic(ui,(secondary?4:10)*f,1);nk_spacer(ui);
    nk_layout_row_dynamic(ui,lines*line+2*padding+2*f,1);struct nk_rect r;enum nk_widget_layout_states visible=nk_widget(&r,ui);*bounds=r;
    if(visible==NK_WIDGET_INVALID)return false;
    struct nk_rect target=plot_intersection(r,ui->current->layout->clip);bool focused=false;
    bool pressed=ps_ui_button_at_custom_focus(ui,target,label,false,&focused);
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);bool hover=nk_input_is_mouse_hovering_rect(&ui->input,target);
    if(!secondary || hover)nk_fill_rect(canvas,r,6*f,hover?UI_COLOR(button_hover):UI_COLOR(button));
    if(a->preferences.theme==PS_THEME_HIGH_CONTRAST)nk_stroke_rect(canvas,nk_rect(r.x+f/2,r.y+f/2,r.w-f,r.h-f),5.5f*f,f,UI_COLOR(border));
    int at=0,length=(int)strlen(label);float y=r.y+(r.h-lines*line)/2;
    while(at<length) {
        int count=ps_ui_text_line(font,label+at,length-at,fmaxf(1,width-2*padding),false);
        if(count<=0){nk_rune rune;count=nk_utf_decode(label+at,&rune,length-at);}if(count<=0)break;
        float w=font->width(font->userdata,font->height,label+at,count);
        nk_draw_text(canvas,nk_rect(r.x+(r.w-w)/2,y+(line-font->height)/2,w+1,font->height),label+at,count,font,nk_rgba(0,0,0,0),secondary?muted():UI_COLOR(text));
        at+=count;while(at<length && label[at]==' ')at++;y+=line;
    }
    if(focused)nk_stroke_rect(canvas,nk_rect(target.x+2*f,target.y+2*f,fmaxf(0,target.w-4*f),fmaxf(0,target.h-4*f)),4*f,2*f,accent());
    return pressed;
}
static void laboratory_object(app *a) {
    scene_selection_sync(a);const ps_scene *scene=display_scene(a);
    bool selected=a->scene_selected && a->scene_selected_slot<scene->count;const ps_object *object=selected?&scene->objects[a->scene_selected_slot]:NULL;
    a->laboratory_object_heading_bounds=a->laboratory_object_title_bounds=a->laboratory_object_avatar_bounds=nk_rect(0,0,0,0);a->laboratory_property_count=0;
    memset(a->laboratory_property_bounds,0,sizeof a->laboratory_property_bounds);memset(a->laboratory_property_value_bounds,0,sizeof a->laboratory_property_value_bounds);
    if(selected)laboratory_object_heading(a,object);
    if(!workspace_section_begin(a,"Objekt",&a->laboratory_groups[0]))return;
    if(!selected)laboratory_note(a,"Wähle ein Objekt in der Szene oder im Szenenbaum.");
    else {
        laboratory_object_properties(a,object);
        if(object->shape!=PS_GROUP) {
            ps_vec3 world,local=object->shape==PS_FRAME?ps_v3(0,0,0):object->a;
            if(ps_scene_world_point(scene,a->scene_selected_slot,local,&world)==PS_OK)laboratory_object_coordinate(a,"Weltposition / m",world);
            laboratory_object_coordinate(a,"Lokale Position / m",object->a);
        }
        if(laboratory_object_action(a,"Auswahl ausblenden",false,&a->scene_hide_selection_bounds)){a->scene_view.hidden|=UINT32_C(1)<<a->scene_selected_slot;a->scene_selected=false;}
    }
    if(laboratory_object_action(a,"Alle Objekte einblenden",true,&a->display_all_bounds))a->scene_view.hidden=0;
    workspace_section_end(a);
}
