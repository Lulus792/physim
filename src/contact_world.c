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
