typedef struct {
    float document_label,title,intro,outline,card,model_label,note,build_label,environment[3],total;
} workbench_context_plan;
static const char *workbench_context_intro="Deklarationen im aktiven Dokument. Auswahl springt zur Quelle.";
static const char *workbench_context_note="Für dieses Dokument ist keine Modellnotiz hinterlegt.";
static const char *workbench_context_value(app *a,unsigned row,char version[40]) {
    if(row==0)return a->build_info_valid?a->build_info.compiler:"Noch kein Build";
    if(row==1){snprintf(version,40,"Version %u",PS_API_VERSION);return version;}
    struct nk_text_edit *edit=editor_search_target(a);
    return edit==&a->analysis?"Analysemodul":edit==&a->experiment?"Experimentmodul":"Textdokument";
}
static workbench_context_plan workbench_context_measure(app *a,float width) {
    workbench_context_plan p={0};float scale=a->preferences.ui_size/16.f;width=fmaxf(1,floorf(width));
    const struct nk_user_font *section=&a->workbench_fonts[6],*body=&a->workbench_fonts[9],*small=&a->workbench_fonts[1];
    p.document_label=workbench_text_height(section,"IM DOKUMENT",11,width,15*scale,1.6f*scale);
    const char *title=a->loaded?project_name(a):"Textdokument";
    p.title=workbench_text_height(&a->workbench_fonts[5],title,strlen(title),width,24*scale,0);
    p.intro=workbench_text_height(body,workbench_context_intro,strlen(workbench_context_intro),width,15*scale,0);
    p.outline=workbench_outline_height(a);
    p.card=a->api_reference?workbench_api_card_height(a,width):workbench_api_empty_height(a,width);
    p.model_label=workbench_text_height(section,"MODELLNOTIZ",11,width,15*scale,1.6f*scale);
    p.note=workbench_text_height(body,workbench_context_note,strlen(workbench_context_note),width,18*scale,0);
    p.build_label=workbench_text_height(section,"BUILDUMGEBUNG",13,width,15*scale,1.6f*scale);
    p.total=4+p.document_label+7*scale+p.title+6*scale+p.intro+16*scale+p.outline+22*scale+p.card+
        22*scale+p.model_label+8*scale+p.note+22*scale+p.build_label+8*scale+18;
    const char *labels[]={"Compiler","API","Ziel"};
    float left=fminf(width*.38f,ceilf(small->width(small->userdata,small->height,"Compiler",8))+1);
    float right=fmaxf(1,floorf(width-left-10*scale));
    for(unsigned row=0;row<3;row++) {
        char version[40];const char *value=workbench_context_value(a,row,version);
        p.environment[row]=fmaxf(workbench_text_height(small,labels[row],strlen(labels[row]),left,13.5f*scale,0),
            workbench_text_height(small,value,strlen(value),right,13.5f*scale,0));
        p.total+=6*scale+p.environment[row];
    }
    return p;
}
static bool workbench_context_needs_scrollbar(app *a,struct nk_rect bounds) {
    /* Decide using the full content width, before Nuklear reserves scrollbar
     * space. A narrower scrollable layout cannot turn overflow into a fit. */
    workbench_context_plan p=workbench_context_measure(a,bounds.w-32);
    a->workbench_context_scrollbar=p.total>bounds.h;
    return a->workbench_context_scrollbar;
}
static void workbench_context_view(app *a) {
    struct nk_context *ui=a->ui;float scale=a->preferences.ui_size/16.f,width=floorf(nk_window_get_content_region(ui).w);
    workbench_context_plan p=workbench_context_measure(a,width);
    a->workbench_context_expected_height=p.total;
    float start=ui->current->layout->at_y;
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,0));
    const struct nk_user_font *section=&a->workbench_fonts[6],*body=&a->workbench_fonts[9],*small=&a->workbench_fonts[1];
    workbench_text_row(a,section,"IM DOKUMENT",15*scale,1.6f*scale,muted());
    workbench_api_gap(ui,7*scale);
    workbench_text_row(a,&a->workbench_fonts[5],a->loaded?project_name(a):"Textdokument",24*scale,0,UI_COLOR(text));
    workbench_api_gap(ui,6*scale);
    workbench_text_row(a,body,workbench_context_intro,15*scale,0,muted());
    workbench_api_gap(ui,16*scale);
    struct nk_text_edit *edit=editor_search_target(a);
    workbench_outline_view(a,edit,edit?document_language(a,edit):-1);
    workbench_api_gap(ui,22*scale);
    if(a->api_reference)workbench_api_card(a,p.card);else workbench_api_empty(a,p.card);
    workbench_api_gap(ui,22*scale);
    workbench_text_row(a,section,"MODELLNOTIZ",15*scale,1.6f*scale,muted());
    workbench_api_gap(ui,8*scale);
    workbench_text_row(a,body,workbench_context_note,18*scale,0,muted());
    workbench_api_gap(ui,22*scale);
    struct nk_rect environment=workbench_text_row(a,section,"BUILDUMGEBUNG",15*scale,1.6f*scale,muted());
    workbench_api_gap(ui,8*scale);
    const char *labels[]={"Compiler","API","Ziel"};
    float left=fminf(width*.38f,ceilf(small->width(small->userdata,small->height,"Compiler",8))+1);
    float right=fmaxf(1,floorf(width-left-10*scale));
    struct nk_rect last=environment;
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(10*scale,0));
    for(unsigned row=0;row<3;row++) {
        workbench_api_gap(ui,6*scale);ps_ui_row_exact_begin(ui,NK_STATIC,p.environment[row],2);
        nk_layout_row_push(ui,left);struct nk_rect label;nk_widget(&label,ui);
        workbench_text_draw(a,small,label,labels[row],strlen(labels[row]),13.5f*scale,0,muted());
        nk_layout_row_push(ui,right);nk_widget(&last,ui);
        char version[40];const char *value=workbench_context_value(a,row,version);size_t bytes=strlen(value),at=0;
        float pitch=fmaxf(13.5f*scale,small->height),y=last.y+(pitch-small->height)/2;
        while(at<bytes) {
            size_t length=0,used=documentation_line(small,value+at,bytes-at,last.w,&length);if(!used)break;
            float text_width=small->width(small->userdata,small->height,value+at,(int)length);
            nk_draw_text(nk_window_get_canvas(ui),nk_rect(last.x+fmaxf(0,last.w-text_width),y,last.w,small->height),value+at,(int)length,small,nk_rgba(0,0,0,0),UI_COLOR(text));
            at+=used;y+=pitch;
        }
        char reading[512];snprintf(reading,sizeof reading,"%s: %s",labels[row],value);ps_ui_accessibility_label(ui,last,reading);
        nk_layout_row_end(ui);
    }
    nk_style_pop_vec2(ui);
    a->workbench_environment_bounds=nk_rect(environment.x,environment.y,environment.w,last.y+last.h-environment.y);
    workbench_api_gap(ui,18);
    a->workbench_context_measured_height=ui->current->layout->at_y+ui->current->layout->row.height-start;
    nk_style_pop_vec2(ui);
}
static bool workbench_context_key(app *a,struct nk_context *ui,const SDL_KeyboardEvent *key,bool detached) {
    if(!workbench_visible(a) || (key->key!=SDLK_PAGEUP && key->key!=SDLK_PAGEDOWN) ||
       (key->mod&(SDL_KMOD_CTRL|SDL_KMOD_GUI|SDL_KMOD_ALT|SDL_KMOD_SHIFT)) || a->toolbar_menu || a->toolbar_keyboard ||
       a->recovery || a->layout_manager || a->workspace_manager || a->channel_unit_manager || a->run_details_manager)return false;
    if(!detached && (panel_is_detached(a,PS_DOCK_INSPECTOR) ||
       (!nk_sdl_accessibility_is_focused(ui,NULL,"Codekontext ein-/ausblenden") && !nk_sdl_accessibility_focus_in(ui,a->dock_panels[PS_DOCK_INSPECTOR]))))return false;
    struct nk_window *owner=nk_window_find(ui,dock_windows[PS_DOCK_INSPECTOR]);if(!owner)return false;
    float maximum=fmaxf(0,ceilf(a->workbench_context_measured_height-owner->bounds.h));
    float step=fmaxf(1,owner->bounds.h-32),offset=(float)owner->scrollbar.y+(key->key==SDLK_PAGEDOWN?step:-step);
    owner->scrollbar.y=(nk_uint)fmaxf(0,fminf(maximum,offset));
    if(!detached)nk_sdl_accessibility_request_focus(ui,NULL,"Codekontext ein-/ausblenden");
    return true;
}
