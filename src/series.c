#include "physim/series.h"
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#endif

typedef struct {
    FILE *file;
    uint32_t generation;
    uint64_t bytes;
    ps_dataset_info info;
} dataset_slot;
typedef struct {
    bool live;
    uint32_t generation, dataset, dataset_generation, column;
    uint64_t first, bytes, alignment;
    FILE *file; /* NULL for source columns; otherwise own contiguous doubles. */
    ps_series_info info;
} series_slot;
struct ps_analysis_context {
    ps_allocator allocator;
    char prefix[4096];
    uint64_t limit, bytes, serial, alignment_serial;
    dataset_slot datasets[PS_ANALYSIS_MAX_DATASETS];
    series_slot series[PS_ANALYSIS_MAX_SERIES];
};
static FILE *scratch(ps_analysis_context *ctx) {
    char path[4200];
    for (unsigned attempt = 0; attempt < 16; attempt++) {
        if (ctx->serial == UINT64_MAX)
            return NULL;
        snprintf(path, sizeof path, "%s-work-%p-%llu.tmp", ctx->prefix, (void *)ctx,
                 (unsigned long long)++ctx->serial);
#ifdef _WIN32
        wchar_t wide[4200];
        if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, wide, 4200))
            return NULL;
        HANDLE handle =
            CreateFileW(wide, GENERIC_READ | GENERIC_WRITE | DELETE, 0, NULL, CREATE_NEW,
                        FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, NULL);
        if (handle == INVALID_HANDLE_VALUE) {
            if (GetLastError() == ERROR_FILE_EXISTS || GetLastError() == ERROR_ALREADY_EXISTS)
                continue;
            return NULL;
        }
        int fd = _open_osfhandle((intptr_t)handle, _O_BINARY | _O_RDWR);
        if (fd < 0) {
            CloseHandle(handle);
            return NULL;
        }
        FILE *file = _fdopen(fd, "w+b");
        if (!file)
            _close(fd);
#else
        int fd = open(path, O_RDWR | O_CREAT | O_EXCL, 0600);
        if (fd < 0)
            return NULL;
        if (unlink(path)) {
            close(fd);
            return NULL;
        }
        FILE *file = fdopen(fd, "w+b");
        if (!file)
            close(fd);
#endif
        return file;
    }
    return NULL;
}
static bool seek(FILE *f, uint64_t offset) {
    if (offset > INT64_MAX)
        return false;
#ifdef _WIN32
    return _fseeki64(f, (int64_t)offset, SEEK_SET) == 0;
#else
    off_t pos = (off_t)offset;
    return pos >= 0 && (uint64_t)pos == offset && fseeko(f, pos, SEEK_SET) == 0;
#endif
}
static dataset_slot *dataset_get(ps_analysis_context *c, ps_dataset h) {
    if (!c || h.owner != c || !h.generation || h.slot >= PS_ANALYSIS_MAX_DATASETS)
        return NULL;
    dataset_slot *d = &c->datasets[h.slot];
    return d->file && d->generation == h.generation ? d : NULL;
}
static series_slot *series_get(ps_analysis_context *c, ps_series h) {
    if (!c || h.owner != c || !h.generation || h.slot >= PS_ANALYSIS_MAX_SERIES)
        return NULL;
    series_slot *s = &c->series[h.slot];
    return s->live && s->generation == h.generation ? s : NULL;
}
static int free_series(ps_analysis_context *c) {
    for (unsigned i = 0; i < PS_ANALYSIS_MAX_SERIES; i++)
        if (!c->series[i].live && c->series[i].generation != UINT32_MAX)
            return (int)i;
    return -1;
}
static bool aligned(const series_slot *a, const series_slot *b) {
    return a->dataset == b->dataset && a->dataset_generation == b->dataset_generation &&
           a->alignment == b->alignment && a->first == b->first && a->info.count == b->info.count;
}
ps_result ps_series_aligned(ps_analysis_context *c, ps_series left, ps_series right) {
    series_slot *a = series_get(c, left), *b = series_get(c, right);
    return a && b && aligned(a, b) ? PS_OK : PS_INVALID;
}
static ps_unit unit_of(const series_slot *s) {
    ps_unit u = {{0}, s->info.scale, NULL};
    memcpy(u.dimension, s->info.dimension, 7);
    return u;
}
static void set_unit(series_slot *s, ps_unit unit) {
    memcpy(s->info.dimension, unit.dimension, 7);
    s->info.scale = unit.scale;
    (void)ps_unit_format_dimension(unit, s->info.symbol, sizeof s->info.symbol);
}
ps_result ps_analysis_create(const char *prefix, uint64_t limit, ps_analysis_context **out) {
    return ps_analysis_create_with_allocator(prefix, limit, ps_allocator_default(), out);
}
ps_result ps_analysis_create_with_allocator(const char *prefix, uint64_t limit,
                                            ps_allocator allocator, ps_analysis_context **out) {
    if (!prefix || !prefix[0] || strlen(prefix) >= 4096 || !out || !ps_allocator_valid(allocator))
        return PS_INVALID;
    void *storage;
    ps_result result = ps_memory_zero(allocator, 1, sizeof(ps_analysis_context), &storage);
    if (result != PS_OK)
        return result;
    ps_analysis_context *c = storage;
    c->allocator = allocator;
    strcpy(c->prefix, prefix);
    c->limit = limit ? limit : UINT64_C(1073741824);
    *out = c;
    return PS_OK;
}
static void release_slot(ps_analysis_context *c, series_slot *s) {
    if (s->file)
        fclose(s->file);
    c->bytes -= s->bytes;
    s->file = NULL;
    s->bytes = 0;
    s->live = false;
}
void ps_analysis_destroy(ps_analysis_context *c) {
    if (!c)
        return;
    for (unsigned i = 0; i < PS_ANALYSIS_MAX_SERIES; i++)
        if (c->series[i].live)
            release_slot(c, &c->series[i]);
    for (unsigned i = 0; i < PS_ANALYSIS_MAX_DATASETS; i++)
        if (c->datasets[i].file)
            fclose(c->datasets[i].file);
    ps_memory_free(c->allocator, c, sizeof *c);
}
uint64_t ps_analysis_scratch_bytes(const ps_analysis_context *c) { return c ? c->bytes : 0; }
ps_result ps_analysis_open_run(ps_analysis_context *c, const char *path, ps_dataset *out) {
    if (!c || !path || !out)
        return PS_INVALID;
    unsigned slot = 0;
    while (slot < PS_ANALYSIS_MAX_DATASETS &&
           (c->datasets[slot].file || c->datasets[slot].generation == UINT32_MAX))
        slot++;
    if (slot == PS_ANALYSIS_MAX_DATASETS)
        return PS_LIMIT;
    ps_run_reader reader;
    ps_result r = ps_run_open(&reader, path);
    if (r != PS_OK)
        return r;
    for (uint32_t i = 0; i < reader.channels; i++) {
        if (!reader.schema[i].name[0] || !strcmp(reader.schema[i].name, "time")) {
            r = PS_INVALID;
            goto close_reader;
        }
        for (uint32_t j = 0; j < i; j++)
            if (!strcmp(reader.schema[i].name, reader.schema[j].name)) {
                r = PS_INVALID;
                goto close_reader;
            }
    }
    FILE *file = scratch(c);
    if (!file) {
        r = PS_IO;
        goto close_reader;
    }
    uint64_t bytes = 0, count = 0;
    double previous = 0;
    double record[PS_MAX_CHANNELS + 1];
    size_t stride = reader.channels + 1;
    while ((r = ps_run_next(&reader, &record[0], &record[1])) == PS_OK) {
        if (count && record[0] <= previous) {
            r = PS_CORRUPT;
            break;
        }
        if (c->limit - c->bytes - bytes < stride * sizeof(double)) {
            r = PS_LIMIT;
            break;
        }
        if (fwrite(record, sizeof(double), stride, file) != stride) {
            r = PS_IO;
            break;
        }
        bytes += stride * sizeof(double);
        count++;
        previous = record[0];
    }
    if (r == PS_EOF || r == PS_RECOVERED) {
        if (fflush(file))
            r = PS_IO;
        else {
            dataset_slot *d = &c->datasets[slot];
            d->generation++;
            d->file = file;
            d->bytes = bytes;
            d->info.samples = count;
            d->info.channel_count = reader.channels;
            d->info.recovered = r == PS_RECOVERED;
            memcpy(d->info.metadata, reader.metadata, sizeof d->info.metadata);
            memcpy(d->info.channels, reader.schema, sizeof d->info.channels);
            c->bytes += bytes;
            *out = (ps_dataset){c, slot, d->generation};
            ps_run_reader_close(&reader);
            return r == PS_EOF ? PS_OK : PS_RECOVERED;
        }
    }
    fclose(file);
close_reader:
    ps_run_reader_close(&reader);
    return r;
}
ps_result ps_dataset_describe(ps_analysis_context *c, ps_dataset h, ps_dataset_info *out) {
    dataset_slot *d = dataset_get(c, h);
    if (!d || !out)
        return PS_INVALID;
    *out = d->info;
    return PS_OK;
}
ps_result ps_dataset_close(ps_analysis_context *c, ps_dataset h) {
    dataset_slot *d = dataset_get(c, h);
    if (!d)
        return PS_INVALID;
    for (unsigned i = 0; i < PS_ANALYSIS_MAX_SERIES; i++)
        if (c->series[i].live && c->series[i].dataset == h.slot)
            release_slot(c, &c->series[i]);
    fclose(d->file);
    d->file = NULL;
    c->bytes -= d->bytes;
    d->bytes = 0;
    return PS_OK;
}
ps_result ps_dataset_series(ps_analysis_context *c, ps_dataset h, const char *name,
                            ps_series *out) {
    dataset_slot *d = dataset_get(c, h);
    if (!d || !name || !out)
        return PS_INVALID;
    uint32_t col = UINT32_MAX;
    if (!strcmp(name, "time"))
        col = 0;
    else
        for (uint32_t i = 0; i < d->info.channel_count; i++)
            if (!strcmp(name, d->info.channels[i].name)) {
                col = i + 1;
                break;
            }
    if (col == UINT32_MAX)
        return PS_INVALID;
    int slot = free_series(c);
    if (slot < 0)
        return PS_LIMIT;
    series_slot *s = &c->series[slot];
    uint32_t generation = s->generation + 1;
    memset(s, 0, sizeof *s);
    s->generation = generation;
    s->live = true;
    s->dataset = h.slot;
    s->dataset_generation = h.generation;
    s->column = col;
    s->info.count = d->info.samples;
    s->info.scale = 1;
    snprintf(s->info.name, sizeof s->info.name, "%s", name);
    if (!col) {
        memcpy(s->info.dimension, PS_SECOND.dimension, 7);
        strcpy(s->info.symbol, "s");
    } else {
        memcpy(s->info.dimension, d->info.channels[col - 1].dimension, 7);
        snprintf(s->info.symbol, sizeof s->info.symbol, "%s", d->info.channels[col - 1].unit);
    }
    *out = (ps_series){c, (uint32_t)slot, generation};
    return PS_OK;
}
ps_result ps_series_describe(ps_analysis_context *c, ps_series h, ps_series_info *out) {
    series_slot *s = series_get(c, h);
    if (!s || !out)
        return PS_INVALID;
    *out = s->info;
    return PS_OK;
}
ps_result ps_series_release(ps_analysis_context *c, ps_series h) {
    series_slot *s = series_get(c, h);
    if (!s)
        return PS_INVALID;
    release_slot(c, s);
    return PS_OK;
}
static ps_result read_values(ps_analysis_context *c, const series_slot *s, uint64_t at,
                             size_t count, double *out) {
    if (s->file) {
        if (at > UINT64_MAX / 8 || !seek(s->file, at * 8) ||
            fread(out, sizeof(double), count, s->file) != count)
            return PS_IO;
    } else {
        dataset_slot *d = &c->datasets[s->dataset];
        size_t stride = d->info.channel_count + 1;
        if (at > UINT64_MAX / (stride * 8) || !seek(d->file, at * stride * 8))
            return PS_IO;
        double block[PS_SERIES_BLOCK_SIZE * (PS_MAX_CHANNELS + 1)];
        size_t done = 0;
        while (done < count) {
            size_t n = count - done;
            if (n > PS_SERIES_BLOCK_SIZE)
                n = PS_SERIES_BLOCK_SIZE;
            if (fread(block, sizeof(double), n * stride, d->file) != n * stride)
                return PS_IO;
            for (size_t i = 0; i < n; i++)
                out[done + i] = block[i * stride + s->column];
            done += n;
        }
    }
    return PS_OK;
}
ps_result ps_series_read(ps_analysis_context *c, ps_series h, uint64_t at, double *out,
                         size_t capacity, size_t *got) {
    series_slot *s = series_get(c, h);
    if (!s || !got || (!out && capacity) || at > s->info.count ||
        capacity > SIZE_MAX / sizeof(double))
        return PS_INVALID;
    size_t n = (size_t)((s->info.count - at) < capacity ? (s->info.count - at) : capacity);
    ps_result r = n ? read_values(c, s, at, n, out) : PS_OK;
    if (r == PS_OK)
        *got = n;
    return r;
}
/* Reserve only after validation; commit only after all I/O succeeds. */
static ps_result begin_derived(ps_analysis_context *c, const series_slot *input, uint64_t count,
                               series_slot *draft, int *slot) {
    *slot = free_series(c);
    if (*slot < 0)
        return PS_LIMIT;
    if (count > UINT64_MAX / 8 || count * 8 > c->limit - c->bytes)
        return PS_LIMIT;
    *draft = *input;
    draft->file = scratch(c);
    if (!draft->file)
        return PS_IO;
    draft->bytes = count * 8;
    draft->info.count = count;
    draft->generation = c->series[*slot].generation + 1;
    return PS_OK;
}
static ps_result finish_derived(ps_analysis_context *c, series_slot *draft, int slot, ps_result r,
                                ps_series *out) {
    if (r == PS_OK && fflush(draft->file))
        r = PS_IO;
    if (r != PS_OK) {
        fclose(draft->file);
        return r;
    }
    c->series[slot] = *draft;
    c->bytes += draft->bytes;
    *out = (ps_series){c, (uint32_t)slot, draft->generation};
    return PS_OK;
}
static ps_result write_values(FILE *file, const double *values, size_t count) {
    for (size_t i = 0; i < count; i++)
        if (!isfinite(values[i]))
            return PS_NUMERIC;
    return fwrite(values, sizeof(double), count, file) == count ? PS_OK : PS_IO;
}
static ps_result import_values(ps_analysis_context *c, const series_slot *anchor,
                               const double *values, size_t count, ps_unit unit,
                               const char *name, ps_series *out) {
    if (!c || (!values && count) || !name || !*name || strlen(name) >= 64 || !out ||
        !ps_unit_valid(unit) || count > SIZE_MAX / sizeof(double) ||
        (anchor && anchor->info.count != count))
        return PS_INVALID;
    if ((uint64_t)count > (c->limit - c->bytes) / sizeof(double) || free_series(c) < 0)
        return PS_LIMIT;
    for (size_t i = 0; i < count; i++)
        if (!isfinite(values[i]))
            return PS_NUMERIC;
    if (!anchor && c->alignment_serial == UINT64_MAX)
        return PS_LIMIT;
    series_slot root = {0};
    if (!anchor) {
        root.dataset = UINT32_MAX;
        root.alignment = c->alignment_serial + 1;
        anchor = &root;
    }
    series_slot draft;
    int slot;
    ps_result r = begin_derived(c, anchor, count, &draft, &slot);
    if (r != PS_OK)
        return r;
    draft.live = true;
    set_unit(&draft, unit);
    snprintf(draft.info.name, sizeof draft.info.name, "%s", name);
    if (count)
        r = write_values(draft.file, values, count);
    r = finish_derived(c, &draft, slot, r, out);
    if (r == PS_OK && anchor == &root)
        c->alignment_serial++;
    return r;
}
ps_result ps_series_from_values(ps_analysis_context *c, const double *values, size_t count,
                                ps_unit unit, const char *name, ps_series *out) {
    return import_values(c, NULL, values, count, unit, name, out);
}
ps_result ps_series_aligned_values(ps_analysis_context *c, ps_series anchor,
                                   const double *values, size_t count, ps_unit unit,
                                   const char *name, ps_series *out) {
    series_slot *source = series_get(c, anchor);
    return source ? import_values(c, source, values, count, unit, name, out) : PS_INVALID;
}
ps_result ps_series_slice(ps_analysis_context *c, ps_series h, uint64_t first, uint64_t count,
                          ps_series *out) {
    series_slot *s = series_get(c, h);
    if (!s || !out || first > s->info.count || count > s->info.count - first)
        return PS_INVALID;
    series_slot draft;
    int slot;
    ps_result r = begin_derived(c, s, count, &draft, &slot);
    if (r != PS_OK)
        return r;
    draft.first += first;
    double block[PS_SERIES_BLOCK_SIZE];
    for (uint64_t at = 0; at < count;) {
        size_t n = (size_t)(count - at > PS_SERIES_BLOCK_SIZE ? PS_SERIES_BLOCK_SIZE : count - at);
        r = read_values(c, s, first + at, n, block);
        if (r != PS_OK)
            break;
        r = write_values(draft.file, block, n);
        if (r != PS_OK)
            break;
        at += n;
    }
    return finish_derived(c, &draft, slot, r, out);
}
ps_result ps_series_select(ps_analysis_context *c, const ps_series *columns, size_t count,
                           ps_series selector, double accepted, ps_series *out) {
    series_slot *mask = series_get(c, selector), *inputs[32];
    if (!mask || !columns || !out || !count || count > 32 || !isfinite(accepted))
        return PS_INVALID;
    for (unsigned i = 0; i < 7; i++)
        if (mask->info.dimension[i])
            return PS_INVALID;
    for (size_t i = 0; i < count; i++) {
        inputs[i] = series_get(c, columns[i]);
        if (!inputs[i] || !aligned(inputs[i], mask))
            return PS_INVALID;
    }
    int slots[32];
    size_t available = 0;
    for (unsigned i = 0; i < PS_ANALYSIS_MAX_SERIES && available < count; i++)
        if (!c->series[i].live && c->series[i].generation != UINT32_MAX)
            slots[available++] = (int)i;
    if (available != count || c->alignment_serial == UINT64_MAX)
        return PS_LIMIT;
    double flags[PS_SERIES_BLOCK_SIZE], values[PS_SERIES_BLOCK_SIZE];
    uint64_t selected = 0;
    for (uint64_t at = 0; at < mask->info.count;) {
        size_t n = (size_t)(mask->info.count - at > PS_SERIES_BLOCK_SIZE ? PS_SERIES_BLOCK_SIZE
                                                                         : mask->info.count - at);
        ps_result r = read_values(c, mask, at, n, flags);
        if (r != PS_OK)
            return r;
        for (size_t j = 0; j < n; j++)
            selected += flags[j] == accepted;
        at += n;
    }
    if (selected > (c->limit - c->bytes) / (8 * count))
        return PS_LIMIT;
    series_slot drafts[32];
    size_t opened = 0;
    ps_result result = PS_OK;
    for (size_t i = 0; i < count; i++) {
        drafts[i] = *inputs[i];
        drafts[i].file = scratch(c);
        if (!drafts[i].file) {
            result = PS_IO;
            goto cleanup;
        }
        opened++;
        drafts[i].generation = c->series[slots[i]].generation + 1;
        drafts[i].first = 0;
        drafts[i].alignment = c->alignment_serial + 1;
        drafts[i].info.count = selected;
        drafts[i].bytes = selected * 8;
    }
    for (uint64_t at = 0; at < mask->info.count;) {
        size_t n = (size_t)(mask->info.count - at > PS_SERIES_BLOCK_SIZE ? PS_SERIES_BLOCK_SIZE
                                                                         : mask->info.count - at);
        result = read_values(c, mask, at, n, flags);
        if (result != PS_OK)
            goto cleanup;
        for (size_t i = 0; i < count; i++) {
            result = read_values(c, inputs[i], at, n, values);
            if (result != PS_OK)
                goto cleanup;
            size_t kept = 0;
            for (size_t j = 0; j < n; j++)
                if (flags[j] == accepted)
                    values[kept++] = values[j];
            result = write_values(drafts[i].file, values, kept);
            if (result != PS_OK)
                goto cleanup;
        }
        at += n;
    }
    for (size_t i = 0; i < count; i++)
        if (fflush(drafts[i].file)) {
            result = PS_IO;
            goto cleanup;
        }
    c->alignment_serial++;
    for (size_t i = 0; i < count; i++) {
        c->series[slots[i]] = drafts[i];
        c->bytes += drafts[i].bytes;
        out[i] = (ps_series){c, (uint32_t)slots[i], drafts[i].generation};
    }
    return PS_OK;
cleanup:
    for (size_t i = 0; i < opened; i++)
        fclose(drafts[i].file);
    return result;
}
ps_result ps_series_affine(ps_analysis_context *c, ps_series h, double factor, ps_quantity offset,
                           ps_series *out) {
    series_slot *s = series_get(c, h);
    if (!s || !out || !isfinite(factor))
        return PS_INVALID;
    double shift;
    ps_result r = ps_convert(offset.value, offset.unit, unit_of(s), &shift);
    if (r != PS_OK)
        return r;
    series_slot draft;
    int slot;
    r = begin_derived(c, s, s->info.count, &draft, &slot);
    if (r != PS_OK)
        return r;
    snprintf(draft.info.name, sizeof draft.info.name, "affine(%.*s)", 48, s->info.name);
    double block[PS_SERIES_BLOCK_SIZE];
    for (uint64_t at = 0; at < s->info.count;) {
        size_t n = (size_t)(s->info.count - at > PS_SERIES_BLOCK_SIZE ? PS_SERIES_BLOCK_SIZE
                                                                      : s->info.count - at);
        r = read_values(c, s, at, n, block);
        if (r != PS_OK)
            break;
        for (size_t i = 0; i < n; i++)
            block[i] = factor * block[i] + shift;
        r = write_values(draft.file, block, n);
        if (r != PS_OK)
            break;
        at += n;
    }
    return finish_derived(c, &draft, slot, r, out);
}
ps_result ps_series_combine(ps_analysis_context *c, ps_series_operator op, ps_series ha,
                            ps_series hb, ps_series *out) {
    series_slot *a = series_get(c, ha), *b = series_get(c, hb);
    if (!a || !b || !out || !aligned(a, b) || op < PS_SERIES_ADD || op > PS_SERIES_DIVIDE)
        return PS_INVALID;
    ps_unit ua = unit_of(a), ub = unit_of(b), unit = ua;
    ps_result r = PS_OK;
    if (op == PS_SERIES_ADD || op == PS_SERIES_SUBTRACT) {
        if (!ps_unit_compatible(ua, ub))
            return PS_INVALID;
    } else
        r = op == PS_SERIES_MULTIPLY ? ps_unit_multiply(ua, ub, NULL, &unit)
                                     : ps_unit_divide(ua, ub, NULL, &unit);
    if (r != PS_OK)
        return r;
    series_slot draft;
    int slot;
    r = begin_derived(c, a, a->info.count, &draft, &slot);
    if (r != PS_OK)
        return r;
    set_unit(&draft, unit);
    snprintf(draft.info.name, sizeof draft.info.name, "combined series");
    double x[PS_SERIES_BLOCK_SIZE], y[PS_SERIES_BLOCK_SIZE];
    for (uint64_t at = 0; at < a->info.count;) {
        size_t n = (size_t)(a->info.count - at > PS_SERIES_BLOCK_SIZE ? PS_SERIES_BLOCK_SIZE
                                                                      : a->info.count - at);
        r = read_values(c, a, at, n, x);
        if (r != PS_OK)
            break;
        r = read_values(c, b, at, n, y);
        if (r != PS_OK)
            break;
        for (size_t i = 0; i < n; i++) {
            if (op == PS_SERIES_DIVIDE && y[i] == 0) {
                r = PS_NUMERIC;
                break;
            }
            x[i] = op == PS_SERIES_ADD        ? x[i] + y[i]
                   : op == PS_SERIES_SUBTRACT ? x[i] - y[i]
                   : op == PS_SERIES_MULTIPLY ? x[i] * y[i]
                                              : x[i] / y[i];
        }
        if (r != PS_OK)
            break;
        r = write_values(draft.file, x, n);
        if (r != PS_OK)
            break;
        at += n;
    }
    return finish_derived(c, &draft, slot, r, out);
}
ps_result ps_series_derivative(ps_analysis_context *c, ps_series hy, ps_series hx, ps_series *out) {
    series_slot *y = series_get(c, hy), *x = series_get(c, hx);
    if (!y || !x || !out || !aligned(x, y) || y->info.count < 2)
        return PS_INVALID;
    ps_unit unit;
    ps_result r = ps_unit_divide(unit_of(y), unit_of(x), NULL, &unit);
    if (r != PS_OK)
        return r;
    series_slot draft;
    int slot;
    r = begin_derived(c, y, y->info.count, &draft, &slot);
    if (r != PS_OK)
        return r;
    set_unit(&draft, unit);
    snprintf(draft.info.name, sizeof draft.info.name, "derivative(%.*s)", 48, y->info.name);
    double xx[PS_SERIES_BLOCK_SIZE + 2], yy[PS_SERIES_BLOCK_SIZE + 2], values[PS_SERIES_BLOCK_SIZE];
    for (uint64_t at = 0; at < y->info.count;) {
        size_t n = (size_t)(y->info.count - at > PS_SERIES_BLOCK_SIZE ? PS_SERIES_BLOCK_SIZE
                                                                      : y->info.count - at);
        size_t before = at ? 1 : 0, after = at + n < y->info.count ? 1 : 0,
               total = n + before + after;
        r = read_values(c, x, at - before, total, xx);
        if (r != PS_OK)
            break;
        r = read_values(c, y, at - before, total, yy);
        if (r != PS_OK)
            break;
        for (size_t i = 1; i < total; i++)
            if (xx[i] <= xx[i - 1]) {
                r = PS_INVALID;
                break;
            }
        if (r != PS_OK)
            break;
        for (size_t i = 0; i < n; i++) {
            size_t j = i + before, a = j ? j - 1 : 0, b = j + 1 < total ? j + 1 : j;
            double dx = xx[b] - xx[a], dy = yy[b] - yy[a];
            if (!isfinite(dx) || !isfinite(dy)) {
                r = PS_NUMERIC;
                break;
            }
            values[i] = dy / dx;
        }
        if (r != PS_OK)
            break;
        r = write_values(draft.file, values, n);
        if (r != PS_OK)
            break;
        at += n;
    }
    return finish_derived(c, &draft, slot, r, out);
}
ps_result ps_series_integral(ps_analysis_context *c, ps_series hy, ps_series hx,
                             ps_quantity initial, ps_series *out) {
    series_slot *y = series_get(c, hy), *x = series_get(c, hx);
    if (!y || !x || !out || !aligned(x, y) || !y->info.count)
        return PS_INVALID;
    ps_unit unit;
    ps_result r = ps_unit_multiply(unit_of(y), unit_of(x), NULL, &unit);
    if (r != PS_OK)
        return r;
    double sum;
    r = ps_convert(initial.value, initial.unit, unit, &sum);
    if (r != PS_OK)
        return r;
    series_slot draft;
    int slot;
    r = begin_derived(c, y, y->info.count, &draft, &slot);
    if (r != PS_OK)
        return r;
    set_unit(&draft, unit);
    snprintf(draft.info.name, sizeof draft.info.name, "integral(%.*s)", 48, y->info.name);
    double xx[PS_SERIES_BLOCK_SIZE], yy[PS_SERIES_BLOCK_SIZE], values[PS_SERIES_BLOCK_SIZE],
        previous_x = 0, previous_y = 0, compensation = 0;
    for (uint64_t at = 0; at < y->info.count;) {
        size_t n = (size_t)(y->info.count - at > PS_SERIES_BLOCK_SIZE ? PS_SERIES_BLOCK_SIZE
                                                                      : y->info.count - at);
        r = read_values(c, x, at, n, xx);
        if (r != PS_OK)
            break;
        r = read_values(c, y, at, n, yy);
        if (r != PS_OK)
            break;
        for (size_t i = 0; i < n; i++) {
            if (at + i) {
                if (xx[i] <= previous_x) {
                    r = PS_INVALID;
                    break;
                }
                double dx = xx[i] - previous_x;
                if (!isfinite(dx)) {
                    r = PS_NUMERIC;
                    break;
                }
                double delta = dx * (yy[i] * .5 + previous_y * .5) - compensation,
                       next = sum + delta;
                compensation = (next - sum) - delta;
                sum = next;
            }
            values[i] = sum;
            previous_x = xx[i];
            previous_y = yy[i];
        }
        if (r != PS_OK)
            break;
        r = write_values(draft.file, values, n);
        if (r != PS_OK)
            break;
        at += n;
    }
    return finish_derived(c, &draft, slot, r, out);
}
ps_result ps_series_moving_average(ps_analysis_context *c, ps_series h, size_t window,
                                   ps_series *out) {
    series_slot *s = series_get(c, h);
    if (!s || !out || !window || window > PS_SERIES_MAX_WINDOW)
        return PS_INVALID;
    series_slot draft;
    int slot;
    ps_result r = begin_derived(c, s, s->info.count, &draft, &slot);
    if (r != PS_OK)
        return r;
    snprintf(draft.info.name, sizeof draft.info.name, "mean(%.*s)", 48, s->info.name);
    double ring[PS_SERIES_MAX_WINDOW], block[PS_SERIES_BLOCK_SIZE], values[PS_SERIES_BLOCK_SIZE],
        sum = 0, compensation = 0;
    size_t used = 0, index = 0;
    for (uint64_t at = 0; at < s->info.count;) {
        size_t n = (size_t)(s->info.count - at > PS_SERIES_BLOCK_SIZE ? PS_SERIES_BLOCK_SIZE
                                                                      : s->info.count - at);
        r = read_values(c, s, at, n, block);
        if (r != PS_OK)
            break;
        for (size_t i = 0; i < n; i++) {
            double removed = used == window ? ring[index] : 0;
            if (used < window)
                used++;
            ring[index] = block[i];
            index = (index + 1) % window;
            double delta = (block[i] - removed) - compensation, next = sum + delta;
            compensation = (next - sum) - delta;
            sum = next;
            values[i] = sum / (double)used;
        }
        r = write_values(draft.file, values, n);
        if (r != PS_OK)
            break;
        at += n;
    }
    return finish_derived(c, &draft, slot, r, out);
}
/* A forward-only cursor; source columns share a FILE, so every block read seeks. */
typedef struct {
    const series_slot *series;
    uint64_t next;
    size_t used, count;
    double values[PS_SERIES_BLOCK_SIZE];
} series_cursor;
static ps_result cursor_next(ps_analysis_context *c, series_cursor *cursor, double *value) {
    if (cursor->used == cursor->count) {
        uint64_t remaining = cursor->series->info.count - cursor->next;
        if (!remaining)
            return PS_EOF;
        cursor->count =
            (size_t)(remaining > PS_SERIES_BLOCK_SIZE ? PS_SERIES_BLOCK_SIZE : remaining);
        ps_result r = read_values(c, cursor->series, cursor->next, cursor->count, cursor->values);
        if (r != PS_OK)
            return r;
        cursor->next += cursor->count;
        cursor->used = 0;
    }
    *value = cursor->values[cursor->used++];
    return PS_OK;
}
ps_result ps_series_resample_linear(ps_analysis_context *c, ps_series hy, ps_series hx,
                                    ps_series target, ps_series *out) {
    return ps_series_resample(c, hy, hx, target, PS_RESAMPLE_LINEAR, out);
}
ps_result ps_series_resample(ps_analysis_context *c, ps_series hy, ps_series hx, ps_series target,
                             ps_resample_method method, ps_series *out) {
    series_slot *y = series_get(c, hy), *x = series_get(c, hx), *q = series_get(c, target);
    if (method < PS_RESAMPLE_LINEAR || method > PS_RESAMPLE_PREVIOUS || !x || !y || !q || !out ||
        !aligned(x, y) || !x->info.count || !q->info.count ||
        !ps_unit_compatible(unit_of(x), unit_of(q)))
        return PS_INVALID;
    /* Validate the complete source axis, even if the target uses only a prefix. */
    series_cursor xc = {0}, yc = {0};
    xc.series = x;
    yc.series = y;
    double first = 0, last = 0, value;
    ps_result r;
    for (uint64_t i = 0; i < x->info.count; i++) {
        r = cursor_next(c, &xc, &value);
        if (r != PS_OK)
            return r;
        if (!isfinite(value) || (i && value <= last))
            return PS_INVALID;
        if (!i)
            first = value;
        last = value;
    }
    series_slot draft;
    int slot;
    r = begin_derived(c, q, q->info.count, &draft, &slot);
    if (r != PS_OK)
        return r;
    draft.info = y->info;
    draft.info.count = q->info.count;
    const char *names[] = {"linear", "nearest", "previous"};
    snprintf(draft.info.name, sizeof draft.info.name, "%s(%.*s)", names[method], 48, y->info.name);
    xc.next = 0;
    xc.used = xc.count = 0;
    double left_x = 0, right_x = 0, left_y = 0, right_y = 0, previous = 0;
    r = cursor_next(c, &xc, &right_x);
    if (r == PS_OK)
        r = cursor_next(c, &yc, &right_y);
    left_x = right_x;
    left_y = right_y;
    double block[PS_SERIES_BLOCK_SIZE];
    ps_unit from = unit_of(q), to = unit_of(x);
    for (uint64_t at = 0; at < q->info.count && r == PS_OK;) {
        size_t n = (size_t)(q->info.count - at > PS_SERIES_BLOCK_SIZE ? PS_SERIES_BLOCK_SIZE
                                                                      : q->info.count - at);
        r = read_values(c, q, at, n, block);
        for (size_t i = 0; i < n && r == PS_OK; i++) {
            double query;
            r = ps_convert(block[i], from, to, &query);
            if (r != PS_OK)
                break;
            if (query < first || query > last || (at + i && query <= previous)) {
                r = PS_INVALID;
                break;
            }
            previous = query;
            while (right_x < query && r == PS_OK) {
                left_x = right_x;
                left_y = right_y;
                r = cursor_next(c, &xc, &right_x);
                if (r == PS_OK)
                    r = cursor_next(c, &yc, &right_y);
            }
            if (r != PS_OK)
                break;
            if (query == right_x)
                block[i] = right_y;
            else if (method == PS_RESAMPLE_PREVIOUS)
                block[i] = left_y;
            else if (method == PS_RESAMPLE_NEAREST) {
                double left_distance = query - left_x, right_distance = right_x - query;
                if (!isfinite(left_distance) || !isfinite(right_distance)) {
                    left_distance = query * .5 - left_x * .5;
                    right_distance = right_x * .5 - query * .5;
                }
                block[i] = left_distance <= right_distance ? left_y : right_y;
            } else {
                double span = right_x - left_x;
                double fraction = isfinite(span)
                                      ? (query - left_x) / span
                                      : (query * .5 - left_x * .5) / (right_x * .5 - left_x * .5);
                /* Avoid overflow for finite, opposite-sign endpoints. */
                double interpolated = (left_y < 0) != (right_y < 0)
                                          ? (1 - fraction) * left_y + fraction * right_y
                                          : left_y + fraction * (right_y - left_y);
                block[i] = fmax(fmin(left_y, right_y), fmin(fmax(left_y, right_y), interpolated));
            }
        }
        if (r == PS_OK)
            r = write_values(draft.file, block, n);
        at += n;
    }
    return finish_derived(c, &draft, slot, r, out);
}
ps_result ps_series_statistics(ps_analysis_context *c, ps_series h, ps_statistics *out) {
    series_slot *s = series_get(c, h);
    if (!s || !out || !s->info.count)
        return PS_INVALID;
    double block[PS_SERIES_BLOCK_SIZE];
    ps_statistics stats = {0};
    for (uint64_t at = 0; at < s->info.count;) {
        size_t n = (size_t)(s->info.count - at > PS_SERIES_BLOCK_SIZE ? PS_SERIES_BLOCK_SIZE
                                                                      : s->info.count - at);
        ps_result r = read_values(c, s, at, n, block);
        if (r != PS_OK)
            return r;
        for (size_t i = 0; i < n; i++)
            ps_statistics_push(&stats, block[i]);
        if (!isfinite(stats.mean) || !isfinite(stats.m2))
            return PS_NUMERIC;
        at += n;
    }
    *out = stats;
    return PS_OK;
}
static int compare_finite_double(const void *left, const void *right) {
    double a = *(const double *)left, b = *(const double *)right;
    return (a > b) - (a < b);
}
ps_result ps_series_quantile_with_allocator(ps_analysis_context *c, ps_series h,
                                            double probability, ps_allocator allocator,
                                            double *out) {
    series_slot *s = series_get(c, h);
    if (!s || !out || !s->info.count || !isfinite(probability) ||
        probability < 0 || probability > 1)
        return PS_INVALID;
    if (s->info.count > SIZE_MAX / sizeof(double))
        return PS_LIMIT;
    size_t count = (size_t)s->info.count, bytes = count * sizeof(double);
    double *values = NULL;
    ps_result result = ps_memory_allocate(allocator, bytes, (void **)&values);
    if (result != PS_OK)
        return result;
    for (size_t at = 0; at < count && result == PS_OK;) {
        size_t take = count - at > PS_SERIES_BLOCK_SIZE ? PS_SERIES_BLOCK_SIZE : count - at;
        result = read_values(c, s, at, take, values + at);
        for (size_t i = 0; result == PS_OK && i < take; i++)
            if (!isfinite(values[at + i]))
                result = PS_NUMERIC;
        at += take;
    }
    if (result == PS_OK) {
        qsort(values, count, sizeof *values, compare_finite_double);
        double position = (double)(count - 1) * probability;
        size_t lower = (size_t)position;
        double quantile;
        if (lower >= count - 1)
            quantile = values[count - 1];
        else {
            double fraction = position - (double)lower;
            quantile = fraction == 0 ? values[lower]
                                     : (1 - fraction) * values[lower] + fraction * values[lower + 1];
        }
        if (!isfinite(quantile))
            result = PS_NUMERIC;
        else
            *out = quantile;
    }
    ps_memory_free(allocator, values, bytes);
    return result;
}
ps_result ps_series_quantile(ps_analysis_context *c, ps_series h, double probability,
                             double *out) {
    if (!c)
        return PS_INVALID;
    return ps_series_quantile_with_allocator(c, h, probability, c->allocator, out);
}
static void csv_text(FILE *file, const char *text) {
    fputc('"', file);
    for (; *text; text++) {
        if (*text == '"')
            fputc('"', file);
        fputc(*text, file);
    }
    fputc('"', file);
}
ps_result ps_series_export_csv(ps_analysis_context *c, const ps_series *columns, size_t count,
                               const char *path) {
    if (!c || !columns || !count || count > 32 || !path)
        return PS_INVALID;
    series_slot *series[32];
    for (size_t i = 0; i < count; i++) {
        series[i] = series_get(c, columns[i]);
        if (!series[i] || (i && !aligned(series[0], series[i])))
            return PS_INVALID;
    }
    FILE *file = fopen(path, "wx");
    if (!file)
        return PS_IO;
    for (size_t i = 0; i < count; i++) {
        if (i)
            fputc(',', file);
        char label[160];
        snprintf(label, sizeof label, "%s [%s]", series[i]->info.name, series[i]->info.symbol);
        csv_text(file, label);
    }
    fputc('\n', file);
    double block[32][PS_SERIES_BLOCK_SIZE];
    ps_result r = PS_OK;
    for (uint64_t at = 0; at < series[0]->info.count;) {
        size_t n = (size_t)(series[0]->info.count - at > PS_SERIES_BLOCK_SIZE
                                ? PS_SERIES_BLOCK_SIZE
                                : series[0]->info.count - at);
        for (size_t i = 0; i < count; i++) {
            r = read_values(c, series[i], at, n, block[i]);
            if (r != PS_OK)
                break;
        }
        if (r != PS_OK)
            break;
        for (size_t row = 0; row < n; row++) {
            for (size_t i = 0; i < count; i++)
                fprintf(file, "%s%.17g", i ? "," : "", block[i][row]);
            fputc('\n', file);
        }
        if (ferror(file)) {
            r = PS_IO;
            break;
        }
        at += n;
    }
    if (ferror(file))
        r = PS_IO;
    if (fclose(file))
        r = PS_IO;
    return r;
}
