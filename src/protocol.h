#ifndef PS_PROTOCOL_H
#define PS_PROTOCOL_H
#include "physim/data.h"
#define PS_WIRE_MAX PS_SNAPSHOT_MAX
#define PS_WIRE_HEADER 20
#define PS_WIRE_VERSION 3u
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
    PS_MSG_SPEED /* 8-byte little-endian double: 0 offline, 0.1..16 real-time multiplier. */
};
typedef struct {
    unsigned char data[PS_WIRE_MAX + PS_WIRE_HEADER];
    size_t used;
    uint32_t sequence;
} ps_wire_buffer;
/* Explicit little-endian frame: magic, version, type, payload length, sequence. */
size_t ps_wire_encode(unsigned char *out, uint32_t type, uint32_t sequence, const void *data,
                      uint32_t size);
int ps_wire_peek(ps_wire_buffer *buffer, uint32_t *type, const unsigned char **payload,
                 uint32_t *size);
/* peek: 1=complete, 0=partial, -1=invalid. Outputs change only on 1.
 * consume ignores oversized/incomplete lengths and invalid buffer bounds. */
void ps_wire_consume(ps_wire_buffer *buffer, uint32_t size);
#endif
