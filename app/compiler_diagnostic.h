#ifndef PHYSIM_COMPILER_DIAGNOSTIC_H
#define PHYSIM_COMPILER_DIAGNOSTIC_H
#include <stdbool.h>
#include "physim/log.h"
typedef struct {
    char source[4096], message[2048];
    int line, column;
    ps_log_level level;
} ps_compiler_diagnostic;
/* GCC/Clang file:line[:column]: and MSVC file(line[,column]): errors/warnings.
 * Owns all text; rejects invalid coordinates/oversized paths without changing out. */
bool ps_compiler_diagnostic_parse(const char *text, ps_compiler_diagnostic *out);
#endif
