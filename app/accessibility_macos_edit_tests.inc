static void mac_edit_snapshot(ps_a11y_native *b,char *text,size_t capacity,bool enabled,size_t start,size_t end) {
 float bounds[]={10,20,200,30},glyph[]={34,25,12,17};ps_a11y_model *m=b->model;
 ps_a11y_begin(m);ps_a11y_record_edit(m,"fields","Suche θ🌍",bounds,enabled,text,capacity);
 size_t first=start,last=end;ps_a11y_edit_take_selection(m,0,&first,&last);ps_a11y_edit_update(m,0,text,first,last);
 m->draft[0].text_active=true;m->draft[0].text_caret=last;
 memcpy(m->draft[0].caret_bounds,(float[]){46,25,2,17},sizeof glyph);
 if(strlen(text)>=8)ps_a11y_edit_box(m,0,4,8,glyph);
 ps_a11y_publish(m);ps_a11y_native_publish(b,true);
}
static bool mac_edit_test(SDL_Window *window) {
 id pool=send0(send0((id)objc_getClass("NSAutoreleasePool"),"alloc"),"init");
 ps_a11y_model *m=malloc(sizeof *m);SDL_Mutex *mutex=SDL_CreateMutex();
 if(!m || !mutex){free(m);if(mutex)SDL_DestroyMutex(mutex);send0(pool,"drain");return false;}
 ps_a11y_init(m);ps_a11y_native *b=ps_a11y_native_create(window,m,mutex);
 if(!b){SDL_DestroyMutex(mutex);free(m);send0(pool,"drain");return false;}
 bool ok=true;char text[64]="A θ🌍 end";mac_edit_snapshot(b,text,sizeof text,true,2,8);
 id field=send0(element_for(b,m->nodes[0].id),"retain");
#define MAC_EDIT_CHECK(test) do {if(!(test)){fprintf(stderr,"MAC EDIT failure line %d: %s\n",__LINE__,#test);ok=false;}} while(0)
 MAC_EDIT_CHECK(!strcmp(edit_string_bytes(send0(field,"accessibilityRole")),"AXTextField"));
 MAC_EDIT_CHECK(!strcmp(edit_string_bytes(send0(field,"accessibilityValue")),text));
 MAC_EDIT_CHECK(((long(*)(id,SEL))objc_msgSend)(field,sel_registerName("accessibilityNumberOfCharacters"))==9);
 ax_range selected=((ax_range(*)(id,SEL))objc_msgSend)(field,sel_registerName("accessibilitySelectedTextRange"));
 MAC_EDIT_CHECK(selected.location==2 && selected.length==3);
 id ranges=send0(field,"accessibilitySelectedTextRanges");
 MAC_EDIT_CHECK(((unsigned long(*)(id,SEL))objc_msgSend)(ranges,sel_registerName("count"))==1);
 id range_value=((id(*)(id,SEL,unsigned long))objc_msgSend)(ranges,sel_registerName("objectAtIndex:"),0);
 ax_range plural=((ax_range(*)(id,SEL))objc_msgSend)(range_value,sel_registerName("rangeValue"));
 MAC_EDIT_CHECK(plural.location==2 && plural.length==3);
 ax_range line=((ax_range(*)(id,SEL,long))objc_msgSend)(field,sel_registerName("accessibilityRangeForLine:"),0);
 MAC_EDIT_CHECK(line.location==0 && line.length==9);
 MAC_EDIT_CHECK(((long(*)(id,SEL,long))objc_msgSend)(field,sel_registerName("accessibilityLineForIndex:"),9)==0);
 MAC_EDIT_CHECK(((long(*)(id,SEL,long))objc_msgSend)(field,sel_registerName("accessibilityLineForIndex:"),10)==(long)AX_NOT_FOUND);
 MAC_EDIT_CHECK(((long(*)(id,SEL))objc_msgSend)(field,sel_registerName("accessibilityInsertionPointLineNumber"))==0);
 MAC_EDIT_CHECK(!strcmp(edit_string_bytes(send0(field,"accessibilitySelectedText")),"θ🌍"));
 MAC_EDIT_CHECK(!((id(*)(id,SEL,ax_range))objc_msgSend)(field,sel_registerName("accessibilityStringForRange:"),(ax_range){4,1}));
 MAC_EDIT_CHECK(!((id(*)(id,SEL,ax_range))objc_msgSend)(field,sel_registerName("accessibilityStringForRange:"),(ax_range){ULONG_MAX,2}));
 ax_range visible=((ax_range(*)(id,SEL))objc_msgSend)(field,sel_registerName("accessibilityVisibleCharacterRange"));
 MAC_EDIT_CHECK(visible.location==3 && visible.length==2);
 SEL frame_sel=sel_registerName("accessibilityFrameForRange:");IMP frame_method=method_getImplementation(class_getInstanceMethod(object_getClass(field),frame_sel));
 CGRect frame=((CGRect(*)(id,SEL,ax_range))frame_method)(field,frame_sel,(ax_range){3,2});
 MAC_EDIT_CHECK(frame.size.width==12 && frame.size.height==17);
 frame=((CGRect(*)(id,SEL,ax_range))frame_method)(field,frame_sel,(ax_range){5,0});
 MAC_EDIT_CHECK(frame.size.width==2 && frame.size.height==17);
 send1(field,"setAccessibilitySelectedText:",string("β"));MAC_EDIT_CHECK(m->pending_text_id && !strcmp(m->pending_text,"A β end"));
 mac_edit_snapshot(b,text,sizeof text,true,0,0);MAC_EDIT_CHECK(!strcmp(text,"A β end"));
 send1(field,"setAccessibilityValue:",string("A θ🌍 end"));mac_edit_snapshot(b,text,sizeof text,true,0,0);
 range_value=((id(*)(id,SEL,ax_range))objc_msgSend)((id)objc_getClass("NSValue"),sel_registerName("valueWithRange:"),(ax_range){2,3});
 ranges=((id(*)(id,SEL,id))objc_msgSend)((id)objc_getClass("NSArray"),sel_registerName("arrayWithObject:"),range_value);
 send1(field,"setAccessibilitySelectedTextRanges:",ranges);
 MAC_EDIT_CHECK(m->pending_selection_start==2 && m->pending_selection_end==8 && m->pending_focus==m->nodes[0].id);
 mac_edit_snapshot(b,text,sizeof text,true,0,0);
 selected=((ax_range(*)(id,SEL))objc_msgSend)(field,sel_registerName("accessibilitySelectedTextRange"));
 MAC_EDIT_CHECK(selected.location==2 && selected.length==3 && !m->pending_selection_id);
 ((void(*)(id,SEL,ax_range))objc_msgSend)(field,sel_registerName("setAccessibilitySelectedTextRange:"),(ax_range){4,0});MAC_EDIT_CHECK(!m->pending_selection_id);
 ((void(*)(id,SEL,ax_range))objc_msgSend)(field,sel_registerName("setAccessibilityVisibleCharacterRange:"),(ax_range){3,2});
 MAC_EDIT_CHECK(m->pending_edit_scroll_start==4 && m->pending_edit_scroll_end==8);
 m->pending_edit_scroll_id=0;
 send1(field,"setAccessibilityValue:",string("line\nbreak"));MAC_EDIT_CHECK(!m->pending_text_id);
 char oversized[65];memset(oversized,'x',64);oversized[64]=0;send1(field,"setAccessibilityValue:",string(oversized));MAC_EDIT_CHECK(!m->pending_text_id);
 id number=((id(*)(id,SEL,long))objc_msgSend)((id)objc_getClass("NSNumber"),sel_registerName("numberWithLong:"),1);
 send1(field,"setAccessibilityValue:",number);MAC_EDIT_CHECK(!m->pending_text_id);
 ranges=((id(*)(id,SEL,id))objc_msgSend)((id)objc_getClass("NSArray"),sel_registerName("arrayWithObject:"),string("bad range"));
 send1(field,"setAccessibilitySelectedTextRanges:",ranges);MAC_EDIT_CHECK(!m->pending_selection_id);
 const char embedded[]={'A',0,'B'};
 id nul=((id(*)(id,SEL,const void*,unsigned long,unsigned long))objc_msgSend)(send0((id)objc_getClass("NSString"),"alloc"),sel_registerName("initWithBytes:length:encoding:"),embedded,3,4);
 send1(field,"setAccessibilityValue:",nul);send0(nul,"release");MAC_EDIT_CHECK(!m->pending_text_id);
 SDL_HideWindow(window);ps_a11y_native_publish(b,false);
 MAC_EDIT_CHECK(send0(field,"accessibilityValue") && !edit_enabled(field,NULL) && !edit_focused(field,NULL));
 selected=((ax_range(*)(id,SEL))objc_msgSend)(field,sel_registerName("accessibilitySelectedTextRange"));MAC_EDIT_CHECK(selected.location==AX_NOT_FOUND);
 frame=((CGRect(*)(id,SEL,ax_range))frame_method)(field,frame_sel,(ax_range){3,2});MAC_EDIT_CHECK(CGRectIsEmpty(frame));
 send1(field,"setAccessibilityValue:",string("hidden"));((void(*)(id,SEL,ax_range))objc_msgSend)(field,sel_registerName("setAccessibilitySelectedTextRange:"),(ax_range){0,1});
 ((void(*)(id,SEL,ax_range))objc_msgSend)(field,sel_registerName("setAccessibilityVisibleCharacterRange:"),(ax_range){0,1});
 MAC_EDIT_CHECK(!m->pending_text_id && !m->pending_selection_id && !m->pending_edit_scroll_id);
 SDL_ShowWindow(window);mac_edit_snapshot(b,text,sizeof text,false,2,8);send1(field,"setAccessibilityValue:",string("disabled"));MAC_EDIT_CHECK(!m->pending_text_id);
 ps_a11y_begin(m);ps_a11y_publish(m);ps_a11y_native_publish(b,true);
 MAC_EDIT_CHECK(!send0(field,"accessibilityValue") && !element_valid(field,NULL));
 mac_edit_snapshot(b,text,sizeof text,true,2,8);id replacement=send0(element_for(b,m->nodes[0].id),"retain");
 MAC_EDIT_CHECK(replacement!=field && !send0(field,"accessibilityValue"));ps_a11y_native_destroy(b);
 send1(replacement,"setAccessibilityValue:",string("destroyed"));MAC_EDIT_CHECK(!send0(replacement,"accessibilityValue"));
 send0(field,"release");send0(replacement,"release");ps_a11y_destroy(m);free(m);send0(pool,"drain");
#undef MAC_EDIT_CHECK
 if(ok)puts("MAC EDIT: AppKit UTF-16 value, selection, caret, replacement, geometry, reveal, disabled/hidden rejection and retained teardown passed");return ok;
}
/* This diagnostic runs between actual F1 UI frames. Every setter below is an
 * AppKit message and its effects must be delivered by the next drawn field. */
bool ps_a11y_native_edit_test_step(ps_a11y_native *b,int step) {
 if(!b)return false;id field=nil;const char *name=step<2?"Alle Inhalte durchsuchen":"Aktuelles Dokument durchsuchen";
 SDL_LockMutex(b->mutex);
 if(b->model)for(size_t i=0;i<b->model->count;i++) {
  const ps_a11y_node *n=&b->model->nodes[i];if(n->role==PS_A11Y_EDIT && !strcmp(n->label,name)){field=send0(element_for(b,n->id),"retain");break;}
 }SDL_UnlockMutex(b->mutex);if(!field)return false;
 id pool=send0(send0((id)objc_getClass("NSAutoreleasePool"),"alloc"),"init");bool ok=true;
 const char *value=edit_string_bytes(send0(field,"accessibilityValue"));
 SEL frame_sel=sel_registerName("accessibilityFrameForRange:");IMP method=method_getImplementation(class_getInstanceMethod(object_getClass(field),frame_sel));
 switch(step) {
 case 0:send1(field,"setAccessibilityValue:",string("Pendel"));break;
 case 1:ok=value && !strcmp(value,"Pendel");break;
 case 2:send1(field,"setAccessibilityValue:",string("A θ🌍 end"));break;
 case 3:
  ok=value && !strcmp(value,"A θ🌍 end");
  ((void(*)(id,SEL,ax_range))objc_msgSend)(field,sel_registerName("setAccessibilitySelectedTextRange:"),(ax_range){2,3});break;
 case 4: {
  ax_range range=((ax_range(*)(id,SEL))objc_msgSend)(field,sel_registerName("accessibilitySelectedTextRange"));
  const char *selected=edit_string_bytes(send0(field,"accessibilitySelectedText"));
  CGRect frame=((CGRect(*)(id,SEL,ax_range))method)(field,frame_sel,(ax_range){2,3});
  ok=range.location==2 && range.length==3 && selected && !strcmp(selected,"θ🌍") && !CGRectIsEmpty(frame) && edit_focused(field,NULL);
  send1(field,"setAccessibilitySelectedText:",string("β"));break;
 }
 case 5:
  {ax_range range=((ax_range(*)(id,SEL))objc_msgSend)(field,sel_registerName("accessibilitySelectedTextRange"));
   ok=value && !strcmp(value,"A β end") && range.location==3 && !range.length;}
  ((void(*)(id,SEL,ax_range))objc_msgSend)(field,sel_registerName("setAccessibilitySelectedTextRange:"),(ax_range){3,0});break;
 case 6: {
  ax_range range=((ax_range(*)(id,SEL))objc_msgSend)(field,sel_registerName("accessibilitySelectedTextRange"));
  CGRect caret=((CGRect(*)(id,SEL,ax_range))method)(field,frame_sel,(ax_range){3,0});
  ok=range.location==3 && !range.length && !CGRectIsEmpty(caret);
  char long_text[107];memset(long_text,'x',100);strcpy(long_text+100,"θ🌍");send1(field,"setAccessibilityValue:",string(long_text));break;
 }
 case 7:
  ok=value && strlen(value)==106 && !strcmp(value+100,"θ🌍");
  ((void(*)(id,SEL,ax_range))objc_msgSend)(field,sel_registerName("setAccessibilityVisibleCharacterRange:"),(ax_range){0,1});break;
 case 8: {
  CGRect first=((CGRect(*)(id,SEL,ax_range))method)(field,frame_sel,(ax_range){0,1});
  CGRect last=((CGRect(*)(id,SEL,ax_range))method)(field,frame_sel,(ax_range){100,3});
  ok=!CGRectIsEmpty(first) && CGRectIsEmpty(last);
  ((void(*)(id,SEL,ax_range))objc_msgSend)(field,sel_registerName("setAccessibilityVisibleCharacterRange:"),(ax_range){100,3});break;
 }
 case 9: {
  CGRect frame=((CGRect(*)(id,SEL,ax_range))method)(field,frame_sel,(ax_range){100,3});
  ok=value && strlen(value)==106 && !CGRectIsEmpty(frame);break;
 }
 case 10: {
  CGRect frame=((CGRect(*)(id,SEL,ax_range))method)(field,frame_sel,(ax_range){100,3});
  ok=value && strlen(value)==106 && !edit_enabled(field,NULL) && CGRectIsEmpty(frame);
  send1(field,"setAccessibilityValue:",string("hidden replacement"));
  SDL_LockMutex(b->mutex);ok &= b->model && !b->model->pending_text_id;SDL_UnlockMutex(b->mutex);break;
 }
 case 11:ok=value && strlen(value)==106 && edit_enabled(field,NULL);break;
 default:ok=false;break;
 }
 if(!ok)fprintf(stderr,"MAC F1 EDIT native step %d failed\n",step);
 send0(field,"release");send0(pool,"drain");return ok;
}
