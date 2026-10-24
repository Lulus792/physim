/* Controlled rendering cases over an unchanged real paused pendulum project. */
static struct {unsigned kind,settle;bool active;float header;struct nk_rect head;unsigned segments;float stroke;char text[3][512];} curve_design;
static const char *curve_design_kind(void){const char *k[]={"empty","one","missing","gaps","offset","long","unrepresentable"};return k[curve_design.kind<7?curve_design.kind:0];}
static bool curve_design_start(app *a,const char *root){memset(&curve_design,0,sizeof curve_design);return inspector_design_start(a,root);}
static void curve_design_draw(app *a) {
    if(!curve_design.active){nk_window_show(a->ui,"curve-probe",NK_HIDDEN);return;}
    const ps_ui_palette *previous_palette=ui_palette;
    const ps_ui_palette *base=a->preferences.theme==PS_THEME_HIGH_CONTRAST?&PS_UI_HIGH_CONTRAST:a->preferences.theme==PS_THEME_LIGHT?&PS_UI_LIGHT:&PS_UI_DARK;
    ui_palette=ps_laboratory_palette(base);
    float f=a->preferences.ui_size/16.f,width=inspector_design.variant<2?420:200;
    double times[]={0,1,2,3,4,5},values[]={1,2,3,4,5,6};int count=6;bool represented=true;
    const char *name="Messkanal",*unit="m/s";
    if(curve_design.kind==0)count=0;
    if(curve_design.kind==1)count=1;
    if(curve_design.kind==2)for(unsigned i=0;i<6;i++)values[i]=NAN;
    if(curve_design.kind==3)values[2]=values[3]=NAN;
    if(curve_design.kind==4)for(unsigned i=0;i<6;i++){times[i]=1e100+i*1e85;values[i]=1e90+i*1e75;}
    if(curve_design.kind==5){name="Vollständiger Kanalname mit langen Metadaten";unit="kg·m²/(A·s³) · vollständige Anzeigeeinheit";}
    if(curve_design.kind==6){represented=false;for(unsigned i=0;i<6;i++)values[i]=NAN;}
    struct nk_context *ui=a->ui;float total=laboratory_curve_minimum(a,name,unit,times,values,count,represented,width);
    nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(16*f,16*f));nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,0));
    nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(UI_COLOR(window)));
    nk_window_show(ui,"curve-probe",NK_SHOWN);
    if(nk_begin(ui,"curve-probe",nk_rect(20,245,width+32*f,total+32*f),NK_WINDOW_NO_SCROLLBAR)) {
        float w=nk_window_get_content_region(ui).w;
        curve_design.header=laboratory_channel_measure(a,name,unit,nk_rect(0,0,w,0)).height;
        ps_ui_row_exact_dynamic(ui,curve_design.header,1);nk_widget(&curve_design.head,ui);
        laboratory_channel_paint p={a,0,true,name,unit,name};laboratory_channel_field(ui,curve_design.head,false,&p);
        laboratory_curve(a,times,values,count,represented,total-curve_design.header,0);
    }nk_end(ui);nk_sdl_window_raise(ui,"curve-probe");nk_style_pop_style_item(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
    memset(curve_design.text,0,sizeof curve_design.text);curve_design.segments=0;curve_design.stroke=0;
    struct nk_rect areas[]={a->laboratory_channel_names[0],a->laboratory_channel_units[0],a->laboratory_curve_messages[0]};
    const struct nk_command *cmd;struct nk_color color=laboratory_curve_color(0);
    nk_foreach(cmd,ui) {
        if(cmd->type==NK_COMMAND_TEXT){const struct nk_command_text *t=(const struct nk_command_text*)cmd;
            if(t->font==&a->workspace_status_font)for(unsigned i=0;i<3;i++)if(areas[i].w>0 && camera_design_contains(areas[i],t))camera_design_append(curve_design.text[i],512,t);
        }
        if(cmd->type==NK_COMMAND_LINE){const struct nk_command_line *l=(const struct nk_command_line*)cmd;struct nk_rect r=a->laboratory_curve_frames[0];
            if(!memcmp(&l->color,&color,sizeof color) && l->begin.x>=r.x && l->end.x<=r.x+r.w && l->begin.y>=r.y && l->end.y<=r.y+r.h){curve_design.segments++;curve_design.stroke=l->line_thickness;}
        }
    }
    ui_palette=previous_palette;
}
static void curve_design_step(app *a,int *stage,int *code,const char **capture) {
    if(*stage<4){inspector_design_frame(a,stage,code,capture);return;}
    bool ok=inspector_design_same(a);
    switch(*stage) {
    case 4:inspector_design_frame(a,stage,code,capture);curve_design.active=true;curve_design.settle=0;return;
    case 5:
        if(curve_design.settle++<3)return;
        {static char path[100];snprintf(path,sizeof path,"curve-%s-%s.png",inspector_design_variant(),curve_design_kind());*capture=path;}
        printf("CURVE CASE variant=%s kind=%s size=%u preserved=%d segments=%u stroke=%.4f header=%.4f",inspector_design_variant(),curve_design_kind(),a->preferences.ui_size,ok,curve_design.segments,curve_design.stroke,curve_design.header);
        camera_design_rect("head",curve_design.head);camera_design_rect("name",a->laboratory_channel_names[0]);camera_design_rect("unit",a->laboratory_channel_units[0]);camera_design_rect("frame",a->laboratory_curve_frames[0]);camera_design_rect("area",a->laboratory_curve_areas[0]);camera_design_rect("note",a->laboratory_curve_notes[0]);camera_design_rect("message",a->laboratory_curve_messages[0]);
        for(unsigned i=0;i<3;i++){printf(" paint%u=",i);camera_design_hex(curve_design.text[i]);}puts("");break;
    case 6:
        curve_design.settle=0;if(++curve_design.kind<7){*stage=5;return;}
        curve_design.kind=0;if(++inspector_design.variant<6){*stage=4;return;}curve_design.active=false;*stage=30;return;
    case 30:{uint32_t crc=0;ok=ok && timeline_design_run_crc(a->last_run,&crc) && crc==inspector_design.run_crc;command(a,PS_MSG_STOP);a->stop_at=ps_clock();break;}
    case 31:if(!idle(a) || a->library_thread)return;puts("CURVE DESIGN SELF-TEST: PASSED");a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"CURVE DESIGN FAILED stage=%d variant=%u kind=%u\n",*stage,inspector_design.variant,curve_design.kind);*code=1;a->quitting=true;}else (*stage)++;
}
