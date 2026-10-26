/* Deliberate UI fixtures; never injected by the product's startup path. */
static uint32_t protocol_test_source_crc;
static char protocol_test_root[4096];
static char protocol_wrap_test_text[12289];
_Static_assert(sizeof protocol_wrap_test_text==sizeof(((ps_protocol_ui *)0)->original_page),"Test the complete original page capacity");
static bool protocol_ui_test_start(app *a,const char *root,const char *mode) {
    snprintf(protocol_test_root,sizeof protocol_test_root,"%s",root);
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    join(a->project_input,sizeof a->project_input,root,"project");a->example=0;new_project(a);if(!a->loaded)return false;
    ps_preferences preferences=a->preferences;preferences.theme=strstr(mode,"light")?PS_THEME_LIGHT:strstr(mode,"contrast")?PS_THEME_HIGH_CONTRAST:PS_THEME_DARK;
    preferences.ui_size=strstr(mode,"large")?22:16;preferences_apply(a,&preferences);
    SDL_SetWindowSize(a->window,strstr(mode,"narrow")?1080:strstr(mode,"boundary")?1150:1440,strstr(mode,"narrow")?740:1000);
    ps_protocol_clear(&a->protocol);a->protocol_view=(ps_protocol_ui){0};
    if(!strstr(mode,"empty")) {
        const struct {ps_log_level level;ps_protocol_origin origin;const char *title,*raw;} rows[]={
            {PS_LOG_ERROR,PS_PROTOCOL_BUILD,"Prüfmeldung: „gravity_m_s“ ist nicht definiert","main.c:13:14: error: 'gravity_m_s' undeclared\n  dy[1] = -gravity_m_s / length_m * sin(y[0]);\n           ^~~~~~~~~~~"},
            {PS_LOG_WARNING,PS_PROTOCOL_BUILD,"Prüfmeldung: Variable „energy“ wird nicht verwendet","main.c:24:12: warning: unused variable 'energy'"},
            {PS_LOG_ERROR,PS_PROTOCOL_SIMULATION,"Prüfmeldung: Modellwert α","Original α\nZeile zwei\t☃"},
            {PS_LOG_WARNING,PS_PROTOCOL_SIMULATION,"Prüfmeldung: Modellannahme prüfen","Eine Warnung allein stoppt keinen Lauf."},
            {PS_LOG_INFO,PS_PROTOCOL_APP,"Prüfmeldung: Sitzung bereit","Native UI fixture; no measured data."},
            {PS_LOG_DEBUG,PS_PROTOCOL_ANALYSIS,"Prüfmeldung: Mehrzeiliger Originaltext","Reset „Prüfung“\nSeed: 42\tdt: 0.01 s"}
        };
        if(strstr(mode,"wrap")) {
            const char suffix[]="\r\n\tOriginal α e\xcc\x81 🌍\r\nHIER IST DAS ECHTE ENDE!";
            size_t prefix=sizeof protocol_wrap_test_text-sizeof suffix;
            for(size_t i=0;i<prefix;i++)protocol_wrap_test_text[i]=(char)('A'+i%26);
            memcpy(protocol_wrap_test_text+prefix,suffix,sizeof suffix);
        }
        for(unsigned i=0;i<6;i++) {
            ps_protocol_message m={.level=rows[i].level,.origin=rows[i].origin,.title=rows[i].title,.raw=rows[i].raw,.raw_size=strlen(rows[i].raw),
                .context=a->project,.project=a->project,.source=i==0?"main.c":"",.line=i==0?13:0,.column=i==0?14:0,
                .has_time=i==2 || i==3,.time_s=2.4};
            if(!i && strstr(mode,"wrap")){m.raw=protocol_wrap_test_text;m.raw_size=strlen(protocol_wrap_test_text);}
            if(ps_protocol_push(&a->protocol,&m,NULL)!=PS_OK)return false;
        }
    }
    a->project_manager=false;a->tab=0;a->show_log=true;a->preferences.log_height=340;
    protocol_test_source_crc=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));return true;
}
static void protocol_ui_interactive_frame(app *a) {
    static uint32_t height;static bool initialized,resizing;ps_protocol_ui *p=&a->protocol_view;
    if(!initialized || height!=a->preferences.log_height || resizing!=p->resize) {
        height=a->preferences.log_height;resizing=p->resize;initialized=true;
        printf("{\"kind\":\"protocol_manual_state\",\"height\":%u,\"resizing\":%s,\"keyboard_focus\":%s,\"separator\":[%.2f,%.2f,%.2f,%.2f]}\n",
               height,resizing?"true":"false",SDL_GetKeyboardFocus()==a->window?"true":"false",
               p->separator.x,p->separator.y,p->separator.w,p->separator.h);fflush(stdout);
    }
}
#include "protocol_original_tests.inc"
static void protocol_ui_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    if(strstr(mode,"wrap")){protocol_original_test_frame(a,mode,stage,code,capture);return;}
    if(*stage<3){(*stage)++;return;}
    /* Inputs and filter derivation are committed in successive full renders. */
    static unsigned rendered;if(*stage>3 && rendered++<2)return;rendered=0;
    ps_protocol_ui *p=&a->protocol_view;int w,h;SDL_GetWindowSize(a->window,&w,&h);
    static unsigned scroll_attempts,resize_focus_setup;static uint64_t chosen,focus_wait_started;bool ok=true;
    switch(*stage) {
    case 3:
        ok=p->bounds.w==w && fabsf(p->bounds.y+p->bounds.h-(h-WORKSPACE_STATUS_HEIGHT))<1 && p->bounds.h==340 &&
            a->dock_visible[PS_DOCK_LOG] && p->panes[0].w>0 && p->panes[1].w>0 && p->panes[2].w>0 && p->row_count==(strstr(mode,"empty")?0:6);
        if(w>1150*a->preferences.ui_size/16.f)ok &= p->panes[2].x>p->panes[1].x && fabsf(p->panes[2].y-p->panes[1].y)<1;
        else ok &= p->panes[2].y>p->panes[1].y;
        ok &= p->levels[0].w<p->levels[1].w && p->levels[1].w<p->levels[2].w;
        for(unsigned i=0;i<5;i++)ok &= p->levels[i].x>=p->bounds.x && p->levels[i].x+p->levels[i].w<=p->bounds.x+p->bounds.w;
        *capture="protocol-navigator.png";
        if(strcmp(mode,"protocol-ui-dark"))a->quitting=true;
        break;
    case 4:test_mouse(a,p->levels[1],true);break;
    case 5:test_mouse(a,p->levels[1],false);break;
    case 6:ok=p->level==PS_LOG_ERROR && p->row_count==2;*capture="protocol-errors.bmp";test_mouse(a,p->search,true);break;
    case 7:test_mouse(a,p->search,false);break;
    case 8:layout_test_text(a,"Α");break;
    case 9:ok=!strcmp(p->query,"Α") && p->row_count==1;*capture="protocol-search.bmp";break;
    case 10:test_mouse(a,p->rows[0],true);break;
    case 11:test_mouse(a,p->rows[0],false);break;
    case 12:chosen=p->row_ids[0];ok=p->selected==chosen && p->original && strstr(p->original,"Original α\nZeile zwei\t☃");test_mouse(a,p->search,true);break;
    case 13:test_mouse(a,p->search,false);break;
    case 14:test_window_key_mod(a->window,SDLK_A,PS_UI_COMMAND_MOD);break;
    case 15:test_window_key_mod(a->window,SDLK_BACKSPACE,0);break;
    case 16:ok=!p->query[0] && p->row_count==2;test_mouse(a,p->origins[2],true);break;
    case 17:test_mouse(a,p->origins[2],false);break;
    case 18:ok=p->origin==PS_PROTOCOL_SIMULATION && p->row_count==1 && p->selected==chosen;test_window_key_mod(a->window,SDLK_2,PS_UI_COMMAND_MOD);break;
    case 19:ok=a->tab==1 && a->show_log && p->origin==PS_PROTOCOL_SIMULATION && p->level==PS_LOG_ERROR && p->selected==chosen && a->preferences.log_height==340;*capture="protocol-simulate.bmp";test_window_key_mod(a->window,SDLK_3,PS_UI_COMMAND_MOD);break;
    case 20:ok=a->tab==2 && a->show_log && p->selected==chosen && a->preferences.log_height==340;*capture="protocol-analyze.bmp";test_window_key_mod(a->window,SDLK_1,PS_UI_COMMAND_MOD);break;
    case 21:ok=a->tab==0 && a->show_log;test_mouse(a,p->levels[0],true);break;
    case 22:test_mouse(a,p->levels[0],false);break;
    case 23:test_mouse(a,p->origins[0],true);break;
    case 24:test_mouse(a,p->origins[0],false);break;
    case 25:ok=p->row_count==6 && p->selected==chosen && p->row_ids[0]==ps_protocol_at(&a->protocol,0)->id;test_mouse(a,p->rows[0],true);break;
    case 26:test_mouse(a,p->rows[0],false);break;
    case 27:
        if(p->source.y<p->panes[2].y || p->source.y+p->source.h>p->panes[2].y+p->panes[2].h) {
            if(scroll_attempts++>30){ok=false;break;}test_plot_wheel(a,p->panes[2].x+p->panes[2].w/2,p->panes[2].y+p->panes[2].h/2,-3);return;
        }
        scroll_attempts=0;ok=p->selected==ps_protocol_at(&a->protocol,0)->id;
        a->experiment.select_start=a->experiment.select_end=a->experiment.cursor=0;test_mouse(a,p->source,true);break;
    case 28:test_mouse(a,p->source,false);break;
    case 29:{
        nk_rune rune;int length;const char *start=nk_str_at_const(&a->experiment.string,a->experiment.select_start,&rune,&length);
        unsigned line=1;for(const char *s=nk_str_get_const(&a->experiment.string);start && s<start;s++)if(*s=='\n')line++;
        ok=a->tab==0 && start && line==13 && a->experiment.select_end>a->experiment.select_start;*capture="protocol-source.bmp";break;
    }
    case 30:test_mouse(a,p->levels[1],true);break;
    case 31:test_mouse(a,p->levels[1],false);break;
    case 32:join(p->export_test_path,sizeof p->export_test_path,protocol_test_root,"filtered.txt");test_mouse(a,p->store,true);break;
    case 33:test_mouse(a,p->store,false);break;
    case 34: {
        if(p->save)return;size_t size=0;char *text=SDL_LoadFile(p->export_test_path,&size);
        ok=text && size && strstr(text,"gravity_m_s") && strstr(text,"Modellwert α") && !strstr(text,"energy") && !strstr(text,"Modellannahme prüfen");SDL_free(text);
        *capture="protocol-export.bmp";break;
    }
    case 35:join(p->export_test_path,sizeof p->export_test_path,protocol_test_root,"missing/protocol.txt");test_mouse(a,p->store,true);break;
    case 36:test_mouse(a,p->store,false);break;
    case 37:if(p->save)return;ok=strstr(p->feedback,"fehlgeschlagen")!=NULL;break;
    case 38:
        if(!SDL_getenv("PHYSIM_TEST_PROTOCOL_UNFOCUSED")) {
            /* Real search activation must relinquish ownership to the range,
             * including after its first pending-focus frame. */
            if(resize_focus_setup==0){test_mouse(a,p->search,true);resize_focus_setup++;return;}
            if(resize_focus_setup==1){test_mouse(a,p->search,false);resize_focus_setup++;return;}
            ok=nk_sdl_accessibility_is_focused(a->ui,NULL,"Meldung oder Quelle");
            fprintf(stderr,"PROTOCOL RANGE FOCUS ORIGIN: search=%d\n",ok);
        }
        if(SDL_getenv("PHYSIM_TEST_PROTOCOL_UNFOCUSED"))SDL_HideWindow(a->window);
        focus_wait_started=SDL_GetTicks();ok &= nk_sdl_accessibility_request_focus(a->ui,NULL,"Protokoll-Höhe");break;
    case 39:
        if(SDL_getenv("PHYSIM_TEST_PROTOCOL_UNFOCUSED"))ok=SDL_GetKeyboardFocus()!=a->window;
        else if(SDL_GetKeyboardFocus()!=a->window || !nk_sdl_accessibility_is_focused(a->ui,NULL,"Protokoll-Höhe")) {
            if(SDL_GetTicks()-focus_wait_started<30000){SDL_RaiseWindow(a->window);return;}
            fprintf(stderr,"PROTOCOL KEY PRECONDITION: actual=%d separator=%d\n",SDL_GetKeyboardFocus()==a->window,nk_sdl_accessibility_is_focused(a->ui,NULL,"Protokoll-Höhe"));ok=false;break;
        }
        test_window_key_mod(a->window,SDLK_UP,SDL_KMOD_SHIFT);break;
    case 40:
        if(SDL_getenv("PHYSIM_TEST_PROTOCOL_UNFOCUSED")){ok=a->preferences.log_height==340 && SDL_GetKeyboardFocus()!=a->window;a->quitting=true;break;}
        ok=a->preferences.log_height==380;test_window_key_mod(a->window,SDLK_HOME,0);break;
    case 41:ok=a->preferences.log_height==150;test_window_key_mod(a->window,SDLK_END,0);break;
    case 42:ok=a->preferences.log_height==500;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 43:ok=a->preferences.log_height==340;test_mouse(a,p->separator,true);break;
    case 44:test_plot_motion(a,p->separator.w/2,p->separator.y+p->separator.h/2-30);break;
    case 45:ok=a->preferences.log_height==370 && p->resize && nk_sdl_accessibility_is_focused(a->ui,NULL,"Protokoll-Höhe");test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    case 46:ok=a->preferences.log_height==340 && !p->resize;test_mouse(a,p->separator,false);*capture="protocol-resize-restored.bmp";break;
    case 47:test_mouse(a,p->close,true);break;
    case 48:test_mouse(a,p->close,false);break;
    case 49:ok=!a->show_log;test_window_key_mod(a->window,SDLK_L,PS_UI_COMMAND_MOD);break;
    case 50:ok=a->show_log && p->level==PS_LOG_ERROR && p->origin==-1 && !p->query[0] && a->preferences.log_height==340;*capture="protocol-retained.bmp";test_mouse(a,p->levels[0],true);break;
    case 51:test_mouse(a,p->levels[0],false);break;
    case 52:{ok=p->row_count==6;char name[2300];protocol_row_name(ps_protocol_at(&a->protocol,0),name);ok &= nk_sdl_accessibility_request_focus(a->ui,NULL,name);break;}
    case 53:test_window_key_mod(a->window,SDLK_DOWN,0);test_window_key_mod(a->window,SDLK_DOWN,0);break;
    case 54:ok=p->selected==chosen;test_window_key_mod(a->window,SDLK_END,0);break;
    case 55:ok=p->selected==p->row_ids[5] && p->rows[5].y>=p->panes[1].y && p->rows[5].y+p->rows[5].h<=p->panes[1].y+p->panes[1].h;test_window_key_mod(a->window,SDLK_UP,0);test_window_key_mod(a->window,SDLK_UP,0);break;
    case 56:{const ps_protocol_record *r=ps_protocol_find(&a->protocol,p->selected);ok=r && strstr(r->message.title,"Modellannahme");test_window_key_mod(a->window,SDLK_HOME,0);break;}
    case 57:ok=p->selected==p->row_ids[0];*capture="protocol-keyboard.bmp";break;
    case 58:test_mouse(a,a->panels[PS_DOCK_LOG].action,true);break;
    case 59:test_plot_motion(a,(float)w+40,p->bounds.y+24);break;
    case 60:test_mouse(a,nk_rect((float)w+40,p->bounds.y+24,1,1),false);break;
    case 61:
        ok=a->panels[PS_DOCK_LOG].detached && a->show_log && a->panels[PS_DOCK_LOG].window;
        if(ok){int pw,ph;SDL_GetWindowSize(a->panels[PS_DOCK_LOG].window,&pw,&ph);ok=pw==w && ph==340 && p->panes[2].x>p->panes[1].x && fabsf(p->panes[2].y-p->panes[1].y)<1;}
        if(ok)join(a->panels[PS_DOCK_LOG].capture,sizeof a->panels[PS_DOCK_LOG].capture,protocol_test_root,"protocol-detached.png");
        *capture="protocol-detached-placeholder.png";break;
    case 62:{SDL_Window *window=a->window;a->window=a->panels[PS_DOCK_LOG].window;test_mouse(a,p->close,true);a->window=window;break;}
    case 63:{SDL_Window *window=a->window;a->window=a->panels[PS_DOCK_LOG].window;test_mouse(a,p->close,false);a->window=window;break;}
    case 64:ok=!a->panels[PS_DOCK_LOG].detached && a->show_log && p->bounds.w==w && p->bounds.h==340 && a->preferences.log_height==340;*capture="protocol-returned.bmp";break;
    case 65:SDL_SetWindowSize(a->panels[PS_DOCK_LOG].window,700,500);test_mouse(a,a->panels[PS_DOCK_LOG].action,true);break;
    case 66:test_plot_motion(a,(float)w+40,p->bounds.y+24);break;
    case 67:test_mouse(a,nk_rect((float)w+40,p->bounds.y+24,1,1),false);break;
    case 68:{int pw=0,ph=0;SDL_GetWindowSize(a->panels[PS_DOCK_LOG].window,&pw,&ph);ok=a->panels[PS_DOCK_LOG].detached && pw==w && ph==340;break;}
    case 69:{SDL_Window *window=a->window;a->window=a->panels[PS_DOCK_LOG].window;test_mouse(a,p->close,true);a->window=window;break;}
    case 70:{SDL_Window *window=a->window;a->window=a->panels[PS_DOCK_LOG].window;test_mouse(a,p->close,false);a->window=window;break;}
    case 71:break;
    case 72:ok=!a->panels[PS_DOCK_LOG].detached && a->show_log && p->bounds.w==w && p->bounds.h==340; a->quitting=true;break;
    }
    uint32_t source_crc=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
    ok &= protocol_test_source_crc==source_crc;
    if(*stage==3 || *stage==61 || *stage==68) {
        int pw=0,ph=0;if(a->panels[PS_DOCK_LOG].window)SDL_GetWindowSize(a->panels[PS_DOCK_LOG].window,&pw,&ph);
        printf("{\"kind\":\"protocol_ui_geometry\",\"stage\":%d,\"popup\":[%d,%d],\"source\":[%.4f,%.4f],\"filters\":[",*stage,pw,ph,a->panels[PS_DOCK_LOG].home.w,a->panels[PS_DOCK_LOG].home.h);
        for(unsigned i=0;i<5;i++){struct nk_rect r=p->levels[i];printf("%s[%.4f,%.4f,%.4f,%.4f]",i?",":"",r.x,r.y,r.w,r.h);}
        printf("],\"panes\":[");for(unsigned i=0;i<3;i++){struct nk_rect r=p->panes[i];printf("%s[%.4f,%.4f,%.4f,%.4f]",i?",":"",r.x,r.y,r.w,r.h);}puts("]}");
    }
    /* State evidence replaces repeated full-window images of input actions. */
    printf("{\"kind\":\"protocol_ui_step\",\"stage\":%d,\"passed\":%s,\"rows\":%zu,\"level\":%d,\"origin\":%d,\"query_bytes\":%zu,\"selected\":%llu,\"height\":%u,\"visible\":%s,\"detached\":%s,\"source_crc\":%u,\"selection_start\":%d,\"selection_end\":%d}\n",
        *stage,ok?"true":"false",p->row_count,p->level,p->origin,strlen(p->query),(unsigned long long)p->selected,
        a->preferences.log_height,a->show_log?"true":"false",a->panels[PS_DOCK_LOG].detached?"true":"false",(unsigned)source_crc,a->experiment.select_start,a->experiment.select_end);
    if(!SDL_getenv("PHYSIM_TEST_ALL_CAPTURES") && *stage!=3 && *stage!=61)*capture=NULL;
    if(!ok)*capture="protocol-failure.png";
    if(!ok){fprintf(stderr,"Protocol UI stage %d: rows=%zu level=%d origin=%d query=%s height=%u selected=%llu resize=%d feedback=%s\n",*stage,p->row_count,p->level,p->origin,p->query,a->preferences.log_height,(unsigned long long)p->selected,p->resize,p->feedback);*code=1;a->quitting=true;}
    if(a->quitting)printf("PROTOCOL NAVIGATOR %s SELF-TEST: %s\n",mode,*code?"FAILED":"PASSED");
    (*stage)++;
}
