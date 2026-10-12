#include "../src/text_validation.h"
static void run_details_select(app *a,int tab) {
    a->run_details_tab=tab;
    const char *text=tab?a->run_details_metadata:a->run_details_overview;
    size_t used=0;
    for(size_t i=0;text[i];i++) {
        if(text[i]=='\r' && text[i+1]=='\n')continue;
        a->run_details_display[used++]=text[i]=='\r'?'\n':text[i];
    }
    a->run_details_display[used]=0;a->run_details_active=false;
}
static bool run_details_open(app *a) {
    if(a->data.thread || !a->data.channel_count || !a->data.path[0])return false;
    snprintf(a->run_details_path,sizeof a->run_details_path,"%s",a->data.path);
    snprintf(a->run_details_metadata,sizeof a->run_details_metadata,"%s",a->data.metadata);
    ps_result result=ps_run_details_format(a->run_details_metadata,strlen(a->run_details_metadata),
                                         a->run_details_overview,sizeof a->run_details_overview);
    if(result!=PS_OK) {
        snprintf(a->run_details_overview,sizeof a->run_details_overview,
                 "Modellangaben konnten nicht dargestellt werden (%s).",ps_result_string(result));
        if(!ps_text_valid(a->run_details_metadata,sizeof a->run_details_metadata,true))a->run_details_metadata[0]=0;
    }
    run_details_select(a,0);a->run_details_manager=true;return true;
}
static void run_details_view(app *a,int width,int height) {
    struct nk_context *ui=a->ui;
    nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(0,5));
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,0));
    if(nk_begin(ui,"run-details-header",nk_rect(0,0,(float)width,38),NK_WINDOW_NO_SCROLLBAR)) {
        nk_layout_row_begin(ui,NK_STATIC,28,4);nk_layout_row_push(ui,fmaxf(1,(float)width-138));
        a->window_drag_bounds=nk_widget_bounds(ui);nk_label(ui,"Physim · Laufdetails",NK_TEXT_LEFT);
        for(unsigned i=0;i<3;i++){nk_layout_row_push(ui,46);window_control(a,i);}nk_layout_row_end(ui);
    }nk_end(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
    float w=fminf(960,(float)width-24),h=fmaxf(180,(float)height-54);
    if(nk_begin(ui,"run-details",nk_rect(((float)width-w)/2,44,w,h),NK_WINDOW_NO_SCROLLBAR)) {
        nk_layout_row_dynamic(ui,32,1);nk_style_push_font(ui,a->font_title);
        nk_label(ui,"Modell und Lauf",NK_TEXT_LEFT);nk_style_pop_font(ui);
        nk_layout_row_dynamic(ui,28,1);struct nk_rect path=nk_widget_bounds(ui);
        nk_label(ui,diagnostic_basename(a->run_details_path),NK_TEXT_LEFT);
        if(nk_input_is_mouse_hovering_rect(&ui->input,path))nk_tooltip(ui,a->run_details_path);
        nk_layout_row_dynamic(ui,34,2);const char *tabs[]={"Übersicht","Alle Metadaten"};
        for(int i=0;i<2;i++){a->run_details_tabs[i]=nk_widget_bounds(ui);if(action_button(ui,tabs[i],a->run_details_tab==i))run_details_select(a,i);}
        nk_layout_row_dynamic(ui,34,2);a->run_details_copy_bounds=nk_widget_bounds(ui);
        if(nk_button_label(ui,"Text kopieren")) {
            const char *text=a->run_details_tab?a->run_details_metadata:a->run_details_overview;
            status(a,SDL_SetClipboardText(text)?"Laufdetails kopiert.":"Text konnte nicht kopiert werden.");
        }
        a->run_details_close_bounds=nk_widget_bounds(ui);
        if(nk_button_label(ui,"Schließen · Esc"))a->run_details_manager=false;
        nk_layout_row_dynamic(ui,fmaxf(70,h-190),1);a->run_details_text_bounds=nk_widget_bounds(ui);
        nk_style_push_font(ui,a->font_code);
        nk_flags flags=nk_edit_string_zero_terminated(ui,NK_EDIT_BOX|NK_EDIT_READ_ONLY,a->run_details_display,
                                                     (int)strlen(a->run_details_display)+1,nk_filter_default);
        a->run_details_active=(flags&NK_EDIT_ACTIVE)!=0;nk_style_pop_font(ui);
    }nk_end(ui);
}
