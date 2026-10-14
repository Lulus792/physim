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
typedef struct {size_t live;bool fail;} documentation_font_failure;
static void *documentation_font_allocate(nk_handle user,void *old,nk_size size) {
 (void)old;documentation_font_failure *state=user.ptr;if(state->fail)return NULL;
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
    static unsigned seeks,attempts;
    static int target,last_topic=-1,last_stage=-1;
    int entries[12*PS_DOC_MAX_BLOCKS+DOCUMENTATION_TOPIC_COUNT+16];
    unsigned count=documentation_focus_list(a,entries);
    if(target!=id || last_topic!=a->doc_topic || last_stage!=*stage) {
        attempts=0;target=id;last_topic=a->doc_topic;last_stage=*stage;
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
        return true;
    }
    test_window_key_mod(a->doc_window,SDLK_TAB,0);--*stage;return false;
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
        if(!documentation_test_seek(a,DOC_GROUP,stage,code))return;
        API_KEY(SDLK_RIGHT);API_KEY(SDLK_RIGHT);break;
    case 2:
        ok=a->doc_group==2;
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
        if(!documentation_test_seek(a,DOC_CONTENTS,stage,code))return;API_KEY(SDLK_RETURN);break;
    case 4:
        ok=a->doc_contents;
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
        if(!documentation_test_seek(a,DOC_TOPICS,stage,code))return;API_KEY(SDLK_RETURN);break;
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
        if(!documentation_test_seek(a,DOC_CONTENTS,stage,code))return;API_KEY(SDLK_RETURN);break;
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
        ok=a->doc_topic==1 && !ps_doc_history_peek(&a->doc_history,1);
        if(!documentation_test_seek(a,DOC_TOPICS,stage,code))return;HISTORY_KEY(SDLK_RETURN);break;
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
        "| **Länge** | `l` in Metern; [C](c-guide.md) |\n\n"
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
        SDL_SetWindowSize(a->doc_window,1080,780);break;
    case 2: {
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
        ps_doc_block *b=&d->blocks[link];clicked=nk_rect(b->hit[0],b->hit[1],b->hit[2],b->hit[3]);
        ok &= clicked.w>20 && clicked.w<150 && clicked.y>=a->doc_body_bounds.y;
        b=&d->blocks[copy];
        printf("DOCUMENTATION INLINE CODE INK: %.0f %.0f %.0f %.0f\n",b->hit[0]+12,b->hit[1]+8,b->hit[2]-24,b->hit[3]-16);
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
        *capture="docs-inline-32-light.bmp";break;
    case 6:
        if(!documentation_test_seek(a,DOC_LINK+external,stage,code))return;
        ok=a->doc_focus_bounds.h<=a->doc_body_bounds.h && a->doc_focus_bounds.y>=a->doc_body_bounds.y &&
           a->documentation->blocks[external].hit[3]>a->doc_body_bounds.h;
        *capture="docs-inline-wrapped-focus.bmp";INLINE_KEY(SDLK_SPACE);break;
    case 7:
        ok=a->doc_topic==guide && documentation_test_clipboard("https://example.org/physim#proof");
        if(!documentation_test_seek(a,DOC_LINK+table_link,stage,code))return;
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
    ps_a11y_model *model=malloc(sizeof *model);if(!model)return false;ps_a11y_init(model);
    size_t total=0,maximum=0;bool ok=true;float viewport[]={0,0,760,540},hidden[]={0,0,0,0};
    for(int i=0;ok && i<DOCUMENTATION_TOPIC_COUNT;i++) {
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
  for(unsigned role=0;role<4;role++) {
   const struct nk_user_font **fonts=role==0?a->doc_ui_fonts:role==1?a->doc_title_fonts:role==2?a->doc_bold_fonts:a->doc_code_fonts;
   for(unsigned i=0;i<(role==2?11u:9u);i++) {
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
 if(ok)printf("DOCUMENTATION MATH GLYPHS: %u characters in 38 font variants\n",count);return ok;
}
static void documentation_math_test_frame(app *a,int *stage,int *code,const char **capture) {
 static unsigned zoom_frames;bool ok=true;switch((*stage)++) {
 case 0:test_window_key_mod(a->window,SDLK_F1,0);break;
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
  const char *source="# Zeichenprobe\n\nθ 🌍 😀 ☃ ᵀ eᵣ ∫\nAaBb **AaBb**\n\n```text\n┌──┬──┐\n│  │  │\n├──┼──┤\n└──┴──┘\n```\n";
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
  *capture="docs-unicode-16.bmp";for(int i=0;i<8;i++)test_window_key_mod(a->doc_window,SDLK_PLUS,PS_UI_COMMAND_MOD);
  zoom_frames=0;break;
 }
 case 13: {
  if(zoom_frames++<2 || a->doc_scroll_pending){--*stage;return;}
  const char *r=strstr(a->documentation->reading,"🌍");
  ok=documentation_font_size(a)==32 && r &&
   nk_sdl_accessibility_document_visible(a->doc_ui,a->doc_reading_version,(size_t)(r-a->documentation->reading));
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
  if(fabsf(a->doc_grid_probe[3]-a->doc_grid_probe[4])>.1f) {
   fprintf(stderr,"Documentation grid row %.3f does not meet source vertical span %.3f\n",a->doc_grid_probe[3],a->doc_grid_probe[4]);ok=false;
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
        ok=a->doc_navigation_hidden && a->doc_body_bounds.w>=before.w+250 && a->doc_body_bounds.h>=before.h+40 &&
           a->doc_reading_version==version && nk_sdl_accessibility_document_visible(a->doc_ui,version,anchor);
        printf("DOCUMENTATION NAVIGATION: before=%.1fx%.1f reader=%.1fx%.1f anchor=%zu\n",before.w,before.h,a->doc_body_bounds.w,a->doc_body_bounds.h,anchor);
        *capture="docs-navigation-reader.bmp";tabs=0;NAV_KEY(SDLK_TAB);break;
    case 6:
        ok=a->doc_focus!=DOC_C && a->doc_focus!=DOC_PHYSIM && a->doc_focus!=DOC_TOPICS &&
           a->doc_focus!=DOC_CONTENTS && a->doc_focus!=DOC_FILTER && a->doc_focus!=DOC_GROUP &&
           !(a->doc_focus>=DOC_TOPIC && a->doc_focus<DOC_COPY);
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
        if(!documentation_test_seek(a,DOC_TOPICS,stage,code))return;
        NAV_KEY(SDLK_RETURN);break;
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
        ok=ok && !nk_sdl_accessibility_focus(a->doc_ui,NULL,"Alle Inhalte durchsuchen") &&
           nk_sdl_accessibility_press(a->doc_ui,"Navigation einblenden");
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
static void documentation_keyboard_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
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
            a->quitting=true;break;
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
        static unsigned settle;bool ok=true;char path[4096];FILE *ready;
        switch(*stage) {
        case 0:test_window_key_mod(a->window,SDLK_F1,0);++*stage;break;
        case 1:SDL_SetWindowSize(a->doc_window,760,540);puts("EDIT SCROLL READY VALUE");fflush(stdout);++*stage;break;
        case 2:
            if(strlen(a->doc_query)<90 || !strstr(a->doc_query,"Anfang θ") || !strstr(a->doc_query,"Ziel ᵣ") ||
               !a->doc_search_active || !nk_sdl_accessibility_is_focused(a->doc_ui,NULL,"Aktuelles Dokument durchsuchen"))return;
            test_window_key_mod(a->doc_window,SDLK_TAB,0);++*stage;break;
        case 3:
            if(settle++<3)return;
            ok=a->doc_focus==DOC_PREVIOUS && !a->doc_search_active && !nk_sdl_accessibility_has_focus(a->doc_ui);
            puts("EDIT SCROLL READY END");fflush(stdout);++*stage;break;
        case 4:
            join(path,sizeof path,a->project,"scroll-end-applied");ready=fopen(path,"rb");if(!ready)return;fclose(ready);
            ok=a->doc_focus==DOC_PREVIOUS && !a->doc_search_active && !nk_sdl_accessibility_has_focus(a->doc_ui);
            *capture="docs-native-edit-scroll-end.bmp";++*stage;break;
        case 5:puts("EDIT SCROLL READY START");fflush(stdout);++*stage;break;
        case 6:
            join(path,sizeof path,a->project,"scroll-start-applied");ready=fopen(path,"rb");if(!ready)return;fclose(ready);
            ok=a->doc_focus==DOC_PREVIOUS && !a->doc_search_active && !nk_sdl_accessibility_has_focus(a->doc_ui);
            *capture="docs-native-edit-scroll-start.bmp";++*stage;break;
        case 7:puts("DOCUMENTATION EDIT SCROLL SELF-TEST: PASSED");a->quitting=true;break;
        }
        if(!ok){fprintf(stderr,"Native edit scroll stage %d changed keyboard focus\n",*stage);*code=1;a->quitting=true;}
        return;
    }
    if(!strcmp(mode,"docs-keyboard-math")){documentation_math_test_frame(a,stage,code,capture);return;}
    if(!strcmp(mode,"docs-keyboard-edit-remote")) {
        static int header;static int settle,experiment_length,analysis_length;static double simulation_time;bool ok=true;
        switch(*stage) {
        case 0:
            experiment_length=a->experiment.string.len;analysis_length=a->analysis.string.len;simulation_time=a->simulation_time;
            test_window_key_mod(a->window,SDLK_F1,0);++*stage;break;
        case 1:
            header=-1;for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++)if(!strcmp(documentation_topics[i].path,"include/physim/core.h"))header=i;
            ok=header>=0;open_documentation(a,header);SDL_SetWindowSize(a->doc_window,760,540);++*stage;break;
        case 2:puts("EDIT READY SEARCH");fflush(stdout);++*stage;break;
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
            puts("EDIT APPLIED GLOBAL");fflush(stdout);test_window_key_mod(a->doc_window,SDLK_TAB,0);++*stage;break;
        case 9:
            ok=a->doc_focus==DOC_GROUP && !a->doc_filter_active;
            ok &= a->experiment.string.len==experiment_length && a->analysis.string.len==analysis_length && a->simulation_time==simulation_time;
            a->quitting=true;puts("DOCUMENTATION EDIT SELF-TEST: PASSED");break;
        }
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
            if(*stage==11){*capture="docs-macos-edit-end.bmp";SDL_HideWindow(a->doc_window);}
            ++*stage;break;
        case 12:
            ok=nk_sdl_accessibility_native_edit_test_step(a->doc_ui,10);SDL_ShowWindow(a->doc_window);++*stage;break;
        case 13:
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
        SDL_SetWindowSize(a->doc_window,760,540);DOC_KEY(SDLK_TAB);break;
    case 2: ok=a->doc_keyboard && a->doc_focus==DOC_C;DOC_KEY(SDLK_RETURN);break;
    case 3: ok=a->doc_topic==DOCUMENTATION_C_GUIDE_TOPIC;DOC_KEY(SDLK_TAB);DOC_KEY(SDLK_RETURN);break;
    case 4: ok=a->doc_topic==DOCUMENTATION_PHYSIM_GUIDE_TOPIC;test_window_key_mod(a->doc_window,SDLK_TAB,SDL_KMOD_SHIFT);DOC_KEY(SDLK_RETURN);break;
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
    case 6: if(!documentation_test_seek(a,DOC_CONTENTS,stage,code))return;DOC_KEY(SDLK_RETURN);break;
    case 7: if(!documentation_test_seek(a,DOC_HEADING+heading,stage,code))return;DOC_KEY(SDLK_RETURN);break;
    case 8: ok=a->doc_scroll_y>0;*capture="docs-keyboard-heading.bmp";break;
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
    case 26: if(!documentation_test_seek(a,DOC_TOPICS,stage,code))return;DOC_KEY(SDLK_RETURN);break;
    case 27: if(!documentation_test_seek(a,DOC_FILTER,stage,code))return;break;
    case 28: {ok=a->doc_filter_active;SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->doc_window);e.text.text="Pendel";SDL_PushEvent(&e);break;}
    case 29: if(a->doc_ui->input.keyboard.text_len){--*stage;return;}ok=!strcmp(a->doc_filter,"Pendel");DOC_KEY(SDLK_TAB);break;
    case 30: ok=a->doc_focus==DOC_GROUP && !a->doc_filter_active;DOC_KEY(SDLK_SPACE);break;
    case 31: ok=!a->doc_filter[0];test_window_key_mod(a->doc_window,SDLK_F,PS_UI_COMMAND_MOD);break;
    case 32: if(a->doc_focus_reveal){--*stage;return;}
        {SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->doc_window);e.text.text="Z";SDL_PushEvent(&e);DOC_KEY(SDLK_TAB);}break;
    case 33: ok=a->doc_focus==DOC_PREVIOUS && !strcmp(a->doc_query,"PhysimZ");DOC_KEY(SDLK_ESCAPE);*stage=24;break;
    case 24: ok=!a->doc_visible && !a->doc_keyboard;a->quitting=true;printf("DOCUMENTATION KEYBOARD SELF-TEST: %s\n",*code?"FAILED":"PASSED");break;
    }
    if(!ok){fprintf(stderr,"Query debug: query=%s active=%d events bytes=%d window=%s edit=%d flags=%u\n",a->doc_query,a->doc_search_active,a->doc_ui->input.keyboard.text_len,a->doc_ui->active?a->doc_ui->active->name_string:"none",a->doc_ui->active?a->doc_ui->active->edit.active:0,a->doc_ui->active?(unsigned)a->doc_ui->active->flags:0);fprintf(stderr,"Documentation keyboard failure stage %d focus=%d topic=%d scroll=%u reveal=%d\n",*stage-1,a->doc_focus,a->doc_topic,a->doc_scroll_y,a->doc_focus_reveal);*code=1;a->quitting=true;}
#undef DOC_KEY
}
