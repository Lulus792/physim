/* Windows UI Automation provider for the same immutable published model used
 * by AX and AT-SPI. All client callbacks serialize model access; queued actions
 * are delivered by the next UI frame, never by a foreign COM thread. */
#define COBJMACROS
#define CONST_VTABLE
#include <windows.h>
#include <commctrl.h>
#include <uiautomation.h>
#include <stddef.h>
#include <wchar.h>
#include "text_validation.h"

typedef struct uia_element uia_element;
typedef struct uia_table_element uia_table_element;
typedef struct {double bounds[4];bool enabled,offscreen;} uia_link_state;
struct ps_a11y_native {
 SDL_Window *window;HWND hwnd;ps_a11y_model *model;SDL_Mutex *mutex;
 LONG references;bool com_initialized;double scale_x,scale_y;
 uia_element *root,*elements[PS_A11Y_MAX_NODES];
 uint64_t focused_id;bool document_focused;char title[256];
 uint64_t reading_owner,reading_version;BSTR reading_text;size_t reading_length;size_t *reading_bytes;
 uint64_t announced_reading_version;
 uia_table_element *table_elements;
};
struct uia_element {
 IRawElementProviderSimple simple;IRawElementProviderFragment fragment;
 IRawElementProviderFragmentRoot root;IInvokeProvider invoke;IToggleProvider toggle;
 ISelectionItemProvider item;ISelectionProvider selection;IRangeValueProvider range;
 ITextProvider text;
 ITextProvider2 text2;
 IValueProvider value;
 LONG references;ps_a11y_native *bridge;uint64_t id;ps_a11y_role role;
 char previous_text[PS_A11Y_LABEL_BYTES];
 size_t previous_start,previous_end,previous_caret;
 uia_link_state previous_link;bool has_link_state;
};
static uia_element *uia_new(ps_a11y_native *b,uint64_t id,ps_a11y_role role);
static IRawElementProviderSimple *uia_any_locked(ps_a11y_native *b,uint64_t id);
static IRawElementProviderFragment *uia_any_fragment_locked(ps_a11y_native *b,uint64_t id);
static bool uia_semantic_parent_locked(ps_a11y_native *b,uint64_t id,uint64_t *parent);
static size_t uia_semantic_count_locked(ps_a11y_native *b,uint64_t parent);
static bool uia_semantic_child_locked(ps_a11y_native *b,uint64_t parent,size_t index,uint64_t *id);
static uint64_t uia_enclosing_table_locked(ps_a11y_native *b,size_t begin,size_t end);
static HRESULT uia_child_range_locked(ps_a11y_native *b,IRawElementProviderSimple *child,size_t *begin,size_t *end,uint64_t *identity);
static HRESULT uia_range_children_locked(ps_a11y_native *b,uint64_t version,size_t begin,size_t end,uint64_t parent,SAFEARRAY **out);
static void uia_tables_publish(ps_a11y_native *b);
static void uia_tables_destroy(ps_a11y_native *b);
static void uia_bridge_release(ps_a11y_native *b) {
 if(!InterlockedDecrement(&b->references)){SysFreeString(b->reading_text);free(b->reading_bytes);SDL_DestroyMutex(b->mutex);free(b);}
}
static ULONG uia_addref(uia_element *e){return (ULONG)InterlockedIncrement(&e->references);}
static ULONG uia_release(uia_element *e) {
 LONG refs=InterlockedDecrement(&e->references);
 if(!refs){ps_a11y_native *b=e->bridge;free(e);uia_bridge_release(b);}return (ULONG)refs;
}
static HRESULT uia_query(uia_element *e,REFIID iid,void **out) {
 if(!out)return E_POINTER;*out=NULL;
 if(IsEqualIID(iid,&IID_IUnknown) || IsEqualIID(iid,&IID_IRawElementProviderSimple))*out=&e->simple;
 else if(IsEqualIID(iid,&IID_IRawElementProviderFragment))*out=&e->fragment;
 else if(!e->id && IsEqualIID(iid,&IID_IRawElementProviderFragmentRoot))*out=&e->root;
 else if((!e->id || e->role==PS_A11Y_EDIT) && IsEqualIID(iid,&IID_ITextProvider))*out=&e->text;
 else if(e->id && e->role==PS_A11Y_EDIT && IsEqualIID(iid,&IID_ITextProvider2))*out=&e->text2;
 else if(e->id && (e->role==PS_A11Y_BUTTON || e->role==PS_A11Y_LINK) && IsEqualIID(iid,&IID_IInvokeProvider))*out=&e->invoke;
 else if(e->id && e->role==PS_A11Y_CHECKBOX && IsEqualIID(iid,&IID_IToggleProvider))*out=&e->toggle;
 else if(e->id && e->role==PS_A11Y_RADIO && IsEqualIID(iid,&IID_ISelectionItemProvider))*out=&e->item;
 else if(e->id && e->role==PS_A11Y_RADIO_GROUP && IsEqualIID(iid,&IID_ISelectionProvider))*out=&e->selection;
 else if(e->id && ps_a11y_range_role(e->role) && IsEqualIID(iid,&IID_IRangeValueProvider))*out=&e->range;
 else if(e->id && e->role==PS_A11Y_EDIT && IsEqualIID(iid,&IID_IValueProvider))*out=&e->value;
 if(!*out)return E_NOINTERFACE;uia_addref(e);return S_OK;
}
#define UIA_OWNER(type,member,p) ((uia_element*)((char*)(p)-offsetof(uia_element,member)))
#define UIA_UNKNOWN(type,member,prefix) \
 static HRESULT STDMETHODCALLTYPE prefix##_query(type *p,REFIID iid,void **out){return uia_query(UIA_OWNER(type,member,p),iid,out);} \
 static ULONG STDMETHODCALLTYPE prefix##_addref(type *p){return uia_addref(UIA_OWNER(type,member,p));} \
 static ULONG STDMETHODCALLTYPE prefix##_release(type *p){return uia_release(UIA_OWNER(type,member,p));}
UIA_UNKNOWN(IRawElementProviderSimple,simple,uia_simple)
UIA_UNKNOWN(IRawElementProviderFragment,fragment,uia_fragment)
UIA_UNKNOWN(IRawElementProviderFragmentRoot,root,uia_root)
UIA_UNKNOWN(IInvokeProvider,invoke,uia_invoke)
UIA_UNKNOWN(IToggleProvider,toggle,uia_toggle)
UIA_UNKNOWN(ISelectionItemProvider,item,uia_item)
UIA_UNKNOWN(ISelectionProvider,selection,uia_selection)
UIA_UNKNOWN(IRangeValueProvider,range,uia_range)
UIA_UNKNOWN(ITextProvider,text,uia_text)
UIA_UNKNOWN(ITextProvider2,text2,uia_text2)
UIA_UNKNOWN(IValueProvider,value,uia_edit)
/* Caller holds the bridge mutex. Returned provider has its own COM reference. */
static uia_element *uia_for_locked(ps_a11y_native *b,uint64_t id) {
 if(!b->model)return NULL;
 if(!id){uia_addref(b->root);return b->root;}
 const ps_a11y_node *node=ps_a11y_find(b->model,id);if(!node)return NULL;
 for(size_t i=0;i<PS_A11Y_MAX_NODES;i++)if(b->elements[i] && b->elements[i]->id==id){uia_addref(b->elements[i]);return b->elements[i];}
 for(size_t i=0;i<PS_A11Y_MAX_NODES;i++)if(!b->elements[i]) {
  uia_element *e=uia_new(b,id,node->role);if(!e)return NULL;b->elements[i]=e;uia_addref(e);return e;
 }
 return NULL;
}
static bool uia_node(uia_element *e,ps_a11y_node *out) {
 ps_a11y_native *b=e->bridge;SDL_LockMutex(b->mutex);
 const ps_a11y_node *node=b->model?ps_a11y_find(b->model,e->id):NULL;
 if(node)*out=*node;SDL_UnlockMutex(b->mutex);return node!=NULL;
}
static HRESULT uia_string(const char *text,VARIANT *out) {
 int n=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,text,-1,NULL,0);if(!n)return E_INVALIDARG;
 BSTR value=SysAllocStringLen(NULL,(UINT)(n-1));if(!value)return E_OUTOFMEMORY;
 if(!MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,text,-1,value,n)){SysFreeString(value);return E_INVALIDARG;}
 out->vt=VT_BSTR;out->bstrVal=value;return S_OK;
}
static bool uia_window_viewable_locked(ps_a11y_native *b) {
 return b->model && !b->model->suspended && b->hwnd && IsWindowVisible(b->hwnd) && !IsIconic(b->hwnd);
}
static HRESULT uia_bounds(uia_element *e,struct UiaRect *out) {
 if(!out)return E_POINTER;ps_a11y_native *b=e->bridge;SDL_LockMutex(b->mutex);
 const ps_a11y_node *node=b->model && e->id?ps_a11y_find(b->model,e->id):NULL;
 if(!b->model || !b->hwnd || (e->id && !node)){SDL_UnlockMutex(b->mutex);return UIA_E_ELEMENTNOTAVAILABLE;}
 if(!uia_window_viewable_locked(b)){*out=(struct UiaRect){0};SDL_UnlockMutex(b->mutex);return S_OK;}
 POINT origin={0};RECT client={0};bool valid=ClientToScreen(b->hwnd,&origin) && GetClientRect(b->hwnd,&client);
 if(valid)*out=node?(struct UiaRect){origin.x+node->bounds[0]*b->scale_x,origin.y+node->bounds[1]*b->scale_y,node->bounds[2]*b->scale_x,node->bounds[3]*b->scale_y}:
                  (struct UiaRect){origin.x,origin.y,client.right-client.left,client.bottom-client.top};
 SDL_UnlockMutex(b->mutex);return valid?S_OK:UIA_E_ELEMENTNOTAVAILABLE;
}
/* Caller holds the model mutex. Include native window state and screen origin;
 * moving the host changes geometry even when the UI model did not reflow. */
static bool uia_link_state_locked(uia_element *e,uia_link_state *out) {
 ps_a11y_native *b=e->bridge;const ps_a11y_node *node=b->model?ps_a11y_find(b->model,e->id):NULL;
 POINT origin={0};if(!node || node->role!=PS_A11Y_LINK || !b->hwnd || !ClientToScreen(b->hwnd,&origin))return false;
 if(!uia_window_viewable_locked(b)){*out=(uia_link_state){{0},false,true};return true;}
 *out=(uia_link_state){{origin.x+node->bounds[0]*b->scale_x,origin.y+node->bounds[1]*b->scale_y,
      node->bounds[2]*b->scale_x,node->bounds[3]*b->scale_y},
      node->enabled && IsWindowEnabled(b->hwnd),node->offscreen || !IsWindowVisible(b->hwnd) || IsIconic(b->hwnd)};
 return true;
}
static HRESULT uia_rectangle_value(const double bounds[4],VARIANT *out) {
 VariantInit(out);SAFEARRAY *array=SafeArrayCreateVector(VT_R8,0,4);if(!array)return E_OUTOFMEMORY;
 for(LONG i=0;i<4;i++)if(FAILED(SafeArrayPutElement(array,&i,(void*)&bounds[i]))){SafeArrayDestroy(array);return E_OUTOFMEMORY;}
 out->vt=VT_ARRAY|VT_R8;out->parray=array;return S_OK;
}
static void uia_link_changed(uia_element *e,const uia_link_state *old,const uia_link_state *next) {
 if(memcmp(old->bounds,next->bounds,sizeof old->bounds)) {
  VARIANT before,after;VariantInit(&before);VariantInit(&after);
  if(SUCCEEDED(uia_rectangle_value(old->bounds,&before)) && SUCCEEDED(uia_rectangle_value(next->bounds,&after)))
   UiaRaiseAutomationPropertyChangedEvent(&e->simple,UIA_BoundingRectanglePropertyId,before,after);
  VariantClear(&before);VariantClear(&after);
 }
 if(old->enabled!=next->enabled){VARIANT before={0},after={0};before.vt=after.vt=VT_BOOL;
  before.boolVal=old->enabled?VARIANT_TRUE:VARIANT_FALSE;after.boolVal=next->enabled?VARIANT_TRUE:VARIANT_FALSE;
  UiaRaiseAutomationPropertyChangedEvent(&e->simple,UIA_IsEnabledPropertyId,before,after);}
 if(old->offscreen!=next->offscreen){VARIANT before={0},after={0};before.vt=after.vt=VT_BOOL;
  before.boolVal=old->offscreen?VARIANT_TRUE:VARIANT_FALSE;after.boolVal=next->offscreen?VARIANT_TRUE:VARIANT_FALSE;
  UiaRaiseAutomationPropertyChangedEvent(&e->simple,UIA_IsOffscreenPropertyId,before,after);}
}
static HRESULT STDMETHODCALLTYPE uia_options(IRawElementProviderSimple *p,enum ProviderOptions *out) {
 (void)p;if(!out)return E_POINTER;*out=ProviderOptions_ServerSideProvider|ProviderOptions_UseComThreading;return S_OK;
}
static HRESULT STDMETHODCALLTYPE uia_pattern(IRawElementProviderSimple *p,PATTERNID pattern,IUnknown **out) {
 if(!out)return E_POINTER;*out=NULL;uia_element *e=UIA_OWNER(IRawElementProviderSimple,simple,p);
 ps_a11y_node node={0};if(e->id && !uia_node(e,&node))return UIA_E_ELEMENTNOTAVAILABLE;
 if(!e->id && pattern==UIA_TextPatternId) {
  ps_a11y_native *b=e->bridge;SDL_LockMutex(b->mutex);
  bool reading=b->model && b->model->document && b->model->document->active;
  SDL_UnlockMutex(b->mutex);return reading?uia_query(e,&IID_ITextProvider,(void**)out):S_OK;
 }
 if(e->id && node.role==PS_A11Y_EDIT && pattern==UIA_TextPatternId)return uia_query(e,&IID_ITextProvider,(void**)out);
 if(e->id && node.role==PS_A11Y_EDIT && pattern==UIA_TextPattern2Id)return uia_query(e,&IID_ITextProvider2,(void**)out);
 REFIID iid=pattern==UIA_InvokePatternId?&IID_IInvokeProvider:pattern==UIA_TogglePatternId?&IID_IToggleProvider:
  pattern==UIA_SelectionItemPatternId?&IID_ISelectionItemProvider:pattern==UIA_SelectionPatternId?&IID_ISelectionProvider:
  pattern==UIA_RangeValuePatternId?&IID_IRangeValueProvider:pattern==UIA_ValuePatternId?&IID_IValueProvider:NULL;
 if(!iid)return S_OK;HRESULT result=uia_query(e,iid,(void**)out);return result==E_NOINTERFACE?S_OK:result;
}
static HRESULT STDMETHODCALLTYPE uia_property(IRawElementProviderSimple *p,PROPERTYID property,VARIANT *out) {
 if(!out)return E_POINTER;VariantInit(out);uia_element *e=UIA_OWNER(IRawElementProviderSimple,simple,p);ps_a11y_native *b=e->bridge;
 ps_a11y_node node={0};SDL_LockMutex(b->mutex);
 const ps_a11y_node *current=b->model && e->id?ps_a11y_find(b->model,e->id):NULL;
 bool live=b->model && (!e->id || current);HWND hwnd=b->hwnd;if(current)node=*current;
 bool reading=b->model && b->model->document && b->model->document->active;
 bool viewable=uia_window_viewable_locked(b);
 SDL_UnlockMutex(b->mutex);if(!live)return UIA_E_ELEMENTNOTAVAILABLE;
 /* SDK property IDs are extern const values, not C integer constants. */
 if(property==UIA_NamePropertyId)return uia_string(e->id?node.label:b->title,out);
 if(property==UIA_AutomationIdPropertyId) {
  char id[96];snprintf(id,sizeof id,"physim-%llu-%llu",(unsigned long long)(uintptr_t)hwnd,(unsigned long long)e->id);
  return uia_string(id,out);
 }
 if(property==UIA_FrameworkIdPropertyId)return uia_string("Physim",out);
 if(property==UIA_ControlTypePropertyId) {
  out->vt=VT_I4;out->lVal=!e->id?(reading?UIA_DocumentControlTypeId:UIA_PaneControlTypeId):node.role==PS_A11Y_BUTTON?UIA_ButtonControlTypeId:
   node.role==PS_A11Y_LINK?UIA_HyperlinkControlTypeId:node.role==PS_A11Y_CHECKBOX?UIA_CheckBoxControlTypeId:node.role==PS_A11Y_RADIO?UIA_RadioButtonControlTypeId:
   node.role==PS_A11Y_RADIO_GROUP?UIA_GroupControlTypeId:node.role==PS_A11Y_SEPARATOR?UIA_SeparatorControlTypeId:node.role==PS_A11Y_SLIDER?UIA_SliderControlTypeId:node.role==PS_A11Y_EDIT?UIA_EditControlTypeId:UIA_TextControlTypeId;
 } else if(property==UIA_OrientationPropertyId && node.role==PS_A11Y_SEPARATOR) {
  out->vt=VT_I4;out->lVal=node.bounds[2]<node.bounds[3]?OrientationType_Vertical:OrientationType_Horizontal;
 } else if(property==UIA_IsEnabledPropertyId){out->vt=VT_BOOL;out->boolVal=viewable && (!e->id || node.enabled) && IsWindowEnabled(hwnd)?VARIANT_TRUE:VARIANT_FALSE;}
 else if(property==UIA_IsKeyboardFocusablePropertyId){out->vt=VT_BOOL;out->boolVal=viewable && (e->id?node.focusable:(b->model->document && b->model->document->active))?VARIANT_TRUE:VARIANT_FALSE;}
 else if(property==UIA_HasKeyboardFocusPropertyId){out->vt=VT_BOOL;out->boolVal=viewable && (e->id?node.focused:ps_a11y_document_focused(b->model))?VARIANT_TRUE:VARIANT_FALSE;}
 else if(property==UIA_IsControlElementPropertyId || property==UIA_IsContentElementPropertyId){out->vt=VT_BOOL;out->boolVal=VARIANT_TRUE;}
 else if(property==UIA_IsOffscreenPropertyId){out->vt=VT_BOOL;out->boolVal=viewable && !node.offscreen?VARIANT_FALSE:VARIANT_TRUE;}
 return S_OK;
}
static HRESULT STDMETHODCALLTYPE uia_host(IRawElementProviderSimple *p,IRawElementProviderSimple **out) {
 if(!out)return E_POINTER;*out=NULL;uia_element *e=UIA_OWNER(IRawElementProviderSimple,simple,p);
 if(e->id)return S_OK;ps_a11y_native *b=e->bridge;SDL_LockMutex(b->mutex);HWND hwnd=b->model?b->hwnd:NULL;SDL_UnlockMutex(b->mutex);
 return hwnd?UiaHostProviderFromHwnd(hwnd,out):UIA_E_ELEMENTNOTAVAILABLE;
}
static HRESULT STDMETHODCALLTYPE uia_navigate(IRawElementProviderFragment *p,enum NavigateDirection direction,IRawElementProviderFragment **out) {
 if(!out)return E_POINTER;*out=NULL;uia_element *e=UIA_OWNER(IRawElementProviderFragment,fragment,p);ps_a11y_native *b=e->bridge;
 SDL_LockMutex(b->mutex);const ps_a11y_node *node=b->model && e->id?ps_a11y_find(b->model,e->id):NULL;
 if(!b->model || (e->id && !node)){SDL_UnlockMutex(b->mutex);return UIA_E_ELEMENTNOTAVAILABLE;}
 uint64_t id=0;bool found=false;
 if(direction==NavigateDirection_Parent && e->id)found=uia_semantic_parent_locked(b,e->id,&id);
 else if(direction==NavigateDirection_FirstChild || direction==NavigateDirection_LastChild) {
  size_t count=uia_semantic_count_locked(b,e->id);
  if(count)found=uia_semantic_child_locked(b,e->id,direction==NavigateDirection_FirstChild?0:count-1,&id);
 } else if(e->id && (direction==NavigateDirection_NextSibling || direction==NavigateDirection_PreviousSibling)) {
  uint64_t parent=0;uia_semantic_parent_locked(b,e->id,&parent);
  size_t count=uia_semantic_count_locked(b,parent),index=0;uint64_t current=0;
  for(;index<count;index++)if(uia_semantic_child_locked(b,parent,index,&current) && current==e->id)break;
  if(direction==NavigateDirection_NextSibling && index+1<count)found=uia_semantic_child_locked(b,parent,index+1,&id);
  else if(direction==NavigateDirection_PreviousSibling && index && index<count)found=uia_semantic_child_locked(b,parent,index-1,&id);
 }
 IRawElementProviderFragment *target=found?uia_any_fragment_locked(b,id):NULL;SDL_UnlockMutex(b->mutex);
 if(found && !target)return E_OUTOFMEMORY;*out=target;return S_OK;
}
static HRESULT STDMETHODCALLTYPE uia_runtime(IRawElementProviderFragment *p,SAFEARRAY **out) {
 if(!out)return E_POINTER;*out=NULL;uia_element *e=UIA_OWNER(IRawElementProviderFragment,fragment,p);
 if(!e->id)return S_OK;ps_a11y_node node;if(!uia_node(e,&node))return UIA_E_ELEMENTNOTAVAILABLE;
 SAFEARRAY *ids=SafeArrayCreateVector(VT_I4,0,3);if(!ids)return E_OUTOFMEMORY;
 LONG values[]={UiaAppendRuntimeId,(LONG)(e->id>>32),(LONG)(e->id&UINT32_MAX)};
 for(LONG i=0;i<3;i++)if(FAILED(SafeArrayPutElement(ids,&i,&values[i]))){SafeArrayDestroy(ids);return E_FAIL;}
 *out=ids;return S_OK;
}
static HRESULT STDMETHODCALLTYPE uia_rectangle(IRawElementProviderFragment *p,struct UiaRect *out) {return uia_bounds(UIA_OWNER(IRawElementProviderFragment,fragment,p),out);}
static HRESULT STDMETHODCALLTYPE uia_embedded(IRawElementProviderFragment *p,SAFEARRAY **out) {(void)p;if(!out)return E_POINTER;*out=NULL;return S_OK;}
static HRESULT STDMETHODCALLTYPE uia_set_focus(IRawElementProviderFragment *p) {
 uia_element *e=UIA_OWNER(IRawElementProviderFragment,fragment,p);ps_a11y_native *b=e->bridge;SDL_LockMutex(b->mutex);
 const ps_a11y_node *node=b->model?ps_a11y_find(b->model,e->id):NULL;
 if(!e->id){
  const ps_a11y_document *d=b->model?b->model->document:NULL;
  HRESULT result=!d || !d->active || !b->hwnd?UIA_E_ELEMENTNOTAVAILABLE:
   !uia_window_viewable_locked(b) || !IsWindowEnabled(b->hwnd)?UIA_E_ELEMENTNOTENABLED:
   ps_a11y_document_focus(b->model,d->version)?S_OK:UIA_E_INVALIDOPERATION;
  SDL_UnlockMutex(b->mutex);return result;
 }
 HRESULT result=!node || !b->hwnd?UIA_E_ELEMENTNOTAVAILABLE:!node->enabled || !uia_window_viewable_locked(b) || !IsWindowEnabled(b->hwnd)?UIA_E_ELEMENTNOTENABLED:
  ps_a11y_focus(b->model,e->id)?S_OK:UIA_E_INVALIDOPERATION;SDL_UnlockMutex(b->mutex);return result;
}
static HRESULT STDMETHODCALLTYPE uia_fragment_root(IRawElementProviderFragment *p,IRawElementProviderFragmentRoot **out) {
 if(!out)return E_POINTER;*out=NULL;uia_element *e=UIA_OWNER(IRawElementProviderFragment,fragment,p);ps_a11y_native *b=e->bridge;
 SDL_LockMutex(b->mutex);uia_element *root=b->model && (!e->id || ps_a11y_find(b->model,e->id))?uia_for_locked(b,0):NULL;SDL_UnlockMutex(b->mutex);
 if(!root)return UIA_E_ELEMENTNOTAVAILABLE;*out=&root->root;return S_OK;
}
static HRESULT STDMETHODCALLTYPE uia_point(IRawElementProviderFragmentRoot *p,double x,double y,IRawElementProviderFragment **out) {
 if(!out)return E_POINTER;*out=NULL;if(!isfinite(x) || !isfinite(y))return E_INVALIDARG;
 uia_element *e=UIA_OWNER(IRawElementProviderFragmentRoot,root,p);ps_a11y_native *b=e->bridge;
 struct UiaRect bounds;HRESULT result=uia_bounds(e,&bounds);if(FAILED(result))return result;
 if(x<bounds.left || y<bounds.top || x>=bounds.left+bounds.width || y>=bounds.top+bounds.height)return S_OK;
 SDL_LockMutex(b->mutex);if(!b->model){SDL_UnlockMutex(b->mutex);return UIA_E_ELEMENTNOTAVAILABLE;}
 uint64_t id=0;
 for(size_t i=b->model->count;i>0;i--) {
  const ps_a11y_node *n=&b->model->nodes[i-1];double nx=bounds.left+n->bounds[0]*b->scale_x,ny=bounds.top+n->bounds[1]*b->scale_y;
  if(x>=nx && y>=ny && x<nx+n->bounds[2]*b->scale_x && y<ny+n->bounds[3]*b->scale_y){id=n->id;break;}
 }
 if(!id) {
  ps_a11y_table_cell cell;
  if(ps_a11y_table_cell_at_point(b->model,(float)((x-bounds.left)/b->scale_x),(float)((y-bounds.top)/b->scale_y),&cell))id=cell.id;
 }
 IRawElementProviderFragment *target=uia_any_fragment_locked(b,id);SDL_UnlockMutex(b->mutex);if(!target)return E_OUTOFMEMORY;*out=target;return S_OK;
}
/* focused_id is the retained logical target. Native focus and announcements
 * must describe the currently focused, visible host instead. */
static uint64_t uia_focused_control_locked(ps_a11y_native *b) {
 if(!uia_window_viewable_locked(b) || !b->model->keyboard_focus)return 0;
 const ps_a11y_node *node=ps_a11y_find(b->model,b->model->focused_id);
 return node && node->focused?node->id:0;
}
static HRESULT STDMETHODCALLTYPE uia_focus(IRawElementProviderFragmentRoot *p,IRawElementProviderFragment **out) {
 if(!out)return E_POINTER;*out=NULL;uia_element *e=UIA_OWNER(IRawElementProviderFragmentRoot,root,p);ps_a11y_native *b=e->bridge;
 SDL_LockMutex(b->mutex);if(!b->model){SDL_UnlockMutex(b->mutex);return UIA_E_ELEMENTNOTAVAILABLE;}
 uint64_t control=uia_focused_control_locked(b);
 uia_element *target=uia_window_viewable_locked(b) && (control || ps_a11y_document_focused(b->model))?uia_for_locked(b,control):NULL;SDL_UnlockMutex(b->mutex);
 if(target)*out=&target->fragment;return S_OK;
}
static HRESULT uia_press(uia_element *e) {
 ps_a11y_native *b=e->bridge;SDL_LockMutex(b->mutex);const ps_a11y_node *node=b->model?ps_a11y_find(b->model,e->id):NULL;
 HRESULT result=!node || !b->hwnd?UIA_E_ELEMENTNOTAVAILABLE:!node->enabled || !uia_window_viewable_locked(b) || !IsWindowEnabled(b->hwnd)?UIA_E_ELEMENTNOTENABLED:
  ps_a11y_press(b->model,e->id)?S_OK:UIA_E_INVALIDOPERATION;SDL_UnlockMutex(b->mutex);return result;
}
static HRESULT STDMETHODCALLTYPE uia_do_invoke(IInvokeProvider *p){return uia_press(UIA_OWNER(IInvokeProvider,invoke,p));}
static HRESULT STDMETHODCALLTYPE uia_do_toggle(IToggleProvider *p){return uia_press(UIA_OWNER(IToggleProvider,toggle,p));}
static HRESULT STDMETHODCALLTYPE uia_toggle_state(IToggleProvider *p,enum ToggleState *out) {
 if(!out)return E_POINTER;ps_a11y_node node;if(!uia_node(UIA_OWNER(IToggleProvider,toggle,p),&node))return UIA_E_ELEMENTNOTAVAILABLE;
 *out=node.checked?ToggleState_On:ToggleState_Off;return S_OK;
}
static HRESULT STDMETHODCALLTYPE uia_select(ISelectionItemProvider *p) {
 uia_element *e=UIA_OWNER(ISelectionItemProvider,item,p);ps_a11y_node node;if(!uia_node(e,&node))return UIA_E_ELEMENTNOTAVAILABLE;
 if(!node.enabled)return UIA_E_ELEMENTNOTENABLED;return node.checked?S_OK:uia_press(e);
}
static HRESULT STDMETHODCALLTYPE uia_remove_selection(ISelectionItemProvider *p){ps_a11y_node node;return uia_node(UIA_OWNER(ISelectionItemProvider,item,p),&node)?UIA_E_INVALIDOPERATION:UIA_E_ELEMENTNOTAVAILABLE;}
static HRESULT STDMETHODCALLTYPE uia_selected(ISelectionItemProvider *p,BOOL *out) {
 if(!out)return E_POINTER;ps_a11y_node node;if(!uia_node(UIA_OWNER(ISelectionItemProvider,item,p),&node))return UIA_E_ELEMENTNOTAVAILABLE;
 *out=node.checked;return S_OK;
}
static HRESULT STDMETHODCALLTYPE uia_container(ISelectionItemProvider *p,IRawElementProviderSimple **out) {
 if(!out)return E_POINTER;*out=NULL;uia_element *e=UIA_OWNER(ISelectionItemProvider,item,p);ps_a11y_native *b=e->bridge;
 SDL_LockMutex(b->mutex);const ps_a11y_node *node=b->model?ps_a11y_find(b->model,e->id):NULL;
 uia_element *parent=node?uia_for_locked(b,node->parent):NULL;SDL_UnlockMutex(b->mutex);if(!parent)return UIA_E_ELEMENTNOTAVAILABLE;
 *out=&parent->simple;return S_OK;
}
static HRESULT STDMETHODCALLTYPE uia_selection_array(ISelectionProvider *p,SAFEARRAY **out) {
 if(!out)return E_POINTER;*out=NULL;uia_element *e=UIA_OWNER(ISelectionProvider,selection,p);ps_a11y_native *b=e->bridge;
 SDL_LockMutex(b->mutex);if(!b->model || !ps_a11y_find(b->model,e->id)){SDL_UnlockMutex(b->mutex);return UIA_E_ELEMENTNOTAVAILABLE;}
 uint64_t id=0;for(size_t i=0;i<b->model->count;i++)if(b->model->nodes[i].parent==e->id && b->model->nodes[i].checked){id=b->model->nodes[i].id;break;}
 uia_element *selected=id?uia_for_locked(b,id):NULL;SDL_UnlockMutex(b->mutex);
 SAFEARRAY *array=SafeArrayCreateVector(VT_UNKNOWN,0,id?1:0);if(!array){if(selected)uia_release(selected);return E_OUTOFMEMORY;}
 if(id && !selected){SafeArrayDestroy(array);return E_OUTOFMEMORY;}
 if(selected){LONG index=0;HRESULT result=SafeArrayPutElement(array,&index,&selected->simple);uia_release(selected);if(FAILED(result)){SafeArrayDestroy(array);return result;}}
 *out=array;return S_OK;
}
static HRESULT STDMETHODCALLTYPE uia_multiple(ISelectionProvider *p,BOOL *out){if(!out)return E_POINTER;ps_a11y_node node;if(!uia_node(UIA_OWNER(ISelectionProvider,selection,p),&node))return UIA_E_ELEMENTNOTAVAILABLE;*out=FALSE;return S_OK;}
static HRESULT STDMETHODCALLTYPE uia_required(ISelectionProvider *p,BOOL *out){if(!out)return E_POINTER;ps_a11y_node node;if(!uia_node(UIA_OWNER(ISelectionProvider,selection,p),&node))return UIA_E_ELEMENTNOTAVAILABLE;*out=TRUE;return S_OK;}
static HRESULT STDMETHODCALLTYPE uia_set_value(IRangeValueProvider *p,double value) {
 uia_element *e=UIA_OWNER(IRangeValueProvider,range,p);ps_a11y_native *b=e->bridge;SDL_LockMutex(b->mutex);
 const ps_a11y_node *node=b->model?ps_a11y_find(b->model,e->id):NULL;
 HRESULT result=!node || !b->hwnd?UIA_E_ELEMENTNOTAVAILABLE:!node->enabled || !uia_window_viewable_locked(b) || !IsWindowEnabled(b->hwnd)?UIA_E_ELEMENTNOTENABLED:
  !isfinite(value) || value<node->minimum || value>node->maximum?E_INVALIDARG:
  ps_a11y_set_value(b->model,e->id,value)?S_OK:UIA_E_INVALIDOPERATION;SDL_UnlockMutex(b->mutex);return result;
}
#define UIA_RANGE_GET(name,field) static HRESULT STDMETHODCALLTYPE name(IRangeValueProvider *p,double *out){ \
 if(!out)return E_POINTER;ps_a11y_node node;if(!uia_node(UIA_OWNER(IRangeValueProvider,range,p),&node))return UIA_E_ELEMENTNOTAVAILABLE;*out=node.field;return S_OK;}
UIA_RANGE_GET(uia_value,value)
UIA_RANGE_GET(uia_minimum,minimum)
UIA_RANGE_GET(uia_maximum,maximum)
UIA_RANGE_GET(uia_change,increment)
static HRESULT STDMETHODCALLTYPE uia_readonly(IRangeValueProvider *p,BOOL *out) {
 if(!out)return E_POINTER;ps_a11y_node node;if(!uia_node(UIA_OWNER(IRangeValueProvider,range,p),&node))return UIA_E_ELEMENTNOTAVAILABLE;*out=!node.enabled;return S_OK;
}
static const IRawElementProviderSimpleVtbl uia_simple_vtable={uia_simple_query,uia_simple_addref,uia_simple_release,uia_options,uia_pattern,uia_property,uia_host};
static const IRawElementProviderFragmentVtbl uia_fragment_vtable={uia_fragment_query,uia_fragment_addref,uia_fragment_release,uia_navigate,uia_runtime,uia_rectangle,uia_embedded,uia_set_focus,uia_fragment_root};
static const IRawElementProviderFragmentRootVtbl uia_root_vtable={uia_root_query,uia_root_addref,uia_root_release,uia_point,uia_focus};
static const IInvokeProviderVtbl uia_invoke_vtable={uia_invoke_query,uia_invoke_addref,uia_invoke_release,uia_do_invoke};
static const IToggleProviderVtbl uia_toggle_vtable={uia_toggle_query,uia_toggle_addref,uia_toggle_release,uia_do_toggle,uia_toggle_state};
static const ISelectionItemProviderVtbl uia_item_vtable={uia_item_query,uia_item_addref,uia_item_release,uia_select,uia_select,uia_remove_selection,uia_selected,uia_container};
static const ISelectionProviderVtbl uia_selection_vtable={uia_selection_query,uia_selection_addref,uia_selection_release,uia_selection_array,uia_multiple,uia_required};
static const IRangeValueProviderVtbl uia_range_vtable={uia_range_query,uia_range_addref,uia_range_release,uia_set_value,uia_value,uia_readonly,uia_maximum,uia_minimum,uia_change,uia_change};
static HRESULT STDMETHODCALLTYPE uia_edit_set(IValueProvider *p,LPCWSTR value) {
 if(!value)return E_INVALIDARG;size_t units=wcsnlen(value,PS_A11Y_LABEL_BYTES);if(units>=PS_A11Y_LABEL_BYTES)return E_INVALIDARG;
 char text[PS_A11Y_LABEL_BYTES];int bytes=WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,value,(int)units,text,sizeof text-1,NULL,NULL);
 if(units && !bytes)return E_INVALIDARG;text[bytes]=0;
 uia_element *e=UIA_OWNER(IValueProvider,value,p);ps_a11y_native *b=e->bridge;SDL_LockMutex(b->mutex);
 const ps_a11y_node *node=b->model?ps_a11y_find(b->model,e->id):NULL;
 HRESULT result=!node || !b->hwnd?UIA_E_ELEMENTNOTAVAILABLE:!node->enabled || !uia_window_viewable_locked(b) || !IsWindowEnabled(b->hwnd)?UIA_E_ELEMENTNOTENABLED:
  !ps_text_valid(text,node->text_capacity,false)?E_INVALIDARG:ps_a11y_set_text(b->model,e->id,text)?S_OK:UIA_E_INVALIDOPERATION;
 SDL_UnlockMutex(b->mutex);return result;
}
static HRESULT STDMETHODCALLTYPE uia_edit_value(IValueProvider *p,BSTR *out) {
 if(!out)return E_POINTER;*out=NULL;ps_a11y_node node;if(!uia_node(UIA_OWNER(IValueProvider,value,p),&node))return UIA_E_ELEMENTNOTAVAILABLE;
 VARIANT text;VariantInit(&text);HRESULT result=uia_string(node.text_value,&text);if(SUCCEEDED(result))*out=text.bstrVal;return result;
}
static HRESULT STDMETHODCALLTYPE uia_edit_readonly(IValueProvider *p,BOOL *out) {
 if(!out)return E_POINTER;ps_a11y_node node;if(!uia_node(UIA_OWNER(IValueProvider,value,p),&node))return UIA_E_ELEMENTNOTAVAILABLE;*out=!node.enabled;return S_OK;
}
static const IValueProviderVtbl uia_edit_vtable={uia_edit_query,uia_edit_addref,uia_edit_release,uia_edit_set,uia_edit_value,uia_edit_readonly};
#include "accessibility_windows_text.inc"
static uia_element *uia_new(ps_a11y_native *b,uint64_t id,ps_a11y_role role) {
 uia_element *e=calloc(1,sizeof *e);if(!e)return NULL;
 e->simple.lpVtbl=&uia_simple_vtable;e->fragment.lpVtbl=&uia_fragment_vtable;e->root.lpVtbl=&uia_root_vtable;
 e->invoke.lpVtbl=&uia_invoke_vtable;e->toggle.lpVtbl=&uia_toggle_vtable;e->item.lpVtbl=&uia_item_vtable;
 e->selection.lpVtbl=&uia_selection_vtable;e->range.lpVtbl=&uia_range_vtable;
 e->text.lpVtbl=&uia_text_vtable;
 e->text2.lpVtbl=&uia_text2_vtable;
 e->value.lpVtbl=&uia_edit_vtable;
 if(role==PS_A11Y_EDIT){const ps_a11y_node *node=ps_a11y_find(b->model,id);if(node){strcpy(e->previous_text,node->text_value);e->previous_start=node->text_start;e->previous_end=node->text_end;e->previous_caret=node->text_caret;}}
 e->references=1;e->bridge=b;e->id=id;e->role=role;InterlockedIncrement(&b->references);
 if(role==PS_A11Y_LINK)e->has_link_state=uia_link_state_locked(e,&e->previous_link);return e;
}
#include "accessibility_windows_tables.inc"
bool ps_a11y_native_document_link_invoked(ps_a11y_native *b,uint64_t version,size_t begin,size_t end) {
 if(!b || !version || begin>=end)return false;SDL_LockMutex(b->mutex);uint64_t id=0;
 for(size_t i=0;b->model && i<b->model->count;i++) {
  const ps_a11y_node *node=&b->model->nodes[i];
  if(node->role==PS_A11Y_LINK && node->enabled && node->document_version==version && node->document_begin==begin && node->document_end==end){id=node->id;break;}
 }
 uia_element *e=id && b->hwnd && IsWindowEnabled(b->hwnd)?uia_for_locked(b,id):NULL;SDL_UnlockMutex(b->mutex);
 if(!e)return false;HRESULT result=UiaRaiseAutomationEvent(&e->simple,UIA_Invoke_InvokedEventId);uia_release(e);return SUCCEEDED(result);
}
static LRESULT CALLBACK uia_window_proc(HWND hwnd,UINT message,WPARAM wparam,LPARAM lparam,UINT_PTR subclass,DWORD_PTR data) {
 ps_a11y_native *b=(ps_a11y_native*)data;
 if(message==WM_GETOBJECT && (LONG)lparam==UiaRootObjectId)return UiaReturnRawElementProvider(hwnd,wparam,lparam,&b->root->simple);
 if(message==WM_NCDESTROY) {
  RemoveWindowSubclass(hwnd,uia_window_proc,subclass);SDL_LockMutex(b->mutex);b->hwnd=NULL;SDL_UnlockMutex(b->mutex);
 }
 return DefSubclassProc(hwnd,message,wparam,lparam);
}
ps_a11y_native *ps_a11y_native_create(SDL_Window *window,ps_a11y_model *model,SDL_Mutex *mutex) {
 if(!window || !model || !mutex)return NULL;
 HWND hwnd=(HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(window),SDL_PROP_WINDOW_WIN32_HWND_POINTER,NULL);if(!hwnd)return NULL;
 HRESULT com=CoInitializeEx(NULL,COINIT_MULTITHREADED);if(FAILED(com) && com!=RPC_E_CHANGED_MODE)return NULL;
 ps_a11y_native *b=calloc(1,sizeof *b);if(!b){if(SUCCEEDED(com))CoUninitialize();return NULL;}
 b->references=1;b->window=window;b->hwnd=hwnd;b->model=model;b->mutex=mutex;b->com_initialized=SUCCEEDED(com);b->scale_x=b->scale_y=1;
 snprintf(b->title,sizeof b->title,"%s",SDL_GetWindowTitle(window));b->root=uia_new(b,0,PS_A11Y_TEXT);
 if(!b->root || !SetWindowSubclass(hwnd,uia_window_proc,(UINT_PTR)b,(DWORD_PTR)b)) {
  if(b->root)uia_release(b->root);if(b->com_initialized)CoUninitialize();free(b);return NULL;
 }
 return b;
}
void ps_a11y_native_publish(ps_a11y_native *b,bool changed) {
 if(!b)return;uia_element *retired[PS_A11Y_MAX_NODES];size_t count=0;
 uia_element *edits[PS_A11Y_MAX_NODES];VARIANT old_text[PS_A11Y_MAX_NODES],new_text[PS_A11Y_MAX_NODES];size_t edit_count=0;
 uia_element *selections[PS_A11Y_MAX_NODES];size_t selection_count=0;
 uia_element *links[PS_A11Y_MAX_NODES];uia_link_state old_links[PS_A11Y_MAX_NODES],new_links[PS_A11Y_MAX_NODES];size_t link_count=0;
 int w=0,h=0;SDL_GetWindowSize(b->window,&w,&h);RECT client={0};if(b->hwnd)GetClientRect(b->hwnd,&client);
 SDL_LockMutex(b->mutex);
 if(w>0 && h>0){b->scale_x=(double)(client.right-client.left)/w;b->scale_y=(double)(client.bottom-client.top)/h;}
 uint64_t focused=uia_focused_control_locked(b),previous=b->focused_id;b->focused_id=focused;
 bool document_focused=ps_a11y_document_focused(b->model),previous_document_focus=b->document_focused;b->document_focused=document_focused;
 uint64_t reading=b->model && b->model->document && b->model->document->active?b->model->document->version:0;
 bool reading_changed=reading!=b->announced_reading_version;b->announced_reading_version=reading;
 for(size_t i=0;i<PS_A11Y_MAX_NODES;i++)if(b->elements[i] && (!b->model || !ps_a11y_find(b->model,b->elements[i]->id))) {
  retired[count++]=b->elements[i];b->elements[i]=NULL;
 }
 for(size_t i=0;i<PS_A11Y_MAX_NODES;i++)if(b->elements[i] && b->elements[i]->role==PS_A11Y_EDIT) {
  uia_element *edit=b->elements[i];const ps_a11y_node *node=ps_a11y_find(b->model,edit->id);
  if(node && (edit->previous_start!=node->text_start || edit->previous_end!=node->text_end || edit->previous_caret!=node->text_caret)) {
   uia_addref(edit);selections[selection_count++]=edit;edit->previous_start=node->text_start;edit->previous_end=node->text_end;edit->previous_caret=node->text_caret;
  }
  if(node && strcmp(edit->previous_text,node->text_value)) {
   VariantInit(&old_text[edit_count]);VariantInit(&new_text[edit_count]);
   if(SUCCEEDED(uia_string(edit->previous_text,&old_text[edit_count])) && SUCCEEDED(uia_string(node->text_value,&new_text[edit_count]))) {
    uia_addref(edit);edits[edit_count++]=edit;strcpy(edit->previous_text,node->text_value);
   } else {VariantClear(&old_text[edit_count]);VariantClear(&new_text[edit_count]);}
  }
 }
 for(size_t i=0;i<PS_A11Y_MAX_NODES;i++)if(b->elements[i] && b->elements[i]->role==PS_A11Y_LINK) {
  uia_element *link=b->elements[i];uia_link_state next;
  if(uia_link_state_locked(link,&next)) {
   if(link->has_link_state && (memcmp(link->previous_link.bounds,next.bounds,sizeof next.bounds) ||
      link->previous_link.enabled!=next.enabled || link->previous_link.offscreen!=next.offscreen)) {
    uia_addref(link);links[link_count]=link;old_links[link_count]=link->previous_link;new_links[link_count++]=next;
   }
   link->previous_link=next;link->has_link_state=true;
  }
 }
 uia_element *focus=(focused && focused!=previous) || (document_focused && !previous_document_focus)?uia_for_locked(b,focused):NULL;SDL_UnlockMutex(b->mutex);
 for(size_t i=0;i<count;i++){UiaDisconnectProvider(&retired[i]->simple);uia_release(retired[i]);}
 if(changed)UiaRaiseStructureChangedEvent(&b->root->simple,StructureChangeType_ChildrenInvalidated,NULL,0);
 if(reading && reading_changed)UiaRaiseAutomationEvent(&b->root->simple,UIA_Text_TextChangedEventId);
 if(focus){UiaRaiseAutomationEvent(&focus->simple,UIA_AutomationFocusChangedEventId);uia_release(focus);}
 for(size_t i=0;i<edit_count;i++){
  UiaRaiseAutomationPropertyChangedEvent(&edits[i]->simple,UIA_ValueValuePropertyId,old_text[i],new_text[i]);
  UiaRaiseAutomationEvent(&edits[i]->simple,UIA_Text_TextChangedEventId);
  VariantClear(&old_text[i]);VariantClear(&new_text[i]);uia_release(edits[i]);
 }
 for(size_t i=0;i<selection_count;i++){UiaRaiseAutomationEvent(&selections[i]->simple,UIA_Text_TextSelectionChangedEventId);uia_release(selections[i]);}
 for(size_t i=0;i<link_count;i++){uia_link_changed(links[i],&old_links[i],&new_links[i]);uia_release(links[i]);}
 uia_tables_publish(b);
}
void ps_a11y_native_destroy(ps_a11y_native *b) {
 if(!b)return;if(b->hwnd)RemoveWindowSubclass(b->hwnd,uia_window_proc,(UINT_PTR)b);
 SDL_LockMutex(b->mutex);b->model=NULL;b->hwnd=NULL;SDL_UnlockMutex(b->mutex);
 uia_tables_destroy(b);
 for(size_t i=0;i<PS_A11Y_MAX_NODES;i++)if(b->elements[i]){UiaDisconnectProvider(&b->elements[i]->simple);uia_release(b->elements[i]);}
 UiaDisconnectProvider(&b->root->simple);uia_release(b->root);
 if(b->com_initialized)CoUninitialize();uia_bridge_release(b);
}
static uia_element *uia_named(ps_a11y_native *b,const char *group,const char *label) {
 if(!b || !label)return NULL;SDL_LockMutex(b->mutex);uint64_t id=0;
 for(size_t i=0;b->model && i<b->model->count;i++) {
  const ps_a11y_node *n=&b->model->nodes[i],*parent=ps_a11y_find(b->model,n->parent);
  if(!strcmp(n->label,label) && (!group || (parent && !strcmp(parent->label,group)))){id=n->id;break;}
 }
 uia_element *e=id?uia_for_locked(b,id):NULL;SDL_UnlockMutex(b->mutex);return e;
}
bool ps_a11y_native_press_label(ps_a11y_native *b,const char *label) {
 uia_element *e=uia_named(b,NULL,label);if(!e)return false;
 HRESULT result=e->role==PS_A11Y_BUTTON || e->role==PS_A11Y_LINK?IInvokeProvider_Invoke(&e->invoke):e->role==PS_A11Y_CHECKBOX?IToggleProvider_Toggle(&e->toggle):UIA_E_INVALIDOPERATION;
 uia_release(e);return SUCCEEDED(result);
}
bool ps_a11y_native_press_choice(ps_a11y_native *b,const char *group,const char *label) {
 uia_element *e=uia_named(b,group,label);if(!e)return false;
 HRESULT result=e->role==PS_A11Y_RADIO?ISelectionItemProvider_Select(&e->item):UIA_E_INVALIDOPERATION;uia_release(e);return SUCCEEDED(result);
}
bool ps_a11y_native_focus_label(ps_a11y_native *b,const char *group,const char *label) {
 uia_element *e=uia_named(b,group,label);if(!e)return false;HRESULT result=IRawElementProviderFragment_SetFocus(&e->fragment);uia_release(e);return SUCCEEDED(result);
}
/* Kept beside the bridge so tests exercise COM interfaces and retained providers. */
#include "accessibility_windows_tables_tests.inc"
#include "accessibility_windows_tests.inc"
