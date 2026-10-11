#include "physim/experiment.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"Pendulum balance %d: %s\n",__LINE__,#x);return 1; } } while (0)
static const ps_experiment_api *load(const char *path,void **module) {
    *module=ps_module_open(path); if (!*module)return NULL;
    void *symbol=ps_module_symbol(*module,"ps_get_experiment");
    ps_experiment_entry entry=NULL;memcpy(&entry,&symbol,sizeof entry);return entry?entry():NULL;
}
static int evaluate(const ps_experiment_api *api,unsigned method,double density,double dt,
                    double *error,double final[3]) {
    ps_context c={.struct_size=sizeof c,.api_version=PS_API_VERSION,.seed=42};
    const char *names[]={"length","mass","initialAngle","airDensity","area","dragCoefficient","integrator"};
    double values[]={1.2,2,.6,density,.08,.8,method};
    for(unsigned i=0;i<7;i++)CHECK(ps_parameter_override(&c,names[i],values[i])==PS_OK);
    CHECK(api->create(&c)==PS_OK && c.channel_count==11);
    CHECK(!strcmp(c.channels[9].name,"energy.dissipated") && !strcmp(c.channels[10].name,"energy.balance"));
    const int8_t energy_dim[]={2,1,-2,0,0,0,0};
    CHECK(!memcmp(c.channels[9].dimension,energy_dim,7) && !memcmp(c.channels[10].dimension,energy_dim,7));
    double initial=2*9.80665*1.2*(1-cos(.6)),previous=0;
    CHECK(c.values[9]==0 && fabs(c.values[10]-initial)<1e-14);
    *error=0;
    unsigned steps=(unsigned)llround(4/dt);
    for(unsigned i=0;i<steps;i++) {
        CHECK(api->step(&c,dt)==PS_OK);c.time_s=(i+1)*dt;
        CHECK(c.values[9]>=previous && c.values[10]==c.values[4]+c.values[9]);
        if(!density)CHECK(c.values[9]==0);
        previous=c.values[9];*error=fmax(*error,fabs(c.values[10]-initial));
    }
    final[0]=c.values[0];final[1]=c.values[1];final[2]=c.values[9];
    if(density)CHECK(c.values[9]>.1);
    if(density && method>=2)CHECK(c.values[4]<initial*.99);
    CHECK(api->reset(&c)==PS_OK && c.values[9]==0 && fabs(c.values[10]-initial)<1e-14);
    api->destroy(&c);return 0;
}
int main(int argc,char **argv) {
    CHECK(argc==3);void *modules[2];
    const ps_experiment_api *api[]={load(argv[1],&modules[0]),load(argv[2],&modules[1])};
    CHECK(api[0] && api[1]);
    for(unsigned method=0;method<5;method++) {
        double errors[2][3],finals[2][3][3];
        for(unsigned language=0;language<2;language++)
            for(unsigned level=0;level<3;level++)
                CHECK(!evaluate(api[language],method,method==3?0:1.225,.08/pow(2,level),
                                &errors[language][level],finals[language][level]));
        for(unsigned level=0;level<3;level++)
            for(unsigned field=0;field<3;field++)
                CHECK(fabs(finals[0][level][field]-finals[1][level][field])<2e-10);
        for(unsigned language=0;language<2;language++) {
            printf("Method %u language %u balance errors: %.9g %.9g %.9g\n",method,language,errors[language][0],errors[language][1],errors[language][2]);
            if(method==2) {
                /* An invariant can converge faster than the state solution.
                 * Require at least fourth-order reduction, not an upper bound. */
                CHECK(errors[language][0]/errors[language][1]>12);
                CHECK(errors[language][1]/errors[language][2]>12);
            } else if(method<2)CHECK(errors[language][0]/errors[language][2]>3);
            else if(method==3)CHECK(errors[language][0]/errors[language][2]>14);
            else CHECK(errors[language][2]<2e-7);
        }
        printf("Method %u balance errors: %.9g %.9g %.9g\n",method,errors[0][0],errors[0][1],errors[0][2]);
    }
    for(unsigned i=0;i<2;i++)ps_module_close(modules[i]);
    puts("Pendulum energy balance: 30 C/Physim integrations, SI, dissipation, reset and convergence passed");
    return 0;
}
