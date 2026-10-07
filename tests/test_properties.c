#include "physim/properties.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Property line %d: %s\n",__LINE__,#x);return 1;}} while(0)
int main(void) {
    double t[]={250,300,400},pressure[]={0,1e5,5e5},v[9];
    for(size_t i=0;i<3;i++)for(size_t j=0;j<3;j++)v[i*3+j]=1200-.2*t[i]+1e-6*pressure[j];
    ps_unit density={{-3,1,0,0,0,0,0},1,"kg/m³"};
    ps_property p={.model=PS_PROPERTY_TABLE,.name="Dichte α",.source="synthetic affine reference",.value_unit=density,
        .minimum_temperature_k=273,.maximum_temperature_k=373,.minimum_pressure_pa=0,.maximum_pressure_pa=5e5,
        .temperature_k=t,.pressure_pa=pressure,.values_si=v,.temperature_count=3,.pressure_count=3};
    CHECK(ps_property_validate(&p)==PS_OK);ps_quantity out={17,PS_ONE};
    for(unsigned i=0;i<=100;i++)for(unsigned j=0;j<=100;j++) {
        double temperature=273+i,pa=j*5000.0;
        CHECK(ps_property_evaluate(&p,temperature,pa,&out)==PS_OK);
        CHECK(fabs(out.value-(1200-.2*temperature+1e-6*pa))<5e-13);
        CHECK(ps_unit_compatible(out.unit,density) && out.unit.scale==1);
    }
    ps_quantity before=out;
    CHECK(ps_property_evaluate(&p,272,0,&out)==PS_INVALID && !memcmp(&out,&before,sizeof out));
    CHECK(ps_property_evaluate(&p,300,5e5+1,&out)==PS_INVALID && !memcmp(&out,&before,sizeof out));
    CHECK(ps_property_evaluate(&p,NAN,0,&out)==PS_INVALID && !memcmp(&out,&before,sizeof out));
    ps_property bad=p;bad.value_unit.scale=.01;CHECK(ps_property_validate(&bad)==PS_INVALID);
    bad=p;bad.temperature_count=65;CHECK(ps_property_validate(&bad)==PS_LIMIT);
    bad=p;bad.source="\xc0\x80";CHECK(ps_property_validate(&bad)==PS_INVALID);
    double repeated[]={250,300,300};bad=p;bad.temperature_k=repeated;CHECK(ps_property_validate(&bad)==PS_INVALID);
    bad=p;bad.maximum_temperature_k=450;CHECK(ps_property_validate(&bad)==PS_INVALID);
    bad=p;bad.model=(ps_property_model)999;CHECK(ps_property_validate(&bad)==PS_INVALID);
    bad=p;bad.values_si=NULL;CHECK(ps_property_validate(&bad)==PS_INVALID);
    double nonfinite[]={0,1,2,3,NAN,5,6,7,8};bad=p;bad.values_si=nonfinite;CHECK(ps_property_validate(&bad)==PS_INVALID);
    p.model=PS_PROPERTY_CONSTANT;p.constant_value_si=42;
    CHECK(ps_property_evaluate(&p,300,0,&out)==PS_OK && out.value==42);
    p.model=PS_PROPERTY_TABLE;p.temperature_count=p.pressure_count=1;
    p.temperature_k=(double[]){300};p.pressure_pa=(double[]){1e5};p.values_si=(double[]){42};
    CHECK(ps_property_evaluate(&p,273,5e5,&out)==PS_OK && out.value==42);
    /* Opposite extremes interpolate without an overflowing difference. */
    p.temperature_count=2;p.temperature_k=(double[]){273,373};p.pressure_count=1;
    p.values_si=(double[]){-DBL_MAX,DBL_MAX};
    CHECK(ps_property_evaluate(&p,323,1e5,&out)==PS_OK && out.value==0);
    CHECK(ps_property_evaluate(&p,273,1e5,&out)==PS_OK && out.value==-DBL_MAX);
    /* No hidden allocation: maximum 64x64 table, exact corner and affine oracle. */
    double axis[64],grid[4096];for(unsigned i=0;i<64;i++)axis[i]=i;
    for(unsigned i=0;i<64;i++)for(unsigned j=0;j<64;j++)grid[i*64+j]=i+2*j;
    p=(ps_property){.model=PS_PROPERTY_TABLE,.value_unit=PS_ONE,.minimum_temperature_k=0,.maximum_temperature_k=63,
        .minimum_pressure_pa=0,.maximum_pressure_pa=63,.temperature_k=axis,.pressure_pa=axis,.values_si=grid,.temperature_count=64,.pressure_count=64};
    CHECK(ps_property_evaluate(&p,31.5,20.5,&out)==PS_OK && out.value==72.5);
    CHECK(ps_property_evaluate(&p,63,63,&out)==PS_OK && out.value==189);
    /* Non-square grid with a genuine T*P term detects wrong row order and
     * missing cross weights; affine planes alone cannot do that. */
    p.temperature_count=3;p.pressure_count=2;p.maximum_temperature_k=20;p.maximum_pressure_pa=20;
    p.temperature_k=(double[]){0,10,20};p.pressure_pa=(double[]){0,20};
    p.values_si=(double[]){2,82,32,1112,62,2142};
    CHECK(ps_property_evaluate(&p,15,3,&out)==PS_OK && fabs(out.value-284)<1e-12);
    puts("Properties: SI domains, affine oracle, independent axes, constants, UTF-8, maximum grid and atomic errors passed");return 0;
}
