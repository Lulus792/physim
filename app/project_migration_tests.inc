static void project_migration_test_frame(app *a,const char *mode,int *stage,int *code) {
 bool ok=true;bool stale=!strcmp(mode,"stale"),dirty=!strcmp(mode,"dirty");
 switch(*stage) {
 case 0:
  ok=a->loaded && a->project_format_version==1;a->project_build_disclosure=NK_MAXIMIZED;
  if(stale){char path[4096];join(path,sizeof path,a->project,"physim.project");FILE *f=fopen(path,"ab");ok=ok && f;
   if(f){ok=ok && fputs("# external change\n",f)>=0;ok=!fclose(f) && ok;}}
  if(dirty)a->project_settings_dirty=true;
  (*stage)++;break;
 case 1:if(a->migration_bounds.w<=0)return;test_mouse(a,a->migration_bounds,true);(*stage)++;break;
 case 2:test_mouse(a,a->migration_bounds,false);(*stage)++;break;
 case 3:
  if(!stale && !dirty && a->project_format_version!=2)return;
  if(stale)ok=a->project_format_version==1 && strstr(a->status,"extern")!=NULL;
  else if(dirty){ok=a->project_format_version==1 && a->project_settings_dirty && !migrate_current_project(a);a->project_settings_dirty=false;}
  else ok=a->project_format_version==2 && !strncmp(a->project_manifest_snapshot.saved,"physim_project=2",16);
  a->quitting=true;if(ok)puts("PROJECT MIGRATION UI SELF-TEST: PASSED");break;
 }
 if(!ok){fprintf(stderr,"Project migration UI failed stage %d mode %s\n",*stage,mode);*code=1;a->quitting=true;}
}
