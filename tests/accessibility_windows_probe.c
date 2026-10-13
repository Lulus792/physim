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
int wmain(int argc,wchar_t **argv) {
 if(argc!=4 && argc!=5)return 2;
 wchar_t *end=NULL;unsigned long pid=wcstoul(argv[1],&end,10);if(!pid || *end)return 2;
 bool focus=!wcscmp(argv[2],L"focus"),toggle=!wcscmp(argv[2],L"toggle"),select=!wcscmp(argv[2],L"select");
 if(!focus && !toggle && !select)return 2;
 HRESULT initialized=CoInitializeEx(NULL,COINIT_MULTITHREADED);if(FAILED(initialized))return 1;
 IUIAutomation *automation=NULL;IUIAutomationElement *desktop=NULL,*window=NULL,*group=NULL,*control=NULL;
 IUnknown *unknown=NULL;bool ok=SUCCEEDED(CoCreateInstance(&CLSID_CUIAutomation,NULL,CLSCTX_INPROC_SERVER,&IID_IUIAutomation,(void**)&automation));
 if(ok)ok=SUCCEEDED(IUIAutomation_GetRootElement(automation,&desktop));
 VARIANT process;VariantInit(&process);process.vt=VT_I4;process.lVal=(LONG)pid;
 if(ok)ok=SUCCEEDED(find(automation,desktop,TreeScope_Children,UIA_ProcessIdPropertyId,process,&window)) && window;
 if(ok && argc==5)ok=SUCCEEDED(named(automation,window,argv[4],UIA_GroupControlTypeId,&group)) && group;
 if(ok)ok=SUCCEEDED(named(automation,group?group:window,argv[3],0,&control)) && control;
 CONTROLTYPEID role=0;BOOL enabled=FALSE,focusable=FALSE;HRESULT action=E_FAIL;
 if(ok)ok=SUCCEEDED(IUIAutomationElement_get_CurrentControlType(control,&role)) &&
  SUCCEEDED(IUIAutomationElement_get_CurrentIsEnabled(control,&enabled)) && enabled;
 if(ok && focus) {
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
 else fprintf(stderr,"Independent Windows UIA failed: pid=%lu action=0x%08lX\n",pid,(unsigned long)action);
 return ok?0:1;
}
