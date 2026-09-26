#ifndef PHYSIM_LANGUAGE_STRING_H
#define PHYSIM_LANGUAGE_STRING_H
/* Internal immutable String storage for generated Physim code. */
#include "language_runtime.h"
#include "language_array.h"
#include <inttypes.h>
#include <math.h>
#include <stdio.h>

typedef psrt_array psrt_string;
static const psrt_element_type psrt_string_byte_type = {sizeof(unsigned char), NULL, NULL};

/* Validate Unicode scalars, including the shortest encoding and scalar range.
 * Escaped controls are legal in language strings; embedded NUL is not. */
static inline int psrt_string_utf8_valid(const void *bytes, size_t length) {
    if (length && !bytes)
        return 0;
    const unsigned char *data = bytes;
    for (size_t i = 0; i < length;) {
        unsigned char a = data[i];
        if (!a)
            return 0;
        if (a < 0x80) {
            i++;
            continue;
        }
        size_t width = a >= 0xC2 && a <= 0xDF ? 2
                     : a >= 0xE0 && a <= 0xEF ? 3
                     : a >= 0xF0 && a <= 0xF4 ? 4 : 0;
        if (!width || width > length - i)
            return 0;
        unsigned char b = data[i + 1];
        if (b < 0x80 || b > 0xBF ||
            (a == 0xE0 && b < 0xA0) || (a == 0xED && b >= 0xA0) ||
            (a == 0xF0 && b < 0x90) || (a == 0xF4 && b >= 0x90))
            return 0;
        for (size_t j = 2; j < width; j++)
            if (data[i + j] < 0x80 || data[i + j] > 0xBF)
                return 0;
        i += width;
    }
    return 1;
}

static inline int psrt_string_valid(const psrt_string *value) {
    if (!value || value->type != &psrt_string_byte_type || !value->block)
        return 0;
    size_t count = psrt_array_count(value);
    const unsigned char *data = psrt_array_data(value);
    return count && data[count - 1] == 0;
}
static inline const char *psrt_string_cstr(const psrt_string *value) {
    return psrt_string_valid(value) ? psrt_array_data(value) : NULL;
}
static inline bool psrt_valid_string_element(const void *value) {
    return psrt_string_valid(value);
}
static inline int psrt_compare_string_element(const void *left, const void *right) {
    return strcmp(psrt_string_cstr(left), psrt_string_cstr(right));
}
static inline size_t psrt_string_byte_count(const psrt_string *value) {
    return psrt_string_valid(value) ? psrt_array_count(value) - 1 : 0;
}
/* Unicode 15.0.0 PropList.txt White_Space, distinct from Pattern_White_Space. */
static inline int psrt_string_white_space(uint32_t scalar) {
    return (scalar >= 0x09u && scalar <= 0x0Du) || scalar == 0x20u ||
           scalar == 0x85u || scalar == 0xA0u || scalar == 0x1680u ||
           (scalar >= 0x2000u && scalar <= 0x200Au) ||
           scalar == 0x2028u || scalar == 0x2029u || scalar == 0x202Fu ||
           scalar == 0x205Fu || scalar == 0x3000u;
}
static inline uint32_t psrt_string_scalar_at(const unsigned char *data, size_t width) {
    uint32_t scalar = data[0] & (width == 1 ? 0x7Fu : width == 2 ? 0x1Fu
                                             : width == 3 ? 0x0Fu : 0x07u);
    for (size_t i = 1; i < width; i++)
        scalar = (scalar << 6) | (data[i] & 0x3Fu);
    return scalar;
}
/* String indices count Unicode scalars rather than bytes or grapheme clusters. */
static inline size_t psrt_string_scalar_count(const psrt_string *value) {
    if (!psrt_string_valid(value))
        return 0;
    const unsigned char *data = psrt_array_data(value);
    size_t bytes = psrt_string_byte_count(value), count = 0;
    for (size_t i = 0; i < bytes; count++)
        i += data[i] < 0x80 ? 1 : data[i] < 0xE0 ? 2 : data[i] < 0xF0 ? 3 : 4;
    return count;
}
/* Find a complete UTF-8 substring and report its Unicode-scalar position.
 * The empty needle matches at zero; a missing needle leaves index unchanged. */
static inline ps_result psrt_string_find(const psrt_string *haystack,
                                         const psrt_string *needle, bool *found,
                                         int64_t *index) {
    if (!psrt_string_valid(haystack) || !psrt_string_valid(needle) || !found || !index)
        return PS_INVALID;
    const unsigned char *data = psrt_array_data(haystack);
    const unsigned char *pattern = psrt_array_data(needle);
    size_t bytes = psrt_string_byte_count(haystack);
    size_t wanted = psrt_string_byte_count(needle);
    for (size_t i = 0, scalar = 0; i <= bytes;) {
        if (wanted <= bytes - i && !memcmp(data + i, pattern, wanted)) {
            *found = true;
            *index = (int64_t)scalar;
            return PS_OK;
        }
        if (i == bytes)
            break;
        i += data[i] < 0x80 ? 1 : data[i] < 0xE0 ? 2 : data[i] < 0xF0 ? 3 : 4;
        scalar++;
    }
    *found = false;
    return PS_OK;
}
/* The empty needle matches after the final scalar. On a miss, index is unchanged. */
static inline ps_result psrt_string_find_last(const psrt_string *haystack,
                                              const psrt_string *needle, bool *found,
                                              int64_t *index) {
    if (!psrt_string_valid(haystack) || !psrt_string_valid(needle) || !found || !index)
        return PS_INVALID;
    const unsigned char *data = psrt_array_data(haystack);
    const unsigned char *pattern = psrt_array_data(needle);
    size_t bytes = psrt_string_byte_count(haystack);
    size_t wanted = psrt_string_byte_count(needle);
    bool matched = false;
    int64_t last = 0;
    for (size_t i = 0, scalar = 0; i <= bytes;) {
        if (wanted <= bytes - i && !memcmp(data + i, pattern, wanted)) {
            matched = true;
            last = (int64_t)scalar;
        }
        if (i == bytes)
            break;
        i += data[i] < 0x80 ? 1 : data[i] < 0xE0 ? 2 : data[i] < 0xF0 ? 3 : 4;
        scalar++;
    }
    *found = matched;
    if (matched)
        *index = last;
    return PS_OK;
}
static inline ps_result psrt_string_matches_edge(const psrt_string *value,
                                                  const psrt_string *part, bool suffix,
                                                  bool *matches) {
    if (!psrt_string_valid(value) || !psrt_string_valid(part) || !matches)
        return PS_INVALID;
    size_t bytes = psrt_string_byte_count(value), wanted = psrt_string_byte_count(part);
    const unsigned char *data = psrt_array_data(value);
    const unsigned char *pattern = psrt_array_data(part);
    *matches = wanted <= bytes &&
               !memcmp(data + (suffix ? bytes - wanted : 0), pattern, wanted);
    return PS_OK;
}
/* Construct into unowned storage. No partial value is published on failure. */
static inline ps_result psrt_string_make(ps_allocator allocator, const void *bytes, size_t length,
                                         psrt_string *out) {
    if (!out)
        return PS_INVALID;
    if (length == SIZE_MAX)
        return PS_LIMIT;
    if (!psrt_string_utf8_valid(bytes, length))
        return PS_INVALID;
    psrt_string result;
    ps_result status = psrt_array_init(&psrt_string_byte_type, allocator, 0, &result);
    if (status != PS_OK)
        return status;
    status = psrt_array_build_begin(&result, length + 1);
    if (status != PS_OK)
        return status;
    unsigned char *destination = (unsigned char *)(result.block + 1);
    if (length)
        memcpy(destination, bytes, length);
    destination[length] = 0;
    result.block->value.count = length + 1;
    *out = result;
    return PS_OK;
}
static inline ps_result psrt_string_clone(const psrt_string *source, psrt_string *out) {
    return psrt_string_valid(source) ? psrt_array_clone(source, out) : PS_INVALID;
}
/* Advance a byte cursor by one complete Unicode scalar only after its owned
 * String value has been constructed. Source, cursor and output stay unchanged
 * when allocation fails. */
static inline ps_result psrt_string_next_scalar(const psrt_string *source, size_t *cursor,
                                                ps_allocator allocator, psrt_string *out) {
    if (!psrt_string_valid(source) || !cursor || !ps_allocator_valid(allocator) || !out ||
        source == out)
        return PS_INVALID;
    size_t bytes = psrt_string_byte_count(source);
    const unsigned char *data = psrt_array_data(source);
    if (*cursor >= bytes || (data[*cursor] & 0xC0) == 0x80)
        return PS_LIMIT;
    size_t width = data[*cursor] < 0x80 ? 1 : data[*cursor] < 0xE0 ? 2
                          : data[*cursor] < 0xF0 ? 3 : 4;
    if (width > bytes - *cursor)
        return PS_LIMIT;
    ps_result status = psrt_string_make(allocator, data + *cursor, width, out);
    if (status == PS_OK)
        *cursor += width;
    return status;
}
/* Move a byte cursor back by one complete Unicode scalar after constructing
 * its owned String value. On failure, leave cursor and output unchanged. */
static inline ps_result psrt_string_previous_scalar(const psrt_string *source, size_t *cursor,
                                                    ps_allocator allocator, psrt_string *out) {
    if (!psrt_string_valid(source) || !cursor || !ps_allocator_valid(allocator) || !out ||
        source == out)
        return PS_INVALID;
    size_t bytes = psrt_string_byte_count(source);
    const unsigned char *data = psrt_array_data(source);
    if (!*cursor || *cursor > bytes ||
        (*cursor < bytes && (data[*cursor] & 0xC0) == 0x80))
        return PS_LIMIT;
    size_t start = *cursor - 1;
    while (start && (data[start] & 0xC0) == 0x80)
        start--;
    ps_result status = psrt_string_make(allocator, data + start, *cursor - start, out);
    if (status == PS_OK)
        *cursor = start;
    return status;
}
static inline void psrt_string_destroy(void *value) {
    psrt_array_destroy(value);
}
/* Build owned one-scalar String elements for Sequence-style ordering methods.
 * On failure the output stays untouched and all partial values are released. */
static inline ps_result psrt_string_scalars(const psrt_string *source,
                                           const psrt_element_type *element_type,
                                           ps_allocator allocator, psrt_array *out) {
    if (!psrt_string_valid(source) || !element_type ||
        element_type->size != sizeof(psrt_string) || !element_type->copy ||
        !element_type->destroy || !ps_allocator_valid(allocator) || !out ||
        (const void *)source == (const void *)out)
        return PS_INVALID;
    psrt_array result;
    ps_result status = psrt_array_init(element_type, allocator, 0, &result);
    if (status != PS_OK)
        return status;
    size_t cursor = 0, bytes = psrt_string_byte_count(source);
    while (cursor < bytes) {
        psrt_string item;
        status = psrt_string_next_scalar(source, &cursor, allocator, &item);
        if (status != PS_OK)
            break;
        status = psrt_array_builder_append(&result, &item, 1);
        psrt_string_destroy(&item);
        if (status != PS_OK)
            break;
    }
    if (status != PS_OK) {
        psrt_array_destroy(&result);
        return status;
    }
    *out = result;
    return PS_OK;
}
static inline ps_result psrt_string_from_int64(int64_t value, ps_allocator allocator,
                                               psrt_string *out) {
    char text[32];
    int length = snprintf(text, sizeof text, "%" PRId64, value);
    if (length < 0 || (size_t)length >= sizeof text)
        return PS_LIMIT;
    return psrt_string_make(allocator, text, (size_t)length, out);
}
static inline ps_result psrt_string_from_float64(double value, ps_allocator allocator,
                                                 psrt_string *out) {
    char text[64];
    size_t length = psrt_format_float64(value, text);
    return length ? psrt_string_make(allocator, text, length, out) : PS_INVALID;
}
static inline ps_result psrt_string_from_bool(bool value, ps_allocator allocator,
                                              psrt_string *out) {
    const char *text = value ? "true" : "false";
    return psrt_string_make(allocator, text, strlen(text), out);
}
/* Concatenation builds a new value; inputs and output stay unchanged on error. */
static inline ps_result psrt_string_concat(const psrt_string *left, const psrt_string *right,
                                           ps_allocator allocator, psrt_string *out) {
    if (!psrt_string_valid(left) || !psrt_string_valid(right) || !out ||
        out == left || out == right)
        return PS_INVALID;
    size_t a = psrt_string_byte_count(left), b = psrt_string_byte_count(right);
    if (a > SIZE_MAX - b - 1)
        return PS_LIMIT;
    psrt_string result;
    ps_result status = psrt_array_init(&psrt_string_byte_type, allocator, 0, &result);
    if (status != PS_OK)
        return status;
    status = psrt_array_build_begin(&result, a + b + 1);
    if (status != PS_OK)
        return status;
    unsigned char *destination = (unsigned char *)(result.block + 1);
    memcpy(destination, psrt_array_data(left), a);
    memcpy(destination + a, psrt_array_data(right), b + 1);
    result.block->value.count = a + b + 1;
    *out = result;
    return PS_OK;
}
/* Repeat the UTF-8 bytes without changing scalar boundaries or the source.
 * Negative counts and oversized results fail before allocating. */
static inline ps_result psrt_string_repeat(const psrt_string *source, int64_t count,
                                           ps_allocator allocator, psrt_string *out) {
    if (!psrt_string_valid(source) || count < 0 || !ps_allocator_valid(allocator) ||
        !out || source == out)
        return PS_INVALID;
    size_t bytes = psrt_string_byte_count(source);
    if (!bytes || !count)
        return psrt_string_make(allocator, NULL, 0, out);
    if ((uint64_t)count > (SIZE_MAX - 1) / bytes)
        return PS_LIMIT;
    if (count == 1)
        return psrt_string_clone(source, out);
    size_t repeats = (size_t)count, total = repeats * bytes;
    psrt_string result;
    ps_result status = psrt_array_init(&psrt_string_byte_type, allocator, 0, &result);
    if (status != PS_OK)
        return status;
    status = psrt_array_build_begin(&result, total + 1);
    if (status != PS_OK)
        return status;
    unsigned char *destination = (unsigned char *)(result.block + 1);
    const unsigned char *data = psrt_array_data(source);
    memcpy(destination, data, bytes);
    for (size_t written = bytes; written < total;) {
        size_t chunk = total - written < written ? total - written : written;
        memcpy(destination + written, destination, chunk);
        written += chunk;
    }
    destination[total] = 0;
    result.block->value.count = total + 1;
    *out = result;
    return PS_OK;
}
/* Keep the span from the first through the last non-White_Space scalar.
 * Trimming never changes interior bytes or splits a UTF-8 scalar. */
static inline ps_result psrt_string_trimmed(const psrt_string *source,
                                            ps_allocator allocator, psrt_string *out) {
    if (!psrt_string_valid(source) || !ps_allocator_valid(allocator) || !out || source == out)
        return PS_INVALID;
    size_t bytes = psrt_string_byte_count(source);
    const unsigned char *data = psrt_array_data(source);
    if (!psrt_string_utf8_valid(data, bytes))
        return PS_INVALID;
    size_t first = bytes, last = 0;
    for (size_t i = 0; i < bytes;) {
        size_t width = data[i] < 0x80 ? 1 : data[i] < 0xE0 ? 2 : data[i] < 0xF0 ? 3 : 4;
        if (!psrt_string_white_space(psrt_string_scalar_at(data + i, width))) {
            if (first == bytes)
                first = i;
            last = i + width;
        }
        i += width;
    }
    if (first == bytes)
        return psrt_string_make(allocator, NULL, 0, out);
    if (!first && last == bytes)
        return psrt_string_clone(source, out);
    return psrt_string_make(allocator, data + first, last - first, out);
}
/* Reverse Unicode scalars while preserving the byte order within each UTF-8
 * sequence. The source and output remain unchanged if allocation fails. */
static inline ps_result psrt_string_reversed(const psrt_string *source,
                                              ps_allocator allocator, psrt_string *out) {
    if (!psrt_string_valid(source) || !ps_allocator_valid(allocator) || !out || source == out)
        return PS_INVALID;
    size_t bytes = psrt_string_byte_count(source);
    const unsigned char *data = psrt_array_data(source);
    if (!psrt_string_utf8_valid(data, bytes))
        return PS_INVALID;
    psrt_string result;
    ps_result status = psrt_array_init(&psrt_string_byte_type, allocator, 0, &result);
    if (status != PS_OK)
        return status;
    status = psrt_array_build_begin(&result, bytes + 1);
    if (status != PS_OK)
        return status;
    unsigned char *destination = (unsigned char *)(result.block + 1);
    for (size_t i = 0; i < bytes;) {
        size_t width = data[i] < 0x80 ? 1 : data[i] < 0xE0 ? 2 : data[i] < 0xF0 ? 3 : 4;
        memcpy(destination + bytes - i - width, data + i, width);
        i += width;
    }
    destination[bytes] = 0;
    result.block->value.count = bytes + 1;
    *out = result;
    return PS_OK;
}

/* Replace non-overlapping scalar-aligned matches from left to right. An empty
 * search matches at every scalar boundary, including both ends. */
static inline ps_result psrt_string_replace(const psrt_string *source,
                                             const psrt_string *search,
                                             const psrt_string *replacement,
                                             ps_allocator allocator, psrt_string *out) {
    if (!psrt_string_valid(source) || !psrt_string_valid(search) ||
        !psrt_string_valid(replacement) || !ps_allocator_valid(allocator) || !out ||
        out == source || out == search || out == replacement)
        return PS_INVALID;
    size_t bytes = psrt_string_byte_count(source);
    size_t wanted = psrt_string_byte_count(search);
    size_t inserted = psrt_string_byte_count(replacement);
    const unsigned char *data = psrt_array_data(source);
    const unsigned char *pattern = psrt_array_data(search);
    const unsigned char *substitute = psrt_array_data(replacement);
    size_t matches = 0;
    for (size_t i = 0; i < bytes || (!wanted && i == bytes);) {
        if (wanted <= bytes - i && !memcmp(data + i, pattern, wanted)) {
            matches++;
            if (wanted) {
                i += wanted;
                continue;
            }
        }
        if (i == bytes)
            break;
        i += data[i] < 0x80 ? 1 : data[i] < 0xE0 ? 2 : data[i] < 0xF0 ? 3 : 4;
    }
    if (!matches || (wanted == 0 && inserted == 0))
        return psrt_string_clone(source, out);
    size_t result_bytes = bytes;
    if (inserted > wanted) {
        size_t growth = inserted - wanted;
        if (matches > (SIZE_MAX - 1 - bytes) / growth)
            return PS_LIMIT;
        result_bytes += matches * growth;
    } else
        result_bytes -= matches * (wanted - inserted);
    psrt_string result;
    ps_result status = psrt_array_init(&psrt_string_byte_type, allocator, 0, &result);
    if (status != PS_OK)
        return status;
    status = psrt_array_build_begin(&result, result_bytes + 1);
    if (status != PS_OK)
        return status;
    unsigned char *destination = (unsigned char *)(result.block + 1);
    size_t written = 0;
    for (size_t i = 0; i < bytes || (!wanted && i == bytes);) {
        if (wanted <= bytes - i && !memcmp(data + i, pattern, wanted)) {
            if (inserted)
                memcpy(destination + written, substitute, inserted);
            written += inserted;
            if (wanted) {
                i += wanted;
                continue;
            }
        }
        if (i == bytes)
            break;
        size_t width = data[i] < 0x80 ? 1 : data[i] < 0xE0 ? 2 : data[i] < 0xF0 ? 3 : 4;
        memcpy(destination + written, data + i, width);
        written += width;
        i += width;
    }
    destination[written] = 0;
    result.block->value.count = written + 1;
    *out = result;
    return PS_OK;
}

static inline ps_result psrt_string_split_push(psrt_array *parts, ps_allocator allocator,
                                               const unsigned char *data, size_t begin,
                                               size_t end) {
    psrt_string part;
    ps_result status = psrt_string_make(allocator, data + begin, end - begin, &part);
    if (status != PS_OK)
        return status;
    status = psrt_array_copy_one(parts->block, &part);
    psrt_string_destroy(&part);
    return status;
}
/* Split at scalar-aligned matches. An empty separator is a Physim extension
 * that splits between Unicode scalars. */
static inline ps_result psrt_string_split(const psrt_string *source,
                                          const psrt_string *separator,
                                          int64_t max_splits, bool omit_empty,
                                          ps_allocator allocator, psrt_array *out) {
    if (!psrt_string_valid(source) || !psrt_string_valid(separator) ||
        max_splits < 0 || !ps_allocator_valid(allocator) || !out ||
        out == source || out == separator)
        return PS_INVALID;
    size_t bytes = psrt_string_byte_count(source);
    size_t length = psrt_string_byte_count(separator);
    const unsigned char *data = psrt_array_data(source);
    const unsigned char *pattern = psrt_array_data(separator);
    uint64_t limit = (uint64_t)max_splits;
    size_t count = 0, begin = 0, produced = 0;
    if (length) {
        for (size_t i = 0; i < bytes && (uint64_t)produced < limit;) {
            if (length <= bytes - i && !memcmp(data + i, pattern, length)) {
                if (!omit_empty || i > begin) {
                    count++;
                    produced++;
                }
                i += length;
                begin = i;
            } else
                i += data[i] < 0x80 ? 1 : data[i] < 0xE0 ? 2 : data[i] < 0xF0 ? 3 : 4;
        }
        if (!omit_empty || begin < bytes)
            count++;
    } else if (bytes) {
        for (size_t i = 0; i < bytes;) {
            count++;
            i += data[i] < 0x80 ? 1 : data[i] < 0xE0 ? 2 : data[i] < 0xF0 ? 3 : 4;
        }
        if ((uint64_t)count > limit + 1)
            count = (size_t)(limit + 1);
    }
    psrt_array result;
    ps_result status = psrt_array_init(&psrt_array_element_type, allocator, 0, &result);
    if (status != PS_OK)
        return status;
    status = psrt_array_build_begin(&result, count);
    if (status != PS_OK)
        return status;
    begin = 0;
    produced = 0;
    if (length) {
        for (size_t i = 0; i < bytes && (uint64_t)produced < limit;) {
            if (length <= bytes - i && !memcmp(data + i, pattern, length)) {
                if (!omit_empty || i > begin) {
                    status = psrt_string_split_push(&result, allocator, data, begin, i);
                    if (status != PS_OK) goto failed;
                    produced++;
                }
                i += length;
                begin = i;
            } else
                i += data[i] < 0x80 ? 1 : data[i] < 0xE0 ? 2 : data[i] < 0xF0 ? 3 : 4;
        }
        if (!omit_empty || begin < bytes) {
            status = psrt_string_split_push(&result, allocator, data, begin, bytes);
            if (status != PS_OK) goto failed;
        }
    } else if (bytes) {
        for (size_t i = 0; i < bytes;) {
            size_t width = data[i] < 0x80 ? 1 : data[i] < 0xE0 ? 2 : data[i] < 0xF0 ? 3 : 4;
            size_t end = i + width;
            if (end < bytes && (uint64_t)produced < limit) {
                status = psrt_string_split_push(&result, allocator, data, i, end);
                produced++;
            } else {
                status = psrt_string_split_push(&result, allocator, data, i, bytes);
                end = bytes;
            }
            if (status != PS_OK) goto failed;
            i = end;
        }
    }
    *out = result;
    return PS_OK;
failed:
    psrt_array_destroy(&result);
    return status;
}

/* Join String array elements in order, inserting the separator between values.
 * The output is independently owned and is published only after success. */
static inline ps_result psrt_string_join(const psrt_array *parts,
                                         const psrt_string *separator,
                                         ps_allocator allocator, psrt_string *out) {
    if (!parts || parts->type != &psrt_array_element_type ||
        !psrt_string_valid(separator) || !ps_allocator_valid(allocator) || !out ||
        out == parts || out == separator)
        return PS_INVALID;
    size_t count = psrt_array_count(parts);
    const psrt_string *items = psrt_array_data(parts);
    size_t bytes = 0;
    for (size_t i = 0; i < count; i++) {
        if (!psrt_string_valid(&items[i]))
            return PS_INVALID;
        size_t length = psrt_string_byte_count(&items[i]);
        if (length > SIZE_MAX - 1 - bytes)
            return PS_LIMIT;
        bytes += length;
    }
    size_t gap = psrt_string_byte_count(separator);
    if (count > 1) {
        if (gap > (SIZE_MAX - 1 - bytes) / (count - 1))
            return PS_LIMIT;
        bytes += (count - 1) * gap;
    }
    psrt_string result;
    ps_result status = psrt_array_init(&psrt_string_byte_type, allocator, 0, &result);
    if (status != PS_OK)
        return status;
    status = psrt_array_build_begin(&result, bytes + 1);
    if (status != PS_OK)
        return status;
    unsigned char *destination = (unsigned char *)(result.block + 1);
    size_t written = 0;
    for (size_t i = 0; i < count; i++) {
        if (i && gap) {
            memcpy(destination + written, psrt_array_data(separator), gap);
            written += gap;
        }
        size_t length = psrt_string_byte_count(&items[i]);
        if (length) {
            memcpy(destination + written, psrt_array_data(&items[i]), length);
            written += length;
        }
    }
    destination[written] = 0;
    result.block->value.count = written + 1;
    *out = result;
    return PS_OK;
}

/* Slice by scalar indices. The end is exclusive unless closed; missing bounds
 * mean start/end of the string. A positive step selects individual scalars. */
static inline ps_result psrt_string_slice_bounds(const psrt_string *source, int has_begin,
                                                 int64_t begin, int has_end, int64_t end,
                                                 int closed, int64_t step,
                                                 ps_allocator allocator, psrt_string *out) {
    if (!psrt_string_valid(source) || !out || source == out)
        return PS_INVALID;
    if (step <= 0)
        return PS_LIMIT;
    size_t length = psrt_string_scalar_count(source);
    if ((has_begin && (begin < 0 || (uint64_t)begin > length)) ||
        (has_end && (end < 0 || (uint64_t)end > length ||
                     (closed && (uint64_t)end == length))) ||
        (has_begin && has_end && end < begin))
        return PS_LIMIT;
    size_t first = has_begin ? (size_t)begin : 0;
    size_t stop = has_end ? (size_t)end + (closed ? 1u : 0u) : length;
    if (stop < first)
        return PS_LIMIT;
    const unsigned char *data = psrt_array_data(source);
    size_t bytes = psrt_string_byte_count(source), selected_bytes = 0;
    for (size_t i = 0, scalar = 0; i < bytes; scalar++) {
        size_t width = data[i] < 0x80 ? 1 : data[i] < 0xE0 ? 2 : data[i] < 0xF0 ? 3 : 4;
        if (scalar >= first && scalar < stop && (scalar - first) % (uint64_t)step == 0)
            selected_bytes += width;
        i += width;
    }
    psrt_string result;
    ps_result status = psrt_array_init(&psrt_string_byte_type, allocator, 0, &result);
    if (status != PS_OK)
        return status;
    status = psrt_array_build_begin(&result, selected_bytes + 1);
    if (status != PS_OK)
        return status;
    unsigned char *destination = (unsigned char *)(result.block + 1);
    size_t written = 0;
    for (size_t i = 0, scalar = 0; i < bytes; scalar++) {
        size_t width = data[i] < 0x80 ? 1 : data[i] < 0xE0 ? 2 : data[i] < 0xF0 ? 3 : 4;
        if (scalar >= first && scalar < stop && (scalar - first) % (uint64_t)step == 0) {
            memcpy(destination + written, data + i, width);
            written += width;
        }
        i += width;
    }
    destination[written] = 0;
    result.block->value.count = written + 1;
    *out = result;
    return PS_OK;
}
/* Select or omit Unicode scalars at a String edge. The slice owns its bytes;
 * source and output may not be the same handle. */
static inline ps_result psrt_string_select_edge(const psrt_string *source, int64_t requested,
                                                bool drop, bool last, ps_allocator allocator,
                                                psrt_string *out) {
    if (!psrt_string_valid(source) || !out || source == out)
        return PS_INVALID;
    if (requested < 0)
        return PS_LIMIT;
    size_t length = psrt_string_scalar_count(source);
    size_t amount = (uint64_t)requested < length ? (size_t)requested : length;
    size_t count = drop ? length - amount : amount;
    size_t start = drop ? (last ? 0 : amount) : (last ? length - count : 0);
    return psrt_string_slice_bounds(source, 1, (int64_t)start, 1,
                                    (int64_t)(start + count), 0, 1, allocator, out);
}

/* C SDK Unit and Quantity values borrow their symbol pointer. Keep the source
 * String alive for the module lifetime, deduplicating equal symbols so calls
 * in a simulation loop do not keep adding the same symbol. */
typedef struct psrt_string_pin {
    struct psrt_string_pin *next;
    psrt_string value;
} psrt_string_pin;
static inline ps_result psrt_string_pin_cstr(psrt_memory *memory, const psrt_string *value,
                                             const char **out) {
    if (!memory || !psrt_string_valid(value) || !out)
        return PS_INVALID;
    const char *text = psrt_string_cstr(value);
    for (psrt_string_pin *pin = memory->string_pins; pin; pin = pin->next)
        if (!strcmp(psrt_string_cstr(&pin->value), text)) {
            *out = psrt_string_cstr(&pin->value);
            return PS_OK;
        }
    psrt_string_pin *pin = NULL;
    ps_allocator allocator = psrt_memory_allocator(memory);
    ps_result status = ps_memory_allocate(allocator, sizeof *pin, (void **)&pin);
    if (status != PS_OK)
        return status;
    status = psrt_string_clone(value, &pin->value);
    if (status != PS_OK) {
        ps_memory_free(allocator, pin, sizeof *pin);
        return status;
    }
    pin->next = memory->string_pins;
    memory->string_pins = pin;
    *out = psrt_string_cstr(&pin->value);
    return PS_OK;
}
static inline void psrt_string_pins_destroy(psrt_memory *memory) {
    if (!memory)
        return;
    ps_allocator allocator = psrt_memory_allocator(memory);
    while (memory->string_pins) {
        psrt_string_pin *pin = memory->string_pins;
        memory->string_pins = pin->next;
        psrt_string_destroy(&pin->value);
        ps_memory_free(allocator, pin, sizeof *pin);
    }
}
static inline ps_result psrt_string_array_cstr(const psrt_array *values, const char **out,
                                                size_t capacity) {
    if (!values || !out)
        return PS_INVALID;
    size_t count = psrt_array_count(values);
    if (count > capacity)
        return PS_LIMIT;
    const psrt_string *items = psrt_array_data(values);
    for (size_t i = 0; i < count; i++)
        if (!psrt_string_valid(&items[i]))
            return PS_INVALID;
    for (size_t i = 0; i < count; i++)
        out[i] = psrt_string_cstr(&items[i]);
    return PS_OK;
}
#endif
