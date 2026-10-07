#ifndef PS_BATCH_H
#define PS_BATCH_H
#include "physim/data.h"
#include "physim/experiment.h"
#include "physim/report.h"

#include "physim/batch.h"
/* Called on the invoking thread; false requests cancellation of the entire pool.
 * completed counts fully validated, journaled runs; active counts live children
 * observed by the controller. Partial raw files remain, but only a complete
 * successful series receives a report. Results/aggregates use fixed index order. */
typedef bool (*ps_batch_continue)(uint32_t completed, uint32_t active, void *user);
ps_result ps_batch_validate(const ps_batch_options *options);
/* Load immutable saved options and archived module/source for continuation into
 * a new directory. Runner must match the saved fingerprint. Outputs unchanged
 * on failure. Versions lacking a checkpoint cannot be resumed. */
ps_result ps_batch_resume_load(const char *series,const char *runner,const char *new_directory,
                               ps_batch_options *out);
ps_result ps_batch_run(const ps_batch_options *options, ps_batch_continue proceed, void *user,
                       ps_batch_result *result);
/* Final-value statistics, Type-7 quantiles. Approximate normal mean CI only for
 * n >= 200, assuming independent samples and a well-behaved finite variance. */
ps_result ps_batch_report(const double *values, uint32_t count, const ps_channel *channel,
                          const char *provenance, ps_report **out);
#endif
