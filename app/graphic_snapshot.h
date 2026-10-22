#ifndef PS_GRAPHIC_SNAPSHOT_H
#define PS_GRAPHIC_SNAPSHOT_H
#include "physim/report.h"

#define PS_GRAPHIC_MAX_INPUTS PS_REPORT_MAX_CURVES
#define PS_GRAPHIC_PATH_BYTES 4096u
#define PS_GRAPHIC_NO_REFERENCE UINT32_MAX
typedef struct ps_graphic_snapshot ps_graphic_snapshot;
typedef struct {
    double full[4],visible[4]; /* finite increasing X/Y bounds in display units */
    bool cropped;
} ps_graphic_axes;
typedef struct {
    ps_plot_info plot;
    ps_graphic_axes axes;
    uint32_t reference,input_count;
    bool reduced;
} ps_graphic_description;
typedef struct {
    char path[PS_GRAPHIC_PATH_BYTES],label[96];
    ps_channel channel;
    uint64_t samples;
    bool recovered;
    ps_result result; /* comparison: unavailable inputs remain named, without a curve */
    uint32_t curve; /* PS_GRAPHIC_NO_REFERENCE when unavailable; otherwise plot curve index */
    ps_result statistics_result;
    ps_statistics si_statistics; /* full valid source samples, before display conversion */
} ps_graphic_run_info;
typedef struct {
    const char *path,*label;
    ps_plot_kind kind; /* 0/LINE or SCATTER; captured from the displayed curve */
} ps_graphic_run_input;
typedef struct {
    ps_graphic_run_input inputs[PS_GRAPHIC_MAX_INPUTS];
    uint32_t count,reference;
    ps_channel channel;
    ps_report_unit display_unit;
} ps_graphic_run_request;
typedef struct {ps_result result;uint32_t input;bool cancelled;} ps_graphic_failure;
typedef bool (*ps_graphic_cancel)(void *user);

/* Immutable owned data for both comparison rendering and export. No source
 * report, file, view or string is retained. Outputs survive every failed build.
 * Allocator callbacks/user outlive the snapshot. Serialize source report writes
 * against the synchronous copy. NULL axes uses the complete plot bounds. */
ps_result ps_graphic_snapshot_from_report(const ps_report *report,uint32_t plot,
    const ps_graphic_axes *axes,uint32_t reference,ps_allocator allocator,ps_graphic_snapshot **out);
/* Synchronous loader intended for an owned background preview request. Inputs
 * are borrowed until return; each run keeps its independent time axis. Matching
 * channels/dimensions, status-domain validation and explicit masking are required.
 * Reduction preserves endpoints/extrema/gaps and exposes original source_count.
 * Scratch prefix/quota follow ps_analysis_create; each dataset closes after its
 * owned preview/statistics are captured. Failure reports the input, or UINT32_MAX.
 * Cooperative cancellation is observed at phase/block boundaries, not inside
 * an in-progress core snapshot/reduction operation; it returns PS_EOF with
 * failure.cancelled=true. It never publishes a partial snapshot. */
ps_result ps_graphic_snapshot_from_runs(const ps_graphic_run_request *request,
    const char *scratch_prefix,uint64_t scratch_quota,ps_graphic_cancel cancel,void *user,
    ps_allocator allocator,ps_graphic_snapshot **out,ps_graphic_failure *failure);
/* Visible comparison: retain unavailable inputs and their errors while drawing
 * compatible inputs in selection order. Resource/cancellation failures remain
 * transactional. No valid curve returns PS_INVALID with out unchanged. reference
 * describes the selected reference's curve, or NO_REFERENCE if unavailable. */
ps_result ps_graphic_snapshot_compare_runs(const ps_graphic_run_request *request,
    const char *scratch_prefix,uint64_t scratch_quota,ps_graphic_cancel cancel,void *user,
    ps_allocator allocator,ps_graphic_snapshot **out,ps_graphic_failure *failure);
/* Copy the currently displayed cache at dialog-open time, without reopening its
 * original files. NULL axes retains the source axes, including exact crop. */
ps_result ps_graphic_snapshot_copy(const ps_graphic_snapshot *source,const ps_graphic_axes *axes,
    ps_allocator allocator,ps_graphic_snapshot **out);
void ps_graphic_snapshot_destroy(ps_graphic_snapshot *snapshot);
const ps_report *ps_graphic_snapshot_report(const ps_graphic_snapshot *snapshot);
ps_result ps_graphic_snapshot_describe(const ps_graphic_snapshot *snapshot,ps_graphic_description *out);
ps_result ps_graphic_snapshot_run(const ps_graphic_snapshot *snapshot,uint32_t index,ps_graphic_run_info *out);
#endif
