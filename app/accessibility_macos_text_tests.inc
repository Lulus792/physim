/* Actual AppKit objects, including NSRange/CGRect ABI on Intel and ARM64. */
static bool mac_document_test(SDL_Window *window,bool range_control) {
 id pool=send0(send0((id)objc_getClass("NSAutoreleasePool"),"alloc"),"init");
 ps_a11y_model *model=malloc(sizeof *model);SDL_Mutex *mutex=SDL_CreateMutex();
 if(!model || !mutex){free(model);if(mutex)SDL_DestroyMutex(mutex);send0(pool,"drain");return false;}
 ps_a11y_init(model);ps_a11y_native *b=ps_a11y_native_create(window,model,mutex);
 if(!b){SDL_DestroyMutex(mutex);free(model);send0(pool,"drain");return false;}
 const char *source="A θ🌍\nLast\n";float view[]={10,20,200,70},box[]={34,20,12,17};
 double collision_value=5;
 ps_a11y_begin(model);
 if(range_control)ps_a11y_record_range(model,"controls","Collision guard",box,true,0,10,1,&collision_value);
 else ps_a11y_record(model,"controls","Collision guard",PS_A11Y_BUTTON,box,true);
 bool ok=ps_a11y_document_begin(model,1,"Lesetext θ🌍",source,view);
 ok &= ps_a11y_document_box(model,4,8,box) && ps_a11y_publish(model);ps_a11y_native_publish(b,true);
 id document=send0(b->document,"retain");
 ok &= identifier(document)==model->nodes[0].id;
 ok &= !((BOOL(*)(id,SEL))objc_msgSend)(document,sel_registerName("accessibilityPerformPress"));
 ((void(*)(id,SEL,BOOL))objc_msgSend)(document,sel_registerName("setAccessibilityFocused:"),YES);
 ok &= !model->pending_press && !model->pending_focus;
 id number=((id(*)(id,SEL,double))objc_msgSend)((id)objc_getClass("NSNumber"),sel_registerName("numberWithDouble:"),7.0);
 send1(document,"setAccessibilityValue:",number);
 ok &= !model->pending_value_id && collision_value==5;
 /* AXValue writability is verified by the independent AXUIElement client.
  * NSAccessibilityElement does not implement the deprecated attribute selector. */
 const char *role=((const char*(*)(id,SEL))objc_msgSend)(send0(document,"accessibilityRole"),sel_registerName("UTF8String"));
 id value=send0(document,"accessibilityValue");
 ok &= role && !strcmp(role,"AXStaticText") &&
  !strcmp(((const char*(*)(id,SEL))objc_msgSend)(value,sel_registerName("UTF8String")),source) &&
  ((long(*)(id,SEL))objc_msgSend)(document,sel_registerName("accessibilityNumberOfCharacters"))==11;
 ax_range range={2,3};id part=((id(*)(id,SEL,ax_range))objc_msgSend)(document,sel_registerName("accessibilityStringForRange:"),range);
 const char *utf8=((const char*(*)(id,SEL))objc_msgSend)(part,sel_registerName("UTF8String"));ok &= utf8 && !strcmp(utf8,"θ🌍");
 ok &= !((id(*)(id,SEL,ax_range))objc_msgSend)(document,sel_registerName("accessibilityStringForRange:"),(ax_range){4,1});
 ok &= !((id(*)(id,SEL,ax_range))objc_msgSend)(document,sel_registerName("accessibilityStringForRange:"),(ax_range){AX_NOT_FOUND,1});
 ax_range visible=((ax_range(*)(id,SEL))objc_msgSend)(document,sel_registerName("accessibilityVisibleCharacterRange"));
 ok &= visible.location==3 && visible.length==2;
 ax_range line=((ax_range(*)(id,SEL,long))objc_msgSend)(document,sel_registerName("accessibilityRangeForLine:"),1);
 ok &= line.location==6 && line.length==5 &&
  ((long(*)(id,SEL,long))objc_msgSend)(document,sel_registerName("accessibilityLineForIndex:"),9)==1;
 SEL frame_selector=sel_registerName("accessibilityFrameForRange:");
 IMP frame_method=method_getImplementation(class_getInstanceMethod(object_getClass(document),frame_selector));
 CGRect frame=((CGRect(*)(id,SEL,ax_range))frame_method)(document,frame_selector,(ax_range){3,2});
 ok &= frame.size.width==12 && frame.size.height==17;
 ok &= !((BOOL(*)(id,SEL,SEL))objc_msgSend)(document,sel_registerName("isAccessibilitySelectorAllowed:"),sel_registerName("setAccessibilityValue:"));
 ((void(*)(id,SEL,ax_range))objc_msgSend)(document,sel_registerName("setAccessibilityVisibleCharacterRange:"),(ax_range){6,4});
 ok &= model->document_scroll_version==1 && model->document_scroll_byte==9;
 model->document_scroll_version=0;SDL_HideWindow(window);ps_a11y_native_publish(b,false);
 frame=((CGRect(*)(id,SEL,ax_range))frame_method)(document,frame_selector,(ax_range){3,2});
 ok &= CGRectIsEmpty(frame) && send0(document,"accessibilityValue") && !document_enabled(document,sel_registerName("isAccessibilityEnabled"));
 ((void(*)(id,SEL,ax_range))objc_msgSend)(document,sel_registerName("setAccessibilityVisibleCharacterRange:"),(ax_range){6,4});ok &= !model->document_scroll_version;
 SDL_ShowWindow(window);ps_a11y_begin(model);ps_a11y_publish(model);ps_a11y_native_publish(b,true);
 ok &= !document_valid(document,sel_registerName("isAccessibilityElement")) && !send0(document,"accessibilityValue");
 ps_a11y_begin(model);ok &= ps_a11y_document_begin(model,2,"Neu","β",view);ps_a11y_publish(model);ps_a11y_native_publish(b,true);
 ok &= b->document!=document && !document_valid(document,sel_registerName("isAccessibilityElement"));
 id replacement=send0(b->document,"retain");ps_a11y_native_destroy(b);
 ok &= !send0(replacement,"accessibilityValue");send0(replacement,"release");send0(document,"release");
 ps_a11y_destroy(model);free(model);send0(pool,"drain");
 if(ok)puts("MAC DOCUMENT: UTF-16 ranges, actual AppKit text/geometry, hidden rejection, generations and retained teardown passed");
 return ok;
}
bool ps_a11y_native_document_read_test(ps_a11y_native *b) {
 if(!b)return false;char *expected=NULL;id document=nil;size_t bytes=0,last=0;
 SDL_LockMutex(b->mutex);const ps_a11y_document *d=b->model?b->model->document:NULL;
 if(d && d->active && b->document && document_index_locked(b,d)) {
  expected=SDL_strdup(d->text);bytes=d->length;size_t end=bytes;
  while(end && (d->text[end-1]=='\n' || d->text[end-1]=='\r' || d->text[end-1]==' '))end--;
  if(end && ps_a11y_utf16_offset(&b->document_index,end-1,&last))document=send0(b->document,"retain");
 }
 SDL_UnlockMutex(b->mutex);if(!document || !expected){SDL_free(expected);return false;}
 id pool=send0(send0((id)objc_getClass("NSAutoreleasePool"),"alloc"),"init");
 id value=send0(document,"accessibilityValue");const char *utf8=((const char*(*)(id,SEL))objc_msgSend)(value,sel_registerName("UTF8String"));
 long count=((long(*)(id,SEL))objc_msgSend)(document,sel_registerName("accessibilityNumberOfCharacters"));
 bool ok=bytes>256*1024 && utf8 && strlen(utf8)==bytes && !memcmp(utf8,expected,bytes) &&
  count==((long(*)(id,SEL))objc_msgSend)(value,sel_registerName("length"));
 id full=((id(*)(id,SEL,ax_range))objc_msgSend)(document,sel_registerName("accessibilityStringForRange:"),(ax_range){0,(unsigned long)count});
 const char *full_bytes=((const char*(*)(id,SEL))objc_msgSend)(full,sel_registerName("UTF8String"));ok &= full_bytes && !strcmp(full_bytes,expected);
 SEL selector=sel_registerName("accessibilityFrameForRange:");IMP frame=method_getImplementation(class_getInstanceMethod(object_getClass(document),selector));
 CGRect rectangle=((CGRect(*)(id,SEL,ax_range))frame)(document,selector,(ax_range){0,1});ok &= rectangle.size.width>0 && rectangle.size.height>0;
 if(ok)((void(*)(id,SEL,ax_range))objc_msgSend)(document,sel_registerName("setAccessibilityVisibleCharacterRange:"),(ax_range){last,1});
 printf("MAC F1 READING: bytes=%zu UTF16=%ld native_value=%d native_range=%d geometry=%d\n",bytes,count,
  utf8 && !strcmp(utf8,expected),full_bytes && !strcmp(full_bytes,expected),rectangle.size.width>0 && rectangle.size.height>0);
 send0(document,"release");SDL_free(expected);send0(pool,"drain");return ok;
}
