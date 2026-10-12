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
        {char *text=SDL_GetClipboardText();ok=text && !strcmp(text,a->documentation->text+a->documentation->blocks[example].text);SDL_free(text);}
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
        {char *copy=SDL_GetClipboardText();ok=copy && !strcmp(copy,a->documentation->text+a->documentation->blocks[0].text);SDL_free(copy);}
        if(!documentation_test_seek(a,DOC_BODY,stage,code))return;ANCHOR_KEY(SDLK_PAGEDOWN);break;
    case 13:
        ok=a->doc_scroll_y>0 && !a->doc_error[0];*capture="docs-large-source-scrolled.bmp";a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Documentation anchor stage %d failed: topic=%d scroll=%u error=%s\n",*stage-1,a->doc_topic,a->doc_scroll_y,a->doc_error);*code=1;a->quitting=true;}
    if(a->quitting)printf("DOCUMENTATION ANCHOR SELF-TEST: %s\n",*code?"FAILED":"PASSED");
#undef ANCHOR_KEY
}
static void documentation_keyboard_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
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
        ok=a->doc_visible && tutorial_topic>=0;SDL_SetWindowSize(a->doc_window,760,540);DOC_KEY(SDLK_TAB);break;
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
    case 10: {char *text=SDL_GetClipboardText();ok=text && !strcmp(text,a->documentation->text+a->documentation->blocks[copy_block].text);SDL_free(text);break;}
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
