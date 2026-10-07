#include "physim/electromagnetism.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Electromagnetism %d: %s\n",__LINE__,#x);return 1;}}while(0)
static bool near(double x,double y) { return isfinite(x) && fabs(x-y)<=3e-13*fmax(fabs(y),1e-300); }
int main(void) {
    ps_vec3 zero={0},field,force;double potential,current,voltage,power,r,energy,next;
    CHECK(ps_point_charge_field(1e-9,zero,ps_v3(1,0,0),PS_VACUUM_PERMITTIVITY,&field)==PS_OK);
    CHECK(near(field.x,8.987551786170817) && field.y==0 && field.z==0);
    CHECK(ps_point_charge_potential(1e-9,zero,ps_v3(1,0,0),PS_VACUUM_PERMITTIVITY,&potential)==PS_OK && near(potential,field.x));
    CHECK(ps_point_charge_field(-1e-9,zero,ps_v3(2,0,0),PS_VACUUM_PERMITTIVITY,&field)==PS_OK && near(field.x,-8.987551786170817/4));
    CHECK(ps_point_charge_field(1,ps_v3(1,2,3),ps_v3(4,6,3),1,&field)==PS_OK);
    CHECK(near(field.x,3/(500*PS_PI)) && near(field.y,4/(500*PS_PI)) && field.z==0);
    /* Field is minus the potential gradient, independently approximated. */
    for(unsigned i=0;i<3;i++) {
        ps_vec3 a=ps_v3(3,4,5),b=a;double *ap=i==0?&a.x:i==1?&a.y:&a.z,*bp=i==0?&b.x:i==1?&b.y:&b.z;*ap+=1e-4;*bp-=1e-4;
        double va,vb;CHECK(ps_point_charge_potential(1,zero,a,1,&va)==PS_OK && ps_point_charge_potential(1,zero,b,1,&vb)==PS_OK);
        CHECK(ps_point_charge_field(1,zero,ps_v3(3,4,5),1,&field)==PS_OK);double e=i==0?field.x:i==1?field.y:field.z;
        CHECK(fabs(-(va-vb)/.0002-e)<1e-12);
    }
    CHECK(ps_lorentz_force(2,ps_v3(1,2,3),ps_v3(4,0,0),ps_v3(0,0,5),&force)==PS_OK && force.x==2 && force.y==-36 && force.z==6);
    CHECK(ps_lorentz_force(-2,zero,ps_v3(4,3,2),ps_v3(1,2,5),&force)==PS_OK && ps_vdot(force,ps_v3(4,3,2))==0);
    CHECK(ps_resistor_current(-12,100,&current)==PS_OK && near(current,-.12));
    CHECK(ps_resistor_voltage(current,100,&voltage)==PS_OK && near(voltage,-12));
    CHECK(ps_resistor_power(-12,100,&power)==PS_OK && near(power,1.44));
    CHECK(ps_resistance_series(100,300,&r)==PS_OK && r==400);
    CHECK(ps_resistance_parallel(100,300,&r)==PS_OK && near(r,75));
    CHECK(ps_capacitor_energy(.002,12,&energy)==PS_OK && near(energy,.144));
    CHECK(ps_rc_voltage_step(1000,.002,0,12,2,&next)==PS_OK && near(next,7.585446705942692));
    CHECK(ps_rc_voltage_step(1000,.002,12,0,2,&next)==PS_OK && near(next,4.414553294057308));
    CHECK(ps_rc_voltage_step(1000,.002,-12,12,2,&next)==PS_OK && near(next,3.1708934118853838));
    double half,whole;CHECK(ps_rc_voltage_step(1000,.002,0,12,1,&half)==PS_OK);
    CHECK(ps_rc_voltage_step(1000,.002,half,12,1,&next)==PS_OK && ps_rc_voltage_step(1000,.002,0,12,2,&whole)==PS_OK && near(next,whole));
    CHECK(ps_rc_voltage_step(1e200,1e200,0,1e300,1,&next)==PS_OK && near(next,1e-100));
    CHECK(ps_rc_voltage_step(1e-200,1e-200,12,3,1,&next)==PS_OK && next==3);
    CHECK(ps_rc_voltage_step(1,1,DBL_MAX,-DBL_MAX,1,&next)==PS_OK && isfinite(next));
    CHECK(ps_resistor_power(1e200,1e200,&power)==PS_OK && near(power,1e200));
    CHECK(ps_capacitor_energy(1e-300,1e300,&energy)==PS_OK && near(energy,5e299));
    CHECK(ps_resistance_parallel(DBL_MAX,DBL_MAX,&r)==PS_OK && r==DBL_MAX/2);
    CHECK(ps_point_charge_field(1e300,ps_v3(-1e300,0,0),ps_v3(1e300,0,0),1e-300,&field)==PS_OK && near(field.x,1/(16*PS_PI)));
    CHECK(ps_point_charge_field(1e300,ps_v3(-1e308,0,0),ps_v3(1e308,0,0),1e-300,&field)==PS_OK && near(field.x,1e-16/(16*PS_PI)));
    CHECK(ps_lorentz_force(1e-300,zero,ps_v3(1e300,0,0),ps_v3(0,0,1e300),&force)==PS_OK && near(force.y,-1e300));
    CHECK(ps_lorentz_force(1e300,ps_v3(1e-300,0,0),ps_v3(0,1e300,1e300),ps_v3(0,1e300,1e300),&force)==PS_OK && near(force.x,1));
    double bad[]={0,-1,NAN,INFINITY};
    for(unsigned i=0;i<4;i++) {
        voltage=17;CHECK(ps_resistor_current(1,bad[i],&voltage)==PS_INVALID && voltage==17);
        CHECK(ps_capacitor_energy(bad[i],1,&voltage)==PS_INVALID && voltage==17);
        field=ps_v3(17,23,29);ps_vec3 before=field;
        CHECK(ps_point_charge_field(1,zero,ps_v3(1,0,0),bad[i],&field)==PS_INVALID && !memcmp(&field,&before,sizeof field));
    }
    field=ps_v3(17,23,29);ps_vec3 before=field;
    CHECK(ps_point_charge_field(1,zero,zero,1,&field)==PS_SINGULAR && !memcmp(&field,&before,sizeof field));
    CHECK(ps_point_charge_field(DBL_MAX,zero,ps_v3(DBL_MIN,0,0),1,&field)==PS_NUMERIC && !memcmp(&field,&before,sizeof field));
    CHECK(ps_lorentz_force(DBL_MAX,ps_v3(DBL_MAX,0,0),zero,zero,&field)==PS_NUMERIC && !memcmp(&field,&before,sizeof field));
    voltage=17;CHECK(ps_resistance_series(DBL_MAX,DBL_MAX,&voltage)==PS_NUMERIC && voltage==17);
    CHECK(ps_rc_voltage_step(1,1,12,0,-1,&voltage)==PS_INVALID && voltage==17);
    CHECK(ps_rc_voltage_step(1,1,12,0,0,&voltage)==PS_OK && voltage==12);
    CHECK(ps_point_charge_field(1,zero,ps_v3(1,0,0),1,NULL)==PS_INVALID);
    puts("Electromagnetism: Coulomb field/gradient, Lorentz work, circuit laws, exact RC and atomic extremes passed");return 0;
}
