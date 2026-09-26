#ifndef PHYSIM_REPORT_INTERNAL_H
#define PHYSIM_REPORT_INTERNAL_H
#include "physim/report.h"
/* Private bridge for exporters; not part of the public SDK API. */
ps_allocator ps_report_allocator_internal(const ps_report *report);
#endif
