#ifndef PS_RUN_IMPORT_H
#define PS_RUN_IMPORT_H
#include "physim/core.h"
/* Copy and validate a run, including snapshots and strictly increasing times.
 * Publish without replacing any destination file. Source files are read-only.
 * Optional experiment sources and resource-limit sidecars travel with the run.
 * PS_RECOVERED publishes a validated incomplete run. On failure remove only
 * artifacts created by this call. Memory usage is independent of run length. */
ps_result ps_run_import(const char *source,const char *destination);
#endif
