#include "physim/experiment.h"
#include "physim/data.h"
#include "platform.h"
#include "protocol.h"
#include "pacing.h"
#include <errno.h>
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
static void report(bool interactive, const char *error) {
    if (interactive)
        send_message(PS_MSG_ERROR, error, (uint32_t)strlen(error));
    else
        fprintf(stderr, "%s\n", error);
}
static ps_result snapshot(const ps_experiment_api *api, ps_context *c, ps_run_writer *writer,
                          bool paused, bool emit) {
    ps_scene scene = {0};
    c->error[0] = 0;
    api->build_scene(c, &scene);
    if (c->error[0])
        return PS_NUMERIC;
    if(scene.count>PS_MAX_OBJECTS) return PS_INVALID;
    /* Legacy ABI-3 modules own the same object size but their tail padding has
     * no meaning. Never interpret those bytes as parent IDs without opt-in. */
    if(!(api->capabilities&PS_EXPERIMENT_SCENE_HIERARCHY))
        for(uint32_t i=0;i<scene.count;i++) scene.objects[i].parent_id=0;
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
                        "[--speed 0|0.1..16 (interactive only)] [--record-scenes]\n"
                        "       physim-runner module --describe\n");
        return 2;
    }
    bool describe = argc == 3 && !strcmp(argv[2], "--describe");
    bool interactive = false, record_scenes = false;
    uint64_t steps = 4000, seed = 42;
    double dt = 0.005, speed = 1;
    bool speed_option = false;
    ps_context c = {0};
    c.struct_size = sizeof c;
    c.api_version = PS_API_VERSION;
    for (int i = 3; i < argc; i++) {
        char *end = NULL;
        if (!strcmp(argv[i], "--interactive")) {
            interactive = true;
            continue;
        }
        if (!strcmp(argv[i], "--record-scenes")) {
            record_scenes = true;
            continue;
        }
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
            char *value_end = NULL;
            double selected = strtod(equals + 1, &value_end);
            if (errno || value_end == equals + 1 || !value_end || *value_end ||
                ps_parameter_override(&c, name, selected) != PS_OK)
                return 2;
            continue;
        }
        if (!strcmp(key, "--dt"))
            dt = strtod(argv[i], &end);
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
    if (!isfinite(dt) || dt <= 0 || dt > 1 || steps > UINT64_C(1000000000) ||
        !ps_speed_valid(speed) || (speed_option && !interactive))
        return 2;
    ps_binary_stdio();
    void *module = ps_module_open(argv[1]);
    if (!module) {
        report(interactive, "Cannot load experiment module");
        return 3;
    }
    ps_experiment_entry entry = NULL;
    void *symbol = ps_module_symbol(module, "ps_get_experiment");
    memcpy(&entry, &symbol, sizeof entry);
    const ps_experiment_api *api = entry ? entry() : NULL;
    if (!api || api->struct_size < sizeof *api || api->abi_version != PS_ABI_VERSION ||
        !api->name || !api->create || !api->step || !api->reset || !api->build_scene ||
        !api->destroy) {
        report(interactive, "Experiment ABI mismatch or missing callback");
        ps_module_close(module);
        return 4;
    }
    c.dt_s = dt;
    c.seed = seed;
    ps_rng_seed(&c.rng, seed);
    ps_result result = api->create(&c);
    if (result != PS_OK) {
        report(interactive, c.error[0] ? c.error : ps_result_string(result));
        api->destroy(&c);
        ps_module_close(module);
        return 5;
    }
    if (ps_parameter_finalize(&c) != PS_OK) {
        report(interactive, "Unknown experiment parameter");
        api->destroy(&c);
        ps_module_close(module);
        return 5;
    }
    if (describe) {
        int wrote = printf("PHYSIM_PARAMETERS_1\n%u\n", c.parameter_count);
        for (uint32_t i = 0; wrote >= 0 && i < c.parameter_count; i++) {
            const ps_parameter *p = &c.parameters[i];
            wrote = printf("%s\t%.17g\t%.17g\t%.17g\t%s\n", p->name,
                           p->default_value, p->minimum, p->maximum, p->description);
        }
        int flushed = fflush(stdout);
        api->destroy(&c);
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
    snprintf(c.model_metadata + used, sizeof c.model_metadata - used,
             "\nmodule_fnv1a64=%016llx\nrunner_build=%s %s", (unsigned long long)hash, __DATE__,
             __TIME__);
    ps_run_writer writer;
    result = ps_run_create(&writer, argv[2], &c, api->name);
    if (result != PS_OK) {
        report(interactive, "Cannot create run (path missing or file already exists)");
        api->destroy(&c);
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
            if (!single && (!handshake || !ps_pacer_due(&pacer, now, dt))) {
                ps_sleep(1);
                continue;
            }
        } else if (tick >= steps)
            break;
        result = api->step(&c, dt);
        if (result != PS_OK)
            break;
        tick++;
        c.time_s = (double)tick * dt;
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
        report(interactive, c.error[0] ? c.error : ps_result_string(result));
    if (interactive)
        send_message(PS_MSG_BYE, NULL, 0);
    else
        printf("%llu samples written to %s\n", (unsigned long long)(tick + 1), argv[2]);
    api->destroy(&c);
    ps_module_close(module);
    return result == PS_OK ? 0 : 7;
}
