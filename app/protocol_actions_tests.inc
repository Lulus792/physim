/* One visible action-state capture; pointer, clipboard and geometry are text. */
static bool protocol_actions_clipboard_matches(const char *raw,const char *clipboard) {
    if(!raw || !clipboard)return false;
    for(size_t i=0;raw[i];i++) {
#if defined(_WIN32)
        /* SDL 3.2.30's CF_UNICODETEXT serializer inserts CR before bare LF.
         * Match that exact output; do not strip arbitrary CR or whitespace. */
        if(raw[i]=='\n' && (!i || raw[i-1]!='\r') && *clipboard++!='\r')return false;
#endif
        if(*clipboard++!=raw[i])return false;
    }
    return !*clipboard;
}
static void protocol_actions_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static unsigned rendered,attempts;
    if(*stage<3){(*stage)++;return;}
    if(rendered++<2)return;rendered=0;
    ps_protocol_ui *p=&a->protocol_view;float f=a->preferences.ui_size/16.f;bool ok=true;
    struct nk_user_font font=*a->font_ui;font.height=14*a->workbench_regular_em*f;
    float source_width=font.width(font.userdata,font.height,"Quellstelle ansehen →",(int)strlen("Quellstelle ansehen →"))+24.4f*f;
    float copy_width=font.width(font.userdata,font.height,"Meldung kopieren",(int)strlen("Meldung kopieren"))+24.4f*f;
    float height=37.14f*f;
    switch(*stage) {
    case 3:test_mouse(a,nk_rect(p->bounds.x+4,p->bounds.y+16,1,1),true);break;
    case 4:test_mouse(a,nk_rect(p->bounds.x+4,p->bounds.y+16,1,1),false);break;
    case 5: {
        float top=fmaxf(p->panes[2].y+20*f,p->bounds.y+30*f),bottom=fminf(p->panes[2].y+p->panes[2].h-20*f,p->bounds.y+p->bounds.h-30*f);
        if(p->copy.y<top || p->copy.y+p->copy.h>bottom) {
            if(++attempts>100){ok=false;break;}
            float x=p->bounds.x+8,y=p->bounds.y+p->bounds.h*.5f,delta=-3;
            bool fits_parent=p->copy.y>=p->panes[2].y+20*f && p->copy.y+p->copy.h<=p->panes[2].y+p->panes[2].h-20*f;
            if(!fits_parent && p->panes[2].y<p->bounds.y+p->bounds.h-30*f && p->panes[2].y+p->panes[2].h>p->bounds.y+30*f) {
                x=p->panes[2].x+22*f;y=fmaxf(top+2,fminf(bottom-2,(top+bottom)/2));delta=p->copy.y<top?3:-3;
            }
            test_plot_wheel(a,x,y,delta);return;
        }break;
    }
    case 6:
        ok=fabsf(p->copy.w-copy_width)<.05f && fabsf(p->copy.h-height)<.05f;
        if(strstr(mode,"nosource"))ok &= p->source.w==0 && fabsf(p->copy.x-p->panes[2].x-20*f)<.05f;
        else {
            ok &= fabsf(p->source.w-source_width)<.05f && fabsf(p->source.h-height)<.05f && fabsf(p->source.x-p->panes[2].x-20*f)<.05f;
            if(fabsf(p->copy.y-p->source.y)<.05f)ok &= fabsf(p->copy.x-p->source.x-p->source.w-10*f)<.05f;
            else ok &= fabsf(p->copy.x-p->source.x)<.05f && fabsf(p->copy.y-p->source.y-height-10*f)<.05f;
        }
        *capture="protocol-actions.png";break;
    case 7:test_mouse(a,p->copy,true);break;
    case 8:test_mouse(a,p->copy,false);break;
    case 9:{char *text=SDL_GetClipboardText();ok=protocol_actions_clipboard_matches(p->original,text);fprintf(stderr,"PROTOCOL ACTION COPY: host=%d feedback=%s clipboard-bytes=%zu original-bytes=%zu\n",SDL_GetKeyboardFocus()==a->window,p->feedback,text?strlen(text):0,p->original?strlen(p->original):0);SDL_free(text);a->quitting=true;break;}
    }
    uint32_t crc=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
    ok &= crc==protocol_test_source_crc;
    float arrow_x=p->source.x+12.2f*f+font.width(font.userdata,font.height,"Quellstelle ansehen ",(int)strlen("Quellstelle ansehen "));
    float arrow_w=font.width(font.userdata,font.height,"→",3);
    int window_w,window_h;SDL_GetWindowSize(a->window,&window_w,&window_h);
    printf("{\"kind\":\"protocol_actions\",\"stage\":%d,\"passed\":%s,\"source_crc\":%u,\"scale\":%.4f,\"pane\":[%.4f,%.4f,%.4f,%.4f],\"source\":[%.4f,%.4f,%.4f,%.4f],\"copy\":[%.4f,%.4f,%.4f,%.4f],\"expected_widths\":[%.4f,%.4f],\"expected_font\":%.4f,\"arrow\":[%.4f,%.4f,%.4f,%.4f],\"window\":[%d,%d]}\n",
        *stage,ok?"true":"false",(unsigned)crc,f,p->panes[2].x,p->panes[2].y,p->panes[2].w,p->panes[2].h,
        p->source.x,p->source.y,p->source.w,p->source.h,p->copy.x,p->copy.y,p->copy.w,p->copy.h,source_width,copy_width,font.height,
        arrow_x,p->source.y+(height-font.height)/2,arrow_w,font.height,window_w,window_h);fflush(stdout);
    if(!ok){*code=1;a->quitting=true;*capture="protocol-actions-failure.png";fprintf(stderr,"Protocol action design stage %d failed\n",*stage);}
    if(a->quitting)printf("PROTOCOL ACTIONS %s SELF-TEST: %s\n",mode,*code?"FAILED":"PASSED");
    (*stage)++;
}
