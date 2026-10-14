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
static bool text_range_bounds(ps_a11y_native *w,size_t begin,size_t end,unsigned coordinate,int out[4]) {
    double dx,dy;if(!text_coordinate(w,coordinate,&dx,&dy))return false;
    memset(out,0,4*sizeof *out);
    const ps_a11y_document *d=w->model->document;
    if(!w->visible || w->model->suspended || !d || !d->active)return true;
    double left=0,top=0,right=0,bottom=0;bool found=false;
    for(size_t i=0;i<d->count;i++) {
        const ps_a11y_text_box *box=&d->boxes[i];if(box->end<=begin || box->begin>=end)continue;
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
static const char *document_text_method(ps_a11y_native *w,DBusMessage *message,const char *method,ax_reply *r) {
    const char *failure=NULL;dbus_int32_t first=0,last=0;dbus_uint32_t type=0;
    SDL_LockMutex(w->mutex);
    if(!document_index(w)){SDL_UnlockMutex(w->mutex);return DBUS_ERROR_NO_MEMORY;}
    const ps_a11y_text_index *index=&w->document_index;size_t begin=0,end=0;
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
        else if(!text_range_bounds(w,begin,end,type,rectangle))failure=DBUS_ERROR_NOT_SUPPORTED;
        else for(size_t i=0;i<4;i++)number(r,&r->root,rectangle[i]);
    } else if(!strcmp(method,"GetOffsetAtPoint") && dbus_message_has_signature(message,"iiu")) {
        dbus_message_get_args(message,NULL,DBUS_TYPE_INT32,&first,DBUS_TYPE_INT32,&last,DBUS_TYPE_UINT32,&type,DBUS_TYPE_INVALID);
        double dx,dy;int offset=-1;
        if(!text_coordinate(w,type,&dx,&dy))failure=DBUS_ERROR_NOT_SUPPORTED;
        else if(w->visible && !w->model->suspended) {
            const ps_a11y_document *d=w->model->document;
            for(size_t i=0;i<d->count;i++) {
                const ps_a11y_text_box *box=&d->boxes[i];double x=box->bounds[0]+dx,y=box->bounds[1]+dy;
                if(first>=x && last>=y && first<x+box->bounds[2] && last<y+box->bounds[3] &&
                   ps_a11y_text_byte_offset(index,box->begin,&offset))break;
            }
        }
        if(!failure)number(r,&r->root,offset);
    } else if(!strcmp(method,"ScrollSubstringTo") && dbus_message_has_signature(message,"iiu")) {
        dbus_message_get_args(message,NULL,DBUS_TYPE_INT32,&first,DBUS_TYPE_INT32,&last,DBUS_TYPE_UINT32,&type,DBUS_TYPE_INVALID);
        if(!ps_a11y_text_range(index,first,last,&begin,&end) || begin==end || type>6)failure=DBUS_ERROR_INVALID_ARGS;
        else boolean(r,&r->root,type!=4 && type!=5 && w->visible && !w->model->suspended &&
            ps_a11y_document_scroll_align(w->model,index->version,begin,type==0 || type==2));
    } else if(!strcmp(method,"GetNSelections") && dbus_message_has_signature(message,""))number(r,&r->root,0);
    else if(!strcmp(method,"GetSelection") && dbus_message_has_signature(message,"i"))failure=DBUS_ERROR_INVALID_ARGS;
    else if((!strcmp(method,"SetCaretOffset") || !strcmp(method,"RemoveSelection")) && dbus_message_has_signature(message,"i"))boolean(r,&r->root,false);
    else if(!strcmp(method,"AddSelection") && dbus_message_has_signature(message,"ii"))boolean(r,&r->root,false);
    else if(!strcmp(method,"SetSelection") && dbus_message_has_signature(message,"iii"))boolean(r,&r->root,false);
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
    SDL_UnlockMutex(w->mutex);return failure;
}
