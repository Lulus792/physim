static const char *hypertext_xml=
    "<interface name='org.a11y.atspi.Hypertext'><property name='version' type='u' access='read'/>"
    "<method name='GetNLinks'><arg type='i' direction='out'/></method>"
    "<method name='GetLink'><arg type='i' direction='in'/><arg type='(so)' direction='out'/></method>"
    "<method name='GetLinkIndex'><arg type='i' direction='in'/><arg type='i' direction='out'/></method></interface>";
static const char *hyperlink_xml=
    "<interface name='org.a11y.atspi.Hyperlink'><property name='version' type='u' access='read'/>"
    "<property name='NAnchors' type='i' access='read'/><property name='StartIndex' type='i' access='read'/>"
    "<property name='EndIndex' type='i' access='read'/>"
    "<method name='GetObject'><arg type='i' direction='in'/><arg type='(so)' direction='out'/></method>"
    "<method name='GetURI'><arg type='i' direction='in'/><arg type='s' direction='out'/></method>"
    "<method name='GetIndexRange'><arg type='i' direction='out'/><arg type='i' direction='out'/></method>"
    "<method name='IsValid'><arg type='b' direction='out'/></method></interface>";
static const char *hypertext_method(ps_a11y_native *w,DBusMessage *message,const char *method,ax_reply *r) {
    const char *failure=NULL;dbus_int32_t position=0;
    SDL_LockMutex(w->mutex);
    if(!document_index(w))failure=DBUS_ERROR_NO_MEMORY;
    else if(!strcmp(method,"GetNLinks") && dbus_message_has_signature(message,""))
        number(r,&r->root,(int)document_link_count(w));
    else if(dbus_message_has_signature(message,"i")) {
        dbus_message_get_args(message,NULL,DBUS_TYPE_INT32,&position,DBUS_TYPE_INVALID);
        if(!strcmp(method,"GetLink")) {
            const ps_a11y_node *link=position>=0?document_link_at(w,(size_t)position):NULL;
            if(!link)failure=DBUS_ERROR_INVALID_ARGS;
            else hyperlink_reference(r,&r->root,w->server,w->window_id,link->id);
        } else if(!strcmp(method,"GetLinkIndex")) {
            if(position<0 || (size_t)position>w->document_index.count)failure=DBUS_ERROR_INVALID_ARGS;
            else {
                size_t byte=w->document_index.bytes[position];int found=-1;
                for(size_t i=0;i<document_link_count(w);i++) {
                    const ps_a11y_node *link=document_link_at(w,i);
                    if(link->document_begin<=byte && byte<link->document_end){found=(int)i;break;}
                }
                number(r,&r->root,found);
            }
        } else failure=DBUS_ERROR_UNKNOWN_METHOD;
    } else failure=DBUS_ERROR_INVALID_ARGS;
    SDL_UnlockMutex(w->mutex);return failure;
}
static const char *hyperlink_method(ps_a11y_native *w,const ps_a11y_node *node,DBusMessage *message,const char *method,ax_reply *r) {
    const char *failure=NULL;dbus_int32_t anchor=0;
    SDL_LockMutex(w->mutex);const ps_a11y_node *link=ps_a11y_find(w->model,node->id);
    bool valid=document_link_valid(w,link);
    if(!strcmp(method,"IsValid") && dbus_message_has_signature(message,""))boolean(r,&r->root,valid);
    else if(!valid)failure="org.a11y.atspi.Error.Defunct";
    else if(!strcmp(method,"GetIndexRange") && dbus_message_has_signature(message,"")) {
        int start,finish;
        if(!document_index(w))failure=DBUS_ERROR_NO_MEMORY;
        else if(!ps_a11y_text_byte_offset(&w->document_index,link->document_begin,&start) ||
                !ps_a11y_text_byte_offset(&w->document_index,link->document_end,&finish))failure=DBUS_ERROR_INVALID_ARGS;
        else {number(r,&r->root,start);number(r,&r->root,finish);}
    }
    else if(dbus_message_has_signature(message,"i")) {
        dbus_message_get_args(message,NULL,DBUS_TYPE_INT32,&anchor,DBUS_TYPE_INVALID);
        if(anchor!=0)failure=DBUS_ERROR_INVALID_ARGS;
        else if(!strcmp(method,"GetObject"))local_reference(r,&r->root,w->server,w->window_id,link->id);
        else if(!strcmp(method,"GetURI"))text(r,&r->root,link->document_target);
        else failure=DBUS_ERROR_UNKNOWN_METHOD;
    } else failure=DBUS_ERROR_INVALID_ARGS;
    SDL_UnlockMutex(w->mutex);return failure;
}
