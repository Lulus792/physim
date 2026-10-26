/* One final-state image per distinct theme/size; input steps are JSON only. */
static bool protocol_original_test_reveal(app *a,unsigned *attempts) {
    ps_protocol_ui *p=&a->protocol_view;struct nk_rect box=p->original_bounds,pane=p->panes[2],outer=p->bounds;
    float top=fmaxf(pane.y+2,outer.y+2),bottom=fminf(pane.y+pane.h-2,outer.y+outer.h-30);
    if(p->original_layout && p->original_viewport.h>=p->original_layout->visual.row_height)return true;
    /* A wheel does not activate Nuklear's root window. Focus the neutral log
     * margin through a real click before asking its child groups to scroll. */
    if(*attempts<2) {
        struct nk_rect neutral=nk_rect(outer.x+4,outer.y+16,1,1);
        test_mouse(a,neutral,*attempts==0);(*attempts)++;return false;
    }
    if(++*attempts>100)return false;
    float x=outer.x+8,y=outer.y+outer.h*.5f,delta=-3;
    /* A layout breakpoint can move the whole details group above the root
     * viewport. Its previously rendered field rectangle is then stale. Reveal
     * the group first, before using that rectangle for its inner scrollbar. */
    if(pane.y+pane.h<=outer.y+30)delta=3;
    else if(pane.y>=outer.y+outer.h-30)delta=-3;
    else if(box.h>0 && pane.y<outer.y+outer.h-30 && pane.y+pane.h>outer.y+30) {
        /* nk_panel_begin removes group padding from layout->bounds. Use the
         * field's own border, inside that content but outside its text area. */
        x=box.x+1;y=fmaxf(top+20,fminf(bottom-5,pane.y+pane.h*.5f));
        delta=box.y+box.h<=top?3:-3;
    } else if(box.h>0 && box.y<top)delta=3;
    if(*attempts%20==3)printf("{\"kind\":\"protocol_original_reveal\",\"attempt\":%u,\"point\":[%.2f,%.2f],\"field_y\":%.2f,\"pane_y\":%.2f,\"active\":\"%s\"}\n",
        *attempts,x,y,box.y,pane.y,a->ui->active?a->ui->active->name_string:"");
    test_plot_wheel(a,x,y,delta);return false;
}
static void protocol_original_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static unsigned rendered,attempts;
    if(*stage<3){(*stage)++;return;}
    if(rendered++<2)return;rendered=0;
    ps_protocol_ui *p=&a->protocol_view;bool ok=true,copy_equal=true;
    int w=0,h=0;SDL_GetWindowSize(a->window,&w,&h);
    if((*stage==3 || *stage==11) && !protocol_original_test_reveal(a,&attempts)) {
        if(attempts<=100)return;
        ok=false;
    } else if(*stage==3 || *stage==11)attempts=0;
    if(ok)switch(*stage) {
    case 3:test_mouse(a,p->original_viewport,true);break;
    case 4:test_mouse(a,p->original_viewport,false);break;
    case 5:
        ok=p->original_edit.active;
        /* Keep hover text from obscuring the glyph being measured. Pointer
         * motion alone preserves the original field's keyboard ownership. */
        test_plot_motion(a,p->bounds.x+4,p->bounds.y+16);
        test_window_key_mod(a->window,SDLK_END,PS_UI_COMMAND_MOD);break;
    case 6:
        ok=p->original_layout && p->original_layout->visual.row_count>10 && p->original_edit.cursor==p->original_edit.string.len &&
            p->original_edit.scrollbar.y==0 && p->detail_scroll_y>0 &&
            fabsf(p->original_bounds.h-p->original_layout->visual.row_count*p->original_layout->visual.row_height)<.02f;
        *capture="protocol-original-end.png";break;
    case 7:test_window_key_mod(a->window,SDLK_A,PS_UI_COMMAND_MOD);break;
    case 8:test_window_key_mod(a->window,SDLK_C,PS_UI_COMMAND_MOD);break;
    case 9:{char *copied=SDL_GetClipboardText();copy_equal=copied && !strcmp(copied,protocol_wrap_test_text);ok=copy_equal;SDL_free(copied);break;}
    case 10:SDL_SetWindowSize(a->window,w>=1240?w-160:w+160,h);break;
    case 11:
        ok=p->original_edit.select_start==0 && p->original_edit.select_end==p->original_edit.string.len;
        ok &= nk_sdl_accessibility_request_focus(a->ui,NULL,"Originalmeldung");break;
    case 12:ok=p->original_edit.active;test_window_key_mod(a->window,SDLK_DELETE,0);break;
    case 13:ok=!strcmp(p->original_page,protocol_wrap_test_text) && !strcmp(p->original,protocol_wrap_test_text);a->quitting=true;break;
    }
    uint32_t source_crc=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
    ok &= source_crc==protocol_test_source_crc;
    float end_box[4]={0};
    if(p->original_layout && p->original_layout->visual.glyph_count>0) {
        const struct nk_text_visual *v=&p->original_layout->visual;int last=v->glyph_count-1;
        int row=nk_text_visual_row_at(v,last,0);
        end_box[0]=p->original_bounds.x+v->glyphs[last].x;
        end_box[1]=p->original_bounds.y+row*v->row_height-p->original_edit.scrollbar.y+(v->row_height-p->fonts[6].height)/2;
        end_box[2]=v->glyphs[last].advance;end_box[3]=p->fonts[6].height;
    }
    printf("{\"kind\":\"protocol_original_step\",\"stage\":%d,\"passed\":%s,\"source_crc\":%u,\"original_crc\":%u,\"original_bytes\":%zu,\"rows\":%d,\"cursor\":%d,\"selection\":[%d,%d],\"scroll_y\":%.4f,\"copy_equal\":%s,\"window\":[%d,%d],\"viewport\":[%.4f,%.4f,%.4f,%.4f],\"end_box\":[%.4f,%.4f,%.4f,%.4f],\"field\":[%.4f,%.4f,%.4f,%.4f],\"row_height\":%.4f,\"detail_scroll_y\":%u}\n",
        *stage,ok?"true":"false",(unsigned)source_crc,(unsigned)ps_crc32((const unsigned char*)p->original_page,strlen(p->original_page)),strlen(p->original_page),
        p->original_layout?p->original_layout->visual.row_count:0,p->original_edit.cursor,p->original_edit.select_start,p->original_edit.select_end,p->original_edit.scrollbar.y,
        copy_equal?"true":"false",w,h,p->original_viewport.x,p->original_viewport.y,p->original_viewport.w,p->original_viewport.h,end_box[0],end_box[1],end_box[2],end_box[3],
        p->original_bounds.x,p->original_bounds.y,p->original_bounds.w,p->original_bounds.h,p->original_layout?p->original_layout->visual.row_height:0,(unsigned)p->detail_scroll_y);fflush(stdout);
    if(!ok){*code=1;a->quitting=true;*capture="protocol-original-failure.png";fprintf(stderr,"Original wrap stage %d failed\n",*stage);}
    if(a->quitting)printf("PROTOCOL ORIGINAL %s SELF-TEST: %s\n",mode,*code?"FAILED":"PASSED");
    (*stage)++;
}
