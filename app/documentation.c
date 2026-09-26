#include "documentation.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static uint32_t folded_char(const char **text) {
    const unsigned char *p = (const unsigned char *)*text;
    uint32_t c = *p++;
    unsigned extra = 0;
    if (c >= 0xc2 && c <= 0xdf) {
        c &= 31;
        extra = 1;
    } else if (c >= 0xe0 && c <= 0xef) {
        c &= 15;
        extra = 2;
    } else if (c >= 0xf0 && c <= 0xf4) {
        c &= 7;
        extra = 3;
    }
    for (unsigned i = 0; i < extra; i++) {
        if ((p[i] & 0xc0) != 0x80) {
            uint32_t original = (unsigned char)**text;
            (*text)++;
            return original;
        }
        c = (c << 6) | (p[i] & 63);
    }
    *text = (const char *)(p + extra);
    if ((c >= 'A' && c <= 'Z') || (c >= 0xc0 && c <= 0xde && c != 0xd7))
        c += 32;
    if (c == 0x1e9e)
        c = 0xdf;
    return c;
}
const char *ps_document_find(const char *text, const char *query) {
    if (!text || !query || !*query)
        return NULL;
    for (const char *p = text; *p;) {
        const char *a = p, *b = query;
        bool same = true;
        while (*a && *b) {
            uint32_t ca = folded_char(&a), cb = folded_char(&b);
            if (ca != cb) {
                same = false;
                break;
            }
        }
        if (same && !*b)
            return p;
        folded_char(&p);
    }
    return NULL;
}
static bool append(ps_document *d, ps_doc_kind kind, unsigned level, const char *s, size_t n,
                   const char *target, size_t tn) {
    if (d->count == PS_DOC_MAX_BLOCKS || n + tn + 2 > sizeof d->text - d->used)
        return false;
    ps_doc_block *b = &d->blocks[d->count++];
    b->kind = kind;
    b->level = level;
    b->text = d->used;
    b->length = n;
    memcpy(d->text + d->used, s, n);
    d->used += n;
    d->text[d->used++] = 0;
    b->target = d->used;
    if (tn)
        memcpy(d->text + d->used, target, tn);
    d->used += tn;
    d->text[d->used++] = 0;
    return true;
}
/* Inline decoration is reduced to readable text; links remain explicit actions. */
static bool paragraph(ps_document *d, const char *s, size_t n, ps_doc_kind kind, unsigned level) {
    char *plain = malloc(n + 1);
    if (!plain)
        return false;
    size_t used = 0;
    for (size_t i = 0; i < n;) {
        if (s[i] == '`' || (s[i] == '*' && i + 1 < n && s[i + 1] == '*')) {
            i += s[i] == '`' ? 1 : 2;
            continue;
        }
        if (s[i] == '[') {
            const char *close = memchr(s + i + 1, ']', n - i - 1);
            if (close && close + 1 < s + n && close[1] == '(') {
                const char *end = memchr(close + 2, ')', (size_t)(s + n - close - 2));
                if (end) {
                    size_t length = (size_t)(close - (s + i + 1));
                    memcpy(plain + used, s + i + 1, length);
                    used += length;
                    i = (size_t)(end - s) + 1;
                    continue;
                }
            }
        }
        plain[used++] = s[i++];
    }
    bool ok = append(d, kind, level, plain, used, NULL, 0);
    free(plain);
    if (!ok)
        return false;
    for (size_t i = 0; i < n; i++)
        if (s[i] == '[') {
            const char *close = memchr(s + i + 1, ']', n - i - 1);
            if (close && close + 1 < s + n && close[1] == '(') {
                const char *end = memchr(close + 2, ')', (size_t)(s + n - close - 2));
                if (end) {
                    if (!append(d, PS_DOC_LINK, 0, s + i + 1, (size_t)(close - s - i - 1),
                                close + 2, (size_t)(end - close - 2)))
                        return false;
                    i = (size_t)(end - s);
                }
            }
        }
    return true;
}
ps_result ps_document_parse(const char *source, size_t size, bool code, ps_document **out) {
    if (!source || !out || size > PS_DOC_MAX_BYTES || memchr(source, 0, size))
        return PS_INVALID;
    ps_document *d = calloc(1, sizeof *d);
    char *pending = malloc(size + 1);
    if (!d || !pending) {
        free(d);
        free(pending);
        return PS_MEMORY;
    }
    if (code) {
        if (!append(d, PS_DOC_CODE, 0, source, size, NULL, 0))
            goto limit;
    } else {
        size_t at = 0, length = 0;
        bool fenced = false;
        while (at < size) {
            size_t begin = at;
            while (at < size && source[at] != '\n')
                at++;
            size_t end = at;
            if (at < size)
                at++;
            if (end > begin && source[end - 1] == '\r')
                end--;
            const char *line = source + begin;
            size_t n = end - begin;
            bool fence = n >= 3 && !memcmp(line, "```", 3);
            if (fence) {
                if (length && !(fenced ? append(d, PS_DOC_CODE, 0, pending, length, NULL, 0)
                                       : paragraph(d, pending, length, PS_DOC_TEXT, 0)))
                    goto limit;
                length = 0;
                fenced = !fenced;
                continue;
            }
            if (fenced) {
                memcpy(pending + length, line, n);
                length += n;
                pending[length++] = '\n';
                continue;
            }
            unsigned heading = 0;
            while (heading < n && line[heading] == '#')
                heading++;
            bool list =
                n >= 2 && ((line[0] == '-' && line[1] == ' ') ||
                           (isdigit((unsigned char)line[0]) && memchr(line, '.', n < 4 ? n : 4)));
            bool special =
                !n || heading || list || line[0] == '|' || (n == 3 && !memcmp(line, "---", 3));
            if (special && length) {
                if (!paragraph(d, pending, length, PS_DOC_TEXT, 0))
                    goto limit;
                length = 0;
            }
            if (!n)
                continue;
            if (heading && heading < n && line[heading] == ' ') {
                if (!paragraph(d, line + heading + 1, n - heading - 1, PS_DOC_HEADING, heading))
                    goto limit;
            } else if (n == 3 && !memcmp(line, "---", 3)) {
                if (!append(d, PS_DOC_RULE, 0, "", 0, NULL, 0))
                    goto limit;
            } else if (list || line[0] == '|') {
                bool separator = line[0] == '|' && memchr(line, '-', n);
                for (size_t i = 0; separator && i < n; i++)
                    if (line[i] != '|' && line[i] != '-' && line[i] != ':' && line[i] != ' ' &&
                        line[i] != '\t')
                        separator = false;
                if (separator) {
                    for (size_t i = d->count; i > 0; i--)
                        if (d->blocks[i - 1].kind == PS_DOC_TABLE_ROW) {
                            d->blocks[i - 1].level = 1;
                            break;
                        }
                    continue;
                }
                if (!paragraph(d, line, n, line[0] == '|' ? PS_DOC_TABLE_ROW : PS_DOC_TEXT, 0))
                    goto limit;
            } else {
                if (length)
                    pending[length++] = ' ';
                memcpy(pending + length, line, n);
                length += n;
            }
        }
        if (length && !(fenced ? append(d, PS_DOC_CODE, 0, pending, length, NULL, 0)
                               : paragraph(d, pending, length, PS_DOC_TEXT, 0)))
            goto limit;
    }
    free(pending);
    *out = d;
    return PS_OK;
limit:
    free(pending);
    free(d);
    return PS_LIMIT;
}
