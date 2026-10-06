/* Preview reduction retains extrema and records segment boundaries explicitly.
 * A skipped missing row must never become a line between retained valid rows. */
typedef struct {double x,y;uint64_t index,segment;} masked_point;
static ps_result masked_point_add(ps_curve_data *c,uint8_t *flags,uint64_t *previous,
                                  masked_point p,bool valid) {
    if(c->count==PS_REPORT_MAX_POINTS)return PS_LIMIT;
    flags[c->count]=valid?(p.segment==*previous?1u:3u):0u;
    if(valid)*previous=p.segment;
    point(c,valid?p.x:0,valid?p.y:0);return PS_OK;
}
static ps_result masked_extrema(ps_curve_data *c,uint8_t *flags,uint64_t *previous,
                                masked_point low,masked_point high) {
    if(low.index>high.index){masked_point t=low;low=high;high=t;}
    ps_result r=masked_point_add(c,flags,previous,low,true);
    if(r==PS_OK && low.index!=high.index)r=masked_point_add(c,flags,previous,high,true);
    return r;
}
static ps_result report_masked_series(ps_report *r,ps_plot_handle h,ps_analysis_context *ctx,
                                      ps_series xs,ps_series ys,const char *label,ps_plot_kind kind,
                                      const ps_series_info *xi,const ps_series_info *yi) {
    ps_plot_info *plot=&r->plot[h.index].info;
    if(memcmp(xi->dimension,plot->x_unit.dimension,7) || memcmp(yi->dimension,plot->y_unit.dimension,7))return PS_INVALID;
    ps_curve_data *curve=report_memory(r->allocator,sizeof *curve);if(!curve)return PS_MEMORY;
    strcpy(curve->label,label);curve->kind=kind;curve->source_count=xi->count;
    uint8_t flags[PS_REPORT_MAX_POINTS];uint64_t segment=0,previous_segment=0,bucket=0;
    uint64_t bucket_size=xi->count/1023+(xi->count%1023!=0);
    bool gap=false,started=false;masked_point low={0},high={0};
    ps_unit from_x={{0},xi->scale,NULL},from_y={{0},yi->scale,NULL};
    ps_unit to_x={{0},plot->x_unit.scale,NULL},to_y={{0},plot->y_unit.scale,NULL};
    memcpy(from_x.dimension,xi->dimension,7);memcpy(from_y.dimension,yi->dimension,7);
    memcpy(to_x.dimension,xi->dimension,7);memcpy(to_y.dimension,yi->dimension,7);
    ps_result result=PS_OK;
    for(uint64_t at=0;at<xi->count && result==PS_OK;) {
        double x[PS_SERIES_BLOCK_SIZE],y[PS_SERIES_BLOCK_SIZE];uint8_t vx[PS_SERIES_BLOCK_SIZE],vy[PS_SERIES_BLOCK_SIZE];
        size_t nx=0,ny=0;result=ps_series_read_masked(ctx,xs,at,x,vx,PS_SERIES_BLOCK_SIZE,&nx);
        if(result==PS_OK)result=ps_series_read_masked(ctx,ys,at,y,vy,nx,&ny);
        if(result!=PS_OK)break;
        if(!nx || nx!=ny){result=PS_CORRUPT;break;}
        for(size_t j=0;j<nx && result==PS_OK;j++) {
            uint64_t i=at+j;bool valid=vx[j] && vy[j];
            if(!valid)gap=true;
            else {
                if(gap){segment++;gap=false;}
                result=ps_convert(x[j],from_x,to_x,&x[j]);
                if(result==PS_OK)result=ps_convert(y[j],from_y,to_y,&y[j]);
                if(result!=PS_OK)break;
            }
            masked_point p={x[j],y[j],i,segment};
            if(xi->count<=PS_REPORT_MAX_POINTS)result=masked_point_add(curve,flags,&previous_segment,p,valid);
            else if(kind==PS_PLOT_SCATTER) {
                uint64_t target=(xi->count-1)/(PS_REPORT_MAX_POINTS-1)*curve->count+
                    ((xi->count-1)%(PS_REPORT_MAX_POINTS-1)*curve->count)/(PS_REPORT_MAX_POINTS-1);
                if(i==target)result=masked_point_add(curve,flags,&previous_segment,p,valid);
            } else if(valid) {
                if(!i || i==xi->count-1) {
                    if(started){result=masked_extrema(curve,flags,&previous_segment,low,high);started=false;}
                    if(result==PS_OK)result=masked_point_add(curve,flags,&previous_segment,p,true);
                } else {
                    uint64_t next=i/bucket_size;
                    if(started && next!=bucket){result=masked_extrema(curve,flags,&previous_segment,low,high);started=false;}
                    if(!started){bucket=next;low=high=p;started=true;}
                    if(p.y<low.y)low=p;
                    if(p.y>high.y)high=p;
                }
            }
        }
        at+=nx;
    }
    if(result==PS_OK && started)result=masked_extrema(curve,flags,&previous_segment,low,high);
    if(result==PS_OK && !curve->count){curve->count=1;flags[0]=0;}
    if(result==PS_OK)result=ps_report_add_curve_masked(r,h,curve,flags);
    ps_memory_free(r->allocator,curve,sizeof *curve);return result;
}
