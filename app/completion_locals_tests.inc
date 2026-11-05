/* Actual indented Physim source: no brace-syntax/browser scope substitute. */
#define LOCALS_SOURCE "let amplitude: Float64 = 1.0\nfunc closed(foreign: Float64):\n    let gone: Float64 = foreign\nfunc step(dt: Float64):\n    let velocity: Vec3 = Vec3(0.0, 1.0, 0.0)\n    let gravity: Vec3 = Vec3(0.0, -9.81, 0.0)\n    var damping: Float64 = 0.02\n    let Größe: Float64 = dt\n    let sin: Float64 = dt\n    if true:\n        let amplitude: String = \"shadow\"\n        print"
#define C_LOCALS_SOURCE "double amplitude=1;\nvoid closed(double foreign){double gone=foreign;}\nvoid step(double dt){\n ps_vec3 velocity=ps_v3(0,1,0);\n ps_vec3 gravity=ps_v3(0,-9.81,0);\n double damping=.02, Größe=dt, sin=dt;\n if(dt){const char *amplitude=\"shadow\"; ps_vadd"
static bool locals_c_mode;
static const char *locals_source(void){return locals_c_mode?C_LOCALS_SOURCE:LOCALS_SOURCE;}
static unsigned locals_draw_generation,locals_wait_generation;
static struct {bool list,prefix,signature,source,kind,no_reference;} locals_ink;
static void completion_locals_draw(app *a){
    locals_draw_generation++;memset(&locals_ink,0,sizeof locals_ink);
    if(!a->completion_active || !a->completion_count)return;
    const ps_completion_symbol *symbol=completion_symbol(a,a->completion_indices[a->completion_selected]);
    const ps_completion_local *local=completion_local(a,symbol);if(!local)return;
    const ps_ui_palette *palette=a->preferences.theme==PS_THEME_LIGHT?&PS_UI_LIGHT:a->preferences.theme==PS_THEME_HIGH_CONTRAST?&PS_UI_HIGH_CONTRAST:&PS_UI_DARK;
    char source[4160];completion_source_label(a,a->completion_edit,local,source);
    struct nk_rect row=a->completion_choice_bounds[a->completion_selected-a->completion_first];
    const struct nk_command *cmd;
    nk_foreach(cmd,a->ui)if(cmd->type==NK_COMMAND_TEXT){
        const struct nk_command_text *t=(const struct nk_command_text*)cmd;
        if(completion_ink_region(t,row) && completion_ink_color(t->foreground,palette->code_variable)){
            if(completion_ink_word(t,"x"))locals_ink.list=true;
            if(completion_ink_word(t,a->completion_query.prefix) && t->font->userdata.ptr==a->workbench_marker_source->userdata.ptr)locals_ink.prefix=true;
        }
        if(completion_ink_region(t,a->completion_signature_bounds) && completion_ink_word(t,symbol->name) &&
           completion_ink_color(t->foreground,palette->code_variable))locals_ink.signature=true;
        if(completion_ink_region(t,a->completion_info_bounds)){
            if(completion_ink_word(t,source))locals_ink.source=true;
            if(completion_ink_word(t,"Variable"))locals_ink.kind=true;
        }
    }
    locals_ink.no_reference=!a->completion_reference_bounds.w && !a->completion_arguments_bounds.w && !a->completion_help.document;
}
static bool completion_locals_seed(app *a,struct nk_text_edit *edit,const char *tail,const char *caret){
    char text[2048];snprintf(text,sizeof text,"%s%s",locals_source(),tail);
    if(!completion_info_seed(a,edit,text))return false;
    const char *at=caret?strstr(text,caret):NULL;
    if(at)edit->cursor=edit->select_start=edit->select_end=nk_utf_len(text,(int)(at-text));
    locals_wait_generation=locals_draw_generation+1;return true;
}
static bool completion_locals_choose(app *a,const char *name){
    for(size_t i=0;i<a->completion_count;i++)if(!strcmp(completion_symbol(a,a->completion_indices[i])->name,name)){
        a->completion_selected=i;a->completion_reveal=true;a->completion_explicit=true;completion_info_request(a,true);return true;
    }return false;
}
static void completion_locals_frame(app *a,bool c_mode,int *stage,int *code,const char **capture){
    locals_c_mode=c_mode;
    static unsigned target;static bool prepared;static double timeout;static ps_document *inspector;
    static char original[2048],changed[2048];bool ok=true;
    workspace_document *doc=active_document(a);
    struct nk_text_edit *edit=target==0?&a->experiment:target==1?&a->analysis:doc?&doc->edit:NULL;
    if(a->library_thread || locals_draw_generation<locals_wait_generation)return;
#define LOCAL_NEXT(n) do{*stage=(n);timeout=ps_clock()+4;}while(0)
#define LOCAL_WAIT(x) do{if(!(x)){if(ps_clock()<timeout)return;ok=false;goto finished;}}while(0)
#define LOCAL_KEY(k,m) do{test_window_key_mod(a->window,k,m);locals_wait_generation=locals_draw_generation+2;}while(0)
    switch(*stage){
    case 0:
        if(!prepared){ok=open_project(a);prepared=ok;}
        {ps_preferences p=a->preferences;p.theme=target;p.completion_rows=5;p.completion_auto=true;p.ui_size=target==1?22:16;p.editor_size=target==1?22:12;preferences_apply(a,&p);}
        if(target==2){char path[4096];join(path,sizeof path,a->project,c_mode?"completion-note.c":"completion-note.phys");preview_workspace_path(a,path);}
        else{a->tab=target?2:0;if(target)a->analysis_tab=1;}
        ok=ok && api_reference_open(a,ps_completion_lookup(c_mode?"ps_vec3":"Vec3",c_mode?7:4,!c_mode));inspector=a->api_reference;
        locals_wait_generation=locals_draw_generation+1;LOCAL_NEXT(1);break;
    case 1:
        ok=completion_locals_seed(a,edit,c_mode?"(GröOLD, gravity); double later=2; }}":"(GröOLD, gravity)\n        let later: Float64 = 2.0\n","OLD");
        snprintf(original,sizeof original,"%s%s",locals_source(),c_mode?"(GröOLD, gravity); double later=2; }}":"(GröOLD, gravity)\n        let later: Float64 = 2.0\n");
        snprintf(changed,sizeof changed,"%s%s",locals_source(),c_mode?"(Größe, gravity); double later=2; }}":"(Größe, gravity)\n        let later: Float64 = 2.0\n");LOCAL_NEXT(2);break;
    case 2:LOCAL_WAIT(edit && edit->active);LOCAL_KEY(SDLK_SPACE,SDL_KMOD_CTRL);LOCAL_NEXT(3);break;
    case 3:
        LOCAL_WAIT(a->completion_active && a->completion_list_bounds.w>0);
        ok=a->completion_count==1 && !strcmp(completion_symbol(a,a->completion_indices[0])->name,"Größe") && a->completion_locals.call.active;
        LOCAL_KEY(SDLK_SPACE,SDL_KMOD_CTRL);LOCAL_NEXT(4);break;
    case 4:
        LOCAL_WAIT(a->completion_info_visible && locals_ink.signature && locals_ink.source);
        ok=locals_ink.list && locals_ink.prefix && locals_ink.kind && locals_ink.no_reference && a->api_reference==inspector &&
           ps_ui_font_has_glyph(a->workbench_marker_source,0xf6) && ps_ui_font_has_glyph(a->workbench_marker_source,0xdf) &&
           completion_test_equal(edit,original);
        if(c_mode)*capture=target==0?"locals-model.bmp":target==1?"locals-analysis.bmp":"locals-document.bmp";
        printf("COMPLETION LOCALS target=%u theme=%u unicode-prefix=1 actual-semibold=1 variable-list=1 signature=1 source=1 no-fake-reference=1\n",target,target);
        LOCAL_KEY(SDLK_F1,SDL_KMOD_SHIFT);LOCAL_NEXT(5);break;
    case 5:
        ok=a->completion_active && a->api_reference==inspector && completion_test_equal(edit,original);
        LOCAL_KEY(SDLK_TAB,0);LOCAL_NEXT(6);break;
    case 6:
        LOCAL_WAIT(!a->completion_active && completion_test_equal(edit,changed));
        LOCAL_KEY(SDLK_Z,PS_UI_COMMAND_MOD);LOCAL_NEXT(7);break;
    case 7:
        LOCAL_WAIT(completion_test_equal(edit,original));
        ok=completion_locals_seed(a,edit,c_mode?"(siOLD, gravity); }}":"(siOLD, gravity)","OLD");LOCAL_NEXT(8);break;
    case 8:LOCAL_WAIT(edit->active);LOCAL_KEY(SDLK_SPACE,SDL_KMOD_CTRL);LOCAL_NEXT(9);break;
    case 9:
        LOCAL_WAIT(a->completion_active);ok=completion_locals_choose(a,"sin");
        {unsigned named=0;for(size_t i=0;i<a->completion_count;i++)if(!strcmp(completion_symbol(a,a->completion_indices[i])->name,"sin"))named++;
        ok=ok && named==1 && completion_symbol(a,a->completion_indices[0])->kind==PS_COMPLETION_VARIABLE;}
        locals_wait_generation=locals_draw_generation+1;LOCAL_NEXT(10);break;
    case 10:
        LOCAL_WAIT(locals_ink.signature && locals_ink.source);
        ok=locals_ink.no_reference && a->api_reference==inspector;
        ok=ok && completion_locals_seed(a,edit,c_mode?"(sin, gravity); }}":"(sin, gravity)",", gravity");LOCAL_NEXT(20);break;
    case 20:LOCAL_WAIT(edit->active);LOCAL_KEY(SDLK_F1,SDL_KMOD_SHIFT);LOCAL_NEXT(21);break;
    case 21:
        ok=!a->completion_active && a->api_reference==inspector;
        ok=ok && completion_locals_seed(a,edit,"",NULL);LOCAL_NEXT(11);break;
    case 11:LOCAL_WAIT(edit->active);completion_auto_text(a,"(");locals_wait_generation=locals_draw_generation+2;LOCAL_NEXT(12);break;
    case 12:
        LOCAL_WAIT(a->completion_active && a->completion_list_bounds.w>0);
        ok=!a->completion_manual && !*a->completion_query.prefix && a->completion_locals.call.active && a->completion_locals.call.at_start &&
            a->completion_locals.count==7 && a->completion_count>7 && a->completion_visible_rows==5 && a->completion_scroll_bounds.h>0;
        for(size_t i=0;i<a->completion_locals.count;i++)ok=ok && completion_symbol(a,a->completion_indices[i])->kind==PS_COMPLETION_VARIABLE;
        ok=ok && !ps_completion_locals_find(&a->completion_locals,"gone",4) && !ps_completion_locals_find(&a->completion_locals,"foreign",7) &&
            !ps_completion_locals_find(&a->completion_locals,"later",5) && !strcmp(ps_completion_locals_find(&a->completion_locals,"amplitude",9)->type,c_mode?"const char *":"String");
        ok=ok && completion_locals_choose(a,"sin");locals_wait_generation=locals_draw_generation+1;LOCAL_NEXT(13);break;
    case 13:
        LOCAL_WAIT(locals_ink.signature);ok=a->completion_selected>=5 && a->completion_first>0;
        LOCAL_KEY(SDLK_RETURN,0);LOCAL_NEXT(14);break;
    case 14:{
        char text[2048];snprintf(text,sizeof text,"%s(sin",locals_source());
        LOCAL_WAIT(!a->completion_active && completion_test_equal(edit,text));
        LOCAL_KEY(SDLK_Z,PS_UI_COMMAND_MOD);LOCAL_NEXT(15);break;}
    case 15:{
        char text[2048];snprintf(text,sizeof text,"%s(",locals_source());LOCAL_WAIT(completion_test_equal(edit,text));
        completion_auto_text(a,"velocity,");locals_wait_generation=locals_draw_generation+2;LOCAL_NEXT(16);break;}
    case 16:
        LOCAL_WAIT(a->completion_active && a->completion_locals.call.argument==1 && a->completion_locals.call.at_start);
        ok=!a->completion_manual && a->completion_locals.count==7 && a->api_reference==inspector;
        ok=ok && a->completion_locals.language==!c_mode;
        printf("COMPLETION LOCALS language=%s target=%u actual-model-language=1\n",c_mode?"c":"phys",target);
        printf("COMPLETION LOCALS target=%u exact-replace=1 undo=1 shadow-catalog=1 nearest-scope=1 exclude-foreign-future=1 locals-first=1 all-results=1 empty-open=1 empty-comma=1\n",target);
        completion_close(a);
        if(target<2){target++;LOCAL_NEXT(0);}else a->quitting=true;
        break;
    }
finished:
    if(!ok){fprintf(stderr,"Completion locals target=%u stage=%d active=%d count=%zu locals=%zu selected=%zu first=%zu call=%d/%zu/%d ink=%d/%d/%d/%d/%d/%d cursor=%d\n",
        target,*stage,a->completion_active,a->completion_count,a->completion_locals.count,a->completion_selected,a->completion_first,
        a->completion_locals.call.active,a->completion_locals.call.argument,a->completion_locals.call.at_start,
        locals_ink.list,locals_ink.prefix,locals_ink.signature,locals_ink.source,locals_ink.kind,locals_ink.no_reference,edit?edit->cursor:-1);*code=1;a->quitting=true;}
    if(a->quitting)printf("COMPLETION LOCALS SELF-TEST: %s\n",*code?"FAILED":"PASSED");
#undef LOCAL_NEXT
#undef LOCAL_WAIT
#undef LOCAL_KEY
}
#undef LOCALS_SOURCE
#undef C_LOCALS_SOURCE
