/* Inspect the actual text commands: every wrapped glyph must have enough
 * drawing width, including the last glyph returned by Nuklear's clamp. */
static bool test_ui_wrapped_button(app *a) {
    struct nk_context ctx;
    if(!nk_init_default(&ctx,a->font_ui))return false;
    ps_ui_font_layout layout={.magic=PS_UI_LAYOUT_MAGIC,.ui_size=a->preferences.ui_size};
    ctx.userdata=nk_handle_ptr(&layout);
    const char *label="Messdaten im Ordner öffnen";
    if(nk_begin(&ctx,"wrap-proof",nk_rect(0,0,200,300),0)) {
        nk_layout_row_dynamic(&ctx,28,1);
        nk_button_label(&ctx,label);
    }
    nk_end(&ctx);
    const struct nk_command *command;
    size_t seen=0;unsigned lines=0;bool ok=true;
    nk_foreach(command,&ctx)if(command->type==NK_COMMAND_TEXT) {
        const struct nk_command_text *text=(const struct nk_command_text *)command;
        float width=text->font->width(text->font->userdata,text->height,text->string,text->length);
        ok=ok && width<=text->w && seen+(size_t)text->length<=strlen(label);
        if(ok)ok=!memcmp(label+seen,text->string,(size_t)text->length);
        seen+=(size_t)text->length;lines++;
    }
    ok=ok && seen==strlen(label) && lines>=2;
    nk_free(&ctx);return ok;
}
static void test_ui_size_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    unsigned target=(unsigned)atoi(mode+8);
    bool reading=strstr(mode,"-read")!=NULL,cancel=strstr(mode,"-cancel")!=NULL;
    bool defaults=strstr(mode,"-defaults")!=NULL;
    static unsigned previous;
    bool ok=target>=16 && target<=22 && target%2==0;
    switch((*stage)++) {
    case 0:
        previous=a->preferences.ui_size;
        SDL_SetWindowSize(a->window,1080,740);
        open_documentation(a,8);
        ok=ok && a->doc_visible && !a->doc_error[0];
        ok=ok && ps_report_create("Schriftgröße","UI-Diagramm",&a->analysis_report)==PS_OK;
        if(ok) {
            ps_plot_info info={0};strcpy(info.title,"Position über Zeit");
            strcpy(info.x_label,"Zeit");strcpy(info.y_label,"Position");
            ps_report_unit_from(PS_SECOND,&info.x_unit);ps_report_unit_from(PS_METRE,&info.y_unit);
            ps_plot_handle plot;ok=ps_report_add_plot(a->analysis_report,&info,&plot)==PS_OK;
            ps_curve_data *curve=calloc(1,sizeof *curve);ok=ok && curve!=NULL;
            if(ok){strcpy(curve->label,"Messung");curve->kind=PS_PLOT_LINE;curve->count=curve->source_count=4;
                for(unsigned i=0;i<4;i++){curve->x[i]=i;curve->y[i]=i*i;}
                ok=ps_report_add_curve(a->analysis_report,plot,curve)==PS_OK;}
            free(curve);
        }
        if(reading)ok=ok && previous==target;
        open_settings(a);break;
    case 2:
        test_mouse(a,defaults?a->settings_bounds[5]:a->ui_size_bounds[(target-16)/2],true);break;
    case 3: test_mouse(a,defaults?a->settings_bounds[5]:a->ui_size_bounds[(target-16)/2],false);break;
    case 5:
        ok=ok && a->settings_draft.ui_size==target && a->preferences.ui_size==previous;
        test_mouse(a,a->settings_bounds[cancel || reading?4:0],true);break;
    case 6: test_mouse(a,a->settings_bounds[cancel || reading?4:0],false);break;
    case 8: {
        unsigned expected=cancel?previous:target;
        ok=ok && a->preferences.ui_size==expected && a->font_ui->height==expected &&
           a->doc_font_ui->height==expected && a->font_title->height==expected+7 &&
           a->doc_font_title->height==expected+7 && a->ui->style.font==a->font_ui &&
           a->doc_ui->style.font==a->doc_font_ui && a->font_code->height==a->preferences.editor_size;
        if(expected>16)ok=ok && nk_sdl_row_height(a->ui,18)>=expected+4 &&
                                  nk_sdl_row_height(a->doc_ui,18)>=expected+4 &&
                                  test_ui_wrapped_button(a);
        a->toolbar_menu=1;*capture="ui-size-menu.bmp";break;
    }
    case 9: *capture="docs-ui-size.bmp";break;
    case 10:
        a->toolbar_menu=0;open_settings(a);break;
    case 11: *capture="ui-size-settings.bmp";break;
    case 12: a->tab=0;break;
    case 13: *capture="ui-size-editor.bmp";break;
    case 14:
        ok=ok && a->preferences.editor_size==16;
        a->tab=2;a->show_report=true;break;
    case 15: *capture="ui-size-plot.bmp";break;
    case 16:
        test_window_key_mod(a->window,SDLK_F10,0);
        test_window_key_mod(a->window,SDLK_DOWN,0);break;
    case 17:
        ok=ok && a->toolbar_keyboard && a->toolbar_menu==1;
        test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    case 18:
        ok=ok && !a->toolbar_keyboard && !a->toolbar_menu;
        a->quitting=true;printf("UI SIZE SELF-TEST: %s\n",*code?"FAILED":"PASSED");break;
    }
    if(!ok){fprintf(stderr,"UI size failure stage %d: target=%u current=%u draft=%u\n",*stage-1,target,a->preferences.ui_size,a->settings_draft.ui_size);*code=1;a->quitting=true;}
}
