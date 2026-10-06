#include "batch.h"
#include "platform.h"
#include <float.h>
#include "number_parse.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

static bool absolute(const char *path, size_t capacity, bool optional) {
    const char *end = memchr(path, 0, capacity);
    if (!end || end - path > 4000 || strchr(path, '\n') || strchr(path, '\r'))
        return false;
    if (!*path)
        return optional;
#ifdef _WIN32
    return (end - path >= 3 &&
            ((path[0] >= 'A' && path[0] <= 'Z') || (path[0] >= 'a' && path[0] <= 'z')) &&
            path[1] == ':' && (path[2] == '/' || path[2] == '\\')) ||
           (end - path >= 3 && path[0] == '\\' && path[1] == '\\');
#else
    return path[0] == '/';
#endif
}
static bool sweep_name_valid(const char *name) {
    if (!memchr(name, 0, 48) || !name[0] ||
        !((name[0] >= 'A' && name[0] <= 'Z') ||
          (name[0] >= 'a' && name[0] <= 'z') || name[0] == '_'))
        return false;
    for (const char *p = name + 1; *p; p++)
        if (!((*p >= 'A' && *p <= 'Z') || (*p >= 'a' && *p <= 'z') ||
              (*p >= '0' && *p <= '9') || *p == '_' || *p == '.' || *p == '-'))
            return false;
    return true;
}
static double sweep_value(const ps_batch_options *o, uint32_t index) {
    if (index == 0)
        return o->sweep_start;
    if (index == o->runs - 1)
        return o->sweep_end;
    double fraction = (double)index / (o->runs - 1);
    return (1 - fraction) * o->sweep_start + fraction * o->sweep_end;
}
ps_result ps_batch_validate(const ps_batch_options *o) {
    if (!o || !absolute(o->runner, sizeof o->runner, false) ||
        !absolute(o->module, sizeof o->module, false) ||
        !absolute(o->source, sizeof o->source, true) ||
        !absolute(o->directory, sizeof o->directory, false) ||
        !absolute(o->resume_from,sizeof o->resume_from,true) ||
        !memchr(o->channel, 0, sizeof o->channel) || !o->channel[0] || !o->runs ||
        o->runs > PS_BATCH_MAX_RUNS || !o->workers || o->workers > PS_BATCH_MAX_WORKERS ||
        !o->steps || o->steps > 100000 ||
        (uint64_t)o->runs * (o->steps + 1u) > PS_BATCH_MAX_SAMPLES ||
        o->seed > UINT64_MAX - (o->runs - 1u) || !isfinite(o->dt) || o->dt < DBL_MIN || o->dt > 1 ||
        !isfinite(o->end_time) || o->end_time<0 || o->end_time>1e9 ||
        (o->adaptive && (o->end_time==0 || !isfinite(o->minimum_dt) || o->minimum_dt<DBL_MIN ||
                        !isfinite(o->maximum_dt) || o->maximum_dt>1 ||
                        o->minimum_dt>o->dt || o->dt>o->maximum_dt)) ||
        !isfinite(o->timeout_s) || o->timeout_s <= 0 || o->timeout_s > 3600 ||
        o->memory_bytes > UINT64_C(17179869184) ||
        o->source_size > 256u * 1024u || (!o->source_text && o->source_size) ||
        o->parameter_count > PS_MAX_PARAMETERS ||
        (o->sweep && (o->runs < 2 || !sweep_name_valid(o->sweep_name) ||
                      o->sweep_start == o->sweep_end ||
                      !isfinite(o->sweep_start) || !isfinite(o->sweep_end))))
        return PS_INVALID;
    if (o->sweep && o->parameter_count == PS_MAX_PARAMETERS)
        return PS_INVALID;
    for (uint32_t i = 0; i < o->parameter_count; i++) {
        if (!sweep_name_valid(o->parameters[i].name) ||
            !isfinite(o->parameters[i].value) ||
            (o->sweep && !strcmp(o->parameters[i].name, o->sweep_name)))
            return PS_INVALID;
        for (uint32_t j = 0; j < i; j++)
            if (!strcmp(o->parameters[i].name, o->parameters[j].name))
                return PS_INVALID;
    }
    for (const char *p = o->channel; *p; p++)
        if (!((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9') ||
              *p == '.' || *p == '_' || *p == '-'))
            return PS_INVALID;
    return PS_OK;
}
#include "batch_checkpoint.inc"

static int compare(const void *a, const void *b) {
    double x = *(const double *)a, y = *(const double *)b;
    return (x > y) - (x < y);
}
static double quantile(const double *sorted, uint32_t count, double p) {
    double h = (count - 1u) * p;
    uint32_t j = (uint32_t)h;
    double fraction = h - j;
    return fraction == 0 ? sorted[j] : (1 - fraction) * sorted[j] + fraction * sorted[j + 1];
}
ps_result ps_batch_report(const double *values, uint32_t count, const ps_channel *channel,
                          const char *provenance, ps_report **out) {
    if (!values || !channel || !out || !count || count > PS_BATCH_MAX_RUNS ||
        !memchr(channel->name, 0, sizeof channel->name) ||
        !memchr(channel->unit, 0, sizeof channel->unit))
        return PS_INVALID;
    double sorted[PS_BATCH_MAX_RUNS], scale = 0, mean = 0, m2 = 0;
    for (uint32_t i = 0; i < count; i++) {
        if (!isfinite(values[i]))
            return PS_NUMERIC;
        sorted[i] = values[i];
        scale = fmax(scale, fabs(values[i]));
    }
    qsort(sorted, count, sizeof *sorted, compare);
    for (uint32_t i = 0; i < count; i++) {
        double x = scale ? values[i] / scale : 0, delta = x - mean;
        mean += delta / (i + 1u);
        m2 += delta * (x - mean);
    }
    mean *= scale;
    double deviation = count > 1 ? sqrt(fmax(0, m2) / (count - 1u)) * scale : 0;
    double margin = (deviation / sqrt(count)) * 1.959963984540054;
    if (!isfinite(mean) || !isfinite(deviation) ||
        (count >= 200 && (!isfinite(mean - margin) || !isfinite(mean + margin))))
        return PS_NUMERIC;
    ps_report *report = NULL;
    ps_result r = ps_report_create("Monte Carlo · Endwerte", provenance, &report);
    if (r != PS_OK)
        return r;
    ps_plot_info plot = {0};
    snprintf(plot.title, sizeof plot.title, "Verteilung · %s", channel->name);
    snprintf(plot.x_label, sizeof plot.x_label, "%s", channel->name);
    snprintf(plot.y_label, sizeof plot.y_label, "Anzahl Läufe");
    memcpy(plot.x_unit.dimension, channel->dimension, 7);
    plot.x_unit.scale = plot.y_unit.scale = 1;
    snprintf(plot.x_unit.symbol, sizeof plot.x_unit.symbol, "%s", channel->unit);
    ps_plot_handle handle;
    r = ps_report_add_plot(report, &plot, &handle);
    ps_curve_data *curve = calloc(1, sizeof *curve);
    if (!curve) {
        r = PS_MEMORY;
        goto done;
    }
    curve->kind = PS_PLOT_HISTOGRAM;
    curve->source_count = count;
    snprintf(curve->label, sizeof curve->label, "Endwerte aller %u Läufe", count);
    double low = sorted[0], high = sorted[count - 1];
    if (low == high) {
        curve->count = 1;
        curve->x[0] = low;
        curve->y[0] = count;
        curve->bar_width = fmax(1e-12, fabs(low) * .01);
        if (!isfinite(low - curve->bar_width / 2) || !isfinite(high + curve->bar_width / 2))
            r = PS_NUMERIC;
    } else {
        curve->count = (uint32_t)ceil(sqrt(count));
        /* Divide before subtraction to avoid overflowing a finite range. */
        curve->bar_width = high / curve->count - low / curve->count;
        if (!isfinite(curve->bar_width) || curve->bar_width <= 0)
            r = PS_NUMERIC;
        for (uint32_t b = 0; b < curve->count; b++) {
            double f = (b + .5) / curve->count;
            curve->x[b] = (1 - f) * low + f * high;
        }
        for (uint32_t i = 0; i < count; i++) {
            double f = ps_report_axis_fraction(values[i], low, high);
            uint32_t b = (uint32_t)(f * curve->count);
            if (b >= curve->count)
                b = curve->count - 1;
            curve->y[b]++;
        }
    }
    if (r == PS_OK)
        r = ps_report_add_curve(report, handle, curve);
    free(curve);
    ps_table_info table = {0};
    snprintf(table.title, sizeof table.title, "Endwert-Statistik · %u Läufe", count);
    table.columns = 1;
    snprintf(table.column[0].label, sizeof table.column[0].label, "%s", channel->name);
    table.column[0].unit = plot.x_unit;
    ps_table_handle th = {0};
    if (r == PS_OK)
        r = ps_report_add_table(report, &table, &th);
    const char *labels[] = {"Mittelwert",
                            "Minimum",
                            "Quantil 2,5 %",
                            "Median",
                            "Quantil 97,5 %",
                            "Maximum",
                            "Stichproben-Standardabweichung"};
    double statistics[] = {mean,
                           low,
                           quantile(sorted, count, .025),
                           quantile(sorted, count, .5),
                           quantile(sorted, count, .975),
                           high,
                           deviation};
    for (unsigned i = 0; r == PS_OK && i < (count > 1 ? 7u : 6u); i++) {
        ps_table_row row = {0};
        snprintf(row.label, sizeof row.label, "%s", labels[i]);
        row.values[0] = statistics[i];
        r = ps_report_add_row(report, th, &row);
    }
    if (r == PS_OK && count >= 200) {
        snprintf(table.title, sizeof table.title, "Mittelwert · 95%%-KI (Normalnäherung)");
        r = ps_report_add_table(report, &table, &th);
        for (unsigned i = 0; r == PS_OK && i < 2; i++) {
            ps_table_row row = {0};
            snprintf(row.label, sizeof row.label, "%s", i ? "Obergrenze" : "Untergrenze");
            row.values[0] = i ? mean + margin : mean - margin;
            r = ps_report_add_row(report, th, &row);
        }
    }
done:
    if (r == PS_OK)
        *out = report;
    else
        ps_report_destroy(report);
    return r;
}
static ps_result sweep_report(const ps_batch_options *o, const ps_batch_result *result,
                              const char *provenance, ps_report **out) {
    ps_report *report = NULL;
    ps_result r = ps_report_create("Parameterstudie · Endwerte", provenance, &report);
    if (r != PS_OK)
        return r;
    ps_plot_info plot = {0};
    snprintf(plot.title, sizeof plot.title, "%s nach %s", o->channel, o->sweep_name);
    snprintf(plot.x_label, sizeof plot.x_label, "%s", o->sweep_name);
    snprintf(plot.y_label, sizeof plot.y_label, "%s", o->channel);
    plot.x_unit.scale = plot.y_unit.scale = 1;
    if(result->sweep_unit.declared) {
        memcpy(plot.x_unit.dimension,result->sweep_unit.dimension,7);
        plot.x_unit.scale=result->sweep_unit.scale;
        snprintf(plot.x_unit.symbol,sizeof plot.x_unit.symbol,"%s",result->sweep_unit.symbol);
    }
    memcpy(plot.y_unit.dimension, result->channel.dimension, 7);
    snprintf(plot.y_unit.symbol, sizeof plot.y_unit.symbol, "%s", result->channel.unit);
    ps_plot_handle handle;
    r = ps_report_add_plot(report, &plot, &handle);
    ps_curve_data *curve = calloc(1, sizeof *curve);
    if (!curve && r == PS_OK)
        r = PS_MEMORY;
    if (r == PS_OK) {
        snprintf(curve->label, sizeof curve->label, "Endwert je Parameter");
        curve->kind = PS_PLOT_LINE;
        curve->count = o->runs;
        curve->source_count = o->runs;
        for (uint32_t i = 0; i < o->runs; i++) {
            curve->x[i] = sweep_value(o, i)/plot.x_unit.scale;
            curve->y[i] = result->values[i];
        }
        r = ps_report_add_curve(report, handle, curve);
    }
    free(curve);
    if (r == PS_OK)
        *out = report;
    else
        ps_report_destroy(report);
    return r;
}
static bool copy(const char *from, const char *to) {
    FILE *in = fopen(from, "rb");
    if (!in)
        return false;
    FILE *out = fopen(to, "wbx");
    bool ok = out != NULL;
    unsigned char buffer[16384];
    size_t n;
    while (ok && (n = fread(buffer, 1, sizeof buffer, in)) != 0)
        ok = fwrite(buffer, 1, n, out) == n;
    ok = ok && !ferror(in);
    fclose(in);
    if (out && fclose(out))
        ok = false;
    return ok;
}
static bool keep_going(ps_batch_continue proceed, void *user, ps_batch_result *result) {
    if (proceed && !proceed(result->completed, result->active, user))
        result->cancelled = true;
    return !result->cancelled;
}
typedef struct {
    enum { SLOT_EMPTY, SLOT_RUNNING, SLOT_READING } state;
    ps_process child;
    ps_run_reader reader;
    uint32_t index, samples;
    int channel, status_channel;
    double deadline, value, measurement_status, previous_time;
    char path[4096];
} batch_slot;
static double endpoint_time(const ps_batch_options *o) {
    return o->end_time>0?o->end_time:o->steps*o->dt;
}
static bool csv_row(FILE *file, const ps_batch_options *o, uint32_t index, double value) {
    if (o->sweep)
        return fprintf(file, "%u,%llu,run-%04u.psrun,%.17g,%.17g,%.17g\n", index + 1,
                       (unsigned long long)(o->seed + index), index + 1, endpoint_time(o),
                       sweep_value(o, index), value) >= 0;
    return fprintf(file, "%u,%llu,run-%04u.psrun,%.17g,%.17g\n", index + 1,
                   (unsigned long long)(o->seed + index), index + 1, endpoint_time(o), value) >= 0;
}
static ps_result open_endpoint(batch_slot *slot, const ps_batch_options *o) {
    ps_result r = ps_run_open(&slot->reader, slot->path);
    if (r != PS_OK)
        return r;
    slot->state = SLOT_READING;
    if(*o->resume_from) {
        char expected[128];uint64_t size,hash;
        if(!batch_fingerprint(o->module,&size,&hash))return PS_IO;
        snprintf(expected,sizeof expected,"\nmodule_fnv1a64=%016llx\n",(unsigned long long)hash);
        if(!strstr(slot->reader.metadata,expected))return PS_CORRUPT;
        snprintf(expected,sizeof expected,"\nseed=%llu\n",(unsigned long long)(o->seed+slot->index));
        if(!strstr(slot->reader.metadata,expected))return PS_CORRUPT;
        snprintf(expected,sizeof expected,"\ndt_s=%.17g\n",o->dt);
        if(!strstr(slot->reader.metadata,expected))return PS_CORRUPT;
    }
    if(o->end_time>0) {
        char expected[128];
        snprintf(expected,sizeof expected,"\nend_time_s=%.17g\n",o->end_time);
        if(!strstr(slot->reader.metadata,expected) ||
           !strstr(slot->reader.metadata,o->adaptive?"\nstep_mode=adaptive\n":"\nstep_mode=fixed\n"))
            return PS_CORRUPT;
        snprintf(expected,sizeof expected,"\nmaximum_accepted_steps=%u\n",o->steps);
        if(!strstr(slot->reader.metadata,expected)) return PS_CORRUPT;
        snprintf(expected,sizeof expected,"\nseed=%llu\n",(unsigned long long)(o->seed+slot->index));
        if(!strstr(slot->reader.metadata,expected)) return PS_CORRUPT;
        snprintf(expected,sizeof expected,"\ndt_s=%.17g\n",o->dt);
        if(!strstr(slot->reader.metadata,expected)) return PS_CORRUPT;
        if(o->adaptive) {
            snprintf(expected,sizeof expected,"\nminimum_dt_s=%.17g\n",o->minimum_dt);
            if(!strstr(slot->reader.metadata,expected)) return PS_CORRUPT;
            snprintf(expected,sizeof expected,"\nmaximum_dt_s=%.17g\n",o->maximum_dt);
            if(!strstr(slot->reader.metadata,expected)) return PS_CORRUPT;
        }
    }
    if (o->sweep) {
        char expected[128];
        snprintf(expected, sizeof expected, "\nparameter.%s=%.17g\n", o->sweep_name,
                 sweep_value(o, slot->index));
        if (!strstr(slot->reader.metadata, expected))
            return PS_CORRUPT;
    }
    for (uint32_t i = 0; i < o->parameter_count; i++) {
        char expected[128];
        snprintf(expected, sizeof expected, "\nparameter.%s=%.17g\n",
                 o->parameters[i].name, o->parameters[i].value);
        if (!strstr(slot->reader.metadata, expected))
            return PS_CORRUPT;
    }
    slot->samples = 0;
    slot->previous_time=0;
    slot->channel = -1;
    for (uint32_t i = 0; i < slot->reader.channels; i++)
        if (!strcmp(slot->reader.schema[i].name, o->channel))
            slot->channel = (int)i;
    return slot->channel < 0
               ? PS_INVALID
               : ps_channel_status_index(slot->reader.schema, slot->reader.channels,
                                         (uint32_t)slot->channel, &slot->status_channel);
}
/* Bound disk work per scheduling pass too: a large finished run must not prevent
 * other pipes, deadlines or cancellation from being serviced. */
static ps_result read_endpoint(batch_slot *slot, const ps_batch_options *o, bool *done) {
    *done = false;
    for (unsigned chunk = 0; chunk < 512; chunk++) {
        double time, values[PS_MAX_CHANNELS];
        ps_result r = ps_run_next(&slot->reader, &time, values);
        if (r == PS_EOF) {
            *done = true;
            if (slot->status_channel >= 0 && slot->measurement_status != 1)
                return PS_INVALID;
            if(o->end_time>0)
                return slot->samples>=2 && slot->previous_time==o->end_time?PS_OK:PS_CORRUPT;
            return slot->samples == o->steps + 1u ? PS_OK : PS_CORRUPT;
        }
        if (r != PS_OK || slot->samples > o->steps)
            return PS_CORRUPT;
        if(o->end_time>0) {
            if(!slot->samples) {if(time!=0) return PS_CORRUPT;}
            else {
                if(time<=slot->previous_time || time>o->end_time) return PS_CORRUPT;
                if(o->adaptive) {
                    double minimum=fmin(o->minimum_dt,o->end_time-slot->previous_time);
                    if(time<slot->previous_time+minimum || time>slot->previous_time+o->maximum_dt)
                        return PS_CORRUPT;
                } else if(time!=fmin((double)slot->samples*o->dt,o->end_time)) return PS_CORRUPT;
            }
        } else if(fabs(time-slot->samples*o->dt)>1e-10*fmax(1,fabs(time))) return PS_CORRUPT;
        slot->previous_time=time;
        slot->value = values[slot->channel];
        if (slot->status_channel >= 0) {
            slot->measurement_status = values[slot->status_channel];
            if (slot->measurement_status != 0 && slot->measurement_status != 1 &&
                slot->measurement_status != 2)
                return PS_CORRUPT;
        }
        slot->samples++;
    }
    return PS_OK;
}
static bool batch_resume_options_match(const ps_batch_options *a,const ps_batch_options *b) {
    if(strcmp(a->module,b->module) || strcmp(a->source,b->source) || strcmp(a->channel,b->channel) ||
       a->source_text || a->source_size || a->runs!=b->runs || a->steps!=b->steps || a->workers!=b->workers ||
       a->seed!=b->seed || a->dt!=b->dt || a->timeout_s!=b->timeout_s || a->memory_bytes!=b->memory_bytes ||
       a->adaptive!=b->adaptive || a->end_time!=b->end_time || a->minimum_dt!=b->minimum_dt || a->maximum_dt!=b->maximum_dt ||
       a->sweep!=b->sweep || strcmp(a->sweep_name,b->sweep_name) || a->sweep_start!=b->sweep_start || a->sweep_end!=b->sweep_end ||
       a->parameter_count!=b->parameter_count)return false;
    for(unsigned i=0;i<a->parameter_count;i++)if(strcmp(a->parameters[i].name,b->parameters[i].name) || a->parameters[i].value!=b->parameters[i].value)return false;
    return true;
}
static bool batch_csv_unsigned(const char *text,uint64_t *out) {
    if(!*text)return false;
    uint64_t value=0;
    for(const unsigned char *p=(const unsigned char*)text;*p;p++) {
        if(*p<'0' || *p>'9' || value>(UINT64_MAX-(*p-'0'))/10)return false;
        value=value*10+(*p-'0');
    }
    *out=value;return true;
}
static ps_result batch_reuse(const ps_batch_options *o,FILE *journal,ps_batch_continue proceed,void *user,ps_batch_result *result) {
    char path[4096],line[512];snprintf(path,sizeof path,"%s/completed.csv",o->resume_from);
    FILE *old=fopen(path,"rb");if(!old)return PS_IO;
    const char *header=o->sweep?"index,seed,file,time_s,parameter_value,value\n":"index,seed,file,time_s,value\n";
    ps_result r=PS_OK;
    if(!fgets(line,sizeof line,old) || strcmp(line,header)){fclose(old);return PS_CORRUPT;}
    while(fgets(line,sizeof line,old)) {
        if(!strchr(line,'\n')) {if(feof(old))break;r=PS_CORRUPT;break;}
        if(!keep_going(proceed,user,result))break;
        line[strlen(line)-1]=0;char *fields[6];unsigned count=0;fields[count++]=line;
        for(char *p=line;*p;p++)if(*p==','){*p=0;if(count==6){count=0;break;}fields[count++]=p+1;}
        uint64_t index64,seed;double time,value,parameter=0;char name[64];
        if(count!=(o->sweep?6u:5u) || !batch_csv_unsigned(fields[0],&index64) || !index64 || index64>o->runs ||
           !batch_csv_unsigned(fields[1],&seed) || !ps_parse_finite_number(fields[3],NULL,&time) ||
           !ps_parse_finite_number(fields[count-1],NULL,&value) ||
           (o->sweep && !ps_parse_finite_number(fields[4],NULL,&parameter))){r=PS_CORRUPT;break;}
        unsigned index=(unsigned)index64;
        if(result->finished[index-1] || seed!=o->seed+index-1 || time!=endpoint_time(o) ||
           (o->sweep && parameter!=sweep_value(o,index-1))){r=PS_CORRUPT;break;}
        snprintf(name,sizeof name,"run-%04u.psrun",index);if(strcmp(name,fields[2])){r=PS_CORRUPT;break;}
        batch_slot slot={.index=index-1};snprintf(slot.path,sizeof slot.path,"%s/%s",o->resume_from,name);
        r=open_endpoint(&slot,o);bool done=false;
        while(r==PS_OK && !done && keep_going(proceed,user,result))r=read_endpoint(&slot,o,&done);
        if(r==PS_OK && !result->cancelled && (!done || memcmp(&slot.value,&value,sizeof value)))r=PS_CORRUPT;
        ps_channel schema={0};ps_parameter_unit unit={0};
        if(r==PS_OK && !result->cancelled) {
            schema=slot.reader.schema[slot.channel];
            if(result->completed && (strcmp(schema.unit,result->channel.unit) || memcmp(schema.dimension,result->channel.dimension,7)))r=PS_CORRUPT;
            if(o->sweep)r=ps_parameter_unit_parse(slot.reader.metadata,o->sweep_name,&unit);
            if(r==PS_OK && o->sweep && (!isfinite(o->sweep_start/unit.scale) || !isfinite(o->sweep_end/unit.scale)))r=PS_CORRUPT;
            if(r==PS_OK && o->sweep && result->completed &&
               (unit.declared!=result->sweep_unit.declared || unit.scale!=result->sweep_unit.scale ||
                strcmp(unit.symbol,result->sweep_unit.symbol) || memcmp(unit.dimension,result->sweep_unit.dimension,7)))r=PS_CORRUPT;
        }
        ps_run_reader_close(&slot.reader);if(r!=PS_OK || result->cancelled)break;
        snprintf(path,sizeof path,"%s/%s",o->directory,name);
        if(!copy(slot.path,path) || !csv_row(journal,o,index-1,value) || fflush(journal)){r=PS_IO;break;}
        result->values[index-1]=value;result->finished[index-1]=true;result->completed++;result->reused++;
        if(result->completed==1 || index==1){result->channel=schema;result->sweep_unit=unit;}
    }
    if(ferror(old))r=PS_IO;
    if(fclose(old))r=PS_IO;
    return r;
}
static ps_result run_pool(const ps_batch_options *o, const char *module, FILE *journal,
                          ps_batch_continue proceed, void *user, ps_batch_result *result) {
    uint32_t workers = o->workers < o->runs ? o->workers : o->runs, next = 0;
    batch_slot *slots = calloc(workers, sizeof *slots);
    if (!slots)
        return PS_MEMORY;
    ps_result r = PS_OK;
    bool sweep_unit_known=result->completed!=0;
    char dt[64], steps[32], seed[32], work[4096],target[64],minimum[64],maximum[64];
    snprintf(dt, sizeof dt, "%.17g", o->dt);
    snprintf(steps, sizeof steps, "%u", o->steps);
    snprintf(target,sizeof target,"%.17g",o->end_time);
    snprintf(minimum,sizeof minimum,"%.17g",o->minimum_dt);
    snprintf(maximum,sizeof maximum,"%.17g",o->maximum_dt);
    while (result->completed < o->runs && keep_going(proceed, user, result)) {
        for (uint32_t s = 0; s < workers; s++) {
            batch_slot *slot = &slots[s];
            if (slot->state != SLOT_RUNNING)
                continue;
            char output[4096];
            for (unsigned drain = 0; drain < 8; drain++)
                if (ps_process_read(&slot->child, output, sizeof output) <= 0)
                    break;
            if (ps_process_poll(&slot->child)) {
                if (ps_clock() >= slot->deadline) {
                    snprintf(result->error, sizeof result->error,
                             "Lauf %u: Zeitlimit überschritten.", slot->index + 1);
                    r = PS_LIMIT;
                    goto done;
                }
            } else {
                int code = slot->child.exit_code;
                ps_process_close(&slot->child);
                result->active--;
                if (slot->child.timed_out) {
                    snprintf(result->error, sizeof result->error,
                             "Lauf %u: Zeitlimit überschritten.", slot->index + 1);
                    r = PS_LIMIT;
                    goto done;
                }
                if (code) {
                    snprintf(result->error, sizeof result->error, "Lauf %u: Runner-Exitcode %d.",
                             slot->index + 1, code);
                    r = PS_IO;
                    goto done;
                }
                r = open_endpoint(slot, o);
                if(r==PS_OK && o->sweep) {
                    ps_parameter_unit unit;
                    r=ps_parameter_unit_parse(slot->reader.metadata,o->sweep_name,&unit);
                    if(r==PS_OK && (!isfinite(o->sweep_start/unit.scale) ||
                                    !isfinite(o->sweep_end/unit.scale))) r=PS_CORRUPT;
                    if(r==PS_OK && sweep_unit_known &&
                       (unit.declared!=result->sweep_unit.declared || unit.scale!=result->sweep_unit.scale ||
                        strcmp(unit.symbol,result->sweep_unit.symbol) ||
                        memcmp(unit.dimension,result->sweep_unit.dimension,7))) r=PS_CORRUPT;
                    if(r==PS_OK && !sweep_unit_known) {
                        result->sweep_unit=unit;sweep_unit_known=true;
                    }
                }
                if (r != PS_OK) {
                    snprintf(result->error, sizeof result->error,
                             "Lauf %u: Messdatei/Kanal %s: %s.", slot->index + 1, o->channel,
                             ps_result_string(r));
                    goto done;
                }
            }
            if (!keep_going(proceed, user, result))
                goto done;
        }
        for (uint32_t s = 0; s < workers; s++) {
            batch_slot *slot = &slots[s];
            if (slot->state != SLOT_READING)
                continue;
            bool complete;
            r = read_endpoint(slot, o, &complete);
            if (r != PS_OK) {
                if (r == PS_INVALID && complete && slot->status_channel >= 0)
                    snprintf(result->error, sizeof result->error,
                             "Lauf %u: Endwert von %s ist kein gültiger Messwert (Status %.0f).",
                             slot->index + 1, o->channel, slot->measurement_status);
                else
                    snprintf(result->error, sizeof result->error,
                             "Lauf %u: Messdatei ungültig (%s).", slot->index + 1,
                             ps_result_string(r));
                goto done;
            }
            if (complete) {
                ps_channel schema = slot->reader.schema[slot->channel];
                if (result->completed && (strcmp(schema.unit, result->channel.unit) ||
                                          memcmp(schema.dimension, result->channel.dimension, 7))) {
                    snprintf(result->error, sizeof result->error,
                             "Lauf %u: Einheit von %s stimmt nicht überein.", slot->index + 1,
                             o->channel);
                    r = PS_INVALID;
                    goto done;
                }
                if (!csv_row(journal, o, slot->index, slot->value) || fflush(journal)) {
                    r = PS_IO;
                    goto done;
                }
                if (!result->completed || !slot->index)
                    result->channel = schema;
                result->values[slot->index] = slot->value;
                result->finished[slot->index] = true;
                result->completed++;
                ps_run_reader_close(&slot->reader);
                slot->state = SLOT_EMPTY;
            }
            if (!keep_going(proceed, user, result))
                goto done;
        }
        for (uint32_t s = 0; s < workers && next < o->runs; s++) {
            while(next<o->runs && result->finished[next])next++;
            if(next==o->runs)break;
            batch_slot *slot = &slots[s];
            if (slot->state != SLOT_EMPTY)
                continue;
            if (!keep_going(proceed, user, result))
                goto done;
            snprintf(work, sizeof work, "%s/work-%04u", o->directory, next + 1);
            if (!ps_make_directory_exclusive(work)) {
                r = PS_IO;
                goto done;
            }
            snprintf(slot->path, sizeof slot->path, "%s/run-%04u.psrun", o->directory, next + 1);
            snprintf(seed, sizeof seed, "%llu", (unsigned long long)(o->seed + next));
            char parameters[PS_MAX_PARAMETERS][128];
            const char *args[17 + 2 * PS_MAX_PARAMETERS] = {
                o->runner, module, slot->path, "--steps", steps, "--dt", dt, "--seed", seed};
            size_t argument_count = 9;
            if(o->end_time>0) {args[argument_count++]="--until";args[argument_count++]=target;}
            if(o->adaptive) {
                args[argument_count++]="--adaptive";
                args[argument_count++]="--min-dt";args[argument_count++]=minimum;
                args[argument_count++]="--max-dt";args[argument_count++]=maximum;
            }
            for (uint32_t i = 0; i < o->parameter_count; i++) {
                snprintf(parameters[i], sizeof parameters[i], "%s=%.17g",
                         o->parameters[i].name, o->parameters[i].value);
                args[argument_count++] = "--param";
                args[argument_count++] = parameters[i];
            }
            if (o->sweep) {
                snprintf(parameters[o->parameter_count], sizeof parameters[0], "%s=%.17g",
                         o->sweep_name, sweep_value(o, next));
                args[argument_count++] = "--param";
                args[argument_count++] = parameters[o->parameter_count];
            }
            args[argument_count] = NULL;
            ps_process_limits limits = {o->memory_bytes, o->timeout_s};
            if (!ps_process_start_limited(&slot->child, args, work, &limits)) {
                snprintf(result->error, sizeof result->error,
                         "Lauf %u: Runner konnte nicht starten.", next + 1);
                r = PS_IO;
                goto done;
            }
            slot->deadline = ps_clock() + o->timeout_s;
            slot->index = next++;
            slot->state = SLOT_RUNNING;
            result->started++;
            result->active++;
            if (result->active > result->peak_active)
                result->peak_active = result->active;
        }
        if (result->completed < o->runs)
            ps_sleep(2);
    }
done:
    for (uint32_t s = 0; s < workers; s++) {
        ps_process_close(&slots[s].child);
        if (slots[s].state == SLOT_READING)
            ps_run_reader_close(&slots[s].reader);
    }
    result->active = 0;
    free(slots);
    return r;
}
/* Ordered output includes every accepted completion, even if a lower index is
 * missing after cancellation. The append-only journal survives abrupt shutdowns. */
static ps_result ordered_endpoints(const ps_batch_options *o, const ps_batch_result *result) {
    char path[4096];
    snprintf(path, sizeof path, "%s/endpoints.csv", o->directory);
    FILE *file = fopen(path, "wbx");
    if (!file)
        return PS_IO;
    bool ok = fprintf(file, o->sweep ? "index,seed,file,time_s,parameter_value,value\n"
                                     : "index,seed,file,time_s,value\n") >= 0;
    for (uint32_t i = 0; ok && i < o->runs; i++)
        if (result->finished[i])
            ok = csv_row(file, o, i, result->values[i]);
    if (fclose(file))
        ok = false;
    return ok ? PS_OK : PS_IO;
}
ps_result ps_batch_run(const ps_batch_options *o, ps_batch_continue proceed, void *user,
                       ps_batch_result *result) {
    if (!result)
        return PS_INVALID;
    memset(result, 0, sizeof *result);
    ps_result r = ps_batch_validate(o);
    if (r != PS_OK) {
        snprintf(result->error, sizeof result->error,
                 "Ungültige Laufserie: Pfade, Seed und Grenzen prüfen.");
        return r;
    }
    if(*o->resume_from) {
        ps_batch_options saved;
        r=ps_batch_resume_load(o->resume_from,o->runner,o->directory,&saved);
        if(r!=PS_OK || !batch_resume_options_match(o,&saved)) {
            snprintf(result->error,sizeof result->error,"Fortsetzung abgewiesen: gespeicherte Konfiguration oder Dateien stimmen nicht überein.");
            return r==PS_OK?PS_INVALID:r;
        }
    }
    if (!keep_going(proceed, user, result))
        return PS_OK;
    if (!ps_make_directory_exclusive(o->directory)) {
        snprintf(result->error, sizeof result->error,
                 "Ausgabeordner existiert bereits oder ist nicht anlegbar.");
        return PS_IO;
    }
    char path[4096], module[4096], archived_source[4096]={0};
#ifdef _WIN32
    snprintf(module, sizeof module, "%s/experiment.dll", o->directory);
#else
    snprintf(module, sizeof module, "%s/experiment.so", o->directory);
#endif
    FILE *csv = NULL;
    if (!copy(o->module, module)) {
        r = PS_IO;
        goto finish;
    }
    if (*o->source || o->source_text) {
        size_t source_length = strlen(o->source);
        const char *extension = source_length >= 5 &&
                                        !strcmp(o->source + source_length - 5, ".phys")
                                    ? "phys" : "c";
        snprintf(path, sizeof path, "%s/experiment.%s", o->directory, extension);
        snprintf(archived_source,sizeof archived_source,"%s",path);
        bool copied;
        if (o->source_text) {
            FILE *source = fopen(path, "wbx");
            copied = source != NULL;
            if (source) {
                copied = fwrite(o->source_text, 1, o->source_size, source) == o->source_size;
                if (fclose(source))
                    copied = false;
            }
        } else
            copied = copy(o->source, path);
        if (!copied) {
            r = PS_IO;
            goto finish;
        }
    }
    r=batch_checkpoint_write(o,module,archived_source);if(r!=PS_OK)goto finish;
    snprintf(path, sizeof path, "%s/series.txt", o->directory);
    FILE *manifest = fopen(path, "wbx");
    if (!manifest) {
        r = PS_IO;
        goto finish;
    }
    int wrote = fprintf(
        manifest,
        "physim_batch=%u\nruns=%u\nsteps=%u\ndt=%.17g\nbase_seed=%llu\n"
        "seed_policy=base+i (i=0..runs-1)\nchannel=%s\nmetric=last_sample\n"
        "timeout_s=%.17g\nrunner=%s\nsource=%s\noriginal_module=%s\n"
        "workers=%u\nmemory_bytes=%llu\nworking_directory=work-NNNN (one per run)\n"
        "journal=completed.csv (completion order)\nendpoints=endpoints.csv (index order)\n",
        o->end_time>0?4u:o->sweep ? 3u : 2u, o->runs, o->steps, o->dt, (unsigned long long)o->seed, o->channel,
        o->timeout_s, o->runner,
        *o->source       ? o->source
        : o->source_text ? "in-memory snapshot"
                         : "not supplied",
        o->module, o->workers, (unsigned long long)o->memory_bytes);
    if(wrote>=0 && o->end_time>0)
        wrote=fprintf(manifest,"end_time_s=%.17g\nstep_mode=%s\nmaximum_accepted_steps=%u\n"
                      "minimum_dt_s=%.17g\nmaximum_dt_s=%.17g\nterminal_step=clip_to_target\n",
                      o->end_time,o->adaptive?"adaptive":"fixed",o->steps,
                      o->adaptive?o->minimum_dt:o->dt,o->adaptive?o->maximum_dt:o->dt);
    if (wrote >= 0 && o->sweep)
        wrote = fprintf(manifest,
                        "study=linear_parameter\nparameter=%s\nparameter_start=%.17g\n"
                        "parameter_end=%.17g\nparameter_policy=linear_inclusive\n",
                        o->sweep_name, o->sweep_start, o->sweep_end);
    for (uint32_t i = 0; wrote >= 0 && i < o->parameter_count; i++)
        wrote = fprintf(manifest, "fixed_parameter.%s=%.17g\n",
                        o->parameters[i].name, o->parameters[i].value);
    if(wrote>=0 && *o->resume_from)wrote=fprintf(manifest,"resume_from=%s\nresume_policy=verified_journal_reuse_into_new_directory\n",o->resume_from);
    int closed = fclose(manifest);
    if (wrote < 0 || closed) {
        r = PS_IO;
        goto finish;
    }
    snprintf(path, sizeof path, "%s/completed.csv", o->directory);
    csv = fopen(path, "wbx");
    if (!csv || fprintf(csv, o->sweep ? "index,seed,file,time_s,parameter_value,value\n"
                                      : "index,seed,file,time_s,value\n") < 0) {
        r = PS_IO;
        goto finish;
    }
    if(*o->resume_from)r=batch_reuse(o,csv,proceed,user,result);
    if(r==PS_OK && !result->cancelled)r = run_pool(o, module, csv, proceed, user, result);
    if (csv) {
        if (fclose(csv))
            r = PS_IO;
        csv = NULL;
    }
    if(r==PS_OK && o->sweep && !result->cancelled && result->sweep_unit.declared) {
        snprintf(path,sizeof path,"%s/series.txt",o->directory);
        FILE *units=fopen(path,"ab");
        if(!units)r=PS_IO;
        else {
            const ps_parameter_unit *u=&result->sweep_unit;
            int written=fprintf(units,"parameter_value_storage=SI\n"
                "parameter_unit.%s=%s\nparameter_scale.%s=%.17g\n"
                "parameter_dimension.%s=%d,%d,%d,%d,%d,%d,%d\n",
                o->sweep_name,u->symbol,o->sweep_name,u->scale,o->sweep_name,
                u->dimension[0],u->dimension[1],u->dimension[2],u->dimension[3],
                u->dimension[4],u->dimension[5],u->dimension[6]);
            int closed_units=fclose(units);
            if(written<0 || closed_units)r=PS_IO;
        }
    }
    ps_result ordered = ordered_endpoints(o, result);
    if (ordered != PS_OK)
        r = ordered;
    if (r == PS_OK && !result->cancelled && keep_going(proceed, user, result)) {
        char provenance[8192];
        if (o->sweep)
            snprintf(provenance, sizeof provenance,
                     "Parameterstudie mit einem separaten Runner-Prozess je Punkt.\n"
                     "Rohdaten, Seeds und Snapshots: %s\n"
                     "Parameter %s linear von %.17g bis %.17g, %u inklusive Punkte.\n"
                     "Deklarierte Parameterwerte in Rohdaten und CSV sind SI; die X-Achse verwendet die Anzeigeeinheit.\n"
                     "Kanal %s; Endzeit %.17g s; Startseed %llu; Parallelität %u.\n"
                     "Die Seeds steigen ebenfalls je Laufindex; ein stochastisches Modell "
                     "kann daher zusätzliche Streuung erzeugen.\n"
                     "Die Kurve zeigt Rohendwerte, keine statistischen Schätzungen.",
                     o->directory, o->sweep_name, o->sweep_start, o->sweep_end, o->runs,
                     o->channel, endpoint_time(o), (unsigned long long)o->seed, o->workers);
        else
            snprintf(provenance, sizeof provenance,
                 "Endwert je unabhängigem Runner-Prozess.\nRohdaten, Seeds und Snapshots: %s\n"
                 "Kanal: %s; Endzeit: %.17g s; Startseed: %llu; Läufe: %u; Parallelität: %u.\n"
                 "Auswertung in fester Laufindex-Reihenfolge, unabhängig vom Abschlusszeitpunkt.\n"
                 "Quantile: linear, Typ 7. Streuung: Stichproben-Standardabweichung (n-1).\n"
                 "95%%-KI des Mittelwerts: Normalnäherung, nur ab 200 Läufen.\n"
                 "Voraussetzung: unabhängige Stichproben mit endlicher Varianz; starke Schiefe/"
                 "Ausreißer können die Näherung unbrauchbar machen.\n"
                 "Das KI ist kein Vorhersageintervall für einen einzelnen Lauf.\n"
                 "Wiederholbarkeit setzt voraus, dass der Experimentcode nur den expliziten Seed "
                 "als Zufallsquelle nutzt; dieselbe Binärdatei und Laufumgebung verwenden.",
                 o->directory, o->channel, endpoint_time(o), (unsigned long long)o->seed, o->runs,
                 o->workers);
        ps_report *report = NULL;
        r = o->sweep ? sweep_report(o, result, provenance, &report)
                     : ps_batch_report(result->values, result->completed, &result->channel,
                                       provenance, &report);
        if (r == PS_OK) {
            snprintf(path, sizeof path, "%s/summary.psreport", o->directory);
            r = ps_report_save(report, path);
            if (r != PS_OK)
                remove(path); /* Only our exclusively created directory is writable here. */
        }
        ps_report_destroy(report);
    }
finish:
    if (csv && fclose(csv))
        r = PS_IO;
    if (r != PS_OK && !result->error[0])
        snprintf(result->error, sizeof result->error,
                 "Laufserie nach %u Läufen fehlgeschlagen: %s (Kanal %s).", result->completed,
                 ps_result_string(r), o->channel);
    snprintf(path, sizeof path, "%s/status.txt", o->directory);
    FILE *status = fopen(path, "wbx");
    if (status) {
        int n = fprintf(
            status, "status=%s\ncompleted=%u\nrequested=%u\nstarted=%u\npeak_active=%u\nreused=%u\nerror=%s\n",
            r != PS_OK          ? "failed"
            : result->cancelled ? "cancelled"
                                : "complete",
            result->completed, o->runs, result->started, result->peak_active, result->reused, result->error);
        int c = fclose(status);
        if (n < 0 || c)
            r = PS_IO;
    } else
        r = PS_IO;
    if (r != PS_OK) {
        snprintf(path, sizeof path, "%s/summary.psreport", o->directory);
        remove(path);
        if (!result->error[0])
            snprintf(result->error, sizeof result->error,
                     "Status der Laufserie konnte nicht gespeichert werden.");
    }
    return r;
}
