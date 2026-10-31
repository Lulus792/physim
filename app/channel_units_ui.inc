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
static bool channel_units_save(app *a,const ps_channel_units *next) {
    ps_result r=ps_channel_units_write(a->channel_units_path,next);
    if(r!=PS_OK) {
        snprintf(a->channel_units_error,sizeof a->channel_units_error,"Speichern fehlgeschlagen (%s).",ps_result_string(r));return false;
    }
    a->channel_units=*next;a->channel_units_writable=true;a->channel_units_error[0]=0;return true;
}
static bool channel_unit_fits(app *a,const ps_channel *channel,const ps_display_unit *u) {
    double converted;
    for(uint32_t i=0;i<a->channel_count;i++)if(!strcmp(a->live_channels[i].name,channel->name) &&
        !memcmp(a->live_channels[i].dimension,channel->dimension,7)) {
        const double *shown=display_values(a);int mask=a->channel_status[i];
        if((mask<0 || shown[mask]==1) && isfinite(shown[i]) && ps_display_unit_value(u,shown[i],&converted)!=PS_OK)return false;
        for(int j=0;j<a->history_count;j++)if(isfinite(a->history_v[i][j]) && ps_display_unit_value(u,a->history_v[i][j],&converted)!=PS_OK)return false;
    }
    if(!a->data.thread)for(uint32_t i=0;i<a->data.channel_count;i++)if(!strcmp(a->data.channels[i].name,channel->name) &&
        !memcmp(a->data.channels[i].dimension,channel->dimension,7)) {
        for(int j=0;j<a->data.count;j++)if(isfinite(a->data.values[i][j]) && ps_display_unit_value(u,a->data.values[i][j],&converted)!=PS_OK)return false;
        if(a->data.stats[i].count) {
            double values[]={a->data.stats[i].mean,a->data.stats[i].min,a->data.stats[i].max};
            for(unsigned j=0;j<3;j++)if(ps_display_unit_value(u,values[j],&converted)!=PS_OK)return false;
            if(a->data.stats[i].count>1 && ps_display_unit_value(u,ps_statistics_stddev(&a->data.stats[i]),&converted)!=PS_OK)return false;
        }
    }
    return true;
}
static bool channel_unit_commit(app *a,const ps_channel *channel,const ps_display_unit *unit) {
    if(!a->channel_units_writable)return false;
    if(!channel_unit_fits(a,channel,unit)) {
        snprintf(a->channel_units_error,sizeof a->channel_units_error,"Dieser Faktor kann die vorhandenen Messwerte oder Statistiken nicht darstellen.");return false;
    }
    ps_channel_units next=a->channel_units;ps_result r=ps_channel_units_put(&next,channel,unit);
    if(r!=PS_OK){snprintf(a->channel_units_error,sizeof a->channel_units_error,"Einheit ungültig oder alle 64 Plätze belegt (%s).",ps_result_string(r));return false;}
    return channel_units_save(a,&next);
}
typedef struct {const char *symbol;double scale;int8_t dimension[7];} channel_unit_preset;
static const channel_unit_preset channel_unit_presets[]={
    {"1",1,{0}}, {"%",.01,{0}}, {"rad",1,{0}}, {"°",0.017453292519943295,{0}},
    {"m",1,{1}}, {"mm",.001,{1}}, {"cm",.01,{1}}, {"km",1000,{1}},
    {"s",1,{0,0,1}}, {"ms",.001,{0,0,1}}, {"min",60,{0,0,1}}, {"h",3600,{0,0,1}},
    {"kg",1,{0,1}}, {"g",.001,{0,1}},
    {"m/s",1,{1,0,-1}}, {"cm/s",.01,{1,0,-1}}, {"km/h",1.0/3.6,{1,0,-1}},
    {"m/s²",1,{1,0,-2}}, {"N",1,{1,1,-2}}, {"kN",1000,{1,1,-2}},
    {"J",1,{2,1,-2}}, {"kJ",1000,{2,1,-2}},
    {"Pa",1,{-1,1,-2}}, {"kPa",1000,{-1,1,-2}}, {"bar",100000,{-1,1,-2}},
    {"W",1,{2,1,-3}}, {"kW",1000,{2,1,-3}}, {"Hz",1,{0,0,-1}},
    {"A",1,{0,0,0,1}}, {"K",1,{0,0,0,0,1}}, {"mol",1,{0,0,0,0,0,1}}, {"cd",1,{0,0,0,0,0,0,1}}
};
#include "channel_unit_window.inc"
