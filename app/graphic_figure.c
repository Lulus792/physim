#include "graphic_figure.h"
#include "plot_view.h"
#include "text_validation.h"
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define TEXT_COUNT 32u
#define TEXT_BYTES 768u
struct ps_graphic_figure {
    ps_allocator allocator;
    ps_figure_command *commands;
    size_t count,capacity;
    unsigned texts;
    char text[TEXT_COUNT][TEXT_BYTES];
    char family[2][96];
    ps_figure_measure measure;
    void *measure_user;
};
struct ps_graphic_svg {ps_allocator allocator;char *bytes;size_t length,capacity,limit;};
typedef struct {unsigned paper,ink,muted,grid,curves[8];} palette;
static const palette light={0xffffff,0x20242c,0x555e6c,0xdce1e9,
    {0x17569e,0x914800,0x00643b,0x7844a0,0xa52d52,0x126875,0x6e6410,0x4c4f9f}};
static const palette dark={0x1b1c20,0xedeef2,0xa6a9b1,0x3b3d45,
    {0x8abaff,0xf5b46c,0x8bcca5,0xd0a6ec,0xf4a9b8,0x7dd5df,0xddd48c,0xbcbcff}};
/* Same semantic colors as PS_UI_HIGH_CONTRAST, without a UI/SDL dependency. */
static const palette high_contrast={0x000000,0xffffff,0xd4d4d4,0x8c8c8c,
    {0x8cc8ff,0xffd282,0x82f5aa,0xdcb4ff,0xffaad7,0x8cebf5,0xfff08c,0xe6e6e6}};
static ps_result push(ps_graphic_figure *f,ps_figure_command command) {
    if(f->count==PS_GRAPHIC_FIGURE_COMMAND_LIMIT)return PS_LIMIT;
    if(f->count==f->capacity) {
        size_t next=f->capacity?f->capacity*2:128;
        if(next>PS_GRAPHIC_FIGURE_COMMAND_LIMIT)next=PS_GRAPHIC_FIGURE_COMMAND_LIMIT;
        void *memory=NULL;ps_result r=ps_memory_resize(f->allocator,f->commands,
            f->capacity*sizeof *f->commands,next*sizeof *f->commands,&memory);
        if(r!=PS_OK)return r;
        f->commands=memory;f->capacity=next;
    }
    f->commands[f->count++]=command;return PS_OK;
}
static ps_result line(ps_graphic_figure *f,double x,double y,double xx,double yy,double width,unsigned color,int curve) {
    return push(f,(ps_figure_command){.kind=PS_FIGURE_LINE,.color=color,.x=x,.y=y,.w=xx,.h=yy,.stroke=width,.curve=curve});
}
static ps_result text(ps_graphic_figure *f,double x,double y,double width,double size,unsigned color,
    ps_figure_anchor anchor,bool strong,double spacing,const char *value) {
    size_t n=strlen(value);if(n>=TEXT_BYTES || f->texts==TEXT_COUNT)return PS_LIMIT;
    double advance=f->measure(f->measure_user,value,size,spacing,strong);
    if(!isfinite(advance) || advance<0 || (n && !advance))return PS_INVALID;
    if(advance>width) {
        double factor=width/advance;size*=factor;spacing*=factor;
        advance=f->measure(f->measure_user,value,size,spacing,strong);
        if(!isfinite(advance) || advance<=0 || advance>width+1e-5)return PS_INVALID;
    }
    char *owned=f->text[f->texts++];memcpy(owned,value,n+1);
    return push(f,(ps_figure_command){.kind=PS_FIGURE_TEXT,.color=color,.x=x,.y=y,.w=width,
        .h=size,.spacing=spacing,.advance=advance,.text=owned,.anchor=anchor,.strong=strong,.curve=-1});
}
void ps_graphic_figure_destroy(ps_graphic_figure *f) {
    if(f){ps_allocator a=f->allocator;ps_memory_free(a,f->commands,f->capacity*sizeof *f->commands);ps_memory_free(a,f,sizeof *f);}
}
size_t ps_graphic_figure_count(const ps_graphic_figure *f){return f?f->count:0;}
ps_result ps_graphic_figure_command(const ps_graphic_figure *f,size_t index,ps_figure_command *out) {
    if(!f || !out || index>=f->count)return PS_INVALID;
    *out=f->commands[index];return PS_OK;
}
ps_result ps_graphic_figure_draw(const ps_graphic_figure *f,ps_figure_sink sink,void *user) {
    if(!f || !sink)return PS_INVALID;
    for(size_t i=0;i<f->count;i++){ps_result r=sink(user,f->commands+i);if(r!=PS_OK)return r;}
    return PS_OK;
}
/* Native captions use decimal commas. Scientific notation protects small or
 * large values from ambiguous fixed-decimal ticks. Offset labels preserve a
 * large common origin. No locale mutation is needed or performed. */
static void number(char out[64],double value,double step,bool y) {
    unsigned digits=y?2:0;double s=fabs(step);
    if(s>0 && s<1)digits=(unsigned)fmin(8,ceil(-log10(s))+1);
    else if(s>=1 && floor(s)!=s)digits=y?2:1;
    if(value==0)value=0;
    if((fabs(value)>0 && fabs(value)<1e-7) || fabs(value)>=1e9 || (s>0 && s<1e-7))snprintf(out,64,"%.9g",value);
    else snprintf(out,64,"%.*f",(int)digits,value);
    for(char *p=out;*p;p++)if(*p=='.')*p=',';
}
static void range_numbers(char first[64],char last[64],double low,double high) {
    snprintf(first,64,"%.3f",low);snprintf(last,64,"%.3f",high);
    if(fmax(fabs(low),fabs(high))>=1e9 || high-low<.001 || !strcmp(first,last)) {
        snprintf(first,64,"%.17g",low);snprintf(last,64,"%.17g",high);
    }
    for(char *p=first;*p;p++)if(*p=='.')*p=',';
    for(char *p=last;*p;p++)if(*p=='.')*p=',';
}
static unsigned blend(unsigned a,unsigned b) {
    unsigned c=0;for(unsigned shift=0;shift<24;shift+=8)c|=((((a>>shift)&255)+((b>>shift)&255))/2)<<shift;return c;
}
static bool mapped(double value,double low,double high,double *out) {
    double v=ps_report_axis_fraction(value,low,high);
    /* An offscreen finite point can overflow value-low while its ratio remains
     * small and representable (e.g. 1e308 in [-1e308,0]). */
    if(!isfinite(v) && isfinite(high-low))v=value/(high-low)-low/(high-low);
    /* Beyond this bound double clipping cannot promise subpixel positions.
     * Report an unsupported extent rather than silently losing a curve. */
    if(!isfinite(v) || fabs(v)>1e9)return false;
    *out=v;return true;
}
static ps_result segment(ps_graphic_figure *f,double x,double y,double xx,double yy,
    double dash,double *phase,unsigned color,int curve) {
    double x0=x,y0=y,x1=xx,y1=yy;
    double dx=(xx-x)*1032,dy=(yy-y)*470,length=hypot(dx,dy);
    if(!isfinite(length))return PS_LIMIT;
    double period=dash+6,old=*phase;
    if(dash)*phase=fmod(old+fmod(length,period),period);
    if(!ps_plot_clip_line(&x0,&y0,&x1,&y1))return PS_OK;
    double ax=116+x0*1032,ay=700-y0*470,bx=116+x1*1032,by=700-y1*470;
    if(!dash)return line(f,ax,ay,bx,by,2.5,color,curve);
    double clipped=hypot(bx-ax,by-ay);
    if(clipped==0)return PS_OK;
    double skipped=hypot((x0-x)*1032,(y0-y)*470);
    double at=0,pos=fmod(old+fmod(skipped,period),period);
    while(at<clipped) {
        bool ink=pos<dash;double amount=fmin(clipped-at,(ink?dash:period)-pos);
        if(amount<=0)return PS_NUMERIC;
        if(ink){double a=at/clipped,b=(at+amount)/clipped;
            ps_result r=line(f,ax+(bx-ax)*a,ay+(by-ay)*a,ax+(bx-ax)*b,ay+(by-ay)*b,2.5,color,curve);if(r!=PS_OK)return r;}
        at+=amount;pos=ink?dash:0;
    }
    return PS_OK;
}
static ps_result curve_draw(ps_graphic_figure *f,const ps_curve_data *c,const uint8_t *mask,
    const double b[4],double dash,unsigned color,int index) {
    double phase=0,previous_x=0,previous_y=0;
    for(uint32_t i=0;i<c->count;i++) {
        if(mask && !(mask[i]&1))continue;
        double x,y;if(!mapped(c->x[i],b[0],b[1],&x) || !mapped(c->y[i],b[2],b[3],&y))return PS_LIMIT;
        bool before=i && (!mask || ((mask[i-1]&1) && !(mask[i]&2)));
        bool after=i+1<c->count && (!mask || ((mask[i+1]&1) && !(mask[i+1]&2)));
        ps_result r=PS_OK;
        if(c->kind==PS_PLOT_HISTOGRAM) {
            double left,right,zero;
            if(!mapped(c->x[i]-c->bar_width/2,b[0],b[1],&left) ||
               !mapped(c->x[i]+c->bar_width/2,b[0],b[1],&right) || !mapped(0,b[2],b[3],&zero))return PS_LIMIT;
            double top=fmax(0,fmin(y,zero)),bottom=fmin(1,fmax(y,zero));
            left=fmax(0,left);right=fmin(1,right);
            if(left<right && top<bottom)r=push(f,(ps_figure_command){.kind=PS_FIGURE_RECT,.color=color,
                .x=116+left*1032,.y=700-bottom*470,.w=(right-left)*1032,.h=(bottom-top)*470,.curve=index});
        } else if(c->kind==PS_PLOT_LINE && before)r=segment(f,previous_x,previous_y,x,y,dash,&phase,color,index);
        if(c->kind==PS_PLOT_SCATTER || (c->kind==PS_PLOT_LINE && !before && !after)) {
            if(x>=0 && x<=1 && y>=0 && y<=1)r=push(f,(ps_figure_command){.kind=PS_FIGURE_CIRCLE,
                .color=color,.x=116+x*1032,.y=700-y*470,.w=3,.curve=index});
        }
        if(r!=PS_OK)return r;
        if(!before)phase=0;
        previous_x=x;previous_y=y;
    }
    return PS_OK;
}
/* Every adapter consumes these same canonical coordinates, including explicit
 * dash segments. No independent SVG or raster plot-layout implementation. */
ps_result ps_graphic_figure_create(const ps_graphic_snapshot *snapshot,const ps_graphic_figure_options *options,
    ps_figure_measure measure,void *user,ps_allocator allocator,ps_graphic_figure **out) {
    ps_graphic_description d;ps_graphic_figure_options o={0};if(options)o=*options;
    if(!out || !measure || !ps_allocator_valid(allocator) || ps_graphic_snapshot_describe(snapshot,&d)!=PS_OK ||
       (o.visible && !d.axes.cropped))return PS_INVALID;
    if((o.font_family && (!*o.font_family || !ps_text_valid(o.font_family,96,false))) ||
       (o.strong_font_family && (!*o.strong_font_family || !ps_text_valid(o.strong_font_family,96,false))))return PS_INVALID;
    void *memory=NULL;ps_result r=ps_memory_zero(allocator,1,sizeof(ps_graphic_figure),&memory);if(r!=PS_OK)return r;
    ps_graphic_figure *f=memory;f->allocator=allocator;f->measure=measure;f->measure_user=user;
    if(o.font_family)snprintf(f->family[0],96,"%s",o.font_family);
    if(o.strong_font_family || o.font_family)snprintf(f->family[1],96,"%s",o.strong_font_family?o.strong_font_family:o.font_family);
    const palette *p=o.high_contrast?&high_contrast:o.dark?&dark:&light;
    const double *b=o.visible?d.axes.visible:d.axes.full;
    double xo=ps_plot_axis_offset(b[0],b[1]),yo=ps_plot_axis_offset(b[2],b[3]);
    double xs=(b[1]-xo)/5-(b[0]-xo)/5,ys=(b[3]-yo)/4-(b[2]-yo)/4;
    const ps_report *report=ps_graphic_snapshot_report(snapshot);
    char title[192],provenance[8192],label[TEXT_BYTES],a[64],z[64];
#define DO(call) do{r=(call);if(r!=PS_OK)goto failed;}while(0)
    DO(ps_report_describe(report,title,provenance,NULL,NULL));
    DO(push(f,(ps_figure_command){.kind=PS_FIGURE_RECT,.color=p->paper,.w=1200,.h=850,.curve=-1}));
    DO(text(f,64,58,1084,13,p->muted,PS_FIGURE_START,false,2,"PHYSIM / AUSWERTUNG"));
    DO(text(f,64,104,1084,34,p->ink,PS_FIGURE_START,true,0,d.plot.title));
    range_numbers(a,z,b[0],b[1]);
    snprintf(label,sizeof label,"%s · %s–%s%s%s",title,a,z,d.plot.x_unit.symbol[0]?" ":"",d.plot.x_unit.symbol);
    DO(text(f,64,136,1084,18,p->muted,PS_FIGURE_START,false,0,label));
    for(uint32_t i=0;i<d.plot.curves;i++) {
        const ps_curve_data *c=NULL;DO(ps_report_curve_view(report,0,i,&c));
        double x=64+(i%4)*280,y=173+(i/4)*30;
        bool ref=i==d.reference;double dash=(ref || (d.reference==PS_GRAPHIC_NO_REFERENCE && !i))?0:(i%2?10:3);
        if(c->kind==PS_PLOT_SCATTER)DO(push(f,(ps_figure_command){.kind=PS_FIGURE_CIRCLE,.color=p->curves[i],.x=x+15,.y=y-5,.w=3,.curve=-1}));
        else if(c->kind==PS_PLOT_HISTOGRAM)DO(push(f,(ps_figure_command){.kind=PS_FIGURE_RECT,.color=p->curves[i],.x=x,.y=y-13,.w=30,.h=12,.curve=-1}));
        else for(double at=0;at<30;) {
            double end=dash?fmin(30,at+dash):30;DO(line(f,x+at,y-5,x+end,y-5,3,p->curves[i],-1));at=end+6;
        }
        snprintf(label,sizeof label,"%s%s",c->label,ref?" · Referenz":"");
        DO(text(f,x+40,y,fmin(232,1148-(x+40)),17,p->muted,PS_FIGURE_START,false,0,label));
    }
    for(unsigned i=0;i<=4;i++) {
        double y=230+i*470./4,value=ps_plot_axis_value(b[2]-yo,b[3]-yo,1-i/4.);
        DO(line(f,116,y,1148,y,1,p->grid,-1));number(label,value,ys,true);
        DO(text(f,98,y+7,90,19,p->muted,PS_FIGURE_END,false,0,label));
    }
    for(unsigned i=0;i<=5;i++) {
        double x=116+i*1032./5,value=ps_plot_axis_value(b[0]-xo,b[1]-xo,i/5.);
        DO(line(f,x,230,x,700,1,p->grid,-1));number(label,value,xs,false);
        DO(text(f,x,734,180,19,p->muted,PS_FIGURE_MIDDLE,false,0,label));
    }
    if(b[2]<0 && b[3]>0)DO(line(f,116,700-ps_report_axis_fraction(0,b[2],b[3])*470,1148,
        700-ps_report_axis_fraction(0,b[2],b[3])*470,1,blend(p->muted,p->paper),-1));
    snprintf(label,sizeof label,"%s%s%s%s",d.plot.y_label,d.plot.y_unit.symbol[0]?" [":"",d.plot.y_unit.symbol,d.plot.y_unit.symbol[0]?"]":"");
    DO(text(f,116,216,1032,19,p->muted,PS_FIGURE_START,false,0,label));
    snprintf(label,sizeof label,"%s%s%s%s",d.plot.x_label,d.plot.x_unit.symbol[0]?" [":"",d.plot.x_unit.symbol,d.plot.x_unit.symbol[0]?"]":"");
    DO(text(f,1148,773,1032,19,p->muted,PS_FIGURE_END,false,0,label));
    DO(push(f,(ps_figure_command){.kind=PS_FIGURE_CLIP,.x=116,.y=230,.w=1032,.h=470,.curve=-1}));
    for(uint32_t i=0;i<d.plot.curves;i++) {
        const ps_curve_data *c=NULL;const uint8_t *mask=NULL;
        DO(ps_report_curve_view(report,0,i,&c));DO(ps_report_curve_mask(report,0,i,&mask));
        double dash=(i==d.reference || (d.reference==PS_GRAPHIC_NO_REFERENCE && !i))?0:(i%2?10:3);
        DO(curve_draw(f,c,mask,b,dash,p->curves[i],(int)i));
    }
    DO(push(f,(ps_figure_command){.kind=PS_FIGURE_UNCLIP,.curve=-1}));
    if(xo || yo) {
        number(a,xo,xs,false);number(z,yo,ys,false);
        snprintf(label,sizeof label,"Achsenoffset: X %s %s · Y %s %s · Wert = Offset + Achsenwert",a,d.plot.x_unit.symbol,z,d.plot.y_unit.symbol);
        DO(text(f,64,794,1084,12,p->muted,PS_FIGURE_START,false,0,label));
    }
    snprintf(label,sizeof label,"%s · %u %s%s",o.visible?"Sichtbarer Ausschnitt":"Gesamtes Diagramm",d.plot.curves,
        d.plot.curves==1?"Kurve":"Kurven",d.reduced?" · Vorschau reduziert; mehr Pixel erzeugen keine Messpunkte":"");
    DO(text(f,64,814,1084,14,p->muted,PS_FIGURE_START,false,0,label));
    f->measure=NULL;f->measure_user=NULL;*out=f;return PS_OK;
failed:ps_graphic_figure_destroy(f);return r;
#undef DO
}
static ps_result append(ps_graphic_svg *s,const char *bytes,size_t count) {
    if(count>s->limit-s->length-1)return PS_LIMIT;
    size_t needed=s->length+count+1;
    if(needed>s->capacity) {
        size_t next=s->capacity?s->capacity:4096;while(next<needed)next*=2;
        if(next>s->limit)next=s->limit;
        void *memory=NULL;ps_result r=ps_memory_resize(s->allocator,s->bytes,s->capacity,next,&memory);if(r!=PS_OK)return r;
        s->bytes=memory;s->capacity=next;
    }
    memcpy(s->bytes+s->length,bytes,count);s->length+=count;s->bytes[s->length]=0;return PS_OK;
}
static ps_result literal(ps_graphic_svg *s,const char *text){return append(s,text,strlen(text));}
static ps_result xml(ps_graphic_svg *s,const char *text) {
    for(const char *p=text;*p;p++) {
        const char *escaped=NULL;
        switch(*p){case '&':escaped="&amp;";break;case '<':escaped="&lt;";break;case '>':escaped="&gt;";break;
        case '\"':escaped="&quot;";break;case '\'':escaped="&apos;";break;default:break;}
        ps_result r=escaped?literal(s,escaped):append(s,p,1);if(r!=PS_OK)return r;
    }
    return PS_OK;
}
/* Locale-independent fixed SVG coordinates, all in [0,1200]. */
static void scalar(char out[32],double value) {
    unsigned long long scaled=(unsigned long long)floor(value*1000000+.5);
    snprintf(out,32,"%llu.%06u",scaled/1000000,(unsigned)(scaled%1000000));
}
typedef struct {ps_graphic_svg *svg;const ps_graphic_figure *figure;} svg_drawing;
static ps_result svg_command(void *user,const ps_figure_command *c) {
    svg_drawing *drawing=user;ps_graphic_svg *s=drawing->svg;
    char x[32],y[32],w[32],h[32],stroke[32],spacing[32],advance[32],linebuf[768];
    scalar(x,c->x);scalar(y,c->y);scalar(w,c->w);scalar(h,c->h);scalar(stroke,c->stroke);scalar(spacing,c->spacing);
    switch(c->kind) {
    case PS_FIGURE_RECT:snprintf(linebuf,sizeof linebuf,"<rect x=\"%s\" y=\"%s\" width=\"%s\" height=\"%s\" fill=\"#%06x\"/>\n",x,y,w,h,c->color);break;
    case PS_FIGURE_LINE:snprintf(linebuf,sizeof linebuf,"<line x1=\"%s\" y1=\"%s\" x2=\"%s\" y2=\"%s\" stroke=\"#%06x\" stroke-width=\"%s\"/>\n",x,y,w,h,c->color,stroke);break;
    case PS_FIGURE_CIRCLE:snprintf(linebuf,sizeof linebuf,"<circle cx=\"%s\" cy=\"%s\" r=\"%s\" fill=\"#%06x\"/>\n",x,y,w,c->color);break;
    case PS_FIGURE_CLIP:return literal(s,"<g clip-path=\"url(#plot-clip)\">\n");
    case PS_FIGURE_UNCLIP:return literal(s,"</g>\n");
    case PS_FIGURE_TEXT: {
        const char *anchor=c->anchor==PS_FIGURE_END?"end":c->anchor==PS_FIGURE_MIDDLE?"middle":"start";
        scalar(advance,c->advance);
        snprintf(linebuf,sizeof linebuf,"<text x=\"%s\" y=\"%s\" font-size=\"%s\" font-weight=\"%u\" letter-spacing=\"%s\" text-anchor=\"%s\" fill=\"#%06x\"",x,y,h,c->strong?600:400,spacing,anchor,c->color);
        ps_result r=literal(s,linebuf);
        const char *family=drawing->figure->family[c->strong?1:0];
        if(r==PS_OK && *family)r=literal(s," font-family=\"");
        if(r==PS_OK && *family)r=xml(s,family);
        if(r==PS_OK && *family)r=literal(s,"\"");
        if(r==PS_OK && c->advance>0){snprintf(linebuf,sizeof linebuf," textLength=\"%s\" lengthAdjust=\"spacingAndGlyphs\"",advance);r=literal(s,linebuf);}
        if(r==PS_OK)r=literal(s,">");
        if(r==PS_OK)r=xml(s,c->text);
        if(r==PS_OK)r=literal(s,"</text>\n");
        return r;
    }
    default:return PS_INVALID;
    }
    return literal(s,linebuf);
}
void ps_graphic_svg_destroy(ps_graphic_svg *s) {
    if(s){ps_allocator a=s->allocator;ps_memory_free(a,s->bytes,s->capacity);ps_memory_free(a,s,sizeof *s);}
}
const char *ps_graphic_svg_bytes(const ps_graphic_svg *s,size_t *length) {
    if(!s)return NULL;
    if(length)*length=s->length;
    return s->bytes;
}
ps_result ps_graphic_figure_svg(const ps_graphic_figure *f,size_t byte_limit,ps_allocator allocator,ps_graphic_svg **out) {
    if(!f || !out || !ps_allocator_valid(allocator))return PS_INVALID;
    if(byte_limit>PS_GRAPHIC_FIGURE_SVG_LIMIT)return PS_LIMIT;
    void *memory=NULL;ps_result r=ps_memory_zero(allocator,1,sizeof(ps_graphic_svg),&memory);if(r!=PS_OK)return r;
    ps_graphic_svg *s=memory;s->allocator=allocator;s->limit=byte_limit?byte_limit:PS_GRAPHIC_FIGURE_SVG_LIMIT;
    r=literal(s,"<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"1200\" height=\"850\" viewBox=\"0 0 1200 850\" role=\"img\" aria-labelledby=\"figure-title figure-desc\">\n<title id=\"figure-title\">");
    const char *title="Diagramm";for(size_t i=0;i<f->count;i++)if(f->commands[i].kind==PS_FIGURE_TEXT && f->commands[i].strong){title=f->commands[i].text;break;}
    if(r==PS_OK)r=xml(s,title);
    if(r==PS_OK)r=literal(s,"</title><desc id=\"figure-desc\">Physim: festgehaltene native Diagrammdaten.</desc>\n<defs><clipPath id=\"plot-clip\"><rect x=\"116\" y=\"230\" width=\"1032\" height=\"470\"/></clipPath></defs>\n<g font-family=\"Segoe UI, Arial, sans-serif\">\n");
    svg_drawing drawing={s,f};
    if(r==PS_OK)r=ps_graphic_figure_draw(f,svg_command,&drawing);
    if(r==PS_OK)r=literal(s,"</g></svg>\n");
    if(r!=PS_OK){ps_graphic_svg_destroy(s);return r;}
    *out=s;return PS_OK;
}
