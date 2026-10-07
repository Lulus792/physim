#include "physim/core.h"
#include <time.h>
#include <stdio.h>
static void slope(double t,const double *y,double *out,void *u){(void)t;(void)y;(void)u;out[0]=1;}
int main(void){double total=0;clock_t start=clock();for(unsigned i=0;i<200000;i++){double y=0;if(ps_ode_step(PS_RK4,slope,NULL,0,.001,&y,1)!=PS_OK)return 1;total+=y;}printf("cpu_seconds=%.6f total=%.17g\n",(double)(clock()-start)/CLOCKS_PER_SEC,total);return 0;}
