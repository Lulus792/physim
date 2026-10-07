#include "physim/waves.h"
#include "physim/optics.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Waves/optics %d: %s\n",__LINE__,#x);return 1;}}while(0)
static bool near(double x,double y){return isfinite(x) && fabs(x-y)<=4e-13*fmax(fabs(y),1e-300);}
int main(void) {
    ps_vec2 oscillator={1,0},next;double speed;
    CHECK(ps_harmonic_step(oscillator,2,PS_PI/4,&next)==PS_OK && fabs(next.x)<1e-15 && near(next.y,-2));
    ps_vec2 half,whole;
    CHECK(ps_harmonic_step(oscillator,2,.25,&half)==PS_OK && ps_harmonic_step(half,2,.25,&next)==PS_OK && ps_harmonic_step(oscillator,2,.5,&whole)==PS_OK);
    CHECK(near(next.x,whole.x) && near(next.y,whole.y) && near(4*next.x*next.x+next.y*next.y,4));
    CHECK(ps_string_wave_speed(100,.01,&speed)==PS_OK && speed==100);
    CHECK(ps_string_wave_speed(1e300,1e-300,&speed)==PS_OK && near(speed,1e300));
    CHECK(ps_string_wave_speed(1e-300,1e300,&speed)==PS_OK && near(speed,1e-300));
    ps_vec3 wave;
    CHECK(ps_traveling_wave(.1,2,4,0,.5,.25,&wave)==PS_OK && wave.x==0 && near(wave.y,-.4) && near(wave.z,.2));
    ps_vec3 translated;
    CHECK(ps_traveling_wave(.1,2,4,.2,.7,.3,&wave)==PS_OK && ps_traveling_wave(.1,2,4,.2,1.3,.6,&translated)==PS_OK);
    CHECK(fabs(wave.x-translated.x)<1e-15 && fabs(wave.y-translated.y)<1e-15);
    CHECK(ps_traveling_wave(1e-300,1e300,1e300,0,1e300,1e300,&wave)==PS_OK && wave.x==0 && near(wave.y,-1) && near(wave.z,1));
    double previous[65],current[65],result[65];double lambda=.4,theta=2*asin(lambda*sin(PS_PI/128));
    for(unsigned j=0;j<65;j++){current[j]=sin(PS_PI*j/64);previous[j]=current[j]*cos(theta);}current[0]=current[64]=previous[0]=previous[64]=0;
    for(unsigned n=1;n<=300;n++) {
        CHECK(ps_string_wave_step(previous,current,65,1,1.0/64,lambda/64,result)==PS_OK);
        for(unsigned j=0;j<65;j++)CHECK(fabs(result[j]-sin(PS_PI*j/64)*cos(n*theta))<2e-13);
        memcpy(previous,current,sizeof current);memcpy(current,result,sizeof current);
    }
    double p[]={0,1,0},c[]={0,1,0};
    CHECK(ps_string_wave_step(p,c,3,1,1,1,c)==PS_OK && c[0]==0 && c[1]==-1 && c[2]==0);
    double smallp[]={0,0,0,0,0},smallc[]={0,1e300,0,0,0},smallout[5];
    CHECK(ps_string_wave_step(smallp,smallc,5,1e-200,1,1,smallout)==PS_OK && near(smallout[2],1e-100));
    smallp[1]=2e300;CHECK(ps_string_wave_step(smallp,smallc,5,1e-200,1,1,smallout)==PS_OK && near(smallout[1],-2e-100));
    ps_vec3 normal=ps_v3(0,1,0),incident=ps_v3(.6,-.8,0),ray;
    CHECK(ps_ray_reflect(incident,normal,&ray)==PS_OK && near(ray.x,.6) && near(ray.y,.8));
    CHECK(ps_ray_refract(incident,normal,1,1.5,&ray)==PS_OK && near(ray.x,.4) && near(ray.y,-sqrt(.84)));
    CHECK(near(ps_vlength(ray),1) && near(1*.6,1.5*ray.x));
    CHECK(ps_ray_refract(ps_v3(0,-1,0),normal,DBL_MAX,DBL_MIN,&ray)==PS_OK && ray.y==-1);
    CHECK(ps_ray_refract(ps_v3(2.0/3,-sqrt(5.0)/3,0),normal,1.5,1,&ray)==PS_OK && near(ray.x,1) && fabs(ray.y)<1e-7);
    ray=ps_v3(17,23,29);ps_vec3 saved=ray;
    CHECK(ps_ray_refract(ps_v3(.8,-.6,0),normal,1.5,1,&ray)==PS_SINGULAR && !memcmp(&ray,&saved,sizeof ray));
    CHECK(ps_ray_reflect(ps_v3(0,1,0),normal,&ray)==PS_INVALID && !memcmp(&ray,&saved,sizeof ray));
    CHECK(ps_ray_reflect(ps_v3(2,-1,0),normal,&ray)==PS_INVALID && !memcmp(&ray,&saved,sizeof ray));
    CHECK(ps_thin_lens_image(.1,.3,&next)==PS_OK && near(next.x,.15) && near(next.y,-.5));
    CHECK(ps_thin_lens_image(.1,.05,&next)==PS_OK && near(next.x,-.1) && near(next.y,2));
    CHECK(ps_thin_lens_image(-.1,.3,&next)==PS_OK && near(next.x,-.075) && near(next.y,.25));
    CHECK(ps_thin_lens_image(-DBL_MAX,DBL_MAX,&next)==PS_OK && near(next.x,-DBL_MAX/2) && next.y==.5);
    next=(ps_vec2){17,23};ps_vec2 saved2=next;
    CHECK(ps_thin_lens_image(.1,.1,&next)==PS_SINGULAR && !memcmp(&next,&saved2,sizeof next));
    CHECK(ps_harmonic_step(oscillator,DBL_MAX,DBL_MAX,&next)==PS_NUMERIC && !memcmp(&next,&saved2,sizeof next));
    double before[5]={17,23,29,31,37};memcpy(smallout,before,sizeof before);
    CHECK(ps_string_wave_step(smallp,smallc,5,2,1,1,smallout)==PS_INVALID && !memcmp(smallout,before,sizeof before));
    smallc[4]=1;CHECK(ps_string_wave_step(smallp,smallc,5,1,1,1,smallout)==PS_INVALID && !memcmp(smallout,before,sizeof before));
    CHECK(ps_string_wave_step(NULL,NULL,4097,1,1,1,NULL)==PS_LIMIT);
    CHECK(ps_harmonic_step(oscillator,1,0,&next)==PS_OK && next.x==1 && next.y==0);
    CHECK(ps_harmonic_step(oscillator,0,1,&next)==PS_INVALID);
    CHECK(ps_ray_reflect(incident,normal,NULL)==PS_INVALID);
    double *large_previous=calloc(4096,sizeof(double)),*large_current=calloc(4096,sizeof(double)),*large_next=calloc(4096,sizeof(double));
    CHECK(large_previous && large_current && large_next);
    CHECK(ps_string_wave_step(large_previous,large_current,4096,1,1,.5,large_next)==PS_OK);
    for(unsigned j=0;j<4096;j++)CHECK(large_next[j]==0);
    large_current[4094]=NAN;
    CHECK(ps_string_wave_step(large_previous,large_current,4096,1,1,.5,large_next)==PS_INVALID);
    for(unsigned j=0;j<4096;j++)CHECK(large_next[j]==0);
    free(large_previous);free(large_current);free(large_next);
    puts("Waves/optics: oscillator invariant, traveling phase, discrete eigenmode, CFL/alias/extremes, Snell/TIR and lens signs passed");return 0;
}
