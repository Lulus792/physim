#ifndef PHYSIM_LANGUAGE_CONTACT_WORLD_H
#define PHYSIM_LANGUAGE_CONTACT_WORLD_H
/* Internal compiled-language values. Immutable owned snapshots share storage;
 * solve/reset always construct a new snapshot, so copies evolve independently. */
#include "contact_world.h"
#include "language_constraints.h"
typedef struct {
    ps_contact_world world;
    ps_body bodies[PS_CONTACT_GRAPH_MAX_BODIES];
    ps_contact_world_result result;
} psrt_world_storage;
typedef struct { psrt_array storage; } psrt_contact_world;
static inline void psrt_world_drop(void *value) {
    psrt_contact_world *w=value;psrt_array_destroy(&w->storage);memset(w,0,sizeof *w);
}
static inline ps_result psrt_world_copy(void *destination,const void *source) {
    const psrt_contact_world *s=source;psrt_contact_world copy=*s;
    ps_result status=psrt_array_clone(&s->storage,&copy.storage);
    if(status==PS_OK)*(psrt_contact_world *)destination=copy;
    return status;
}
static inline void psrt_world_keep(psrt_contact_world *value,psrt_site site) {
    psrt_contact_world copy;if(psrt_world_copy(&copy,value)!=PS_OK)psrt_fail(site,"ContactWorld ownership limit exceeded");*value=copy;
}
static inline const psrt_world_storage *psrt_world_data(psrt_contact_world value,psrt_site site) {
    if(!value.storage.block)psrt_fail(site,"Uninitialized ContactWorld");
    return psrt_array_data(&value.storage);
}
static inline psrt_contact_world psrt_world_store(ps_allocator allocator,const psrt_world_storage *storage,psrt_site site) {
    static const psrt_element_type element={sizeof(psrt_world_storage),NULL,NULL};
    psrt_contact_world value={0};
    if(psrt_array_init(&element,allocator,1,&value.storage)!=PS_OK ||
       psrt_array_replace(&value.storage,0,0,storage,1)!=PS_OK)
        psrt_fail(site,"ContactWorld memory budget or allocation exhausted");
    return value;
}
static inline psrt_contact_world psrt_world_make(ps_allocator allocator,double distance,double dot,double ratio,double warm,psrt_site site) {
    psrt_world_storage storage={0};ps_contact_world_settings settings={distance,dot,ratio,warm};
    if(ps_contact_world_init(&storage.world,sizeof storage.world,&settings)!=PS_OK)psrt_fail(site,"Invalid ContactWorld settings");
    return psrt_world_store(allocator,&storage,site);
}
static inline psrt_contact_world psrt_world_defaults(ps_allocator allocator,psrt_site site) {
    return psrt_world_make(allocator,PS_CONTACT_WORLD_DEFAULT.match_distance_m,PS_CONTACT_WORLD_DEFAULT.minimum_normal_dot,
        PS_CONTACT_WORLD_DEFAULT.maximum_dt_ratio,PS_CONTACT_WORLD_DEFAULT.warm_fraction,site);
}
static inline psrt_contact_world psrt_world_reset(ps_allocator allocator,psrt_contact_world value,psrt_site site) {
    ps_contact_world_settings s=psrt_world_data(value,site)->world.settings;
    return psrt_world_make(allocator,s.match_distance_m,s.minimum_normal_dot,s.maximum_dt_ratio,s.warm_fraction,site);
}
static inline ps_collider psrt_collider_make(int64_t id,int64_t body,ps_collider_shape shape,ps_vec3 size,ps_vec3 normal,psrt_site site) {
    if(id<=0 || (uint64_t)id>UINT32_MAX || body<0 || body>=PS_CONTACT_GRAPH_MAX_BODIES)
        psrt_fail(site,"Collider ID must be in 1..4294967295 and body index in 0..127");
    if(!isfinite(size.x) || !isfinite(size.y) || !isfinite(size.z) || !isfinite(normal.x) || !isfinite(normal.y) || !isfinite(normal.z) ||
       (shape==PS_COLLIDER_SPHERE && (size.x<=0 || size.y!=0 || size.z!=0)) ||
       (shape==PS_COLLIDER_BOX && (size.x<=0 || size.y<=0 || size.z<=0)) ||
       (shape==PS_COLLIDER_PLANE && fabs(hypot(hypot(normal.x,normal.y),normal.z)-1)>1e-8))
        psrt_fail(site,"Invalid Collider geometry");
    return (ps_collider){(uint32_t)id,(uint32_t)body,shape,size,normal};
}
static inline ps_collider psrt_collider_sphere(int64_t id,int64_t body,double radius,psrt_site site) {
    return psrt_collider_make(id,body,PS_COLLIDER_SPHERE,ps_v3(radius,0,0),ps_v3(0,0,0),site);
}
static inline ps_collider psrt_collider_box(int64_t id,int64_t body,ps_vec3 size,psrt_site site) {
    return psrt_collider_make(id,body,PS_COLLIDER_BOX,size,ps_v3(0,0,0),site);
}
static inline ps_collider psrt_collider_plane(int64_t id,int64_t body,ps_vec3 normal,psrt_site site) {
    return psrt_collider_make(id,body,PS_COLLIDER_PLANE,ps_v3(0,0,0),normal,site);
}
static inline int64_t psrt_collider_id(ps_collider c,psrt_site site){(void)site;return c.id;}
static inline int64_t psrt_collider_body(ps_collider c,psrt_site site){(void)site;return c.body;}
static inline int64_t psrt_collider_shape(ps_collider c,psrt_site site){(void)site;return c.shape;}
static inline ps_vec3 psrt_collider_size(ps_collider c,psrt_site site){(void)site;return c.size_m;}
static inline ps_vec3 psrt_collider_normal(ps_collider c,psrt_site site){(void)site;return c.plane_normal;}
static inline psrt_contact_world psrt_world_solve(ps_allocator allocator,psrt_contact_world previous,
    const ps_body *bodies,size_t body_count,const ps_collider *colliders,size_t collider_count,
    ps_contact_solver solver,double dt,psrt_site site) {
    if(body_count>PS_CONTACT_GRAPH_MAX_BODIES || collider_count>PS_CONTACT_GRAPH_MAX_BODIES)
        psrt_fail(site,"ContactWorld capacity exceeded (128 bodies/colliders, 512 contacts)");
    psrt_world_storage storage={0};storage.world=psrt_world_data(previous,site)->world;
    if(body_count)memcpy(storage.bodies,bodies,body_count*sizeof *bodies);
    ps_result result=ps_contact_world_solve(&storage.world,storage.bodies,body_count,colliders,collider_count,&solver,dt,&storage.result);
    if(result!=PS_OK)psrt_fail_code(site,result,"ContactWorld solve failed: invalid bodies/colliders, capacity or numeric range");
    return psrt_world_store(allocator,&storage,site);
}
#define PSRT_WORLD_COUNT(name,field) \
static inline int64_t psrt_world_##name(psrt_contact_world w,psrt_site site){return psrt_world_data(w,site)->field;}
PSRT_WORLD_COUNT(body_count,world.body_count)
PSRT_WORLD_COUNT(collider_count,world.model_count)
PSRT_WORLD_COUNT(contact_count,result.count)
PSRT_WORLD_COUNT(matched,result.matched)
PSRT_WORLD_COUNT(created,result.created)
PSRT_WORLD_COUNT(ended,result.ended)
PSRT_WORLD_COUNT(warmed,result.warmed)
#undef PSRT_WORLD_COUNT
static inline double psrt_world_normal_error(psrt_contact_world w,psrt_site site){return psrt_world_data(w,site)->result.solution.max_normal_error_m_s;}
static inline double psrt_world_projection_error(psrt_contact_world w,psrt_site site){return psrt_world_data(w,site)->result.solution.max_projection_error_m;}
static inline double psrt_world_dt(psrt_contact_world w,psrt_site site){return psrt_world_data(w,site)->world.dt_s;}
static inline ps_body psrt_world_body(psrt_contact_world w,int64_t index,psrt_site site) {
    const psrt_world_storage *s=psrt_world_data(w,site);
    if(index<0 || (uint64_t)index>=s->world.body_count)psrt_fail(site,"ContactWorld body index out of bounds");return s->bodies[index];
}
static inline psrt_array psrt_world_bodies(ps_allocator allocator,psrt_contact_world w,psrt_site site) {
    const psrt_world_storage *s=psrt_world_data(w,site);static const psrt_element_type element={sizeof(ps_body),NULL,NULL};psrt_array array;
    if(psrt_array_init(&element,allocator,0,&array)!=PS_OK || psrt_array_replace(&array,0,0,s->bodies,s->world.body_count)!=PS_OK)
        psrt_fail(site,"ContactWorld body array memory budget or allocation exhausted");return array;
}
static inline const ps_cached_contact *psrt_world_contact(psrt_contact_world w,int64_t index,psrt_site site) {
    const psrt_world_storage *s=psrt_world_data(w,site);
    if(index<0 || (uint64_t)index>=s->world.count)psrt_fail(site,"ContactWorld contact index out of bounds");return &s->world.contacts[index];
}
static inline int64_t psrt_world_id_a(psrt_contact_world w,int64_t i,psrt_site site){return psrt_world_contact(w,i,site)->id_a;}
static inline int64_t psrt_world_id_b(psrt_contact_world w,int64_t i,psrt_site site){return psrt_world_contact(w,i,site)->id_b;}
static inline ps_vec3 psrt_world_point(psrt_contact_world w,int64_t i,psrt_site site){return psrt_world_contact(w,i,site)->constraint.contact.point_m;}
static inline ps_vec3 psrt_world_normal(psrt_contact_world w,int64_t i,psrt_site site){return psrt_world_contact(w,i,site)->normal;}
static inline ps_vec3 psrt_world_local_a(psrt_contact_world w,int64_t i,psrt_site site){return psrt_world_contact(w,i,site)->local_a_m;}
static inline ps_vec3 psrt_world_local_b(psrt_contact_world w,int64_t i,psrt_site site){return psrt_world_contact(w,i,site)->local_b_m;}
static inline ps_vec3 psrt_world_impulse(psrt_contact_world w,int64_t i,psrt_site site){return psrt_world_contact(w,i,site)->impulse_on_a_ns;}
static inline double psrt_world_penetration(psrt_contact_world w,int64_t i,psrt_site site){return psrt_world_contact(w,i,site)->constraint.contact.penetration_m;}
#endif
