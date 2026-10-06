#ifndef PHYSIM_LOG_H
#define PHYSIM_LOG_H
#include "core.h"
#define PS_LOG_MESSAGE_MAX 1024u
typedef enum { PS_LOG_DEBUG=1, PS_LOG_INFO, PS_LOG_WARNING, PS_LOG_ERROR } ps_log_level;
typedef struct {
    ps_log_level level;
    double time_s;
    char message[PS_LOG_MESSAGE_MAX+1u];
} ps_log_record;
/* Explicit synchronous sink; no global state or implicit stdout/stderr output.
 * The borrowed record exists only during write. Sink/user must outlive the
 * logger, and calls require external synchronization. NULL write disables it. */
typedef struct {
    void *user;
    ps_result (*write)(void *user,const ps_log_record *record);
} ps_logger;
/* Finite logical time, bounded terminated UTF-8. Newline/CR/tab are permitted;
 * other control characters, empty messages and unknown levels are invalid. */
bool ps_log_record_valid(const ps_log_record *record);
/* Validate before invoking the sink. A disabled sink succeeds without I/O.
 * Failures from the sink propagate; the caller chooses whether they are fatal. */
ps_result ps_logger_emit(const ps_logger *logger,ps_log_level level,double time_s,
                          const char *message);
const char *ps_log_level_name(ps_log_level level);
#endif
