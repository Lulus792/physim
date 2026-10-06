#include "batch.h"
#include "physim/measurement.h"
#include "physim/report.h"
#include "platform.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Sensor reference line %d: %s\n", __LINE__, #x);                       \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static int run(const char *const *args) {
    ps_process p = {0};
    if (!ps_process_start(&p, args, NULL))
        return -1;
    double until = ps_clock() + 20;
    char buffer[4096];
    while (ps_process_poll(&p) && ps_clock() < until) {
        while (ps_process_read(&p, buffer, sizeof buffer) > 0) {
        }
        ps_sleep(1);
    }
    int code = p.running ? -1 : p.exit_code;
    ps_process_close(&p);
    return code;
}
static double model[6001][7], valid[3001][6];
int main(int argc, char **argv) {
    if (argc != 8)
        return 2;
    char root[4096], path[4096], prefix[4096], report_path[4096];
    snprintf(root, sizeof root, "%s/sensor-reference-%.0f", argv[7], ps_clock() * 1e9);
    CHECK(ps_make_directory_exclusive(root));
    for (unsigned mode = 0; mode < 3; mode++) {
        snprintf(path, sizeof path, "%s/mode-%u.psrun", root, mode);
        const char *args[] = {argv[1], argv[mode + 2], path,     "--steps", "6000",
                              "--dt",  ".005",         "--seed", "42",      NULL};
        CHECK(run(args) == 0);
        ps_run_reader reader;
        CHECK(ps_run_open(&reader, path) == PS_OK && reader.channels == 15);
        CHECK(strstr(reader.metadata, "measurement_api=1") &&
              strstr(reader.metadata, "sensor_rate_hz=100"));
        int index = -1;
        CHECK(ps_channel_status_index(reader.schema, reader.channels, 7, &index) == PS_OK &&
              index == 9);
        CHECK(ps_channel_status_index(reader.schema, reader.channels, 12, &index) == PS_OK &&
              index == 9);
        CHECK(ps_channel_status_index(reader.schema, reader.channels, 9, &index) == PS_OK &&
              index == -1);
        reader.schema[9].dimension[0] = 1;
        index = 77;
        CHECK(ps_channel_status_index(reader.schema, reader.channels, 7, &index) == PS_INVALID &&
              index == 77);
        reader.schema[9].dimension[0] = 0;
        double time, values[PS_MAX_CHANNELS], sum = 0, squares = 0, maxerr = 0, u_sum = 0,
                                              sensor_sum = 0;
        unsigned count = 0, states[3] = {0};
        ps_result r;
        while ((r = ps_run_next(&reader, &time, values)) == PS_OK) {
            CHECK(count < 6001 && fabs(time - count * .005) < 1e-12 &&
                  fabs(values[11] - time) < 1e-12 && values[14] == 0);
            if (!mode)
                memcpy(model[count], values, 7 * sizeof(double));
            else
                CHECK(!memcmp(model[count], values, 7 * sizeof(double)));
            for (unsigned axis = 0; axis < 2; axis++) {
                double status = values[9 + axis];
                CHECK(status == 0 || status == 1 || status == 2);
                if (count % 2)
                    CHECK(status == 0);
                else
                    CHECK(status != 0);
                if (status == 1) {
                    double expected = mode == 1
                                          ? 0
                                          : sqrt(.003 * .003 + pow(fabs(values[axis]) * .001, 2) +
                                                 .02 * .02 + .005 * .005 / 12);
                    CHECK(fabs(values[12 + axis] - expected) < 1e-12);
                    if (mode == 1)
                        CHECK(values[7 + axis] == values[axis]);
                    else
                        CHECK(fabs(values[7 + axis] / .005 - round(values[7 + axis] / .005)) <
                              1e-9);
                } else
                    CHECK(values[7 + axis] == 0 && values[12 + axis] == 0);
            }
            unsigned state = (unsigned)values[9];
            if (state == 1) {
                double error = values[7] - values[0];
                unsigned at = states[1];
                CHECK(at < 3001);
                double row[] = {time, values[0], values[5], values[7], error, values[12]};
                memcpy(valid[at], row, sizeof row);
                sum += error;
                squares += error * error;
                maxerr = fmax(maxerr, fabs(error));
                u_sum += values[12];
                sensor_sum += values[7];
            }
            states[state]++;
            count++;
        }
        CHECK(r == PS_EOF && count == 6001 && states[0] == 3000);
        if (!mode)
            CHECK(states[2] > 100 && states[2] < 200 && states[1] + states[2] == 3001);
        if (mode == 1)
            CHECK(states[1] == 3001 && states[2] == 0);
        if (mode == 2)
            CHECK(states[1] == 0 && states[2] == 3001);
        ps_run_reader_close(&reader);
        snprintf(prefix, sizeof prefix, "%s/report-%u", root, mode);
        const char *analysis[] = {argv[5], argv[6], path, prefix, NULL};
        CHECK(run(analysis) == 0);
        snprintf(report_path, sizeof report_path, "%s.psreport", prefix);
        ps_report *report = NULL;
        CHECK(ps_report_load(report_path, &report) == PS_OK);
        uint32_t plots, tables;
        CHECK(ps_report_describe(report, NULL, NULL, &plots, &tables) == PS_OK && plots == 4 &&
              tables == (mode == 2 ? 3u : 4u));
        ps_plot_info plot;
        CHECK(ps_report_plot_read(report, 3, &plot) == PS_OK &&
              plot.curves == (mode == 2 ? 2u : 3u));
        ps_table_row row;
        CHECK(ps_report_row_read(report, 2, 0, &row) == PS_OK);
        CHECK(row.values[0] == states[1] && row.values[1] == states[2] &&
              row.values[2] == states[0]);
        if (states[1]) {
            CHECK(ps_report_row_read(report, 3, 0, &row) == PS_OK);
            CHECK(fabs(row.values[0] - sum / states[1]) < 1e-12 && row.values[1] == maxerr &&
                  fabs(row.values[2] - u_sum / states[1]) < 1e-12);
            CHECK(fabs(row.values[3] - sqrt((squares - sum * sum / states[1]) / (states[1] - 1))) <
                  1e-12);
            ps_curve_data *curve = malloc(sizeof *curve);
            CHECK(curve);
            CHECK(ps_report_curve_read(report, 3, 2, curve) == PS_OK &&
                  curve->kind == PS_PLOT_SCATTER);
            CHECK(curve->source_count == states[1] && curve->count == 2048);
            for (unsigned i = 0; i < curve->count; i++) {
                unsigned rank = (unsigned)((uint64_t)i * (states[1] - 1) / (curve->count - 1));
                CHECK(curve->x[i] == valid[rank][0] && curve->y[i] == valid[rank][3]);
            }
            free(curve);
        }
        snprintf(report_path, sizeof report_path, "%s-sensor.csv", prefix);
        FILE *file = fopen(report_path, "r");
        CHECK(file);
        char line[2048];
        CHECK(fgets(line, sizeof line, file));
        unsigned rows = 0;
        while (fgets(line, sizeof line, file)) {
            double actual[6];
            CHECK(sscanf(line, "%lf,%lf,%lf,%lf,%lf,%lf", &actual[0], &actual[1], &actual[2],
                         &actual[3], &actual[4], &actual[5]) == 6 &&
                  rows < states[1]);
            for (unsigned i = 0; i < 6; i++)
                CHECK(fabs(actual[i] - valid[rows][i]) < 1e-12);
            rows++;
        }
        CHECK(!ferror(file) && rows == states[1]);
        fclose(file);
        snprintf(report_path, sizeof report_path, "%s-summary.csv", prefix);
        file = fopen(report_path, "r");
        CHECK(file);
        bool found = false;
        while (fgets(line, sizeof line, file))
            if (!strncmp(line, "\"sensor.x\",\"m\",", 15)) {
                found = true;
                unsigned n = 0;
                double average = 0;
                if (!states[1])
                    CHECK(!strcmp(line, "\"sensor.x\",\"m\",0,,,,\n"));
                else {
                    CHECK(sscanf(line + 15, "%u,%lf", &n, &average) == 2 && n == states[1]);
                    CHECK(fabs(average - sensor_sum / states[1]) < 1e-10);
                }
            }
        fclose(file);
        CHECK(found);
        ps_report_destroy(report);
        printf("Sensor mode %u: valid=%u, dropped=%u, not due=%u\n", mode, states[1], states[2],
               states[0]);
    }
    /* Small and malformed datasets exercise validity, not only long previews. */
    for (unsigned mode = 0; mode < 4; mode++) {
        ps_context context = {0};
        context.dt_s = .01;
        ps_channel_add(&context, "position.x", PS_METRE, "");
        ps_channel_add(&context, "nominal.x", PS_METRE, "");
        ps_channel_add(&context, "sensor.x", PS_METRE, "");
        ps_channel_add(&context, "sensor.x.status", mode == 2 ? PS_METRE : PS_ONE, "");
        ps_channel_add(&context, "sensor.x.u", PS_METRE, "");
        ps_channel_add(&context, "sensor.time", PS_SECOND, "");
        ps_channel_add(&context, "energy", PS_JOULE, "");
        snprintf(path, sizeof path, "%s/small-%u.psrun", root, mode);
        ps_run_writer writer;
        CHECK(ps_run_create(&writer, path, &context, "Validity fixture") == PS_OK);
        double first[] = {10, 10, 11, mode == 1 ? 3 : 1, mode == 3 ? -.1 : .1, 0, 0};
        double second[] = {11, 11, 0, 2, 0, .01, 0};
        CHECK(ps_run_append(&writer, 0, first) == PS_OK &&
              ps_run_append(&writer, .01, second) == PS_OK && ps_run_close(&writer) == PS_OK);
        snprintf(prefix, sizeof prefix, "%s/small-report-%u", root, mode);
        const char *args[] = {argv[5], argv[6], path, prefix, NULL};
        int code = run(args);
        snprintf(report_path, sizeof report_path, "%s.psreport", prefix);
        if (!mode) {
            CHECK(code == 0);
            ps_report *report = NULL;
            CHECK(ps_report_load(report_path, &report) == PS_OK);
            ps_table_info table;
            ps_table_row row;
            ps_curve_data *curve = malloc(sizeof *curve);
            CHECK(curve);
            CHECK(ps_report_table_read(report, 3, &table) == PS_OK && table.columns == 3);
            CHECK(ps_report_row_read(report, 3, 0, &row) == PS_OK && row.values[0] == 1 &&
                  row.values[1] == 1 && row.values[2] == .1);
            CHECK(ps_report_curve_read(report, 3, 2, curve) == PS_OK && curve->count == 1 &&
                  curve->source_count == 1 && curve->x[0] == 0 && curve->y[0] == 11);
            free(curve);
            ps_report_destroy(report);
            snprintf(report_path, sizeof report_path, "%s-summary.csv", prefix);
            FILE *file = fopen(report_path, "r");
            CHECK(file);
            char line[2048];
            bool found = false;
            while (fgets(line, sizeof line, file))
                if (!strcmp(line, "\"sensor.x\",\"m\",1,11,,11,11\n"))
                    found = true;
            fclose(file);
            CHECK(found);
        } else {
            CHECK(code != 0);
            FILE *file = fopen(report_path, "rb");
            CHECK(file == NULL);
        }
    }
    ps_batch_options options = {0};
    ps_batch_result result;
    snprintf(options.runner, sizeof options.runner, "%s", argv[1]);
    snprintf(options.channel, sizeof options.channel, "sensor.x");
    options.seed = 42;
    options.runs = 1;
    options.workers = 1;
    options.timeout_s = 10;
    options.dt = .005;
    for (unsigned mode = 0; mode < 3; mode++) {
        snprintf(options.module, sizeof options.module, "%s", argv[mode == 2 ? 4 : 3]);
        snprintf(options.directory, sizeof options.directory, "%s/batch-%u", root, mode);
        options.steps = mode ? 2 : 1;
        ps_result r = ps_batch_run(&options, NULL, NULL, &result);
        if (mode == 1)
            CHECK(r == PS_OK && result.completed == 1 &&
                  fabs(result.values[0] - model[2][0]) < 1e-12);
        else {
            CHECK(r==PS_OK && result.completed==1 && result.valid==0 && result.finished[0] &&
                  result.endpoint_status[0]==(mode==0?0:2));
            char summary[4096];snprintf(summary,sizeof summary,"%s/summary.psreport",options.directory);
            ps_report *report=NULL;CHECK(ps_report_load(summary,&report)==PS_OK);
            uint32_t plots,tables;CHECK(ps_report_describe(report,NULL,NULL,&plots,&tables)==PS_OK && plots==0 && tables==1);
            ps_table_row row;CHECK(ps_report_row_read(report,0,1,&row)==PS_OK && row.values[0]==0);
            CHECK(ps_report_row_read(report,0,mode==0?2:3,&row)==PS_OK && row.values[0]==1 && row.values[1]==100);
            ps_report_destroy(report);
        }
    }
    puts("Sensor reference, masked report/CSV and batch endpoint coverage passed.");
    return 0;
}
