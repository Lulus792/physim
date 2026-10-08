#ifndef PS_PLATFORM_H
#define PS_PLATFORM_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
typedef struct {
    void *handle, *input, *output, *job;
    int pid, exit_code;
    bool running;
    double deadline;
    bool timed_out;
} ps_process;
typedef struct {
    uint64_t memory_bytes; /* 0 disables. Windows: job commit; Linux: per-process RLIMIT_AS. */
    double wall_seconds;  /* 0 disables; finite and nonnegative, includes waiting/pauses. */
} ps_process_limits;
/* argv includes executable in argv[0], ends with NULL. No shell interpretation. */
bool ps_process_start(ps_process *p, const char *const *argv, const char *directory);
/* p must not own a running process/handles. Limits are installed before user code
 * executes. The parent must poll regularly for the wall deadline; timeout kills
 * the process group/job and reports timed_out=true, exit_code=124. Linux child
 * setup failure exits 125 without exec. These limits are not a security sandbox. */
bool ps_process_start_limited(ps_process *p, const char *const *argv, const char *directory,
                               const ps_process_limits *limits);
int ps_process_read(ps_process *p, void *buffer, size_t capacity);
bool ps_process_write(ps_process *p, const void *data, size_t size);
bool ps_process_poll(ps_process *p);
void ps_process_kill(ps_process *p);
void ps_process_close(ps_process *p);
void *ps_module_open(const char *path);
void *ps_module_symbol(void *module, const char *name);
void ps_module_close(void *module);
double ps_clock(void);
/* Current process, all threads; excludes children. CPU is cumulative user/system
 * time, peak resident bytes are the lifetime high-water mark, not current RAM or
 * a phase-local allocation count. No file access or allocation. Failure preserves
 * out. Windows working set / POSIX ru_maxrss have OS-specific accounting. */
typedef struct {
    double user_seconds, system_seconds;
    uint64_t peak_resident_bytes;
} ps_process_usage;
bool ps_process_usage_self(ps_process_usage *out);
void ps_sleep(unsigned ms);
int ps_stdin_read(void *buffer, size_t capacity);
void ps_binary_stdio(void);
/* Start a detached watchdog for an offline child with a dedicated stdin pipe.
 * Pipe EOF/error exits the entire process with 125, even while model callbacks
 * are blocked. Call once before loading user code; not for interactive input.
 * This is parent-lifetime protection, not an adversarial security boundary. */
bool ps_parent_watch_start(void);
bool ps_make_directory(const char *path);
bool ps_make_directory_exclusive(const char *path);
bool ps_executable_path(char *out, size_t capacity);
#endif
