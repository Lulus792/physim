#include "physim/report.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "Dataset channel index %d: %s\n", __LINE__, #x); return 1; } } while (0)
static int fixture(void) {
    ps_context context = {.struct_size = sizeof context, .api_version = PS_API_VERSION, .dt_s = .125};
    CHECK(ps_channel_add(&context, "time", PS_SECOND, "Independent measured clock α") == 0);
    CHECK(ps_channel_add(&context, "position.x", PS_METRE, "Linear reference") == 1);
    ps_run_writer writer; CHECK(ps_run_create(&writer, "clock.psrun", &context, "Recorded time channel") == PS_OK);
    for (unsigned i = 0; i < 513; i++) {
        double values[] = {1000 + 5 * i, 10 + 2 * i};
        CHECK(ps_run_append(&writer, .125 * i, values) == PS_OK);
    }
    CHECK(ps_run_close(&writer) == PS_OK);
    return 0;
}
int main(int argc, char **argv) {
    CHECK(argc == 1 || argc == 3); CHECK(fixture() == 0);
    ps_analysis_context *context = NULL; CHECK(ps_analysis_create("clock-analysis", 0, &context) == PS_OK);
    ps_dataset dataset; CHECK(ps_analysis_open_run(context, "clock.psrun", &dataset) == PS_OK);
    ps_series time, clock, position, by_name, runtime_rate, measured_rate;
    CHECK(ps_dataset_series(context, dataset, "time", &time) == PS_OK);
    CHECK(ps_dataset_channel_series(context, dataset, 0, &clock) == PS_OK);
    CHECK(ps_dataset_channel_series(context, dataset, 1, &position) == PS_OK);
    CHECK(ps_dataset_series(context, dataset, "position.x", &by_name) == PS_OK);
    CHECK(ps_series_aligned(context, time, clock) == PS_OK && ps_series_aligned(context, position, by_name) == PS_OK);
    double values[258]; size_t count = 0;
    CHECK(ps_series_read(context, time, 255, values, 258, &count) == PS_OK && count == 258);
    for (size_t i = 0; i < count; i++) CHECK(values[i] == .125 * (255 + i));
    CHECK(ps_series_read(context, clock, 255, values, 258, &count) == PS_OK && count == 258);
    for (size_t i = 0; i < count; i++) CHECK(values[i] == 1000 + 5 * (255 + i));
    ps_series_info info;
    CHECK(ps_series_describe(context, clock, &info) == PS_OK && info.count == 513 && info.scale == 1 &&
          !strcmp(info.name, "time") && !strcmp(info.symbol, "s") && !memcmp(info.dimension, PS_SECOND.dimension, 7));
    CHECK(ps_series_derivative(context, position, time, &runtime_rate) == PS_OK);
    CHECK(ps_series_derivative(context, position, clock, &measured_rate) == PS_OK);
    ps_statistics statistics;
    CHECK(ps_series_statistics(context, runtime_rate, &statistics) == PS_OK && statistics.mean == 16 && statistics.count == 513);
    CHECK(ps_series_statistics(context, measured_rate, &statistics) == PS_OK && fabs(statistics.mean - .4) < 1e-15);
    uint64_t bytes = ps_analysis_scratch_bytes(context); ps_series preserved = clock;
    CHECK(ps_dataset_channel_series(context, dataset, 2, &preserved) == PS_INVALID && !memcmp(&preserved, &clock, sizeof clock));
    CHECK(ps_dataset_channel_series(context, dataset, UINT32_MAX, &preserved) == PS_INVALID && !memcmp(&preserved, &clock, sizeof clock));
    CHECK(ps_analysis_scratch_bytes(context) == bytes);
    ps_analysis_context *other = NULL; CHECK(ps_analysis_create("other-clock", 0, &other) == PS_OK);
    CHECK(ps_dataset_channel_series(other, dataset, 0, &preserved) == PS_INVALID && !memcmp(&preserved, &clock, sizeof clock));
    CHECK(!ps_analysis_scratch_bytes(other)); ps_analysis_destroy(other);
    unsigned available = PS_ANALYSIS_MAX_SERIES - 6;
    for (unsigned i = 0; i < available; i++) CHECK(ps_dataset_channel_series(context, dataset, 1, &preserved) == PS_OK);
    ps_series last = preserved;
    CHECK(ps_dataset_channel_series(context, dataset, 0, &preserved) == PS_LIMIT && !memcmp(&preserved, &last, sizeof last));
    CHECK(ps_analysis_scratch_bytes(context) == bytes);
    CHECK(ps_dataset_close(context, dataset) == PS_OK && !ps_analysis_scratch_bytes(context));
    preserved = clock;
    CHECK(ps_dataset_channel_series(context, dataset, 0, &preserved) == PS_INVALID && !memcmp(&preserved, &clock, sizeof clock));
    ps_dataset reopened; CHECK(ps_analysis_open_run(context, "clock.psrun", &reopened) == PS_OK);
    CHECK(reopened.slot == dataset.slot && reopened.generation != dataset.generation);
    CHECK(ps_dataset_channel_series(context, dataset, 0, &preserved) == PS_INVALID && !memcmp(&preserved, &clock, sizeof clock));
    CHECK(ps_dataset_channel_series(context, reopened, 0, &preserved) == PS_OK);
    CHECK(ps_series_read(context, preserved, 0, values, 1, &count) == PS_OK && count == 1 && values[0] == 1000);
    CHECK(ps_dataset_close(context, reopened) == PS_OK && !ps_analysis_scratch_bytes(context));
    ps_analysis_destroy(context);
    if (argc == 3) {
        const char *args[] = {argv[1], argv[2], "clock.psrun", "clock-report", NULL};
        ps_process child = {0}; CHECK(ps_process_start(&child, args, NULL));
        double until = ps_clock() + 30; char output[4096];
        while (ps_clock() < until) {
            int running = ps_process_poll(&child), n = ps_process_read(&child, output, sizeof output);
            if (n > 0) fwrite(output, 1, (size_t)n, stdout);
            if (!running && n <= 0) break;
            ps_sleep(1);
        }
        bool ok = !child.running && !child.exit_code;
        if (child.running) ps_process_kill(&child); ps_process_close(&child); CHECK(ok);
        ps_report *report = NULL; CHECK(ps_report_load("clock-report.psreport", &report) == PS_OK);
        ps_curve_data curve; CHECK(ps_report_curve_read(report, 0, 0, &curve) == PS_OK && curve.count == 513);
        for (unsigned i = 0; i < curve.count; i++) CHECK(curve.x[i] == 1000 + 5 * i && fabs(curve.y[i] - .4) < 1e-15);
        ps_report_destroy(report);
    }
    CHECK(remove("clock.psrun") == 0);
    puts("Dataset channel index: measured time, runtime axis, SI dimensions, alignment and rejected indices passed");
    return 0;
}
