/* Bundled diagrams are drawn with native vector commands; no SVG interpreter,
 * web view or document scripts. The inert Markdown targets select these figures. */
static void documentation_diagram_text(app *a,struct nk_rect r,const char *text,struct nk_color color) {
    nk_draw_text(nk_window_get_canvas(a->doc_ui),r,text,(int)strlen(text),a->doc_font_ui,nk_rgba(0,0,0,0),color);
}
static void documentation_diagram(app *a,int kind,struct nk_rect r) {
    a->doc_diagram_bounds=r;
    struct nk_command_buffer *c=nk_window_get_canvas(a->doc_ui);
    nk_fill_rect(c,r,8,UI_COLOR(doc_body));
    if(kind==1) {
        const char *labels[]={"Modell","Build","Simulation","Laufdatei","Analyse"};
        const char *details[]={"Quellen + Parameter","Ausführbare Module","Zustand + Messung","Archiv .psrun","Bericht + Export"};
        float gap=7,w=(r.w-24-gap*4)/5;
        /* Narrow windows retain readable labels in a vertical sequence. */
        bool narrow=w<145;
        for(int i=0;i<5;i++) {
            struct nk_rect box=narrow?nk_rect(r.x+12,r.y+8+i*60,r.w-24,52):nk_rect(r.x+12+i*(w+gap),r.y+60,w,130);
            nk_fill_rect(c,box,5,UI_COLOR(doc_header));
            documentation_diagram_text(a,nk_rect(box.x+8,box.y+5,box.w-16,28),labels[i],UI_COLOR(doc_text));
            if(narrow)documentation_diagram_text(a,nk_rect(box.x+170,box.y+14,fmaxf(1,box.w-178),28),details[i],UI_COLOR(muted));
            else {
                size_t length;documentation_line(a->doc_font_ui,details[i],strlen(details[i]),box.w-16,&length);
                nk_draw_text(c,nk_rect(box.x+8,box.y+45,box.w-16,28),details[i],(int)length,a->doc_font_ui,nk_rgba(0,0,0,0),UI_COLOR(muted));
                if(length<strlen(details[i]))documentation_diagram_text(a,nk_rect(box.x+8,box.y+78,box.w-16,28),details[i]+length,UI_COLOR(muted));
            }
        }
        return;
    }
    float ox=r.x+r.w*.32f,oy=r.y+78,L=fminf(240,r.w*.5f),theta=.45f;
    float bx=ox+sinf(theta)*L,by=oy+cosf(theta)*L;
    nk_stroke_line(c,ox,oy,ox+r.w*.4f,oy,2,UI_COLOR(muted));
    nk_stroke_line(c,ox,oy,ox,oy-38,2,UI_COLOR(muted));
    nk_stroke_line(c,ox,oy,ox,oy+L,1,UI_COLOR(muted));
    for(int i=0;i<12;i++)nk_stroke_line(c,ox+sinf(theta*i/12)*45,oy+cosf(theta*i/12)*45,
        ox+sinf(theta*(i+1)/12)*45,oy+cosf(theta*(i+1)/12)*45,2,UI_COLOR(accent));
    nk_stroke_line(c,ox,oy,bx,by,5,UI_COLOR(doc_text));
    nk_fill_circle(c,nk_rect(bx-12,by-12,24,24),nk_rgb(70,148,196));
    nk_stroke_line(c,bx,by,bx,by+65,4,nk_rgb(205,86,86));
    nk_fill_triangle(c,bx,by+72,bx-6,by+58,bx+6,by+58,nk_rgb(205,86,86));
    float rx=bx-sinf(theta)*70,ry=by-cosf(theta)*70;
    nk_stroke_line(c,bx,by,rx,ry,4,nk_rgb(89,180,136));
    nk_fill_triangle(c,rx,ry,rx+12,ry+10,rx,ry+17,nk_rgb(89,180,136));
    documentation_diagram_text(a,nk_rect(ox+8,oy-55,90,30),"+y",UI_COLOR(doc_text));
    documentation_diagram_text(a,nk_rect(ox+r.w*.4f-35,oy+8,60,30),"+x",UI_COLOR(doc_text));
    documentation_diagram_text(a,nk_rect(ox+15,oy+48,120,30),"θ",UI_COLOR(accent));
    documentation_diagram_text(a,nk_rect((ox+bx)/2+10,(oy+by)/2,60,30),"L",UI_COLOR(doc_text));
    documentation_diagram_text(a,nk_rect(r.x+12,r.y+r.h-100,r.w-24,30),"x = L sin θ; y = −L cos θ",UI_COLOR(doc_text));
    documentation_diagram_text(a,nk_rect(r.x+12,r.y+r.h-65,r.w-24,30),"Rot: Gewicht; grün: Stangenkraft",UI_COLOR(doc_text));
    documentation_diagram_text(a,nk_rect(r.x+12,r.y+r.h-30,r.w-24,30),"Schematische Kraftpfeile",UI_COLOR(muted));
}
static void documentation_model_init(app *a) {
    if(a->doc_model_initialized)return;
    a->doc_model_initialized=true;a->doc_theta=.45f;a->doc_length=1.5f;
    a->doc_yaw=.58f;a->doc_pitch=.23f;a->doc_zoom=5;
}
static void documentation_model_scene(app *a) {
    ps_scene *s=&a->doc_visual_scene;memset(s,0,sizeof *s);
    double theta=a->doc_theta,L=a->doc_length,omega=a->doc_omega,g=9.80665;
    ps_vec3 zero=ps_v3(0,0,0),bob=ps_v3(L*sin(theta),-L*cos(theta),0);
    double tension=g*cos(theta)+L*omega*omega;
    ps_scene_add(s,PS_LINE,zero,bob,0,0xb5c4d8ff);
    ps_scene_add(s,PS_SPHERE,bob,zero,.09,0x4694c4ff);
    ps_scene_add(s,PS_ARROW,bob,ps_vadd(bob,ps_v3(0,-g*.05,0)),0,0xe87979ff);
    ps_scene_add(s,PS_ARROW,bob,ps_vadd(bob,ps_v3(-sin(theta)*tension*.05,cos(theta)*tension*.05,0)),0,0x91d28aff);
    ps_scene_add(s,PS_ARROW,bob,ps_vadd(bob,ps_v3(L*omega*cos(theta)*.3,L*omega*sin(theta)*.3,0)),0,0xf2c572ff);
    ps_scene_add(s,PS_ARROW,zero,ps_v3(1,0,0),0,0x90a0b5ff);
    ps_scene_add(s,PS_ARROW,zero,ps_v3(0,.6,0),0,0x90a0b5ff);
    ps_scene_add(s,PS_ARROW,zero,ps_v3(0,0,1),0,0x90a0b5ff);
}
static void documentation_space_text(app *a,const char *text,bool wrap) {
    struct nk_rect r;if(nk_widget(&r,a->doc_ui)==NK_WIDGET_INVALID)return;
    size_t at=0,n=strlen(text);float y=r.y;
    do {
        size_t length=n-at,used=length;
        if(wrap)used=documentation_line(a->doc_font_ui,text+at,n-at,r.w,&length);
        if(y+a->doc_font_ui->height>r.y+r.h)break;
        nk_draw_text(nk_window_get_canvas(a->doc_ui),nk_rect(r.x,y,r.w,a->doc_font_ui->height+6),text+at,(int)length,a->doc_font_ui,nk_rgba(0,0,0,0),UI_COLOR(doc_text));
        at+=used;y+=a->doc_font_ui->height+6;
    } while(wrap && at<n);
}
static void documentation_pendulum(app *a,int block,float width,float height) {
    documentation_model_init(a);
    struct nk_context *ui=a->doc_ui;
    nk_layout_space_begin(ui,NK_STATIC,height,32);
    float row=fmaxf(34,a->doc_font_ui->height+12),y=0;
    const char *labels[]={"Winkel θ (rad)","Länge L (m)","Winkeltempo ω (rad/s)","Kamera seitlich (rad)","Kamera hoch (rad)","Kameraabstand (m)"};
    float *values[]={&a->doc_theta,&a->doc_length,&a->doc_omega,&a->doc_yaw,&a->doc_pitch,&a->doc_zoom};
    float lo[]={-1.2f,.5f,-3,-(float)PS_PI,-(float)PS_PI/2,2},hi[]={1.2f,3,3,(float)PS_PI,(float)PS_PI/2,12},step[]={.01f,.05f,.05f,.08f,.08f,.2f};
    for(int i=0;i<6;i++,y+=row+8) {
        char label[128];snprintf(label,sizeof label,"%s: %.2f",labels[i],*values[i]);
        nk_layout_space_push(ui,nk_rect(0,y,width*.56f,row));documentation_space_text(a,label,false);
        nk_layout_space_push(ui,nk_rect(width*.57f,y,width*.42f,row));
        a->doc_visual_bounds[i]=nk_widget_bounds(ui);ps_ui_slider_named(ui,labels[i],lo[i],values[i],hi[i],step[i]);
        documentation_focus_draw(a,DOC_VISUAL+block*16+i,a->doc_visual_bounds[i],true);
    }
    const char *buttons[]={"Vorne","Raum","Oben"};
    for(int i=0;i<3;i++) {
        nk_layout_space_push(ui,nk_rect(i*width/3,y,width/3-8,row));a->doc_visual_bounds[6+i]=nk_widget_bounds(ui);
        if(nk_button_label(ui,buttons[i])){a->doc_yaw=i==1?.58f:0;a->doc_pitch=i==2?(float)PS_PI/2:i==1?.23f:0;a->doc_zoom=5;}
        documentation_focus_draw(a,DOC_VISUAL+block*16+6+i,a->doc_visual_bounds[6+i],true);
    }
    y+=row+8;nk_layout_space_push(ui,nk_rect(0,y,width,row));a->doc_visual_bounds[9]=nk_widget_bounds(ui);
    nk_checkbox_label(ui,"Orthografische Projektion",&a->doc_orthographic);
    documentation_focus_draw(a,DOC_VISUAL+block*16+9,a->doc_visual_bounds[9],true);y+=row+8;
    nk_layout_space_push(ui,nk_rect(0,y,width,260));struct nk_rect r;
    enum nk_widget_layout_states state=nk_widget(&r,ui);a->doc_visual_bounds[10]=r;
    if(state!=NK_WIDGET_INVALID) {
        if(state==NK_WIDGET_VALID && nk_input_is_mouse_hovering_rect(&ui->input,r) && nk_input_is_mouse_down(&ui->input,NK_BUTTON_RIGHT)) {
            a->doc_yaw=remainderf(a->doc_yaw-ui->input.mouse.delta.x*.008f,2*(float)PS_PI);
            a->doc_pitch=fmaxf(-(float)PS_PI/2,fminf((float)PS_PI/2,a->doc_pitch+ui->input.mouse.delta.y*.008f));
        }
        documentation_model_scene(a);
        ps_camera camera={a->doc_yaw,a->doc_pitch,a->doc_zoom,ps_v3(0,-a->doc_length*.5,0),a->doc_orthographic,true,true};
        int w,h,pw,ph;SDL_GetWindowSize(a->doc_window,&w,&h);SDL_GetWindowSizeInPixels(a->doc_window,&pw,&ph);
        unsigned texture=ps_graphics_scene(a->doc_graphics,&a->doc_visual_scene,&camera,(int)fmaxf(1,r.w*pw/fmaxf(1,w)),(int)fmaxf(1,r.h*ph/fmaxf(1,h)));
        if(texture){
            struct nk_command_buffer *canvas=nk_window_get_canvas(ui);struct nk_image image=nk_image_id((int)texture);nk_draw_image(canvas,r,&image,nk_rgb(255,255,255));
            struct nk_rect previous=canvas->clip;
            float left=fmaxf(previous.x,r.x),top=fmaxf(previous.y,r.y),right=fminf(previous.x+previous.w,r.x+r.w),bottom=fminf(previous.y+previous.h,r.y+r.h);
            nk_push_scissor(canvas,nk_rect(left,top,fmaxf(0,right-left),fmaxf(0,bottom-top)));
            const char *axes[]={"+x","+y","+z"};
            for(int axis=0;axis<3;axis++) {
                float projected_x,projected_y;
                if(ps_graphics_project(&camera,a->doc_visual_scene.objects[5+axis].b,(double)r.w/r.h,&projected_x,&projected_y))
                    documentation_diagram_text(a,nk_rect(r.x+projected_x*r.w+5,r.y+projected_y*r.h,48,30),axes[axis],UI_COLOR(doc_text));
            }
            nk_push_scissor(canvas,previous);
        }
        documentation_focus_draw(a,DOC_VISUAL+block*16+10,r,true);
    }
    y+=268;
    double theta=a->doc_theta,L=a->doc_length,omega=a->doc_omega;
    char output[200];snprintf(output,sizeof output,"x = %.3f m; y = %.3f m; z = 0 m",L*sin(theta),-L*cos(theta));
    nk_layout_space_push(ui,nk_rect(0,y,width,row));documentation_space_text(a,output,false);y+=row+4;
    snprintf(output,sizeof output,"Stangenkraft: %.3f N; Energie: %.3f J",9.80665*cos(theta)+L*omega*omega,.5*L*L*omega*omega+9.80665*L*(1-cos(theta)));
    nk_layout_space_push(ui,nk_rect(0,y,width,row));documentation_space_text(a,output,false);y+=row+4;
    nk_layout_space_push(ui,nk_rect(0,y,width,180));documentation_space_text(a,"Rot: Gewicht · Grün: Stangenkraft · Gelb: Geschwindigkeit. Maßstäbe: 0,05 m/N und 0,3 s. Rechte Maus dreht. Tab und Pfeile bedienen die Regler. In der Szene: Pfeile drehen, Bild auf/ab zoomt, Pos1 setzt die Kamera zurück.",true);
    nk_layout_space_end(ui);
}
