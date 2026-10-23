/* A path stays complete, including separators and spaces. Ordinary paths use
 * the existing popup material; exceptionally long paths have a bounded scroll
 * area. Retaining hover over that area makes its contents reachable. */
typedef struct {struct nk_mouse mouse;bool held;} workspace_tip_guard;
static bool workspace_tip_key(const struct nk_context *ui) {
    for(unsigned i=0;i<NK_KEY_MAX;i++)if(ui->input.keyboard.keys[i].clicked)return true;
    return ui->input.keyboard.text_len>0;
}
static workspace_tip_guard workspace_tip_prepare(app *a) {
    struct nk_context *ui=a->ui;workspace_tip_guard guard={0};
    if(a->workspace_tip.owner==ui && a->workspace_tip.sequence+1!=ui->seq)a->workspace_tip.visible=false;
    bool inside=a->workspace_tip.visible && a->workspace_tip.owner==ui && nk_input_is_mouse_hovering_rect(&ui->input,a->workspace_tip.bounds);
    if(a->workspace_tip.dismissed || workspace_tip_key(ui) || (!inside && (ui->input.mouse.buttons[NK_BUTTON_LEFT].down || ui->input.mouse.buttons[NK_BUTTON_LEFT].clicked || ui->input.mouse.buttons[NK_BUTTON_RIGHT].down || ui->input.mouse.buttons[NK_BUTTON_RIGHT].clicked)))a->workspace_tip.visible=false;
    if(inside && a->workspace_tip.visible) {
        guard.held=true;guard.mouse=ui->input.mouse;
        memset(&ui->input.mouse,0,sizeof ui->input.mouse);ui->input.mouse.pos=nk_vec2(-10000,-10000);
    }
    return guard;
}
static void workspace_tip_frame_begin(app *a) {
    workspace_tip_guard guard=workspace_tip_prepare(a);
    a->workspace_tip.input_held=guard.held;if(guard.held)a->workspace_tip.mouse=guard.mouse;
}
static void workspace_tip_frame_end(app *a) {
    if(a->workspace_tip.input_held)a->ui->input.mouse=a->workspace_tip.mouse;
    a->workspace_tip.input_held=false;
}
static unsigned workspace_tip_lines(app *a,const struct nk_user_font *font,float space) {
    int length=(int)strlen(a->workspace_tip.text),at=0;unsigned lines=0;a->workspace_tip.offsets[0]=0;
    while(at<length && lines<SDL_arraysize(a->workspace_tip.offsets)-1) {
        int bytes=ps_ui_text_line(font,a->workspace_tip.text+at,length-at,space,true);if(!bytes)return 0;
        at+=bytes;a->workspace_tip.offsets[++lines]=(uint16_t)at;
    }
    return at==length?lines:0;
}
static void workspace_tip_content(app *a,const char *candidate,workspace_tip_guard guard) {
    struct nk_context *ui=a->ui;
    if(a->workspace_tip.dismissed || workspace_tip_key(ui) || (!guard.held && (ui->input.mouse.buttons[NK_BUTTON_LEFT].down || ui->input.mouse.buttons[NK_BUTTON_LEFT].clicked || ui->input.mouse.buttons[NK_BUTTON_RIGHT].down || ui->input.mouse.buttons[NK_BUTTON_RIGHT].clicked)) ||
       (!guard.held && !candidate)) {a->workspace_tip.visible=false;return;}
    if(ui->current->popup.active && ui->current->popup.type!=NK_PANEL_TOOLTIP){a->workspace_tip.visible=false;return;}
    bool changed=a->workspace_tip.owner!=ui;
    if(!guard.held && candidate) {
        size_t length=strlen(candidate);if(length>=sizeof a->workspace_tip.text){a->workspace_tip.visible=false;return;}
        changed=changed || strcmp(candidate,a->workspace_tip.text)!=0;
        if(changed)memcpy(a->workspace_tip.text,candidate,length+1);
    }
    if(changed)a->workspace_tip.scroll=0;
    int width=0,height=0;if(!SDL_GetWindowSize(a->window,&width,&height) || width<40 || height<40){a->workspace_tip.visible=false;return;}
    const struct nk_user_font *font=ui->style.font;float f=a->preferences.ui_size/16.f,margin=8;
    float pad_x=ui->style.window.popup_padding.x,pad_y=ui->style.window.popup_padding.y,border=ui->style.window.popup_border;
    float natural=font->width(font->userdata,font->height,a->workspace_tip.text,(int)strlen(a->workspace_tip.text));
    float outer_x=2*(pad_x+border),outer_y=2*(pad_y+border),line=nk_sdl_row_height(ui,font->height+2*ui->style.text.padding.y);
    bool reflow=changed || a->workspace_tip.font!=font || a->workspace_tip.font_user.ptr!=font->userdata.ptr || a->workspace_tip.font_height!=font->height ||
        a->workspace_tip.viewport_width!=width || a->workspace_tip.viewport_height!=height || a->workspace_tip.line_height!=line || a->workspace_tip.padding_x!=outer_x || a->workspace_tip.padding_y!=outer_y;
    float w=reflow?fminf(width-2*margin,fmaxf(40,fminf(520*f,natural+outer_x+1))):a->workspace_tip.bounds.w;
    float space=reflow?w-outer_x-1:a->workspace_tip.space;
    unsigned lines=reflow?workspace_tip_lines(a,font,space):a->workspace_tip.lines;
    if(!lines){a->workspace_tip.visible=false;return;}
    if(reflow && lines*line+outer_y>height-2*margin && w<width-2*margin) {
        w=width-2*margin;space=w-outer_x-1;lines=workspace_tip_lines(a,font,space);reflow=true;
    }
    bool scroll=reflow?lines*line+outer_y>height-2*margin:a->workspace_tip.scrollable;
    if(reflow && scroll){space-=ui->style.window.scrollbar_size.x;lines=workspace_tip_lines(a,font,space);}
    if(!lines){a->workspace_tip.visible=false;return;}
    float h=fminf(height-2*margin,lines*line+outer_y);
    float x=guard.held?a->workspace_tip.bounds.x:ui->input.mouse.pos.x+12*f;
    float y=guard.held?a->workspace_tip.bounds.y:ui->input.mouse.pos.y+20*f;
    if(!guard.held && y+h>height-margin)y=ui->input.mouse.pos.y-h-12*f;
    x=fmaxf(margin,fminf(x,width-margin-w));y=fmaxf(margin,fminf(y,height-margin-h));
    a->workspace_tip.bounds=nk_rect(x,y,w,h);a->workspace_tip.owner=ui;a->workspace_tip.lines=lines;
    a->workspace_tip.font=font;a->workspace_tip.font_user=font->userdata;a->workspace_tip.font_height=font->height;a->workspace_tip.space=space;
    a->workspace_tip.line_height=line;a->workspace_tip.content_height=lines*line;
    a->workspace_tip.viewport_width=width;a->workspace_tip.viewport_height=height;a->workspace_tip.padding_x=outer_x;a->workspace_tip.padding_y=outer_y;a->workspace_tip.scrollable=scroll;
    if(reflow)a->workspace_tip.scroll=(unsigned)fminf(a->workspace_tip.scroll,fmaxf(0,lines*line-(h-outer_y)));
    if(guard.held && scroll && ui->input.mouse.scroll_delta.y) {
        /* Tooltip hover must not replace the editor's active window merely to
         * satisfy Nuklear's ordinary root-window wheel gate. Route this wheel
         * only to the owned path viewport; the frame guard protects the rest. */
        float maximum=ceilf(fmaxf(0,lines*line-(h-outer_y)));
        float offset=a->workspace_tip.scroll-ui->input.mouse.scroll_delta.y*3*line;
        a->workspace_tip.scroll=(unsigned)fmaxf(0,fminf(maximum,offset));ui->input.mouse.scroll_delta.y=0;
    }
    struct nk_window *owner=ui->current;struct nk_rect clip=owner->layout->clip;
    nk_flags flags=NK_WINDOW_BORDER|(scroll?0:NK_WINDOW_NO_SCROLLBAR);
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,0));
    /* Every line is wrapped; reserve no unused horizontal scrollbar footer. */
    nk_style_push_vec2(ui,&ui->style.window.scrollbar_size,nk_vec2(ui->style.window.scrollbar_size.x,0));
    a->workspace_tip.visible=nk_popup_begin(ui,NK_POPUP_STATIC,"__##WorkspacePath##__",flags,nk_rect(x-clip.x,y-clip.y,w,h))!=0;
    if(!a->workspace_tip.visible){nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);return;}
    /* nk_tooltip_end makes the popup transient: nk_clear frees it each frame.
     * Apply the saved offset to the newly created popup, before laying out text. */
    nk_popup_set_scroll(ui,0,a->workspace_tip.scroll);
    a->workspace_tip.sequence=ui->seq;
    owner->layout->flags&=~(nk_flags)NK_WINDOW_ROM;owner->popup.type=NK_PANEL_TOOLTIP;ui->current->layout->type=NK_PANEL_TOOLTIP;
    struct nk_window *popup=ui->current;
    for(unsigned i=0;i<lines;i++) {
        int begin=a->workspace_tip.offsets[i],length=a->workspace_tip.offsets[i+1]-begin;
        nk_layout_row_dynamic(ui,line,1);struct nk_rect r;nk_widget(&r,ui);
        nk_draw_text(nk_window_get_canvas(ui),nk_rect(r.x,r.y+ui->style.text.padding.y,r.w,font->height),a->workspace_tip.text+begin,length,font,nk_rgba(0,0,0,0),UI_COLOR(text));
        for(int at=0;at<length;) {
            char label[512];int n=(int)fminf(length-at,sizeof label-1);
            while(n && at+n<length && ((unsigned char)a->workspace_tip.text[begin+at+n]&0xc0)==0x80)n--;
            if(!n)break;memcpy(label,a->workspace_tip.text+begin+at,(size_t)n);label[n]=0;
            float offset=font->width(font->userdata,font->height,a->workspace_tip.text+begin,at);
            ps_ui_accessibility_label(ui,nk_rect(r.x+offset,r.y,font->width(font->userdata,font->height,label,n),r.h),label);at+=n;
        }
    }
    nk_tooltip_end(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);a->workspace_tip.scroll=popup->scrollbar.y;
}
static void workspace_tip_draw(app *a,const char *candidate) {
    workspace_tip_guard guard={0};guard.held=a->workspace_tip.input_held;
    if(guard.held){guard.mouse=a->workspace_tip.mouse;a->ui->input.mouse=guard.mouse;}
    workspace_tip_content(a,candidate,guard);
    if(guard.held){memset(&a->ui->input.mouse,0,sizeof a->ui->input.mouse);a->ui->input.mouse.pos=nk_vec2(-10000,-10000);}
}
