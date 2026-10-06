#ifndef PHYSIM_REPORT_H
#define PHYSIM_REPORT_H
#include "series.h"

#define PS_REPORT_MAX_PLOTS 16u
#define PS_REPORT_MAX_CURVES 8u
#define PS_REPORT_MAX_POINTS 2048u
#define PS_REPORT_MAX_TABLES 8u
#define PS_REPORT_MAX_COLUMNS 8u
#define PS_REPORT_MAX_ROWS 256u
#define PS_REPORT_MAX_BINS 128u
typedef struct ps_report ps_report;
/* Handles are valid only in their live report. Items are append-only. */
typedef struct {
    const ps_report *owner;
    uint32_t index;
} ps_plot_handle;
typedef struct {
    const ps_report *owner;
    uint32_t index;
} ps_table_handle;
typedef enum { PS_PLOT_LINE = 1, PS_PLOT_SCATTER = 2, PS_PLOT_HISTOGRAM = 3 } ps_plot_kind;
typedef struct {
    int8_t dimension[7];
    double scale;
    char symbol[64];
} ps_report_unit;
typedef struct {
    char title[192], x_label[96], y_label[96];
    ps_report_unit x_unit, y_unit;
    uint32_t curves;
} ps_plot_info;
typedef struct {
    char label[96];
    ps_plot_kind kind;
    uint32_t count;
    uint64_t source_count;
    double bar_width;
    double x[PS_REPORT_MAX_POINTS], y[PS_REPORT_MAX_POINTS];
} ps_curve_data;
typedef struct {
    char label[96];
    ps_report_unit unit;
} ps_table_column;
typedef struct {
    char title[192];
    uint32_t columns, rows;
    ps_table_column column[PS_REPORT_MAX_COLUMNS];
} ps_table_info;
typedef struct {
    char label[96];
    double values[PS_REPORT_MAX_COLUMNS];
} ps_table_row;

/* Titles/labels are bounded, valid UTF-8 without control characters. Provenance
 * is inert UTF-8 text, up to 8191 bytes; newline/tab are permitted. No file or
 * URL in a report is ever followed by the reader. Failed mutations are atomic. */
ps_result ps_report_create(const char *title, const char *provenance, ps_report **out);
/* Allocator covers the report, curves/tables and save/load/export work buffers. The
 * descriptor is copied; callback code/user state must outlive the report.
 * The normal constructor/load use ps_allocator_default(). C stdio internals
 * are outside this domain. See memory.h for alignment and ownership rules. */
ps_result ps_report_create_with_allocator(const char *title, const char *provenance,
                                          ps_allocator allocator, ps_report **out);
void ps_report_destroy(ps_report *report);
ps_result ps_report_describe(const ps_report *report, char title[192], char provenance[8192],
                             uint32_t *plots, uint32_t *tables);
ps_result ps_report_unit_from(ps_unit unit, ps_report_unit *out);
ps_result ps_report_add_plot(ps_report *report, const ps_plot_info *info, ps_plot_handle *out);
/* Data already uses the plot axis units. Finite values only. A histogram's
 * positive bar_width uses the X unit; its Y values are nonnegative counts. */
ps_result ps_report_add_curve(ps_report *report, ps_plot_handle plot, const ps_curve_data *data);
/* Optional curve flags: 0=missing, 1=valid/connected, 3=valid/new segment.
 * NULL means all valid/connected. Flags are copied and must match data->count.
 * Existing ps_curve_data layout and ordinary curves remain unchanged. */
ps_result ps_report_add_curve_masked(ps_report *report, ps_plot_handle plot,
                                     const ps_curve_data *data, const uint8_t *flags);
/* Borrowed flags; NULL denotes an unmasked curve. Same lifetime as curve_view. */
ps_result ps_report_curve_mask(const ps_report *report, uint32_t plot, uint32_t curve,
                                const uint8_t **out);
/* Copies aligned x/y data with checked dimensions and axis-scale conversion.
 * Line previews preserve bucket extrema and both endpoints; scatter previews
 * sample uniformly. Masks and segment boundaries survive reduction; missing
 * points are not drawn and line segments never bridge gaps. source_count
 * exposes reduction. Full data remains in Series. */
ps_result ps_report_add_series(ps_report *report, ps_plot_handle plot, ps_analysis_context *ctx,
                               ps_series x, ps_series y, const char *label, ps_plot_kind kind);
/* Equal-width bins over the entire series; final bin includes the maximum.
 * Constant input produces a single nonzero-width bin. Only valid samples count. */
ps_result ps_report_add_histogram(ps_report *report, ps_analysis_context *ctx, ps_series series,
                                  const char *title, const char *x_label, uint32_t bins,
                                  ps_plot_handle *out);
ps_result ps_report_add_table(ps_report *report, const ps_table_info *info, ps_table_handle *out);
ps_result ps_report_add_row(ps_report *report, ps_table_handle table, const ps_table_row *row);
ps_result ps_report_plot_read(const ps_report *report, uint32_t index, ps_plot_info *out);
ps_result ps_report_curve_read(const ps_report *report, uint32_t plot, uint32_t curve,
                               ps_curve_data *out);
/* Borrowed immutable curve, no allocation/copy. Valid until report destruction;
 * appending curves, plots or tables does not invalidate it. Never free or modify
 * the returned object. Serialize report mutations/destruction against readers.
 * On failure *out is unchanged. Use curve_read for an independently owned copy. */
ps_result ps_report_curve_view(const ps_report *report, uint32_t plot, uint32_t curve,
                               const ps_curve_data **out);
ps_result ps_report_table_read(const ps_report *report, uint32_t table, ps_table_info *out);
ps_result ps_report_row_read(const ps_report *report, uint32_t table, uint32_t row,
                             ps_table_row *out);
/* Range includes all curves and histogram edges/zero. Empty plots are invalid.
 * Mapping is overflow-safe, including very large finite coordinates. */
ps_result ps_report_plot_bounds(const ps_report *report, uint32_t plot, double bounds[4]);
double ps_report_axis_fraction(double value, double minimum, double maximum);
/* Versioned little-endian .psreport, CRC checked, bounded to 8 MiB. Version 1
 * remains for ordinary reports; version 2 adds masks. Both versions load. Load is
 * transactional; truncated, malformed, nonfinite or oversized data is rejected.
 * Outputs are created exclusively; failed writes may leave a partial file. */
ps_result ps_report_save(const ps_report *report, const char *path);
ps_result ps_report_load(const char *path, ps_report **out);
ps_result ps_report_load_with_allocator(const char *path, ps_allocator allocator, ps_report **out);
ps_result ps_report_export_svg(const ps_report *report, uint32_t plot, const char *path);
/* Finite increasing {xmin,xmax,ymin,ymax}; NULL exports the complete plot.
 * Curves are clipped to the axes. Unrepresentable coordinates return PS_LIMIT. */
ps_result ps_report_export_svg_region(const ps_report *report, uint32_t plot,
                                      const double bounds[4], const char *path);
ps_result ps_report_export_table_csv(const ps_report *report, uint32_t table, const char *path);
ps_result ps_report_export_plot_csv(const ps_report *report, uint32_t plot, const char *path);
#endif
