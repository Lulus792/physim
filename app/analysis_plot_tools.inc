/* The same CSS chart-tools component serves raw data and report diagrams. */
typedef struct {
    struct nk_rect buttons[4];
    float preferred,height,scale,padding;
    struct nk_vec2 pixels;
    unsigned count,rows;
} analysis_tools_layout;
static const char *const analysis_tool_labels[]={"+","−","Alles zeigen","Grafik exportieren…"};
static const char *const analysis_tool_names[]={"Zoom +","Zoom -","Alles zeigen","Grafik exportieren…"};
static analysis_tools_layout analysis_tools_measure(app *a,float available,unsigned count) {
    analysis_tools_layout p={0};p.count=count;p.scale=a->preferences.ui_size/16.f;p.pixels=nk_vec2(1,1);
    const struct nk_user_font *font=&a->workbench_fonts[2];float f=p.scale;
    int w=0,h=0,pw=0,ph=0;
    if(SDL_GetWindowSize(a->window,&w,&h) && SDL_GetWindowSizeInPixels(a->window,&pw,&ph) && w>0 && h>0 && pw>0 && ph>0)p.pixels=nk_vec2((float)pw/w,(float)ph/h);
    p.padding=(w<=1200*f?7:9)*f;
    float used[4]={0},heights[4]={0};unsigned row[4]={0};
    available=fmaxf(1,available);
    for(unsigned i=0;i<count;i++) {
        float gap=i?4*f:0,margin=i==3 && w>760*f?8*f:0;
        int length=(int)strlen(analysis_tool_labels[i]);
        float text=font->width(font->userdata,font->height,analysis_tool_labels[i],length);
        float width=fmaxf(28*f,text+2*p.padding+2+(i==3?23*f:0));
        p.preferred+=width+gap+margin;
        width=fminf(width,fmaxf(1,available-margin));
        unsigned line=p.rows;
        if(used[line] && used[line]+gap+margin+width>available){line=++p.rows;gap=0;}
        float text_space=fmaxf(1,width-2*p.padding-2-(i==3?23*f:0));
        unsigned lines=0;int at=0;
        while(at<length){int n=ps_ui_text_line(font,analysis_tool_labels[i]+at,length-at,text_space,false);if(!n){lines=1;break;}at+=n;lines++;}
        float height=fmaxf(28*f,18*f*lines+8*f);
        row[i]=line;p.buttons[i]=nk_rect(used[line]+gap+margin,0,width,height);
        used[line]=p.buttons[i].x+width;heights[line]=fmaxf(heights[line],height);
    }
    p.rows++;
    for(unsigned i=0;i<count;i++) {
        float y=0;for(unsigned line=0;line<row[i];line++)y+=heights[line]+4*f;
        p.buttons[i].x+=available-used[row[i]];
        p.buttons[i].y=y+(heights[row[i]]-p.buttons[i].h)/2;
    }
    for(unsigned line=0;line<p.rows;line++)p.height+=heights[line]+(line?4*f:0);
    return p;
}
static void analysis_tool_segment(struct nk_command_buffer *c,float x,float y,float xx,float yy,float t,struct nk_color color) {
    nk_stroke_line(c,x,y,xx,yy,t,color);
    nk_fill_circle(c,nk_rect(x-t/2,y-t/2,t,t),color);nk_fill_circle(c,nk_rect(xx-t/2,yy-t/2,t,t),color);
}
static void analysis_tool_export_icon(struct nk_command_buffer *c,float x,float y,float f,struct nk_color color) {
    /* Original 20x20 SVG, displayed at 16px, round caps and joins. */
    float s=16*f/20,t=1.4f*s;
    const float points[][4]={{10,13,10,2},{6,6,10,2},{10,2,14,6},{4,11,4,17},{4,17,16,17},{16,17,16,11}};
    for(unsigned i=0;i<SDL_arraysize(points);i++)analysis_tool_segment(c,x+points[i][0]*s,y+points[i][1]*s,x+points[i][2]*s,y+points[i][3]*s,t,color);
}
static int analysis_tools_draw(app *a,const analysis_tools_layout *p,float x,float y,bool enabled) {
    struct nk_context *ui=a->ui;struct nk_command_buffer *c=nk_window_get_canvas(ui);int action=-1;
    const struct nk_user_font *font=&a->workbench_fonts[2];float f=p->scale;
    for(unsigned i=0;i<p->count;i++) {
        struct nk_rect r=p->buttons[i];r.x+=x;r.y+=y;
        if(i<3)a->plot_buttons[i]=r;else a->result_export_bounds[2]=r;
        bool focused=false;nk_begin_disabled(ui,!enabled);
        if(ps_ui_button_at_custom_focus(ui,r,analysis_tool_names[i],false,&focused))action=(int)i;
        /* Dock ROM routes pointer input by panel ownership. It does not mean
         * that a valid data action is semantically disabled. */
        bool active=enabled && !ui->current->widgets_disabled;
        bool hover=active && !(ui->current->layout->flags&NK_WINDOW_ROM) && nk_input_is_mouse_hovering_rect(&ui->input,c->clip) && nk_input_is_mouse_hovering_rect(&ui->input,r);
        struct nk_color border=UI_COLOR(border),color=i==3?UI_COLOR(accent):UI_COLOR(text);
        if(!active){border.a=127;color.a=127;}
        float left=roundf(r.x*p->pixels.x)/p->pixels.x,top=roundf(r.y*p->pixels.y)/p->pixels.y;
        struct nk_rect paint=nk_rect(left,top,roundf((r.x+r.w)*p->pixels.x)/p->pixels.x-left,roundf((r.y+r.h)*p->pixels.y)/p->pixels.y-top);
        nk_fill_rect(c,paint,7*f,border);
        nk_fill_rect(c,nk_rect(paint.x+1,paint.y+1,paint.w-2,paint.h-2),fmaxf(0,7*f-1),hover?UI_COLOR(button):UI_COLOR(edit));
        int length=(int)strlen(analysis_tool_labels[i]),at=0;unsigned lines=0;
        float space=fmaxf(1,r.w-2*p->padding-2-(i==3?23*f:0));
        while(at<length){int n=ps_ui_text_line(font,analysis_tool_labels[i]+at,length-at,space,false);if(!n)break;at+=n;lines++;}
        float yy=r.y+(r.h-18*f*lines)/2+(18*f-font->height)/2;at=0;
        if(i==3)analysis_tool_export_icon(c,r.x+p->padding+1,r.y+(r.h-16*f)/2,f,color);
        while(at<length) {
            int n=ps_ui_text_line(font,analysis_tool_labels[i]+at,length-at,space,false);if(!n)break;
            float width=font->width(font->userdata,font->height,analysis_tool_labels[i]+at,n);
            float xx=i==3?r.x+p->padding+1+23*f:r.x+(r.w-width)/2;
            nk_draw_text(c,nk_rect(xx,yy,width+1,font->height),analysis_tool_labels[i]+at,n,font,nk_rgba(0,0,0,0),color);
            at+=n;yy+=18*f;
        }
        if(active && focused) {
            struct nk_rect clip=c->clip,window=nk_window_get_bounds(ui);float margin=5*f;
            float lx=fmaxf(window.x,clip.x-margin),ty=fmaxf(window.y,clip.y-margin);
            nk_push_scissor(c,nk_rect(lx,ty,fminf(window.x+window.w,clip.x+clip.w+margin)-lx,fminf(window.y+window.h,clip.y+clip.h+margin)-ty));
            nk_stroke_rect(c,nk_rect(r.x-4*f,r.y-4*f,r.w+8*f,r.h+8*f),11*f,2*f,UI_COLOR(accent));nk_push_scissor(c,clip);
        }
        nk_end_disabled(ui);
    }
    return action;
}
