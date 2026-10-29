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
    case 10:if(!manager_test_focus(a,7))return;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 11:okay=a->manager_step==1;if(!manager_test_focus(a,1))return;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 12:okay=browser_active(a) && a->browser!=owner && a->browser->worker!=owner->worker && a->project_manager &&
            !strcmp(owner->query,"main") && owner->selected==selected && owner->preview.text==preview && !strcmp(owner->location,location);
        okay &= browser_test_row(a,"runs").h>0;test_mouse(a,browser_test_row(a,"runs"),true);break;
    case 13:test_mouse(a,browser_test_row(a,"runs"),false);break;
    case 14:if(!browser_test_focus(a,20))return;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 15:{char path[4096];ps_browser_child(browser_test_root,"runs",path);okay=!browser_active(a) && !a->dialog_pending && !strcmp(a->manager_parent,path) && owner->preview.text==preview && !strcmp(owner->query,"main");test_window_key_mod(a->window,SDLK_ESCAPE,0);break;}
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
