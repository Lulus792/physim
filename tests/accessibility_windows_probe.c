/* Independent UI Automation client: no renderer, SDL, bridge or model linkage. */
#define COBJMACROS
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
static HRESULT find_edit(IUIAutomation *a,IUIAutomationElement *desktop,LONG pid,const wchar_t *label,IUIAutomationElement **out) {
 *out=NULL;IUIAutomationCondition *condition=NULL;IUIAutomationElementArray *windows=NULL;
 VARIANT process;VariantInit(&process);process.vt=VT_I4;process.lVal=pid;
 HRESULT result=IUIAutomation_CreatePropertyCondition(a,UIA_ProcessIdPropertyId,process,&condition);
 if(SUCCEEDED(result))result=IUIAutomationElement_FindAll(desktop,TreeScope_Children,condition,&windows);
 int count=0;if(SUCCEEDED(result))result=IUIAutomationElementArray_get_Length(windows,&count);
 for(int i=0;SUCCEEDED(result) && !*out && i<count;i++) {
  IUIAutomationElement *window=NULL;result=IUIAutomationElementArray_GetElement(windows,i,&window);
  if(SUCCEEDED(result))result=named(a,window,label,UIA_EditControlTypeId,out);if(window)IUIAutomationElement_Release(window);
 }
 if(windows)IUIAutomationElementArray_Release(windows);if(condition)IUIAutomationCondition_Release(condition);return result;
}
int wmain(int argc,wchar_t **argv) {
 if(argc!=4 && argc!=5)return 2;
 wchar_t *end=NULL;unsigned long pid=wcstoul(argv[1],&end,10);if(!pid || *end)return 2;
 bool focus=!wcscmp(argv[2],L"focus"),toggle=!wcscmp(argv[2],L"toggle"),select=!wcscmp(argv[2],L"select"),read=!wcscmp(argv[2],L"read"),edit=!wcscmp(argv[2],L"edit");
 if(!focus && !toggle && !select && !read && !edit)return 2;if((read || edit) && argc!=5)return 2;
 HRESULT initialized=CoInitializeEx(NULL,COINIT_MULTITHREADED);if(FAILED(initialized))return 1;
 IUIAutomation *automation=NULL;IUIAutomationElement *desktop=NULL,*window=NULL,*group=NULL,*control=NULL;
 IUnknown *unknown=NULL;bool ok=SUCCEEDED(CoCreateInstance(&CLSID_CUIAutomation,NULL,CLSCTX_INPROC_SERVER,&IID_IUIAutomation,(void**)&automation));
 if(ok)ok=SUCCEEDED(IUIAutomation_GetRootElement(automation,&desktop));
 VARIANT process;VariantInit(&process);process.vt=VT_I4;process.lVal=(LONG)pid;
 if(ok)ok=SUCCEEDED(find(automation,desktop,TreeScope_Children,UIA_ProcessIdPropertyId,process,&window)) && window;
 if(ok && edit)ok=SUCCEEDED(find_edit(automation,desktop,(LONG)pid,argv[3],&control)) && control;
 else if(ok && read) {
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
 if(ok && edit) {
  IUIAutomationValuePattern *pattern=NULL;BOOL readonly=TRUE;
  ok=role==UIA_EditControlTypeId && SUCCEEDED(IUIAutomationElement_GetCurrentPattern(control,UIA_ValuePatternId,&unknown)) && unknown &&
   SUCCEEDED(IUnknown_QueryInterface(unknown,&IID_IUIAutomationValuePattern,(void**)&pattern)) &&
   SUCCEEDED(IUIAutomationValuePattern_get_CurrentIsReadOnly(pattern,&readonly)) && !readonly;
  if(ok)action=IUIAutomationValuePattern_SetValue(pattern,argv[4]);
  if(ok && SUCCEEDED(action))action=IUIAutomationElement_SetFocus(control);
  if(pattern)IUIAutomationValuePattern_Release(pattern);
 } else if(ok && read)action=read_document(control,argv[4]);
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
