/* Personal presentation choices never alter stored measurements. */
#include "../src/number_parse.h"
static void channel_units_start(app *a) {
    a->channel_units=(ps_channel_units){0};a->channel_units_path[0]=a->channel_units_error[0]=0;
    const char *path=a->preferences_path,*slash=strrchr(path,'/'),*back=strrchr(path,'\\');
    if(back && (!slash || back>slash))slash=back;
    size_t parent=slash?(size_t)(slash-path)+1:0;
    if(*path && parent+sizeof "channel-units.bin"<=sizeof a->channel_units_path) {
        memcpy(a->channel_units_path,path,parent);
        memcpy(a->channel_units_path+parent,"channel-units.bin",sizeof "channel-units.bin");
    }
    ps_result r=ps_channel_units_read(a->channel_units_path,&a->channel_units);
    a->channel_units_writable=r==PS_OK || r==PS_EOF;
    if(!a->channel_units_writable)snprintf(a->channel_units_error,sizeof a->channel_units_error,
        "Anzeigeeinheiten konnten nicht geladen werden (%s). Die Datei bleibt erhalten.",ps_result_string(r));
}
static ps_display_unit channel_display_unit(app *a,const ps_channel *ch) {
    ps_display_unit u={.scale=1};memcpy(u.dimension,ch->dimension,7);
    snprintf(u.symbol,sizeof u.symbol,"%s",*ch->unit?ch->unit:"SI");
    (void)ps_channel_units_get(&a->channel_units,ch,&u);return u;
}
static void channel_display_text(const ps_display_unit *u,double value,char *text,size_t size) {
    double converted;
    if(ps_display_unit_value(u,value,&converted)==PS_OK)snprintf(text,size,"%.8g %s",converted,u->symbol);
    else snprintf(text,size,"nicht darstellbar");
}
static bool channel_display_values(const ps_display_unit *u,const double *source,size_t n,double *out) {
    bool ok=true;
    for(size_t i=0;i<n;i++) {
        out[i]=NAN;
        if(isfinite(source[i]) && ps_display_unit_value(u,source[i],out+i)!=PS_OK)ok=false;
    }
    return ok;
}
static void channel_unit_cell(struct nk_context *ui,const char *text) {
    struct nk_rect bounds=nk_widget_bounds(ui);const struct nk_user_font *font=ui->style.font;
    float available=fmaxf(0,bounds.w-2*ui->style.text.padding.x-4);
    size_t count=strlen(text);char visible[128];
    bool truncated=font->width(font->userdata,font->height,text,(int)count)>available;
    if(truncated) {
        float dots=font->width(font->userdata,font->height,"...",3);
        while(count && font->width(font->userdata,font->height,text,(int)count)+dots>available) {
            count--;while(count && ((unsigned char)text[count]&0xc0)==0x80)count--;
        }
    }
    snprintf(visible,sizeof visible,"%.*s%s",(int)count,text,truncated?"...":"");
    nk_label(ui,visible,NK_TEXT_LEFT);
    if(nk_input_is_mouse_hovering_rect(&ui->input,bounds))nk_tooltip(ui,text);
}
static void channel_unit_open(app *a,const ps_channel *ch) {
    a->unit_channel=*ch;a->unit_draft=channel_display_unit(a,ch);
    snprintf(a->channel_unit_scale,sizeof a->channel_unit_scale,"%.17g",a->unit_draft.scale);
    if(a->channel_units_writable)a->channel_units_error[0]=0;
    a->channel_unit_manager=true;
}
static bool channel_units_save(app *a,const ps_channel_units *next) {
    ps_result r=ps_channel_units_write(a->channel_units_path,next);
    if(r!=PS_OK) {
        snprintf(a->channel_units_error,sizeof a->channel_units_error,"Speichern fehlgeschlagen (%s).",ps_result_string(r));return false;
    }
    a->channel_units=*next;a->channel_units_writable=true;a->channel_units_error[0]=0;return true;
}
static bool channel_unit_fits(app *a,const ps_display_unit *u) {
    double converted;
    for(uint32_t i=0;i<a->channel_count;i++)if(!strcmp(a->live_channels[i].name,a->unit_channel.name) &&
        !memcmp(a->live_channels[i].dimension,a->unit_channel.dimension,7)) {
        const double *shown=display_values(a);int mask=a->channel_status[i];
        if((mask<0 || shown[mask]==1) && isfinite(shown[i]) && ps_display_unit_value(u,shown[i],&converted)!=PS_OK)return false;
        for(int j=0;j<a->history_count;j++)if(isfinite(a->history_v[i][j]) && ps_display_unit_value(u,a->history_v[i][j],&converted)!=PS_OK)return false;
    }
    if(!a->data.thread)for(uint32_t i=0;i<a->data.channel_count;i++)if(!strcmp(a->data.channels[i].name,a->unit_channel.name) &&
        !memcmp(a->data.channels[i].dimension,a->unit_channel.dimension,7)) {
        for(int j=0;j<a->data.count;j++)if(isfinite(a->data.values[i][j]) && ps_display_unit_value(u,a->data.values[i][j],&converted)!=PS_OK)return false;
        if(a->data.stats[i].count) {
            double values[]={a->data.stats[i].mean,a->data.stats[i].min,a->data.stats[i].max};
            for(unsigned j=0;j<3;j++)if(ps_display_unit_value(u,values[j],&converted)!=PS_OK)return false;
            if(a->data.stats[i].count>1 && ps_display_unit_value(u,ps_statistics_stddev(&a->data.stats[i]),&converted)!=PS_OK)return false;
        }
    }
    return true;
}
static void channel_unit_apply(app *a) {
    ps_display_unit u=a->unit_draft;
    if(!ps_parse_finite_number(a->channel_unit_scale,NULL,&u.scale) || !ps_display_unit_valid(&u)) {
        snprintf(a->channel_units_error,sizeof a->channel_units_error,"Symbol und einen endlichen Faktor größer als null eingeben.");return;
    }
    if(!channel_unit_fits(a,&u)) {
        snprintf(a->channel_units_error,sizeof a->channel_units_error,"Dieser Faktor kann die vorhandenen Messwerte oder Statistiken nicht darstellen.");return;
    }
    ps_channel_units next=a->channel_units;ps_result r=ps_channel_units_put(&next,&a->unit_channel,&u);
    if(r!=PS_OK){snprintf(a->channel_units_error,sizeof a->channel_units_error,"Einheit ungültig oder alle 64 Plätze belegt (%s).",ps_result_string(r));return;}
    if(channel_units_save(a,&next))a->channel_unit_manager=false;
}
typedef struct {const char *symbol;double scale;int8_t dimension[7];} channel_unit_preset;
static const channel_unit_preset channel_unit_presets[]={
    {"1",1,{0}}, {"%",.01,{0}}, {"rad",1,{0}}, {"°",0.017453292519943295,{0}},
    {"m",1,{1}}, {"mm",.001,{1}}, {"cm",.01,{1}}, {"km",1000,{1}},
    {"s",1,{0,0,1}}, {"ms",.001,{0,0,1}}, {"min",60,{0,0,1}}, {"h",3600,{0,0,1}},
    {"kg",1,{0,1}}, {"g",.001,{0,1}},
    {"m/s",1,{1,0,-1}}, {"km/h",1.0/3.6,{1,0,-1}},
    {"m/s²",1,{1,0,-2}}, {"N",1,{1,1,-2}}, {"kN",1000,{1,1,-2}},
    {"J",1,{2,1,-2}}, {"kJ",1000,{2,1,-2}},
    {"Pa",1,{-1,1,-2}}, {"kPa",1000,{-1,1,-2}}, {"bar",100000,{-1,1,-2}},
    {"W",1,{2,1,-3}}, {"kW",1000,{2,1,-3}}, {"Hz",1,{0,0,-1}},
    {"A",1,{0,0,0,1}}, {"K",1,{0,0,0,0,1}}, {"mol",1,{0,0,0,0,0,1}}, {"cd",1,{0,0,0,0,0,0,1}}
};
static void channel_unit_manager_view(app *a,int width,int height) {
    struct nk_context *ui=a->ui;
    nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(0,5));
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,0));
    if(nk_begin(ui,"channel-units-header",nk_rect(0,0,(float)width,38),NK_WINDOW_NO_SCROLLBAR)) {
        nk_layout_row_begin(ui,NK_STATIC,28,4);nk_layout_row_push(ui,(float)width-138);
        a->window_drag_bounds=nk_widget_bounds(ui);nk_label(ui,"   Physim  ·  Anzeigeeinheit",NK_TEXT_LEFT);
        for(unsigned i=0;i<3;i++){nk_layout_row_push(ui,46);window_control(a,i);}nk_layout_row_end(ui);
    }
    nk_end(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
    float w=fminf(640,(float)width-32),h=fminf(540,(float)height-54);
    if(nk_begin(ui,"channel-units-manager",nk_rect(((float)width-w)/2,((float)height-h)/2,w,h),0)) {
        nk_layout_row_dynamic(ui,38,1);nk_style_push_font(ui,a->font_title);nk_label(ui,"Anzeigeeinheit",NK_TEXT_LEFT);nk_style_pop_font(ui);
        char label[160];snprintf(label,sizeof label,"Kanal: %s [%s]",a->unit_channel.name,a->unit_channel.unit);
        nk_layout_row_dynamic(ui,28,1);nk_label(ui,label,NK_TEXT_LEFT);
        nk_layout_row_dynamic(ui,52,1);nk_label_wrap(ui,"Die Auswahl gilt für Live-Werte, Messkurven und Statistiken dieses Kanals. Messdatei, Modell und CSV bleiben in SI.");
        nk_begin_disabled(ui,!a->channel_units_writable);
        nk_layout_row_dynamic(ui,30,1);a->channel_unit_bounds[0]=nk_widget_bounds(ui);
        if(nk_combo_begin_label(ui,"Passende Einheit auswählen",nk_vec2(w-40,220))) {
            nk_layout_row_dynamic(ui,26,1);
            for(unsigned i=0;i<sizeof channel_unit_presets/sizeof channel_unit_presets[0];i++) {
                const channel_unit_preset *p=channel_unit_presets+i;
                if(memcmp(p->dimension,a->unit_channel.dimension,7))continue;
                if(nk_button_label(ui,p->symbol)) {
                    snprintf(a->unit_draft.symbol,sizeof a->unit_draft.symbol,"%s",p->symbol);
                    snprintf(a->channel_unit_scale,sizeof a->channel_unit_scale,"%.17g",p->scale);nk_combo_close(ui);
                }
            }nk_combo_end(ui);
        }
        nk_layout_row_dynamic(ui,24,2);nk_label(ui,"Symbol",NK_TEXT_LEFT);nk_label(ui,"SI-Einheiten pro Anzeigeeinheit",NK_TEXT_LEFT);
        nk_layout_row_dynamic(ui,32,2);a->channel_unit_bounds[1]=nk_widget_bounds(ui);
        nk_edit_string_zero_terminated(ui,NK_EDIT_FIELD,a->unit_draft.symbol,sizeof a->unit_draft.symbol,nk_filter_default);
        a->channel_unit_bounds[2]=nk_widget_bounds(ui);
        nk_edit_string_zero_terminated(ui,NK_EDIT_FIELD,a->channel_unit_scale,sizeof a->channel_unit_scale,nk_filter_default);
        nk_layout_row_dynamic(ui,32,1);nk_label_wrap(ui,"Beispiel: cm mit Faktor 0.01. Nur lineare Einheiten ohne Temperatur-Offset.");
        nk_layout_row_dynamic(ui,32,1);
        ps_display_unit preview=a->unit_draft;double si=1;
        for(uint32_t i=0;i<a->channel_count;i++)if(!strcmp(a->live_channels[i].name,a->unit_channel.name) &&
            !memcmp(a->live_channels[i].dimension,a->unit_channel.dimension,7)) {
            const double *shown=display_values(a);int mask=a->channel_status[i];
            if(mask<0 || shown[mask]==1)si=shown[i];
            break;
        }
        char converted[96];
        if(ps_parse_finite_number(a->channel_unit_scale,NULL,&preview.scale))channel_display_text(&preview,si,converted,sizeof converted);
        else snprintf(converted,sizeof converted,"ungültiger Faktor");
        snprintf(label,sizeof label,"Vorschau: %.8g SI = %s",si,converted);nk_label(ui,label,NK_TEXT_LEFT);
        nk_layout_row_dynamic(ui,36,2);a->channel_unit_bounds[3]=nk_widget_bounds(ui);
        if(nk_button_label(ui,"Anwenden"))channel_unit_apply(a);
        a->channel_unit_bounds[4]=nk_widget_bounds(ui);
        if(nk_button_label(ui,"SI wiederherstellen")) {
            ps_channel_units next=a->channel_units;
            if(ps_channel_units_remove(&next,&a->unit_channel)==PS_OK && channel_units_save(a,&next))a->channel_unit_manager=false;
        }
        nk_end_disabled(ui);
        nk_layout_row_dynamic(ui,52,1);nk_label_wrap(ui,a->channel_units_error);
        nk_layout_row_dynamic(ui,36,2);a->channel_unit_bounds[5]=nk_widget_bounds(ui);
        if(!a->channel_units_writable && *a->channel_units_path) {
            if(nk_button_label(ui,"Datei bewusst zurücksetzen")) {ps_channel_units empty={0};(void)channel_units_save(a,&empty);}
        } else nk_label(ui,"",NK_TEXT_LEFT);
        a->channel_unit_bounds[6]=nk_widget_bounds(ui);
        if(nk_button_label(ui,"Abbrechen · Esc"))a->channel_unit_manager=false;
    }nk_end(ui);
}
