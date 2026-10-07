#ifndef PHYSIM_LANGUAGE_ANALYSIS_SDK_H
#define PHYSIM_LANGUAGE_ANALYSIS_SDK_H
#include "language_sdk.h"
#include "language_array.h"
#include "language_string.h"
#include "report.h"

/* All resources belong to the synchronous analysis invocation. Copies of handles
 * preserve the core's owner/generation checks; cleanup also runs after traps. */
typedef struct {
    ps_analysis_context *context;
    ps_report *report;
    const char *const *inputs;
    size_t count;
    const char *prefix, *provenance;
    ps_dataset datasets[PS_ANALYSIS_MAX_INPUTS];
    bool recovered;
    ps_result result;
    const ps_analysis_services *services;
} psra_host;
static inline void psra_check(psra_host *h, ps_result r, psrt_site site) {
    if (r != PS_OK) {
        h->result = r;
        psrt_fail(site, ps_result_string(r));
    }
}
#include "language_batch_host.h"
/* Handles remain owned by the analysis context. Only the returned array block
 * is a language owner; releasing a copied Series still invalidates its aliases. */
static inline ps_series psra_mask(psra_host *h,ps_series input,ps_series selector,double accepted,psrt_site site) {
    ps_series result;psra_check(h,ps_series_mask(h->context,input,selector,accepted,&result),site);return result;
}
static inline ps_series psra_validity(psra_host *h,ps_series input,psrt_site site) {
    ps_series result;psra_check(h,ps_series_validity(h->context,input,&result),site);return result;
}
static inline bool psra_has_mask(psra_host *h,ps_series input,psrt_site site) {
    bool result;psra_check(h,ps_series_is_masked(h->context,input,&result),site);return result;
}
static inline bool psra_is_valid(psra_host *h,ps_series input,int64_t index,psrt_site site) {
    if(index<0)psra_check(h,PS_INVALID,site);
    double value;uint8_t valid;size_t got;
    psra_check(h,ps_series_read_masked(h->context,input,(uint64_t)index,&value,&valid,1,&got),site);
    if(got!=1)psra_check(h,PS_INVALID,site);
    return valid!=0;
}
static inline psrt_array psra_select(psra_host *h, ps_allocator allocator,
    const ps_series *columns, size_t count, ps_series selector, double accepted, psrt_site site) {
    if (!count || count > 32)
        psra_check(h, PS_INVALID, site);
    ps_series selected[32];
    psra_check(h, ps_series_select(h->context, columns, count, selector, accepted, selected), site);
    static const psrt_element_type element = {sizeof(ps_series), NULL, NULL};
    psrt_array result;
    psra_check(h, psrt_array_init(&element, allocator, 0, &result), site);
    /* replace is transactional: on failure result still owns no allocation.
     * The invocation's trap cleanup releases all selected scratch series. */
    psra_check(h, psrt_array_replace(&result, 0, 0, selected, count), site);
    return result;
}
static inline ps_table_handle psra_table(psra_host *h, const char *title,
    const char *const *labels, size_t count, const ps_unit *units, size_t unit_count, psrt_site site) {
    if (!count || count > PS_REPORT_MAX_COLUMNS || count != unit_count)
        psra_check(h, PS_INVALID, site);
    ps_table_info info = {0};
    if (strlen(title) >= sizeof info.title)
        psra_check(h, PS_LIMIT, site);
    memcpy(info.title, title, strlen(title) + 1);
    info.columns = (uint32_t)count;
    for (size_t i = 0; i < count; i++) {
        if (strlen(labels[i]) >= sizeof info.column[i].label)
            psra_check(h, PS_LIMIT, site);
        memcpy(info.column[i].label, labels[i], strlen(labels[i]) + 1);
        psra_check(h, ps_report_unit_from(units[i], &info.column[i].unit), site);
    }
    ps_table_handle table;
    psra_check(h, ps_report_add_table(h->report, &info, &table), site);
    return table;
}
static inline void psra_row(psra_host *h, ps_table_handle table, const char *label,
    const ps_quantity *values, size_t count, psrt_site site) {
    if (table.owner != h->report)
        psra_check(h, PS_INVALID, site);
    ps_table_info info;
    psra_check(h, ps_report_table_read(h->report, table.index, &info), site);
    if (count != info.columns)
        psra_check(h, PS_INVALID, site);
    ps_table_row row = {0};
    if (strlen(label) >= sizeof row.label)
        psra_check(h, PS_LIMIT, site);
    memcpy(row.label, label, strlen(label) + 1);
    for (size_t i = 0; i < count; i++) {
        ps_unit unit = {{0}, info.column[i].unit.scale, info.column[i].unit.symbol};
        memcpy(unit.dimension, info.column[i].unit.dimension, 7);
        psra_check(h, ps_convert(values[i].value, values[i].unit, unit, &row.values[i]), site);
    }
    psra_check(h, ps_report_add_row(h->report, table, &row), site);
}
static inline void psra_path(psra_host *h, const char *suffix, const char *extension,
                             char path[4096], psrt_site site) {
    if (!*suffix || strlen(suffix) > 64)
        psra_check(h, PS_INVALID, site);
    for (const unsigned char *p = (const unsigned char *)suffix; *p; p++)
        if (!((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9') ||
              *p == '-' || *p == '_'))
            psra_check(h, PS_INVALID, site);
    int n = snprintf(path, 4096, "%s-%s.%s", h->prefix, suffix, extension);
    if (n < 0 || n >= 4096)
        psra_check(h, PS_LIMIT, site);
}
static inline psrt_string psra_input_path(psra_host *h,ps_allocator allocator,int64_t index,psrt_site site) {
    if(index<0 || (uint64_t)index>=h->count)psra_check(h,PS_INVALID,site);
    psrt_string result;psra_check(h,psrt_string_make(allocator,h->inputs[index],strlen(h->inputs[index]),&result),site);return result;
}
static inline psrt_string psra_output_prefix(psra_host *h,ps_allocator allocator,psrt_site site) {
    psrt_string result;psra_check(h,psrt_string_make(allocator,h->prefix,strlen(h->prefix),&result),site);return result;
}
static inline int64_t psra_input_count(psra_host *h, psrt_site site) {
    (void)site;
    return (int64_t)h->count;
}
static inline ps_dataset psra_dataset(psra_host *h, int64_t index, psrt_site site) {
    if (index < 0 || (uint64_t)index >= h->count)
        psra_check(h, PS_INVALID, site);
    if (!h->datasets[index].owner) {
        ps_result r = ps_analysis_open_run(h->context, h->inputs[index], &h->datasets[index]);
        if (r == PS_RECOVERED)
            h->recovered = true;
        else
            psra_check(h, r, site);
    }
    return h->datasets[index];
}
static inline int64_t psra_dataset_sample_count(psra_host *h, ps_dataset dataset,
                                                psrt_site site) {
    ps_dataset_info info;
    psra_check(h, ps_dataset_describe(h->context, dataset, &info), site);
    if (info.samples > INT64_MAX)
        psra_check(h, PS_LIMIT, site);
    return (int64_t)info.samples;
}
static inline int64_t psra_dataset_channel_count(psra_host *h, ps_dataset dataset,
                                                 psrt_site site) {
    ps_dataset_info info;
    psra_check(h, ps_dataset_describe(h->context, dataset, &info), site);
    return (int64_t)info.channel_count;
}
static inline ps_channel psra_channel_info(psra_host *h, ps_dataset dataset, int64_t index,
                                           psrt_site site) {
    ps_dataset_info info;
    psra_check(h, ps_dataset_describe(h->context, dataset, &info), site);
    if (info.channel_count > PS_MAX_CHANNELS)
        psra_check(h, PS_CORRUPT, site);
    if (index < 0 || (uint64_t)index >= info.channel_count)
        psra_check(h, PS_INVALID, site);
    return info.channels[index];
}
static inline psrt_string psra_channel_text(psra_host *h, ps_allocator allocator,
                                            const char *text, size_t capacity,
                                            psrt_site site) {
    size_t length = 0;
    while (length < capacity && text[length])
        length++;
    if (length == capacity)
        psra_check(h, PS_CORRUPT, site);
    psrt_string result;
    psra_check(h, psrt_string_make(allocator, text, length, &result), site);
    return result;
}
static inline psrt_string psra_channel_name(psra_host *h, ps_allocator allocator,
                                            ps_dataset dataset, int64_t index, psrt_site site) {
    ps_channel channel = psra_channel_info(h, dataset, index, site);
    return psra_channel_text(h, allocator, channel.name, sizeof channel.name, site);
}
static inline psrt_string psra_channel_unit_symbol(psra_host *h, ps_allocator allocator,
                                                   ps_dataset dataset, int64_t index,
                                                   psrt_site site) {
    ps_channel channel = psra_channel_info(h, dataset, index, site);
    return psra_channel_text(h, allocator, channel.unit, sizeof channel.unit, site);
}
static inline psrt_string psra_channel_description(psra_host *h, ps_allocator allocator,
                                                   ps_dataset dataset, int64_t index,
                                                   psrt_site site) {
    ps_channel channel = psra_channel_info(h, dataset, index, site);
    return psra_channel_text(h, allocator, channel.description, sizeof channel.description, site);
}
static inline int64_t psra_channel_exponent(psra_host *h, ps_dataset dataset, int64_t index,
                                            int64_t axis, psrt_site site) {
    if (axis < 0 || axis >= 7)
        psra_check(h, PS_INVALID, site);
    ps_channel channel = psra_channel_info(h, dataset, index, site);
    return channel.dimension[axis];
}
static inline bool psra_dataset_recovered(psra_host *h, ps_dataset dataset,
                                          psrt_site site) {
    ps_dataset_info info;
    psra_check(h, ps_dataset_describe(h->context, dataset, &info), site);
    return info.recovered;
}
static inline psrt_string psra_dataset_metadata(psra_host *h, ps_allocator allocator,
                                                 ps_dataset dataset, psrt_site site) {
    ps_dataset_info info;
    psra_check(h, ps_dataset_describe(h->context, dataset, &info), site);
    size_t length = 0;
    while (length < sizeof info.metadata && info.metadata[length])
        length++;
    if (length == sizeof info.metadata)
        psra_check(h, PS_CORRUPT, site);
    psrt_string result;
    psra_check(h, psrt_string_make(allocator, info.metadata, length, &result), site);
    return result;
}
static inline ps_series psra_series(psra_host *h, ps_dataset dataset, const char *name,
                                    psrt_site site) {
    ps_series result;
    psra_check(h, ps_dataset_series(h->context, dataset, name, &result), site);
    return result;
}
static inline ps_series psra_series_from_values(psra_host *h, const double *values, size_t count,
                                                ps_unit unit, const char *name, psrt_site site) {
    ps_series result;
    psra_check(h, ps_series_from_values(h->context, values, count, unit, name, &result), site);
    return result;
}
static inline ps_series psra_series_aligned_values(psra_host *h, ps_series anchor,
                                                   const double *values, size_t count,
                                                   ps_unit unit, const char *name,
                                                   psrt_site site) {
    ps_series result;
    psra_check(h, ps_series_aligned_values(h->context, anchor, values, count, unit, name, &result),
               site);
    return result;
}
static inline int64_t psra_count(psra_host *h, ps_series input, psrt_site site) {
    ps_series_info info;
    psra_check(h, ps_series_describe(h->context, input, &info), site);
    if (info.count > INT64_MAX)
        psra_check(h, PS_LIMIT, site);
    return (int64_t)info.count;
}
static inline psrt_string psra_series_name(psra_host *h, ps_allocator allocator,
                                           ps_series input, psrt_site site) {
    ps_series_info info;
    psra_check(h, ps_series_describe(h->context, input, &info), site);
    size_t length = 0;
    while (length < sizeof info.name && info.name[length])
        length++;
    if (length == sizeof info.name)
        psra_check(h, PS_CORRUPT, site);
    psrt_string result;
    psra_check(h, psrt_string_make(allocator, info.name, length, &result), site);
    return result;
}
static inline psrt_string psra_series_unit_symbol(psra_host *h, ps_allocator allocator,
                                                  ps_series input, psrt_site site) {
    ps_series_info info;
    psra_check(h, ps_series_describe(h->context, input, &info), site);
    size_t length = 0;
    while (length < sizeof info.symbol && info.symbol[length])
        length++;
    if (length == sizeof info.symbol)
        psra_check(h, PS_CORRUPT, site);
    psrt_string result;
    psra_check(h, psrt_string_make(allocator, info.symbol, length, &result), site);
    return result;
}
static inline double psra_series_unit_scale(psra_host *h, ps_series input, psrt_site site) {
    ps_series_info info;
    psra_check(h, ps_series_describe(h->context, input, &info), site);
    return psrt_finite(info.scale, site);
}
static inline int64_t psra_series_exponent(psra_host *h, ps_series input, int64_t axis,
                                           psrt_site site) {
    if (axis < 0 || axis >= 7)
        psra_check(h, PS_INVALID, site);
    ps_series_info info;
    psra_check(h, ps_series_describe(h->context, input, &info), site);
    return info.dimension[axis];
}
static inline bool psra_series_aligned(psra_host *h, ps_series left, ps_series right,
                                       psrt_site site) {
    ps_series_info info;
    psra_check(h, ps_series_describe(h->context, left, &info), site);
    psra_check(h, ps_series_describe(h->context, right, &info), site);
    return ps_series_aligned(h->context, left, right) == PS_OK;
}
static inline ps_series psra_slice(psra_host *h, ps_series input, int64_t first,
                                   int64_t count, psrt_site site) {
    if (first < 0 || count < 0)
        psra_check(h, PS_INVALID, site);
    ps_series result;
    psra_check(h, ps_series_slice(h->context, input, (uint64_t)first, (uint64_t)count,
                                 &result), site);
    return result;
}
static inline double psra_value(psra_host *h, ps_series input, int64_t index, psrt_site site) {
    if (index < 0)
        psra_check(h, PS_INVALID, site);
    double value;
    size_t count;uint8_t valid;
    psra_check(h, ps_series_read_masked(h->context,input,(uint64_t)index,&value,&valid,1,&count), site);
    if (count != 1 || !valid)
        psra_check(h, PS_INVALID, site);
    return psrt_finite(value, site);
}
static inline psrt_array psra_values(psra_host *h, ps_allocator allocator, ps_series input,
                                     int64_t first, int64_t count, psrt_site site) {
    if (first < 0 || count < 0)
        psra_check(h, PS_INVALID, site);
    ps_series_info info;
    psra_check(h, ps_series_describe(h->context, input, &info), site);
    if ((uint64_t)first > info.count || (uint64_t)count > info.count - (uint64_t)first)
        psra_check(h, PS_INVALID, site);
    if ((uint64_t)count > SIZE_MAX)
        psra_check(h, PS_LIMIT, site);
    static const psrt_element_type element = {sizeof(double), NULL, NULL};
    psrt_array result;
    psra_check(h, psrt_array_init(&element, allocator, (size_t)count, &result), site);
    psra_check(h, psrt_array_build_begin(&result, (size_t)count), site);
    double block[PS_SERIES_BLOCK_SIZE];uint8_t valid[PS_SERIES_BLOCK_SIZE];
    for (uint64_t at = 0; at < (uint64_t)count;) {
        size_t take = (size_t)((uint64_t)count - at > PS_SERIES_BLOCK_SIZE
                                   ? PS_SERIES_BLOCK_SIZE : (uint64_t)count - at);
        size_t got = 0;
        ps_result status=ps_series_read_masked(h->context,input,(uint64_t)first+at,block,valid,take,&got);
        if (status == PS_OK && got != take)
            status = PS_CORRUPT;
        for (size_t i = 0; i < got && status == PS_OK; i++) {
            if(!valid[i])status=PS_INVALID;
            else if (!isfinite(block[i]))
                status = PS_NUMERIC;
            else
                status = psrt_array_copy_one(result.block, &block[i]);
        }
        if (status != PS_OK) {
            psrt_array_destroy(&result);
            psra_check(h, status, site);
        }
        at += take;
    }
    return result;
}
static inline double psra_mean(psra_host *h, ps_series input, psrt_site site) {
    ps_statistics stats;
    psra_check(h, ps_series_statistics(h->context, input, &stats), site);
    if (!stats.count)
        psra_check(h, PS_INVALID, site);
    return psrt_finite(stats.mean, site);
}
static inline double psra_stddev(psra_host *h, ps_series input, psrt_site site) {
    ps_statistics stats;
    psra_check(h, ps_series_statistics(h->context, input, &stats), site);
    if (stats.count < 2)
        psra_check(h, PS_INVALID, site);
    return psrt_finite(ps_statistics_stddev(&stats), site);
}
static inline double psra_quantile(psra_host *h, ps_allocator allocator, ps_series input,
                                   double probability, psrt_site site) {
    double value;
    psra_check(h, ps_series_quantile_with_allocator(h->context, input, probability,
                                                    allocator, &value), site);
    return psrt_finite(value, site);
}
static inline double psra_extreme(psra_host *h, ps_series input, bool maximum,
                                  psrt_site site) {
    ps_series_info info;
    psra_check(h, ps_series_describe(h->context, input, &info), site);
    if (!info.count)
        psra_check(h, PS_INVALID, site);
    double values[PS_SERIES_BLOCK_SIZE], result = 0;
    bool seen = false;
    for (uint64_t at = 0; at < info.count;) {
        size_t got = 0;
        psra_check(h, ps_series_read(h->context, input, at, values,
                                     PS_SERIES_BLOCK_SIZE, &got), site);
        if (!got || got > PS_SERIES_BLOCK_SIZE || got > info.count - at)
            psra_check(h, PS_CORRUPT, site);
        for (size_t i = 0; i < got; i++) {
            double value = psrt_finite(values[i], site);
            if (!seen || (maximum ? value > result : value < result))
                result = value;
            seen = true;
        }
        at += got;
    }
    return result;
}
static inline double psra_minimum(psra_host *h, ps_series input, psrt_site site) {
    return psra_extreme(h, input, false, site);
}
static inline double psra_maximum(psra_host *h, ps_series input, psrt_site site) {
    return psra_extreme(h, input, true, site);
}
static inline ps_series psra_derivative(psra_host *h, ps_series y, ps_series x, psrt_site site) {
    ps_series result;
    psra_check(h, ps_series_derivative(h->context, y, x, &result), site);
    return result;
}
static inline ps_series psra_average(psra_host *h, ps_series input, int64_t window,
                                     psrt_site site) {
    if (window < 1 || window > PS_SERIES_MAX_WINDOW)
        psra_check(h, PS_INVALID, site);
    ps_series result;
    psra_check(h, ps_series_moving_average(h->context, input, (size_t)window, &result), site);
    return result;
}
static inline ps_series psra_integral(psra_host *h, ps_series y, ps_series x, double initial,
                                      ps_unit unit, psrt_site site) {
    ps_series result;
    psra_check(h, ps_series_integral(h->context, y, x, (ps_quantity){initial, unit}, &result),
               site);
    return result;
}
static inline ps_series psra_affine(psra_host *h, ps_series input, double factor, double offset,
                                    ps_unit unit, psrt_site site) {
    ps_series result;
    psra_check(h, ps_series_affine(h->context, input, factor, (ps_quantity){offset, unit}, &result),
               site);
    return result;
}
#define PSRA_COMBINE(name, operation)                                                              \
    static inline ps_series name(psra_host *h, ps_series left, ps_series right, psrt_site site) {  \
        ps_series result;                                                                          \
        psra_check(h, ps_series_combine(h->context, operation, left, right, &result), site);       \
        return result;                                                                             \
    }
PSRA_COMBINE(psra_add, PS_SERIES_ADD)
PSRA_COMBINE(psra_subtract, PS_SERIES_SUBTRACT)
PSRA_COMBINE(psra_multiply, PS_SERIES_MULTIPLY)
PSRA_COMBINE(psra_divide, PS_SERIES_DIVIDE)
#undef PSRA_COMBINE
#define PSRA_RESAMPLE(name, method)                                                                \
    static inline ps_series name(psra_host *h, ps_series y, ps_series x, ps_series target,         \
                                 psrt_site site) {                                                 \
        ps_series result;                                                                          \
        psra_check(h, ps_series_resample(h->context, y, x, target, method, &result), site);        \
        return result;                                                                             \
    }
PSRA_RESAMPLE(psra_linear, PS_RESAMPLE_LINEAR)
PSRA_RESAMPLE(psra_nearest, PS_RESAMPLE_NEAREST)
PSRA_RESAMPLE(psra_previous, PS_RESAMPLE_PREVIOUS)
PSRA_RESAMPLE(psra_pchip, PS_RESAMPLE_PCHIP)
#undef PSRA_RESAMPLE
static inline void psra_release(psra_host *h, ps_series input, psrt_site site) {
    psra_check(h, ps_series_release(h->context, input), site);
}
static inline void psra_close(psra_host *h, ps_dataset dataset, psrt_site site) {
    psra_check(h, ps_dataset_close(h->context, dataset), site);
    for (size_t i = 0; i < h->count; i++)
        if (h->datasets[i].owner == dataset.owner && h->datasets[i].slot == dataset.slot &&
            h->datasets[i].generation == dataset.generation)
            h->datasets[i] = (ps_dataset){0};
}
static inline void psra_report(psra_host *h, const char *title, psrt_site site) {
    if (h->report)
        psra_check(h, PS_INVALID, site);
    psra_check(h, ps_report_create(title, h->provenance, &h->report), site);
}
static inline void psra_curve(psra_host *h, ps_plot_handle plot, ps_series x, ps_series y,
                              const char *label, psrt_site site) {
    psra_check(h, ps_report_add_series(h->report, plot, h->context, x, y, label, PS_PLOT_LINE),
               site);
}
static inline void psra_points(psra_host *h, ps_plot_handle plot, ps_series x, ps_series y,
                                const char *label, psrt_site site) {
    psra_check(h, ps_report_add_series(h->report, plot, h->context, x, y, label, PS_PLOT_SCATTER), site);
}
static inline ps_plot_handle psra_plot(psra_host *h, ps_series x, ps_series y, const char *title,
                                       const char *label, psrt_site site) {
    ps_series_info xi, yi;
    psra_check(h, ps_series_describe(h->context, x, &xi), site);
    psra_check(h, ps_series_describe(h->context, y, &yi), site);
    ps_plot_info info = {0};
    if (strlen(title) >= sizeof info.title)
        psra_check(h, PS_LIMIT, site);
    memcpy(info.title, title, strlen(title) + 1);
    memcpy(info.x_label, xi.name, strlen(xi.name) + 1);
    memcpy(info.y_label, yi.name, strlen(yi.name) + 1);
    memcpy(info.x_unit.dimension, xi.dimension, 7);
    memcpy(info.y_unit.dimension, yi.dimension, 7);
    info.x_unit.scale = xi.scale;
    info.y_unit.scale = yi.scale;
    memcpy(info.x_unit.symbol, xi.symbol, strlen(xi.symbol) + 1);
    memcpy(info.y_unit.symbol, yi.symbol, strlen(yi.symbol) + 1);
    ps_plot_handle result;
    psra_check(h, ps_report_add_plot(h->report, &info, &result), site);
    psra_curve(h, result, x, y, label, site);
    return result;
}
static inline ps_plot_handle psra_histogram(psra_host *h, ps_series input, const char *title,
                                            int64_t bins, psrt_site site) {
    if (bins < 1 || bins > PS_REPORT_MAX_BINS)
        psra_check(h, PS_INVALID, site);
    ps_plot_handle result;
    psra_check(h,
               ps_report_add_histogram(h->report, h->context, input, title, "value", (uint32_t)bins,
                                       &result),
               site);
    return result;
}
static inline void psra_export(psra_host *h, ps_series x, ps_series y, const char *suffix,
                               psrt_site site) {
    char path[4096];
    psra_path(h, suffix, "csv", path, site);
    ps_series columns[] = {x, y};
    psra_check(h, ps_series_export_csv(h->context, columns, 2, path), site);
}
static inline void psra_export_columns(psra_host *h, const ps_series *columns, size_t count,
                                       const char *suffix, psrt_site site) {
    if (!count || count > 32)
        psra_check(h, PS_INVALID, site);
    char path[4096];
    psra_path(h, suffix, "csv", path, site);
    psra_check(h, ps_series_export_csv(h->context, columns, count, path), site);
}
static inline void psra_svg(psra_host *h, ps_plot_handle plot, const char *suffix, psrt_site site) {
    if (plot.owner != h->report)
        psra_check(h, PS_INVALID, site);
    char path[4096];
    psra_path(h, suffix, "svg", path, site);
    psra_check(h, ps_report_export_svg(h->report, plot.index, path), site);
}
static inline void psra_table_csv(psra_host *h, ps_table_handle table, const char *suffix, psrt_site site) {
    if (table.owner != h->report)
        psra_check(h, PS_INVALID, site);
    char path[4096];
    psra_path(h, suffix, "csv", path, site);
    psra_check(h, ps_report_export_table_csv(h->report, table.index, path), site);
}
#endif
