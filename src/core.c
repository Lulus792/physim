#include "physim/core.h"
#include "physim/experiment.h"
#include "physim/data.h"
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

static bool diagnostic_error(ps_result code) {
    return code>=PS_INVALID && code<=PS_NUMERIC && code!=PS_EOF && code!=PS_RECOVERED;
}
void ps_diagnostic_clear(ps_diagnostic *d) {
    if(d){memset(d,0,sizeof *d);d->struct_size=sizeof *d;d->version=PS_DIAGNOSTIC_VERSION;}
}
bool ps_diagnostic_valid(const ps_diagnostic *d) {
    if(!d || d->struct_size!=sizeof *d || d->version!=PS_DIAGNOSTIC_VERSION ||
       !ps_text_valid(d->operation,sizeof d->operation,false) ||
       !ps_text_valid(d->argument,sizeof d->argument,false) ||
       !ps_text_valid(d->source,sizeof d->source,false) ||
       !ps_text_valid(d->message,sizeof d->message,true) ||
       (d->column && !d->line) || ((d->line || d->column) && !d->source[0]))return false;
    if(d->code==PS_OK)return !d->operation[0] && !d->argument[0] && !d->source[0] && !d->message[0] && !d->line && !d->column;
    return diagnostic_error(d->code) && d->message[0];
}
ps_result ps_diagnostic_set(ps_diagnostic *out,ps_result code,const char *operation,const char *argument,
                            const char *source,uint32_t line,uint32_t column,const char *message) {
    if(!out || !message || !diagnostic_error(code))return PS_INVALID;
    operation=operation?operation:"";argument=argument?argument:"";source=source?source:"";
    if(!ps_text_valid(operation,PS_DIAGNOSTIC_LABEL_MAX+1u,false) ||
       !ps_text_valid(argument,PS_DIAGNOSTIC_LABEL_MAX+1u,false) ||
       !ps_text_valid(source,PS_DIAGNOSTIC_SOURCE_MAX+1u,false) ||
       !ps_text_valid(message,PS_DIAGNOSTIC_MESSAGE_MAX+1u,true))return PS_INVALID;
    ps_diagnostic d;ps_diagnostic_clear(&d);d.code=code;d.line=line;d.column=column;
    memcpy(d.operation,operation,strlen(operation)+1);memcpy(d.argument,argument,strlen(argument)+1);
    memcpy(d.source,source,strlen(source)+1);memcpy(d.message,message,strlen(message)+1);
    if(!ps_diagnostic_valid(&d))return PS_INVALID;
    *out=d;return PS_OK;
}
ps_result ps_diagnostic_format(const ps_diagnostic *d,char *out,size_t capacity) {
    if(!out || !capacity || !ps_diagnostic_valid(d))return PS_INVALID;
    char text[PS_DIAGNOSTIC_WIRE_MAX+256],location[PS_DIAGNOSTIC_SOURCE_MAX+64];location[0]=0;
    if(d->source[0]) {
        if(d->line)snprintf(location,sizeof location,"%s:%u:%u: ",d->source,d->line,d->column?d->column:1);
        else snprintf(location,sizeof location,"%s: ",d->source);
    }
    if(d->code==PS_OK)text[0]=0;
    else snprintf(text,sizeof text,"%serror [%s%s%s%s%s]: %s",location,ps_result_string(d->code),
        d->operation[0]?"; ":"",d->operation,d->argument[0]?"/":"",d->argument,d->message);
    size_t n=strlen(text),copied=n<capacity?n:capacity-1;
    while(copied && ((unsigned char)text[copied]&0xc0)==0x80)copied--;
    memcpy(out,text,copied);out[copied]=0;return copied==n?PS_OK:PS_LIMIT;
}
size_t ps_diagnostic_encode(unsigned char *out,size_t capacity,const ps_diagnostic *d) {
    if(!out || !ps_diagnostic_valid(d) || !diagnostic_error(d->code))return 0;
    const char *fields[]={d->operation,d->argument,d->source,d->message};uint32_t lengths[4];size_t size=40;
    for(unsigned i=0;i<4;i++){lengths[i]=(uint32_t)strlen(fields[i]);size+=lengths[i];}
    if(capacity<size)return 0;
    ps_put_u32(out,UINT32_C(0x47445350));ps_put_u32(out+4,PS_DIAGNOSTIC_VERSION);
    ps_put_u32(out+8,(uint32_t)d->code);ps_put_u32(out+12,d->line);ps_put_u32(out+16,d->column);
    size_t at=36;
    for(unsigned i=0;i<4;i++){ps_put_u32(out+20+4*i,lengths[i]);memcpy(out+at,fields[i],lengths[i]);at+=lengths[i];}
    ps_put_u32(out+at,ps_crc32(out,at));return size;
}
ps_result ps_diagnostic_decode(const unsigned char *data,size_t size,ps_diagnostic *out) {
    if(!data || !out)return PS_INVALID;
    if(size<40 || size>PS_DIAGNOSTIC_WIRE_MAX || ps_get_u32(data)!=UINT32_C(0x47445350))return PS_CORRUPT;
    if(ps_get_u32(data+4)!=PS_DIAGNOSTIC_VERSION)return PS_VERSION;
    uint32_t lengths[4],limits[]={PS_DIAGNOSTIC_LABEL_MAX,PS_DIAGNOSTIC_LABEL_MAX,PS_DIAGNOSTIC_SOURCE_MAX,PS_DIAGNOSTIC_MESSAGE_MAX};size_t expected=40;
    for(unsigned i=0;i<4;i++){lengths[i]=ps_get_u32(data+20+4*i);if(lengths[i]>limits[i])return PS_CORRUPT;expected+=lengths[i];}
    if(size!=expected || ps_get_u32(data+size-4)!=ps_crc32(data,size-4))return PS_CORRUPT;
    ps_diagnostic d;ps_diagnostic_clear(&d);d.code=(ps_result)ps_get_u32(data+8);d.line=ps_get_u32(data+12);d.column=ps_get_u32(data+16);
    char *fields[]={d.operation,d.argument,d.source,d.message};size_t at=36;
    for(unsigned i=0;i<4;i++){if(memchr(data+at,0,lengths[i]))return PS_CORRUPT;memcpy(fields[i],data+at,lengths[i]);at+=lengths[i];}
    if(!ps_diagnostic_valid(&d) || !diagnostic_error(d.code))return PS_CORRUPT;
    *out=d;return PS_OK;
}
ps_result ps_diagnostic_save(const char *path,const ps_diagnostic *d) {
    if(!path || !*path)return PS_INVALID;
    unsigned char data[PS_DIAGNOSTIC_WIRE_MAX];size_t n=ps_diagnostic_encode(data,sizeof data,d);if(!n)return PS_INVALID;
    FILE *f=fopen(path,"wbx");if(!f)return PS_IO;
    bool ok=fwrite(data,1,n,f)==n && !fflush(f);if(fclose(f))ok=false;return ok?PS_OK:PS_IO;
}
ps_result ps_diagnostic_load(const char *path,ps_diagnostic *out) {
    if(!path || !*path || !out)return PS_INVALID;
    FILE *f=fopen(path,"rb");if(!f)return PS_IO;
    unsigned char data[PS_DIAGNOSTIC_WIRE_MAX];size_t n=fread(data,1,sizeof data,f);int extra=fgetc(f);
    bool failed=ferror(f)!=0;if(fclose(f))failed=true;
    if(failed)return PS_IO;
    if(extra!=EOF)return PS_CORRUPT;
    return ps_diagnostic_decode(data,n,out);
}
static bool diagnostic_context(const ps_context *c) {
    return c && c->api_version==PS_API_VERSION && c->struct_size>=offsetof(ps_context,diagnostic)+sizeof c->diagnostic;
}
ps_result ps_experiment_fail(ps_context *c,const ps_diagnostic *d) {
    if(!c || c->api_version!=PS_API_VERSION || c->struct_size<offsetof(ps_context,error)+sizeof c->error)return PS_VERSION;
    if(!ps_diagnostic_valid(d) || !diagnostic_error(d->code))return PS_INVALID;
    ps_diagnostic copy=*d;char text[sizeof c->error];(void)ps_diagnostic_format(&copy,text,sizeof text);
    memcpy(c->error,text,strlen(text)+1);if(diagnostic_context(c))c->diagnostic=copy;return copy.code;
}
ps_result ps_experiment_diagnostic(const ps_context *c,ps_diagnostic *out) {
    if(!out)return PS_INVALID;
    if(!diagnostic_context(c))return PS_VERSION;
    if(!c->diagnostic.struct_size && c->diagnostic.code==PS_OK){ps_diagnostic_clear(out);return PS_OK;}
    if(!ps_diagnostic_valid(&c->diagnostic))return PS_CORRUPT;
    *out=c->diagnostic;return PS_OK;
}
