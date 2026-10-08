#include "physim/experiment.h"
#include "physim/analysis.h"
#include "physim/units.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"Pendulum medium %d: %s\n",__LINE__,#x);return 1; } } while (0)
static const ps_experiment_api *load(const char *path,void **module) {
    *module=ps_module_open(path);if(!*module)return NULL;
    void *symbol=ps_module_symbol(*module,"ps_get_experiment");
    ps_experiment_entry entry=NULL;memcpy(&entry,&symbol,sizeof entry);return entry?entry():NULL;
}
static bool near(double a,double b) {return isfinite(a) && isfinite(b) && fabs(a-b)<5e-10*fmax(1,fabs(b));}
static int parameters(ps_context *c,double density) {
    const char *names[]={"length","initialAngle","mass","airDensity","dragCoefficient","area","sensorNoise"};
    double values[]={1.2,.6,2,density,.8,.08,.02};
    for(unsigned i=0;i<7;i++)CHECK(ps_parameter_override(c,names[i],values[i])==PS_OK);
    return 0;
}
static int units(ps_context *c) {
    const char *names[]={"length","initialAngle","mass","airDensity","dragCoefficient","area","sensorNoise"};
    const int8_t dimensions[][7]={{1,0,0,0,0,0,0},{0},{0,1,0,0,0,0,0},
        {-3,1,0,0,0,0,0},{0},{2,0,0,0,0,0,0},{0}};
    CHECK(c->parameter_count==7);
    for(unsigned i=0;i<7;i++) {
        unsigned at=0;while(at<c->parameter_count && strcmp(c->parameters[at].name,names[i]))at++;
        ps_parameter_unit unit;CHECK(at<c->parameter_count && ps_parameter_unit_read(c,at,&unit)==PS_OK);
        CHECK(!memcmp(unit.dimension,dimensions[i],7) && unit.scale==1);
    }
    return 0;
}
static int forces(const ps_experiment_api *api,ps_context *c,double density) {
    ps_scene scene={0};api->build_scene(c,&scene);CHECK(!c->error[0] && ps_scene_valid(&scene));
    double total_x=0,total_y=0;unsigned arrows=0;
    double angle=c->values[0],rate=c->values[1],x=1.2*sin(angle),y=-1.2*cos(angle);
    for(unsigned i=0;i<scene.count;i++) {
        const ps_object *v=&scene.objects[i];
        if(v->id<7 || v->id>9)continue;
        CHECK(v->shape==PS_ARROW && v->parent_id==3 && near(v->a.x,x) && near(v->a.y,y));
        total_x+=(v->b.x-v->a.x)/.05;total_y+=(v->b.y-v->a.y)/.05;arrows++;
        if(v->id==7)CHECK(near(v->b.y-v->a.y,-.05*2*9.80665));
    }
    CHECK(arrows==(density?3u:2u));
    double speed=1.2*rate;
    double tangent_force=-2*9.80665*sin(angle)-.5*density*.8*.08*speed*fabs(speed);
    CHECK(near(total_x,tangent_force*cos(angle)-2*rate*rate*x));
    CHECK(near(total_y,tangent_force*sin(angle)-2*rate*rate*y));
    return 0;
}
static int pair(const char *c_path,const char *phys_path,bool verlet) {
    void *modules[2];const ps_experiment_api *api[]={load(c_path,&modules[0]),load(phys_path,&modules[1])};
    CHECK(api[0] && api[1]);
    ps_context c[2]={{.struct_size=sizeof(ps_context),.api_version=PS_API_VERSION,.seed=42}};c[1]=c[0];
    double density=verlet?0:1.225;
    for(unsigned i=0;i<2;i++) {
        CHECK(!parameters(&c[i],density) && api[i]->create(&c[i])==PS_OK && ps_parameter_finalize(&c[i])==PS_OK);
        CHECK(!units(&c[i]) && c[i].channel_count==6);
    }
    double initial[6];memcpy(initial,c[0].values,sizeof initial);
    ps_statistics noise={0};
    for(unsigned step=0;step<=5000;step++) {
        for(unsigned channel=0;channel<6;channel++)CHECK(near(c[0].values[channel],c[1].values[channel]));
        ps_statistics_push(&noise,c[0].values[5]-c[0].values[0]);
        if(step%1000==0)for(unsigned i=0;i<2;i++)CHECK(!forces(api[i],&c[i],density));
        if(step<5000)for(unsigned i=0;i<2;i++) {
            CHECK(api[i]->step(&c[i],.002)==PS_OK);c[i].time_s=(step+1)*.002;
        }
    }
    CHECK(fabs(noise.mean)<.0015 && ps_statistics_stddev(&noise)>.018 && ps_statistics_stddev(&noise)<.022);
    if(density)CHECK(c[0].values[4]<initial[4]*.99);
    for(unsigned i=0;i<2;i++) {
        ps_context peer={.struct_size=sizeof peer,.api_version=PS_API_VERSION,.seed=1007};
        double held[6];memcpy(held,c[i].values,sizeof held);ps_rng held_rng=c[i].rng;
        CHECK(ps_parameter_override(&peer,"mass",.25)==PS_OK &&
              ps_parameter_override(&peer,"length",.7)==PS_OK &&
              ps_parameter_override(&peer,"initialAngle",-.5)==PS_OK);
        CHECK(api[i]->create(&peer)==PS_OK && api[i]->step(&peer,.01)==PS_OK);
        CHECK(!memcmp(held,c[i].values,sizeof held) && !memcmp(&held_rng,&c[i].rng,sizeof held_rng));
        CHECK(api[i]->reset(&c[i])==PS_OK);
        for(unsigned channel=0;channel<6;channel++)CHECK(near(c[i].values[channel],initial[channel]));
        api[i]->destroy(&peer);
        double saved[6];memcpy(saved,c[i].values,sizeof saved);ps_rng rng=c[i].rng;
        CHECK(api[i]->step(&c[i],NAN)==PS_INVALID && !memcmp(saved,c[i].values,sizeof saved) && !memcmp(&rng,&c[i].rng,sizeof rng));
        CHECK(api[i]->step(&c[i],1e155)!=PS_OK && !memcmp(saved,c[i].values,sizeof saved) &&
              !memcmp(&rng,&c[i].rng,sizeof rng));
        api[i]->destroy(&c[i]);
        ps_context invalid={.struct_size=sizeof invalid,.api_version=PS_API_VERSION,.seed=42};
        CHECK(ps_parameter_override(&invalid,"mass",0)==PS_OK && api[i]->create(&invalid)!=PS_OK);
        api[i]->destroy(&invalid);
        if(verlet) {
            invalid=(ps_context){.struct_size=sizeof invalid,.api_version=PS_API_VERSION,.seed=42};
            CHECK(!parameters(&invalid,1.225) && api[i]->create(&invalid)!=PS_OK);
            CHECK(strstr(invalid.error,"Velocity Verlet requires zero velocity-dependent drag"));
            api[i]->destroy(&invalid);
            invalid=(ps_context){.struct_size=sizeof invalid,.api_version=PS_API_VERSION,.seed=42};
            CHECK(ps_parameter_override(&invalid,"airDensity",1e-200)==PS_OK &&
                  ps_parameter_override(&invalid,"area",1e-200)==PS_OK && api[i]->create(&invalid)!=PS_OK);
            api[i]->destroy(&invalid);
            invalid=(ps_context){.struct_size=sizeof invalid,.api_version=PS_API_VERSION,.seed=42};
            CHECK(ps_parameter_override(&invalid,"airDensity",1.225)==PS_OK &&
                  ps_parameter_override(&invalid,"dragCoefficient",0)==PS_OK &&
                  api[i]->create(&invalid)==PS_OK && api[i]->step(&invalid,.01)==PS_OK);
            api[i]->destroy(&invalid);
        }
        ps_module_close(modules[i]);
    }
    return 0;
}
int main(int argc,char **argv) {
    CHECK(argc==10);
    CHECK(!pair(argv[1],argv[5],false));
    CHECK(!pair(argv[2],argv[6],false) && !pair(argv[2],argv[7],false));
    CHECK(!pair(argv[3],argv[8],false) && !pair(argv[4],argv[9],true));
    puts("Pendulum medium: seven SI parameters, C/Physim drag/noise parity, force sums, seeded reset and Verlet guards passed");
    return 0;
}
