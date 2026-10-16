/* Mixed prose, math, Unicode and link fixture; executable UI checks below. */
static const char documentation_inline_math_fixture[]=
"# Inline-Formeln\n\nVor θ🌍 $\\nu=\\frac\\mu\\rho$ danach.\n\nVor θ🌍 **$E=\\frac12mv^2$** danach.\n\nVor θ🌍 $\\mathbf v=L\\omega\\begin{pmatrix}\\cos\\theta\\\\\\sin\\theta\\\\0\\end{pmatrix}$ danach.\n\nVor θ🌍 $|a-b|\\le\\varepsilon_{\\mathrm{abs}}+\\varepsilon_{\\mathrm{rel}}\\cdot\\max(|a|,|b|)$ danach.\n\nVor θ🌍 $\\mathbf v(\\tau)=\\frac{\\mathbf B'(\\tau/T)}{T}$ danach.\n\nVor θ🌍 $T_0=2\\pi\\sqrt{\\frac{L}{g}}$ danach.\n\n$z=0$ $\\dot{y}=v$ $\\int c\\,\\mathrm{d}x$ $v=at$ $\\nu=\\frac\\mu\\rho$ $I=\\frac{V}{R}$ $\\omega=ck$ $\\frac{\\mathrm{d}\\mathbf{r}}{\\mathrm{d}t}=\\mathbf{v}$ $V=IR$ $\\alpha=\\frac{c}{2m}$ $C=mc$ $P=\\frac{V^2}{R}$ $\\mathbf p=m\\mathbf v$ $G=\\frac{kA}{L}$ $v_{\\infty}=\\frac{a}{\\lambda}$ $\\lambda=\\frac{c\\Delta t}{\\Delta x}$ $b=\\frac{fd}{d-f}$ $f=\\frac{V_{\\mathrm{sub}}}{V_K}$ $x_{\\mathrm{SI}}=xs$ $c=\\sqrt{\\frac{T}{\\mu}}$\n\nVor [$x=L\\sin\\theta$](units.md) danach.\n";
static void documentation_inline_math_test_frame(app *a,int *stage,int *code,const char **capture) {
    static size_t blocks[8];static unsigned sample,frames;static int editor,analysis;
    static double simulation;static char image[80];bool okay=true;
    static unsigned resizes,themes;static double started;
    switch((*stage)++) {
    case 0:editor=a->experiment.string.len;analysis=a->analysis.string.len;simulation=a->simulation_time;
        resizes=themes=0;started=ps_clock();
        test_window_key_mod(a->window,SDLK_F1,0);break;
    case 1: {
        ps_document *document=NULL;
        okay=ps_document_parse(documentation_inline_math_fixture,strlen(documentation_inline_math_fixture),false,&document)==PS_OK;
        if(!okay)break;
        free(a->documentation);a->documentation=document;++a->doc_reading_version;
        document->layout_width=0;a->doc_query[0]=a->doc_last_query[0]=0;a->doc_move_match=false;
        unsigned count=0;
        for(size_t i=0;i<document->count;i++) {
            const ps_doc_block *b=&document->blocks[i];bool math=false;
            for(size_t j=0;j<b->span_count;j++)if(document->spans[b->first_span+j].formula)math=true;
            if(math){if(count<8)blocks[count]=i;++count;}
        }
        okay=count==8 && strstr(document->reading,"Vor θ🌍 ν = μ/ρ danach.") && !strstr(document->reading,"\\frac");
        sample=0;break;
    }
    case 2: {
        if(!sample)documentation_zoom(a,0);
        if(sample==24)for(int i=0;i<8;i++)documentation_zoom(a,1);
        int width=sample<24?1100:760,height=sample<24?800:540;
        if(!sample || sample==24) {
            okay=SDL_SetWindowSize(a->doc_window,width,height) && SDL_SyncWindow(a->doc_window);++resizes;
        }
        int actual_width=0,actual_height=0;
        okay &= SDL_GetWindowSize(a->doc_window,&actual_width,&actual_height) && actual_width==width && actual_height==height;
        if(sample%8==0){ps_preferences p=a->preferences;p.theme=(sample/8)%3;preferences_apply(a,&p);++themes;}
        okay &= a->preferences.theme==(sample/8)%3;
        a->doc_navigation_hidden=sample%8==6;frames=0;break;
    }
    case 3:
        if(frames++<3){--*stage;return;}
        a->doc_jump_block=(int)blocks[sample%8];frames=0;break;
    case 4:
        if(frames++<3 || a->doc_scroll_pending || a->doc_jump_block>=0){--*stage;return;}
        a->doc_inline_math_test_block=blocks[sample%8];a->doc_inline_math_test_case=sample;
        a->doc_inline_math_test_requested=true;
        snprintf(image,sizeof image,"docs-inline-math-%02u.bmp",sample);*capture=image;break;
    case 5:
        okay=a->doc_inline_math_test_ok && documentation_font_size(a)==(sample<24?16:32) &&
            a->experiment.string.len==editor && a->analysis.string.len==analysis && a->simulation_time==simulation;
        if(sample%8==6) {
            okay &= a->doc_math && a->doc_math->count>=32;
            printf("INLINE MATH CACHE: case=%u twenty simultaneous textures retained=%d\n",sample,okay);
        }
        if(++sample<48)*stage=2;
        else test_window_mouse(a->doc_window,a->doc_inline_math_test_link,true);
        break;
    case 6:test_window_mouse(a->doc_window,a->doc_inline_math_test_link,false);frames=0;break;
    case 7:
        if(frames++<3){--*stage;return;}
        okay=!strcmp(documentation_topics[a->doc_topic].path,"docs/units.md") &&
            a->experiment.string.len==editor && a->analysis.string.len==analysis && a->simulation_time==simulation;
        *capture="docs-inline-math-link.bmp";a->quitting=true;break;
    }
    if(!okay){fprintf(stderr,"Inline math stage %d case %u failed\n",*stage-1,sample);*code=1;a->quitting=true;}
    if(a->quitting) {
        printf("INLINE MATH TRANSITIONS: resizes=%u themes=%u elapsed=%.3f\n",resizes,themes,ps_clock()-started);
        printf("INLINE MATH SELF-TEST: %s count=%u\n",*code?"FAILED":"PASSED",sample);
    }
}
