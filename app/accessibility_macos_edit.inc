/* Native single-line fields use NSString UTF-16 ranges; the model owns UTF-8.
 * Requests only enter the normal UI queue, never mutate a live Nuklear edit. */
static const ps_a11y_node *edit_live_locked(id object,ps_a11y_native *b) {
 const ps_a11y_node *n=b->model?ps_a11y_find(b->model,identifier(object)):NULL;
 return n && n->role==PS_A11Y_EDIT?n:NULL;
}
static bool edit_available_locked(ps_a11y_native *b,const ps_a11y_node *n) {
 return n && n->enabled && document_viewable_locked(b);
}
static bool edit_index(const ps_a11y_node *n,ps_a11y_utf16_index *index) {
 return n && ps_a11y_utf16_build(index,n->text_value,strlen(n->text_value),n->text_version);
}
static const char *edit_string_bytes(id value) {
 if(!value || !((BOOL(*)(id,SEL,Class))objc_msgSend)(value,sel_registerName("isKindOfClass:"),objc_getClass("NSString")))return NULL;
 const char *bytes=((const char*(*)(id,SEL))objc_msgSend)(value,sel_registerName("UTF8String"));
 unsigned long length=((unsigned long(*)(id,SEL,unsigned long))objc_msgSend)(value,sel_registerName("lengthOfBytesUsingEncoding:"),4);
 return bytes && strlen(bytes)==length?bytes:NULL;
}
static id edit_role(id object,SEL selector) {(void)selector;ps_a11y_node n;return node_copy(object,&n)?string("AXTextField"):nil;}
static BOOL edit_enabled(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return NO;
 SDL_LockMutex(b->mutex);bool ok=edit_available_locked(b,edit_live_locked(object,b));SDL_UnlockMutex(b->mutex);return ok;
}
static BOOL edit_focused(id object,SEL selector) {return edit_enabled(object,selector) && element_focused(object,selector);}
static void edit_set_focused(id object,SEL selector,BOOL focused) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return;
 SDL_LockMutex(b->mutex);if(edit_available_locked(b,edit_live_locked(object,b))) {
  if(focused)ps_a11y_focus(b->model,identifier(object));else ps_a11y_blur(b->model,identifier(object));
 }SDL_UnlockMutex(b->mutex);
}
static id edit_value(id object,SEL selector) {(void)selector;ps_a11y_node n;return node_copy(object,&n)?string(n.text_value):nil;}
static void edit_set_value(id object,SEL selector,id value) {
 (void)selector;const char *bytes=edit_string_bytes(value);ps_a11y_native *b=owner(object);if(!b || !bytes)return;
 SDL_LockMutex(b->mutex);if(edit_available_locked(b,edit_live_locked(object,b)))ps_a11y_set_text(b->model,identifier(object),bytes);SDL_UnlockMutex(b->mutex);
}
static long edit_count(id object,SEL selector) {
 (void)selector;ps_a11y_node n;ps_a11y_utf16_index index={0};long count=0;
 if(node_copy(object,&n) && edit_index(&n,&index))count=(long)index.count;ps_a11y_utf16_clear(&index);return count;
}
static ax_range edit_selection(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);ax_range range={AX_NOT_FOUND,0};if(!b)return range;
 ps_a11y_utf16_index index={0};size_t start,end;SDL_LockMutex(b->mutex);const ps_a11y_node *n=edit_live_locked(object,b);
 if(edit_available_locked(b,n) && edit_index(n,&index) && ps_a11y_utf16_offset(&index,n->text_start,&start) &&
    ps_a11y_utf16_offset(&index,n->text_end,&end))range=(ax_range){start,end-start};
 SDL_UnlockMutex(b->mutex);ps_a11y_utf16_clear(&index);return range;
}
static id edit_string_range(id object,SEL selector,ax_range range) {
 (void)selector;ps_a11y_node n;ps_a11y_utf16_index index={0};size_t begin,end;id value=nil;
 if(node_copy(object,&n) && edit_index(&n,&index) && ps_a11y_utf16_range(&index,range.location,range.length,&begin,&end))
  value=((id(*)(id,SEL,ax_range))objc_msgSend)(string(index.text),sel_registerName("substringWithRange:"),range);
 ps_a11y_utf16_clear(&index);return value;
}
static id edit_selected_text(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return nil;id value=nil;
 SDL_LockMutex(b->mutex);const ps_a11y_node *n=edit_live_locked(object,b);
 if(edit_available_locked(b,n)) {char bytes[PS_A11Y_LABEL_BYTES];size_t count=n->text_end-n->text_start;
  memcpy(bytes,n->text_value+n->text_start,count);bytes[count]=0;value=string(bytes);
 }SDL_UnlockMutex(b->mutex);return value;
}
static id edit_selection_ranges(id object,SEL selector) {
 ax_range range=edit_selection(object,selector);id array=send0((id)objc_getClass("NSMutableArray"),"array");
 if(range.location!=AX_NOT_FOUND) {
  id value=((id(*)(id,SEL,ax_range))objc_msgSend)((id)objc_getClass("NSValue"),sel_registerName("valueWithRange:"),range);
  send1(array,"addObject:",value);
 }return array;
}
static void edit_set_selection(id object,SEL selector,ax_range range) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return;ps_a11y_utf16_index index={0};size_t begin,end;
 SDL_LockMutex(b->mutex);const ps_a11y_node *n=edit_live_locked(object,b);
 if(edit_available_locked(b,n) && edit_index(n,&index) && ps_a11y_utf16_range(&index,range.location,range.length,&begin,&end))
  ps_a11y_edit_select(b->model,n->id,n->text_version,begin,end);
 SDL_UnlockMutex(b->mutex);ps_a11y_utf16_clear(&index);
}
static void edit_set_selection_ranges(id object,SEL selector,id ranges) {
 if(!ranges || !((BOOL(*)(id,SEL,Class))objc_msgSend)(ranges,sel_registerName("isKindOfClass:"),objc_getClass("NSArray")) ||
    ((unsigned long(*)(id,SEL))objc_msgSend)(ranges,sel_registerName("count"))!=1)return;
 id value=((id(*)(id,SEL,unsigned long))objc_msgSend)(ranges,sel_registerName("objectAtIndex:"),0);
 if(!((BOOL(*)(id,SEL,Class))objc_msgSend)(value,sel_registerName("isKindOfClass:"),objc_getClass("NSValue")))return;
 const char *type=((const char*(*)(id,SEL))objc_msgSend)(value,sel_registerName("objCType"));
 if(type && !strcmp(type,AX_RANGE))edit_set_selection(object,selector,((ax_range(*)(id,SEL))objc_msgSend)(value,sel_registerName("rangeValue")));
}
static ax_range edit_line_range(id object,SEL selector,long line) {
 (void)selector;ps_a11y_node n;ps_a11y_utf16_index index={0};ax_range range={AX_NOT_FOUND,0};
 if(line==0 && node_copy(object,&n) && edit_index(&n,&index))range=(ax_range){0,index.count};
 ps_a11y_utf16_clear(&index);return range;
}
static long edit_line_index(id object,SEL selector,long unit) {
 (void)selector;ps_a11y_node n;ps_a11y_utf16_index index={0};long line=(long)AX_NOT_FOUND;
 if(unit>=0 && node_copy(object,&n) && edit_index(&n,&index) && (unsigned long)unit<=index.count)line=0;
 ps_a11y_utf16_clear(&index);return line;
}
static long edit_caret_line(id object,SEL selector) {
 ps_a11y_node n;return edit_enabled(object,selector) && node_copy(object,&n) && n.text_active?0:(long)AX_NOT_FOUND;
}
static id edit_attributed_range(id object,SEL selector,ax_range range) {
 id value=edit_string_range(object,selector,range);if(!value)return nil;
 id result=((id(*)(id,SEL,id))objc_msgSend)(send0((id)objc_getClass("NSAttributedString"),"alloc"),sel_registerName("initWithString:"),value);
 return send0(result,"autorelease");
}
static void edit_set_selected_text(id object,SEL selector,id value) {
 (void)selector;ps_a11y_native *b=owner(object);const char *bytes=edit_string_bytes(value);if(!b || !bytes)return;
 size_t added=strlen(bytes);SDL_LockMutex(b->mutex);const ps_a11y_node *n=edit_live_locked(object,b);
 if(edit_available_locked(b,n)) {size_t tail=strlen(n->text_value+n->text_end),start=n->text_start;
  if(added<n->text_capacity && start+tail<n->text_capacity-added) {
   char result[PS_A11Y_LABEL_BYTES];memcpy(result,n->text_value,start);memcpy(result+start,bytes,added);
   memcpy(result+start+added,n->text_value+n->text_end,tail+1);ps_a11y_set_text(b->model,n->id,result);
  }
 }SDL_UnlockMutex(b->mutex);
}
static void edit_reveal(id object,SEL selector,ax_range range) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return;ps_a11y_utf16_index index={0};size_t begin,end;
 SDL_LockMutex(b->mutex);const ps_a11y_node *n=edit_live_locked(object,b);
 if(edit_available_locked(b,n) && edit_index(n,&index) && ps_a11y_utf16_range(&index,range.location,range.length,&begin,&end))
  ps_a11y_edit_scroll(b->model,n->id,n->text_version,begin,end,true);
 SDL_UnlockMutex(b->mutex);ps_a11y_utf16_clear(&index);
}
static ax_range edit_visible_range(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);ax_range range={0};if(!b)return range;
 ps_a11y_utf16_index index={0};SDL_LockMutex(b->mutex);const ps_a11y_node *n=edit_live_locked(object,b);
 if(edit_available_locked(b,n) && n->box_count && edit_index(n,&index)) {
  size_t begin=strlen(n->text_value),end=0,start,finish;
  for(size_t i=0;i<n->box_count;i++){const ps_a11y_text_box *box=&b->model->edit_boxes[n->box_offset+i];if(box->begin<begin)begin=box->begin;if(box->end>end)end=box->end;}
  if(ps_a11y_utf16_offset(&index,begin,&start) && ps_a11y_utf16_offset(&index,end,&finish))range=(ax_range){start,finish-start};
 }SDL_UnlockMutex(b->mutex);ps_a11y_utf16_clear(&index);return range;
}
static CGRect edit_range_frame(id object,SEL selector,ax_range range) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return CGRectZero;ps_a11y_utf16_index index={0};
 CGRect relative=CGRectZero;bool found=false;size_t begin,end;SDL_LockMutex(b->mutex);const ps_a11y_node *n=edit_live_locked(object,b);
 if(edit_available_locked(b,n) && edit_index(n,&index) && ps_a11y_utf16_range(&index,range.location,range.length,&begin,&end)) {
  for(size_t i=0;i<n->box_count;i++) {const ps_a11y_text_box *box=&b->model->edit_boxes[n->box_offset+i];
   if(box->end<=begin || box->begin>=end)continue;
   CGRect rect=CGRectMake(box->bounds[0]-n->bounds[0],n->bounds[3]-(box->bounds[1]-n->bounds[1])-box->bounds[3],box->bounds[2],box->bounds[3]);
   relative=found?CGRectUnion(relative,rect):rect;found=true;
  }
  if(begin==end && n->text_active && begin==n->text_caret && n->caret_bounds[2]>0 && n->caret_bounds[3]>0) {
   relative=CGRectMake(n->caret_bounds[0]-n->bounds[0],n->bounds[3]-(n->caret_bounds[1]-n->bounds[1])-n->caret_bounds[3],n->caret_bounds[2],n->caret_bounds[3]);found=true;
  }
 }SDL_UnlockMutex(b->mutex);ps_a11y_utf16_clear(&index);if(!found)return CGRectZero;
 CGRect frame=element_frame(object,sel_registerName("accessibilityFrame"));if(CGRectIsEmpty(frame))return CGRectZero;
 relative.origin.x+=frame.origin.x;relative.origin.y+=frame.origin.y;return relative;
}
static BOOL edit_allowed(id object,SEL selector,SEL requested) {
 if(requested==sel_registerName("setAccessibilityValue:") || requested==sel_registerName("setAccessibilitySelectedText:") ||
    requested==sel_registerName("setAccessibilitySelectedTextRange:") || requested==sel_registerName("setAccessibilitySelectedTextRanges:") || requested==sel_registerName("setAccessibilityVisibleCharacterRange:") ||
    requested==sel_registerName("setAccessibilityFocused:"))return edit_enabled(object,selector);
 return element_allowed(object,selector,requested);
}
static bool make_edit_class(void) {
 if(edit_class)return true;edit_class=objc_allocateClassPair(element_class,"PhysimAccessibilityEdit",0);if(!edit_class)return false;
 class_addMethod(edit_class,sel_registerName("accessibilityRole"),(IMP)edit_role,"@@:");
 class_addMethod(edit_class,sel_registerName("isAccessibilityEnabled"),(IMP)edit_enabled,PS_AX_BOOL "@:");
 class_addMethod(edit_class,sel_registerName("isAccessibilityFocused"),(IMP)edit_focused,PS_AX_BOOL "@:");
 class_addMethod(edit_class,sel_registerName("setAccessibilityFocused:"),(IMP)edit_set_focused,"v@:" PS_AX_BOOL);
 class_addMethod(edit_class,sel_registerName("accessibilityValue"),(IMP)edit_value,"@@:");
 class_addMethod(edit_class,sel_registerName("setAccessibilityValue:"),(IMP)edit_set_value,"v@:@");
 class_addMethod(edit_class,sel_registerName("accessibilityNumberOfCharacters"),(IMP)edit_count,"q@:");
 class_addMethod(edit_class,sel_registerName("accessibilitySelectedTextRange"),(IMP)edit_selection,AX_RANGE "@:");
 class_addMethod(edit_class,sel_registerName("setAccessibilitySelectedTextRange:"),(IMP)edit_set_selection,"v@:" AX_RANGE);
 class_addMethod(edit_class,sel_registerName("accessibilitySelectedTextRanges"),(IMP)edit_selection_ranges,"@@:");
 class_addMethod(edit_class,sel_registerName("setAccessibilitySelectedTextRanges:"),(IMP)edit_set_selection_ranges,"v@:@");
 class_addMethod(edit_class,sel_registerName("accessibilitySelectedText"),(IMP)edit_selected_text,"@@:");
 class_addMethod(edit_class,sel_registerName("setAccessibilitySelectedText:"),(IMP)edit_set_selected_text,"v@:@");
 class_addMethod(edit_class,sel_registerName("accessibilityStringForRange:"),(IMP)edit_string_range,"@@:" AX_RANGE);
 class_addMethod(edit_class,sel_registerName("accessibilityAttributedStringForRange:"),(IMP)edit_attributed_range,"@@:" AX_RANGE);
 class_addMethod(edit_class,sel_registerName("accessibilityRangeForLine:"),(IMP)edit_line_range,AX_RANGE "@:q");
 class_addMethod(edit_class,sel_registerName("accessibilityLineForIndex:"),(IMP)edit_line_index,"q@:q");
 class_addMethod(edit_class,sel_registerName("accessibilityInsertionPointLineNumber"),(IMP)edit_caret_line,"q@:");
 class_addMethod(edit_class,sel_registerName("accessibilityVisibleCharacterRange"),(IMP)edit_visible_range,AX_RANGE "@:");
 class_addMethod(edit_class,sel_registerName("setAccessibilityVisibleCharacterRange:"),(IMP)edit_reveal,"v@:" AX_RANGE);
 class_addMethod(edit_class,sel_registerName("accessibilityFrameForRange:"),(IMP)edit_range_frame,"{CGRect={CGPoint=dd}{CGSize=dd}}@:" AX_RANGE);
 class_addMethod(edit_class,sel_registerName("isAccessibilitySelectorAllowed:"),(IMP)edit_allowed,PS_AX_BOOL "@::");
 objc_registerClassPair(edit_class);return true;
}
