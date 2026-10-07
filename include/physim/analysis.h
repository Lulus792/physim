#ifndef PHYSIM_ANALYSIS_H
#define PHYSIM_ANALYSIS_H
#include "data.h"
#include "batch.h"
typedef struct {
    uint64_t count;
    double mean, m2, min, max;
} ps_statistics;
/* Welford accumulation stores the unscaled squared deviation in m2. Very
 * large finite inputs may overflow it; check mean/m2 before using the result.
 * ps_series_statistics and ps_analyze_run return PS_NUMERIC in that case. */
void ps_statistics_push(ps_statistics *stats, double value);
double ps_statistics_stddev(const ps_statistics *stats);
/* Central secants and one-sided endpoints (not quadratic interpolation).
 * Finite, strictly increasing x; finite y; n>=2. Output may alias inputs.
 * PS_NUMERIC for an unrepresentable slope; all failures leave out unchanged. */
ps_result ps_derivative(const double *x, const double *y, size_t n, double *out);
/* Finite strictly increasing x, finite y, n>=2. NAN for invalid input or
 * an unrepresentable interval/partial sum. */
double ps_trapezoid(const double *x, const double *y, size_t n);
typedef struct {
    uint32_t struct_size, abi_version;
    const char *name;
    ps_result (*run)(const char *input_run, const char *output_prefix);
    /* Optional tail extension of ABI 2. The runner checks struct_size before
     * reading it. Receives 0..8 explicit paths, in selection order, no resampling.
     * Zero inputs are for self-generated analyses; modules may reject them.
     * Input strings are borrowed for the duration of this synchronous call. */
    ps_result (*run_many)(const char *const *input_runs, size_t count, const char *output_prefix);
    /* Optional ABI-3 tail. Explicit diagnostic output owned by the caller.
     * Receives 0..8 inputs; return result and write a matching error record if
     * available. Clear the record on success. Inputs/prefix borrowed synchronously. */
    ps_result (*run_diagnostic)(const char *const *input_runs, size_t count,
                                const char *output_prefix, ps_diagnostic *diagnostic);
    /* Optional ABI-3 host-service tail. Services and strings are borrowed only
     * for this call; a module must not retain them after returning. */
    ps_result (*run_host)(const char *const *input_runs,size_t count,const char *output_prefix,
                          const ps_analysis_services *services,ps_diagnostic *diagnostic);
} ps_analysis_api;
#define PS_ANALYSIS_API_BASE_SIZE offsetof(ps_analysis_api, run_many)
#define PS_ANALYSIS_API_MANY_SIZE offsetof(ps_analysis_api, run_diagnostic)
#define PS_ANALYSIS_API_DIAGNOSTIC_SIZE offsetof(ps_analysis_api, run_host)
#define PS_ANALYSIS_MAX_INPUTS 8u
typedef const ps_analysis_api *(*ps_analysis_entry)(void);
/* Streaming statistics, bounded preview SVG and CSV table. Prefix is a filename
 * prefix. Energy deviation uses energy.balance when present, otherwise energy;
 * the manifest records energy_metric_channel. Sources are expected to use SI.
 * Associated measurement .status channels mask statistics and energy/period
 * metrics (only status=1); gaps interrupt period counting. Missing energy
 * deviation is nan in the manifest. PS_NUMERIC precedes export on statistical
 * accumulator overflow; the unscaled variance range is the limiting factor.
 * Empty statistics and the standard deviation for n<2 are blank in CSV. */
ps_result ps_analyze_run(const char *input_run, const char *output_prefix);
#endif
