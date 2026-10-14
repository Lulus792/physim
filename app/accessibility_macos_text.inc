typedef struct {unsigned long location,length;} ax_range;
#define AX_NOT_FOUND ((unsigned long)LONG_MAX)
#define AX_RANGE "{_NSRange=QQ}"
/* Each document has its own element generation. Numeric control IDs never
 * participate in document lookup, and held elements remain defunct after removal. */
static const ps_a11y_document *document_live_locked(id object,ps_a11y_native *b) {
 const ps_a11y_document *d=b->model?b->model->document:NULL;
 return b->document && identifier(object)==b->document_identity && d && d->active?d:NULL;
}
static bool document_index_locked(ps_a11y_native *b,const ps_a11y_document *d) {
 return b->document_index.version==d->version || ps_a11y_utf16_build(&b->document_index,d->text,d->length,d->version);
}
static bool document_viewable_locked(ps_a11y_native *b) {
 return b->model && !b->model->suspended && b->document_viewable;
}
static BOOL document_valid(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return NO;
 SDL_LockMutex(b->mutex);bool valid=document_live_locked(object,b)!=NULL;SDL_UnlockMutex(b->mutex);return valid;
}
static BOOL document_enabled(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return NO;
 SDL_LockMutex(b->mutex);bool enabled=document_live_locked(object,b) && document_viewable_locked(b);SDL_UnlockMutex(b->mutex);return enabled;
}
static id document_role(id object,SEL selector){return document_valid(object,selector)?string("AXStaticText"):nil;}
static id document_label(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return nil;
 SDL_LockMutex(b->mutex);const ps_a11y_document *d=document_live_locked(object,b);id value=d?string(d->name):nil;SDL_UnlockMutex(b->mutex);return value;
}
static id document_value(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return nil;
 SDL_LockMutex(b->mutex);const ps_a11y_document *d=document_live_locked(object,b);id value=d?string(d->text):nil;SDL_UnlockMutex(b->mutex);return value;
}
static id document_identifier(id object,SEL selector) {
 (void)selector;char name[96];snprintf(name,sizeof name,"physim-document-%llu",(unsigned long long)identifier(object));return string(name);
}
static id document_parent(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return nil;
 SDL_LockMutex(b->mutex);id view=document_live_locked(object,b)?send0(b->view,"retain"):nil;SDL_UnlockMutex(b->mutex);
 return view?send0(view,"autorelease"):nil;
}
static id document_children(id object,SEL selector){(void)object;(void)selector;return send0((id)objc_getClass("NSArray"),"array");}
static CGRect document_frame(id object,SEL selector) {
 if(!document_enabled(object,selector))return CGRectZero;
 return ((CGRect(*)(id,SEL))base_frame)(object,sel_registerName("accessibilityFrame"));
}
static long document_count(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return 0;long count=0;
 SDL_LockMutex(b->mutex);const ps_a11y_document *d=document_live_locked(object,b);
 if(d && document_index_locked(b,d))count=(long)b->document_index.count;SDL_UnlockMutex(b->mutex);return count;
}
static id document_string_range(id object,SEL selector,ax_range range) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return nil;id result=nil;size_t begin,end;
 SDL_LockMutex(b->mutex);const ps_a11y_document *d=document_live_locked(object,b);
 if(d && document_index_locked(b,d) && ps_a11y_utf16_range(&b->document_index,range.location,range.length,&begin,&end)) {
  id value=string(b->document_index.text);
  result=((id(*)(id,SEL,ax_range))objc_msgSend)(value,sel_registerName("substringWithRange:"),range);
 }
 SDL_UnlockMutex(b->mutex);return result;
}
static id document_attributed_range(id object,SEL selector,ax_range range) {
 id value=document_string_range(object,selector,range);if(!value)return nil;
 id result=((id(*)(id,SEL,id))objc_msgSend)(send0((id)objc_getClass("NSAttributedString"),"alloc"),sel_registerName("initWithString:"),value);
 return send0(result,"autorelease");
}
static ax_range document_selected_range(id object,SEL selector){(void)object;(void)selector;return (ax_range){AX_NOT_FOUND,0};}
static ax_range document_visible_range(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);ax_range result={0};if(!b)return result;
 SDL_LockMutex(b->mutex);const ps_a11y_document *d=document_live_locked(object,b);
 if(d && document_viewable_locked(b) && document_index_locked(b,d) && d->count) {
  size_t begin=d->length,end=0,start,finish;
  for(size_t i=0;i<d->count;i++){if(d->boxes[i].begin<begin)begin=d->boxes[i].begin;if(d->boxes[i].end>end)end=d->boxes[i].end;}
  if(ps_a11y_utf16_offset(&b->document_index,begin,&start) && ps_a11y_utf16_offset(&b->document_index,end,&finish))result=(ax_range){start,finish-start};
 }
 SDL_UnlockMutex(b->mutex);return result;
}
static void document_reveal(id object,SEL selector,ax_range range) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return;size_t begin,end;
 SDL_LockMutex(b->mutex);const ps_a11y_document *d=document_live_locked(object,b);
 if(d && document_viewable_locked(b) && document_index_locked(b,d) &&
    ps_a11y_utf16_range(&b->document_index,range.location,range.length,&begin,&end))ps_a11y_document_scroll(b->model,d->version,begin);
 SDL_UnlockMutex(b->mutex);
}
static CGRect document_range_frame(id object,SEL selector,ax_range range) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return CGRectZero;
 float viewport[4]={0};CGRect relative=CGRectZero;bool found=false;size_t begin,end;
 SDL_LockMutex(b->mutex);const ps_a11y_document *d=document_live_locked(object,b);
 if(d && document_viewable_locked(b) && document_index_locked(b,d) &&
    ps_a11y_utf16_range(&b->document_index,range.location,range.length,&begin,&end)) {
  memcpy(viewport,d->viewport,sizeof viewport);
  for(size_t i=0;i<d->count;i++) {
   const ps_a11y_text_box *box=&d->boxes[i];if(box->end<=begin || box->begin>=end)continue;
   CGRect rect=CGRectMake(box->bounds[0]-viewport[0],viewport[3]-(box->bounds[1]-viewport[1])-box->bounds[3],box->bounds[2],box->bounds[3]);
   relative=found?CGRectUnion(relative,rect):rect;found=true;
  }
 }
 SDL_UnlockMutex(b->mutex);if(!found)return CGRectZero;
 CGRect frame=document_frame(object,sel_registerName("accessibilityFrame"));
 if(CGRectIsEmpty(frame))return CGRectZero;relative.origin.x+=frame.origin.x;relative.origin.y+=frame.origin.y;return relative;
}
static long document_line_index(id object,SEL selector,long index) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return -1;long line=-1;
 SDL_LockMutex(b->mutex);const ps_a11y_document *d=document_live_locked(object,b);
 if(d && document_index_locked(b,d) && index>=0 && (size_t)index<=b->document_index.count) {
  size_t byte=b->document_index.bytes[index];line=0;for(size_t i=0;i<byte;i++)if(d->text[i]=='\n')line++;
 }
 SDL_UnlockMutex(b->mutex);return line;
}
static ax_range document_line_range(id object,SEL selector,long line) {
 (void)selector;ps_a11y_native *b=owner(object);ax_range result={AX_NOT_FOUND,0};if(!b || line<0)return result;
 SDL_LockMutex(b->mutex);const ps_a11y_document *d=document_live_locked(object,b);
 if(d && document_index_locked(b,d)) {
  size_t begin=0,end=0,start,finish;long current=0;
  while(begin<d->length && current<line){if(d->text[begin++]=='\n')current++;}
  if(current==line){end=begin;while(end<d->length && d->text[end++]!='\n'){}
   if(ps_a11y_utf16_offset(&b->document_index,begin,&start) && ps_a11y_utf16_offset(&b->document_index,end,&finish))result=(ax_range){start,finish-start};}
 }
 SDL_UnlockMutex(b->mutex);return result;
}
static BOOL document_allowed(id object,SEL selector,SEL requested) {
 (void)selector;const char *name=sel_getName(requested);
 if(!strcmp(name,"setAccessibilityVisibleCharacterRange:"))return document_enabled(object,selector);
 return strncmp(name,"setAccessibility",16) && strncmp(name,"accessibilityPerform",20);
}
static bool make_document_class(void) {
 if(document_class)return true;document_class=objc_allocateClassPair(element_class,"PhysimAccessibilityDocument",0);if(!document_class)return false;
 Protocol *protocol=objc_getProtocol("NSAccessibilityNavigableStaticText");if(protocol)class_addProtocol(document_class,protocol);
 class_addMethod(document_class,sel_registerName("isAccessibilityElement"),(IMP)document_valid,PS_AX_BOOL "@:");
 class_addMethod(document_class,sel_registerName("isAccessibilityEnabled"),(IMP)document_enabled,PS_AX_BOOL "@:");
 class_addMethod(document_class,sel_registerName("accessibilityRole"),(IMP)document_role,"@@:");
 class_addMethod(document_class,sel_registerName("accessibilityLabel"),(IMP)document_label,"@@:");
 class_addMethod(document_class,sel_registerName("accessibilityValue"),(IMP)document_value,"@@:");
 class_addMethod(document_class,sel_registerName("accessibilityIdentifier"),(IMP)document_identifier,"@@:");
 class_addMethod(document_class,sel_registerName("accessibilityParent"),(IMP)document_parent,"@@:");
 class_addMethod(document_class,sel_registerName("accessibilityChildren"),(IMP)document_children,"@@:");
 class_addMethod(document_class,sel_registerName("accessibilityFrame"),(IMP)document_frame,"{CGRect={CGPoint=dd}{CGSize=dd}}@:");
 class_addMethod(document_class,sel_registerName("accessibilityNumberOfCharacters"),(IMP)document_count,"q@:");
 class_addMethod(document_class,sel_registerName("accessibilityStringForRange:"),(IMP)document_string_range,"@@:" AX_RANGE);
 class_addMethod(document_class,sel_registerName("accessibilityAttributedStringForRange:"),(IMP)document_attributed_range,"@@:" AX_RANGE);
 class_addMethod(document_class,sel_registerName("accessibilitySelectedTextRange"),(IMP)document_selected_range,AX_RANGE "@:");
 class_addMethod(document_class,sel_registerName("accessibilityVisibleCharacterRange"),(IMP)document_visible_range,AX_RANGE "@:");
 class_addMethod(document_class,sel_registerName("setAccessibilityVisibleCharacterRange:"),(IMP)document_reveal,"v@:" AX_RANGE);
 class_addMethod(document_class,sel_registerName("accessibilityFrameForRange:"),(IMP)document_range_frame,"{CGRect={CGPoint=dd}{CGSize=dd}}@:" AX_RANGE);
 class_addMethod(document_class,sel_registerName("accessibilityLineForIndex:"),(IMP)document_line_index,"q@:q");
 class_addMethod(document_class,sel_registerName("accessibilityRangeForLine:"),(IMP)document_line_range,AX_RANGE "@:q");
 class_addMethod(document_class,sel_registerName("isAccessibilitySelectorAllowed:"),(IMP)document_allowed,PS_AX_BOOL "@::");
 objc_registerClassPair(document_class);return true;
}
/* UI thread owns publication; caller holds the model mutex. */
static bool document_publish_locked(ps_a11y_native *b,id roots) {
 const ps_a11y_document *d=b->model->document;
 if(!d || !d->active) {
  if(b->document){send1(b->document,"setAccessibilityParent:",nil);send0(b->document,"release");b->document=nil;}
  b->document_version=0;ps_a11y_utf16_clear(&b->document_index);return false;
 }
 if(!b->document) {
  if(b->document_identity==UINT64_MAX)return false;
  b->document=send0((id)document_class,"new");if(!b->document)return false;
  ++b->document_identity;SDL_AddAtomicInt(&b->references,1);
  memcpy((char*)b->document+ivar_getOffset(owner_ivar),&b,sizeof b);
  memcpy((char*)b->document+ivar_getOffset(id_ivar),&b->document_identity,sizeof b->document_identity);
 }
 send1(b->document,"setAccessibilityParent:",b->view);send1(roots,"addObject:",b->document);
 int height;SDL_GetWindowSize(b->window,NULL,&height);BOOL flipped=((BOOL(*)(id,SEL))objc_msgSend)(b->view,sel_registerName("isFlipped"));
 CGRect frame=CGRectMake(d->viewport[0],flipped?d->viewport[1]:height-d->viewport[1]-d->viewport[3],d->viewport[2],d->viewport[3]);
 ((void(*)(id,SEL,CGRect))objc_msgSend)(b->document,sel_registerName("setAccessibilityFrameInParentSpace:"),frame);
 bool changed=b->document_version!=d->version;b->document_version=d->version;return changed;
}
