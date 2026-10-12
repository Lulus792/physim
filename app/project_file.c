#include "project_file.h"
#include "pacing.h"
#include "physim/detail/run_identity.h"
#include <SDL3/SDL_stdinc.h>
#include <errno.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ps_project_id_generate(char out[33]) {
    snprintf(out,33,"%08x%08x%08x%08x",(unsigned)SDL_rand_bits(),(unsigned)SDL_rand_bits(),
             (unsigned)SDL_rand_bits(),(unsigned)SDL_rand_bits());
}
bool ps_project_build_info_read(const char *path, ps_project_build_info *out) {
    if (!path || !out) return false;
    ps_text_document document = {0};
    if (ps_text_document_open(&document, path) != PS_DOCUMENT_OK) return false;
    ps_project_build_info info = {0};
    unsigned seen = 0;
    bool valid = document.length <= 256u * 1024u;
    for (size_t position = 0; valid && position < document.length;) {
        size_t end = position;
        while (end < document.length && document.saved[end] != '\n') ++end;
        size_t length = end - position;
        if (length && document.saved[position + length - 1] == '\r') --length;
        const char *line = document.saved + position;
        const char *keys[] = {"physim_native_build=", "compiler=", "profile=", "api_version=", "abi_version="};
        for (unsigned i = 0; i < 5; ++i) {
            size_t prefix = strlen(keys[i]);
            if (length < prefix || memcmp(line, keys[i], prefix)) continue;
            if (seen & (1u << i)) { valid = false; break; }
            seen |= 1u << i;
            const char *value = line + prefix;
            size_t count = length - prefix;
            if (i == 0) valid = count == 1 && value[0] == '1';
            else if (i == 1) {
                valid = count && count < sizeof info.compiler;
                for (size_t j = 0; valid && j < count; ++j)
                    valid = (unsigned char)value[j] >= 32 && value[j] != 127;
                if (valid) { memcpy(info.compiler, value, count); info.compiler[count] = 0; }
            } else if (i == 2) {
                info.release = count == 7 && !memcmp(value, "Release", 7);
                valid = info.release || (count == 5 && !memcmp(value, "Debug", 5));
            } else {
                uint64_t version = 0;
                valid = count && count <= 10;
                for (size_t j = 0; valid && j < count; ++j) {
                    valid = value[j] >= '0' && value[j] <= '9';
                    if (valid) version = version * 10 + (unsigned)(value[j] - '0');
                }
                valid = valid && version && version <= UINT32_MAX;
                if (valid) { if (i == 3) info.api_version = (uint32_t)version;
                             else info.abi_version = (uint32_t)version; }
            }
            break;
        }
        position = end < document.length ? end + 1 : end;
    }
    valid = valid && (seen & 7u) == 7u && ((seen & 24u) == 0 || (seen & 24u) == 24u);
    ps_text_document_destroy(&document);
    if (valid) *out = info;
    return valid;
}

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
static const char *const display_keys[]={"display.camera.mode=","display.camera.plane=",
    "display.camera.projection=","display.vectors=","display.paths=","display.points=",
    "display.labels=","display.grid="};
static int display_key(const char *line) {
    for(unsigned i=0;i<8;i++)if(!strncmp(line,display_keys[i],strlen(display_keys[i])))return (int)i;
    return -1;
}
bool ps_project_display_valid(const ps_project_display *display) {
    return display && (!display->defined || (display->plane<3 && !(display->flags&~PS_PROJECT_VIEW_FLAGS)));
}
static ps_document_result parse(const ps_text_document *document, ps_project_settings *out) {
    if(document->length>256u*1024u)return PS_DOCUMENT_LIMIT;
    ps_project_settings settings = {.timestep = .005, .seed = 42, .speed = 1,
                                    .minimum_timestep=1e-8,.maximum_timestep=.1};
    bool seen[11] = {false},identity_seen[2]={false};
    unsigned display_seen=0;
    size_t position = 0;
    unsigned index = 0, version=1;
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
            if(!strcmp(line,"physim_project=2"))version=2;
            else if (strcmp(line, "physim_project=1"))return PS_DOCUMENT_INVALID;
            continue;
        }
        const char *value = strchr(line, '=');
        if(!strncmp(line,"project.id=",11) || !strncmp(line,"experiment.id=",14)) {
            unsigned which=!strncmp(line,"experiment.id=",14);
            if(identity_seen[which] || !ps_run_identity_valid(value+1))return PS_DOCUMENT_INVALID;
            identity_seen[which]=true;memcpy(which?settings.experiment_id:settings.project_id,value+1,33);
        } else if (!strncmp(line,"kind=",5)) {
            if(seen[9] || (strcmp(value+1,"analysis") && strcmp(value+1,"experiment")))return PS_DOCUMENT_INVALID;
            seen[9]=true;settings.analysis_only=!strcmp(value+1,"analysis");
        } else if (!strncmp(line, "experiment=", 11) || !strncmp(line, "analysis=", 9)) {
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
        } else if (!strncmp(line, "simulation.breakpoints=", 23)) {
            ps_step_breakpoints points;
            if (seen[10] || strlen(value + 1) >= sizeof settings.breakpoints ||
                !ps_step_breakpoints_parse(value + 1, &points)) return PS_DOCUMENT_INVALID;
            seen[10] = true; strcpy(settings.breakpoints, value + 1);
        } else if(display_key(line)>=0) {
            unsigned key=(unsigned)display_key(line),bit=1u<<key;
            if(display_seen&bit)return PS_DOCUMENT_INVALID;
            display_seen|=bit;const char *entry=value+1;
            if(key==0) {
                if(strcmp(entry,"2d") && strcmp(entry,"3d"))return PS_DOCUMENT_INVALID;
                settings.display.planar=!strcmp(entry,"2d");
            } else if(key==1) {
                if(!strcmp(entry,"xy"))settings.display.plane=0;
                else if(!strcmp(entry,"yz"))settings.display.plane=1;
                else if(!strcmp(entry,"xz"))settings.display.plane=2;
                else return PS_DOCUMENT_INVALID;
            } else if(key==2) {
                if(strcmp(entry,"orthographic") && strcmp(entry,"perspective"))return PS_DOCUMENT_INVALID;
                settings.display.orthographic=!strcmp(entry,"orthographic");
            } else {
                if(strcmp(entry,"0") && strcmp(entry,"1"))return PS_DOCUMENT_INVALID;
                if(*entry=='1')settings.display.flags|=1u<<(key-3);
            }
        } else if (!strncmp(line, "parameter.", 10)) {
            if (!value)
                return PS_DOCUMENT_INVALID;
            line[value - line] = 0;
            if (!ps_parameter_catalog_restore(&settings.parameters, line + 10, value + 1))
                return PS_DOCUMENT_INVALID;
        } else if (!strncmp(line, "physim_project=", 15))
            return PS_DOCUMENT_INVALID;
    }
    if (!index || (version==2 && !seen[9]) || (settings.analysis_only && (version!=2 || seen[0])) || !ps_project_step_bounds_valid(&settings))
        return PS_DOCUMENT_INVALID;
    if(display_seen && display_seen!=255u)return PS_DOCUMENT_INVALID;
    settings.display.defined=display_seen==255u;
    settings.format_version=version;
    *out = settings;
    return PS_DOCUMENT_OK;
}
ps_document_result ps_project_settings_read_document(const ps_text_document *document,ps_project_settings *settings) {
    if(!document || !document->saved || !settings)return PS_DOCUMENT_INVALID;
    return parse(document,settings);
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
    ps_step_breakpoints breakpoints;
    if (!memchr(settings->breakpoints, 0, sizeof settings->breakpoints) ||
        !ps_step_breakpoints_parse(settings->breakpoints, &breakpoints)) return PS_DOCUMENT_INVALID;
    const ps_parameter_catalog *parameters = &settings->parameters;
    if (parameters->count > PS_MAX_PARAMETERS)
        return PS_DOCUMENT_INVALID;
    ps_text_document document = {0};
    ps_document_result result = ps_text_document_open(&document, path);
    ps_project_settings previous;
    if (result == PS_DOCUMENT_OK)
        result = parse(&document, &previous);
    if (result == PS_DOCUMENT_OK && settings->analysis_only != previous.analysis_only)
        result=PS_DOCUMENT_INVALID; /* Saving settings cannot silently change project kind. */
    if (result != PS_DOCUMENT_OK) {
        ps_text_document_destroy(&document);
        return result;
    }
    size_t capacity = document.length + PS_MAX_PARAMETERS * 160 + 1024, used = 0;
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
            strncmp(line,"simulation.minimum_dt=",22) && strncmp(line,"simulation.maximum_dt=",22) && strncmp(line,"simulation.breakpoints=",23)) {
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
    if (result == PS_DOCUMENT_OK && breakpoints.count) {
        n = snprintf(text + used, capacity - used, "simulation.breakpoints=%s%s", settings->breakpoints, newline);
        if (n < 0 || (size_t)n >= capacity - used) result = PS_DOCUMENT_LIMIT; else used += (size_t)n;
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

ps_document_result ps_project_display_save_document(ps_text_document *document,const ps_project_display *display) {
    if(!document || !document->saved || !document->path[0] || !ps_project_display_valid(display))return PS_DOCUMENT_INVALID;
    ps_project_settings settings;ps_document_result result=parse(document,&settings);
    if(result!=PS_DOCUMENT_OK)return result;
    size_t capacity=document->length+512,used=0;char *text=malloc(capacity);
    if(!text)return PS_DOCUMENT_MEMORY;
    const char *newline=strstr(document->saved,"\r\n")?"\r\n":"\n";
    for(size_t position=0;position<document->length;) {
        size_t end=position;while(end<document->length && document->saved[end]!='\n')end++;
        if(end<document->length)end++;
        if(display_key(document->saved+position)<0) {
            memcpy(text+used,document->saved+position,end-position);used+=end-position;
        }
        position=end;
    }
    if(display->defined) {
        if(used && text[used-1]!='\n'){memcpy(text+used,newline,strlen(newline));used+=strlen(newline);}
        const char *planes[]={"xy","yz","xz"};
        for(unsigned key=0;key<8;key++) {
            const char *value=key==0?(display->planar?"2d":"3d"):
                key==1?planes[display->plane]:key==2?(display->orthographic?"orthographic":"perspective"):
                (display->flags&(1u<<(key-3)))?"1":"0";
            int n=snprintf(text+used,capacity-used,"%s%s%s",display_keys[key],value,newline);
            if(n<0 || (size_t)n>=capacity-used){result=PS_DOCUMENT_LIMIT;break;}used+=(size_t)n;
        }
    }
    if(result==PS_DOCUMENT_OK && used==document->length && !memcmp(text,document->saved,used)) {
        ps_text_document current={0};result=ps_text_document_open(&current,document->path);
        if(result==PS_DOCUMENT_OK && (current.length!=document->length || memcmp(current.saved,document->saved,document->length)))result=PS_DOCUMENT_CONFLICT;
        ps_text_document_destroy(&current);
    } else if(result==PS_DOCUMENT_OK)result=ps_text_document_save(document,text,used);
    free(text);return result;
}

ps_document_result ps_project_migrate_document(ps_text_document *document,ps_project_migration *out) {
    if(!document || !document->saved || !document->path[0])return PS_DOCUMENT_INVALID;
    ps_project_settings settings;
    ps_document_result result=parse(document,&settings);if(result!=PS_DOCUMENT_OK)return result;
    ps_project_migration report={settings.format_version,PS_PROJECT_CURRENT_VERSION,false};
    if(settings.format_version==PS_PROJECT_CURRENT_VERSION) {
        ps_text_document current={0};result=ps_text_document_open(&current,document->path);
        if(result==PS_DOCUMENT_OK && (current.length!=document->length || memcmp(current.saved,document->saved,document->length)))result=PS_DOCUMENT_CONFLICT;
        ps_text_document_destroy(&current);if(result==PS_DOCUMENT_OK && out)*out=report;return result;
    }
    size_t header=0;while(header<document->length && document->saved[header]!='\n')header++;
    const char *newline=header && document->saved[header-1]=='\r'?"\r\n":"\n";
    size_t first_end=header<document->length?header+1:header;bool kind=false;
    for(size_t position=first_end;position<document->length;) {
        if(document->length-position>=5 && !memcmp(document->saved+position,"kind=",5))kind=true;
        while(position<document->length && document->saved[position]!='\n')position++;
        if(position<document->length)position++;
    }
    size_t header_newline=header==document->length?(header && document->saved[header-1]=='\r'?1:strlen(newline)):0;
    size_t extra=kind?0:strlen("kind=experiment")+strlen(newline)+header_newline;
    if(document->length>256u*1024u-extra)return PS_DOCUMENT_LIMIT;
    size_t length=document->length+extra;char *text=malloc(length+1);if(!text)return PS_DOCUMENT_MEMORY;
    memcpy(text,document->saved,first_end);text[15]='2';size_t used=first_end;
    if(!kind) {
        if(header==document->length){const char *ending=header_newline==1?"\n":newline;memcpy(text+used,ending,header_newline);used+=header_newline;}
        memcpy(text+used,"kind=experiment",15);used+=15;
        memcpy(text+used,newline,strlen(newline));used+=strlen(newline);
    }
    memcpy(text+used,document->saved+first_end,document->length-first_end);text[length]=0;
    ps_text_document proposed={0};proposed.saved=text;proposed.length=length;ps_project_settings verified;
    result=parse(&proposed,&verified);
    if(result==PS_DOCUMENT_OK)result=ps_text_document_save(document,text,length);
    free(text);
    if(result==PS_DOCUMENT_OK){report.changed=true;if(out)*out=report;}
    return result;
}
ps_document_result ps_project_migrate_file(const char *path,ps_project_migration *out) {
    ps_text_document document={0};ps_document_result result=ps_text_document_open(&document,path);
    if(result==PS_DOCUMENT_OK)result=ps_project_migrate_document(&document,out);
    ps_text_document_destroy(&document);return result;
}

ps_document_result ps_project_assign_ids_document(ps_text_document *document,ps_project_identity_assignment *out) {
    if(!document || !document->saved || !document->path[0])return PS_DOCUMENT_INVALID;
    ps_project_settings settings;
    ps_document_result result=parse(document,&settings);if(result!=PS_DOCUMENT_OK)return result;
    ps_text_document current={0};result=ps_text_document_open(&current,document->path);
    if(result==PS_DOCUMENT_OK && (current.length!=document->length || memcmp(current.saved,document->saved,document->length)))result=PS_DOCUMENT_CONFLICT;
    ps_text_document_destroy(&current);if(result!=PS_DOCUMENT_OK)return result;
    ps_project_identity_assignment report={0};
    memcpy(report.project_id,settings.project_id,33);memcpy(report.experiment_id,settings.experiment_id,33);
    report.project_created=!settings.project_id[0];
    report.experiment_created=!settings.analysis_only && !settings.experiment_id[0];
    report.changed=report.project_created || report.experiment_created;
    if(!report.changed){if(out)*out=report;return PS_DOCUMENT_OK;}
    const char *first=strchr(document->saved,'\n');
    const char *newline=(first && first>document->saved && first[-1]=='\r') ||
        (!first && document->length && document->saved[document->length-1]=='\r')?"\r\n":"\n";
    size_t ending=strlen(newline);
    size_t separator=document->length && document->saved[document->length-1]!='\n'?
        (document->saved[document->length-1]=='\r'?1:ending):0;
    size_t extra=separator+(report.project_created?11+32+ending:0)+(report.experiment_created?14+32+ending:0);
    if(document->length>256u*1024u-extra)return PS_DOCUMENT_LIMIT;
    char *text=malloc(document->length+extra+1);if(!text)return PS_DOCUMENT_MEMORY;
    if(report.project_created)ps_project_id_generate(report.project_id);
    if(report.experiment_created)ps_project_id_generate(report.experiment_id);
    size_t used=document->length;memcpy(text,document->saved,used);
    if(separator){memcpy(text+used,separator==1?"\n":newline,separator);used+=separator;}
    if(report.project_created)used+=(size_t)sprintf(text+used,"project.id=%s%s",report.project_id,newline);
    if(report.experiment_created)used+=(size_t)sprintf(text+used,"experiment.id=%s%s",report.experiment_id,newline);
    text[used]=0;
    ps_text_document proposed={0};proposed.saved=text;proposed.length=used;ps_project_settings verified;
    result=parse(&proposed,&verified);
    if(result==PS_DOCUMENT_OK)result=ps_text_document_save(document,text,used);
    free(text);if(result==PS_DOCUMENT_OK && out)*out=report;return result;
}
ps_document_result ps_project_assign_ids_file(const char *path,ps_project_identity_assignment *out) {
    ps_text_document document={0};ps_document_result result=ps_text_document_open(&document,path);
    if(result==PS_DOCUMENT_OK)result=ps_project_assign_ids_document(&document,out);
    ps_text_document_destroy(&document);return result;
}
