extern id const NSAccessibilityLinkTextAttribute;
static const ps_a11y_node *link_live_locked(id object,ps_a11y_native *b) {
 const ps_a11y_node *n=b->model?ps_a11y_find(b->model,identifier(object)):NULL;
 const ps_a11y_document *d=b->model?b->model->document:NULL;
 return n && n->role==PS_A11Y_LINK && d && d->active && n->document_version==d->version &&
  n->document_begin<n->document_end && n->document_end<=d->length?n:NULL;
}
static bool link_available_locked(ps_a11y_native *b,const ps_a11y_node *n) {
 return n && n->enabled && document_viewable_locked(b);
}
static bool link_range_locked(ps_a11y_native *b,const ps_a11y_node *n,ax_range *range) {
 const ps_a11y_document *d=b->model?b->model->document:NULL;size_t start,end;
 if(!n || !d || !document_index_locked(b,d) || !ps_a11y_utf16_offset(&b->document_index,n->document_begin,&start) ||
    !ps_a11y_utf16_offset(&b->document_index,n->document_end,&end))return false;
 *range=(ax_range){start,end-start};return true;
}
static BOOL link_valid(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return NO;
 SDL_LockMutex(b->mutex);bool valid=link_live_locked(object,b)!=NULL;SDL_UnlockMutex(b->mutex);return valid;
}
static BOOL link_enabled(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return NO;
 SDL_LockMutex(b->mutex);bool enabled=link_available_locked(b,link_live_locked(object,b));SDL_UnlockMutex(b->mutex);return enabled;
}
static id link_role(id object,SEL selector){return link_valid(object,selector)?string("AXLink"):nil;}
static ax_range link_range(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);ax_range range={AX_NOT_FOUND,0};if(!b)return range;
 SDL_LockMutex(b->mutex);link_range_locked(b,link_live_locked(object,b),&range);SDL_UnlockMutex(b->mutex);return range;
}
static id link_value(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return nil;id value=nil;ax_range range;
 SDL_LockMutex(b->mutex);if(link_range_locked(b,link_live_locked(object,b),&range))
  value=((id(*)(id,SEL,ax_range))objc_msgSend)(string(b->document_index.text),sel_registerName("substringWithRange:"),range);
 SDL_UnlockMutex(b->mutex);return value;
}
static id link_url(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return nil;id url=nil;
 SDL_LockMutex(b->mutex);const ps_a11y_node *n=link_live_locked(object,b);
 if(n && n->document_target[0])url=((id(*)(id,SEL,id))objc_msgSend)((id)objc_getClass("NSURL"),sel_registerName("URLWithString:"),string(n->document_target));
 SDL_UnlockMutex(b->mutex);return url;
}
static id link_parent(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return nil;
 SDL_LockMutex(b->mutex);const ps_a11y_node *node=link_live_locked(object,b);
 id parent=node?mac_table_parent_for_link_locked(b,node):nil;if(parent)send0(parent,"retain");SDL_UnlockMutex(b->mutex);
 return parent?send0(parent,"autorelease"):nil;
}
static id link_shared_elements(id object,SEL selector) {
 (void)selector;id array=send0((id)objc_getClass("NSMutableArray"),"array");ps_a11y_native *b=owner(object);if(!b)return array;
 SDL_LockMutex(b->mutex);if(link_live_locked(object,b) && b->document)send1(array,"addObject:",b->document);SDL_UnlockMutex(b->mutex);return array;
}
static CGRect link_frame(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return CGRectZero;ax_range range;id document=nil;
 SDL_LockMutex(b->mutex);const ps_a11y_node *n=link_live_locked(object,b);
 if(n && document_viewable_locked(b) && link_range_locked(b,n,&range) && b->document)document=send0(b->document,"retain");
 SDL_UnlockMutex(b->mutex);if(!document)return CGRectZero;
 CGRect frame=document_range_frame(document,sel_registerName("accessibilityFrameForRange:"),range);send0(document,"release");return frame;
}
static BOOL link_focused(id object,SEL selector) {return link_enabled(object,selector) && element_focused(object,selector);}
static void link_focus(id object,SEL selector,BOOL focused) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return;
 SDL_LockMutex(b->mutex);const ps_a11y_node *n=link_live_locked(object,b);
 if(link_available_locked(b,n)){if(focused)ps_a11y_focus(b->model,n->id);else ps_a11y_blur(b->model,n->id);}
 SDL_UnlockMutex(b->mutex);
}
static BOOL link_press(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return NO;bool accepted=false;
 SDL_LockMutex(b->mutex);const ps_a11y_node *n=link_live_locked(object,b);
 if(link_available_locked(b,n))accepted=ps_a11y_press(b->model,n->id);SDL_UnlockMutex(b->mutex);return accepted;
}
static BOOL link_allowed(id object,SEL selector,SEL requested) {
 if(requested==sel_registerName("accessibilityPerformPress") || requested==sel_registerName("setAccessibilityFocused:"))return link_enabled(object,selector);
 const char *name=sel_getName(requested);return strncmp(name,"setAccessibility",16) && strncmp(name,"accessibilityPerform",20);
}
static bool make_link_class(void) {
 if(link_class)return true;link_class=objc_allocateClassPair(element_class,"PhysimAccessibilityLink",0);if(!link_class)return false;
 class_addMethod(link_class,sel_registerName("isAccessibilityElement"),(IMP)link_valid,PS_AX_BOOL "@:");
 class_addMethod(link_class,sel_registerName("isAccessibilityEnabled"),(IMP)link_enabled,PS_AX_BOOL "@:");
 class_addMethod(link_class,sel_registerName("accessibilityRole"),(IMP)link_role,"@@:");
 class_addMethod(link_class,sel_registerName("accessibilityLabel"),(IMP)link_value,"@@:");
 class_addMethod(link_class,sel_registerName("accessibilityValue"),(IMP)link_value,"@@:");
 class_addMethod(link_class,sel_registerName("setAccessibilityValue:"),(IMP)document_no_value,"v@:@");
 class_addMethod(link_class,sel_registerName("accessibilityURL"),(IMP)link_url,"@@:");
 class_addMethod(link_class,sel_registerName("accessibilityParent"),(IMP)link_parent,"@@:");
 class_addMethod(link_class,sel_registerName("accessibilitySharedCharacterRange"),(IMP)link_range,AX_RANGE "@:");
 class_addMethod(link_class,sel_registerName("accessibilitySharedTextUIElements"),(IMP)link_shared_elements,"@@:");
 class_addMethod(link_class,sel_registerName("accessibilityFrame"),(IMP)link_frame,"{CGRect={CGPoint=dd}{CGSize=dd}}@:");
 class_addMethod(link_class,sel_registerName("isAccessibilityFocused"),(IMP)link_focused,PS_AX_BOOL "@:");
 class_addMethod(link_class,sel_registerName("setAccessibilityFocused:"),(IMP)link_focus,"v@:" PS_AX_BOOL);
 class_addMethod(link_class,sel_registerName("accessibilityPerformPress"),(IMP)link_press,PS_AX_BOOL "@:");
 class_addMethod(link_class,sel_registerName("accessibilityPerformIncrement"),(IMP)document_no_action,PS_AX_BOOL "@:");
 class_addMethod(link_class,sel_registerName("accessibilityPerformDecrement"),(IMP)document_no_action,PS_AX_BOOL "@:");
 class_addMethod(link_class,sel_registerName("isAccessibilitySelectorAllowed:"),(IMP)link_allowed,PS_AX_BOOL "@::");
 objc_registerClassPair(link_class);return true;
}
static id document_link_children(id object,bool visible) {
 ps_a11y_native *b=owner(object);id array=send0((id)objc_getClass("NSMutableArray"),"array");if(!b)return array;
 SDL_LockMutex(b->mutex);const ps_a11y_document *d=document_live_locked(object,b);
 if(d && (!visible || document_viewable_locked(b))) {
  size_t order[PS_A11Y_MAX_NODES],count=0;
  for(size_t i=0;i<b->model->count;i++) {const ps_a11y_node *n=&b->model->nodes[i];
   if(n->role!=PS_A11Y_LINK || n->document_version!=d->version || (visible && n->offscreen))continue;
   size_t at=count;while(at && b->model->nodes[order[at-1]].document_begin>n->document_begin){order[at]=order[at-1];--at;}order[at]=i;count++;
  }
  for(size_t i=0;i<count;i++){id link=element_for(b,b->model->nodes[order[i]].id);if(link)send1(array,"addObject:",link);}
 }SDL_UnlockMutex(b->mutex);return array;
}
static id document_visible_links(id object,SEL selector){(void)selector;return mac_document_children(object,true);}
static id document_link_attributed(id object,ax_range range) {
 ps_a11y_native *b=owner(object);if(!b)return nil;id result=nil;size_t begin,end;
 SDL_LockMutex(b->mutex);const ps_a11y_document *d=document_live_locked(object,b);
 if(d && document_index_locked(b,d) && ps_a11y_utf16_range(&b->document_index,range.location,range.length,&begin,&end)) {
  id value=((id(*)(id,SEL,ax_range))objc_msgSend)(string(b->document_index.text),sel_registerName("substringWithRange:"),range);
  result=((id(*)(id,SEL,id))objc_msgSend)(send0((id)objc_getClass("NSMutableAttributedString"),"alloc"),sel_registerName("initWithString:"),value);
  for(size_t i=0;i<b->model->count;i++){const ps_a11y_node *n=&b->model->nodes[i];ax_range link;
   if(n->role!=PS_A11Y_LINK || n->document_version!=d->version || n->document_begin>=end || n->document_end<=begin || !link_range_locked(b,n,&link))continue;
   id element=element_for(b,n->id);if(!element)continue;
   unsigned long first=link.location>range.location?link.location:range.location;
   unsigned long last=link.location+link.length<range.location+range.length?link.location+link.length:range.location+range.length;
   ((void(*)(id,SEL,id,id,ax_range))objc_msgSend)(result,sel_registerName("addAttribute:value:range:"),NSAccessibilityLinkTextAttribute,element,(ax_range){first-range.location,last-first});
  }
 }SDL_UnlockMutex(b->mutex);return result?send0(result,"autorelease"):nil;
}
static void document_link_publish_locked(ps_a11y_native *b,id roots) {
 const ps_a11y_document *d=b->model->document;if(!b->document || !d || !d->active)return;
 for(size_t i=0;i<b->count;i++){const ps_a11y_node *n=&b->model->nodes[i];if(n->role!=PS_A11Y_LINK || n->document_version!=d->version)continue;
  id link=element_for(b,n->id);send1(roots,"removeObjectIdenticalTo:",link);send1(link,"setAccessibilityParent:",mac_table_parent_for_link_locked(b,n));
  CGRect frame=CGRectMake(n->bounds[0]-d->viewport[0],d->viewport[3]-(n->bounds[1]-d->viewport[1])-n->bounds[3],n->bounds[2],n->bounds[3]);
  ((void(*)(id,SEL,CGRect))objc_msgSend)(link,sel_registerName("setAccessibilityFrameInParentSpace:"),frame);
 }
}
bool ps_a11y_native_document_link_invoked(ps_a11y_native *b,uint64_t version,size_t begin,size_t end) {
 if(!b)return false;bool completed=false;SDL_LockMutex(b->mutex);
 if(b->model && document_viewable_locked(b) && b->completed_link_serial!=UINT64_MAX)
  for(size_t i=0;i<b->model->count;i++){const ps_a11y_node *n=&b->model->nodes[i];
   if(n->role==PS_A11Y_LINK && n->enabled && n->document_version==version && n->document_begin==begin && n->document_end==end) {
    b->completed_link_id=n->id;++b->completed_link_serial;completed=true;break;
   }
  }
 SDL_UnlockMutex(b->mutex);return completed;
}
