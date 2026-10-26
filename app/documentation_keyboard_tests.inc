#include "accessibility.h"
/* SDL 3.2.30 writes CF_UNICODETEXT with CRLF under Windows. Only that
 * representation change is equivalent; indentation and every other byte matter. */
static bool documentation_test_same_text(const char *actual,const char *expected) {
    if(!actual || !expected)return false;
    while(*actual || *expected) {
        if(actual[0]=='\r' && actual[1]=='\n')++actual;
        if(expected[0]=='\r' && expected[1]=='\n')++expected;
        if(*actual!=*expected)return false;
        if(*actual){++actual;++expected;}
    }
    return true;
}
typedef struct {size_t live;bool fail;size_t requests,fail_after;} documentation_font_failure;
static void *documentation_font_allocate(nk_handle user,void *old,nk_size size) {
 (void)old;documentation_font_failure *state=user.ptr;
 ++state->requests;if(state->fail || (state->fail_after && state->requests>=state->fail_after))return NULL;
 void *memory=malloc(size);if(memory)++state->live;return memory;
}
static void documentation_font_release(nk_handle user,void *memory) {
 if(memory){documentation_font_failure *state=user.ptr;--state->live;free(memory);}
}
static bool documentation_test_clipboard(const char *expected) {
    char *actual=SDL_GetClipboardText();
    bool equal=documentation_test_same_text(actual,expected);
    SDL_free(actual);return equal;
}
/* Traverse actual SDL focus events; no pointer input or direct focus assignment. */
static bool documentation_test_seek(app *a,int id,int *stage,int *code) {
    static unsigned seeks,attempts,tabs,panes;static double started;
    static int target,last_topic=-1,last_stage=-1;
    bool action=id==DOC_BACK || id==DOC_FORWARD || id==DOC_HOME || id==DOC_WORKSPACE ||
        id==DOC_ZOOM_OUT || id==DOC_ZOOM_RESET || id==DOC_ZOOM_IN;
    if(action && !a->doc_actions_open) {
        test_window_key_mod(a->doc_window,SDLK_F10,SDL_KMOD_SHIFT);--*stage;return false;
    }
    if(!action && a->doc_actions_open) {
        test_window_key_mod(a->doc_window,SDLK_ESCAPE,0);--*stage;return false;
    }
    if((id==DOC_SEARCH || id==DOC_PREVIOUS || id==DOC_NEXT) && !a->doc_find_open) {
        test_window_key_mod(a->doc_window,SDLK_F,PS_UI_COMMAND_MOD);--*stage;return false;
    }
    int entries[12*PS_DOC_MAX_BLOCKS+DOCUMENTATION_TOPIC_COUNT+16];
    unsigned count=documentation_focus_list(a,entries);
    if(target!=id || last_topic!=a->doc_topic || last_stage!=*stage) {
        attempts=tabs=panes=0;started=ps_clock();target=id;last_topic=a->doc_topic;last_stage=*stage;
    }
    bool present=false;for(unsigned i=0;i<count;i++)if(entries[i]==id)present=true;
    if(!present || ++attempts>4*count+32) {
        fprintf(stderr,"Documentation focus cannot be reached: stage=%d target=%d focus=%d reveal=%d topic=%d attempts=%u entries=%u\n",
                *stage-1,id,a->doc_focus,a->doc_focus_reveal,a->doc_topic,attempts,count);
        *code=1;a->quitting=true;return false;
    }
    if(SDL_getenv("PHYSIM_TEST_TRACE") && (++seeks%32)==1) {
        fprintf(stderr,"Documentation seek stage=%d target=%d focus=%d reveal=%d topic=%d scroll=%u entries=%u bounds=%.3f/%.3f/%.3f/%.3f\n",
            *stage-1,id,a->doc_focus,a->doc_focus_reveal,a->doc_topic,a->doc_scroll_y,count,
            a->doc_focus_bounds.x,a->doc_focus_bounds.y,a->doc_focus_bounds.w,a->doc_focus_bounds.h);
    }
    if(a->doc_keyboard && a->doc_focus==id) {
        if(a->doc_focus_reveal){--*stage;return false;}
        printf("DOCUMENTATION SEEK: target=%d tabs=%u panes=%u elapsed=%.3f\n",id,tabs,panes,ps_clock()-started);
        return true;
    }
    unsigned current=count,wanted=count;
    for(unsigned i=0;i<count;i++){if(entries[i]==a->doc_focus)current=i;if(entries[i]==id)wanted=i;}
    if((id==DOC_BODY || id>=DOC_COPY) && (!a->doc_keyboard ||
       (a->doc_focus!=DOC_BODY && (a->doc_focus<DOC_COPY || current>wanted)))) {
        ++panes;test_window_key_mod(a->doc_window,SDLK_F6,0);
    } else {++tabs;test_window_key_mod(a->doc_window,SDLK_TAB,0);}
    --*stage;return false;
}
static void documentation_api_test_frame(app *a,int *stage,int *code,const char **capture) {
    static int core,units,guide,type,function,unit_type,example;
    bool ok=true;
#define API_KEY(key) test_window_key_mod(a->doc_window,key,0)
    switch((*stage)++) {
    case 0:test_window_key_mod(a->window,SDLK_F1,0);break;
    case 1:
        core=units=guide=-1;
        for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++) {
            if(!strcmp(documentation_topics[i].path,"docs/reference/core.md"))core=i;
            if(!strcmp(documentation_topics[i].path,"docs/reference/units.md"))units=i;
            if(!strcmp(documentation_topics[i].path,"docs/units.md"))guide=i;
        }
        ok=a->doc_visible && core>=0 && units>=0 && guide>=0;SDL_SetWindowSize(a->doc_window,760,540);
        break; /* All reference groups are directly visible in H. */
    case 2:
        if(!documentation_test_seek(a,DOC_TOPIC+core,stage,code))return;
        API_KEY(SDLK_RETURN);break;
    case 3:
        ok=a->doc_topic==core;type=function=-1;
        for(size_t i=0;i<a->documentation->count;i++) {
            ps_doc_block *b=&a->documentation->blocks[i];
            if(b->kind==PS_DOC_HEADING && !strcmp(a->documentation->text+b->text,"ps_vec3"))type=(int)i;
            if(b->kind==PS_DOC_HEADING && !strcmp(a->documentation->text+b->text,"ps_vadd"))function=(int)i;
        }
        ok &= type>=0 && function>=0 && a->documentation->blocks[type].level==3;
        break; /* Direct article sections need no mode switch. */
    case 4:
        if(!documentation_test_seek(a,DOC_HEADING+type,stage,code))return;API_KEY(SDLK_RETURN);break;
    case 5:
        if(a->doc_jump_block>=0 || a->doc_scroll_pending){--*stage;return;}
        ok=a->doc_scroll_y>0;*capture="docs-api-type.bmp";break;
    case 6:
        if(!documentation_test_seek(a,DOC_HEADING+function,stage,code))return;API_KEY(SDLK_RETURN);break;
    case 7:
        if(a->doc_jump_block>=0 || a->doc_scroll_pending){--*stage;return;}
        ok=a->doc_scroll_y>0;*capture="docs-api-function.bmp";break;
    case 8:
        break;
    case 9:
        if(!documentation_test_seek(a,DOC_TOPIC+units,stage,code))return;API_KEY(SDLK_RETURN);break;
    case 10:
        ok=a->doc_topic==units;unit_type=example=-1;
        for(size_t i=0;i<a->documentation->count;i++) {
            ps_doc_block *b=&a->documentation->blocks[i];
            if(b->kind==PS_DOC_HEADING && !strcmp(a->documentation->text+b->text,"ps_quantity"))unit_type=(int)i;
            if(example<0 && b->kind==PS_DOC_LINK)example=(int)i;
        }
        ok &= unit_type>=0 && example>=0;
        break;
    case 11:
        if(!documentation_test_seek(a,DOC_HEADING+unit_type,stage,code))return;API_KEY(SDLK_RETURN);break;
    case 12:
        if(a->doc_jump_block>=0 || a->doc_scroll_pending){--*stage;return;}
        ok=a->doc_scroll_y>0;*capture="docs-api-unit.bmp";break;
    case 13:
        if(!documentation_test_seek(a,DOC_LINK+example,stage,code))return;API_KEY(SDLK_RETURN);break;
    case 14:
        ok=a->doc_topic==guide;example=-1;
        for(size_t i=0;i<a->documentation->count;i++)if(a->documentation->blocks[i].kind==PS_DOC_CODE){example=(int)i;break;}
        ok &= example>=0;
        if(!documentation_test_seek(a,DOC_COPY+example,stage,code))return;
        break;
    case 15:
        *capture="docs-api-example.bmp";SDL_SetClipboardText("sentinel");API_KEY(SDLK_SPACE);break;
    case 16:
        ok=documentation_test_clipboard(a->documentation->text+a->documentation->blocks[example].text);
        a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"API browser stage %d: topic=%d focus=%d\n",*stage-1,a->doc_topic,a->doc_focus);*code=1;a->quitting=true;}
    if(a->quitting)printf("API BROWSER SELF-TEST: %s\n",*code?"FAILED":"PASSED");
#undef API_KEY
}
static void documentation_media_test_frame(app *a,int *stage,int *code,const char **capture) {
    static int topic,block,guide,diagram;
    static ps_scene original;
    static double original_time;
    static float theta,omega,yaw;
    bool ok=true;
#define MEDIA_KEY(key) test_window_key_mod(a->doc_window,key,0)
    switch((*stage)++) {
    case 0: original=a->scene;original_time=a->simulation_time;test_window_key_mod(a->window,SDLK_F1,0);break;
    case 1:
        topic=guide=-1;
        for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++) {
            if(!strcmp(documentation_topics[i].path,"docs/spatial-explanations.md"))topic=i;
            if(!strcmp(documentation_topics[i].path,"docs/user-guide.md"))guide=i;
        }
        ok=a->doc_visible && topic>=0 && guide>=0;
        SDL_SetWindowSize(a->doc_window,1000,760);open_documentation(a,topic);break;
    case 2:
        block=-1;for(size_t i=0;i<a->documentation->count;i++)if(a->documentation->blocks[i].kind==PS_DOC_IMAGE && documentation_media_kind(a->documentation->text+a->documentation->blocks[i].target)==3)block=(int)i;
        ok=block>=0;if(!ok)break;
        if(!documentation_test_seek(a,DOC_VISUAL+block*16,stage,code))return;
        theta=a->doc_theta;MEDIA_KEY(SDLK_RIGHT);break;
    case 3:
        ok=fabsf(a->doc_theta-theta-.01f)<1e-6f;
        if(!documentation_test_seek(a,DOC_VISUAL+block*16+2,stage,code))return;
        omega=a->doc_omega;MEDIA_KEY(SDLK_RIGHT);break;
    case 4:
        ok=fabsf(a->doc_omega-omega-.05f)<1e-6f;
        if(!documentation_test_seek(a,DOC_VISUAL+block*16+6,stage,code))return;MEDIA_KEY(SDLK_RETURN);break;
    case 5:
        ok=a->doc_yaw==0 && a->doc_pitch==0;
        if(!documentation_test_seek(a,DOC_VISUAL+block*16+9,stage,code))return;MEDIA_KEY(SDLK_SPACE);break;
    case 6:
        ok=a->doc_orthographic;
        if(!documentation_test_seek(a,DOC_VISUAL+block*16+10,stage,code))return;
        yaw=a->doc_yaw;MEDIA_KEY(SDLK_RIGHT);break;
    case 7: {
        ok=fabsf(a->doc_yaw-yaw-.08f)<1e-6f;
        ps_scene_render_stats stats;
        ok=ok && a->doc_visual_scene.count==8 && ps_scene_valid(&a->doc_visual_scene) &&
           ps_graphics_scene_stats(a->doc_graphics,&stats) && stats.vertices>0;
        ps_vec3 bob=a->doc_visual_scene.objects[1].a;
        ok=ok && fabs(bob.x-a->doc_length*sin(a->doc_theta))<1e-12 &&
           fabs(bob.y+a->doc_length*cos(a->doc_theta))<1e-12;
        *capture="docs-native-3d.bmp";break;
    }
    case 8:
        if(!documentation_test_seek(a,DOC_VISUAL+block*16+8,stage,code))return;MEDIA_KEY(SDLK_RETURN);break;
    case 9:
        ok=a->doc_pitch==(float)PS_PI/2 && a->doc_yaw==0;
        *capture="docs-native-top.bmp";
        if(!documentation_test_seek(a,DOC_VISUAL+block*16+10,stage,code))return;MEDIA_KEY(SDLK_HOME);break;
    case 10:
        ok=a->doc_yaw==.58f && a->doc_pitch==.23f && a->doc_zoom==5;
        open_documentation(a,guide);break;
    case 11:
        diagram=-1;for(size_t i=0;i<a->documentation->count;i++)if(a->documentation->blocks[i].kind==PS_DOC_IMAGE && documentation_media_kind(a->documentation->text+a->documentation->blocks[i].target)==1)diagram=(int)i;
        ok=diagram>=0;a->doc_jump_block=diagram;break;
    case 12:
        if(a->doc_jump_block>=0 || a->doc_scroll_pending){--*stage;return;}
        ok=a->doc_diagram_bounds.w>100 && a->doc_diagram_bounds.h==320;
        *capture="docs-native-workflow.bmp";break;
    case 13:
        ok=!memcmp(&a->scene,&original,sizeof original) && a->simulation_time==original_time;
        a->quitting=true;printf("DOCUMENTATION NATIVE MEDIA SELF-TEST: %s\n",*code || !ok?"FAILED":"PASSED");break;
    }
    if(!ok){fprintf(stderr,"Native documentation media stage %d failed\n",*stage-1);*code=1;a->quitting=true;}
#undef MEDIA_KEY
}
static int documentation_test_link(const ps_document *d,const char *target) {
    for(size_t i=0;i<d->count;i++)if(d->blocks[i].kind==PS_DOC_LINK && !strcmp(d->text+d->blocks[i].target,target))return (int)i;
    return -1;
}
static void documentation_anchor_test_frame(app *a,int *stage,int *code,const char **capture) {
    static int guide,workspace,collision,link,settle;static nk_uint scroll;static ps_document *previous;
    bool ok=true;
#define ANCHOR_KEY(key) test_window_key_mod(a->doc_window,key,0)
    switch((*stage)++) {
    case 0:test_window_key_mod(a->window,SDLK_F1,0);break;
    case 1:
        guide=workspace=collision=-1;
        for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++) {
            if(!strcmp(documentation_topics[i].path,"docs/user-guide.md"))guide=i;
            if(!strcmp(documentation_topics[i].path,"docs/workspace.md"))workspace=i;
            if(!strcmp(documentation_topics[i].path,"docs/collision-tutorial.md"))collision=i;
        }
        ok=guide>=0 && workspace>=0 && collision>=0;open_documentation(a,guide);
        SDL_SetWindowSize(a->doc_window,760,540);break;
    case 2:
        link=documentation_test_link(a->documentation,"workspace.md#fenster-hilfe-und-tastenkürzel");
        if(!documentation_test_seek(a,DOC_LINK+link,stage,code))return;
        ANCHOR_KEY(SDLK_RETURN);break;
    case 3:
        if(a->doc_jump_block>=0 || a->doc_scroll_pending){--*stage;return;}
        ok=a->doc_topic==workspace && a->doc_scroll_y>0 && !a->doc_error[0];
        {size_t b=0;ok &= ps_document_anchor(a->documentation,"fenster-hilfe-und-tastenkürzel",&b)==PS_OK;
         bool visible=a->doc_anchor_heading==(int)b && a->doc_anchor_bounds.y>=a->doc_body_bounds.y && a->doc_anchor_bounds.y<a->doc_body_bounds.y+32;
         if(!visible && settle++<4){--*stage;return;}ok &= visible;}
        *capture="docs-anchor-cross-page.bmp";break;
    case 4:
        scroll=a->doc_scroll_y;previous=a->documentation;
        documentation_link(a,"workspace.md#missing-section");break;
    case 5:
        ok=a->documentation==previous && a->doc_scroll_y==scroll && a->doc_error[0];
        documentation_link(a,"collision-tutorial.md#missing-section");break;
    case 6:
        ok=a->documentation==previous && a->doc_topic==workspace && a->doc_scroll_y==scroll;
        documentation_link(a,"https://example.org/source#part");
        {char *copy=SDL_GetClipboardText();ok &= copy && !strcmp(copy,"https://example.org/source#part");SDL_free(copy);}
        open_documentation(a,collision);break;
    case 7:
        link=documentation_test_link(a->documentation,"#erwartete-ergebnisse-und-grenzen");
        if(!documentation_test_seek(a,DOC_LINK+link,stage,code))return;ANCHOR_KEY(SDLK_RETURN);break;
    case 8:
        if(a->doc_jump_block>=0 || a->doc_scroll_pending){--*stage;return;}
        ok=a->doc_topic==collision && a->doc_scroll_y>0 && !a->doc_error[0];
        *capture="docs-anchor-same-page.bmp";documentation_link(a,"#");break;
    case 9:
        if(a->doc_jump_block>=0 || a->doc_scroll_pending){--*stage;return;}
        ok=a->doc_scroll_y==0;documentation_link(a,"./collision-tutorial.md#erwartete-ergebnisse-und-grenzen");break;
    case 10:
        if(a->doc_jump_block>=0 || a->doc_scroll_pending){--*stage;return;}
        ok=a->doc_scroll_y>0 && !a->doc_error[0];
        {int audit=-1;for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++)if(!strcmp(documentation_topics[i].path,"docs/project-audit.json"))audit=i;
         ok &= audit>=0;open_documentation(a,audit);}break;
    case 11:
        ok=!a->doc_error[0] && a->documentation->count==1 && a->documentation->blocks[0].kind==PS_DOC_CODE && a->documentation->blocks[0].length>PS_DOC_MAX_BYTES;
        *capture="docs-large-source.bmp";
        if(!documentation_test_seek(a,DOC_COPY,stage,code))return;ANCHOR_KEY(SDLK_RETURN);break;
    case 12:
        ok=documentation_test_clipboard(a->documentation->text+a->documentation->blocks[0].text);
        if(!documentation_test_seek(a,DOC_BODY,stage,code))return;ANCHOR_KEY(SDLK_PAGEDOWN);break;
    case 13:
        ok=a->doc_scroll_y>0 && !a->doc_error[0];*capture="docs-large-source-scrolled.bmp";a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Documentation anchor stage %d failed: topic=%d scroll=%u error=%s\n",*stage-1,a->doc_topic,a->doc_scroll_y,a->doc_error);*code=1;a->quitting=true;}
    if(a->quitting)printf("DOCUMENTATION ANCHOR SELF-TEST: %s\n",*code?"FAILED":"PASSED");
#undef ANCHOR_KEY
}
static void documentation_history_test_frame(app *a,int *stage,int *code,const char **capture) {
    static int guide,workspace,settings,header,link;static nk_uint guide_scroll,workspace_scroll;
    bool ok=true;
#define HISTORY_KEY(key) test_window_key_mod(a->doc_window,key,0)
#define HISTORY_BACK() test_window_key_mod(a->doc_window,SDLK_LEFT,SDL_KMOD_ALT)
#define HISTORY_FORWARD() test_window_key_mod(a->doc_window,SDLK_RIGHT,SDL_KMOD_ALT)
    switch((*stage)++) {
    case 0:test_window_key_mod(a->window,SDLK_F1,0);break;
    case 1:
        guide=workspace=settings=header=-1;
        for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++) {
            if(!strcmp(documentation_topics[i].path,"docs/user-guide.md"))guide=i;
            if(!strcmp(documentation_topics[i].path,"docs/workspace.md"))workspace=i;
            if(!strcmp(documentation_topics[i].path,"docs/settings.md"))settings=i;
            if(!strcmp(documentation_topics[i].path,"include/physim/core.h"))header=i;
        }
        ok=guide>=0 && workspace>=0 && settings>=0 && header>=0;open_documentation(a,guide);
        SDL_SetWindowSize(a->doc_window,760,540);break;
    case 2:
        link=documentation_test_link(a->documentation,"workspace.md");
        if(!documentation_test_seek(a,DOC_LINK+link,stage,code))return;
        guide_scroll=a->doc_scroll_y;ok=guide_scroll>0;HISTORY_KEY(SDLK_RETURN);break;
    case 3:
        ok=a->doc_topic==workspace;link=documentation_test_link(a->documentation,"settings.md#panelanordnung");
        if(!documentation_test_seek(a,DOC_LINK+link,stage,code))return;
        workspace_scroll=a->doc_scroll_y;ok &= workspace_scroll>0;HISTORY_KEY(SDLK_RETURN);break;
    case 4:ok=a->doc_topic==settings;HISTORY_BACK();break;
    case 5:
        if(a->doc_history_restore){--*stage;return;}
        ok=a->doc_topic==workspace && a->doc_scroll_y==workspace_scroll;HISTORY_BACK();break;
    case 6:
        if(a->doc_history_restore){--*stage;return;}
        ok=a->doc_topic==guide && a->doc_scroll_y==guide_scroll;
        *capture="docs-history-restored.bmp";HISTORY_FORWARD();break;
    case 7:
        if(a->doc_history_restore){--*stage;return;}
        ok=a->doc_topic==workspace && a->doc_scroll_y==workspace_scroll;HISTORY_FORWARD();break;
    case 8:
        if(a->doc_history_restore){--*stage;return;}
        ok=a->doc_topic==settings && !ps_doc_history_peek(&a->doc_history,1);HISTORY_BACK();break;
    case 9:
        if(a->doc_history_restore){--*stage;return;}
        if(!documentation_test_seek(a,DOC_HOME,stage,code))return;HISTORY_KEY(SDLK_RETURN);break;
    case 10:
        ok=a->doc_topic==DOCUMENTATION_C_GUIDE_TOPIC && !ps_doc_history_peek(&a->doc_history,1);break;
    case 11:if(!documentation_test_seek(a,DOC_FILTER,stage,code))return;break;
    case 12:
        ok=a->doc_filter_active;
        {SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->doc_window);e.text.text="ps_rng_seed";SDL_PushEvent(&e);}break;
    case 13:
        if(a->doc_ui->input.keyboard.text_len){--*stage;return;}
        ok=!strcmp(a->doc_filter,"ps_rng_seed") && documentation_hits[header];
        if(!documentation_test_seek(a,DOC_TOPIC+header,stage,code))return;HISTORY_KEY(SDLK_RETURN);break;
    case 14:
        ok=a->doc_topic==header && a->documentation->blocks[0].kind==PS_DOC_CODE &&
           !strcmp(a->doc_query,"ps_rng_seed") && !a->doc_error[0];
        *capture="docs-source-search.bmp";a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Documentation history stage %d failed: topic=%d scroll=%u\n",*stage-1,a->doc_topic,a->doc_scroll_y);*code=1;a->quitting=true;}
    if(a->quitting)printf("DOCUMENTATION HISTORY SELF-TEST: %s\n",*code?"FAILED":"PASSED");
#undef HISTORY_KEY
#undef HISTORY_BACK
#undef HISTORY_FORWARD
}
static void documentation_zoom_test_frame(app *a,int *stage,int *code,const char **capture) {
    static unsigned layout_frames;bool ok=true;
    switch((*stage)++) {
    case 0:test_window_key_mod(a->window,SDLK_F1,0);break;
    case 1:
        ok=SDL_SetWindowSize(a->doc_window,760,540) && SDL_SyncWindow(a->doc_window);layout_frames=0;
        for(int i=0;i<8;i++)test_window_key_mod(a->doc_window,SDLK_PLUS,PS_UI_COMMAND_MOD);
        break;
    case 2:
        if(layout_frames++<2){--*stage;return;}
        ok=documentation_font_size(a)==32 && documentation_code_size(a)==32 &&
           a->doc_font_ui->height==32 && a->doc_body_bounds.h>=120 && a->preferences.ui_size==16;
        {const nk_rune symbols[]={0x2192,0x221a,0x222b,0x2212,0x2080,0x2264};
         for(unsigned i=0;i<sizeof symbols/sizeof symbols[0];i++) {
             const struct nk_font_glyph *glyph=nk_font_find_glyph((struct nk_font*)a->doc_font_ui->userdata.ptr,symbols[i]);
             ok &= glyph && glyph->codepoint==symbols[i];
         }}
        *capture="docs-200-percent-small.bmp";
        test_window_key_mod(a->doc_window,SDLK_F,PS_UI_COMMAND_MOD);break;
    case 3:
        if(a->doc_focus_reveal){--*stage;return;}
        ok=a->doc_search_active;
        {SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->doc_window);e.text.text="Physim";SDL_PushEvent(&e);}break;
    case 4:
        if(a->doc_ui->input.keyboard.text_len){--*stage;return;}
        ok=!strcmp(a->doc_query,"Physim") && a->doc_body_bounds.h>=120;
        *capture="docs-200-percent-search.bmp";test_window_key_mod(a->doc_window,SDLK_0,PS_UI_COMMAND_MOD);break;
    case 5:
        ok=documentation_font_size(a)==16 && documentation_code_size(a)==16 && a->preferences.ui_size==16;
        *capture="docs-zoom-reset.bmp";break;
    case 6:
        {ps_preferences p=a->preferences;p.theme=PS_THEME_LIGHT;preferences_apply(a,&p);}
        for(int i=0;i<8;i++)test_window_key_mod(a->doc_window,SDLK_PLUS,PS_UI_COMMAND_MOD);
        break;
    case 7:
        ok=documentation_font_size(a)==32 && a->preferences.theme==PS_THEME_LIGHT && a->doc_body_bounds.h>=120;
        *capture="docs-200-percent-light.bmp";break;
    case 8:
        {ps_preferences p=a->preferences;p.theme=PS_THEME_HIGH_CONTRAST;preferences_apply(a,&p);}break;
    case 9:
        ok=documentation_font_size(a)==32 && a->preferences.theme==PS_THEME_HIGH_CONTRAST && a->doc_body_bounds.h>=120;
        *capture="docs-200-percent-contrast.bmp";break;
    case 10:
        {ps_preferences p=a->preferences;p.theme=PS_THEME_DARK;preferences_apply(a,&p);}
        ok=SDL_SetWindowSize(a->doc_window,1080,780) && SDL_SyncWindow(a->doc_window);layout_frames=0;break;
    case 11:
        if(layout_frames++<2){--*stage;return;}
        ok=documentation_font_size(a)==32 && a->doc_body_bounds.h>=300;
        *capture="docs-200-percent-large.bmp";test_window_key_mod(a->doc_window,SDLK_ESCAPE,0);break;
    case 12:ok=!a->doc_visible;test_window_key_mod(a->window,SDLK_F1,0);break;
    case 13:
        ok=a->doc_visible && documentation_font_size(a)==32;
        test_window_key_mod(a->doc_window,SDLK_0,PS_UI_COMMAND_MOD);break;
    case 14:ok=documentation_font_size(a)==16 && a->preferences.ui_size==16;a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Documentation zoom stage %d failed: size=%u body=%.1f\n",*stage-1,documentation_font_size(a),a->doc_body_bounds.h);*code=1;a->quitting=true;}
    if(a->quitting)printf("DOCUMENTATION ZOOM SELF-TEST: %s\n",*code?"FAILED":"PASSED");
}
static bool documentation_inline_fixture_variant(app *a,int topic,bool invalid_link) {
    const char *source=
        "# Lesen und Navigation\n\n"
        "Normaler Text mit **deutlicher Hervorhebung**, `ps_vec3` und einem [C-Lernweg](c-guide.md). "
        "Eine [Quelle mit einem langen beschreibenden Linktext, der auch bei doppelter Schrift und kleinem Fenster "
        "vollständig lesbar und mit der Tastatur erreichbar bleibt, ohne andere Textstellen als Link zu behandeln](https://example.org/physim#proof).\n\n"
        "## Bedeutung und Einheiten\n\n"
        "Die Formel `x = l * sin(θ)` erhält ihre Zeichen. Escaped \\* bleibt sichtbar; `**literal**` bleibt Code.\n\n"
        "| Begriff | Bedeutung |\n| --- | --- |\n"
        "| **Länge** | `l|r` in Metern; [C](c-guide.md) |\n\n"
        "```c\n// unverändert kopieren: θ\n  double x = length * sin(theta);\n```\n\n"
        "### Prüfung\n\nDie gespeicherten Werte bestätigen das Ergebnis.\n";
    ps_document *d=NULL;
    char *input=SDL_strdup(source);if(!input)return false;
    if(invalid_link){char *target=strstr(input,"(c-guide.md)");if(target)target[1]='x';else{SDL_free(input);return false;}}
    ps_result parsed=ps_document_parse(input,strlen(input),false,&d);SDL_free(input);if(parsed!=PS_OK)return false;
    free(a->documentation);a->documentation=d;a->doc_topic=topic;
    ++a->doc_reading_version;
    a->doc_query[0]=a->doc_last_query[0]=a->doc_error[0]=0;
    a->doc_jump_block=-1;a->doc_scroll_x=a->doc_scroll_y=0;
    a->doc_scroll_adjustment=0;a->doc_history_restore=0;
    a->doc_contents=true;a->doc_keyboard=false;a->doc_focus=DOC_BODY;
    return true;
}
static bool documentation_inline_fixture(app *a,int topic){return documentation_inline_fixture_variant(a,topic,false);}
static void documentation_inline_test_frame(app *a,int *stage,int *code,const char **capture) {
    static int guide,cguide,link,external,table_link,copy;static struct nk_rect clicked;
    static uint64_t first_cell,zoom_cell;
    static unsigned table_phase,table_frames;
    bool ok=true;
#define INLINE_KEY(key) test_window_key_mod(a->doc_window,key,0)
    switch((*stage)++) {
    case 0:test_window_key_mod(a->window,SDLK_F1,0);break;
    case 1:
        guide=cguide=-1;
        for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++) {
            if(!strcmp(documentation_topics[i].path,"docs/user-guide.md"))guide=i;
            if(!strcmp(documentation_topics[i].path,"docs/c-guide.md"))cguide=i;
        }
        ok=guide>=0 && cguide>=0 && documentation_inline_fixture(a,guide);
        table_phase=table_frames=0;
        SDL_SetWindowSize(a->doc_window,1080,780);break;
    case 2: {
        /* The same document/cell must survive real 16 -> 32 -> 16 text reflow.
         * Wait for completed publication instead of accepting the prior frame. */
        if(table_phase==1 || table_phase==2) {
            if(table_frames++<2 || a->doc_scroll_pending){--*stage;return;}
            ps_a11y_table table;ps_a11y_table_cell cell;
            ok=nk_sdl_accessibility_document_table_cell(a->doc_ui,0,1,1,&table,&cell) &&
                cell.id==first_cell && cell.version==a->doc_reading_version &&
                documentation_font_size(a)==(table_phase==1?32:16);
            if(!ok)break;
            if(table_phase==1) {
                test_window_key_mod(a->doc_window,SDLK_0,PS_UI_COMMAND_MOD);
                table_phase=2;table_frames=0;--*stage;return;
            }
            table_phase=3;
        }
        ps_document *d=a->documentation;link=external=table_link=copy=-1;
        const struct nk_user_font *h1=NULL,*h2=NULL,*h3=NULL;
        for(size_t i=0;i<d->count;i++) {
            ps_doc_block *b=&d->blocks[i];
            if(b->kind==PS_DOC_LINK) {
                if(!strcmp(d->text+b->text,"C-Lernweg"))link=(int)i;
                else if(!strncmp(d->text+b->target,"https://",8))external=(int)i;
                else table_link=(int)i;
                ok &= b->inline_parent<d->count && b->height==0;
            }
            if(b->kind==PS_DOC_CODE)copy=(int)i;
            if(b->kind==PS_DOC_HEADING){const struct nk_user_font *f=documentation_block_font(a,b);
                if(b->level==1)h1=f;else if(b->level==2)h2=f;else if(b->level==3)h3=f;}
        }
        ok &= link>=0 && external>=0 && table_link>=0 && copy>=0 && h1 && h2 && h3 && h1->height>h2->height && h2->height>h3->height;
        if(!ok)break;
        {
            const ps_doc_block *row=&d->blocks[d->blocks[table_link].inline_parent];
            const char *pipe=strstr(d->reading+row->reading_begin,"l|r");
            ok &= row->cell_count==2 && pipe &&
                nk_sdl_accessibility_document_visible(a->doc_ui,a->doc_reading_version,
                    (size_t)(pipe-d->reading)+1);
            ps_a11y_table table;ps_a11y_table_cell cell;
            bool published=nk_sdl_accessibility_document_table_cell(a->doc_ui,0,1,1,&table,&cell);
            ok &= published && table.rows==2 && table.columns==2 && table.column_headers &&
                cell.version==a->doc_reading_version && pipe && cell.begin<=(size_t)(pipe-d->reading) &&
                cell.end>=(size_t)(pipe-d->reading)+3 && cell.bounds[2]>0;
            first_cell=published?cell.id:0;
        }
        if(ok && !table_phase) {
            for(int i=0;i<8;i++)test_window_key_mod(a->doc_window,SDLK_PLUS,PS_UI_COMMAND_MOD);
            table_phase=1;table_frames=0;--*stage;return;
        }
        ps_doc_block *b=&d->blocks[link];clicked=nk_rect(b->hit[0],b->hit[1],b->hit[2],b->hit[3]);
        ok &= clicked.w>20 && clicked.w<150 && clicked.y>=a->doc_body_bounds.y;
        b=&d->blocks[copy];
        printf("DOCUMENTATION INLINE CODE INK: %.0f %.0f %.0f %.0f\n",b->hit[0]+15,b->hit[1]+18,b->hit[2]-30,b->hit[3]-36);
        *capture="docs-inline-16.bmp";test_window_mouse(a->doc_window,clicked,true);break;
    }
    case 3:test_window_mouse(a->doc_window,clicked,false);break;
    case 4:
        ok=a->doc_topic==cguide && !a->doc_error[0] && documentation_inline_fixture(a,guide);
        SDL_SetWindowSize(a->doc_window,760,540);
        {ps_preferences p=a->preferences;p.theme=PS_THEME_LIGHT;preferences_apply(a,&p);}
        for(int i=0;i<8;i++)test_window_key_mod(a->doc_window,SDLK_PLUS,PS_UI_COMMAND_MOD);break;
    case 5:
        ok=documentation_font_size(a)==32;
        {
            ps_a11y_table table;ps_a11y_table_cell cell;
            bool published=nk_sdl_accessibility_document_table_cell(a->doc_ui,0,0,1,&table,&cell);
            ok &= published && cell.column_header && cell.end-cell.begin==strlen("Bedeutung") &&
                !memcmp(a->documentation->reading+cell.begin,"Bedeutung",cell.end-cell.begin);
            published=nk_sdl_accessibility_document_table_cell(a->doc_ui,0,1,1,&table,&cell);
            ok &= published && cell.id!=first_cell && cell.version==a->doc_reading_version;
            zoom_cell=published?cell.id:0;
        }
        *capture="docs-inline-32-light.bmp";break;
    case 6:
        if(!documentation_test_seek(a,DOC_LINK+external,stage,code))return;
        printf("DOCUMENTATION INLINE FOCUS: outline=%.3f,%.3f viewport=%.3f,%.3f full-height=%.3f\n",
            a->doc_focus_bounds.y,a->doc_focus_bounds.h,a->doc_body_bounds.y,a->doc_body_bounds.h,a->documentation->blocks[external].hit[3]);
        ok=a->doc_focus_bounds.h<=a->doc_body_bounds.h && a->doc_focus_bounds.y>=a->doc_body_bounds.y &&
           a->documentation->blocks[external].hit[3]>a->doc_body_bounds.h;
        *capture="docs-inline-wrapped-focus.bmp";INLINE_KEY(SDLK_SPACE);break;
    case 7:
        ok=a->doc_topic==guide && documentation_test_clipboard("https://example.org/physim#proof");
        if(!documentation_test_seek(a,DOC_LINK+table_link,stage,code))return;
        {
            ps_a11y_table table;ps_a11y_table_cell cell;
            ok &= nk_sdl_accessibility_document_table_cell(a->doc_ui,0,1,1,&table,&cell) &&
                cell.id==zoom_cell && cell.bounds[2]>0;
        }
        *capture="docs-inline-table.bmp";INLINE_KEY(SDLK_RETURN);break;
    case 8:
        ok=a->doc_topic==cguide && documentation_inline_fixture(a,guide);
        {ps_preferences p=a->preferences;p.theme=PS_THEME_HIGH_CONTRAST;preferences_apply(a,&p);}
        test_window_key_mod(a->doc_window,SDLK_0,PS_UI_COMMAND_MOD);break;
    case 9:
        *capture="docs-inline-16-contrast.bmp";
        if(!documentation_test_seek(a,DOC_COPY+copy,stage,code))return;INLINE_KEY(SDLK_RETURN);break;
    case 10:
        ok=documentation_test_clipboard(a->documentation->text+a->documentation->blocks[copy].text);
        *capture="docs-inline-code.bmp";a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Documentation inline stage %d failed: focus=%d size=%u topic=%d\n",*stage-1,a->doc_focus,documentation_font_size(a),a->doc_topic);*code=1;a->quitting=true;}
    if(a->quitting)printf("DOCUMENTATION INLINE SELF-TEST: %s\n",*code?"FAILED":"PASSED");
#undef INLINE_KEY
}
static bool documentation_link_inventory(app *a) {
    bool trace=SDL_getenv("PHYSIM_TEST_TRACE")!=NULL;
    if(trace)fprintf(stderr,"APP TEST TRACE: link inventory begin %.6f\n",ps_clock());
    ps_a11y_model *model=malloc(sizeof *model);if(!model)return false;ps_a11y_init(model);
    size_t total=0,maximum=0;bool ok=true;float viewport[]={0,0,760,540},hidden[]={0,0,0,0};
    for(int i=0;ok && i<DOCUMENTATION_TOPIC_COUNT;i++) {
        if(trace && !(i%32))fprintf(stderr,"APP TEST TRACE: link inventory page %d %.6f\n",i,ps_clock());
        char path[4096];join(path,sizeof path,a->root,documentation_topics[i].path);size_t length=0;
        char *source=load_utf8_text_limit(path,&length,documentation_topics[i].code?PS_DOC_MAX_CODE_BYTES:PS_DOC_MAX_BYTES);
        ps_document *document=NULL;ok=source && ps_document_parse(source,length,documentation_topics[i].code,&document)==PS_OK;free(source);
        if(!ok){fprintf(stderr,"Link inventory could not parse %s\n",path);break;}
        ps_a11y_begin(model);ok=ps_a11y_document_begin(model,(uint64_t)i+1,documentation_topics[i].title,document->reading,viewport);
        size_t count=0;
        for(size_t j=0;ok && j<document->count;j++) {
            const ps_doc_block *b=&document->blocks[j];
            if(b->kind!=PS_DOC_LINK || !strncmp(document->text+b->target,"http",4) || !b->length)continue;
            ok=b->reading_begin!=SIZE_MAX && b->reading_length==b->length &&
                !memcmp(document->reading+b->reading_begin,document->text+b->text,b->length);
            size_t before=model->draft_count;
            if(ok)ps_a11y_document_link(model,"document",document->text+b->text,(uint64_t)i+1,
                b->reading_begin,b->reading_begin+b->reading_length,hidden,true);
            ok=ok && model->draft_count==before+1 && !strcmp(model->draft[before].label,document->text+b->text);count++;
        }
        /* Reserve the toolbar/sidebar's bounded controls in the shared model. */
        ok=ok && count+64<=PS_A11Y_MAX_NODES && !model->dropped;
        total+=count;if(count>maximum)maximum=count;
        if(!ok)fprintf(stderr,"Link inventory failed for %s (%zu links)\n",path,count);
        ps_a11y_publish(model);free(document);
    }
    ps_a11y_destroy(model);free(model);
    if(trace)fprintf(stderr,"APP TEST TRACE: link inventory end %.6f\n",ps_clock());
    if(ok)printf("DOCUMENTATION LINK INVENTORY: %d pages, %zu links, %zu maximum per page\n",DOCUMENTATION_TOPIC_COUNT,total,maximum);
    return ok;
}
static bool documentation_math_fonts(app *a) {
 char path[4096];join(path,sizeof path,a->project,"required-glyphs.txt");FILE *input=fopen(path,"rb");if(!input)return false;
 unsigned value,count=0;bool ok=true;
 /* U+0378 is in the declared Greek range but absent from these platform fonts.
  * Raw atlas metadata must identify the missing source; lookup must skip .notdef. */
 const struct nk_font *first=a->doc_ui_fonts[0]->userdata.ptr;bool declared_missing=false;
 for(nk_rune i=0;i<first->info.glyph_count;i++)
  if(first->glyphs[i].codepoint==0x0378 && !first->glyphs[i].source_present)declared_missing=true;
 const struct nk_font_glyph *missing=nk_font_find_glyph(first,0x0378);
 if(!declared_missing || !missing || missing->codepoint==0x0378 || ps_ui_font_has_glyph(a->doc_ui_fonts[0],0x0378))ok=false;
 for(unsigned i=0;i<9;i++) {
  const struct nk_user_font *regular=a->doc_ui_fonts[i],*bold=a->doc_bold_fonts[i];
  float normal=regular->width(regular->userdata,regular->height,"AaBb",4);
  float strong=bold->width(bold->userdata,bold->height,"AaBb",4);
  if(fabsf(normal-strong)<.2f) {
   fprintf(stderr,"Documentation strong font indistinguishable at %.0f px\n",regular->height);ok=false;
  }
 }
 while(fscanf(input,"%x",&value)==1) {
  ++count;
  for(unsigned role=0;role<5;role++) {
   const struct nk_user_font **fonts=role==0?a->doc_ui_fonts:role==1?a->doc_title_fonts:role==2?a->doc_bold_fonts:role==3?a->doc_code_fonts:a->doc_chrome_fonts;
   for(unsigned i=0;i<(role==4?4u:role==2?11u:9u);i++) {
    const struct nk_font_glyph *glyph=nk_font_find_glyph(fonts[i]->userdata.ptr,(nk_rune)value);
    if(!ps_ui_font_has_glyph(fonts[i],(nk_rune)value) || !glyph ||
       (value==0x20 ? glyph->xadvance<=0 : (glyph->x1<=glyph->x0 || glyph->y1<=glyph->y0))) {
     fprintf(stderr,"Documentation math missing U+%04X role=%u size=%.0f\n",value,role,fonts[i]->height);ok=false;
    }
    if(role==3 && value>=0x2500 && value<=0x257f) {
     const struct nk_font_glyph *space=nk_font_find_glyph(fonts[i]->userdata.ptr,32);
     if(!glyph || !space || fabsf(glyph->xadvance-space->xadvance)>.05f) {
      fprintf(stderr,"Documentation box advance differs from code space U+%04X size=%.0f\n",value,fonts[i]->height);ok=false;
     }
    }
   }
  }
 }
 if(!feof(input) || count<1)ok=false;fclose(input);
 static const nk_rune figure_glyphs[]={'L','m',0x3b8};
 for(unsigned i=0;i<4;i++) {
  const struct nk_user_font *font=a->doc_figure_fonts[i];
  if(!font || font==a->doc_ui_fonts[i] || !ps_ui_font_has_glyph(font,'A') || ps_ui_font_has_glyph(font,0x378))ok=false;
  for(unsigned j=0;j<3;j++)if(!ps_ui_font_has_glyph(font,figure_glyphs[j])) {
   fprintf(stderr,"Documentation figure missing U+%04X variant=%u\n",figure_glyphs[j],i);ok=false;
  }
 }
 if(ok){printf("DOCUMENTATION MATH GLYPHS: %u characters in 42 font variants\n",count);
  puts("DOCUMENTATION FIGURE GLYPHS: 3 characters in 4 italic serif variants");}return ok;
}
static void documentation_math_test_frame(app *a,int *stage,int *code,const char **capture) {
 static unsigned zoom_frames;bool ok=true;switch((*stage)++) {
 case 0:a->doc_full_font_inventory=true;test_window_key_mod(a->window,SDLK_F1,0);break;
 case 1: {
  int topic=-1;for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++)if(!strcmp(documentation_topics[i].path,"docs/pendulum-tutorial.md"))topic=i;
  ok=topic>=0;open_documentation(a,topic);SDL_SetWindowSize(a->doc_window,760,540);break;
 }
 case 2:ok=documentation_link_inventory(a) && documentation_math_fonts(a);test_window_key_mod(a->doc_window,SDLK_F,PS_UI_COMMAND_MOD);break;
 case 3: {
  if(!a->doc_search_active){--*stage;return;}
  SDL_Event input={0};input.type=SDL_EVENT_TEXT_INPUT;input.text.windowID=SDL_GetWindowID(a->doc_window);input.text.text="eᵣ";SDL_PushEvent(&input);break;
 }
 case 4: {
  if(a->doc_ui->input.keyboard.text_len || a->doc_scroll_pending){--*stage;return;}
  const char *r=strstr(a->documentation->reading,"ᵣ");ok=!strcmp(a->doc_query,"eᵣ") && r &&
   nk_sdl_accessibility_document_visible(a->doc_ui,a->doc_reading_version,(size_t)(r-a->documentation->reading));
  *capture="docs-math-16.bmp";for(int i=0;i<8;i++)test_window_key_mod(a->doc_window,SDLK_PLUS,PS_UI_COMMAND_MOD);break;
 }
 case 5: {
  /* The snapshot checked here belongs to a completed 32 px layout, after input
   * and reflow have both been drawn. A 16 px snapshot accepted a wrong image. */
  if(zoom_frames++<2){--*stage;return;}
  if(a->doc_scroll_pending){--*stage;return;}
  const char *r=strstr(a->documentation->reading,"ᵣ");ok=documentation_font_size(a)==32 && r &&
   nk_sdl_accessibility_document_visible(a->doc_ui,a->doc_reading_version,(size_t)(r-a->documentation->reading));
  *capture="docs-math-32.bmp";test_window_key_mod(a->doc_window,SDLK_0,PS_UI_COMMAND_MOD);zoom_frames=0;break;
 }
 case 6:
  if(zoom_frames++<2){--*stage;return;}
  ok=documentation_font_size(a)==16;test_window_key_mod(a->doc_window,SDLK_A,PS_UI_COMMAND_MOD);break;
 case 7:test_window_key_mod(a->doc_window,SDLK_BACKSPACE,0);zoom_frames=0;break;
 case 8: {
  if(a->doc_query[0] || zoom_frames++<2){--*stage;return;}
  const char *r=strstr(a->documentation->reading,"ᵣ");ok=r && nk_sdl_accessibility_document_visible(a->doc_ui,a->doc_reading_version,(size_t)(r-a->documentation->reading));
  *capture="docs-reading-16.bmp";for(int i=0;i<8;i++)test_window_key_mod(a->doc_window,SDLK_PLUS,PS_UI_COMMAND_MOD);zoom_frames=0;break;
 }
 case 9: {
  if(zoom_frames++<2 || a->doc_scroll_pending){--*stage;return;}
  const char *r=strstr(a->documentation->reading,"ᵣ");ok=!a->doc_query[0] && documentation_font_size(a)==32 && r &&
   nk_sdl_accessibility_document_visible(a->doc_ui,a->doc_reading_version,(size_t)(r-a->documentation->reading));
  *capture="docs-reading-32.bmp";test_window_key_mod(a->doc_window,SDLK_0,PS_UI_COMMAND_MOD);zoom_frames=0;break;
 }
 case 10: {
  if(zoom_frames++<2 || a->doc_scroll_pending){--*stage;return;}
  const char *r=strstr(a->documentation->reading,"ᵣ");ok=!a->doc_query[0] && documentation_font_size(a)==16 && r &&
   nk_sdl_accessibility_document_visible(a->doc_ui,a->doc_reading_version,(size_t)(r-a->documentation->reading));
  *capture="docs-reading-reset.bmp";break;
 }
 case 11: {
  const char *source="# Zeichenprobe\n\nθ 🌍 😀 ☃ ᵀ eᵣ cᵥ uⱼ ∫\nAaBb **AaBb**\n\n```text\n┌──┬──┐\n│  │  │\n├──┼──┤\n└──┴──┘\n```\n";
  ps_document *document=NULL;ok=ps_document_parse(source,strlen(source),false,&document)==PS_OK;
  if(ok) {
   free(a->documentation);a->documentation=document;++a->doc_reading_version;
   a->doc_query[0]=a->doc_last_query[0]=0;a->doc_scroll_x=a->doc_scroll_y=0;
   a->doc_scroll_adjustment=0;a->doc_history_restore=0;a->doc_jump_block=-1;
   a->doc_focus=DOC_BODY;zoom_frames=0;
  }
  break;
 }
 case 12: {
  if(zoom_frames++<2 || a->doc_scroll_pending){--*stage;return;}
  const char *r=strstr(a->documentation->reading,"🌍");
  ok=documentation_font_size(a)==16 && r &&
   nk_sdl_accessibility_document_visible(a->doc_ui,a->doc_reading_version,(size_t)(r-a->documentation->reading));
  const char *v=strstr(a->documentation->reading,"ᵥ");
  ok=ok && v && nk_sdl_accessibility_document_visible(a->doc_ui,a->doc_reading_version,(size_t)(v-a->documentation->reading));
  const char *j=strstr(a->documentation->reading,"ⱼ");
  ok=ok && j && nk_sdl_accessibility_document_visible(a->doc_ui,a->doc_reading_version,(size_t)(j-a->documentation->reading));
  *capture="docs-unicode-16.bmp";for(int i=0;i<8;i++)test_window_key_mod(a->doc_window,SDLK_PLUS,PS_UI_COMMAND_MOD);
  zoom_frames=0;break;
 }
 case 13: {
  if(zoom_frames++<2 || a->doc_scroll_pending){--*stage;return;}
  const char *r=strstr(a->documentation->reading,"🌍");
  ok=documentation_font_size(a)==32 && r &&
   nk_sdl_accessibility_document_visible(a->doc_ui,a->doc_reading_version,(size_t)(r-a->documentation->reading));
  const char *v=strstr(a->documentation->reading,"ᵥ");
  ok=ok && v && nk_sdl_accessibility_document_visible(a->doc_ui,a->doc_reading_version,(size_t)(v-a->documentation->reading));
  const char *j=strstr(a->documentation->reading,"ⱼ");
  ok=ok && j && nk_sdl_accessibility_document_visible(a->doc_ui,a->doc_reading_version,(size_t)(j-a->documentation->reading));
  *capture="docs-unicode-32.bmp";a->doc_jump_block=2;zoom_frames=0;break;
 }
 case 14: {
  if(zoom_frames++<2 || a->doc_scroll_pending){--*stage;return;}
  if(!documentation_test_seek(a,DOC_BODY,stage,code))return;
  test_window_key_mod(a->doc_window,SDLK_END,0);zoom_frames=0;break;
 }
 case 15: {
  if(zoom_frames++<2 || a->doc_scroll_pending){--*stage;return;}
  const char *r=strstr(a->documentation->reading,"┴");
  ok=documentation_font_size(a)==32 && r &&
   nk_sdl_accessibility_document_visible(a->doc_ui,a->doc_reading_version,(size_t)(r-a->documentation->reading));
  *capture="docs-unicode-boxes-32.bmp";
  printf("DOCUMENTATION GRID: %.3f %.3f %.3f %.3f %.3f %.3f\n",a->doc_grid_probe[0],a->doc_grid_probe[1],a->doc_grid_probe[2],a->doc_grid_probe[3],a->doc_grid_probe[4],a->doc_grid_probe[5]);
  float overlap=a->doc_grid_probe[4]-a->doc_grid_probe[3];
  if(overlap<-.01f || overlap>1.01f) {
   fprintf(stderr,"Documentation grid row %.3f has invalid overlap with source vertical span %.3f\n",a->doc_grid_probe[3],a->doc_grid_probe[4]);ok=false;
  }
  if(a->doc_grid_probe[1]+3.5f*a->doc_grid_probe[3]>a->doc_grid_probe[5]-1) {
   fprintf(stderr,"Documentation grid closing stroke clipped\n");ok=false;
  }
  a->quitting=true;break;
 }
 }
 if(!ok){fprintf(stderr,"Documentation math stage %d failed\n",*stage-1);*code=1;a->quitting=true;}
 if(a->quitting)printf("DOCUMENTATION MATH SELF-TEST: %s\n",*code?"FAILED":"PASSED");
}
static void documentation_navigation_test_frame(app *a,int *stage,int *code,const char **capture) {
    static unsigned frames,tabs;static struct nk_rect before;
    static uint64_t version;static size_t anchor;
    static int experiment_length,analysis_length;static double simulation_time;
    bool ok=true;
#define NAV_KEY(key) test_window_key_mod(a->doc_window,key,0)
#define NAV_TOGGLE() test_window_key_mod(a->doc_window,SDLK_B,PS_UI_COMMAND_MOD|SDL_KMOD_SHIFT)
    switch((*stage)++) {
    case 0:
        experiment_length=a->experiment.string.len;analysis_length=a->analysis.string.len;simulation_time=a->simulation_time;
        test_window_key_mod(a->window,SDLK_F1,0);break;
    case 1:
        ok=documentation_inline_fixture(a,a->doc_topic);
        version=a->doc_reading_version;
        ok=ok && SDL_SetWindowSize(a->doc_window,760,540) && SDL_SyncWindow(a->doc_window);
        for(int i=0;i<8;i++)test_window_key_mod(a->doc_window,SDLK_PLUS,PS_UI_COMMAND_MOD);
        frames=0;break;
    case 2:
        if(frames++<2 || a->doc_scroll_pending){--*stage;return;}
        ok=documentation_font_size(a)==32 && !a->doc_navigation_hidden;
        before=a->doc_body_bounds;*capture="docs-navigation-before.bmp";break;
    case 3:
        if(!documentation_test_seek(a,DOC_BODY,stage,code))return;
        NAV_KEY(SDLK_PAGEDOWN);frames=0;break;
    case 4:
        if(frames++<2 || a->doc_scroll_pending){--*stage;return;}
        anchor=SIZE_MAX;
        for(size_t at=0;at<strlen(a->documentation->reading);) {
            if(nk_sdl_accessibility_document_visible(a->doc_ui,version,at)){anchor=at;break;}
            nk_rune rune;int n=nk_utf_decode(a->documentation->reading+at,&rune,(int)strlen(a->documentation->reading+at));if(n<=0)break;at+=(size_t)n;
        }
        ok=anchor!=SIZE_MAX && a->doc_scroll_y>0;NAV_TOGGLE();frames=0;break;
    case 5:
        if(frames++<2 || a->doc_scroll_pending){--*stage;return;}
        ok=a->doc_navigation_hidden && a->doc_body_bounds.w>=before.w+220 && a->doc_body_bounds.h>=before.h &&
           a->doc_reading_version==version && nk_sdl_accessibility_document_visible(a->doc_ui,version,anchor);
        printf("DOCUMENTATION NAVIGATION: before=%.1fx%.1f reader=%.1fx%.1f anchor=%zu\n",before.w,before.h,a->doc_body_bounds.w,a->doc_body_bounds.h,anchor);
        *capture="docs-navigation-reader.bmp";tabs=0;NAV_KEY(SDLK_TAB);break;
    case 6:
        ok=!(a->doc_focus>=DOC_TOPIC && a->doc_focus<DOC_COPY);
        if(a->doc_focus!=DOC_NAVIGATION && tabs++<64){NAV_KEY(SDLK_TAB);--*stage;break;}
        ok=ok && a->doc_focus==DOC_NAVIGATION && tabs>3;
        test_window_key_mod(a->doc_window,SDLK_F,PS_UI_COMMAND_MOD);break;
    case 7: {
        if(a->doc_focus_reveal){--*stage;return;}
        ok=a->doc_search_active;SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->doc_window);e.text.text="θ";SDL_PushEvent(&e);
        NAV_TOGGLE();frames=0;break;
    }
    case 8:
        if(frames++<2 || a->doc_scroll_pending){--*stage;return;}
        ok=!a->doc_navigation_hidden && !strcmp(a->doc_query,"θ") && !a->doc_search_active &&
           fabsf(a->doc_body_bounds.w-before.w)<1 && a->doc_reading_version==version;
        break; /* Topics and section links are already displayed together. */
    case 9:
        if(!documentation_test_seek(a,DOC_FILTER,stage,code))return;
        break;
    case 10: {
        ok=a->doc_filter_active;SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->doc_window);e.text.text="Pendel";SDL_PushEvent(&e);
        NAV_TOGGLE();frames=0;break;
    }
    case 11:
        if(frames++<2 || a->doc_scroll_pending){--*stage;return;}
        ok=a->doc_navigation_hidden && !strcmp(a->doc_filter,"Pendel") && !a->doc_filter_active && !a->doc_search_active;
        {SDL_Event e={0};e.type=SDL_EVENT_KEY_DOWN;e.key.windowID=SDL_GetWindowID(a->doc_window);e.key.key=SDLK_B;e.key.mod=PS_UI_COMMAND_MOD|SDL_KMOD_SHIFT;e.key.repeat=true;SDL_PushEvent(&e);}
        break;
    case 12:
        ok=a->doc_navigation_hidden;
#ifdef _WIN32
        ok=ok && nk_sdl_accessibility_press(a->doc_ui,"Themennavigation einblenden (geschlossen)");
#else
        if(!documentation_test_seek(a,DOC_NAVIGATION,stage,code))return;
        NAV_KEY(SDLK_SPACE);
#endif
        frames=0;break;
    case 13:
        if(frames++<2 || a->doc_scroll_pending){--*stage;return;}
        ok=!a->doc_navigation_hidden && !a->doc_filter_active && !strcmp(a->doc_filter,"Pendel");
        *capture="docs-navigation-restored.bmp";test_window_mouse(a->doc_window,a->doc_navigation_bounds,true);break;
    case 14:
        test_window_mouse(a->doc_window,a->doc_navigation_bounds,false);frames=0;break;
    case 15:
        if(frames++<2 || a->doc_scroll_pending){--*stage;return;}
        ok=a->doc_navigation_hidden;
        {ps_preferences p=a->preferences;p.theme=PS_THEME_LIGHT;preferences_apply(a,&p);}frames=0;break;
    case 16:
        if(frames++<2){--*stage;return;}
        *capture="docs-navigation-light.bmp";break;
    case 17:
        {ps_preferences p=a->preferences;p.theme=PS_THEME_HIGH_CONTRAST;preferences_apply(a,&p);}frames=0;break;
    case 18:
        if(frames++<2){--*stage;return;}
        *capture="docs-navigation-contrast.bmp";NAV_KEY(SDLK_ESCAPE);break;
    case 19:ok=!a->doc_visible;test_window_key_mod(a->window,SDLK_F1,0);frames=0;break;
    case 20:
        if(frames++<2){--*stage;return;}
        ok=a->doc_visible && a->doc_navigation_hidden && documentation_font_size(a)==32 &&
           a->doc_reading_version==version && !strcmp(a->doc_query,"θ") && !strcmp(a->doc_filter,"Pendel") &&
           a->experiment.string.len==experiment_length && a->analysis.string.len==analysis_length && a->simulation_time==simulation_time;
        a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Documentation navigation stage %d failed: hidden=%d focus=%d keyboard=%d native=%d body=%.1fx%.1f query=%s filter=%s\n",*stage-1,a->doc_navigation_hidden,a->doc_focus,a->doc_keyboard,nk_sdl_accessibility_has_focus(a->doc_ui),a->doc_body_bounds.w,a->doc_body_bounds.h,a->doc_query,a->doc_filter);*code=1;a->quitting=true;}
    if(a->quitting)printf("DOCUMENTATION NAVIGATION SELF-TEST: %s\n",*code?"FAILED":"PASSED");
#undef NAV_KEY
#undef NAV_TOGGLE
}
static unsigned documentation_font_variants(const app *a) {
    unsigned count=0;for(unsigned i=0;i<9;i++)count+=(a->doc_ui_fonts[i]!=NULL)+(a->doc_title_fonts[i]!=NULL)+(a->doc_code_fonts[i]!=NULL);
    for(unsigned i=0;i<11;i++)count+=a->doc_bold_fonts[i]!=NULL;
    for(unsigned i=0;i<4;i++)count+=(a->doc_chrome_fonts[i]!=NULL)+(a->doc_figure_fonts[i]!=NULL);return count;
}
static void documentation_font_demand_test(app *a,int *stage,int *code,const char **capture) {
    static const struct nk_user_font *original;static uint64_t version;
    static unsigned frames;
    static ps_preferences initial;static int editor;static double simulation;
    bool ok=true;switch((*stage)++) {
    case 0:
        initial=a->preferences;editor=a->experiment.string.len;simulation=a->simulation_time;
        test_window_key_mod(a->window,SDLK_F1,0);break;
    case 1:
        ok=a->doc_visible && documentation_font_variants(a)==7 && documentation_inline_fixture(a,a->doc_topic);
        ok=ok && SDL_SetWindowSize(a->doc_window,760,540) && SDL_SyncWindow(a->doc_window);
        original=a->doc_font_ui;version=a->doc_reading_version;
        printf("FONT DEMAND: initial variants=%u\n",documentation_font_variants(a));
        *capture="docs-font-demand-ready.bmp";break;
    case 2: {
        documentation_font_failure failure={0};failure.fail_after=3;
        struct nk_allocator allocator={nk_handle_ptr(&failure),documentation_font_allocate,documentation_font_release};
        SDL_GLContext before=SDL_GL_GetCurrentContext();a->doc_test_font_allocator=&allocator;
        documentation_zoom(a,1);a->doc_test_font_allocator=NULL;
        ok=documentation_font_size(a)==16 && a->doc_font_ui==original && a->doc_ui->style.font==original &&
           documentation_font_variants(a)==7 && a->doc_reading_version==version && !failure.live &&
           failure.requests>=3 && SDL_GL_GetCurrentContext()==before && a->doc_error[0];
        *capture="docs-font-demand-failed.bmp";break;
    }
    case 3: {
        ps_preferences disk_before={0},disk_after={0};ps_result before=ps_preferences_read(a->preferences_path,&disk_before);
        documentation_font_failure failure={0};failure.fail_after=3;
        struct nk_allocator allocator={nk_handle_ptr(&failure),documentation_font_allocate,documentation_font_release};
        a->settings_draft=a->preferences;a->settings_draft.ui_size=22;a->settings_draft.editor_size=18;
        a->doc_test_font_allocator=&allocator;settings_commit(a);a->doc_test_font_allocator=NULL;
        ps_result after=ps_preferences_read(a->preferences_path,&disk_after);
        ok=before==after && (before!=PS_OK || !memcmp(&disk_before,&disk_after,sizeof disk_before)) &&
           a->preferences.ui_size==initial.ui_size && a->preferences.editor_size==initial.editor_size && a->doc_font_ui==original &&
           documentation_font_variants(a)==7 && !failure.live && a->preferences_error[0];
        puts("FONT DEMAND: failed zoom/settings preserve fonts, context, values and preference file");
        break;
    }
    case 4:
        documentation_zoom(a,1);
        ok=documentation_font_size(a)==18 && a->doc_font_ui->height==18 &&
           fabsf(a->doc_font_code->height/a->doc_font_em[3][1]-14.4f)<.01f &&
           a->doc_ui_fonts[0]==original && documentation_font_variants(a)==12 && !a->doc_error[0];
        *capture="docs-font-demand-retry.bmp";break;
    case 5: {
        documentation_zoom(a,0);ok=a->doc_font_ui==original && documentation_font_variants(a)==12;
        ps_preferences p=a->preferences;p.ui_size=22;p.editor_size=18;preferences_apply(a,&p);
        break;
    }
    case 6:
        ok=documentation_font_size(a)==22 && documentation_code_size(a)==18 &&
           a->doc_font_ui->height==22 && fabsf(a->doc_font_code->height/a->doc_font_em[3][1]-14.4f)<.01f && a->doc_ui_fonts[0]==original &&
           documentation_font_variants(a)==17 && a->doc_reading_version==version;
        printf("FONT DEMAND: independent sizes ui=22 code=18 variants=%u\n",documentation_font_variants(a));
        *capture="docs-font-demand-settings.bmp";break;
    case 7:
        preferences_apply(a,&initial);
        ok=a->doc_font_ui==original && fabsf(a->doc_font_code->height/a->doc_font_em[3][0]-12.8f)<.01f && documentation_font_variants(a)==17 &&
           a->doc_reading_version==version && a->experiment.string.len==editor && a->simulation_time==simulation;
        *capture="docs-font-demand-cache.bmp";break;
    case 8:a->doc_test_density=2;frames=0;break;
    case 9: {
        if(frames++<2){--*stage;return;}
        const struct nk_font *font=a->doc_font_ui->userdata.ptr;
        ok=a->doc_raster_density==2 && font->info.height==32 && a->doc_font_ui->height==16 &&
           documentation_font_variants(a)==7 && !a->doc_ui_fonts[1] && !a->doc_code_fonts[1] &&
           a->doc_reading_version==version;
        *capture="docs-font-demand-density.png";break;
    }
    case 10:a->doc_test_density=1;frames=0;break;
    case 11:
        if(frames++<2){--*stage;return;}
        ok=a->doc_raster_density==1 && a->doc_font_ui->height==16 && documentation_font_variants(a)==7 &&
           a->doc_reading_version==version && a->experiment.string.len==editor && a->simulation_time==simulation;
        puts("FONT DEMAND: populated bank retired at density 2 and rebuilt at density 1");
        *capture="docs-font-demand-reset.bmp";a->doc_test_density=0;a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Font demand stage %d failed: ui=%u code=%u variants=%u\n",*stage-1,documentation_font_size(a),documentation_code_size(a),documentation_font_variants(a));*code=1;a->quitting=true;}
    if(a->quitting)printf("DOCUMENTATION FONT DEMAND SELF-TEST: %s\n",*code?"FAILED":"PASSED");
}
static void documentation_typeset_test_frame(app *a,int *stage,int *code,const char **capture) {
    static size_t blocks[6];static unsigned sample,frames;static int editor,analysis;static double simulation;
    static char image[80];bool okay=true;
    switch((*stage)++) {
    case 0:editor=a->experiment.string.len;analysis=a->analysis.string.len;simulation=a->simulation_time;
        test_window_key_mod(a->window,SDLK_F1,0);break;
    case 1: {
        int topic=-1;for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++)if(!strcmp(documentation_topics[i].path,"docs/pendulum-tutorial.md"))topic=i;
        okay=topic>=0 && SDL_SetWindowSize(a->doc_window,760,540) && SDL_SyncWindow(a->doc_window);
        if(topic>=0)open_documentation(a,topic);
        unsigned count=0;
        for(size_t i=0;a->documentation && i<a->documentation->count;i++)if(a->documentation->blocks[i].kind==PS_DOC_MATH) {
            if(count<6)blocks[count]=i;
            count++;
        }
        okay &= count>=6;sample=0;break;
    }
    case 2: {
        if(!sample)documentation_zoom(a,0);
        if(sample==18)for(int i=0;i<8;i++)documentation_zoom(a,1);
        static const unsigned themes[]={PS_THEME_DARK,PS_THEME_LIGHT,PS_THEME_HIGH_CONTRAST};
        ps_preferences p=a->preferences;p.theme=themes[(sample/6)%3];preferences_apply(a,&p);
        frames=0;break;
    }
    case 3:
        if(frames++<3){--*stage;return;}
        /* Finish zoom position preservation before requesting a new formula. */
        a->doc_jump_block=(int)blocks[sample%6];frames=0;break;
    case 4:
        if(frames++<3 || a->doc_scroll_pending || a->doc_jump_block>=0){--*stage;return;}
        a->doc_math_test_block=blocks[sample%6];a->doc_math_test_case=sample;a->doc_math_test_requested=true;
        snprintf(image,sizeof image,"docs-typeset-%02u.bmp",sample);*capture=image;break;
    case 5: {
        const ps_doc_block *b=&a->documentation->blocks[blocks[sample%6]];
        okay=a->doc_math_test_ok && documentation_font_size(a)==(sample<18?16:32) &&
            nk_sdl_accessibility_document_visible(a->doc_ui,a->doc_reading_version,b->reading_begin) &&
            nk_sdl_accessibility_document_visible(a->doc_ui,a->doc_reading_version,b->reading_begin+b->reading_length-1) &&
            a->experiment.string.len==editor && a->analysis.string.len==analysis && a->simulation_time==simulation;
        if(!sample && okay) {
            const ps_doc_formula_variant *v=documentation_math_variant(a,a->documentation,b,b->hit[2]-24);
            ps_doc_formula_variant bad=*v;bad.crc^=1;unsigned char *invalid=documentation_math_coverage(a,&bad);
            okay &= !invalid;free(invalid);bad=*v;bad.raster_width=8193;
            invalid=documentation_math_coverage(a,&bad);okay &= !invalid;free(invalid);
            const unsigned char pixels[]={255,0,0,255,0,0};
            unsigned char *reduced=documentation_math_reduce(pixels,3,2,2,1);
            okay &= reduced && reduced[0]==170 && reduced[1]==0;free(reduced);
        }
        if(++sample<36)*stage=2;else a->quitting=true;
        break;
    }
    }
    if(!okay){fprintf(stderr,"Formula typeset stage %d case %u failed\n",*stage-1,sample);*code=1;a->quitting=true;}
    if(a->quitting)printf("FORMULA TYPESET SELF-TEST: %s\n",*code?"FAILED":"PASSED");
}
typedef enum {
    DOC_FORMULAS_LEARNING,DOC_FORMULAS_PHYSICS,DOC_FORMULAS_MODELS,DOC_FORMULAS_MECHANICS,DOC_FORMULAS_BASICS
} documentation_formula_group;
static void documentation_formula_catalog_test_frame(app *a,int *stage,int *code,const char **capture,documentation_formula_group group,bool narrow) {
    static const char *learning_paths[]={"docs/user-guide.md","docs/pendulum-tutorial.md","docs/projectile-tutorial.md",
        "docs/projectile-drag-tutorial.md","docs/material-tutorial.md","docs/spring.md","docs/collision-tutorial.md"};
    static const char *physics_paths[]={"docs/thermodynamics.md","docs/electromagnetism.md","docs/waves-optics.md","docs/fluid.md"};
    static const char *model_paths[]={"docs/real-gas.md","docs/measurement.md","docs/monte-carlo-tutorial.md"};
    static const char *mechanics_paths[]={"docs/mechanics.md","docs/buoyancy.md","docs/numerics.md"};
    static const char *basic_paths[]={"docs/math.md","docs/units.md","docs/properties.md","docs/spatial-explanations.md","docs/c-workflow.md"};
    static const struct {const char *const *paths;size_t count;} groups[]={
        {learning_paths,sizeof learning_paths/sizeof learning_paths[0]},
        {physics_paths,sizeof physics_paths/sizeof physics_paths[0]},
        {model_paths,sizeof model_paths/sizeof model_paths[0]},
        {mechanics_paths,sizeof mechanics_paths/sizeof mechanics_paths[0]},
        {basic_paths,sizeof basic_paths/sizeof basic_paths[0]}
    };
    const char *const *paths=groups[group].paths;size_t path_count=groups[group].count;
    static const char *narrow_paths[]={"docs/waves-optics.md"};
    if(narrow && group==DOC_FORMULAS_PHYSICS){paths=narrow_paths;path_count=sizeof narrow_paths/sizeof narrow_paths[0];}
    static size_t page,block;static unsigned sample,frames;static char image[80];
    static int editor,analysis;static double simulation,started;bool okay=true;
    switch((*stage)++) {
    case 0:started=ps_clock();editor=a->experiment.string.len;analysis=a->analysis.string.len;simulation=a->simulation_time;
        test_window_key_mod(a->window,SDLK_F1,0);break;
    case 1: {
        okay=SDL_SetWindowSize(a->doc_window,narrow?760:1100,narrow?540:800) && SDL_SyncWindow(a->doc_window);
        ps_preferences p=a->preferences;p.theme=PS_THEME_LIGHT;preferences_apply(a,&p);
        documentation_zoom(a,0);
        if(narrow)for(int i=0;i<8;i++)documentation_zoom(a,1);
        page=block=sample=0;break;
    }
    case 2: {
        int topic=-1;for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++)if(!strcmp(documentation_topics[i].path,paths[page]))topic=i;
        okay=topic>=0;if(topic>=0)open_documentation(a,topic);block=0;frames=0;break;
    }
    /* Wait for a completed publication, not several extra identical frames. */
    case 3:
        if(frames++<1 || a->doc_scroll_pending || !a->documentation || a->documentation->layout_width<=0){--*stage;return;}break;
    case 4:
        while(a->documentation && block<a->documentation->count) {
            const ps_doc_block *candidate=&a->documentation->blocks[block];const char *text=a->documentation->text+candidate->text;
            bool selected=!narrow || group!=DOC_FORMULAS_PHYSICS || !strncmp(text,"E_K = ",6) || !strncmp(text,"E_G = ",6) ||
                !strncmp(text,"vⱼ^(n−½)",strlen("vⱼ^(n−½)")) || !strncmp(text,"uⱼⁿ⁺¹",strlen("uⱼⁿ⁺¹"));
            if(candidate->kind==PS_DOC_MATH && selected)break;
            ++block;
        }
        if(!a->documentation){okay=false;break;}
        if(block==a->documentation->count) {
            if(++page<path_count)*stage=2;
            else {okay=narrow && group==DOC_FORMULAS_PHYSICS?sample==4:sample>0;a->quitting=true;}
            break;
        }
        a->doc_jump_block=(int)block;frames=0;break;
    case 5:
        if(frames++<1 || a->doc_scroll_pending || a->doc_jump_block>=0 || a->documentation->layout_width<=0){--*stage;return;}
        a->doc_math_test_block=block;a->doc_math_test_case=sample;a->doc_math_test_requested=true;
        snprintf(image,sizeof image,"docs-formula-catalog-%03u.bmp",sample);*capture=image;break;
    case 6: {
        const ps_doc_block *b=&a->documentation->blocks[block];
        okay=a->doc_math_test_ok && documentation_font_size(a)==(narrow?32:16) &&
            nk_sdl_accessibility_document_visible(a->doc_ui,a->doc_reading_version,b->reading_begin) &&
            nk_sdl_accessibility_document_visible(a->doc_ui,a->doc_reading_version,b->reading_begin+b->reading_length-1) &&
            a->experiment.string.len==editor && a->analysis.string.len==analysis && a->simulation_time==simulation;
        printf("FORMULA CATALOG: case=%u page=%s block=%zu complete=%d reading=%s\n",sample,paths[page],block,okay,a->documentation->text+b->text);
        ++block;++sample;*stage=4;break;
    }
    }
    if(!okay){fprintf(stderr,"Formula catalog stage %d page %zu block %zu failed\n",*stage-1,page,block);*code=1;a->quitting=true;}
    if(a->quitting)printf("FORMULA CATALOG SELF-TEST: %s count=%u elapsed=%.3f\n",*code?"FAILED":"PASSED",sample,ps_clock()-started);
}
static void documentation_detail_test_frame(app *a,int *stage,int *code,const char **capture) {
    static unsigned sample,frames;static int editor,analysis;static double simulation,started;static char image[64];bool okay=true;
    switch((*stage)++) {
    case 0:started=ps_clock();editor=a->experiment.string.len;analysis=a->analysis.string.len;simulation=a->simulation_time;
        test_window_key_mod(a->window,SDLK_F1,0);break;
    case 1:sample=0;break;
    case 2: {
        unsigned size=(sample/9)%2?32:16;bool wide=sample>=18;
        okay=SDL_SetWindowSize(a->doc_window,wide?1100:760,wide?800:540) && SDL_SyncWindow(a->doc_window);
        if(documentation_font_size(a)!=size) {
            documentation_zoom(a,0);if(size==32)for(int i=0;i<8;i++)documentation_zoom(a,1);
        }
        static const unsigned themes[]={PS_THEME_DARK,PS_THEME_LIGHT,PS_THEME_HIGH_CONTRAST};
        ps_preferences p=a->preferences;p.theme=themes[(sample/3)%3];preferences_apply(a,&p);
        int topic=-1;for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++)if(!strcmp(documentation_topics[i].path,"docs/pendulum-tutorial.md"))topic=i;
        okay &= topic>=0;if(topic>=0)open_documentation(a,topic);
        a->doc_query[0]=a->doc_last_query[0]=a->doc_error[0]=0;
        if(sample%3==1)snprintf(a->doc_query,sizeof a->doc_query,"E");
        if(sample%3==2)okay &= !documentation_link(a,"missing-detail-target.md") && a->doc_error[0];
        frames=0;break;
    }
    case 3:
        if(frames++<1 || a->doc_scroll_pending || !a->documentation || a->documentation->layout_width<=0){--*stage;return;}
        a->doc_detail_test_case=sample;a->doc_detail_test_requested=true;
        snprintf(image,sizeof image,"docs-detail-%02u.bmp",sample);*capture=image;break;
    case 4:
        okay=a->doc_detail_test_ok && documentation_font_size(a)==((sample/9)%2?32:16) && a->doc_body_bounds.h>0 &&
            a->experiment.string.len==editor && a->analysis.string.len==analysis && a->simulation_time==simulation;
        if(++sample<36)*stage=2;else a->quitting=true;break;
    }
    if(!okay){fprintf(stderr,"Documentation detail stage %d case %u failed\n",*stage-1,sample);*code=1;a->quitting=true;}
    if(a->quitting)printf("DOCUMENTATION DETAIL SELF-TEST: %s elapsed=%.3f\n",*code?"FAILED":"PASSED",ps_clock()-started);
}
#include "documentation_inline_math_tests.inc"
#include "documentation_h_tests.inc"
#include "documentation_pane_tests.inc"
#include "documentation_scroll_tests.inc"
#include "documentation_code_preview_tests.inc"
#include "documentation_model_tests.inc"
static void documentation_keyboard_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    if(!strcmp(mode,"docs-keyboard-model")){documentation_model_test_frame(a,stage,code,capture);return;}
    if(!strcmp(mode,"docs-keyboard-context-code")){documentation_code_preview_test_frame(a,stage,code,capture);return;}
    if(!strcmp(mode,"docs-keyboard-scroll")){documentation_scroll_test_frame(a,stage,code,capture);return;}
    if(!strcmp(mode,"docs-keyboard-panes")){documentation_pane_test_frame(a,stage,code,capture);return;}
    if(!strcmp(mode,"docs-keyboard-h")){documentation_h_test_frame(a,stage,code,capture);return;}
    if(!strcmp(mode,"docs-keyboard-inline-math")){documentation_inline_math_test_frame(a,stage,code,capture);return;}
    if(!strcmp(mode,"docs-keyboard-basics-narrow")){documentation_formula_catalog_test_frame(a,stage,code,capture,DOC_FORMULAS_BASICS,true);return;}
    if(!strcmp(mode,"docs-keyboard-basics")){documentation_formula_catalog_test_frame(a,stage,code,capture,DOC_FORMULAS_BASICS,false);return;}
    if(!strcmp(mode,"docs-keyboard-detail")){documentation_detail_test_frame(a,stage,code,capture);return;}
    if(!strcmp(mode,"docs-keyboard-mechanics-narrow")){documentation_formula_catalog_test_frame(a,stage,code,capture,DOC_FORMULAS_MECHANICS,true);return;}
    if(!strcmp(mode,"docs-keyboard-mechanics")){documentation_formula_catalog_test_frame(a,stage,code,capture,DOC_FORMULAS_MECHANICS,false);return;}
    if(!strcmp(mode,"docs-keyboard-models-narrow")){documentation_formula_catalog_test_frame(a,stage,code,capture,DOC_FORMULAS_MODELS,true);return;}
    if(!strcmp(mode,"docs-keyboard-models")){documentation_formula_catalog_test_frame(a,stage,code,capture,DOC_FORMULAS_MODELS,false);return;}
    if(!strcmp(mode,"docs-keyboard-formula-narrow")){documentation_formula_catalog_test_frame(a,stage,code,capture,DOC_FORMULAS_PHYSICS,true);return;}
    if(!strcmp(mode,"docs-keyboard-formula-physics")){documentation_formula_catalog_test_frame(a,stage,code,capture,DOC_FORMULAS_PHYSICS,false);return;}
    if(!strcmp(mode,"docs-keyboard-formula-catalog")){documentation_formula_catalog_test_frame(a,stage,code,capture,DOC_FORMULAS_LEARNING,false);return;}
    if(!strcmp(mode,"docs-keyboard-typeset")){documentation_typeset_test_frame(a,stage,code,capture);return;}
    if(!strcmp(mode,"docs-keyboard-font-demand")){documentation_font_demand_test(a,stage,code,capture);return;}
    if(!strcmp(mode,"docs-keyboard-sidebar")) {
        static int heading,experiment_length,analysis_length;static double simulation_time;static unsigned frames;
        bool ok=true;switch((*stage)++) {
        case 0:
            experiment_length=a->experiment.string.len;analysis_length=a->analysis.string.len;simulation_time=a->simulation_time;
            test_window_key_mod(a->window,SDLK_F1,0);break;
        case 1:
            open_documentation(a,DOCUMENTATION_C_GUIDE_TOPIC);a->doc_contents=true;
            ok=SDL_SetWindowSize(a->doc_window,760,540) && SDL_SyncWindow(a->doc_window);heading=-1;
            for(size_t i=0;i<a->documentation->count;i++)if(a->documentation->blocks[i].kind==PS_DOC_HEADING &&
                !strcmp(a->documentation->text+a->documentation->blocks[i].text,"1. Einrichten und das erste Projekt öffnen"))heading=(int)i;
            ok=ok && heading>=0;break;
        case 2:
            if(!documentation_test_seek(a,DOC_HEADING+heading,stage,code))return;
            a->doc_sidebar_test_requested=true;*capture="docs-sidebar-16.bmp";break;
        case 3:
            ok=a->doc_sidebar_test_ok;test_window_key_mod(a->doc_window,SDLK_RETURN,0);frames=0;break;
        case 4:
            if(frames++<2 || a->doc_scroll_pending || a->doc_jump_block>=0){--*stage;return;}
            {const ps_doc_block *b=&a->documentation->blocks[heading];
            ok=nk_sdl_accessibility_document_visible(a->doc_ui,a->doc_reading_version,b->reading_begin) &&
               nk_sdl_accessibility_document_visible(a->doc_ui,a->doc_reading_version,b->reading_begin+b->reading_length-1);}
            *capture="docs-sidebar-jump-16.bmp";for(int i=0;i<8;i++)test_window_key_mod(a->doc_window,SDLK_PLUS,PS_UI_COMMAND_MOD);frames=0;break;
        case 5:
            if(frames++<2 || a->doc_scroll_pending){--*stage;return;}
            a->doc_focus_reveal=true;frames=0;break;
        case 6:
            if(!documentation_test_seek(a,DOC_HEADING+heading,stage,code))return;
            a->doc_sidebar_test_requested=true;*capture="docs-sidebar-32.bmp";break;
        case 7:
            ok=documentation_font_size(a)==32 && a->doc_sidebar_test_ok;
            {ps_preferences p=a->preferences;p.theme=PS_THEME_LIGHT;preferences_apply(a,&p);}frames=0;break;
        case 8:
            if(frames++<2){--*stage;return;}
            a->doc_sidebar_test_requested=true;*capture="docs-sidebar-32-light.bmp";break;
        case 9:
            ok=a->doc_sidebar_test_ok;{ps_preferences p=a->preferences;p.theme=PS_THEME_HIGH_CONTRAST;preferences_apply(a,&p);}frames=0;break;
        case 10:
            if(frames++<2){--*stage;return;}
            a->doc_sidebar_test_requested=true;*capture="docs-sidebar-32-contrast.bmp";break;
        case 11:
            ok=a->doc_sidebar_test_ok;test_window_mouse(a->doc_window,a->doc_focus_bounds,true);break;
        case 12:test_window_mouse(a->doc_window,a->doc_focus_bounds,false);frames=0;break;
        case 13:
            if(frames++<2 || a->doc_scroll_pending || a->doc_jump_block>=0){--*stage;return;}
            {const ps_doc_block *b=&a->documentation->blocks[heading];
            ok=nk_sdl_accessibility_document_visible(a->doc_ui,a->doc_reading_version,b->reading_begin) &&
               nk_sdl_accessibility_document_visible(a->doc_ui,a->doc_reading_version,b->reading_begin+b->reading_length-1);}
            ok=ok && a->experiment.string.len==experiment_length && a->analysis.string.len==analysis_length && a->simulation_time==simulation_time;
            *capture="docs-sidebar-jump-32.bmp";
            documentation_zoom(a,0);a->quitting=true;
            puts("DOCUMENTATION SIDEBAR DIRECT: topics and sections, keyboard and pointer jumps, focus and input isolation passed");break;
        }
        if(!ok){fprintf(stderr,"Sidebar stage %d failed\n",*stage-1);*code=1;a->quitting=true;}
        if(a->quitting)printf("DOCUMENTATION SIDEBAR SELF-TEST: %s\n",*code?"FAILED":"PASSED");return;
    }
    if(!strcmp(mode,"docs-keyboard-navigation")){documentation_navigation_test_frame(a,stage,code,capture);return;}
    if(!strcmp(mode,"docs-keyboard-density")) {
        static unsigned frames,index;static uint64_t version;static float reference;static int editor_length;
        static const float densities[]={1.25f,1.5f,2,1};bool ok=true;
        switch(*stage) {
        case 0:editor_length=a->experiment.string.len;test_window_key_mod(a->window,SDLK_F1,0);++*stage;break;
        case 1:
            ok=documentation_inline_fixture(a,a->doc_topic);version=a->doc_reading_version;
            ok=ok && SDL_SetWindowSize(a->doc_window,760,540) && SDL_SyncWindow(a->doc_window);
            reference=a->doc_font_ui->width(a->doc_font_ui->userdata,16,"AaBb 0123",9);
            index=frames=0;a->doc_test_density=densities[index];++*stage;break;
        case 2: {
            if(frames++<2 || a->doc_scroll_pending)return;
            const struct nk_font *font=a->doc_font_ui->userdata.ptr;
            float width=a->doc_font_ui->width(a->doc_font_ui->userdata,16,"AaBb 0123",9);
            ok=fabsf(a->doc_raster_density-densities[index])<.01f &&
               fabsf(font->info.height-16*densities[index])<.01f && a->doc_font_ui->height==16 &&
               fabsf(width-reference)<.02f && a->doc_reading_version==version && a->experiment.string.len==editor_length;
            printf("DOCUMENTATION DENSITY: %.2f baked=%.2f logical=%.2f width=%.3f\n",densities[index],font->info.height,a->doc_font_ui->height,width);
            static const char *names[]={"docs-density-125.bmp","docs-density-150.bmp","docs-density-200.png","docs-density-reset.bmp"};
            *capture=names[index];frames=0;++index;*stage=3;
            break;
        }
        case 3:
            if(index<4){a->doc_test_density=densities[index];*stage=2;}
            else *stage=4;
            break;
        case 4: {
            /* Invalid replacement leaves live fonts and the current text intact. */
            const struct nk_user_font *before=a->doc_font_ui;
            ok=!documentation_window_fonts(a,NAN) && a->doc_font_ui==before && a->doc_reading_version==version;
            struct nk_font_atlas invalid;nk_font_atlas_init_default(&invalid);nk_font_atlas_begin(&invalid);
            struct nk_font *foreign=a->doc_font_ui->userdata.ptr;
            ok=ok && !nk_sdl_font_stash_replace(a->doc_ui,&invalid,2,foreign);
            nk_font_atlas_clear(&invalid);
            documentation_font_failure failure={0};struct nk_allocator allocator={nk_handle_ptr(&failure),documentation_font_allocate,documentation_font_release};
            nk_font_atlas_init(&invalid,&allocator);nk_font_atlas_begin(&invalid);
            struct nk_font *candidate=nk_font_atlas_add_default(&invalid,32,NULL);ok=ok && candidate;
            failure.fail=true;
            ok=ok && !nk_sdl_font_stash_replace(a->doc_ui,&invalid,2,candidate) && a->doc_ui->style.font==before;
            failure.fail=false;nk_font_atlas_clear(&invalid);ok=ok && failure.live==0;
            a->doc_test_density=0;a->quitting=true;break;
        }
        }
        if(!ok){fprintf(stderr,"Documentation density stage %d failed\n",*stage);*code=1;a->quitting=true;}
        if(a->quitting)printf("DOCUMENTATION DENSITY SELF-TEST: %s\n",*code?"FAILED":"PASSED");return;
    }
    if(!strcmp(mode,"docs-keyboard-lifecycle-remote")) {
        static int experiment_length,analysis_length;static double simulation_time;static uint64_t version;
        bool ok=true;char path[4096];FILE *marker;switch(*stage) {
        case 0:
            experiment_length=a->experiment.string.len;analysis_length=a->analysis.string.len;simulation_time=a->simulation_time;
            test_window_key_mod(a->window,SDLK_F1,0);++*stage;break;
        case 1:
            ok=documentation_inline_fixture(a,a->doc_topic);version=a->doc_reading_version;
            ok=ok && SDL_SetWindowSize(a->doc_window,760,540) && SDL_SyncWindow(a->doc_window);++*stage;break;
        case 2:puts("LIFECYCLE READY");fflush(stdout);++*stage;break;
        case 3:
            join(path,sizeof path,a->project,"close-request");marker=fopen(path,"rb");if(!marker)return;fclose(marker);
            test_window_key_mod(a->doc_window,SDLK_ESCAPE,0);++*stage;break;
        case 4:
            join(path,sizeof path,a->project,"hidden-checked");marker=fopen(path,"rb");if(!marker)return;fclose(marker);
            ok=!a->doc_visible && !a->doc_search_active && !a->doc_filter_active;
            ok=ok && a->doc_reading_version==version && !strcmp(a->doc_query,"θ🌍");open_documentation(a,a->doc_topic);++*stage;break;
        case 5:
            join(path,sizeof path,a->project,"reopen-checked");marker=fopen(path,"rb");if(!marker)return;fclose(marker);
            ok=a->doc_visible && a->doc_reading_version==version && !strcmp(a->doc_query,"θ🌍");
            *capture="docs-lifecycle-reopened.bmp";++*stage;break;
        case 6:
            join(path,sizeof path,a->project,"reopen-app-captured");marker=fopen(path,"wb");if(marker){fputs("verified\n",marker);fclose(marker);}else ok=false;
            join(path,sizeof path,a->project,"minimized-checked");marker=fopen(path,"rb");if(!marker)return;fclose(marker);
            ok=(SDL_GetWindowFlags(a->doc_window)&SDL_WINDOW_MINIMIZED)!=0;
            join(path,sizeof path,a->project,"minimized-app-checked");marker=fopen(path,"wb");if(marker){fputs("verified\n",marker);fclose(marker);}else ok=false;
            ++*stage;break;
        case 7:
            join(path,sizeof path,a->project,"restored-checked");marker=fopen(path,"rb");if(!marker)return;fclose(marker);
            ok=!(SDL_GetWindowFlags(a->doc_window)&SDL_WINDOW_MINIMIZED) && a->doc_reading_version==version && !strcmp(a->doc_query,"θ🌍");
            *capture="docs-lifecycle-restored.bmp";++*stage;break;
        case 8:
            ok=a->experiment.string.len==experiment_length && a->analysis.string.len==analysis_length && a->simulation_time==simulation_time;
            a->quitting=true;break;
        }
        if(!ok){fprintf(stderr,"Native lifecycle stage %d failed\n",*stage-1);*code=1;a->quitting=true;}
        if(a->quitting)printf("DOCUMENTATION LIFECYCLE SELF-TEST: %s\n",*code?"FAILED":"PASSED");return;
    }
    if(!strcmp(mode,"docs-keyboard-links-native")) {
        static int experiment_length,analysis_length,source_topic;static uint64_t source_version;
        static double simulation_time;static unsigned settle;bool ok=true;
        switch(*stage) {
        case 0:
            experiment_length=a->experiment.string.len;analysis_length=a->analysis.string.len;simulation_time=a->simulation_time;
            test_window_key_mod(a->window,SDLK_F1,0);++*stage;break;
        case 1:
            ok=documentation_link_inventory(a) && documentation_inline_fixture_variant(a,a->doc_topic,true);
            source_topic=a->doc_topic;source_version=a->doc_reading_version;SDL_SetWindowSize(a->doc_window,760,540);++*stage;break;
        case 2:ok=nk_sdl_accessibility_native_links_test_step(a->doc_ui,0);settle=0;++*stage;break;
        case 3:
            if(settle++<2)return;ok=nk_sdl_accessibility_native_links_test_step(a->doc_ui,1);
            *capture="docs-macos-link-focus.bmp";++*stage;break;
        case 4:ok=nk_sdl_accessibility_native_links_test_step(a->doc_ui,2);settle=0;++*stage;break;
        case 5:
            if(settle++<2)return;
            ok=a->doc_topic==source_topic && a->doc_reading_version==source_version && strstr(a->doc_error,"nicht in der mitgelieferten") && nk_sdl_accessibility_native_links_test_step(a->doc_ui,3);
            *capture="docs-macos-link-failed.bmp";++*stage;break;
        case 6:ok=nk_sdl_accessibility_native_links_test_step(a->doc_ui,4);settle=0;++*stage;break;
        case 7:
            if(settle++<2)return;
            ok=!strcmp(documentation_topics[a->doc_topic].path,"docs/c-guide.md") && !a->doc_error[0] && nk_sdl_accessibility_native_links_test_step(a->doc_ui,5);
            *capture="docs-macos-link-open.bmp";test_window_key_mod(a->doc_window,SDLK_ESCAPE,0);++*stage;break;
        case 8:
            ok=!a->doc_visible && a->experiment.string.len==experiment_length && a->analysis.string.len==analysis_length && a->simulation_time==simulation_time;
            a->quitting=true;if(ok)puts("MAC F1 LINKS: native attributed text, URL, focused glyphs, failed target feedback, completed activation, stale reference and editor isolation passed");break;
        }
        if(!ok){fprintf(stderr,"Native macOS links stage %d failed\n",*stage-1);*code=1;a->quitting=true;}
        if(a->quitting)printf("DOCUMENTATION LINKS SELF-TEST: %s\n",*code?"FAILED":"PASSED");return;
    }
    if(!strcmp(mode,"docs-keyboard-links-remote")) {
        static int experiment_length,analysis_length,source_topic;static double simulation_time;static uint64_t source_version;bool ok=true;
        char path[4096];FILE *done;switch(*stage) {
        case 0:
            experiment_length=a->experiment.string.len;analysis_length=a->analysis.string.len;simulation_time=a->simulation_time;
            test_window_key_mod(a->window,SDLK_F1,0);++*stage;break;
        case 1:
            ok=documentation_link_inventory(a) && documentation_inline_fixture_variant(a,a->doc_topic,true);
            source_topic=a->doc_topic;source_version=a->doc_reading_version;SDL_SetWindowSize(a->doc_window,760,540);++*stage;break;
        case 2:puts("LINKS READY FOCUS");fflush(stdout);++*stage;break;
        case 3:
            join(path,sizeof path,a->project,"link-focus-done");done=fopen(path,"rb");if(!done)return;fclose(done);
            ok=nk_sdl_accessibility_is_focused(a->doc_ui,NULL,"C");*capture="docs-native-link-focus.bmp";++*stage;break;
        case 4:puts("LINKS READY INVALID");fflush(stdout);++*stage;break;
        case 5:
            join(path,sizeof path,a->project,"link-invalid-done");done=fopen(path,"rb");if(!done)return;fclose(done);
            ok=a->doc_topic==source_topic && a->doc_reading_version==source_version && strstr(a->doc_error,"nicht in der mitgelieferten")!=NULL;
            *capture="docs-native-link-failed.bmp";++*stage;break;
        case 6:puts("LINKS READY INVOKE");fflush(stdout);++*stage;break;
        case 7:
            join(path,sizeof path,a->project,"link-invoke-done");done=fopen(path,"rb");if(!done)return;fclose(done);
            ok=!strcmp(documentation_topics[a->doc_topic].path,"docs/c-guide.md") && !a->doc_error[0];
            *capture="docs-native-link-open.bmp";test_window_key_mod(a->doc_window,SDLK_ESCAPE,0);++*stage;break;
        case 8:
            ok=!a->doc_visible && a->experiment.string.len==experiment_length && a->analysis.string.len==analysis_length && a->simulation_time==simulation_time;
            a->quitting=true;break;
        }
        if(!ok){fprintf(stderr,"Native links stage %d failed (keyboard=%d flags=0x%llx focus=%d)\n",*stage-1,
            SDL_GetKeyboardFocus()==a->doc_window,(unsigned long long)SDL_GetWindowFlags(a->doc_window),nk_sdl_accessibility_has_focus(a->doc_ui));*code=1;a->quitting=true;}
        if(a->quitting)printf("DOCUMENTATION LINKS SELF-TEST: %s\n",*code?"FAILED":"PASSED");return;
    }
    if(!strcmp(mode,"docs-keyboard-edit-scroll-remote")) {
        static unsigned settle,open_frames;bool ok=true;char path[4096];FILE *ready;
        switch(*stage) {
        case 0:test_window_key_mod(a->window,SDLK_F1,0);++*stage;break;
        case 1:
            if(!open_frames){SDL_SetWindowSize(a->doc_window,760,540);test_window_key_mod(a->doc_window,SDLK_F,PS_UI_COMMAND_MOD);}
            if(open_frames++<3)return;
            ok=a->doc_find_open && a->doc_search_bounds.w>0 && a->doc_search_bounds.h>0;
            if(ok){puts("EDIT SCROLL READY VALUE");fflush(stdout);}++*stage;break;
        case 2:
            if(strlen(a->doc_query)<90 || !strstr(a->doc_query,"Anfang θ") || !strstr(a->doc_query,"Ziel ᵣ") ||
               !a->doc_search_active || !nk_sdl_accessibility_is_focused(a->doc_ui,NULL,"Aktuelles Dokument durchsuchen"))return;
            test_window_key_mod(a->doc_window,SDLK_TAB,0);++*stage;break;
        case 3:
            if(settle++<3)return;
            ok=a->doc_focus==DOC_PREVIOUS && !a->doc_search_active && nk_sdl_accessibility_is_focused(a->doc_ui,NULL,"Vorheriger Treffer");
            puts("EDIT SCROLL READY END");fflush(stdout);++*stage;break;
        case 4:
            join(path,sizeof path,a->project,"scroll-end-applied");ready=fopen(path,"rb");if(!ready)return;fclose(ready);
            ok=a->doc_focus==DOC_PREVIOUS && !a->doc_search_active && nk_sdl_accessibility_is_focused(a->doc_ui,NULL,"Vorheriger Treffer");
            *capture="docs-native-edit-scroll-end.bmp";++*stage;break;
        case 5:puts("EDIT SCROLL READY START");fflush(stdout);++*stage;break;
        case 6:
            join(path,sizeof path,a->project,"scroll-start-applied");ready=fopen(path,"rb");if(!ready)return;fclose(ready);
            ok=a->doc_focus==DOC_PREVIOUS && !a->doc_search_active && nk_sdl_accessibility_is_focused(a->doc_ui,NULL,"Vorheriger Treffer");
            *capture="docs-native-edit-scroll-start.bmp";++*stage;break;
        case 7:puts("DOCUMENTATION EDIT SCROLL SELF-TEST: PASSED");a->quitting=true;break;
        }
        if(!SDL_getenv("PHYSIM_TEST_ALL_CAPTURES"))*capture=NULL;
        if(!ok){fprintf(stderr,"Native edit scroll stage %d changed keyboard focus\n",*stage);*code=1;a->quitting=true;}
        return;
    }
    if(!strcmp(mode,"docs-keyboard-math")){documentation_math_test_frame(a,stage,code,capture);return;}
    if(!strcmp(mode,"docs-keyboard-edit-remote")) {
        static int header;static int settle,experiment_length,analysis_length;static unsigned open_frames;static double simulation_time;bool ok=true;
        switch(*stage) {
        case 0:
            experiment_length=a->experiment.string.len;analysis_length=a->analysis.string.len;simulation_time=a->simulation_time;
            test_window_key_mod(a->window,SDLK_F1,0);++*stage;break;
        case 1:
            header=-1;for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++)if(!strcmp(documentation_topics[i].path,"include/physim/core.h"))header=i;
            ok=header>=0;open_documentation(a,header);SDL_SetWindowSize(a->doc_window,760,540);++*stage;break;
        case 2:
            /* Design H opens the article search explicitly; an article alone
             * does not publish this field. Exercise the same Ctrl+F route as
             * the user, then let independent native clients own the edits. */
            if(!open_frames)test_window_key_mod(a->doc_window,SDLK_F,PS_UI_COMMAND_MOD);
            if(open_frames++<3)return;
            ok=a->doc_find_open && a->doc_search_bounds.w>0 && a->doc_search_bounds.h>0;
            if(ok){puts("EDIT READY SEARCH");fflush(stdout);}++*stage;break;
        case 3:
            if(strcmp(a->doc_query,"ps_rng_seed θ🌍") || !a->doc_search_active || !nk_sdl_accessibility_is_focused(a->doc_ui,NULL,"Aktuelles Dokument durchsuchen"))return;
            *capture="docs-native-search.bmp";puts("EDIT APPLIED SEARCH");fflush(stdout);
            ++*stage;break;
        case 4:
            {char path[4096];join(path,sizeof path,a->project,"selection-applied");FILE *done=fopen(path,"rb");if(!done)return;fclose(done);}
            *capture="docs-native-selection.bmp";
            {SDL_Event input={0};input.type=SDL_EVENT_TEXT_INPUT;input.text.windowID=SDL_GetWindowID(a->doc_window);input.text.text="Z";SDL_PushEvent(&input);}++*stage;break;
        case 5:
            if(a->doc_ui->input.keyboard.text_len)return;
            ok=!strcmp(a->doc_query,"ps_rng_seed Z");test_window_key_mod(a->doc_window,SDLK_TAB,0);++*stage;break;
        case 6:
            ok=a->doc_focus==DOC_PREVIOUS && !a->doc_search_active;
            a->doc_query[0]=0;a->doc_contents=false;a->doc_reset_sidebar=true;a->doc_filter[0]=0;documentation_filter(a);
            ++*stage;break;
        case 7:
            if(settle++<2)return;puts("EDIT READY GLOBAL");fflush(stdout);++*stage;break;
        case 8:
            if(strcmp(a->doc_filter,"ps_rng_seed") || !a->doc_filter_active || !nk_sdl_accessibility_is_focused(a->doc_ui,NULL,"Alle Inhalte durchsuchen"))return;
            ok=documentation_hits[header];*capture="docs-native-global-search.bmp";
            puts("EDIT APPLIED GLOBAL");fflush(stdout);++*stage;break;
        case 9:
            {char path[4096];join(path,sizeof path,a->project,"global-focus-checked");FILE *done=fopen(path,"rb");if(!done)return;fclose(done);}
            test_window_key_mod(a->doc_window,SDLK_TAB,0);++*stage;break;
        case 10:
            ok=a->doc_focus==DOC_CONTEXT && !a->doc_filter_active;
            test_window_key_mod(a->doc_window,SDLK_F,PS_UI_COMMAND_MOD);++*stage;break;
        case 11:
            if(!a->doc_search_active || a->doc_focus!=DOC_SEARCH)return;
            puts("EDIT READY RETURN");fflush(stdout);++*stage;break;
        case 12:
            {char path[4096];join(path,sizeof path,a->project,"return-focus-checked");FILE *done=fopen(path,"rb");if(!done)return;fclose(done);}
            test_window_key_mod(a->doc_window,SDLK_TAB,0);++*stage;break;
        case 13:
            ok=a->doc_focus==DOC_PREVIOUS && !a->doc_search_active;
            puts("EDIT READY PREVIOUS");fflush(stdout);++*stage;break;
        case 14:
            {char path[4096];join(path,sizeof path,a->project,"previous-focus-checked");FILE *done=fopen(path,"rb");if(!done)return;fclose(done);}
            test_window_key_mod(a->doc_window,SDLK_TAB,0);++*stage;break;
        case 15:
            ok=a->doc_focus==DOC_NEXT && !a->doc_search_active;
            puts("EDIT READY NEXT");fflush(stdout);++*stage;break;
        case 16:
            {char path[4096];join(path,sizeof path,a->project,"next-focus-checked");FILE *done=fopen(path,"rb");if(!done)return;fclose(done);}
            test_window_key_mod(a->doc_window,SDLK_TAB,SDL_KMOD_SHIFT);++*stage;break;
        case 17:
            ok=a->doc_focus==DOC_PREVIOUS && !a->doc_search_active;
            puts("EDIT READY PREVIOUS RETURN");fflush(stdout);++*stage;break;
        case 18:
            {char path[4096];join(path,sizeof path,a->project,"previous-return-focus-checked");FILE *done=fopen(path,"rb");if(!done)return;fclose(done);}
            ok=a->experiment.string.len==experiment_length && a->analysis.string.len==analysis_length && a->simulation_time==simulation_time;
            a->quitting=true;puts("DOCUMENTATION EDIT SELF-TEST: PASSED");break;
        }
        if(!SDL_getenv("PHYSIM_TEST_ALL_CAPTURES"))*capture=NULL;
        if(!ok){fprintf(stderr,"Native documentation edit stage %d failed: focus=%d query=%s filter=%s\n",*stage,a->doc_focus,a->doc_query,a->doc_filter);*code=1;a->quitting=true;}
        return;
    }
    if(!strcmp(mode,"docs-keyboard-edit-native")) {
        static int experiment_length,analysis_length;bool ok=true;
        switch(*stage) {
        case 0:
            experiment_length=a->experiment.string.len;analysis_length=a->analysis.string.len;
            test_window_key_mod(a->window,SDLK_F1,0);++*stage;break;
        case 1:
            SDL_SetWindowSize(a->doc_window,760,540);a->doc_contents=false;a->doc_reset_sidebar=true;++*stage;break;
        case 2:case 3:case 4:case 5:case 6:case 7:case 8:case 9:case 10:case 11:
            ok=nk_sdl_accessibility_native_edit_test_step(a->doc_ui,*stage-2);
            if(*stage==11)*capture="docs-macos-edit-end.bmp";
            ++*stage;break;
        case 12:
            SDL_HideWindow(a->doc_window);++*stage;break;
        case 13:
            ok=nk_sdl_accessibility_native_edit_test_step(a->doc_ui,10);SDL_ShowWindow(a->doc_window);++*stage;break;
        case 14:
            ok=nk_sdl_accessibility_native_edit_test_step(a->doc_ui,11) &&
               a->experiment.string.len==experiment_length && a->analysis.string.len==analysis_length;
            a->quitting=true;puts("MAC F1 EDIT: actual fields, Unicode selection/replacement, caret, end reveal, hidden rejection and editor isolation passed");
            break;
        }
        if(!ok){fprintf(stderr,"Native macOS F1 field stage %d failed\n",*stage-1);*code=1;a->quitting=true;}
        if(a->quitting)printf("DOCUMENTATION EDIT SELF-TEST: %s\n",*code?"FAILED":"PASSED");
        return;
    }
    if(!strcmp(mode,"docs-keyboard-reading-remote") || !strcmp(mode,"docs-keyboard-reading-native")) {
        switch(*stage) {
        case 0:test_window_key_mod(a->window,SDLK_F1,0);++*stage;break;
        case 1: {
            int topic=-1;for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++)if(!strcmp(documentation_topics[i].path,"docs/project-audit.json"))topic=i;
            if(topic<0){*code=1;a->quitting=true;return;}
            open_documentation(a,topic);SDL_SetWindowSize(a->doc_window,760,540);++*stage;break;
        }
        case 2:
            if(!strcmp(mode,"docs-keyboard-reading-native") && !nk_sdl_accessibility_document_native_read_test(a->doc_ui)) {
                *code=1;a->quitting=true;puts("DOCUMENTATION READING SELF-TEST: FAILED");return;
            }
            puts("READING READY");fflush(stdout);++*stage;break;
        case 3:
            if(a->doc_scroll_y<10000 || a->doc_scroll_pending)return;
            {size_t last=a->documentation->reading_length;
             while(last && strchr(" \t\r\n",a->documentation->reading[last-1]))--last;
             if(!last || !nk_sdl_accessibility_document_visible(a->doc_ui,a->doc_reading_version,last-1))return;}
            *capture="docs-reading-end.bmp";puts("READING END");fflush(stdout);++*stage;break;
        case 4:a->quitting=true;puts("DOCUMENTATION READING SELF-TEST: PASSED");break;
        }
        return;
    }
    if(!strcmp(mode,"docs-keyboard-inline")){documentation_inline_test_frame(a,stage,code,capture);return;}
    if(!strcmp(mode,"docs-keyboard-zoom")){documentation_zoom_test_frame(a,stage,code,capture);return;}
    if(!strcmp(mode,"docs-keyboard-history")){documentation_history_test_frame(a,stage,code,capture);return;}
    if(!strncmp(mode,"docs-keyboard-anchors",21)){documentation_anchor_test_frame(a,stage,code,capture);return;}
    if(!strncmp(mode,"docs-keyboard-media",19)){documentation_media_test_frame(a,stage,code,capture);return;}
    if(!strcmp(mode,"docs-keyboard-api")){documentation_api_test_frame(a,stage,code,capture);return;}
    static int copy_block,link_block,link_topic,heading,tutorial_topic;
    static nk_uint before_scroll;
    bool ok=true;
#define DOC_KEY(key) test_window_key_mod(a->doc_window,key,0)
    switch((*stage)++) {
    case 0: test_window_key_mod(a->window,SDLK_F1,0);break;
    case 1: tutorial_topic=-1;for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++)if(!strcmp(documentation_topics[i].path,"docs/experiment-tutorial.md"))tutorial_topic=i;
        ok=a->doc_visible && tutorial_topic>=0 &&
           documentation_test_same_text("θ\r\n  x;\r\n","θ\n  x;\n") &&
           !documentation_test_same_text("θ\r\n x;\r\n","θ\n  x;\n") &&
           !documentation_test_same_text("θ\r\n  y;\r\n","θ\n  x;\n") &&
           !documentation_test_same_text("θ\r\n  x;","θ\n  x;\n") &&
           !documentation_test_same_text("θ\r  x;","θ\n  x;");
        SDL_SetWindowSize(a->doc_window,760,540);DOC_KEY(SDLK_TAB);DOC_KEY(SDLK_TAB);break;
    case 2: ok=a->doc_keyboard && a->doc_focus==DOC_C;DOC_KEY(SDLK_TAB);DOC_KEY(SDLK_RETURN);break;
    case 3: ok=a->doc_topic==DOCUMENTATION_PHYSIM_GUIDE_TOPIC;test_window_key_mod(a->doc_window,SDLK_TAB,SDL_KMOD_SHIFT);DOC_KEY(SDLK_RETURN);break;
    case 4: ok=a->doc_topic==DOCUMENTATION_C_GUIDE_TOPIC;break;
    case 5:
        if(!documentation_test_seek(a,!strcmp(mode,"docs-keyboard-unreachable")?99999:DOC_TOPIC+tutorial_topic,stage,code))return;DOC_KEY(SDLK_RETURN);*stage=25;break;
    case 25:
        ok=a->doc_topic==tutorial_topic;copy_block=link_block=link_topic=heading=-1;
        for(size_t i=0;i<a->documentation->count;i++) {
            ps_doc_block *b=&a->documentation->blocks[i];
            if(copy_block<0 && b->kind==PS_DOC_CODE)copy_block=(int)i;
            if(heading<0 && b->kind==PS_DOC_HEADING && b->level<=2 && i>0)heading=(int)i;
            if(link_block<0 && b->kind==PS_DOC_LINK) {
                const char *target=a->documentation->text+b->target;
                for(int j=0;j<DOCUMENTATION_TOPIC_COUNT;j++)if(!strcmp(target,documentation_topics[j].path) || (!strncmp(documentation_topics[j].path,"docs/",5) && !strcmp(target,documentation_topics[j].path+5))){link_block=(int)i;link_topic=j;break;}
            }
        }
        ok=ok && copy_block>=0 && link_block>=0 && heading>=0;*stage=6;break;
    case 6: break; /* Sections are directly present beside topics in H. */
    case 7: if(!documentation_test_seek(a,DOC_HEADING+heading,stage,code))return;DOC_KEY(SDLK_RETURN);break;
    case 8:
        if(a->doc_jump_block>=0 || a->doc_scroll_pending || a->doc_history_restore){--*stage;return;}
        ok=a->doc_scroll_y>0 && fabsf(a->documentation->blocks[heading].hit[1]-a->doc_body_bounds.y)<40;
        if(!ok)fprintf(stderr,"Heading activation target=%d anchor=%d scroll=%u pending=%d history=%u keyboard=%d owner=%d bounds=%.0f/%.0f\n",
            heading,a->doc_anchor_heading,a->doc_scroll_y,a->doc_scroll_pending,a->doc_history_restore,a->doc_keyboard,
            nk_sdl_accessibility_keyboard_owned(a->doc_ui),a->documentation->blocks[heading].hit[1],a->doc_body_bounds.y);
        *capture="docs-keyboard-heading.bmp";break;
    case 9: if(!documentation_test_seek(a,DOC_COPY+copy_block,stage,code))return;SDL_SetClipboardText("sentinel");DOC_KEY(SDLK_SPACE);break;
    case 10: ok=documentation_test_clipboard(a->documentation->text+a->documentation->blocks[copy_block].text);break;
    case 11: if(!documentation_test_seek(a,DOC_LINK+link_block,stage,code))return;DOC_KEY(SDLK_RETURN);break;
    case 12: ok=a->doc_topic==link_topic;break;
    case 13: if(!documentation_test_seek(a,DOC_BACK,stage,code))return;DOC_KEY(SDLK_RETURN);break;
    case 14: ok=a->doc_topic==tutorial_topic;break;
    case 15: if(!documentation_test_seek(a,DOC_BODY,stage,code))return;DOC_KEY(SDLK_HOME);break;
    case 16: ok=a->doc_scroll_y==0;before_scroll=a->doc_scroll_y;DOC_KEY(SDLK_PAGEDOWN);break;
    case 17: ok=a->doc_scroll_y>before_scroll;DOC_KEY(SDLK_END);break;
    case 18: ok=a->doc_scroll_y>0;DOC_KEY(SDLK_HOME);break;
    case 19: ok=a->doc_scroll_y==0;test_window_key_mod(a->doc_window,SDLK_F,PS_UI_COMMAND_MOD);break;
    case 20: {if(a->doc_focus_reveal){--*stage;return;}ok=a->doc_focus==DOC_SEARCH && a->doc_search_active;SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->doc_window);e.text.text="Physim";SDL_PushEvent(&e);break;}
    case 21: if(a->doc_ui->input.keyboard.text_len){--*stage;return;}ok=!strcmp(a->doc_query,"Physim");DOC_KEY(SDLK_TAB);break;
    case 22: ok=a->doc_focus==DOC_PREVIOUS && !a->doc_search_active;DOC_KEY(SDLK_TAB);DOC_KEY(SDLK_RETURN);break;
    case 23: ok=a->doc_focus==DOC_NEXT;*capture="docs-keyboard-search.bmp";*stage=26;break;
    case 26: break; /* The global search stays in the common header. */
    case 27: if(!documentation_test_seek(a,DOC_FILTER,stage,code))return;break;
    case 28: {ok=a->doc_filter_active;SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->doc_window);e.text.text="Pendel";SDL_PushEvent(&e);break;}
    case 29: if(a->doc_ui->input.keyboard.text_len){--*stage;return;}ok=!strcmp(a->doc_filter,"Pendel");break;
    case 30: ok=a->doc_focus==DOC_FILTER && a->doc_filter_active;test_window_key_mod(a->doc_window,SDLK_A,PS_UI_COMMAND_MOD);break;
    case 31: DOC_KEY(SDLK_BACKSPACE);*stage=34;break;
    case 34: DOC_KEY(SDLK_TAB);break;
    case 35: ok=!a->doc_filter[0];test_window_key_mod(a->doc_window,SDLK_F,PS_UI_COMMAND_MOD);*stage=32;break;
    case 32: if(a->doc_focus_reveal){--*stage;return;}
        {SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->doc_window);e.text.text="Z";SDL_PushEvent(&e);DOC_KEY(SDLK_TAB);}break;
    case 33: ok=a->doc_focus==DOC_PREVIOUS && !strcmp(a->doc_query,"PhysimZ");DOC_KEY(SDLK_ESCAPE);*stage=24;break;
    case 24: ok=!a->doc_visible && !a->doc_keyboard;a->quitting=true;printf("DOCUMENTATION KEYBOARD SELF-TEST: %s\n",*code?"FAILED":"PASSED");break;
    }
    if(!ok){fprintf(stderr,"Query debug: query=%s active=%d events bytes=%d window=%s edit=%d flags=%u\n",a->doc_query,a->doc_search_active,a->doc_ui->input.keyboard.text_len,a->doc_ui->active?a->doc_ui->active->name_string:"none",a->doc_ui->active?a->doc_ui->active->edit.active:0,a->doc_ui->active?(unsigned)a->doc_ui->active->flags:0);fprintf(stderr,"Documentation keyboard failure stage %d focus=%d topic=%d scroll=%u reveal=%d\n",*stage-1,a->doc_focus,a->doc_topic,a->doc_scroll_y,a->doc_focus_reveal);*code=1;a->quitting=true;}
#undef DOC_KEY
}
