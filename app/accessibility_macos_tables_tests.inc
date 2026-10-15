static unsigned long mac_table_array_count(id array){return array?((unsigned long(*)(id,SEL))objc_msgSend)(array,sel_registerName("count")):0;}
static id mac_table_array_at(id array,unsigned long at){return at<mac_table_array_count(array)?((id(*)(id,SEL,unsigned long))objc_msgSend)(array,sel_registerName("objectAtIndex:"),at):nil;}
static bool mac_table_text_is(id value,const char *text){const char *bytes=edit_string_bytes(value);return bytes && !strcmp(bytes,text);}
static CGRect mac_table_test_frame(id object) {
 SEL selector=sel_registerName("accessibilityFrame");IMP method=object?method_getImplementation(class_getInstanceMethod(object_getClass(object),selector)):NULL;
 return method?((CGRect(*)(id,SEL))method)(object,selector):CGRectZero;
}
static id mac_table_test_cell(id table,long row,long column) {
 return ((id(*)(id,SEL,long,long))objc_msgSend)(table,sel_registerName("accessibilityCellForColumn:row:"),column,row);
}
static bool mac_tables_snapshot(ps_a11y_native *b,uint64_t version,bool tail_visible) {
 static char text[1800];if(!text[0]){strcpy(text,"θ🌍\tHeader\tLast\n");size_t n=strlen(text);memset(text+n,'x',1536);strcpy(text+n+1536,"\tgo\ntail\n");}
 ps_a11y_cell_range cells[3][3]={0};ps_a11y_table_row_input rows[3]={0};size_t at=0;
 for(size_t row=0;row<3;row++) {
  size_t begin=at,column=0,first=at;
  for(;;at++)if(text[at]=='\t' || text[at]=='\n') {
   cells[row][column++]=(ps_a11y_cell_range){first,at};first=at+1;if(text[at]=='\n')break;
  }
  rows[row]=(ps_a11y_table_row_input){.begin=begin,.end=at,.count=column,.cells=cells[row],.bounds={10,row==0?20:row==1?45:tail_visible?70:140,240,20}};at++;
 }
 ps_a11y_model *m=b->model;float view[]={10,20,240,70},link_box[]={95,48,16,16},a[]={95,48,8,16},z[]={103,48,8,16};
 ps_a11y_begin(m);bool okay=ps_a11y_document_begin(m,version,"Table θ🌍",text,view) && ps_a11y_document_table(m,0,rows[2].end,3,3,true,rows);
 ps_a11y_document_link(m,"table","go",version,cells[1][1].begin,cells[1][1].end,link_box,true);
 okay &= ps_a11y_document_link_target(m,0,"docs/c-guide.md");
 okay &= ps_a11y_document_box(m,cells[1][1].begin,cells[1][1].begin+1,a) && ps_a11y_document_box(m,cells[1][1].begin+1,cells[1][1].end,z);
 float theta[]={12,21,10,16},globe[]={22,21,12,16};
 okay &= ps_a11y_document_box(m,0,2,theta) && ps_a11y_document_box(m,2,6,globe);
 ps_a11y_record(m,"controls","Guard",PS_A11Y_BUTTON,link_box,true);ps_a11y_publish(m);ps_a11y_native_publish(b,true);return okay;
}
static bool mac_tables_test(SDL_Window *window) {
 id pool=send0(send0((id)objc_getClass("NSAutoreleasePool"),"alloc"),"init");
 ps_a11y_model *m=malloc(sizeof *m);SDL_Mutex *mutex=SDL_CreateMutex();
 if(!m || !mutex){free(m);if(mutex)SDL_DestroyMutex(mutex);send0(pool,"drain");return false;}
 ps_a11y_init(m);ps_a11y_native *b=ps_a11y_native_create(window,m,mutex);
 if(!b){SDL_DestroyMutex(mutex);free(m);send0(pool,"drain");return false;}
 bool okay=mac_tables_snapshot(b,1,false);id document=send0(b->document,"retain");
 id table=send0(mac_table_array_at(send0(document,"accessibilityChildren"),0),"retain");
 if(!table){fprintf(stderr,"MAC TABLE missing native table\n");ps_a11y_native_destroy(b);ps_a11y_destroy(m);free(m);send0(document,"release");send0(pool,"drain");return false;}
#define MAC_TABLE_CHECK(test) do {if(!(test)){fprintf(stderr,"MAC TABLE failure line %d: %s\n",__LINE__,#test);okay=false;}}while(0)
 MAC_TABLE_CHECK(table && mac_table_text_is(send0(table,"accessibilityRole"),"AXTable"));
 MAC_TABLE_CHECK(((long(*)(id,SEL))objc_msgSend)(table,sel_registerName("accessibilityRowCount"))==3);
 MAC_TABLE_CHECK(((long(*)(id,SEL))objc_msgSend)(table,sel_registerName("accessibilityColumnCount"))==3);
 id rows=send0(table,"accessibilityRows"),columns=send0(table,"accessibilityColumns");
 MAC_TABLE_CHECK(mac_table_array_count(rows)==3 && mac_table_array_count(columns)==3);
 MAC_TABLE_CHECK(mac_table_array_count(send0(table,"accessibilityChildren"))==6);
 MAC_TABLE_CHECK(mac_table_array_count(send0(table,"accessibilityChildrenInNavigationOrder"))==3);
 id row=send0(mac_table_array_at(rows,2),"retain"),column=mac_table_array_at(columns,1);
 id empty=send0(mac_table_test_cell(table,2,2),"retain"),other=send0(mac_table_test_cell(table,2,1),"retain");
 id header=send0(mac_table_test_cell(table,0,0),"retain"),long_cell=mac_table_test_cell(table,1,0),linked_cell=mac_table_test_cell(table,1,1);
 if(!row || !column || !empty || !other || !header || !long_cell || !linked_cell) {
  fprintf(stderr,"MAC TABLE missing row, column or cell\n");ps_a11y_native_destroy(b);ps_a11y_destroy(m);free(m);
  send0(table,"release");send0(document,"release");send0(header,"release");send0(row,"release");send0(empty,"release");send0(other,"release");send0(pool,"drain");return false;
 }
 MAC_TABLE_CHECK(mac_table_text_is(send0(row,"accessibilityRole"),"AXRow") && mac_table_text_is(send0(column,"accessibilityRole"),"AXColumn"));
 MAC_TABLE_CHECK(mac_table_text_is(send0(empty,"accessibilityRole"),"AXCell") && empty!=other && identifier(empty)!=identifier(other));
 MAC_TABLE_CHECK(mac_table_text_is(send0(empty,"accessibilityValue"),"") && mac_table_text_is(send0(other,"accessibilityValue"),""));
 MAC_TABLE_CHECK(((long(*)(id,SEL))objc_msgSend)(row,sel_registerName("accessibilityIndex"))==2);
 MAC_TABLE_CHECK(((long(*)(id,SEL))objc_msgSend)(column,sel_registerName("accessibilityIndex"))==1);
 MAC_TABLE_CHECK(send0(empty,"accessibilityParent")==row && send0(row,"accessibilityParent")==table && send0(table,"accessibilityParent")==document);
 MAC_TABLE_CHECK(mac_table_array_at(send0(row,"accessibilityChildren"),2)==empty);
 MAC_TABLE_CHECK(mac_table_array_count(send0(table,"accessibilityColumnHeaderUIElements"))==3);
 MAC_TABLE_CHECK(send0(column,"accessibilityHeader")==mac_table_test_cell(table,0,1));
 MAC_TABLE_CHECK(mac_table_array_at(send0(empty,"accessibilityColumnHeaderUIElements"),0)==mac_table_test_cell(table,0,2));
 MAC_TABLE_CHECK(mac_table_array_count(send0(table,"accessibilitySelectedRows"))==0 && mac_table_array_count(send0(empty,"accessibilityRowHeaderUIElements"))==0);
 MAC_TABLE_CHECK(mac_table_text_is(send0(header,"accessibilityValue"),"θ🌍"));
 MAC_TABLE_CHECK(((unsigned long(*)(id,SEL))objc_msgSend)(send0(long_cell,"accessibilityValue"),sel_registerName("length"))==1536);
 ax_range coordinate=((ax_range(*)(id,SEL))objc_msgSend)(empty,sel_registerName("accessibilityColumnIndexRange"));
 MAC_TABLE_CHECK(coordinate.location==2 && coordinate.length==1);
 coordinate=((ax_range(*)(id,SEL))objc_msgSend)(empty,sel_registerName("accessibilityRowIndexRange"));MAC_TABLE_CHECK(coordinate.location==2 && coordinate.length==1);
 ax_range shared=((ax_range(*)(id,SEL))objc_msgSend)(header,sel_registerName("accessibilitySharedCharacterRange"));MAC_TABLE_CHECK(shared.location==0 && shared.length==3);
 MAC_TABLE_CHECK(mac_table_array_at(send0(header,"accessibilitySharedTextUIElements"),0)==document);
 id part=((id(*)(id,SEL,ax_range))objc_msgSend)(header,sel_registerName("accessibilityStringForRange:"),(ax_range){1,2});MAC_TABLE_CHECK(mac_table_text_is(part,"🌍"));
 MAC_TABLE_CHECK(!((id(*)(id,SEL,ax_range))objc_msgSend)(header,sel_registerName("accessibilityStringForRange:"),(ax_range){2,1}));
 ax_range visible=mac_table_visible_range(header,NULL);MAC_TABLE_CHECK(visible.location==0 && visible.length==3);
 MAC_TABLE_CHECK(mac_table_visible_range(long_cell,NULL).length==0);
 SEL range_frame=sel_registerName("accessibilityFrameForRange:");IMP range_method=method_getImplementation(class_getInstanceMethod(object_getClass(header),range_frame));
 CGRect glyph=((CGRect(*)(id,SEL,ax_range))range_method)(header,range_frame,(ax_range){1,2});MAC_TABLE_CHECK(glyph.size.width==12 && glyph.size.height==16);
 MAC_TABLE_CHECK(!mac_table_test_cell(table,-1,0) && !mac_table_test_cell(table,0,3));
 id link=mac_table_array_at(send0(linked_cell,"accessibilityChildren"),0);
 MAC_TABLE_CHECK(link && send0(link,"accessibilityParent")==linked_cell && mac_table_array_at(send0(link,"accessibilitySharedTextUIElements"),0)==document);
 CGRect frame=mac_table_test_frame(header);MAC_TABLE_CHECK(frame.size.width==80 && frame.size.height==20);
 MAC_TABLE_CHECK(((id(*)(id,SEL,CGPoint))objc_msgSend)(document,sel_registerName("accessibilityHitTest:"),CGPointMake(frame.origin.x+2,frame.origin.y+2))==header);
 MAC_TABLE_CHECK(CGRectIsEmpty(mac_table_test_frame(empty)) && mac_table_array_count(send0(table,"accessibilityVisibleRows"))==2);
 MAC_TABLE_CHECK(!((BOOL(*)(id,SEL))objc_msgSend)(empty,sel_registerName("accessibilityPerformPress")));
 ((void(*)(id,SEL,BOOL))objc_msgSend)(empty,sel_registerName("setAccessibilityFocused:"),YES);
 ((void(*)(id,SEL,id))objc_msgSend)(empty,sel_registerName("setAccessibilityValue:"),string("blocked"));
 MAC_TABLE_CHECK(!m->pending_press && !m->pending_focus);
 ((void(*)(id,SEL,ax_range))objc_msgSend)(empty,sel_registerName("setAccessibilityVisibleCharacterRange:"),(ax_range){0,0});
 ps_a11y_table_cell model_cell;MAC_TABLE_CHECK(ps_a11y_table_cell_find(m,identifier(empty),&model_cell));size_t byte=0;
 MAC_TABLE_CHECK(ps_a11y_document_take_scroll(m,1,&byte) && byte==model_cell.begin);
 MAC_TABLE_CHECK(mac_tables_snapshot(b,1,true));MAC_TABLE_CHECK(mac_table_test_cell(table,2,2)==empty && !CGRectIsEmpty(mac_table_test_frame(empty)));
 SDL_HideWindow(window);ps_a11y_native_publish(b,false);
 MAC_TABLE_CHECK(mac_table_text_is(send0(header,"accessibilityValue"),"θ🌍") && CGRectIsEmpty(mac_table_test_frame(empty)) && !mac_table_enabled(empty,NULL));
 ((void(*)(id,SEL,ax_range))objc_msgSend)(empty,sel_registerName("setAccessibilityVisibleCharacterRange:"),(ax_range){0,0});MAC_TABLE_CHECK(!m->document_scroll_version);
 SDL_ShowWindow(window);MAC_TABLE_CHECK(mac_tables_snapshot(b,1,true) && mac_table_test_cell(table,2,2)==empty);
 MAC_TABLE_CHECK(m->count==2 && !m->dropped);
 MAC_TABLE_CHECK(mac_tables_snapshot(b,2,true));MAC_TABLE_CHECK(!mac_table_valid(empty,NULL) && !mac_table_valid(table,NULL) && !send0(empty,"accessibilityValue") && !send0(empty,"accessibilityParent"));
 MAC_TABLE_CHECK(mac_table_shared_range(header,NULL).location==AX_NOT_FOUND && mac_table_index(row,NULL)==-1);
 ps_a11y_native_destroy(b);MAC_TABLE_CHECK(!mac_table_valid(empty,NULL) && CGRectIsEmpty(mac_table_test_frame(header)));
 ps_a11y_destroy(m);free(m);
 send0(table,"release");send0(document,"release");send0(header,"release");send0(row,"release");send0(empty,"release");send0(other,"release");send0(pool,"drain");
#undef MAC_TABLE_CHECK
 if(okay)puts("MAC TABLES: rows, columns, Unicode, full names, virtual cells, headers, shared text, hit testing, reveal, reflow, hidden state and retained teardown passed");return okay;
}
/* Real F1 fixture, called alongside its native link navigation task. */
static bool mac_f1_table_check(ps_a11y_native *b,bool visible) {
 /* Discover through the actual Cocoa view and public selectors. This client
  * does not consult the model, provider cache, classes or byte-range helpers. */
 if(!b)return false;id document=nil,table=nil;
 id window=SDL_GetPointerProperty(SDL_GetWindowProperties(b->window),SDL_PROP_WINDOW_COCOA_WINDOW_POINTER,NULL);
 id roots=send0(send0(window,"contentView"),"accessibilityChildren");
 for(unsigned long r=0;r<mac_table_array_count(roots);r++) {
  id candidate=mac_table_array_at(roots,r),children=send0(candidate,"accessibilityChildren");
  for(unsigned long i=0;i<mac_table_array_count(children);i++) {
   id child=mac_table_array_at(children,i);if(mac_table_text_is(send0(child,"accessibilityRole"),"AXTable")){document=candidate;table=child;}
  }
 }
 bool okay=document && table;
 if(!table || ((long(*)(id,SEL))objc_msgSend)(table,sel_registerName("accessibilityRowCount"))!=2 || ((long(*)(id,SEL))objc_msgSend)(table,sel_registerName("accessibilityColumnCount"))!=2)return false;
 id cell=mac_table_test_cell(table,1,1),header=mac_table_test_cell(table,0,1);
 okay &= mac_table_text_is(send0(cell,"accessibilityValue"),"l|r in Metern; C") && mac_table_text_is(send0(header,"accessibilityValue"),"Bedeutung");
 okay &= mac_table_array_at(send0(cell,"accessibilityColumnHeaderUIElements"),0)==header;
 id link=mac_table_array_at(send0(cell,"accessibilityChildren"),0);okay &= link && send0(link,"accessibilityParent")==cell;
 ax_range range=((ax_range(*)(id,SEL))objc_msgSend)(cell,sel_registerName("accessibilitySharedCharacterRange"));id text=((id(*)(id,SEL,ax_range))objc_msgSend)(document,sel_registerName("accessibilityStringForRange:"),range);
 okay &= range.location!=AX_NOT_FOUND && mac_table_text_is(text,"l|r in Metern; C");
 if(visible) {
  CGRect frame=mac_table_test_frame(cell);okay &= !CGRectIsEmpty(frame);
  okay &= ((id(*)(id,SEL,CGPoint))objc_msgSend)(document,sel_registerName("accessibilityHitTest:"),CGPointMake(frame.origin.x+2,frame.origin.y+2))==cell;
 }
 if(okay)puts(visible?"MAC F1 TABLE: visible cell and actual point hit passed":"MAC F1 TABLE: 2x2 rows/columns, headers, full text and nested link passed");return okay;
}
