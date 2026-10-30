/* Real free-view widgets plus a separately owned nested picker. Editor contents
 * are deliberately seeded dirty; subsequent navigation must preserve them. */
static void browser_inline_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static unsigned settle;static uint32_t text_crc,undo_crc;static int cursor,selection_start,selection_end;
    static ps_dock_layout dock,hidden_return;static browser_dialog *owner;static char *preview;static char location[4096];static int selected;
    if(settle++<2)return;settle=0;
    browser_dialog *d=a->browser_workspace;workspace_document *document=active_document(a);bool okay=true;
    if((d && (browser_busy(d) || (browser_workspace_visible(a) && d->cache_dirty))) ||
       (browser_active(a) && (browser_busy(a->browser) || a->browser->cache_dirty)))return;
#define INLINE_FOCUS(id) do{if(!d || d->focus!=(id) || !d->keyboard || d->reveal){test_window_key_mod(a->window,SDLK_TAB,0);return;}}while(0)
    switch(*stage){
    case 0:{char path[4096];ps_browser_child(browser_test_root,"main.c",path);preview_workspace_path(a,path);break;}
    case 1:
        okay=document && a->tab==8;if(!okay)break;
        nk_textedit_text(&document->edit,"// Ungesichert λ\n",(int)strlen("// Ungesichert λ\n"));
        text_crc=ps_crc32((const unsigned char *)nk_str_get_const(&document->edit.string),(size_t)nk_str_len_char(&document->edit.string));
        undo_crc=ps_crc32((const unsigned char *)&document->edit.undo,sizeof document->edit.undo);
        cursor=document->edit.cursor;selection_start=document->edit.select_start;selection_end=document->edit.select_end;
        if(strstr(mode,"hidden")){hidden_return=a->preferences.dock;okay=ps_dock_hide(&a->preferences.dock,PS_DOCK_WORKSPACE) && ps_dock_valid(&a->preferences.dock);}
        dock=a->preferences.dock;
        toolbar_dispatch(a,TOOLBAR_BROWSE);break;
    case 2:{
        int width,height;SDL_GetWindowSize(a->window,&width,&height);
        okay=d && d->embedded && browser_workspace_visible(a) && !browser_active(a) && !a->dialog_pending && d->count==6 && a->tab==9;
        okay &= d && d->bounds.x==0 && d->bounds.y==WORKSPACE_CONTENT_TOP && d->bounds.w==width && d->bounds.h==height-WORKSPACE_CONTENT_TOP-WORKSPACE_STATUS_HEIGHT;
        okay &= !memcmp(&dock,&a->preferences.dock,sizeof dock);
        if(strstr(mode,"hidden")){okay &= ps_dock_valid(&a->preferences.dock);a->preferences.dock=hidden_return;dock=hidden_return;}
        if(!okay)break;owner=d;strcpy(location,d->location);
        printf("{\"kind\":\"explorer_inline\",\"scale\":%.4f,\"frame\":[%.4f,%.4f,%.4f,%.4f],\"list\":[%.4f,%.4f,%.4f,%.4f],\"preview\":[%.4f,%.4f,%.4f,%.4f],\"body\":%.4f,\"source_crc\":%u}\n",
            a->preferences.ui_size/16.f,d->bounds.x,d->bounds.y,d->bounds.w,d->bounds.h,d->list_bounds.x,d->list_bounds.y,d->list_bounds.w,d->list_bounds.h,d->preview_bounds.x,d->preview_bounds.y,d->preview_bounds.w,d->preview_bounds.h,d->list_bounds.h,text_crc);
        struct nk_rect row={0};for(unsigned i=0;i<d->row_count;i++)if(!strcmp(ps_browser_entry_name(&d->listing.entries[d->rows[i].index]),"main.c"))row=d->rows[i].bounds;
        okay=row.h>0;test_mouse(a,row,true);break;}
    case 3:{struct nk_rect row={0};for(unsigned i=0;i<d->row_count;i++)if(!strcmp(ps_browser_entry_name(&d->listing.entries[d->rows[i].index]),"main.c"))row=d->rows[i].bounds;test_mouse(a,row,false);break;}
    case 4:
        okay=d->preview.text && strstr(d->preview.text,"Actual read-only preview") && !strstr(d->preview.text,"Ungesichert");
        if(!strcmp(mode,"explorer-a-inline-dark") || !strcmp(mode,"explorer-a-inline-large"))*capture="explorer-inline.png";
        test_window_key_mod(a->window,SDLK_F,PS_UI_COMMAND_MOD);break;
    case 5:browser_test_type(a,"main");test_window_key_mod(a->window,SDLK_TAB,0);break;
    case 6:
        okay=!strcmp(d->query,"main") && d->count==1;preview=d->preview.text;selected=d->selected;
        test_mouse(a,a->navigation_bounds[2],true);break;
    case 7:test_mouse(a,a->navigation_bounds[2],false);break;
    case 8:okay=a->tab==2 && !browser_workspace_visible(a) && !browser_active(a);toolbar_dispatch(a,TOOLBAR_BROWSE);break;
    case 9:okay=d==owner && browser_workspace_visible(a) && !strcmp(d->query,"main") && d->selected==selected && d->preview.text==preview && !strcmp(d->location,location);open_project_manager(a,"Datei");break;
    case 10:if(!manager_test_focus(a,7))return;manager_test_key(a,SDLK_RETURN,0);break;
    case 11:okay=a->manager_step==1;if(!manager_test_focus(a,1))return;manager_test_key(a,SDLK_RETURN,0);break;
    case 12:okay=browser_active(a) && a->browser!=owner && a->browser->worker!=owner->worker && a->project_manager &&
            !strcmp(owner->query,"main") && owner->selected==selected && owner->preview.text==preview && !strcmp(owner->location,location);
        okay &= browser_test_row(a,"runs").h>0;test_mouse(a,browser_test_row(a,"runs"),true);break;
    case 13:test_mouse(a,browser_test_row(a,"runs"),false);break;
    case 14:if(!browser_test_focus(a,20))return;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 15:{char path[4096];ps_browser_child(browser_test_root,"runs",path);okay=!browser_active(a) && !a->dialog_pending && !strcmp(a->manager_parent,path) && owner->preview.text==preview && !strcmp(owner->query,"main");manager_test_key(a,SDLK_ESCAPE,0);break;}
    case 16:okay=browser_workspace_visible(a) && d==owner && !strcmp(d->query,"main") && d->selected==selected && !strcmp(d->location,location);toolbar_dispatch(a,TOOLBAR_WORKSPACES);break;
    case 17:okay=a->workspace_manager && !browser_workspace_visible(a);test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    case 18:okay=!a->workspace_manager && browser_workspace_visible(a) && !strcmp(d->query,"main");test_window_key_mod(a->window,SDLK_F,PS_UI_COMMAND_MOD);toolbar_dispatch(a,TOOLBAR_LOG);break;
    case 19:{int width,height;SDL_GetWindowSize(a->window,&width,&height);okay=a->show_log && fabsf(d->bounds.y+d->bounds.h-a->protocol_view.bounds.y)<.05f && !memcmp(&dock,&a->preferences.dock,sizeof dock);toolbar_dispatch(a,TOOLBAR_LOG);break;}
    case 20:okay=!a->show_log;INLINE_FOCUS(20);test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 21:
        okay=!browser_workspace_visible(a) && !browser_active(a) && !a->dialog_pending && a->tab==8 && a->document_count==1;
        printf("EXPLORER INLINE SELF-TEST: %s\n",okay?"PASSED":"FAILED");a->quitting=true;break;
    }
    document=active_document(a);
    if(*stage>1)okay &= document!=NULL;
    if(*stage>1 && document){okay &= ps_crc32((const unsigned char *)nk_str_get_const(&document->edit.string),(size_t)nk_str_len_char(&document->edit.string))==text_crc;
        okay &= ps_crc32((const unsigned char *)&document->edit.undo,sizeof document->edit.undo)==undo_crc;
        okay &= document->edit.cursor==cursor && document->edit.select_start==selection_start && document->edit.select_end==selection_end;
        /* Case8 queues the return from analysis; its layout restoration runs
         * in the next real UI frame. Check that completed state at case9. */
        if(a->tab==9 && *stage!=8)okay &= !memcmp(&dock,&a->preferences.dock,sizeof dock);}
    if(!okay){fprintf(stderr,"Explorer inline stage%d failed: tab=%d inline=%d modal=%d focus=%d reveal=%d count=%zu\n",*stage,a->tab,browser_workspace_visible(a),browser_active(a),d?d->focus:-1,d?d->reveal:0,d?d->count:0);*code=1;a->quitting=true;}
    (*stage)++;
#undef INLINE_FOCUS
}

/* Source bytes remain unchanged across soft wrap, copy, resize and readonly
 * input. Native reader mode waits for the independent process to complete. */
static void browser_preview_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static unsigned settle;static char *clipboard;static int copy_step;static Uint64 copied_at;
    static int start,end;static uint32_t checksum;bool reader=strstr(mode,"reader")!=NULL,okay=true;
    if(settle++<2)return;settle=0;browser_dialog *d=a->browser_workspace;
    if(d && (browser_busy(d) || d->cache_dirty))return;
    if(*stage==0){char text[8192]="";for(unsigned i=0;i<20;i++)strcat(text,"Zeile λ Á 🌍\t123456789_abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");strcat(text,"ENDE λ🌍");
        okay=browser_fixture_write("lang.txt",text) && (!reader || browser_fixture_write("main.c",text));toolbar_dispatch(a,TOOLBAR_BROWSE);
    }else if(*stage==1 || *stage==2){struct nk_rect row={0};for(unsigned k=0;k<d->row_count;k++)if(!strcmp(ps_browser_entry_name(&d->listing.entries[d->rows[k].index]),"main.c"))row=d->rows[k].bounds;okay=row.h>0;test_mouse(a,row,*stage==1);
    }else if(reader){
        if(*stage==3){if(d->focus!=12 || !d->keyboard || d->reveal){test_window_key_mod(a->window,SDLK_TAB,0);return;}
            test_mouse(a,nk_rect(d->preview_bounds.x+3,d->preview_text_bounds.y+3,6,6),true);}
        else if(*stage==4)test_mouse(a,nk_rect(d->preview_bounds.x+3,d->preview_text_bounds.y+3,6,6),false);
        else if(*stage==5){okay=!d->edits[3].active;puts("EXPLORER PREVIEW READER READY");fflush(stdout);}
        else {char marker[4096];join(marker,sizeof marker,browser_test_root,"../explorer-reader-complete");if(!exists(marker))return;
            okay=!browser_active(a) && !browser_workspace_visible(a) && !a->document_count && !a->workspace_open;
            puts(okay?"EXPLORER PREVIEW READER SELF-TEST: PASSED":"EXPLORER PREVIEW READER SELF-TEST: FAILED");a->quitting=true;}
    }else if(*stage==3 || *stage==9){
        if(d->focus!=12 || !d->keyboard || d->reveal){test_window_key_mod(a->window,SDLK_TAB,0);return;}
        okay=d->preview.text && d->edits[3].visual && d->preview_text_bounds.h>0;
        if(okay){const struct nk_text_visual *v=d->edits[3].visual;float f=a->preferences.ui_size/16.f;bool long_text=*stage==9;
            okay &= long_text?fabsf(d->preview_text_bounds.h-240*f)<.05f:d->preview_text_bounds.h<100*f;
            for(int k=0;k<v->row_count;k++)okay &= v->rows[k].width<=v->width+.05f;
            printf("{\"kind\":\"explorer_preview_flow\",\"long\":%s,\"scale\":%.4f,\"height\":%.4f,\"rows\":%d,\"pitch\":%.4f,\"horizontal_scroll\":%.4f,\"source_bytes\":%zu}\n",long_text?"true":"false",f,d->preview_text_bounds.h,v->row_count,v->row_height,d->edits[3].scrollbar.x,d->preview.length);
            checksum=ps_crc32((const unsigned char*)d->preview.text,d->preview.length);
            if(!copy_step){clipboard=SDL_GetClipboardText();test_window_key_mod(a->window,SDLK_A,PS_UI_COMMAND_MOD);copy_step++;return;}
            if(copy_step==1){test_window_key_mod(a->window,SDLK_C,PS_UI_COMMAND_MOD);copied_at=SDL_GetTicks();copy_step++;return;}
            char *actual=SDL_GetClipboardText();bool equal=browser_clipboard_text_equal(d->preview.text,actual);SDL_free(actual);
            if(!equal && SDL_GetTicks()-copied_at<1500)return;okay &= equal && d->edits[3].scrollbar.x==0;
            if(clipboard){SDL_SetClipboardText(clipboard);SDL_free(clipboard);clipboard=NULL;}copy_step=0;
            if(!long_text){start=d->edits[3].select_start;end=d->edits[3].select_end;SDL_SetWindowSize(a->window,1080,740);browser_focus_set(a,d,12);}
            else {SDL_Event event={0};event.type=SDL_EVENT_TEXT_INPUT;event.text.windowID=SDL_GetWindowID(a->window);event.text.text="must remain readonly";SDL_PushEvent(&event);}
        }
    }else if(*stage==4){if(d->reveal)return;okay=d->edits[3].select_start==start && d->edits[3].select_end==end && ps_crc32((const unsigned char*)d->preview.text,d->preview.length)==checksum;test_window_key_mod(a->window,SDLK_F,PS_UI_COMMAND_MOD);
    }else if(*stage==5)browser_test_type(a,"lang");
    else if(*stage==6){if(d->focus!=11 || !d->keyboard || d->reveal){test_window_key_mod(a->window,SDLK_TAB,0);return;}}
    else if(*stage==7){okay=d->selected>=0 && d->preview.text && strstr(d->preview.text,"ENDE λ🌍");browser_focus_set(a,d,12);}
    else if(*stage==8){if(d->reveal)return;}
    else if(*stage==10){okay=ps_crc32((const unsigned char*)d->preview.text,d->preview.length)==checksum && !a->document_count;
        if(!strstr(mode,"large"))*capture="explorer-preview-flow.png";puts(okay?"EXPLORER PREVIEW FLOW SELF-TEST: PASSED":"EXPLORER PREVIEW FLOW SELF-TEST: FAILED");a->quitting=true;}
    if(!okay){fprintf(stderr,"Explorer preview stage%d failed\n",*stage);*code=1;a->quitting=true;}(*stage)++;
}
