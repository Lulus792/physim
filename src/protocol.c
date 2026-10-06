#include "protocol.h"
#include <math.h>
#include <string.h>
size_t ps_wire_encode(unsigned char *p, uint32_t type, uint32_t seq, const void *data, uint32_t n) {
    if (!p || n > PS_WIRE_MAX || (!data && n))
        return 0;
    ps_put_u32(p, 0x5053494d);
    ps_put_u32(p + 4, PS_WIRE_VERSION);
    ps_put_u32(p + 8, type);
    ps_put_u32(p + 12, n);
    ps_put_u32(p + 16, seq);
    if (n)
        memcpy(p + 20, data, n);
    return 20 + n;
}
int ps_wire_peek(ps_wire_buffer *b, uint32_t *type, const unsigned char **payload, uint32_t *n) {
    if (!b || !type || !payload || !n || b->used > sizeof b->data)
        return -1;
    if (b->used < 20)
        return 0;
    if (ps_get_u32(b->data) != 0x5053494d || ps_get_u32(b->data + 4) != PS_WIRE_VERSION)
        return -1;
    uint32_t length = ps_get_u32(b->data + 12);
    if (length > PS_WIRE_MAX)
        return -1;
    if (b->used < 20 + length)
        return 0;
    if (ps_get_u32(b->data + 16) != b->sequence)
        return -1;
    *type = ps_get_u32(b->data + 8);
    *payload = b->data + 20;
    *n = length;
    return 1;
}
void ps_wire_consume(ps_wire_buffer *b, uint32_t n) {
    if (!b || b->used > sizeof b->data || n > PS_WIRE_MAX || b->used < PS_WIRE_HEADER ||
        n > b->used - PS_WIRE_HEADER)
        return;
    b->used -= 20 + n;
    memmove(b->data, b->data + 20 + n, b->used);
    b->sequence++;
}

size_t ps_wire_log_encode(unsigned char *out,size_t capacity,const ps_log_record *record) {
    if(!out || !ps_log_record_valid(record))return 0;
    size_t n=strlen(record->message);if(capacity<12+n)return 0;
    ps_put_u32(out,(uint32_t)record->level);ps_put_f64(out+4,record->time_s);
    memcpy(out+12,record->message,n);return 12+n;
}
bool ps_wire_log_decode(const unsigned char *data,size_t size,ps_log_record *out) {
    if(!data || !out || size<=12 || size>12+PS_LOG_MESSAGE_MAX || memchr(data+12,0,size-12))return false;
    ps_log_record record={0};record.level=(ps_log_level)ps_get_u32(data);record.time_s=ps_get_f64(data+4);
    memcpy(record.message,data+12,size-12);
    if(!ps_log_record_valid(&record))return false;
    *out=record;return true;
}
