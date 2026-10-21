#include "graphic_snapshot.h"
#include "physim/data.h"
#include "test_allocator.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Graphic snapshot %d: %s\n",__LINE__,#x);return 1;}}while(0)

static int report_copy(void) {
    ps_report *report=NULL;CHECK(ps_report_create("Original <&> α","immutable provenance\nfixture",&report)==PS_OK);
    ps_plot_info plot={.title="Position",.x_label="Zeit",.y_label="Weg"};ps_plot_handle handle;
    CHECK(ps_report_unit_from(PS_SECOND,&plot.x_unit)==PS_OK && ps_report_unit_from(PS_METRE,&plot.y_unit)==PS_OK);
    CHECK(ps_report_add_plot(report,&plot,&handle)==PS_OK);
    ps_curve_data *curve=calloc(1,sizeof *curve);CHECK(curve);
    strcpy(curve->label,"Referenz ä");curve->kind=PS_PLOT_LINE;curve->count=5;curve->source_count=100;
    for(unsigned i=0;i<5;i++){curve->x[i]=i;curve->y[i]=(double)i-2;}
    uint8_t mask[]={3,1,0,3,1};CHECK(ps_report_add_curve_masked(report,handle,curve,mask)==PS_OK);
    curve->kind=PS_PLOT_SCATTER;curve->count=2;curve->source_count=2;curve->x[0]=.25;curve->x[1]=3.25;
    strcpy(curve->label,"Vergleich");CHECK(ps_report_add_curve(report,handle,curve)==PS_OK);free(curve);
    ps_graphic_axes axes={.full={-1,5,-3,8},.visible={.25,2,-1,3},.cropped=true};
    ps_graphic_snapshot *snapshot=NULL;CHECK(ps_graphic_snapshot_from_report(report,0,&axes,1,ps_allocator_default(),&snapshot)==PS_OK);
    ps_graphic_description description;CHECK(ps_graphic_snapshot_describe(snapshot,&description)==PS_OK);
    CHECK(description.plot.curves==2 && description.reference==1 && description.reduced && !description.input_count && description.axes.cropped);
    CHECK(description.axes.visible[0]==.25 && !strcmp(description.plot.title,"Position"));
    const ps_curve_data *owned,*original;const uint8_t *flags;
    CHECK(ps_report_curve_view(ps_graphic_snapshot_report(snapshot),0,0,&owned)==PS_OK);
    CHECK(ps_report_curve_view(report,0,0,&original)==PS_OK && owned!=original && !memcmp(owned,original,sizeof *owned));
    CHECK(ps_report_curve_mask(ps_graphic_snapshot_report(snapshot),0,0,&flags)==PS_OK && flags && !memcmp(flags,mask,5));
    test_allocator tracker={0};ps_graphic_snapshot *candidate=NULL;
    CHECK(ps_graphic_snapshot_from_report(report,0,&axes,1,test_domain(&tracker),&candidate)==PS_OK);
    size_t allocations=tracker.attempts;ps_graphic_snapshot_destroy(candidate);
    CHECK(allocations>=5 && !tracker.invalid && !tracker.live_bytes && !tracker.live_blocks);
    for(size_t fail=1;fail<=allocations;fail++) {
        test_allocator failing={.fail_on=fail};candidate=snapshot;
        CHECK(ps_graphic_snapshot_from_report(report,0,&axes,1,test_domain(&failing),&candidate)==PS_MEMORY && candidate==snapshot);
        CHECK(!failing.live_blocks && !failing.live_bytes && !failing.invalid);
    }
    axes.visible[0]=NAN;candidate=snapshot;
    CHECK(ps_graphic_snapshot_from_report(report,0,&axes,1,ps_allocator_default(),&candidate)==PS_INVALID && candidate==snapshot);
    CHECK(ps_graphic_snapshot_from_report(report,0,NULL,2,ps_allocator_default(),&candidate)==PS_INVALID && candidate==snapshot);
    CHECK(ps_graphic_snapshot_copy(snapshot,NULL,ps_allocator_default(),&candidate)==PS_OK && candidate!=snapshot);
    ps_report_destroy(report);ps_graphic_snapshot_destroy(snapshot);
    CHECK(ps_graphic_snapshot_describe(candidate,&description)==PS_OK && description.axes.visible[0]==.25 && description.reference==1);
    char title[192],provenance[8192];CHECK(ps_report_describe(ps_graphic_snapshot_report(candidate),title,provenance,NULL,NULL)==PS_OK);
    CHECK(!strcmp(title,"Original <&> α") && !strcmp(provenance,"immutable provenance\nfixture"));
    ps_graphic_snapshot_destroy(candidate);
    printf("GRAPHIC SNAPSHOT REPORT: deep-copy, masks, kinds, reference, crop and %zu allocation failures passed\n",allocations);
    return 0;
}
static bool make_run(const char *path,unsigned count,double start,double step,bool reorder,int mode) {
    ps_context context={0};context.dt_s=step;
    if(reorder && ps_channel_add(&context,"other",PS_ONE,"")<0)return false;
    if(ps_channel_add(&context,"position",PS_METRE,"")<0 || ps_channel_add(&context,"position.status",PS_ONE,"")<0)return false;
    ps_run_writer writer;if(ps_run_create(&writer,path,&context,"Snapshot fixture")!=PS_OK)return false;
    bool ok=true;
    for(unsigned i=0;i<count && ok;i++) {
        double value=i==2783?2000:i==2784?-2000:(double)(i%10)*.01;
        double status=mode==1?3:mode==2?0:(i==0 || i==count-1 || i==100 || i==101)?(i%2?2:0):1;
        if(mode==3)value=i%2?DBL_MAX:-DBL_MAX;
        double values[3]={value,status,0};if(reorder){values[0]=999;values[1]=value;values[2]=status;}
        ok=ps_run_append(&writer,start+step*i,values)==PS_OK;
    }
    return ps_run_close(&writer)==PS_OK && ok;
}
typedef struct {unsigned calls,stop;} stop_state;
static bool stop(void *user){stop_state *s=user;return ++s->calls>=s->stop;}
static bool simple_run(const char *path,const char *name,ps_unit unit) {
    ps_context context={0};context.dt_s=1;
    if(ps_channel_add(&context,name[0]?name:"legacy-name",unit,"")<0)return false;
    /* The raw codec can serialize this schema, but the analysis/Series reader
     * requires nonempty names. Keep its rejection as a negative fixture. */
    if(!name[0])context.channels[0].name[0]=0;
    ps_run_writer writer;if(ps_run_create(&writer,path,&context,"Unit fixture")!=PS_OK)return false;
    bool ok=true;for(unsigned i=0;i<3 && ok;i++){double v[]={i*.1};ok=ps_run_append(&writer,i,v)==PS_OK;}
    return ps_run_close(&writer)==PS_OK && ok;
}
static int angles_and_names(const char *work) {
    char angle[4096],scalar[4096],unnamed[4096],long_name[4096],prefix[4096];
    snprintf(angle,sizeof angle,"%s/angle.psrun",work);snprintf(scalar,sizeof scalar,"%s/scalar.psrun",work);
    snprintf(prefix,sizeof prefix,"%s/names-work",work);
    snprintf(unnamed,sizeof unnamed,"%s/empty-name.psrun",work);
    int begin=snprintf(long_name,sizeof long_name,"%s/",work);CHECK(begin>0 && (size_t)begin+107<sizeof long_name);
    for(unsigned i=0;i<50;i++)memcpy(long_name+(size_t)begin+2*i,"α",2);
    memcpy(long_name+(size_t)begin+100,".psrun",7);
    CHECK(simple_run(angle,"theta",PS_RADIAN) && simple_run(scalar,"theta",PS_ONE) && simple_run(unnamed,"",PS_METRE) && simple_run(long_name,"position",PS_METRE));
    ps_graphic_run_request r={.count=1,.reference=0,.channel={.name="theta",.unit="rad"},.display_unit={.scale=PS_PI/180,.symbol="°"}};
    r.inputs[0]=(ps_graphic_run_input){.path=angle,.kind=PS_PLOT_SCATTER};
    ps_graphic_snapshot *s=NULL;ps_graphic_failure failure;
    CHECK(ps_graphic_snapshot_from_runs(&r,prefix,0,NULL,NULL,ps_allocator_default(),&s,&failure)==PS_OK);
    const ps_curve_data *c;CHECK(ps_report_curve_view(ps_graphic_snapshot_report(s),0,0,&c)==PS_OK);
    CHECK(c->kind==PS_PLOT_SCATTER && c->count==3 && fabs(c->y[1]-.1*180/PS_PI)<1e-12);
    ps_graphic_snapshot *same=s;r.inputs[0].path=scalar;
    CHECK(ps_graphic_snapshot_from_runs(&r,prefix,0,NULL,NULL,ps_allocator_default(),&same,&failure)==PS_INVALID && same==s && failure.input==0);
    r.inputs[0].path=angle;r.inputs[0].kind=PS_PLOT_HISTOGRAM;
    CHECK(ps_graphic_snapshot_from_runs(&r,prefix,0,NULL,NULL,ps_allocator_default(),&same,&failure)==PS_INVALID && same==s);
    r.inputs[0].kind=PS_PLOT_LINE;r.inputs[0].label="bad\xc0\xaf";
    CHECK(ps_graphic_snapshot_from_runs(&r,prefix,0,NULL,NULL,ps_allocator_default(),&same,&failure)==PS_INVALID && same==s);
    r.inputs[0]=(ps_graphic_run_input){.path=unnamed};
    r.channel=(ps_channel){.name="position",.unit="m",.dimension={1}};r.display_unit=(ps_report_unit){.dimension={1},.scale=1,.symbol="m"};
    CHECK(ps_graphic_snapshot_from_runs(&r,prefix,0,NULL,NULL,ps_allocator_default(),&same,&failure)==PS_INVALID && same==s);
    ps_graphic_snapshot_destroy(s);s=NULL;r.inputs[0].path=long_name;
    CHECK(ps_graphic_snapshot_from_runs(&r,prefix,0,NULL,NULL,ps_allocator_default(),&s,&failure)==PS_OK);
    ps_graphic_description d;ps_graphic_run_info run;CHECK(ps_graphic_snapshot_describe(s,&d)==PS_OK && !strcmp(d.plot.title,"position"));
    CHECK(ps_graphic_snapshot_run(s,0,&run)==PS_OK && !strcmp(run.channel.name,"position") && !strcmp(run.path,long_name));
    CHECK(strlen(run.label)==95 && !memcmp(run.label+92,"…",4));
    ps_graphic_snapshot_destroy(s);CHECK(!remove(angle) && !remove(scalar) && !remove(unnamed) && !remove(long_name));
    puts("GRAPHIC SNAPSHOT TYPES: scatter, rad/degree, angle/scalar separation, invalid unnamed schema and intact UTF8 labels passed");return 0;
}
static int runs(const char *work) {
    char a[4096],b[4096],small[4096],bad[4096],missing[4096],numeric[4096],prefix[4096];
    snprintf(a,sizeof a,"%s/Referenz ä.psrun",work);snprintf(b,sizeof b,"%s/Vergleich.psrun",work);
    snprintf(small,sizeof small,"%s/small.psrun",work);snprintf(bad,sizeof bad,"%s/invalid-status.psrun",work);
    snprintf(missing,sizeof missing,"%s/all-missing.psrun",work);snprintf(numeric,sizeof numeric,"%s/numeric-statistics.psrun",work);
    snprintf(prefix,sizeof prefix,"%s/snapshot-work",work);
    CHECK(make_run(a,6001,0,1,false,0) && make_run(b,9,.25,1.5,true,0) && make_run(small,9,0,.5,false,0));
    CHECK(make_run(bad,9,0,1,true,1) && make_run(missing,9,0,1,false,2) && make_run(numeric,9,0,1,false,3));
    ps_graphic_run_request request={.count=2,.reference=1,.channel={.name="position",.unit="m",.dimension={1}},
        .display_unit={.dimension={1},.scale=.01,.symbol="cm"}};
    request.inputs[0]=(ps_graphic_run_input){.path=a};request.inputs[1]=(ps_graphic_run_input){.path=b,.label="B α"};
    ps_graphic_snapshot *snapshot=NULL;ps_graphic_failure failure;
    CHECK(ps_graphic_snapshot_from_runs(&request,prefix,0,NULL,NULL,ps_allocator_default(),&snapshot,&failure)==PS_OK && failure.result==PS_OK);
    ps_graphic_description d;CHECK(ps_graphic_snapshot_describe(snapshot,&d)==PS_OK);
    CHECK(d.input_count==2 && d.reference==1 && d.reduced && d.axes.full[0]==0 && d.axes.full[1]==6000 && !d.axes.cropped);
    CHECK(d.plot.y_unit.scale==.01 && !strcmp(d.plot.y_unit.symbol,"cm"));
    ps_graphic_run_info info;CHECK(ps_graphic_snapshot_run(snapshot,0,&info)==PS_OK);
    CHECK(info.samples==6001 && info.statistics_result==PS_OK && info.si_statistics.count==5997 && info.si_statistics.min==-2000 && info.si_statistics.max==2000);
    CHECK(!strcmp(info.path,a) && !strcmp(info.label,"Referenz ä.psrun"));
    CHECK(ps_graphic_snapshot_run(snapshot,1,&info)==PS_OK && info.samples==9 && !strcmp(info.label,"B α"));
    const ps_curve_data *curve;const uint8_t *mask;CHECK(ps_report_curve_view(ps_graphic_snapshot_report(snapshot),0,0,&curve)==PS_OK);
    CHECK(curve->source_count==6001 && curve->count<=PS_REPORT_MAX_POINTS);
    bool low=false,high=false,segment=false;CHECK(ps_report_curve_mask(ps_graphic_snapshot_report(snapshot),0,0,&mask)==PS_OK && mask);
    for(uint32_t i=0;i<curve->count;i++){low|=(mask[i]&1) && curve->y[i]==-200000;high|=(mask[i]&1) && curve->y[i]==200000;segment|=i && mask[i]==3;}
    CHECK(low && high && segment);
    CHECK(ps_report_curve_view(ps_graphic_snapshot_report(snapshot),0,1,&curve)==PS_OK && curve->source_count==9 && curve->x[1]==1.75);
    ps_graphic_axes axes={.full={0,6000,-250000,250000},.visible={100,200,-400,500},.cropped=true};ps_graphic_snapshot *frozen=NULL;
    CHECK(ps_graphic_snapshot_copy(snapshot,&axes,ps_allocator_default(),&frozen)==PS_OK);
    ps_graphic_snapshot_destroy(snapshot);snapshot=frozen;
    CHECK(ps_graphic_snapshot_describe(snapshot,&d)==PS_OK && d.axes.visible[0]==100 && d.reference==1);

    request.inputs[0]=(ps_graphic_run_input){.path=small};request.inputs[1]=(ps_graphic_run_input){.path=b};
    test_allocator tracking={0};ps_graphic_snapshot *candidate=NULL;
    CHECK(ps_graphic_snapshot_from_runs(&request,prefix,0,NULL,NULL,test_domain(&tracking),&candidate,&failure)==PS_OK);
    size_t allocations=tracking.attempts;ps_graphic_snapshot_destroy(candidate);
    CHECK(!tracking.live_blocks && !tracking.invalid && !tracking.live_bytes);
    for(size_t fail=1;fail<=allocations;fail++) {
        test_allocator failing={.fail_on=fail};candidate=snapshot;
        CHECK(ps_graphic_snapshot_from_runs(&request,prefix,0,NULL,NULL,test_domain(&failing),&candidate,&failure)==PS_MEMORY && candidate==snapshot);
        CHECK(!failing.live_blocks && !failing.live_bytes && !failing.invalid);
    }
    for(unsigned i=1;i<=8;i++) {
        stop_state state={.stop=i};candidate=snapshot;
        CHECK(ps_graphic_snapshot_from_runs(&request,prefix,0,stop,&state,ps_allocator_default(),&candidate,&failure)==PS_EOF && failure.cancelled && candidate==snapshot);
    }
    candidate=snapshot;CHECK(ps_graphic_snapshot_from_runs(&request,prefix,1,NULL,NULL,ps_allocator_default(),&candidate,&failure)==PS_LIMIT && candidate==snapshot);
    request.inputs[1].path=bad;
    CHECK(ps_graphic_snapshot_from_runs(&request,prefix,0,NULL,NULL,ps_allocator_default(),&candidate,&failure)==PS_CORRUPT && failure.input==1 && candidate==snapshot);
    {
        ps_graphic_run_request saved=request;request.count=1;request.reference=0;
        request.inputs[0]=(ps_graphic_run_input){.path=bad};request.channel=(ps_channel){.name="other",.unit="1"};
        request.display_unit=(ps_report_unit){.scale=1,.symbol="1"};
        ps_result actual=ps_graphic_snapshot_from_runs(&request,prefix,0,NULL,NULL,ps_allocator_default(),&candidate,&failure);
        bool rejected=actual==PS_CORRUPT && candidate==snapshot && failure.input==0;
        if(actual==PS_OK)ps_graphic_snapshot_destroy(candidate);
        if(!rejected)fprintf(stderr,"Unselected measurement status: result=%d, expected PS_CORRUPT=%d\n",actual,PS_CORRUPT);
        CHECK(rejected);request=saved;candidate=snapshot;
    }
    request.inputs[1].path=missing;
    CHECK(ps_graphic_snapshot_from_runs(&request,prefix,0,NULL,NULL,ps_allocator_default(),&candidate,&failure)==PS_OK);
    CHECK(ps_graphic_snapshot_run(candidate,1,&info)==PS_OK && info.statistics_result==PS_INVALID && !info.si_statistics.count);
    ps_graphic_snapshot_destroy(candidate);candidate=snapshot;
    request.inputs[0].path=missing;
    CHECK(ps_graphic_snapshot_from_runs(&request,prefix,0,NULL,NULL,ps_allocator_default(),&candidate,&failure)==PS_INVALID && candidate==snapshot);
    request.inputs[0].path=numeric;request.inputs[1].path=b;request.display_unit.scale=1;strcpy(request.display_unit.symbol,"m");
    CHECK(ps_graphic_snapshot_from_runs(&request,prefix,0,NULL,NULL,ps_allocator_default(),&candidate,&failure)==PS_OK);
    CHECK(ps_graphic_snapshot_run(candidate,0,&info)==PS_OK && info.statistics_result==PS_NUMERIC);ps_graphic_snapshot_destroy(candidate);candidate=snapshot;
    request.inputs[0].path=small;strcpy(request.channel.name,"absent");
    CHECK(ps_graphic_snapshot_from_runs(&request,prefix,0,NULL,NULL,ps_allocator_default(),&candidate,&failure)==PS_INVALID && failure.input==0 && candidate==snapshot);
    strcpy(request.channel.name,"position");request.channel.dimension[0]=0;request.channel.dimension[1]=1;request.display_unit.dimension[0]=0;request.display_unit.dimension[1]=1;
    CHECK(ps_graphic_snapshot_from_runs(&request,prefix,0,NULL,NULL,ps_allocator_default(),&candidate,&failure)==PS_INVALID && failure.input==0 && candidate==snapshot);
    request.channel.dimension[0]=1;request.channel.dimension[1]=0;request.display_unit.dimension[0]=1;request.display_unit.dimension[1]=0;
    request.count=8;request.reference=7;for(unsigned i=0;i<8;i++)request.inputs[i]=(ps_graphic_run_input){.path=small};
    CHECK(ps_graphic_snapshot_from_runs(&request,prefix,0,NULL,NULL,ps_allocator_default(),&candidate,&failure)==PS_OK);
    CHECK(ps_graphic_snapshot_describe(candidate,&d)==PS_OK && d.input_count==8 && d.plot.curves==8 && d.reference==7);ps_graphic_snapshot_destroy(candidate);candidate=snapshot;
    request.count=9;CHECK(ps_graphic_snapshot_from_runs(&request,prefix,0,NULL,NULL,ps_allocator_default(),&candidate,&failure)==PS_INVALID && candidate==snapshot);
    CHECK(!remove(a) && !remove(b) && !remove(small) && !remove(bad) && !remove(missing) && !remove(numeric));
    CHECK(ps_graphic_snapshot_describe(snapshot,&d)==PS_OK && d.reference==1 && d.axes.visible[1]==200);
    CHECK(ps_graphic_snapshot_run(snapshot,0,&info)==PS_OK && info.samples==6001 && !strcmp(info.path,a));
    CHECK(ps_report_curve_view(ps_graphic_snapshot_report(snapshot),0,0,&curve)==PS_OK && curve->source_count==6001);
    ps_graphic_snapshot_destroy(snapshot);
    printf("GRAPHIC SNAPSHOT RUNS: independent times, units, extrema/gaps, full statistics, 8 inputs, source deletion, quota/cancel and %zu allocation failures passed\n",allocations);
    return 0;
}
int main(int argc,char **argv) {
    CHECK(argc==2);CHECK(report_copy()==0);CHECK(runs(argv[1])==0);CHECK(angles_and_names(argv[1])==0);
    puts("Graphic snapshot: immutable report/run data and transactional ownership passed");return 0;
}
