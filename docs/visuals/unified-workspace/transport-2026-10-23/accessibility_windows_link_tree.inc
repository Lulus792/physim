static HRESULT table_client_pattern(IUIAutomationElement *element,PATTERNID pattern,REFIID iid,void **out) {
 *out=NULL;IUnknown *unknown=NULL;HRESULT result=IUIAutomationElement_GetCurrentPattern(element,pattern,&unknown);
 if(SUCCEEDED(result))result=unknown?IUnknown_QueryInterface(unknown,iid,out):E_FAIL;
 if(unknown)IUnknown_Release(unknown);return result;
}
static HRESULT validate_table_hit(IUIAutomation *automation,IUIAutomationGridPattern *grid) {
 IUIAutomationElement *cell=NULL,*hit=NULL;RECT bounds={0};BOOL same=FALSE;
 HRESULT result=IUIAutomationGridPattern_GetItem(grid,1,1,&cell);
 if(SUCCEEDED(result))result=IUIAutomationElement_get_CurrentBoundingRectangle(cell,&bounds);
 if(SUCCEEDED(result) && (bounds.right-bounds.left<5 || bounds.bottom-bounds.top<2))result=E_FAIL;
 POINT point={bounds.left+2,bounds.top+(bounds.bottom-bounds.top)/2};
 if(SUCCEEDED(result))result=IUIAutomation_ElementFromPoint(automation,point,&hit);
 if(SUCCEEDED(result))result=IUIAutomation_CompareElements(automation,cell,hit,&same);
 if(SUCCEEDED(result) && !same)result=E_FAIL;
 if(hit)IUIAutomationElement_Release(hit);if(cell)IUIAutomationElement_Release(cell);
 if(SUCCEEDED(result))puts("WINDOWS TABLE HIT: actual visible cell resolved from screen point");return result;
}
static HRESULT validate_document_table(IUIAutomation *automation,IUIAutomationElement *element,IUIAutomationTextPattern *text,
 IUIAutomationGridPattern **retained_grid,IUIAutomationElement **retained_table) {
 IUIAutomationGridPattern *grid=NULL;IUIAutomationTablePattern *table=NULL;
 IUIAutomationGridItemPattern *item=NULL;IUIAutomationTableItemPattern *table_item=NULL;
 IUIAutomationElementArray *headers=NULL,*items=NULL;IUIAutomationElement *cell=NULL,*header=NULL,*parent=NULL;
 IUIAutomationTreeWalker *walker=NULL;IUIAutomationTextRange *range=NULL;IUIAutomationElement *enclosing=NULL;
 BSTR name=NULL;BOOL same=FALSE;int count=0;enum RowOrColumnMajor major=RowOrColumnMajor_Indeterminate;
 HRESULT result=table_client_pattern(element,UIA_GridPatternId,&IID_IUIAutomationGridPattern,(void**)&grid);
#define TABLE_STEP(x) do {if(SUCCEEDED(result)){if(getenv("PHYSIM_TEST_TRACE"))fprintf(stderr,"WINDOWS TABLE CALL: %s\n",#x);result=(x);}}while(0)
#define TABLE_REQUIRE(x) do {if(SUCCEEDED(result) && !(x))result=E_FAIL;}while(0)
 TABLE_STEP(table_client_pattern(element,UIA_TablePatternId,&IID_IUIAutomationTablePattern,(void**)&table));
 TABLE_STEP(IUIAutomationGridPattern_get_CurrentRowCount(grid,&count));TABLE_REQUIRE(count==2);
 TABLE_STEP(IUIAutomationGridPattern_get_CurrentColumnCount(grid,&count));TABLE_REQUIRE(count==2);
 TABLE_STEP(IUIAutomationTablePattern_get_CurrentRowOrColumnMajor(table,&major));TABLE_REQUIRE(major==RowOrColumnMajor_RowMajor);
 TABLE_STEP(IUIAutomationTablePattern_GetCurrentColumnHeaders(table,&headers));
 TABLE_STEP(IUIAutomationElementArray_get_Length(headers,&count));TABLE_REQUIRE(count==2);
 for(int i=0;SUCCEEDED(result) && i<2;i++) {
  CONTROLTYPEID role=0;
  result=IUIAutomationElementArray_GetElement(headers,i,&header);
  TABLE_STEP(IUIAutomationElement_get_CurrentName(header,&name));TABLE_REQUIRE(name && !wcscmp(name,i?L"Bedeutung":L"Begriff"));
  TABLE_STEP(IUIAutomationElement_get_CurrentControlType(header,&role));TABLE_REQUIRE(role==UIA_HeaderItemControlTypeId);
  SysFreeString(name);name=NULL;if(header)IUIAutomationElement_Release(header);header=NULL;
 }
 TABLE_STEP(IUIAutomationGridPattern_GetItem(grid,1,1,&cell));
 TABLE_STEP(IUIAutomationElement_get_CurrentName(cell,&name));TABLE_REQUIRE(name && !wcscmp(name,L"l|r in Metern; C"));SysFreeString(name);name=NULL;
 TABLE_STEP(table_client_pattern(cell,UIA_GridItemPatternId,&IID_IUIAutomationGridItemPattern,(void**)&item));
 TABLE_STEP(table_client_pattern(cell,UIA_TableItemPatternId,&IID_IUIAutomationTableItemPattern,(void**)&table_item));
 TABLE_STEP(IUIAutomationGridItemPattern_get_CurrentRow(item,&count));TABLE_REQUIRE(count==1);
 TABLE_STEP(IUIAutomationGridItemPattern_get_CurrentColumn(item,&count));TABLE_REQUIRE(count==1);
 TABLE_STEP(IUIAutomationGridItemPattern_get_CurrentRowSpan(item,&count));TABLE_REQUIRE(count==1);
 TABLE_STEP(IUIAutomationGridItemPattern_get_CurrentColumnSpan(item,&count));TABLE_REQUIRE(count==1);
 TABLE_STEP(IUIAutomationGridItemPattern_get_CurrentContainingGrid(item,&parent));
 TABLE_STEP(IUIAutomation_CompareElements(automation,parent,element,&same));TABLE_REQUIRE(same);
 if(parent)IUIAutomationElement_Release(parent);parent=NULL;
 TABLE_STEP(IUIAutomation_get_RawViewWalker(automation,&walker));
 TABLE_STEP(IUIAutomationTreeWalker_GetParentElement(walker,cell,&parent));
 TABLE_STEP(IUIAutomation_CompareElements(automation,parent,element,&same));TABLE_REQUIRE(same);
 TABLE_STEP(IUIAutomationTableItemPattern_GetCurrentColumnHeaderItems(table_item,&items));
 TABLE_STEP(IUIAutomationElementArray_get_Length(items,&count));TABLE_REQUIRE(count==1);
 TABLE_STEP(IUIAutomationElementArray_GetElement(items,0,&header));
 TABLE_STEP(IUIAutomationElement_get_CurrentName(header,&name));TABLE_REQUIRE(name && !wcscmp(name,L"Bedeutung"));SysFreeString(name);name=NULL;
 TABLE_STEP(IUIAutomationTextPattern_RangeFromChild(text,cell,&range));
 TABLE_STEP(IUIAutomationTextRange_GetText(range,-1,&name));TABLE_REQUIRE(name && !wcscmp(name,L"l|r in Metern; C"));SysFreeString(name);name=NULL;
 TABLE_STEP(IUIAutomationTextRange_GetEnclosingElement(range,&enclosing));
 TABLE_STEP(IUIAutomation_CompareElements(automation,enclosing,cell,&same));TABLE_REQUIRE(same);
 if(SUCCEEDED(result) && !*retained_grid){*retained_grid=grid;IUIAutomationGridPattern_AddRef(grid);*retained_table=element;IUIAutomationElement_AddRef(element);}
 if(enclosing)IUIAutomationElement_Release(enclosing);if(range)IUIAutomationTextRange_Release(range);
 if(parent)IUIAutomationElement_Release(parent);if(walker)IUIAutomationTreeWalker_Release(walker);
 if(header)IUIAutomationElement_Release(header);if(cell)IUIAutomationElement_Release(cell);
 if(items)IUIAutomationElementArray_Release(items);if(headers)IUIAutomationElementArray_Release(headers);
 if(table_item)IUIAutomationTableItemPattern_Release(table_item);if(item)IUIAutomationGridItemPattern_Release(item);
 if(table)IUIAutomationTablePattern_Release(table);if(grid)IUIAutomationGridPattern_Release(grid);
 if(SUCCEEDED(result))puts("WINDOWS TABLE CLIENT: 2x2 Grid/Table, headers, cell coordinates, parents and exact text ranges passed");
#undef TABLE_STEP
#undef TABLE_REQUIRE
 return result;
}
/* Walk the TextPattern hierarchy: document -> table -> cell -> hyperlink.
 * A cell consisting only of a link has that innermost link as its enclosing
 * element, so it is collected before asking for further children. */
static HRESULT collect_document_links(IUIAutomation *automation,IUIAutomationTextPattern *pattern,IUIAutomationTextRange *range,
 unsigned depth,IUIAutomationElement **links,int *used,IUIAutomationGridPattern **grid,IUIAutomationElement **table) {
 if(depth>8)return E_FAIL;
 IUIAutomationElement *enclosing=NULL;CONTROLTYPEID role=0;
 HRESULT result=IUIAutomationTextRange_GetEnclosingElement(range,&enclosing);
 if(SUCCEEDED(result))result=IUIAutomationElement_get_CurrentControlType(enclosing,&role);
 if(SUCCEEDED(result) && role==UIA_HyperlinkControlTypeId) {
  if(*used>=2)result=E_FAIL;else {links[(*used)++]=enclosing;enclosing=NULL;}
  if(enclosing)IUIAutomationElement_Release(enclosing);return result;
 }
 if(SUCCEEDED(result) && role==UIA_TableControlTypeId)result=validate_document_table(automation,enclosing,pattern,grid,table);
 if(enclosing)IUIAutomationElement_Release(enclosing);
 IUIAutomationElementArray *children=NULL;
 if(SUCCEEDED(result))result=IUIAutomationTextRange_GetChildren(range,&children);
 int count=0;if(SUCCEEDED(result))result=IUIAutomationElementArray_get_Length(children,&count);
 for(int i=0;SUCCEEDED(result) && i<count;i++) {
  IUIAutomationElement *child=NULL;IUIAutomationTextRange *part=NULL;
  result=IUIAutomationElementArray_GetElement(children,i,&child);
  if(SUCCEEDED(result))result=IUIAutomationTextPattern_RangeFromChild(pattern,child,&part);
  if(SUCCEEDED(result))result=collect_document_links(automation,pattern,part,depth+1,links,used,grid,table);
  if(part)IUIAutomationTextRange_Release(part);if(child)IUIAutomationElement_Release(child);
 }
 if(children)IUIAutomationElementArray_Release(children);return result;
}
