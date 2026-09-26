#ifndef PHYSIM_DATA_H
#define PHYSIM_DATA_H
#include "experiment.h"
#include <stdio.h>
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
ps_result ps_run_append(ps_run_writer *writer, double time_s, const double *values);
ps_result ps_run_close(ps_run_writer *writer);
ps_result ps_run_open(ps_run_reader *reader, const char *path);
/* Streaming read: PS_EOF = finalized run; PS_RECOVERED = incomplete/corrupt tail.
 * Time and values change only on PS_OK, after the entire sample is validated.
 * Stop reading on any other result; the file cursor may already have advanced. */
ps_result ps_run_next(ps_run_reader *reader, double *time_s, double values[PS_MAX_CHANNELS]);
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
