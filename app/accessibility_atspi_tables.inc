/* Numeric IDs are shared with the sparse model; synthetic documents retain
 * their separate object path. No dense matrix or bounded control slot is used. */
static bool ax_table_role(ps_a11y_role role) {
    return role==AX_TABLE_ROLE || role==AX_CELL_ROLE || role==AX_HEADER_ROLE;
}
static bool ax_table_snapshot(ps_a11y_native *w,uint64_t id,ps_a11y_node *node) {
    ps_a11y_table t={0};ps_a11y_table_cell c={0};memset(node,0,sizeof *node);
    if(ps_a11y_table_get(w->model,id,&t)) {
        node->role=AX_TABLE_ROLE;node->document_begin=t.begin;node->document_end=t.end;
        ps_a11y_table_bounds(w->model,id,node->bounds);
        snprintf(node->label,sizeof node->label,"Tabelle: %s",w->model->document->name);
    } else if(ps_a11y_table_cell_find(w->model,id,&c)) {
        node->role=c.column_header?AX_HEADER_ROLE:AX_CELL_ROLE;node->parent=c.table;
        node->document_begin=c.begin;node->document_end=c.end;memcpy(node->bounds,c.bounds,sizeof node->bounds);
    } else return false;
    node->id=id;node->document_version=w->model->document->version;
    node->enabled=w->visible && !w->model->suspended;
    if(!node->enabled)memset(node->bounds,0,sizeof node->bounds);
    node->offscreen=node->bounds[2]<=0 || node->bounds[3]<=0;return true;
}
static ps_a11y_node ax_semantic_node(ps_a11y_native *w,const ps_a11y_node *source) {
    ps_a11y_node node=*source;ps_a11y_table_cell cell;
    if(document_link_valid(w,source) && ps_a11y_table_cell_for_range(w->model,source->document_begin,source->document_end,&cell))node.parent=cell.id;
    return node;
}
static bool ax_is_document_child(const ps_a11y_node *node) {
    return node->role==AX_TABLE_ROLE || (node->role==PS_A11Y_LINK && node->document_version && !node->parent);
}
static bool ax_child_matches(const ps_a11y_node *parent,const ps_a11y_node *child) {
    if(parent->role==AX_DOCUMENT_ROLE)return ax_is_document_child(child);
    return !ax_is_document_child(child) && child->parent==parent->id;
}
static size_t ax_child_count_locked(ps_a11y_native *w,const ps_a11y_node *parent) {
    ps_a11y_table table;
    if(parent->role==AX_TABLE_ROLE && ps_a11y_table_get(w->model,parent->id,&table))return table.rows*table.columns;
    size_t count=0;
    for(size_t i=0;i<w->model->count;i++) {
        ps_a11y_node child=ax_semantic_node(w,&w->model->nodes[i]);
        if(ax_child_matches(parent,&child))count++;
    }
    if(parent->role==AX_DOCUMENT_ROLE)count+=ps_a11y_table_count(w->model);
    if(!parent->id && w->previous_document.document_version && w->model->document && w->model->document->active)
        count++;
    return count;
}
static bool ax_child_id_locked(ps_a11y_native *w,const ps_a11y_node *parent,size_t index,uint64_t *id) {
    ps_a11y_table t;
    if(parent->role==AX_TABLE_ROLE && ps_a11y_table_get(w->model,parent->id,&t)) {
        if(index>=t.rows*t.columns)return false;
        *id=t.id+1+index;return true;
    }
    if(parent->role==AX_DOCUMENT_ROLE) {
        /* Both inventories follow reading order. Merge them without a large
         * stack array on the D-Bus thread's small platform stack. */
        size_t ti=0,li=0;
        for(;;) {
            const ps_a11y_node *link=NULL;
            while(li<w->model->count) {
                const ps_a11y_node *candidate=&w->model->nodes[li];
                ps_a11y_node child=ax_semantic_node(w,candidate);
                if(ax_is_document_child(&child)){link=candidate;break;}
                li++;
            }
            bool table=ps_a11y_table_at(w->model,ti,&t);
            if(!table && !link)return false;
            bool choose_table=table && (!link || t.begin<=link->document_begin);
            uint64_t next=choose_table?t.id:link->id;
            if(!index--){*id=next;return true;}
            if(choose_table)ti++;else li++;
        }
    }
    for(size_t i=0;i<w->model->count;i++) {
        ps_a11y_node child=ax_semantic_node(w,&w->model->nodes[i]);
        if(ax_child_matches(parent,&child) && !index--){*id=child.id;return true;}
    }
    if(!parent->id && index==0 && w->previous_document.document_version && w->model->document && w->model->document->active){*id=0;return true;}
    return false;
}
static size_t ax_index_locked(ps_a11y_native *w,const ps_a11y_node *node) {
    if(node->role==AX_DOCUMENT_ROLE) {
        size_t index=0;ps_a11y_node root={0};
        for(size_t i=0;i<w->model->count;i++) {
            ps_a11y_node child=ax_semantic_node(w,&w->model->nodes[i]);
            if(ax_child_matches(&root,&child))index++;
        }
        return index;
    }
    ps_a11y_table_cell cell;
    if((node->role==AX_CELL_ROLE || node->role==AX_HEADER_ROLE) && ps_a11y_table_cell_find(w->model,node->id,&cell)) {
        ps_a11y_table t;if(ps_a11y_table_get(w->model,cell.table,&t))return cell.row*t.columns+cell.column;
    }
    ps_a11y_node parent={0};
    if(ax_is_document_child(node))parent.role=AX_DOCUMENT_ROLE;
    else if(node->parent && !ax_table_snapshot(w,node->parent,&parent)) {
        const ps_a11y_node *p=ps_a11y_find(w->model,node->parent);if(p)parent=*p;
    }
    size_t count=ax_child_count_locked(w,&parent);
    for(size_t i=0;i<count;i++){uint64_t id;if(ax_child_id_locked(w,&parent,i,&id) && id==node->id)return i;}
    return count;
}
static bool ax_cell_name(ax_reply *r,DBusMessageIter *iter,ps_a11y_native *w,uint64_t id) {
    ps_a11y_table_cell cell;if(!ps_a11y_table_cell_find(w->model,id,&cell))return false;
    size_t length=cell.end-cell.begin;char *name=malloc(length+1);if(!name){r->okay=false;return true;}
    memcpy(name,w->model->document->text+cell.begin,length);name[length]=0;text(r,iter,name);free(name);return true;
}
static const char *ax_table_signature(const char *iface,const char *name) {
    if(!strcmp(iface,AX_PREFIX "Table")) {
        if(!strcmp(name,"Caption") || !strcmp(name,"Summary"))return "(so)";
        if(!strcmp(name,"NRows") || !strcmp(name,"NColumns") || !strcmp(name,"NSelectedRows") || !strcmp(name,"NSelectedColumns"))return "i";
    } else if(!strcmp(iface,AX_PREFIX "TableCell")) {
        if(!strcmp(name,"Position"))return "(ii)";
        if(!strcmp(name,"Table"))return "(so)";
        if(!strcmp(name,"RowSpan") || !strcmp(name,"ColumnSpan"))return "i";
    }
    return NULL;
}
static void ax_table_property(ax_reply *r,DBusMessageIter *iter,ps_a11y_native *w,const ps_a11y_node *node,const char *name) {
    SDL_LockMutex(w->mutex);ps_a11y_table t={0};ps_a11y_table_cell c={0};
    if(node->role==AX_TABLE_ROLE && ps_a11y_table_get(w->model,node->id,&t)) {
        if(!strcmp(name,"Caption") || !strcmp(name,"Summary"))reference(r,iter,"",AX_NULL);
        else number(r,iter,!strcmp(name,"NRows")?(int)t.rows:!strcmp(name,"NColumns")?(int)t.columns:0);
    } else if(ps_a11y_table_cell_find(w->model,node->id,&c)) {
        if(!strcmp(name,"Table"))local_reference(r,iter,w->server,w->window_id,c.table);
        else if(!strcmp(name,"Position")) {
            DBusMessageIter value;open_container(r,iter,DBUS_TYPE_STRUCT,NULL,&value);
            number(r,&value,(int)c.row);number(r,&value,(int)c.column);close_container(r,iter,&value);
        } else number(r,iter,1);
    } else r->okay=false;
    SDL_UnlockMutex(w->mutex);
}
static const char *ax_table_method(ps_a11y_native *w,const ps_a11y_node *node,DBusMessage *message,const char *method,ax_reply *r) {
    const char *failure=NULL;dbus_int32_t first=0,second=0;
    SDL_LockMutex(w->mutex);ps_a11y_table t={0};ps_a11y_table_cell c={0};
    bool table=node->role==AX_TABLE_ROLE;
    if(table?!ps_a11y_table_get(w->model,node->id,&t):!ps_a11y_table_cell_find(w->model,node->id,&c))failure="org.a11y.atspi.Error.Defunct";
    else if(!table) {
        if(!dbus_message_has_signature(message,""))failure=DBUS_ERROR_INVALID_ARGS;
        else if(!strcmp(method,"GetRowColumnSpan")) {
            /* libatspi consumes iiii despite the extra boolean in upstream XML.
             * Match the actual independent client and our introspection. */
            number(r,&r->root,(int)c.row);number(r,&r->root,(int)c.column);number(r,&r->root,1);number(r,&r->root,1);
        } else if(!strcmp(method,"GetColumnHeaderCells") || !strcmp(method,"GetRowHeaderCells")) {
            DBusMessageIter array;open_container(r,&r->root,DBUS_TYPE_ARRAY,"(so)",&array);ps_a11y_table_cell h;
            if(!strcmp(method,"GetColumnHeaderCells") && ps_a11y_table_column_header(w->model,c.table,c.column,&h))
                local_reference(r,&array,w->server,w->window_id,h.id);
            close_container(r,&r->root,&array);
        } else failure=DBUS_ERROR_UNKNOWN_METHOD;
    } else if(dbus_message_has_signature(message,"ii")) {
        dbus_message_get_args(message,NULL,DBUS_TYPE_INT32,&first,DBUS_TYPE_INT32,&second,DBUS_TYPE_INVALID);
        bool valid=first>=0 && second>=0 && ps_a11y_table_cell_get(w->model,t.id,(size_t)first,(size_t)second,&c);
        if(!strcmp(method,"GetAccessibleAt")) {
            if(valid)local_reference(r,&r->root,w->server,w->window_id,c.id);else reference(r,&r->root,"",AX_NULL);
        } else if(!strcmp(method,"GetIndexAt"))number(r,&r->root,valid?(int)(c.row*t.columns+c.column):-1);
        else if(!strcmp(method,"GetRowExtentAt") || !strcmp(method,"GetColumnExtentAt"))number(r,&r->root,valid?1:0);
        else if(!strcmp(method,"IsSelected"))boolean(r,&r->root,false);
        else failure=DBUS_ERROR_UNKNOWN_METHOD;
    } else if(dbus_message_has_signature(message,"i")) {
        dbus_message_get_args(message,NULL,DBUS_TYPE_INT32,&first,DBUS_TYPE_INVALID);
        bool valid=first>=0 && (size_t)first<t.rows*t.columns;
        if(!strcmp(method,"GetRowAtIndex"))number(r,&r->root,valid?(int)((size_t)first/t.columns):-1);
        else if(!strcmp(method,"GetColumnAtIndex"))number(r,&r->root,valid?(int)((size_t)first%t.columns):-1);
        else if(!strcmp(method,"GetRowColumnExtentsAtIndex")) {
            boolean(r,&r->root,valid);number(r,&r->root,valid?(int)((size_t)first/t.columns):-1);number(r,&r->root,valid?(int)((size_t)first%t.columns):-1);
            number(r,&r->root,valid?1:0);number(r,&r->root,valid?1:0);boolean(r,&r->root,false);
        } else if(!strcmp(method,"GetColumnHeader")) {
            if(first>=0 && ps_a11y_table_column_header(w->model,t.id,(size_t)first,&c))local_reference(r,&r->root,w->server,w->window_id,c.id);
            else reference(r,&r->root,"",AX_NULL);
        } else if(!strcmp(method,"GetRowHeader"))reference(r,&r->root,"",AX_NULL);
        else if(!strcmp(method,"GetColumnDescription")) {
            if(first<0 || !ps_a11y_table_column_header(w->model,t.id,(size_t)first,&c))text(r,&r->root,"");else ax_cell_name(r,&r->root,w,c.id);
        } else if(!strcmp(method,"GetRowDescription"))text(r,&r->root,"");
        else if(!strcmp(method,"IsRowSelected") || !strcmp(method,"IsColumnSelected") || !strcmp(method,"AddRowSelection") || !strcmp(method,"AddColumnSelection") || !strcmp(method,"RemoveRowSelection") || !strcmp(method,"RemoveColumnSelection"))boolean(r,&r->root,false);
        else failure=DBUS_ERROR_UNKNOWN_METHOD;
    } else if(dbus_message_has_signature(message,"") && (!strcmp(method,"GetSelectedRows") || !strcmp(method,"GetSelectedColumns"))) {
        DBusMessageIter array;open_container(r,&r->root,DBUS_TYPE_ARRAY,"i",&array);close_container(r,&r->root,&array);
    } else failure=DBUS_ERROR_INVALID_ARGS;
    SDL_UnlockMutex(w->mutex);return failure;
}
