#include "physim/units.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Quantity sum %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void) {
    ps_unit twice=PS_METRE;twice.scale=2;twice.symbol="2m";
    ps_quantity a={-0x1p1023,PS_METRE},b={0x1p1023,twice},out={19,PS_SECOND};
    CHECK(ps_quantity_add(a,b,&out)==PS_OK && out.value==0x1p1023 && out.unit.symbol==a.unit.symbol && out.unit.scale==1);
    a.value=0x1p1023;CHECK(ps_quantity_subtract(a,b,&out)==PS_OK && out.value==-0x1p1023);
    /* Exact dyadic half-subnormal conversion can still contribute to the sum. */
    ps_unit half=PS_METRE;half.scale=.5;half.symbol="half m";
    a.value=DBL_TRUE_MIN;b=(ps_quantity){DBL_TRUE_MIN,half};
    CHECK(ps_quantity_add(a,b,&out)==PS_OK && out.value==2*DBL_TRUE_MIN);
    b.unit.scale=nextafter(.5,0);
    CHECK(ps_quantity_subtract(a,b,&out)==PS_OK && out.value==DBL_TRUE_MIN);
    b.unit.scale=nextafter(.5,1);
    CHECK(ps_quantity_subtract(a,b,&out)==PS_NUMERIC);
    b.unit=half;
    out=(ps_quantity){19,PS_SECOND};
    CHECK(ps_quantity_subtract(a,b,&out)==PS_NUMERIC && out.value==19 && out.unit.symbol==PS_SECOND.symbol);
    b=(ps_quantity){DBL_TRUE_MIN,PS_METRE};CHECK(ps_quantity_subtract(a,b,&out)==PS_OK && out.value==0);
    a.value=1;b=(ps_quantity){DBL_TRUE_MIN,half};CHECK(ps_quantity_add(a,b,&out)==PS_OK && out.value==1);
    a=(ps_quantity){1,PS_METRE};b=(ps_quantity){50,PS_METRE};b.unit.scale=.01;b.unit.symbol="cm";
    CHECK(ps_quantity_add(a,b,&out)==PS_OK && out.value==1.5);
    CHECK(ps_quantity_add(b,a,&out)==PS_OK && out.value==150 && out.unit.scale==.01 && out.unit.symbol==b.unit.symbol);
    CHECK(ps_quantity_subtract(b,a,&b)==PS_OK && b.value==-50 && b.unit.scale==.01);
    out=(ps_quantity){19,PS_SECOND};ps_quantity saved=out;
    a.value=DBL_MAX;b=(ps_quantity){DBL_MAX,PS_METRE};
    CHECK(ps_quantity_add(a,b,&out)==PS_NUMERIC && !memcmp(&out,&saved,sizeof out));
    b.unit=PS_SECOND;CHECK(ps_quantity_add(a,b,&out)==PS_INVALID && !memcmp(&out,&saved,sizeof out));
    b.unit=PS_METRE;b.value=NAN;CHECK(ps_quantity_subtract(a,b,&out)==PS_INVALID && !memcmp(&out,&saved,sizeof out));
    b.value=1;b.unit.scale=0;CHECK(ps_quantity_add(a,b,&out)==PS_INVALID && !memcmp(&out,&saved,sizeof out));
    CHECK(ps_quantity_add(a,a,NULL)==PS_INVALID);
    a.value=-0.0;b=(ps_quantity){0,PS_METRE};CHECK(ps_quantity_subtract(a,b,&out)==PS_OK && signbit(out.value));
    puts("Quantity sums: overflowing conversion, half-subnormal contribution, cancellation, left units, aliases and atomic errors passed");return 0;
}
