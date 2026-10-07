#include "physim/fluid.h"
#include "physim/numerics.h"
#include <math.h>
#include <string.h>
static bool positive(double x){return isfinite(x) && x>0;}
static bool nonnegative(double x){return isfinite(x) && x>=0;}
static double ratio(const double *n,size_t nc,const double *d,size_t dc) {
    double m=1;int exponent=0;
    for(size_t i=0;i<nc;i++){if(n[i]==0)return 0;int e;m*=frexp(n[i],&e);exponent+=e;}
    for(size_t i=0;i<dc;i++){int e;m/=frexp(d[i],&e);exponent-=e;}
    return scalbn(m,exponent);
}
static ps_result scalar(double value,bool require_positive,double *out) {
    if(!isfinite(value) || (require_positive && value<=0))return PS_NUMERIC;
    *out=value;return PS_OK;
}
ps_result ps_pipe_conductance(double radius,double length,double viscosity,double *out) {
    if(!out || !positive(radius) || !positive(length) || !positive(viscosity))return PS_INVALID;
    double n[]={PS_PI,radius,radius,radius,radius},d[]={8,viscosity,length};
    return scalar(ratio(n,5,d,3),true,out);
}
static void difference(double a,double b,double *delta,double *factor) {
    *delta=a-b;*factor=1;
    if(!isfinite(*delta)){*delta=a*.5-b*.5;*factor=2;}
}
ps_result ps_pipe_flow(double g,double a,double b,double *out) {
    if(!out || !nonnegative(g) || !isfinite(a) || !isfinite(b))return PS_INVALID;
    double delta,factor;difference(a,b,&delta,&factor);
    double n[]={g,delta,factor};return scalar(ratio(n,3,NULL,0),false,out);
}
ps_result ps_pipe_power(double g,double a,double b,double *out) {
    if(!out || !nonnegative(g) || !isfinite(a) || !isfinite(b))return PS_INVALID;
    double delta,factor;difference(a,b,&delta,&factor);
    double n[]={g,delta,delta,factor,factor};return scalar(ratio(n,5,NULL,0),false,out);
}
ps_result ps_reynolds_number(double density,double velocity,double diameter,double viscosity,double *out) {
    if(!out || !positive(density) || !isfinite(velocity) || !positive(diameter) || !positive(viscosity))return PS_INVALID;
    double n[]={density,fabs(velocity),diameter},d[]={viscosity};return scalar(ratio(n,3,d,1),false,out);
}
ps_result ps_hydrostatic_pressure(double p0,double density,double gravity,double depth,double *out) {
    if(!out || !isfinite(p0) || !positive(density) || !nonnegative(gravity) || !isfinite(depth))return PS_INVALID;
    /* Normalize the pressure term and reference together before final scaling. */
    int e1,e2,e3,e0;double m1=frexp(density,&e1),m2=frexp(gravity,&e2),m3=frexp(depth,&e3),m0=frexp(p0,&e0);
    int exponent=e1+e2+e3;if(gravity==0 || depth==0)exponent=e0;
    if(p0!=0 && e0>exponent)exponent=e0;
    double sum=scalbn(m1*m2*m3,e1+e2+e3-exponent)+scalbn(m0,e0-exponent);
    return scalar(scalbn(sum,exponent),false,out);
}
static unsigned root(unsigned *parents,unsigned node){while(parents[node]!=node)node=parents[node];return node;}
static bool overlap(const void *a,size_t as,const void *b,size_t bs) {
    if(!as || !bs)return false;
    uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
    if(x>UINTPTR_MAX-as || y>UINTPTR_MAX-bs)return true;
    return x<y+bs && y<x+as;
}
ps_result ps_pipe_network_solve(const uint8_t *fixed,const double *boundary,size_t nodes,
                                const ps_pipe_edge *edges,size_t count,double *pressures,double *flows) {
    if(nodes>PS_PIPE_NETWORK_MAX_NODES || count>PS_PIPE_NETWORK_MAX_EDGES)return PS_LIMIT;
    if(!nodes || !fixed || !boundary || !pressures || (count && (!edges || !flows)) ||
        overlap(pressures,nodes*sizeof(double),flows,count*sizeof(double)))return PS_INVALID;
    unsigned parents[16],indices[16];bool anchored[16]={0};size_t unknown=0;
    double maximum_g=0,pressure_scale[16]={0},output_p[16],output_q[32];
    for(unsigned i=0;i<nodes;i++) {
        if(fixed[i]>1 || (fixed[i] && !isfinite(boundary[i])))return PS_INVALID;
        parents[i]=i;indices[i]=fixed[i]?UINT32_MAX:(unsigned)unknown++;
    }
    for(size_t e=0;e<count;e++) {
        ps_pipe_edge edge=edges[e];if(edge.a>=nodes || edge.b>=nodes || edge.a==edge.b || !nonnegative(edge.conductance_m3_s_pa))return PS_INVALID;
        maximum_g=fmax(maximum_g,edge.conductance_m3_s_pa);
        if(edge.conductance_m3_s_pa>0)parents[root(parents,edge.a)]=root(parents,edge.b);
    }
    for(unsigned i=0;i<nodes;i++)if(fixed[i]) {
        unsigned component=root(parents,i);anchored[component]=true;
        pressure_scale[component]=fmax(pressure_scale[component],fabs(boundary[i]));
    }
    for(unsigned i=0;i<nodes;i++)if(!anchored[root(parents,i)])return PS_SINGULAR;
    for(unsigned i=0;i<nodes;i++)if(pressure_scale[i]==0)pressure_scale[i]=1;
    for(unsigned i=0;i<nodes;i++)if(fixed[i] && boundary[i]!=0 &&
        boundary[i]/pressure_scale[root(parents,i)]==0)return PS_NUMERIC;
    double matrix[16*16]={0},rhs[16]={0},solution[16]={0};
    for(size_t e=0;e<count;e++) {
        ps_pipe_edge edge=edges[e];if(edge.conductance_m3_s_pa==0)continue;
        double g=edge.conductance_m3_s_pa/maximum_g;if(g==0)return PS_NUMERIC;
        unsigned ends[]={edge.a,edge.b};
        for(unsigned side=0;side<2;side++) {
            unsigned a=ends[side],b=ends[1-side];if(fixed[a])continue;
            unsigned row=indices[a];matrix[row*unknown+row]+=g;
            if(fixed[b])rhs[row]+=g*(boundary[b]/pressure_scale[root(parents,a)]);
            else matrix[row*unknown+indices[b]]-=g;
        }
    }
    if(unknown) {
        ps_result r=ps_linear_solve(matrix,rhs,unknown,0,solution);if(r!=PS_OK)return r;
    }
    for(unsigned i=0;i<nodes;i++) {
        output_p[i]=fixed[i]?boundary[i]:solution[indices[i]]*pressure_scale[root(parents,i)];
        if(!isfinite(output_p[i]))return PS_NUMERIC;
    }
    for(size_t e=0;e<count;e++) {
        ps_result r=ps_pipe_flow(edges[e].conductance_m3_s_pa,output_p[edges[e].a],output_p[edges[e].b],&output_q[e]);
        if(r!=PS_OK)return r;
    }
    memcpy(pressures,output_p,nodes*sizeof(double));if(count)memcpy(flows,output_q,count*sizeof(double));return PS_OK;
}
ps_result ps_transport_periodic_step(const double *input,size_t count,double velocity,double diffusion,double dx,double dt,double *out) {
    if(count>PS_TRANSPORT_MAX_CELLS)return PS_LIMIT;
    if(!input || !out || count<3 || !isfinite(velocity) || !nonnegative(diffusion) || !positive(dx) || !nonnegative(dt))return PS_INVALID;
    double na[]={fabs(velocity),dt},nd[]={diffusion,dt},da[]={dx},dd[]={dx,dx};
    double a=ratio(na,2,da,1),d=ratio(nd,2,dd,2);
    if(!isfinite(a) || !isfinite(d) || a+2*d>1)return PS_INVALID;
    for(size_t i=0;i<count;i++)if(!nonnegative(input[i]))return PS_INVALID;
    if(dt==0){memmove(out,input,count*sizeof(double));return PS_OK;}
    double result[PS_TRANSPORT_MAX_CELLS],center_weight=fmax(0,1-a-2*d);
    for(size_t i=0;i<count;i++) {
        size_t left=i?i-1:count-1,right=i+1==count?0:i+1,upwind=velocity>=0?left:right;
        if(input[left]==input[i] && input[right]==input[i]){result[i]=input[i];continue;}
        double adv[]={input[upwind],fabs(velocity),dt},dl[]={input[left],diffusion,dt},dr[]={input[right],diffusion,dt};
        double value=input[i]*center_weight+ratio(adv,3,da,1)+ratio(dl,3,dd,2)+ratio(dr,3,dd,2);
        /* Exact update is convex. Roundoff at DBL_MAX must not violate its bound. */
        double maximum=fmax(input[i],fmax(input[left],input[right]));
        if(value>maximum)value=maximum;
        if(!nonnegative(value))return PS_NUMERIC;
        result[i]=value;
    }
    memcpy(out,result,count*sizeof(double));return PS_OK;
}
