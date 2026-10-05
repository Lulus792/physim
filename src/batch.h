#ifndef PS_BATCH_H
#define PS_BATCH_H
#include "physim/data.h"
#include "physim/experiment.h"
#include "physim/report.h"

#define PS_BATCH_MAX_RUNS 1000u
#define PS_BATCH_MAX_SAMPLES 5000000u
#define PS_BATCH_MAX_WORKERS 8u
/* Private application service; all paths must be absolute. The output directory
 * must not exist. Each child has its own process and working directory. */
typedef struct {
    char runner[4096], module[4096], source[4096], directory[4096], channel[48];
    /* Optional immutable UTF-8 source captured by the UI before the worker starts.
     * Borrowed for the duration of ps_batch_run; source remains its original path. */
    const char *source_text;
    size_t source_size;
    uint64_t seed;
    uint64_t memory_bytes; /* Per runner; zero disables. Platform semantics: platform.h. */
    uint32_t runs, steps, workers; /* 1..PS_BATCH_MAX_WORKERS; capped by runs. */
    double dt, timeout_s;
    /* end_time=0 retains the fixed sample-count workflow. A positive target
     * clips the final interval; steps becomes an accepted-step budget.
     * Adaptive series require a common target, not equal step counts. */
    bool adaptive;
    double end_time, minimum_dt, maximum_dt;
    /* Optional linear parameter study. Index 0 uses start, the final index end. */
    bool sweep;
    char sweep_name[48];
    double sweep_start, sweep_end;
    /* Fixed overrides applied to every run, excluding the sweep parameter. */
    uint32_t parameter_count;
    struct {
        char name[48];
        double value;
    } parameters[PS_MAX_PARAMETERS];
} ps_batch_options;
typedef struct {
    uint32_t completed, started, active, peak_active;
    bool cancelled;
    char error[256];
    ps_channel channel;
    double values[PS_BATCH_MAX_RUNS];
    bool finished[PS_BATCH_MAX_RUNS]; /* values[i] is valid iff finished[i]. */
} ps_batch_result;
/* Called on the invoking thread; false requests cancellation of the entire pool.
 * completed counts fully validated, journaled runs; active counts live children
 * observed by the controller. Partial raw files remain, but only a complete
 * successful series receives a report. Results/aggregates use fixed index order. */
typedef bool (*ps_batch_continue)(uint32_t completed, uint32_t active, void *user);
ps_result ps_batch_validate(const ps_batch_options *options);
ps_result ps_batch_run(const ps_batch_options *options, ps_batch_continue proceed, void *user,
                       ps_batch_result *result);
/* Final-value statistics, Type-7 quantiles. Approximate normal mean CI only for
 * n >= 200, assuming independent samples and a well-behaved finite variance. */
ps_result ps_batch_report(const double *values, uint32_t count, const ps_channel *channel,
                          const char *provenance, ps_report **out);
#endif
