static bool mac_links_snapshot(ps_a11y_native *b,uint64_t version,bool enabled,bool tail_visible) {
 const char *text="A θ🌍 end; Tail";float view[]={10,20,200,70},first[]={20,25,9,17},second[]={29,25,12,17},tail[]={20,60,30,17},hidden[]={0,0,0,0};
 ps_a11y_model *m=b->model;ps_a11y_begin(m);bool ok=ps_a11y_document_begin(m,version,"Links",text,view);
 /* Reverse registration order deliberately; the native reading order is text order. */
 ps_a11y_document_link(m,"links","Tail",version,14,18,tail_visible?tail:hidden,enabled);
 ok &= ps_a11y_document_link_target(m,0,"docs/c-guide.md#3-ein-vollstaendiges-experiment-schreiben");
 ps_a11y_document_link(m,"links","θ🌍",version,2,8,first,enabled);ok &= ps_a11y_document_link_target(m,1,"docs/c-guide.md");
 ps_a11y_record(m,"controls","Guard",PS_A11Y_BUTTON,first,true);
 ok &= ps_a11y_document_box(m,2,4,first) && ps_a11y_document_box(m,4,8,second);
 if(tail_visible)ok &= ps_a11y_document_box(m,14,18,tail);
 ps_a11y_publish(m);ps_a11y_native_publish(b,true);return ok;
}
static bool mac_links_test(SDL_Window *window) {
 id pool=send0(send0((id)objc_getClass("NSAutoreleasePool"),"alloc"),"init");
 ps_a11y_model *m=malloc(sizeof *m);SDL_Mutex *mutex=SDL_CreateMutex();
 if(!m || !mutex){free(m);if(mutex)SDL_DestroyMutex(mutex);send0(pool,"drain");return false;}
 ps_a11y_init(m);ps_a11y_native *b=ps_a11y_native_create(window,m,mutex);
 if(!b){SDL_DestroyMutex(mutex);free(m);send0(pool,"drain");return false;}
 bool ok=mac_links_snapshot(b,1,true,false);id tail=send0(element_for(b,m->nodes[0].id),"retain"),link=send0(element_for(b,m->nodes[1].id),"retain"),document=send0(b->document,"retain");
#define MAC_LINK_CHECK(test) do {if(!(test)){fprintf(stderr,"MAC LINK failure line %d: %s\n",__LINE__,#test);ok=false;}} while(0)
 MAC_LINK_CHECK(!strcmp(edit_string_bytes(send0(link,"accessibilityRole")),"AXLink"));
 MAC_LINK_CHECK(!strcmp(edit_string_bytes(send0(link,"accessibilityValue")),"θ🌍"));
 MAC_LINK_CHECK(!strcmp(edit_string_bytes(send0(send0(link,"accessibilityURL"),"relativeString")),"docs/c-guide.md"));
 MAC_LINK_CHECK(send0(link,"accessibilityParent")==document);
 ax_range range=((ax_range(*)(id,SEL))objc_msgSend)(link,sel_registerName("accessibilitySharedCharacterRange"));
 MAC_LINK_CHECK(range.location==2 && range.length==3);
 id children=send0(document,"accessibilityChildren");
 MAC_LINK_CHECK(((unsigned long(*)(id,SEL))objc_msgSend)(children,sel_registerName("count"))==2);
 MAC_LINK_CHECK(((id(*)(id,SEL,unsigned long))objc_msgSend)(children,sel_registerName("objectAtIndex:"),0)==link);
 MAC_LINK_CHECK(((unsigned long(*)(id,SEL))objc_msgSend)(send0(document,"accessibilityVisibleChildren"),sel_registerName("count"))==1);
 MAC_LINK_CHECK(((unsigned long(*)(id,SEL))objc_msgSend)(send0(document,"accessibilitySelectedChildren"),sel_registerName("count"))==0);
 MAC_LINK_CHECK(((unsigned long(*)(id,SEL))objc_msgSend)(send0(b->view,"accessibilityChildren"),sel_registerName("count"))==2);
 id attributed=((id(*)(id,SEL,ax_range))objc_msgSend)(document,sel_registerName("accessibilityAttributedStringForRange:"),(ax_range){0,15});
 ax_range effective={0};id attribute=((id(*)(id,SEL,id,unsigned long,ax_range*))objc_msgSend)(attributed,sel_registerName("attribute:atIndex:effectiveRange:"),NSAccessibilityLinkTextAttribute,3,&effective);
 MAC_LINK_CHECK(attribute==link && effective.location==2 && effective.length==3);
 attribute=((id(*)(id,SEL,id,unsigned long,ax_range*))objc_msgSend)(attributed,sel_registerName("attribute:atIndex:effectiveRange:"),NSAccessibilityLinkTextAttribute,0,NULL);MAC_LINK_CHECK(!attribute);
 id partial=((id(*)(id,SEL,ax_range))objc_msgSend)(document,sel_registerName("accessibilityAttributedStringForRange:"),(ax_range){3,2});
 attribute=((id(*)(id,SEL,id,unsigned long,ax_range*))objc_msgSend)(partial,sel_registerName("attribute:atIndex:effectiveRange:"),NSAccessibilityLinkTextAttribute,0,&effective);
 MAC_LINK_CHECK(attribute==link && effective.location==0 && effective.length==2 && !strcmp(edit_string_bytes(send0(partial,"string")),"🌍"));
 MAC_LINK_CHECK(!((id(*)(id,SEL,ax_range))objc_msgSend)(document,sel_registerName("accessibilityAttributedStringForRange:"),(ax_range){4,1}));
 SEL frame_sel=sel_registerName("accessibilityFrame");IMP method=method_getImplementation(class_getInstanceMethod(object_getClass(link),frame_sel));
 CGRect frame=((CGRect(*)(id,SEL))method)(link,frame_sel);MAC_LINK_CHECK(frame.size.width==21 && frame.size.height==17);
 frame=((CGRect(*)(id,SEL))method)(tail,frame_sel);MAC_LINK_CHECK(CGRectIsEmpty(frame));
 ((void(*)(id,SEL,BOOL))objc_msgSend)(tail,sel_registerName("setAccessibilityFocused:"),YES);MAC_LINK_CHECK(m->pending_focus==identifier(tail));
 MAC_LINK_CHECK(mac_links_snapshot(b,1,true,true));MAC_LINK_CHECK(link_focused(tail,NULL));
 frame=((CGRect(*)(id,SEL))method)(tail,frame_sel);MAC_LINK_CHECK(frame.size.width==30 && frame.size.height==17);
 MAC_LINK_CHECK(((BOOL(*)(id,SEL))objc_msgSend)(link,sel_registerName("accessibilityPerformPress")) && m->pending_press==identifier(link));
 MAC_LINK_CHECK(mac_links_snapshot(b,1,true,true) && !m->pending_press);
 SDL_HideWindow(window);ps_a11y_native_publish(b,false);
 MAC_LINK_CHECK(link_value(link,NULL) && link_range(link,NULL).location==2 && !link_enabled(link,NULL));
 MAC_LINK_CHECK(!((BOOL(*)(id,SEL))objc_msgSend)(link,sel_registerName("accessibilityPerformPress")) && CGRectIsEmpty(link_frame(link,NULL)));
 ((void(*)(id,SEL,BOOL))objc_msgSend)(link,sel_registerName("setAccessibilityFocused:"),YES);MAC_LINK_CHECK(!m->pending_focus);
 SDL_ShowWindow(window);MAC_LINK_CHECK(mac_links_snapshot(b,1,false,true));MAC_LINK_CHECK(!link_press(link,NULL));
 MAC_LINK_CHECK(mac_links_snapshot(b,2,true,true));MAC_LINK_CHECK(!link_valid(link,NULL) && !link_press(link,NULL) && !link_value(link,NULL));
 MAC_LINK_CHECK(!link_url(link,NULL) && link_range(link,NULL).location==AX_NOT_FOUND);
 id replacement=send0(element_for(b,m->nodes[1].id),"retain");
 char caption[1537];memset(caption,'x',1536);caption[1536]=0;float viewport[]={10,20,200,70},glyph[]={20,25,9,17};
 ps_a11y_begin(m);MAC_LINK_CHECK(ps_a11y_document_begin(m,3,"Long caption",caption,viewport));
 ps_a11y_document_link(m,"links",caption,3,0,1536,glyph,true);MAC_LINK_CHECK(ps_a11y_document_link_target(m,0,"docs/guide.md"));
 MAC_LINK_CHECK(ps_a11y_document_box(m,0,1,glyph));ps_a11y_publish(m);ps_a11y_native_publish(b,true);
 id long_link=send0(element_for(b,m->nodes[0].id),"retain");
 MAC_LINK_CHECK(strlen(m->nodes[0].label)<1536 && !strcmp(edit_string_bytes(send0(long_link,"accessibilityLabel")),caption));
 id full=((id(*)(id,SEL,ax_range))objc_msgSend)(b->document,sel_registerName("accessibilityAttributedStringForRange:"),(ax_range){0,1536});
 attribute=((id(*)(id,SEL,id,unsigned long,ax_range*))objc_msgSend)(full,sel_registerName("attribute:atIndex:effectiveRange:"),NSAccessibilityLinkTextAttribute,1535,&effective);
 MAC_LINK_CHECK(attribute==long_link && effective.location==0 && effective.length==1536);
 ps_a11y_native_destroy(b);MAC_LINK_CHECK(!link_value(long_link,NULL));send0(long_link,"release");
 MAC_LINK_CHECK(!link_valid(replacement,NULL) && !link_press(replacement,NULL) && !link_value(replacement,NULL));
 send0(replacement,"release");send0(link,"release");send0(tail,"release");send0(document,"release");ps_a11y_destroy(m);free(m);send0(pool,"drain");
#undef MAC_LINK_CHECK
 if(ok)puts("MAC LINKS: attributed UTF-16 spans, clipped ranges, text order, URLs, actual frames, offscreen focus, hidden/disabled rejection, versions and retained teardown passed");return ok;
}
bool ps_a11y_native_links_test_step(ps_a11y_native *b,int step) {
 if(!b)return false;id pool=send0(send0((id)objc_getClass("NSAutoreleasePool"),"alloc"),"init"),link=nil;
 const char *label=step==2?"C-Lernweg":"C";bool ok=true;size_t expected=0;
 SDL_LockMutex(b->mutex);
 if(b->model)for(size_t i=0;i<b->model->count;i++){const ps_a11y_node *n=&b->model->nodes[i];
  if(n->role==PS_A11Y_LINK){++expected;if(!strcmp(n->label,label))link=send0(element_for(b,n->id),"retain");}
 }SDL_UnlockMutex(b->mutex);
 if(step==0) {
  ok=link && b->document && expected>=2;
  id children=ok?document_link_children(b->document,false):nil;
  ok &= children && ((unsigned long(*)(id,SEL))objc_msgSend)(children,sel_registerName("count"))==expected;
  unsigned long previous=0;
  for(size_t i=0;i<expected && ok;i++) {
   id child=((id(*)(id,SEL,unsigned long))objc_msgSend)(children,sel_registerName("objectAtIndex:"),(unsigned long)i);
   ax_range range=((ax_range(*)(id,SEL))objc_msgSend)(child,sel_registerName("accessibilitySharedCharacterRange"));
   id attributed=((id(*)(id,SEL,ax_range))objc_msgSend)(b->document,sel_registerName("accessibilityAttributedStringForRange:"),range);
   id attribute=attributed?((id(*)(id,SEL,id,unsigned long,ax_range*))objc_msgSend)(attributed,sel_registerName("attribute:atIndex:effectiveRange:"),NSAccessibilityLinkTextAttribute,0,NULL):nil;
   const char *plain=attributed?edit_string_bytes(send0(attributed,"string")):NULL,*value=edit_string_bytes(send0(child,"accessibilityValue"));
   SDL_LockMutex(b->mutex);const ps_a11y_node *node=ps_a11y_find(b->model,identifier(child));id parent=node?mac_table_parent_for_link_locked(b,node):nil;SDL_UnlockMutex(b->mutex);
   ok=range.location!=AX_NOT_FOUND && range.length && (!i || range.location>=previous) && attribute==child && plain && value && !strcmp(plain,value) && send0(child,"accessibilityParent")==parent && send0(child,"accessibilityURL");
   previous=range.location+range.length;
  }
  ok &= mac_f1_table_check(b,false);
  if(ok){if(b->links_probe)send0(b->links_probe,"release");b->links_probe=send0(link,"retain");b->links_probe_serial=b->completed_link_serial;
   ((void(*)(id,SEL,BOOL))objc_msgSend)(link,sel_registerName("setAccessibilityFocused:"),YES);}
 }else if(step==1) {
  SEL selector=sel_registerName("accessibilityFrame");IMP method=link?method_getImplementation(class_getInstanceMethod(object_getClass(link),selector)):NULL;
  CGRect frame=method?((CGRect(*)(id,SEL))method)(link,selector):CGRectZero;
  ok=link && ((BOOL(*)(id,SEL))objc_msgSend)(link,sel_registerName("isAccessibilityFocused")) && !CGRectIsEmpty(frame);
  ok &= mac_f1_table_check(b,true);
 }else if(step==2 || step==4) {
  ok=link && ((BOOL(*)(id,SEL))objc_msgSend)(link,sel_registerName("accessibilityPerformPress"));
 }else if(step==3)ok=b->completed_link_serial==b->links_probe_serial;
 else if(step==5) {
  ok=b->links_probe && b->completed_link_serial==b->links_probe_serial+1 && b->completed_link_id==identifier(b->links_probe) &&
     !((BOOL(*)(id,SEL))objc_msgSend)(b->links_probe,sel_registerName("isAccessibilityElement")) &&
     !((BOOL(*)(id,SEL))objc_msgSend)(b->links_probe,sel_registerName("accessibilityPerformPress")) && !send0(b->links_probe,"accessibilityValue");
  if(b->links_probe){send0(b->links_probe,"release");b->links_probe=nil;}
 }else ok=false;
 if(link)send0(link,"release");if(!ok)fprintf(stderr,"MAC F1 LINKS native step %d failed\n",step);send0(pool,"drain");return ok;
}
