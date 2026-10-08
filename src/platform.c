#ifdef __APPLE__
/* ru_maxrss is a Darwin extension hidden by the build's POSIX feature level. */
#define _DARWIN_C_SOURCE
#endif
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
bool ps_process_start(ps_process *p, const char *const *argv, const char *dir) {
    return ps_process_start_limited(p, argv, dir, NULL);
}
static bool prepare_process(ps_process *p, const char *const *argv,
                            const ps_process_limits *limits, ps_process_limits *out) {
    if (!p)
        return false;
    memset(p, 0, sizeof *p);
    *out = limits ? *limits : (ps_process_limits){0};
    if (!argv || !argv[0] || !argv[0][0] || !isfinite(out->wall_seconds) ||
        out->wall_seconds < 0 || out->memory_bytes > SIZE_MAX)
        return false;
    if (out->wall_seconds) {
        p->deadline = ps_clock() + out->wall_seconds;
        if (!isfinite(p->deadline))
            return false;
    }
    return true;
}
static bool process_deadline(ps_process *p) {
    if (p->deadline && ps_clock() >= p->deadline) {
        double deadline = p->deadline;
        p->deadline = 0; /* kill may poll; do not recursively enforce this deadline. */
        ps_process_kill(p);
        p->deadline = deadline;
        p->timed_out = true;
        p->exit_code = 124;
    }
    return p->running;
}
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <direct.h>
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#ifndef PSAPI_VERSION
#define PSAPI_VERSION 2
#endif
#include <psapi.h>
bool ps_process_usage_self(ps_process_usage *out) {
    if (!out) return false;
    FILETIME created, exited, kernel, user;
    PROCESS_MEMORY_COUNTERS memory = {0};
    memory.cb = sizeof memory;
    if (!GetProcessTimes(GetCurrentProcess(), &created, &exited, &kernel, &user) ||
        !K32GetProcessMemoryInfo(GetCurrentProcess(), &memory, sizeof memory))
        return false;
    ULARGE_INTEGER u, k;
    u.LowPart = user.dwLowDateTime; u.HighPart = user.dwHighDateTime;
    k.LowPart = kernel.dwLowDateTime; k.HighPart = kernel.dwHighDateTime;
    *out = (ps_process_usage){(double)u.QuadPart / 1e7, (double)k.QuadPart / 1e7,
                              (uint64_t)memory.PeakWorkingSetSize};
    return true;
}
static wchar_t *wide(const char *s) {
    int n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s, -1, NULL, 0);
    if (!n)
        return NULL;
    wchar_t *w = malloc((size_t)n * sizeof *w);
    if (w)
        MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s, -1, w, n);
    return w;
}
bool ps_process_start_limited(ps_process *p, const char *const *argv, const char *dir,
                               const ps_process_limits *requested) {
    ps_process_limits requested_limits;
    if (!prepare_process(p, argv, requested, &requested_limits))
        return false;
    SECURITY_ATTRIBUTES sa = {sizeof sa, NULL, TRUE};
    HANDLE in_r = NULL, in_w = NULL, out_r = NULL, out_w = NULL;
    if (!CreatePipe(&in_r, &in_w, &sa, 0) || !CreatePipe(&out_r, &out_w, &sa, 0))
        goto fail;
    SetHandleInformation(in_w, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(out_r, HANDLE_FLAG_INHERIT, 0);
    /* Windows CRT quoting: double backslashes preceding a quote or closing quote. */
    char cmd[32768];
    size_t k = 0;
    for (int i = 0; argv[i]; i++) {
        if (k + 4 >= sizeof cmd)
            goto fail;
        if (i)
            cmd[k++] = ' ';
        cmd[k++] = '"';
        const char *s = argv[i];
        while (*s) {
            size_t slashes = 0;
            while (*s == '\\') {
                slashes++;
                s++;
            }
            size_t n = (*s == '"' || !*s) ? slashes * 2 : slashes;
            if (k + n + 3 >= sizeof cmd)
                goto fail;
            while (n--)
                cmd[k++] = '\\';
            if (*s == '"')
                cmd[k++] = '\\';
            if (*s)
                cmd[k++] = *s++;
        }
        cmd[k++] = '"';
    }
    cmd[k] = 0;
    wchar_t *command = wide(cmd), *directory = dir ? wide(dir) : NULL;
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    memset(&si, 0, sizeof si);
    memset(&pi, 0, sizeof pi);
    si.cb = sizeof si;
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = in_r;
    si.hStdOutput = out_w;
    si.hStdError = out_w;
    BOOL ok = command && (!dir || directory) &&
              CreateProcessW(NULL, command, NULL, NULL, TRUE, CREATE_NO_WINDOW | CREATE_SUSPENDED,
                             NULL, directory, &si, &pi);
    free(command);
    free(directory);
    if (!ok)
        goto fail;
    HANDLE job = CreateJobObjectW(NULL, NULL);
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits;
    memset(&limits, 0, sizeof limits);
    limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if (requested_limits.memory_bytes) {
        limits.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_JOB_MEMORY;
        limits.JobMemoryLimit = (SIZE_T)requested_limits.memory_bytes;
    }
    if (!job ||
        !SetInformationJobObject(job, JobObjectExtendedLimitInformation, &limits, sizeof limits) ||
        !AssignProcessToJobObject(job, pi.hProcess)) {
        TerminateProcess(pi.hProcess, 126);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        if (job)
            CloseHandle(job);
        goto fail;
    }
    if (ResumeThread(pi.hThread) == (DWORD)-1) {
        TerminateJobObject(job, 126);
        CloseHandle(job);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        goto fail;
    }
    p->job = job;
    CloseHandle(in_r);
    CloseHandle(out_w);
    CloseHandle(pi.hThread);
    p->handle = pi.hProcess;
    p->input = in_w;
    p->output = out_r;
    p->running = true;
    return true;
fail:
    if (in_r)
        CloseHandle(in_r);
    if (in_w)
        CloseHandle(in_w);
    if (out_r)
        CloseHandle(out_r);
    if (out_w)
        CloseHandle(out_w);
    return false;
}
int ps_process_read(ps_process *p, void *b, size_t n) {
    DWORD available = 0, got = 0;
    if (!p->output || !PeekNamedPipe(p->output, NULL, 0, NULL, &available, NULL))
        return -1;
    if (!available)
        return 0;
    if (n > available)
        n = available;
    return ReadFile(p->output, b, (DWORD)n, &got, NULL) ? (int)got : -1;
}
bool ps_process_write(ps_process *p, const void *b, size_t n) {
    DWORD done = 0;
    return p->input && WriteFile(p->input, b, (DWORD)n, &done, NULL) && done == n;
}
bool ps_process_poll(ps_process *p) {
    if (!p->running)
        return false;
    if (WaitForSingleObject(p->handle, 0) == WAIT_TIMEOUT)
        return process_deadline(p);
    DWORD code = 0;
    GetExitCodeProcess(p->handle, &code);
    p->exit_code = (int)code;
    p->running = false;
    return false;
}
void ps_process_kill(ps_process *p) {
    if (p->running) {
        if (p->job)
            TerminateJobObject(p->job, 137);
        else
            TerminateProcess(p->handle, 137);
        WaitForSingleObject(p->handle, 2000);
        ps_process_poll(p);
    }
}
void ps_process_close(ps_process *p) {
    if (p->running)
        ps_process_kill(p);
    if (p->handle)
        CloseHandle(p->handle);
    if (p->input)
        CloseHandle(p->input);
    if (p->output)
        CloseHandle(p->output);
    if (p->job)
        CloseHandle(p->job);
    p->handle = p->input = p->output = p->job = NULL;
}
void *ps_module_open(const char *path) {
    wchar_t *p = wide(path);
    void *m = p ? (void *)LoadLibraryW(p) : NULL;
    free(p);
    return m;
}
void *ps_module_symbol(void *m, const char *s) { return (void *)GetProcAddress((HMODULE)m, s); }
void ps_module_close(void *m) {
    if (m)
        FreeLibrary((HMODULE)m);
}
double ps_clock(void) {
    LARGE_INTEGER n, f;
    QueryPerformanceCounter(&n);
    QueryPerformanceFrequency(&f);
    return (double)n.QuadPart / (double)f.QuadPart;
}
void ps_sleep(unsigned ms) { Sleep(ms); }
int ps_stdin_read(void *b, size_t n) {
    HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
    DWORD available = 0, got = 0;
    if (!PeekNamedPipe(h, NULL, 0, NULL, &available, NULL))
        return -1;
    if (!available)
        return 0;
    if (n > available)
        n = available;
    return ReadFile(h, b, (DWORD)n, &got, NULL) ? (int)got : -1;
}
void ps_binary_stdio(void) {
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
}
static DWORD WINAPI parent_watch(void *unused) {
    (void)unused;
    HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
    unsigned char buffer[64];DWORD got;
    while (ReadFile(input,buffer,sizeof buffer,&got,NULL) && got) {}
    _Exit(125);
}
bool ps_parent_watch_start(void) {
    if(GetFileType(GetStdHandle(STD_INPUT_HANDLE))!=FILE_TYPE_PIPE)return false;
    HANDLE thread=CreateThread(NULL,65536,parent_watch,NULL,0,NULL);
    if(!thread)return false;
    CloseHandle(thread);return true;
}
bool ps_make_directory(const char *path) {
    wchar_t *p = wide(path);
    if (!p)
        return false;
    BOOL ok = CreateDirectoryW(p, NULL);
    DWORD e = GetLastError();
    free(p);
    return ok || e == ERROR_ALREADY_EXISTS;
}
bool ps_make_directory_exclusive(const char *path) {
    wchar_t *p = wide(path);
    if (!p)
        return false;
    BOOL ok = CreateDirectoryW(p, NULL);
    free(p);
    return ok != 0;
}
bool ps_executable_path(char *out, size_t cap) {
    wchar_t p[32768];
    DWORD n = GetModuleFileNameW(NULL, p, 32768);
    return n && n < 32768 && WideCharToMultiByte(CP_UTF8, 0, p, -1, out, (int)cap, NULL, NULL) > 0;
}
#else
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <poll.h>
#include <pthread.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif
static int fd(void *p) { return (int)(intptr_t)p - 1; }
static void *handle(int f) { return (void *)(intptr_t)(f + 1); }
bool ps_process_start_limited(ps_process *p, const char *const *argv, const char *dir,
                               const ps_process_limits *requested) {
    ps_process_limits limits;
    if (!prepare_process(p, argv, requested, &limits))
        return false;
    if (limits.memory_bytes &&
        ((uint64_t)(rlim_t)limits.memory_bytes != limits.memory_bytes ||
         (rlim_t)limits.memory_bytes == RLIM_INFINITY))
        return false;
    int in[2], out[2];
    if (pipe(in))
        return false;
    if (pipe(out)) {
        close(in[0]);
        close(in[1]);
        return false;
    }
    pid_t id = fork();
    if (id < 0) {
        close(in[0]);
        close(in[1]);
        close(out[0]);
        close(out[1]);
        return false;
    }
    if (!id) {
        setpgid(0, 0);
        dup2(in[0], 0);
        dup2(out[1], 1);
        dup2(out[1], 2);
        close(in[0]);
        close(in[1]);
        close(out[0]);
        close(out[1]);
        if (limits.memory_bytes) {
            struct rlimit memory;
            if (getrlimit(RLIMIT_AS, &memory))
                _exit(125);
            rlim_t bound = (rlim_t)limits.memory_bytes;
            if (memory.rlim_cur != RLIM_INFINITY && memory.rlim_cur < bound)
                bound = memory.rlim_cur;
            if (memory.rlim_max != RLIM_INFINITY && memory.rlim_max < bound)
                bound = memory.rlim_max;
            memory.rlim_cur = memory.rlim_max = bound;
            if (setrlimit(RLIMIT_AS, &memory))
                _exit(125);
        }
        if (dir && chdir(dir))
            _exit(126);
        execvp(argv[0], (char *const *)argv);
        _exit(127);
    }
    close(in[0]);
    close(out[1]);
    fcntl(out[0], F_SETFL, O_NONBLOCK);
    fcntl(in[1], F_SETFD, FD_CLOEXEC);
    fcntl(out[0], F_SETFD, FD_CLOEXEC);
    signal(SIGPIPE, SIG_IGN);
    p->pid = (int)id;
    p->input = handle(in[1]);
    p->output = handle(out[0]);
    p->running = true;
    return true;
}
int ps_process_read(ps_process *p, void *b, size_t n) {
    ssize_t r = read(fd(p->output), b, n);
    return r < 0 && (errno == EAGAIN || errno == EINTR) ? 0 : r == 0 ? -1 : (int)r;
}
bool ps_process_write(ps_process *p, const void *b, size_t n) {
    size_t done = 0;
    while (done < n) {
        ssize_t r = write(fd(p->input), (const char *)b + done, n - done);
        if (r < 0 && errno == EINTR)
            continue;
        if (r <= 0)
            return false;
        done += (size_t)r;
    }
    return true;
}
bool ps_process_poll(ps_process *p) {
    if (!p->running)
        return false;
    int status;
    pid_t r = waitpid(p->pid, &status, WNOHANG);
    if (!r)
        return process_deadline(p);
    if (r < 0 && errno == EINTR)
        return true;
    p->running = false;
    p->exit_code = r < 0 ? -1 : WIFEXITED(status) ? WEXITSTATUS(status) : 128 + WTERMSIG(status);
    return false;
}
void ps_process_kill(ps_process *p) {
    if (p->running) {
        kill(-p->pid, SIGKILL);
        kill(p->pid, SIGKILL);
        int s;
        while (waitpid(p->pid, &s, 0) < 0 && errno == EINTR) {
        }
        p->running = false;
        p->exit_code = 137;
    }
}
void ps_process_close(ps_process *p) {
    if (p->running)
        ps_process_kill(p);
    if (p->input)
        close(fd(p->input));
    if (p->output)
        close(fd(p->output));
    p->input = p->output = NULL;
}
void *ps_module_open(const char *path) { return dlopen(path, RTLD_NOW | RTLD_LOCAL); }
void *ps_module_symbol(void *m, const char *s) { return dlsym(m, s); }
void ps_module_close(void *m) {
    if (m)
        dlclose(m);
}
double ps_clock(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}
bool ps_process_usage_self(ps_process_usage *out) {
    if (!out) return false;
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) || usage.ru_maxrss < 0) return false;
    uint64_t peak = (uint64_t)usage.ru_maxrss;
#ifndef __APPLE__
    if (peak > UINT64_MAX / 1024) return false;
    peak *= 1024;
#endif
    *out = (ps_process_usage){(double)usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1e6,
                              (double)usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1e6,
                              peak};
    return true;
}
void ps_sleep(unsigned ms) {
    struct timespec t = {(time_t)(ms / 1000), (long)(ms % 1000) * 1000000};
    while (nanosleep(&t, &t) && errno == EINTR) {
    }
}
int ps_stdin_read(void *b, size_t n) {
    ssize_t r = read(0, b, n);
    return r < 0 && (errno == EAGAIN || errno == EINTR) ? 0 : r == 0 ? -1 : (int)r;
}
void ps_binary_stdio(void) {
    fcntl(0, F_SETFL, O_NONBLOCK);
    signal(SIGPIPE, SIG_IGN);
}
static void *parent_watch(void *unused) {
    (void)unused;
    unsigned char buffer[64];struct pollfd input={0,POLLIN|POLLHUP,0};
    for(;;) {
        int ready=poll(&input,1,-1);
        if(ready<0 && errno==EINTR)continue;
        if(ready<=0 || (input.revents&(POLLERR|POLLNVAL)))_Exit(125);
        ssize_t got=read(0,buffer,sizeof buffer);
        if(got>0)continue;
        if(got<0 && (errno==EINTR || errno==EAGAIN))continue;
        _Exit(125);
    }
}
bool ps_parent_watch_start(void) {
    struct stat info;if(fstat(0,&info) || !S_ISFIFO(info.st_mode))return false;
    pthread_attr_t attributes;
    if(pthread_attr_init(&attributes))return false;
    int error=pthread_attr_setdetachstate(&attributes,PTHREAD_CREATE_DETACHED);
    if(!error)error=pthread_attr_setstacksize(&attributes,65536);
    pthread_t thread;if(!error)error=pthread_create(&thread,&attributes,parent_watch,NULL);
    pthread_attr_destroy(&attributes);return error==0;
}
bool ps_make_directory(const char *p) { return !mkdir(p, 0755) || errno == EEXIST; }
bool ps_make_directory_exclusive(const char *p) { return !mkdir(p, 0755); }
bool ps_executable_path(char *p, size_t n) {
    if (!p || n < 2)
        return false;
#ifdef __APPLE__
    uint32_t size = 0;
    _NSGetExecutablePath(NULL, &size);
    char *path = malloc(size);
    if (!path)
        return false;
    bool ok = false;
    if (_NSGetExecutablePath(path, &size) == 0) {
        char *resolved = realpath(path, NULL);
        if (resolved && strlen(resolved) < n) {
            memcpy(p, resolved, strlen(resolved) + 1);
            ok = true;
        }
        free(resolved);
    }
    free(path);
    return ok;
#else
    ssize_t r = readlink("/proc/self/exe", p, n - 1);
    if (r <= 0 || (size_t)r >= n - 1)
        return false;
    p[r] = 0;
    return true;
#endif
}
#endif
