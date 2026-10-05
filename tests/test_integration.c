#include "platform.h"
#include "protocol.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define REQUIRE(x)                                                                                 \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x);                                   \
            ps_process_close(&p);                                                                  \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static bool send(ps_process *p, uint32_t type, uint32_t seq) {
    unsigned char b[32], payload[4];
    ps_put_u32(payload, PS_ABI_VERSION);
    size_t n = ps_wire_encode(b, type, seq, payload, type == PS_MSG_HELLO ? 4 : 0);
    return ps_process_write(p, b, n);
}
static int receive(ps_process *p, ps_wire_buffer *b, uint32_t wanted, double *time, bool *pause) {
    double until = ps_clock() + 5;
    while (ps_clock() < until) {
        int n = ps_process_read(p, b->data + b->used, sizeof b->data - b->used);
        if (n > 0)
            b->used += (size_t)n;
        uint32_t type, size;
        const unsigned char *payload;
        int status;
        while ((status = ps_wire_peek(b, &type, &payload, &size)) > 0) {
            bool found = type == wanted;
            if (found && type == PS_MSG_SNAPSHOT) {
                double values[PS_MAX_CHANNELS];
                uint32_t count;
                ps_scene scene;
                bool paused;
                if (!ps_snapshot_decode(payload, size, time, values, &count, &scene, &paused))
                    return -1;
                if (pause) *pause = paused;
            }
            ps_wire_consume(b, size);
            if (found)
                return 1;
        }
        if (status < 0)
            return -1;
        if (!ps_process_poll(p) && n <= 0)
            return -1;
        ps_sleep(1);
    }
    return 0;
}
int main(int argc, char **argv) {
    if (argc != 6)
        return 2;
    ps_process p = {0};
    ps_wire_buffer wire = {0};
    double t = 0;
    bool paused = false;
    remove("interactive.psrun");
    const char *args[] = {argv[1], argv[2], "interactive.psrun", "--interactive", NULL};
    REQUIRE(ps_process_start(&p, args, NULL));
    REQUIRE(receive(&p, &wire, PS_MSG_HELLO, &t, &paused) == 1);
    REQUIRE(send(&p, PS_MSG_HELLO, 0));
    REQUIRE(receive(&p, &wire, PS_MSG_SNAPSHOT, &t, &paused) == 1);
    REQUIRE(t == 0 && paused);
    REQUIRE(send(&p, PS_MSG_STEP, 1));
    REQUIRE(receive(&p, &wire, PS_MSG_SNAPSHOT, &t, &paused) == 1);
    REQUIRE(fabs(t - 0.005) < 1e-12);
    ps_sleep(30);
    REQUIRE(send(&p, PS_MSG_STEP, 2));
    REQUIRE(receive(&p, &wire, PS_MSG_SNAPSHOT, &t, &paused) == 1);
    REQUIRE(fabs(t - 0.01) < 1e-12);
    REQUIRE(send(&p, PS_MSG_RUN, 3));
    REQUIRE(receive(&p, &wire, PS_MSG_SNAPSHOT, &t, &paused) == 1);
    REQUIRE(fabs(t - 0.01) < 1e-12 && !paused); /* RUN acknowledges state before dt is due. */
    REQUIRE(receive(&p, &wire, PS_MSG_SNAPSHOT, &t, &paused) == 1);
    REQUIRE(t > 0.01 && !paused);
    REQUIRE(send(&p, PS_MSG_PAUSE, 4));
    double paused_until = ps_clock() + 5;
    do { REQUIRE(receive(&p, &wire, PS_MSG_SNAPSHOT, &t, &paused) == 1); }
    while (!paused && ps_clock() < paused_until);
    REQUIRE(paused);
    double held_time = t;
    ps_sleep(30);
    REQUIRE(send(&p, PS_MSG_PAUSE, 5));
    REQUIRE(receive(&p, &wire, PS_MSG_SNAPSHOT, &t, &paused) == 1);
    REQUIRE(paused && t == held_time);
    REQUIRE(send(&p, PS_MSG_STOP, 6));
    REQUIRE(receive(&p, &wire, PS_MSG_BYE, &t, &paused) == 1);
    double until = ps_clock() + 5;
    while (ps_process_poll(&p) && ps_clock() < until)
        ps_sleep(1);
    REQUIRE(!p.running && p.exit_code == 0);
    ps_process_close(&p);
    ps_run_reader reader;
    REQUIRE(ps_run_open(&reader, "interactive.psrun") == PS_OK);
    double values[PS_MAX_CHANNELS];
    ps_result result;
    while ((result = ps_run_next(&reader, &t, values)) == PS_OK) {
    }
    REQUIRE(result == PS_EOF && reader.samples >= 4);
    ps_run_reader_close(&reader);
    for (int mode = 0; mode < 3; mode++) {
        char file[64];
        snprintf(file, sizeof file, "failure-%d.psrun", mode);
        remove(file);
        const char *failure[] = {argv[1], argv[mode + 3], file, "--interactive", NULL};
        memset(&wire, 0, sizeof wire);
        REQUIRE(ps_process_start(&p, failure, NULL));
        if (mode < 2) {
            REQUIRE(receive(&p, &wire, PS_MSG_HELLO, &t, &paused) == 1);
            REQUIRE(send(&p, PS_MSG_HELLO, 0));
            REQUIRE(receive(&p, &wire, PS_MSG_SNAPSHOT, &t, &paused) == 1);
            REQUIRE(send(&p, PS_MSG_RUN, 1));
        }
        if (mode == 1) {
            ps_sleep(30);
            ps_process_kill(&p);
            REQUIRE(!p.running);
        } else {
            until = ps_clock() + 8;
            while (ps_process_poll(&p) && ps_clock() < until)
                ps_sleep(5);
            REQUIRE(!p.running && p.exit_code != 0);
        }
        ps_process_close(&p);
        if (mode < 2) {
            REQUIRE(ps_run_open(&reader, file) == PS_OK);
            REQUIRE(ps_run_next(&reader, &t, values) == PS_OK);
            REQUIRE(ps_run_next(&reader, &t, values) == PS_RECOVERED);
            ps_run_reader_close(&reader);
        }
        remove(file);
    }
    remove("interactive.psrun");
    puts("Handshake, pause/step/resume, crash isolation, hang termination and ABI rejection "
         "passed.");
    return 0;
}
