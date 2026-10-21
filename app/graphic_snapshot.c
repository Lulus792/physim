#include "graphic_snapshot.h"
#include "text_validation.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

struct ps_graphic_snapshot {
    ps_allocator allocator;
    ps_report *report;
    ps_graphic_axes axes;
    uint32_t reference,input_count;
    ps_graphic_run_info inputs[PS_GRAPHIC_MAX_INPUTS];
};
static bool bounds_valid(const double b[4]) {
    return isfinite(b[0]) && isfinite(b[1]) && isfinite(b[2]) && isfinite(b[3]) && b[0]<b[1] && b[2]<b[3];
}
static bool axes_copy(ps_graphic_axes *out,const double full[4],const ps_graphic_axes *in) {
    ps_graphic_axes a={0};memcpy(a.full,in?in->full:full,sizeof a.full);
    if(!bounds_valid(a.full))return false;
    if(in && in->cropped) {
        if(!bounds_valid(in->visible))return false;
        memcpy(a.visible,in->visible,sizeof a.visible);
        for(unsigned i=0;i<4;i++)a.cropped|=a.full[i]!=a.visible[i];
    } else memcpy(a.visible,a.full,sizeof a.visible);
    *out=a;return true;
}
static ps_result create(const char *title,const char *provenance,ps_allocator allocator,ps_graphic_snapshot **out) {
    void *memory=NULL;ps_result result=ps_memory_zero(allocator,1,sizeof(ps_graphic_snapshot),&memory);
    if(result!=PS_OK)return result;
    ps_graphic_snapshot *s=memory;s->allocator=allocator;s->reference=PS_GRAPHIC_NO_REFERENCE;
    result=ps_report_create_with_allocator(title,provenance,allocator,&s->report);
    if(result!=PS_OK){ps_graphic_snapshot_destroy(s);return result;}
    *out=s;return PS_OK;
}
void ps_graphic_snapshot_destroy(ps_graphic_snapshot *s) {
    if(s){ps_report_destroy(s->report);ps_memory_free(s->allocator,s,sizeof *s);}
}
const ps_report *ps_graphic_snapshot_report(const ps_graphic_snapshot *s) {return s?s->report:NULL;}
ps_result ps_graphic_snapshot_describe(const ps_graphic_snapshot *s,ps_graphic_description *out) {
    if(!s || !out)return PS_INVALID;
    ps_graphic_description d={0};ps_result result=ps_report_plot_read(s->report,0,&d.plot);
    if(result!=PS_OK)return result;
    d.axes=s->axes;d.reference=s->reference;d.input_count=s->input_count;
    for(uint32_t i=0;i<d.plot.curves;i++) {
        const ps_curve_data *c=NULL;result=ps_report_curve_view(s->report,0,i,&c);if(result!=PS_OK)return result;
        d.reduced|=c->kind!=PS_PLOT_HISTOGRAM && c->count<c->source_count;
    }
    *out=d;return PS_OK;
}
ps_result ps_graphic_snapshot_run(const ps_graphic_snapshot *s,uint32_t index,ps_graphic_run_info *out) {
    if(!s || !out || index>=s->input_count)return PS_INVALID;
    *out=s->inputs[index];return PS_OK;
}
ps_result ps_graphic_snapshot_from_report(const ps_report *report,uint32_t plot,const ps_graphic_axes *axes,
    uint32_t reference,ps_allocator allocator,ps_graphic_snapshot **out) {
    ps_plot_info info;double bounds[4];char title[192],provenance[8192];ps_graphic_axes captured;
    if(!out || !ps_allocator_valid(allocator) || ps_report_plot_read(report,plot,&info)!=PS_OK ||
       ps_report_plot_bounds(report,plot,bounds)!=PS_OK || !axes_copy(&captured,bounds,axes) ||
       (reference!=PS_GRAPHIC_NO_REFERENCE && reference>=info.curves))return PS_INVALID;
    ps_result result=ps_report_describe(report,title,provenance,NULL,NULL);if(result!=PS_OK)return result;
    ps_graphic_snapshot *s=NULL;result=create(title,provenance,allocator,&s);if(result!=PS_OK)return result;
    uint32_t curves=info.curves;info.curves=0;ps_plot_handle target;
    result=ps_report_add_plot(s->report,&info,&target);
    for(uint32_t i=0;i<curves && result==PS_OK;i++) {
        const ps_curve_data *c=NULL;const uint8_t *mask=NULL;
        result=ps_report_curve_view(report,plot,i,&c);
        if(result==PS_OK)result=ps_report_curve_mask(report,plot,i,&mask);
        if(result==PS_OK)result=ps_report_add_curve_masked(s->report,target,c,mask);
    }
    if(result!=PS_OK){ps_graphic_snapshot_destroy(s);return result;}
    s->reference=reference;s->axes=captured;*out=s;return PS_OK;
}
ps_result ps_graphic_snapshot_copy(const ps_graphic_snapshot *source,const ps_graphic_axes *axes,
    ps_allocator allocator,ps_graphic_snapshot **out) {
    if(!source || !out)return PS_INVALID;
    ps_graphic_snapshot *s=NULL;ps_result result=ps_graphic_snapshot_from_report(source->report,0,
        axes?axes:&source->axes,source->reference,allocator,&s);
    if(result!=PS_OK)return result;
    s->input_count=source->input_count;memcpy(s->inputs,source->inputs,sizeof s->inputs);*out=s;return PS_OK;
}
static bool channel_matches(const ps_channel *expected,const ps_channel *actual) {
    if(strcmp(expected->name,actual->name) || memcmp(expected->dimension,actual->dimension,7))return false;
    const int8_t zero[7]={0};
    /* A dimensionless angle must not silently become an ordinary scalar. */
    if(!memcmp(expected->dimension,zero,7)) {
        bool left=!strcmp(expected->unit,"rad") || !strcmp(expected->unit,"°") || !strcmp(expected->unit,"deg");
        bool right=!strcmp(actual->unit,"rad") || !strcmp(actual->unit,"°") || !strcmp(actual->unit,"deg");
        if(left!=right)return false;
    }
    return true;
}
static void run_label(char out[96],const ps_graphic_run_input *input) {
    const char *text=input->label;
    if(!text) {
        text=input->path;
        for(const char *p=input->path;*p;p++)if(*p=='/' || *p=='\\')text=p+1;
    }
    size_t n=strlen(text);if(n<=95){memcpy(out,text,n+1);return;}
    n=92;while(n && ((unsigned char)text[n]&0xc0)==0x80)n--;
    memcpy(out,text,n);memcpy(out+n,"…",4);
}
static ps_result status_domain(ps_analysis_context *context,ps_series status,ps_graphic_cancel cancel,void *user,bool *cancelled) {
    ps_series_info info;ps_result result=ps_series_describe(context,status,&info);if(result!=PS_OK)return result;
    for(uint64_t at=0;at<info.count;) {
        if(cancel && cancel(user)){*cancelled=true;return PS_EOF;}
        double values[PS_SERIES_BLOCK_SIZE];size_t count=0;
        result=ps_series_read(context,status,at,values,PS_SERIES_BLOCK_SIZE,&count);
        if(result!=PS_OK)return result;if(!count)return PS_CORRUPT;
        for(size_t i=0;i<count;i++)if(values[i]!=0 && values[i]!=1 && values[i]!=2)return PS_CORRUPT;
        at+=count;
    }
    return PS_OK;
}
static ps_result dataset_status(ps_analysis_context *context,ps_dataset dataset,const ps_dataset_info *info,
    uint32_t selected,int *selected_status,ps_graphic_cancel cancel,void *user,bool *cancelled) {
    bool checked[PS_MAX_CHANNELS]={false};
    for(uint32_t channel=0;channel<info->channel_count;channel++) {
        int index=-1;ps_result result=ps_channel_status_index(info->channels,info->channel_count,channel,&index);
        if(result!=PS_OK)return result;
        if(channel==selected)*selected_status=index;
        if(index<0 || checked[index])continue;
        checked[index]=true;ps_series status;
        result=ps_dataset_channel_series(context,dataset,(uint32_t)index,&status);
        if(result==PS_OK)result=status_domain(context,status,cancel,user,cancelled);
        if(result!=PS_OK)return result;
    }
    return PS_OK;
}
ps_result ps_graphic_snapshot_from_runs(const ps_graphic_run_request *request,const char *prefix,uint64_t quota,
    ps_graphic_cancel cancel,void *user,ps_allocator allocator,ps_graphic_snapshot **out,ps_graphic_failure *failure) {
    ps_result result=PS_INVALID;uint32_t failed=UINT32_MAX;bool cancelled=false;
    ps_graphic_snapshot *s=NULL;ps_analysis_context *context=NULL;
    if(!request || !out || !prefix || !ps_allocator_valid(allocator) || !request->count || request->count>PS_GRAPHIC_MAX_INPUTS ||
       (request->reference!=PS_GRAPHIC_NO_REFERENCE && request->reference>=request->count) ||
       !request->channel.name[0] || !ps_text_valid(request->channel.name,sizeof request->channel.name,false) ||
       !ps_text_valid(request->channel.unit,sizeof request->channel.unit,false) ||
       memcmp(request->channel.dimension,request->display_unit.dimension,7))goto done;
    for(uint32_t i=0;i<request->count;i++) {
        const ps_graphic_run_input *input=request->inputs+i;
        if(!input->path || !*input->path || !ps_text_valid(input->path,PS_GRAPHIC_PATH_BYTES,false) ||
           (input->label && (!*input->label || !ps_text_valid(input->label,96,false))) ||
           (input->kind && input->kind!=PS_PLOT_LINE && input->kind!=PS_PLOT_SCATTER)){failed=i;goto done;}
    }
    if(cancel && cancel(user)){cancelled=true;result=PS_EOF;goto done;}
    const char *channel_name=request->channel.name;
    result=create(channel_name,"Festgehaltene Messdatenvorschau",allocator,&s);if(result!=PS_OK)goto done;
    ps_plot_info info={0};snprintf(info.title,sizeof info.title,"%s",channel_name);
    snprintf(info.x_label,sizeof info.x_label,"Zeit");snprintf(info.y_label,sizeof info.y_label,"%s",channel_name);
    ps_report_unit_from(PS_SECOND,&info.x_unit);info.y_unit=request->display_unit;
    ps_plot_handle plot;result=ps_report_add_plot(s->report,&info,&plot);if(result!=PS_OK)goto done;
    result=ps_analysis_create_with_allocator(prefix,quota,allocator,&context);if(result!=PS_OK)goto done;
    double first=INFINITY,last=-INFINITY;
    for(uint32_t i=0;i<request->count;i++) {
        failed=i;if(cancel && cancel(user)){cancelled=true;result=PS_EOF;goto done;}
        ps_dataset dataset;result=ps_analysis_open_run(context,request->inputs[i].path,&dataset);
        bool recovered=result==PS_RECOVERED;if(result!=PS_OK && !recovered)goto done;
        ps_dataset_info dataset_info;result=ps_dataset_describe(context,dataset,&dataset_info);if(result!=PS_OK)goto done;
        uint32_t channel=0;while(channel<dataset_info.channel_count && !channel_matches(&request->channel,dataset_info.channels+channel))channel++;
        if(channel==dataset_info.channel_count || !dataset_info.samples){result=PS_INVALID;goto done;}
        ps_graphic_run_info *run=s->inputs+i;snprintf(run->path,sizeof run->path,"%s",request->inputs[i].path);
        run_label(run->label,request->inputs+i);run->channel=dataset_info.channels[channel];run->recovered=recovered;run->samples=dataset_info.samples;
        ps_series time,values;result=ps_dataset_series(context,dataset,"time",&time);if(result!=PS_OK)goto done;
        result=ps_dataset_channel_series(context,dataset,channel,&values);if(result!=PS_OK)goto done;
        double edge;size_t read=0;result=ps_series_read(context,time,0,&edge,1,&read);if(result!=PS_OK || read!=1){if(result==PS_OK)result=PS_CORRUPT;goto done;}first=fmin(first,edge);
        result=ps_series_read(context,time,dataset_info.samples-1,&edge,1,&read);if(result!=PS_OK || read!=1){if(result==PS_OK)result=PS_CORRUPT;goto done;}last=fmax(last,edge);
        int mask=-1;result=dataset_status(context,dataset,&dataset_info,channel,&mask,cancel,user,&cancelled);if(result!=PS_OK)goto done;
        if(mask>=0) {
            ps_series status;result=ps_dataset_channel_series(context,dataset,(uint32_t)mask,&status);if(result!=PS_OK)goto done;
            ps_series masked;result=ps_series_mask(context,values,status,1,&masked);if(result!=PS_OK)goto done;values=masked;
        }
        run->statistics_result=ps_series_statistics(context,values,&run->si_statistics);
        if(run->statistics_result!=PS_OK && run->statistics_result!=PS_INVALID && run->statistics_result!=PS_NUMERIC){result=run->statistics_result;goto done;}
        if(cancel && cancel(user)){cancelled=true;result=PS_EOF;goto done;}
        ps_plot_kind kind=request->inputs[i].kind?request->inputs[i].kind:PS_PLOT_LINE;
        result=ps_report_add_series(s->report,plot,context,time,values,run->label,kind);if(result!=PS_OK)goto done;
        result=ps_dataset_close(context,dataset);if(result!=PS_OK)goto done;
    }
    failed=UINT32_MAX;if(cancel && cancel(user)){cancelled=true;result=PS_EOF;goto done;}
    result=ps_report_plot_bounds(s->report,0,s->axes.full);if(result!=PS_OK)goto done;
    if(first<last){s->axes.full[0]=first;s->axes.full[1]=last;}
    memcpy(s->axes.visible,s->axes.full,sizeof s->axes.visible);s->reference=request->reference;s->input_count=request->count;
    *out=s;s=NULL;result=PS_OK;
done:
    ps_analysis_destroy(context);ps_graphic_snapshot_destroy(s);
    if(failure)*failure=(ps_graphic_failure){result,failed,cancelled};
    return result;
}
