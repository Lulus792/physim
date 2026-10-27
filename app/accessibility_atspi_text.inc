/* Included only in the Linux provider. Calls hold server->mutex; text and
 * geometry are read under the model mutex, never through UI-owned buffers. */
static const char *text_xml =
    "<interface name='org.a11y.atspi.Text'>"
    "<property name='version' type='u' access='read'/><property name='CharacterCount' type='i' access='read'/><property name='CaretOffset' type='i' access='read'/>"
    "<method name='GetText'><arg type='i' direction='in'/><arg type='i' direction='in'/><arg type='s' direction='out'/></method>"
    "<method name='GetCharacterAtOffset'><arg type='i' direction='in'/><arg type='i' direction='out'/></method>"
    "<method name='GetStringAtOffset'><arg type='i' direction='in'/><arg type='u' direction='in'/><arg type='s' direction='out'/><arg type='i' direction='out'/><arg type='i' direction='out'/></method>"
    "<method name='GetTextAtOffset'><arg type='i' direction='in'/><arg type='u' direction='in'/><arg type='s' direction='out'/><arg type='i' direction='out'/><arg type='i' direction='out'/></method>"
    "<method name='GetTextBeforeOffset'><arg type='i' direction='in'/><arg type='u' direction='in'/><arg type='s' direction='out'/><arg type='i' direction='out'/><arg type='i' direction='out'/></method>"
    "<method name='GetTextAfterOffset'><arg type='i' direction='in'/><arg type='u' direction='in'/><arg type='s' direction='out'/><arg type='i' direction='out'/><arg type='i' direction='out'/></method>"
    "<method name='GetCharacterExtents'><arg type='i' direction='in'/><arg type='u' direction='in'/><arg type='i' direction='out'/><arg type='i' direction='out'/><arg type='i' direction='out'/><arg type='i' direction='out'/></method>"
    "<method name='GetRangeExtents'><arg type='i' direction='in'/><arg type='i' direction='in'/><arg type='u' direction='in'/><arg type='i' direction='out'/><arg type='i' direction='out'/><arg type='i' direction='out'/><arg type='i' direction='out'/></method>"
    "<method name='GetOffsetAtPoint'><arg type='i' direction='in'/><arg type='i' direction='in'/><arg type='u' direction='in'/><arg type='i' direction='out'/></method>"
    "<method name='GetNSelections'><arg type='i' direction='out'/></method>"
    "<method name='GetSelection'><arg type='i' direction='in'/><arg type='i' direction='out'/><arg type='i' direction='out'/></method>"
    "<method name='SetCaretOffset'><arg type='i' direction='in'/><arg type='b' direction='out'/></method>"
    "<method name='AddSelection'><arg type='i' direction='in'/><arg type='i' direction='in'/><arg type='b' direction='out'/></method>"
    "<method name='RemoveSelection'><arg type='i' direction='in'/><arg type='b' direction='out'/></method>"
    "<method name='SetSelection'><arg type='i' direction='in'/><arg type='i' direction='in'/><arg type='i' direction='in'/><arg type='b' direction='out'/></method>"
    "<method name='GetDefaultAttributes'><arg type='a{ss}' direction='out'/></method>"
    "<method name='GetDefaultAttributeSet'><arg type='a{ss}' direction='out'/></method>"
    "<method name='GetAttributeValue'><arg type='i' direction='in'/><arg type='s' direction='in'/><arg type='s' direction='out'/></method>"
    "<method name='GetAttributes'><arg type='i' direction='in'/><arg type='a{ss}' direction='out'/><arg type='i' direction='out'/><arg type='i' direction='out'/></method>"
    "<method name='GetAttributeRun'><arg type='i' direction='in'/><arg type='b' direction='in'/><arg type='a{ss}' direction='out'/><arg type='i' direction='out'/><arg type='i' direction='out'/></method>"
    "<method name='ScrollSubstringTo'><arg type='i' direction='in'/><arg type='i' direction='in'/><arg type='u' direction='in'/><arg type='b' direction='out'/></method>"
    "</interface>";
static const char *editable_text_xml =
    "<interface name='org.a11y.atspi.EditableText'><property name='version' type='u' access='read'/>"
    "<method name='SetTextContents'><arg type='s' direction='in'/><arg type='b' direction='out'/></method>"
    "<method name='InsertText'><arg type='i' direction='in'/><arg type='s' direction='in'/><arg type='i' direction='in'/><arg type='b' direction='out'/></method>"
    "<method name='DeleteText'><arg type='i' direction='in'/><arg type='i' direction='in'/><arg type='b' direction='out'/></method>"
    "<method name='CopyText'><arg type='i' direction='in'/><arg type='i' direction='in'/></method>"
    "<method name='CutText'><arg type='i' direction='in'/><arg type='i' direction='in'/><arg type='b' direction='out'/></method>"
    "<method name='PasteText'><arg type='i' direction='in'/><arg type='b' direction='out'/></method></interface>";
static bool edit_replace(ps_a11y_native *w,const ps_a11y_node *edit,size_t begin,size_t end,const char *insert,size_t bytes) {
    size_t length=ps_a11y_node_text_size(edit);
    if(edit->text_read_only || begin>end || end>length || bytes>PS_A11Y_LABEL_BYTES || length-(end-begin)+bytes>=edit->text_capacity)return false;
    char next[PS_A11Y_LABEL_BYTES];memcpy(next,ps_a11y_node_text(edit),begin);memcpy(next+begin,insert,bytes);
    memcpy(next+begin+bytes,ps_a11y_node_text(edit)+end,length-end+1);
    return ps_a11y_set_text(w->model,edit->id,next);
}
static bool clipboard_queue(ps_a11y_native *w,const ps_a11y_node *edit,unsigned action,size_t begin,size_t end) {
    ps_a11y_model *m=w->model;
    if(w->clipboard_action || !w->visible || m->suspended || !edit->enabled || (edit->text_read_only && action!=1) || m->pending_text_id ||
       m->pending_selection_id || m->pending_press || m->pending_value_id || (m->pending_focus && m->pending_focus!=edit->id))return false;
    w->clipboard_action=action;w->clipboard_id=edit->id;w->clipboard_version=edit->text_version;
    w->clipboard_begin=begin;w->clipboard_end=end;return true;
}
static const char *editable_text_method(ps_a11y_native *w,const ps_a11y_node *node,DBusMessage *message,const char *method,ax_reply *r) {
    const char *failure=NULL,*value=NULL;dbus_int32_t first=0,last=0,amount=0;bool accepted=false,copy=false;
    SDL_LockMutex(w->mutex);const ps_a11y_node *edit=ps_a11y_find(w->model,node->id);ps_a11y_text_index index={0};
    if(!edit || edit->role!=PS_A11Y_EDIT){SDL_UnlockMutex(w->mutex);return "org.a11y.atspi.Error.Defunct";}
    if(!ps_a11y_text_index_build(&index,ps_a11y_node_text(edit),ps_a11y_node_text_size(edit),edit->text_version)){SDL_UnlockMutex(w->mutex);return DBUS_ERROR_NO_MEMORY;}
    size_t begin=0,end=0;
    bool available=w->visible && !w->model->suspended && edit->enabled && !w->clipboard_action;
    if(!strcmp(method,"SetTextContents") && dbus_message_has_signature(message,"s")) {
        dbus_message_get_args(message,NULL,DBUS_TYPE_STRING,&value,DBUS_TYPE_INVALID);
        accepted=available && ps_a11y_set_text(w->model,edit->id,value);
    } else if(!strcmp(method,"InsertText") && dbus_message_has_signature(message,"isi")) {
        dbus_message_get_args(message,NULL,DBUS_TYPE_INT32,&first,DBUS_TYPE_STRING,&value,DBUS_TYPE_INT32,&amount,DBUS_TYPE_INVALID);
        /* The protocol length is a UTF-8 byte count, while position is a scalar
         * offset. Reject a prefix which would split a multibyte character. */
        size_t length=strlen(value),bytes=amount<0 || (size_t)amount>=length?length:(size_t)amount;
        if(amount<-1 || !ps_a11y_text_range(&index,first,first,&begin,&end))failure=DBUS_ERROR_INVALID_ARGS;
        else if(bytes<length && ((unsigned char)value[bytes]&0xc0)==0x80)failure=DBUS_ERROR_INVALID_ARGS;
        else accepted=available && edit_replace(w,edit,begin,end,value,bytes);
    } else if((!strcmp(method,"DeleteText") || !strcmp(method,"CopyText") || !strcmp(method,"CutText")) && dbus_message_has_signature(message,"ii")) {
        dbus_message_get_args(message,NULL,DBUS_TYPE_INT32,&first,DBUS_TYPE_INT32,&last,DBUS_TYPE_INVALID);
        copy=!strcmp(method,"CopyText");
        if(!ps_a11y_text_range(&index,first,last,&begin,&end))failure=DBUS_ERROR_INVALID_ARGS;
        else if(!strcmp(method,"DeleteText"))accepted=available && edit_replace(w,edit,begin,end,"",0);
        else accepted=available && clipboard_queue(w,edit,copy?1:2,begin,end);
        if(copy && !failure && !accepted)failure=DBUS_ERROR_NOT_SUPPORTED;
    } else if(!strcmp(method,"PasteText") && dbus_message_has_signature(message,"i")) {
        dbus_message_get_args(message,NULL,DBUS_TYPE_INT32,&first,DBUS_TYPE_INVALID);
        if(!ps_a11y_text_range(&index,first,first,&begin,&end))failure=DBUS_ERROR_INVALID_ARGS;
        else accepted=available && clipboard_queue(w,edit,3,begin,end);
    } else failure=DBUS_ERROR_INVALID_ARGS;
    if(!failure && !copy)boolean(r,&r->root,accepted);
    ps_a11y_text_index_clear(&index);SDL_UnlockMutex(w->mutex);return failure;
}
/* Clipboard APIs run on the UI thread. Drop both provider/model locks before
 * consulting the desktop, then validate the owner and version once again. */
static void clipboard_deliver(ps_a11y_native *w) {
    if(!w->clipboard_action)return;
    unsigned action=w->clipboard_action;uint64_t id=w->clipboard_id,version=w->clipboard_version;
    size_t begin=w->clipboard_begin,end=w->clipboard_end;w->clipboard_action=0;
    char *selected=NULL;bool available=false;
    SDL_LockMutex(w->mutex);const ps_a11y_node *edit=ps_a11y_find(w->model,id);
    if(edit && edit->role==PS_A11Y_EDIT && edit->text_version==version && edit->enabled && (!edit->text_read_only || action==1) && w->visible &&
       !w->model->suspended && !w->model->pending_text_id && !w->model->pending_selection_id && begin<=end && end<=ps_a11y_node_text_size(edit)) {
        selected=malloc(end-begin+1);
        if(selected){memcpy(selected,ps_a11y_node_text(edit)+begin,end-begin);selected[end-begin]=0;available=true;}
    }
    SDL_UnlockMutex(w->mutex);if(!available)return;
    SDL_UnlockMutex(w->server->mutex);
    char *paste=action==3?SDL_GetClipboardText():NULL;
    bool copied=action==3 || SDL_SetClipboardText(selected);
    SDL_LockMutex(w->server->mutex);SDL_LockMutex(w->mutex);edit=ps_a11y_find(w->model,id);
    if(copied && edit && edit->role==PS_A11Y_EDIT && edit->text_version==version && edit->enabled &&
       w->visible && !w->model->suspended) {
        if(action==2)edit_replace(w,edit,begin,end,"",0);
        if(action==3 && paste && ps_text_valid(paste,PS_A11Y_LABEL_BYTES,false))edit_replace(w,edit,begin,end,paste,strlen(paste));
    }
    SDL_UnlockMutex(w->mutex);SDL_free(paste);free(selected);
}
static void text_slice(ax_reply *r,const ps_a11y_text_index *index,int start,int end) {
    size_t begin,finish;
    if(!ps_a11y_text_range(index,start,end,&begin,&finish)){r->okay=false;return;}
    char *copy=malloc(finish-begin+1);if(!copy){r->okay=false;return;}
    memcpy(copy,index->text+begin,finish-begin);copy[finish-begin]=0;text(r,&r->root,copy);free(copy);
}
static void text_attributes(ax_reply *r) {
    DBusMessageIter array;open_container(r,&r->root,DBUS_TYPE_ARRAY,"{ss}",&array);close_container(r,&r->root,&array);
}
static bool text_coordinate(ps_a11y_native *w,unsigned coordinate,double *x,double *y) {
    if(coordinate>2 || (!coordinate && !w->screen_position))return false;
    *x=coordinate==0?w->x:0;*y=coordinate==0?w->y:0;return true;
}
static bool text_range_bounds(ps_a11y_native *w,const ps_a11y_node *edit,size_t begin,size_t end,unsigned coordinate,int out[4]) {
    double dx,dy;if(!text_coordinate(w,coordinate,&dx,&dy))return false;
    memset(out,0,4*sizeof *out);
    const ps_a11y_document *d=w->model->document;
    if(!w->visible || w->model->suspended || (!edit && (!d || !d->active)))return true;
    const ps_a11y_text_box *boxes=edit?w->model->edit_boxes+edit->box_offset:d->boxes;
    size_t count=edit?edit->box_count:d->count;
    double left=0,top=0,right=0,bottom=0;bool found=false;
    for(size_t i=0;i<count;i++) {
        const ps_a11y_text_box *box=&boxes[i];if(box->end<=begin || box->begin>=end)continue;
        double x=box->bounds[0]+dx,y=box->bounds[1]+dy,x2=x+box->bounds[2],y2=y+box->bounds[3];
        if(!found){left=x;top=y;right=x2;bottom=y2;found=true;}
        else {left=fmin(left,x);top=fmin(top,y);right=fmax(right,x2);bottom=fmax(bottom,y2);}
    }
    if(found) {
        double values[]={floor(left),floor(top),ceil(right)-floor(left),ceil(bottom)-floor(top)};
        for(size_t i=0;i<4;i++)if(!isfinite(values[i]) || values[i]<INT_MIN || values[i]>INT_MAX)return false;
        for(size_t i=0;i<4;i++)out[i]=(int)values[i];
    }
    return true;
}
static const char *native_text_method(ps_a11y_native *w,const ps_a11y_node *node,DBusMessage *message,const char *method,ax_reply *r) {
    const char *failure=NULL;dbus_int32_t first=0,last=0;dbus_uint32_t type=0;
    SDL_LockMutex(w->mutex);
    ps_a11y_text_index local={0};const ps_a11y_node *edit=NULL;
    const ps_a11y_text_index *index=&w->document_index;
    if(node->role==PS_A11Y_EDIT) {
        edit=ps_a11y_find(w->model,node->id);
        if(!edit){SDL_UnlockMutex(w->mutex);return "org.a11y.atspi.Error.Defunct";}
        if(!ps_a11y_text_index_build(&local,ps_a11y_node_text(edit),ps_a11y_node_text_size(edit),edit->text_version))failure=DBUS_ERROR_NO_MEMORY;
        index=&local;
    } else if(!document_index(w))failure=DBUS_ERROR_NO_MEMORY;
    if(failure){SDL_UnlockMutex(w->mutex);return failure;}
    size_t begin=0,end=0;
    if(!strcmp(method,"GetText") && dbus_message_has_signature(message,"ii")) {
        dbus_message_get_args(message,NULL,DBUS_TYPE_INT32,&first,DBUS_TYPE_INT32,&last,DBUS_TYPE_INVALID);
        if(!ps_a11y_text_range(index,first,last,&begin,&end))failure=DBUS_ERROR_INVALID_ARGS;
        else text_slice(r,index,first,last);
    } else if(!strcmp(method,"GetCharacterAtOffset") && dbus_message_has_signature(message,"i")) {
        dbus_message_get_args(message,NULL,DBUS_TYPE_INT32,&first,DBUS_TYPE_INVALID);
        if(first<0 || (size_t)first>=index->count)failure=DBUS_ERROR_INVALID_ARGS;
        else number(r,&r->root,(int)index->characters[first]);
    } else if((!strcmp(method,"GetStringAtOffset") || !strcmp(method,"GetTextAtOffset") ||
               !strcmp(method,"GetTextBeforeOffset") || !strcmp(method,"GetTextAfterOffset")) && dbus_message_has_signature(message,"iu")) {
        dbus_message_get_args(message,NULL,DBUS_TYPE_INT32,&first,DBUS_TYPE_UINT32,&type,DBUS_TYPE_INVALID);
        int direction=!strcmp(method,"GetTextBeforeOffset")?-1:!strcmp(method,"GetTextAfterOffset")?1:0;
        if(!strcmp(method,"GetStringAtOffset")) {
            static const unsigned boundaries[]={0,1,3,5,5};
            if(type>4)failure=DBUS_ERROR_INVALID_ARGS;else type=boundaries[type];
        }
        int start,finish;
        if(!failure && !ps_a11y_text_span(index,first,type,direction,&start,&finish))failure=DBUS_ERROR_INVALID_ARGS;
        if(!failure){text_slice(r,index,start,finish);number(r,&r->root,start);number(r,&r->root,finish);}
    } else if((!strcmp(method,"GetCharacterExtents") && dbus_message_has_signature(message,"iu")) ||
              (!strcmp(method,"GetRangeExtents") && dbus_message_has_signature(message,"iiu"))) {
        if(!strcmp(method,"GetCharacterExtents")) {
            dbus_message_get_args(message,NULL,DBUS_TYPE_INT32,&first,DBUS_TYPE_UINT32,&type,DBUS_TYPE_INVALID);
            last=first<INT_MAX?first+1:first;
        } else dbus_message_get_args(message,NULL,DBUS_TYPE_INT32,&first,DBUS_TYPE_INT32,&last,DBUS_TYPE_UINT32,&type,DBUS_TYPE_INVALID);
        int rectangle[4];
        if(!ps_a11y_text_range(index,first,last,&begin,&end) ||
           (!strcmp(method,"GetCharacterExtents") && (size_t)first>=index->count))failure=DBUS_ERROR_INVALID_ARGS;
        else if(!text_range_bounds(w,edit,begin,end,type,rectangle))failure=DBUS_ERROR_NOT_SUPPORTED;
        else for(size_t i=0;i<4;i++)number(r,&r->root,rectangle[i]);
    } else if(!strcmp(method,"GetOffsetAtPoint") && dbus_message_has_signature(message,"iiu")) {
        dbus_message_get_args(message,NULL,DBUS_TYPE_INT32,&first,DBUS_TYPE_INT32,&last,DBUS_TYPE_UINT32,&type,DBUS_TYPE_INVALID);
        double dx,dy;int offset=-1;
        if(!text_coordinate(w,type,&dx,&dy))failure=DBUS_ERROR_NOT_SUPPORTED;
        else if(w->visible && !w->model->suspended) {
            const ps_a11y_document *d=w->model->document;
            const ps_a11y_text_box *boxes=edit?w->model->edit_boxes+edit->box_offset:d->boxes;
            size_t count=edit?edit->box_count:d->count;
            for(size_t i=0;i<count;i++) {
                const ps_a11y_text_box *box=&boxes[i];double x=box->bounds[0]+dx,y=box->bounds[1]+dy;
                if(first>=x && last>=y && first<x+box->bounds[2] && last<y+box->bounds[3] &&
                   ps_a11y_text_byte_offset(index,box->begin,&offset))break;
            }
        }
        if(!failure)number(r,&r->root,offset);
    } else if(!strcmp(method,"ScrollSubstringTo") && dbus_message_has_signature(message,"iiu")) {
        dbus_message_get_args(message,NULL,DBUS_TYPE_INT32,&first,DBUS_TYPE_INT32,&last,DBUS_TYPE_UINT32,&type,DBUS_TYPE_INVALID);
        if(!ps_a11y_text_range(index,first,last,&begin,&end) || (!edit && begin==end) || type>6)failure=DBUS_ERROR_INVALID_ARGS;
        else boolean(r,&r->root,w->visible && !w->model->suspended && (edit?
            ps_a11y_edit_scroll(w->model,edit->id,edit->text_version,begin,end,type!=1 && type!=3 && type!=5):
            type!=4 && type!=5 && ps_a11y_document_scroll_align(w->model,index->version,begin,type==0 || type==2)));
    } else if(!strcmp(method,"GetNSelections") && dbus_message_has_signature(message,""))number(r,&r->root,edit && edit->text_start!=edit->text_end?1:0);
    else if(!strcmp(method,"GetSelection") && dbus_message_has_signature(message,"i")) {
        dbus_message_get_args(message,NULL,DBUS_TYPE_INT32,&first,DBUS_TYPE_INVALID);int start,finish;
        if(!edit || first!=0 || edit->text_start==edit->text_end || !ps_a11y_text_byte_offset(index,edit->text_start,&start) ||
           !ps_a11y_text_byte_offset(index,edit->text_end,&finish))failure=DBUS_ERROR_INVALID_ARGS;
        else {number(r,&r->root,start);number(r,&r->root,finish);}
    } else if(!strcmp(method,"SetCaretOffset") && dbus_message_has_signature(message,"i")) {
        dbus_message_get_args(message,NULL,DBUS_TYPE_INT32,&first,DBUS_TYPE_INVALID);
        boolean(r,&r->root,edit && ps_a11y_text_range(index,first,first,&begin,&end) && w->visible &&
            ps_a11y_edit_select(w->model,edit->id,edit->text_version,begin,begin));
    } else if(!strcmp(method,"RemoveSelection") && dbus_message_has_signature(message,"i")) {
        dbus_message_get_args(message,NULL,DBUS_TYPE_INT32,&first,DBUS_TYPE_INVALID);
        boolean(r,&r->root,edit && first==0 && edit->text_start!=edit->text_end && w->visible &&
            ps_a11y_edit_select(w->model,edit->id,edit->text_version,edit->text_caret,edit->text_caret));
    } else if((!strcmp(method,"AddSelection") && dbus_message_has_signature(message,"ii")) ||
              (!strcmp(method,"SetSelection") && dbus_message_has_signature(message,"iii"))) {
        dbus_int32_t selection=0;
        if(!strcmp(method,"SetSelection"))dbus_message_get_args(message,NULL,DBUS_TYPE_INT32,&selection,DBUS_TYPE_INT32,&first,DBUS_TYPE_INT32,&last,DBUS_TYPE_INVALID);
        else dbus_message_get_args(message,NULL,DBUS_TYPE_INT32,&first,DBUS_TYPE_INT32,&last,DBUS_TYPE_INVALID);
        boolean(r,&r->root,edit && selection==0 && (!strcmp(method,"SetSelection") || edit->text_start==edit->text_end) &&
            ps_a11y_text_range(index,first,last,&begin,&end) && w->visible &&
            ps_a11y_edit_select(w->model,edit->id,edit->text_version,begin,end));
    }
    else if((!strcmp(method,"GetDefaultAttributes") || !strcmp(method,"GetDefaultAttributeSet")) && dbus_message_has_signature(message,""))text_attributes(r);
    else if(!strcmp(method,"GetAttributeValue") && dbus_message_has_signature(message,"is")) {
        const char *attribute;dbus_message_get_args(message,NULL,DBUS_TYPE_INT32,&first,DBUS_TYPE_STRING,&attribute,DBUS_TYPE_INVALID);
        if(first<0 || (size_t)first>index->count)failure=DBUS_ERROR_INVALID_ARGS;else text(r,&r->root,"");
    } else if((!strcmp(method,"GetAttributes") && dbus_message_has_signature(message,"i")) ||
              (!strcmp(method,"GetAttributeRun") && dbus_message_has_signature(message,"ib"))) {
        dbus_bool_t defaults;
        if(!strcmp(method,"GetAttributeRun"))dbus_message_get_args(message,NULL,DBUS_TYPE_INT32,&first,DBUS_TYPE_BOOLEAN,&defaults,DBUS_TYPE_INVALID);
        else dbus_message_get_args(message,NULL,DBUS_TYPE_INT32,&first,DBUS_TYPE_INVALID);
        if(first<0 || (size_t)first>index->count)failure=DBUS_ERROR_INVALID_ARGS;
        else {text_attributes(r);number(r,&r->root,0);number(r,&r->root,(int)index->count);}
    } else failure=DBUS_ERROR_INVALID_ARGS;
    ps_a11y_text_index_clear(&local);SDL_UnlockMutex(w->mutex);return failure;
}
