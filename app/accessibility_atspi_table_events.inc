/* Keep only objects actually queried, plus the table roots. The sparse model
 * remains authoritative; unqueried virtual cells require no event allocation. */
static void ax_watch_store(ax_table_watch *watch,const ps_a11y_node *node) {
    watch->id=node->id;watch->parent=node->parent;watch->role=node->role;
    memcpy(watch->bounds,node->bounds,sizeof watch->bounds);
    watch->enabled=node->enabled;watch->offscreen=node->offscreen;
}
static void ax_table_watch_query(ps_a11y_native *w,const ps_a11y_node *node) {
    if(!ax_table_role(node->role))return;
    for(ax_table_watch *at=w->table_watches;at;at=at->next)if(at->id==node->id)return;
    ax_table_watch *watch=calloc(1,sizeof *watch);if(!watch)return;
    ax_watch_store(watch,node);watch->index=ax_index_locked(w,node);
    watch->document=w->document_identity;watch->next=w->table_watches;w->table_watches=watch;
}
static void ax_parent_path(ps_a11y_native *w,const ps_a11y_node *node,char path[128]) {
    if(ax_is_document_child(node))document_path(path,w->window_id,w->document_identity);
    else path_for(path,w->window_id,node->parent);
}
static void ax_table_child_event(ps_a11y_native *w,const char *parent,const char *operation,size_t index,uint64_t id) {
    event_reference(w->server,parent,"ChildrenChanged",operation,(int)index,w->window_id,id);
}
static void ax_table_events(ps_a11y_native *w,bool destroy,bool moved) {
    if(!destroy)for(size_t i=0;i<ps_a11y_table_count(w->model);i++) {
        ps_a11y_table table;ps_a11y_node node;
        if(!ps_a11y_table_at(w->model,i,&table) || !ax_table_snapshot(w,table.id,&node))continue;
        bool found=false;for(ax_table_watch *at=w->table_watches;at;at=at->next)if(at->id==table.id){found=true;break;}
        if(!found) {
            ax_table_watch_query(w,&node);size_t index=ax_index_locked(w,&node);char parent[128];ax_parent_path(w,&node,parent);
            cache_signal(w->server,w,&node,(int)index,true);ax_table_child_event(w,parent,"add",index,node.id);
        }
    }
    ax_table_watch **at=&w->table_watches;
    while(*at) {
        ax_table_watch *old=*at;ps_a11y_node node;char path[128];path_for(path,w->window_id,old->id);
        if(destroy || !ax_table_snapshot(w,old->id,&node)) {
            ps_a11y_node retired={.id=old->id,.parent=old->parent,.role=old->role};
            cache_signal(w->server,w,&retired,(int)old->index,false);event_state(w->server,path,"defunct",true);
            if(old->role==AX_TABLE_ROLE) {
                char parent[128];document_path(parent,w->window_id,old->document);
                ax_table_child_event(w,parent,"remove",old->index,old->id);
            }
            *at=old->next;free(old);continue;
        }
        if(moved || memcmp(old->bounds,node.bounds,sizeof node.bounds))event_bounds(w->server,path,w,&node);
        if(old->offscreen!=node.offscreen)event_state(w->server,path,"showing",w->visible && !node.offscreen);
        if(old->enabled!=node.enabled) {
            event_state(w->server,path,"enabled",node.enabled);event_state(w->server,path,"sensitive",node.enabled);
            event_state(w->server,path,"visible",w->visible);
        }
        ax_watch_store(old,&node);at=&old->next;
    }
}
