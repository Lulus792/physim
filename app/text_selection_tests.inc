/* Actual code/search/readonly widgets and queued SDL input. No physical-focus claim. */
static struct {char root[4096];unsigned kind,settle;const char *phase;char *clipboard;Uint64 copied_at;} selection_a2;
static const char selection_a2_code[]="/* λ Original */\nint main(void) {\n  const char *label = \"λ Á\";\n  double value = 2.5;\n  return 0;\n}\n";
static struct nk_text_edit *selection_a2_edit(app *a) {
    return selection_a2.kind==0?&a->experiment:selection_a2.kind==1?&a->browser->edits[1]:&a->browser->edits[3];
}
static const char *selection_a2_text(app *a) {return selection_a2.kind==0?nk_str_get_const(&a->experiment.string):selection_a2.kind==1?a->browser->query:a->browser->preview.text;}
static size_t selection_a2_bytes(app *a){return selection_a2.kind==0?(size_t)nk_str_len_char(&a->experiment.string):strlen(selection_a2_text(a));}
static bool selection_a2_start(app *a,const char *root,const char *mode) {
    memset(&selection_a2,0,sizeof selection_a2);snprintf(selection_a2.root,sizeof selection_a2.root,"%s",root);
    selection_a2.kind=strstr(mode,"-code-")?0:strstr(mode,"-field-")?1:2;
    bool okay=selection_a2.kind==0?workbench_test_start(a,root,mode):browser_test_start(a,root,mode);if(!okay)return false;
    if(selection_a2.kind==0){nk_textedit_select_all(&a->experiment);nk_textedit_paste(&a->experiment,selection_a2_code,(int)strlen(selection_a2_code));
        a->experiment.cursor=a->experiment.select_start=a->experiment.select_end=0;a->experiment.scrollbar=nk_vec2(0,0);a->show_log=false;}
    SDL_SetWindowSize(a->window,strstr(mode,"large")?1600:1440,1000);
    selection_a2.clipboard=SDL_GetClipboardText();return true;
}
static bool selection_a2_inside(struct nk_rect r,float x,float y){return x>=r.x && x<r.x+r.w && y>=r.y && y<r.y+r.h;}
static void selection_a2_rect(struct nk_rect r){printf("[%.4f,%.4f,%.4f,%.4f]",r.x,r.y,r.w,r.h);}
static void selection_a2_probe(app *a,struct nk_context *ui,struct nk_rect field) {
    if(!selection_a2.phase)return;
    const ps_ui_palette *palette=a->preferences.theme==PS_THEME_LIGHT?&PS_UI_LIGHT:a->preferences.theme==PS_THEME_HIGH_CONTRAST?&PS_UI_HIGH_CONTRAST:&PS_UI_DARK;
    struct nk_text_edit *edit=selection_a2_edit(a);const char *value=selection_a2_text(a);if(!value)value="";size_t bytes=selection_a2_bytes(a);
    printf("{\"kind\":\"text_selection_a2\",\"phase\":\"%s\",\"context\":%u,\"crc\":%u,\"bytes\":%zu,\"selection\":[%d,%d],\"field\":",selection_a2.phase,selection_a2.kind,ps_crc32((const unsigned char*)value,bytes),bytes,edit->select_start,edit->select_end);selection_a2_rect(field);
    struct nk_rect clip=nk_rect(0,0,100000,100000);const struct nk_command *command;bool comma=false;
    printf(",\"fills\":[");
    nk_foreach(command,ui){
        if(command->type==NK_COMMAND_SCISSOR){const struct nk_command_scissor *c=(const struct nk_command_scissor*)command;clip=nk_rect(c->x,c->y,c->w,c->h);}
        if(command->type!=NK_COMMAND_RECT_FILLED)continue;const struct nk_command_rect_filled *r=(const struct nk_command_rect_filled*)command;
        if(memcmp(&r->color,&palette->text_selection,sizeof r->color) || !selection_a2_inside(field,r->x,r->y))continue;
        if(comma)putchar(',');comma=true;printf("{\"box\":");selection_a2_rect(nk_rect(r->x,r->y,r->w,r->h));printf(",\"clip\":");selection_a2_rect(clip);printf(",\"color\":[%u,%u,%u,%u]}",r->color.r,r->color.g,r->color.b,r->color.a);
    }
    printf("],\"texts\":[");comma=false;clip=nk_rect(0,0,100000,100000);
    nk_foreach(command,ui){
        if(command->type==NK_COMMAND_SCISSOR){const struct nk_command_scissor *c=(const struct nk_command_scissor*)command;clip=nk_rect(c->x,c->y,c->w,c->h);}
        if(command->type!=NK_COMMAND_TEXT)continue;const struct nk_command_text *t=(const struct nk_command_text*)command;
        if(!t->length || t->length>256 || !t->foreground.a || !selection_a2_inside(field,t->x,t->y))continue;
        if(comma)putchar(',');comma=true;printf("{\"box\":");selection_a2_rect(nk_rect(t->x,t->y,t->w,t->h));printf(",\"clip\":");selection_a2_rect(clip);printf(",\"color\":[%u,%u,%u,%u],\"hex\":\"",t->foreground.r,t->foreground.g,t->foreground.b,t->foreground.a);
        for(int k=0;k<t->length;k++)printf("%02x",(unsigned char)t->string[k]);printf("\"}");
    }
    printf("]}\n");selection_a2.phase=NULL;
}
static app *selection_a2_owner;
static void selection_a2_browser_probe(browser_dialog *d){selection_a2_probe(selection_a2_owner,d->ui,selection_a2.kind==1?d->controls[4]:d->preview_text_bounds);}
static void selection_a2_draw(app *a){if(selection_a2.kind==0)selection_a2_probe(a,a->ui,a->source_editor_bounds);}
static void selection_a2_capture(app *a,const char *phase,const char **capture) {
    selection_a2.phase=phase;const char *name=!strcmp(phase,"before")?"a2-before.png":"a2-after.png";
    if(selection_a2.kind==0)*capture=name;else join(a->browser->capture,sizeof a->browser->capture,selection_a2.root,name);
}
static void selection_a2_frame(app *a,int *stage,int *code,const char **capture) {
    if(selection_a2.settle++<3)return;selection_a2.settle=0;browser_dialog *d=a->browser;bool okay=true;
    if(selection_a2.kind && d && (browser_busy(d) || d->cache_dirty || d->reveal))return;
    SDL_Window *window=selection_a2.kind?browser_test_window(a):a->window;
    switch(*stage) {
    case 0:if(selection_a2.kind){choose_workspace_path(a,PS_DIALOG_BROWSE);selection_a2_owner=a;}break;
    case 1:
        if(selection_a2.kind){okay=d && d->window;if(!okay)break;d->draw_probe=selection_a2_browser_probe;
            if(selection_a2.kind==1){if(!browser_test_focus(a,4))return;browser_test_type(a,"main");}
            else browser_test_mouse(a,browser_test_row(a,"main.c"),true);}
        break;
    case 2:if(selection_a2.kind==2)browser_test_mouse(a,browser_test_row(a,"main.c"),false);break;
    case 3:
        if(selection_a2.kind==1){okay=!strcmp(d->query,"main");if(!browser_test_focus(a,4))return;}
        else if(selection_a2.kind==2){if(!browser_test_focus(a,12))return;okay=d->preview.text && d->preview_text_bounds.h>0;}
        break;
    case 4:selection_a2_capture(a,"before",capture);break;
    case 5:
        if(!selection_a2.kind)test_window_mouse(window,nk_rect(a->source_editor_bounds.x+40,a->source_editor_bounds.y+15,2,2),true);
        else if(!browser_test_focus(a,selection_a2.kind==1?4:12))return;
        break;
    case 6:if(!selection_a2.kind)test_window_mouse(window,nk_rect(a->source_editor_bounds.x+40,a->source_editor_bounds.y+15,2,2),false);break;
    case 7:test_window_key_mod(window,SDLK_A,PS_UI_COMMAND_MOD);break;
    case 8:{const char *value=selection_a2_text(a);struct nk_text_edit *edit=selection_a2_edit(a);okay=value && abs(edit->select_end-edit->select_start)==nk_utf_len(value,(int)selection_a2_bytes(a));
        if(!selection_a2.kind)okay&=selection_a2_bytes(a)==sizeof selection_a2_code-1 && !memcmp(value,selection_a2_code,sizeof selection_a2_code-1);
        selection_a2_capture(a,"after",capture);test_window_key_mod(window,SDLK_C,PS_UI_COMMAND_MOD);selection_a2.copied_at=SDL_GetTicks();break;}
    case 9:{char *copy=SDL_GetClipboardText();okay=browser_clipboard_text_equal(selection_a2.kind?selection_a2_text(a):selection_a2_code,copy);SDL_free(copy);if(!okay && SDL_GetTicks()-selection_a2.copied_at<1500)return;
        if(selection_a2.clipboard){SDL_SetClipboardText(selection_a2.clipboard);SDL_free(selection_a2.clipboard);selection_a2.clipboard=NULL;}break;}
    case 10:puts("TEXT SELECTION A2 SELF-TEST: PASSED");a->quitting=true;break;
    }
    if(!okay){fprintf(stderr,"A2 text selection failed stage%d context%u\n",*stage,selection_a2.kind);*code=1;a->quitting=true;}else (*stage)++;
}
