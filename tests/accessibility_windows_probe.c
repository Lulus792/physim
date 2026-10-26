/* Independent UI Automation client: no renderer, SDL, bridge or model linkage. */
#define COBJMACROS
#define CONST_VTABLE
#include <windows.h>
#include <uiautomation.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <wchar.h>
static HRESULT find(IUIAutomation *a,IUIAutomationElement *root,enum TreeScope scope,
                    PROPERTYID property,VARIANT value,IUIAutomationElement **out) {
 IUIAutomationCondition *condition=NULL;
 HRESULT result=IUIAutomation_CreatePropertyCondition(a,property,value,&condition);
 if(SUCCEEDED(result))result=IUIAutomationElement_FindFirst(root,scope,condition,out);
 if(condition)IUIAutomationCondition_Release(condition);return result;
}
static HRESULT named(IUIAutomation *a,IUIAutomationElement *root,const wchar_t *label,CONTROLTYPEID role,IUIAutomationElement **out) {
 VARIANT value;VariantInit(&value);value.vt=VT_BSTR;value.bstrVal=SysAllocString(label);
 if(!value.bstrVal)return E_OUTOFMEMORY;
 IUIAutomationCondition *name=NULL,*type=NULL,*both=NULL;
 HRESULT result=IUIAutomation_CreatePropertyCondition(a,UIA_NamePropertyId,value,&name);
 if(SUCCEEDED(result) && role) {
  VARIANT control_type;VariantInit(&control_type);control_type.vt=VT_I4;control_type.lVal=role;
  result=IUIAutomation_CreatePropertyCondition(a,UIA_ControlTypePropertyId,control_type,&type);
  if(SUCCEEDED(result))result=IUIAutomation_CreateAndCondition(a,name,type,&both);
 }
 if(SUCCEEDED(result))result=IUIAutomationElement_FindFirst(root,TreeScope_Descendants,both?both:name,out);
 if(both)IUIAutomationCondition_Release(both);if(type)IUIAutomationCondition_Release(type);if(name)IUIAutomationCondition_Release(name);
 VariantClear(&value);return result;
}
#include "accessibility_windows_events_probe.inc"
#include "accessibility_windows_link_tree.inc"
static HRESULT document_links(IUIAutomation *automation,IUIAutomationElement *control,const wchar_t *label,bool invoke,bool invalid,unsigned focus_cycles) {
 const char *phase="text and child ranges";
 IUnknown *unknown=NULL;IUIAutomationTextPattern *pattern=NULL;IUIAutomationTextRange *whole=NULL,*selected_range=NULL;
 IUIAutomationElementArray *children=NULL;IUIAutomationElement *selected=NULL,*ordered_links[2]={NULL,NULL};
 IUIAutomationGridPattern *table_grid=NULL;IUIAutomationElement *table_element=NULL;link_events *table_events=NULL;
 HRESULT result=IUIAutomationElement_GetCurrentPattern(control,UIA_TextPatternId,&unknown);
 if(SUCCEEDED(result) && unknown)result=IUnknown_QueryInterface(unknown,&IID_IUIAutomationTextPattern,(void**)&pattern);else result=E_FAIL;
 if(SUCCEEDED(result))result=IUIAutomationTextPattern_get_DocumentRange(pattern,&whole);
 int count=0;if(SUCCEEDED(result))result=collect_document_links(automation,pattern,whole,0,ordered_links,&count,&table_grid,&table_element);
 if(SUCCEEDED(result) && (!table_grid || !table_element)) {
  fprintf(stderr,"WINDOWS TABLE CLIENT: missing embedded Table/Grid\n");result=E_FAIL;
 }
 if(SUCCEEDED(result) && count!=2)result=E_FAIL;
 for(int i=0;SUCCEEDED(result) && i<count;i++) {
  IUIAutomationElement *child=NULL,*enclosing=NULL;IUIAutomationTextRange *range=NULL;IUIAutomationElementArray *nested=NULL;
  BSTR name=NULL,text=NULL;CONTROLTYPEID role=0;BOOL same=FALSE;
  child=ordered_links[i];IUIAutomationElement_AddRef(child);
  if(SUCCEEDED(result))result=IUIAutomationElement_get_CurrentName(child,&name);
  if(SUCCEEDED(result))result=IUIAutomationElement_get_CurrentControlType(child,&role);
  if(SUCCEEDED(result) && (role!=UIA_HyperlinkControlTypeId || wcscmp(name,i?L"C":L"C-Lernweg")))result=E_FAIL;
  if(SUCCEEDED(result))result=IUIAutomationTextPattern_RangeFromChild(pattern,child,&range);
  if(SUCCEEDED(result))result=IUIAutomationTextRange_GetText(range,-1,&text);
  if(SUCCEEDED(result) && wcscmp(text,name))result=E_FAIL;
  if(SUCCEEDED(result))result=IUIAutomationTextRange_GetEnclosingElement(range,&enclosing);
  if(SUCCEEDED(result))result=IUIAutomation_CompareElements(automation,child,enclosing,&same);
  if(SUCCEEDED(result) && !same)result=E_FAIL;
  if(SUCCEEDED(result))result=IUIAutomationTextRange_GetChildren(range,&nested);
  int descendants=-1;if(SUCCEEDED(result))result=IUIAutomationElementArray_get_Length(nested,&descendants);
  if(SUCCEEDED(result) && descendants)result=E_FAIL;
  if(SUCCEEDED(result) && !wcscmp(name,label)){selected=child;IUIAutomationElement_AddRef(selected);selected_range=range;IUIAutomationTextRange_AddRef(selected_range);}
  SysFreeString(name);SysFreeString(text);if(nested)IUIAutomationElementArray_Release(nested);
  if(enclosing)IUIAutomationElement_Release(enclosing);if(range)IUIAutomationTextRange_Release(range);if(child)IUIAutomationElement_Release(child);
 }
 if(SUCCEEDED(result) && !selected)result=E_FAIL;
 link_events *events=NULL;if(SUCCEEDED(result))result=events_start(automation,selected,&events);
 if(SUCCEEDED(result))result=table_element?events_start(automation,table_element,&table_events):E_FAIL;
 if(SUCCEEDED(result) && !invoke) {
  if(SUCCEEDED(result))phase="initial offscreen state";
  BOOL offscreen=FALSE;result=IUIAutomationElement_get_CurrentIsOffscreen(selected,&offscreen);
  if(SUCCEEDED(result) && !offscreen)result=E_FAIL;
  if(SUCCEEDED(result))result=IUIAutomationElement_SetFocus(selected);
  if(SUCCEEDED(result))phase="focus and visibility convergence";
  ULONGLONG deadline=GetTickCount64()+10000;BOOL focused=FALSE;
  while(SUCCEEDED(result)) {
   result=IUIAutomationElement_get_CurrentIsOffscreen(selected,&offscreen);
   if(SUCCEEDED(result))result=IUIAutomationElement_get_CurrentHasKeyboardFocus(selected,&focused);
   if(FAILED(result) || (!offscreen && focused))break;
   if(GetTickCount64()>=deadline){fprintf(stderr,"Link focus timeout: offscreen=%d focused=%d\n",offscreen,focused);result=E_FAIL;break;}Sleep(20);
  }
  /* The same marshalled text range survives scrolling; its glyphs become real. */
  if(SUCCEEDED(result))phase="visible glyph rectangles";
  SAFEARRAY *bounds=NULL;if(SUCCEEDED(result))result=IUIAutomationTextRange_GetBoundingRectangles(selected_range,&bounds);
  if(SUCCEEDED(result)){LONG high=-1;SafeArrayGetUBound(bounds,1,&high);if(high<3)result=E_FAIL;}
  if(bounds)SafeArrayDestroy(bounds);
  if(SUCCEEDED(result))phase="visible table cell hit testing";
  if(SUCCEEDED(result))result=validate_table_hit(automation,table_grid);
  if(SUCCEEDED(result))phase="visibility and geometry events";
  if(SUCCEEDED(result))result=events_wait(&events->offscreen,1);
  if(SUCCEEDED(result))result=events_wait(&events->bounds,1);
  if(SUCCEEDED(result) && events_count(&events->invoked))result=E_FAIL;
  UIA_HWND native=NULL;HWND host=NULL;
  if(SUCCEEDED(result))phase="host window handle";
  if(SUCCEEDED(result))result=IUIAutomationElement_get_CurrentNativeWindowHandle(control,&native);
  host=(HWND)native;if(SUCCEEDED(result) && !IsWindow(host)){fprintf(stderr,"Link host handle invalid: %p\n",(void*)host);result=E_FAIL;}
  for(unsigned cycle=0;SUCCEEDED(result) && cycle<focus_cycles;cycle++) {
   LONG disabled_before=events_count(&events->disabled),enabled_before=events_count(&events->enabled);
   if(SUCCEEDED(result)) {
    if(SUCCEEDED(result))phase="host disable and disabled event";
    EnableWindow(host,FALSE);BOOL enabled=TRUE;
    result=IUIAutomationElement_get_CurrentIsEnabled(selected,&enabled);
    if(SUCCEEDED(result) && enabled)result=E_FAIL;
    if(SUCCEEDED(result))result=events_wait(&events->disabled,disabled_before+1);
    IUnknown *action=NULL;IUIAutomationInvokePattern *press=NULL;
    if(SUCCEEDED(result))result=IUIAutomationElement_GetCurrentPattern(selected,UIA_InvokePatternId,&action);
    if(SUCCEEDED(result) && action)result=IUnknown_QueryInterface(action,&IID_IUIAutomationInvokePattern,(void**)&press);
    if(SUCCEEDED(result) && (!press || IUIAutomationInvokePattern_Invoke(press)!=UIA_E_ELEMENTNOTENABLED))result=E_FAIL;
    if(press)IUIAutomationInvokePattern_Release(press);if(action)IUnknown_Release(action);
    EnableWindow(host,TRUE); /* Always restore the host, including failure paths. */
    if(SUCCEEDED(result))result=events_wait(&events->enabled,enabled_before+1);
    if(SUCCEEDED(result))result=IUIAutomationElement_SetFocus(selected);
    deadline=GetTickCount64()+10000;focused=FALSE;
    while(SUCCEEDED(result) && !focused) {
     result=IUIAutomationElement_get_CurrentHasKeyboardFocus(selected,&focused);
     if(FAILED(result) || focused)break;if(GetTickCount64()>=deadline){result=E_FAIL;break;}Sleep(20);
    }
   }
   if(SUCCEEDED(result)) {
    if(SUCCEEDED(result))phase="window movement and quiet events";
    Sleep(200);LONG before=events_count(&events->bounds),table_before=events_count(&table_events->bounds);
    RECT host_bounds={0},old_rect={0},old_table_rect={0};
    if(!GetWindowRect(host,&host_bounds))result=E_FAIL;
    if(SUCCEEDED(result))result=IUIAutomationElement_get_CurrentBoundingRectangle(selected,&old_rect);
    if(SUCCEEDED(result))result=IUIAutomationElement_get_CurrentBoundingRectangle(table_element,&old_table_rect);
    if(SUCCEEDED(result) && !SetWindowPos(host,NULL,host_bounds.left+13,host_bounds.top+7,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE))result=E_FAIL;
    if(SUCCEEDED(result))result=events_wait(&events->bounds,before+1);
    if(SUCCEEDED(result))result=events_wait(&table_events->bounds,table_before+1);
    RECT moved={0};if(SUCCEEDED(result))result=IUIAutomationElement_get_CurrentBoundingRectangle(selected,&moved);
    if(SUCCEEDED(result) && (moved.left!=old_rect.left+13 || moved.top!=old_rect.top+7))result=E_FAIL;
    RECT moved_table={0};if(SUCCEEDED(result))result=IUIAutomationElement_get_CurrentBoundingRectangle(table_element,&moved_table);
    if(SUCCEEDED(result) && (moved_table.left!=old_table_rect.left+13 || moved_table.top!=old_table_rect.top+7))result=E_FAIL;
    Sleep(100);LONG stable_bounds=events_count(&events->bounds),stable_offscreen=events_count(&events->offscreen),stable_disabled=events_count(&events->disabled),stable_enabled=events_count(&events->enabled);
    LONG table_stable_bounds=events_count(&table_events->bounds),table_stable_offscreen=events_count(&table_events->offscreen),
        table_stable_disabled=events_count(&table_events->disabled),table_stable_enabled=events_count(&table_events->enabled);
    Sleep(200);
    if(SUCCEEDED(result) && (events_count(&events->bounds)!=stable_bounds || events_count(&events->offscreen)!=stable_offscreen ||
       events_count(&events->disabled)!=stable_disabled || events_count(&events->enabled)!=stable_enabled || events_count(&events->invoked)))result=E_FAIL;
    if(SUCCEEDED(result) && (events_count(&table_events->bounds)!=table_stable_bounds || events_count(&table_events->offscreen)!=table_stable_offscreen ||
       events_count(&table_events->disabled)!=table_stable_disabled || events_count(&table_events->enabled)!=table_stable_enabled || events_count(&table_events->invoked)))result=E_FAIL;
    if(SUCCEEDED(result))result=IUIAutomationElement_get_CurrentHasKeyboardFocus(selected,&focused);
    if(SUCCEEDED(result) && !focused){fprintf(stderr,"Link lost focus after window movement: foreground_host=%d enabled=%d visible=%d\n",GetForegroundWindow()==host,IsWindowEnabled(host),IsWindowVisible(host));result=E_FAIL;}
    if(SUCCEEDED(result))puts("WINDOWS LINK EVENTS: host disable/enable, rejected invocation, screen-position geometry and quiet idle passed");
   }
   if(focus_cycles>1)fprintf(stderr,"WINDOWS LINK FOCUS CYCLE: %u/%u result=0x%08lx\n",cycle+1,focus_cycles,(unsigned long)result);
  }
 } else if(SUCCEEDED(result)) {
  IUnknown *action=NULL;IUIAutomationInvokePattern *press=NULL;
  result=IUIAutomationElement_GetCurrentPattern(selected,UIA_InvokePatternId,&action);
  if(SUCCEEDED(result) && action)result=IUnknown_QueryInterface(action,&IID_IUIAutomationInvokePattern,(void**)&press);else result=E_FAIL;
  if(SUCCEEDED(result)) {
   if(getenv("PHYSIM_TEST_TRACE"))fprintf(stderr,"WINDOWS TABLE TRACE: invoke begin\n");
   result=IUIAutomationInvokePattern_Invoke(press);
   if(getenv("PHYSIM_TEST_TRACE"))fprintf(stderr,"WINDOWS TABLE TRACE: invoke returned 0x%08lx\n",(unsigned long)result);
  }
  ULONGLONG deadline=GetTickCount64()+10000;
  while(SUCCEEDED(result) && !invalid) {
   BSTR old=NULL;HRESULT stale=IUIAutomationTextRange_GetText(selected_range,-1,&old);SysFreeString(old);
   if(stale==UIA_E_ELEMENTNOTAVAILABLE)break;
   if(FAILED(stale) || GetTickCount64()>=deadline){result=E_FAIL;break;}Sleep(20);
  }
  if(press)IUIAutomationInvokePattern_Release(press);if(action)IUnknown_Release(action);
  if(SUCCEEDED(result) && invalid) {
   IUIAutomationElement *error=NULL;
   while(SUCCEEDED(result) && !error) {
    if(getenv("PHYSIM_TEST_TRACE"))fprintf(stderr,"WINDOWS TABLE TRACE: error lookup begin\n");
    result=named(automation,control,L"Dieses Linkziel ist nicht in der mitgelieferten Dokumentation enthalten.",UIA_TextControlTypeId,&error);
    if(getenv("PHYSIM_TEST_TRACE"))fprintf(stderr,"WINDOWS TABLE TRACE: error lookup returned 0x%08lx found=%d\n",(unsigned long)result,error!=NULL);
    if(FAILED(result) || error)break;if(GetTickCount64()>=deadline){result=E_FAIL;break;}Sleep(20);
   }
   if(error)IUIAutomationElement_Release(error);
   BSTR original=NULL;if(SUCCEEDED(result))result=IUIAutomationTextRange_GetText(selected_range,-1,&original);
   if(SUCCEEDED(result) && (!original || wcscmp(original,label)))result=E_FAIL;SysFreeString(original);
   Sleep(250);if(SUCCEEDED(result) && events_count(&events->invoked))result=E_FAIL;
  } else {
   if(SUCCEEDED(result))result=events_wait(&events->invoked,1);
   if(SUCCEEDED(result) && events_count(&events->invoked)!=1)result=E_FAIL;
  }
 }
 if(FAILED(result))fprintf(stderr,"WINDOWS LINK CHECK FAILED: %s (0x%08lx)\n",phase,(unsigned long)result);
 if(SUCCEEDED(result) && table_grid) {
  int rows=0;HRESULT state=IUIAutomationGridPattern_get_CurrentRowCount(table_grid,&rows);
  printf("WINDOWS TABLE LIFETIME: result=0x%08lx rows=%d expected_stale=%d\n",(unsigned long)state,rows,invoke && !invalid);
  if(invoke && !invalid) {
   BSTR old_name=NULL;IUIAutomationElement *old_cell=NULL;
   HRESULT old_name_state=IUIAutomationElement_get_CurrentName(table_element,&old_name);
   HRESULT old_item_state=IUIAutomationGridPattern_GetItem(table_grid,0,0,&old_cell);
   printf("WINDOWS TABLE RETIRED: name=0x%08lx units=%u item=0x%08lx cell=%d\n",
       (unsigned long)old_name_state,old_name?SysStringLen(old_name):0,(unsigned long)old_item_state,old_cell!=NULL);
   /* UIA's client property getter returns S_OK/0 after disconnection on this
    * Windows build. Require explicit element/method unavailability, no old
    * cell and no retained row data; direct provider HRESULTs are tested too. */
   if(old_name_state!=UIA_E_ELEMENTNOTAVAILABLE || old_item_state!=UIA_E_ELEMENTNOTAVAILABLE ||
      old_cell || (old_name && SysStringLen(old_name)) ||
      !(state==UIA_E_ELEMENTNOTAVAILABLE || (state==S_OK && rows==0)))result=E_FAIL;
   SysFreeString(old_name);if(old_cell)IUIAutomationElement_Release(old_cell);
  }
  else if(FAILED(state) || rows!=2)result=E_FAIL;
  if(FAILED(result))fprintf(stderr,"WINDOWS TABLE LIFETIME FAILED: result=0x%08lx rows=%d\n",(unsigned long)state,rows);
 }
 if(table_events) {
  LONG bounds=events_count(&table_events->bounds),disabled=events_count(&table_events->disabled),enabled=events_count(&table_events->enabled);
  printf("WINDOWS TABLE EVENTS: bounds=%ld disabled=%ld enabled=%ld invoked=%ld bad=%ld\n",bounds,disabled,enabled,events_count(&table_events->invoked),events_count(&table_events->bad));
  if(events_count(&table_events->bad) || events_count(&table_events->invoked) || (!invoke && (bounds<1 || disabled<1 || enabled<1)))result=E_FAIL;
  if(getenv("PHYSIM_TEST_TRACE"))fprintf(stderr,"WINDOWS TABLE TRACE: table event cleanup begin\n");
  events_stop(automation,table_element,table_events);
  if(getenv("PHYSIM_TEST_TRACE"))fprintf(stderr,"WINDOWS TABLE TRACE: table event cleanup returned\n");
 }
 if(events){printf("WINDOWS LINK EVENTS: bounds=%ld offscreen=%ld disabled=%ld enabled=%ld invoked=%ld bad=%ld\n",events_count(&events->bounds),events_count(&events->offscreen),events_count(&events->disabled),events_count(&events->enabled),events_count(&events->invoked),events_count(&events->bad));if(events_count(&events->bad))result=E_FAIL;events_stop(automation,selected,events);}
 if(selected_range)IUIAutomationTextRange_Release(selected_range);if(selected)IUIAutomationElement_Release(selected);
 if(children)IUIAutomationElementArray_Release(children);if(whole)IUIAutomationTextRange_Release(whole);
 for(int i=0;i<2;i++)if(ordered_links[i])IUIAutomationElement_Release(ordered_links[i]);
 if(table_grid)IUIAutomationGridPattern_Release(table_grid);if(table_element)IUIAutomationElement_Release(table_element);
 if(pattern)IUIAutomationTextPattern_Release(pattern);if(unknown)IUnknown_Release(unknown);
 if(SUCCEEDED(result))puts(invalid?"DOCUMENT LINKS: rejected target stays readable without invoked event":invoke?"DOCUMENT LINKS: navigation and stale range passed":"DOCUMENT LINKS: two ordered hyperlinks, exact text ranges, offscreen focus and visible glyphs passed");
 return result;
}
static HRESULT read_document(IUIAutomationElement *control,const wchar_t *path) {
 IUnknown *unknown=NULL;IUIAutomationTextPattern *pattern=NULL;IUIAutomationTextRange *whole=NULL,*copy=NULL,*last=NULL;
 BSTR text=NULL,query=NULL;SAFEARRAY *bounds=NULL;HRESULT result=IUIAutomationElement_GetCurrentPattern(control,UIA_TextPatternId,&unknown);
 if(SUCCEEDED(result) && unknown)result=IUnknown_QueryInterface(unknown,&IID_IUIAutomationTextPattern,(void**)&pattern);
 else result=E_FAIL;
 if(SUCCEEDED(result))result=IUIAutomationTextPattern_get_DocumentRange(pattern,&whole);
 if(SUCCEEDED(result))result=IUIAutomationTextRange_GetText(whole,-1,&text);
 if(SUCCEEDED(result))result=IUIAutomationTextRange_Clone(whole,&copy);
 BOOL equal=FALSE;if(SUCCEEDED(result))result=IUIAutomationTextRange_Compare(whole,copy,&equal);
 if(SUCCEEDED(result) && !equal)result=E_FAIL;
 if(SUCCEEDED(result))result=IUIAutomationTextRange_GetBoundingRectangles(whole,&bounds);
 if(SUCCEEDED(result)){LONG high=-1;SafeArrayGetUBound(bounds,1,&high);if(high<3)result=E_FAIL;}
 if(SUCCEEDED(result)) {
  int count=WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,text,(int)SysStringLen(text),NULL,0,NULL,NULL);
  char *bytes=count>0?malloc((size_t)count):NULL;FILE *file=NULL;
  if(bytes && WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,text,(int)SysStringLen(text),bytes,count,NULL,NULL))file=_wfopen(path,L"wb");
  if(!file)result=E_FAIL;else {if(fwrite(bytes,1,(size_t)count,file)!=(size_t)count)result=E_FAIL;if(fclose(file))result=E_FAIL;}
  free(bytes);
 }
 if(SUCCEEDED(result)) {
  UINT length=SysStringLen(text),n=length<64?length:64;query=SysAllocStringLen(text+length-n,n);
  result=query?IUIAutomationTextRange_FindText(whole,query,TRUE,FALSE,&last):E_OUTOFMEMORY;
  if(SUCCEEDED(result) && !last)result=E_FAIL;
  if(SUCCEEDED(result))result=IUIAutomationTextRange_ScrollIntoView(last,TRUE);
 }
 if(bounds)SafeArrayDestroy(bounds);SysFreeString(query);SysFreeString(text);
 if(last)IUIAutomationTextRange_Release(last);if(copy)IUIAutomationTextRange_Release(copy);if(whole)IUIAutomationTextRange_Release(whole);
 if(pattern)IUIAutomationTextPattern_Release(pattern);if(unknown)IUnknown_Release(unknown);return result;
}
static HRESULT find_edit(IUIAutomation *a,IUIAutomationElement *desktop,LONG pid,const wchar_t *label,CONTROLTYPEID type,IUIAutomationElement **out) {
 *out=NULL;IUIAutomationCondition *condition=NULL;IUIAutomationElementArray *windows=NULL;
 VARIANT process;VariantInit(&process);process.vt=VT_I4;process.lVal=pid;
 HRESULT result=IUIAutomation_CreatePropertyCondition(a,UIA_ProcessIdPropertyId,process,&condition);
 if(SUCCEEDED(result))result=IUIAutomationElement_FindAll(desktop,TreeScope_Children,condition,&windows);
 int count=0;if(SUCCEEDED(result))result=IUIAutomationElementArray_get_Length(windows,&count);
 for(int i=0;SUCCEEDED(result) && !*out && i<count;i++) {
  IUIAutomationElement *window=NULL;result=IUIAutomationElementArray_GetElement(windows,i,&window);
  if(SUCCEEDED(result))result=named(a,window,label,type,out);if(window)IUIAutomationElement_Release(window);
 }
 if(windows)IUIAutomationElementArray_Release(windows);if(condition)IUIAutomationCondition_Release(condition);return result;
}
/* Query desktop focus independently of the expected field's own property. */
static void trace_field_host(IUIAutomationElement *expected,const char *phase) {
 if(!getenv("PHYSIM_UIA_FOCUS_TRACE"))return;
 int target=0;DWORD foreground_pid=0;HWND foreground=GetForegroundWindow(),console=GetConsoleWindow();
 bool available=SUCCEEDED(IUIAutomationElement_get_CurrentProcessId(expected,&target)) && target>0;
 GetWindowThreadProcessId(foreground,&foreground_pid);
 fprintf(stderr,"WINDOWS FIELD HOST: phase=%s target_available=%d foreground_target=%d console_visible=%d foreground_console=%d\n",
  phase,available,available && foreground_pid==(DWORD)target,console && IsWindowVisible(console),console && foreground==console);
}
static HRESULT current_field_focus(IUIAutomation *a,IUIAutomationElement *expected) {
 ULONGLONG deadline=GetTickCount64()+5000;unsigned stable=0;HRESULT result=S_OK;
 BOOL focused=FALSE,same=FALSE;
 trace_field_host(expected,"begin");
 do {
  IUIAutomationElement *actual=NULL;focused=same=FALSE;
  result=IUIAutomationElement_get_CurrentHasKeyboardFocus(expected,&focused);
  if(SUCCEEDED(result))result=IUIAutomation_GetFocusedElement(a,&actual);
  if(SUCCEEDED(result) && actual) {
   result=IUIAutomation_CompareElements(a,expected,actual,&same);
  }
  if(actual)IUIAutomationElement_Release(actual);
  if(SUCCEEDED(result) && focused && same)++stable;else stable=0;
  if(stable==3){trace_field_host(expected,"passed");puts("WINDOWS FIELD FOCUS: property and GetFocusedElement agree");return S_OK;}
  if(FAILED(result) && result!=UIA_E_ELEMENTNOTAVAILABLE)break;
  Sleep(20);
 } while(GetTickCount64()<deadline);
 fprintf(stderr,"WINDOWS FIELD FOCUS: mismatch property=%d same=%d status=0x%08lx\n",focused,same,(unsigned long)result);
 trace_field_host(expected,"failed");
 return FAILED(result)?result:HRESULT_FROM_WIN32(WAIT_TIMEOUT);
}
static HRESULT select_text(IUIAutomationElement *control,const wchar_t *query) {
 IUnknown *unknown=NULL;IUIAutomationTextPattern *pattern=NULL;IUIAutomationTextRange *whole=NULL,*match=NULL;
 BSTR text=SysAllocString(query);SAFEARRAY *rectangles=NULL;enum SupportedTextSelection supported=SupportedTextSelection_None;
 HRESULT result=IUIAutomationElement_GetCurrentPattern(control,UIA_TextPatternId,&unknown);
 if(SUCCEEDED(result))result=unknown?IUnknown_QueryInterface(unknown,&IID_IUIAutomationTextPattern,(void**)&pattern):E_FAIL;
 if(SUCCEEDED(result))result=IUIAutomationTextPattern_get_SupportedTextSelection(pattern,&supported);
 if(SUCCEEDED(result) && supported!=SupportedTextSelection_Single)result=E_FAIL;
 if(SUCCEEDED(result))result=IUIAutomationTextPattern_get_DocumentRange(pattern,&whole);
 if(SUCCEEDED(result))result=text?IUIAutomationTextRange_FindText(whole,text,FALSE,FALSE,&match):E_OUTOFMEMORY;
 if(SUCCEEDED(result) && !match)result=E_FAIL;
 if(SUCCEEDED(result))result=IUIAutomationTextRange_GetBoundingRectangles(match,&rectangles);
 if(SUCCEEDED(result)){LONG high=-1;SafeArrayGetUBound(rectangles,1,&high);if(high<3)result=E_FAIL;}
 if(SUCCEEDED(result))result=IUIAutomationTextRange_Select(match);
 bool applied=false;ULONGLONG deadline=GetTickCount64()+4000;
 while(SUCCEEDED(result) && !applied && GetTickCount64()<deadline) {
  IUIAutomationTextRangeArray *selection=NULL;IUIAutomationTextRange *range=NULL;BSTR selected=NULL;int count=0;
  result=IUIAutomationTextPattern_GetSelection(pattern,&selection);
  if(SUCCEEDED(result))result=IUIAutomationTextRangeArray_get_Length(selection,&count);
  if(SUCCEEDED(result) && count==1)result=IUIAutomationTextRangeArray_GetElement(selection,0,&range);
  if(SUCCEEDED(result) && range){result=IUIAutomationTextRange_GetText(range,-1,&selected);applied=SUCCEEDED(result) && selected && !wcscmp(selected,query);}
  SysFreeString(selected);if(range)IUIAutomationTextRange_Release(range);if(selection)IUIAutomationTextRangeArray_Release(selection);
  if(!applied)Sleep(10);
 }
 if(SUCCEEDED(result) && !applied)result=E_FAIL;
 if(SUCCEEDED(result)) {
  IUnknown *caret_unknown=NULL;IUIAutomationTextPattern2 *caret_pattern=NULL;IUIAutomationTextRange *caret=NULL;BOOL active=FALSE;int distance=99;
  result=IUIAutomationElement_GetCurrentPattern(control,UIA_TextPattern2Id,&caret_unknown);
  if(SUCCEEDED(result))result=caret_unknown?IUnknown_QueryInterface(caret_unknown,&IID_IUIAutomationTextPattern2,(void**)&caret_pattern):E_FAIL;
  if(SUCCEEDED(result))result=IUIAutomationTextPattern2_GetCaretRange(caret_pattern,&active,&caret);
  if(SUCCEEDED(result) && (!active || !caret))result=E_FAIL;
  if(SUCCEEDED(result))result=IUIAutomationTextRange_CompareEndpoints(caret,TextPatternRangeEndpoint_Start,match,TextPatternRangeEndpoint_End,&distance);
  if(SUCCEEDED(result) && distance)result=E_FAIL;
  SAFEARRAY *caret_bounds=NULL;if(SUCCEEDED(result))result=IUIAutomationTextRange_GetBoundingRectangles(caret,&caret_bounds);
  if(SUCCEEDED(result)){LONG high=-1;SafeArrayGetUBound(caret_bounds,1,&high);if(high!=3)result=E_FAIL;}
  if(caret_bounds)SafeArrayDestroy(caret_bounds);if(caret)IUIAutomationTextRange_Release(caret);
  if(caret_pattern)IUIAutomationTextPattern2_Release(caret_pattern);if(caret_unknown)IUnknown_Release(caret_unknown);
 }
 if(rectangles)SafeArrayDestroy(rectangles);SysFreeString(text);if(match)IUIAutomationTextRange_Release(match);if(whole)IUIAutomationTextRange_Release(whole);
 if(pattern)IUIAutomationTextPattern_Release(pattern);if(unknown)IUnknown_Release(unknown);return result;
}
static HRESULT text_selection(IUIAutomationTextPattern *pattern,IUIAutomationTextRange **out) {
 *out=NULL;IUIAutomationTextRangeArray *ranges=NULL;int count=0;HRESULT result=IUIAutomationTextPattern_GetSelection(pattern,&ranges);
 if(SUCCEEDED(result))result=IUIAutomationTextRangeArray_get_Length(ranges,&count);
 if(SUCCEEDED(result))result=count==1?IUIAutomationTextRangeArray_GetElement(ranges,0,out):E_FAIL;
 if(ranges)IUIAutomationTextRangeArray_Release(ranges);return result;
}
static HRESULT text_visible(IUIAutomationTextPattern *pattern,IUIAutomationTextRange *match,bool *out) {
 *out=false;IUIAutomationTextRangeArray *ranges=NULL;int count=0;HRESULT result=IUIAutomationTextPattern_GetVisibleRanges(pattern,&ranges);
 if(SUCCEEDED(result))result=IUIAutomationTextRangeArray_get_Length(ranges,&count);
 for(int i=0;SUCCEEDED(result) && i<count && !*out;i++) {
  IUIAutomationTextRange *range=NULL;int start=-1,end=1;result=IUIAutomationTextRangeArray_GetElement(ranges,i,&range);
  if(SUCCEEDED(result))result=IUIAutomationTextRange_CompareEndpoints(match,TextPatternRangeEndpoint_Start,range,TextPatternRangeEndpoint_Start,&start);
  if(SUCCEEDED(result))result=IUIAutomationTextRange_CompareEndpoints(match,TextPatternRangeEndpoint_End,range,TextPatternRangeEndpoint_End,&end);
  if(SUCCEEDED(result))*out=start>=0 && end<=0;if(range)IUIAutomationTextRange_Release(range);
 }
 if(ranges)IUIAutomationTextRangeArray_Release(ranges);return result;
}
static HRESULT scroll_text(IUIAutomationElement *control,const wchar_t *query,BOOL top) {
 IUnknown *unknown=NULL;IUIAutomationTextPattern2 *pattern=NULL;IUIAutomationTextPattern *base=NULL;IUIAutomationValuePattern *value=NULL;
 IUIAutomationTextRange *whole=NULL,*match=NULL,*before=NULL,*after=NULL,*caret_before=NULL,*caret_after=NULL;
 BSTR text=SysAllocString(query),old_value=NULL,new_value=NULL;BOOL old_focus=TRUE,new_focus=TRUE,old_active=TRUE,new_active=TRUE;bool visible=false;
 HRESULT result=IUIAutomationElement_GetCurrentPattern(control,UIA_TextPattern2Id,&unknown);
 if(SUCCEEDED(result))result=unknown?IUnknown_QueryInterface(unknown,&IID_IUIAutomationTextPattern2,(void**)&pattern):E_FAIL;
 if(unknown){IUnknown_Release(unknown);unknown=NULL;}
 if(SUCCEEDED(result))result=IUIAutomationElement_GetCurrentPattern(control,UIA_TextPatternId,&unknown);
 if(SUCCEEDED(result))result=unknown?IUnknown_QueryInterface(unknown,&IID_IUIAutomationTextPattern,(void**)&base):E_FAIL;
 if(unknown){IUnknown_Release(unknown);unknown=NULL;}
 if(SUCCEEDED(result))result=IUIAutomationElement_GetCurrentPattern(control,UIA_ValuePatternId,&unknown);
 if(SUCCEEDED(result))result=unknown?IUnknown_QueryInterface(unknown,&IID_IUIAutomationValuePattern,(void**)&value):E_FAIL;
 if(SUCCEEDED(result))result=IUIAutomationValuePattern_get_CurrentValue(value,&old_value);
 if(SUCCEEDED(result))result=IUIAutomationElement_get_CurrentHasKeyboardFocus(control,&old_focus);
 if(SUCCEEDED(result))result=text_selection(base,&before);
 if(SUCCEEDED(result))result=IUIAutomationTextPattern2_GetCaretRange(pattern,&old_active,&caret_before);
 if(SUCCEEDED(result))result=IUIAutomationTextPattern_get_DocumentRange(base,&whole);
 if(SUCCEEDED(result))result=text?IUIAutomationTextRange_FindText(whole,text,FALSE,FALSE,&match):E_OUTOFMEMORY;
 if(SUCCEEDED(result) && !match)result=E_FAIL;
 if(SUCCEEDED(result))result=text_visible(base,match,&visible);
 if(getenv("PHYSIM_UIA_SCROLL_TRACE"))fprintf(stderr,"UIA SCROLL BEFORE: result=0x%08lx visible=%d focus=%d active=%d top=%d\n",(unsigned long)result,visible,old_focus,old_active,top);
 if(SUCCEEDED(result) && (visible || old_focus || old_active))result=E_FAIL;
 if(SUCCEEDED(result))result=IUIAutomationTextRange_ScrollIntoView(match,top);
 ULONGLONG deadline=GetTickCount64()+4000;
 while(SUCCEEDED(result) && !visible && GetTickCount64()<deadline){result=text_visible(base,match,&visible);if(!visible)Sleep(10);}
 if(getenv("PHYSIM_UIA_SCROLL_TRACE"))fprintf(stderr,"UIA SCROLL VISIBLE: result=0x%08lx visible=%d\n",(unsigned long)result,visible);
 if(SUCCEEDED(result) && !visible)result=E_FAIL;
 if(SUCCEEDED(result))result=IUIAutomationValuePattern_get_CurrentValue(value,&new_value);
 if(SUCCEEDED(result))result=IUIAutomationElement_get_CurrentHasKeyboardFocus(control,&new_focus);
 if(SUCCEEDED(result))result=text_selection(base,&after);
 if(SUCCEEDED(result))result=IUIAutomationTextPattern2_GetCaretRange(pattern,&new_active,&caret_after);
 if(getenv("PHYSIM_UIA_SCROLL_TRACE"))fprintf(stderr,"UIA SCROLL AFTER: result=0x%08lx focus=%d active=%d value-equal=%d\n",(unsigned long)result,new_focus,new_active,old_value && new_value && !wcscmp(old_value,new_value));
 if(SUCCEEDED(result) && (wcscmp(old_value,new_value) || old_focus!=new_focus || old_active!=new_active))result=E_FAIL;
 IUIAutomationTextRange *left[]={before,caret_before},*right[]={after,caret_after};
 for(int i=0;SUCCEEDED(result) && i<2;i++)for(int end=0;SUCCEEDED(result) && end<2;end++) {
  enum TextPatternRangeEndpoint endpoint=end?TextPatternRangeEndpoint_End:TextPatternRangeEndpoint_Start;int distance=99;
  result=IUIAutomationTextRange_CompareEndpoints(left[i],endpoint,right[i],endpoint,&distance);if(SUCCEEDED(result) && distance)result=E_FAIL;
 }
 if(caret_after)IUIAutomationTextRange_Release(caret_after);if(caret_before)IUIAutomationTextRange_Release(caret_before);
 if(after)IUIAutomationTextRange_Release(after);if(before)IUIAutomationTextRange_Release(before);if(match)IUIAutomationTextRange_Release(match);if(whole)IUIAutomationTextRange_Release(whole);
 SysFreeString(text);SysFreeString(old_value);SysFreeString(new_value);
 if(value)IUIAutomationValuePattern_Release(value);if(base)IUIAutomationTextPattern_Release(base);if(pattern)IUIAutomationTextPattern2_Release(pattern);if(unknown)IUnknown_Release(unknown);
 return result;
}
#include "accessibility_windows_lifecycle_probe.inc"
int wmain(int argc,wchar_t **argv) {
 setvbuf(stdout,NULL,_IONBF,0);setvbuf(stderr,NULL,_IONBF,0);
 if(argc!=4 && argc!=5)return 2;
 wchar_t *end=NULL;unsigned long pid=wcstoul(argv[1],&end,10);if(!pid || *end)return 2;
 bool focus=!wcscmp(argv[2],L"focus"),toggle=!wcscmp(argv[2],L"toggle"),select=!wcscmp(argv[2],L"select"),read=!wcscmp(argv[2],L"read"),edit=!wcscmp(argv[2],L"edit"),text_select=!wcscmp(argv[2],L"select-text");
 bool scroll_start=!wcscmp(argv[2],L"scroll-text-start"),scroll_end=!wcscmp(argv[2],L"scroll-text-end");
 bool stress_links=!wcscmp(argv[2],L"links-stress");
 bool links=!wcscmp(argv[2],L"links") || stress_links,invoke_link=!wcscmp(argv[2],L"invoke-link");
 bool invalid_link=!wcscmp(argv[2],L"invoke-link-invalid");
 bool lifecycle=!wcscmp(argv[2],L"lifecycle");
 bool focused_field=!wcscmp(argv[2],L"focused-field"),focused_button=!wcscmp(argv[2],L"focused-button"),focused_control=!wcscmp(argv[2],L"focused-control");
 if(!focus && !toggle && !select && !read && !edit && !text_select && !scroll_start && !scroll_end && !links && !invoke_link && !invalid_link && !lifecycle && !focused_field && !focused_button && !focused_control)return 2;
 if((read || edit || text_select || scroll_start || scroll_end || links || invoke_link || invalid_link || lifecycle) && argc!=5)return 2;
 HRESULT initialized=CoInitializeEx(NULL,COINIT_MULTITHREADED);if(FAILED(initialized))return 1;
 IUIAutomation *automation=NULL;IUIAutomationElement *desktop=NULL,*window=NULL,*group=NULL,*control=NULL;
 IUnknown *unknown=NULL;bool ok=SUCCEEDED(CoCreateInstance(&CLSID_CUIAutomation,NULL,CLSCTX_INPROC_SERVER,&IID_IUIAutomation,(void**)&automation));
 if(ok)ok=SUCCEEDED(IUIAutomation_GetRootElement(automation,&desktop));
 VARIANT process;VariantInit(&process);process.vt=VT_I4;process.lVal=(LONG)pid;
 if(ok)ok=SUCCEEDED(find(automation,desktop,TreeScope_Children,UIA_ProcessIdPropertyId,process,&window)) && window;
 if(ok && (edit || focused_field || focused_button || focused_control || text_select || scroll_start || scroll_end))ok=SUCCEEDED(find_edit(automation,desktop,(LONG)pid,argv[3],focused_control?0:focused_button?UIA_ButtonControlTypeId:UIA_EditControlTypeId,&control)) && control;
 else if(ok && (read || links || invoke_link || invalid_link || lifecycle)) {
  IUIAutomationCondition *pid_condition=NULL,*title_condition=NULL,*both=NULL;VARIANT title;VariantInit(&title);title.vt=VT_BSTR;title.bstrVal=SysAllocString(argv[3]);
  ok=title.bstrVal && SUCCEEDED(IUIAutomation_CreatePropertyCondition(automation,UIA_ProcessIdPropertyId,process,&pid_condition)) &&
   SUCCEEDED(IUIAutomation_CreatePropertyCondition(automation,UIA_NamePropertyId,title,&title_condition)) &&
   SUCCEEDED(IUIAutomation_CreateAndCondition(automation,pid_condition,title_condition,&both)) &&
   SUCCEEDED(IUIAutomationElement_FindFirst(desktop,TreeScope_Children,both,&control)) && control;
  if(both)IUIAutomationCondition_Release(both);if(title_condition)IUIAutomationCondition_Release(title_condition);if(pid_condition)IUIAutomationCondition_Release(pid_condition);VariantClear(&title);
 } else {
  if(ok && argc==5)ok=SUCCEEDED(named(automation,window,argv[4],UIA_GroupControlTypeId,&group)) && group;
  if(ok)ok=SUCCEEDED(named(automation,group?group:window,argv[3],0,&control)) && control;
 }
 CONTROLTYPEID role=0;BOOL enabled=FALSE,focusable=FALSE;HRESULT action=E_FAIL;
 if(ok)ok=SUCCEEDED(IUIAutomationElement_get_CurrentControlType(control,&role)) &&
  SUCCEEDED(IUIAutomationElement_get_CurrentIsEnabled(control,&enabled)) && enabled;
 if(ok && (focused_field || focused_button || focused_control))action=current_field_focus(automation,control);
 else if(ok && edit) {
  IUIAutomationValuePattern *pattern=NULL;BOOL readonly=TRUE;
  ok=role==UIA_EditControlTypeId && SUCCEEDED(IUIAutomationElement_GetCurrentPattern(control,UIA_ValuePatternId,&unknown)) && unknown &&
   SUCCEEDED(IUnknown_QueryInterface(unknown,&IID_IUIAutomationValuePattern,(void**)&pattern)) &&
   SUCCEEDED(IUIAutomationValuePattern_get_CurrentIsReadOnly(pattern,&readonly)) && !readonly;
  if(ok)action=IUIAutomationValuePattern_SetValue(pattern,argv[4]);
  if(ok && SUCCEEDED(action))action=IUIAutomationElement_SetFocus(control);
  if(pattern)IUIAutomationValuePattern_Release(pattern);
 } else if(ok && (scroll_start || scroll_end))action=scroll_text(control,argv[4],scroll_start);
 else if(ok && text_select)action=select_text(control,argv[4]);
 else if(ok && read)action=read_document(control,argv[4]);
 else if(ok && (links || invoke_link || invalid_link))action=document_links(automation,control,argv[4],invoke_link || invalid_link,invalid_link,stress_links?10:1);
 else if(ok && lifecycle)action=document_lifecycle(automation,control,argv[4]);
 else if(ok && focus) {
  ok=SUCCEEDED(IUIAutomationElement_get_CurrentIsKeyboardFocusable(control,&focusable)) && focusable;
  if(ok)action=IUIAutomationElement_SetFocus(control);
 } else if(ok && toggle) {
  IUIAutomationTogglePattern *pattern=NULL;
  ok=role==UIA_CheckBoxControlTypeId && SUCCEEDED(IUIAutomationElement_GetCurrentPattern(control,UIA_TogglePatternId,&unknown)) && unknown &&
   SUCCEEDED(IUnknown_QueryInterface(unknown,&IID_IUIAutomationTogglePattern,(void**)&pattern));
  if(ok)action=IUIAutomationTogglePattern_Toggle(pattern);if(pattern)IUIAutomationTogglePattern_Release(pattern);
 } else if(ok && select) {
  IUIAutomationSelectionItemPattern *pattern=NULL;
  ok=role==UIA_RadioButtonControlTypeId && SUCCEEDED(IUIAutomationElement_GetCurrentPattern(control,UIA_SelectionItemPatternId,&unknown)) && unknown &&
   SUCCEEDED(IUnknown_QueryInterface(unknown,&IID_IUIAutomationSelectionItemPattern,(void**)&pattern));
  if(ok)action=IUIAutomationSelectionItemPattern_Select(pattern);if(pattern)IUIAutomationSelectionItemPattern_Release(pattern);
 }
 ok=ok && SUCCEEDED(action);
 if(unknown)IUnknown_Release(unknown);if(control)IUIAutomationElement_Release(control);if(group)IUIAutomationElement_Release(group);
 if(window)IUIAutomationElement_Release(window);if(desktop)IUIAutomationElement_Release(desktop);if(automation)IUIAutomation_Release(automation);
 CoUninitialize();
 if(ok)printf("INDEPENDENT WINDOWS UIA: PASSED role=%d\n",role);
 else fprintf(stderr,"Independent Windows UIA failed: pid=%lu action=0x%08lX role=%d enabled=%d found=%d\n",pid,(unsigned long)action,role,enabled,control!=NULL);
 return ok?0:1;
}
