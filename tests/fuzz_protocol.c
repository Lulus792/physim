#include "protocol.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Shared invariant harness: standalone deterministic campaign or libFuzzer entry. */
static const unsigned char *current_input;
static size_t current_size;
static unsigned long long accepted, rejected;
static void require(bool condition) {
    if (condition)
        return;
#ifdef PS_FUZZ_STANDALONE
    FILE *file = fopen("protocol-failure.bin", "wb");
    if (file) {
        fwrite(current_input, 1, current_size, file);
        fclose(file);
    }
#endif
    fputs("Protocol fuzz invariant failed; replay protocol-failure.bin\n", stderr);
    abort();
}
int LLVMFuzzerTestOneInput(const unsigned char *data, size_t size) {
    if (!size || size > PS_WIRE_MAX + PS_WIRE_HEADER + 1)
        return 0;
    current_input = data;
    current_size = size;
    if (data[0] & 1) {
        ps_scene scene, before;
        memset(&scene, 0xa5, sizeof scene);
        before = scene;
        double time = -123, values[PS_MAX_CHANNELS], saved[PS_MAX_CHANNELS];
        for (unsigned i = 0; i < PS_MAX_CHANNELS; i++)
            values[i] = saved[i] = -456.0 - i;
        uint32_t count = 999;
        bool paused = false;
        bool ok = ps_snapshot_decode(data + 1, (uint32_t)(size - 1), &time, values, &count, &scene,
                                     &paused);
        if (!ok) {
            rejected++;
            require(time == -123 && count == 999 && !paused);
            require(!memcmp(values, saved, sizeof values) &&
                    !memcmp(&scene, &before, sizeof scene));
        } else {
            accepted++;
            require(isfinite(time) && count <= PS_MAX_CHANNELS && ps_scene_valid(&scene));
            ps_context context = {0};
            context.time_s = time;
            context.channel_count = count;
            for (unsigned i = 0; i < count; i++) {
                require(isfinite(values[i]));
                context.values[i] = values[i];
            }
            for (unsigned i = count; i < PS_MAX_CHANNELS; i++)
                require(values[i] == saved[i]);
            unsigned char encoded[PS_WIRE_MAX];
            size_t n = ps_snapshot_encode(encoded, &context, &scene, paused);
            require(n > 0 && n <= PS_WIRE_MAX);
            ps_scene again = {0};
            double t, v[PS_MAX_CHANNELS];
            uint32_t c;
            bool p;
            require(ps_snapshot_decode(encoded, (uint32_t)n, &t, v, &c, &again, &p));
            require(t == time && c == count && p == paused);
            require(!memcmp(v, values, count * sizeof *v));
            /* Ignore permitted noncanonical bytes after label terminators. */
            unsigned char canonical[PS_WIRE_MAX];
            require(ps_snapshot_encode(canonical, &context, &again, p) == n &&
                    !memcmp(encoded, canonical, n));
        }
    } else {
        ps_wire_buffer buffer = {0};
        buffer.used = size - 1;
        memcpy(buffer.data, data + 1, buffer.used);
        if (buffer.used >= 20)
            buffer.sequence = ps_get_u32(buffer.data + 16);
        ps_wire_buffer before = buffer;
        uint32_t type = 999, length = 999;
        const unsigned char *payload = NULL;
        int result = ps_wire_peek(&buffer, &type, &payload, &length);
        require(!memcmp(&buffer, &before, sizeof buffer));
        if (result == 1) {
            accepted++;
            require(length <= PS_WIRE_MAX && payload == buffer.data + 20 &&
                    length + 20 <= buffer.used);
            ps_wire_consume(&buffer, length);
            require(buffer.used == before.used - 20 - length &&
                    buffer.sequence == before.sequence + 1);
            require(!memcmp(buffer.data, before.data + 20 + length, buffer.used));
            buffer = before;
            buffer.sequence++;
            require(ps_wire_peek(&buffer, &type, &payload, &length) == -1);
        } else {
            rejected++;
            require((result == 0 || result == -1) && type == 999 && length == 999 &&
                    payload == NULL);
        }
        /* Mutated lengths also exercise consume independently of peek success. */
        uint32_t bad = size >= 5 ? ps_get_u32(data + 1) : UINT32_MAX;
        buffer = before;
        if (bad > PS_WIRE_MAX || before.used < 20 || bad > before.used - 20) {
            ps_wire_consume(&buffer, bad);
            require(!memcmp(&buffer, &before, sizeof buffer));
        }
        const uint32_t extremes[] = {PS_WIRE_MAX + 1, UINT32_MAX - 19, UINT32_MAX};
        for (unsigned i = 0; i < sizeof extremes / sizeof *extremes; i++) {
            buffer = before;
            ps_wire_consume(&buffer, extremes[i]);
            require(!memcmp(&buffer, &before, sizeof buffer));
        }
        buffer.used = sizeof buffer.data + 1;
        before = buffer;
        require(ps_wire_peek(&buffer, &type, &payload, &length) == -1);
        ps_wire_consume(&buffer, 0);
        require(!memcmp(&buffer, &before, sizeof buffer));
    }
    return 0;
}
#ifdef PS_FUZZ_STANDALONE
int main(int argc, char **argv) {
    unsigned char seed[PS_WIRE_MAX + PS_WIRE_HEADER + 1] = {0}, mutated[sizeof seed];
    bool write_seeds = argc == 2 && !strcmp(argv[1], "--write-seeds");
    if (argc == 2 && !write_seeds) {
        FILE *f = fopen(argv[1], "rb");
        if (!f)
            return 2;
        size_t n = fread(seed, 1, sizeof seed, f);
        int extra = fgetc(f);
        fclose(f);
        if (extra != EOF)
            return 2;
        LLVMFuzzerTestOneInput(seed, n);
        puts("Protocol replay passed");
        return 0;
    }
    if (argc != 1 && !write_seeds)
        return 2;
    ps_context context = {0};
    context.time_s = 1.25;
    context.channel_count = PS_MAX_CHANNELS;
    for (unsigned i = 0; i < PS_MAX_CHANNELS; i++)
        context.values[i] = .25 * i;
    ps_scene scene = {0};
    ps_vec3 points[PS_MAX_SCENE_POINTS];
    for (unsigned i = 0; i < PS_MAX_SCENE_POINTS; i++)
        points[i] = ps_v3(i * .1, sin(i * .1), 0);
    require(ps_scene_polyline(&scene, points, PS_MAX_SCENE_POINTS, .02, 0xffffffff) == PS_OK);
    require(ps_scene_label(&scene, ps_v3(1, 2, 3), "K\xc3\xb6rper", 0xffffffff) == PS_OK);
    for (unsigned i = scene.count; i < PS_MAX_OBJECTS; i++) {
        ps_object object = {0};
        object.shape = i % 6;
        object.radius = .1;
        object.b = ps_v3(1, 2, 3);
        object.orientation.w = 1;
        object.id = i + 1;
        require(ps_scene_push(&scene, &object) == PS_OK);
    }
    seed[0] = 1;
    size_t n = 1 + ps_snapshot_encode(seed + 1, &context, &scene, true);
    require(n > 1);
    unsigned long long cases = 0;
    uint32_t rng = UINT32_C(0x70687973);
    for (unsigned mode = 0; mode < 2; mode++) {
        if (mode) {
            unsigned char snapshot[PS_WIRE_MAX];
            memcpy(snapshot, seed + 1, n - 1);
            n = 1 + ps_wire_encode(seed + 1, PS_MSG_SNAPSHOT, 42, snapshot, (uint32_t)n - 1);
            seed[0] = 0;
        }
        if (write_seeds) {
            FILE *file = fopen(mode ? "frame.seed" : "snapshot.seed", "wbx");
            if (!file) return 2;
            bool ok = fwrite(seed, 1, n, file) == n;
            if (fclose(file) || !ok) return 2;
            continue;
        }
        for (size_t cut = 1; cut <= n; cut++) {
            LLVMFuzzerTestOneInput(seed, cut);
            cases++;
        }
        for (size_t byte = 1; byte < n; byte++)
            for (unsigned bit = 0; bit < 8; bit++) {
                memcpy(mutated, seed, n);
                mutated[byte] ^= (unsigned char)(1u << bit);
                LLVMFuzzerTestOneInput(mutated, n);
                cases++;
            }
        for (unsigned trial = 0; trial < 20000; trial++) {
            memcpy(mutated, seed, n);
            for (unsigned k = 0; k < 4; k++) {
                rng = rng * 1664525u + 1013904223u;
                size_t offset = 1 + rng % (n - 1);
                rng = rng * 1664525u + 1013904223u;
                mutated[offset] = (unsigned char)(rng >> 24);
            }
            LLVMFuzzerTestOneInput(mutated, n);
            cases++;
        }
    }
    if (write_seeds) {
        puts("Valid snapshot and frame seeds written");
        return 0;
    }
    require(accepted > 100 && rejected > 100);
    printf(
        "Protocol mutation campaign: %llu cases, %llu accepted, %llu rejected; seed 0x70687973\n",
        cases, accepted, rejected);
    return 0;
}
#endif
