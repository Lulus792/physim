#ifndef PS_RUNNER_OUTPUT_H
#define PS_RUNNER_OUTPUT_H
#include <stdbool.h>
#include <stdint.h>
/* Private process-lifetime transport. One producer, one dedicated stdout writer.
 * At most 8 MiB queued, one replaceable preview and one queued heartbeat.
 * Reliable frames are FIFO; sequence numbers are assigned only when writing.
 * Queue exhaustion/pipe failure are explicit errors, never successful drops.
 * Preview replacement never changes the independently written run file. */
typedef struct ps_runner_output ps_runner_output;
ps_runner_output *ps_runner_output_create(void);
bool ps_runner_output_send(ps_runner_output *output,uint32_t type,const void *payload,uint32_t size,bool preview);
bool ps_runner_output_healthy(ps_runner_output *output);
/* Close, drain and join within timeout. Frees a completed writer, including on
 * pipe failure. On timeout its memory remains owned by the live thread; caller
 * must terminate the process, never free/unload this transport underneath it. */
bool ps_runner_output_finish(ps_runner_output *output,double timeout);
#endif
