#include "project_file.h"
#include "physim/detail/run_identity.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <sys/stat.h>
#endif
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Identity assignment %d: %s\n",__LINE__,#x);return 1;}}while(0)
static bool write_text(const char *path,const char *text){FILE *f=fopen(path,"wb");if(!f)return false;size_t n=strlen(text);bool ok=fwrite(text,1,n,f)==n;return !fclose(f)&&ok;}
static bool matches(const char *path,const char *text){ps_text_document d={0};bool ok=ps_text_document_open(&d,path)==PS_DOCUMENT_OK && d.length==strlen(text) && !memcmp(d.saved,text,d.length);ps_text_document_destroy(&d);return ok;}
int main(int argc,char **argv) {
 CHECK(argc==2);char path[4096],backup[4096];snprintf(path,sizeof path,"%s/identity.project",argv[1]);snprintf(backup,sizeof backup,"%s.bak",path);
 const char *input[]={"physim_project=1","physim_project=1\r","physim_project=2\r\nkind=experiment\r\n# α\r\nextension.keep=owner", "physim_project=2\nkind=analysis\nanalysis=analysis.phys\n", "physim_project=1\nproject.id=0123456789abcdef0123456789abcdef\n"};
 for(unsigned i=0;i<sizeof input/sizeof *input;i++) {
  CHECK(write_text(path,input[i]));
#ifndef _WIN32
  CHECK(!chmod(path,0700));
#endif
  ps_project_identity_assignment report;CHECK(ps_project_assign_ids_file(path,&report)==PS_DOCUMENT_OK && report.changed && ps_run_identity_valid(report.project_id));
  CHECK(report.project_created==(i!=4) && report.experiment_created==(i!=3));
  CHECK(i==3?!report.experiment_id[0]:ps_run_identity_valid(report.experiment_id));CHECK(matches(backup,input[i]));
  ps_text_document document={0};CHECK(ps_text_document_open(&document,path)==PS_DOCUMENT_OK && document.length>strlen(input[i]) && !memcmp(document.saved,input[i],strlen(input[i])));
  ps_project_settings settings;CHECK(ps_project_settings_read_document(&document,&settings)==PS_DOCUMENT_OK && !strcmp(settings.project_id,report.project_id) && !strcmp(settings.experiment_id,report.experiment_id));
  CHECK(settings.format_version==(i==2 || i==3?2:1));
  CHECK(ps_project_assign_ids_file(path,&report)==PS_DOCUMENT_OK && !report.changed && matches(path,document.saved) && matches(backup,input[i]));
#ifndef _WIN32
  struct stat source_stat,backup_stat;CHECK(!stat(path,&source_stat) && !stat(backup,&backup_stat) && (source_stat.st_mode&0777)==0700 && (backup_stat.st_mode&0777)==0700);
#endif
  ps_text_document_destroy(&document);
 }
 ps_project_identity_assignment report,sentinel;memset(&sentinel,0x5a,sizeof sentinel);report=sentinel;
 ps_text_document stale={0};CHECK(write_text(path,input[0]) && ps_text_document_open(&stale,path)==PS_DOCUMENT_OK);
 CHECK(write_text(path,"physim_project=1\n# external\n"));CHECK(ps_project_assign_ids_document(&stale,&report)==PS_DOCUMENT_CONFLICT && !memcmp(&report,&sentinel,sizeof report) && !strcmp(stale.saved,input[0]));ps_text_document_destroy(&stale);
 const char *bad[]={"physim_project=99\n", "physim_project=2\n", "physim_project=1\nproject.id=bad\n", "physim_project=1\nproject.id=0123456789abcdef0123456789abcdef\nproject.id=0123456789abcdef0123456789abcdef\n"};
 for(unsigned i=0;i<sizeof bad/sizeof *bad;i++){CHECK(write_text(path,bad[i]));CHECK(ps_project_assign_ids_file(path,&report)==PS_DOCUMENT_INVALID && !memcmp(&report,&sentinel,sizeof report) && matches(path,bad[i]));}
 CHECK(write_text(path,input[0]) && SDL_RemovePath(backup) && SDL_CreateDirectory(backup));CHECK(ps_project_assign_ids_file(path,&report)==PS_DOCUMENT_IO && matches(path,input[0]) && !memcmp(&report,&sentinel,sizeof report));CHECK(SDL_RemovePath(backup));
 char *large=malloc(256u*1024u+1);CHECK(large);memcpy(large,"physim_project=1\n",17);for(size_t i=17;i<256u*1024u;i++)large[i]=(i-17)%2?'\n':'#';large[256u*1024u]=0;
 CHECK(write_text(path,large) && ps_project_assign_ids_file(path,&report)==PS_DOCUMENT_LIMIT && matches(path,large) && !memcmp(&report,&sentinel,sizeof report));free(large);
 puts("Project identities: retained bytes/IDs/format, LF/CRLF, analysis-only, no-op/backup, permissions, stale conflicts and failure atomicity passed");return 0;
}
