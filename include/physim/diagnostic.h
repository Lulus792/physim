#ifndef PHYSIM_DIAGNOSTIC_H
#define PHYSIM_DIAGNOSTIC_H
#include "core.h"
#define PS_DIAGNOSTIC_VERSION 1u
#define PS_DIAGNOSTIC_LABEL_MAX 64u
#define PS_DIAGNOSTIC_SOURCE_MAX 1024u
#define PS_DIAGNOSTIC_MESSAGE_MAX 1024u
#define PS_DIAGNOSTIC_WIRE_MAX 2216u
/* Owned UTF-8, no global last-error state. Line/column are 1-based; zero means
 * unknown. Column requires a line; coordinates require a source. Messages may
 * contain LF/CR/tab, other fields have no controls. Empty labels are allowed. */
typedef struct {
    uint32_t struct_size, version;
    ps_result code;
    uint32_t line, column;
    char operation[PS_DIAGNOSTIC_LABEL_MAX+1u], argument[PS_DIAGNOSTIC_LABEL_MAX+1u];
    char source[PS_DIAGNOSTIC_SOURCE_MAX+1u], message[PS_DIAGNOSTIC_MESSAGE_MAX+1u];
} ps_diagnostic;
/* Clear is a valid PS_OK record with empty fields. Failure records exclude
 * PS_OK, PS_EOF and PS_RECOVERED. All copies remain independent. */
void ps_diagnostic_clear(ps_diagnostic *diagnostic);
bool ps_diagnostic_valid(const ps_diagnostic *diagnostic);
/* Copy synchronously; NULL optional labels/source mean empty. Message required.
 * Invalid input leaves output unchanged. Returns PS_OK after storing the error;
 * the stored code is independent of this constructor's result. */
ps_result ps_diagnostic_set(ps_diagnostic *out, ps_result code, const char *operation,
                            const char *argument, const char *source, uint32_t line,
                            uint32_t column, const char *message);
/* NUL-terminated UTF-8, bounded truncation; PS_LIMIT if shortened. A clear record
 * formats to an empty string. Invalid arguments preserve the output. */
ps_result ps_diagnostic_format(const ps_diagnostic *diagnostic, char *out, size_t capacity);
/* Versioned little-endian payload with exact lengths and CRC. Encode returns 0
 * for clear/invalid records or insufficient capacity; decode is transactional. */
size_t ps_diagnostic_encode(unsigned char *out, size_t capacity, const ps_diagnostic *diagnostic);
ps_result ps_diagnostic_decode(const unsigned char *data, size_t size, ps_diagnostic *out);
/* Exclusive binary sidecar. No overwrite; load validates complete file/CRC and
 * leaves output unchanged on error. This is not a power-loss durability guarantee. */
ps_result ps_diagnostic_save(const char *path, const ps_diagnostic *diagnostic);
ps_result ps_diagnostic_load(const char *path, ps_diagnostic *out);
#endif
