#ifndef PHYSIM_LANGUAGE_FLUID_H
#define PHYSIM_LANGUAGE_FLUID_H
#include "language_runtime.h"
#include "language_array.h"
#include "fluid.h"
#define PSRT_FLUID_THREE(name,call) \
    static inline double name(double a,double b,double c,psrt_site site) { \
        double value=0;ps_result r=call(a,b,c,&value); \
        if(r!=PS_OK)psrt_raise(site,r,"Fluid model: invalid SI inputs or numeric range",NULL);return value; }
PSRT_FLUID_THREE(psrt_pipe_conductance,ps_pipe_conductance)
PSRT_FLUID_THREE(psrt_pipe_flow,ps_pipe_flow)
PSRT_FLUID_THREE(psrt_pipe_power,ps_pipe_power)
#undef PSRT_FLUID_THREE
static inline double psrt_reynolds(double density,double velocity,double diameter,double viscosity,psrt_site site) {
    double value=0;ps_result r=ps_reynolds_number(density,velocity,diameter,viscosity,&value);
    if(r!=PS_OK)psrt_raise(site,r,"Reynolds number: invalid SI inputs or numeric range",NULL);return value;
}
static inline double psrt_hydrostatic(double reference,double density,double gravity,double depth,psrt_site site) {
    double value=0;ps_result r=ps_hydrostatic_pressure(reference,density,gravity,depth,&value);
    if(r!=PS_OK)psrt_raise(site,r,"Hydrostatic pressure: invalid SI inputs or numeric range",NULL);return value;
}
static inline psrt_array psrt_pipe_network(ps_allocator allocator,const int64_t *a,size_t ac,
    const int64_t *b,size_t bc,const double *conductance,size_t ec,const int64_t *fixed,size_t nc,
    const double *boundary,size_t pc,psrt_site site) {
    if(ac!=bc || ac!=ec || nc!=pc || !nc || nc>PS_PIPE_NETWORK_MAX_NODES || ec>PS_PIPE_NETWORK_MAX_EDGES)
        psrt_fail(site,"Pipe network array sizes or limits are invalid");
    ps_pipe_edge edges[32];uint8_t flags[16];double pressures[16],flows[32];
    for(size_t i=0;i<nc;i++){if(fixed[i]!=0 && fixed[i]!=1)psrt_fail(site,"Pipe network fixed flags must be 0 or 1");flags[i]=(uint8_t)fixed[i];}
    for(size_t e=0;e<ec;e++) {
        if(a[e]<0 || b[e]<0 || (uint64_t)a[e]>=nc || (uint64_t)b[e]>=nc)psrt_fail(site,"Pipe network endpoint out of bounds");
        edges[e]=(ps_pipe_edge){(uint32_t)a[e],(uint32_t)b[e],conductance[e]};
    }
    ps_result r=ps_pipe_network_solve(flags,boundary,nc,edges,ec,pressures,flows);
    if(r!=PS_OK)psrt_raise(site,r,"Pipe network: unanchored/ill-conditioned graph or invalid numeric data",NULL);
    static const psrt_element_type element={sizeof(double),NULL,NULL};psrt_array result;
    if(psrt_array_init(&element,allocator,48,&result)!=PS_OK || psrt_array_build_begin(&result,nc+ec)!=PS_OK)
        psrt_fail(site,"Pipe network result allocation exhausted");
    double *data=(double *)(result.block+1);memcpy(data,pressures,nc*sizeof(double));memcpy(data+nc,flows,ec*sizeof(double));
    result.block->value.count=nc+ec;return result;
}
static inline psrt_array psrt_transport_step(ps_allocator allocator,const double *input,size_t count,
    double velocity,double diffusion,double dx,double dt,psrt_site site) {
    if(count>PS_TRANSPORT_MAX_CELLS)psrt_raise(site,PS_LIMIT,"Transport supports at most 4096 cells",NULL);
    static const psrt_element_type element={sizeof(double),NULL,NULL};psrt_array result;
    if(psrt_array_init(&element,allocator,PS_TRANSPORT_MAX_CELLS,&result)!=PS_OK || psrt_array_build_begin(&result,count)!=PS_OK)
        psrt_fail(site,"Transport result allocation exhausted");
    double *data=result.block?(double *)(result.block+1):NULL;
    ps_result r=ps_transport_periodic_step(input,count,velocity,diffusion,dx,dt,data);
    if(r!=PS_OK){psrt_array_destroy(&result);psrt_raise(site,r,"Transport: invalid concentration, coefficients or stability bound",NULL);}
    result.block->value.count=count;return result;
}
#endif
