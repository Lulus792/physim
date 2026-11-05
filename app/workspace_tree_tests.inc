static struct {bool active,complete,intact,part_intact,in_bounds,legacy_split,scroll_requested;unsigned generation,wait_generation;float width;} project_name_probe;
static void project_tree_name_draw(app *a){
    if(!project_name_probe.active)return;
    project_name_probe.generation++;project_name_probe.complete=false;project_name_probe.intact=false;project_name_probe.part_intact=false;project_name_probe.in_bounds=true;project_name_probe.legacy_split=false;
    const char *name="completion-note.phys";size_t bytes=strlen(name),at=0;const char *dot=strrchr(name,'.');
    struct nk_rect row=a->workspace_tree_test_bounds,clip=nk_rect(-100000,-100000,200000,200000);
    const struct nk_command *cmd;bool first=true;
    nk_foreach(cmd,a->ui){
        if(cmd->type==NK_COMMAND_SCISSOR){const struct nk_command_scissor *s=(const struct nk_command_scissor*)cmd;clip=nk_rect(s->x,s->y,s->w,s->h);continue;}
        if(cmd->type!=NK_COMMAND_TEXT || at==bytes)continue;
        const struct nk_command_text *t=(const struct nk_command_text*)cmd;
        if(t->x<row.x || t->x>=row.x+row.w || t->y<row.y || t->y>=row.y+row.h ||
           t->length<=0 || (size_t)t->length>bytes-at || memcmp(t->string,name+at,(size_t)t->length))continue;
        float ink=t->font->width(t->font->userdata,t->font->height,t->string,t->length);
        if(t->x<clip.x || t->x+ink>clip.x+clip.w+1 || t->y<clip.y || t->y+t->font->height>clip.y+clip.h+1 ||
           t->x+ink>row.x+row.w+1 || t->y+t->font->height>row.y+row.h+1)project_name_probe.in_bounds=false;
        if(first){
            project_name_probe.width=t->w;size_t offset=0;
            while(offset<bytes){size_t length=0,used=documentation_line(t->font,name+offset,bytes-offset,t->w,&length);if(!used)break;
                if(offset+length>(size_t)(dot-name) && offset+length<bytes)project_name_probe.legacy_split=true;offset+=used;}
            first=false;
        }
        if(at<=(size_t)(dot-name) && at+(size_t)t->length==bytes)project_name_probe.intact=true;
        if(at<=strlen("completion-") && at+(size_t)t->length==bytes)project_name_probe.part_intact=true;
        at+=(size_t)t->length;
    }
    project_name_probe.complete=at==bytes;
}
static size_t workspace_tree_test_find(app *a, const char *name, unsigned root) {
    for (size_t i = 0; i < a->workspace_tree.count; i++)
        if (a->workspace_tree.entries[i].root == root &&
            !strcmp(workspace_name(a->workspace_tree.entries[i].path), name))
            return i;
    return SIZE_MAX;
}
static bool workspace_tree_test_target(app *a, const char *name, unsigned root) {
    size_t index = workspace_tree_test_find(a, name, root);
    if (index == SIZE_MAX)
        return false;
    snprintf(a->workspace_tree_test_path, sizeof a->workspace_tree_test_path, "%s",
             a->workspace_tree.entries[index].path);
    return true;
}
static bool workspace_tree_test_write(const char *path) {
    FILE *f = fopen(path, "wb");
    if (!f)
        return false;
    bool ok = fputs("Tree nested preview: α = 42\n", f) >= 0;
    return !fclose(f) && ok;
}
static bool workspace_tree_test_start(app *a, const char *directory) {
    char root[4096], folder[4096], path[4096], extra[4096];
    join(root, sizeof root, directory, "tree-root");
    join(folder, sizeof folder, root, "models/oscillator");
    join(path, sizeof path, folder, "notes.txt");
    if (!SDL_CreateDirectory(folder) || !workspace_tree_test_write(path))
        return false;
    join(extra, sizeof extra, directory, "references");
    join(path, sizeof path, extra, "reference.txt");
    if (!SDL_CreateDirectory(extra) || !workspace_tree_test_write(path))
        return false;
    if (!open_workspace_path(a, root))
        return false;
    add_workspace_path(a, extra);
    return !a->loaded && workspace_tree_test_target(a, "models", 0);
}
static bool project_tree_test_start(app *a,const char *directory) {
    char path[4096],assets[4096];
    join(a->project_input,sizeof a->project_input,directory,"project");a->example=0;new_project(a);
    if(!a->loaded)return false;
    join(assets,sizeof assets,a->project,"assets");
    join(path,sizeof path,assets,"note α.txt");
    if(!ps_make_directory(assets) || !workspace_tree_test_write(path))return false;
    join(path,sizeof path,a->project,"completion-note.phys");if(!workspace_tree_test_write(path))return false;
    ps_context context={.struct_size=sizeof context,.api_version=PS_API_VERSION,.dt_s=.01};
    if(ps_channel_add(&context,"position.x",PS_METRE,"Tree run position")!=0)return false;
    join(path,sizeof path,a->project,"runs/recorded ä.PSRUN");ps_run_writer writer;
    if(ps_run_create(&writer,path,&context,"Project tree fixture")!=PS_OK)return false;
    bool ok=true;
    for(unsigned i=0;i<3;i++){context.values[0]=4+i;ok &= ps_run_append(&writer,i*.01,context.values)==PS_OK;}
    ok &= ps_run_close(&writer)==PS_OK;
    join(path,sizeof path,a->project,"runs/invalid.psrun");ok &= workspace_tree_test_write(path);
    refresh_workspace_entries(a);a->workspace_disclosure=NK_MAXIMIZED;
    a->project_build_disclosure=NK_MINIMIZED;SDL_SetWindowSize(a->window,1080,740);
    return ok;
}
static void project_tree_test_frame(app *a,int *stage,int *code,const char **capture) {
    static unsigned item;static int invalid_tab;static const char *pending_capture;
    static const char *names[]={"main.c","analysis.c","assets","note α.txt","assets","runs","recorded ä.PSRUN","invalid.psrun"};
    bool ok=true;unsigned tested_item=item;int tested_stage=*stage;
    if(a->library_thread)return;
    switch((*stage)++) {
    case 0:item=0;pending_capture=NULL;break;
    case 1:ok=workspace_tree_test_target(a,names[item],0);break;
    case 2:ok=a->workspace_tree_test_bounds.w>0;test_mouse(a,a->workspace_tree_test_bounds,true);break;
    case 3:test_mouse(a,a->workspace_tree_test_bounds,false);break;
    case 4:
        if(a->data.thread){--*stage;return;}
        if(item==0)ok=a->tab==0 && a->document_count==0;
        else if(item==1)ok=a->tab==2 && a->analysis_tab==1 && a->document_count==0;
        else if(item==2)ok=workspace_tree_test_find(a,"note α.txt",0)!=SIZE_MAX;
        else if(item==3){workspace_document *doc=active_document(a);ok=a->tab==8 && doc && strstr(doc->file.saved,"Tree nested preview: α = 42");
            if(ok){ok=set_editor_text(&doc->edit,"Asset edit α\n",strlen("Asset edit α\n"));document_changed(a,&doc->edit);}}
        else if(item==4)ok=workspace_tree_test_find(a,"note α.txt",0)==SIZE_MAX;
        else if(item==5){ok=workspace_tree_test_find(a,"recorded ä.PSRUN",0)!=SIZE_MAX;pending_capture="project-tree-files.bmp";}
        else if(item==6){char path[4096];join(path,sizeof path,a->project,"runs/recorded ä.PSRUN");
            ok=a->tab==2 && a->analysis_tab==0 && workbench_path_equal(a->last_run,path) && a->data.result==PS_EOF && a->data.total==3 && a->data.channel_count==1 &&
               a->data.values[0][0]==4 && a->data.values[0][1]==5 && a->data.values[0][2]==6 &&
               a->data.time[0]==0 && a->data.time[1]==.01 && a->data.time[2]==.02 &&
               !strcmp(a->data.channels[0].name,"position.x") && !memcmp(a->data.channels[0].dimension,PS_METRE.dimension,7) && a->document_count==1;
            pending_capture="project-tree-run.bmp";}
        else if(item==7){char path[4096];join(path,sizeof path,a->project,"runs/recorded ä.PSRUN");workspace_document *doc=active_document(a);
            ok=a->tab==invalid_tab && a->analysis_tab==0 && workbench_path_equal(a->last_run,path) && a->data.total==3 && a->document_count==1 &&
               doc && doc->dirty && nk_str_len_char(&doc->edit.string)==(int)strlen("Asset edit α\n") &&
               !memcmp(nk_str_get_const(&doc->edit.string),"Asset edit α\n",strlen("Asset edit α\n")) && strstr(a->status,"Laufdatei");
            pending_capture="project-tree-invalid.bmp";}
        item++;
        if(pending_capture){test_mouse(a,nk_rect(900,700,1,1),false);*stage=5;}
        else if(item<8)*stage=1;
        break;
    case 5:
        *capture=pending_capture;pending_capture=NULL;
        if(item==7) {
            /* Analysis A owns a run list, not the Workspace file tree. Return
             * through the real navigation before targeting the invalid file. */
            test_mouse(a,a->navigation_bounds[0],true);*stage=6;
        } else if(item<8)*stage=1;else {
            ps_preferences pref=a->preferences;pref.ui_size=22;preferences_apply(a,&pref);
            ok=workspace_tree_test_target(a,"completion-note.phys",0);
            project_name_probe.active=true;project_name_probe.wait_generation=project_name_probe.generation+2;*stage=8;
        }
        break;
    case 6:test_mouse(a,a->navigation_bounds[0],false);break;
    case 7:
        ok=workspace_active_area(a)==0 && workbench_visible(a);invalid_tab=a->tab;
        puts("PROJECT TREE: returned through Develop navigation before invalid-file action");*stage=1;
        break;
    case 8:
        if(project_name_probe.generation<project_name_probe.wait_generation){--*stage;return;}
        if((!project_name_probe.complete || !project_name_probe.in_bounds) && !project_name_probe.scroll_requested){
            struct nk_rect tree=a->dock_panels[PS_DOCK_SIDEBAR];test_plot_wheel(a,tree.x+tree.w*.5f,tree.y+tree.h-10,-100);
            project_name_probe.scroll_requested=true;project_name_probe.wait_generation=project_name_probe.generation+2;--*stage;return;
        }
        ok=project_name_probe.complete && project_name_probe.intact && project_name_probe.part_intact && project_name_probe.in_bounds && project_name_probe.legacy_split;
        if(!ok)fprintf(stderr,"Project filename paint: complete=%d extension=%d inside=%d legacy-split=%d width=%.3f row=%.3f/%.3f/%.3f/%.3f\n",project_name_probe.complete,project_name_probe.intact,project_name_probe.in_bounds,project_name_probe.legacy_split,project_name_probe.width,a->workspace_tree_test_bounds.x,a->workspace_tree_test_bounds.y,a->workspace_tree_test_bounds.w,a->workspace_tree_test_bounds.h);
        *capture="project-tree-large-name.bmp";test_mouse(a,a->workspace_tree_test_bounds,true);*stage=9;break;
    case 9:test_mouse(a,a->workspace_tree_test_bounds,false);*stage=10;break;
    case 10:
        if(a->document_pending){--*stage;return;}
        {workspace_document *doc=active_document(a);ok=doc && !strcmp(workspace_name(doc->path),"completion-note.phys") && strstr(doc->file.saved,"Tree nested preview: α = 42");}
        if(ok)puts("PROJECT FILENAME ui=22 full-painted=1 extension-intact=1 within-clip=1 legacy-split-rejected=1 opened-exact-file=1 basename-part-intact=1");
        a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Project tree failure item=%u stage=%d tab=%d status=%s last-run=%s\n",tested_item,tested_stage,a->tab,a->status,a->last_run);*code=1;a->quitting=true;}
    if(a->quitting)printf("PROJECT TREE SELF-TEST: %s\n",*code?"FAILED":"PASSED");
}
static void workspace_tree_test_frame(app *a, int *stage, int *code, const char **capture) {
    bool ok = true;
    switch ((*stage)++) {
    case 0:
        break;
    case 1:
    case 5:
    case 9:
    case 14:
    case 18:
        ok = a->workspace_tree_test_bounds.w > 0;
        test_mouse(a, a->workspace_tree_test_bounds, true);
        break;
    case 2:
    case 6:
    case 10:
    case 15:
    case 19:
        test_mouse(a, a->workspace_tree_test_bounds, false);
        break;
    case 4:
        ok = workspace_tree_test_target(a, "oscillator", 0);
        break;
    case 8:
        ok = workspace_tree_test_target(a, "notes.txt", 0);
        break;
    case 12:
        ok = a->tab == 8 && strstr(active_document(a)->file.saved, "Tree nested preview: α = 42") &&
             !a->loaded && !a->job.running && !a->runner.running;
        refresh_workspace_entries(a);
        ok = ok && workspace_tree_test_find(a, "notes.txt", 0) != SIZE_MAX;
        test_mouse(a, nk_rect(900, 700, 1, 1), false);
        break;
    case 13:
        *capture = "workspace-tree-open.bmp";
        ok = workspace_tree_test_target(a, "models", 0);
        break;
    case 17:
        ok = workspace_tree_test_find(a, "notes.txt", 0) == SIZE_MAX &&
             strstr(active_document(a)->file.saved, "Tree nested preview") &&
             workspace_tree_test_target(a, "references", 1);
        break;
    case 21:
        ok = workspace_tree_test_find(a, "reference.txt", 1) != SIZE_MAX;
        test_mouse(a, nk_rect(900, 700, 1, 1), false);
        break;
    case 22:
        *capture = "workspace-tree-added.bmp";
        a->quitting = true;
        break;
    }
    if (!ok) {
        *code = 1;
        a->quitting = true;
    }
    if (a->quitting)
        printf("WORKSPACE TREE UI: %s (stage %d)\n", *code ? "FAILED" : "PASSED", *stage);
}
