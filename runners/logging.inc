/* One explicit logging domain per runner. Log files are lazy, exclusive JSONL;
 * ordinary modules incur no sidecar and parameter discovery performs no I/O. */
typedef struct {
    FILE *file,*pending;
    ps_run_store *archive;
    ps_run_write_handle writer;
    bool archive_failed;
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
static ps_result runner_log_archive(runner_log_state *state,const ps_log_record *record) {
    if(state->archive_failed)return PS_IO;
    ps_result r=PS_OK;
    if(state->archive)r=ps_run_writer_event(state->archive,state->writer,record);
    else {
        if(!state->pending)state->pending=tmpfile();
        if(!state->pending || fwrite(record,sizeof *record,1,state->pending)!=1)r=PS_IO;
    }
    if(r!=PS_OK)state->archive_failed=true;
    return r;
}
static ps_result runner_log_bind(runner_log_state *state,ps_run_store *store,ps_run_write_handle writer) {
    state->archive=store;state->writer=writer;
    if(state->archive_failed)return PS_IO;
    if(state->pending) {
        if(fflush(state->pending) || fseek(state->pending,0,SEEK_SET))state->archive_failed=true;
        ps_log_record event;
        while(!state->archive_failed && fread(&event,sizeof event,1,state->pending)==1)
            if(ps_run_writer_event(store,writer,&event)!=PS_OK)state->archive_failed=true;
        if(ferror(state->pending))state->archive_failed=true;
        if(fclose(state->pending))state->archive_failed=true;
        state->pending=NULL;
    }
    return state->archive_failed?PS_IO:PS_OK;
}
static ps_result runner_log_record(runner_log_state *state,const ps_log_record *record) {
    if(!ps_log_record_valid(record))return PS_INVALID;
    ps_result archived=runner_log_archive(state,record);
    if(state->failed)return PS_IO;
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
    state->last_time_s=record->time_s;return archived;
}
static ps_result runner_log_write(void *user,const ps_log_record *record) {
    runner_log_state *state=user;
    if(!state || !ps_log_record_valid(record))return PS_INVALID;
    if(state->closed)return PS_INVALID;
    if(state->disabled)return PS_OK;
    size_t bytes=strlen(record->message);
    if(state->accepted>=4096 || bytes>UINT64_C(4194304)-state->bytes) {
        if(state->dropped<UINT64_MAX)state->dropped++;
        return PS_LIMIT;
    }
    ps_result result=runner_log_record(state,record);
    state->accepted++;state->bytes+=bytes;
    return result;
}
static void runner_log_close(runner_log_state *state,double time_s) {
    if(state->closed)return;
    if(state->dropped) {
        ps_log_record summary={.level=PS_LOG_WARNING,.time_s=isfinite(time_s)?time_s:state->last_time_s};
        snprintf(summary.message,sizeof summary.message,"Logger suppressed %llu messages after its bounded budget was exhausted.",
            (unsigned long long)state->dropped);
        (void)runner_log_record(state,&summary);
    }
    if(state->file){if(fclose(state->file))state->failed=true;state->file=NULL;}
    if(state->pending){fclose(state->pending);state->pending=NULL;}
    state->closed=true;
}
