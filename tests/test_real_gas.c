#include "physim/thermodynamics.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Real gas %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int near(double a,double b){return isfinite(a) && fabs(a-b)<3e-12*fmax(1,fabs(b));}
int main(void) {
    double p,d,u,s,ideal;
    CHECK(ps_vdw_gas_pressure(1,450,.001,.4,4e-5,&p)==PS_OK);
    CHECK(near(p,PS_MOLAR_GAS_CONSTANT*450/.00096-.4/1e-6));
    CHECK(ps_vdw_gas_pressure_derivative(1,450,.001,.4,4e-5,&d)==PS_OK && d<0);
    CHECK(ps_vdw_gas_energy(1,20.8,450,.001,.4,&u)==PS_OK && near(u,8960));
    CHECK(ps_vdw_gas_entropy_change(1,20.8,450,.005,450,.001,4e-5,&s)==PS_OK && s<0);
    double reverse;CHECK(ps_vdw_gas_entropy_change(1,20.8,450,.001,450,.005,4e-5,&reverse)==PS_OK && near(reverse,-s));
    for(unsigned i=1;i<50;i++) {
        double v=.001+i*.0001;
        CHECK(ps_vdw_gas_pressure(2,300,v,0,0,&p)==PS_OK && ps_ideal_gas_pressure(2,300,v,&ideal)==PS_OK && near(p,ideal));
        CHECK(ps_vdw_gas_energy(2,12.5,300,v,0,&u)==PS_OK && u==7500);
    }
    /* Homogeneous subcritical algebra is returned, not silently stabilized. */
    double tc=8*.4/(27*PS_MOLAR_GAS_CONSTANT*4e-5);
    CHECK(ps_vdw_gas_pressure_derivative(1,.8*tc,3*4e-5,.4,4e-5,&d)==PS_OK && d>0);
    CHECK(ps_vdw_gas_pressure(1,1,.001,.4,4e-5,&p)==PS_OK && p<0);
    CHECK(ps_vdw_gas_energy(1,1,1,.001,.4,&u)==PS_OK && u<0);
    double h=1e-8,left,right;
    CHECK(ps_vdw_gas_pressure(1,450,.001-h,.4,4e-5,&left)==PS_OK);
    CHECK(ps_vdw_gas_pressure(1,450,.001+h,.4,4e-5,&right)==PS_OK);
    CHECK(ps_vdw_gas_pressure_derivative(1,450,.001,.4,4e-5,&d)==PS_OK && fabs((right-left)/(2*h)/d-1)<2e-9);
    p=19;
    CHECK(ps_vdw_gas_pressure(1,300,4e-5,.4,4e-5,&p)==PS_INVALID && p==19);
    CHECK(ps_vdw_gas_pressure(1,300,1,-1,0,&p)==PS_INVALID && p==19);
    CHECK(ps_vdw_gas_pressure(1,NAN,1,0,0,&p)==PS_INVALID && p==19);
    CHECK(ps_vdw_gas_pressure(DBL_MAX,DBL_MAX,1,0,0,&p)==PS_NUMERIC && p==19);
    CHECK(ps_vdw_gas_entropy_change(1,1,300,1,300,0,0,&p)==PS_INVALID && p==19);
    CHECK(ps_vdw_gas_pressure(1e300,1e300,1e300,0,0,&p)==PS_OK && near(p,PS_MOLAR_GAS_CONSTANT*1e300));
    /* Both terms overflow separately, but their cancellation is finite. */
    CHECK(ps_vdw_gas_energy(1e200,1e100,1e10,1e200,1e110,&u)==PS_OK && isfinite(u));
    CHECK(ps_vdw_gas_pressure(1,300,1,0,0,NULL)==PS_INVALID);
    double extensive;
    CHECK(ps_vdw_gas_pressure(3,450,.003, .4,4e-5,&extensive)==PS_OK);
    CHECK(ps_vdw_gas_pressure(1,450,.001,.4,4e-5,&p)==PS_OK && near(extensive,p));
    puts("Real gas: ideal limit, stable/unstable algebra, energy reference, entropy reversal, derivative and atomic extremes passed");return 0;
}
