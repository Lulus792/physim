#include "project_file.h"
#include <SDL3/SDL.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Project line %d: %s\n", __LINE__, #x);                                \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static bool write_text(const char *path, const char *text) {
    FILE *f = fopen(path, "wb");
    if (!f)
        return false;
    bool ok = fwrite(text, 1, strlen(text), f) == strlen(text);
    return !fclose(f) && ok;
}
static bool matches(const char *path, const char *text) {
    ps_text_document document = {0};
    bool ok = ps_text_document_open(&document, path) == PS_DOCUMENT_OK &&
              document.length == strlen(text) && !memcmp(document.saved, text, document.length);
    ps_text_document_destroy(&document);
    return ok;
}
int main(int argc, char **argv) {
    CHECK(argc == 2);
    char root[4096], path[4096], backup[4096];
    SDL_Time timestamp;
    CHECK(SDL_GetCurrentTime(&timestamp));
    snprintf(root, sizeof root, "%s/project-file-%llu ä", argv[1], (unsigned long long)timestamp);
    CHECK(SDL_CreateDirectory(root));
    snprintf(path, sizeof path, "%s/physim.project", root);
    snprintf(backup, sizeof backup, "%s.bak", path);
    ps_project_settings settings = {0};
    CHECK(write_text(path, "physim_project=1"));
    CHECK(ps_project_settings_read(path, &settings) == PS_DOCUMENT_OK);
    CHECK(!settings.release && !settings.language_experiment && !settings.language_analysis);
    CHECK(settings.timestep == .005 && settings.seed == 42);
    uint64_t seed = 7;
    CHECK(ps_project_seed_parse("0", &seed) && seed == 0);
    CHECK(ps_project_seed_parse("00042", &seed) && seed == 42);
    CHECK(ps_project_seed_parse("18446744073709551615", &seed) && seed == UINT64_MAX);
    const char *bad_seeds[] = {
        "", "-1", "+1", " 1", "1 ", "1.0", "1e3", "18446744073709551616", "999999999999999999999"};
    for (unsigned i = 0; i < sizeof bad_seeds / sizeof bad_seeds[0]; i++) {
        CHECK(!ps_project_seed_parse(bad_seeds[i], &seed) && seed == UINT64_MAX);
    }
    CHECK(ps_project_timestep_valid(DBL_MIN) && ps_project_timestep_valid(1));
    const char *original = "physim_project=1\r\n# Projekt α\r\nexperiment=main.phys\r\n"
                           "analysis=analysis.c\r\nprofile=Debug\r\nparameter.mass=2.5\r\n"
                           "modules=core,mechanics\r\nfuture.option=preserve this";
    CHECK(write_text(path, original));
    CHECK(ps_project_settings_read(path, &settings) == PS_DOCUMENT_OK);
    CHECK(settings.language_experiment && !settings.language_analysis &&
          settings.parameters.count == 1);
    strcpy(settings.parameters.selected[0], "3.125");
    settings.release = true;
    settings.timestep = .125;
    settings.seed = UINT64_MAX;
    CHECK(ps_project_settings_save(path, &settings) == PS_DOCUMENT_OK);
    CHECK(matches(backup, original));
    const char *saved =
        "physim_project=1\r\n# Projekt α\r\nexperiment=main.phys\r\n"
        "analysis=analysis.c\r\nmodules=core,mechanics\r\nfuture.option=preserve this\r\n"
        "profile=Release\r\nsimulation.dt=0.125\r\nsimulation.seed="
        "18446744073709551615\r\nparameter.mass=3.125\r\n";
    CHECK(matches(path, saved));
    CHECK(ps_project_settings_read(path, &settings) == PS_DOCUMENT_OK && settings.release);
    CHECK(!strcmp(settings.parameters.selected[0], "3.125"));
    CHECK(settings.timestep == .125 && settings.seed == UINT64_MAX);
    CHECK(ps_project_settings_save(path, &settings) == PS_DOCUMENT_OK);
    CHECK(matches(path, saved) &&
          matches(backup, original)); /* No redundant backup on unchanged save. */
    ps_project_settings before = settings;
    const char *invalid[] = {"",
                             "physim_project=2\n",
                             "physim_project=1\nphysim_project=1\n",
                             "physim_project=1\nprofile=Release\nprofile=Debug\n",
                             "physim_project=1\nprofile=Fast\n",
                             "physim_project=1\nsimulation.dt=0\n",
                             "physim_project=1\nsimulation.dt=-1\n",
                             "physim_project=1\nsimulation.dt=1.01\n",
                             "physim_project=1\nsimulation.dt=nan\n",
                             "physim_project=1\nsimulation.dt=inf\n",
                             "physim_project=1\nsimulation.dt=1e-999\n",
                             "physim_project=1\nsimulation.dt=0.1\nsimulation.dt=0.2\n",
                             "physim_project=1\nsimulation.seed=-1\n",
                             "physim_project=1\nsimulation.seed=18446744073709551616\n",
                             "physim_project=1\nsimulation.seed=1\nsimulation.seed=2\n",
                             "physim_project=1\nexperiment=other.c\n",
                             "physim_project=1\nanalysis=analysis.c\nanalysis=analysis.phys\n",
                             "physim_project=1\nparameter.mass=nan\n",
                             "physim_project=1\nparameter.mass=inf\n",
                             "physim_project=1\nparameter.mass=1\nparameter.mass=2\n",
                             "physim_project=1\nparameter.mass\n"};
    for (unsigned i = 0; i < sizeof invalid / sizeof invalid[0]; i++) {
        CHECK(write_text(path, invalid[i]));
        CHECK(ps_project_settings_read(path, &settings) == PS_DOCUMENT_INVALID);
        CHECK(!memcmp(&before, &settings, sizeof settings));
        CHECK(ps_project_settings_save(path, &before) == PS_DOCUMENT_INVALID);
        CHECK(matches(path, invalid[i]) && matches(backup, original));
    }
    CHECK(write_text(path, saved));
    const double bad_steps[] = {0, -1, 1.01, NAN, INFINITY, DBL_MIN / 2};
    for (unsigned i = 0; i < sizeof bad_steps / sizeof bad_steps[0]; i++) {
        settings = before;
        settings.timestep = bad_steps[i];
        CHECK(ps_project_settings_save(path, &settings) == PS_DOCUMENT_INVALID);
        CHECK(matches(path, saved));
    }
    settings = before;
    strcpy(settings.parameters.entries[0].name, "mass=1\nother");
    CHECK(ps_project_settings_save(path, &settings) == PS_DOCUMENT_INVALID);
    CHECK(matches(path, saved));
    settings = before;
    strcpy(settings.parameters.selected[0], "not a number");
    CHECK(ps_project_settings_save(path, &settings) == PS_DOCUMENT_INVALID);
    CHECK(matches(path, saved));
    settings = before;
    settings.release = false;
    CHECK(SDL_RemovePath(backup));
    CHECK(SDL_CreateDirectory(backup));
    CHECK(ps_project_settings_save(path, &settings) == PS_DOCUMENT_IO);
    CHECK(matches(path, saved)); /* Backup failure preserves the original. */
    settings = before;
    char long_line[1100];
    memset(long_line, 'x', sizeof long_line);
    memcpy(long_line, "physim_project=1\n", 17);
    long_line[sizeof long_line - 1] = 0;
    CHECK(write_text(path, long_line));
    CHECK(ps_project_settings_read(path, &settings) == PS_DOCUMENT_LIMIT);
    CHECK(!memcmp(&before, &settings, sizeof settings));
    puts("Project defaults, profiles, parameters, preserved extensions, invalid input and write "
         "failures passed.");
    return 0;
}
