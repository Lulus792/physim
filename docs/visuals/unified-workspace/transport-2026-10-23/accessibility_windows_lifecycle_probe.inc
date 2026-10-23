static HRESULT lifecycle_file(const wchar_t *directory,const wchar_t *name,bool wait) {
 wchar_t path[4096];if(swprintf_s(path,4096,L"%s/%s",directory,name)<0)return E_INVALIDARG;
 ULONGLONG deadline=GetTickCount64()+15000;
 do {FILE *file=_wfopen(path,wait?L"rb":L"wb");if(file){if(!wait)fputs("verified\n",file);fclose(file);return S_OK;}
  if(!wait || GetTickCount64()>=deadline)return E_FAIL;Sleep(20);
 }while(true);
}
static HRESULT lifecycle_views(IUIAutomationTextPattern *pattern,IUIAutomationTextRange *whole,bool visible) {
 IUIAutomationTextRangeArray *ranges=NULL;SAFEARRAY *bounds=NULL;HRESULT result=IUIAutomationTextPattern_GetVisibleRanges(pattern,&ranges);
 int count=0;bool ink=false;
 if(SUCCEEDED(result))result=IUIAutomationTextRangeArray_get_Length(ranges,&count);
 for(int i=0;SUCCEEDED(result) && i<count;i++) {
  IUIAutomationTextRange *range=NULL;BSTR text=NULL;result=IUIAutomationTextRangeArray_GetElement(ranges,i,&range);
  if(SUCCEEDED(result))result=IUIAutomationTextRange_GetText(range,-1,&text);
  if(SUCCEEDED(result) && text && SysStringLen(text))ink=true;
  SysFreeString(text);if(range)IUIAutomationTextRange_Release(range);
 }
 if(SUCCEEDED(result) && ink!=visible){fprintf(stderr,"Lifecycle visible text: expected=%d actual=%d ranges=%d\n",visible,ink,count);result=E_FAIL;}
 if(SUCCEEDED(result))result=IUIAutomationTextRange_GetBoundingRectangles(whole,&bounds);
 if(SUCCEEDED(result)){LONG high=-1;SafeArrayGetUBound(bounds,1,&high);if((high>=3)!=visible){fprintf(stderr,"Lifecycle text geometry: visible=%d last=%ld\n",visible,high);result=E_FAIL;}}
 if(bounds)SafeArrayDestroy(bounds);if(ranges)IUIAutomationTextRangeArray_Release(ranges);return result;
}
static HRESULT lifecycle_text(IUIAutomationTextRange *range,const wchar_t *expected) {
 BSTR text=NULL;HRESULT result=IUIAutomationTextRange_GetText(range,-1,&text);
 if(SUCCEEDED(result) && (!text || wcscmp(text,expected)))result=E_FAIL;SysFreeString(text);return result;
}
static HRESULT lifecycle_unfocused(IUIAutomationElement *field,IUIAutomationTextPattern2 *pattern) {
 BOOL focused=TRUE,active=TRUE;IUIAutomationTextRange *caret=NULL;
 HRESULT result=IUIAutomationElement_get_CurrentHasKeyboardFocus(field,&focused);
 if(SUCCEEDED(result))result=IUIAutomationTextPattern2_GetCaretRange(pattern,&active,&caret);
 if(caret)IUIAutomationTextRange_Release(caret);
 if(SUCCEEDED(result) && (focused || active)){fputs("Lifecycle restored an old editable focus\n",stderr);result=E_FAIL;}return result;
}
static HRESULT lifecycle_hidden(IUIAutomationElement *field,IUIAutomationTextPattern2 *pattern,
 IUIAutomationTextPattern *document,IUIAutomationTextRange *whole,IUIAutomationTextRange *field_range,
 IUIAutomationTextPattern *field_text,IUIAutomationElement *link,link_events *events,LONG hidden,HWND host) {
 HRESULT result=S_OK;ULONGLONG deadline=GetTickCount64()+10000;
 while(IsWindowVisible(host) && !IsIconic(host)){if(GetTickCount64()>=deadline){result=E_FAIL;break;}Sleep(20);}
 BOOL focused=TRUE;
 if(SUCCEEDED(result))result=IUIAutomationElement_get_CurrentHasKeyboardFocus(field,&focused);
 if(SUCCEEDED(result) && focused){fputs("Lifecycle retained a hidden focus\n",stderr);result=E_FAIL;}
 BOOL active=TRUE;IUIAutomationTextRange *caret=NULL;
 if(SUCCEEDED(result))result=IUIAutomationTextPattern2_GetCaretRange(pattern,&active,&caret);
 if(SUCCEEDED(result) && active)result=E_FAIL;
 if(caret)IUIAutomationTextRange_Release(caret);
 if(SUCCEEDED(result))result=lifecycle_views(document,whole,false);
 if(SUCCEEDED(result))result=lifecycle_views(field_text,field_range,false);
 if(SUCCEEDED(result))result=lifecycle_text(field_range,L"θ🌍");
 if(SUCCEEDED(result) && SUCCEEDED(IUIAutomationElement_SetFocus(link))){fputs("Lifecycle accepted hidden link focus\n",stderr);result=E_FAIL;}
 IUnknown *unknown=NULL;IUIAutomationInvokePattern *invoke=NULL;
 if(SUCCEEDED(result))result=IUIAutomationElement_GetCurrentPattern(link,UIA_InvokePatternId,&unknown);
 if(SUCCEEDED(result) && unknown)result=IUnknown_QueryInterface(unknown,&IID_IUIAutomationInvokePattern,(void**)&invoke);else result=E_FAIL;
 if(SUCCEEDED(result) && SUCCEEDED(IUIAutomationInvokePattern_Invoke(invoke)))result=E_FAIL;
 if(invoke)IUIAutomationInvokePattern_Release(invoke);if(unknown)IUnknown_Release(unknown);
 if(SUCCEEDED(result) && events_count(&events->invoked))result=E_FAIL;
 if(SUCCEEDED(result))result=events_wait(&events->hidden,hidden);return result;
}
static HRESULT document_lifecycle(IUIAutomation *a,IUIAutomationElement *control,const wchar_t *directory) {
 IUIAutomationElement *field=NULL,*link=NULL;IUnknown *unknown=NULL;
 IUIAutomationTextPattern *document=NULL,*field_text=NULL;IUIAutomationTextPattern2 *field2=NULL;
 IUIAutomationValuePattern *value=NULL;IUIAutomationTextRange *whole=NULL,*field_range=NULL;
 BSTR original=NULL;link_events *events=NULL;
 HRESULT result=named(a,control,L"Aktuelles Dokument durchsuchen",UIA_EditControlTypeId,&field);
 if(SUCCEEDED(result) && !field)result=E_FAIL;
 if(SUCCEEDED(result))result=IUIAutomationElement_GetCurrentPattern(field,UIA_ValuePatternId,&unknown);
 if(SUCCEEDED(result) && unknown)result=IUnknown_QueryInterface(unknown,&IID_IUIAutomationValuePattern,(void**)&value);else result=E_FAIL;
 if(unknown)IUnknown_Release(unknown);unknown=NULL;
 if(SUCCEEDED(result))result=IUIAutomationValuePattern_SetValue(value,L"θ🌍");
 if(SUCCEEDED(result))result=IUIAutomationElement_SetFocus(field);
 ULONGLONG deadline=GetTickCount64()+10000;
 while(SUCCEEDED(result)) {BSTR text=NULL;BOOL focused=FALSE;result=IUIAutomationValuePattern_get_CurrentValue(value,&text);
  if(SUCCEEDED(result))result=IUIAutomationElement_get_CurrentHasKeyboardFocus(field,&focused);
  bool ready=text && !wcscmp(text,L"θ🌍") && focused;SysFreeString(text);if(FAILED(result) || ready)break;
  if(GetTickCount64()>=deadline){result=E_FAIL;break;}Sleep(20);
 }
 if(SUCCEEDED(result))result=IUIAutomationElement_GetCurrentPattern(control,UIA_TextPatternId,&unknown);
 if(SUCCEEDED(result) && unknown)result=IUnknown_QueryInterface(unknown,&IID_IUIAutomationTextPattern,(void**)&document);else result=E_FAIL;
 if(unknown)IUnknown_Release(unknown);unknown=NULL;
 if(SUCCEEDED(result))result=IUIAutomationTextPattern_get_DocumentRange(document,&whole);
 if(SUCCEEDED(result))result=IUIAutomationTextRange_GetText(whole,-1,&original);
 if(SUCCEEDED(result))result=IUIAutomationElement_GetCurrentPattern(field,UIA_TextPattern2Id,&unknown);
 if(SUCCEEDED(result) && unknown)result=IUnknown_QueryInterface(unknown,&IID_IUIAutomationTextPattern2,(void**)&field2);else result=E_FAIL;
 if(unknown)IUnknown_Release(unknown);unknown=NULL;
 if(SUCCEEDED(result))result=IUIAutomationTextPattern2_QueryInterface(field2,&IID_IUIAutomationTextPattern,(void**)&field_text);
 if(SUCCEEDED(result))result=IUIAutomationTextPattern_get_DocumentRange(field_text,&field_range);
 if(SUCCEEDED(result))result=named(a,control,L"C-Lernweg",UIA_HyperlinkControlTypeId,&link);
 if(SUCCEEDED(result) && !link)result=E_FAIL;
 if(SUCCEEDED(result))result=events_start(a,link,&events);
 if(SUCCEEDED(result))result=lifecycle_views(document,whole,true);
 if(SUCCEEDED(result))result=lifecycle_views(field_text,field_range,true);
 UIA_HWND native=NULL;if(SUCCEEDED(result))result=IUIAutomationElement_get_CurrentNativeWindowHandle(control,&native);
 HWND host=(HWND)native;if(SUCCEEDED(result) && !IsWindow(host))result=E_FAIL;
 if(SUCCEEDED(result))result=lifecycle_file(directory,L"close-request",false);
 if(SUCCEEDED(result))result=lifecycle_hidden(field,field2,document,whole,field_range,field_text,link,events,1,host);
 if(SUCCEEDED(result))result=lifecycle_text(whole,original);
 if(SUCCEEDED(result))result=lifecycle_file(directory,L"hidden-checked",false);
 if(SUCCEEDED(result))result=events_wait(&events->offscreen,1);
 if(SUCCEEDED(result))result=lifecycle_unfocused(field,field2);
 if(SUCCEEDED(result))result=lifecycle_views(document,whole,true);
 if(SUCCEEDED(result))result=lifecycle_views(field_text,field_range,true);
 if(SUCCEEDED(result))result=lifecycle_text(whole,original);
 if(SUCCEEDED(result))result=lifecycle_text(field_range,L"θ🌍");
 if(SUCCEEDED(result))result=lifecycle_file(directory,L"reopen-checked",false);
 if(SUCCEEDED(result))result=lifecycle_file(directory,L"reopen-app-captured",true);
 if(SUCCEEDED(result)){ShowWindow(host,SW_MINIMIZE);result=lifecycle_hidden(field,field2,document,whole,field_range,field_text,link,events,2,host);}
 if(SUCCEEDED(result))result=lifecycle_file(directory,L"minimized-checked",false);
 if(SUCCEEDED(result))result=lifecycle_file(directory,L"minimized-app-checked",true);
 if(host && IsWindow(host))ShowWindow(host,SW_RESTORE);
 if(SUCCEEDED(result))result=events_wait(&events->offscreen,2);
 if(SUCCEEDED(result))result=lifecycle_unfocused(field,field2);
 if(SUCCEEDED(result))result=lifecycle_views(document,whole,true);
 if(SUCCEEDED(result))result=lifecycle_views(field_text,field_range,true);
 if(SUCCEEDED(result))result=lifecycle_text(whole,original);
 if(SUCCEEDED(result))result=lifecycle_text(field_range,L"θ🌍");
 if(SUCCEEDED(result) && (events_count(&events->bad) || events_count(&events->invoked)))result=E_FAIL;
 if(SUCCEEDED(result))result=lifecycle_file(directory,L"restored-checked",false);
 if(events){printf("WINDOWS LIFECYCLE EVENTS: hidden=%ld shown=%ld invoked=%ld bad=%ld\n",events_count(&events->hidden),events_count(&events->offscreen),events_count(&events->invoked),events_count(&events->bad));events_stop(a,link,events);}
 SysFreeString(original);if(field_range)IUIAutomationTextRange_Release(field_range);if(whole)IUIAutomationTextRange_Release(whole);
 if(value)IUIAutomationValuePattern_Release(value);if(field_text)IUIAutomationTextPattern_Release(field_text);if(field2)IUIAutomationTextPattern2_Release(field2);
 if(document)IUIAutomationTextPattern_Release(document);if(link)IUIAutomationElement_Release(link);if(field)IUIAutomationElement_Release(field);
 if(SUCCEEDED(result))puts("DOCUMENT LIFECYCLE: hidden/minimized views and focus cleared, restored text and identities preserved");return result;
}
