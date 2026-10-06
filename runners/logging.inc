/* One explicit logging domain per runner. Log files are lazy, exclusive JSONL;
 * ordinary modules incur no sidecar and parameter discovery performs no I/O. */
typedef struct {
    FILE *file;
    double last_time_s;
    char path[4096];
    uint64_t accepted, bytes, dropped;
    bool events, disabled, failed, closed;
} runner_log_state;
static bool runner_log_json(FILE *file,const char *text) {
    if(fputc('"',file)==EOF)return false;
    for(const unsigned char *p=(const unsigned char*)text;*p;p++) {
        if(*p=='"' || *p=='\\'){if(fputc('\\',file)==EOF)return false;}
        if(*p=='\n'){if(fputs("\\n",file)==EOF)return false;}
        else if(*p=='\r'){if(fputs("\\r",file)==EOF)return false;}
        else if(*p=='\t'){if(fputs("\\t",file)==EOF)return false;}
        else if(fputc(*p,file)==EOF)return false;
    }
    return fputc('"',file)!=EOF;
}
static ps_result runner_log_record(runner_log_state *state,const ps_log_record *record) {
    if(!ps_log_record_valid(record))return PS_INVALID;
    if(!state->file) {
        state->file=fopen(state->path,"wbx");
        if(!state->file){state->failed=true;return PS_IO;}
    }
    bool ok=fprintf(state->file,"{\"level\":%u,\"time_s\":%.17g,\"message\":",
        (unsigned)record->level,record->time_s)>=0 && runner_log_json(state->file,record->message) &&
        fputs("}\n",state->file)!=EOF && !fflush(state->file);
    if(ok && state->events) {
        unsigned char payload[12+PS_LOG_MESSAGE_MAX];
        size_t n=ps_wire_log_encode(payload,sizeof payload,record);
        ok=n && send_message(PS_MSG_LOG,payload,(uint32_t)n);
    }
    if(!ok){state->failed=true;return PS_IO;}
    state->last_time_s=record->time_s;return PS_OK;
}
static ps_result runner_log_write(void *user,const ps_log_record *record) {
    runner_log_state *state=user;
    if(!state || !ps_log_record_valid(record))return PS_INVALID;
    if(state->closed)return PS_INVALID;
    if(state->disabled)return PS_OK;
    if(state->failed)return PS_IO;
    size_t bytes=strlen(record->message);
    if(state->accepted>=4096 || bytes>UINT64_C(4194304)-state->bytes) {
        if(state->dropped<UINT64_MAX)state->dropped++;
        return PS_LIMIT;
    }
    ps_result result=runner_log_record(state,record);
    if(result==PS_OK){state->accepted++;state->bytes+=bytes;}
    return result;
}
static void runner_log_close(runner_log_state *state,double time_s) {
    if(state->closed)return;
    if(state->dropped && !state->failed) {
        ps_log_record summary={.level=PS_LOG_WARNING,.time_s=isfinite(time_s)?time_s:state->last_time_s};
        snprintf(summary.message,sizeof summary.message,"Logger suppressed %llu messages after its bounded budget was exhausted.",
            (unsigned long long)state->dropped);
        (void)runner_log_record(state,&summary);
    }
    if(state->file){if(fclose(state->file))state->failed=true;state->file=NULL;}
    state->closed=true;
}
