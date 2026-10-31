#ifdef __APPLE__
static bool native_test_shell(app *a);
static bool native_test_focus_wait(app *a,SDL_Window *window,int *stage,double deadline);
static bool native_test_track_menu(app *a,toolbar_action action,const char *text,unsigned short raw,bool cancel);
static bool ui_size_native_cancel_menu(app *a,int *stage,bool *okay) {
    static bool waiting;static double deadline;
    if(!waiting){waiting=true;deadline=ps_clock()+5;SDL_RaiseWindow(a->window);}
    if(!native_test_focus_wait(a,a->window,stage,deadline))return false;
    *okay &= SDL_GetKeyboardFocus()==a->window && native_test_shell(a);
    if(*okay) {
        double started=ps_clock();
        bool cancelled=native_test_track_menu(a,TOOLBAR_OPEN,NULL,0,true);
        double elapsed=ps_clock()-started;
        /* The helper has a 2 s emergency cancellation. That watchdog is not
         * evidence that the real queued arrow/Escape input was accepted. */
        *okay=cancelled && elapsed<1.5;
        printf("UI SIZE NATIVE MENU: cancelled=%d elapsed=%.3f watchdog-accepted=0 capture=SDL-shell-only\n",*okay,elapsed);
    }
    waiting=false;return true;
}
#endif
/* Inspect the actual text commands: every wrapped glyph must have enough
 * drawing width, including the last glyph returned by Nuklear's clamp. */
static bool test_ui_wrapped_button(app *a) {
    struct nk_context ctx;
    if(!nk_init_default(&ctx,a->font_ui))return false;
    ps_ui_font_layout layout={.magic=PS_UI_LAYOUT_MAGIC,.ui_size=a->preferences.ui_size};
    ctx.userdata=nk_handle_ptr(&layout);
    const char *label="Messdaten im Ordner öffnen";
    /* The label fits at 200 px with Segoe UI 18. Force overflow using the
     * actual font measurement, so this tests wrapping on every platform. */
    float label_width=a->font_ui->width(a->font_ui->userdata,a->font_ui->height,label,(int)strlen(label));
    if(nk_begin(&ctx,"wrap-proof",nk_rect(0,0,floorf(label_width*.75f),300),0)) {
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
    if(!ok)fprintf(stderr,"Wrapped button: size=%u seen=%zu/%zu lines=%u label-width=%.1f\n",a->preferences.ui_size,seen,strlen(label),lines,a->font_ui->width(a->font_ui->userdata,a->font_ui->height,label,(int)strlen(label)));
    nk_free(&ctx);return ok;
}
static void test_ui_size_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    unsigned target=(unsigned)atoi(mode+8);
    bool reading=strstr(mode,"-read")!=NULL,cancel=strstr(mode,"-cancel")!=NULL;
    bool defaults=strstr(mode,"-defaults")!=NULL;
    static unsigned previous,previous_editor;
    bool ok=target>=16 && target<=22 && target%2==0;
    switch((*stage)++) {
    case 0:
        previous=a->preferences.ui_size;
        previous_editor=a->preferences.editor_size;
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
        float body=documentation_type_font(a,DOC_TYPE_BODY)->height/a->doc_type_em[DOC_TYPE_BODY];
        float heading=documentation_type_font(a,DOC_TYPE_TITLE)->height/a->doc_type_em[DOC_TYPE_TITLE];
        float chrome=documentation_type_font(a,DOC_TYPE_CHROME)->height/a->doc_type_em[DOC_TYPE_CHROME];
        float control=documentation_type_font(a,DOC_TYPE_CONTROL)->height/a->doc_type_em[DOC_TYPE_CONTROL];
        /* H uses its CSS reading/title roles, not the main window's +7 px title.
         * Without a user reading override, F1 follows the selected UI size. */
        bool h_fonts=documentation_font_size(a)==expected && fabsf(body-expected)<.01f &&
            fabsf(heading-expected*(expected==32?1.3f:1.65f))<.01f &&
            fabsf(chrome-11*expected/16.f)<.01f && fabsf(control-12*expected/16.f)<.01f;
        printf("UI SIZE FONT ROLES: ui=%u reader=%u body-em=%.3f title-em=%.3f chrome-em=%.3f control-em=%.3f editor=%u complete=%d\n",
            expected,documentation_font_size(a),body,heading,chrome,control,a->preferences.editor_size,h_fonts);
        if(a->font_ui->height!=expected || a->doc_font_ui->height!=expected || a->font_title->height!=expected+7 || !h_fonts)
            fprintf(stderr,"UI fonts expected=%u ui=%.1f doc=%.1f title=%.1f H-title-em=%.3f\n",expected,a->font_ui->height,a->doc_font_ui->height,a->font_title->height,heading);
        ok=ok && a->preferences.ui_size==expected && a->font_ui->height==expected &&
           a->doc_font_ui->height==expected && a->font_title->height==expected+7 &&
           h_fonts && a->ui->style.font==a->font_ui &&
           a->doc_ui->style.font==a->doc_font_ui && a->font_code->height==a->preferences.editor_size;
        if(expected>16)ok=ok && nk_sdl_row_height(a->ui,18)>=expected+4 &&
                                  nk_sdl_row_height(a->doc_ui,18)>=expected+4 &&
                                  test_ui_wrapped_button(a);
#ifdef __APPLE__
        ok=ok && native_test_shell(a);
#else
        a->toolbar_menu=1;
#endif
        *capture="ui-size-menu.bmp";break;
    }
    case 9: *capture="docs-ui-size.bmp";break;
    case 10:
        a->toolbar_menu=0;open_settings(a);break;
    case 11: *capture="ui-size-settings.bmp";break;
    case 12: a->tab=0;break;
    case 13: *capture="ui-size-editor.bmp";break;
    case 14:
        {
            unsigned expected_editor=defaults?PS_PREFERENCES_DEFAULT.editor_size:previous_editor;
            ok=ok && a->preferences.editor_size==expected_editor && a->font_code->height==expected_editor;
            printf("UI SIZE EDITOR PRESERVED: expected=%u actual=%u code-height=%.1f complete=%d\n",
                expected_editor,a->preferences.editor_size,a->font_code->height,ok);
        }
        a->tab=2;a->show_report=true;break;
    case 15: *capture="ui-size-plot.bmp";break;
    case 16:
#ifdef __APPLE__
        if(!ui_size_native_cancel_menu(a,stage,&ok))return;
#else
        test_window_key_mod(a->window,SDLK_F10,0);
        test_window_key_mod(a->window,SDLK_DOWN,0);
#endif
        break;
    case 17:
#ifdef __APPLE__
        ok=ok && native_test_shell(a) && SDL_GetKeyboardFocus()==a->window && a->tab==2 && !a->project_manager;
#else
        ok=ok && a->toolbar_keyboard && a->toolbar_menu==1;
        test_window_key_mod(a->window,SDLK_ESCAPE,0);
#endif
        break;
    case 18:
        ok=ok && !a->toolbar_keyboard && !a->toolbar_menu;
        a->quitting=true;printf("UI SIZE SELF-TEST: %s\n",*code?"FAILED":"PASSED");break;
    }
    if(!ok){fprintf(stderr,"UI size failure stage %d: target=%u current=%u draft=%u\n",*stage-1,target,a->preferences.ui_size,a->settings_draft.ui_size);*code=1;a->quitting=true;}
}
