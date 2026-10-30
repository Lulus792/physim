/* Observe the actual native text commands before render clears their buffer.
 * The checker derives layout expectations from approved CSS, independently. */
static struct {
    struct {unsigned role;struct nk_rect box,clip;struct nk_color color;float advance;char text[512];} text[256];
    unsigned count;
} browser_layout_commands;
static char browser_layout_long_name[240];
static void browser_layout_draw_probe(browser_dialog *d) {
    browser_layout_commands.count=0;struct nk_rect clip=nk_rect(0,0,100000,100000);const struct nk_command *command;
    nk_foreach(command,d->ui) {
        if(command->type==NK_COMMAND_SCISSOR){const struct nk_command_scissor *c=(const struct nk_command_scissor *)command;clip=nk_rect(c->x,c->y,c->w,c->h);}
        if(command->type!=NK_COMMAND_TEXT)continue;
        const struct nk_command_text *t=(const struct nk_command_text *)command;unsigned role=0;
        while(role<15 && t->font!=&d->fonts[role])role++;
        /* The readonly code editor emits one command per glyph. Its ink/copy/
         * wrapping has a dedicated fixture; keep this bounded observation for
         * the surrounding captions, information, metadata and controls. */
        if(role==15 || role==6 || t->length>=512 || browser_layout_commands.count==256)continue;
        unsigned k=browser_layout_commands.count++;
        browser_layout_commands.text[k].role=role;browser_layout_commands.text[k].box=nk_rect(t->x,t->y,t->w,t->h);
        browser_layout_commands.text[k].clip=clip;browser_layout_commands.text[k].color=t->foreground;
        browser_layout_commands.text[k].advance=t->font->width(t->font->userdata,t->height,t->string,t->length);
        memcpy(browser_layout_commands.text[k].text,t->string,t->length);browser_layout_commands.text[k].text[t->length]=0;
    }
}
static void browser_layout_rect(struct nk_rect r){printf("[%.4f,%.4f,%.4f,%.4f]",r.x,r.y,r.w,r.h);}
static void browser_layout_print(app *a,browser_dialog *d,const char *phase) {
    float f=a->preferences.ui_size/16.f;int width,height;SDL_GetWindowSize(d->window?d->window:a->window,&width,&height);
    printf("{\"kind\":\"explorer_layout\",\"phase\":\"%s\",\"scale\":%.4f,\"regular_em\":%.6f,\"bold_em\":%.6f,\"compact\":%s,\"window\":[%d,%d],\"columns\":%u,\"list\":",phase,f,a->workbench_regular_em,a->workbench_bold_em,d->compact?"true":"false",width,height,d->grid_columns);
    browser_layout_rect(d->list_bounds);printf(",\"content_width\":%.4f,\"preview\":",d->cache_width);browser_layout_rect(d->preview_bounds);printf(",\"rows\":[");
    for(unsigned k=0;k<d->row_count;k++){if(k)putchar(',');printf("{\"name\":\"%s\",\"bounds\":",ps_browser_entry_name(&d->listing.entries[d->rows[k].index]));browser_layout_rect(d->rows[k].bounds);putchar('}');}
    printf("],\"caption_bases\":[");
    const char *captions[]={"DATEI-EXPLORER","ORTE"};
    for(unsigned k=0;k<2;k++) {
        if(k)putchar(',');const struct nk_user_font *base=d->caption_fonts[k].base;float height=d->fonts[k+13].height;
        printf("{\"untracked_advance\":%.4f,\"glyphs\":[",base->width(base->userdata,height,captions[k],(int)strlen(captions[k])));
        for(unsigned j=0;captions[k][j];j++){struct nk_user_font_glyph g;nk_rune next=(unsigned char)captions[k][j+1];
            base->query(base->userdata,height,&g,(nk_rune)(unsigned char)captions[k][j],next);
            printf("%s[%.4f,%.4f,%.4f]",j?",":"",g.offset.x,g.width,g.xadvance);}
        printf("]}");
    }
    printf("],\"texts\":[");
    for(unsigned k=0;k<browser_layout_commands.count;k++) {
        if(k)putchar(',');printf("{\"role\":%u,\"box\":",browser_layout_commands.text[k].role);browser_layout_rect(browser_layout_commands.text[k].box);
        printf(",\"clip\":");browser_layout_rect(browser_layout_commands.text[k].clip);
        struct nk_color c=browser_layout_commands.text[k].color;printf(",\"color\":[%u,%u,%u],\"advance\":%.4f,\"hex\":\"",c.r,c.g,c.b,browser_layout_commands.text[k].advance);
        for(const unsigned char *p=(const unsigned char *)browser_layout_commands.text[k].text;*p;p++)printf("%02x",*p);
        printf("\"}");
    }
    printf("]}\n");
}
static void browser_layout_test_frame(app *a,const char *mode,int *stage,int *code) {
    static unsigned settle,observed;browser_dialog *d=a->browser;bool okay=true;
    if(settle++<3)return;settle=0;
    if(d && (browser_busy(d) || d->cache_dirty || d->reveal))return;
    switch(*stage) {
    case 0:
        strcpy(browser_layout_long_name,"000_Laborprotokoll_");
        for(unsigned i=0;i<9;i++)strcat(browser_layout_long_name,"Lange_Annahmen_");
        strcat(browser_layout_long_name,"Ende.md");
        static char content[66001];memset(content,'x',sizeof content-1);
        okay=browser_fixture_write(browser_layout_long_name,content);
        choose_workspace_path(a,PS_DIALOG_BROWSE);break;
    case 1:
        okay=d && d->window;if(!okay)break;d->draw_probe=browser_layout_draw_probe;
        SDL_SetWindowSize(d->window,strstr(mode,"compact")?320:strstr(mode,"large")?1600:1180,strstr(mode,"large")?1200:1000);break;
    case 2:{
        if(!(observed&16)){browser_layout_print(a,d,"initial");observed|=16;
            if(strstr(mode,"dark") || (strstr(mode,"large") && strstr(mode,"compact"))){char root[4096];okay=ps_browser_parent(browser_test_root,root)==PS_OK;join(d->capture,sizeof d->capture,root,"explorer-layout-initial.png");}}
        struct nk_rect r=browser_test_row(a,browser_layout_long_name);okay&=r.h>0;browser_test_mouse(a,r,true);break;}
    case 3:browser_test_mouse(a,browser_test_row(a,browser_layout_long_name),false);break;
    case 4:
        okay=d->selected>=0 && !strcmp(ps_browser_entry_name(&d->listing.entries[d->selected]),browser_layout_long_name) && d->preview.text && d->preview.truncated;
        /* A long narrow-window row may extend below the outer viewport after
         * a pointer selection. Reveal it through the real keyboard route before
         * requiring every filename glyph to be visible in this observation. */
        if(!(observed&1)){if(!browser_test_focus(a,11))return;browser_layout_print(a,d,"list");observed|=1;}
        if(!browser_test_focus(a,12))return;break;
    case 5:
        if(!(observed&2)){browser_layout_print(a,d,"preview");observed|=2;}
        if(!browser_test_focus(a,7))return;test_window_key_mod(d->window,SDLK_SPACE,0);break;
    case 6:okay=d->grid;if(!browser_test_focus(a,11))return;break;
    case 7:
        if(!(observed&4)){browser_layout_print(a,d,"grid");observed|=4;
            if(strstr(mode,"dark") || (strstr(mode,"large") && strstr(mode,"compact"))){char root[4096];okay=ps_browser_parent(browser_test_root,root)==PS_OK;join(d->capture,sizeof d->capture,root,"explorer-layout-grid.png");}}
        if(!browser_test_focus(a,19))return;break;
    case 8:browser_layout_print(a,d,"footer");test_window_key_mod(d->window,SDLK_ESCAPE,0);break;
    case 9:okay=!browser_active(a);if(okay)puts("EXPLORER LAYOUT SELF-TEST: PASSED");a->quitting=true;break;
    }
    if(!okay){fprintf(stderr,"EXPLORER LAYOUT FAILED stage=%d\n",*stage);*code=1;a->quitting=true;}else (*stage)++;
}
