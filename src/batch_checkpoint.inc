/* Versioned private batch checkpoint. Fixed-width little-endian fields; no
 * pointers, struct padding or current output path enter the file. */
#define BATCH_CHECKPOINT_BYTES (4348u + PS_MAX_PARAMETERS*56u)
static bool batch_fingerprint(const char *path,uint64_t *size,uint64_t *hash) {
    FILE *f=fopen(path,"rb");if(!f)return false;
    unsigned char bytes[32768];size_t n;*size=0;*hash=UINT64_C(14695981039346656037);
    while((n=fread(bytes,1,sizeof bytes,f))!=0) {
        if(*size>UINT64_MAX-n){fclose(f);return false;}*size+=n;
        for(size_t i=0;i<n;i++){*hash^=bytes[i];*hash*=UINT64_C(1099511628211);}
    }
    bool ok=!ferror(f);return !fclose(f) && ok;
}
static void batch_put64(unsigned char *p,uint64_t value) {for(unsigned i=0;i<8;i++)p[i]=(unsigned char)(value>>(8*i));}
static uint64_t batch_get64(const unsigned char *p) {uint64_t value=0;for(unsigned i=0;i<8;i++)value|=(uint64_t)p[i]<<(8*i);return value;}
static ps_result batch_checkpoint_write(const ps_batch_options *o,const char *module,const char *source) {
    unsigned char bytes[BATCH_CHECKPOINT_BYTES]={0};size_t at=12;
    memcpy(bytes,"PSBRES01",8);ps_put_u32(bytes+8,sizeof bytes);
    memcpy(bytes+at,o->runner,strlen(o->runner));at+=4096;
    memcpy(bytes+at,o->channel,strlen(o->channel));at+=48;
    uint32_t numbers[]={o->runs,o->steps,o->workers,o->parameter_count,
        (o->adaptive?1u:0u)|(o->sweep?2u:0u)|(*source?4u:0u)|
        (*source && strlen(source)>=5 && !strcmp(source+strlen(source)-5,".phys")?8u:0u)};
    for(unsigned i=0;i<5;i++){ps_put_u32(bytes+at,numbers[i]);at+=4;}
    batch_put64(bytes+at,o->seed);at+=8;batch_put64(bytes+at,o->memory_bytes);at+=8;
    double values[]={o->dt,o->timeout_s,o->end_time,o->minimum_dt,o->maximum_dt,o->sweep_start,o->sweep_end};
    for(unsigned i=0;i<7;i++){ps_put_f64(bytes+at,values[i]);at+=8;}
    memcpy(bytes+at,o->sweep_name,strlen(o->sweep_name));at+=48;
    for(unsigned i=0;i<PS_MAX_PARAMETERS;i++) {
        if(i<o->parameter_count){memcpy(bytes+at,o->parameters[i].name,strlen(o->parameters[i].name));ps_put_f64(bytes+at+48,o->parameters[i].value);}
        at+=56;
    }
    const char *paths[]={o->runner,module,source};
    for(unsigned i=0;i<3;i++) {
        uint64_t size=0,hash=0;
        if(*paths[i] && !batch_fingerprint(paths[i],&size,&hash))return PS_IO;
        batch_put64(bytes+at,size);batch_put64(bytes+at+8,hash);at+=16;
    }
    if(at!=sizeof bytes-4)return PS_INVALID;
    ps_put_u32(bytes+at,ps_crc32(bytes,at));char path[4096];snprintf(path,sizeof path,"%s/resume.bin",o->directory);
    FILE *f=fopen(path,"wbx");if(!f)return PS_IO;
    bool ok=fwrite(bytes,1,sizeof bytes,f)==sizeof bytes;return !fclose(f) && ok?PS_OK:PS_IO;
}
ps_result ps_batch_resume_load(const char *series,const char *runner,const char *directory,ps_batch_options *out) {
    if(!series || !runner || !directory || !out || strlen(series)>4000 || strlen(runner)>4000 || strlen(directory)>4000)return PS_INVALID;
    char path[4096];snprintf(path,sizeof path,"%s/resume.bin",series);FILE *f=fopen(path,"rb");if(!f)return PS_IO;
    unsigned char bytes[BATCH_CHECKPOINT_BYTES];size_t n=fread(bytes,1,sizeof bytes,f);
    bool ok=n==sizeof bytes && fgetc(f)==EOF && !ferror(f);if(fclose(f))ok=false;
    if(!ok || memcmp(bytes,"PSBRES",6) || ps_get_u32(bytes+8)!=sizeof bytes)return PS_CORRUPT;
    if(memcmp(bytes+6,"01",2))return PS_VERSION;
    if(ps_get_u32(bytes+sizeof bytes-4)!=ps_crc32(bytes,sizeof bytes-4))return PS_CORRUPT;
    ps_batch_options o={0};size_t at=12;memcpy(o.runner,bytes+at,4096);at+=4096;
    if(!memchr(o.runner,0,sizeof o.runner))return PS_CORRUPT;
    memcpy(o.channel,bytes+at,48);at+=48;
    o.runs=ps_get_u32(bytes+at);at+=4;o.steps=ps_get_u32(bytes+at);at+=4;o.workers=ps_get_u32(bytes+at);at+=4;
    o.parameter_count=ps_get_u32(bytes+at);at+=4;uint32_t flags=ps_get_u32(bytes+at);at+=4;
    if(flags>15 || ((flags&8) && !(flags&4)))return PS_CORRUPT;
    o.adaptive=(flags&1)!=0;o.sweep=(flags&2)!=0;
    o.seed=batch_get64(bytes+at);at+=8;o.memory_bytes=batch_get64(bytes+at);at+=8;
    double *values[]={&o.dt,&o.timeout_s,&o.end_time,&o.minimum_dt,&o.maximum_dt,&o.sweep_start,&o.sweep_end};
    for(unsigned i=0;i<7;i++){*values[i]=ps_get_f64(bytes+at);if(!isfinite(*values[i]))return PS_CORRUPT;at+=8;}
    memcpy(o.sweep_name,bytes+at,48);at+=48;
    if(!memchr(o.sweep_name,0,sizeof o.sweep_name))return PS_CORRUPT;
    for(unsigned i=0;i<PS_MAX_PARAMETERS;i++){memcpy(o.parameters[i].name,bytes+at,48);o.parameters[i].value=ps_get_f64(bytes+at+48);at+=56;}
    snprintf(o.runner,sizeof o.runner,"%s",runner);snprintf(o.directory,sizeof o.directory,"%s",directory);
    snprintf(o.resume_from,sizeof o.resume_from,"%s",series);
#ifdef _WIN32
    snprintf(o.module,sizeof o.module,"%s/experiment.dll",series);
#else
    snprintf(o.module,sizeof o.module,"%s/experiment.so",series);
#endif
    if(flags&4)snprintf(o.source,sizeof o.source,"%s/experiment.%s",series,flags&8?"phys":"c");
    if(ps_batch_validate(&o)!=PS_OK)return PS_CORRUPT;
    const char *paths[]={o.runner,o.module,o.source};
    for(unsigned i=0;i<3;i++) {
        uint64_t size=0,hash=0;
        if(*paths[i] && !batch_fingerprint(paths[i],&size,&hash))return PS_IO;
        if(size!=batch_get64(bytes+at) || hash!=batch_get64(bytes+at+8))return PS_CORRUPT;
        at+=16;
    }
    *out=o;return PS_OK;
}
