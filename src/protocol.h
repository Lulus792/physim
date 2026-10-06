#ifndef PS_PROTOCOL_H
#define PS_PROTOCOL_H
#include "physim/data.h"
#define PS_WIRE_MAX PS_SNAPSHOT_MAX
#define PS_WIRE_HEADER 20
#define PS_WIRE_VERSION 5u
#define PS_WIRE_OBJECT_SIZE PS_SNAPSHOT_OBJECT_SIZE
enum {
    PS_MSG_HELLO = 1,
    PS_MSG_RUN,
    PS_MSG_PAUSE,
    PS_MSG_STEP,
    PS_MSG_STOP,
    PS_MSG_SNAPSHOT,
    PS_MSG_ERROR,
    PS_MSG_BYE,
    PS_MSG_HEARTBEAT,
    PS_MSG_SPEED, /* 8-byte little-endian double: 0 offline, 0.1..16 real-time multiplier. */
    PS_MSG_LOG, /* Explicit --log-events opt-in. */
    PS_MSG_DIAGNOSTIC /* Versioned diagnostic payload; explicit --diagnostics opt-in. */
};
typedef struct {
    unsigned char data[PS_WIRE_MAX + PS_WIRE_HEADER];
    size_t used;
    uint32_t sequence;
} ps_wire_buffer;
size_t ps_wire_log_encode(unsigned char *out,size_t capacity,const ps_log_record *record);
bool ps_wire_log_decode(const unsigned char *data,size_t size,ps_log_record *out);
/* Explicit little-endian frame: magic, version, type, payload length, sequence. */
size_t ps_wire_encode(unsigned char *out, uint32_t type, uint32_t sequence, const void *data,
                      uint32_t size);
int ps_wire_peek(ps_wire_buffer *buffer, uint32_t *type, const unsigned char **payload,
                 uint32_t *size);
/* peek: 1=complete, 0=partial, -1=invalid. Outputs change only on 1.
 * consume ignores oversized/incomplete lengths and invalid buffer bounds. */
void ps_wire_consume(ps_wire_buffer *buffer, uint32_t size);
#endif
