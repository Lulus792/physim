/* Real Explorer widgets, worker result, immutable signature selection/copy,
 * resize and the existing F1 window. SDL events are injected; no physical-key
 * or independent screenreader claim is made by this fixture. */
static void browser_api_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static unsigned settle,copy_step;static char *clipboard;static Uint64 copied_at;
    static uint32_t source_crc,signature_crc;static int start,end;
    if(settle++<2)return;settle=0;
    browser_dialog *d=a->browser_workspace;bool okay=true;
    if(d && (browser_busy(d) || d->cache_dirty))return;
#define API_FOCUS(id) do{if(d->focus!=(id) || !d->keyboard || d->reveal){test_window_key_mod(browser_test_window(a),SDLK_TAB,0);return;}}while(0)
    if(*stage==0){
        okay=browser_fixture_write("main.c","// ps_scene_add_id(scene, 1) is only a comment\nint channel = ps_channel_add(context, \"angle\", PS_RADIAN, \"Winkel λ\");\n");
        toolbar_dispatch(a,TOOLBAR_BROWSE);
    }else if(*stage==1 || *stage==2){struct nk_rect row={0};
        for(unsigned k=0;k<d->row_count;k++)if(!strcmp(ps_browser_entry_name(&d->listing.entries[d->rows[k].index]),"main.c"))row=d->rows[k].bounds;
        okay=row.h>0;browser_test_mouse(a,row,*stage==1);
    }else if(*stage==3){
        okay=d->preview.status==PS_OK && d->preview.api.status==PS_OK && d->preview.api.document && !d->api_expanded && !a->api_reference;
        okay &= d->preview.api.symbol && !strcmp(d->preview.api.symbol->name,"ps_channel_add") && d->preview.api.preview.arguments!=SIZE_MAX;
        if(okay){source_crc=ps_crc32((unsigned char*)d->preview.text,d->preview.length);API_FOCUS(21);test_window_key_mod(browser_test_window(a),SDLK_RETURN,0);}
    }else if(*stage==4){
        okay=d->api_expanded && !a->api_reference;if(okay){API_FOCUS(23);
            const ps_doc_block *block=&d->preview.api.document->blocks[d->preview.api.preview.signature];
            signature_crc=ps_crc32((unsigned char*)d->preview.api.document->text+block->text,block->length);
            clipboard=SDL_GetClipboardText();test_window_key_mod(browser_test_window(a),SDLK_A,PS_UI_COMMAND_MOD);}
    }else if(*stage==5){
        if(!copy_step){test_window_key_mod(browser_test_window(a),SDLK_C,PS_UI_COMMAND_MOD);copied_at=SDL_GetTicks();copy_step++;return;}
        const ps_doc_block *block=&d->preview.api.document->blocks[d->preview.api.preview.signature];
        char *actual=SDL_GetClipboardText();bool equal=browser_clipboard_text_equal(d->preview.api.document->text+block->text,actual);SDL_free(actual);
        if(!equal && SDL_GetTicks()-copied_at<1500)return;okay=equal;
        if(clipboard){SDL_SetClipboardText(clipboard);SDL_free(clipboard);clipboard=NULL;}
        start=d->edits[4].select_start;end=d->edits[4].select_end;
        SDL_Event event={0};event.type=SDL_EVENT_TEXT_INPUT;event.text.windowID=SDL_GetWindowID(browser_test_window(a));event.text.text="readonly";SDL_PushEvent(&event);
    }else if(*stage==6){
        const ps_doc_block *block=&d->preview.api.document->blocks[d->preview.api.preview.signature];
        okay=ps_crc32((unsigned char*)d->preview.api.document->text+block->text,block->length)==signature_crc;
        SDL_SetWindowSize(a->window,1080,740);browser_focus_set(a,d,23);
    }else if(*stage==7){
        if(d->reveal)return;const struct nk_text_visual *v=d->edits[4].visual;
        okay=v && d->edits[4].select_start==start && d->edits[4].select_end==end && d->edits[4].scrollbar.x==0;
        if(v)for(int row=0;row<v->row_count;row++)okay &= v->rows[row].width<=v->width+.05f;
        if(v){browser_api_table_layout *table=browser_api_table(d,v->width,a->preferences.ui_size/16.f);
            okay &= table && table->count==2 && table->columns[0]<table->columns[1] && fabsf(table->columns[0]+table->columns[1]-v->width)<.05f;}
        if(okay){API_FOCUS(22);}
    }else if(*stage==8){
        if(!strstr(mode,"large") && !strstr(mode,"light") && !strstr(mode,"contrast"))*capture="explorer-api-context.png";
        float f=a->preferences.ui_size/16.f;
        printf("{\"kind\":\"explorer_api_context\",\"scale\":%.4f,\"symbol\":\"%s\",\"signature_rows\":%d,\"pitch\":%.4f,\"parameter_rows\":%zu,\"source_bytes\":%zu,\"signature_selection\":[%d,%d]}\n",f,d->preview.api.symbol->name,d->edits[4].visual->row_count,d->edits[4].visual->row_height,d->preview.api.document->blocks[d->preview.api.preview.arguments].table_rows,d->preview.length,start,end);
        test_window_key_mod(browser_test_window(a),SDLK_RETURN,0);
    }else if(*stage==9){
        if(a->doc_jump_block>=0 || a->doc_scroll_pending)return;
        okay=a->doc_visible && a->documentation && !strcmp(documentation_topics[a->doc_topic].path,"docs/reference/experiment.md") && a->doc_scroll_y>0;
        test_window_key_mod(a->doc_window,SDLK_ESCAPE,0);
    }else if(*stage==10){
        okay=!a->doc_visible && d->api_expanded && !a->api_reference && ps_crc32((unsigned char*)d->preview.text,d->preview.length)==source_crc;
        SDL_SetWindowSize(a->window,1440,1000);browser_focus_set(a,d,21);
    }else if(*stage==11){API_FOCUS(21);test_window_key_mod(browser_test_window(a),SDLK_RETURN,0);
    }else if(*stage==12){okay=!d->api_expanded;test_window_key_mod(browser_test_window(a),SDLK_F,PS_UI_COMMAND_MOD);
    }else if(*stage==13){browser_test_type(a,"analysis");
    }else if(*stage==14){API_FOCUS(11);okay=d->count==1;
    }else{
        okay=d->preview.text && strstr(d->preview.text,"Analyse Ω") && !d->preview.api.symbol && !d->preview.api.document && !d->api_expanded && !a->api_reference;
        puts(okay?"EXPLORER API CONTEXT SELF-TEST: PASSED":"EXPLORER API CONTEXT SELF-TEST: FAILED");a->quitting=true;
    }
    if(!okay){fprintf(stderr,"Explorer API stage%d failed: focus=%d reveal=%d expanded=%d\n",*stage,d?d->focus:-1,d?d->reveal:0,d?d->api_expanded:0);*code=1;a->quitting=true;}
    (*stage)++;
#undef API_FOCUS
}
