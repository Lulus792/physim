#include "batch.h"
#include "platform.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Resume line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static bool cancel(uint32_t completed,uint32_t active,void *user){(void)active;return completed<*(uint32_t*)user;}
static bool same(const char *a,const char *b) {
    FILE *x=fopen(a,"rb"),*y=fopen(b,"rb");if(!x || !y){if(x)fclose(x);if(y)fclose(y);return false;}
    bool ok=true;int cx,cy;do{cx=fgetc(x);cy=fgetc(y);if(cx!=cy)ok=false;}while(cx!=EOF && cy!=EOF);
    if(ferror(x) || ferror(y))ok=false;
    fclose(x);fclose(y);return ok;
}
static bool replace_bytes(const char *path,const void *bytes,size_t size) {
    FILE *f=fopen(path,"wb");if(!f)return false;
    bool ok=fwrite(bytes,1,size,f)==size;return !fclose(f) && ok;
}
static bool file_exists(const char *p){FILE *f=fopen(p,"rb");if(!f)return false;fclose(f);return true;}
int main(int argc,char **argv) {
    CHECK(argc==6); /* runner, fixture, batch CLI, source, work */
    ps_batch_options o={0};snprintf(o.runner,sizeof o.runner,"%s",argv[1]);snprintf(o.module,sizeof o.module,"%s",argv[2]);
    snprintf(o.source,sizeof o.source,"%s",argv[4]);strcpy(o.channel,"value");o.runs=6;o.steps=3;o.workers=4;o.dt=.01;o.timeout_s=5;
    char old[4096],dest[4096],reference[4096],a[4608],b[4608];
    snprintf(old,sizeof old,"%s/cancelled",argv[5]);snprintf(dest,sizeof dest,"%s/resumed",argv[5]);snprintf(reference,sizeof reference,"%s/reference",argv[5]);
    strcpy(o.directory,old);uint32_t after=2;ps_batch_result partial,continued,fresh;
    CHECK(ps_batch_run(&o,cancel,&after,&partial)==PS_OK && partial.cancelled && partial.completed==2 && !partial.finished[0]);
    ps_batch_options loaded={0};CHECK(ps_batch_resume_load(old,o.runner,dest,&loaded)==PS_OK && !strcmp(loaded.resume_from,old));
    CHECK(ps_batch_run(&loaded,NULL,NULL,&continued)==PS_OK && continued.completed==6 && continued.reused==2 && continued.started==4);
    strcpy(o.directory,reference);CHECK(ps_batch_run(&o,NULL,NULL,&fresh)==PS_OK && !memcmp(fresh.values,continued.values,6*sizeof(double)));
    for(unsigned i=0;i<6;i++)if(partial.finished[i]) {
        snprintf(a,sizeof a,"%s/run-%04u.psrun",old,i+1);snprintf(b,sizeof b,"%s/run-%04u.psrun",dest,i+1);CHECK(same(a,b));
        snprintf(b,sizeof b,"%s/work-%04u",dest,i+1); /* Reused indices do not start a process. */
        struct stat info;CHECK(stat(b,&info)!=0);
    }
    snprintf(a,sizeof a,"%s/endpoints.csv",dest);snprintf(b,sizeof b,"%s/endpoints.csv",reference);CHECK(same(a,b));
    ps_batch_options changed=loaded;changed.seed++;snprintf(changed.directory,sizeof changed.directory,"%s/rejected",argv[5]);
    CHECK(ps_batch_run(&changed,NULL,NULL,&fresh)==PS_INVALID);snprintf(a,sizeof a,"%s/resume.bin",changed.directory);CHECK(!file_exists(a));
    /* Accept legacy journals without the status column: their entries must all
     * agree with genuinely valid terminal measurements. */
    snprintf(a,sizeof a,"%s/completed.csv",dest);FILE *legacy_in=fopen(a,"rb");CHECK(legacy_in);
    char legacy[8192],line[512];size_t legacy_size=0;
    while(fgets(line,sizeof line,legacy_in)) {
        char *last=strrchr(line,',');CHECK(last);*last='\n';last[1]=0;
        size_t row_size=strlen(line);CHECK(legacy_size+row_size<sizeof legacy);
        memcpy(legacy+legacy_size,line,row_size);legacy_size+=row_size;
    }
    CHECK(!ferror(legacy_in) && !fclose(legacy_in) && replace_bytes(a,legacy,legacy_size));
    /* A fully completed continuation starts zero new processes. */
    char all[4096];snprintf(all,sizeof all,"%s/all-reused",argv[5]);CHECK(ps_batch_resume_load(dest,o.runner,all,&loaded)==PS_OK);
    CHECK(ps_batch_run(&loaded,NULL,NULL,&fresh)==PS_OK && fresh.reused==6 && fresh.started==0 && !memcmp(fresh.values,continued.values,6*sizeof(double)));
    /* Exact same entry point through CLI. */
    char cli[4096];snprintf(cli,sizeof cli,"%s/cli",argv[5]);const char *args[]={argv[3],"--resume",argv[1],old,cli,NULL};ps_process p={0};CHECK(ps_process_start(&p,args,NULL));
    char output[4096];double deadline=ps_clock()+15;
    while(ps_process_poll(&p) && ps_clock()<deadline){while(ps_process_read(&p,output,sizeof output)>0){}ps_sleep(1);}
    CHECK(!p.running && p.exit_code==0);ps_process_close(&p);
    snprintf(a,sizeof a,"%s/endpoints.csv",cli);snprintf(b,sizeof b,"%s/endpoints.csv",dest);CHECK(same(a,b));
    /* Changed archived source rejects loading without modifying caller options. */
    snprintf(a,sizeof a,"%s/experiment.c",old);FILE *input=fopen(a,"rb");CHECK(input);
    unsigned char original[8192];size_t length=fread(original,1,sizeof original,input);CHECK(!ferror(input) && !fclose(input) && length<sizeof original);
    unsigned char damaged[8192];memcpy(damaged,original,length);damaged[0]^=1;CHECK(replace_bytes(a,damaged,length));
    ps_batch_options preserved=loaded;CHECK(ps_batch_resume_load(old,o.runner,cli,&loaded)==PS_CORRUPT && !memcmp(&preserved,&loaded,sizeof loaded));
    CHECK(replace_bytes(a,original,length));
    /* Journal overflow, duplicate indices and corrupt complete runs are rejected. */
    snprintf(a,sizeof a,"%s/completed.csv",old);input=fopen(a,"rb");CHECK(input);
    length=fread(original,1,sizeof original,input);CHECK(!ferror(input) && !fclose(input) && length<sizeof original);
    const char invalid[]="index,seed,file,time_s,value\n184467440737095516160,0,run-0001.psrun,0.03,100\n";
    CHECK(replace_bytes(a,invalid,sizeof invalid-1));snprintf(dest,sizeof dest,"%s/bad-journal",argv[5]);
    CHECK(ps_batch_resume_load(old,o.runner,dest,&loaded)==PS_OK && ps_batch_run(&loaded,NULL,NULL,&fresh)==PS_CORRUPT);
    CHECK(replace_bytes(a,original,length));char *first=strchr((char*)original,'\n');CHECK(first);
    size_t start=(size_t)(first+1-(char*)original);char *end=memchr(original+start,'\n',length-start);CHECK(end);
    size_t row=(size_t)(end+1-(char*)original)-start;memcpy(damaged,original,length);memcpy(damaged+length,original+start,row);
    CHECK(replace_bytes(a,damaged,length+row));snprintf(dest,sizeof dest,"%s/duplicate-journal",argv[5]);
    CHECK(ps_batch_resume_load(old,o.runner,dest,&loaded)==PS_OK && ps_batch_run(&loaded,NULL,NULL,&fresh)==PS_CORRUPT);
    CHECK(replace_bytes(a,original,length));
    unsigned finished=0;while(!partial.finished[finished])finished++;
    snprintf(a,sizeof a,"%s/run-%04u.psrun",old,finished+1);input=fopen(a,"rb");CHECK(input);
    length=fread(original,1,sizeof original,input);CHECK(!ferror(input) && !fclose(input) && length<sizeof original && length>16);
    memcpy(damaged,original,length);damaged[length-1]^=1;CHECK(replace_bytes(a,damaged,length));
    snprintf(dest,sizeof dest,"%s/corrupt-run",argv[5]);CHECK(ps_batch_resume_load(old,o.runner,dest,&loaded)==PS_OK);
    CHECK(ps_batch_run(&loaded,NULL,NULL,&fresh)==PS_CORRUPT);CHECK(replace_bytes(a,original,length));
    /* Parameter studies keep their SI sweep and fixed options across reuse. */
    ps_batch_options study=o;study.runs=5;study.sweep=true;strcpy(study.sweep_name,"rate");study.sweep_start=.5;study.sweep_end=2;
    snprintf(study.directory,sizeof study.directory,"%s/study",argv[5]);uint32_t stop_after=2;
    CHECK(ps_batch_run(&study,cancel,&stop_after,&partial)==PS_OK && partial.cancelled);
    snprintf(dest,sizeof dest,"%s/study-resumed",argv[5]);CHECK(ps_batch_resume_load(study.directory,o.runner,dest,&loaded)==PS_OK);
    CHECK(ps_batch_run(&loaded,NULL,NULL,&continued)==PS_OK && continued.completed==5 && continued.reused==partial.completed);
    snprintf(study.directory,sizeof study.directory,"%s/study-reference",argv[5]);CHECK(ps_batch_run(&study,NULL,NULL,&fresh)==PS_OK);
    CHECK(!memcmp(fresh.values,continued.values,5*sizeof(double)));
    /* Header corruption and finite-field validation preserve caller output. */
    snprintf(a,sizeof a,"%s/resume.bin",old);FILE *checkpoint=fopen(a,"rb");CHECK(checkpoint);
    unsigned char bytes[8192];size_t size=fread(bytes,1,sizeof bytes,checkpoint);CHECK(!fclose(checkpoint) && size>4000);
    unsigned char saved[8192];memcpy(saved,bytes,size);ps_batch_options unchanged=loaded;
    for(size_t i=0;i<size;i+=97) {
        memcpy(bytes,saved,size);bytes[i]^=64;FILE *mutated=fopen(a,"wb");CHECK(mutated && fwrite(bytes,1,size,mutated)==size && !fclose(mutated));
        CHECK(ps_batch_resume_load(old,o.runner,cli,&loaded)!=PS_OK && !memcmp(&unchanged,&loaded,sizeof loaded));
    }
    checkpoint=fopen(a,"wb");CHECK(checkpoint && fwrite(saved,1,size,checkpoint)==size && !fclose(checkpoint));
    /* CRC and unsupported checkpoint versions leave the output object intact. */
    snprintf(a,sizeof a,"%s/resume.bin",old);FILE *f=fopen(a,"r+b");CHECK(f);CHECK(fseek(f,6,SEEK_SET)==0 && fputc('9',f)!=EOF && !fclose(f));
    ps_batch_options before=loaded;CHECK(ps_batch_resume_load(old,o.runner,cli,&loaded)==PS_VERSION && !memcmp(&before,&loaded,sizeof loaded));
    puts("Batch continuation: non-contiguous reuse, deterministic endpoints, zero reruns, CLI, mismatch and versions passed");return 0;
}
