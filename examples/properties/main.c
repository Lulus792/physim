/* Nine caller-supplied properties: synthetic teaching data, no real material. */
#include "physim/properties.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"Material property example line %d\n",__LINE__);return 1; } } while (0)
int main(void) {
    const char *names[]={"density","dynamic_viscosity","kinematic_viscosity","friction","restitution",
                         "thermal_conductivity","specific_heat","electrical_conductivity","permittivity"};
    const double values[]={1200,.006,5e-6,.25,.8,.5,2000,250,2e-11};
    ps_unit units[9],volume,area,denominator,amperes_squared;
    CHECK(ps_unit_power(PS_METRE,3,"m3",&volume)==PS_OK);
    CHECK(ps_unit_divide(PS_KILOGRAM,volume,"kg/m3",&units[0])==PS_OK);
    CHECK(ps_unit_multiply(PS_PASCAL,PS_SECOND,"Pa s",&units[1])==PS_OK);
    CHECK(ps_unit_power(PS_METRE,2,"m2",&area)==PS_OK);
    CHECK(ps_unit_divide(area,PS_SECOND,"m2/s",&units[2])==PS_OK);
    units[3]=units[4]=PS_ONE;
    CHECK(ps_unit_multiply(PS_METRE,PS_KELVIN,"m K",&denominator)==PS_OK);
    CHECK(ps_unit_divide(PS_WATT,denominator,"W/(m K)",&units[5])==PS_OK);
    CHECK(ps_unit_multiply(PS_KILOGRAM,PS_KELVIN,"kg K",&denominator)==PS_OK);
    CHECK(ps_unit_divide(PS_JOULE,denominator,"J/(kg K)",&units[6])==PS_OK);
    CHECK(ps_unit_power(PS_AMPERE,2,"A2",&amperes_squared)==PS_OK);
    CHECK(ps_unit_multiply(PS_WATT,PS_METRE,"W m",&denominator)==PS_OK);
    CHECK(ps_unit_divide(amperes_squared,denominator,"S/m",&units[7])==PS_OK);
    CHECK(ps_unit_multiply(units[7],PS_SECOND,"F/m",&units[8])==PS_OK);
    const double queries[][2]={{280,0},{300,100000},{320,200000}};
    for(unsigned q=0;q<3;q++) {
        ps_quantity samples[9];
        for(unsigned i=0;i<9;i++) {
            ps_property property={.model=PS_PROPERTY_CONSTANT,.name=names[i],
                .source="synthetic teaching data; no calibrated material",.value_unit=units[i],
                .minimum_temperature_k=280,.maximum_temperature_k=320,
                .minimum_pressure_pa=0,.maximum_pressure_pa=200000,.constant_value_si=values[i]};
            CHECK(ps_property_evaluate(&property,queries[q][0],queries[q][1],&samples[i])==PS_OK);
            ps_quantity before;memcpy(&before,&samples[i],sizeof before);
            CHECK(ps_property_evaluate(&property,279,100000,&samples[i])==PS_INVALID &&
                  !memcmp(&before,&samples[i],sizeof before));
            printf("%u,%s,%.17g",q,names[i],samples[i].value);
            for(unsigned axis=0;axis<7;axis++) printf(",%d",samples[i].unit.dimension[axis]);
            putchar('\n');
        }
        /* This consuming example checks its physical contract explicitly. */
        CHECK(samples[0].value>0 && samples[1].value>=0 && samples[2].value>=0 &&
              samples[3].value>=0 && samples[4].value>=0 && samples[4].value<=1 &&
              samples[5].value>=0 && samples[6].value>0 && samples[7].value>=0 && samples[8].value>0);
        ps_quantity kinematic;
        CHECK(ps_quantity_divide(samples[1],samples[0],"m2/s",&kinematic)==PS_OK &&
              ps_unit_compatible(kinematic.unit,units[2]));
        CHECK(kinematic.value>4.99999999999999e-6 && kinematic.value<5.00000000000001e-6);
    }
    return 0;
}
