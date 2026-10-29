/* Actual native widgets and queued SDL gestures; no selected-path callback is
 * injected. Physical Windows focus/keys and external readers remain separate. */
static char browser_test_root[4096],browser_test_name[256];
static bool browser_fixture_write(const char *name,const char *text) {
    char path[4096];if(ps_browser_child(browser_test_root,name,path)!=PS_OK)return false;
#ifdef _WIN32
    char native[4104];snprintf(native,sizeof native,"\\\\?\\%s",path);for(char *p=native;*p;p++)if(*p=='/')*p='\\';
    wchar_t *wide=(wchar_t *)SDL_iconv_string("UTF-16LE","UTF-8",native,strlen(native)+1);if(!wide)return false;
    FILE *file=_wfopen(wide,L"wb");SDL_free(wide);
#else
    FILE *file=fopen(path,"wb");
#endif
    if(!file)return false;bool ok=fputs(text,file)>=0;return !fclose(file) && ok;
}
static bool browser_test_start(app *a,const char *root,const char *mode) {
    if(ps_browser_create_directory(root,"files",browser_test_root)!=PS_BROWSER_CREATED)return false;
    char path[4096];if(ps_browser_create_directory(browser_test_root,"assets",path)!=PS_BROWSER_CREATED ||
        ps_browser_create_directory(browser_test_root,"runs",path)!=PS_BROWSER_CREATED)return false;
    if(!browser_fixture_write("main.c","// Actual read-only preview λ\nint sample_value = 7;\n") ||
       !browser_fixture_write("analysis.phys","// Analyse Ω\n") || !browser_fixture_write("physim.project","Projektdateivorschau\n") ||
       !browser_fixture_write("Versuchsnotizen und Annahmen — überarbeitete Fassung.md","# Annahmen\nReale Testdatei, schreibgeschützt als Vorschau.\n") ||
       !browser_fixture_write(".versteckt.txt","Versteckter Eintrag\n"))return false;
    snprintf(a->manager_parent,sizeof a->manager_parent,"%s",browser_test_root);SDL_SetWindowSize(a->window,1440,1000);
    ps_preferences p=a->preferences;
    if(strstr(mode,"light"))p.theme=PS_THEME_LIGHT;if(strstr(mode,"contrast"))p.theme=PS_THEME_HIGH_CONTRAST;
    if(strstr(mode,"large"))p.ui_size=22;preferences_apply(a,&p);
    if(strstr(mode,"narrow"))SDL_SetWindowSize(a->window,1080,740);
    snprintf(browser_test_name,sizeof browser_test_name,"%s",a->manager_name);return true;
}
static struct nk_rect browser_test_row(app *a,const char *name) {
    browser_dialog *d=a->browser;if(d)for(unsigned k=0;k<d->row_count;k++)
        if(!strcmp(ps_browser_entry_name(&d->listing.entries[d->rows[k].index]),name))return d->rows[k].bounds;
    return nk_rect(0,0,0,0);
}
static bool browser_test_focus(app *a,int id) {
    browser_dialog *d=a->browser;if(d && d->focus==id && d->keyboard && !d->reveal)return true;
    test_window_key_mod(a->window,SDLK_TAB,0);return false;
}
static void browser_test_type(app *a,const char *text) {
    test_window_key_mod(a->window,SDLK_A,PS_UI_COMMAND_MOD);SDL_Event event={0};event.type=SDL_EVENT_TEXT_INPUT;
    event.text.windowID=SDL_GetWindowID(a->window);event.text.text=text;SDL_PushEvent(&event);
}
static void browser_reader_frame(app *a,bool embedded,int *stage,int *code) {
    if(*stage==0){SDL_SetWindowSize(a->window,1080,740);if(embedded)toolbar_dispatch(a,TOOLBAR_BROWSE);else choose_workspace_path(a,PS_DIALOG_BROWSE);(*stage)++;return;}
    browser_dialog *d=embedded?a->browser_workspace:a->browser;if(!d || browser_busy(d) || d->cache_dirty)return;
    if(*stage==1){test_window_key_mod(a->window,SDLK_L,PS_UI_COMMAND_MOD);(*stage)++;return;}
    if(*stage==2){if(!d->path_edit || d->reveal)return;puts("EXPLORER READER READY");fflush(stdout);(*stage)++;return;}
    char marker[4096];join(marker,sizeof marker,browser_test_root,"../explorer-reader-complete");
    if(!exists(marker))return;
    bool okay=!browser_active(a) && !browser_workspace_visible(a) && !a->dialog_pending && !a->workspace_open && !a->loaded && !a->document_count;
    printf("EXPLORER READER SELF-TEST: %s\n",okay?"PASSED":"FAILED");fflush(stdout);*code=okay?0:1;a->quitting=true;
}
static bool browser_clipboard_text_equal(const char *source,const char *copy) {
    if(!source || !copy)return false;
#ifdef _WIN32
    /* CF_UNICODETEXT requires CRLF. Only this precise native transformation
     * is accepted; every other original UTF-8 byte must remain identical. */
    for(size_t i=0,j=0;;i++,j++) {
        if(source[i]=='\n' && (!i || source[i-1]!='\r')){if(copy[j]!='\r')return false;j++;}
        if(copy[j]!=source[i])return false;if(!source[i])return true;
    }
#else
    return !strcmp(source,copy);
#endif
}
#include "file_browser_inline_tests.inc"
static void browser_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    if(strstr(mode,"reader")){browser_reader_frame(a,strstr(mode,"inline")!=NULL,stage,code);return;}
    if(strstr(mode,"inline")){browser_inline_test_frame(a,mode,stage,code,capture);return;}
    static unsigned settle;if(settle++<2)return;settle=0;bool ok=true;browser_dialog *d=a->browser;float f=a->preferences.ui_size/16.f;
    if(d && (browser_busy(d) || (d->visible && !d->failed && d->cache_dirty)))return;
#define BROWSER_FOCUS(id) do{if(!browser_test_focus(a,id))return;}while(0)
    switch(*stage) {
    case 0:choose_workspace_path(a,PS_DIALOG_BROWSE);break;
    case 1:
        ok=browser_active(a) && a->dialog_pending && !a->workspace_open && !a->document_count && d->count==6;
        printf("{\"kind\":\"explorer_a\",\"passed\":%s,\"scale\":%.4f,\"modal\":[%.4f,%.4f,%.4f,%.4f],\"list\":[%.4f,%.4f,%.4f,%.4f],\"preview\":[%.4f,%.4f,%.4f,%.4f],\"row_pitch\":%.4f,\"fonts\":[",
            ok?"true":"false",f,d->bounds.x,d->bounds.y,d->bounds.w,d->bounds.h,d->list_bounds.x,d->list_bounds.y,d->list_bounds.w,d->list_bounds.h,d->preview_bounds.x,d->preview_bounds.y,d->preview_bounds.w,d->preview_bounds.h,d->tops[1]);
        for(unsigned k=0;k<8;k++){bool strong=k==1 || k==5,mono=k==6;float em=mono?a->workbench_code_em:strong?a->workbench_bold_em:a->workbench_regular_em;printf("%s%.4f",k?",":"",d->fonts[k].height/(em*f));}printf("]}\n");
        ok &= browser_test_row(a,"main.c").h>0;test_mouse(a,browser_test_row(a,"main.c"),true);break;
    case 2:test_mouse(a,browser_test_row(a,"main.c"),false);break;
    case 3:{static unsigned copy_stage;static char *clipboard;static Uint64 copy_started;
        ok=d->selected>=0 && d->preview.text && strstr(d->preview.text,"sample_value = 7") && !a->document_count && !a->workspace_open;
        if(!copy_stage){BROWSER_FOCUS(12);clipboard=SDL_GetClipboardText();test_window_key_mod(a->window,SDLK_A,PS_UI_COMMAND_MOD);copy_stage++;return;}
        if(copy_stage==1){test_window_key_mod(a->window,SDLK_C,PS_UI_COMMAND_MOD);copy_started=SDL_GetTicks();copy_stage++;return;}
        char *copied=SDL_GetClipboardText();bool equal=browser_clipboard_text_equal(d->preview.text,copied);
        if(!equal && SDL_GetTicks()-copy_started<1500){SDL_free(copied);return;}
        fprintf(stderr,"EXPLORER COPY: equal=%d bytes=%zu expected=%zu active=%d selection=%d/%d host=%d\n",equal,copied?strlen(copied):0,d->preview.length,d->edits[3].active,d->edits[3].select_start,d->edits[3].select_end,SDL_GetKeyboardFocus()==a->window);
        ok &= equal;SDL_free(copied);
        if(clipboard){SDL_SetClipboardText(clipboard);SDL_free(clipboard);clipboard=NULL;}copy_stage=0;
        if(!strcmp(mode,"explorer-a-dark") || !strcmp(mode,"explorer-a-large"))*capture="explorer-a.png";
        break;}
    case 4:BROWSER_FOCUS(7);test_window_key_mod(a->window,SDLK_SPACE,0);break;
    case 5:ok=d->grid;BROWSER_FOCUS(6);test_window_key_mod(a->window,SDLK_SPACE,0);break;
    case 6:ok=!d->grid;test_window_key_mod(a->window,SDLK_F,PS_UI_COMMAND_MOD);break;
    case 7:browser_test_type(a,"main");test_window_key_mod(a->window,SDLK_TAB,0);break;
    case 8:ok=!strcmp(d->query,"main") && d->count==1;test_window_key_mod(a->window,SDLK_F,PS_UI_COMMAND_MOD);break;
    case 9:{static bool selected_all;
        /* An empty text-input event does not delete a selection. Perform the
         * real select-all and Delete gestures in successive input frames. */
        if(!selected_all){test_window_key_mod(a->window,SDLK_A,PS_UI_COMMAND_MOD);selected_all=true;return;}
        test_window_key_mod(a->window,SDLK_DELETE,0);test_window_key_mod(a->window,SDLK_TAB,0);selected_all=false;break;}
    case 10:ok=d->count==6;BROWSER_FOCUS(15);test_window_key_mod(a->window,SDLK_SPACE,0);break;
    case 11:ok=d->create;BROWSER_FOCUS(16);browser_test_type(a,"Neu λ");test_window_key_mod(a->window,SDLK_TAB,0);break;
    case 12:BROWSER_FOCUS(17);test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 13:ok=!d->create && d->count==7;test_window_key_mod(a->window,SDLK_L,PS_UI_COMMAND_MOD);break;
    case 14:{static char missing[4096];snprintf(missing,sizeof missing,"%s/missing",browser_test_root);browser_test_type(a,missing);test_window_key_mod(a->window,SDLK_RETURN,0);break;}
    case 15:ok=d->failed && !a->workspace_open && !a->document_count;BROWSER_FOCUS(1);test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 16:ok=!d->failed && d->count==7;test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    case 17:ok=!browser_active(a) && !a->dialog_pending;open_project_manager(a,"Datei");break;
    case 18:if(!manager_test_focus(a,7))return;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 19:ok=a->manager_step==1;if(!manager_test_focus(a,1))return;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 20:ok=browser_active(a) && a->project_manager && d->mode==PS_DIALOG_MANAGER_PARENT && d->filter==PS_BROWSER_FOLDERS && d->count==3;
        ok &= browser_test_row(a,"runs").h>0;test_mouse(a,browser_test_row(a,"runs"),true);break;
    case 21:test_mouse(a,browser_test_row(a,"runs"),false);break;
    case 22:BROWSER_FOCUS(20);test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 23:{char path[4096];ps_browser_child(browser_test_root,"runs",path);ok=!browser_active(a) && !a->dialog_pending && a->project_manager && a->manager_step==1 && !strcmp(a->manager_parent,path) && !strcmp(a->manager_name,browser_test_name);test_window_key_mod(a->window,SDLK_ESCAPE,0);break;}
    case 24:ok=!a->project_manager;choose_workspace_path(a,PS_DIALOG_BROWSE);break;
    case 25:test_window_key_mod(a->window,SDLK_UP,SDL_KMOD_ALT);break;
    case 26:ok=d->count==7 && browser_test_row(a,"main.c").h>0;test_mouse(a,browser_test_row(a,"main.c"),true);break;
    case 27:test_mouse(a,browser_test_row(a,"main.c"),false);break;
    case 28:BROWSER_FOCUS(20);test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 29:ok=!browser_active(a) && !a->dialog_pending && a->document_count==1 && !a->workspace_open && !a->loaded && active_document(a)!=NULL;
        printf("EXPLORER A SELF-TEST: %s\n",ok?"PASSED":"FAILED");a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Explorer A stage %d: active=%d modal=%d focus=%d keyboard=%d reveal=%d count=%zu failed=%d message=%s\n",*stage,browser_active(a),a->project_manager,d?d->focus:-1,d?d->keyboard:0,d?d->reveal:0,d?d->count:0,d?d->failed:0,d?d->message:"");*code=1;a->quitting=true;}
    (*stage)++;
#undef BROWSER_FOCUS
}
