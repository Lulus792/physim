#include "physim/experiment.h"
#include "physim/data.h"
#include "platform.h"
#include "protocol.h"
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
static uint32_t sequence;
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
static ps_result snapshot(const ps_experiment_api *api, ps_context *c, bool paused) {
    ps_scene scene = {0};
    c->error[0] = 0;
    api->build_scene(c, &scene);
    if (c->error[0])
        return PS_NUMERIC;
    unsigned char p[PS_WIRE_MAX];
    size_t n = ps_snapshot_encode(p, c, &scene, paused);
    return n && send_message(PS_MSG_SNAPSHOT, p, (uint32_t)n) ? PS_OK : PS_IO;
}
int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: physim-runner module output.psrun [--steps N | --interactive] "
                        "[--dt seconds] [--seed N] [--param name=value]...\n"
                        "       physim-runner module --describe\n");
        return 2;
    }
    bool describe = argc == 3 && !strcmp(argv[2], "--describe");
    bool interactive = false;
    uint64_t steps = 4000, seed = 42;
    double dt = 0.005;
    ps_context c = {0};
    c.struct_size = sizeof c;
    c.api_version = PS_API_VERSION;
    for (int i = 3; i < argc; i++) {
        char *end = NULL;
        if (!strcmp(argv[i], "--interactive")) {
            interactive = true;
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
        else if (!strcmp(key, "--steps"))
            steps = strtoull(argv[i], &end, 10);
        else if (!strcmp(key, "--seed"))
            seed = strtoull(argv[i], &end, 10);
        else
            return 2;
        if (errno || !end || *end || argv[i][0] == '-')
            return 2;
    }
    if (!isfinite(dt) || dt <= 0 || dt > 1 || steps > UINT64_C(1000000000))
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
    bool paused = true, stop = false, handshake = false;
    uint64_t tick = 0;
    ps_wire_buffer wire = {0};
    double next = ps_clock(), last_frame = next, last_heartbeat = next, start = next;
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
                    result = snapshot(api, &c, paused);
                    if (result != PS_OK)
                        break;
                } else if (!handshake || n != 0) {
                    result = PS_VERSION;
                    break;
                } else if (type == PS_MSG_RUN) {
                    paused = false;
                    next = now;
                } else if (type == PS_MSG_PAUSE) {
                    paused = true;
                    result = snapshot(api, &c, true);
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
            if (!single && (paused || !handshake || now < next)) {
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
            ps_result scene_result = snapshot(api, &c, paused);
            if (scene_result != PS_OK) {
                result = scene_result;
                break;
            }
            last_frame = now;
        }
        next += dt;
        if (next < now - 0.25)
            next = now;
    }
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
