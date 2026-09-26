#ifndef PHYSIM_REPORT_IMAGE_H
#define PHYSIM_REPORT_IMAGE_H
#include "physim/report.h"
struct nk_context;
struct nk_user_font;
/* Full report plot, independent of application viewport and interactive zoom. */
ps_result ps_report_image(struct nk_context *ui, const struct nk_user_font *font,
                          const ps_report *report, uint32_t plot, const char *path);
/* Same 1200x850 logical layout; scale 1..4 controls exact PNG pixel dimensions.
 * The legacy entry point above uses scale=2. Invalid scale creates no file. */
ps_result ps_report_image_scaled(struct nk_context *ui, const struct nk_user_font *font,
                                 const ps_report *report, uint32_t plot, unsigned scale,
                                 const char *path);
/* Explicit finite, increasing axis bounds {xmin,xmax,ymin,ymax}; NULL = full plot. */
ps_result ps_report_image_region(struct nk_context *ui, const struct nk_user_font *font,
                                 const ps_report *report, uint32_t plot, unsigned scale,
                                 const double region[4], const char *path);
#endif
