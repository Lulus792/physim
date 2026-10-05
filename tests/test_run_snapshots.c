#include "physim/data.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "Run snapshots line %d: %s\n", __LINE__, #x); return 1; } } while (0)
static ps_context context;
static ps_scene scene;
static bool read_all(const char *path) {
    ps_run_reader r;
    if (ps_run_open(&r, path) != PS_OK) return false;
    bool ok = true;
    for (unsigned i = 0; i < 3; i++) {
        double time, values[PS_MAX_CHANNELS];
        ok &= ps_run_next(&r, &time, values) == PS_OK && time == i * .01;
        for (unsigned j = 0; j < PS_MAX_CHANNELS; j++) ok &= values[j] == i + .1 * j;
    }
    double time, values[PS_MAX_CHANNELS];
    ok &= ps_run_next(&r, &time, values) == PS_EOF && r.samples == 3;
    ps_run_reader_close(&r);
    return ok;
}
static bool copy_mutate(const char *source, const char *target, unsigned mode) {
    FILE *in = fopen(source, "rb"), *out = fopen(target, "wbx");
    if (!in || !out) { if (in) fclose(in); if (out) fclose(out); return false; }
    unsigned char header[16], payload[PS_SNAPSHOT_MAX];
    bool ok = fread(header, 1, 16, in) == 16 && fwrite(header, 1, 16, out) == 16, changed = false;
    while (ok && fread(header, 1, 12, in) == 12) {
        uint32_t type = ps_get_u32(header), n = ps_get_u32(header + 4);
        if (n > sizeof payload || fread(payload, 1, n, in) != n) { ok = false; break; }
        if (type == 5 && !changed) {
            if (mode == 1) ps_put_u32(payload, PS_SNAPSHOT_VERSION + 1);
            if (mode == 2) ps_put_u32(payload + 4 + 12, PS_MAX_OBJECTS + 1);
            if (mode == 3) ps_put_u32(payload + 4 + 16, 2);
            if (mode == 4) payload[4] ^= 1;
            if (mode != 4) ps_put_u32(header + 8, ps_crc32(payload, n));
            changed = true;
        }
        if (type == 4 && mode == 5) n = 1; /* Incomplete footer. */
        ok = fwrite(header, 1, 12, out) == 12 && fwrite(payload, 1, n, out) == n;
    }
    if (ferror(in)) ok = false;
    fclose(in);
    if (fclose(out)) ok = false;
    return ok;
}
int main(int argc, char **argv) {
    CHECK(argc == 2);
    context.struct_size = sizeof context;
    context.api_version = PS_API_VERSION;
    context.dt_s = .01;
    for (unsigned i = 0; i < PS_MAX_CHANNELS; i++) {
        char name[48]; snprintf(name, sizeof name, "channel_%u", i);
        CHECK(ps_channel_add(&context, name, PS_METRE, "all channels") == (int)i);
    }
    ps_vec3 points[PS_MAX_SCENE_POINTS];
    for (unsigned i = 0; i < PS_MAX_SCENE_POINTS; i++) points[i] = ps_v3(i * .1, sin(i), -(double)i * .02);
    CHECK(ps_scene_polyline_id(&scene, 1, points, PS_MAX_SCENE_POINTS, .01, 0x12345678) == PS_OK);
    CHECK(ps_scene_label_id(&scene, 2, ps_v3(1, 2, 3), "Körper α", 0xabcdef80) == PS_OK);
    for (unsigned i = 2; i < PS_MAX_OBJECTS; i++) {
        ps_object object = {.shape = i % 6, .color = 0x123456ff, .radius = .1, .id = i + 1};
        object.a = ps_v3(i, 2, 3); object.b = ps_v3(1, 2, 3);
        object.orientation = ps_quat_axis_angle(ps_v3(0, 1, 0), .3);
        CHECK(ps_scene_push(&scene, &object) == PS_OK);
    }
    char path[4096], legacy[4096];
    snprintf(path, sizeof path, "%s/scenes.psrun", argv[1]);
    snprintf(legacy, sizeof legacy, "%s/legacy.psrun", argv[1]);
    ps_run_writer w;
    CHECK(ps_run_create(&w, path, &context, "scene archive") == PS_OK);
    for (unsigned i = 0; i < 3; i++) {
        context.time_s = i * .01;
        for (unsigned j = 0; j < PS_MAX_CHANNELS; j++) context.values[j] = i + .1 * j;
        CHECK(ps_run_append(&w, context.time_s, context.values) == PS_OK);
        long at = ftell(w.file);
        context.values[0] = NAN;
        CHECK(ps_run_append_snapshot(&w, &context, &scene, true) == PS_INVALID && ftell(w.file) == at);
        context.values[0] = i;
        CHECK(ps_run_append_snapshot(&w, &context, &scene, i != 1) == PS_OK && w.samples == i + 1);
    }
    CHECK(ps_run_close(&w) == PS_OK && read_all(path));
    ps_run_reader r;
    CHECK(ps_run_open(&r, path) == PS_OK);
    for (unsigned i = 0; i < 3; i++) {
        ps_snapshot s;
        CHECK(ps_run_snapshot_next(&r, &s) == PS_OK && s.time == i * .01 && s.count == PS_MAX_CHANNELS);
        CHECK(s.paused == (i != 1) && s.scene.count == PS_MAX_OBJECTS && s.scene.point_count == PS_MAX_SCENE_POINTS);
        context.time_s = i * .01;
        for (unsigned j = 0; j < PS_MAX_CHANNELS; j++) { CHECK(s.values[j] == i + .1 * j); context.values[j] = s.values[j]; }
        unsigned char a[PS_SNAPSHOT_MAX], b[PS_SNAPSHOT_MAX];
        size_t size = ps_snapshot_encode(a, &context, &scene, s.paused);
        CHECK(size == ps_snapshot_encode(b, &context, &s.scene, s.paused) && !memcmp(a, b, size));
    }
    ps_snapshot s = {0};
    CHECK(ps_run_snapshot_next(&r, &s) == PS_EOF && r.samples == 3);
    ps_run_reader_close(&r);
    CHECK(ps_run_create(&w, legacy, &context, "legacy") == PS_OK);
    for (unsigned i = 0; i < 3; i++) {
        for (unsigned j = 0; j < PS_MAX_CHANNELS; j++) context.values[j] = i + .1 * j;
        CHECK(ps_run_append(&w, i * .01, context.values) == PS_OK);
    }
    CHECK(ps_run_close(&w) == PS_OK && read_all(legacy));
    CHECK(ps_run_open(&r, legacy) == PS_OK && ps_run_snapshot_next(&r, &s) == PS_EOF && r.samples == 3);
    ps_run_reader_close(&r);
    for (unsigned mode = 1; mode <= 5; mode++) {
        char altered[4096]; snprintf(altered, sizeof altered, "%s/altered-%u.psrun", argv[1], mode);
        CHECK(copy_mutate(path, altered, mode));
        if (mode <= 3) CHECK(read_all(altered)); /* Numeric data survives unknown/invalid scene semantics. */
        CHECK(ps_run_open(&r, altered) == PS_OK);
        memset(&s, 0xa5, sizeof s); ps_snapshot before = s;
        if (mode == 5) {
            for (unsigned i = 0; i < 3; i++) CHECK(ps_run_snapshot_next(&r, &s) == PS_OK);
            CHECK(ps_run_snapshot_next(&r, &s) == PS_RECOVERED && r.samples == 3);
        } else {
            CHECK(ps_run_snapshot_next(&r, &s) == (mode == 1 ? PS_VERSION : mode == 4 ? PS_RECOVERED : PS_CORRUPT));
            CHECK(!memcmp(&s, &before, sizeof s));
        }
        ps_run_reader_close(&r);
    }
    puts("Versioned scene chunks, full geometry/channel round trips, old readers, footer counts and recovery passed.");
    return 0;
}
