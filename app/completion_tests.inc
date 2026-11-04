#define COMPLETION_C_PRELUDE "// α😀\n#include \"physim/core.h\"\ntypedef ps_vec3 Velocity;\nvoid step(double dt) {\n ps_vec3 velocity = ps_v3(0.0, 1.0, 0.0);\n const char *label = \"label\";\n if (dt) return;\n}\n"
#define COMPLETION_PH_PRELUDE "// α😀\nstruct Particle { var mass: Float64 }\nfunc step(dt: Float64) {\n let velocity: Vec3 = Vec3(0.0, 1.0, 0.0)\n let label = \"label\"\n if true { return }\n}\n"
static const char *completion_c_original[]={COMPLETION_C_PRELUDE "ps_vlOLD\n",COMPLETION_C_PRELUDE "PS_METOLD\n",COMPLETION_C_PRELUDE "ps_memory_alOLD\n"};
static const char *completion_c_changed[]={COMPLETION_C_PRELUDE "ps_vlength\n",COMPLETION_C_PRELUDE "PS_METRE\n",COMPLETION_C_PRELUDE "ps_memory_alignment\n"};
static const char *completion_ph_original[]={COMPLETION_PH_PRELUDE "VecOLD\n",COMPLETION_PH_PRELUDE "Mat4.transOLD\n",COMPLETION_PH_PRELUDE "direction.normOLD\n"};
static const char *completion_ph_changed[]={COMPLETION_PH_PRELUDE "Vec3\n",COMPLETION_PH_PRELUDE "Mat4.translation\n",COMPLETION_PH_PRELUDE "direction.normalized\n"};
#undef COMPLETION_C_PRELUDE
#undef COMPLETION_PH_PRELUDE
static struct {unsigned source;bool list,header,signature,prefix,summary,error,argument,third;unsigned theme;struct nk_text_edit *edit;const ps_completion_symbol *symbol;} completion_ink;
static bool completion_ink_word(const struct nk_command_text *t,const char *word){return t->length==(int)strlen(word) && !memcmp(t->string,word,(size_t)t->length);}
static bool completion_ink_color(struct nk_color a,struct nk_color b){return a.r==b.r && a.g==b.g && a.b==b.b && a.a==b.a;}
static bool completion_ink_region(const struct nk_command_text *t,struct nk_rect r){return t->x>=r.x && t->y>=r.y && t->x<r.x+r.w && t->y<r.y+r.h;}
/* Observe real final draw commands after draw_ui, before GPU submission. Expected
 * colors come independently from the approved palette, not syntax_color(). */
static void completion_syntax_draw(app *a){
    if(!a->completion_active || !a->completion_count){memset(&completion_ink,0,sizeof completion_ink);return;}
    const ps_ui_palette *p=a->preferences.theme==PS_THEME_LIGHT?&PS_UI_LIGHT:a->preferences.theme==PS_THEME_HIGH_CONTRAST?&PS_UI_HIGH_CONTRAST:&PS_UI_DARK;
    const ps_completion_symbol *symbol=ps_completion_at(a->completion_indices[a->completion_selected]);
    if(completion_ink.symbol!=symbol || completion_ink.theme!=(unsigned)a->preferences.theme || completion_ink.edit!=a->completion_edit)memset(&completion_ink,0,sizeof completion_ink);
    completion_ink.symbol=symbol;completion_ink.theme=(unsigned)a->preferences.theme;completion_ink.edit=a->completion_edit;
    struct nk_color selected=symbol->kind==PS_COMPLETION_FUNCTION?p->code_function:symbol->kind==PS_COMPLETION_TYPE?p->code_type:p->code_variable;
    bool language=document_language(a,a->completion_edit)==1;
    struct nk_rect row=a->completion_choice_bounds[a->completion_selected-a->completion_first];row.x+=33;row.w-=33;
    size_t drawn=0;const struct nk_command *cmd;
    nk_foreach(cmd,a->ui)if(cmd->type==NK_COMMAND_TEXT){
        const struct nk_command_text *t=(const struct nk_command_text*)cmd;
        if(completion_ink_region(t,row) && completion_ink_color(t->foreground,selected)){
            drawn+=(size_t)t->length;
            if(t->font->userdata.ptr==a->workbench_marker_source->userdata.ptr && completion_ink_word(t,a->completion_query.prefix))completion_ink.prefix=true;
        }
        if(completion_ink_region(t,a->completion_info_bounds) && t->y<a->completion_signature_bounds.y && completion_ink_word(t,symbol->name) && completion_ink_color(t->foreground,selected))completion_ink.header=true;
        const char *leaf=strrchr(symbol->name,'.');if(!leaf)leaf=symbol->name;else leaf++;
        if(completion_ink_region(t,a->completion_signature_bounds) && completion_ink_word(t,leaf) && completion_ink_color(t->foreground,selected))completion_ink.signature=true;
        const char *summary=language?(a->preferences.theme==0?"Vektor":a->preferences.theme==1?"Affine":"Normiert"):
            a->preferences.theme==0?"Euklidische":a->preferences.theme==1?"Keine Kurzbeschreibung":"UCRT";
        struct nk_color prose=a->preferences.theme==PS_THEME_LIGHT?nk_rgb(32,36,44):p->text;
        if(completion_ink_region(t,a->completion_summary_bounds) && t->length>=(int)strlen(summary) && !memcmp(t->string,summary,strlen(summary)) && completion_ink_color(t->foreground,prose))completion_ink.summary=true;
        if(completion_ink_region(t,a->completion_summary_bounds) && t->length>=13 && !memcmp(t->string,"Dokumentation",13))completion_ink.error=true;
        if(completion_ink_region(t,a->completion_arguments_bounds) && completion_ink_word(t,"z. B. PS_RADIAN"))completion_ink.argument=true;
        if(completion_ink_region(t,a->completion_arguments_bounds) && completion_ink_word(t,"Dritter sichtbarer Parameter"))completion_ink.third=true;
        nk_size offset=(nk_size)((const char*)cmd-(const char*)a->ui->memory.memory.ptr);
        if(offset<a->syntax_commands_begin || offset>=a->syntax_commands_end)continue;
#define INK(word,role,field) if(completion_ink_word(t,word) && completion_ink_color(t->foreground,p->field))completion_ink.source|=1u<<(role)
        INK("=",PS_SYNTAX_TEXT,code_text);
        INK("\"label\"",PS_SYNTAX_STRING,code_string);
        INK(language?"let":"double",PS_SYNTAX_KEYWORD,code_keyword);
        INK(language?"Vec3":"ps_vec3",PS_SYNTAX_TYPE,code_type);
        INK("include",PS_SYNTAX_PREPROCESSOR,code_preprocessor);
        INK("0.0",PS_SYNTAX_NUMBER,code_number);
        INK(language?"step":"ps_v3",PS_SYNTAX_FUNCTION,code_function);
        INK("velocity",PS_SYNTAX_VARIABLE,code_variable);
        INK("if",PS_SYNTAX_CONTROL,code_control);
        if(t->length>=2 && !memcmp(t->string,"//",2) && completion_ink_color(t->foreground,p->code_comment))completion_ink.source|=1u<<PS_SYNTAX_COMMENT;
#undef INK
    }
    completion_ink.list|=drawn==strlen(ps_completion_insertion(&a->completion_query,symbol));
}
static bool completion_reference_visible(app *a){
    struct nk_rect r=a->completion_reference_bounds,b=a->completion_help_body_bounds;
    return r.h>0 && r.y>=b.y && r.y+r.h<=b.y+b.h-1;
}
static void completion_help_test_scroll(app *a,float amount){
    struct nk_rect b=a->completion_help_body_bounds;float x=b.x+b.w/2,y=b.y+b.h/2;
    SDL_Event e={0};e.type=SDL_EVENT_MOUSE_MOTION;e.motion.windowID=SDL_GetWindowID(a->window);e.motion.which=PS_TEST_MOUSE_ID;e.motion.x=x;e.motion.y=y;SDL_PushEvent(&e);
    e=(SDL_Event){0};e.type=SDL_EVENT_MOUSE_WHEEL;e.wheel.windowID=SDL_GetWindowID(a->window);e.wheel.which=PS_TEST_MOUSE_ID;e.wheel.x=0;e.wheel.y=amount;e.wheel.mouse_x=x;e.wheel.mouse_y=y;SDL_PushEvent(&e);
}
static ps_result completion_help_three_rows(void *user,const char *path,ps_document **out){
    (void)user;(void)path;
    const char *text="# Renderer-Prüfdaten\n\n## ps_channel_add\n\nDrei vollständig sichtbare Argumenteinträge prüfen.\n\n| Argument | Renderer-Prüfdaten |\n| --- | --- |\n| context | Erster Eintrag |\n| name | Zweiter Eintrag |\n| unit | Dritter sichtbarer Parameter |\n";
    return ps_document_parse(text,strlen(text),false,out);
}
static bool completion_test_start(app *a,const char *root,const char *mode) {
    bool language=!strcmp(mode,"completion-phys");
    join(a->project_input,sizeof a->project_input,root,"project");a->example=language?8:0;a->template_analysis_language=language?1:0;new_project(a);
    if(!a->loaded)return false;
    const char *names[]={experiment_source(a),analysis_source(a),language?"completion-note.phys":"completion-note.c"};
    for(unsigned i=0;i<3;i++){char path[4096];join(path,sizeof path,a->project,names[i]);FILE *file=fopen(path,"wb");
        if(!file)return false;bool ok=fputs(language?completion_ph_original[i]:completion_c_original[i],file)>=0;ok &= fclose(file)==0;if(!ok)return false;}
    SDL_SetWindowSize(a->window,1080,740);return true;
}
static bool completion_test_equal(struct nk_text_edit *edit,const char *text) {
    size_t length=strlen(text);const char *actual=nk_str_get_const(&edit->string);
    return nk_str_len_char(&edit->string)==(int)length && (!length || (actual && !memcmp(actual,text,length)));
}
static void completion_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static unsigned target,seeks;static bool prepared,captured,reference_drawn;static double reference_due;bool language=!strcmp(mode,"completion-phys"),ok=true;
    static ps_document *reference_owned;static char saved_root[4096];static nk_uint inspector_scroll;static struct nk_vec2 editor_scroll;
    const char *original=language?completion_ph_original[target]:completion_c_original[target];
    const char *changed=language?completion_ph_changed[target]:completion_c_changed[target];
    workspace_document *doc=active_document(a);
    struct nk_text_edit *edit=target==0?&a->experiment:target==1?&a->analysis:doc?&doc->edit:NULL;
    if(a->library_thread)return;
    switch((*stage)++) {
    case 0:
        if(!prepared){ok=open_project(a);prepared=ok;}
        seeks=0;captured=false;
        {ps_preferences p=a->preferences;p.theme=target;p.ui_size=target==1?22:16;p.editor_size=target==1?22:12;preferences_apply(a,&p);}
        if(target==2){char path[4096];join(path,sizeof path,a->project,language?"completion-note.phys":"completion-note.c");preview_workspace_path(a,path);}
        else {a->tab=target?2:0;if(target)a->analysis_tab=1;}
        break;
    case 1:ok=edit!=NULL;test_mouse(a,nk_rect(a->source_editor_bounds.x+20,a->source_editor_bounds.y+20,2,2),true);break;
    case 2:test_mouse(a,nk_rect(a->source_editor_bounds.x+20,a->source_editor_bounds.y+20,2,2),false);break;
    case 3:
        ok=completion_test_equal(edit,original);edit->cursor=nk_utf_len(original,(int)strlen(original)-4);edit->select_start=edit->select_end=edit->cursor;
        if(target==2){
#ifdef __APPLE__
            if(!native_test_command(a,TOOLBAR_COMPLETION,stage,&ok))return;
            *stage=40;
#else
            test_mouse(a,a->toolbar_bounds[2],true);*stage=35;
#endif
        }
        else test_window_key_mod(a->window,SDLK_SPACE,SDL_KMOD_CTRL);
        break;
    case 35:test_mouse(a,a->toolbar_bounds[2],false);break;
    case 36:ok=a->toolbar_menu==3;test_mouse(a,a->completion_button_bounds,true);break;
    case 37:test_mouse(a,a->completion_button_bounds,false);*stage=40;break;
    case 40:*stage=4;break;
    case 4: {
        const char *desired=language?(target==0?"Vec3":target==1?"Mat4.translation":"Vec3.normalized"):(target==0?"ps_vlength":target==1?"PS_METRE":"ps_memory_alignment");
        ok=a->completion_active && a->completion_count>0;
        if(ok && strcmp(ps_completion_at(a->completion_indices[a->completion_selected])->name,desired)) {
            if(++seeks>ps_completion_count()){ok=false;break;}
            test_window_key_mod(a->window,SDLK_DOWN,0);--*stage;return;
        }
        if(ok && !a->completion_info_visible)test_window_key_mod(a->window,SDLK_SPACE,SDL_KMOD_CTRL);
        break;
    }
    case 5:
        if(!captured){*capture=target==0?"completion-experiment.bmp":target==1?"completion-analysis.bmp":"completion-document.bmp";captured=true;reference_due=ps_clock()+4;}
        if(!completion_ink.signature || !completion_ink.summary || !completion_reference_visible(a)){
            if(ps_clock()<reference_due){completion_help_test_scroll(a,-3);--*stage;return;}
        }
        {unsigned expected=(1u<<PS_SYNTAX_COUNT)-1;if(language)expected&=~(1u<<PS_SYNTAX_PREPROCESSOR);
        ok=(completion_ink.source&expected)==expected && completion_ink.list && completion_ink.header && completion_ink.signature && completion_ink.prefix;
        printf("COMPLETION SYNTAX target=%u theme=%u source=%u expected=%u list=%d header=%d signature=%d prefix-bold=%d\n",target,a->preferences.theme,completion_ink.source,expected,completion_ink.list,completion_ink.header,completion_ink.signature,completion_ink.prefix);}
        ok=ok && completion_ink.summary && completion_reference_visible(a) && a->completion_help.status==PS_OK &&
            a->completion_help.document && a->completion_help.document!=a->api_reference && a->completion_help.symbol==completion_ink.symbol;
        if(a->completion_help_natural_height+28<=260)ok=ok && a->completion_help_scroll_y==0;
        printf("COMPLETION HELP target=%u summary=%d own-document=%d reference-visible=%d scroll=%u\n",target,completion_ink.summary,a->completion_help.document!=a->api_reference,completion_reference_visible(a),a->completion_help_scroll_y);
        if(target==2)test_mouse(a,a->completion_choice_bounds[a->completion_selected-a->completion_first],true);
        else test_window_key_mod(a->window,target?SDLK_TAB:SDLK_RETURN,0);
        break;
    case 6:if(target==2){test_mouse(a,a->completion_choice_bounds[a->completion_selected-a->completion_first],false);*stage=16;}break;
    case 16:*stage=17;break;
    case 17:*stage=7;break;
    case 7:
        ok=completion_test_equal(edit,changed) && !a->completion_active && (target==0?a->dirty:target==1?a->analysis_dirty:doc->dirty);
        test_window_key_mod(a->window,SDLK_Z,PS_UI_COMMAND_MOD);break;
    case 8:break;
    case 9:ok=completion_test_equal(edit,original);test_window_key_mod(a->window,SDLK_Z,PS_UI_COMMAND_MOD|SDL_KMOD_SHIFT);break;
    case 10:break;
    case 11:ok=completion_test_equal(edit,changed);test_window_key_mod(a->window,SDLK_Z,PS_UI_COMMAND_MOD);break;
    case 12:break;
    case 13:ok=completion_test_equal(edit,original);if(target<2){target++;*stage=0;}break;
    case 14:
        {const char *text=language?"// α\nMat":"// α\nps_v";ok=set_editor_text(edit,text,strlen(text));
        edit->cursor=nk_utf_len(text,(int)strlen(text));edit->select_start=edit->select_end=edit->cursor;}
        test_window_key_mod(a->window,SDLK_SPACE,SDL_KMOD_CTRL);break;
    case 15: {
        ok=a->completion_active;SDL_Event event={0};event.type=SDL_EVENT_TEXT_INPUT;
        event.text.windowID=SDL_GetWindowID(a->window);event.text.text=language?"4.trans":"leng";SDL_PushEvent(&event);
        test_window_key_mod(a->window,SDLK_RETURN,0);*stage=22;break;
    }
    case 22:*stage=23;break;
    case 23:
        ok=!a->completion_active && completion_test_equal(edit,language?"// α\nMat4.translation":"// α\nps_vlength");
        test_window_key_mod(a->window,SDLK_Z,PS_UI_COMMAND_MOD);break;
    case 24:*stage=25;break;
    case 25:
        ok=completion_test_equal(edit,language?"// α\nMat4.trans":"// α\nps_vleng");
        test_window_key_mod(a->window,SDLK_Z,PS_UI_COMMAND_MOD|SDL_KMOD_SHIFT);break;
    case 26:break;
    case 27:
        ok=completion_test_equal(edit,language?"// α\nMat4.translation":"// α\nps_vlength");
        test_window_key_mod(a->window,SDLK_SPACE,SDL_KMOD_CTRL);break;
    case 28:ok=a->completion_active;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 29:break;
    case 30:test_window_key_mod(a->window,SDLK_Z,PS_UI_COMMAND_MOD);break;
    case 31:break;
    case 32:ok=completion_test_equal(edit,language?"// α\nMat4.trans":"// α\nps_vleng");break;
    case 33:
        ok=set_editor_text(edit,"\"ps_vl\"",7);edit->cursor=6;edit->select_start=edit->select_end=6;
        test_window_key_mod(a->window,SDLK_SPACE,SDL_KMOD_CTRL);break;
    case 34:
        ok=!a->completion_active && completion_test_equal(edit,"\"ps_vl\"");
        ok &= set_editor_text(edit,"",0);edit->cursor=edit->select_start=edit->select_end=0;
        test_window_key_mod(a->window,SDLK_SPACE,SDL_KMOD_CTRL);*stage=38;break;
    case 38:ok=a->completion_active && a->completion_count>0;test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    case 39:{const char *name=language?"Mat4.translation":"ps_vlength";
        ok=!a->completion_active && completion_test_equal(edit,"");ok=ok && set_editor_text(edit,name,strlen(name));
        edit->cursor=edit->select_start=edit->select_end=(int)strlen(name);test_window_key_mod(a->window,SDLK_SPACE,SDL_KMOD_CTRL);reference_due=ps_clock()+3;reference_drawn=false;*stage=41;break;}
    case 41:
        ok=a->completion_active;
        if(ok && !a->completion_info_visible){if(ps_clock()<reference_due){test_window_key_mod(a->window,SDLK_SPACE,SDL_KMOD_CTRL);--*stage;return;}ok=false;}
        if(ok && !reference_drawn && (completion_ink.symbol!=ps_completion_at(a->completion_indices[a->completion_selected]) || !completion_ink.header || !completion_ink.signature)){
            if(ps_clock()<reference_due){--*stage;return;}ok=false;
        }
        if(ok){reference_drawn=true;if(!completion_reference_visible(a)){
            if(ps_clock()<reference_due){completion_help_test_scroll(a,-4);--*stage;return;}ok=false;}}
        if(ok)test_mouse(a,a->completion_reference_bounds,true);break;
    case 42:test_mouse(a,a->completion_reference_bounds,false);break;
    case 43:ok=!a->completion_active && a->api_reference && !strcmp(a->api_reference_name,language?"Mat4.translation":"ps_vlength") && completion_test_equal(edit,language?"Mat4.translation":"ps_vlength");
        if(ok){puts("COMPLETION REFERENCE: pointer=1 source-preserved=1");reference_owned=a->api_reference;
            const char *name=language?"Vec3":"ps_channel_add";ok=set_editor_text(edit,name,strlen(name));
            edit->cursor=edit->select_start=edit->select_end=(int)strlen(name);test_window_key_mod(a->window,SDLK_SPACE,SDL_KMOD_CTRL);reference_due=ps_clock()+3;}break;
    case 44:
        if(!a->completion_info_visible){if(ps_clock()<reference_due){test_window_key_mod(a->window,SDLK_SPACE,SDL_KMOD_CTRL);--*stage;return;}ok=false;break;}
        if(a->completion_help.symbol!=completion_ink.symbol || !a->completion_help.attempted){if(ps_clock()<reference_due){--*stage;return;}ok=false;break;}
        ok=a->completion_active && a->completion_help.status==PS_OK && a->completion_help.document &&
            a->completion_help.document!=reference_owned && a->api_reference==reference_owned && completion_test_equal(edit,language?"Vec3":"ps_channel_add");
        inspector_scroll=a->api_reference_scroll_y;editor_scroll=edit->scrollbar;break;
    case 45:
        if(!completion_reference_visible(a)){if(ps_clock()<reference_due){completion_help_test_scroll(a,-4);--*stage;return;}ok=false;break;}
        ok=a->api_reference_scroll_y==inspector_scroll && edit->scrollbar.y==editor_scroll.y && edit->scrollbar.x==editor_scroll.x;
        if(!language){ps_completion_help *h=&a->completion_help;size_t rows=0;
            ok=ok && h->preview.arguments!=SIZE_MAX;
            if(ok)for(size_t i=h->preview.arguments+1;i<h->end;i++){const ps_doc_block *b=&h->document->blocks[i];if(b->kind!=PS_DOC_TABLE_ROW || b->table_first!=h->preview.arguments)break;rows++;}
            ok=ok && rows==2 && completion_ink.argument && a->completion_arguments_bounds.h>0 && a->completion_arguments_bounds.y+a->completion_arguments_bounds.h<=a->completion_help_body_bounds.y+a->completion_help_body_bounds.h;
            if(ok)printf("COMPLETION HELP ARGUMENTS: rows=%zu last-visible=1 inspector-preserved=1 scroll=%u\n",rows,a->completion_help_scroll_y);
            if(ok){reference_due=ps_clock()+3;*stage=60;}
        }break;
    case 60:{
        const ps_completion_symbol *symbol=a->completion_help.symbol;ps_completion_help_destroy(&a->completion_help);
        ok=ps_completion_help_select(&a->completion_help,symbol,completion_help_three_rows,NULL)==PS_OK;
        a->completion_help_scroll_x=a->completion_help_scroll_y=0;completion_ink.third=false;break;}
    case 61:
        if(!completion_reference_visible(a) || !completion_ink.third){
            if(ps_clock()<reference_due){completion_help_test_scroll(a,-4);--*stage;return;}ok=false;break;}
        ok=a->completion_help.preview.arguments!=SIZE_MAX && a->completion_help.document->blocks[a->completion_help.preview.arguments].table_rows==4 && a->api_reference==reference_owned;
        if(ok)puts("COMPLETION HELP TABLE FIXTURE: rows=3 last-painted=1 inspector-preserved=1");
        *stage=46;break;
    case 46:
        completion_close(a);snprintf(saved_root,sizeof saved_root,"%s",a->root);
        snprintf(a->root,sizeof a->root,"%s/no-completion-help-reference",saved_root);
        test_window_key_mod(a->window,SDLK_SPACE,SDL_KMOD_CTRL);reference_due=ps_clock()+3;break;
    case 47:
        if(!a->completion_info_visible){if(ps_clock()<reference_due){test_window_key_mod(a->window,SDLK_SPACE,SDL_KMOD_CTRL);--*stage;return;}ok=false;break;}
        if(!a->completion_help.attempted){if(ps_clock()<reference_due){--*stage;return;}ok=false;break;}
        ok=a->completion_active && a->completion_help.status!=PS_OK && !a->completion_help.document &&
            a->completion_help.first==0 && a->completion_help.end==0 && a->api_reference==reference_owned;break;
    case 48:
        if(!completion_ink.error || !completion_reference_visible(a)){
            if(ps_clock()<reference_due){completion_help_test_scroll(a,-4);--*stage;return;}ok=false;break;}
        *capture="completion-reference-error.bmp";test_mouse(a,a->completion_reference_bounds,true);break;
    case 49:test_mouse(a,a->completion_reference_bounds,false);break;
    case 50:
        ok=a->completion_active && a->api_reference==reference_owned && completion_test_equal(edit,language?"Vec3":"ps_channel_add");
        snprintf(a->root,sizeof a->root,"%s",saved_root);completion_close(a);
        test_window_key_mod(a->window,SDLK_SPACE,SDL_KMOD_CTRL);reference_due=ps_clock()+3;break;
    case 51:
        if(!a->completion_info_visible){if(ps_clock()<reference_due){test_window_key_mod(a->window,SDLK_SPACE,SDL_KMOD_CTRL);--*stage;return;}ok=false;break;}
        if(!a->completion_help.attempted){if(ps_clock()<reference_due){--*stage;return;}ok=false;break;}
        ok=a->completion_active && a->completion_help.status==PS_OK && a->completion_help.document && a->api_reference==reference_owned;
        if(ok)puts("COMPLETION HELP FAILURE: visible=1 disabled-reference=1 inspector-preserved=1 retry-after-close=1");
        completion_close(a);a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Completion UI target=%u stage=%d active=%u cursor=%d text=%.*s help-status=%d scroll=%u footer=%d natural=%.1f body=%.1f/%.1f ref=%.1f/%.1f arg=%.1f/%.1f painted=%d\n",target,*stage-1,a->completion_active,edit?edit->cursor:-1,edit?nk_str_len_char(&edit->string):7,edit?nk_str_get_const(&edit->string):"missing",a->completion_help.status,a->completion_help_scroll_y,completion_reference_visible(a),a->completion_help_natural_height,a->completion_help_body_bounds.y,a->completion_help_body_bounds.h,a->completion_reference_bounds.y,a->completion_reference_bounds.h,a->completion_arguments_bounds.y,a->completion_arguments_bounds.h,completion_ink.argument);*code=1;a->quitting=true;}
    if(a->quitting)printf("COMPLETION SELF-TEST: %s\n",*code?"FAILED":"PASSED");
}
