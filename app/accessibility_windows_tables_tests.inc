/* Direct COM boundary probe; the independent application client is separate. */
static size_t wt_test_count(ps_a11y_native *b) {
 size_t count=0;for(uia_table_element *e=b->table_elements;e;e=e->next)++count;return count;
}
static bool uia_native_tables_probe(SDL_Window *window) {
 bool ok=true;ps_a11y_model *m=malloc(sizeof *m);SDL_Mutex *mutex=NULL;ps_a11y_native *b=NULL;
 IRawElementProviderSimple *table=NULL,*cell=NULL,*header=NULL,*empty=NULL,*same=NULL,*link=NULL,*enclosing=NULL;
 IGridProvider *grid=NULL;ITableProvider *pattern=NULL;IGridItemProvider *item=NULL;ITableItemProvider *table_item=NULL;
 IScrollItemProvider *scroll=NULL;IRawElementProviderFragment *fragment=NULL,*parent=NULL;
 ITextRangeProvider *whole=NULL,*table_range=NULL,*cell_range=NULL,*empty_range=NULL,*link_range=NULL;
 SAFEARRAY *array=NULL;VARIANT value={0};BSTR read=NULL;
 char text[2048]="H1 | θ🌍\n\nname | go\n\n";size_t long_begin=strlen(text);
 memset(text+long_begin,'x',1536);text[long_begin+1536]='\n';text[long_begin+1537]=0;
 size_t theta=(size_t)(strstr(text,"θ")-text),name=(size_t)(strstr(text,"name")-text),go=(size_t)(strstr(text,"go")-text);
 ps_a11y_cell_range head[]={{0,2},{theta,theta+6}},body[]={{name,name+4},{go,go+2}},last[]={{long_begin,long_begin+1536}};
 ps_a11y_table_row_input rows[]={
  {.begin=0,.end=theta+6,.count=2,.cells=head,.bounds={5,5,210,20}},
  {.begin=name,.end=go+2,.count=2,.cells=body,.bounds={5,500,210,20}},
  {.begin=long_begin,.end=long_begin+1536,.count=1,.cells=last,.bounds={5,540,210,20}}
 };
 float view[]={0,0,400,200},hidden[]={0,0,0,0};uint64_t table_id=0,cell_id=0;
#define WTCHECK(x) do {if(!(x)){fprintf(stderr,"Windows table COM %d: %s\n",__LINE__,#x);ok=false;goto cleanup;}}while(0)
 WTCHECK(m);ps_a11y_init(m);mutex=SDL_CreateMutex();WTCHECK(mutex);
 b=ps_a11y_native_create(window,m,mutex);WTCHECK(b);mutex=NULL;
 ps_a11y_begin(m);WTCHECK(ps_a11y_document_begin(m,71,"Tabellen θ🌍",text,view));
 WTCHECK(ps_a11y_document_table(m,0,long_begin+1536,3,3,true,rows));
 ps_a11y_document_link(m,"Table","go",71,go,go+2,hidden,true);WTCHECK(m->draft_count==1);
 WTCHECK(ps_a11y_publish(m));ps_a11y_native_publish(b,true);
 ps_a11y_table t;WTCHECK(ps_a11y_table_at(m,0,&t));table_id=t.id;
 SDL_LockMutex(b->mutex);table=uia_any_locked(b,t.id);SDL_UnlockMutex(b->mutex);WTCHECK(table);
 WTCHECK(SUCCEEDED(IRawElementProviderSimple_QueryInterface(table,&IID_IGridProvider,(void**)&grid)) && grid);
 WTCHECK(SUCCEEDED(IRawElementProviderSimple_QueryInterface(table,&IID_ITableProvider,(void**)&pattern)) && pattern);
 int count=0;WTCHECK(SUCCEEDED(IGridProvider_get_RowCount(grid,&count)) && count==3);
 WTCHECK(SUCCEEDED(IGridProvider_get_ColumnCount(grid,&count)) && count==3);
 WTCHECK(SUCCEEDED(IGridProvider_GetItem(grid,1,1,&cell)) && cell);cell_id=t.id+1+4;
 WTCHECK(SUCCEEDED(IGridProvider_GetItem(grid,0,1,&header)) && header);
 WTCHECK(IGridProvider_GetItem(grid,-1,0,&same)==E_INVALIDARG && !same);
 WTCHECK(IGridProvider_GetItem(grid,0,3,&same)==E_INVALIDARG && !same);
 WTCHECK(SUCCEEDED(IRawElementProviderSimple_GetPropertyValue(header,UIA_NamePropertyId,&value)) && value.vt==VT_BSTR &&
  SysStringLen(value.bstrVal)==3 && !wcscmp(value.bstrVal,L"θ🌍"));VariantClear(&value);
 WTCHECK(SUCCEEDED(ITableProvider_GetColumnHeaders(pattern,&array)) && array);
 LONG low=0,high=-1;WTCHECK(SUCCEEDED(SafeArrayGetLBound(array,1,&low)) && SUCCEEDED(SafeArrayGetUBound(array,1,&high)) && high-low+1==3);
 SafeArrayDestroy(array);array=NULL;
 WTCHECK(SUCCEEDED(IRawElementProviderSimple_QueryInterface(cell,&IID_IGridItemProvider,(void**)&item)) && item);
 WTCHECK(SUCCEEDED(IRawElementProviderSimple_QueryInterface(cell,&IID_ITableItemProvider,(void**)&table_item)) && table_item);
 WTCHECK(SUCCEEDED(IGridItemProvider_get_Row(item,&count)) && count==1);
 WTCHECK(SUCCEEDED(IGridItemProvider_get_Column(item,&count)) && count==1);
 WTCHECK(SUCCEEDED(IGridItemProvider_get_RowSpan(item,&count)) && count==1);
 WTCHECK(SUCCEEDED(IGridItemProvider_get_ColumnSpan(item,&count)) && count==1);
 WTCHECK(SUCCEEDED(IGridItemProvider_get_ContainingGrid(item,&same)) && same==table);IRawElementProviderSimple_Release(same);same=NULL;
 WTCHECK(SUCCEEDED(ITableItemProvider_GetColumnHeaderItems(table_item,&array)) && array);
 WTCHECK(SUCCEEDED(SafeArrayGetUBound(array,1,&high)) && high==0);
 IUnknown *unknown=NULL;LONG zero=0;WTCHECK(SUCCEEDED(SafeArrayGetElement(array,&zero,&unknown)) && unknown);
 HRESULT query=IUnknown_QueryInterface(unknown,&IID_IRawElementProviderSimple,(void**)&same);IUnknown_Release(unknown);
 WTCHECK(SUCCEEDED(query) && same==header);IRawElementProviderSimple_Release(same);same=NULL;SafeArrayDestroy(array);array=NULL;
 WTCHECK(SUCCEEDED(IRawElementProviderSimple_GetPropertyValue(cell,UIA_IsOffscreenPropertyId,&value)) && value.boolVal==VARIANT_TRUE);VariantClear(&value);
 WTCHECK(SUCCEEDED(IRawElementProviderSimple_QueryInterface(cell,&IID_IScrollItemProvider,(void**)&scroll)) && scroll);
 EnableWindow(b->hwnd,FALSE);WTCHECK(IScrollItemProvider_ScrollIntoView(scroll)==UIA_E_ELEMENTNOTENABLED);EnableWindow(b->hwnd,TRUE);
 WTCHECK(SUCCEEDED(IScrollItemProvider_ScrollIntoView(scroll)));size_t byte=0;
 WTCHECK(ps_a11y_document_take_scroll(m,71,&byte) && byte==go);
 WTCHECK(SUCCEEDED(IRawElementProviderSimple_QueryInterface(cell,&IID_IRawElementProviderFragment,(void**)&fragment)) && fragment);
 WTCHECK(SUCCEEDED(IRawElementProviderFragment_Navigate(fragment,NavigateDirection_Parent,&parent)) && parent);
 WTCHECK(SUCCEEDED(IRawElementProviderFragment_QueryInterface(parent,&IID_IRawElementProviderSimple,(void**)&same)) && same==table);
 IRawElementProviderSimple_Release(same);same=NULL;IRawElementProviderFragment_Release(parent);parent=NULL;
 WTCHECK(SUCCEEDED(IRawElementProviderFragment_Navigate(fragment,NavigateDirection_FirstChild,&parent)) && parent);
 WTCHECK(SUCCEEDED(IRawElementProviderFragment_QueryInterface(parent,&IID_IRawElementProviderSimple,(void**)&link)) && link);
 IRawElementProviderFragment_Release(parent);parent=NULL;
 WTCHECK(SUCCEEDED(ITextProvider_get_DocumentRange(&b->root->text,&whole)) && whole);
 WTCHECK(SUCCEEDED(ITextRangeProvider_GetChildren(whole,&array)) && array);
 WTCHECK(SUCCEEDED(SafeArrayGetUBound(array,1,&high)) && high==0);SafeArrayDestroy(array);array=NULL;
 WTCHECK(SUCCEEDED(ITextProvider_RangeFromChild(&b->root->text,table,&table_range)) && table_range);
 WTCHECK(SUCCEEDED(ITextRangeProvider_GetEnclosingElement(table_range,&enclosing)) && enclosing==table);
 IRawElementProviderSimple_Release(enclosing);enclosing=NULL;
 WTCHECK(SUCCEEDED(ITextRangeProvider_GetChildren(table_range,&array)) && array);
 WTCHECK(SUCCEEDED(SafeArrayGetUBound(array,1,&high)) && high==8);SafeArrayDestroy(array);array=NULL;
 WTCHECK(SUCCEEDED(ITextProvider_RangeFromChild(&b->root->text,cell,&cell_range)) && cell_range);
 WTCHECK(SUCCEEDED(ITextProvider_RangeFromChild(&b->root->text,link,&link_range)) && link_range);
 BOOL equal=FALSE;WTCHECK(SUCCEEDED(ITextRangeProvider_Compare(cell_range,link_range,&equal)) && equal);
 WTCHECK(SUCCEEDED(ITextRangeProvider_GetEnclosingElement(cell_range,&enclosing)) && enclosing==link);
 IRawElementProviderSimple_Release(enclosing);enclosing=NULL;
 WTCHECK(SUCCEEDED(IGridProvider_GetItem(grid,2,2,&empty)) && empty);
 WTCHECK(SUCCEEDED(ITextProvider_RangeFromChild(&b->root->text,empty,&empty_range)) && empty_range);
 WTCHECK(SUCCEEDED(ITextRangeProvider_GetEnclosingElement(empty_range,&enclosing)) && enclosing==empty);
 IRawElementProviderSimple_Release(enclosing);enclosing=NULL;
 WTCHECK(SUCCEEDED(ITextRangeProvider_GetText(empty_range,-1,&read)) && read && !SysStringLen(read));SysFreeString(read);read=NULL;
 /* Full labels and weak caching do not consume the 256 control providers. */
 SDL_LockMutex(b->mutex);size_t before=wt_test_count(b);same=uia_any_locked(b,t.id+1+6);
 bool long_ok=same && SUCCEEDED(IRawElementProviderSimple_GetPropertyValue(same,UIA_NamePropertyId,&value)) &&
  value.vt==VT_BSTR && SysStringLen(value.bstrVal)==1536;
 VariantClear(&value);if(same)IRawElementProviderSimple_Release(same);same=NULL;
 long_ok=long_ok && wt_test_count(b)==before;SDL_UnlockMutex(b->mutex);WTCHECK(long_ok);
 rows[1].bounds[1]=40;rows[2].bounds[1]=80;
 ps_a11y_begin(m);WTCHECK(ps_a11y_document_begin(m,71,"Tabellen θ🌍",text,view));
 WTCHECK(ps_a11y_document_table(m,0,long_begin+1536,3,3,true,rows));
 float visible[]={75,40,20,20};ps_a11y_document_link(m,"Table","go",71,go,go+2,visible,true);
 WTCHECK(ps_a11y_publish(m));ps_a11y_native_publish(b,true);
 WTCHECK(SUCCEEDED(IGridProvider_GetItem(grid,1,1,&same)) && same==cell);IRawElementProviderSimple_Release(same);same=NULL;
 WTCHECK(SUCCEEDED(IRawElementProviderFragment_get_BoundingRectangle(fragment,&(struct UiaRect){0})));
 WTCHECK(SDL_HideWindow(window));ps_a11y_suspend(m);ps_a11y_native_publish(b,true);
 struct UiaRect bounds={0};WTCHECK(SUCCEEDED(IRawElementProviderFragment_get_BoundingRectangle(fragment,&bounds)) && bounds.width==0);
 WTCHECK(IScrollItemProvider_ScrollIntoView(scroll)==UIA_E_ELEMENTNOTENABLED);
 WTCHECK(SUCCEEDED(IRawElementProviderSimple_GetPropertyValue(header,UIA_NamePropertyId,&value)) && !wcscmp(value.bstrVal,L"θ🌍"));VariantClear(&value);
 WTCHECK(SDL_ShowWindow(window));ps_a11y_begin(m);WTCHECK(ps_a11y_document_begin(m,71,"Tabellen θ🌍",text,view));
 WTCHECK(ps_a11y_document_table(m,0,long_begin+1536,3,3,true,rows));ps_a11y_document_link(m,"Table","go",71,go,go+2,visible,true);
 WTCHECK(ps_a11y_publish(m));ps_a11y_native_publish(b,true);
 WTCHECK(ps_a11y_table_get(m,table_id,&t) && SUCCEEDED(IGridProvider_GetItem(grid,1,1,&same)) && same==cell);
 IRawElementProviderSimple_Release(same);same=NULL;ps_a11y_table_cell retained;WTCHECK(ps_a11y_table_cell_find(m,cell_id,&retained));
 ps_a11y_begin(m);WTCHECK(ps_a11y_document_begin(m,72,"Neu","replacement",view));WTCHECK(ps_a11y_publish(m));ps_a11y_native_publish(b,true);
 WTCHECK(IGridProvider_get_RowCount(grid,&count)==UIA_E_ELEMENTNOTAVAILABLE);
 WTCHECK(IGridItemProvider_get_Row(item,&count)==UIA_E_ELEMENTNOTAVAILABLE);
 WTCHECK(ITextRangeProvider_GetText(table_range,-1,&read)==UIA_E_ELEMENTNOTAVAILABLE && !read);
 ps_a11y_native_destroy(b);b=NULL;
 WTCHECK(IRawElementProviderSimple_GetPropertyValue(header,UIA_NamePropertyId,&value)==UIA_E_ELEMENTNOTAVAILABLE);
cleanup:
 if(b){EnableWindow(b->hwnd,TRUE);ps_a11y_native_destroy(b);}if(mutex)SDL_DestroyMutex(mutex);
 VariantClear(&value);SysFreeString(read);if(array)SafeArrayDestroy(array);
 if(enclosing)IRawElementProviderSimple_Release(enclosing);if(same)IRawElementProviderSimple_Release(same);
 if(parent)IRawElementProviderFragment_Release(parent);if(fragment)IRawElementProviderFragment_Release(fragment);
 if(whole)ITextRangeProvider_Release(whole);if(table_range)ITextRangeProvider_Release(table_range);
 if(cell_range)ITextRangeProvider_Release(cell_range);if(empty_range)ITextRangeProvider_Release(empty_range);if(link_range)ITextRangeProvider_Release(link_range);
 if(scroll)IScrollItemProvider_Release(scroll);if(table_item)ITableItemProvider_Release(table_item);
 if(item)IGridItemProvider_Release(item);if(pattern)ITableProvider_Release(pattern);if(grid)IGridProvider_Release(grid);
 if(link)IRawElementProviderSimple_Release(link);if(empty)IRawElementProviderSimple_Release(empty);
 if(header)IRawElementProviderSimple_Release(header);if(cell)IRawElementProviderSimple_Release(cell);if(table)IRawElementProviderSimple_Release(table);
 if(m){ps_a11y_destroy(m);free(m);}SDL_ShowWindow(window);
 if(ok)puts("Windows table COM: Grid/Table, cell headers, UTF-16, full names, nested text ranges, virtual cells, weak identity, reflow, hide/restore and stale providers passed");
#undef WTCHECK
 return ok;
}
