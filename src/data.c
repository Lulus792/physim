#include "physim/data.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
ps_result ps_channel_status_index(const ps_channel *schema, uint32_t count, uint32_t channel,
                                  int *index) {
    if (!schema || !index || !count || count > PS_MAX_CHANNELS || channel >= count)
        return PS_INVALID;
    for (uint32_t i = 0; i < count; i++) {
        if (!memchr(schema[i].name, 0, sizeof schema[i].name))
            return PS_INVALID;
        for (uint32_t j = 0; j < i; j++)
            if (!strcmp(schema[i].name, schema[j].name))
                return PS_INVALID;
    }
    char name[64];
    size_t length = strlen(schema[channel].name);
    if (length > 2 && !strcmp(schema[channel].name + length - 2, ".u"))
        length -= 2;
    snprintf(name, sizeof name, "%.*s.status", (int)length, schema[channel].name);
    int found = -1;
    for (uint32_t i = 0; i < count; i++)
        if (!strcmp(name, schema[i].name)) {
            const int8_t zero[7] = {0};
            if (memcmp(schema[i].dimension, zero, 7))
                return PS_INVALID;
            found = (int)i;
        }
    *index = found;
    return PS_OK;
}
void ps_put_u32(unsigned char *p, uint32_t v) {
    for (int i = 0; i < 4; i++)
        p[i] = (unsigned char)(v >> (8 * i));
}
uint32_t ps_get_u32(const unsigned char *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
void ps_put_f64(unsigned char *p, double v) {
    uint64_t u;
    memcpy(&u, &v, 8);
    for (int i = 0; i < 8; i++)
        p[i] = (unsigned char)(u >> (8 * i));
}
double ps_get_f64(const unsigned char *p) {
    uint64_t u = 0;
    double v;
    for (int i = 0; i < 8; i++)
        u |= (uint64_t)p[i] << (8 * i);
    memcpy(&v, &u, 8);
    return v;
}
uint32_t ps_crc32(const unsigned char *p, size_t n) {
    /* Reflected polynomial 0xedb88320, initial/final XOR 0xffffffff.
     * Immutable table: no initialization race or platform-specific instructions. */
    static const uint32_t table[256] = {
        UINT32_C(0x00000000), UINT32_C(0x77073096), UINT32_C(0xee0e612c), UINT32_C(0x990951ba),
        UINT32_C(0x076dc419), UINT32_C(0x706af48f), UINT32_C(0xe963a535), UINT32_C(0x9e6495a3),
        UINT32_C(0x0edb8832), UINT32_C(0x79dcb8a4), UINT32_C(0xe0d5e91e), UINT32_C(0x97d2d988),
        UINT32_C(0x09b64c2b), UINT32_C(0x7eb17cbd), UINT32_C(0xe7b82d07), UINT32_C(0x90bf1d91),
        UINT32_C(0x1db71064), UINT32_C(0x6ab020f2), UINT32_C(0xf3b97148), UINT32_C(0x84be41de),
        UINT32_C(0x1adad47d), UINT32_C(0x6ddde4eb), UINT32_C(0xf4d4b551), UINT32_C(0x83d385c7),
        UINT32_C(0x136c9856), UINT32_C(0x646ba8c0), UINT32_C(0xfd62f97a), UINT32_C(0x8a65c9ec),
        UINT32_C(0x14015c4f), UINT32_C(0x63066cd9), UINT32_C(0xfa0f3d63), UINT32_C(0x8d080df5),
        UINT32_C(0x3b6e20c8), UINT32_C(0x4c69105e), UINT32_C(0xd56041e4), UINT32_C(0xa2677172),
        UINT32_C(0x3c03e4d1), UINT32_C(0x4b04d447), UINT32_C(0xd20d85fd), UINT32_C(0xa50ab56b),
        UINT32_C(0x35b5a8fa), UINT32_C(0x42b2986c), UINT32_C(0xdbbbc9d6), UINT32_C(0xacbcf940),
        UINT32_C(0x32d86ce3), UINT32_C(0x45df5c75), UINT32_C(0xdcd60dcf), UINT32_C(0xabd13d59),
        UINT32_C(0x26d930ac), UINT32_C(0x51de003a), UINT32_C(0xc8d75180), UINT32_C(0xbfd06116),
        UINT32_C(0x21b4f4b5), UINT32_C(0x56b3c423), UINT32_C(0xcfba9599), UINT32_C(0xb8bda50f),
        UINT32_C(0x2802b89e), UINT32_C(0x5f058808), UINT32_C(0xc60cd9b2), UINT32_C(0xb10be924),
        UINT32_C(0x2f6f7c87), UINT32_C(0x58684c11), UINT32_C(0xc1611dab), UINT32_C(0xb6662d3d),
        UINT32_C(0x76dc4190), UINT32_C(0x01db7106), UINT32_C(0x98d220bc), UINT32_C(0xefd5102a),
        UINT32_C(0x71b18589), UINT32_C(0x06b6b51f), UINT32_C(0x9fbfe4a5), UINT32_C(0xe8b8d433),
        UINT32_C(0x7807c9a2), UINT32_C(0x0f00f934), UINT32_C(0x9609a88e), UINT32_C(0xe10e9818),
        UINT32_C(0x7f6a0dbb), UINT32_C(0x086d3d2d), UINT32_C(0x91646c97), UINT32_C(0xe6635c01),
        UINT32_C(0x6b6b51f4), UINT32_C(0x1c6c6162), UINT32_C(0x856530d8), UINT32_C(0xf262004e),
        UINT32_C(0x6c0695ed), UINT32_C(0x1b01a57b), UINT32_C(0x8208f4c1), UINT32_C(0xf50fc457),
        UINT32_C(0x65b0d9c6), UINT32_C(0x12b7e950), UINT32_C(0x8bbeb8ea), UINT32_C(0xfcb9887c),
        UINT32_C(0x62dd1ddf), UINT32_C(0x15da2d49), UINT32_C(0x8cd37cf3), UINT32_C(0xfbd44c65),
        UINT32_C(0x4db26158), UINT32_C(0x3ab551ce), UINT32_C(0xa3bc0074), UINT32_C(0xd4bb30e2),
        UINT32_C(0x4adfa541), UINT32_C(0x3dd895d7), UINT32_C(0xa4d1c46d), UINT32_C(0xd3d6f4fb),
        UINT32_C(0x4369e96a), UINT32_C(0x346ed9fc), UINT32_C(0xad678846), UINT32_C(0xda60b8d0),
        UINT32_C(0x44042d73), UINT32_C(0x33031de5), UINT32_C(0xaa0a4c5f), UINT32_C(0xdd0d7cc9),
        UINT32_C(0x5005713c), UINT32_C(0x270241aa), UINT32_C(0xbe0b1010), UINT32_C(0xc90c2086),
        UINT32_C(0x5768b525), UINT32_C(0x206f85b3), UINT32_C(0xb966d409), UINT32_C(0xce61e49f),
        UINT32_C(0x5edef90e), UINT32_C(0x29d9c998), UINT32_C(0xb0d09822), UINT32_C(0xc7d7a8b4),
        UINT32_C(0x59b33d17), UINT32_C(0x2eb40d81), UINT32_C(0xb7bd5c3b), UINT32_C(0xc0ba6cad),
        UINT32_C(0xedb88320), UINT32_C(0x9abfb3b6), UINT32_C(0x03b6e20c), UINT32_C(0x74b1d29a),
        UINT32_C(0xead54739), UINT32_C(0x9dd277af), UINT32_C(0x04db2615), UINT32_C(0x73dc1683),
        UINT32_C(0xe3630b12), UINT32_C(0x94643b84), UINT32_C(0x0d6d6a3e), UINT32_C(0x7a6a5aa8),
        UINT32_C(0xe40ecf0b), UINT32_C(0x9309ff9d), UINT32_C(0x0a00ae27), UINT32_C(0x7d079eb1),
        UINT32_C(0xf00f9344), UINT32_C(0x8708a3d2), UINT32_C(0x1e01f268), UINT32_C(0x6906c2fe),
        UINT32_C(0xf762575d), UINT32_C(0x806567cb), UINT32_C(0x196c3671), UINT32_C(0x6e6b06e7),
        UINT32_C(0xfed41b76), UINT32_C(0x89d32be0), UINT32_C(0x10da7a5a), UINT32_C(0x67dd4acc),
        UINT32_C(0xf9b9df6f), UINT32_C(0x8ebeeff9), UINT32_C(0x17b7be43), UINT32_C(0x60b08ed5),
        UINT32_C(0xd6d6a3e8), UINT32_C(0xa1d1937e), UINT32_C(0x38d8c2c4), UINT32_C(0x4fdff252),
        UINT32_C(0xd1bb67f1), UINT32_C(0xa6bc5767), UINT32_C(0x3fb506dd), UINT32_C(0x48b2364b),
        UINT32_C(0xd80d2bda), UINT32_C(0xaf0a1b4c), UINT32_C(0x36034af6), UINT32_C(0x41047a60),
        UINT32_C(0xdf60efc3), UINT32_C(0xa867df55), UINT32_C(0x316e8eef), UINT32_C(0x4669be79),
        UINT32_C(0xcb61b38c), UINT32_C(0xbc66831a), UINT32_C(0x256fd2a0), UINT32_C(0x5268e236),
        UINT32_C(0xcc0c7795), UINT32_C(0xbb0b4703), UINT32_C(0x220216b9), UINT32_C(0x5505262f),
        UINT32_C(0xc5ba3bbe), UINT32_C(0xb2bd0b28), UINT32_C(0x2bb45a92), UINT32_C(0x5cb36a04),
        UINT32_C(0xc2d7ffa7), UINT32_C(0xb5d0cf31), UINT32_C(0x2cd99e8b), UINT32_C(0x5bdeae1d),
        UINT32_C(0x9b64c2b0), UINT32_C(0xec63f226), UINT32_C(0x756aa39c), UINT32_C(0x026d930a),
        UINT32_C(0x9c0906a9), UINT32_C(0xeb0e363f), UINT32_C(0x72076785), UINT32_C(0x05005713),
        UINT32_C(0x95bf4a82), UINT32_C(0xe2b87a14), UINT32_C(0x7bb12bae), UINT32_C(0x0cb61b38),
        UINT32_C(0x92d28e9b), UINT32_C(0xe5d5be0d), UINT32_C(0x7cdcefb7), UINT32_C(0x0bdbdf21),
        UINT32_C(0x86d3d2d4), UINT32_C(0xf1d4e242), UINT32_C(0x68ddb3f8), UINT32_C(0x1fda836e),
        UINT32_C(0x81be16cd), UINT32_C(0xf6b9265b), UINT32_C(0x6fb077e1), UINT32_C(0x18b74777),
        UINT32_C(0x88085ae6), UINT32_C(0xff0f6a70), UINT32_C(0x66063bca), UINT32_C(0x11010b5c),
        UINT32_C(0x8f659eff), UINT32_C(0xf862ae69), UINT32_C(0x616bffd3), UINT32_C(0x166ccf45),
        UINT32_C(0xa00ae278), UINT32_C(0xd70dd2ee), UINT32_C(0x4e048354), UINT32_C(0x3903b3c2),
        UINT32_C(0xa7672661), UINT32_C(0xd06016f7), UINT32_C(0x4969474d), UINT32_C(0x3e6e77db),
        UINT32_C(0xaed16a4a), UINT32_C(0xd9d65adc), UINT32_C(0x40df0b66), UINT32_C(0x37d83bf0),
        UINT32_C(0xa9bcae53), UINT32_C(0xdebb9ec5), UINT32_C(0x47b2cf7f), UINT32_C(0x30b5ffe9),
        UINT32_C(0xbdbdf21c), UINT32_C(0xcabac28a), UINT32_C(0x53b39330), UINT32_C(0x24b4a3a6),
        UINT32_C(0xbad03605), UINT32_C(0xcdd70693), UINT32_C(0x54de5729), UINT32_C(0x23d967bf),
        UINT32_C(0xb3667a2e), UINT32_C(0xc4614ab8), UINT32_C(0x5d681b02), UINT32_C(0x2a6f2b94),
        UINT32_C(0xb40bbe37), UINT32_C(0xc30c8ea1), UINT32_C(0x5a05df1b), UINT32_C(0x2d02ef8d),
    };
    uint32_t c = UINT32_MAX;
    for (size_t i = 0; i < n; i++)
        c = (c >> 8) ^ table[(c ^ p[i]) & 0xffu];
    return ~c;
}
static ps_result chunk(FILE *f, uint32_t type, const unsigned char *p, uint32_t n) {
    unsigned char h[12];
    ps_put_u32(h, type);
    ps_put_u32(h + 4, n);
    ps_put_u32(h + 8, ps_crc32(p, n));
    return fwrite(h, 1, 12, f) == 12 && fwrite(p, 1, n, f) == n ? PS_OK : PS_IO;
}
static ps_result flush_file(FILE *f) {
    if (fflush(f))
        return PS_IO;
#ifdef _WIN32
    return _commit(_fileno(f)) ? PS_IO : PS_OK;
#else
    return fsync(fileno(f)) ? PS_IO : PS_OK;
#endif
}
ps_result ps_run_create(ps_run_writer *w, const char *path, const ps_context *c, const char *name) {
    if (!w || !path || !c || !name || !c->channel_count || c->channel_count > PS_MAX_CHANNELS)
        return PS_INVALID;
    uint32_t parameter_count = 0;
    if (c->struct_size >= offsetof(ps_context, parameters) + sizeof c->parameters)
        parameter_count = c->parameter_count;
    if (parameter_count && ps_parameter_finalize(c) != PS_OK)
        return PS_INVALID;
    memset(w, 0, sizeof *w);
    w->file = fopen(path, "wbx");
    if (!w->file)
        return PS_IO;
    w->channels = c->channel_count;
    unsigned char h[16] = {'P', 'S', 'R', 'U', 'N', '1', '7', '\n', 1, 0, 0, 0, 4, 3, 2, 1};
    char meta[8192];
    int n =
        snprintf(meta, sizeof meta, "format=1\napi=%u\nexperiment=%s\ndt_s=%.17g\nseed=%llu\n%s\n",
                 PS_API_VERSION, name, c->dt_s, (unsigned long long)c->seed, c->model_metadata);
    ps_result r = PS_OK;
    if (n < 0 || n >= (int)sizeof meta)
        r = PS_IO;
    for (uint32_t i = 0; r == PS_OK && i < parameter_count; i++) {
        const ps_parameter *p = &c->parameters[i];
        int extra = snprintf(meta + n, sizeof meta - (size_t)n,
                             "parameter.%s=%.17g\nparameter_default.%s=%.17g\n"
                             "parameter_min.%s=%.17g\nparameter_max.%s=%.17g\n",
                             p->name, p->value, p->name, p->default_value,
                             p->name, p->minimum, p->name, p->maximum);
        if (extra < 0 || extra >= (int)(sizeof meta - (size_t)n))
            r = PS_LIMIT;
        else
            n += extra;
        ps_parameter_unit unit;
        if (r==PS_OK && ps_parameter_unit_read(c,i,&unit)!=PS_OK) r=PS_INVALID;
        if (r==PS_OK && unit.declared) {
            extra=snprintf(meta+n,sizeof meta-(size_t)n,
                "parameter_unit.%s=%s\nparameter_scale.%s=%.17g\n"
                "parameter_dimension.%s=%d,%d,%d,%d,%d,%d,%d\n",
                p->name,unit.symbol,p->name,unit.scale,p->name,
                unit.dimension[0],unit.dimension[1],unit.dimension[2],unit.dimension[3],
                unit.dimension[4],unit.dimension[5],unit.dimension[6]);
            if(extra<0 || extra>=(int)(sizeof meta-(size_t)n)) r=PS_LIMIT;
            else n+=extra;
        }
    }
    if (r == PS_OK && (fwrite(h, 1, 16, w->file) != 16 ||
                       chunk(w->file, 1, (unsigned char *)meta, (uint32_t)n) != PS_OK))
        r = PS_IO;
    unsigned char schema[4 + PS_MAX_CHANNELS * 167];
    ps_put_u32(schema, c->channel_count);
    for (uint32_t i = 0; i < c->channel_count; i++) {
        unsigned char *p = schema + 4 + i * 167;
        memcpy(p, c->channels[i].name, 48);
        memcpy(p + 48, c->channels[i].unit, 16);
        memcpy(p + 64, c->channels[i].description, 96);
        memcpy(p + 160, c->channels[i].dimension, 7);
    }
    if (r == PS_OK)
        r = chunk(w->file, 2, schema, 4 + c->channel_count * 167);
    if (r == PS_OK)
        r = flush_file(w->file);
    if (r != PS_OK) {
        fclose(w->file);
        w->file = NULL;
    }
    return r;
}
ps_result ps_run_append(ps_run_writer *w, double t, const double *v) {
    if (!w || !w->file || !v || !isfinite(t))
        return PS_INVALID;
    unsigned char p[8 * (PS_MAX_CHANNELS + 1)];
    ps_put_f64(p, t);
    for (uint32_t i = 0; i < w->channels; i++) {
        if (!isfinite(v[i]))
            return PS_INVALID;
        ps_put_f64(p + 8 * (i + 1), v[i]);
    }
    ps_result r = chunk(w->file, 3, p, 8 * (w->channels + 1));
    if (r != PS_OK)
        return r;
    w->samples++;
    if (fflush(w->file))
        return PS_IO;
    return w->samples % 100 == 0 ? flush_file(w->file) : PS_OK;
}
ps_result ps_run_close(ps_run_writer *w) {
    if (!w || !w->file)
        return PS_INVALID;
    unsigned char p[8];
    ps_put_u32(p, (uint32_t)w->samples);
    ps_put_u32(p + 4, (uint32_t)(w->samples >> 32));
    ps_result r = chunk(w->file, 4, p, 8);
    if (flush_file(w->file) != PS_OK)
        r = PS_IO;
    if (fclose(w->file))
        r = PS_IO;
    w->file = NULL;
    return r;
}
ps_result ps_run_append_snapshot(ps_run_writer *w, const ps_context *c,
                                 const ps_scene *scene, bool paused) {
    if (!w || !w->file || !c || c->channel_count != w->channels)
        return PS_INVALID;
    unsigned char payload[PS_SNAPSHOT_MAX];
    ps_put_u32(payload, PS_SNAPSHOT_VERSION);
    size_t size = ps_snapshot_encode(payload + 4, c, scene, paused);
    if (!size || size > sizeof payload - 4)
        return PS_INVALID;
    ps_result result = chunk(w->file, 5, payload, (uint32_t)size + 4);
    if (result == PS_OK && fflush(w->file)) result = PS_IO;
    return result;
}
static ps_result read_chunk(FILE *f, uint32_t *type, unsigned char *p, uint32_t *size) {
    unsigned char h[12];
    if (fread(h, 1, 12, f) != 12)
        return ferror(f) ? PS_IO : PS_RECOVERED;
    *type = ps_get_u32(h);
    *size = ps_get_u32(h + 4);
    if (*size > 8192)
        return PS_CORRUPT;
    if (fread(p, 1, *size, f) != *size)
        return PS_RECOVERED;
    if (ps_crc32(p, *size) != ps_get_u32(h + 8))
        return PS_RECOVERED;
    return PS_OK;
}
ps_result ps_run_open(ps_run_reader *r, const char *path) {
    if (!r || !path)
        return PS_INVALID;
    memset(r, 0, sizeof *r);
    r->file = fopen(path, "rb");
    if (!r->file)
        return PS_IO;
    unsigned char h[16], p[8192];
    ps_result error = PS_CORRUPT;
    if (fread(h, 1, 16, r->file) != 16 || memcmp(h, "PSRUN17\n", 8))
        goto fail;
    if (ps_get_u32(h + 8) != 1 || ps_get_u32(h + 12) != 0x01020304) {
        error = PS_VERSION;
        goto fail;
    }
    uint32_t type, n;
    if (read_chunk(r->file, &type, p, &n) != PS_OK || type != 1 || n >= sizeof r->metadata)
        goto fail;
    memcpy(r->metadata, p, n);
    r->metadata[n] = 0;
    if (read_chunk(r->file, &type, p, &n) != PS_OK || type != 2 || n < 4)
        goto fail;
    r->channels = ps_get_u32(p);
    if (!r->channels || r->channels > PS_MAX_CHANNELS || n != 4 + 167 * r->channels)
        goto fail;
    for (uint32_t i = 0; i < r->channels; i++) {
        unsigned char *a = p + 4 + 167 * i;
        ps_channel *s = &r->schema[i];
        memcpy(s->name, a, 48);
        memcpy(s->unit, a + 48, 16);
        memcpy(s->description, a + 64, 96);
        memcpy(s->dimension, a + 160, 7);
        s->name[47] = s->unit[15] = s->description[95] = 0;
    }
    return PS_OK;
fail:
    fclose(r->file);
    r->file = NULL;
    return error;
}
static ps_result next_record(ps_run_reader *r, double *t, double *v, ps_snapshot *snapshot) {
    if (!r || !r->file || (!snapshot && (!t || !v)))
        return PS_INVALID;
    if (r->complete)
        return PS_EOF;
    for (;;) {
        uint32_t type, n;
        unsigned char p[8192];
        ps_result e = read_chunk(r->file, &type, p, &n);
        if (e != PS_OK)
            return e;
        if (type == 3) {
            if (n != 8 * (r->channels + 1))
                return PS_CORRUPT;
            double time = ps_get_f64(p), values[PS_MAX_CHANNELS];
            if (!isfinite(time))
                return PS_CORRUPT;
            for (uint32_t i = 0; i < r->channels; i++) {
                values[i] = ps_get_f64(p + 8 * (i + 1));
                if (!isfinite(values[i]))
                    return PS_CORRUPT;
            }
            r->samples++;
            if (!snapshot) {
                *t = time;
                memcpy(v, values, r->channels * sizeof *v);
                return PS_OK;
            }
        }
        if (type == 5 && snapshot) {
            if (n < 4 || (ps_get_u32(p)!=1 && ps_get_u32(p) != PS_SNAPSHOT_VERSION))
                return n < 4 ? PS_CORRUPT : PS_VERSION;
            ps_snapshot decoded = {0};
            if (!ps_snapshot_decode_version(ps_get_u32(p),p + 4, n - 4, &decoded.time, decoded.values,
                                    &decoded.count, &decoded.scene, &decoded.paused) ||
                decoded.count != r->channels)
                return PS_CORRUPT;
            *snapshot = decoded;
            return PS_OK;
        }
        if (type == 4) {
            if (n != 8 ||
                ((uint64_t)ps_get_u32(p) | ((uint64_t)ps_get_u32(p + 4) << 32)) != r->samples)
                return PS_CORRUPT;
            r->complete = true;
            return PS_EOF;
        }
        if (type == 1 || type == 2)
            return PS_CORRUPT;
    }
}
ps_result ps_run_next(ps_run_reader *r, double *t, double *v) {
    return next_record(r, t, v, NULL);
}
ps_result ps_run_snapshot_next(ps_run_reader *r, ps_snapshot *snapshot) {
    if (!snapshot) return PS_INVALID;
    return next_record(r, NULL, NULL, snapshot);
}
void ps_run_reader_close(ps_run_reader *r) {
    if (r && r->file) {
        fclose(r->file);
        r->file = NULL;
    }
}
static void csv_string(FILE *f, const char *s) {
    fputc('"', f);
    for (; *s; s++) {
        if (*s == '"')
            fputc('"', f);
        fputc(*s, f);
    }
    fputc('"', f);
}
ps_result ps_run_export_csv(const char *input, const char *output) {
    if (!input || !output || !strcmp(input, output))
        return PS_INVALID;
    ps_run_reader r;
    ps_result e = ps_run_open(&r, input);
    if (e != PS_OK)
        return e;
    FILE *f = fopen(output, "wx");
    if (!f) {
        ps_run_reader_close(&r);
        return PS_IO;
    }
    fputs("time [s]", f);
    for (uint32_t i = 0; i < r.channels; i++) {
        char title[80];
        snprintf(title, sizeof title, "%s [%s]", r.schema[i].name, r.schema[i].unit);
        fputc(',', f);
        csv_string(f, title);
    }
    fputc('\n', f);
    double t, v[PS_MAX_CHANNELS];
    while ((e = ps_run_next(&r, &t, v)) == PS_OK) {
        fprintf(f, "%.17g", t);
        for (uint32_t i = 0; i < r.channels; i++)
            fprintf(f, ",%.17g", v[i]);
        fputc('\n', f);
    }
    if (ferror(f))
        e = PS_IO;
    if (fclose(f))
        e = PS_IO;
    ps_run_reader_close(&r);
    return e == PS_EOF ? PS_OK : e;
}
