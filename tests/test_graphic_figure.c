#include "graphic_figure.h"
#include "test_allocator.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Graphic figure %d: %s\n",__LINE__,#x);return 1;}}while(0)
/* Deliberate deterministic fixture metrics, not a real font/render proof. */
static double measure(void *user,const char *text,double size,double spacing,bool strong) {
    (void)user;size_t n=0;for(const unsigned char *p=(const unsigned char *)text;*p;p++)if((*p&0xc0)!=0x80)n++;
    return n*size*(strong?.55:.5)+(n?n-1:0)*spacing;
}
static double bad_measure(void *user,const char *text,double size,double spacing,bool strong) {
    (void)user;(void)text;(void)size;(void)spacing;(void)strong;return NAN;
}
static ps_result make_snapshot(const ps_graphic_axes *axes,ps_graphic_snapshot **out) {
    ps_report *report=NULL;ps_result r=ps_report_create("Modell <&> α","Real report fixture",&report);
    ps_plot_info p={.title="Auslenkung <&> α",.x_label="Zeit",.y_label="Signal"};ps_plot_handle handle={0};
    if(r==PS_OK)r=ps_report_unit_from(PS_SECOND,&p.x_unit);
    if(r==PS_OK)r=ps_report_unit_from(PS_ONE,&p.y_unit);
    if(r==PS_OK)r=ps_report_add_plot(report,&p,&handle);
    ps_curve_data *c=calloc(1,sizeof *c);if(!c){ps_report_destroy(report);return PS_MEMORY;}
    strcpy(c->label,"A <&> α");c->kind=PS_PLOT_LINE;c->count=6;c->source_count=6001;
    const double y[]={0,4,0,-4,0,4};uint8_t flags[]={3,1,0,3,1,1};
    for(unsigned i=0;i<6;i++){c->x[i]=i*2;c->y[i]=y[i];}
    if(r==PS_OK)r=ps_report_add_curve_masked(report,handle,c,flags);
    strcpy(c->label,"Referenz B");c->count=2;c->source_count=2;c->x[0]=0;c->x[1]=10;c->y[0]=c->y[1]=1;
    if(r==PS_OK)r=ps_report_add_curve(report,handle,c);
    strcpy(c->label,"Scatter C");c->kind=PS_PLOT_SCATTER;c->count=3;c->source_count=3;
    c->x[0]=1;c->x[1]=5;c->x[2]=9;c->y[0]=0;c->y[1]=1;c->y[2]=0;
    if(r==PS_OK)r=ps_report_add_curve(report,handle,c);
    memset(c->label,'D',95);c->label[95]=0;c->kind=PS_PLOT_HISTOGRAM;c->count=2;c->source_count=2;c->bar_width=2;
    c->x[0]=2;c->x[1]=8;c->y[0]=4;c->y[1]=3;
    if(r==PS_OK)r=ps_report_add_curve(report,handle,c);
    if(r==PS_OK)r=ps_graphic_snapshot_from_report(report,0,axes,1,ps_allocator_default(),out);
    free(c);ps_report_destroy(report);return r;
}
typedef struct {size_t seen,stop;} sink_state;
static ps_result sink(void *user,const ps_figure_command *c) {
    sink_state *s=user;(void)c;s->seen++;return s->seen==s->stop?PS_IO:PS_OK;
}
static int verify(const ps_graphic_figure *f,bool dark,bool visible) {
    ps_figure_command c;unsigned lines[4]={0},dots=0,bars=0,texts=0;bool clip=false,ref=false,title=false,notice=false,long_label=false;
    CHECK(ps_graphic_figure_command(f,0,&c)==PS_OK && c.kind==PS_FIGURE_RECT && c.w==1200 && c.h==850 && c.color==(dark?0x1b1c20u:0xffffffu));
    for(size_t i=0;i<ps_graphic_figure_count(f);i++) {
        CHECK(ps_graphic_figure_command(f,i,&c)==PS_OK);
        CHECK(isfinite(c.x) && isfinite(c.y) && isfinite(c.w) && isfinite(c.h));
        if(c.kind==PS_FIGURE_CLIP){CHECK(!clip && c.x==116 && c.y==230 && c.w==1032 && c.h==470);clip=true;}
        if(c.kind==PS_FIGURE_UNCLIP){CHECK(clip);clip=false;}
        if(c.kind==PS_FIGURE_TEXT) {
            texts++;CHECK(c.text && c.advance<=c.w+1e-5 && c.h>0);
            if(c.strong){CHECK(!strcmp(c.text,"Auslenkung <&> α") && c.x==64 && c.y==104 && c.h==34);title=true;}
            if(!strcmp(c.text,"Referenz B · Referenz"))ref=true;
            if(strstr(c.text,"Vorschau reduziert"))notice=true;
            if(strlen(c.text)==95 && c.text[0]=='D'){CHECK(c.h<17 && c.advance<=c.w+1e-5);long_label=true;}
        }
        if(c.curve<0)continue;
        CHECK(clip && c.curve<4);
        CHECK(c.x>=116-1e-6 && c.x<=1148+1e-6 && c.y>=230-1e-6 && c.y<=700+1e-6);
        if(c.kind==PS_FIGURE_LINE) {
            lines[c.curve]++;CHECK(c.w>=116-1e-6 && c.w<=1148+1e-6 && c.h>=230-1e-6 && c.h<=700+1e-6);
            if(c.curve==0){CHECK(hypot(c.w-c.x,c.h-c.y)<=3+1e-5);if(!visible)CHECK(c.w<=322.4+1e-5 || c.x>=735.2-1e-5);}
            if(c.curve==1){CHECK(c.x==116 && c.w==1148 && fabs(c.y-(visible?347.5:418))<1e-6 && c.h==c.y);}
        } else if(c.kind==PS_FIGURE_CIRCLE){CHECK(c.curve==2);dots++;}
        else if(c.kind==PS_FIGURE_RECT){CHECK(c.curve==3 && c.x+c.w<=1148+1e-6 && c.y+c.h<=700+1e-6);bars++;}
        else CHECK(false);
    }
    CHECK(!clip && ref && title && notice && long_label && texts>=20 && lines[0]>5 && lines[1]==1 && !lines[2] && !lines[3]);
    CHECK(dots==(visible?1u:3u) && bars==2);
    sink_state s={.stop=6};CHECK(ps_graphic_figure_draw(f,sink,&s)==PS_IO && s.seen==6);
    s=(sink_state){0};CHECK(ps_graphic_figure_draw(f,sink,&s)==PS_OK && s.seen==ps_graphic_figure_count(f));
    c.x=789;CHECK(ps_graphic_figure_command(f,ps_graphic_figure_count(f),&c)==PS_INVALID && c.x==789);
    return 0;
}
static int write_svg(const ps_graphic_figure *f,const char *path) {
    ps_graphic_svg *svg=NULL;CHECK(ps_graphic_figure_svg(f,0,ps_allocator_default(),&svg)==PS_OK);
    size_t n=0;const char *bytes=ps_graphic_svg_bytes(svg,&n);
    CHECK(bytes && n==strlen(bytes) && n>4000 && n<PS_GRAPHIC_FIGURE_SVG_LIMIT);
    CHECK(strstr(bytes,"viewBox=\"0 0 1200 850\"") && strstr(bytes,"Auslenkung &lt;&amp;&gt; α"));
    CHECK(strstr(bytes,"textLength=\"") && strstr(bytes,"clip-path=\"url(#plot-clip)\""));
    CHECK(!strstr(bytes,"<script") && !strstr(bytes,"<image") && !strstr(bytes,"stroke-dasharray"));
    FILE *file=fopen(path,"wbx");CHECK(file);bool ok=fwrite(bytes,1,n,file)==n;if(fclose(file))ok=false;CHECK(ok);
    ps_graphic_svg_destroy(svg);return 0;
}
static int ownership(void) {
    ps_graphic_axes axes={.full={0,10,-5,5},.visible={2,8,-2,2},.cropped=true};ps_graphic_snapshot *s=NULL;
    CHECK(make_snapshot(&axes,&s)==PS_OK);
    ps_graphic_figure *f=NULL;test_allocator tracker={0};
    CHECK(ps_graphic_figure_create(s,NULL,measure,NULL,test_domain(&tracker),&f)==PS_OK);
    size_t count=tracker.attempts;ps_graphic_figure_destroy(f);CHECK(count>=2 && !tracker.live_blocks && !tracker.invalid);
    for(size_t n=1;n<=count;n++) {
        test_allocator failing={.fail_on=n};f=NULL;
        CHECK(ps_graphic_figure_create(s,NULL,measure,NULL,test_domain(&failing),&f)==PS_MEMORY && !f);
        CHECK(!failing.live_blocks && !failing.live_bytes && !failing.invalid);
    }
    CHECK(ps_graphic_figure_create(s,NULL,bad_measure,NULL,ps_allocator_default(),&f)==PS_INVALID && !f);
    CHECK(ps_graphic_figure_create(s,NULL,measure,NULL,ps_allocator_default(),&f)==PS_OK);
    ps_graphic_figure *prior=f;
    CHECK(ps_graphic_figure_create(s,NULL,bad_measure,NULL,ps_allocator_default(),&prior)==PS_INVALID && prior==f);
    ps_graphic_snapshot_destroy(s);CHECK(verify(f,false,false)==0);
    ps_graphic_svg *svg=NULL;tracker=(test_allocator){0};
    CHECK(ps_graphic_figure_svg(f,0,test_domain(&tracker),&svg)==PS_OK);size_t svg_count=tracker.attempts;
    ps_graphic_svg_destroy(svg);CHECK(svg_count>=3 && !tracker.live_blocks && !tracker.invalid);
    for(size_t n=1;n<=svg_count;n++) {
        test_allocator failing={.fail_on=n};svg=NULL;
        CHECK(ps_graphic_figure_svg(f,0,test_domain(&failing),&svg)==PS_MEMORY && !svg);
        CHECK(!failing.live_blocks && !failing.live_bytes && !failing.invalid);
    }
    ps_graphic_figure_destroy(f);
    printf("GRAPHIC FIGURE OWNERSHIP: %zu scene and %zu SVG allocation failures, detached text/data and sink failure passed\n",count,svg_count);
    return 0;
}
static int resources(void) {
    ps_report *report=NULL;CHECK(ps_report_create("Limits","Bounded resource fixture",&report)==PS_OK);
    ps_plot_info p={.title="Limits"};ps_plot_handle h={0};
    CHECK(ps_report_unit_from(PS_ONE,&p.x_unit)==PS_OK && ps_report_unit_from(PS_ONE,&p.y_unit)==PS_OK);
    CHECK(ps_report_add_plot(report,&p,&h)==PS_OK);
    ps_curve_data *c=calloc(1,sizeof *c);CHECK(c);strcpy(c->label,"Dense zigzag");
    c->kind=PS_PLOT_LINE;c->count=c->source_count=1024;
    for(unsigned i=0;i<c->count;i++)c->x[i]=c->y[i]=i%2;
    for(unsigned i=0;i<3;i++)CHECK(ps_report_add_curve(report,h,c)==PS_OK);
    ps_graphic_axes axes={.full={0,1,0,1}};ps_graphic_snapshot *s=NULL;
    CHECK(ps_graphic_snapshot_from_report(report,0,&axes,0,ps_allocator_default(),&s)==PS_OK);
    ps_graphic_figure *f=NULL;CHECK(ps_graphic_figure_create(s,NULL,measure,NULL,ps_allocator_default(),&f)==PS_OK);
    CHECK(ps_graphic_figure_count(f)>100000 && ps_graphic_figure_count(f)<PS_GRAPHIC_FIGURE_COMMAND_LIMIT);
    test_allocator tracker={0};ps_graphic_svg *svg=NULL;
    ps_result limited=ps_graphic_figure_svg(f,1024,test_domain(&tracker),&svg);
    bool released=limited==PS_LIMIT && !svg && !tracker.live_blocks && !tracker.invalid;
    ps_graphic_svg_destroy(svg);
    ps_graphic_figure_destroy(f);ps_graphic_snapshot_destroy(s);
    if(!released){free(c);ps_report_destroy(report);CHECK(released);}
    CHECK(ps_report_add_curve(report,h,c)==PS_OK);
    CHECK(ps_graphic_snapshot_from_report(report,0,&axes,0,ps_allocator_default(),&s)==PS_OK);
    tracker=(test_allocator){0};f=NULL;
    CHECK(ps_graphic_figure_create(s,NULL,measure,NULL,test_domain(&tracker),&f)==PS_LIMIT && !f);
    CHECK(!tracker.live_blocks && !tracker.live_bytes && !tracker.invalid);
    ps_graphic_snapshot_destroy(s);ps_report_destroy(report);
    CHECK(ps_report_create("Extreme clip","Finite values",&report)==PS_OK && ps_report_add_plot(report,&p,&h)==PS_OK);
    c->count=c->source_count=2;c->x[0]=-1e308;c->x[1]=1e308;c->y[0]=c->y[1]=.5;
    CHECK(ps_report_add_curve(report,h,c)==PS_OK);free(c);
    axes=(ps_graphic_axes){.full={-1e308,1e308,0,1},.visible={-1e308,0,0,1},.cropped=true};
    CHECK(ps_graphic_snapshot_from_report(report,0,&axes,0,ps_allocator_default(),&s)==PS_OK);
    ps_graphic_figure_options o={.visible=true};CHECK(ps_graphic_figure_create(s,&o,measure,NULL,ps_allocator_default(),&f)==PS_OK);
    unsigned clipped=0;for(size_t i=0;i<ps_graphic_figure_count(f);i++) {
        ps_figure_command cmd;CHECK(ps_graphic_figure_command(f,i,&cmd)==PS_OK);
        if(cmd.curve==0){CHECK(cmd.kind==PS_FIGURE_LINE && cmd.x==116 && cmd.w==1148 && cmd.y==465 && cmd.h==465);clipped++;}
    }
    CHECK(clipped==1);ps_graphic_figure_destroy(f);ps_graphic_snapshot_destroy(s);ps_report_destroy(report);
    CHECK(ps_report_create("Offset","Adjacent large values",&report)==PS_OK && ps_report_add_plot(report,&p,&h)==PS_OK);
    c=calloc(1,sizeof *c);CHECK(c);strcpy(c->label,"Offset");c->kind=PS_PLOT_LINE;c->count=c->source_count=2;
    c->x[0]=1000000000000000.25;c->x[1]=1000000000000000.375;c->y[0]=c->y[1]=.5;
    CHECK(ps_report_add_curve(report,h,c)==PS_OK);
    axes=(ps_graphic_axes){.full={c->x[0],c->x[1],0,1}};free(c);
    CHECK(ps_graphic_snapshot_from_report(report,0,&axes,0,ps_allocator_default(),&s)==PS_OK);
    CHECK(ps_graphic_figure_create(s,NULL,measure,NULL,ps_allocator_default(),&f)==PS_OK);
    char tick[6][64]={{0}};unsigned ticks=0;bool distinct=true;
    for(size_t i=0;i<ps_graphic_figure_count(f);i++) {
        ps_figure_command cmd;CHECK(ps_graphic_figure_command(f,i,&cmd)==PS_OK);
        if(cmd.kind==PS_FIGURE_TEXT && cmd.y==734) {
            if(ticks>=6){distinct=false;continue;}
            for(unsigned j=0;j<ticks;j++)if(!strcmp(tick[j],cmd.text))distinct=false;
            snprintf(tick[ticks++],64,"%s",cmd.text);
        }
    }
    ps_graphic_figure_destroy(f);ps_graphic_snapshot_destroy(s);ps_report_destroy(report);
    CHECK(ticks==6 && distinct);
    puts("GRAPHIC FIGURE LIMITS: command/SVG budgets release all ownership; finite overflow clipping and six distinct large-offset ticks survive");
    return 0;
}
int main(int argc,char **argv) {
    CHECK(argc==2);char path[4096];ps_graphic_axes axes={.full={0,10,-5,5},.visible={2,8,-2,2},.cropped=true};
    ps_graphic_snapshot *s=NULL;CHECK(make_snapshot(&axes,&s)==PS_OK);
    for(unsigned theme=0;theme<2;theme++)for(unsigned crop=0;crop<2;crop++) {
        ps_graphic_figure_options o={.dark=theme!=0,.visible=crop!=0};ps_graphic_figure *f=NULL;
        CHECK(ps_graphic_figure_create(s,&o,measure,NULL,ps_allocator_default(),&f)==PS_OK && verify(f,o.dark,o.visible)==0);
        CHECK(snprintf(path,sizeof path,"%s/figure-%u-%u.svg",argv[1],theme,crop)>0 && write_svg(f,path)==0);
        ps_graphic_figure_destroy(f);
    }
    {
        ps_graphic_figure_options contrast={.dark=true,.high_contrast=true};ps_graphic_figure *figure=NULL;
        CHECK(ps_graphic_figure_create(s,&contrast,measure,NULL,ps_allocator_default(),&figure)==PS_OK);
        ps_figure_command command;CHECK(ps_graphic_figure_command(figure,0,&command)==PS_OK && command.color==0);
        bool curve=false,grid=false,title=false;
        for(size_t i=0;i<ps_graphic_figure_count(figure);i++) {
            CHECK(ps_graphic_figure_command(figure,i,&command)==PS_OK);
            if(command.curve==0){CHECK(command.color==0x8cc8ff);curve=true;}
            grid|=command.kind==PS_FIGURE_LINE && command.color==0x8c8c8c;
            title|=command.kind==PS_FIGURE_TEXT && command.strong && command.color==0xffffff;
        }
        CHECK(curve && grid && title);ps_graphic_figure_destroy(figure);
    }
    ps_graphic_snapshot_destroy(s);CHECK(ownership()==0);
    axes.visible[1]=1e-20;axes.visible[0]=0;s=NULL;CHECK(make_snapshot(&axes,&s)==PS_OK);
    ps_graphic_figure_options o={.visible=true};ps_graphic_figure *f=NULL;
    CHECK(ps_graphic_figure_create(s,&o,measure,NULL,ps_allocator_default(),&f)==PS_LIMIT && !f);
    ps_graphic_snapshot_destroy(s);
    axes=(ps_graphic_axes){.full={-DBL_MAX,DBL_MAX,-DBL_MAX,DBL_MAX}};
    CHECK(make_snapshot(&axes,&s)==PS_OK && ps_graphic_figure_create(s,NULL,measure,NULL,ps_allocator_default(),&f)==PS_OK);
    CHECK(snprintf(path,sizeof path,"%s/figure-extreme.svg",argv[1])>0 && write_svg(f,path)==0);
    ps_graphic_figure_destroy(f);ps_graphic_snapshot_destroy(s);
    CHECK(resources()==0);
    puts("GRAPHIC FIGURE: canonical layout, explicit reference, independent axes, masks, dashes, scatter, histograms, full XY crop, themes and bounded numeric errors passed");
    return 0;
}
