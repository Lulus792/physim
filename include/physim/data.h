#ifndef PHYSIM_DATA_H
#define PHYSIM_DATA_H
#include "experiment.h"
#include "snapshot.h"
#include <stdio.h>
typedef struct { char project_id[33],experiment_id[33]; } ps_run_identity;
/* Both identities are exactly 32 lower-case hexadecimal bytes plus NUL.
 * Construction/decoding preserves output on failure. No allocation or RNG.
 * Decode PS_EOF means absent/unavailable identity; malformed/duplicate fields
 * return PS_CORRUPT. Metadata is an explicit bounded byte span. */
ps_result ps_run_identity_make(const char *project_id,const char *experiment_id,ps_run_identity *out);
ps_result ps_run_identity_validate(const ps_run_identity *identity);
ps_result ps_run_identity_read(const char *metadata,size_t length,ps_run_identity *out);
/* System wall-clock UTC/POSIX epoch (1970-01-01), not simulation/monotonic time.
 * Nanoseconds are 0..999999999; representation does not promise clock precision.
 * Output is preserved on all failures. Missing archive fields return PS_EOF. */
typedef struct { int64_t unix_seconds; uint32_t nanoseconds; } ps_run_start_time;
ps_result ps_run_start_time_make(int64_t unix_seconds,uint32_t nanoseconds,ps_run_start_time *out);
ps_result ps_run_start_time_now(ps_run_start_time *out);
ps_result ps_run_start_time_validate(const ps_run_start_time *time);
ps_result ps_run_start_time_read(const char *metadata,size_t length,ps_run_start_time *out);
typedef struct {
    FILE *file;
    uint32_t channels;
    uint64_t samples;
} ps_run_writer;
typedef struct {
    FILE *file;
    uint32_t channels;
    uint64_t samples;
    bool complete;
    char metadata[8192];
    ps_channel schema[PS_MAX_CHANNELS];
} ps_run_reader;
/* Open exclusively: existing runs are never overwritten. Close all successful opens. */
ps_result ps_run_create(ps_run_writer *writer, const char *path, const ps_context *context,
                        const char *experiment_name);
/* Typed host identity. Model metadata must not declare either host ID field.
 * Validation/collision failures create no file and preserve the writer. */
ps_result ps_run_create_with_identity(ps_run_writer *writer,const char *path,const ps_context *context,
                                      const char *experiment_name,const ps_run_identity *identity);
ps_result ps_run_append(ps_run_writer *writer, double time_s, const double *values);
/* Optional event chunk: validated level, logical time and UTF-8 message.
 * Does not change measurement counts. Invalid records do not write bytes. */
ps_result ps_run_append_event(ps_run_writer *writer, const ps_log_record *event);
/* Optional versioned scene chunk. Does not add a measurement row or change its
 * footer count. Existing readers skip it after CRC validation. */
ps_result ps_run_append_snapshot(ps_run_writer *writer, const ps_context *context,
                                const ps_scene *scene, bool paused);
/* Writes bounded CRC-protected index pages and the existing footer. Validates
 * the recorded prefix once at finalization; no allocation. Previous readers
 * skip index chunks. Failure closes the file without a successful footer. */
ps_result ps_run_close(ps_run_writer *writer);
ps_result ps_run_open(ps_run_reader *reader, const char *path);
/* Streaming read: PS_EOF = finalized run; PS_RECOVERED = incomplete/corrupt tail.
 * Time and values change only on PS_OK, after the entire sample is validated.
 * Stop reading on any other result; the file cursor may already have advanced. */
ps_result ps_run_next(ps_run_reader *reader, double *time_s, double values[PS_MAX_CHANNELS]);
/* Use a separate reader to stream snapshots. Measurement rows are validated and
 * counted while scanning; PS_EOF/PS_RECOVERED have the same meanings as above.
 * Legacy runs contain no snapshots. Failed reads leave snapshot unchanged. */
ps_result ps_run_snapshot_next(ps_run_reader *reader, ps_snapshot *snapshot);
/* Use a separate reader for events. Validates/counts skipped samples;
 * failure preserves the event output. */
ps_result ps_run_event_next(ps_run_reader *reader, ps_log_record *event);
void ps_run_reader_close(ps_run_reader *reader);
/* Optional measurement convention: <name>.status masks <name> and <name>.u.
 * States are 0=not due, 1=valid, 2=dropped. Returns index=-1 when absent;
 * rejects malformed/ambiguous names or a status channel with physical dimensions.
 * Raw files/CSV and Series retain every row; consumers must apply this mask. */
ps_result ps_channel_status_index(const ps_channel *schema, uint32_t count, uint32_t channel,
                                  int *index);
ps_result ps_run_export_csv(const char *input, const char *output);
/* CRC-32: reflected polynomial 0xedb88320, initial/final XOR 0xffffffff.
 * No allocation or mutable state. data may be NULL only when size is zero. */
uint32_t ps_crc32(const unsigned char *data, size_t size);
void ps_put_u32(unsigned char *out, uint32_t value);
uint32_t ps_get_u32(const unsigned char *in);
void ps_put_f64(unsigned char *out, double value);
double ps_get_f64(const unsigned char *in);
#endif
