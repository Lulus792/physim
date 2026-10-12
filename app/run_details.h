#ifndef PHYSIM_RUN_DETAILS_H
#define PHYSIM_RUN_DETAILS_H
#include "physim/core.h"
#define PS_RUN_DETAILS_TEXT_CAPACITY 32768u
/* Format archive-declared model layers, assumptions, provenance and parameters.
 * Explicit byte span, bounded UTF-8. Missing fields remain undocumented.
 * Known layer.<name> = active|detail, ideal|detail, omitted|detail.
 * Unknown fields are retained by the caller's raw metadata view.
 * Errors preserve the output; this function never opens paths or evaluates text. */
ps_result ps_run_details_format(const char *metadata,size_t length,char *out,size_t capacity);
#endif
