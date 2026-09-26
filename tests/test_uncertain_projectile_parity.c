#include "physim/data.h"
#include "physim/measurement.h"
#include "physim/report.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Uncertain projectile parity line %d: %s\n", __LINE__, #x);           \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)

static int near(double a, double b, double tolerance) {
    return isfinite(a) && isfinite(b) && fabs(a - b) <= tolerance;
}
static int run(const char *runner, const char *module, const char *path, const char *work,
               const char *seed) {
    const char *args[] = {runner, module, path, "--steps", "200", "--dt", "0.005", "--seed",
                          seed, NULL};
    ps_process child = {0};
    CHECK(ps_process_start(&child, args, work));
    double deadline = ps_clock() + 20;
    char output[2048];
    while (ps_process_poll(&child) && ps_clock() < deadline) {
        int size = ps_process_read(&child, output, sizeof output);
        if (size > 0)
            fwrite(output, 1, (size_t)size, stdout);
        ps_sleep(1);
    }
    if (child.running)
        ps_process_kill(&child);
    int code = child.exit_code;
    ps_process_close(&child);
    CHECK(code == 0);
    return 0;
}
static int compare(const char *c_path, const char *language_path) {
    ps_run_reader c, language;
    CHECK(ps_run_open(&c, c_path) == PS_OK && ps_run_open(&language, language_path) == PS_OK);
    CHECK(c.channels == 15 && language.channels == 15);
    CHECK(strstr(c.metadata, "integrator=RK4") && strstr(language.metadata, "integrator=RK4"));
    CHECK(strstr(c.metadata, "sensor_seed_x=seed XOR 0xa0761d6478bd642f") &&
          strstr(language.metadata, "sensor_seed_x=seed XOR 0xa0761d6478bd642f"));
    for (unsigned channel = 0; channel < 15; channel++) {
        CHECK(!strcmp(c.schema[channel].name, language.schema[channel].name));
        CHECK(!memcmp(c.schema[channel].dimension, language.schema[channel].dimension, 7));
        CHECK(!strcmp(c.schema[channel].unit, language.schema[channel].unit));
    }
    unsigned status[3] = {0};
    double vx = 0, vy = 0;
    for (unsigned frame = 0; frame <= 200; frame++) {
        double ct, lt, cv[PS_MAX_CHANNELS], lv[PS_MAX_CHANNELS];
        CHECK(ps_run_next(&c, &ct, cv) == PS_OK && ps_run_next(&language, &lt, lv) == PS_OK);
        CHECK(near(ct, frame * .005, 1e-12) && near(lt, ct, 1e-12));
        for (unsigned channel = 0; channel < 15; channel++) {
            if (!near(cv[channel], lv[channel], 2e-10)) {
                fprintf(stderr, "frame %u channel %u (%s): C %.17g, Physim %.17g\n",
                        frame, channel, c.schema[channel].name, cv[channel], lv[channel]);
                return 1;
            }
        }
        if (!frame) {
            vx = cv[2];
            vy = cv[3];
            CHECK(near(cv[0], -2, 1e-12) && near(cv[1], 0, 1e-12));
        }
        CHECK(near(cv[0], -2 + vx * ct, 2e-10));
        CHECK(near(cv[1], vy * ct - .5 * 9.80665 * ct * ct, 2e-10));
        CHECK(near(cv[2], vx, 2e-10) && near(cv[3], vy - 9.80665 * ct, 2e-10));
        CHECK(near(cv[4], .5 * (vx * vx + vy * vy), 2e-10));
        CHECK(cv[9] == 0 || cv[9] == 1 || cv[9] == 2);
        CHECK(cv[10] == 0 || cv[10] == 1 || cv[10] == 2);
        status[(unsigned)cv[9]]++;
        if (cv[9] != PS_MEASUREMENT_VALID)
            CHECK(cv[7] == 0 && cv[12] == 0);
        if (cv[10] != PS_MEASUREMENT_VALID)
            CHECK(cv[8] == 0 && cv[13] == 0);
        if (cv[9] == PS_MEASUREMENT_VALID) {
            double expected = sqrt(.003 * .003 + pow(fabs(cv[0]) * .001, 2) + .02 * .02 +
                                   .005 * .005 / 12);
            CHECK(near(cv[12], expected, 1e-12));
        }
    }
    CHECK(status[0] && status[1] && status[2]);
    double time, values[PS_MAX_CHANNELS];
    CHECK(ps_run_next(&c, &time, values) == PS_EOF);
    CHECK(ps_run_next(&language, &time, values) == PS_EOF);
    CHECK(c.complete && language.complete && c.samples == 201 && language.samples == 201);
    ps_run_reader_close(&c);
    ps_run_reader_close(&language);
    return 0;
}
static int analyze(const char *module_path, const char *run_path, const char *work,
                   unsigned run_index, unsigned analysis_index, unsigned source_index) {
    char prefix[4096], path[4096];
    CHECK(snprintf(prefix, sizeof prefix, "%s/sensor-parity-report-%u-%u-%u-%.0f", work,
                   run_index, analysis_index, source_index, ps_clock() * 1e9) < (int)sizeof prefix);
    void *module = ps_module_open(module_path);
    CHECK(module);
    void *symbol = ps_module_symbol(module, "ps_get_analysis");
    ps_analysis_entry entry = NULL;
    CHECK(symbol);
    memcpy(&entry, &symbol, sizeof entry);
    const ps_analysis_api *api = entry();
    CHECK(api && api->run(run_path, prefix) == PS_OK);
    CHECK(snprintf(path, sizeof path, "%s.psreport", prefix) < (int)sizeof path);
    ps_report *report = NULL;
    CHECK(ps_report_load(path, &report) == PS_OK);
    uint32_t plots = 0, tables = 0;
    CHECK(ps_report_describe(report, NULL, NULL, &plots, &tables) == PS_OK);
    CHECK(plots >= 2 && tables >= 2);
    ps_report_destroy(report);
    CHECK(snprintf(path, sizeof path, "%s-sensor.csv", prefix) < (int)sizeof path);
    FILE *csv = fopen(path, "rb");
    CHECK(csv);
    char line[1024];
    unsigned rows = 0;
    while (fgets(line, sizeof line, csv))
        rows++;
    fclose(csv);
    CHECK(rows > 50 && rows < 202);
    ps_module_close(module);
    return 0;
}
int main(int argc, char **argv) {
    CHECK(argc == 7);
    const char *seeds[] = {"42", "18446744073709551615"};
    for (unsigned run_index = 0; run_index < 2; run_index++) {
        char c_path[4096], language_path[4096];
        CHECK(snprintf(c_path, sizeof c_path, "%s/sensor-parity-c-%u-%.0f.psrun", argv[4],
                       run_index, ps_clock() * 1e9) < (int)sizeof c_path);
        CHECK(snprintf(language_path, sizeof language_path,
                       "%s/sensor-parity-language-%u-%.0f.psrun", argv[4], run_index,
                       ps_clock() * 1e9) < (int)sizeof language_path);
        CHECK(run(argv[1], argv[2], c_path, argv[4], seeds[run_index]) == 0);
        CHECK(run(argv[1], argv[3], language_path, argv[4], seeds[run_index]) == 0);
        CHECK(compare(c_path, language_path) == 0);
        if (!run_index) {
            CHECK(analyze(argv[5], c_path, argv[4], run_index, 0, 0) == 0);
            CHECK(analyze(argv[5], language_path, argv[4], run_index, 0, 1) == 0);
            CHECK(analyze(argv[6], c_path, argv[4], run_index, 1, 0) == 0);
            CHECK(analyze(argv[6], language_path, argv[4], run_index, 1, 1) == 0);
        }
    }
    puts("Uncertain projectile: C and Physim runs and cross-language analyses agree");
    return 0;
}
