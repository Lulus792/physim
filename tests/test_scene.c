#include "protocol.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Scene test line %d: %s\n", __LINE__, #x);                             \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
int main(void) {
    ps_context context = {0};
    context.time_s = 1.25;
    context.channel_count = 16;
    for (unsigned i = 0; i < 16; i++)
        context.values[i] = (double)i * .5;
    ps_scene scene = {0};
    ps_vec3 points[PS_MAX_SCENE_POINTS];
    for (unsigned i = 0; i < PS_MAX_SCENE_POINTS; i++)
        points[i] = ps_v3(i * .1, sin(i * .1), 0);
    CHECK(ps_scene_polyline(&scene, points, PS_MAX_SCENE_POINTS, .02, 0x123456ff) == PS_OK);
    CHECK(ps_scene_label(&scene, ps_v3(1, 2, 3), "K\xc3\xb6rper", 0xffabcdee) == PS_OK);
    for (unsigned i = scene.count; i < PS_MAX_OBJECTS; i++) {
        ps_object o = {0};
        o.shape = i % 6;
        o.a = ps_v3(i, 0, 0);
        o.b = ps_v3(1, 2, 3);
        o.radius = .1;
        o.orientation = ps_quat_axis_angle(ps_v3(0, 1, 0), .4);
        o.color = 0xffffffff;
        o.id = i + 100;
        CHECK(ps_scene_push(&scene, &o) == PS_OK);
    }
    CHECK(ps_scene_valid(&scene));
    ps_scene duplicate = scene;
    duplicate.objects[0].id = scene.objects[2].id;
    CHECK(!ps_scene_valid(&duplicate));
    ps_scene unchanged = scene;
    CHECK(ps_scene_polyline(&scene, points, 2, .1, 0) == PS_INVALID);
    CHECK(ps_scene_label(&scene, ps_v3(0, 0, 0), "overflow", 0) == PS_INVALID);
    CHECK(!memcmp(&scene, &unchanged, sizeof scene));
    unsigned char payload[PS_WIRE_MAX], damaged[PS_WIRE_MAX];
    size_t size = ps_snapshot_encode(payload, &context, &scene, true);
    CHECK(size == 7960 && size <= PS_WIRE_MAX);
    CHECK(ps_get_u32(payload + 20) == 96);
    size_t base = 24 + 16 * 8;
    CHECK(payload[base] == PS_POLYLINE && payload[base + 4] == 0xff && payload[base + 5] == 0x56);
    CHECK(payload[base + 94] == 0xf0 && payload[base + 95] == 0x3f); /* identity quaternion w */
    CHECK(!memcmp(payload + base + PS_WIRE_OBJECT_SIZE + 96, "K\xc3\xb6rper", 7));
    ps_scene decoded = {0};
    double time = -7, values[PS_MAX_CHANNELS] = {0};
    uint32_t count = 99;
    bool paused = false;
    CHECK(ps_snapshot_decode(payload, (uint32_t)size, &time, values, &count, &decoded, &paused));
    CHECK(time == 1.25 && paused && count == 16 && decoded.count == 32 &&
          decoded.point_count == 96);
    CHECK(!strcmp(decoded.objects[1].text, "K\xc3\xb6rper"));
    CHECK(decoded.objects[0].point_count == 96 && decoded.points[95].x == points[95].x);
    CHECK(fabs(decoded.objects[2].orientation.y - sin(.2)) < 1e-14);
    CHECK(decoded.objects[2].id == 102 &&
          ps_get_u32(payload + base + 2 * PS_WIRE_OBJECT_SIZE + 168) == 102);
    memcpy(damaged, payload, size);
    ps_put_u32(damaged + base + 168, 102);
    CHECK(!ps_snapshot_decode(damaged, (uint32_t)size, &time, values, &count, &decoded, &paused));
    unchanged = decoded;
    for (uint32_t n = 0; n < size; n++) {
        CHECK(!ps_snapshot_decode(payload, n, &time, values, &count, &decoded, &paused));
        CHECK(!memcmp(&decoded, &unchanged, sizeof decoded) && count == 16 && time == 1.25 &&
              paused);
    }
    const size_t fields[] = {16, 20, base, base + 160, base + 164};
    for (size_t i = 0; i < sizeof fields / sizeof *fields; i++) {
        memcpy(damaged, payload, size);
        ps_put_u32(damaged + fields[i], UINT32_MAX);
        CHECK(
            !ps_snapshot_decode(damaged, (uint32_t)size, &time, values, &count, &decoded, &paused));
    }
    memcpy(damaged, payload, size);
    memset(damaged + base + PS_WIRE_OBJECT_SIZE + 96, 'x', 64);
    CHECK(!ps_snapshot_decode(damaged, (uint32_t)size, &time, values, &count, &decoded, &paused));
    memcpy(damaged, payload, size);
    damaged[base + PS_WIRE_OBJECT_SIZE + 96] = 0xc0;
    damaged[base + PS_WIRE_OBJECT_SIZE + 97] = 0x80;
    CHECK(!ps_snapshot_decode(damaged, (uint32_t)size, &time, values, &count, &decoded, &paused));
    memcpy(damaged, payload, size);
    ps_put_f64(damaged + size - 8, NAN);
    CHECK(!ps_snapshot_decode(damaged, (uint32_t)size, &time, values, &count, &decoded, &paused));
    ps_scene empty = {0};
    ps_object named = {.shape = PS_SPHERE, .radius = .1, .id = UINT32_MAX};
    CHECK(ps_scene_push(&empty, &named) == PS_OK);
    CHECK(ps_scene_push(&empty, &named) == PS_INVALID && empty.count == 1);
    empty = (ps_scene){0};
    CHECK(ps_scene_add_id(&empty, 7, PS_SPHERE, ps_v3(0, 0, 0), ps_v3(0, 0, 0), .1, 0) == PS_OK);
    ps_scene before_ids = empty;
    CHECK(ps_scene_polyline_id(&empty, 7, points, 2, .1, 0) == PS_INVALID);
    CHECK(ps_scene_label_id(&empty, 7, ps_v3(0, 0, 0), "duplicate", 0) == PS_INVALID);
    CHECK(!memcmp(&empty, &before_ids, sizeof empty));
    CHECK(ps_scene_polyline_id(&empty, 8, points, 2, .1, 0) == PS_OK);
    CHECK(ps_scene_label_id(&empty, 9, ps_v3(0, 0, 0), "named", 0) == PS_OK);
    CHECK(empty.objects[1].id == 8 && empty.objects[2].id == 9 && ps_scene_valid(&empty));
    empty = (ps_scene){0};
    CHECK(ps_scene_label(&empty, ps_v3(0, 0, 0), "bad\xed\xa0\x80", 0) == PS_INVALID);
    CHECK(ps_scene_label(&empty, ps_v3(0, 0, 0), "bad\xf4\x90\x80\x80", 0) == PS_INVALID);
    CHECK(ps_scene_label(&empty, ps_v3(0, 0, 0), "line\nbreak", 0) == PS_INVALID);
    CHECK(empty.count == 0);
    ps_vec3 bad[2] = {{0, 0, 0}, {NAN, 0, 0}};
    CHECK(ps_scene_polyline(&empty, bad, 2, .1, 0) == PS_INVALID && empty.point_count == 0);
    ps_rng rng;
    ps_rng_seed(&rng, 53);
    for (unsigned i = 0; i < 5000; i++) {
        memcpy(damaged, payload, size);
        for (unsigned j = 0; j < 3; j++)
            damaged[ps_rng_u32(&rng) % size] ^= (unsigned char)(1u << (ps_rng_u32(&rng) % 8));
        if (ps_snapshot_decode(damaged, (uint32_t)size, &time, values, &count, &decoded, &paused))
            CHECK(ps_scene_valid(&decoded));
    }
    ps_wire_buffer wire = {0};
    wire.used = ps_wire_encode(wire.data, PS_MSG_HELLO, 0, NULL, 0);
    ps_put_u32(wire.data + 4, 2);
    uint32_t type, length;
    const unsigned char *body;
    CHECK(ps_wire_peek(&wire, &type, &body, &length) == -1);
    puts("Scene API, maximal snapshot, truncation, malformed UTF-8, ranges and mutations passed.");
    return 0;
}
