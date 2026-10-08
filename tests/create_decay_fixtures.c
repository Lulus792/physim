#include "physim/data.h"
#include <math.h>
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"Decay fixture %d: %s\n",__LINE__,#x);return 1; } } while (0)
int main(int argc,char **argv) {
    CHECK(argc==2);
    const char *names[]={"decay","growth","irregular","plateau","rest","single"};
    for(unsigned kind=0;kind<6;kind++) {
        ps_context c={.struct_size=sizeof c,.api_version=PS_API_VERSION};
        ps_unit rate={{0,0,-1,0,0,0,0},1,"rad/s"};
        CHECK(ps_channel_add(&c,"angle",PS_RADIAN,"synthetic signal")==0);
        CHECK(ps_channel_add(&c,"angular_velocity",rate,"unused fixture channel")==1);
        CHECK(ps_channel_add(&c,"position.x",PS_METRE,"synthetic analysis input")==2);
        CHECK(ps_channel_add(&c,"position.y",PS_METRE,"unused fixture channel")==3);
        CHECK(ps_channel_add(&c,"energy",PS_JOULE,"constant fixture energy")==4);
        CHECK(ps_channel_add(&c,"sensor.angle",PS_RADIAN,"synthetic signal")==5);
        char path[4096];snprintf(path,sizeof path,"%s/%s.psrun",argv[1],names[kind]);
        ps_run_writer writer;CHECK(ps_run_create(&writer,path,&c,
            "model=synthetic sampled amplitude envelope, not a physical experiment\nintegrator=RK4\n")==PS_OK);
        unsigned count=kind==4?20:kind==5?3:kind==3?2001:1601;
        for(unsigned i=0;i<count;i++) {
            double t=0,a=0;
            if(kind==4){t=i*.1;a=.3;}
            else if(kind==5){t=i*.1;a=i==1?.5:0;}
            else if(kind==3) {
                unsigned phase=i%5,cycle=i/5;t=cycle+phase*.2;
                double amplitude=exp(-.1*(cycle+.3));
                a=phase==1 || phase==2?amplitude:phase==4?-amplitude:0;
            } else {
                unsigned phase=i%4,cycle=i/4;
                const double offsets[]={0,.1,.27,.48};
                t=kind==2?cycle*.7+offsets[phase]:i*.125;
                double amplitude=exp((kind==1?.1:-.1)*t);
                a=phase==1?amplitude:phase==3?-amplitude:0;
            }
            double values[]={a,0,a,0,1,a};CHECK(ps_run_append(&writer,t,values)==PS_OK);
        }
        CHECK(ps_run_close(&writer)==PS_OK);
    }
    puts("Decay fixtures: analytic peak envelopes, irregular times, plateaus, rest and one peak written");
    return 0;
}
