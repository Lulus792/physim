/* Hidden SDL/OpenGL fixture. Exercises the actual production raster path. */
typedef struct {size_t calls,live,blocks,fail_after;bool invalid;} graphic_test_memory;
static void *graphic_test_allocate(void *user,size_t bytes) {
    graphic_test_memory *m=user;if(++m->calls>=m->fail_after && m->fail_after)return NULL;
    void *p=malloc(bytes);if(p){m->live+=bytes;m->blocks++;}return p;
}
static void graphic_test_free(void *user,void *p,size_t bytes) {
    graphic_test_memory *m=user;if(bytes>m->live || !m->blocks)m->invalid=true;
    else {m->live-=bytes;m->blocks--;}free(p);
}
typedef struct {const char *root;bool fail_strong;} graphic_test_loader;
static struct nk_font *graphic_test_font(void *user,struct nk_font_atlas *atlas,float height,bool strong,
    const unsigned *ranges,char family[96]) {
    graphic_test_loader *loader=user;
    return strong && loader->fail_strong?NULL:graphic_font_load((void *)loader->root,atlas,height,strong,ranges,family);
}
static ps_result graphic_test_snapshot(bool marks,ps_graphic_snapshot **out) {
    ps_report *report=NULL;ps_result result=ps_report_create(marks?"Messpunkte":"Gedämpftes Pendel","Native raster fixture",&report);
    ps_curve_data *c=calloc(1,sizeof *c);if(!c){ps_report_destroy(report);return PS_MEMORY;}
    ps_plot_info p={0};snprintf(p.title,sizeof p.title,"%s",marks?"Messpunkte und Häufigkeiten":"Auslenkung");
    snprintf(p.x_label,sizeof p.x_label,"%s",marks?"Position":"Zeit");snprintf(p.y_label,sizeof p.y_label,"%s",marks?"Anzahl":"θ");
    if(result==PS_OK)result=ps_report_unit_from(marks?PS_ONE:PS_SECOND,&p.x_unit);
    if(result==PS_OK)result=ps_report_unit_from(marks?PS_ONE:PS_RADIAN,&p.y_unit);
    ps_plot_handle plot={0};if(result==PS_OK)result=ps_report_add_plot(report,&p,&plot);
    for(unsigned curve=0;curve<(marks?3u:2u) && result==PS_OK;curve++) {
        memset(c,0,sizeof *c);uint8_t mask[PS_REPORT_MAX_POINTS]={0};
        if(!marks) {
            c->count=curve?769:1024;c->source_count=c->count;c->kind=PS_PLOT_LINE;
            snprintf(c->label,sizeof c->label,"Lauf %s",curve?"023":"024");
            for(unsigned j=0;j<c->count;j++) {
                double t=(curve?.25:0)+(curve?19.5:20)*j/(c->count-1);
                c->x[j]=t;c->y[j]=.35*exp(-(curve?.02:.05)*t)*cos((curve?3.1:3.141592653589793)*t+(curve?.03:0));
                mask[j]=!curve && t>=4.75 && t<=5.5?0:1;
            }
        } else if(curve==0) {
            c->kind=PS_PLOT_SCATTER;c->count=5;strcpy(c->label,"Punkte α");
            for(unsigned j=0;j<5;j++){c->x[j]=1+j*2;c->y[j]=1+(j%3);mask[j]=j==1?0:1;}
        } else if(curve==1) {
            c->kind=PS_PLOT_HISTOGRAM;c->count=2;c->bar_width=1;strcpy(c->label,"Häufigkeiten");
            c->x[0]=2;c->x[1]=6;c->y[0]=2;c->y[1]=3;mask[0]=mask[1]=1;
        } else {c->kind=PS_PLOT_LINE;c->count=1;strcpy(c->label,"Einzelwert");c->x[0]=4;c->y[0]=2;mask[0]=3;}
        if(marks)c->source_count=c->count;
        result=ps_report_add_curve_masked(report,plot,c,mask);
    }
    ps_graphic_axes axes=marks?(ps_graphic_axes){.full={0,10,0,4}}:
        (ps_graphic_axes){.full={0,20,-.5,.5},.visible={4,12,-.2,.2},.cropped=true};
    if(result==PS_OK)result=ps_graphic_snapshot_from_report(report,0,&axes,0,ps_allocator_default(),out);
    free(c);ps_report_destroy(report);return result;
}
static float graphic_test_dummy_width(nk_handle user,float height,const char *text,int length) {
    (void)user;(void)height;(void)text;(void)length;return 0;
}
static ps_result graphic_test_preview(ps_graphics *graphics,ps_graphic_raster *r,const char *path) {
    unsigned texture=ps_graphic_raster_texture(r);if(!texture)return PS_IO;
    const ps_graphics_image *source=ps_graphic_raster_image(r);unsigned width=0,height=0;size_t stride=0;const unsigned char *pixels=NULL;
    if(ps_graphics_image_read(source,&width,&height,&stride,&pixels)!=PS_OK)return PS_INVALID;
    struct nk_user_font dummy={0};dummy.height=1;dummy.width=graphic_test_dummy_width;
    struct nk_context drawing;if(!nk_init_default(&drawing,&dummy))return PS_MEMORY;
    drawing.style.window.padding=nk_vec2(0,0);drawing.style.window.border=0;
    drawing.style.window.fixed_background=nk_style_item_color(nk_rgba(0,0,0,0));
    bool shown=nk_begin(&drawing,"preview",nk_rect(0,0,(float)width,(float)height),NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_NO_INPUT)!=0;
    ps_result result=PS_MEMORY;ps_graphics_image *copy=NULL;
    if(shown) {
        struct nk_command_buffer *canvas=nk_window_get_canvas(&drawing);nk_push_scissor(canvas,nk_rect(0,0,(float)width,(float)height));
        struct nk_image image=nk_image_id((int)texture);nk_draw_image(canvas,nk_rect(0,0,(float)width,(float)height),&image,nk_rgb(255,255,255));
        nk_end(&drawing);struct nk_draw_null_texture null_texture={nk_handle_id((int)texture),{0,0}};
        result=ps_graphics_ui_image(graphics,&drawing,&null_texture,width,height,ps_allocator_default(),&copy);
        unsigned pw=0,ph=0;size_t ps=0;const unsigned char *pp=NULL;
        if(result==PS_OK)result=ps_graphics_image_read(copy,&pw,&ph,&ps,&pp);
        if(result==PS_OK && (pw!=width || ph!=height || ps!=stride || memcmp(pixels,pp,stride*height)))result=PS_CORRUPT;
        if(result==PS_OK)result=ps_graphics_image_png(copy,path);
    } else if(drawing.current)nk_end(&drawing);
    ps_graphics_image_destroy(copy);nk_free(&drawing);return result;
}
static bool graphic_raster_self_test(ps_graphics *graphics,SDL_Window *window,const char *root,const char *directory) {
    ps_graphic_snapshot *snapshot=NULL,*marks=NULL;ps_graphic_raster *raster=NULL,*held=NULL;
    bool ok=false;char path[4096],name[64];float baseline[2]={0},glyph_width[2]={0};
#define REQUIRE(x) do{if(!(x)){fprintf(stderr,"Graphic raster line %d: %s (%s)\n",__LINE__,#x,SDL_GetError());goto done;}}while(0)
    REQUIRE(graphic_test_snapshot(false,&snapshot)==PS_OK && graphic_test_snapshot(true,&marks)==PS_OK);
    for(unsigned mode=0;mode<3;mode++)for(unsigned theme=0;theme<2;theme++)for(unsigned scale=1;scale<=4;scale++) {
        if(mode && scale!=(mode==1?2u:1u))continue;
        ps_graphic_figure_options options={.dark=theme!=0,.visible=mode==1};
        Uint64 resize_started=SDL_GetTicksNS();
        REQUIRE(SDL_SetWindowSize(window,scale%2?760:1600,scale%2?540:900));
        SDL_PumpEvents();REQUIRE(SDL_SyncWindow(window));SDL_PumpEvents();
        double resize_seconds=(double)(SDL_GetTicksNS()-resize_started)/1e9;
        REQUIRE(ps_graphic_raster_create(graphics,mode==2?marks:snapshot,&options,scale,graphic_font_load,(void *)root,ps_allocator_default(),&raster)==PS_OK);
        ps_graphic_raster_info info;REQUIRE(ps_graphic_raster_describe(raster,&info)==PS_OK);
        REQUIRE(info.width==1200*scale && info.height==850*scale && info.glyphs>=100 && info.glyphs<512 && info.atlas_width && info.atlas_height);
        if(!mode && !theme && scale==1){baseline[0]=info.regular_height;baseline[1]=info.strong_height;glyph_width[0]=info.regular_glyph_width;glyph_width[1]=info.strong_glyph_width;}
        if(mode<2)REQUIRE(fabsf(info.regular_height/scale-baseline[0])<.001f && fabsf(info.strong_height/scale-baseline[1])<.001f);
        REQUIRE(info.regular_glyph_width>=scale*(glyph_width[0]-2) && info.regular_glyph_width<=scale*(glyph_width[0]+2) &&
            info.strong_glyph_width>=scale*(glyph_width[1]-2) && info.strong_glyph_width<=scale*(glyph_width[1]+2));
        const ps_graphics_image *image=ps_graphic_raster_image(raster);unsigned iw=0,ih=0;size_t stride=0;const unsigned char *rgb=NULL;
        REQUIRE(ps_graphics_image_read(image,&iw,&ih,&stride,&rgb)==PS_OK && iw==info.width && ih==info.height && stride==iw*3u);
        snprintf(name,sizeof name,"%s-%s-%u",mode==2?"marks":mode==1?"crop":"full",theme?"dark":"light",scale);
        REQUIRE(snprintf(path,sizeof path,"%s/%s.png",directory,name)>0 && ps_graphics_image_png(image,path)==PS_OK);
        REQUIRE(ps_graphics_image_png(image,path)==PS_IO);
        int ww=0,wh=0;REQUIRE(SDL_GetWindowSize(window,&ww,&wh));
        printf("GRAPHIC RASTER CASE: name=%s width=%u height=%u scale=%u glyphs=%u atlas=%u,%u font=%.5f,%.5f peak=%zu window=%d,%d crc=%08x\n",name,iw,ih,scale,info.glyphs,info.atlas_width,info.atlas_height,info.regular_height,info.strong_height,info.font_peak_bytes,ww,wh,ps_crc32(rgb,stride*ih));
        printf("GRAPHIC RASTER FONT: %s | %s\n",info.family[0],info.family[1]);
        printf("GRAPHIC RASTER GLYPH: name=%s A=%.3f,%.3f\n",name,info.regular_glyph_width,info.strong_glyph_width);
        printf("GRAPHIC RASTER TIME: name=%s resize=%.6f fonts=%.6f render=%.6f total=%.6f\n",name,resize_seconds,info.font_seconds,info.render_seconds,info.total_seconds);
        ps_graphic_svg *svg=NULL;REQUIRE(ps_graphic_figure_svg(ps_graphic_raster_figure(raster),0,ps_allocator_default(),&svg)==PS_OK);
        size_t length=0;const char *xml=ps_graphic_svg_bytes(svg,&length);
        REQUIRE(snprintf(path,sizeof path,"%s/%s.svg",directory,name)>0);
        FILE *file=fopen(path,"wbx");bool wrote=file && fwrite(xml,1,length,file)==length;if(file && fclose(file))wrote=false;
        ps_graphic_svg_destroy(svg);REQUIRE(wrote);
        if(!mode && !theme && scale==1){held=raster;raster=NULL;}
        ps_graphic_raster_destroy(raster);raster=NULL;
    }
    /* The retained RGB/figure survive destruction of all input reports and font
     * banks. The preview is a real texture round trip, not a reread of its PNG. */
    ps_graphic_snapshot_destroy(snapshot);snapshot=NULL;
    REQUIRE(snprintf(path,sizeof path,"%s/preview-light-1.png",directory)>0 && graphic_test_preview(graphics,held,path)==PS_OK);
    REQUIRE(graphic_test_snapshot(false,&snapshot)==PS_OK);
    for(size_t fail=1;fail<=2;fail++) {
        graphic_test_memory tracker={.fail_after=fail};ps_allocator domain={&tracker,graphic_test_allocate,graphic_test_free};
        raster=held;ps_result result=ps_graphic_raster_create(graphics,snapshot,NULL,2,graphic_font_load,(void *)root,domain,&raster);
        bool same=raster==held;if(!same)ps_graphic_raster_destroy(raster);raster=NULL;
        REQUIRE(result==PS_MEMORY && same && !tracker.live && !tracker.blocks && !tracker.invalid);
    }
    graphic_test_memory tracker={0};ps_allocator domain={&tracker,graphic_test_allocate,graphic_test_free};
    graphic_test_loader loader={root,true};raster=held;
    ps_result result=ps_graphic_raster_create(graphics,snapshot,NULL,2,graphic_test_font,&loader,domain,&raster);
    bool same=raster==held;if(!same)ps_graphic_raster_destroy(raster);raster=NULL;
    REQUIRE(result==PS_IO && same && tracker.calls>4 && !tracker.live && !tracker.blocks && !tracker.invalid);
    REQUIRE(ps_graphic_raster_create(graphics,snapshot,NULL,0,graphic_font_load,(void *)root,ps_allocator_default(),&raster)==PS_INVALID && !raster);
    struct nk_context empty={0};struct nk_draw_null_texture null_texture={0};ps_graphics_image *candidate=NULL;
    REQUIRE(ps_graphics_ui_image(graphics,&empty,&null_texture,8193,1,ps_allocator_default(),&candidate)==PS_LIMIT && !candidate);
    tracker=(graphic_test_memory){.fail_after=1};
    REQUIRE(ps_graphics_ui_image(graphics,&empty,&null_texture,1200,850,domain,&candidate)==PS_MEMORY && !candidate && !tracker.live);
    puts("GRAPHIC RASTER SELF-TEST: PASSED sizes=4 themes=2 crop=xy marks=line,scatter,histogram preview=identical-rgb source=detached errors=atomic");ok=true;
done:
    ps_graphic_raster_destroy(raster);ps_graphic_raster_destroy(held);
    ps_graphic_snapshot_destroy(snapshot);ps_graphic_snapshot_destroy(marks);return ok;
#undef REQUIRE
}
