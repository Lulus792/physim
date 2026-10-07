#include "physim/units.h"
#include "physim/experiment.h"
#include "physim/thermodynamics.h"
#include <math.h>
#include <stdio.h>
/* Synthetic molar SI coefficients; prescribed isothermal compression. */
static ps_result measure(ps_context *c,double time) {
    double v=.005-.004*fmin(1,time),values[7]={v};ps_result r;
    if((r=ps_ideal_gas_pressure(1,450,v,&values[1]))!=PS_OK ||
       (r=ps_vdw_gas_pressure(1,450,v,.4,4e-5,&values[2]))!=PS_OK ||
       (r=ps_ideal_gas_energy(1,20.8,450,&values[3]))!=PS_OK ||
       (r=ps_vdw_gas_energy(1,20.8,450,v,.4,&values[4]))!=PS_OK ||
       (r=ps_vdw_gas_entropy_change(1,20.8,450,.005,450,v,4e-5,&values[5]))!=PS_OK ||
       (r=ps_vdw_gas_pressure_derivative(1,450,v,.4,4e-5,&values[6]))!=PS_OK)return r;
    for(unsigned i=0;i<7;i++)c->values[i]=values[i];return PS_OK;
}
static ps_result reset(ps_context *c){return measure(c,0);}
static ps_result create(ps_context *c) {
    const char *names[]={"volume","ideal_pressure","vdw_pressure","ideal_energy","vdw_energy","entropy_change","pressure_derivative"};
    ps_unit volume={{3,0,0,0,0,0,0},1,"m3"},entropy={{2,1,-2,0,-1,0,0},1,"J/K"},derivative={{-4,1,-2,0,0,0,0},1,"Pa/m3"};
    ps_unit units[]={volume,PS_PASCAL,PS_PASCAL,PS_JOULE,PS_JOULE,entropy,derivative};
    for(unsigned i=0;i<7;i++)if(ps_channel_add(c,names[i],units[i],names[i])!=(int)i)return PS_LIMIT;
    snprintf(c->model_metadata,sizeof c->model_metadata,"model=homogeneous van der Waals vs ideal gas\nsource=synthetic coefficients; not a calibrated gas\namount=1 mol\ntemperature=450 K\nattraction=0.4 Pa m6/mol2\ncovolume=0.00004 m3/mol\nmolar_cv=20.8 J/(mol K)\nvolume=0.005 to 0.001 m3 in 1 s; then fixed\nenergy_reference=U=n cv T-a n2/V\nexcluded=phase coexistence,Maxwell construction,latent heat,temperature-dependent coefficients\n");return reset(c);
}
static ps_result step(ps_context *c,double dt){return measure(c,c->time_s+dt);}
static void scene(ps_context *c,ps_scene *out){ps_scene_add_id(out,1,PS_SPHERE,ps_v3(c->time_s,0,0),ps_v3(0,0,0),.1,0x53aeefff);}
static void destroy(ps_context *c){(void)c;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,.name="Real gas comparison",
        .create=create,.reset=reset,.step=step,.build_scene=scene,.destroy=destroy};return &api;
}
