#ifndef PS_CHANNEL_UNITS_H
#define PS_CHANNEL_UNITS_H
#include "physim/experiment.h"
#define PS_CHANNEL_UNIT_MAX 64u
typedef struct { int8_t dimension[7]; double scale; char symbol[64]; } ps_display_unit;
typedef struct { char name[48]; ps_display_unit unit; } ps_channel_unit_entry;
typedef struct { uint32_t count; ps_channel_unit_entry entries[PS_CHANNEL_UNIT_MAX]; } ps_channel_units;
bool ps_display_unit_valid(const ps_display_unit *unit);
bool ps_channel_units_valid(const ps_channel_units *units);
/* Channel name AND dimensions identify a choice. Default values are SI with
 * the channel's original symbol. All mutations preserve output on error. */
ps_result ps_channel_units_get(const ps_channel_units *units,const ps_channel *channel,ps_display_unit *out);
ps_result ps_channel_units_put(ps_channel_units *units,const ps_channel *channel,const ps_display_unit *unit);
ps_result ps_channel_units_remove(ps_channel_units *units,const ps_channel *channel);
/* Scale is SI units per display unit. Reject overflow and nonzero underflow;
 * preserve signed zero. Units are linear; offsets are not supported. */
ps_result ps_display_unit_value(const ps_display_unit *unit,double si_value,double *out);
ps_result ps_channel_units_read(const char *path,ps_channel_units *out);
ps_result ps_channel_units_write(const char *path,const ps_channel_units *units);
#endif
