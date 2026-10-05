#include "project_file.h"
#include "pacing.h"
#include <errno.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool ps_project_seed_parse(const char *text, uint64_t *seed) {
    if (!text || !*text || !seed)
        return false;
    uint64_t value = 0;
    for (const unsigned char *p = (const unsigned char *)text; *p; ++p) {
        if (*p < '0' || *p > '9')
            return false;
        unsigned digit = *p - '0';
        if (value > (UINT64_MAX - digit) / 10)
            return false;
        value = value * 10 + digit;
    }
    *seed = value;
    return true;
}
bool ps_project_timestep_valid(double timestep) {
    return isfinite(timestep) && timestep >= DBL_MIN && timestep <= 1;
}
bool ps_project_step_bounds_valid(const ps_project_settings *s) {
    return s && (!s->adaptive || (ps_project_timestep_valid(s->minimum_timestep) &&
           ps_project_timestep_valid(s->maximum_timestep) &&
           s->minimum_timestep<=s->timestep && s->timestep<=s->maximum_timestep));
}
static ps_document_result parse(const ps_text_document *document, ps_project_settings *out) {
    ps_project_settings settings = {.timestep = .005, .seed = 42, .speed = 1,
                                    .minimum_timestep=1e-8,.maximum_timestep=.1};
    bool seen[9] = {false};
    size_t position = 0;
    unsigned index = 0;
    while (position < document->length) {
        size_t end = position;
        while (end < document->length && document->saved[end] != '\n')
            ++end;
        size_t length = end - position;
        if (length && document->saved[position + length - 1] == '\r')
            --length;
        if (length >= 1024)
            return PS_DOCUMENT_LIMIT;
        char line[1024];
        memcpy(line, document->saved + position, length);
        line[length] = 0;
        position = end < document->length ? end + 1 : end;
        if (!index++) {
            if (strcmp(line, "physim_project=1"))
                return PS_DOCUMENT_INVALID;
            continue;
        }
        const char *value = strchr(line, '=');
        if (!strncmp(line, "experiment=", 11) || !strncmp(line, "analysis=", 9)) {
            unsigned which = !strncmp(line, "analysis=", 9);
            bool language = !strcmp(value + 1, which ? "analysis.phys" : "main.phys");
            if (seen[which] || (!language && strcmp(value + 1, which ? "analysis.c" : "main.c")))
                return PS_DOCUMENT_INVALID;
            seen[which] = true;
            if (which)
                settings.language_analysis = language;
            else
                settings.language_experiment = language;
        } else if (!strncmp(line, "profile=", 8)) {
            if (seen[2] || (strcmp(value + 1, "Debug") && strcmp(value + 1, "Release")))
                return PS_DOCUMENT_INVALID;
            seen[2] = true;
            settings.release = !strcmp(value + 1, "Release");
        } else if (!strncmp(line, "simulation.dt=", 14)) {
            if (seen[3])
                return PS_DOCUMENT_INVALID;
            seen[3] = true;
            char *number_end;
            errno = 0;
            double dt = strtod(value + 1, &number_end);
            if (errno || number_end == value + 1 || *number_end || !ps_project_timestep_valid(dt))
                return PS_DOCUMENT_INVALID;
            settings.timestep = dt;
        } else if (!strncmp(line, "simulation.seed=", 16)) {
            if (seen[4] || !ps_project_seed_parse(value + 1, &settings.seed))
                return PS_DOCUMENT_INVALID;
            seen[4] = true;
        } else if (!strncmp(line, "simulation.speed=", 17)) {
            if (seen[5]) return PS_DOCUMENT_INVALID;
            seen[5] = true;
            char *number_end;
            errno = 0;
            double speed = strtod(value + 1, &number_end);
            if (errno || number_end == value + 1 || *number_end || !ps_speed_valid(speed))
                return PS_DOCUMENT_INVALID;
            settings.speed = speed;
        } else if(!strncmp(line,"simulation.steps=",17)) {
            if(seen[6] || (strcmp(value+1,"fixed") && strcmp(value+1,"adaptive")))
                return PS_DOCUMENT_INVALID;
            seen[6]=true;settings.adaptive=!strcmp(value+1,"adaptive");
        } else if(!strncmp(line,"simulation.minimum_dt=",22) || !strncmp(line,"simulation.maximum_dt=",22)) {
            unsigned which=!strncmp(line,"simulation.minimum_dt=",22)?7:8;
            if(seen[which]) return PS_DOCUMENT_INVALID;
            seen[which]=true;char *number_end;errno=0;
            double dt=strtod(value+1,&number_end);
            if(errno || number_end==value+1 || *number_end || !ps_project_timestep_valid(dt))
                return PS_DOCUMENT_INVALID;
            if(which==7) settings.minimum_timestep=dt;else settings.maximum_timestep=dt;
        } else if (!strncmp(line, "parameter.", 10)) {
            if (!value)
                return PS_DOCUMENT_INVALID;
            line[value - line] = 0;
            if (!ps_parameter_catalog_restore(&settings.parameters, line + 10, value + 1))
                return PS_DOCUMENT_INVALID;
        } else if (!strncmp(line, "physim_project=", 15))
            return PS_DOCUMENT_INVALID;
    }
    if (!index || !ps_project_step_bounds_valid(&settings))
        return PS_DOCUMENT_INVALID;
    *out = settings;
    return PS_DOCUMENT_OK;
}
ps_document_result ps_project_settings_read(const char *path, ps_project_settings *settings) {
    if (!path || !settings)
        return PS_DOCUMENT_INVALID;
    ps_text_document document = {0};
    ps_document_result result = ps_text_document_open(&document, path);
    if (result == PS_DOCUMENT_OK)
        result = parse(&document, settings);
    ps_text_document_destroy(&document);
    return result;
}
ps_document_result ps_project_settings_save(const char *path, const ps_project_settings *settings) {
    if (!path || !settings || !ps_project_timestep_valid(settings->timestep) ||
        !ps_speed_valid(settings->speed) || !ps_project_step_bounds_valid(settings))
        return PS_DOCUMENT_INVALID;
    const ps_parameter_catalog *parameters = &settings->parameters;
    if (parameters->count > PS_MAX_PARAMETERS)
        return PS_DOCUMENT_INVALID;
    ps_text_document document = {0};
    ps_document_result result = ps_text_document_open(&document, path);
    ps_project_settings previous;
    if (result == PS_DOCUMENT_OK)
        result = parse(&document, &previous);
    if (result != PS_DOCUMENT_OK) {
        ps_text_document_destroy(&document);
        return result;
    }
    size_t capacity = document.length + PS_MAX_PARAMETERS * 160 + 384, used = 0;
    char *text = malloc(capacity);
    if (!text) {
        ps_text_document_destroy(&document);
        return PS_DOCUMENT_MEMORY;
    }
    const char *newline = strstr(document.saved, "\r\n") ? "\r\n" : "\n";
    for (size_t position = 0; position < document.length;) {
        size_t end = position;
        while (end < document.length && document.saved[end] != '\n')
            ++end;
        if (end < document.length)
            ++end;
        const char *line = document.saved + position;
        if (strncmp(line, "profile=", 8) && strncmp(line, "parameter.", 10) &&
            strncmp(line, "simulation.dt=", 14) && strncmp(line, "simulation.seed=", 16) &&
            strncmp(line, "simulation.speed=", 17) && strncmp(line,"simulation.steps=",17) &&
            strncmp(line,"simulation.minimum_dt=",22) && strncmp(line,"simulation.maximum_dt=",22)) {
            memcpy(text + used, line, end - position);
            used += end - position;
        }
        position = end;
    }
    if (used && text[used - 1] != '\n') {
        memcpy(text + used, newline, strlen(newline));
        used += strlen(newline);
    }
    int n = snprintf(text + used, capacity - used,
                     "profile=%s%ssimulation.dt=%.17g%ssimulation.seed=%llu%ssimulation.speed=%.17g%s",
                     settings->release ? "Release" : "Debug", newline, settings->timestep, newline,
                     (unsigned long long)settings->seed, newline, settings->speed, newline);
    if (n < 0 || (size_t)n >= capacity - used)
        result = PS_DOCUMENT_LIMIT;
    else
        used += (size_t)n;
    if(result==PS_DOCUMENT_OK && settings->adaptive) {
        n=snprintf(text+used,capacity-used,
                   "simulation.steps=adaptive%ssimulation.minimum_dt=%.17g%ssimulation.maximum_dt=%.17g%s",
                   newline,settings->minimum_timestep,newline,settings->maximum_timestep,newline);
        if(n<0 || (size_t)n>=capacity-used) result=PS_DOCUMENT_LIMIT;else used+=(size_t)n;
    }
    for (uint32_t i = 0; result == PS_DOCUMENT_OK && i < parameters->count; i++) {
        double value;
        if (ps_parameter_catalog_value(parameters, i, &value) != PS_OK) {
            result = PS_DOCUMENT_INVALID;
            break;
        }
        if (!memchr(parameters->entries[i].name, 0, sizeof parameters->entries[i].name)) {
            result = PS_DOCUMENT_INVALID;
            break;
        }
        char number[64];
        snprintf(number, sizeof number, "%.17g", value);
        ps_parameter_catalog check = {0};
        if (!ps_parameter_catalog_restore(&check, parameters->entries[i].name, number)) {
            result = PS_DOCUMENT_INVALID;
            break;
        }
        n = snprintf(text + used, capacity - used, "parameter.%s=%.17g%s",
                     parameters->entries[i].name, value, newline);
        if (n < 0 || (size_t)n >= capacity - used)
            result = PS_DOCUMENT_LIMIT;
        else
            used += (size_t)n;
    }
    ps_text_document proposed = {0};
    proposed.saved = text;
    proposed.length = used;
    ps_project_settings verified;
    if (result == PS_DOCUMENT_OK)
        result = parse(&proposed, &verified);
    if (result == PS_DOCUMENT_OK && (used != document.length || memcmp(text, document.saved, used)))
        result = ps_text_document_save(&document, text, used);
    free(text);
    ps_text_document_destroy(&document);
    return result;
}
