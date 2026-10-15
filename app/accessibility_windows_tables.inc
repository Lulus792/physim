/* Lazy providers share the document's stable table/cell IDs. The list is weak:
 * held COM references retain the bridge, and the last release removes the
 * provider under the recursive SDL mutex. Merely reading a large table does
 * not permanently fill the interactive-provider cache. */
struct uia_table_element {
 IRawElementProviderSimple simple;IRawElementProviderFragment fragment;
 IGridProvider grid;ITableProvider table;IGridItemProvider grid_item;
 ITableItemProvider table_item;IScrollItemProvider scroll;
 LONG references;ps_a11y_native *bridge;uint64_t id;bool cell,listed;
 uia_table_element *next,*event_next;
 uia_link_state previous,event_old,event_new;
};
#define WT_OWNER(type,member,p) ((uia_table_element*)((char*)(p)-offsetof(uia_table_element,member)))
static ULONG wt_addref(uia_table_element *e){return (ULONG)InterlockedIncrement(&e->references);}
static ULONG wt_release(uia_table_element *e) {
 ps_a11y_native *b=e->bridge;SDL_LockMutex(b->mutex);LONG refs=InterlockedDecrement(&e->references);
 if(!refs && e->listed) {
  uia_table_element **at=&b->table_elements;while(*at && *at!=e)at=&(*at)->next;
  if(*at)*at=e->next;e->listed=false;
 }
 SDL_UnlockMutex(b->mutex);
 if(!refs){free(e);uia_bridge_release(b);}return (ULONG)refs;
}
static bool wt_live_locked(uia_table_element *e,ps_a11y_table *table,ps_a11y_table_cell *cell) {
 if(!e->bridge->model)return false;
 return e->cell?ps_a11y_table_cell_find(e->bridge->model,e->id,cell):ps_a11y_table_get(e->bridge->model,e->id,table);
}
static HRESULT wt_query(uia_table_element *e,REFIID iid,void **out) {
 if(!out)return E_POINTER;*out=NULL;
 if(IsEqualIID(iid,&IID_IUnknown) || IsEqualIID(iid,&IID_IRawElementProviderSimple))*out=&e->simple;
 else if(IsEqualIID(iid,&IID_IRawElementProviderFragment))*out=&e->fragment;
 else if(!e->cell && IsEqualIID(iid,&IID_IGridProvider))*out=&e->grid;
 else if(!e->cell && IsEqualIID(iid,&IID_ITableProvider))*out=&e->table;
 else if(e->cell && IsEqualIID(iid,&IID_IGridItemProvider))*out=&e->grid_item;
 else if(e->cell && IsEqualIID(iid,&IID_ITableItemProvider))*out=&e->table_item;
 else if(e->cell && IsEqualIID(iid,&IID_IScrollItemProvider))*out=&e->scroll;
 if(!*out)return E_NOINTERFACE;wt_addref(e);return S_OK;
}
#define WT_UNKNOWN(type,member,prefix) \
 static HRESULT STDMETHODCALLTYPE prefix##_query(type *p,REFIID iid,void **out){return wt_query(WT_OWNER(type,member,p),iid,out);} \
 static ULONG STDMETHODCALLTYPE prefix##_addref(type *p){return wt_addref(WT_OWNER(type,member,p));} \
 static ULONG STDMETHODCALLTYPE prefix##_release(type *p){return wt_release(WT_OWNER(type,member,p));}
WT_UNKNOWN(IRawElementProviderSimple,simple,wt_simple)
WT_UNKNOWN(IRawElementProviderFragment,fragment,wt_fragment)
WT_UNKNOWN(IGridProvider,grid,wt_grid)
WT_UNKNOWN(ITableProvider,table,wt_table)
WT_UNKNOWN(IGridItemProvider,grid_item,wt_grid_item)
WT_UNKNOWN(ITableItemProvider,table_item,wt_table_item)
WT_UNKNOWN(IScrollItemProvider,scroll,wt_scroll)
static bool wt_state_locked(uia_table_element *e,uia_link_state *out) {
 ps_a11y_native *b=e->bridge;ps_a11y_table t={0};ps_a11y_table_cell c={0};
 if(!b->hwnd || !wt_live_locked(e,&t,&c))return false;
 *out=(uia_link_state){{0},false,true};
 if(!uia_window_viewable_locked(b))return true;
 float box[4];if(e->cell)memcpy(box,c.bounds,sizeof box);else if(!ps_a11y_table_bounds(b->model,e->id,box))return false;
 POINT origin={0};if(!ClientToScreen(b->hwnd,&origin))return false;
 out->enabled=IsWindowEnabled(b->hwnd)!=0;out->offscreen=box[2]<=0 || box[3]<=0;
 if(!out->offscreen){out->bounds[0]=origin.x+box[0]*b->scale_x;out->bounds[1]=origin.y+box[1]*b->scale_y;
  out->bounds[2]=box[2]*b->scale_x;out->bounds[3]=box[3]*b->scale_y;}
 return true;
}
static HRESULT STDMETHODCALLTYPE wt_options(IRawElementProviderSimple *p,enum ProviderOptions *out) {
 (void)p;if(!out)return E_POINTER;*out=ProviderOptions_ServerSideProvider|ProviderOptions_UseComThreading;return S_OK;
}
static HRESULT STDMETHODCALLTYPE wt_pattern(IRawElementProviderSimple *p,PATTERNID pattern,IUnknown **out) {
 if(!out)return E_POINTER;*out=NULL;uia_table_element *e=WT_OWNER(IRawElementProviderSimple,simple,p);
 ps_a11y_table t={0};ps_a11y_table_cell c={0};SDL_LockMutex(e->bridge->mutex);bool live=wt_live_locked(e,&t,&c);SDL_UnlockMutex(e->bridge->mutex);
 if(!live)return UIA_E_ELEMENTNOTAVAILABLE;
 REFIID iid=pattern==UIA_GridPatternId?&IID_IGridProvider:pattern==UIA_TablePatternId?&IID_ITableProvider:
  pattern==UIA_GridItemPatternId?&IID_IGridItemProvider:pattern==UIA_TableItemPatternId?&IID_ITableItemProvider:
  pattern==UIA_ScrollItemPatternId?&IID_IScrollItemProvider:NULL;
 if(!iid)return S_OK;HRESULT result=wt_query(e,iid,(void**)out);return result==E_NOINTERFACE?S_OK:result;
}
static HRESULT wt_slice(const char *text,size_t length,VARIANT *out) {
 int n=length?MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,text,(int)length,NULL,0):0;
 if(length && !n)return E_INVALIDARG;BSTR value=SysAllocStringLen(NULL,(UINT)n);if(!value)return E_OUTOFMEMORY;
 if(n && !MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,text,(int)length,value,n)){SysFreeString(value);return E_INVALIDARG;}
 out->vt=VT_BSTR;out->bstrVal=value;return S_OK;
}
static HRESULT STDMETHODCALLTYPE wt_property(IRawElementProviderSimple *p,PROPERTYID property,VARIANT *out) {
 if(!out)return E_POINTER;VariantInit(out);uia_table_element *e=WT_OWNER(IRawElementProviderSimple,simple,p);ps_a11y_native *b=e->bridge;
 ps_a11y_table t={0};ps_a11y_table_cell c={0};SDL_LockMutex(b->mutex);
 if(!wt_live_locked(e,&t,&c)){SDL_UnlockMutex(b->mutex);return UIA_E_ELEMENTNOTAVAILABLE;}
 HRESULT result=S_OK;uia_link_state state={0};bool has_state=wt_state_locked(e,&state);
 if(property==UIA_NamePropertyId) {
  if(e->cell)result=wt_slice(b->model->document->text+c.begin,c.end-c.begin,out);
  else {char name[384];snprintf(name,sizeof name,"Tabelle: %s",b->model->document->name);result=uia_string(name,out);}
 } else if(property==UIA_AutomationIdPropertyId) {
  char id[96];snprintf(id,sizeof id,"physim-%llu-%llu",(unsigned long long)(uintptr_t)b->hwnd,(unsigned long long)e->id);result=uia_string(id,out);
 } else if(property==UIA_FrameworkIdPropertyId)result=uia_string("Physim",out);
 else if(property==UIA_ControlTypePropertyId){out->vt=VT_I4;out->lVal=e->cell?(c.column_header?UIA_HeaderItemControlTypeId:UIA_TextControlTypeId):UIA_TableControlTypeId;}
 else if(property==UIA_IsControlElementPropertyId || property==UIA_IsContentElementPropertyId){out->vt=VT_BOOL;out->boolVal=VARIANT_TRUE;}
 else if(property==UIA_IsKeyboardFocusablePropertyId || property==UIA_HasKeyboardFocusPropertyId){out->vt=VT_BOOL;out->boolVal=VARIANT_FALSE;}
 else if(property==UIA_IsEnabledPropertyId){out->vt=VT_BOOL;out->boolVal=has_state && state.enabled?VARIANT_TRUE:VARIANT_FALSE;}
 else if(property==UIA_IsOffscreenPropertyId){out->vt=VT_BOOL;out->boolVal=!has_state || state.offscreen?VARIANT_TRUE:VARIANT_FALSE;}
 SDL_UnlockMutex(b->mutex);return result;
}
static HRESULT STDMETHODCALLTYPE wt_host(IRawElementProviderSimple *p,IRawElementProviderSimple **out) {
 (void)p;if(!out)return E_POINTER;*out=NULL;return S_OK;
}
static HRESULT wt_navigate_id_locked(ps_a11y_native *b,uint64_t current,enum NavigateDirection direction,IRawElementProviderFragment **out) {
 uint64_t parent=0,id=0;bool found=false;
 if(!uia_semantic_parent_locked(b,current,&parent))return UIA_E_ELEMENTNOTAVAILABLE;
 if(direction==NavigateDirection_Parent)found=current!=0,id=parent;
 else if(direction==NavigateDirection_FirstChild || direction==NavigateDirection_LastChild) {
  size_t count=uia_semantic_count_locked(b,current);
  if(count)found=uia_semantic_child_locked(b,current,direction==NavigateDirection_FirstChild?0:count-1,&id);
 } else if(direction==NavigateDirection_NextSibling || direction==NavigateDirection_PreviousSibling) {
  size_t count=uia_semantic_count_locked(b,parent),index=0;uint64_t child=0;
  for(;index<count;index++)if(uia_semantic_child_locked(b,parent,index,&child) && child==current)break;
  if(direction==NavigateDirection_NextSibling && index+1<count)found=uia_semantic_child_locked(b,parent,index+1,&id);
  else if(direction==NavigateDirection_PreviousSibling && index && index<count)found=uia_semantic_child_locked(b,parent,index-1,&id);
 } else return E_INVALIDARG;
 if(found){*out=uia_any_fragment_locked(b,id);if(!*out)return E_OUTOFMEMORY;}return S_OK;
}
static HRESULT STDMETHODCALLTYPE wt_navigate(IRawElementProviderFragment *p,enum NavigateDirection direction,IRawElementProviderFragment **out) {
 if(!out)return E_POINTER;*out=NULL;uia_table_element *e=WT_OWNER(IRawElementProviderFragment,fragment,p);
 SDL_LockMutex(e->bridge->mutex);HRESULT result=wt_navigate_id_locked(e->bridge,e->id,direction,out);SDL_UnlockMutex(e->bridge->mutex);return result;
}
static HRESULT STDMETHODCALLTYPE wt_runtime(IRawElementProviderFragment *p,SAFEARRAY **out) {
 if(!out)return E_POINTER;*out=NULL;uia_table_element *e=WT_OWNER(IRawElementProviderFragment,fragment,p);
 ps_a11y_table t={0};ps_a11y_table_cell c={0};SDL_LockMutex(e->bridge->mutex);bool live=wt_live_locked(e,&t,&c);SDL_UnlockMutex(e->bridge->mutex);
 if(!live)return UIA_E_ELEMENTNOTAVAILABLE;SAFEARRAY *ids=SafeArrayCreateVector(VT_I4,0,3);if(!ids)return E_OUTOFMEMORY;
 LONG values[]={UiaAppendRuntimeId,(LONG)(e->id>>32),(LONG)(e->id&UINT32_MAX)};
 for(LONG i=0;i<3;i++)if(FAILED(SafeArrayPutElement(ids,&i,&values[i]))){SafeArrayDestroy(ids);return E_OUTOFMEMORY;}
 *out=ids;return S_OK;
}
static HRESULT STDMETHODCALLTYPE wt_rectangle(IRawElementProviderFragment *p,struct UiaRect *out) {
 if(!out)return E_POINTER;uia_table_element *e=WT_OWNER(IRawElementProviderFragment,fragment,p);uia_link_state state={0};
 SDL_LockMutex(e->bridge->mutex);bool live=wt_state_locked(e,&state);SDL_UnlockMutex(e->bridge->mutex);
 if(!live)return UIA_E_ELEMENTNOTAVAILABLE;*out=(struct UiaRect){state.bounds[0],state.bounds[1],state.bounds[2],state.bounds[3]};return S_OK;
}
static HRESULT STDMETHODCALLTYPE wt_embedded(IRawElementProviderFragment *p,SAFEARRAY **out){(void)p;if(!out)return E_POINTER;*out=NULL;return S_OK;}
static HRESULT STDMETHODCALLTYPE wt_focus(IRawElementProviderFragment *p) {
 uia_table_element *e=WT_OWNER(IRawElementProviderFragment,fragment,p);ps_a11y_table t={0};ps_a11y_table_cell c={0};
 SDL_LockMutex(e->bridge->mutex);bool live=wt_live_locked(e,&t,&c);SDL_UnlockMutex(e->bridge->mutex);
 return live?UIA_E_INVALIDOPERATION:UIA_E_ELEMENTNOTAVAILABLE;
}
static HRESULT STDMETHODCALLTYPE wt_root(IRawElementProviderFragment *p,IRawElementProviderFragmentRoot **out) {
 if(!out)return E_POINTER;*out=NULL;uia_table_element *e=WT_OWNER(IRawElementProviderFragment,fragment,p);
 SDL_LockMutex(e->bridge->mutex);ps_a11y_table t={0};ps_a11y_table_cell c={0};bool live=wt_live_locked(e,&t,&c);
 if(live){uia_addref(e->bridge->root);*out=&e->bridge->root->root;}SDL_UnlockMutex(e->bridge->mutex);return live?S_OK:UIA_E_ELEMENTNOTAVAILABLE;
}
static HRESULT wt_dimension(uia_table_element *e,bool columns,int *out) {
 if(!out)return E_POINTER;ps_a11y_table t={0};ps_a11y_table_cell c={0};SDL_LockMutex(e->bridge->mutex);bool live=wt_live_locked(e,&t,&c);
 if(live)*out=(int)(columns?t.columns:t.rows);SDL_UnlockMutex(e->bridge->mutex);return live?S_OK:UIA_E_ELEMENTNOTAVAILABLE;
}
static HRESULT STDMETHODCALLTYPE wt_rows(IGridProvider *p,int *out){return wt_dimension(WT_OWNER(IGridProvider,grid,p),false,out);}
static HRESULT STDMETHODCALLTYPE wt_columns(IGridProvider *p,int *out){return wt_dimension(WT_OWNER(IGridProvider,grid,p),true,out);}
static HRESULT STDMETHODCALLTYPE wt_get_item(IGridProvider *p,int row,int column,IRawElementProviderSimple **out) {
 if(!out)return E_POINTER;*out=NULL;uia_table_element *e=WT_OWNER(IGridProvider,grid,p);ps_a11y_native *b=e->bridge;
 SDL_LockMutex(b->mutex);ps_a11y_table t={0};ps_a11y_table_cell c={0};HRESULT result=S_OK;
 if(!wt_live_locked(e,&t,&c))result=UIA_E_ELEMENTNOTAVAILABLE;
 else if(row<0 || column<0 || (size_t)row>=t.rows || (size_t)column>=t.columns)result=E_INVALIDARG;
 else if(ps_a11y_table_cell_get(b->model,t.id,(size_t)row,(size_t)column,&c)) {
  *out=uia_any_locked(b,c.id);if(!*out)result=E_OUTOFMEMORY;
 } else result=UIA_E_ELEMENTNOTAVAILABLE;
 SDL_UnlockMutex(b->mutex);return result;
}
static HRESULT wt_headers(uia_table_element *e,bool columns,SAFEARRAY **out) {
 if(!out)return E_POINTER;*out=NULL;ps_a11y_native *b=e->bridge;SDL_LockMutex(b->mutex);
 ps_a11y_table t={0};ps_a11y_table_cell c={0};HRESULT result=S_OK;
 if(!wt_live_locked(e,&t,&c))result=UIA_E_ELEMENTNOTAVAILABLE;
 else if(e->cell && !ps_a11y_table_get(b->model,c.table,&t))result=UIA_E_ELEMENTNOTAVAILABLE;
 size_t count=SUCCEEDED(result) && columns && t.column_headers?(e->cell?1:t.columns):0;
 if(SUCCEEDED(result)){*out=SafeArrayCreateVector(VT_UNKNOWN,0,(ULONG)count);if(!*out)result=E_OUTOFMEMORY;}
 for(LONG i=0;SUCCEEDED(result) && i<(LONG)count;i++) {
  ps_a11y_table_cell h={0};
  if(!ps_a11y_table_column_header(b->model,t.id,e->cell?c.column:(size_t)i,&h)){result=UIA_E_ELEMENTNOTAVAILABLE;break;}
  IRawElementProviderSimple *provider=uia_any_locked(b,h.id);if(!provider){result=E_OUTOFMEMORY;break;}
  result=SafeArrayPutElement(*out,&i,provider);IRawElementProviderSimple_Release(provider);
 }
 SDL_UnlockMutex(b->mutex);if(FAILED(result) && *out){SafeArrayDestroy(*out);*out=NULL;}return result;
}
static HRESULT STDMETHODCALLTYPE wt_column_headers(ITableProvider *p,SAFEARRAY **out){return wt_headers(WT_OWNER(ITableProvider,table,p),true,out);}
static HRESULT STDMETHODCALLTYPE wt_row_headers(ITableProvider *p,SAFEARRAY **out){return wt_headers(WT_OWNER(ITableProvider,table,p),false,out);}
static HRESULT STDMETHODCALLTYPE wt_column_items(ITableItemProvider *p,SAFEARRAY **out){return wt_headers(WT_OWNER(ITableItemProvider,table_item,p),true,out);}
static HRESULT STDMETHODCALLTYPE wt_row_items(ITableItemProvider *p,SAFEARRAY **out){return wt_headers(WT_OWNER(ITableItemProvider,table_item,p),false,out);}
static HRESULT STDMETHODCALLTYPE wt_major(ITableProvider *p,enum RowOrColumnMajor *out) {
 if(!out)return E_POINTER;uia_table_element *e=WT_OWNER(ITableProvider,table,p);ps_a11y_table t={0};ps_a11y_table_cell c={0};
 SDL_LockMutex(e->bridge->mutex);bool live=wt_live_locked(e,&t,&c);SDL_UnlockMutex(e->bridge->mutex);
 if(live)*out=RowOrColumnMajor_RowMajor;return live?S_OK:UIA_E_ELEMENTNOTAVAILABLE;
}
static HRESULT wt_coordinate(uia_table_element *e,bool column,bool span,int *out) {
 if(!out)return E_POINTER;ps_a11y_table t={0};ps_a11y_table_cell c={0};SDL_LockMutex(e->bridge->mutex);bool live=wt_live_locked(e,&t,&c);
 if(live)*out=span?1:(int)(column?c.column:c.row);SDL_UnlockMutex(e->bridge->mutex);return live?S_OK:UIA_E_ELEMENTNOTAVAILABLE;
}
static HRESULT STDMETHODCALLTYPE wt_item_row(IGridItemProvider *p,int *out){return wt_coordinate(WT_OWNER(IGridItemProvider,grid_item,p),false,false,out);}
static HRESULT STDMETHODCALLTYPE wt_item_column(IGridItemProvider *p,int *out){return wt_coordinate(WT_OWNER(IGridItemProvider,grid_item,p),true,false,out);}
static HRESULT STDMETHODCALLTYPE wt_row_span(IGridItemProvider *p,int *out){return wt_coordinate(WT_OWNER(IGridItemProvider,grid_item,p),false,true,out);}
static HRESULT STDMETHODCALLTYPE wt_column_span(IGridItemProvider *p,int *out){return wt_coordinate(WT_OWNER(IGridItemProvider,grid_item,p),true,true,out);}
static HRESULT STDMETHODCALLTYPE wt_containing_grid(IGridItemProvider *p,IRawElementProviderSimple **out) {
 if(!out)return E_POINTER;*out=NULL;uia_table_element *e=WT_OWNER(IGridItemProvider,grid_item,p);ps_a11y_table t={0};ps_a11y_table_cell c={0};
 SDL_LockMutex(e->bridge->mutex);bool live=wt_live_locked(e,&t,&c);
 if(live)*out=uia_any_locked(e->bridge,c.table);SDL_UnlockMutex(e->bridge->mutex);
 return !live?UIA_E_ELEMENTNOTAVAILABLE:*out?S_OK:E_OUTOFMEMORY;
}
static HRESULT STDMETHODCALLTYPE wt_scroll_into_view(IScrollItemProvider *p) {
 uia_table_element *e=WT_OWNER(IScrollItemProvider,scroll,p);ps_a11y_native *b=e->bridge;SDL_LockMutex(b->mutex);
 ps_a11y_table t={0};ps_a11y_table_cell c={0};HRESULT result=!wt_live_locked(e,&t,&c)?UIA_E_ELEMENTNOTAVAILABLE:
  !uia_window_viewable_locked(b) || !IsWindowEnabled(b->hwnd)?UIA_E_ELEMENTNOTENABLED:
  ps_a11y_table_cell_scroll(b->model,e->id)?S_OK:UIA_E_INVALIDOPERATION;
 SDL_UnlockMutex(b->mutex);return result;
}
static const IRawElementProviderSimpleVtbl wt_simple_vtable={wt_simple_query,wt_simple_addref,wt_simple_release,wt_options,wt_pattern,wt_property,wt_host};
static const IRawElementProviderFragmentVtbl wt_fragment_vtable={wt_fragment_query,wt_fragment_addref,wt_fragment_release,wt_navigate,wt_runtime,wt_rectangle,wt_embedded,wt_focus,wt_root};
static const IGridProviderVtbl wt_grid_vtable={wt_grid_query,wt_grid_addref,wt_grid_release,wt_get_item,wt_rows,wt_columns};
static const ITableProviderVtbl wt_table_vtable={wt_table_query,wt_table_addref,wt_table_release,wt_row_headers,wt_column_headers,wt_major};
static const IGridItemProviderVtbl wt_grid_item_vtable={wt_grid_item_query,wt_grid_item_addref,wt_grid_item_release,wt_item_row,wt_item_column,wt_row_span,wt_column_span,wt_containing_grid};
static const ITableItemProviderVtbl wt_table_item_vtable={wt_table_item_query,wt_table_item_addref,wt_table_item_release,wt_row_items,wt_column_items};
static const IScrollItemProviderVtbl wt_scroll_vtable={wt_scroll_query,wt_scroll_addref,wt_scroll_release,wt_scroll_into_view};
static IRawElementProviderSimple *uia_any_locked(ps_a11y_native *b,uint64_t id) {
 if(!b->model)return NULL;ps_a11y_table t={0};ps_a11y_table_cell c={0};
 bool table=ps_a11y_table_get(b->model,id,&t),cell=!table && ps_a11y_table_cell_find(b->model,id,&c);
 if(!table && !cell){uia_element *e=uia_for_locked(b,id);return e?&e->simple:NULL;}
 for(uia_table_element *e=b->table_elements;e;e=e->next)if(e->id==id){wt_addref(e);return &e->simple;}
 uia_table_element *e=calloc(1,sizeof *e);if(!e)return NULL;
 e->simple.lpVtbl=&wt_simple_vtable;e->fragment.lpVtbl=&wt_fragment_vtable;
 e->grid.lpVtbl=&wt_grid_vtable;e->table.lpVtbl=&wt_table_vtable;e->grid_item.lpVtbl=&wt_grid_item_vtable;
 e->table_item.lpVtbl=&wt_table_item_vtable;e->scroll.lpVtbl=&wt_scroll_vtable;
 e->references=1;e->bridge=b;e->id=id;e->cell=cell;e->listed=true;e->next=b->table_elements;b->table_elements=e;
 InterlockedIncrement(&b->references);wt_state_locked(e,&e->previous);return &e->simple;
}
static IRawElementProviderFragment *uia_any_fragment_locked(ps_a11y_native *b,uint64_t id) {
 IRawElementProviderSimple *provider=uia_any_locked(b,id);if(!provider)return NULL;
 IRawElementProviderFragment *fragment=NULL;IRawElementProviderSimple_QueryInterface(provider,&IID_IRawElementProviderFragment,(void**)&fragment);
 IRawElementProviderSimple_Release(provider);return fragment;
}
static bool uia_semantic_parent_locked(ps_a11y_native *b,uint64_t id,uint64_t *parent) {
 if(!b->model)return false;if(!id){*parent=0;return true;}
 ps_a11y_table t={0};ps_a11y_table_cell c={0};
 if(ps_a11y_table_get(b->model,id,&t)){*parent=0;return true;}
 if(ps_a11y_table_cell_find(b->model,id,&c)){*parent=c.table;return true;}
 const ps_a11y_node *n=ps_a11y_find(b->model,id);if(!n)return false;
 if(n->role==PS_A11Y_LINK && n->document_version && ps_a11y_table_cell_for_range(b->model,n->document_begin,n->document_end,&c))*parent=c.id;
 else *parent=n->parent;return true;
}
typedef struct {uint64_t id;size_t start,order;} wt_child;
static int wt_child_compare(const void *a,const void *b) {
 const wt_child *x=a,*y=b;if(x->start!=y->start)return x->start<y->start?-1:1;
 return x->order==y->order?0:x->order<y->order?-1:1;
}
static size_t wt_children_locked(ps_a11y_native *b,uint64_t parent,wt_child *out) {
 size_t count=0;
 for(size_t i=0;i<b->model->count;i++) {
  const ps_a11y_node *n=&b->model->nodes[i];uint64_t p=0;
  if(uia_semantic_parent_locked(b,n->id,&p) && p==parent)out[count++]=(wt_child){n->id,n->document_version?n->document_begin+1:0,i};
 }
 if(!parent)for(size_t i=0;i<ps_a11y_table_count(b->model);i++) {
  ps_a11y_table t={0};if(ps_a11y_table_at(b->model,i,&t))out[count++]=(wt_child){t.id,t.begin+1,PS_A11Y_MAX_NODES+i};
 }
 qsort(out,count,sizeof *out,wt_child_compare);return count;
}
static size_t uia_semantic_count_locked(ps_a11y_native *b,uint64_t parent) {
 if(!b->model)return 0;ps_a11y_table t={0};
 if(ps_a11y_table_get(b->model,parent,&t))return t.rows*t.columns;
 wt_child children[PS_A11Y_MAX_NODES+PS_A11Y_TABLE_LIMIT];return wt_children_locked(b,parent,children);
}
static bool uia_semantic_child_locked(ps_a11y_native *b,uint64_t parent,size_t index,uint64_t *id) {
 if(!b->model)return false;ps_a11y_table t={0};
 if(ps_a11y_table_get(b->model,parent,&t)) {if(index>=t.rows*t.columns)return false;*id=t.id+1+index;return true;}
 wt_child children[PS_A11Y_MAX_NODES+PS_A11Y_TABLE_LIMIT];size_t count=wt_children_locked(b,parent,children);
 if(index>=count)return false;*id=children[index].id;return true;
}
static uint64_t uia_enclosing_table_locked(ps_a11y_native *b,size_t begin,size_t end) {
 ps_a11y_table_cell c={0};if(ps_a11y_table_cell_for_range(b->model,begin,end,&c))return c.id;
 for(size_t i=0;i<ps_a11y_table_count(b->model);i++) {
  ps_a11y_table t={0};if(ps_a11y_table_at(b->model,i,&t) && begin>=t.begin && end<=t.end)return t.id;
 }
 return 0;
}
static HRESULT wt_range_id_locked(ps_a11y_native *b,uint64_t id,size_t *begin,size_t *end) {
 ps_a11y_table t={0};ps_a11y_table_cell c={0};
 if(ps_a11y_table_get(b->model,id,&t)){*begin=t.begin;*end=t.end;return S_OK;}
 if(ps_a11y_table_cell_find(b->model,id,&c)){*begin=c.begin;*end=c.end;return S_OK;}
 const ps_a11y_node *n=ps_a11y_find(b->model,id);
 if(!n)return UIA_E_ELEMENTNOTAVAILABLE;
 if(n->role!=PS_A11Y_LINK || !b->model->document || n->document_version!=b->model->document->version)return E_INVALIDARG;
 *begin=n->document_begin;*end=n->document_end;return S_OK;
}
static HRESULT uia_child_range_locked(ps_a11y_native *b,IRawElementProviderSimple *child,size_t *begin,size_t *end,uint64_t *identity) {
 if(!child)return E_INVALIDARG;uint64_t id=0;
 if(child->lpVtbl==&wt_simple_vtable){uia_table_element *e=WT_OWNER(IRawElementProviderSimple,simple,child);if(e->bridge!=b)return E_INVALIDARG;id=e->id;}
 else if(child->lpVtbl==&uia_simple_vtable){uia_element *e=UIA_OWNER(IRawElementProviderSimple,simple,child);if(e->bridge!=b || !e->id)return E_INVALIDARG;id=e->id;}
 else return E_INVALIDARG;
 HRESULT result=wt_range_id_locked(b,id,begin,end);if(SUCCEEDED(result) && identity)*identity=id;return result;
}
static bool wt_intersects(size_t begin,size_t end,size_t first,size_t last) {
 return first==last?first>=begin && first<=end:first<end && last>begin;
}
static HRESULT uia_range_children_locked(ps_a11y_native *b,uint64_t version,size_t begin,size_t end,uint64_t parent,SAFEARRAY **out) {
 *out=NULL;if(!b->model || !b->model->document || b->model->document->version!=version)return UIA_E_ELEMENTNOTAVAILABLE;
 size_t count=uia_semantic_count_locked(b,parent),used=0;
 for(size_t i=0;i<count;i++) {
  uint64_t id=0;size_t first=0,last=0;
  if(uia_semantic_child_locked(b,parent,i,&id) && SUCCEEDED(wt_range_id_locked(b,id,&first,&last)) && wt_intersects(begin,end,first,last))++used;
 }
 SAFEARRAY *array=SafeArrayCreateVector(VT_UNKNOWN,0,(ULONG)used);if(!array)return E_OUTOFMEMORY;
 HRESULT result=S_OK;LONG at=0;
 for(size_t i=0;SUCCEEDED(result) && i<count;i++) {
  uint64_t id=0;size_t first=0,last=0;
  if(!uia_semantic_child_locked(b,parent,i,&id) || FAILED(wt_range_id_locked(b,id,&first,&last)) || !wt_intersects(begin,end,first,last))continue;
  IRawElementProviderSimple *provider=uia_any_locked(b,id);if(!provider){result=E_OUTOFMEMORY;break;}
  result=SafeArrayPutElement(array,&at,provider);IRawElementProviderSimple_Release(provider);++at;
 }
 if(FAILED(result))SafeArrayDestroy(array);else *out=array;return result;
}
static void wt_events(uia_table_element *e) {
 const uia_link_state *old=&e->event_old,*next=&e->event_new;
 if(memcmp(old->bounds,next->bounds,sizeof old->bounds)) {
  VARIANT a={0},b={0};if(SUCCEEDED(uia_rectangle_value(old->bounds,&a)) && SUCCEEDED(uia_rectangle_value(next->bounds,&b)))
   UiaRaiseAutomationPropertyChangedEvent(&e->simple,UIA_BoundingRectanglePropertyId,a,b);VariantClear(&a);VariantClear(&b);
 }
 if(old->enabled!=next->enabled){VARIANT a={0},b={0};a.vt=b.vt=VT_BOOL;a.boolVal=old->enabled?VARIANT_TRUE:VARIANT_FALSE;b.boolVal=next->enabled?VARIANT_TRUE:VARIANT_FALSE;
  UiaRaiseAutomationPropertyChangedEvent(&e->simple,UIA_IsEnabledPropertyId,a,b);}
 if(old->offscreen!=next->offscreen){VARIANT a={0},b={0};a.vt=b.vt=VT_BOOL;a.boolVal=old->offscreen?VARIANT_TRUE:VARIANT_FALSE;b.boolVal=next->offscreen?VARIANT_TRUE:VARIANT_FALSE;
  UiaRaiseAutomationPropertyChangedEvent(&e->simple,UIA_IsOffscreenPropertyId,a,b);}
}
static void uia_tables_update(ps_a11y_native *b,bool destroy) {
 uia_table_element *retired=NULL,*events=NULL;SDL_LockMutex(b->mutex);
 uia_table_element **at=&b->table_elements;
 while(*at) {
  uia_table_element *e=*at;uia_link_state next;
  if(destroy || !wt_state_locked(e,&next)) {
   wt_addref(e);*at=e->next;e->listed=false;e->next=retired;retired=e;continue;
  }
  if(memcmp(e->previous.bounds,next.bounds,sizeof next.bounds) || e->previous.enabled!=next.enabled || e->previous.offscreen!=next.offscreen) {
   wt_addref(e);e->event_old=e->previous;e->event_new=next;e->event_next=events;events=e;
  }
  e->previous=next;at=&e->next;
 }
 SDL_UnlockMutex(b->mutex);
 while(retired){uia_table_element *next=retired->next;UiaDisconnectProvider(&retired->simple);wt_release(retired);retired=next;}
 while(events){uia_table_element *next=events->event_next;wt_events(events);wt_release(events);events=next;}
}
static void uia_tables_publish(ps_a11y_native *b){uia_tables_update(b,false);}
static void uia_tables_destroy(ps_a11y_native *b){uia_tables_update(b,true);}
