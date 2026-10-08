#ifndef PS_APP_PROFILING_H
#define PS_APP_PROFILING_H
#include "graphics.h"
#include "platform.h"
typedef struct ps_app_profile ps_app_profile;
typedef struct {
    uint64_t frame, runner_bytes, job_bytes;
    double time_seconds, interval_seconds, frame_seconds;
    double event_seconds, work_seconds, ui_seconds, render_seconds, present_seconds;
    double documentation_seconds, capture_seconds, startup_ready_seconds;
    ps_process_usage usage;
    ps_scene_render_stats scene;
    ps_ui_render_stats ui;
    bool captured, has_scene, rendered;
} ps_app_profile_frame;
typedef struct { uint64_t written, dropped; bool failed; } ps_app_profile_result;
/* Opt-in, new exclusive directory. Worker owns all filesystem access. UI producer
 * never waits for queue locks or writes. Single producer; finish joins/drains only
 * during shutdown. CPU/peak fields are app-process snapshots, exclude children.
 * Missing frames are explicit; artifacts are diagnostic, not scientific data. */
ps_app_profile *ps_app_profile_start(const char *directory);
bool ps_app_profile_record(ps_app_profile *profile, const ps_app_profile_frame *frame);
ps_app_profile_result ps_app_profile_finish(ps_app_profile *profile);
#endif
