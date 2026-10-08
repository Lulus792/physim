#include "physim/contact_world.h"
#include <math.h>
#include <string.h>
const ps_contact_world_settings PS_CONTACT_WORLD_DEFAULT={.02,.95,4,1};
static bool finite3(ps_vec3 v){return isfinite(v.x) && isfinite(v.y) && isfinite(v.z);}
static double length(ps_vec3 v){return hypot(hypot(v.x,v.y),v.z);}
static bool unit(ps_vec3 v){return finite3(v) && fabs(length(v)-1)<=1e-8;}
static bool zero(ps_vec3 v){return v.x==0 && v.y==0 && v.z==0;}
static bool settings_valid(const ps_contact_world_settings *s) {
    return s && isfinite(s->match_distance_m) && s->match_distance_m>=0 &&
        isfinite(s->minimum_normal_dot) && s->minimum_normal_dot>=0 && s->minimum_normal_dot<=1 &&
        isfinite(s->maximum_dt_ratio) && s->maximum_dt_ratio>=1 &&
        isfinite(s->warm_fraction) && s->warm_fraction>=0 && s->warm_fraction<=1;
}
static bool collider_valid(const ps_collider *c,size_t bodies,double mass) {
    if(!c->id || c->body>=bodies || !finite3(c->size_m) || !finite3(c->plane_normal))return false;
    if(c->shape==PS_COLLIDER_PLANE)return mass==0 && zero(c->size_m) && unit(c->plane_normal);
    if(!zero(c->plane_normal))return false;
    if(c->shape==PS_COLLIDER_SPHERE)return c->size_m.x>0 && c->size_m.y==0 && c->size_m.z==0;
    return c->shape==PS_COLLIDER_BOX && c->size_m.x>0 && c->size_m.y>0 && c->size_m.z>0;
}
static bool world_valid(const ps_contact_world *w) {
    if(!w || w->struct_size<sizeof *w || w->version!=PS_CONTACT_WORLD_VERSION || !settings_valid(&w->settings) ||
       w->count>PS_CONTACT_GRAPH_MAX_CONTACTS || w->body_count>PS_CONTACT_GRAPH_MAX_BODIES || w->model_count>PS_CONTACT_GRAPH_MAX_BODIES ||
       !isfinite(w->dt_s) || (w->count?w->dt_s<=0:w->dt_s<0))return false;
    for(uint32_t i=0;i<w->model_count;i++) {
        const ps_contact_world_model *m=&w->models[i];
        if(!isfinite(m->mass_kg) || m->mass_kg<0 || !finite3(m->inertia_kg_m2) ||
           !collider_valid(&m->collider,w->body_count,m->mass_kg) || (i && w->models[i-1].collider.id>=m->collider.id))return false;
        if(m->mass_kg==0?!zero(m->inertia_kg_m2):(m->inertia_kg_m2.x<=0 || m->inertia_kg_m2.y<=0 || m->inertia_kg_m2.z<=0))return false;
        for(uint32_t j=0;j<i;j++)if(w->models[j].collider.body==m->collider.body)return false;
    }
    for(uint32_t i=0;i<w->count;i++) {
        const ps_cached_contact *c=&w->contacts[i];
        if(!c->id_a || c->id_a>=c->id_b || !finite3(c->local_a_m) || !finite3(c->local_b_m) || !unit(c->normal) ||
           !finite3(c->impulse_on_a_ns) || c->constraint.a>=w->body_count || c->constraint.b>=w->body_count || c->constraint.a==c->constraint.b ||
           !finite3(c->constraint.contact.point_m) || !unit(c->constraint.contact.normal) ||
           !isfinite(c->constraint.contact.penetration_m) || c->constraint.contact.penetration_m<0 ||
           memcmp(&c->normal,&c->constraint.contact.normal,sizeof(ps_vec3)))return false;
        bool a=false,b=false;
        for(uint32_t j=0;j<w->model_count;j++) {
            a |= w->models[j].collider.id==c->id_a && w->models[j].collider.body==c->constraint.a;
            b |= w->models[j].collider.id==c->id_b && w->models[j].collider.body==c->constraint.b;
        }
        if(!a || !b)return false;
    }
    return true;
}
ps_result ps_contact_world_init(ps_contact_world *w,size_t size,const ps_contact_world_settings *settings) {
    if(!w)return PS_INVALID;
    if(size<sizeof *w)return PS_VERSION;
    settings=settings?settings:&PS_CONTACT_WORLD_DEFAULT;if(!settings_valid(settings))return PS_INVALID;
    ps_contact_world_settings copy=*settings;memset(w,0,sizeof *w);w->struct_size=sizeof *w;w->version=PS_CONTACT_WORLD_VERSION;w->settings=copy;return PS_OK;
}
ps_result ps_contact_world_reset(ps_contact_world *w) {
    if(!w)return PS_INVALID;
    if(w->struct_size<sizeof *w || w->version!=PS_CONTACT_WORLD_VERSION)return PS_VERSION;
    if(!world_valid(w))return PS_INVALID;
    return ps_contact_world_init(w,sizeof *w,&w->settings);
}
static ps_quat rotation(const ps_body *body) {
    ps_quat q=body->orientation;double norm=hypot(hypot(q.x,q.y),hypot(q.z,q.w));
    q.x/=norm;q.y/=norm;q.z/=norm;q.w/=norm;return q;
}
static ps_vec3 local_anchor(const ps_body *body,ps_vec3 point) {
    ps_quat q=rotation(body);q.x=-q.x;q.y=-q.y;q.z=-q.z;
    return ps_quat_rotate(q,ps_vsub(point,body->position_m));
}
static const ps_contact_world_model *model_find(const ps_contact_world *w,uint32_t id) {
    for(uint32_t i=0;i<w->model_count;i++)if(w->models[i].collider.id==id)return &w->models[i];
    return NULL;
}
static bool same_model(const ps_contact_world_model *old,const ps_contact_world_model *now) {
    return old && old->collider.shape==now->collider.shape && old->mass_kg==now->mass_kg &&
        !memcmp(&old->collider.size_m,&now->collider.size_m,sizeof(ps_vec3)) &&
        !memcmp(&old->collider.plane_normal,&now->collider.plane_normal,sizeof(ps_vec3)) &&
        !memcmp(&old->inertia_kg_m2,&now->inertia_kg_m2,sizeof(ps_vec3));
}
static ps_result manifold(const ps_contact_world_model *ma,const ps_contact_world_model *mb,
    const ps_body *bodies,ps_contact_manifold *out) {
    const ps_collider *a=&ma->collider,*b=&mb->collider;bool reverse=false,touching=false;
    if(a->shape==PS_COLLIDER_PLANE || (a->shape==PS_COLLIDER_BOX && b->shape==PS_COLLIDER_SPHERE)) {
        const ps_collider *swap=a;a=b;b=swap;reverse=true;
    }
    const ps_body *ba=&bodies[a->body],*bb=&bodies[b->body];ps_result result;
    if(b->shape==PS_COLLIDER_PLANE) {
        ps_vec3 normal=ps_quat_rotate(rotation(bb),b->plane_normal);double norm=length(normal);
        if(!isfinite(norm) || norm==0)return PS_NUMERIC;
        normal=ps_vscale(normal,1/norm);
        if(a->shape==PS_COLLIDER_BOX)result=ps_contacts_box_plane(ba,a->size_m,bb->position_m,normal,out);
        else {result=ps_contact_sphere_plane(ba,a->size_m.x,bb->position_m,normal,&out->points[0],&touching);out->count=touching?1:0;}
    } else if(a->shape==PS_COLLIDER_BOX)result=ps_contacts_boxes(ba,a->size_m,bb,b->size_m,out);
    else if(b->shape==PS_COLLIDER_BOX){result=ps_contact_sphere_box(ba,a->size_m.x,bb,b->size_m,&out->points[0],&touching);out->count=touching?1:0;}
    else {result=ps_contact_spheres(ba,a->size_m.x,bb,b->size_m.x,&out->points[0],&touching);out->count=touching?1:0;}
    if(result==PS_OK && reverse)for(uint32_t i=0;i<out->count;i++)out->points[i].normal=ps_vscale(out->points[i].normal,-1);
    return result;
}
static ps_result add_pair(ps_contact_world *next,const ps_contact_world_model *a,const ps_contact_world_model *b,const ps_body *bodies) {
    if(a->mass_kg==0 && b->mass_kg==0)return PS_OK;
    ps_contact_manifold contacts={0};ps_result result=manifold(a,b,bodies,&contacts);if(result!=PS_OK)return result;
    if(contacts.count>PS_CONTACT_GRAPH_MAX_CONTACTS-next->count)return PS_LIMIT;
    for(uint32_t i=0;i<contacts.count;i++) {
        ps_cached_contact *c=&next->contacts[next->count++];c->id_a=a->collider.id;c->id_b=b->collider.id;
        c->constraint=(ps_contact_constraint){a->collider.body,b->collider.body,contacts.points[i]};c->normal=contacts.points[i].normal;
        c->local_a_m=local_anchor(&bodies[a->collider.body],contacts.points[i].point_m);
        c->local_b_m=local_anchor(&bodies[b->collider.body],contacts.points[i].point_m);
        if(!finite3(c->local_a_m) || !finite3(c->local_b_m))return PS_NUMERIC;
    }
    return PS_OK;
}
static int contact_order(const ps_cached_contact *a,const ps_cached_contact *b) {
    if(a->id_a!=b->id_a)return a->id_a<b->id_a?-1:1;
    return a->id_b<b->id_b?-1:a->id_b>b->id_b;
}
ps_result ps_contact_world_solve(ps_contact_world *world,ps_body *bodies,size_t body_count,
    const ps_collider *colliders,size_t count,const ps_contact_solver *solver,double dt,ps_contact_world_result *out) {
    if(!world)return PS_INVALID;
    if(world->struct_size<sizeof *world || world->version!=PS_CONTACT_WORLD_VERSION)return PS_VERSION;
    if(!world_valid(world) || (!bodies && body_count) || (!colliders && count) || !isfinite(dt) || dt<=0)return PS_INVALID;
    if(body_count>PS_CONTACT_GRAPH_MAX_BODIES || count>PS_CONTACT_GRAPH_MAX_BODIES)return PS_LIMIT;
    for(size_t i=0;i<body_count;i++)if(ps_body_validate(&bodies[i])!=PS_OK)return PS_INVALID;
    ps_contact_world next;ps_result result=ps_contact_world_init(&next,sizeof next,&world->settings);if(result!=PS_OK)return result;
    next.model_count=(uint32_t)count;next.body_count=(uint32_t)body_count;next.dt_s=dt;
    for(size_t i=0;i<count;i++) {
        if(colliders[i].body>=body_count || !collider_valid(&colliders[i],body_count,bodies[colliders[i].body].mass_kg))return PS_INVALID;
        for(size_t j=0;j<i;j++)if(colliders[i].id==colliders[j].id || colliders[i].body==colliders[j].body)return PS_INVALID;
        next.models[i]=(ps_contact_world_model){colliders[i],bodies[colliders[i].body].mass_kg,bodies[colliders[i].body].inertia_kg_m2};
        for(size_t j=i;j && next.models[j].collider.id<next.models[j-1].collider.id;j--) {
            ps_contact_world_model swap=next.models[j];next.models[j]=next.models[j-1];next.models[j-1]=swap;
        }
    }
    ps_aabb bounds[PS_CONTACT_GRAPH_MAX_BODIES];uint32_t finite_models[PS_CONTACT_GRAPH_MAX_BODIES];size_t finite_count=0;
    for(size_t i=0;i<count;i++) {
        const ps_collider *c=&next.models[i].collider;
        if(c->shape==PS_COLLIDER_PLANE)continue;
        result=c->shape==PS_COLLIDER_SPHERE?ps_aabb_sphere(&bodies[c->body],c->size_m.x,&bounds[finite_count]):ps_aabb_box(&bodies[c->body],c->size_m,&bounds[finite_count]);
        if(result!=PS_OK)return result;
        finite_models[finite_count++]=(uint32_t)i;
    }
    ps_collision_pair pairs[PS_CONTACT_GRAPH_MAX_BODIES*(PS_CONTACT_GRAPH_MAX_BODIES-1)/2];size_t pair_count=0;
    result=ps_broad_phase(bounds,finite_count,pairs,sizeof pairs/sizeof *pairs,&pair_count);if(result!=PS_OK)return result;
    for(size_t i=0;i<pair_count;i++) {
        result=add_pair(&next,&next.models[finite_models[pairs[i].a]],&next.models[finite_models[pairs[i].b]],bodies);if(result!=PS_OK)return result;
    }
    for(size_t i=0;i<count;i++)if(next.models[i].collider.shape==PS_COLLIDER_PLANE)
        for(size_t j=0;j<finite_count;j++) {
            uint32_t k=finite_models[j];result=add_pair(&next,&next.models[i<k?i:k],&next.models[i<k?k:i],bodies);if(result!=PS_OK)return result;
        }
    for(uint32_t i=1;i<next.count;i++)for(uint32_t j=i;j && contact_order(&next.contacts[j],&next.contacts[j-1])<0;j--) {
        ps_cached_contact swap=next.contacts[j];next.contacts[j]=next.contacts[j-1];next.contacts[j-1]=swap;
    }
    ps_contact_constraint constraints[PS_CONTACT_GRAPH_MAX_CONTACTS];ps_vec3 initial[PS_CONTACT_GRAPH_MAX_CONTACTS]={0};bool used[PS_CONTACT_GRAPH_MAX_CONTACTS]={0};
    ps_contact_world_result report={0};report.count=next.count;double ratio=world->dt_s>0?dt/world->dt_s:0;
    bool scale_valid=isfinite(ratio) && ratio>=1/world->settings.maximum_dt_ratio && ratio<=world->settings.maximum_dt_ratio;
    for(uint32_t i=0;i<next.count;i++) {
        ps_cached_contact *c=&next.contacts[i];constraints[i]=c->constraint;
        if(!same_model(model_find(world,c->id_a),model_find(&next,c->id_a)) || !same_model(model_find(world,c->id_b),model_find(&next,c->id_b)))continue;
        size_t best=SIZE_MAX;double nearest=INFINITY;
        for(uint32_t j=0;j<world->count;j++) {
            const ps_cached_contact *old=&world->contacts[j];if(used[j] || old->id_a!=c->id_a || old->id_b!=c->id_b || ps_vdot(old->normal,c->normal)<world->settings.minimum_normal_dot)continue;
            double da=length(ps_vsub(old->local_a_m,c->local_a_m)),db=length(ps_vsub(old->local_b_m,c->local_b_m));
            double distance=fmax(da,db);if(distance<=world->settings.match_distance_m && distance<nearest){nearest=distance;best=j;}
        }
        if(best==SIZE_MAX)continue;
        used[best]=true;report.matched++;
        if(scale_valid && world->settings.warm_fraction>0) {
            ps_vec3 va,vb;
            result=ps_body_point_velocity(&bodies[c->constraint.a],c->constraint.contact.point_m,&va);if(result!=PS_OK)return result;
            result=ps_body_point_velocity(&bodies[c->constraint.b],c->constraint.contact.point_m,&vb);if(result!=PS_OK)return result;
            /* Invalid settings are rejected by the graph solver; do not dereference NULL. */
            if(!solver)return PS_INVALID;
            if(ps_vdot(ps_vsub(vb,va),c->normal)<=solver->bounce_threshold_m_s) {
                initial[i]=ps_vscale(world->contacts[best].impulse_on_a_ns,ratio*world->settings.warm_fraction);
                if(!finite3(initial[i]))return PS_NUMERIC;
                if(!zero(initial[i]))report.warmed++;
            }
        }
    }
    ps_body working[PS_CONTACT_GRAPH_MAX_BODIES];if(body_count)memcpy(working,bodies,body_count*sizeof *working);
    result=ps_contacts_resolve_graph_warm(working,body_count,constraints,next.count,solver,initial,&report.solution);if(result!=PS_OK)return result;
    for(uint32_t i=0;i<next.count;i++)next.contacts[i].impulse_on_a_ns=report.solution.impulse_on_a_ns[i];
    report.created=next.count-report.matched;report.ended=world->count-report.matched;
    if(body_count)memcpy(bodies,working,body_count*sizeof *working);
    *world=next;
    if(out)*out=report;
    return PS_OK;
}

const ps_ccd_step_options PS_CCD_STEP_DEFAULT={{1e-8,4096},128,1e-6};
static const uint32_t event_box_triangles[][3]={{0,2,1},{0,3,2},{4,5,6},{4,6,7},{0,1,5},{0,5,4},
    {3,7,6},{3,6,2},{0,4,7},{0,7,3},{1,2,6},{1,6,5}};
static void event_box(ps_vec3 size,ps_vec3 vertices[8],ps_convex_mesh *mesh) {
    static const int sign[8][3]={{-1,-1,-1},{1,-1,-1},{1,1,-1},{-1,1,-1},{-1,-1,1},{1,-1,1},{1,1,1},{-1,1,1}};
    for(unsigned i=0;i<8;i++)vertices[i]=ps_v3(.5*size.x*sign[i][0],.5*size.y*sign[i][1],.5*size.z*sign[i][2]);
    *mesh=(ps_convex_mesh){vertices,8,event_box_triangles,12};
}
static ps_result event_model_valid(const ps_ccd_collider *model,const ps_body *bodies,size_t count) {
    const ps_collider *c=&model->collider;
    if(c->body>=count)return PS_INVALID;
    if(c->shape!=PS_COLLIDER_CONVEX)
        return !model->mesh && collider_valid(c,count,bodies[c->body].mass_kg)?PS_OK:PS_INVALID;
    if(!c->id || !zero(c->size_m) || !zero(c->plane_normal))return PS_INVALID;
    return ps_convex_validate(model->mesh);
}
static ps_rigid_motion event_motion(const ps_body *body,double dt) {
    if(!body->mass_kg)return (ps_rigid_motion){0};
    return (ps_rigid_motion){ps_vscale(body->velocity_m_s,dt),ps_vscale(body->angular_velocity_rad_s,dt),ps_v3(0,0,0)};
}
static ps_result event_query(const ps_ccd_collider *ma,const ps_convex_mesh *mesh_a,
    const ps_ccd_collider *mb,const ps_convex_mesh *mesh_b,const ps_body *bodies,double dt,
    const ps_ccd_settings *settings,ps_sweep_hit *hit,bool *touching) {
    const ps_collider *a=&ma->collider,*b=&mb->collider;bool reverse=false;
    if(a->shape==PS_COLLIDER_PLANE || (a->shape!=PS_COLLIDER_SPHERE && b->shape==PS_COLLIDER_SPHERE)) {
        const ps_collider *swap=a;a=b;b=swap;const ps_convex_mesh *m=mesh_a;mesh_a=mesh_b;mesh_b=m;reverse=true;
    }
    const ps_body *ba=&bodies[a->body],*bb=&bodies[b->body];
    ps_rigid_motion da=event_motion(ba,dt),db=event_motion(bb,dt);ps_result result;
    if(b->shape==PS_COLLIDER_PLANE) {
        ps_vec3 n=ps_quat_rotate(rotation(bb),b->plane_normal);n=ps_vscale(n,1/length(n));
        if(a->shape==PS_COLLIDER_SPHERE)result=ps_sweep_sphere_plane(ba,a->size_m.x,da.translation_m,bb->position_m,n,hit,touching);
        else result=ps_sweep_convex_plane_motion(ba,mesh_a,da,bb->position_m,n,settings,hit,touching);
    } else if(a->shape==PS_COLLIDER_SPHERE && b->shape==PS_COLLIDER_SPHERE)
        result=ps_sweep_spheres(ba,a->size_m.x,da.translation_m,bb,b->size_m.x,db.translation_m,hit,touching);
    else if(a->shape==PS_COLLIDER_SPHERE)
        result=ps_sweep_sphere_convex_motion(ba,a->size_m.x,da,bb,mesh_b,db,settings,hit,touching);
    else result=ps_sweep_convexes_motion(ba,mesh_a,da,bb,mesh_b,db,settings,hit,touching);
    if(result==PS_OK && *touching && reverse)hit->contact.normal=ps_vscale(hit->contact.normal,-1);
    return result;
}
static ps_result event_snapshot(const ps_ccd_collider *ma,const ps_convex_mesh *mesh_a,
    const ps_ccd_collider *mb,const ps_convex_mesh *mesh_b,const ps_body *bodies,double offset,
    const ps_ccd_settings *settings,ps_contact *contact,bool *touching) {
    const ps_collider *a=&ma->collider,*b=&mb->collider;bool reverse=false;
    if(a->shape==PS_COLLIDER_PLANE || (a->shape!=PS_COLLIDER_SPHERE && b->shape==PS_COLLIDER_SPHERE)) {
        const ps_collider *swap=a;a=b;b=swap;const ps_convex_mesh *m=mesh_a;mesh_a=mesh_b;mesh_b=m;reverse=true;
    }
    const ps_body *ba=&bodies[a->body],*bb=&bodies[b->body];ps_result result;
    if(a->shape==PS_COLLIDER_SPHERE && b->shape==PS_COLLIDER_SPHERE)
        result=ps_contact_spheres(ba,a->size_m.x+offset/2,bb,b->size_m.x+offset/2,contact,touching);
    else if(a->shape==PS_COLLIDER_SPHERE && b->shape==PS_COLLIDER_PLANE) {
        ps_vec3 n=ps_quat_rotate(rotation(bb),b->plane_normal);n=ps_vscale(n,1/length(n));
        result=ps_contact_sphere_plane(ba,a->size_m.x+offset,bb->position_m,n,contact,touching);
    } else {
        ps_ccd_settings near=*settings;near.distance_tolerance_m=offset;ps_sweep_hit event;
        result=event_query(ma,mesh_a,mb,mesh_b,bodies,0,&near,&event,touching);
        if(result==PS_OK && *touching)*contact=event.contact;
        /* event_query already returns canonical orientation. */
        return result;
    }
    if(result==PS_OK && *touching && reverse)contact->normal=ps_vscale(contact->normal,-1);
    return result;
}
static bool event_overlap(ps_aabb a,ps_aabb b) {
    return a.minimum_m.x<=b.maximum_m.x && b.minimum_m.x<=a.maximum_m.x &&
           a.minimum_m.y<=b.maximum_m.y && b.minimum_m.y<=a.maximum_m.y &&
           a.minimum_m.z<=b.maximum_m.z && b.minimum_m.z<=a.maximum_m.z;
}
static ps_result event_drift(ps_body *bodies,size_t count,double dt) {
    if(!dt)return PS_OK;
    for(size_t i=0;i<count;i++)if(bodies[i].mass_kg) {
        ps_rigid_motion m=event_motion(&bodies[i],dt);ps_body next;
        ps_result result=ps_body_motion_pose(&bodies[i],m,1,&next);if(result!=PS_OK)return result;
        bodies[i]=next;
    }
    return PS_OK;
}
static ps_result event_contacts(const ps_ccd_collider *ma,const ps_ccd_collider *mb,
    const ps_body *bodies,double tolerance,ps_contact fallback,ps_contact_manifold *contacts) {
    const ps_collider *a=&ma->collider,*b=&mb->collider;bool reverse=false;ps_result result=PS_OK;
    *contacts=(ps_contact_manifold){0};
    if(a->shape==PS_COLLIDER_PLANE){const ps_collider *swap=a;a=b;b=swap;reverse=true;}
    if(a->shape==PS_COLLIDER_BOX && b->shape==PS_COLLIDER_PLANE) {
        ps_vec3 n=ps_quat_rotate(rotation(&bodies[b->body]),b->plane_normal);n=ps_vscale(n,1/length(n));
        ps_vec3 point=ps_vadd(bodies[b->body].position_m,ps_vscale(n,tolerance));
        result=ps_contacts_box_plane(&bodies[a->body],a->size_m,point,n,contacts);
    } else if(a->shape==PS_COLLIDER_BOX && b->shape==PS_COLLIDER_BOX) {
        ps_vec3 sa=ps_vadd(a->size_m,ps_v3(tolerance,tolerance,tolerance));
        ps_vec3 sb=ps_vadd(b->size_m,ps_v3(tolerance,tolerance,tolerance));
        result=ps_contacts_boxes(&bodies[a->body],sa,&bodies[b->body],sb,contacts);
    }
    if(result!=PS_OK)return result;
    if(contacts->count && reverse)for(uint32_t i=0;i<contacts->count;i++)contacts->points[i].normal=ps_vscale(contacts->points[i].normal,-1);
    if(!contacts->count){contacts->count=1;contacts->points[0]=fallback;}
    return PS_OK;
}
ps_result ps_ccd_step(ps_body *bodies,size_t body_count,const ps_ccd_collider *input,size_t count,
    const ps_vec3 *forces,const ps_vec3 *torques,double dt,const ps_contact_solver *velocity_solver,
    const ps_ccd_step_options *supplied,ps_ccd_step_result *out) {
    ps_ccd_step_options options=supplied?*supplied:PS_CCD_STEP_DEFAULT;
    if((!bodies&&body_count)||(!input&&count)||!velocity_solver||!isfinite(dt)||dt<=0||
       !isfinite(options.contact_offset_m)||options.contact_offset_m<=0||
       !isfinite(options.ccd.distance_tolerance_m)||options.ccd.distance_tolerance_m<=0||
       options.ccd.distance_tolerance_m>options.contact_offset_m/8||!options.max_events||!options.ccd.max_iterations)
        return PS_INVALID;
    if(body_count>PS_CONTACT_GRAPH_MAX_BODIES||count>PS_CONTACT_GRAPH_MAX_BODIES||
       options.max_events>PS_CCD_MAX_EVENTS||options.ccd.max_iterations>PS_CCD_MAX_ITERATIONS)return PS_LIMIT;
    ps_body working[PS_CONTACT_GRAPH_MAX_BODIES];
    for(size_t i=0;i<body_count;i++) {
        if(ps_body_validate(&bodies[i])!=PS_OK || (forces&&!finite3(forces[i])) || (torques&&!finite3(torques[i])))return PS_INVALID;
        working[i]=bodies[i];
    }
    /* Validate solver settings even for an empty graph, before any velocity kick. */
    ps_contact_graph_solution check;
    ps_result result=ps_contacts_resolve_graph(working,body_count,NULL,0,velocity_solver,&check);
    if(result!=PS_OK)return result;
    ps_ccd_collider models[PS_CONTACT_GRAPH_MAX_BODIES];
    for(size_t i=0;i<count;i++) {
        result=event_model_valid(&input[i],bodies,body_count);if(result!=PS_OK)return result;
        for(size_t j=0;j<i;j++)if(input[j].collider.id==input[i].collider.id || input[j].collider.body==input[i].collider.body)return PS_INVALID;
        models[i]=input[i];
        for(size_t j=i;j&&models[j].collider.id<models[j-1].collider.id;j--){ps_ccd_collider swap=models[j];models[j]=models[j-1];models[j-1]=swap;}
    }
    ps_vec3 box_vertices[PS_CONTACT_GRAPH_MAX_BODIES][8];ps_convex_mesh meshes[PS_CONTACT_GRAPH_MAX_BODIES]={0};
    for(size_t i=0;i<count;i++) {
        if(models[i].collider.shape==PS_COLLIDER_BOX)event_box(models[i].collider.size_m,box_vertices[i],&meshes[i]);
        else if(models[i].collider.shape==PS_COLLIDER_CONVEX)meshes[i]=*models[i].mesh;
    }
    for(size_t i=0;i<body_count;i++)if(working[i].mass_kg) {
        ps_body next=working[i];
        result=ps_body_step(&next,forces?forces[i]:ps_v3(0,0,0),torques?torques[i]:ps_v3(0,0,0),dt);
        if(result!=PS_OK)return result;
        next.position_m=working[i].position_m;next.orientation=working[i].orientation;working[i]=next;
    }
    ps_ccd_step_result report={0};double remaining=dt;
    while(remaining>0) {
        double first=INFINITY;ps_aabb swept[PS_CONTACT_GRAPH_MAX_BODIES];
        for(size_t i=0;i<count;i++)if(models[i].collider.shape!=PS_COLLIDER_PLANE) {
            const ps_body *body=&working[models[i].collider.body];ps_rigid_motion motion=event_motion(body,remaining);
            result=models[i].collider.shape==PS_COLLIDER_SPHERE
                ?ps_aabb_swept_sphere(body,models[i].collider.size_m.x,motion.translation_m,&swept[i])
                :ps_aabb_motion_convex(body,&meshes[i],motion,&swept[i]);
            if(result!=PS_OK)return result;
        }
        for(size_t i=0;i<count;i++)for(size_t j=i+1;j<count;j++) {
            if(models[i].collider.shape!=PS_COLLIDER_PLANE && models[j].collider.shape!=PS_COLLIDER_PLANE &&
               !event_overlap(swept[i],swept[j]))continue;
            if(!working[models[i].collider.body].mass_kg&&!working[models[j].collider.body].mass_kg)continue;
            ps_sweep_hit event;bool hit;
            result=event_query(&models[i],&meshes[i],&models[j],&meshes[j],working,remaining,&options.ccd,&event,&hit);
            if(result!=PS_OK)return result;
            if(hit)first=fmin(first,event.fraction);
        }
        if(!isfinite(first)) {
            result=event_drift(working,body_count,remaining);if(result!=PS_OK)return result;
            remaining=0;break;
        }
        if(report.events==options.max_events)return PS_LIMIT;
        double elapsed=remaining*first;
        result=event_drift(working,body_count,elapsed);if(result!=PS_OK)return result;
        ps_contact_constraint contacts[PS_CONTACT_GRAPH_MAX_CONTACTS];size_t contact_count=0;
        /* Gather geometrically simultaneous contacts AFTER the drift. Different
         * approach speeds may reach the distance envelope at different fractions. */
        for(size_t i=0;i<count;i++)for(size_t j=i+1;j<count;j++) {
            if(!working[models[i].collider.body].mass_kg&&!working[models[j].collider.body].mass_kg)continue;
            ps_contact contact;bool hit;
            result=event_snapshot(&models[i],&meshes[i],&models[j],&meshes[j],working,
                                  options.contact_offset_m,&options.ccd,&contact,&hit);
            if(result!=PS_OK)return result;
            if(!hit)continue;
            if(contact_count==PS_CONTACT_GRAPH_MAX_CONTACTS)return PS_LIMIT;
            contacts[contact_count++]=(ps_contact_constraint){models[i].collider.body,models[j].collider.body,contact};
        }
        if(!contact_count)return PS_NUMERIC;
        /* Expand near box contacts into simultaneous face manifolds. */
        ps_contact_constraint expanded[PS_CONTACT_GRAPH_MAX_CONTACTS];size_t expanded_count=0;
        for(size_t k=0;k<contact_count;k++) {
            size_t i=0,j=0;
            while(models[i].collider.body!=contacts[k].a)i++;
            while(models[j].collider.body!=contacts[k].b)j++;
            ps_contact_manifold manifold;
            result=event_contacts(&models[i],&models[j],working,options.contact_offset_m,contacts[k].contact,&manifold);
            if(result!=PS_OK)return result;
            if(manifold.count>PS_CONTACT_GRAPH_MAX_CONTACTS-expanded_count)return PS_LIMIT;
            for(uint32_t m=0;m<manifold.count;m++) {
                manifold.points[m].penetration_m+=options.contact_offset_m;
                if(!isfinite(manifold.points[m].penetration_m))return PS_NUMERIC;
                expanded[expanded_count++]=(ps_contact_constraint){contacts[k].a,contacts[k].b,manifold.points[m]};
            }
        }
        ps_contact_solver solver=*velocity_solver;solver.penetration_slop_m=0;solver.correction_fraction=1;
        ps_contact_graph_solution solution;
        ps_body before[PS_CONTACT_GRAPH_MAX_BODIES];if(body_count)memcpy(before,working,body_count*sizeof *before);
        result=ps_contacts_resolve_graph(working,body_count,expanded,expanded_count,&solver,&solution);
        if(result!=PS_OK)return result;
        if(elapsed==0 && !memcmp(before,working,body_count*sizeof *before))return PS_LIMIT;
        report.events++;report.contacts+=(uint32_t)expanded_count;
        report.max_normal_error_m_s=fmax(report.max_normal_error_m_s,solution.max_normal_error_m_s);
        report.max_projection_error_m=fmax(report.max_projection_error_m,solution.max_projection_error_m);
        double next=remaining-elapsed;
        if(elapsed>0 && next==remaining)return PS_LIMIT;
        remaining=next;
    }
    report.elapsed_s=dt;
    if(body_count)memcpy(bodies,working,body_count*sizeof *bodies);
    if(out)*out=report;
    return PS_OK;
}
