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
    CHECK(settings.timestep == .005 && settings.seed == 42 && settings.speed == 1);
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
    settings.speed = 4;
    CHECK(ps_project_settings_save(path, &settings) == PS_DOCUMENT_OK);
    CHECK(matches(backup, original));
    const char *saved =
        "physim_project=1\r\n# Projekt α\r\nexperiment=main.phys\r\n"
        "analysis=analysis.c\r\nmodules=core,mechanics\r\nfuture.option=preserve this\r\n"
        "profile=Release\r\nsimulation.dt=0.125\r\nsimulation.seed="
        "18446744073709551615\r\nsimulation.speed=4\r\nparameter.mass=3.125\r\n";
    CHECK(matches(path, saved));
    CHECK(ps_project_settings_read(path, &settings) == PS_DOCUMENT_OK && settings.release);
    CHECK(!strcmp(settings.parameters.selected[0], "3.125"));
    CHECK(settings.timestep == .125 && settings.seed == UINT64_MAX && settings.speed == 4);
    CHECK(ps_project_settings_save(path, &settings) == PS_DOCUMENT_OK);
    CHECK(matches(path, saved) &&
          matches(backup, original)); /* No redundant backup on unchanged save. */
    CHECK(!settings.adaptive && settings.minimum_timestep==1e-8 && settings.maximum_timestep==.1);
    settings.adaptive=true;settings.minimum_timestep=.00001;settings.maximum_timestep=.25;
    CHECK(ps_project_settings_save(path,&settings)==PS_DOCUMENT_OK);
    ps_project_settings restored;
    CHECK(ps_project_settings_read(path,&restored)==PS_DOCUMENT_OK && restored.adaptive &&
          restored.minimum_timestep==.00001 && restored.maximum_timestep==.25 && restored.timestep==.125);
    ps_project_settings invalid_bounds=restored;invalid_bounds.minimum_timestep=.2;
    CHECK(ps_project_settings_save(path,&invalid_bounds)==PS_DOCUMENT_INVALID);
    restored.adaptive=false;
    CHECK(ps_project_settings_save(path,&restored)==PS_DOCUMENT_OK && matches(path,saved));
    CHECK(write_text(backup,original));
    settings=restored;
    ps_project_settings before = settings;
    const char *analysis_manifest="physim_project=2\nkind=analysis\nanalysis=analysis.phys\n# saved data only\nprofile=Debug\n";
    CHECK(write_text(path,analysis_manifest));
    ps_project_settings only;
    CHECK(ps_project_settings_read(path,&only)==PS_DOCUMENT_OK && only.analysis_only && only.language_analysis);
    only.release=true;CHECK(ps_project_settings_save(path,&only)==PS_DOCUMENT_OK);
    CHECK(ps_project_settings_read(path,&only)==PS_DOCUMENT_OK && only.analysis_only && only.release);
    only.analysis_only=false;CHECK(ps_project_settings_save(path,&only)==PS_DOCUMENT_INVALID);
    CHECK(write_text(path,original) && write_text(backup,original));
    const char *invalid[] = {"",
                             "physim_project=2\n",
                             "physim_project=2\nkind=analysis\nexperiment=main.c\n",
                             "physim_project=2\nkind=analysis\nkind=experiment\n",
                             "physim_project=1\nkind=other\n",
                             "physim_project=1\nkind=analysis\nkind=analysis\n",
                             "physim_project=1\nkind=analysis\nexperiment=main.c\n",
                             "physim_project=1\nexperiment=main.phys\nkind=analysis\n",
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
                             "physim_project=1\nsimulation.speed=\n",
                             "physim_project=1\nsimulation.speed=-1\n",
                             "physim_project=1\nsimulation.speed=0.01\n",
                             "physim_project=1\nsimulation.speed=16.1\n",
                             "physim_project=1\nsimulation.speed=nan\n",
                             "physim_project=1\nsimulation.speed=inf\n",
                             "physim_project=1\nsimulation.speed=1e-999\n",
                             "physim_project=1\nsimulation.speed=1\nsimulation.speed=2\n",
                             "physim_project=1\nsimulation.steps=unknown\n",
                             "physim_project=1\nsimulation.steps=fixed\nsimulation.steps=adaptive\n",
                             "physim_project=1\nsimulation.steps=adaptive\nsimulation.minimum_dt=.1\n",
                             "physim_project=1\nsimulation.steps=adaptive\nsimulation.maximum_dt=.001\n",
                             "physim_project=1\nsimulation.minimum_dt=nan\n",
                             "physim_project=1\nsimulation.maximum_dt=0\n",
                             "physim_project=1\nsimulation.maximum_dt=1.1\n",
                             "physim_project=1\nsimulation.minimum_dt=1e-8\nsimulation.minimum_dt=1e-8\n",
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
    const double bad_speeds[] = {-1, .01, 16.1, NAN, INFINITY};
    for (unsigned i = 0; i < sizeof bad_speeds / sizeof *bad_speeds; i++) {
        settings = before;
        settings.speed = bad_speeds[i];
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
    char unit_path[4096];snprintf(unit_path,sizeof unit_path,"%s/units.project",argv[1]);
    CHECK(write_text(unit_path,"physim_project=1\n"));
    CHECK(ps_project_settings_read(unit_path,&settings)==PS_DOCUMENT_OK);
    CHECK(ps_parameter_catalog_restore(&settings.parameters,"length","0.5"));
    const char *unit_description="PHYSIM_PARAMETERS_2\n1\n"
        "length\t1.5\t0.1\t10\tLength\tcm\t0.01\t1,0,0,0,0,0,0\n";
    CHECK(ps_parameter_catalog_parse(&settings.parameters,unit_description));
    CHECK(!strcmp(settings.parameters.selected[0],"50"));
    CHECK(ps_project_settings_save(unit_path,&settings)==PS_DOCUMENT_OK);
    FILE *units_file=fopen(unit_path,"rb");char unit_text[2048];CHECK(units_file);
    size_t unit_bytes=fread(unit_text,1,sizeof unit_text-1,units_file);unit_text[unit_bytes]=0;CHECK(!fclose(units_file));
    CHECK(strstr(unit_text,"parameter.length=0.5\n") && !strstr(unit_text,"parameter.length=50\n"));
    CHECK(ps_project_settings_read(unit_path,&restored)==PS_DOCUMENT_OK);
    CHECK(ps_parameter_catalog_parse(&restored.parameters,unit_description));
    double si_value;
    CHECK(!strcmp(restored.parameters.selected[0],"50") &&
          ps_parameter_catalog_value(&restored.parameters,0,&si_value)==PS_OK && si_value==.5);
    puts("Project defaults, profiles, parameters, preserved extensions, invalid input and write "
         "failures passed.");
    return 0;
}
