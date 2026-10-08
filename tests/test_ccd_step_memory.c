#define PSRT_MODULE
#define PSRT_SOURCE "ccd-memory.phys"
#include "physim/language_sdk.h"
#include "test_allocator.h"
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"CCD memory %d: %s\n",__LINE__,#x);return 1;}}while(0)
static const ps_vec3 vertices[]={{-1,-1,-1},{1,-1,-1},{1,1,-1},{-1,1,-1},{-1,-1,1},{1,-1,1},{1,1,1},{-1,1,1}};
static const int64_t indices[]={0,2,1,0,3,2,4,5,6,4,6,7,0,1,5,0,5,4,3,7,6,3,6,2,0,4,7,0,7,3,1,2,6,1,6,5};
static test_allocator domain;
static psrt_trap trap;static ps_diagnostic diagnostic;static char error[2048];
static void begin(void){memset(&trap,0,sizeof trap);memset(&diagnostic,0,sizeof diagnostic);trap.error=error;trap.capacity=sizeof error;trap.diagnostic=&diagnostic;trap.operation="ccd.step";psrt_current=&trap;}
int main(void){
    psrt_site site={"ccd-memory.phys",3,4};
    for(volatile unsigned failure=1;failure<=2;failure++) {
        memset(&domain,0,sizeof domain);domain.fail_on=failure;begin();
        if(!setjmp(trap.jump)){(void)psrt_ccd_model_convex(test_domain(&domain),2,1,vertices,8,indices,36,site);CHECK(false);}
        psrt_current=NULL;CHECK(trap.failure_code==PS_MEMORY && domain.live_bytes==0 && domain.live_blocks==0 && !domain.invalid);
    }
    memset(&domain,0,sizeof domain);ps_allocator allocator=test_domain(&domain);begin();
    CHECK(!setjmp(trap.jump));
    psrt_ccd_model model=psrt_ccd_model_convex(allocator,2,1,vertices,8,indices,36,site),copy={0};
    CHECK(psrt_ccd_model_copy(&copy,&model)==PS_OK && domain.live_blocks==2);
    psrt_ccd_model_drop(&copy);CHECK(domain.live_blocks==2);
    psrt_ccd_model untouched;memset(&untouched,0x5a,sizeof untouched);copy=untouched;
    model.vertices.block->value.references=SIZE_MAX;
    CHECK(psrt_ccd_model_copy(&copy,&model)==PS_LIMIT && !memcmp(&copy,&untouched,sizeof copy) && model.vertices.block->value.references==SIZE_MAX);
    model.vertices.block->value.references=1;model.indices.block->value.references=SIZE_MAX;
    CHECK(psrt_ccd_model_copy(&copy,&model)==PS_LIMIT && model.vertices.block->value.references==1 && !memcmp(&copy,&untouched,sizeof copy));
    model.indices.block->value.references=1;
    ps_body bodies[2];CHECK(ps_body_sphere(1,.5,&bodies[0])==PS_OK && ps_body_with_inertia(0,ps_v3(0,0,0),&bodies[1])==PS_OK);
    bodies[0].position_m.x=-4;bodies[0].velocity_m_s.x=10;ps_body saved[2];memcpy(saved,bodies,sizeof bodies);
    psrt_ccd_model models[2]={psrt_ccd_model_make(allocator,(ps_collider){1,0,PS_COLLIDER_SPHERE,{.5,0,0},{0,0,0}},site),model};
    ps_contact_solver solver=PS_CONTACT_SOLVER_DEFAULT;solver.restitution=1;solver.friction=0;solver.bounce_threshold_m_s=0;
    size_t live=domain.live_bytes;domain.fail_on=domain.attempts+1;begin();
    if(!setjmp(trap.jump)){(void)psrt_ccd_step(allocator,solver,bodies,2,models,2,NULL,0,NULL,0,.5,PS_CCD_DEFAULT,64,1e-6,site);CHECK(false);}
    psrt_current=NULL;CHECK(trap.failure_code==PS_MEMORY && domain.live_bytes==live && !memcmp(saved,bodies,sizeof bodies));
    domain.fail_on=0;begin();CHECK(!setjmp(trap.jump));
    psrt_ccd_result result=psrt_ccd_step(allocator,solver,bodies,2,models,2,NULL,0,NULL,0,.5,PS_CCD_DEFAULT,64,1e-6,site),kept={0};
    CHECK(psrt_ccd_result_copy(&kept,&result)==PS_OK);psrt_ccd_result_drop(&result);
    CHECK(psrt_ccd_body(kept,0,site).velocity_m_s.x<0 && !memcmp(saved,bodies,sizeof bodies));
    psrt_array output=psrt_ccd_bodies(allocator,kept,site);CHECK(psrt_array_count(&output)==2);
    psrt_array_destroy(&output);psrt_ccd_result_drop(&kept);psrt_ccd_model_drop(&model);
    psrt_current=NULL;CHECK(domain.live_bytes==0 && domain.live_blocks==0 && !domain.invalid);
    puts("CCD ownership: allocation failures, reference saturation, atomic inputs and independent result snapshots passed");return 0;
}
