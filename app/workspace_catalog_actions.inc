static void workspace_catalog_start(app *a) {
    free(a->workspaces);a->workspaces=calloc(1,sizeof *a->workspaces);
    a->workspace_selected=-1;a->workspace_name[0]=a->workspaces_error[0]=a->workspaces_path[0]=0;
    const char *path=a->workspace_state_path,*slash=strrchr(path,'/'),*backslash=strrchr(path,'\\');
    if(backslash && (!slash || backslash>slash))slash=backslash;
    size_t parent=slash?(size_t)(slash-path)+1:0;
    if(*path && parent+sizeof "workspaces.bin"<=sizeof a->workspaces_path) {
        memcpy(a->workspaces_path,path,parent);memcpy(a->workspaces_path+parent,"workspaces.bin",sizeof "workspaces.bin");
    }
    ps_result r=a->workspaces?ps_workspace_catalog_read(a->workspaces_path,a->workspaces):PS_MEMORY;
    a->workspaces_writable=r==PS_OK || r==PS_EOF;
    if(!a->workspaces_writable)snprintf(a->workspaces_error,sizeof a->workspaces_error,
        "Workspaces konnten nicht geladen werden (%s). Die Datei bleibt erhalten.",ps_result_string(r));
}
static bool workspace_catalog_save(app *a,const ps_workspace_catalog *next) {
    ps_workspace_catalog *target=a->workspaces?a->workspaces:calloc(1,sizeof *target);
    ps_result r=target?ps_workspace_catalog_write(a->workspaces_path,next):PS_MEMORY;
    if(r!=PS_OK){if(target!=a->workspaces)free(target);snprintf(a->workspaces_error,sizeof a->workspaces_error,"Workspaces konnten nicht gespeichert werden (%s).",ps_result_string(r));return false;}
    a->workspaces=target;
    *a->workspaces=*next;a->workspaces_writable=true;a->workspaces_error[0]=0;return true;
}
static void workspace_catalog_store(app *a) {
    ps_workspace_catalog *next=malloc(sizeof *next);ps_workspace_state *state=calloc(1,sizeof *state);
    ps_result r=next && state?workspace_state_capture(a,state):PS_MEMORY;
    if(r==PS_OK){*next=*a->workspaces;r=ps_workspace_catalog_put(next,a->workspace_name,state);}
    if(r==PS_OK) {
        if(workspace_catalog_save(a,next))for(unsigned i=0;i<next->count;i++)
            if(!strcmp(next->entries[i].name,a->workspace_name))a->workspace_selected=(int)i;
    } else snprintf(a->workspaces_error,sizeof a->workspaces_error,"%s",r==PS_LIMIT?
        "Alle acht Plätze sind belegt. Wähle einen vorhandenen Namen zum Ersetzen.":r==PS_INVALID?
        "Bitte einen Namen mit bis zu 63 UTF-8-Bytes ohne Rand-Leerzeichen oder Steuerzeichen eingeben.":"Workspace konnte nicht erfasst werden: Speicher fehlt.");
    free(next);free(state);
}
static void workspace_catalog_open(app *a) {
    if(workspace_restore_state(a,&a->workspaces->entries[a->workspace_selected].state)) {
        a->workspace_manager=false;a->workspaces_error[0]=0;
    } else snprintf(a->workspaces_error,sizeof a->workspaces_error,"%s",a->status);
}
static void workspace_catalog_delete(app *a) {
    ps_workspace_catalog *next=malloc(sizeof *next);
    if(!next){snprintf(a->workspaces_error,sizeof a->workspaces_error,"Workspace konnte nicht gelöscht werden: Speicher fehlt.");return;}
    *next=*a->workspaces;ps_workspace_catalog_remove(next,(unsigned)a->workspace_selected);
    if(workspace_catalog_save(a,next)){a->workspace_selected=-1;a->workspace_name[0]=0;}
    free(next);
}
static void workspace_catalog_reset(app *a) {
    ps_workspace_catalog *empty=calloc(1,sizeof *empty);
    if(!empty){snprintf(a->workspaces_error,sizeof a->workspaces_error,"Workspaces konnten nicht zurückgesetzt werden: Speicher fehlt.");return;}
    if(workspace_catalog_save(a,empty)){a->workspace_selected=-1;a->workspace_name[0]=0;}
    free(empty);
}
