/* Pure client-side UIA event sinks. Cached identity remains usable after a link
 * leaves the tree; callbacks must never infer identity from a translated name. */
#include <stddef.h>
#include <math.h>
typedef struct {
 IUIAutomationPropertyChangedEventHandler properties;
 IUIAutomationEventHandler automation;
 LONG references,bounds,offscreen,disabled,enabled,invoked,bad;BSTR id;
} link_events;
#define EVENTS_OWNER(member,p) ((link_events*)((char*)(p)-offsetof(link_events,member)))
static ULONG events_addref(link_events *e){return (ULONG)InterlockedIncrement(&e->references);}
static ULONG events_release(link_events *e){LONG n=InterlockedDecrement(&e->references);if(!n){SysFreeString(e->id);free(e);}return (ULONG)n;}
static HRESULT events_query(link_events *e,REFIID iid,void **out) {
 if(!out)return E_POINTER;*out=NULL;
 if(IsEqualIID(iid,&IID_IUnknown) || IsEqualIID(iid,&IID_IUIAutomationPropertyChangedEventHandler))*out=&e->properties;
 else if(IsEqualIID(iid,&IID_IUIAutomationEventHandler))*out=&e->automation;
 if(!*out)return E_NOINTERFACE;events_addref(e);return S_OK;
}
static HRESULT STDMETHODCALLTYPE events_property_query(IUIAutomationPropertyChangedEventHandler *p,REFIID iid,void **out){return events_query(EVENTS_OWNER(properties,p),iid,out);}
static ULONG STDMETHODCALLTYPE events_property_addref(IUIAutomationPropertyChangedEventHandler *p){return events_addref(EVENTS_OWNER(properties,p));}
static ULONG STDMETHODCALLTYPE events_property_release(IUIAutomationPropertyChangedEventHandler *p){return events_release(EVENTS_OWNER(properties,p));}
static HRESULT STDMETHODCALLTYPE events_automation_query(IUIAutomationEventHandler *p,REFIID iid,void **out){return events_query(EVENTS_OWNER(automation,p),iid,out);}
static ULONG STDMETHODCALLTYPE events_automation_addref(IUIAutomationEventHandler *p){return events_addref(EVENTS_OWNER(automation,p));}
static ULONG STDMETHODCALLTYPE events_automation_release(IUIAutomationEventHandler *p){return events_release(EVENTS_OWNER(automation,p));}
static bool events_identity(link_events *e,IUIAutomationElement *sender) {
 BSTR id=NULL;bool ok=SUCCEEDED(IUIAutomationElement_get_CachedAutomationId(sender,&id)) && id && !wcscmp(id,e->id);
 SysFreeString(id);if(!ok)InterlockedIncrement(&e->bad);return ok;
}
static HRESULT STDMETHODCALLTYPE events_property_changed(IUIAutomationPropertyChangedEventHandler *p,IUIAutomationElement *sender,PROPERTYID property,VARIANT value) {
 link_events *e=EVENTS_OWNER(properties,p);if(!events_identity(e,sender))return S_OK;
 bool ok=false;
 if(property==UIA_BoundingRectanglePropertyId && value.vt==(VT_ARRAY|VT_R8) && value.parray) {
  LONG low=-1,high=-1;ok=SafeArrayGetDim(value.parray)==1 && SUCCEEDED(SafeArrayGetLBound(value.parray,1,&low)) &&
      SUCCEEDED(SafeArrayGetUBound(value.parray,1,&high)) && low==0 && high==3;
  for(LONG i=0;ok && i<4;i++){double number=0;ok=SUCCEEDED(SafeArrayGetElement(value.parray,&i,&number)) && isfinite(number) && (i<2 || number>=0);}
  if(ok)InterlockedIncrement(&e->bounds);
 } else if(property==UIA_IsOffscreenPropertyId && value.vt==VT_BOOL) {ok=value.boolVal==VARIANT_FALSE; if(ok)InterlockedIncrement(&e->offscreen);}
 else if(property==UIA_IsEnabledPropertyId && value.vt==VT_BOOL) {ok=true;InterlockedIncrement(value.boolVal?&e->enabled:&e->disabled);}
 if(!ok)InterlockedIncrement(&e->bad);return S_OK;
}
static HRESULT STDMETHODCALLTYPE events_automation_raised(IUIAutomationEventHandler *p,IUIAutomationElement *sender,EVENTID event) {
 link_events *e=EVENTS_OWNER(automation,p);if(events_identity(e,sender) && event==UIA_Invoke_InvokedEventId)InterlockedIncrement(&e->invoked);
 else InterlockedIncrement(&e->bad);return S_OK;
}
static const IUIAutomationPropertyChangedEventHandlerVtbl events_property_vtable={events_property_query,events_property_addref,events_property_release,events_property_changed};
static const IUIAutomationEventHandlerVtbl events_automation_vtable={events_automation_query,events_automation_addref,events_automation_release,events_automation_raised};
static LONG events_count(LONG *value){return InterlockedCompareExchange(value,0,0);}
static HRESULT events_wait(LONG *value,LONG minimum) {
 ULONGLONG deadline=GetTickCount64()+10000;
 while(events_count(value)<minimum){if(GetTickCount64()>=deadline)return E_FAIL;Sleep(20);}return S_OK;
}
static HRESULT events_start(IUIAutomation *a,IUIAutomationElement *source,link_events **out) {
 *out=NULL;link_events *e=calloc(1,sizeof *e);if(!e)return E_OUTOFMEMORY;
 e->properties.lpVtbl=&events_property_vtable;e->automation.lpVtbl=&events_automation_vtable;e->references=1;
 IUIAutomationCacheRequest *cache=NULL;HRESULT result=IUIAutomationElement_get_CurrentAutomationId(source,&e->id);
 if(SUCCEEDED(result) && (!e->id || !SysStringLen(e->id)))result=E_FAIL;
 if(SUCCEEDED(result))result=IUIAutomation_CreateCacheRequest(a,&cache);
 if(SUCCEEDED(result))result=IUIAutomationCacheRequest_AddProperty(cache,UIA_AutomationIdPropertyId);
 PROPERTYID properties[]={UIA_BoundingRectanglePropertyId,UIA_IsOffscreenPropertyId,UIA_IsEnabledPropertyId};
 if(SUCCEEDED(result))result=IUIAutomation_AddPropertyChangedEventHandlerNativeArray(a,source,TreeScope_Element,cache,&e->properties,properties,3);
 if(SUCCEEDED(result))result=IUIAutomation_AddAutomationEventHandler(a,UIA_Invoke_InvokedEventId,source,TreeScope_Element,cache,&e->automation);
 if(cache)IUIAutomationCacheRequest_Release(cache);
 if(FAILED(result)){IUIAutomation_RemovePropertyChangedEventHandler(a,source,&e->properties);events_release(e);return result;}
 *out=e;return S_OK;
}
static void events_stop(IUIAutomation *a,IUIAutomationElement *source,link_events *e) {
 if(!e)return;IUIAutomation_RemovePropertyChangedEventHandler(a,source,&e->properties);
 IUIAutomation_RemoveAutomationEventHandler(a,UIA_Invoke_InvokedEventId,source,&e->automation);events_release(e);
}
