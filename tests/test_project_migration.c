#include "project_file.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <sys/stat.h>
#endif
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Migration %d: %s\n",__LINE__,#x);return 1;}}while(0)
static bool write_text(const char *path,const char *text){FILE *f=fopen(path,"wb");if(!f)return false;size_t n=strlen(text);bool ok=fwrite(text,1,n,f)==n;return !fclose(f)&&ok;}
static bool matches(const char *path,const char *text){ps_text_document d={0};bool ok=ps_text_document_open(&d,path)==PS_DOCUMENT_OK && d.length==strlen(text) && !memcmp(d.saved,text,d.length);ps_text_document_destroy(&d);return ok;}
int main(int argc,char **argv){CHECK(argc==2);char path[4096],backup[4096];snprintf(path,sizeof path,"%s/project",argv[1]);snprintf(backup,sizeof backup,"%s.bak",path);
 const char *old[]={"physim_project=1","physim_project=1\r","physim_project=1\r\n# α\r\nexperiment=main.phys\r\nanalysis=analysis.c\r\nsimulation.seed=18446744073709551615\r\nparameter.mass=2\r\nextension.keep=unchanged", "physim_project=1\n# keep\nkind=experiment\nanalysis=analysis.phys\n"};
 const char *next[]={"physim_project=2\nkind=experiment\n","physim_project=2\r\nkind=experiment\r\n","physim_project=2\r\nkind=experiment\r\n# α\r\nexperiment=main.phys\r\nanalysis=analysis.c\r\nsimulation.seed=18446744073709551615\r\nparameter.mass=2\r\nextension.keep=unchanged","physim_project=2\n# keep\nkind=experiment\nanalysis=analysis.phys\n"};
 for(size_t i=0;i<sizeof old/sizeof *old;i++) {
  CHECK(write_text(path,old[i]));
#ifndef _WIN32
  CHECK(!chmod(path,0700));
#endif
  ps_project_settings before,after;CHECK(ps_project_settings_read(path,&before)==PS_DOCUMENT_OK && before.format_version==1);
  ps_project_migration result;CHECK(ps_project_migrate_file(path,&result)==PS_DOCUMENT_OK && result.from_version==1 && result.to_version==2 && result.changed);
  CHECK(matches(path,next[i]) && matches(backup,old[i]));CHECK(ps_project_settings_read(path,&after)==PS_DOCUMENT_OK && after.format_version==2);
  CHECK(after.language_experiment==before.language_experiment && after.language_analysis==before.language_analysis && after.seed==before.seed && after.timestep==before.timestep && after.parameters.count==before.parameters.count);
#ifndef _WIN32
  struct stat source_stat,backup_stat;CHECK(!stat(path,&source_stat) && !stat(backup,&backup_stat) && (source_stat.st_mode&0777)==0700 && (backup_stat.st_mode&0777)==0700);
#endif
  CHECK(ps_project_migrate_file(path,&result)==PS_DOCUMENT_OK && !result.changed && matches(path,next[i]) && matches(backup,old[i]));
 }
 ps_text_document document={0};CHECK(write_text(path,old[0]) && ps_text_document_open(&document,path)==PS_DOCUMENT_OK);
 CHECK(write_text(path,"physim_project=1\n# external\n"));ps_project_migration result={8,9,true},saved=result;
 CHECK(ps_project_migrate_document(&document,&result)==PS_DOCUMENT_CONFLICT && !memcmp(&result,&saved,sizeof result) && !strcmp(document.saved,old[0]) && matches(path,"physim_project=1\n# external\n"));ps_text_document_destroy(&document);
 CHECK(write_text(path,next[0]) && ps_text_document_open(&document,path)==PS_DOCUMENT_OK);CHECK(write_text(path,next[1]));
 CHECK(ps_project_migrate_document(&document,&result)==PS_DOCUMENT_CONFLICT && !memcmp(&result,&saved,sizeof result));ps_text_document_destroy(&document);
 const char *invalid[]={"physim_project=3\nkind=experiment\n","physim_project=2\n","physim_project=1\nkind=analysis\n","physim_project=1\nkind=experiment\nkind=experiment\n"};
 for(size_t i=0;i<sizeof invalid/sizeof *invalid;i++){CHECK(write_text(path,invalid[i]));CHECK(ps_project_migrate_file(path,&result)==PS_DOCUMENT_INVALID && !memcmp(&result,&saved,sizeof result) && matches(path,invalid[i]));}
 CHECK(write_text(path,old[0]) && SDL_RemovePath(backup) && SDL_CreateDirectory(backup));
 CHECK(ps_project_migrate_file(path,&result)==PS_DOCUMENT_IO && matches(path,old[0]) && !memcmp(&result,&saved,sizeof result));CHECK(SDL_RemovePath(backup));
 char *large=malloc(256u*1024u+1);CHECK(large);memcpy(large,"physim_project=1\n",17);for(size_t i=17;i<256u*1024u;i++)large[i]=(i-17)%2?'\n':'#';large[256u*1024u]=0;
 CHECK(write_text(path,large) && ps_project_migrate_file(path,&result)==PS_DOCUMENT_LIMIT && matches(path,large) && !memcmp(&result,&saved,sizeof result));free(large);
 CHECK(write_text(path,"physim_project=2\nkind=analysis\nanalysis=analysis.phys\n"));CHECK(ps_project_migrate_file(path,&result)==PS_DOCUMENT_OK && !result.changed);
 puts("Project migration: exact v1/v2 bytes, semantics, no-op, permissions, stale snapshots, limits and failure preservation passed");return 0;}
