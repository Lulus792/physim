#ifndef PHYSIM_PROJECT_FILE_H
#define PHYSIM_PROJECT_FILE_H
#include "parameter_catalog.h"
#include "text_document.h"

typedef struct {
    bool language_experiment, language_analysis, release;
    bool analysis_only;
    double timestep, speed;
    bool adaptive;
    double minimum_timestep, maximum_timestep;
    uint64_t seed;
    ps_parameter_catalog parameters;
} ps_project_settings;

/* Versions 1/2, UTF-8, LF or CRLF. Version 2 requires kind=analysis
 * or kind=experiment. kind=analysis selects an analysis-only
 * project without an experiment entry; version 1 defaults to experiment.
 * Absent source/profile entries retain the legacy
 * defaults main.c, analysis.c, Debug, 0.005 seconds, seed 42, speed 1.
 * Failed reads leave settings unchanged. */
ps_document_result ps_project_settings_read(const char *path, ps_project_settings *settings);
/* Updates profile, simulation settings and parameter selections, preserving all other entries
 * and comments. Reads the latest file, validates it, then uses the document
 * writer's conflict check, atomic replacement and backup. */
ps_document_result ps_project_settings_save(const char *path, const ps_project_settings *settings);
/* Seed is a decimal uint64 without signs or whitespace. Failed parsing leaves
 * output unchanged. Timestep is a finite normal positive double, at most 1 s. */
bool ps_project_seed_parse(const char *text, uint64_t *seed);
bool ps_project_timestep_valid(double timestep);
bool ps_project_step_bounds_valid(const ps_project_settings *settings);
#endif
