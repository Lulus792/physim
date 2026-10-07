#include "physim/experiment.h"
#include "physim/data.h"
#include "platform.h"
#include "protocol.h"
#include "pacing.h"
#include <errno.h>
#include "number_parse.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
static uint32_t sequence;
static double last_snapshot_time = -1;
static bool send_message(uint32_t type, const void *p, uint32_t n) {
    unsigned char b[PS_WIRE_MAX + 20];
    size_t size = ps_wire_encode(b, type, sequence++, p, n);
    return size && fwrite(b, 1, size, stdout) == size && !fflush(stdout);
}
#include "logging.inc"
static void report(bool interactive,bool structured,const char *path,const ps_context *context,
                    ps_result code,const char *operation,const char *error) {
    ps_diagnostic d;ps_diagnostic_clear(&d);
    if(!context || ps_experiment_diagnostic(context,&d)!=PS_OK || d.code!=code || !ps_diagnostic_valid(&d)) {
        if(ps_diagnostic_set(&d,code,operation,NULL,NULL,0,0,error)!=PS_OK)
            (void)ps_diagnostic_set(&d,code,operation,NULL,NULL,0,0,ps_result_string(code));
    }
    if(path && *path)(void)ps_diagnostic_save(path,&d);
    if(interactive) {
        unsigned char payload[PS_DIAGNOSTIC_WIRE_MAX];size_t n=structured?ps_diagnostic_encode(payload,sizeof payload,&d):0;
        if(n)send_message(PS_MSG_DIAGNOSTIC,payload,(uint32_t)n);
        else send_message(PS_MSG_ERROR,error,(uint32_t)strlen(error));
    } else fprintf(stderr,"%s\n",error);
}
static ps_result snapshot(const ps_experiment_api *api, ps_context *c, ps_run_writer *writer,
                          bool paused, bool emit) {
    ps_scene scene = {0};
    c->error[0] = 0;ps_diagnostic_clear(&c->diagnostic);
    api->build_scene(c, &scene);
    if(ps_diagnostic_valid(&c->diagnostic) && c->diagnostic.code!=PS_OK)
        return c->diagnostic.code;
    if (c->error[0])
        return PS_NUMERIC;
    if(scene.count>PS_MAX_OBJECTS) return PS_INVALID;
    /* Legacy ABI-3 modules own the same object size but their tail padding has
     * no meaning. Never interpret those bytes as parent IDs without opt-in. */
    if(!(api->capabilities&PS_EXPERIMENT_SCENE_HIERARCHY))
        for(uint32_t i=0;i<scene.count;i++) scene.objects[i].parent_id=0;
    if(!(api->capabilities&PS_EXPERIMENT_SCENE_FRAMES))
        for(uint32_t i=0;i<scene.count;i++)if(scene.objects[i].shape==PS_FRAME)return PS_VERSION;
    if(!ps_scene_valid(&scene)) return PS_INVALID;
    ps_result result = ps_run_append_snapshot(writer, c, &scene, paused);
    if (result != PS_OK) return result;
    last_snapshot_time = c->time_s;
    if (!emit) return PS_OK;
    unsigned char p[PS_WIRE_MAX];
    size_t n = ps_snapshot_encode(p, c, &scene, paused);
    return n && send_message(PS_MSG_SNAPSHOT, p, (uint32_t)n) ? PS_OK : PS_IO;
}
int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: physim-runner module output.psrun [--steps N | --interactive] "
                        "[--dt seconds] [--seed N] [--param name=value]... "
                        "[--speed 0|0.1..16 (interactive only)] [--log-events] [--diagnostics] (interactive only) [--record-scenes] "
                        "[--adaptive [--min-dt seconds] [--max-dt seconds]] "
                        "[--until seconds (offline; --steps is the step budget)] [--parent-watch (offline pipe)]\n"
                        "       physim-runner module --describe\n");
        return 2;
    }
    bool describe = argc == 3 && !strcmp(argv[2], "--describe");
    bool interactive = false, record_scenes = false, adaptive = false, step_bounds = false, log_events=false,diagnostics=false;
    uint64_t steps = 4000, seed = 42;
    double dt = 0.005, speed = 1, minimum_dt=1e-8, maximum_dt=.1, end_time=0;
    bool until_option=false;
    bool speed_option = false, parent_watch = false;
    ps_context c = {0};
    c.struct_size = sizeof c;
    c.api_version = PS_API_VERSION;
    for (int i = 3; i < argc; i++) {
        char *end = NULL;
        if(!strcmp(argv[i],"--parent-watch")) {
            if(parent_watch)return 2;
            parent_watch=true;continue;
        }
        if (!strcmp(argv[i], "--interactive")) {
            interactive = true;
            continue;
        }
        if(!strcmp(argv[i],"--diagnostics")){diagnostics=true;continue;}
        if(!strcmp(argv[i],"--log-events")){log_events=true;continue;}
        if (!strcmp(argv[i], "--record-scenes")) {
            record_scenes = true;
            continue;
        }
        if (!strcmp(argv[i], "--adaptive")) { adaptive=true;continue; }
        if (i + 1 >= argc)
            return 2;
        const char *key = argv[i++];
        errno = 0;
        if (!strcmp(key, "--param")) {
            const char *equals = strchr(argv[i], '=');
            if (!equals || equals == argv[i] || equals - argv[i] >=
                (ptrdiff_t)sizeof c.parameters[0].name)
                return 2;
            char name[sizeof c.parameters[0].name];
            size_t length = (size_t)(equals - argv[i]);
            memcpy(name, argv[i], length);
            name[length] = 0;
            double selected;
            if (!ps_parse_finite_number(equals+1,NULL,&selected) ||
                ps_parameter_override(&c, name, selected) != PS_OK)
                return 2;
            continue;
        }
        if (!strcmp(key, "--dt"))
            dt = strtod(argv[i], &end);
        else if(!strcmp(key,"--until")) {
            if(until_option) return 2;
            end_time=strtod(argv[i],&end);until_option=true;
        }
        else if (!strcmp(key,"--min-dt")) {
            minimum_dt=strtod(argv[i],&end);step_bounds=true;
        } else if (!strcmp(key,"--max-dt")) {
            maximum_dt=strtod(argv[i],&end);step_bounds=true;
        }
        else if (!strcmp(key, "--speed")) {
            speed = strtod(argv[i], &end);
            speed_option = true;
        } else if (!strcmp(key, "--steps"))
            steps = strtoull(argv[i], &end, 10);
        else if (!strcmp(key, "--seed"))
            seed = strtoull(argv[i], &end, 10);
        else
            return 2;
        if (errno || !end || end == argv[i] || *end || argv[i][0] == '-')
            return 2;
    }
    if (!isfinite(dt) || dt < DBL_MIN || dt > 1 || steps > UINT64_C(1000000000) ||
        (until_option && (interactive || !isfinite(end_time) || end_time<=0 || end_time>1e9)) ||
        (step_bounds && !adaptive) || (adaptive &&
         (!isfinite(minimum_dt) || minimum_dt<DBL_MIN || !isfinite(maximum_dt) ||
          maximum_dt>1 || minimum_dt>dt || dt>maximum_dt)) ||
        !ps_speed_valid(speed) || (speed_option && !interactive) || ((log_events || diagnostics) && !interactive) ||
        (parent_watch && (interactive || !strcmp(argv[2],"--describe"))))
        return 2;
    ps_binary_stdio();
    if(parent_watch && !ps_parent_watch_start()) {
        fprintf(stderr,"Cannot monitor parent stdin pipe\n");return 125;
    }
    runner_log_state logs={.events=log_events,.disabled=describe};
    if(!describe && snprintf(logs.path,sizeof logs.path,"%s.pslog",argv[2])>=(int)sizeof logs.path)return 2;
    c.logger=(ps_logger){&logs,runner_log_write};
    ps_diagnostic_clear(&c.diagnostic);char diagnostic_path[4096]="";
    if(!describe && snprintf(diagnostic_path,sizeof diagnostic_path,"%s.psdiag",argv[2])>=(int)sizeof diagnostic_path)return 2;
    void *module = ps_module_open(argv[1]);
    if (!module) {
        report(interactive,diagnostics,diagnostic_path,&c,PS_IO,"module.load","Cannot load experiment module");
        return 3;
    }
    ps_experiment_entry entry = NULL;
    void *symbol = ps_module_symbol(module, "ps_get_experiment");
    memcpy(&entry, &symbol, sizeof entry);
    const ps_experiment_api *api = entry ? entry() : NULL;
    if (!api || api->struct_size < PS_EXPERIMENT_API_BASE_SIZE || api->abi_version != PS_ABI_VERSION ||
        ((api->capabilities&PS_EXPERIMENT_SCENE_FRAMES) && !(api->capabilities&PS_EXPERIMENT_SCENE_HIERARCHY)) ||
        !api->name || !api->create || !api->step || !api->reset || !api->build_scene ||
        !api->destroy) {
        report(interactive,diagnostics,diagnostic_path,&c,PS_VERSION,"module.abi","Experiment ABI mismatch or missing callback");
        runner_log_close(&logs,c.time_s);
        ps_module_close(module);
        return 4;
    }
    if(adaptive && (!(api->capabilities&PS_EXPERIMENT_ADAPTIVE_STEPS) ||
                    api->struct_size<sizeof *api || !api->adaptive_step)) {
        report(interactive,diagnostics,diagnostic_path,&c,PS_VERSION,"adaptive_step","Experiment does not provide an adaptive_step callback");
        runner_log_close(&logs,c.time_s);
        ps_module_close(module);return 4;
    }
    c.dt_s = dt;
    c.seed = seed;
    ps_rng_seed(&c.rng, seed);
    ps_result result = api->create(&c);
    if (result != PS_OK) {
        report(interactive,diagnostics,diagnostic_path,&c,result,"create",c.error[0]?c.error:ps_result_string(result));
        api->destroy(&c);
        runner_log_close(&logs,c.time_s);
        ps_module_close(module);
        return 5;
    }
    if (ps_parameter_finalize(&c) != PS_OK) {
        report(interactive,diagnostics,diagnostic_path,&c,PS_INVALID,"parameters","Unknown experiment parameter");
        api->destroy(&c);
        runner_log_close(&logs,c.time_s);
        ps_module_close(module);
        return 5;
    }
    if (describe) {
        bool typed=false;
        for(uint32_t i=0;i<c.parameter_count;i++)typed |= c.parameter_units[i].declared;
        int wrote = printf("PHYSIM_PARAMETERS_%u\n%u\n",typed?2:1, c.parameter_count);
        for (uint32_t i = 0; wrote >= 0 && i < c.parameter_count; i++) {
            const ps_parameter *p = &c.parameters[i];
            wrote = printf("%s\t%.17g\t%.17g\t%.17g\t%s", p->name,
                           p->default_value, p->minimum, p->maximum, p->description);
            ps_parameter_unit unit;ps_parameter_unit_read(&c,i,&unit);
            if(wrote>=0 && typed)
                wrote=printf("\t%s\t%.17g\t%d,%d,%d,%d,%d,%d,%d",unit.symbol,unit.scale,
                    unit.dimension[0],unit.dimension[1],unit.dimension[2],unit.dimension[3],
                    unit.dimension[4],unit.dimension[5],unit.dimension[6]);
            if(wrote>=0)wrote=printf("\n");
        }
        int flushed = fflush(stdout);
        api->destroy(&c);
        runner_log_close(&logs,c.time_s);
        ps_module_close(module);
        return wrote < 0 || flushed ? 6 : 0;
    }
    /* The artifact identity accompanies all model metadata. FNV is provenance, not authentication.
     */
    uint64_t hash = UINT64_C(14695981039346656037);
    FILE *binary = fopen(argv[1], "rb");
    if (binary) {
        unsigned char bytes[4096];
        size_t n;
        while ((n = fread(bytes, 1, sizeof bytes, binary)) != 0)
            for (size_t i = 0; i < n; i++) {
                hash ^= bytes[i];
                hash *= UINT64_C(1099511628211);
            }
        fclose(binary);
    }
    size_t used = strlen(c.model_metadata);
    if(adaptive || until_option) {
        int added=snprintf(c.model_metadata+used,sizeof c.model_metadata-used,
                 "\nstep_mode=%s\nminimum_dt_s=%.17g\nmaximum_dt_s=%.17g",
                 adaptive?"adaptive":"fixed",adaptive?minimum_dt:dt,adaptive?maximum_dt:dt);
        if(added<0 || (size_t)added>=sizeof c.model_metadata-used) {
            report(interactive,diagnostics,diagnostic_path,&c,PS_LIMIT,"metadata","Model metadata has no room for adaptive step provenance");
            api->destroy(&c);
            runner_log_close(&logs,c.time_s);
            ps_module_close(module);return 5;
        }
        used=strlen(c.model_metadata);
    }
    if(until_option) {
        int added=snprintf(c.model_metadata+used,sizeof c.model_metadata-used,
                          "\nend_time_s=%.17g\nmaximum_accepted_steps=%llu",
                          end_time,(unsigned long long)steps);
        if(added<0 || (size_t)added>=sizeof c.model_metadata-used) {
            report(interactive,diagnostics,diagnostic_path,&c,PS_LIMIT,"metadata","Model metadata has no room for target-time provenance");
            api->destroy(&c);
            runner_log_close(&logs,c.time_s);
            ps_module_close(module);return 5;
        }
        used=strlen(c.model_metadata);
    }
    int provenance=snprintf(c.model_metadata + used, sizeof c.model_metadata - used,
             "\nmodule_fnv1a64=%016llx\nrunner_build=%s %s", (unsigned long long)hash, __DATE__,
             __TIME__);
    if((adaptive || until_option) && (provenance<0 || (size_t)provenance>=sizeof c.model_metadata-used)) {
        report(interactive,diagnostics,diagnostic_path,&c,PS_LIMIT,"metadata","Model metadata has no room for module provenance");
        api->destroy(&c);runner_log_close(&logs,c.time_s);
        ps_module_close(module);return 5;
    }
    ps_run_writer writer;
    result = ps_run_create(&writer, argv[2], &c, api->name);
    if (result != PS_OK) {
        report(interactive,diagnostics,diagnostic_path,&c,PS_IO,"run.create","Cannot create run (path missing or file already exists)");
        api->destroy(&c);
        runner_log_close(&logs,c.time_s);
        ps_module_close(module);
        return 6;
    }
    result = ps_run_append(&writer, 0, c.values);
    if (result == PS_OK && record_scenes && !interactive)
        result = snapshot(api, &c, &writer, true, false);
    bool paused = true, stop = false, handshake = false;
    uint64_t tick = 0;
    ps_wire_buffer wire = {0};
    double start = ps_clock(), last_frame = start, last_heartbeat = start;
    ps_pacer pacer = {.speed = speed, .last = start};
    double step_dt=dt;
    if (interactive) {
        char hello[4096];
        int n = snprintf(hello, sizeof hello, "%s\n", api->name);
        for (uint32_t i = 0; i < c.channel_count && n > 0 && n < (int)sizeof hello; i++)
            n += snprintf(hello + n, sizeof hello - (size_t)n, "%s [%s]\n", c.channels[i].name,
                          c.channels[i].unit);
        if (n <= 0 || n >= (int)sizeof hello || !send_message(PS_MSG_HELLO, hello, (uint32_t)n))
            stop = true;
    }
    while (result == PS_OK && !stop) {
        bool single = false;
        double now = ps_clock();
        if (interactive) {
            int got = ps_stdin_read(wire.data + wire.used, sizeof wire.data - wire.used);
            if (got < 0)
                break;
            wire.used += (size_t)got;
            uint32_t type, n;
            const unsigned char *p;
            int status;
            while ((status = ps_wire_peek(&wire, &type, &p, &n)) > 0) {
                if (type == PS_MSG_HELLO && n == 4 && ps_get_u32(p) == PS_ABI_VERSION &&
                    !handshake) {
                    handshake = true;
                    result = snapshot(api, &c, &writer, paused, true);
                    if (result != PS_OK)
                        break;
                } else if (handshake && type == PS_MSG_SPEED && n == 8 &&
                           ps_speed_valid(ps_get_f64(p))) {
                    pacer.speed = ps_get_f64(p);
                    ps_pacer_restart(&pacer, now);
                } else if (!handshake || n != 0) {
                    result = PS_VERSION;
                    break;
                } else if (type == PS_MSG_RUN) {
                    paused = false;
                    pacer.running = true;
                    ps_pacer_restart(&pacer, now);
                    /* Report control state even when a slow, large dt is not due yet. */
                    result = snapshot(api, &c, &writer, false, true);
                    if (result != PS_OK)
                        break;
                    last_frame = now;
                } else if (type == PS_MSG_PAUSE) {
                    paused = true;
                    pacer.running = false;
                    ps_pacer_restart(&pacer, now);
                    result = snapshot(api, &c, &writer, true, true);
                    if (result != PS_OK)
                        break;
                } else if (type == PS_MSG_STEP && paused)
                    single = true;
                else if (type == PS_MSG_STOP)
                    stop = true;
                else {
                    result = PS_INVALID;
                    break;
                }
                ps_wire_consume(&wire, n);
            }
            if (status < 0)
                result = PS_CORRUPT;
            if (!handshake && now - start > 10)
                result = PS_VERSION;
            if (result != PS_OK || stop)
                break;
            if (now - last_heartbeat >= 0.5) {
                if (!send_message(PS_MSG_HEARTBEAT, NULL, 0))
                    break;
                last_heartbeat = now;
            }
            if (!single && (!handshake || !ps_pacer_due(&pacer, now, step_dt))) {
                ps_sleep(1);
                continue;
            }
        } else {
            if(until_option && c.time_s==end_time) break;
            if(tick>=steps) {
                if(until_option) {
                    snprintf(c.error,sizeof c.error,"Accepted step budget exhausted before target time");
                    result=PS_LIMIT;
                }
                break;
            }
        }
        double proposed=step_dt, previous_time=c.time_s;
        bool terminal=false;
        if(until_option) {
            double remaining=end_time-previous_time;
            terminal=adaptive?proposed>=remaining:(double)(tick+1)*dt>end_time;
            if(terminal) proposed=remaining;
        }
        double accepted_dt=proposed, call_minimum=fmin(minimum_dt,proposed);
        if(adaptive) {
            if(previous_time+proposed==previous_time || previous_time+call_minimum==previous_time) {
                snprintf(c.error,sizeof c.error,"Adaptive step cannot advance floating-point time");
                result=PS_LIMIT;break;
            }
            ps_step_interval interval={NAN,NAN};
            c.dt_s=proposed;
            result=api->adaptive_step(&c,proposed,call_minimum,maximum_dt,&interval);
            if(result==PS_OK) {
                double next_time=previous_time+interval.elapsed_s;
                if(c.time_s!=previous_time || !isfinite(interval.elapsed_s) || !isfinite(next_time) ||
                   next_time<=previous_time || next_time<previous_time+call_minimum ||
                   next_time>previous_time+proposed || (until_option && next_time>end_time) ||
                   !isfinite(interval.next_s) || interval.next_s<call_minimum || interval.next_s>maximum_dt) {
                    snprintf(c.error,sizeof c.error,"Invalid adaptive step interval or modified host time");
                    result=PS_INVALID;
                } else {
                    accepted_dt=interval.elapsed_s;
                    if(interactive && !single) ps_pacer_refund(&pacer,step_dt,accepted_dt);
                    step_dt=interval.next_s;
                }
            }
        } else {
            if(until_option) c.dt_s=proposed;
            result = api->step(&c, proposed);
        }
        if (result != PS_OK)
            break;
        tick++;
        c.time_s = adaptive ? previous_time+accepted_dt : terminal?end_time:(double)tick * dt;
        if(adaptive || until_option) c.dt_s=accepted_dt;
        result = ps_run_append(&writer, c.time_s, c.values);
        if (interactive && (single || now - last_frame >= 1.0 / 60)) {
            ps_result scene_result = snapshot(api, &c, &writer, paused, true);
            if (scene_result != PS_OK) {
                result = scene_result;
                break;
            }
            last_frame = now;
        }
        if (!interactive && record_scenes && c.time_s - last_snapshot_time >= 1.0 / 60) {
            ps_result scene_result = snapshot(api, &c, &writer, false, false);
            if (scene_result != PS_OK) result = scene_result;
        }
    }
    if (result == PS_OK && last_snapshot_time >= 0 && c.time_s > last_snapshot_time)
        result = snapshot(api, &c, &writer, true, interactive);
    if (result == PS_OK)
        result = ps_run_close(&writer);
    else {
        fclose(writer.file);
        writer.file = NULL;
    }
    if (result != PS_OK)
        report(interactive,diagnostics,diagnostic_path,&c,result,"step",c.error[0]?c.error:ps_result_string(result));
    api->destroy(&c);
    runner_log_close(&logs,c.time_s);
    if (interactive)
        send_message(PS_MSG_BYE, NULL, 0);
    else
        printf("%llu samples written to %s\n", (unsigned long long)(tick + 1), argv[2]);
    ps_module_close(module);
    return result == PS_OK ? 0 : 7;
}
