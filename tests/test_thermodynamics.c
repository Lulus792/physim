#include "physim/thermodynamics.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"Thermodynamics line %d: %s\n",__LINE__,#x);return 1; } } while(0)
static bool near(double actual,double expected) {
    return isfinite(actual) && fabs(actual-expected)<=2e-13*fmax(fabs(expected),1e-300);
}
int main(void) {
    double p=0,v=0,t=0,u=0,q=0,c=0,s=0;
    CHECK(ps_ideal_gas_pressure(1,300,.025,&p)==PS_OK && near(p,99773.55141783888));
    CHECK(ps_ideal_gas_volume(1,300,p,&v)==PS_OK && near(v,.025));
    CHECK(ps_ideal_gas_temperature(1,p,v,&t)==PS_OK && near(t,300));
    CHECK(ps_ideal_gas_energy(2,12.5,400,&u)==PS_OK && u==10000);
    CHECK(ps_heat_capacity(2,500,&c)==PS_OK && c==1000);
    CHECK(ps_sensible_heat(c,300,350,&q)==PS_OK && q==50000);
    CHECK(ps_sensible_heat(c,350,300,&q)==PS_OK && q==-50000);
    CHECK(ps_heat_flow(20,350,300,&q)==PS_OK && q==1000);
    CHECK(ps_heat_flow(0,350,300,&q)==PS_OK && q==0);
    CHECK(ps_ideal_gas_entropy_change(2,12.5,300,.025,600,.05,&s)==PS_OK);
    CHECK(near(s,28.854972157286591649876854752670296953886561451174799944844700067));
    CHECK(ps_thermal_reservoir_step(100,400,300,5,20,&t)==PS_OK && near(t,336.78794411714423));
    CHECK(ps_thermal_reservoir_step(100,300,400,5,20,&t)==PS_OK && near(t,363.21205588285577));
    ps_vec2 pair;
    CHECK(ps_thermal_pair_step(100,400,300,300,5,15,&pair)==PS_OK);
    CHECK(near(pair.x,352.59095808785815) && near(pair.y,315.80301397071394));
    CHECK(near(100*pair.x+300*pair.y,130000));
    ps_vec2 first=pair;
    CHECK(ps_thermal_pair_step(100,first.x,300,first.y,5,15,&pair)==PS_OK);
    ps_vec2 whole;
    CHECK(ps_thermal_pair_step(100,400,300,300,5,30,&whole)==PS_OK);
    CHECK(near(pair.x,whole.x) && near(pair.y,whole.y));
    CHECK(ps_thermal_pair_step(100,300,300,400,DBL_MAX,DBL_MAX,&pair)==PS_OK);
    CHECK(pair.x==375 && pair.y==375);
    CHECK(ps_thermal_pair_step(100,400,300,300,0,DBL_MAX,&pair)==PS_OK && pair.x==400 && pair.y==300);
    CHECK(ps_thermal_pair_step(DBL_MAX,400,DBL_MAX,300,1,0,&pair)==PS_OK && pair.x==400 && pair.y==300);
    CHECK(ps_thermal_reservoir_step(1,1e300,1,1,700,&t)==PS_OK && near(t,1.0000985967654376));
    CHECK(ps_thermal_reservoir_step(1,1e300,1,1,1000,&t)==PS_OK && t==1);
    CHECK(ps_thermal_pair_step(1e-300,1e300,1e300,1e-300,1,1,&pair)==PS_OK && near(pair.x,2e-300) && near(pair.y,2e-300));
    CHECK(ps_ideal_gas_pressure(1e300,1e300,1e300,&p)==PS_OK && near(p,8.31446261815324e300));
    CHECK(ps_ideal_gas_temperature(1e300,1e300,1e300,&t)==PS_OK && near(t,1.2027235504272604e299));
    CHECK(ps_heat_capacity(1e-200,1e200,&c)==PS_OK && near(c,1));
    CHECK(ps_ideal_gas_entropy_change(1,12.5,1e-300,1,1e300,1,&s)==PS_OK && near(s,17269.388197455342));
    CHECK(ps_ideal_gas_entropy_change(1e300,1e-300,300,1,600,1,&s)==PS_OK && near(s,0.6931471805599453));
    CHECK(ps_thermal_reservoir_step(1,1e-300,1e300,1e-200,1e-200,&t)==PS_OK && near(t,1e-100));
    CHECK(ps_thermal_pair_step(1,1e-300,1,1e300,1e-200,1e-200,&pair)==PS_OK && near(pair.x,1e-100) && pair.y==1e300);
    /* Every error preserves the previous output, including numeric extremes. */
    double invalids[]={0,-1,NAN,INFINITY,-INFINITY};
    for(size_t i=0;i<sizeof invalids/sizeof *invalids;i++) {
        double bad=invalids[i];q=17;
        CHECK(ps_ideal_gas_pressure(bad,300,1,&q)==PS_INVALID && q==17);
        CHECK(ps_ideal_gas_volume(1,bad,1,&q)==PS_INVALID && q==17);
        CHECK(ps_ideal_gas_temperature(1,1,bad,&q)==PS_INVALID && q==17);
        CHECK(ps_ideal_gas_energy(1,bad,300,&q)==PS_INVALID && q==17);
        CHECK(ps_heat_capacity(1,bad,&q)==PS_INVALID && q==17);
        CHECK(ps_sensible_heat(1,bad,300,&q)==PS_INVALID && q==17);
        CHECK(ps_heat_flow(1,bad,300,&q)==PS_INVALID && q==17);
        CHECK(ps_ideal_gas_entropy_change(1,12.5,300,1,400,bad,&q)==PS_INVALID && q==17);
        pair=(ps_vec2){17,23};ps_vec2 before=pair;
        CHECK(ps_thermal_pair_step(1,bad,1,300,1,1,&pair)==PS_INVALID && !memcmp(&before,&pair,sizeof pair));
    }
    q=17;
    CHECK(ps_ideal_gas_pressure(DBL_MAX,DBL_MAX,1,&q)==PS_NUMERIC && q==17);
    CHECK(ps_ideal_gas_pressure(DBL_MIN,DBL_MIN,DBL_MAX,&q)==PS_NUMERIC && q==17);
    CHECK(ps_sensible_heat(DBL_MAX,1,DBL_MAX,&q)==PS_NUMERIC && q==17);
    CHECK(ps_thermal_reservoir_step(1,300,400,-1,1,&q)==PS_INVALID && q==17);
    CHECK(ps_thermal_reservoir_step(1,300,400,1,-1,&q)==PS_INVALID && q==17);
    CHECK(ps_ideal_gas_pressure(1,300,1,NULL)==PS_INVALID);
    puts("Thermodynamics: gas state, energy, entropy, heat flow, relaxation and atomic errors passed");
    return 0;
}
