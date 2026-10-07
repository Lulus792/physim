#ifndef PHYSIM_SERIES_H
#define PHYSIM_SERIES_H
#include "analysis.h"
#include "memory.h"
#include "units.h"

#define PS_ANALYSIS_MAX_DATASETS 8u
#define PS_ANALYSIS_MAX_SERIES 128u
#define PS_SERIES_BLOCK_SIZE 256u
#define PS_SERIES_MAX_WINDOW 4096u
typedef struct ps_analysis_context ps_analysis_context;
/* Handles belong to one live context; zero is invalid. Do not use after destroy.
 * Closing/releasing and reusing a slot never resurrects its old handles. */
typedef struct {
    const ps_analysis_context *owner;
    uint32_t slot, generation;
} ps_dataset;
typedef struct {
    const ps_analysis_context *owner;
    uint32_t slot, generation;
} ps_series;
typedef struct {
    uint64_t samples;
    uint32_t channel_count;
    bool recovered;
    char metadata[8192];
    ps_channel channels[PS_MAX_CHANNELS];
} ps_dataset_info;
typedef struct {
    uint64_t count;
    int8_t dimension[7];
    double scale;
    char name[64], symbol[64];
} ps_series_info;
typedef enum {
    PS_SERIES_ADD,
    PS_SERIES_SUBTRACT,
    PS_SERIES_MULTIPLY,
    PS_SERIES_DIVIDE
} ps_series_operator;

/* work_prefix is an existing writable directory plus a filename prefix, NOT a
 * directory to create. Scratch files are exclusive and deleted on close/process
 * exit. Quota covers all open scratch file payloads; 0 selects 1 GiB. Contexts
 * are independent, but each context requires external synchronization. */
ps_result ps_analysis_create(const char *work_prefix, uint64_t scratch_byte_limit,
                             ps_analysis_context **out);
/* Custom allocator owns the context; C stdio and OS buffers remain outside its
 * domain. Descriptor copied by value, callbacks/user must outlive the context. */
ps_result ps_analysis_create_with_allocator(const char *work_prefix, uint64_t scratch_byte_limit,
                                            ps_allocator allocator, ps_analysis_context **out);
void ps_analysis_destroy(ps_analysis_context *ctx);
uint64_t ps_analysis_scratch_bytes(const ps_analysis_context *ctx);
/* Validated immutable disk snapshot. PS_RECOVERED is success with a partial run;
 * other errors leave the output handle unchanged. Non-increasing times fail. */
ps_result ps_analysis_open_run(ps_analysis_context *ctx, const char *path, ps_dataset *out);
ps_result ps_dataset_describe(ps_analysis_context *ctx, ps_dataset dataset, ps_dataset_info *out);
/* Closes the dataset and invalidates ALL its source and derived series. */
ps_result ps_dataset_close(ps_analysis_context *ctx, ps_dataset dataset);
/* "time" is the time axis, otherwise an exact channel name. .psrun v1 values
 * are SI; unit scales are 1. Duplicate/ambiguous channel names are rejected. */
ps_result ps_dataset_series(ps_analysis_context *ctx, ps_dataset dataset, const char *name,
                            ps_series *out);
/* Convert finite inputs from the declared unit to canonical SI scratch values.
 * Described/output unit.scale is always 1 and the symbol is the SI dimension
 * spelling. Input arrays are preserved. Unrepresentable nonzero SI conversion
 * -> PS_NUMERIC. Empty inputs still acquire canonical metadata. Both forms
 * preserve output handle, quota and alignment serial on every failure.
 * Independent roots have distinct
 * alignment identities; aligned values share the anchor's dataset lifetime,
 * sample range and alignment, and must have the same count. Both forms are
 * transactional, quota-checked, and valid without an input dataset. */
ps_result ps_series_from_values(ps_analysis_context *ctx, const double *values, size_t count,
                                ps_unit unit, const char *name, ps_series *out);
ps_result ps_series_aligned_values(ps_analysis_context *ctx, ps_series anchor,
                                   const double *values, size_t count, ps_unit unit,
                                   const char *name, ps_series *out);
ps_result ps_series_describe(ps_analysis_context *ctx, ps_series series, ps_series_info *out);
ps_result ps_series_release(ps_analysis_context *ctx, ps_series series);
/* PS_OK only for live series on the same dataset, selection and sample range. */
ps_result ps_series_aligned(ps_analysis_context *ctx, ps_series left, ps_series right);
/* Copies up to capacity values starting at index. PS_OK includes an empty read
 * at count; index > count is invalid. Outputs are unspecified on I/O error. */
ps_result ps_series_read(ps_analysis_context *ctx, ps_series series, uint64_t index, double *values,
                         size_t capacity, size_t *read_count);
/* Explicit masks retain every row and the original alignment. Validity is the
 * conjunction of the input/selector masks and selector==accepted. Invalid
 * numeric values remain finite placeholders; use read_masked or validity to
 * distinguish them. Source channels remain unmasked unless explicitly masked.
 * Mask bytes count against the scratch quota. All operations are transactional. */
ps_result ps_series_mask(ps_analysis_context *ctx, ps_series input, ps_series selector,
                         double accepted, ps_series *out);
ps_result ps_series_read_masked(ps_analysis_context *ctx, ps_series input, uint64_t index,
                                double *values, uint8_t *valid, size_t capacity, size_t *read_count);
ps_result ps_series_is_masked(ps_analysis_context *ctx, ps_series input, bool *out);
/* An aligned, unmasked dimensionless series of exact 0/1 flags. */
ps_result ps_series_validity(ps_analysis_context *ctx, ps_series input, ps_series *out);
/* Derived series own scratch data and survive release of their input series.
 * Operations preserve alignment, except slice/select/resample (see below).
 * Matching slices within the same selection can still be paired.
 * Binary operations and x/y operations require the same dataset and sample range;
 * implicit cross-run resampling is deliberately forbidden. */
ps_result ps_series_slice(ps_analysis_context *ctx, ps_series input, uint64_t first, uint64_t count,
                          ps_series *out);
/* Keep rows whose dimensionless selector equals accepted exactly (expressed in
 * selector's stored units, e.g. status=1). Select 1..32 aligned columns together,
 * including their x axis. All columns and selector must be aligned. No implicit
 * handling of .status channels: the caller chooses the selector explicitly.
 * Outputs preserve units and row order, own their data, and share a NEW alignment
 * identity. Separate calls are not aligned, even if they select identical rows.
 * Empty selections succeed; downstream operations keep their normal minimum
 * count requirements. Derivatives/integrals connect retained samples across gaps.
 * Outputs and scratch usage are unchanged on failure. Input/output arrays may
 * alias. Bounded block memory; selected data counts against the scratch quota. */
ps_result ps_series_select(ps_analysis_context *ctx, const ps_series *columns, size_t count,
                           ps_series selector, double accepted, ps_series *out);
/* Factor is dimensionless; offset is converted/combined per valid SI row using
 * Quantity addition. Masked rows are not evaluated. Factor*value must itself be
 * finite; this is not an arbitrary-range three-factor fused operation. */
ps_result ps_series_affine(ps_analysis_context *ctx, ps_series input, double factor,
                           ps_quantity offset, ps_series *out);
ps_result ps_series_combine(ps_analysis_context *ctx, ps_series_operator op, ps_series left,
                            ps_series right, ps_series *out);
/* Central secants, one-sided segment endpoints; x must strictly increase.
 * Missing neighbors are never bridged; isolated valid samples have no derivative.
 * Scaled arithmetic preserves representable slopes even if endpoint differences
 * overflow. PS_NUMERIC for unrepresentable slopes; output/quota unchanged. */
ps_result ps_series_derivative(ps_analysis_context *ctx, ps_series y, ps_series x, ps_series *out);
/* Cumulative trapezoidal integral, first value = initial. A missing x/y sample
 * makes this and all later cumulative values unknown; no invented gap area.
 * Scaled interval arithmetic avoids intermediate overflow and loss of subnormal
 * means. PS_NUMERIC for unrepresentable areas/sums; output/quota unchanged. */
ps_result ps_series_integral(ps_analysis_context *ctx, ps_series y, ps_series x,
                             ps_quantity initial, ps_series *out);
/* Causal moving average, window <= 4096. A missing sample resets the window;
 * only consecutive valid observations contribute. */
ps_result ps_series_moving_average(ps_analysis_context *ctx, ps_series input, size_t window,
                                   ps_series *out);
/* Explicit linear interpolation of aligned (x,y) onto target_x, possibly from
 * another dataset in this context. Both axes must be nonempty and strictly
 * increasing, with compatible units. No extrapolation: all converted target
 * values must lie in the closed source range (PS_INVALID otherwise).
 * Output has y's unit and target_x's dataset/sample alignment. It survives
 * closing the source dataset, but not the target dataset. Bounded block memory;
 * all source x values are validated, including those beyond the last target.
 * Both axes must have valid coordinates. Linear/PCHIP intervals require two
 * valid y knots; nearest/previous propagate the chosen knot validity. PCHIP
 * slopes use contiguous valid neighbors and treat gap edges as endpoints. */
ps_result ps_series_resample_linear(ps_analysis_context *ctx, ps_series y, ps_series x,
                                    ps_series target_x, ps_series *out);
typedef enum { PS_RESAMPLE_LINEAR, PS_RESAMPLE_NEAREST, PS_RESAMPLE_PREVIOUS,
               PS_RESAMPLE_PCHIP } ps_resample_method;
/* Same axis, unit, lifetime and transactional contract as resample_linear.
 * NEAREST selects the earlier sample at equal distances. PREVIOUS holds the
 * last source value at or before each target. Exact source coordinates always
 * return that source value. No extrapolation for any method. */
/* PCHIP is a local, monotonicity-preserving cubic Hermite interpolant with
 * continuous first derivatives. Flat spans and extrema have zero knot slopes;
 * endpoints use limited one-sided slopes. Two source points reduce to linear
 * interpolation; a single point is valid only at that exact coordinate.
 * Bounded block memory, finite-input arithmetic without wider numeric types. */
ps_result ps_series_resample(ps_analysis_context *ctx, ps_series y, ps_series x, ps_series target_x,
                             ps_resample_method method, ps_series *out);
ps_result ps_series_statistics(ps_analysis_context *ctx, ps_series input, ps_statistics *out);
/* Type-7 quantile for a nonempty series. probability must be finite in [0, 1].
 * Uses only valid rows; all-missing input is PS_INVALID. Reads the whole series
 * into allocator-backed temporary memory; output is
 * unchanged on error. */
ps_result ps_series_quantile(ps_analysis_context *ctx, ps_series input, double probability,
                             double *out);
/* Variant for hosts with a separate temporary-memory budget. */
ps_result ps_series_quantile_with_allocator(ps_analysis_context *ctx, ps_series input,
                                            double probability, ps_allocator allocator, double *out);
/* Exports every aligned row without loading them in memory. Masked values are
 * empty fields; export an explicit validity series for flags. Exclusive output.
 * On I/O failure a partial output may remain. At most 32 columns. */
ps_result ps_series_export_csv(ps_analysis_context *ctx, const ps_series *columns, size_t count,
                               const char *path);
#endif
