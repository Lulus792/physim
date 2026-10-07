#include "physim/numerics.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) {fprintf(stderr,"linear range line %d: %s\n",__LINE__,#x);return 1;} } while(0)
int main(void) {
 double a[]={1,-1,1,0,1,0,0,0,1},b[]={1e308,1e308,1e308},x[]={7,8,9};
 double saved_a[9],saved_b[3];memcpy(saved_a,a,sizeof a);memcpy(saved_b,b,sizeof b);
 CHECK(ps_linear_solve(a,b,3,0,x)==PS_OK);
 for(size_t i=0;i<3;i++)CHECK(fabs(x[i]/1e308-1)<1e-14);
 CHECK(!memcmp(a,saved_a,sizeof a) && !memcmp(b,saved_b,sizeof b));
 double pivot[]={1,1,-1,1},right[]={1e308,1e308};
 CHECK(ps_linear_solve(pivot,right,2,0,x)==PS_OK && x[0]==0 && fabs(x[1]/1e308-1)<1e-14);
 double diagonal[]={1,0,0,1},mixed[]={1e308,1e-308};
 CHECK(ps_linear_solve(diagonal,mixed,2,0,x)==PS_OK && x[0]==mixed[0] && x[1]==mixed[1]);
 double small[]={DBL_TRUE_MIN,0,0,1},tiny[]={DBL_TRUE_MIN,DBL_TRUE_MIN};
 CHECK(ps_linear_solve(small,tiny,2,0,x)==PS_OK && x[0]==1 && x[1]==DBL_TRUE_MIN);
 double overflow[]={1,-1,0,1},high[]={DBL_MAX,DBL_MAX};x[0]=7;x[1]=8;
 CHECK(ps_linear_solve(overflow,high,2,0,x)==PS_NUMERIC && x[0]==7 && x[1]==8);
 CHECK(ps_linear_solve(a,b,3,0,b)==PS_OK);
 for(size_t i=0;i<3;i++)CHECK(fabs(b[i]/1e308-1)<1e-14);
 double alias_a[]={1,-1,1,0,1,0,0,0,1};
 CHECK(ps_linear_solve(alias_a,saved_b,3,0,alias_a)==PS_OK);
 for(size_t i=0;i<3;i++)CHECK(fabs(alias_a[i]/1e308-1)<1e-14);
 double invalid[]={1,0,0,NAN};x[0]=7;x[1]=8;
 CHECK(ps_linear_solve(invalid,right,2,0,x)==PS_INVALID && x[0]==7 && x[1]==8);
 CHECK(ps_linear_solve(diagonal,right,2,1,x)==PS_INVALID && x[0]==7 && x[1]==8);
 CHECK(ps_linear_solve(diagonal,right,33,0,x)==PS_INVALID && x[0]==7 && x[1]==8);
 double singular[]={1,2,2,4};
 CHECK(ps_linear_solve(singular,right,2,0,x)==PS_SINGULAR && x[0]==7 && x[1]==8);
 puts("Linear range: finite cancellation, elimination growth, independent exponents, subnormal input, alias and overflow rollback passed");return 0;
}
