#include "text_validation.h"
#include "report_internal.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    ps_plot_info info;
    ps_curve_data *curve[PS_REPORT_MAX_CURVES];
    uint8_t *mask[PS_REPORT_MAX_CURVES];
} plot_data;
typedef struct {
    ps_table_info info;
    ps_table_row row[PS_REPORT_MAX_ROWS];
} table_data;
struct ps_report {
    ps_allocator allocator;
    char title[192], provenance[8192];
    uint32_t plots, tables;
    plot_data plot[PS_REPORT_MAX_PLOTS];
    table_data *table[PS_REPORT_MAX_TABLES];
};
ps_allocator ps_report_allocator_internal(const ps_report *r) {
    return r ? r->allocator : (ps_allocator){0};
}
static void *report_memory(ps_allocator allocator, size_t bytes) {
    void *p = NULL;
    (void)ps_memory_zero(allocator, 1, bytes, &p);
    return p;
}
static void *report_buffer(ps_allocator allocator, size_t bytes) {
    void *p = NULL;
    (void)ps_memory_allocate(allocator, bytes, &p);
    return p;
}
static bool unit_valid(const ps_report_unit *u) {
    return isfinite(u->scale) && u->scale > 0 && ps_text_valid(u->symbol, sizeof u->symbol, false);
}
ps_result ps_report_unit_from(ps_unit u, ps_report_unit *out) {
    if (!out || !ps_unit_valid(u) || !u.symbol || strlen(u.symbol) >= sizeof out->symbol)
        return PS_INVALID;
    ps_report_unit value = {{0}, u.scale, {0}};
    memcpy(value.dimension, u.dimension, 7);
    strcpy(value.symbol, u.symbol);
    if (!unit_valid(&value))
        return PS_INVALID;
    *out = value;
    return PS_OK;
}
ps_result ps_report_create(const char *title, const char *provenance, ps_report **out) {
    return ps_report_create_with_allocator(title, provenance, ps_allocator_default(), out);
}
ps_result ps_report_create_with_allocator(const char *title, const char *provenance,
                                          ps_allocator allocator, ps_report **out) {
    if (!out || !title || !provenance || !title[0] || !ps_text_valid(title, 192, false) ||
        !ps_text_valid(provenance, 8192, true) || !ps_allocator_valid(allocator))
        return PS_INVALID;
    ps_report *r = report_memory(allocator, sizeof *r);
    if (!r)
        return PS_MEMORY;
    r->allocator = allocator;
    strcpy(r->title, title);
    strcpy(r->provenance, provenance);
    *out = r;
    return PS_OK;
}
void ps_report_destroy(ps_report *r) {
    if (!r)
        return;
    for (uint32_t i = 0; i < r->plots; i++)
        for (uint32_t j = 0; j < r->plot[i].info.curves; j++) {
            ps_memory_free(r->allocator,r->plot[i].mask[j],PS_REPORT_MAX_POINTS);
            ps_memory_free(r->allocator, r->plot[i].curve[j], sizeof *r->plot[i].curve[j]);
        }
    for (uint32_t i = 0; i < r->tables; i++)
        ps_memory_free(r->allocator, r->table[i], sizeof *r->table[i]);
    ps_memory_free(r->allocator, r, sizeof *r);
}
ps_result ps_report_describe(const ps_report *r, char title[192], char provenance[8192],
                             uint32_t *plots, uint32_t *tables) {
    if (!r)
        return PS_INVALID;
    if (title)
        memcpy(title, r->title, sizeof r->title);
    if (provenance)
        memcpy(provenance, r->provenance, sizeof r->provenance);
    if (plots)
        *plots = r->plots;
    if (tables)
        *tables = r->tables;
    return PS_OK;
}
static bool plot_valid(const ps_plot_info *p) {
    return p && p->title[0] && ps_text_valid(p->title, sizeof p->title, false) &&
           ps_text_valid(p->x_label, sizeof p->x_label, false) &&
           ps_text_valid(p->y_label, sizeof p->y_label, false) && unit_valid(&p->x_unit) &&
           unit_valid(&p->y_unit);
}
static bool curve_valid(const ps_curve_data *c) {
    if (!c || !ps_text_valid(c->label, sizeof c->label, false) || !c->count ||
        c->count > PS_REPORT_MAX_POINTS || !c->source_count ||
        (c->kind != PS_PLOT_HISTOGRAM && c->source_count < c->count) || c->kind < PS_PLOT_LINE ||
        c->kind > PS_PLOT_HISTOGRAM || !isfinite(c->bar_width))
        return false;
    if (c->kind == PS_PLOT_HISTOGRAM && c->bar_width <= 0)
        return false;
    for (uint32_t i = 0; i < c->count; i++) {
        if (!isfinite(c->x[i]) || !isfinite(c->y[i]))
            return false;
        if (c->kind == PS_PLOT_HISTOGRAM && (c->y[i] < 0 || !isfinite(c->x[i] - c->bar_width / 2) ||
                                             !isfinite(c->x[i] + c->bar_width / 2)))
            return false;
    }
    return true;
}
ps_result ps_report_add_plot(ps_report *r, const ps_plot_info *info, ps_plot_handle *out) {
    if (!r || !out || !plot_valid(info) || info->curves)
        return PS_INVALID;
    if (r->plots == PS_REPORT_MAX_PLOTS)
        return PS_LIMIT;
    r->plot[r->plots].info = *info;
    *out = (ps_plot_handle){r, r->plots++};
    return PS_OK;
}
ps_result ps_report_add_curve(ps_report *r,ps_plot_handle h,const ps_curve_data *data) {
    return ps_report_add_curve_masked(r,h,data,NULL);
}
ps_result ps_report_add_curve_masked(ps_report *r,ps_plot_handle h,const ps_curve_data *data,const uint8_t *flags) {
    if (!r || h.owner != r || h.index >= r->plots || !curve_valid(data))
        return PS_INVALID;
    plot_data *p = &r->plot[h.index];
    if (p->info.curves == PS_REPORT_MAX_CURVES)
        return PS_LIMIT;
    if (data->kind == PS_PLOT_HISTOGRAM) {
        const int8_t zero[7] = {0};
        if (memcmp(p->info.y_unit.dimension, zero, 7) || p->info.y_unit.scale != 1)
            return PS_INVALID;
    }
    bool masked=false;
    if(flags)for(uint32_t i=0;i<data->count;i++) {
        if(flags[i]!=0 && flags[i]!=1 && flags[i]!=3)return PS_INVALID;
        masked |= flags[i]!=1;
    }
    ps_curve_data *c = report_memory(r->allocator, sizeof *c);
    if (!c)
        return PS_MEMORY;
    uint8_t *mask=masked?report_memory(r->allocator,PS_REPORT_MAX_POINTS):NULL;
    if(masked && !mask){ps_memory_free(r->allocator,c,sizeof *c);return PS_MEMORY;}
    if(mask)memcpy(mask,flags,data->count);
    *c = *data;p->mask[p->info.curves]=mask;
    p->curve[p->info.curves++] = c;
    return PS_OK;
}
static bool table_valid(const ps_table_info *t) {
    if (!t || !t->title[0] || !ps_text_valid(t->title, sizeof t->title, false) || !t->columns ||
        t->columns > PS_REPORT_MAX_COLUMNS)
        return false;
    for (uint32_t i = 0; i < t->columns; i++)
        if (!ps_text_valid(t->column[i].label, sizeof t->column[i].label, false) ||
            !unit_valid(&t->column[i].unit))
            return false;
    return true;
}
ps_result ps_report_add_table(ps_report *r, const ps_table_info *info, ps_table_handle *out) {
    if (!r || !out || !table_valid(info) || info->rows)
        return PS_INVALID;
    if (r->tables == PS_REPORT_MAX_TABLES)
        return PS_LIMIT;
    table_data *t = report_memory(r->allocator, sizeof *t);
    if (!t)
        return PS_MEMORY;
    t->info = *info;
    r->table[r->tables] = t;
    *out = (ps_table_handle){r, r->tables++};
    return PS_OK;
}
ps_result ps_report_add_row(ps_report *r, ps_table_handle h, const ps_table_row *row) {
    if (!r || h.owner != r || h.index >= r->tables || !row ||
        !ps_text_valid(row->label, sizeof row->label, false))
        return PS_INVALID;
    table_data *t = r->table[h.index];
    if (t->info.rows == PS_REPORT_MAX_ROWS)
        return PS_LIMIT;
    for (uint32_t i = 0; i < t->info.columns; i++)
        if (!isfinite(row->values[i]))
            return PS_INVALID;
    t->row[t->info.rows++] = *row;
    return PS_OK;
}
ps_result ps_report_plot_read(const ps_report *r, uint32_t i, ps_plot_info *out) {
    if (!r || !out || i >= r->plots)
        return PS_INVALID;
    *out = r->plot[i].info;
    return PS_OK;
}
ps_result ps_report_curve_read(const ps_report *r, uint32_t p, uint32_t c, ps_curve_data *out) {
    if (!r || !out || p >= r->plots || c >= r->plot[p].info.curves)
        return PS_INVALID;
    *out = *r->plot[p].curve[c];
    return PS_OK;
}
ps_result ps_report_curve_view(const ps_report *r, uint32_t p, uint32_t c,
                               const ps_curve_data **out) {
    if (!r || !out || p >= r->plots || c >= r->plot[p].info.curves)
        return PS_INVALID;
    *out = r->plot[p].curve[c];
    return PS_OK;
}
ps_result ps_report_curve_mask(const ps_report *r,uint32_t p,uint32_t c,const uint8_t **out) {
    if(!r || !out || p>=r->plots || c>=r->plot[p].info.curves)return PS_INVALID;
    *out=r->plot[p].mask[c];return PS_OK;
}
ps_result ps_report_table_read(const ps_report *r, uint32_t i, ps_table_info *out) {
    if (!r || !out || i >= r->tables)
        return PS_INVALID;
    *out = r->table[i]->info;
    return PS_OK;
}
ps_result ps_report_row_read(const ps_report *r, uint32_t t, uint32_t row, ps_table_row *out) {
    if (!r || !out || t >= r->tables || row >= r->table[t]->info.rows)
        return PS_INVALID;
    *out = r->table[t]->row[row];
    return PS_OK;
}
static void point(ps_curve_data *c, double x, double y) {
    c->x[c->count] = x;
    c->y[c->count++] = y;
}
#include "report_mask.inc"
ps_result ps_report_add_series(ps_report *r, ps_plot_handle h, ps_analysis_context *ctx,
                               ps_series xs, ps_series ys, const char *label, ps_plot_kind kind) {
    if (!r || h.owner != r || h.index >= r->plots || !label || !ps_text_valid(label, 96, false) ||
        (kind != PS_PLOT_LINE && kind != PS_PLOT_SCATTER) ||
        ps_series_aligned(ctx, xs, ys) != PS_OK)
        return PS_INVALID;
    ps_series_info xi, yi;
    if (ps_series_describe(ctx, xs, &xi) != PS_OK || ps_series_describe(ctx, ys, &yi) != PS_OK ||
        !xi.count)
        return PS_INVALID;
    bool xm=false,ym=false;
    if(ps_series_is_masked(ctx,xs,&xm)!=PS_OK || ps_series_is_masked(ctx,ys,&ym)!=PS_OK)return PS_INVALID;
    if(xm || ym)return report_masked_series(r,h,ctx,xs,ys,label,kind,&xi,&yi);
    ps_plot_info *p = &r->plot[h.index].info;
    if (memcmp(xi.dimension, p->x_unit.dimension, 7) ||
        memcmp(yi.dimension, p->y_unit.dimension, 7))
        return PS_INVALID;
    ps_curve_data *c = report_memory(r->allocator, sizeof *c);
    if (!c)
        return PS_MEMORY;
    strcpy(c->label, label);
    c->kind = kind;
    c->source_count = xi.count;
    ps_unit xu = {{0}, xi.scale, NULL}, yu = {{0}, yi.scale, NULL};
    ps_unit target_x = {{0}, p->x_unit.scale, NULL}, target_y = {{0}, p->y_unit.scale, NULL};
    memcpy(xu.dimension, xi.dimension, 7);
    memcpy(yu.dimension, yi.dimension, 7);
    memcpy(target_x.dimension, p->x_unit.dimension, 7);
    memcpy(target_y.dimension, p->y_unit.dimension, 7);
    ps_result result = PS_OK;
    /* Bucket extrema, in sample order. Endpoints have their own slots. */
    uint64_t bucket_size = xi.count / 1023 + (xi.count % 1023 != 0), bucket = 0, lo_index = 0,
             hi_index = 0;
    double low = 0, high = 0, low_x = 0, high_x = 0;
    bool started = false;
    for (uint64_t at = 0; at < xi.count && result == PS_OK;) {
        double x[PS_SERIES_BLOCK_SIZE], y[PS_SERIES_BLOCK_SIZE];
        size_t nx = 0, ny = 0;
        result = ps_series_read(ctx, xs, at, x, PS_SERIES_BLOCK_SIZE, &nx);
        if (result == PS_OK)
            result = ps_series_read(ctx, ys, at, y, nx, &ny);
        if (result != PS_OK || !nx || nx != ny) {
            if (result == PS_OK)
                result = PS_CORRUPT;
            break;
        }
        for (size_t j = 0; j < nx; j++) {
            uint64_t i = at + j;
            result = ps_convert(x[j], xu, target_x, &x[j]);
            if (result == PS_OK)
                result = ps_convert(y[j], yu, target_y, &y[j]);
            if (result != PS_OK) {
                break;
            }
            if (xi.count <= PS_REPORT_MAX_POINTS)
                point(c, x[j], y[j]);
            else if (kind == PS_PLOT_SCATTER) {
                uint64_t target = (xi.count - 1) / (PS_REPORT_MAX_POINTS - 1) * c->count +
                                  ((xi.count - 1) % (PS_REPORT_MAX_POINTS - 1) * c->count) /
                                      (PS_REPORT_MAX_POINTS - 1);
                if (i == target && c->count < PS_REPORT_MAX_POINTS)
                    point(c, x[j], y[j]);
            } else if (!i || i == xi.count - 1) {
                if (i && started) {
                    if (lo_index <= hi_index) {
                        point(c, low_x, low);
                        if (hi_index != lo_index)
                            point(c, high_x, high);
                    } else {
                        point(c, high_x, high);
                        point(c, low_x, low);
                    }
                    started = false;
                }
                point(c, x[j], y[j]);
            } else {
                uint64_t next = i / bucket_size;
                if (started && next != bucket) {
                    if (lo_index <= hi_index) {
                        point(c, low_x, low);
                        if (hi_index != lo_index)
                            point(c, high_x, high);
                    } else {
                        point(c, high_x, high);
                        point(c, low_x, low);
                    }
                    started = false;
                }
                if (!started) {
                    bucket = next;
                    low = high = y[j];
                    low_x = high_x = x[j];
                    lo_index = hi_index = i;
                    started = true;
                }
                if (y[j] < low) {
                    low = y[j];
                    low_x = x[j];
                    lo_index = i;
                }
                if (y[j] > high) {
                    high = y[j];
                    high_x = x[j];
                    hi_index = i;
                }
            }
        }
        at += nx;
    }
    if (result == PS_OK)
        result = ps_report_add_curve(r, h, c);
    ps_memory_free(r->allocator, c, sizeof *c);
    return result;
}
double ps_report_axis_fraction(double v, double minimum, double maximum) {
    if (minimum == maximum)
        return .5;
    double range = maximum - minimum;
    return isfinite(range) ? (v - minimum) / range
                           : (v / 2 - minimum / 2) / (maximum / 2 - minimum / 2);
}
ps_result ps_report_add_histogram(ps_report *r, ps_analysis_context *ctx, ps_series s,
                                  const char *title, const char *x_label, uint32_t bins,
                                  ps_plot_handle *out) {
    if (!r || !out || !title || !x_label || !ps_text_valid(x_label, 96, false) ||
        !ps_text_valid(title, 192, false) || !title[0] || !bins || bins > PS_REPORT_MAX_BINS)
        return PS_INVALID;
    if (r->plots == PS_REPORT_MAX_PLOTS)
        return PS_LIMIT;
    ps_statistics stats = {0};
    ps_series_info si;
    ps_result result = ps_series_describe(ctx, s, &si);
    if (result == PS_OK)
        result = ps_series_statistics(ctx, s, &stats);
    if (result != PS_OK)
        return result;
    if (!stats.count || stats.count > UINT64_C(9007199254740992))
        return PS_LIMIT;
    ps_curve_data *c = report_memory(r->allocator, sizeof *c);
    if (!c)
        return PS_MEMORY;
    c->kind = PS_PLOT_HISTOGRAM;
    c->source_count = stats.count;
    strcpy(c->label, "Häufigkeit");
    if (stats.min == stats.max)
        bins = 1;
    c->count = bins;
    /* A histogram may have more bins than samples; source_count counts samples. */
    c->bar_width = stats.min == stats.max ? fmax(1, fabs(stats.min) * .05)
                                          : (stats.max / bins - stats.min / bins);
    for (uint32_t i = 0; i < bins; i++) {
        double f = ((double)i + .5) / bins;
        c->x[i] = stats.min == stats.max ? stats.min : (1 - f) * stats.min + f * stats.max;
    }
    for (uint64_t at = 0; at < si.count && result == PS_OK;) {
        double values[PS_SERIES_BLOCK_SIZE];uint8_t valid[PS_SERIES_BLOCK_SIZE];
        size_t count = 0;
        result = ps_series_read_masked(ctx,s,at,values,valid,PS_SERIES_BLOCK_SIZE,&count);
        if (result != PS_OK || !count) {
            if (result == PS_OK)
                result = PS_CORRUPT;
            break;
        }
        for (size_t j = 0; j < count; j++) {
            if(!valid[j])continue;
            double f = ps_report_axis_fraction(values[j], stats.min, stats.max);
            uint32_t bin = f >= 1 ? bins - 1 : (uint32_t)(f * bins);
            c->y[bin]++;
        }
        at += count;
    }
    ps_plot_info info = {0};
    strcpy(info.title, title);
    strcpy(info.x_label, x_label);
    strcpy(info.y_label, "Anzahl");
    memcpy(info.x_unit.dimension, si.dimension, 7);
    info.x_unit.scale = si.scale;
    strcpy(info.x_unit.symbol, si.symbol);
    ps_report_unit_from(PS_ONE, &info.y_unit);
    /* Publish only once all computation and validation succeeds. */
    if (result == PS_OK && (!plot_valid(&info) || !curve_valid(c)))
        result = PS_INVALID;
    if (result == PS_OK) {
        plot_data *p = &r->plot[r->plots];
        p->info = info;
        p->curve[0] = c;
        p->info.curves = 1;
        *out = (ps_plot_handle){r, r->plots++};
    } else
        ps_memory_free(r->allocator, c, sizeof *c);
    return result;
}
ps_result ps_report_plot_bounds(const ps_report *r, uint32_t plot, double bounds[4]) {
    if (!r || !bounds || plot >= r->plots || !r->plot[plot].info.curves)
        return PS_INVALID;
    double b[] = {DBL_MAX, -DBL_MAX, DBL_MAX, -DBL_MAX};
    const plot_data *p = &r->plot[plot];
    for (uint32_t j = 0; j < p->info.curves; j++) {
        const ps_curve_data *c = p->curve[j];
        bool any=false;
        for (uint32_t i = 0; i < c->count; i++) {
            if(p->mask[j] && !(p->mask[j][i]&1))continue;
            any=true;
            double half = c->kind == PS_PLOT_HISTOGRAM ? c->bar_width / 2 : 0;
            b[0] = fmin(b[0], c->x[i] - half);
            b[1] = fmax(b[1], c->x[i] + half);
            b[2] = fmin(b[2], c->y[i]);
            b[3] = fmax(b[3], c->y[i]);
        }
        if (any && c->kind == PS_PLOT_HISTOGRAM)
            b[2] = fmin(0, b[2]);
    }
    if(b[0]>b[1] || b[2]>b[3])return PS_INVALID;
    for (unsigned i = 0; i < 4; i += 2) {
        if (b[i] == b[i + 1]) {
            double delta = fmax(1, fabs(b[i]) * .05);
            if (isfinite(b[i] - delta))
                b[i] -= delta;
            if (isfinite(b[i + 1] + delta))
                b[i + 1] += delta;
        }
    }
    memcpy(bounds, b, sizeof b);
    return PS_OK;
}

#define FILE_LIMIT (8u * 1024u * 1024u)
typedef struct {
    unsigned char *data;
    size_t size, at;
    bool read, ok;
    uint32_t version;
} codec;
static void bytes(codec *c, void *data, size_t size) {
    if (!c->ok || size > c->size - c->at) {
        c->ok = false;
        return;
    }
    if (c->read)
        memcpy(data, c->data + c->at, size);
    else
        memcpy(c->data + c->at, data, size);
    c->at += size;
}
static void integer(codec *c, uint32_t *value) {
    unsigned char b[4] = {0};
    if (!c->read)
        ps_put_u32(b, *value);
    bytes(c, b, sizeof b);
    if (c->read && c->ok)
        *value = ps_get_u32(b);
}
static void number(codec *c, double *value) {
    unsigned char b[8] = {0};
    if (!c->read)
        ps_put_f64(b, *value);
    bytes(c, b, sizeof b);
    if (c->read && c->ok)
        *value = ps_get_f64(b);
}
static void string(codec *c, char *s, size_t capacity) {
    uint32_t n = c->read ? 0 : (uint32_t)strlen(s);
    integer(c, &n);
    if (!c->ok || n >= capacity) {
        c->ok = false;
        return;
    }
    bytes(c, s, n);
    if (c->read) {
        if (memchr(s, 0, n))
            c->ok = false;
        s[n] = 0;
    }
}
static void unit_codec(codec *c, ps_report_unit *u) {
    bytes(c, u->dimension, 7);
    number(c, &u->scale);
    string(c, u->symbol, sizeof u->symbol);
    if (c->read && !unit_valid(u))
        c->ok = false;
}
static ps_result report_codec(codec *c, ps_report *r) {
    string(c, r->title, sizeof r->title);
    string(c, r->provenance, sizeof r->provenance);
    uint32_t plots = r->plots, tables = r->tables;
    integer(c, &plots);
    integer(c, &tables);
    if (!c->ok || plots > PS_REPORT_MAX_PLOTS || tables > PS_REPORT_MAX_TABLES || !r->title[0] ||
        !ps_text_valid(r->title, sizeof r->title, false) ||
        !ps_text_valid(r->provenance, sizeof r->provenance, true))
        return PS_CORRUPT;
    for (uint32_t i = 0; i < plots && c->ok; i++) {
        plot_data *p = &r->plot[i];
        ps_plot_info *v = &p->info;
        if (c->read)
            r->plots++;
        string(c, v->title, sizeof v->title);
        string(c, v->x_label, sizeof v->x_label);
        string(c, v->y_label, sizeof v->y_label);
        unit_codec(c, &v->x_unit);
        unit_codec(c, &v->y_unit);
        uint32_t curves = v->curves;
        integer(c, &curves);
        if (!plot_valid(v) || curves > PS_REPORT_MAX_CURVES)
            return PS_CORRUPT;
        for (uint32_t j = 0; j < curves && c->ok; j++) {
            if (c->read) {
                p->curve[j] = report_memory(r->allocator, sizeof *p->curve[j]);
                if (!p->curve[j])
                    return PS_MEMORY;
                v->curves++;
            }
            ps_curve_data *q = p->curve[j];
            string(c, q->label, sizeof q->label);
            uint32_t kind = (uint32_t)q->kind, lo = (uint32_t)q->source_count,
                     hi = (uint32_t)(q->source_count >> 32);
            integer(c, &kind);
            integer(c, &q->count);
            integer(c, &lo);
            integer(c, &hi);
            if (c->read) {
                q->source_count = lo | ((uint64_t)hi << 32);
                q->kind = (ps_plot_kind)kind;
            }
            number(c, &q->bar_width);
            if (q->count > PS_REPORT_MAX_POINTS)
                return PS_CORRUPT;
            for (uint32_t k = 0; k < q->count && c->ok; k++) {
                number(c, &q->x[k]);
                number(c, &q->y[k]);
            }
            if(c->version==2) {
                uint32_t length=p->mask[j]?q->count:0;integer(c,&length);
                if(!c->ok || (length && length!=q->count))return PS_CORRUPT;
                if(c->read && length) {
                    p->mask[j]=report_memory(r->allocator,PS_REPORT_MAX_POINTS);if(!p->mask[j])return PS_MEMORY;
                }
                if(length) {
                    bytes(c,p->mask[j],length);
                    for(uint32_t k=0;k<length;k++)if(p->mask[j][k]!=0 && p->mask[j][k]!=1 && p->mask[j][k]!=3)return PS_CORRUPT;
                }
            }
            if (!curve_valid(q))
                return PS_CORRUPT;
            if (q->kind == PS_PLOT_HISTOGRAM) {
                const int8_t zero[7] = {0};
                if (memcmp(v->y_unit.dimension, zero, 7) || v->y_unit.scale != 1)
                    return PS_CORRUPT;
            }
        }
    }
    for (uint32_t i = 0; i < tables && c->ok; i++) {
        if (c->read) {
            r->table[i] = report_memory(r->allocator, sizeof *r->table[i]);
            if (!r->table[i])
                return PS_MEMORY;
            r->tables++;
        }
        table_data *t = r->table[i];
        ps_table_info *v = &t->info;
        string(c, v->title, sizeof v->title);
        integer(c, &v->columns);
        integer(c, &v->rows);
        if (v->columns > PS_REPORT_MAX_COLUMNS || v->rows > PS_REPORT_MAX_ROWS)
            return PS_CORRUPT;
        for (uint32_t j = 0; j < v->columns && c->ok; j++) {
            string(c, v->column[j].label, sizeof v->column[j].label);
            unit_codec(c, &v->column[j].unit);
        }
        if (!table_valid(v))
            return PS_CORRUPT;
        for (uint32_t j = 0; j < v->rows && c->ok; j++) {
            string(c, t->row[j].label, sizeof t->row[j].label);
            if (!ps_text_valid(t->row[j].label, sizeof t->row[j].label, false))
                return PS_CORRUPT;
            for (uint32_t k = 0; k < v->columns && c->ok; k++) {
                number(c, &t->row[j].values[k]);
                if (!isfinite(t->row[j].values[k]))
                    return PS_CORRUPT;
            }
        }
    }
    return c->ok ? PS_OK : PS_CORRUPT;
}
ps_result ps_report_save(const ps_report *r, const char *path) {
    if (!r || !path)
        return PS_INVALID;
    unsigned char *buffer = report_buffer(r->allocator, FILE_LIMIT);
    if (!buffer)
        return PS_MEMORY;
    codec c = {buffer,FILE_LIMIT,0,false,true,1};
    for(uint32_t i=0;i<r->plots;i++)for(uint32_t j=0;j<r->plot[i].info.curves;j++)if(r->plot[i].mask[j])c.version=2;
    /* Encoding does not mutate values. */
    ps_result result = report_codec(&c, (ps_report *)r);
    if (result == PS_OK) {
        FILE *f = fopen(path, "wbx");
        if (!f)
            result = PS_IO;
        else {
            unsigned char header[20] = {'P', 'S', 'R', 'P', 'T', '1', '7', '\n'};
            ps_put_u32(header + 8, c.version);
            ps_put_u32(header + 12, (uint32_t)c.at);
            ps_put_u32(header + 16, ps_crc32(buffer, c.at));
            if (fwrite(header, 1, sizeof header, f) != sizeof header ||
                fwrite(buffer, 1, c.at, f) != c.at)
                result = PS_IO;
            if (fclose(f))
                result = PS_IO;
        }
    }
    ps_memory_free(r->allocator, buffer, FILE_LIMIT);
    return result;
}
ps_result ps_report_load(const char *path, ps_report **out) {
    return ps_report_load_with_allocator(path, ps_allocator_default(), out);
}
ps_result ps_report_load_with_allocator(const char *path, ps_allocator allocator, ps_report **out) {
    if (!path || !out || !ps_allocator_valid(allocator))
        return PS_INVALID;
    FILE *f = fopen(path, "rb");
    if (!f)
        return PS_IO;
    unsigned char header[20];
    ps_result result = PS_CORRUPT;
    unsigned char *buffer = NULL;
    ps_report *r = NULL;
    uint32_t n = 0;
    if (fread(header, 1, sizeof header, f) != sizeof header || memcmp(header, "PSRPT17\n", 8))
        goto done;
    if (ps_get_u32(header + 8) != 1 && ps_get_u32(header+8)!=2) {
        result = PS_VERSION;
        goto done;
    }
    n = ps_get_u32(header + 12);
    if (n > FILE_LIMIT) {
        result = PS_LIMIT;
        goto done;
    }
    buffer = report_buffer(allocator, n ? n : 1);
    r = report_memory(allocator, sizeof *r);
    if (r)
        r->allocator = allocator;
    if (!buffer || !r) {
        result = PS_MEMORY;
        goto done;
    }
    if (fread(buffer, 1, n, f) != n || fgetc(f) != EOF || ferror(f) ||
        ps_crc32(buffer, n) != ps_get_u32(header + 16))
        goto done;
    codec c = {buffer,n,0,true,true,ps_get_u32(header+8)};
    result = report_codec(&c, r);
    if (result == PS_OK && c.at != c.size)
        result = PS_CORRUPT;
done:
    if (ferror(f))
        result = PS_IO;
    fclose(f);
    ps_memory_free(allocator, buffer, n ? n : 1);
    if (result == PS_OK)
        *out = r;
    else
        ps_report_destroy(r);
    return result;
}
