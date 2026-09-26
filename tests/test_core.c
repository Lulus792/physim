#include "physim/analysis.h"
#include "protocol.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #x);                           \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
#define NEAR(a, b, e) CHECK(fabs((a) - (b)) < (e))
static void oscillator(double t, const double *s, double *d, void *u) {
    (void)t;
    (void)u;
    d[0] = s[1];
    d[1] = -s[0];
}
static double solve(double dt) {
    double y[2] = {1, 0};
    int steps = (int)round(1 / dt);
    for (int i = 0; i < steps; i++)
        ps_ode_step(PS_RK4, oscillator, NULL, i * dt, dt, y, 2);
    return fabs(y[0] - cos(1));
}
int main(void) {
    ps_vec3 a = ps_vcross(ps_v3(1, 0, 0), ps_v3(0, 1, 0));
    NEAR(a.z, 1, 1e-15);
    a = ps_quat_rotate(ps_quat_axis_angle(ps_v3(0, 0, 1), PS_PI / 2), ps_v3(1, 0, 0));
    NEAR(a.y, 1, 1e-14);
    NEAR(a.x, 0, 1e-14);
    ps_mat4 m = ps_mat4_multiply(ps_mat4_identity(), ps_mat4_identity());
    NEAR(m.m[15], 1, 1e-15);
    double out;
    ps_unit cm = PS_METRE;
    cm.scale = 0.01;
    CHECK(ps_convert(100, cm, PS_METRE, &out) == PS_OK);
    NEAR(out, 1, 1e-15);
    CHECK(ps_convert(1, PS_METRE, PS_SECOND, &out) == PS_INVALID);
    ps_rng rng, rng2;
    ps_rng_seed(&rng, 42);
    ps_rng_seed(&rng2, 42);
    for (int i = 0; i < 10000; i++)
        CHECK(ps_rng_u32(&rng) == ps_rng_u32(&rng2));
    ps_statistics random = {0};
    for (int i = 0; i < 100000; i++)
        ps_statistics_push(&random, ps_rng_normal(&rng, 2, 3));
    NEAR(random.mean, 2, 0.04);
    NEAR(ps_statistics_stddev(&random), 3, 0.04);
    double coarse = solve(0.1), fine = solve(0.05);
    CHECK(coarse / fine > 14 && coarse / fine < 18);
    CHECK(fine < 1e-7);
    double q = 1, v = 0;
    for (int i = 0; i < 10000; i++)
        ps_symplectic_step(&q, &v, -q, 0.005);
    CHECK(fabs((q * q + v * v) / 2 - 0.5) < 0.002);
    ps_particle p = {ps_v3(-0.1, 0, 0), ps_v3(1, 0, 0), 1, 0.2},
                b = {ps_v3(0.1, 0, 0), ps_v3(-1, 0, 0), 1, 0.2};
    CHECK(ps_collide_spheres(&p, &b, 1));
    NEAR(p.velocity_m_s.x, -1, 1e-14);
    NEAR(b.velocity_m_s.x, 1, 1e-14);
    double x[] = {0, 1, 2}, y[] = {0, 1, 4}, d[3];
    CHECK(ps_derivative(x, y, 3, d) == PS_OK);
    NEAR(d[1], 2, 1e-15);
    NEAR(ps_trapezoid(x, y, 3), 3, 1e-15);
    CHECK(ps_crc32((const unsigned char *)"123456789", 9) == 0xcbf43926);
    unsigned char bytes[8];
    ps_put_f64(bytes, -123.456);
    NEAR(ps_get_f64(bytes), -123.456, 1e-14);
    const unsigned char one_le[8] = {0, 0, 0, 0, 0, 0, 0xf0, 0x3f};
    ps_put_f64(bytes, 1.0);
    CHECK(!memcmp(bytes, one_le, 8));
    ps_put_u32(bytes, 0x12345678);
    CHECK(bytes[0] == 0x78 && bytes[1] == 0x56 && bytes[2] == 0x34 && bytes[3] == 0x12);
    ps_context c = {0};
    c.dt_s = 0.01;
    c.seed = 42;
    ps_channel_add(&c, "angle,\"quoted", PS_RADIAN, "test");
    remove("test.psrun");
    remove("test.csv");
    ps_run_writer w;
    CHECK(ps_run_create(&w, "test.psrun", &c, "test") == PS_OK);
    double value[] = {3};
    CHECK(ps_run_append(&w, 0, value) == PS_OK);
    value[0] = 4;
    CHECK(ps_run_append(&w, 0.01, value) == PS_OK);
    CHECK(ps_run_close(&w) == PS_OK);
    CHECK(ps_run_create(&w, "test.psrun", &c, "test") == PS_IO);
    ps_run_reader r;
    CHECK(ps_run_open(&r, "test.psrun") == PS_OK);
    double t, values[PS_MAX_CHANNELS];
    CHECK(ps_run_next(&r, &t, values) == PS_OK);
    NEAR(values[0], 3, 1e-15);
    CHECK(ps_run_next(&r, &t, values) == PS_OK);
    NEAR(t, 0.01, 1e-15);
    CHECK(ps_run_next(&r, &t, values) == PS_EOF);
    ps_run_reader_close(&r);
    CHECK(ps_run_export_csv("test.psrun", "test.csv") == PS_OK);
    remove("quoted-report-summary.csv");
    remove("quoted-report-manifest.txt");
    remove("quoted-report-plot.svg");
    CHECK(ps_analyze_run("test.psrun", "quoted-report") == PS_OK);
    FILE *report = fopen("quoted-report-summary.csv", "r");
    CHECK(report);
    char csv[512] = {0};
    size_t csv_size = fread(csv, 1, sizeof csv - 1, report);
    fclose(report);
    CHECK(csv_size > 0 && strstr(csv, "\"angle,\"\"quoted\",\"rad\",2,") != NULL);
    FILE *f = fopen("test.psrun", "rb");
    CHECK(f);
    unsigned char file[8192];
    size_t size = fread(file, 1, sizeof file, f);
    fclose(f);
    CHECK(size > 20);
    f = fopen("truncated.psrun", "wb");
    CHECK(f);
    CHECK(fwrite(file, 1, size - 9, f) == size - 9);
    fclose(f);
    CHECK(ps_run_open(&r, "truncated.psrun") == PS_OK);
    CHECK(ps_run_next(&r, &t, values) == PS_OK);
    CHECK(ps_run_next(&r, &t, values) == PS_OK);
    CHECK(ps_run_next(&r, &t, values) == PS_RECOVERED);
    ps_run_reader_close(&r);
    /* Every truncation must either reject the header or return only complete samples. */
    for (size_t n = 0; n < size; n++) {
        f = fopen("fuzz.psrun", "wb");
        CHECK(f);
        fwrite(file, 1, n, f);
        fclose(f);
        if (ps_run_open(&r, "fuzz.psrun") == PS_OK) {
            ps_result e;
            while ((e = ps_run_next(&r, &t, values)) == PS_OK) {
            }
            CHECK(e == PS_RECOVERED || e == PS_CORRUPT);
            CHECK(r.samples <= 2);
            ps_run_reader_close(&r);
        }
    }
    /* A changed payload must never be returned as a valid measurement. */
    size_t schema_at = 28 + ps_get_u32(file + 20);
    size_t sample_at = schema_at + 12 + ps_get_u32(file + schema_at + 4);
    CHECK(sample_at + 12 < size);
    file[sample_at + 12] ^= 1;
    f = fopen("fuzz.psrun", "wb");
    CHECK(f);
    fwrite(file, 1, size, f);
    fclose(f);
    CHECK(ps_run_open(&r, "fuzz.psrun") == PS_OK);
    CHECK(ps_run_next(&r, &t, values) == PS_RECOVERED && r.samples == 0);
    ps_run_reader_close(&r);
    file[sample_at + 12] ^= 1;
    /* Version 1 readers skip valid future chunks without changing samples. */
    unsigned char extension[14];
    ps_put_u32(extension, 999);
    ps_put_u32(extension + 4, 2);
    extension[12] = 'o';
    extension[13] = 'k';
    ps_put_u32(extension + 8, ps_crc32(extension + 12, 2));
    f = fopen("fuzz.psrun", "wb");
    CHECK(f);
    fwrite(file, 1, sample_at, f);
    fwrite(extension, 1, sizeof extension, f);
    fwrite(file + sample_at, 1, size - sample_at, f);
    fclose(f);
    CHECK(ps_run_open(&r, "fuzz.psrun") == PS_OK);
    CHECK(ps_run_next(&r, &t, values) == PS_OK);
    NEAR(values[0], 3, 1e-15);
    CHECK(ps_run_next(&r, &t, values) == PS_OK);
    CHECK(ps_run_next(&r, &t, values) == PS_EOF);
    ps_run_reader_close(&r);
    ps_wire_buffer wire = {0};
    unsigned char payload[4];
    ps_put_u32(payload, PS_ABI_VERSION);
    size_t n = ps_wire_encode(wire.data, PS_MSG_HELLO, 0, payload, 4);
    uint32_t type, len;
    const unsigned char *data;
    wire.used = n - 1;
    CHECK(ps_wire_peek(&wire, &type, &data, &len) == 0);
    wire.used = n;
    CHECK(ps_wire_peek(&wire, &type, &data, &len) == 1);
    CHECK(type == PS_MSG_HELLO && len == 4);
    ps_wire_consume(&wire, len);
    CHECK(wire.used == 0 && wire.sequence == 1);
    wire.used = ps_wire_encode(wire.data, PS_MSG_HELLO, 1, NULL, 0);
    ps_put_u32(wire.data + 12, PS_WIRE_MAX + 1);
    CHECK(ps_wire_peek(&wire, &type, &data, &len) == -1);
    ps_scene s = {0}, decoded = {0};
    ps_scene_add(&s, PS_SPHERE, ps_v3(1, 2, 3), ps_v3(0, 0, 0), 0.2, 0xffffffff);
    n = ps_snapshot_encode(file, &c, &s, true);
    uint32_t count;
    bool paused;
    CHECK(ps_snapshot_decode(file, (uint32_t)n, &t, values, &count, &decoded, &paused));
    CHECK(paused && count == 1 && decoded.count == 1);
    NEAR(decoded.objects[0].a.z, 3, 1e-15);
    CHECK(!ps_snapshot_decode(file, (uint32_t)n - 1, &t, values, &count, &decoded, &paused));
    remove("test.psrun");
    remove("truncated.psrun");
    remove("fuzz.psrun");
    remove("test.csv");
    remove("quoted-report-summary.csv");
    remove("quoted-report-manifest.txt");
    remove("quoted-report-plot.svg");
    puts("All core, numerical, protocol and recovery checks passed.");
    return 0;
}
