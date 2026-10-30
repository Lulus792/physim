/* Private workspace presentation, included after main.c's application actions. */
#include "design_tokens.h"
static struct nk_color muted(void) { return UI_COLOR(muted); }
static struct nk_color accent(void) { return UI_COLOR(accent); }
static bool timing_text_field(struct nk_context *ui,char *text,double *displayed,double *value) {
    if(*displayed!=*value && !(isnan(*displayed) && isnan(*value))) {
        snprintf(text,64,"%.9g",*value);*displayed=*value;
    }
    char previous[64];snprintf(previous,sizeof previous,"%s",text);
    nk_edit_string_zero_terminated(ui,NK_EDIT_FIELD,text,64,nk_filter_default);
    if(strcmp(previous,text)) {
        char *end;errno=0;double parsed=strtod(text,&end);
        *value=errno || end==text || *end ? NAN:parsed;
        *displayed=*value;return true;
    }
    return false;
}
static void step_bound_editor(app *a,unsigned which,const char *label,double *value) {
    if(laboratory_visible(a)) {
        nk_layout_row_dynamic(a->ui,24,1);nk_label(a->ui,label,NK_TEXT_LEFT);
        nk_layout_row_dynamic(a->ui,46*a->preferences.ui_size/16.f,1);
    } else nk_layout_row_dynamic(a->ui,32,2);
    if(!laboratory_visible(a))nk_label(a->ui,label,NK_TEXT_LEFT);
    a->adaptive_limit_bounds[which]=nk_widget_bounds(a->ui);
    if(laboratory_visible(a))nk_style_push_font(a->ui,&a->laboratory_value_font);
    if(timing_text_field(a->ui,a->adaptive_limit_text[which],&a->adaptive_limit_displayed[which],value))
        a->project_settings_dirty=true;
    if(laboratory_visible(a))nk_style_pop_font(a->ui);
}
static void style(struct nk_context *ui) {
    ps_ui_set_focus_color(ui,UI_COLOR(accent));
    struct nk_color c[NK_COLOR_COUNT];
    nk_style_default(ui);
    for (int i = 0; i < NK_COLOR_COUNT; i++)
        c[i] = UI_COLOR(surface);
    c[NK_COLOR_TEXT] = UI_COLOR(text);
    c[NK_COLOR_WINDOW] = UI_COLOR(window);
    c[NK_COLOR_HEADER] = UI_COLOR(header);
    c[NK_COLOR_BORDER] = UI_COLOR(border);
    c[NK_COLOR_BUTTON] = UI_COLOR(button);
    c[NK_COLOR_BUTTON_HOVER] = UI_COLOR(button_hover);
    c[NK_COLOR_BUTTON_ACTIVE] = UI_COLOR(button_active);
    c[NK_COLOR_EDIT] = UI_COLOR(edit);
    c[NK_COLOR_EDIT_CURSOR] = accent();
    c[NK_COLOR_SELECT] = UI_COLOR(button);
    c[NK_COLOR_SELECT_ACTIVE] = UI_COLOR(selection_active);
    c[NK_COLOR_TOGGLE] = UI_COLOR(toggle);
    c[NK_COLOR_TOGGLE_HOVER] = UI_COLOR(toggle_hover);
    c[NK_COLOR_TOGGLE_CURSOR] = accent();
    c[NK_COLOR_SLIDER] = UI_COLOR(slider);
    c[NK_COLOR_SLIDER_CURSOR] = accent();
    c[NK_COLOR_SLIDER_CURSOR_HOVER] = UI_COLOR(accent_hover);
    c[NK_COLOR_SLIDER_CURSOR_ACTIVE] = accent();
    c[NK_COLOR_SCROLLBAR] = c[NK_COLOR_WINDOW];
    c[NK_COLOR_SCROLLBAR_CURSOR] = UI_COLOR(scrollbar);
    c[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = UI_COLOR(scrollbar_hover);
    c[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = UI_COLOR(scrollbar_active);
    nk_style_from_table(ui, c);
    ui->style.progress.padding = nk_vec2(0, 0);
    ui->style.progress.rounding = PS_UI_RADIUS_PROGRESS;
    ui->style.progress.cursor_rounding = PS_UI_RADIUS_PROGRESS;
    ui->style.window.rounding = PS_UI_RADIUS_PANEL;
    ui->style.window.padding = nk_vec2(PS_UI_SPACE_LARGE, PS_UI_SPACE_MEDIUM);
    ui->style.window.spacing = nk_vec2(PS_UI_SPACE_SMALL, PS_UI_SPACE_SMALL);
    ui->style.window.scrollbar_size = nk_vec2(7, 7);
    ui->style.button.rounding = PS_UI_RADIUS_CONTROL;
    ui->style.button.border = 0;
    ui->style.button.padding = nk_vec2(10, 6);
    ui->style.edit.rounding = PS_UI_RADIUS_FIELD;
    ui->style.edit.border = 0;
    ui->style.edit.padding = nk_vec2(10, 8);
    ui->style.edit.row_padding = 4;
    ui->style.edit.cursor_size = 1;
    ui->style.combo.rounding = PS_UI_RADIUS_FIELD;
    ui->style.combo.border = 0;
    ui->style.combo.sym_normal=ui->style.combo.sym_hover=ui->style.combo.sym_active=NK_SYMBOL_CHEVRON_DOWN;
    ui->style.combo.button.padding=nk_vec2(7,7);
    ui->style.combo.button.normal=ui->style.combo.button.hover=ui->style.combo.button.active=nk_style_item_color(nk_rgba(0,0,0,0));
    ui->style.property.rounding = PS_UI_RADIUS_FIELD;
    ui->style.tab.background = nk_style_item_color(c[NK_COLOR_WINDOW]);
    ui->style.tab.border = 0;
    ui->style.tab.padding = nk_vec2(0, 8);
    if (ui_palette == &PS_UI_HIGH_CONTRAST) {
        ui->style.button.border = 1;
        ui->style.edit.border = 1;
        ui->style.combo.border = 1;
        ui->style.property.border = 1;
    }
}
static bool action_button(struct nk_context *ui, const char *label, bool primary) {
    if (primary) {
        nk_style_push_style_item(ui, &ui->style.button.normal,
                                 nk_style_item_color(UI_COLOR(primary)));
        nk_style_push_style_item(ui, &ui->style.button.hover,
                                 nk_style_item_color(UI_COLOR(primary_hover)));
        nk_style_push_style_item(ui, &ui->style.button.active,
                                 nk_style_item_color(UI_COLOR(primary_active)));
        nk_style_push_color(ui, &ui->style.button.text_normal, UI_COLOR(primary_text));
        nk_style_push_color(ui, &ui->style.button.text_hover, UI_COLOR(primary_text));
        nk_style_push_color(ui, &ui->style.button.text_active, UI_COLOR(primary_text));
    }
    bool pressed = nk_button_label(ui, label);
    if (primary)
        for (int i = 0; i < 3; i++) {
            nk_style_pop_style_item(ui);
            nk_style_pop_color(ui);
        }
    return pressed;
}
static void section(struct nk_context *ui, const char *label) {
    nk_layout_row_dynamic(ui, 12, 1);
    nk_spacer(ui);
    nk_layout_row_dynamic(ui, 22, 1);
    nk_label_colored(ui, label, NK_TEXT_LEFT, muted());
}
static const char *project_name(const app *a) {
    const char *name = a->project;
    for (const char *p = name; *p; p++)
        if ((*p == '/' || *p == '\\') && p[1])
            name = p + 1;
    return *name ? name : "Neues Experiment";
}
static const char *workspace_name(const char *path) {
    const char *name = path;
    for (const char *p = path; *p; p++)
        if ((*p == '/' || *p == '\\') && p[1])
            name = p + 1;
    return name;
}
static void workbench_file_icon(app *a,struct nk_command_buffer *canvas,const char *path,struct nk_rect r,struct nk_color color) {
    const char *ext=strrchr(path,'.');
    float f=a->preferences.ui_size/16.f;
    if(ext && (!strcmp(ext,".c") || !strcmp(ext,".h") || !strcmp(ext,".phys"))) {
        const struct nk_user_font *font=&a->workbench_marker_font;
        const char *label=!strcmp(ext,".phys")?"P":"C";
        float width=font->width(font->userdata,font->height,label,1);
        nk_draw_text(canvas,nk_rect(r.x+(r.w-width)/2,r.y+(r.h-font->height)/2,width+1,font->height),label,1,font,nk_rgba(0,0,0,0),color);
    } else if(ext && !strcmp(ext,".project")) {
        float x=r.x+r.w/2,y=r.y+r.h/2;
        nk_stroke_line(canvas,x,y-3*f,x-4*f,y,1.2f*f,color);nk_stroke_line(canvas,x-4*f,y,x,y+3*f,1.2f*f,color);
        nk_stroke_line(canvas,x,y+3*f,x+4*f,y,1.2f*f,color);nk_stroke_line(canvas,x+4*f,y,x,y-3*f,1.2f*f,color);
    } else ps_ui_draw_icon(canvas,PS_ICON_FILE,r,color);
}
static float workbench_card_text_height(const struct nk_user_font *font,const char *text,float width) {
    size_t at=0,bytes=strlen(text);unsigned lines=0;
    while(at<bytes) {
        size_t length=0,used=documentation_line(font,text+at,bytes-at,fmaxf(1,width),&length);
        if(!used)break;at+=used;lines++;
    }
    return lines?lines*(font->height+5)-5:0;
}
static void workbench_card_text(struct nk_command_buffer *canvas,const struct nk_user_font *font,struct nk_rect r,const char *text,struct nk_color color) {
    size_t at=0,bytes=strlen(text);float y=r.y;
    while(at<bytes && y+font->height<=r.y+r.h) {
        size_t length=0,used=documentation_line(font,text+at,bytes-at,fmaxf(1,r.w),&length);
        if(!used)break;
        nk_draw_text(canvas,nk_rect(r.x,y,r.w,font->height),text+at,(int)length,font,nk_rgba(0,0,0,0),color);
        at+=used;y+=font->height+5;
    }
}
#include "laboratory_sections.inc"
#include "laboratory_fields.inc"
static void workbench_icon_line(struct nk_command_buffer *canvas,float x,float y,float scale,float ax,float ay,float bx,float by,struct nk_color color) {
    float t=1.2f*scale;ax=x+ax*scale;ay=y+ay*scale;bx=x+bx*scale;by=y+by*scale;
    nk_stroke_line(canvas,ax,ay,bx,by,t,color);nk_fill_circle(canvas,nk_rect(ax-t/2,ay-t/2,t,t),color);nk_fill_circle(canvas,nk_rect(bx-t/2,by-t/2,t,t),color);
}
/* Direct vector translation of workbench.js: 17x17 viewbox, 1.2px round
 * strokes. These are separate from the app's generic window/folder icons. */
static void workbench_icon(struct nk_command_buffer *canvas,ps_ui_icon icon,struct nk_rect r,struct nk_color color) {
    float size=icon==PS_ICON_FOLDER?14:icon==PS_ICON_SIDEBAR || icon==PS_ICON_INSPECTOR?16:15;
    float s=size/17,x=r.x+(r.w-size)/2,y=r.y+(r.h-size)/2;
#define WB_LINE(ax,ay,bx,by) workbench_icon_line(canvas,x,y,s,ax,ay,bx,by,color)
    if(icon==PS_ICON_SIDEBAR || icon==PS_ICON_INSPECTOR) {
        nk_stroke_rect(canvas,nk_rect(x+1.5f*s,y+2.5f*s,14*s,12*s),2*s,1.2f*s,color);
        float divider=icon==PS_ICON_SIDEBAR?6:11;WB_LINE(divider,3,divider,14);
        if(icon==PS_ICON_SIDEBAR){WB_LINE(11,6,9,8.5f);WB_LINE(9,8.5f,11,11);}
        else {WB_LINE(6,6,8,8.5f);WB_LINE(8,8.5f,6,11);}
    } else if(icon==PS_ICON_SEARCH) {
        nk_stroke_circle(canvas,nk_rect(x+2.5f*s,y+2.5f*s,9*s,9*s),1.2f*s,color);WB_LINE(10.5f,10.5f,14.5f,14.5f);
    } else if(icon==PS_ICON_SAVE) {
        WB_LINE(3,2,12,2);WB_LINE(12,2,15,5);WB_LINE(15,5,15,15);WB_LINE(15,15,2,15);WB_LINE(2,15,2,2);WB_LINE(2,2,3,2);
        WB_LINE(5,2,5,7);WB_LINE(5,7,11,7);WB_LINE(11,7,11,2);WB_LINE(5,15,5,9);WB_LINE(5,9,12,9);WB_LINE(12,9,12,15);
    } else if(icon==PS_ICON_FOLDER) {
        WB_LINE(1.5f,5,1.5f,3);WB_LINE(1.5f,3,6.5f,3);WB_LINE(6.5f,3,8.5f,5);WB_LINE(8.5f,5,15.5f,5);
        WB_LINE(15.5f,5,15.5f,14);WB_LINE(15.5f,14,1.5f,14);WB_LINE(1.5f,14,1.5f,5);
    }
#undef WB_LINE
}
static bool workbench_side_button(app *a,struct nk_rect r,bool right,bool open,const char *name) {
    struct nk_context *ui=a->ui;struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    if(!(ui->current->layout->flags&NK_WINDOW_ROM) && nk_input_is_mouse_hovering_rect(&ui->input,r))nk_fill_rect(canvas,r,4,UI_COLOR(button_hover));
    workbench_icon(canvas,right?PS_ICON_INSPECTOR:PS_ICON_SIDEBAR,r,accent());
    bool pressed=ps_ui_button_at(ui,r,name,open);
    if(nk_sdl_accessibility_is_focused(ui,NULL,name))nk_stroke_rect(canvas,r,4,2,accent());
    return pressed;
}
static bool workbench_visible(const app *a);
static bool workbench_context_needs_scrollbar(app *a,struct nk_rect bounds);
// clang-format off
#include "batch_ui.inc"
#include "settings_ui.inc"
#include "project_manager_ui.inc"
#include "file_browser_ui.inc"
#include "timeline_ui.inc"
#include "file_tabs_ui.inc"
#include "docking_ui.inc"
#include "scene_tree_ui.inc"
// clang-format on
static void parameter_form_view(app *a) {
    struct nk_context *ui=a->ui;
    if(!a->built || !a->parameters.count)return;
    struct nk_rect content=nk_window_get_content_region(ui);
    bool expanded=workspace_section_begin(a,laboratory_visible(a)?"Laufparameter":"Parameter",&a->parameter_disclosure);
    struct nk_panel *layout=ui->current->layout;
    a->parameter_header_bounds=nk_rect(content.x,layout->at_y-(float)*layout->offset_y,content.w,
                                      ui->style.font->height+2*ui->style.tab.padding.y);
    if(laboratory_visible(a))a->parameter_header_bounds=a->laboratory_section_bounds[2];
    if (expanded) {
        nk_begin_disabled(ui, a->runner.running || a->reset_pending);
        for (uint32_t i = 0; i < a->parameters.count; i++) {
            const ps_parameter *parameter = &a->parameters.entries[i];
            const ps_parameter_unit *unit=&a->parameters.units[i];
            double scale=unit->declared?unit->scale:1;
            if(laboratory_visible(a)) {
                char low[64],high[64],standard[64],hint[320];snprintf(low,64,"%.6g",parameter->minimum/scale);snprintf(high,64,"%.6g",parameter->maximum/scale);snprintf(standard,64,"%.6g",parameter->default_value/scale);laboratory_decimal(low);laboratory_decimal(high);laboratory_decimal(standard);
                snprintf(hint,sizeof hint,"%s%sBereich %s bis %s · Standard %s",parameter->description,*parameter->description?" · ":"",low,high,standard);
                laboratory_field field=laboratory_edit_field(a,parameter->name,unit->declared?unit->symbol:"",hint,a->parameters.selected[i],sizeof a->parameters.selected[i],nk_filter_default,true,true);
                a->parameter_card_bounds[i]=field.card;a->parameter_label_bounds[i]=field.label;a->parameter_hint_bounds[i]=field.hint;a->parameter_input_bounds[i]=field.input;a->parameter_default_bounds[i]=field.action;
                if(field.changed)a->project_settings_dirty=true;
                if(field.clicked){snprintf(a->parameters.selected[i],sizeof a->parameters.selected[i],"%.17g",parameter->default_value/scale);a->project_settings_dirty=true;}
                double selected;if(ps_parameter_catalog_value(&a->parameters,i,&selected)!=PS_OK)laboratory_field_message(a,"Endlichen Wert innerhalb des gültigen Bereichs eingeben (Komma oder Punkt).",true);
                continue;
            }
            char detail[256];
            snprintf(detail,sizeof detail,"%s%s%s%s",parameter->name,
                     unit->declared?" [":"",unit->symbol,unit->declared?"]":"");
            nk_layout_row_dynamic(ui,documentation_height(a->font_ui,detail,strlen(detail),fmaxf(40,nk_window_get_content_region(ui).w-16))+8,1);
            nk_label_wrap(ui, detail);
            snprintf(detail, sizeof detail, "%s · Bereich %.6g bis %.6g · Standard %.6g",
                     parameter->description, parameter->minimum/scale, parameter->maximum/scale,
                     parameter->default_value/scale);
            nk_layout_row_dynamic(ui,documentation_height(a->font_ui,detail,strlen(detail),fmaxf(40,nk_window_get_content_region(ui).w-16))+8,1);
            nk_label_wrap(ui, detail);
            nk_layout_row_dynamic(ui,laboratory_visible(a)?46*a->preferences.ui_size/16.f:32,2);
            char previous[sizeof a->parameters.selected[i]];
            snprintf(previous, sizeof previous, "%s", a->parameters.selected[i]);
            a->parameter_input_bounds[i]=nk_widget_bounds(ui);
            if(laboratory_visible(a))nk_style_push_font(ui,&a->laboratory_value_font);
            nk_edit_string_zero_terminated(ui, NK_EDIT_FIELD, a->parameters.selected[i],
                                           sizeof a->parameters.selected[i], nk_filter_default);
            if(laboratory_visible(a))nk_style_pop_font(ui);
            if (strcmp(previous, a->parameters.selected[i]))
                a->project_settings_dirty = true;
            a->parameter_default_bounds[i]=nk_widget_bounds(ui);
            if (nk_button_label(ui, "Standard")) {
                snprintf(a->parameters.selected[i], sizeof a->parameters.selected[i], "%.17g",
                         parameter->default_value/scale);
                a->project_settings_dirty = true;
            }
            double selected;
            if (ps_parameter_catalog_value(&a->parameters, i, &selected) != PS_OK) {
                const char *message="Endlichen Wert innerhalb des gültigen Bereichs eingeben.";
                nk_layout_row_dynamic(ui,documentation_height(a->font_ui,message,strlen(message),fmaxf(40,nk_window_get_content_region(ui).w-16))+8,1);
                nk_style_push_color(ui,&ui->style.text.color,UI_COLOR(error));
                nk_label_wrap(ui, message);
                nk_style_pop_color(ui);
            }
        }
        nk_end_disabled(ui);
        workspace_section_end(a);
    }
}
static ps_display_unit channel_display_unit(app *a,const ps_channel *ch);
static void channel_unit_open(app *a,const ps_channel *ch);
static void channel_display_text(const ps_display_unit *u,double value,char *text,size_t size);
static bool channel_display_values(const ps_display_unit *u,const double *source,size_t n,double *out);
static void live_measurements_view(app *a) {
    struct nk_context *ui=a->ui;
    nk_layout_row_dynamic(ui,28,1);
    a->live_measurement_bounds=nk_widget_bounds(ui);
    nk_label(ui,"Messwerte",NK_TEXT_LEFT);
    nk_layout_row_dynamic(ui, 28, 1);
    if (a->channel_count) {
        const char *names[PS_MAX_CHANNELS];char labels[PS_MAX_CHANNELS][128];
        for (uint32_t i = 0; i < a->channel_count; i++) {
            ps_display_unit label_unit=channel_display_unit(a,&a->live_channels[i]);
            snprintf(labels[i],sizeof labels[i],"%s [%s]",a->live_channels[i].name,label_unit.symbol);
            names[i]=labels[i];
        }
        if (a->plot_channel >= (int)a->channel_count)
            a->plot_channel = 0;
        a->plot_channel = nk_combo(ui, names, (int)a->channel_count, a->plot_channel, 26,
                                   nk_vec2(280, 240));
        nk_layout_row_dynamic(ui,28,2);
        a->channel_unit_buttons[0]=nk_widget_bounds(ui);
        if(nk_button_label(ui,"Einheit …"))channel_unit_open(a,&a->live_channels[a->plot_channel]);
        ps_display_unit unit=channel_display_unit(a,&a->live_channels[a->plot_channel]);
        char value[96];
        const double *shown = display_values(a);
        channel_display_text(&unit,shown[a->plot_channel],value,sizeof value);
        int mask = a->channel_status[a->plot_channel];
        if (mask >= 0 && shown[mask] != 1)
            snprintf(value, sizeof value, "%s",
                     shown[mask] == 2 ? "Messung ausgefallen" : "Keine neue Messung");
        nk_label(ui, value, NK_TEXT_RIGHT);
        double displayed[PREVIEW];
        bool represented=channel_display_values(&unit,a->history_v[a->plot_channel],a->history_count,displayed);
        char title[160];snprintf(title,sizeof title,"Live-Messwerte [%s]%s",unit.symbol,represented?"":" · Werte nicht darstellbar");
        plot(a, a->history_t, displayed, a->history_count, 150,
             title, mask >= 0, NULL);
    } else {
        nk_label_wrap(ui,"Messwerte erscheinen während des Laufs.");

    }
    if (a->runner.running && ps_clock() - a->heartbeat > 3) {
        nk_layout_row_dynamic(ui, 28, 1);
        nk_label_colored(ui, "Keine Antwort. Mit Stoppen den Lauf beenden.", NK_TEXT_LEFT,
                         UI_COLOR(error));
    }
}
static const char *live_event_level(ps_log_level level) {
    return level==PS_LOG_ERROR?"Fehler":level==PS_LOG_WARNING?"Warnung":level==PS_LOG_INFO?"Info":"Debug";
}
static void live_event_text(app *a,const ps_log_record *record) {
    struct nk_context *ui=a->ui;char heading[96];
    snprintf(heading,sizeof heading,"%s · %.6g s",live_event_level(record->level),record->time_s);
    nk_layout_row_dynamic(ui,24,1);nk_label_colored(ui,heading,NK_TEXT_LEFT,
        record->level>=PS_LOG_WARNING?UI_COLOR(error):muted());
    char text[4*PS_LOG_MESSAGE_MAX+1];size_t used=0;
    for(size_t i=0;record->message[i];i++) {
        if(record->message[i]=='\t'){memcpy(text+used,"    ",4);used+=4;}
        else if(record->message[i]=='\r'){text[used++]='\n';if(record->message[i+1]=='\n')i++;}
        else text[used++]=record->message[i];
    }
    text[used]=0;float width=fmaxf(40,nk_window_get_content_region(ui).w-16);
    nk_layout_row_dynamic(ui,documentation_height(a->font_ui,text,used,width)+8,1);
    struct nk_rect bounds;
    if(nk_widget(&bounds,ui)!=NK_WIDGET_INVALID) {
        ps_ui_accessibility_label(ui,bounds,text);
        size_t at=0;float y=bounds.y+4;
        do{size_t length,consumed=documentation_line(a->font_ui,text+at,used-at,width,&length);
            nk_draw_text(nk_window_get_canvas(ui),nk_rect(bounds.x+8,y,width,a->font_ui->height+6),
                         text+at,(int)length,a->font_ui,nk_rgba(0,0,0,0),UI_COLOR(text));
            at+=consumed;y+=a->font_ui->height+6;
        }while(at<used);
    }
}
#include "laboratory_tail.inc"
static void live_events_view(app *a) {
    struct nk_context *ui=a->ui;
    if(laboratory_visible(a)){laboratory_events(a);return;}
    a->live_warning_bounds=nk_rect(0,0,0,0);
    if(a->live_warning_valid) {
        nk_layout_row_dynamic(ui,100,1);a->live_warning_bounds=nk_widget_bounds(ui);
        if(nk_group_begin(ui,"live-warning",NK_WINDOW_BORDER)){live_event_text(a,&a->live_warning);nk_group_end(ui);}
    }
    bool large=a->preferences.ui_size>=20;
    nk_layout_row_dynamic(ui,28,large?1:2);nk_label(ui,"Ereignisse",NK_TEXT_LEFT);
    if(large)nk_layout_row_dynamic(ui,28,1);
    if(nk_button_label(ui,"Protokoll"))protocol_open_from(a,"Protokoll");
    if(!a->live_event_count){nk_layout_row_dynamic(ui,38,1);nk_label_wrap(ui,"Noch keine Experimentmeldungen.");return;}
    nk_layout_row_dynamic(ui,140,1);a->live_event_bounds=nk_widget_bounds(ui);
    if(nk_group_scrolled_offset_begin(ui,&a->live_event_scroll_x,&a->live_event_scroll_y,"live-events",NK_WINDOW_BORDER)) {
        for(unsigned i=a->live_event_count;i>0;i--)live_event_text(a,&a->live_events[i-1]);
        nk_group_end(ui);
    }
    if(a->live_event_omitted) {
        char note[128];snprintf(note,sizeof note,"%llu ältere Meldungen in der Protokolldatei.",(unsigned long long)a->live_event_omitted);
        nk_layout_row_dynamic(ui,48,1);nk_label_wrap(ui,note);
    }
}
static bool laboratory_object_action(app *a,const char *label,bool secondary,struct nk_rect *bounds);
static void laboratory_note(app *a,const char *text);
#include "laboratory_camera.inc"
#include "laboratory_scene.inc"
static void scene_camera_controls_view(app *a,bool compact) {
    struct nk_context *ui=a->ui;
    if(laboratory_visible(a)){laboratory_camera(a,compact);return;}
    if (nk_tree_state_push(ui, NK_TREE_TAB, "Darstellung", &a->view_disclosure)) {
        nk_layout_row_dynamic(ui, 28, 1);
        bool planar=a->camera_2d;
        a->camera_mode_bounds=nk_widget_bounds(ui);
        if(nk_checkbox_label(ui,"2D-Modus",&planar)) scene_camera_2d(a,planar);
        a->camera_projection_bounds=nk_widget_bounds(ui);
        nk_checkbox_label(ui, "Orthografische Ansicht", &a->orthographic);
        if(!compact) {
        a->display_vector_bounds=nk_widget_bounds(ui);
        nk_checkbox_label(ui, "Vektoren", &a->show_vectors);
        a->display_path_bounds=nk_widget_bounds(ui);
        nk_checkbox_label(ui, "Flugbahnen", &a->show_paths);
        }
        a->display_points_bounds=nk_widget_bounds(ui);
        nk_checkbox_label(ui, "Punkte", &a->show_points);
        a->display_labels_bounds=nk_widget_bounds(ui);
        nk_checkbox_label(ui, "Beschriftungen", &a->show_labels);
        if(!compact)nk_checkbox_label(ui, "Raster und Achsen", &a->show_grid);
        nk_layout_row_dynamic(ui,a->preferences.ui_size>=20?60:32,1);
        a->display_save_bounds=nk_widget_bounds(ui);
        nk_begin_disabled(ui,!a->loaded || a->display_save_thread || a->recovery);
        if(nk_button_label(ui,"Als Projektvorgabe speichern")) project_display_save(a);
        nk_end_disabled(ui);
        nk_tree_state_pop(ui);
    }
    if (nk_tree_state_push(ui, NK_TREE_TAB, "Kamera", &a->camera_disclosure)) {
        nk_layout_row_dynamic(ui, 22, 1);
        nk_label_colored(ui, "Blickrichtung", NK_TEXT_LEFT, muted());
        nk_layout_row_dynamic(ui, 32, 2);
        a->camera_preset_bounds[0]=nk_widget_bounds(ui);
        if (nk_button_label(ui, "Standard")) {
            a->camera_2d = false;
            a->yaw = PS_CAMERA_DEFAULT.yaw;
            a->pitch = PS_CAMERA_DEFAULT.pitch;
            a->zoom = PS_CAMERA_DEFAULT.distance;
            a->camera_target = PS_CAMERA_DEFAULT.target;
            a->orthographic = PS_CAMERA_DEFAULT.orthographic;
        }
        a->camera_preset_bounds[1]=nk_widget_bounds(ui);
        if (nk_button_label(ui, "Vorne")) {
            scene_camera_plane(a,0);
            a->orthographic = true;
        }
        a->camera_preset_bounds[2]=nk_widget_bounds(ui);
        if (nk_button_label(ui, "Seite")) {
            scene_camera_plane(a,1);
            a->orthographic = true;
        }
        a->camera_preset_bounds[3]=nk_widget_bounds(ui);
        if (nk_button_label(ui, "Oben")) {
            scene_camera_plane(a,2);
            a->orthographic = true;
        }
        nk_layout_row_dynamic(ui, 26, 1);
        if(a->camera_2d) {
            const char *planes[]={"2D-Ebene: XY","2D-Ebene: YZ","2D-Ebene: XZ"};
            nk_label(ui,planes[a->camera_plane],NK_TEXT_LEFT);
        } else {
            nk_label(ui, "Orbit", NK_TEXT_LEFT);
            ps_ui_slider_named(ui,"Kamera seitlich (rad)",-3.14f,&a->yaw,3.14f,.02f);
            nk_label(ui, "Neigung", NK_TEXT_LEFT);
            ps_ui_slider_named(ui,"Kamera hoch (rad)",-(float)PS_PI/2,&a->pitch,(float)PS_PI/2,.02f);
        }
        nk_label(ui, "Zoom", NK_TEXT_LEFT);
        ps_ui_slider_named(ui,"Kameraabstand (m)",1.5f,&a->zoom,12,.1f);
        nk_layout_row_dynamic(ui, 60, 1);
        nk_label_wrap(ui,a->camera_2d?"Rechts/Mitte ziehen: verschieben\nMausrad: zoomen":"Rechts ziehen: drehen\nMitte ziehen: verschieben\nMausrad: zoomen");
        nk_layout_row_dynamic(ui, 112, 1);
        nk_label_wrap(ui,a->camera_2d?"Alt + Pfeile: verschieben. Alt + 2: 2D/3D. Alt + Bild auf/ab: zoomen. Alt + Pos1: Standard.":"Alt + Pfeile: drehen. Alt + Shift + Pfeile: verschieben. Alt + 2: 2D/3D. Alt + Bild auf/ab: zoomen. Alt + Pos1: Standard.");
        nk_tree_state_pop(ui);
    }
}
static void scene_controls_view(app *a) {
    struct nk_context *ui=a->ui;
    scene_selection_sync(a);
    if (a->scene_selected && a->scene_selected_slot < display_scene(a)->count) {
        char selected[96];
        if (a->scene_selected_id) snprintf(selected, sizeof selected, "Auswahl: ID %u", a->scene_selected_id);
        else snprintf(selected, sizeof selected, "Auswahl: Eintrag %u", a->scene_selected_slot + 1);
        nk_layout_row_dynamic(ui, 26, 1);
        nk_label(ui, selected, NK_TEXT_LEFT);
        const ps_scene *scene=display_scene(a);const ps_object *object=&scene->objects[a->scene_selected_slot];
        if(object->shape!=PS_GROUP) {
            ps_vec3 world,local=object->shape==PS_FRAME?ps_v3(0,0,0):object->a;
            if(ps_scene_world_point(scene,a->scene_selected_slot,local,&world)==PS_OK) {
                char position[128];
                snprintf(position,sizeof position,"Welt [m]: %.6g / %.6g / %.6g",world.x,world.y,world.z);
                nk_layout_row_dynamic(ui,laboratory_visible(a)?documentation_height(ui->style.font,position,strlen(position),nk_window_get_content_region(ui).w)+8:20,1);nk_label_wrap(ui,position);
                snprintf(position,sizeof position,"Lokal [m]: %.6g / %.6g / %.6g",object->a.x,object->a.y,object->a.z);
                nk_layout_row_dynamic(ui,laboratory_visible(a)?documentation_height(ui->style.font,position,strlen(position),nk_window_get_content_region(ui).w)+8:20,1);nk_label_wrap(ui,position);
                if(object->shape==PS_FRAME) {
                    nk_layout_row_dynamic(ui,20,1);
                    snprintf(position,sizeof position,"Skalierung: %.6g / %.6g / %.6g",object->b.x,object->b.y,object->b.z);nk_label(ui,position,NK_TEXT_LEFT);
                }
            }
        }
        nk_layout_row_dynamic(ui,26,1);
        a->scene_hide_selection_bounds = nk_widget_bounds(ui);
        if (nk_button_label(ui, "Auswahl ausblenden")) {
            a->scene_view.hidden |= UINT32_C(1) << a->scene_selected_slot;
            a->scene_selected = false;
        }
    }
    if (nk_tree_state_push(ui, NK_TREE_TAB, "Szeneneinträge", &a->scene_disclosure)) {
        nk_layout_row_dynamic(ui, 26, 1);
        a->display_all_bounds=nk_widget_bounds(ui);
        if (nk_button_label(ui, "Alle einblenden")) a->scene_view.hidden = 0;
        const char *hint="Alt + N/P: Auswahl. Alt + H: ausblenden.";
        nk_layout_row_dynamic(ui,fmaxf(26,documentation_height(ui->style.font,hint,strlen(hint),nk_window_get_content_region(ui).w)+8),1);
        nk_style_push_color(ui,&ui->style.text.color,muted());nk_label_wrap(ui,hint);nk_style_pop_color(ui);
        scene_tree_view(a);
        nk_tree_state_pop(ui);
    }
    scene_camera_controls_view(a,false);
}
static void camera_inspector(app *a) {
    struct nk_context *ui=a->ui;
    if(laboratory_visible(a)){laboratory_run_settings(a);return;}
    if(!laboratory_visible(a)) {
        parameter_form_view(a);
        if(a->tab==1){if(!a->analysis_only){live_measurements_view(a);live_events_view(a);}}
        else scene_controls_view(a);
    }
    a->breakpoint_input_active=false;
    if (workspace_section_begin(a,"Laufeinstellungen",&a->steps_disclosure)) {
        nk_layout_row_dynamic(ui, 24, 1);
        nk_label(ui, "Zeitschritt in Sekunden", NK_TEXT_LEFT);
        nk_begin_disabled(ui, a->runner.running || a->reset_pending);
        double previous_dt = a->dt;
        char previous_seed[sizeof a->seed];
        snprintf(previous_seed, sizeof previous_seed, "%s", a->seed);
        if(a->adaptive_steps) step_bound_editor(a,2,"Start (s)",&a->dt);
        else {
            nk_layout_row_dynamic(ui,laboratory_visible(a)?46*a->preferences.ui_size/16.f:32,1);
            if(laboratory_visible(a))nk_style_push_font(ui,&a->laboratory_value_font);
            nk_property_double(ui, "dt", DBL_MIN, &a->dt, 1, .001, .0001f);
            if(laboratory_visible(a))nk_style_pop_font(ui);
        }
        nk_layout_row_dynamic(ui,26,1);
        bool previous_adaptive=a->adaptive_steps;
        double previous_minimum=a->minimum_dt,previous_maximum=a->maximum_dt;
        a->adaptive_bounds=nk_widget_bounds(ui);
        nk_checkbox_label(ui,"Adaptive Schritte",&a->adaptive_steps);
        if(a->adaptive_steps) {
            nk_layout_row_dynamic(ui,68,1);
            nk_label_wrap(ui,"dt ist der Startschritt. Das Modell muss adaptive Schritte unterstützen.");
            step_bound_editor(a,0,"Minimum (s)",&a->minimum_dt);
            step_bound_editor(a,1,"Maximum (s)",&a->maximum_dt);
            ps_project_settings steps={.timestep=a->dt,.adaptive=true,
                .minimum_timestep=a->minimum_dt,.maximum_timestep=a->maximum_dt};
            if(!ps_project_step_bounds_valid(&steps)) {
                nk_layout_row_dynamic(ui,52,1);
                nk_label_wrap(ui,"Positive Zahlen wählen: Minimum ≤ dt ≤ Maximum ≤ 1 s.");
            }
        }
        if(a->loaded && (previous_adaptive!=a->adaptive_steps || previous_minimum!=a->minimum_dt ||
                         previous_maximum!=a->maximum_dt)) a->project_settings_dirty=true;
        nk_layout_row_dynamic(ui, 24, 1);
        nk_label(ui, "Zufallsseed", NK_TEXT_LEFT);
        nk_layout_row_dynamic(ui,laboratory_visible(a)?46*a->preferences.ui_size/16.f:32,1);
        if(laboratory_visible(a))nk_style_push_font(ui,&a->laboratory_value_font);
        nk_edit_string_zero_terminated(ui, NK_EDIT_FIELD, a->seed, sizeof a->seed,
                                       nk_filter_decimal);
        if(laboratory_visible(a))nk_style_pop_font(ui);
        if (!a->analysis_only && nk_tree_state_push(ui, NK_TREE_TAB, "Haltepunkte", &a->breakpoint_disclosure)) {
            char previous[sizeof a->step_breakpoints]; strcpy(previous, a->step_breakpoints);
            nk_layout_row_dynamic(ui, 26, 1);
            nk_label(ui, "Akzeptierte Schritte:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ui, 28, 1);
            nk_label(ui, "10,100,500 · leer: aus", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ui, 32, 1); a->breakpoint_bounds = nk_widget_bounds(ui);
            nk_flags edit=nk_edit_string_zero_terminated(ui, NK_EDIT_FIELD, a->step_breakpoints,
                                           sizeof a->step_breakpoints, nk_filter_default);
            a->breakpoint_input_active=(edit & NK_EDIT_ACTIVE)!=0;
            if (a->loaded && strcmp(previous, a->step_breakpoints)) a->project_settings_dirty = true;
            ps_step_breakpoints points;
            if (!ps_step_breakpoints_parse(a->step_breakpoints, &points)) {
                nk_layout_row_dynamic(ui, 54, 1);
                nk_label_wrap(ui, "Bis zu 32 verschiedene ganze Zahlen von 0 bis 1000000000 eingeben.");
            }
            nk_tree_pop(ui);
        }
        nk_end_disabled(ui);
        if (a->loaded && (previous_dt != a->dt || strcmp(previous_seed, a->seed)))
            a->project_settings_dirty = true;
        uint64_t parsed_seed;
        if (!ps_project_seed_parse(a->seed, &parsed_seed)) {
            nk_layout_row_dynamic(ui, 52, 1);
            nk_label_wrap(ui, "Seed: ganze Zahl von 0 bis 18446744073709551615 eingeben.");
        }
        nk_tree_pop(ui);
    }
}
#include "document_ui.inc"
#include "workspace_tree_ui.inc"
#include "window_header.inc"
#include "run_details_ui.inc"
#include "layout_ui.inc"
#include "channel_units_ui.inc"
#include "workspace_catalog_ui.inc"
#include "toolbar_ui.inc"
static void analysis_runs_view(app *a);
static void analysis_details_view(app *a);
static void laboratory_scene_sidebar(app *a);
static void sidebar_panel(app *a) {
    struct nk_context *ui=a->ui;
    bool compact=workbench_visible(a),laboratory=laboratory_visible(a);
    if(laboratory){nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(8,0));nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,0));nk_style_push_vec2(ui,&ui->style.window.scrollbar_size,nk_vec2(ui->style.window.scrollbar_size.x,0));}
    if(analysis_visible(a)) {
        nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(12,18));
        nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(UI_COLOR(header)));
        if(dock_begin(a,PS_DOCK_SIDEBAR))analysis_runs_view(a);
        nk_end(ui);nk_style_pop_style_item(ui);nk_style_pop_vec2(ui);return;
    }
    if(compact){nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(8,0));nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,0));}
    if (dock_begin(a, PS_DOCK_SIDEBAR)) {
        if(workbench_visible(a) && a->project_build_disclosure!=NK_MAXIMIZED) {
            if(a->workspace_open)workspace_tree_view(a);
            else {nk_layout_row_dynamic(ui,56,1);nk_label_wrap(ui,"Öffne einen Ordner, um seine Dateien hier zu sehen.");}
            nk_end(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);return;
        }
        if(a->loaded && a->tab==1) {
            if(laboratory)laboratory_scene_sidebar(a);else scene_controls_view(a);nk_end(ui);
            if(laboratory){nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);}
            if(compact){nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);}return;
        }
        {
            if (nk_tree_state_push(ui, NK_TREE_TAB, "Workspace", &a->workspace_disclosure)) {
                if (a->workspace_open) {
                    workspace_tree_view(a);
                } else {
                    nk_layout_row_dynamic(ui, 45, 1);
                    nk_label_wrap(ui, "Öffne einen Ordner, um seine Dateien hier zu sehen.");
                }
                workspace_section_end(a);
            }
            document_list_view(a);
            if (a->loaded) {
                nk_layout_row_dynamic(ui, 32, 1);
                if (action_button(ui, "Läufe & Berichte", a->tab == 4))
                    open_library(a);
                a->batch_navigation_bounds = nk_widget_bounds(ui);
                if (action_button(ui, "Monte Carlo", a->tab == 5))
                    a->tab = 5;
                nk_layout_row_dynamic(ui, 32, 1);
                if (nk_button_label(ui, "Messdaten im Ordner")) {
                    char uri[4200];
                    snprintf(uri, sizeof uri, "file:///%s/runs", a->project);
                    SDL_OpenURL(uri);
                }
                {
                    section(ui, "Dateien");
                    nk_layout_row_dynamic(ui, 32, 1);
                    char source_label[32];
                    if(!a->analysis_only) {
                    snprintf(source_label, sizeof source_label, "%s%s", experiment_source(a),
                             a->dirty ? "  •" : "");
                    if (nk_button_label(ui, source_label))
                        a->tab = 0;
                    }
                    snprintf(source_label, sizeof source_label, "%s%s", analysis_source(a),
                             a->analysis_dirty ? "  •" : "");
                    if (nk_button_label(ui, source_label)) {
                        a->tab = 2;
                        a->analysis_tab = 1;
                    }
                    if (nk_tree_state_push(ui, NK_TREE_TAB, "Build-Einstellungen", &a->project_build_disclosure)) {
                        nk_layout_row_dynamic(ui,24,1);char version[80];snprintf(version,sizeof version,"Projektformat %u",a->project_format_version);
                        nk_label(ui,version,NK_TEXT_LEFT);
                        if(a->project_format_version==1) {
                            nk_layout_row_dynamic(ui,30,1);
                            a->migration_bounds=nk_widget_bounds(ui);
                            if(action_button(ui,"Projektformat aktualisieren",idle(a) && !a->dirty && !a->analysis_dirty && !a->project_settings_dirty))migrate_current_project(a);
                        }
                        if(!a->project_id[0] || (!a->analysis_only && !a->experiment_id[0])) {
                            nk_layout_row_dynamic(ui,30,1);a->identity_assignment_bounds=nk_widget_bounds(ui);
                            if(action_button(ui,"Projekt-IDs vergeben",idle(a) && !a->dirty && !a->analysis_dirty && !a->project_settings_dirty))assign_current_project_ids(a);
                        }
                        nk_layout_row_dynamic(ui, 32, 1);
                        const char *profiles[] = {"Debug", "Release"};
                        a->project_profile_bounds=nk_widget_bounds(ui);
                        float profile_height=56+2*ui->style.window.spacing.y+ui->style.window.combo_padding.y+2*ui->style.window.combo_border;
                        if(nk_combo_begin_label(ui,profiles[a->profile],nk_vec2(180,fminf(110,profile_height)))) {
                            nk_layout_row_dynamic(ui,28,1);
                            for(int profile=0;profile<2;profile++) {
                                a->project_profile_choices[profile]=nk_widget_bounds(ui);
                                if(nk_combo_item_label(ui,profiles[profile],NK_TEXT_LEFT))select_build_profile(a,profile);
                            }
                            nk_combo_end(ui);
                        }
                        nk_layout_row_dynamic(ui, 24, 1);
                        char api[96];
                        snprintf(api, sizeof api, "App: API %u · ABI %u", PS_API_VERSION, PS_ABI_VERSION);
                        nk_label(ui, api, NK_TEXT_LEFT);
                        if (a->build_info_valid) {
                            if (a->build_info.api_version) {
                                snprintf(api, sizeof api, "Build: API %u · ABI %u", a->build_info.api_version, a->build_info.abi_version);
                                nk_layout_row_dynamic(ui, 24, 1);
                                nk_label(ui, api, NK_TEXT_LEFT);
                            }
                            nk_layout_row_dynamic(ui, 24, 1);
                            nk_label(ui, a->build_info.release ? "Letzter Build: Release" : "Letzter Build: Debug", NK_TEXT_LEFT);
                            char compiler_label[sizeof a->build_info.compiler+16];
                            snprintf(compiler_label, sizeof compiler_label, "Compiler: %s", a->build_info.compiler);
                            nk_layout_row_dynamic(ui, 48, 1);
                            nk_label_wrap(ui, compiler_label);
                            if (nk_widget_is_hovered(ui)) nk_tooltip(ui, a->build_info.compiler);
                        } else {
                            nk_layout_row_dynamic(ui, 42, 1);
                            nk_label_wrap(ui, "Der Compiler wird nach einem erfolgreichen Build angezeigt.");
                        }
                        nk_tree_pop(ui);
                    }
                    nk_layout_row_dynamic(ui, 48, 1);
                    nk_label_wrap(ui, a->built ? "Build erfolgreich"
                                      : (a->dirty || a->analysis_dirty)
                                          ? "Ungespeicherte Änderungen"
                                          : "Build erforderlich");
                    if (a->autosave_error[0]) {
                        nk_layout_row_dynamic(ui, 76, 1);
                        nk_label_wrap(ui, a->autosave_error);
                    } else if (a->dirty || a->analysis_dirty) {
                        nk_layout_row_dynamic(ui, 42, 1);
                        char note[96];
                        snprintf(note, sizeof note, "Automatische Sicherung alle %u Sekunden",
                                 a->preferences.autosave_seconds);
                        nk_label_wrap(ui, a->autosave_current ? "Automatische Sicherung vorhanden"
                                                              : note);
                    }
                }
            }
        }
    }
    nk_end(ui);
    if(compact){nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);}
    if(laboratory){nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);}
}
#include "workbench_text_ui.inc"
#include "source_outline_ui.inc"
#include "workbench_api_ui.inc"
#include "workbench_context_ui.inc"
#include "laboratory_ui.inc"
static void laboratory_channel_inspector(app *a);
#include "laboratory_sidebar.inc"
#include "model_edit_ui.inc"
static void run_limits_view(app *a) { laboratory_limits(a); }
static void inspector_panel(app *a) {
    struct nk_context *ui=a->ui;
    memset(a->workbench_api_text_bounds,0,sizeof a->workbench_api_text_bounds);
    if(analysis_visible(a)) {
        nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(18,18));
        nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(UI_COLOR(header)));
        if(dock_begin(a,PS_DOCK_INSPECTOR))analysis_details_view(a);
        nk_end(ui);nk_style_pop_style_item(ui);nk_style_pop_vec2(ui);return;
    }
    bool laboratory=laboratory_visible(a);
    if(laboratory){nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(0,0));nk_style_push_vec2(ui,&ui->style.window.group_padding,nk_vec2(16,4));nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,0));}
    bool compact=workbench_visible(a);
    if(compact){nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(16,4));nk_style_push_vec2(ui,&ui->style.window.scrollbar_size,nk_vec2(ui->style.window.scrollbar_size.x,0));}
    if(dock_begin(a,PS_DOCK_INSPECTOR)) {
        if(a->api_reference && !compact && (a->tab==0 || (a->tab==2 && a->analysis_tab==1) || a->tab==8))
            api_reference_view(a);
        else if(workbench_visible(a)) {
            workbench_context_view(a);
        } else if(a->loaded) {
            if(laboratory_visible(a)) {
                struct nk_rect inspector=nk_window_get_content_region(ui);
                float footer_height=laboratory_inspector_footer_height(a,inspector.w);
                float body=fmaxf(40,inspector.h-footer_height);
                memset(a->laboratory_section_bounds,0,sizeof a->laboratory_section_bounds);
                nk_layout_row_dynamic(ui,body,1);
                a->laboratory_inspector_body=nk_widget_bounds(ui);
                if(nk_group_scrolled_offset_begin(ui,&a->laboratory_scroll_x,&a->laboratory_scroll_y,"Messlabor-Inspektor",0)) {
                    /* The native bar overlays the reference's right padding.
                     * Retain Nuklear's scroll/capture behavior; only grant the
                     * reserved bar width to content, with horizontal overflow hidden. */
                    struct nk_panel *inspector_layout=ui->current->layout;
                    struct nk_rect saved_bounds=inspector_layout->bounds,saved_clip=inspector_layout->clip;
                    float overlay=ui->style.window.scrollbar_size.x;
                    inspector_layout->bounds.w+=overlay;inspector_layout->clip.w+=overlay;
                    nk_push_scissor(nk_window_get_canvas(ui),inspector_layout->clip);
                    a->laboratory_scroll_x=0;
                    /* The reference has one scrollable inspector body. A model
                     * error/build banner must not pin away all camera/settings
                     * space in a small window. Its actions scroll with the fields. */
                    model_edit_banner(a);
                    laboratory_tail_begin(a);laboratory_warning(a);
                    if(a->runner.running && ps_clock()-a->heartbeat>3)laboratory_note(a,"Keine Runner-Antwort. Stoppen bleibt oben erreichbar.");
                    laboratory_object(a);
                    model_edit_fields(a);
                    parameter_form_view(a);
                    camera_inspector(a);
                    run_limits_view(a);
                    if(workspace_section_begin(a,"Messkanal & Einheit",&a->laboratory_groups[1])) {
                        laboratory_channel_inspector(a);
                        workspace_section_end(a);
                    }
                    if(workspace_section_begin(a,"Ereignisse",&a->laboratory_groups[2])){live_events_view(a);workspace_section_end(a);}
                    if(workspace_section_begin(a,"Kamera und Darstellung",&a->laboratory_groups[3])){scene_camera_controls_view(a,false);workspace_section_end(a);}
                    if(workspace_section_begin(a,"Aufzeichnung",&a->laboratory_groups[4])){
                        laboratory_recording(a);workspace_section_end(a);
                    }
                    inspector_layout->bounds=saved_bounds;inspector_layout->clip=saved_clip;
                    inspector_layout->max_x=fminf(inspector_layout->max_x,saved_bounds.x+saved_bounds.w);
                    nk_push_scissor(nk_window_get_canvas(ui),saved_clip);
                    nk_group_end(ui);a->laboratory_scroll_x=0;
                }
                a->laboratory_footer_bounds=nk_rect(inspector.x,inspector.y+body,inspector.w,footer_height);
                laboratory_inspector_footer(a,a->laboratory_footer_bounds);
            } else camera_inspector(a);
            if(!laboratory_visible(a))run_limits_view(a);
        } else {
            nk_layout_row_dynamic(ui,64,1);
            nk_label_wrap(ui,"Öffne ein Physim-Projekt, um Szene, Kamera und Experimentparameter zu untersuchen.");
        }
    }
    nk_end(ui);
    if(compact){nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);}
    if(laboratory){nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);}
}
#include "analysis_ui.inc"
#include "laboratory_channel.inc"
static void workspace_panel(app *a) {
    struct nk_context *ui=a->ui;
    bool compact=editor_search_target(a)!=NULL;
    bool laboratory=laboratory_visible(a),analysis=analysis_visible(a);
    if(analysis){nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(24,0));nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(8,0));nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(UI_COLOR(edit)));}
    if(laboratory){nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(0,0));nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,0));}
    if(compact) {
        nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(6,0));
        nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,0));
        nk_style_push_vec2(ui,&ui->style.edit.padding,nk_vec2(6,6));
        nk_style_push_float(ui,&ui->style.edit.rounding,0);
        nk_style_push_float(ui,&ui->style.edit.row_padding,11);
        nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(UI_COLOR(edit)));
    }
    float content=fmaxf(100,a->dock_panels[PS_DOCK_WORKSPACE].h-dock_panel_header_height(a,PS_DOCK_WORKSPACE));
    float status_space=workbench_visible(a) && (!a->dock_visible[PS_DOCK_LOG] ||
        fabsf(a->dock_panels[PS_DOCK_WORKSPACE].x-a->dock_panels[PS_DOCK_LOG].x)>=2 ||
        (a->preferences.dock.floating&((1u<<PS_DOCK_LOG)|(1u<<PS_DOCK_WORKSPACE))))?24:0;
    if (a->dock_visible[PS_DOCK_WORKSPACE] && a->tab==4 && (a->library_search_focus || a->library_search_active))
        nk_window_set_focus(ui,"workspace");
    /* A header's native focus request can be granted after the frame's initial
     * ownership check. Respect it before enabling the background editor. */
    if(a->file_tab_keyboard_edit && nk_sdl_accessibility_has_focus(ui) && !nk_sdl_accessibility_keyboard_owned(ui))
        a->file_tab_keyboard_edit=NULL;
    if(a->file_tab_keyboard_edit && editor_search_target(a)==a->file_tab_keyboard_edit)
        nk_window_set_focus(ui,"workspace");
    if (dock_begin(a, PS_DOCK_WORKSPACE)) {
        if (a->tab == 8 && active_document(a)) {
            document_view(a, content-status_space);
        } else if (a->tab == 6)
            settings_view(a);
        else if (a->tab == 4 && a->loaded)
            library_view(a, content);
        else if (a->tab == 5 && a->loaded)
            batch_view(a);
        else if (!a->loaded) {
            nk_layout_row_dynamic(ui, 34, 1);
            nk_spacer(ui);
            nk_style_push_font(ui, a->font_title);
            nk_layout_row_dynamic(ui, 42, 1);
            nk_label(ui, a->workspace_open ? "Ordner geöffnet" : "Leerer Workspace", NK_TEXT_LEFT);
            nk_style_pop_font(ui);
            if (!a->workspace_open) {
                nk_layout_row_dynamic(ui, 64, 1);
                nk_label_wrap(ui, "Öffne einen Ordner oder lege ein neues Physim-Projekt an. Dateien und weitere Ordner kannst du anschließend hinzufügen.");
                nk_layout_row_static(ui, 36, 220, 2);
                if (action_button(ui, "Ordner öffnen", true))
                    choose_workspace_path_from(a,PS_DIALOG_OPEN_FOLDER,"Ordner öffnen");
                if (nk_button_label(ui, "Neues Projekt"))
                    open_project_manager(a,"Neues Projekt");
                if (a->last_workspace.root[0]) {
                    nk_layout_row_dynamic(ui, 32, 1);
                    nk_label_colored(ui, a->last_workspace.root, NK_TEXT_LEFT, muted());
                    nk_layout_row_static(ui, 36, 260, 2);
                    a->workspace_restore_bounds = nk_widget_bounds(ui);
                    if (nk_button_label(ui, "Letzten Workspace öffnen"))
                        restore_workspace(a);
                    if (nk_button_label(ui, "Eintrag vergessen"))
                        forget_workspace(a);
                }
                if (a->workspace_state_error[0]) {
                    nk_layout_row_dynamic(ui, 64, 1);
                    nk_label_wrap(ui, a->workspace_state_error);
                    nk_layout_row_static(ui, 36, 260, 1);
                    if (nk_button_label(ui, "Workspace zurücksetzen"))
                        forget_workspace(a);
                }
            } else {
                nk_layout_row_dynamic(ui, 64, 1);
                nk_label_wrap(ui, "Kein Physim-Projekt erkannt. Wähle links eine Textdatei zum Bearbeiten oder lege über Datei ein Projekt an.");
            }
        } else if (a->tab == 0 || (a->tab == 2 && a->analysis_tab == 1)) {
            bool analysis = a->tab == 2 || a->analysis_only;
            struct nk_text_edit *edit = analysis ? &a->analysis : &a->experiment;
            editor(a, edit, nk_window_get_content_region(ui).h - 2*ui->style.window.padding.y-status_space);
        } else if (a->tab == 1) {
            laboratory_workspace(a);
        } else {
            analysis_workspace_view(a,content);
        }
    }
    nk_end(ui);
    if(compact){nk_style_pop_style_item(ui);nk_style_pop_float(ui);nk_style_pop_float(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);}
    if(laboratory){nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);}
    if(analysis){nk_style_pop_style_item(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);}
}
static void workbench_log_panel(app *a) {
    struct nk_context *ui=a->ui;
    bool detailed=a->show_log && (a->diagnostic_count || a->diagnostic_raw);
    nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(18,detailed?8:18));
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(6,5));
    if(dock_begin(a,PS_DOCK_LOG)) {
        const struct nk_user_font *font=ui->style.font;
        float body=fmaxf(24,nk_window_get_content_region(ui).h-24);
        float action_width=fmaxf(82,font->width(font->userdata,font->height,"Diagnosen",9)+2*ui->style.button.padding.x+16);
        nk_layout_row_begin(ui,NK_STATIC,fmaxf(24,font->height+2*ui->style.button.padding.y),2);
        nk_layout_row_push(ui,fmaxf(40,nk_window_get_content_region(ui).w-action_width-6));
        const char *title=detailed?(a->diagnostic_raw?"Build-Ausgabe":"Diagnosen"):a->job.running?"Hintergrundjob läuft …":a->built?"Experiment bereit.":"Bereit zum Bauen.";
        if(detailed)nk_label_colored(ui,title,NK_TEXT_LEFT,UI_COLOR(text));
        else {
            struct nk_rect bounds;nk_widget(&bounds,ui);struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
            struct nk_rect circle=nk_rect(bounds.x,bounds.y,24,24);
            nk_stroke_circle(canvas,circle,1,UI_COLOR(border));
            struct nk_color good=a->preferences.theme==PS_THEME_HIGH_CONTRAST?UI_COLOR(accent):a->preferences.theme==PS_THEME_LIGHT?nk_rgb(40,98,64):nk_rgb(139,204,165);
            if(a->built){nk_stroke_line(canvas,circle.x+7,circle.y+12,circle.x+10,circle.y+16,1.5f,good);nk_stroke_line(canvas,circle.x+10,circle.y+16,circle.x+18,circle.y+7,1.5f,good);}
            const struct nk_user_font *bold=&a->workbench_fonts[3];
            nk_draw_text(canvas,nk_rect(bounds.x+40,bounds.y+(bounds.h-bold->height)/2,bounds.w-40,bold->height),title,(int)strlen(title),bold,nk_rgba(0,0,0,0),UI_COLOR(text));
        }
        nk_layout_row_push(ui,action_width);a->diagnostic_mode_bounds=nk_widget_bounds(ui);
        nk_style_push_style_item(ui,&ui->style.button.normal,nk_style_item_color(UI_COLOR(window)));
        if(nk_button_label(ui,a->diagnostic_raw?"Diagnosen":"Ausgabe"))a->diagnostic_raw=!a->diagnostic_raw;
        nk_style_pop_style_item(ui);nk_layout_row_end(ui);
        if(!detailed) {
            nk_style_push_font(ui,&a->workbench_fonts[1]);
            nk_layout_row_begin(ui,NK_STATIC,16,2);nk_layout_row_push(ui,40);nk_spacer(ui);
            nk_layout_row_push(ui,fmaxf(1,nk_window_get_content_region(ui).w-46));nk_label_colored(ui,a->status,NK_TEXT_LEFT,muted());nk_layout_row_end(ui);
            nk_style_pop_font(ui);
        }
        if(detailed) {
            nk_layout_row_dynamic(ui,fmaxf(20,body-29),1);
            if(a->diagnostic_count && !a->diagnostic_raw) {
                if(nk_group_begin(ui,"Compilerdiagnosen",0)) {
                    a->diagnostic_view_bounds=ui->current->layout->clip;
                    nk_layout_row_dynamic(ui,30,1);
                    for(int i=0;i<a->diagnostic_count;i++) {
                        a->diagnostic_bounds[i]=nk_widget_bounds(ui);
                        if(nk_button_label(ui,a->diagnostics[i].text))jump_to_diagnostic(a,&a->diagnostics[i]);
                    }
                    nk_group_end(ui);
                }
            } else {
                nk_style_push_font(ui,font);
                nk_edit_string_zero_terminated(ui,NK_EDIT_BOX|NK_EDIT_READ_ONLY|NK_EDIT_GOTO_END_ON_ACTIVATE,a->log,sizeof a->log,nk_filter_default);
                nk_style_pop_font(ui);
            }
        }
    }
    nk_end(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
}
static bool compact_log_diagnostic_button(app *a,const char *text) {
    struct nk_context *ui=a->ui;struct nk_rect r;nk_widget(&r,ui);
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    bool hover=!(ui->current->layout->flags&NK_WINDOW_ROM) && nk_input_is_mouse_hovering_rect(&ui->input,r);
    nk_fill_rect(canvas,r,ui->style.button.rounding,hover?UI_COLOR(button_hover):UI_COLOR(button));
    if(ui->style.button.border>0)nk_stroke_rect(canvas,r,ui->style.button.rounding,ui->style.button.border,ui->style.button.border_color);
    const struct nk_user_font *font=ui->style.font;
    float inset=ui->style.button.padding.x+ui->style.button.border+ui->style.button.rounding;
    /* A partly clipped row can still contain visible glyphs. Submit their full
     * height; the stock button's tiny content rectangle can be culled first. */
    nk_draw_text(canvas,nk_rect(r.x+inset,r.y+(r.h-font->height)/2,fmaxf(1,r.w-2*inset),font->height),
        text,(int)strlen(text),font,nk_rgba(0,0,0,0),UI_COLOR(text));
    return ps_ui_button_at(ui,r,text,false);
}
#include "protocol_ui.inc"
static void log_panel(app *a) {
    if(protocol_visible(a) || a->panel_render==PS_DOCK_LOG+1){protocol_panel(a);return;}
    if(workbench_visible(a)){workbench_log_panel(a);return;}
    struct nk_context *ui=a->ui;
    bool expanded_log=a->show_log && a->tab!=6;
    bool compact=laboratory_visible(a) && a->dock_panels[PS_DOCK_LOG].h-dock_panel_header_height(a,PS_DOCK_LOG)<126;
    float footer=fmaxf(68,a->dock_panels[PS_DOCK_LOG].h-16);
    nk_style_push_vec2(ui, &ui->style.window.padding, nk_vec2(16, compact?0:8));
    if(compact)nk_style_push_vec2(ui,&ui->style.window.scrollbar_size,nk_vec2(ui->style.window.scrollbar_size.x,0));
    if (dock_begin(a, PS_DOCK_LOG)) {
        nk_layout_row_begin(ui, NK_DYNAMIC, 28, 4);
        nk_layout_row_push(ui, .42f);
        nk_label_colored(ui, a->status, NK_TEXT_LEFT, muted());
        nk_layout_row_push(ui, .16f);
        if (a->job.running) {
            if (nk_button_label(ui, "Abbrechen")) {
                ps_process_kill(&a->job);
                protocol_job_cancel(a);
                ps_process_close(&a->job);
                status(a, "Hintergrundjob abgebrochen.");
            }
        } else
            nk_spacer(ui);
        nk_layout_row_push(ui, .18f);
        a->diagnostic_mode_bounds=nk_widget_bounds(ui);
        if(expanded_log && a->diagnostic_count) {
            if(nk_button_label(ui,a->diagnostic_raw?"Diagnosen":"Ausgabe"))a->diagnostic_raw=!a->diagnostic_raw;
        } else nk_spacer(ui);
        nk_layout_row_push(ui, .24f);
        if (nk_button_label(ui, expanded_log ? "Weniger" : "Protokoll")) {
            protocol_toggle(a,"Protokoll");
        }
        nk_layout_row_end(ui);
        if (expanded_log) {
            nk_layout_row_dynamic(ui, compact?90:footer - 66, 1);
            if (a->diagnostic_count && !a->diagnostic_raw) {
                /* In a short simulation log the panel itself scrolls. A
                 * clipped nested scroll group would trap its diagnostic rows. */
                if (compact || nk_group_begin(ui, "Compilerdiagnosen", 0)) {
                    nk_layout_row_dynamic(ui, 30, 1);
                    for (int i = 0; i < a->diagnostic_count; i++) {
                        a->diagnostic_bounds[i]=nk_widget_bounds(ui);
                        if (compact?compact_log_diagnostic_button(a,a->diagnostics[i].text):nk_button_label(ui, a->diagnostics[i].text))
                            jump_to_diagnostic(a, &a->diagnostics[i]);
                    }
                    if(!compact)nk_group_end(ui);
                }
            } else {
                nk_style_push_font(ui, a->font_code);
                nk_edit_string_zero_terminated(
                    ui, NK_EDIT_BOX | NK_EDIT_READ_ONLY | NK_EDIT_GOTO_END_ON_ACTIVATE, a->log,
                    sizeof a->log, nk_filter_default);
                nk_style_pop_font(ui);
            }
        }
    }
    nk_end(ui);
    if(compact)nk_style_pop_vec2(ui);
    nk_style_pop_vec2(ui);
}
static void workbench_status_view(app *a,float w,float h) {
    struct nk_context *ui=a->ui;bool visible=workbench_visible(a) || browser_workspace_visible(a);
    nk_window_show(ui,"workbench-status",visible?NK_SHOWN:NK_HIDDEN);if(!visible)return;
    nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(8,2));
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(8,0));
    nk_style_push_float(ui,&ui->style.window.rounding,0);
    nk_style_push_font(ui,&a->workspace_status_font);
    nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(UI_COLOR(header)));
    struct nk_window *previous=ui->active;
    if(nk_begin(ui,"workbench-status",nk_rect(0,h-WORKSPACE_STATUS_HEIGHT,w,WORKSPACE_STATUS_HEIGHT),NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BACKGROUND)) {
        bool blocked=a->toolbar_menu || a->toolbar_pointer_consumed;
        bool pointer=nk_input_is_mouse_hovering_rect(&ui->input,nk_rect(0,h-WORKSPACE_STATUS_HEIGHT,w,WORKSPACE_STATUS_HEIGHT));
        bool owns=!blocked && (pointer || previous==ui->current || ui->current->popup.active);
        ui->active=owns?ui->current:previous;
        if(owns){ui->current->flags&=~(nk_flags)NK_WINDOW_ROM;ui->current->layout->flags&=~(nk_flags)NK_WINDOW_ROM;}
        else {ui->current->flags|=NK_WINDOW_ROM;ui->current->layout->flags|=NK_WINDOW_ROM;}
        ps_ui_accessibility_route(ui,!blocked);
        /* The editor's own status belongs below its protocol, not in the app
         * footer. It spans exactly the current editor/log column. */
        float location_width=0;
        const struct nk_user_font *font=ui->style.font;
        float profile_label=fmaxf(32,font->width(font->userdata,font->height,"Profil",6)+2);
        float profile_width=fmaxf(70,font->width(font->userdata,font->height,"Release",7)+24);
        float build_width=fmaxf(130,font->width(font->userdata,font->height,"Build erforderlich",18)+4);
        float action_width=a->job.running?80:0;
        struct nk_command_buffer *canvas=nk_window_get_canvas(ui);workspace_rule(canvas,nk_rect(0,h-WORKSPACE_STATUS_HEIGHT,w,1));
        nk_layout_row_begin(ui,NK_STATIC,23,6);
        nk_layout_row_push(ui,profile_label);nk_label_colored(ui,"Profil",NK_TEXT_LEFT,muted());
        nk_layout_row_push(ui,profile_width);const char *profiles[]={"Debug","Release"};
        a->project_profile_bounds=nk_widget_bounds(ui);
        enum nk_symbol_type old_normal=ui->style.combo.sym_normal,old_hover=ui->style.combo.sym_hover,old_active=ui->style.combo.sym_active;
        ui->style.combo.sym_normal=ui->style.combo.sym_hover=ui->style.combo.sym_active=NK_SYMBOL_CHEVRON_DOWN;
        nk_style_push_style_item(ui,&ui->style.combo.normal,nk_style_item_color(UI_COLOR(header)));
        nk_style_push_vec2(ui,&ui->style.combo.button_padding,nk_vec2(6,6));
        nk_style_push_vec2(ui,&ui->style.combo.button.padding,nk_vec2(2,2));
        if(nk_combo_begin_label(ui,profiles[a->profile],nk_vec2(160,92))) {
            nk_layout_row_dynamic(ui,28,1);
            for(int p=0;p<2;p++){a->project_profile_choices[p]=nk_widget_bounds(ui);if(nk_combo_item_label(ui,profiles[p],NK_TEXT_LEFT))select_build_profile(a,p);}
            nk_combo_end(ui);
        }
        nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);nk_style_pop_style_item(ui);
        ui->style.combo.sym_normal=old_normal;ui->style.combo.sym_hover=old_hover;ui->style.combo.sym_active=old_active;
        nk_layout_row_push(ui,build_width);
        const char *build=a->job.running?"Build läuft …":a->built?"Build erfolgreich":"Build erforderlich";
        struct nk_color good=a->preferences.theme==PS_THEME_HIGH_CONTRAST?UI_COLOR(accent):a->preferences.theme==PS_THEME_LIGHT?nk_rgb(40,98,64):nk_rgb(139,204,165);
        nk_label_colored(ui,build,NK_TEXT_LEFT,a->built?good:muted());
        nk_layout_row_push(ui,fmaxf(40,w-profile_label-profile_width-build_width-action_width-location_width-56));
        struct nk_rect message=nk_widget_bounds(ui);nk_label_colored(ui,a->status,NK_TEXT_LEFT,muted());
        if(nk_input_is_mouse_hovering_rect(&ui->input,message))nk_tooltip(ui,a->status);
        nk_layout_row_push(ui,location_width);nk_spacer(ui);
        nk_layout_row_push(ui,action_width);
        if(a->job.running) {
            if(nk_button_label(ui,"Abbrechen")){ps_process_kill(&a->job);protocol_job_cancel(a);ps_process_close(&a->job);status(a,"Hintergrundjob abgebrochen.");}
        } else nk_spacer(ui);
        nk_layout_row_end(ui);
    }
    nk_end(ui);nk_style_pop_style_item(ui);nk_style_pop_font(ui);nk_style_pop_float(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
}
static void workbench_editor_status(app *a) {
    struct nk_context *ui=a->ui;struct nk_text_edit *edit=editor_search_target(a);
    bool visible=workbench_visible(a) && edit && a->dock_visible[PS_DOCK_WORKSPACE];
    nk_window_show(ui,"workbench-editor-status",visible?NK_SHOWN:NK_HIDDEN);if(!visible)return;
    struct nk_rect pane=a->dock_panels[PS_DOCK_WORKSPACE];
    if(a->dock_visible[PS_DOCK_LOG] && fabsf(pane.x-a->dock_panels[PS_DOCK_LOG].x)<2 &&
       !(a->preferences.dock.floating&((1u<<PS_DOCK_LOG)|(1u<<PS_DOCK_WORKSPACE))))
        pane.h=a->dock_panels[PS_DOCK_LOG].y+a->dock_panels[PS_DOCK_LOG].h-pane.y;
    struct nk_rect bar=nk_rect(pane.x,pane.y+pane.h-24,pane.w,24);
    nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(12,0));
    nk_style_push_float(ui,&ui->style.window.rounding,0);
    struct nk_window *previous=ui->active;
    if(nk_begin(ui,"workbench-editor-status",bar,NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BACKGROUND)) {
        ui->active=previous;ui->current->flags|=NK_WINDOW_ROM;ui->current->layout->flags|=NK_WINDOW_ROM;
        struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
        workspace_rule(canvas,nk_rect(bar.x,bar.y,bar.w,1));
        const char *text=nk_str_get_const(&edit->string);size_t cursor=editor_search_byte(edit,edit->cursor),start=0;unsigned line=1;
        for(size_t i=0;i<cursor;i++)if(text[i]=='\n'){line++;start=i+1;}
        unsigned column=text?(unsigned)nk_utf_len(text+start,(int)(cursor-start))+1:1;
        workspace_document *doc=active_document(a);
        const char *name=a->tab==8 && doc?workspace_name(doc->path):edit==&a->analysis?analysis_source(a):experiment_source(a);
        char location[512];snprintf(location,sizeof location,"%s    Zeile %u, Spalte %u    UTF-8",name,line,column);
        const struct nk_user_font *small=&a->workbench_fonts[1];float y=bar.y+(bar.h-small->height)/2;
        const char *saved=a->dirty || a->analysis_dirty || documents_dirty(a)?"Ungespeicherte Änderungen":"Alle Änderungen gespeichert";
        nk_draw_text(canvas,nk_rect(bar.x+12,y,fmaxf(1,bar.w*.40f-12),small->height),saved,(int)strlen(saved),small,nk_rgba(0,0,0,0),muted());
        float text_width=small->width(small->userdata,small->height,location,(int)strlen(location));
        nk_draw_text(canvas,nk_rect(fmaxf(bar.x+bar.w*.42f,bar.x+bar.w-text_width-12),y,fminf(bar.w*.58f-12,text_width),small->height),location,(int)strlen(location),small,nk_rgba(0,0,0,0),muted());
        ps_ui_accessibility_label(ui,bar,location);
    }
    nk_end(ui);nk_style_pop_float(ui);nk_style_pop_vec2(ui);
}
static void workspace_draw_ui(app *a, int width, int height) {
    a->laboratory_scene_host_width=width;
    struct nk_context *ui = a->ui;
    /* Modal views also cross workspace boundaries (for example Settings with
     * the layout manager open); finish the session transition before returning. */
    workspace_context(a);
    if(a->panel_pull && (a->recovery || a->layout_manager || a->channel_unit_manager || a->workspace_manager || a->run_details_manager))panel_pull_cancel(a);
    ps_graphics_backdrop(a->graphics,NULL);
    ps_graphics_shadows(a->graphics,NULL,0);
    if(a->run_details_manager) {run_details_view(a,width,height);return;}
    if(a->channel_unit_manager) { channel_unit_manager_view(a,width,height);return; }
    if(a->layout_manager) { layout_manager_view(a,width,height);return; }
    if(a->workspace_manager) { workspace_catalog_view(a,width,height);return; }
    if (a->recovery) {
        nk_style_push_vec2(ui, &ui->style.window.padding, nk_vec2(0, 5));
        nk_style_push_vec2(ui, &ui->style.window.spacing, nk_vec2(0, 0));
        nk_style_push_float(ui, &ui->style.window.rounding, 0);
        if (nk_begin(ui, "recovery-header", nk_rect(0, 0, (float)width, 38), NK_WINDOW_NO_SCROLLBAR)) {
            nk_layout_row_begin(ui, NK_STATIC, 28, 4);
            nk_layout_row_push(ui, (float)width - 138);
            a->window_drag_bounds = nk_widget_bounds(ui);
            nk_label(ui, "   Physim  ·  Wiederherstellung", NK_TEXT_LEFT);
            for (unsigned i = 0; i < 3; i++) {
                nk_layout_row_push(ui, 46);
                window_control(a, i);
            }
            nk_layout_row_end(ui);
        }
        nk_end(ui);
        nk_style_pop_float(ui);
        nk_style_pop_vec2(ui);
        nk_style_pop_vec2(ui);
        float panel_width = 640, panel_height = 420;
        if (nk_begin(ui, "recovery",
                     nk_rect(((float)width - panel_width) / 2, ((float)height - panel_height) / 2,
                             panel_width, panel_height),
                     0)) {
            nk_style_push_font(ui, a->font_title);
            nk_layout_row_dynamic(ui, 40, 1);
            nk_label(ui, "Deine Arbeit ist noch da.", NK_TEXT_LEFT);
            nk_style_pop_font(ui);
            nk_layout_row_dynamic(ui, 28, 1);
            nk_label_colored(ui, project_name(a), NK_TEXT_LEFT, muted());
            nk_layout_row_dynamic(ui, 64, 1);
            nk_label_wrap(ui, "Für Experiment- und Analysecode gibt es eine automatische Sicherung. "
                              "Stelle die Änderungen im Editor wieder her oder verwende die "
                              "gespeicherten Quelldateien und verwirf die Sicherung.");
            nk_layout_row_dynamic(ui, 60, 1);
            nk_label_wrap(
                ui,
                a->recovery_conflict
                    ? "Die Quelldateien wurden seit dieser Sicherung geändert. Wiederherstellen "
                      "lädt die gesicherten Texte; es führt die Änderungen nicht zusammen."
                    : "Beim Wiederherstellen bleiben die Quelldateien zunächst unverändert. "
                      "Du kannst beide Texte prüfen und anschließend speichern.");
            nk_layout_row_dynamic(ui, 40, 1);
            a->recovery_bounds[0] = nk_widget_bounds(ui);
            if (action_button(ui, "Änderungen wiederherstellen", true))
                resolve_recovery(a, true);
            nk_layout_row_dynamic(ui, 40, 1);
            a->recovery_bounds[1] = nk_widget_bounds(ui);
            if (nk_button_label(ui, "Gespeicherte Dateien verwenden"))
                resolve_recovery(a, false);
            nk_layout_row_dynamic(ui, 44, 1);
            nk_label_wrap(ui, a->status);
        }
        nk_end(ui);
        return;
    }
    const ps_ui_palette *previous_palette=ui_palette;
    const struct nk_user_font *previous_font=ui->style.font;
    bool laboratory=laboratory_visible(a);
    bool workspace_shell=workspace_shell_visible(a);
    if(a->outline_keyboard && (editor_search_target(a)!=a->outline_editor || a->toolbar_menu || a->search_keyboard ||
       nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT)))a->outline_keyboard=false;
    if(a->file_tab_keyboard_edit && (editor_search_target(a)!=a->file_tab_keyboard_edit || a->toolbar_menu || a->toolbar_keyboard ||
       a->search_keyboard || a->search_focus || (a->preferences.dock.hidden&(1u<<PS_DOCK_WORKSPACE)) ||
       nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT) ||
       (nk_sdl_accessibility_has_focus(ui) && !nk_sdl_accessibility_keyboard_owned(ui))))a->file_tab_keyboard_edit=NULL;
    const float sizes[]={11,9,12,11,23,16,10,10,8,10,12,11};
    for(unsigned i=0;i<SDL_arraysize(a->workbench_fonts);i++) {
        bool mono=i==7 || i==10 || i==11,bold=(i>=3 && i<=6) || i==8;
        a->workbench_fonts[i]=*(mono?a->font_code:bold?a->font_title:a->font_ui);
        float em=mono?a->workbench_code_em:bold?a->workbench_bold_em:a->workbench_regular_em;
        a->workbench_fonts[i].height=sizes[i]*em*a->preferences.ui_size/16.f;
    }
    a->workbench_marker_font=*a->workbench_marker_source;
    a->workbench_marker_font.height=12*a->workbench_marker_em*a->preferences.ui_size/16.f;
    a->workbench_tab_dot_font=*a->font_ui;
    a->workbench_tab_dot_font.height=8*a->workbench_regular_em*a->preferences.ui_size/16.f;
    if(workspace_shell && !laboratory) {
        ui_palette=ps_workbench_palette(ui_palette);style(ui);
        nk_style_set_font(ui,&a->workbench_fonts[0]);
    }
    if(laboratory){ui_palette=ps_laboratory_palette(ui_palette);style(ui);nk_style_set_font(ui,&a->workbench_fonts[2]);
        ui->style.combo.sym_normal=ui->style.combo.sym_hover=ui->style.combo.sym_active=NK_SYMBOL_CHEVRON_DOWN;
        ui->style.combo.button_padding=nk_vec2(6,5);
        ui->style.tab.sym_minimize=NK_SYMBOL_PLUS;ui->style.tab.sym_maximize=NK_SYMBOL_MINUS;
        ui->style.tab.tab_minimize_button.normal=ui->style.tab.tab_maximize_button.normal=nk_style_item_color(UI_COLOR(window));
        ui->style.tab.padding=nk_vec2(0,12);
    }
    a->laboratory_value_font=*a->ui_fonts[3];a->laboratory_value_font.height=21*a->workbench_regular_em*a->preferences.ui_size/16.f;
    a->laboratory_title_font=*a->font_title;a->laboratory_title_font.height=14*a->workbench_bold_em*a->preferences.ui_size/16.f;
    int metric_width=a->laboratory_scene_host_width;
    float metric_size=metric_width<=650?22:metric_width<=850?18:metric_width<=1150?21:23;
    a->laboratory_measurement_font=*a->font_ui;a->laboratory_measurement_font.height=metric_size*a->workbench_regular_em*a->preferences.ui_size/16.f;
    a->laboratory_timeline_font=*a->font_ui;a->laboratory_timeline_font.height=18*a->workbench_regular_em*a->preferences.ui_size/16.f;
    a->laboratory_section_font=*a->font_title;a->laboratory_section_font.height=13*a->workbench_bold_em*a->preferences.ui_size/16.f;
    a->laboratory_caption_font=*a->font_ui;a->laboratory_caption_font.height=11*a->workbench_regular_em*a->preferences.ui_size/16.f;
    a->laboratory_model_font=*a->font_ui;a->laboratory_model_font.height=19*a->workbench_regular_em*a->preferences.ui_size/16.f;
    a->laboratory_control_font=*a->font_ui;a->laboratory_control_font.height=13*a->workbench_regular_em*a->preferences.ui_size/16.f;
    a->laboratory_coordinate_font=*a->font_ui;a->laboratory_coordinate_font.height=14*a->workbench_regular_em*a->preferences.ui_size/16.f;
    a->laboratory_inspector_value_font=*a->font_ui;a->laboratory_inspector_value_font.height=24*a->workbench_regular_em*a->preferences.ui_size/16.f;
    a->laboratory_scene_font=*a->font_ui;a->laboratory_scene_font.height=9*a->workbench_regular_em*a->preferences.ui_size/16.f;
    a->workspace_status_font=*a->font_ui;a->workspace_status_font.height=10*a->workbench_regular_em*a->preferences.ui_size/16.f;
    a->analysis_title_font=*a->font_title;a->analysis_title_font.height=18*a->workbench_bold_em*a->preferences.ui_size/16.f;
    a->analysis_chart_font=*a->font_title;a->analysis_chart_font.height=20*a->workbench_bold_em*a->preferences.ui_size/16.f;
    a->laboratory_card_font_count=0;
    float w=(float)width,h=(float)height;
    a->search_edit=NULL;
    a->toolbar_pointer_consumed=false;
    bool files=browser_workspace_visible(a);
    if(!files)dock_prepare(a,w,h);
    else {
        protocol_resize_input(a,w,h);
        memset(a->dock_visible,0,sizeof a->dock_visible);memset(a->dock_splitters,0,sizeof a->dock_splitters);
        if(protocol_visible(a)){a->dock_panels[PS_DOCK_LOG]=protocol_bounds(a,w,h);a->dock_visible[PS_DOCK_LOG]=true;}
    }
    dock_order(a);
    nk_style_push_vec2(ui, &ui->style.window.padding, nk_vec2(0, 0));
    nk_style_push_vec2(ui, &ui->style.window.spacing, nk_vec2(0, 0));
    nk_style_push_float(ui, &ui->style.window.rounding, 0);
    if (nk_begin(ui, "header", nk_rect(0, 0, w, workspace_shell?WORKSPACE_HEADER_HEIGHT:64), NK_WINDOW_NO_SCROLLBAR)) {
        toolbar_view(a);
        dock_overlay(a,w,h);
    }
    nk_end(ui);
    nk_style_pop_float(ui);
    nk_style_pop_vec2(ui);
    nk_style_pop_vec2(ui);
    if(a->tab!=a->dock_last_tab) { if(!files)dock_show(a,PS_DOCK_WORKSPACE);a->dock_last_tab=a->tab; }
    workspace_path_view(a,w);
    nk_window_show(ui,"Dateien",files?NK_SHOWN:NK_HIDDEN);
    if(files){
        nk_window_show(ui,"sidebar",NK_HIDDEN);nk_window_show(ui,"workspace",NK_HIDDEN);nk_window_show(ui,"inspector",NK_HIDDEN);
        nk_window_show(ui,"workbench-editor-status",NK_HIDDEN);
        float bottom=protocol_visible(a)?a->dock_panels[PS_DOCK_LOG].y:h-WORKSPACE_STATUS_HEIGHT;
        browser_view(a,a->browser_workspace,width,height,nk_rect(0,WORKSPACE_CONTENT_TOP,w,fmaxf(1,bottom-WORKSPACE_CONTENT_TOP)));
        log_panel(a);protocol_separator_view(a);workbench_status_view(a,w,h);
        ui_palette=previous_palette;style(ui);nk_style_set_font(ui,previous_font);return;
    }
    workspace_context(a);
    laboratory_chrome(a,w,h);
    analysis_chrome(a,w,h);
    dock_relayout(a,w,h);
    memset(a->laboratory_plots,0,sizeof a->laboratory_plots);
    a->laboratory_splitters[0]=a->laboratory_splitters[2]=nk_rect(0,0,0,0);
    for(unsigned floating=0;floating<2;floating++) {
        for(unsigned panel=0;panel<PS_DOCK_PANELS;panel++) {
            if (((a->preferences.dock.floating >> panel)&1u)!=floating) continue;
            if(panel==PS_DOCK_SIDEBAR) sidebar_panel(a);
            else if(panel==PS_DOCK_WORKSPACE) workspace_panel(a);
            else if(panel==PS_DOCK_LOG) log_panel(a);
            else inspector_panel(a);
        }
    }
    dock_headers(a);
    workbench_editor_status(a);
    workbench_status_view(a,w,h);
    if(a->tab==4 && a->library_search_active && a->dock_visible[PS_DOCK_WORKSPACE])
        nk_window_set_focus(ui,"workspace");
    dock_order(a);
    dock_separator_views(a);
    laboratory_separators(a);
    protocol_separator_view(a);
    protocol_return_focus(a);
    if(!panel_is_detached(a,PS_DOCK_WORKSPACE)){editor_search_view(a);completion_view(a,width,height);}
    /* Docking temporarily routes hover/wheel to the panel under the pointer.
     * A declaration activation returns keyboard ownership to its source until
     * the user explicitly clicks, tabs, opens a menu or enters another field. */
    if(a->outline_keyboard || a->file_tab_keyboard_edit)ui->active=nk_window_find(ui,"workspace");
    if(workspace_shell){ui_palette=previous_palette;style(ui);}
    if(workspace_shell)nk_style_set_font(ui,previous_font);
}

/* Each native task window publishes and consumes its own controls. */
static void draw_ui(app *a,int width,int height) {
    workspace_tip_frame_begin(a);
    workspace_draw_ui(a,width,height);
    workspace_tip_frame_end(a);
}
