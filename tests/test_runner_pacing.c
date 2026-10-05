#include "physim/data.h"
#include "platform.h"
#include "protocol.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "Runner pacing line %d: %s\n", __LINE__, #x); return 1; } } while (0)
static bool send(ps_process *child, uint32_t *sequence, uint32_t type, double speed) {
    unsigned char frame[32], payload[8];
    uint32_t n = 0;
    if (type == PS_MSG_HELLO) { ps_put_u32(payload, PS_ABI_VERSION); n = 4; }
    if (type == PS_MSG_SPEED) { ps_put_f64(payload, speed); n = 8; }
    size_t size = ps_wire_encode(frame, type, (*sequence)++, payload, n);
    return size && ps_process_write(child, frame, size);
}
static int finish(ps_process *child, double deadline) {
    char bytes[4096];
    while (ps_process_poll(child) && ps_clock() < deadline) {
        while (ps_process_read(child, bytes, sizeof bytes) > 0) {}
        ps_sleep(1);
    }
    if (child->running) ps_process_kill(child);
    int code = child->exit_code;
    ps_process_close(child);
    return code;
}
static int reference(const char *runner, const char *module, const char *path, const char *work) {
    const char *args[] = {runner, module, path, "--steps", "200", "--dt", "0.005", "--seed", "42", NULL};
    ps_process child = {0};
    CHECK(ps_process_start(&child, args, work));
    CHECK(finish(&child, ps_clock() + 15) == 0);
    return 0;
}
static int compare(const char *actual, const char *reference_path) {
    ps_run_reader a, b;
    CHECK(ps_run_open(&a, actual) == PS_OK);
    CHECK(ps_run_open(&b, reference_path) == PS_OK);
    CHECK(a.channels == b.channels && a.channels > 0);
    CHECK(strstr(a.metadata, "\ndt_s=0.0050000000000000001\n") && strstr(a.metadata, "\nseed=42\n"));
    for (unsigned sample = 0; sample <= 200; sample++) {
        double ta, tb, va[PS_MAX_CHANNELS], vb[PS_MAX_CHANNELS];
        CHECK(ps_run_next(&a, &ta, va) == PS_OK && ps_run_next(&b, &tb, vb) == PS_OK);
        CHECK(ta == tb && ta == sample * .005);
        for (uint32_t channel = 0; channel < a.channels; channel++) CHECK(va[channel] == vb[channel]);
    }
    ps_run_reader_close(&a);
    ps_run_reader_close(&b);
    return 0;
}
static int interactive(const char *runner, const char *module, const char *path, const char *work,
                        const char *speed, bool slow_reader, double *duration) {
    const char *args[] = {runner, module, path, "--interactive", "--dt", "0.005", "--seed", "42", "--speed", speed, NULL};
    ps_process child = {0};
    CHECK(ps_process_start(&child, args, work));
    ps_wire_buffer wire = {0};
    uint32_t sequence = 0;
    int stage = 0;
    double deadline = ps_clock() + 12, running_at = 0, paused_at = 0, held_time = 0, next_read = 0;
    bool ok = true;
    while (ok && ps_process_poll(&child) && ps_clock() < deadline && stage < 5) {
        double now = ps_clock();
        if (stage == 3 && now - paused_at >= .15) {
            ok = send(&child, &sequence, PS_MSG_SPEED, 0) && send(&child, &sequence, PS_MSG_STEP, 0);
            stage = 4;
        }
        if (slow_reader && stage == 1 && now < next_read) { ps_sleep(1); continue; }
        int got = ps_process_read(&child, wire.data + wire.used, sizeof wire.data - wire.used);
        if (got < 0) { ok = false; break; }
        wire.used += (size_t)got;
        uint32_t type, n;
        const unsigned char *p;
        int status;
        while ((status = ps_wire_peek(&wire, &type, &p, &n)) > 0) {
            if (type == PS_MSG_HELLO && stage == 0)
                ok = send(&child, &sequence, PS_MSG_HELLO, 0);
            else if (type == PS_MSG_SNAPSHOT) {
                double time, values[PS_MAX_CHANNELS];
                uint32_t count;
                ps_scene scene;
                bool paused;
                ok = ps_snapshot_decode(p, n, &time, values, &count, &scene, &paused);
                if (!ok) break;
                if (stage == 0) {
                    ok = time == 0 && paused && scene.count > 0;
                    running_at = now;
                    ok &= send(&child, &sequence, PS_MSG_RUN, 0);
                    if (slow_reader) ok &= send(&child, &sequence, PS_MSG_SPEED, 4);
                    stage = 1;
                } else if (stage == 1 && time >= 1) {
                    *duration = now - running_at;
                    ok = !paused && send(&child, &sequence, PS_MSG_PAUSE, 0);
                    stage = 2;
                } else if (stage == 2 && paused) {
                    held_time = time;
                    paused_at = now;
                    /* A live speed change is valid while paused, without resuming. */
                    ok = send(&child, &sequence, PS_MSG_SPEED, 8);
                    stage = 3;
                } else if (stage == 3) {
                    ok = paused && time == held_time;
                } else if (stage == 4) {
                    ok = paused && fabs(time - held_time - .005) < 1e-10;
                    ok &= send(&child, &sequence, PS_MSG_STOP, 0);
                    stage = 5;
                }
            } else if (type == PS_MSG_ERROR || type == PS_MSG_BYE) {
                fprintf(stderr, "Unexpected message %u in stage %d: %.*s\n", type, stage, (int)n, p);
                ok = false;
            }
            ps_wire_consume(&wire, n);
            if (!ok) break;
        }
        if (status < 0) ok = false;
        if (slow_reader && stage == 1) next_read = now + .1;
        ps_sleep(1);
    }
    if (!ok || stage != 5) {
        fprintf(stderr, "Interactive %s slow=%d stopped in stage %d\n", speed, slow_reader, stage);
        ps_process_kill(&child);
    }
    int code = finish(&child, deadline);
    CHECK(ok && stage == 5 && code == 0);
    return 0;
}
static int invalid(const char *runner, const char *module, const char *work) {
    const char *bad[] = {"", "nan", "inf", "-1", "0.01", "16.1", "1suffix"};
    for (unsigned i = 0; i < sizeof bad / sizeof *bad; i++) {
        const char *args[] = {runner, module, "unused.psrun", "--interactive", "--speed", bad[i], NULL};
        ps_process child = {0};
        CHECK(ps_process_start(&child, args, work));
        CHECK(finish(&child, ps_clock() + 3) == 2);
    }
    /* Pacing flags are rejected for the already-offline CLI batch mode. */
    const char *args[] = {runner, module, "unused.psrun", "--speed", "1", NULL};
    ps_process child = {0};
    CHECK(ps_process_start(&child, args, work));
    CHECK(finish(&child, ps_clock() + 3) == 2);
    return 0;
}
static bool next_frame(ps_process *child, ps_wire_buffer *wire, uint32_t *type,
                       double *time, bool *paused, double deadline) {
    while (ps_process_poll(child) && ps_clock() < deadline) {
        const unsigned char *p;
        uint32_t n;
        int status = ps_wire_peek(wire, type, &p, &n);
        if (status < 0) return false;
        if (status > 0) {
            bool valid = true;
            if (*type == PS_MSG_SNAPSHOT) {
                double values[PS_MAX_CHANNELS]; uint32_t count; ps_scene scene;
                valid = ps_snapshot_decode(p, n, time, values, &count, &scene, paused);
            }
            ps_wire_consume(wire, n);
            if (*type != PS_MSG_HEARTBEAT) return valid;
            continue;
        }
        int got = ps_process_read(child, wire->data + wire->used, sizeof wire->data - wire->used);
        if (got < 0) return false;
        wire->used += (size_t)got;
        ps_sleep(1);
    }
    return false;
}
static int large_step(const char *runner, const char *module, const char *work) {
    char path[4096];
    snprintf(path, sizeof path, "%s/pacing-large-dt-%.0f.psrun", work, ps_clock() * 1e9);
    const char *args[] = {runner, module, path, "--interactive", "--dt", "1", "--speed", "0.1", NULL};
    ps_process child = {0};
    CHECK(ps_process_start(&child, args, work));
    ps_wire_buffer wire = {0}; uint32_t type, sequence = 0; double time; bool paused;
    double deadline = ps_clock() + 3;
    CHECK(next_frame(&child, &wire, &type, &time, &paused, deadline) && type == PS_MSG_HELLO);
    CHECK(send(&child, &sequence, PS_MSG_HELLO, 0));
    CHECK(next_frame(&child, &wire, &type, &time, &paused, deadline) &&
          type == PS_MSG_SNAPSHOT && paused && time == 0);
    CHECK(send(&child, &sequence, PS_MSG_RUN, 0));
    CHECK(next_frame(&child, &wire, &type, &time, &paused, deadline) &&
          type == PS_MSG_SNAPSHOT && !paused && time == 0);
    CHECK(send(&child, &sequence, PS_MSG_PAUSE, 0));
    CHECK(next_frame(&child, &wire, &type, &time, &paused, deadline) &&
          type == PS_MSG_SNAPSHOT && paused && time == 0);
    CHECK(send(&child, &sequence, PS_MSG_STEP, 0));
    CHECK(next_frame(&child, &wire, &type, &time, &paused, deadline) &&
          type == PS_MSG_SNAPSHOT && paused && time == 1);
    CHECK(send(&child, &sequence, PS_MSG_STOP, 0));
    CHECK(finish(&child, deadline) == 0);
    return 0;
}
static int invalid_commands(const char *runner, const char *module, const char *work) {
    const double bad[] = {NAN, INFINITY, -1, 17, 1, 1};
    for (unsigned i = 0; i < sizeof bad / sizeof *bad; i++) {
        char path[4096];
        snprintf(path, sizeof path, "%s/pacing-invalid-%u-%.0f.psrun", work, i, ps_clock() * 1e9);
        const char *args[] = {runner, module, path, "--interactive", NULL};
        ps_process child = {0};
        CHECK(ps_process_start(&child, args, work));
        ps_wire_buffer wire = {0};
        double deadline = ps_clock() + 3;
        bool hello = false;
        while (ps_process_poll(&child) && ps_clock() < deadline && !hello) {
            int got = ps_process_read(&child, wire.data + wire.used, sizeof wire.data - wire.used);
            CHECK(got >= 0);
            wire.used += (size_t)got;
            uint32_t type, n;
            const unsigned char *p;
            if (ps_wire_peek(&wire, &type, &p, &n) == 1) hello = type == PS_MSG_HELLO;
            ps_sleep(1);
        }
        CHECK(hello);
        uint32_t sequence = 0;
        /* Last case deliberately sends the new command before negotiation. */
        if (i != 5) CHECK(send(&child, &sequence, PS_MSG_HELLO, 0));
        unsigned char frame[32], payload[8];
        ps_put_f64(payload, bad[i]);
        size_t size = ps_wire_encode(frame, PS_MSG_SPEED, sequence, payload, i == 4 ? 7 : 8);
        CHECK(ps_process_write(&child, frame, size));
        CHECK(finish(&child, deadline) == 7);
        ps_run_reader reader;
        CHECK(ps_run_open(&reader, path) == PS_OK);
        double time, values[PS_MAX_CHANNELS];
        CHECK(ps_run_next(&reader, &time, values) == PS_OK && time == 0);
        CHECK(ps_run_next(&reader, &time, values) == PS_RECOVERED);
        ps_run_reader_close(&reader);
    }
    return 0;
}
int main(int argc, char **argv) {
    CHECK(argc == 4);
    CHECK(!invalid(argv[1], argv[2], argv[3]));
    CHECK(!invalid_commands(argv[1], argv[2], argv[3]));
    CHECK(!large_step(argv[1], argv[2], argv[3]));
    char base[4096], paths[4][4096];
    unsigned long long stamp = (unsigned long long)(ps_clock() * 1e9);
    snprintf(base, sizeof base, "%s/pacing-reference-%llu.psrun", argv[3], stamp);
    CHECK(!reference(argv[1], argv[2], base, argv[3]));
    const char *speeds[] = {"0.5", "4", "0", "0.5"};
    double durations[4] = {0};
    for (unsigned i = 0; i < 4; i++) {
        snprintf(paths[i], sizeof paths[i], "%s/pacing-%u-%llu.psrun", argv[3], i, stamp);
        CHECK(!interactive(argv[1], argv[2], paths[i], argv[3], speeds[i], i == 3, &durations[i]));
        CHECK(!compare(paths[i], base));
    }
    printf("Wall seconds: 0.5x=%.3f, 4x=%.3f, offline=%.3f, slow reader=%.3f\n", durations[0], durations[1], durations[2], durations[3]);
    CHECK(durations[0] >= 1.5 && durations[0] > 3 * durations[1]);
    CHECK(durations[1] >= .12 && durations[2] < durations[0] * .5);
    CHECK(durations[3] < durations[0] * .7);
    puts("Paced and offline data match every reference channel; pause, live speed, single step and slow reader passed.");
    return 0;
}
