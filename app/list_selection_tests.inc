/* Native row selection and real queued pointer input. No physical-focus claim. */
static struct {app *owner;char root[4096];unsigned kind,settle;const char *phase;} list_a2;
static bool list_a2_start(app *a,const char *root,const char *mode) {
    memset(&list_a2,0,sizeof list_a2);list_a2.owner=a;snprintf(list_a2.root,sizeof list_a2.root,"%s",root);
    list_a2.kind=strstr(mode,"-protocol-")?0:strstr(mode,"-grid-")?2:1;
    bool okay=list_a2.kind?browser_test_start(a,root,mode):protocol_ui_test_start(a,root,mode);
    if(okay && !list_a2.kind)a->preferences.log_height=500;
    return okay;
}
static struct nk_rect list_a2_target(app *a) {
    return list_a2.kind?browser_test_row(a,"main.c"):a->protocol_view.rows[1];
}
static void selection_row_probe(app *a,struct nk_context *ui,const char *phase,struct nk_rect row) {
    printf("{\"kind\":\"list_selection_a2\",\"phase\":\"%s\",\"scale\":%.4f,\"row\":",phase,a->preferences.ui_size/16.f);selection_a2_rect(row);
    printf(",\"texts\":[");bool comma=false;const struct nk_command *command;struct nk_rect clip=nk_rect(0,0,100000,100000);
    nk_foreach(command,ui) {
        if(command->type==NK_COMMAND_SCISSOR){const struct nk_command_scissor *s=(const struct nk_command_scissor*)command;clip=nk_rect(s->x,s->y,s->w,s->h);}
        if(command->type!=NK_COMMAND_TEXT)continue;const struct nk_command_text *t=(const struct nk_command_text*)command;
        if(!t->length || !t->foreground.a || !selection_a2_inside(row,t->x,t->y))continue;
        if(comma)putchar(',');comma=true;printf("{\"box\":");selection_a2_rect(nk_rect(t->x,t->y,t->w,t->h));printf(",\"clip\":");selection_a2_rect(clip);
        printf(",\"paint_width\":%.4f,\"color\":[%u,%u,%u,%u],\"hex\":\"",t->font->width(t->font->userdata,t->height,t->string,t->length),t->foreground.r,t->foreground.g,t->foreground.b,t->foreground.a);
        for(int k=0;k<t->length;k++)printf("%02x",(unsigned char)t->string[k]);printf("\"}");
    }
    printf("],\"lines\":[");comma=false;
    nk_foreach(command,ui) {
        if(command->type!=NK_COMMAND_LINE)continue;const struct nk_command_line *line=(const struct nk_command_line*)command;
        if(!selection_a2_inside(row,line->begin.x,line->begin.y) || !selection_a2_inside(row,line->end.x,line->end.y))continue;
        if(comma)putchar(',');comma=true;
        printf("{\"points\":[%.4f,%.4f,%.4f,%.4f],\"width\":%.4f,\"color\":[%u,%u,%u,%u]}",line->begin.x,line->begin.y,line->end.x,line->end.y,line->line_thickness,line->color.r,line->color.g,line->color.b,line->color.a);
    }
    printf("],\"rectangles\":[");comma=false;
    nk_foreach(command,ui) {
        if(command->type!=NK_COMMAND_RECT)continue;const struct nk_command_rect *r=(const struct nk_command_rect*)command;
        if(!selection_a2_inside(row,r->x,r->y))continue;
        if(comma)putchar(',');comma=true;printf("{\"box\":");selection_a2_rect(nk_rect(r->x,r->y,r->w,r->h));
        printf(",\"rounding\":%.4f,\"width\":%.4f,\"color\":[%u,%u,%u,%u]}",r->rounding,r->line_thickness,r->color.r,r->color.g,r->color.b,r->color.a);
    }
    printf("]}\n");
}
static void list_a2_probe(app *a,struct nk_context *ui) {
    if(!list_a2.phase)return;selection_row_probe(a,ui,list_a2.phase,list_a2_target(a));list_a2.phase=NULL;
}
static void list_a2_browser_probe(browser_dialog *d){list_a2_probe(list_a2.owner,d->ui);}
static void list_a2_draw(app *a){if(!list_a2.kind)list_a2_probe(a,a->ui);}
static void list_a2_capture(app *a,const char *phase,const char **capture) {
    list_a2.phase=phase;const char *name=!strcmp(phase,"before")?"list-before.png":"list-after.png";
    if(!strcmp(phase,"focus"))name="list-focus.png";
    if(list_a2.kind)join(a->browser->capture,sizeof a->browser->capture,list_a2.root,name);else *capture=name;
}
static void list_a2_frame(app *a,int *stage,int *code,const char **capture) {
    if(list_a2.settle++<3)return;list_a2.settle=0;browser_dialog *d=a->browser;bool okay=true;
    if(list_a2.kind && d && (browser_busy(d) || d->cache_dirty || d->reveal))return;
    switch(*stage) {
    case 0:if(list_a2.kind)choose_workspace_path(a,PS_DIALOG_BROWSE);break;
    case 1:
        if(list_a2.kind){okay=d && d->window;if(!okay)break;d->draw_probe=list_a2_browser_probe;d->grid=list_a2.kind==2;d->cache_dirty=true;SDL_SetWindowSize(d->window,1440,1000);}
        break;
    case 2:okay=list_a2_target(a).h>0;list_a2_capture(a,"before",capture);break;
    case 3:test_window_mouse(list_a2.kind?d->window:a->window,list_a2_target(a),true);break;
    case 4:test_window_mouse(list_a2.kind?d->window:a->window,list_a2_target(a),false);break;
    case 5:
        okay=list_a2.kind?d->selected>=0 && !strcmp(ps_browser_entry_name(&d->listing.entries[d->selected]),"main.c"):
            a->protocol_view.selected==a->protocol_view.row_ids[1];
        list_a2_capture(a,"after",capture);break;
    case 6:
        if(list_a2.kind){if(!browser_test_focus(a,11))return;list_a2_capture(a,"focus",capture);break;}
        puts("LIST SELECTION A2 SELF-TEST: PASSED");a->quitting=true;break;
    case 7:puts("LIST SELECTION A2 SELF-TEST: PASSED");a->quitting=true;break;
    }
    if(!okay){fprintf(stderr,"A2 list failed stage=%d kind=%u\n",*stage,list_a2.kind);*code=1;a->quitting=true;}else (*stage)++;
}
