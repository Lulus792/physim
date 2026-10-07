#include "physim/fluid.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Fluid %d: %s\n",__LINE__,#x);return 1;}}while(0)
static bool near(double x,double y){return isfinite(x) && fabs(x-y)<=4e-13*fmax(fabs(y),1e-300);}
int main(void) {
    double g,q,power,re,pressure;
    CHECK(ps_pipe_conductance(.005,1,.01,&g)==PS_OK && near(g,2.454369260617026e-8));
    CHECK(ps_pipe_flow(g,100,0,&q)==PS_OK && near(q,2.454369260617026e-6));
    CHECK(ps_pipe_power(g,100,0,&power)==PS_OK && near(power,q*100));
    CHECK(ps_reynolds_number(1000,-.1,.01,.01,&re)==PS_OK && re==100);
    CHECK(ps_hydrostatic_pressure(100,1000,9.80665,2,&pressure)==PS_OK && near(pressure,19713.3));
    uint8_t fixed[]={1,0,1,1};double boundary[]={100,NAN,0,0},pressures[4],flows[3];
    ps_pipe_edge edges[]={{0,1,g},{1,2,g},{1,3,g}};
    CHECK(ps_pipe_network_solve(fixed,boundary,4,edges,3,pressures,flows)==PS_OK);
    CHECK(near(pressures[1],100.0/3) && near(flows[0],2*flows[1]) && near(flows[1],flows[2]));
    CHECK(fabs(flows[0]-flows[1]-flows[2])<1e-20);
    CHECK(ps_pipe_network_solve(fixed,boundary,4,edges,3,boundary,flows)==PS_OK && near(boundary[1],100.0/3));
    /* Parallel edges and permutation have the same physical solution. */
    ps_pipe_edge parallel[]={{0,1,.5*g},{0,1,.5*g},{1,2,g},{1,3,g}};double q4[4];
    CHECK(ps_pipe_network_solve(fixed,boundary,4,parallel,4,pressures,q4)==PS_OK && near(pressures[1],100.0/3));
    CHECK(near(q4[0]+q4[1],q4[2]+q4[3]));
    double savedp[4]={17,23,29,31},savedq[3]={37,41,43};memcpy(pressures,savedp,sizeof savedp);memcpy(flows,savedq,sizeof savedq);
    uint8_t unanchored[]={0,0,0,0};CHECK(ps_pipe_network_solve(unanchored,boundary,4,edges,3,pressures,flows)==PS_SINGULAR && !memcmp(pressures,savedp,sizeof savedp) && !memcmp(flows,savedq,sizeof savedq));
    CHECK(ps_pipe_network_solve(fixed,boundary,4,edges,3,pressures,pressures+1)==PS_INVALID);
    CHECK(ps_pipe_network_solve(fixed,boundary,17,edges,3,pressures,flows)==PS_LIMIT);
    ps_pipe_edge overflow[]={{0,1,DBL_MAX},{1,2,DBL_MIN},{1,3,g}};
    CHECK(ps_pipe_network_solve(fixed,boundary,4,overflow,3,pressures,flows)==PS_NUMERIC && !memcmp(pressures,savedp,sizeof savedp));
    uint8_t large_fixed[16]={0};double large_boundary[16]={0},large_p[16],large_q[32];ps_pipe_edge large_edges[32];
    large_fixed[0]=large_fixed[15]=1;large_boundary[0]=150;
    for(unsigned e=0;e<15;e++){large_edges[e]=(ps_pipe_edge){e,e+1,1};large_edges[e+15]=large_edges[e];}
    large_edges[30]=large_edges[31]=(ps_pipe_edge){0,15,.5};
    CHECK(ps_pipe_network_solve(large_fixed,large_boundary,16,large_edges,32,large_p,large_q)==PS_OK);
    for(unsigned i=0;i<16;i++)CHECK(near(large_p[i],150-10.0*i));
    for(unsigned e=0;e<30;e++)CHECK(near(large_q[e],10));
    CHECK(near(large_q[30],75) && near(large_q[31],75));
    uint8_t separate_fixed[]={1,0,1,0};double separate_boundary[]={1e300,0,1e-300,0},separate_p[4],separate_q[3];
    ps_pipe_edge separate_edges[]={{0,1,1},{2,3,1},{1,3,1}};
    CHECK(ps_pipe_network_solve(separate_fixed,separate_boundary,4,separate_edges,2,separate_p,separate_q)==PS_OK);
    CHECK(near(separate_p[0],1e300) && near(separate_p[1],1e300) && near(separate_p[2],1e-300) && near(separate_p[3],1e-300));
    CHECK(separate_q[0]==0 && separate_q[1]==0);
    memcpy(separate_p,savedp,sizeof savedp);
    CHECK(ps_pipe_network_solve(separate_fixed,separate_boundary,4,separate_edges,3,separate_p,separate_q)==PS_NUMERIC && !memcmp(separate_p,savedp,sizeof savedp));
    double cells[]={0,1,0,0},next[4];
    CHECK(ps_transport_periodic_step(cells,4,1,0,1,1,next)==PS_OK && next[2]==1 && next[0]==0 && next[1]==0 && next[3]==0);
    CHECK(ps_transport_periodic_step(cells,4,-1,0,1,1,cells)==PS_OK && cells[0]==1 && cells[1]==0 && cells[2]==0 && cells[3]==0);
    CHECK(ps_transport_periodic_step(cells,4,0,.5,1,1,next)==PS_OK && next[0]==0 && next[1]==.5 && next[2]==0 && next[3]==.5);
    double constant[]={DBL_MAX,DBL_MAX,DBL_MAX};CHECK(ps_transport_periodic_step(constant,3,.4,.1,1,1,constant)==PS_OK);
    for(unsigned j=0;j<3;j++)CHECK(constant[j]==DBL_MAX);
    double tiny[]={0,1e300,0};CHECK(ps_transport_periodic_step(tiny,3,0,1e-200,1,1e-200,tiny)==PS_OK && near(tiny[0],1e-100) && near(tiny[2],1e-100));
    double profile[64],mass=0;
    for(unsigned j=0;j<64;j++){profile[j]=.2+.1*cos(2*PS_PI*j/64);mass+=profile[j];}
    for(unsigned n=0;n<300;n++)CHECK(ps_transport_periodic_step(profile,64,.1,.001,1.0/64,.01,profile)==PS_OK);
    double final_mass=0;for(unsigned j=0;j<64;j++){CHECK(profile[j]>=.1-1e-14 && profile[j]<=.3+1e-14);final_mass+=profile[j];}
    CHECK(near(mass,final_mass));
    double copy[4];memcpy(copy,next,sizeof copy);
    CHECK(ps_transport_periodic_step(cells,4,2,0,1,1,next)==PS_INVALID && !memcmp(next,copy,sizeof copy));
    CHECK(ps_transport_periodic_step(cells,4097,0,0,1,1,next)==PS_LIMIT);
    double *large=calloc(4096,sizeof(double));CHECK(large);
    large[0]=1;CHECK(ps_transport_periodic_step(large,4096,1,0,1,1,large)==PS_OK && large[1]==1 && large[0]==0);free(large);
    CHECK(ps_pipe_conductance(1e100,1e100,1e100,&g)==PS_OK && near(g,PS_PI/8*1e200));
    CHECK(ps_pipe_flow(1e-308,1e308,-1e308,&q)==PS_OK && near(q,2));
    CHECK(ps_pipe_power(1e-310,1e308,-1e308,&power)==PS_OK && near(power,4e306));
    CHECK(ps_reynolds_number(1e300,1e300,1e-300,1e300,&re)==PS_OK && near(re,1));
    pressure=17;CHECK(ps_pipe_conductance(0,1,1,&pressure)==PS_INVALID && pressure==17);
    CHECK(ps_pipe_conductance(DBL_MAX,1,1,&pressure)==PS_NUMERIC && pressure==17);
    CHECK(ps_hydrostatic_pressure(0,1,-1,1,&pressure)==PS_INVALID && pressure==17);
    puts("Fluid: Poiseuille/Reynolds, anchored network conservation, periodic mass/positivity, limits/aliasing and numeric extremes passed");return 0;
}
