#include "physim/core.h"
#include "physim/experiment.h"
#include "text_validation.h"
#include "number_parse.h"
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
const char *ps_result_string(ps_result r) {
    static const char *names[] = {"OK",
                                  "Invalid argument",
                                  "I/O error",
                                  "Out of memory",
                                  "Incompatible version",
                                  "Corrupt data",
                                  "End of file",
                                  "Recovered incomplete run",
                                  "Singular or ill-conditioned system",
                                  "Iteration or step limit reached",
                                  "Non-finite numerical result"};
    return (unsigned)r < sizeof(names) / sizeof(names[0]) ? names[r] : "Unknown error";
}
ps_vec3 ps_v3(double x, double y, double z) {
    ps_vec3 a = {x, y, z};
    return a;
}
ps_vec3 ps_vadd(ps_vec3 a, ps_vec3 b) { return ps_v3(a.x + b.x, a.y + b.y, a.z + b.z); }
ps_vec3 ps_vsub(ps_vec3 a, ps_vec3 b) { return ps_v3(a.x - b.x, a.y - b.y, a.z - b.z); }
ps_vec3 ps_vscale(ps_vec3 a, double s) { return ps_v3(a.x * s, a.y * s, a.z * s); }
double ps_vdot(ps_vec3 a, ps_vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
ps_vec3 ps_vcross(ps_vec3 a, ps_vec3 b) {
    return ps_v3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}
double ps_vlength(ps_vec3 a) { return hypot(hypot(a.x, a.y), a.z); }
ps_vec3 ps_vnormalize(ps_vec3 a) {
    if (!isfinite(a.x) || !isfinite(a.y) || !isfinite(a.z))
        return ps_v3(NAN, NAN, NAN);
    double scale = fmax(fmax(fabs(a.x), fabs(a.y)), fabs(a.z));
    if (scale == 0)
        return ps_v3(0, 0, 0);
    a = ps_v3(a.x / scale, a.y / scale, a.z / scale);
    double n = ps_vlength(a);
    return ps_v3(a.x / n, a.y / n, a.z / n);
}
ps_quat ps_quat_axis_angle(ps_vec3 a, double t) {
    if (!isfinite(a.x) || !isfinite(a.y) || !isfinite(a.z) || !isfinite(t))
        return (ps_quat){NAN, NAN, NAN, NAN};
    if (a.x == 0 && a.y == 0 && a.z == 0)
        return (ps_quat){0, 0, 0, 1};
    a = ps_vnormalize(a);
    double s = sin(t / 2);
    ps_quat q = {a.x * s, a.y * s, a.z * s, cos(t / 2)};
    return q;
}
ps_vec3 ps_quat_rotate(ps_quat q, ps_vec3 v) {
    ps_vec3 u = ps_v3(q.x, q.y, q.z);
    return ps_vadd(v, ps_vscale(ps_vcross(u, ps_vadd(ps_vcross(u, v), ps_vscale(v, q.w))), 2));
}
ps_mat4 ps_mat4_identity(void) {
    ps_mat4 a = {{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}};
    return a;
}
ps_mat4 ps_mat4_multiply(ps_mat4 a, ps_mat4 b) {
    ps_mat4 r = {{0}};
    for (int c = 0; c < 4; c++)
        for (int row = 0; row < 4; row++)
            for (int k = 0; k < 4; k++)
                r.m[c * 4 + row] += a.m[k * 4 + row] * b.m[c * 4 + k];
    return r;
}
const ps_unit PS_METRE = {{1, 0, 0, 0, 0, 0, 0}, 1, "m"},
              PS_SECOND = {{0, 0, 1, 0, 0, 0, 0}, 1, "s"},
              PS_KILOGRAM = {{0, 1, 0, 0, 0, 0, 0}, 1, "kg"},
              PS_RADIAN = {{0, 0, 0, 0, 0, 0, 0}, 1, "rad"},
              PS_JOULE = {{2, 1, -2, 0, 0, 0, 0}, 1, "J"},
              PS_VELOCITY = {{1, 0, -1, 0, 0, 0, 0}, 1, "m/s"};
ps_result ps_convert(double v, ps_unit a, ps_unit b, double *out) {
    if (!out || !isfinite(v) || memcmp(a.dimension, b.dimension, 7) || !isfinite(a.scale) ||
        !isfinite(b.scale) || a.scale <= 0 || b.scale <= 0)
        return PS_INVALID;
    int ev, ea, eb;
    double mv = frexp(v, &ev), ma = frexp(a.scale, &ea), mb = frexp(b.scale, &eb);
    double result = scalbn(mv * ma / mb, ev + ea - eb);
    if (!isfinite(result) || (v != 0 && result == 0))
        return PS_NUMERIC;
    *out = result;
    return PS_OK;
}
uint32_t ps_rng_u32(ps_rng *r) {
    uint64_t old = r->state;
    r->state = old * UINT64_C(6364136223846793005) + r->increment;
    uint32_t x = (uint32_t)(((old >> 18) ^ old) >> 27), rot = (uint32_t)(old >> 59);
    return (x >> rot) | (x << ((32u - rot) & 31u));
}
void ps_rng_seed(ps_rng *r, uint64_t seed) {
    r->state = 0;
    r->increment = UINT64_C(1442695040888963407);
    ps_rng_u32(r);
    r->state += seed;
    ps_rng_u32(r);
}
double ps_rng_uniform(ps_rng *r) { return (ps_rng_u32(r) + 0.5) / 4294967296.0; }
double ps_rng_normal(ps_rng *r, double mean, double sd) {
    return mean + sd * sqrt(-2 * log(ps_rng_uniform(r))) * cos(2 * PS_PI * ps_rng_uniform(r));
}
ps_result ps_ode_step(ps_integrator method, ps_ode_fn f, void *u, double t, double dt, double *y,
                      size_t n) {
    double a[32], b[32], c[32], d[32], z[32];
    if (!f || !y || !n || n > 32 || !isfinite(dt) || dt <= 0 || !isfinite(t) ||
        (method != PS_EULER && method != PS_RK4))
        return PS_INVALID;
    if (!isfinite(t + dt))
        return PS_INVALID;
    for (size_t i = 0; i < n; i++) {
        if (!isfinite(y[i]))
            return PS_INVALID;
        a[i] = b[i] = c[i] = d[i] = NAN;
    }
    f(t, y, a, u);
    for (size_t i = 0; i < n; i++)
        if (!isfinite(a[i]))
            return PS_NUMERIC;
    if (method == PS_EULER) {
        for (size_t i = 0; i < n; i++)
            z[i] = y[i] + dt * a[i];
    } else {
        for (size_t i = 0; i < n; i++)
            z[i] = y[i] + dt * a[i] / 2;
        for (size_t i = 0; i < n; i++)
            if (!isfinite(z[i]))
                return PS_NUMERIC;
        f(t + dt / 2, z, b, u);
        for (size_t i = 0; i < n; i++)
            if (!isfinite(b[i]))
                return PS_NUMERIC;
        for (size_t i = 0; i < n; i++)
            z[i] = y[i] + dt * b[i] / 2;
        for (size_t i = 0; i < n; i++)
            if (!isfinite(z[i]))
                return PS_NUMERIC;
        f(t + dt / 2, z, c, u);
        for (size_t i = 0; i < n; i++)
            if (!isfinite(c[i]))
                return PS_NUMERIC;
        for (size_t i = 0; i < n; i++)
            z[i] = y[i] + dt * c[i];
        for (size_t i = 0; i < n; i++)
            if (!isfinite(z[i]))
                return PS_NUMERIC;
        f(t + dt, z, d, u);
        for (size_t i = 0; i < n; i++)
            if (!isfinite(d[i]))
                return PS_NUMERIC;
        for (size_t i = 0; i < n; i++)
            z[i] = y[i] + dt * (a[i] + 2 * b[i] + 2 * c[i] + d[i]) / 6;
    }
    for (size_t i = 0; i < n; i++)
        if (!isfinite(z[i]))
            return PS_NUMERIC;
    memcpy(y, z, n * sizeof(double));
    return PS_OK;
}
void ps_symplectic_step(double *p, double *v, double a, double dt) {
    *v += a * dt;
    *p += *v * dt;
}
const ps_medium PS_VACUUM = {0, 0, "vacuum"}, PS_AIR = {1.225, 1.81e-5, "air at 15 C, sea level"},
                PS_WATER = {998.2, 1.002e-3, "water at 20 C"};
ps_vec3 ps_drag_force(ps_vec3 v, ps_medium m, double cd, double area) {
    return ps_vscale(v, -0.5 * m.density_kg_m3 * cd * area * ps_vlength(v));
}
bool ps_collide_spheres(ps_particle *a, ps_particle *b, double e) {
    if (!a || !b || a->mass_kg <= 0 || b->mass_kg <= 0 || e < 0 || e > 1)
        return false;
    ps_vec3 d = ps_vsub(b->position_m, a->position_m);
    double n = ps_vlength(d), r = a->radius_m + b->radius_m;
    if (n >= r)
        return false;
    ps_vec3 axis = n > 1e-12 ? ps_vscale(d, 1 / n) : ps_v3(1, 0, 0);
    double ia = 1 / a->mass_kg, ib = 1 / b->mass_kg;
    a->position_m = ps_vsub(a->position_m, ps_vscale(axis, (r - n) * ia / (ia + ib)));
    b->position_m = ps_vadd(b->position_m, ps_vscale(axis, (r - n) * ib / (ia + ib)));
    double v = ps_vdot(ps_vsub(b->velocity_m_s, a->velocity_m_s), axis);
    if (v < 0) {
        double j = -(1 + e) * v / (ia + ib);
        a->velocity_m_s = ps_vsub(a->velocity_m_s, ps_vscale(axis, j * ia));
        b->velocity_m_s = ps_vadd(b->velocity_m_s, ps_vscale(axis, j * ib));
    }
    return true;
}
int ps_channel_add(ps_context *c, const char *name, ps_unit unit, const char *description) {
    if (!c || !name || !unit.symbol || !description || c->channel_count >= PS_MAX_CHANNELS)
        return -1;
    unsigned n = c->channel_count++;
    ps_channel *ch = &c->channels[n];
    snprintf(ch->name, sizeof ch->name, "%s", name);
    snprintf(ch->unit, sizeof ch->unit, "%s", unit.symbol);
    snprintf(ch->description, sizeof ch->description, "%s", description);
    memcpy(ch->dimension, unit.dimension, 7);
    return (int)n;
}
static bool parameter_context(const ps_context *c) {
    return c && c->struct_size >=
                    offsetof(ps_context, parameters) + sizeof c->parameters &&
           c->api_version == PS_API_VERSION;
}
static bool parameter_unit_tail(const ps_context *c) {
    return parameter_context(c) && c->struct_size >=
        offsetof(ps_context, parameter_units) + sizeof c->parameter_units;
}
static bool parameter_unit_valid(ps_unit unit) {
    return isfinite(unit.scale) && unit.scale>0 && unit.symbol && unit.symbol[0] &&
        ps_text_valid(unit.symbol,sizeof(((ps_parameter_unit *)0)->symbol),false);
}
static bool parameter_display_valid(double value,double scale) {
    double shown=value/scale;
    return isfinite(shown) && (value==0 || shown!=0);
}
static bool parameter_name(const char *name) {
    if (!name)
        return false;
    for (size_t i = 0; i < sizeof(((ps_parameter *)0)->name); i++) {
        unsigned char ch = (unsigned char)name[i];
        if (!ch)
            return i > 0;
        if (!((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || ch == '_' ||
              (i && ((ch >= '0' && ch <= '9') || ch == '.' || ch == '-'))))
            return false;
    }
    return false;
}
static bool parameter_description(const char *description) {
    if (!description)
        return false;
    for (size_t i = 0; i < sizeof(((ps_parameter *)0)->description); i++) {
        unsigned char ch = (unsigned char)description[i];
        if (!ch)
            return i > 0;
        if (ch < 32 || ch == 127)
            return false;
    }
    return false;
}
ps_result ps_parameter_override(ps_context *c, const char *name, double value) {
    if (!parameter_context(c))
        return PS_VERSION;
    if (!parameter_name(name) || !isfinite(value) || c->parameter_count > PS_MAX_PARAMETERS)
        return PS_INVALID;
    for (uint32_t i = 0; i < c->parameter_count; i++)
        if (!strcmp(c->parameters[i].name, name))
            return PS_INVALID;
    if (c->parameter_count == PS_MAX_PARAMETERS)
        return PS_LIMIT;
    ps_parameter *entry = &c->parameters[c->parameter_count++];
    if (parameter_unit_tail(c))
        memset(&c->parameter_units[c->parameter_count-1], 0, sizeof c->parameter_units[0]);
    memset(entry, 0, sizeof *entry);
    memcpy(entry->name, name, strlen(name) + 1);
    entry->value = value;
    return PS_OK;
}
ps_result ps_parameter_define(ps_context *c, const char *name, const char *description,
                              double default_value, double minimum, double maximum,
                              double *value) {
    if (!parameter_context(c))
        return PS_VERSION;
    if (!value || !parameter_name(name) || !parameter_description(description) ||
        !isfinite(default_value) || !isfinite(minimum) || !isfinite(maximum) ||
        minimum > maximum || default_value < minimum || default_value > maximum ||
        c->parameter_count > PS_MAX_PARAMETERS)
        return PS_INVALID;
    ps_parameter *entry = NULL;
    for (uint32_t i = 0; i < c->parameter_count; i++)
        if (!strcmp(c->parameters[i].name, name)) {
            entry = &c->parameters[i];
            break;
        }
    if (entry && entry->defined)
        return PS_INVALID;
    if (!entry && c->parameter_count == PS_MAX_PARAMETERS)
        return PS_LIMIT;
    double selected = entry ? entry->value : default_value;
    if (selected < minimum || selected > maximum)
        return PS_INVALID;
    if (!entry) {
        entry = &c->parameters[c->parameter_count++];
        memset(entry, 0, sizeof *entry);
        memcpy(entry->name, name, strlen(name) + 1);
    }
    memcpy(entry->description, description, strlen(description) + 1);
    entry->value = selected;
    entry->default_value = default_value;
    entry->minimum = minimum;
    entry->maximum = maximum;
    entry->defined = true;
    if (parameter_unit_tail(c))
        memset(&c->parameter_units[entry-c->parameters], 0, sizeof c->parameter_units[0]);
    *value = selected;
    return PS_OK;
}
ps_result ps_parameter_define_unit(ps_context *c, const char *name, const char *description,
                                   ps_unit unit, double standard, double minimum,
                                   double maximum, double *value) {
    if (!parameter_unit_tail(c)) return PS_VERSION;
    if (!parameter_unit_valid(unit) || !isfinite(standard) || !isfinite(minimum) ||
        !isfinite(maximum) || !parameter_name(name) || c->parameter_count>PS_MAX_PARAMETERS)
        return PS_INVALID;
    if (!parameter_display_valid(standard,unit.scale) || !parameter_display_valid(minimum,unit.scale) ||
        !parameter_display_valid(maximum,unit.scale) ||
        (minimum!=maximum && minimum/unit.scale==maximum/unit.scale)) return PS_NUMERIC;
    for(uint32_t i=0;i<c->parameter_count;i++)
        if(!strcmp(c->parameters[i].name,name) && !parameter_display_valid(c->parameters[i].value,unit.scale))
            return PS_NUMERIC;
    ps_result result = ps_parameter_define(c,name,description,standard,minimum,maximum,value);
    if (result != PS_OK) return result;
    for (uint32_t i=0;i<c->parameter_count;i++) {
        if (!strcmp(c->parameters[i].name,name)) {
            ps_parameter_unit *stored=&c->parameter_units[i];
            memcpy(stored->dimension,unit.dimension,7);
            stored->scale=unit.scale;
            memcpy(stored->symbol,unit.symbol,strlen(unit.symbol)+1);
            stored->declared=true;
            break;
        }
    }
    return PS_OK;
}
ps_result ps_parameter_unit_read(const ps_context *c,uint32_t index,ps_parameter_unit *unit) {
    if (!parameter_context(c)) return PS_VERSION;
    if (!unit || c->parameter_count>PS_MAX_PARAMETERS || index>=c->parameter_count)
        return PS_INVALID;
    ps_parameter_unit result={0};result.scale=1;
    if (parameter_unit_tail(c) && c->parameter_units[index].declared) {
        result=c->parameter_units[index];
        ps_unit check={{0},result.scale,result.symbol};
        if (!parameter_unit_valid(check)) return PS_INVALID;
    }
    *unit=result;return PS_OK;
}
/* Return one exact line value, rejecting duplicate keys and oversized fields. */
static int parameter_metadata_field(const char *metadata,const char *prefix,const char *name,
                                     char *value,size_t capacity) {
    char key[96];snprintf(key,sizeof key,"%s.%s=",prefix,name);
    const char *cursor=metadata;int found=0;
    while ((cursor=strstr(cursor,key))) {
        if (cursor!=metadata && cursor[-1]!='\n') {cursor++;continue;}
        cursor+=strlen(key);
        const char *end=strchr(cursor,'\n');if(!end)end=cursor+strlen(cursor);
        size_t length=(size_t)(end-cursor);
        if (found || length>=capacity) return -1;
        memcpy(value,cursor,length);value[length]=0;found=1;
    }
    return found;
}
ps_result ps_parameter_unit_parse(const char *metadata,const char *name,ps_parameter_unit *unit) {
    if (!metadata || !parameter_name(name) || !unit) return PS_INVALID;
    char symbol[16],scale[64],dimension[64];
    int a=parameter_metadata_field(metadata,"parameter_unit",name,symbol,sizeof symbol);
    int b=parameter_metadata_field(metadata,"parameter_scale",name,scale,sizeof scale);
    int d=parameter_metadata_field(metadata,"parameter_dimension",name,dimension,sizeof dimension);
    ps_parameter_unit parsed={0};parsed.scale=1;
    if (!a && !b && !d) {*unit=parsed;return PS_OK;}
    if (a!=1 || b!=1 || d!=1) return PS_CORRUPT;
    char *end;
    if (!ps_parse_finite_number(scale,NULL,&parsed.scale)) return PS_CORRUPT;
    ps_unit check={{0},parsed.scale,symbol};
    if (!parameter_unit_valid(check)) return PS_CORRUPT;
    char *cursor=dimension;
    for (unsigned i=0;i<7;i++) {
        errno=0;long exponent=strtol(cursor,&end,10);
        if(errno || end==cursor || exponent<INT8_MIN || exponent>INT8_MAX ||
           *end!=(i<6?',':0)) return PS_CORRUPT;
        parsed.dimension[i]=(int8_t)exponent;cursor=end+1;
    }
    memcpy(parsed.symbol,symbol,strlen(symbol)+1);parsed.declared=true;
    *unit=parsed;return PS_OK;
}
ps_result ps_parameter_finalize(const ps_context *c) {
    if (!parameter_context(c))
        return PS_VERSION;
    if (c->parameter_count > PS_MAX_PARAMETERS)
        return PS_INVALID;
    for (uint32_t i = 0; i < c->parameter_count; i++) {
        ps_parameter_unit unit;
        if (!c->parameters[i].defined || ps_parameter_unit_read(c,i,&unit)!=PS_OK)
            return PS_INVALID;
    }
    return PS_OK;
}
void ps_scene_add(ps_scene *s, ps_shape shape, ps_vec3 a, ps_vec3 b, double radius, uint32_t rgba) {
    (void)ps_scene_add_id(s, 0, shape, a, b, radius, rgba);
}
ps_result ps_scene_add_id(ps_scene *s, uint32_t id, ps_shape shape, ps_vec3 a, ps_vec3 b,
                          double radius, uint32_t rgba) {
    ps_object o = {0};
    o.id = id;
    o.shape = shape;
    o.a = a;
    o.b = b;
    o.radius = radius;
    o.color = rgba;
    o.orientation.w = 1;
    return ps_scene_push(s, &o);
}

const char *ps_log_level_name(ps_log_level level) {
    static const char *names[]={"debug","info","warning","error"};
    return level>=PS_LOG_DEBUG && level<=PS_LOG_ERROR?names[level-PS_LOG_DEBUG]:"unknown";
}
bool ps_log_record_valid(const ps_log_record *record) {
    return record && record->level>=PS_LOG_DEBUG && record->level<=PS_LOG_ERROR &&
        isfinite(record->time_s) && record->message[0] &&
        ps_text_valid(record->message,sizeof record->message,true);
}
ps_result ps_logger_emit(const ps_logger *logger,ps_log_level level,double time_s,const char *message) {
    if(!logger || !message || !ps_text_valid(message,PS_LOG_MESSAGE_MAX+1u,true) || !*message ||
       level<PS_LOG_DEBUG || level>PS_LOG_ERROR || !isfinite(time_s))return PS_INVALID;
    ps_log_record record={.level=level,.time_s=time_s};
    memcpy(record.message,message,strlen(message)+1);
    return logger->write?logger->write(logger->user,&record):PS_OK;
}
ps_result ps_experiment_log(const ps_context *context,ps_log_level level,const char *message) {
    if(!context || context->api_version!=PS_API_VERSION ||
       context->struct_size<offsetof(ps_context,logger)+sizeof context->logger)return PS_VERSION;
    return ps_logger_emit(&context->logger,level,context->time_s,message);
}
