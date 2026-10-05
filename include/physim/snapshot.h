#ifndef PHYSIM_SNAPSHOT_H
#define PHYSIM_SNAPSHOT_H
#include "experiment.h"
#define PS_SNAPSHOT_VERSION 2u
#define PS_SNAPSHOT_MAX 8192u
#define PS_SNAPSHOT_HEADER 24u
#define PS_SNAPSHOT_OBJECT_SIZE 176u
/* Immutable physical state shared by IPC and optional run-file scene chunks. */
typedef struct {
    double time, values[PS_MAX_CHANNELS];
    uint32_t count;
    ps_scene scene;
    bool paused;
} ps_snapshot;
/* out provides PS_SNAPSHOT_MAX bytes. Returns encoded length, or 0 for invalid
 * input. Time and all channel values must be finite; scene must be valid. */
size_t ps_snapshot_encode(unsigned char *out, const ps_context *context,
                          const ps_scene *scene, bool paused);
/* Failure preserves all outputs. Provide space for PS_MAX_CHANNELS values. */
bool ps_snapshot_decode(const unsigned char *in, uint32_t size, double *time, double *values,
                        uint32_t *count, ps_scene *scene, bool *paused);
/* Version 1 restores a flat scene with parent_id=0. Version 2 includes hierarchy.
 * The declared version determines the exact payload size; unknown versions fail. */
bool ps_snapshot_decode_version(uint32_t version, const unsigned char *in, uint32_t size,
                                double *time, double *values, uint32_t *count,
                                ps_scene *scene, bool *paused);
#endif
