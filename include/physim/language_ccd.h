#ifndef PHYSIM_LANGUAGE_CCD_H
#define PHYSIM_LANGUAGE_CCD_H
#include "language_contact_world.h"
#include "language_collision.h"
typedef struct {ps_collider collider;psrt_array vertices,indices;} psrt_ccd_model;
typedef struct {ps_body bodies[PS_CONTACT_GRAPH_MAX_BODIES];ps_ccd_step_result report;size_t count;} psrt_ccd_storage;
typedef struct {psrt_array storage;} psrt_ccd_result;
static inline void psrt_ccd_model_drop(void *value){psrt_ccd_model *m=value;psrt_array_destroy(&m->vertices);psrt_array_destroy(&m->indices);memset(m,0,sizeof *m);}
static inline ps_result psrt_ccd_model_copy(void *destination,const void *source){
    const psrt_ccd_model *s=source;psrt_ccd_model c=*s;ps_result result=PS_OK;
    if(s->vertices.type){result=psrt_array_clone(&s->vertices,&c.vertices);if(result!=PS_OK)return result;}
    if(result==PS_OK && s->indices.type)result=psrt_array_clone(&s->indices,&c.indices);
    if(result!=PS_OK){if(s->vertices.type)psrt_array_destroy(&c.vertices);return result;}
    *(psrt_ccd_model *)destination=c;return PS_OK;
}
static inline void psrt_ccd_model_keep(psrt_ccd_model *value,psrt_site site){psrt_ccd_model copy;ps_result result=psrt_ccd_model_copy(&copy,value);if(result!=PS_OK)psrt_fail_code(site,result,"CCD collider copy failed");*value=copy;}
static inline psrt_ccd_model psrt_ccd_model_make(ps_allocator allocator,ps_collider collider,psrt_site site){
    (void)allocator;
    if(!collider.id || collider.body>=PS_CONTACT_GRAPH_MAX_BODIES || collider.shape<PS_COLLIDER_SPHERE || collider.shape>PS_COLLIDER_PLANE)
        psrt_fail_code(site,PS_INVALID,"Invalid CCD collider");
    psrt_ccd_model model={0};model.collider=collider;return model;
}
static inline psrt_ccd_model psrt_ccd_model_convex(ps_allocator allocator,int64_t id,int64_t body,
    const ps_vec3 *vertices,size_t count,const int64_t *indices,size_t index_count,psrt_site site){
    if(id<=0 || (uint64_t)id>UINT32_MAX || body<0 || body>=PS_CONTACT_GRAPH_MAX_BODIES)
        psrt_fail_code(site,PS_INVALID,"Invalid convex CCD collider identity");
    uint32_t triangles[PS_CONVEX_MAX_TRIANGLES][3];ps_convex_mesh mesh=psrt_convex_mesh(vertices,count,indices,index_count,triangles,site);
    ps_result result=ps_convex_validate(&mesh);if(result!=PS_OK)psrt_fail_code(site,result,"Invalid convex CCD collider mesh");
    psrt_ccd_model model={0};model.collider=(ps_collider){(uint32_t)id,(uint32_t)body,PS_COLLIDER_CONVEX,{0,0,0},{0,0,0}};
    static const psrt_element_type vtype={sizeof(ps_vec3),NULL,NULL},itype={sizeof(int64_t),NULL,NULL};
    result=psrt_array_init(&vtype,allocator,PS_CONVEX_MAX_VERTICES,&model.vertices);
    if(result==PS_OK)result=psrt_array_replace(&model.vertices,0,0,vertices,count);
    if(result==PS_OK)result=psrt_array_init(&itype,allocator,3*PS_CONVEX_MAX_TRIANGLES,&model.indices);
    if(result==PS_OK)result=psrt_array_replace(&model.indices,0,0,indices,index_count);
    if(result!=PS_OK){psrt_ccd_model_drop(&model);psrt_fail_code(site,result,"Convex CCD collider memory exhausted");}
    return model;
}
static inline void psrt_ccd_result_drop(void *value){psrt_ccd_result *r=value;psrt_array_destroy(&r->storage);memset(r,0,sizeof *r);}
static inline ps_result psrt_ccd_result_copy(void *destination,const void *source){const psrt_ccd_result *s=source;psrt_ccd_result r=*s;ps_result result=psrt_array_clone(&s->storage,&r.storage);if(result==PS_OK)*(psrt_ccd_result *)destination=r;return result;}
static inline void psrt_ccd_result_keep(psrt_ccd_result *value,psrt_site site){psrt_ccd_result copy;ps_result result=psrt_ccd_result_copy(&copy,value);if(result!=PS_OK)psrt_fail_code(site,result,"CCD result copy failed");*value=copy;}
static inline const psrt_ccd_storage *psrt_ccd_data(psrt_ccd_result r,psrt_site site){if(!r.storage.block)psrt_fail_code(site,PS_INVALID,"Uninitialized CCD result");return psrt_array_data(&r.storage);}
static inline psrt_ccd_result psrt_ccd_step(ps_allocator allocator,ps_contact_solver solver,const ps_body *bodies,size_t count,
    const psrt_ccd_model *input,size_t model_count,const ps_vec3 *forces,size_t force_count,const ps_vec3 *torques,size_t torque_count,
    double dt,ps_ccd_settings ccd,int64_t max_events,double offset,psrt_site site){
    if(count>PS_CONTACT_GRAPH_MAX_BODIES || model_count>PS_CONTACT_GRAPH_MAX_BODIES || max_events>PS_CCD_MAX_EVENTS)
        psrt_fail_code(site,PS_LIMIT,"CCD step capacity exceeded");
    if(max_events<=0 || (force_count && force_count!=count) || (torque_count && torque_count!=count))
        psrt_fail_code(site,PS_INVALID,"CCD step force/torque count or event budget invalid");
    psrt_ccd_storage storage={0};storage.count=count;if(count)memcpy(storage.bodies,bodies,count*sizeof *bodies);
    ps_ccd_collider models[PS_CONTACT_GRAPH_MAX_BODIES];ps_convex_mesh meshes[PS_CONTACT_GRAPH_MAX_BODIES];
    uint32_t triangles[PS_CONTACT_GRAPH_MAX_BODIES][PS_CONVEX_MAX_TRIANGLES][3];
    for(size_t i=0;i<model_count;i++){
        models[i]=(ps_ccd_collider){input[i].collider,NULL};
        if(input[i].collider.shape==PS_COLLIDER_CONVEX){
            meshes[i]=psrt_convex_mesh(psrt_array_data(&input[i].vertices),psrt_array_count(&input[i].vertices),
                psrt_array_data(&input[i].indices),psrt_array_count(&input[i].indices),triangles[i],site);models[i].mesh=&meshes[i];
        }
    }
    ps_ccd_step_options options={ccd,(uint32_t)max_events,offset};
    ps_result result=ps_ccd_step(storage.bodies,count,models,model_count,force_count?forces:NULL,torque_count?torques:NULL,dt,&solver,&options,&storage.report);
    if(result!=PS_OK)psrt_fail_code(site,result,"Continuous event step unresolved or invalid");
    static const psrt_element_type element={sizeof(psrt_ccd_storage),NULL,NULL};psrt_ccd_result value={0};
    result=psrt_array_init(&element,allocator,1,&value.storage);
    if(result==PS_OK)result=psrt_array_replace(&value.storage,0,0,&storage,1);
    if(result!=PS_OK){psrt_ccd_result_drop(&value);psrt_fail_code(site,result,"CCD result memory exhausted");}
    return value;
}
static inline ps_body psrt_ccd_body(psrt_ccd_result value,int64_t index,psrt_site site){const psrt_ccd_storage *s=psrt_ccd_data(value,site);if(index<0 || (uint64_t)index>=s->count)psrt_fail_code(site,PS_INVALID,"CCD body index out of bounds");return s->bodies[index];}
static inline psrt_array psrt_ccd_bodies(ps_allocator allocator,psrt_ccd_result value,psrt_site site){
    const psrt_ccd_storage *s=psrt_ccd_data(value,site);static const psrt_element_type element={sizeof(ps_body),NULL,NULL};psrt_array r={0};
    ps_result result=psrt_array_init(&element,allocator,PS_CONTACT_GRAPH_MAX_BODIES,&r);
    if(result==PS_OK)result=psrt_array_replace(&r,0,0,s->bodies,s->count);
    if(result!=PS_OK){psrt_array_destroy(&r);psrt_fail_code(site,result,"CCD body array memory exhausted");}return r;
}
#define PSRT_CCD_FIELD(name,type,field) static inline type psrt_ccd_##name(psrt_ccd_result value,psrt_site site){return psrt_ccd_data(value,site)->field;}
PSRT_CCD_FIELD(events,int64_t,report.events)
PSRT_CCD_FIELD(contacts,int64_t,report.contacts)
PSRT_CCD_FIELD(elapsed,double,report.elapsed_s)
PSRT_CCD_FIELD(normal_error,double,report.max_normal_error_m_s)
PSRT_CCD_FIELD(projection_error,double,report.max_projection_error_m)
#endif
