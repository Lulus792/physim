typedef struct {HWND hwnd;SDL_AtomicInt done;bool passed;} uia_client_probe;
static int uia_client_test_thread(void *userdata) {
 uia_client_probe *probe=userdata;HRESULT initialized=CoInitializeEx(NULL,COINIT_MULTITHREADED);
 IUIAutomation *automation=NULL;IUIAutomationElement *root=NULL,*button=NULL;
 IUIAutomationCondition *condition=NULL;IUnknown *unknown=NULL;IUIAutomationInvokePattern *invoke=NULL;
 bool ok=SUCCEEDED(initialized) && SUCCEEDED(CoCreateInstance(&CLSID_CUIAutomation,NULL,CLSCTX_INPROC_SERVER,&IID_IUIAutomation,(void**)&automation));
 if(ok)ok=SUCCEEDED(IUIAutomation_ElementFromHandle(automation,probe->hwnd,&root));
 VARIANT name;VariantInit(&name);name.vt=VT_BSTR;name.bstrVal=SysAllocString(L"Öffnen …");
 if(ok)ok=name.bstrVal && SUCCEEDED(IUIAutomation_CreatePropertyCondition(automation,UIA_NamePropertyId,name,&condition));
 if(ok)ok=SUCCEEDED(IUIAutomationElement_FindFirst(root,TreeScope_Descendants,condition,&button)) && button;
 BSTR label=NULL;CONTROLTYPEID role=0;
 if(ok)ok=SUCCEEDED(IUIAutomationElement_get_CurrentName(button,&label)) && label && !wcscmp(label,L"Öffnen …") &&
  SUCCEEDED(IUIAutomationElement_get_CurrentControlType(button,&role)) && role==UIA_ButtonControlTypeId;
 if(ok)ok=SUCCEEDED(IUIAutomationElement_GetCurrentPattern(button,UIA_InvokePatternId,&unknown)) && unknown &&
  SUCCEEDED(IUnknown_QueryInterface(unknown,&IID_IUIAutomationInvokePattern,(void**)&invoke)) &&
  SUCCEEDED(IUIAutomationInvokePattern_Invoke(invoke));
 SysFreeString(label);VariantClear(&name);
 if(invoke)IUIAutomationInvokePattern_Release(invoke);if(unknown)IUnknown_Release(unknown);
 if(condition)IUIAutomationCondition_Release(condition);if(button)IUIAutomationElement_Release(button);
 if(root)IUIAutomationElement_Release(root);if(automation)IUIAutomation_Release(automation);
 if(SUCCEEDED(initialized))CoUninitialize();probe->passed=ok;SDL_SetAtomicInt(&probe->done,1);return ok?0:1;
}
bool ps_a11y_native_test(SDL_Window *window) {
 ps_a11y_model *m=malloc(sizeof *m);SDL_Mutex *mutex=SDL_CreateMutex();
 if(!m || !mutex){free(m);if(mutex)SDL_DestroyMutex(mutex);return false;}
 ps_a11y_init(m);ps_a11y_native *b=ps_a11y_native_create(window,m,mutex);
 if(!b){SDL_DestroyMutex(mutex);free(m);return false;}
 bool ok=true;float bounds[]={10,20,120,24};ps_a11y_begin(m);
 ps_a11y_record(m,"main","Öffnen …",PS_A11Y_BUTTON,bounds,true);ps_a11y_publish(m);ps_a11y_native_publish(b,true);
 uint64_t button_id=m->nodes[0].id;uia_element *button=uia_named(b,NULL,"Öffnen …");
#define UIA_CHECK(test) do {if(!(test)){fprintf(stderr,"Windows UIA %d: %s\n",__LINE__,#test);ok=false;}}while(0)
 UIA_CHECK(button!=NULL);
 uia_client_probe client={.hwnd=b->hwnd};SDL_Thread *thread=SDL_CreateThread(uia_client_test_thread,"uia-client-probe",&client);
 UIA_CHECK(thread!=NULL);Uint64 deadline=SDL_GetTicks()+8000;
 while(thread && !SDL_GetAtomicInt(&client.done) && SDL_GetTicks()<deadline){SDL_PumpEvents();SDL_Delay(1);}
 if(thread){UIA_CHECK(SDL_GetAtomicInt(&client.done));SDL_WaitThread(thread,NULL);UIA_CHECK(client.passed);}
 UIA_CHECK(m->pending_press==button_id);
 ps_a11y_begin(m);UIA_CHECK(ps_a11y_record(m,"main","Öffnen …",PS_A11Y_BUTTON,bounds,true));ps_a11y_publish(m);ps_a11y_native_publish(b,true);
 ps_a11y_begin(m);UIA_CHECK(!ps_a11y_record(m,"main","Öffnen …",PS_A11Y_BUTTON,bounds,true));ps_a11y_publish(m);ps_a11y_native_publish(b,true);
 if(button) {
  VARIANT value;UIA_CHECK(SUCCEEDED(IRawElementProviderSimple_GetPropertyValue(&button->simple,UIA_NamePropertyId,&value)) && value.vt==VT_BSTR && !wcscmp(value.bstrVal,L"Öffnen …"));VariantClear(&value);
  struct UiaRect rect;UIA_CHECK(SUCCEEDED(IRawElementProviderFragment_get_BoundingRectangle(&button->fragment,&rect)) && rect.width>0 && rect.height>0);
  SAFEARRAY *runtime=NULL;UIA_CHECK(SUCCEEDED(IRawElementProviderFragment_GetRuntimeId(&button->fragment,&runtime)) && runtime);if(runtime)SafeArrayDestroy(runtime);
 }
 ps_a11y_begin(m);ps_a11y_record_state(m,"main","Vektoren",PS_A11Y_CHECKBOX,bounds,true,false);ps_a11y_publish(m);ps_a11y_native_publish(b,true);
 uia_element *checkbox=uia_named(b,NULL,"Vektoren");UIA_CHECK(checkbox && SUCCEEDED(IToggleProvider_Toggle(&checkbox->toggle)));
 ps_a11y_begin(m);UIA_CHECK(ps_a11y_record_state(m,"main","Vektoren",PS_A11Y_CHECKBOX,bounds,true,false));ps_a11y_publish(m);ps_a11y_native_publish(b,true);
 enum ToggleState state;UIA_CHECK(checkbox && SUCCEEDED(IToggleProvider_get_ToggleState(&checkbox->toggle,&state)) && state==ToggleState_On);
 if(button)UIA_CHECK(IInvokeProvider_Invoke(&button->invoke)==UIA_E_ELEMENTNOTAVAILABLE);
 ps_a11y_begin(m);ps_a11y_record_state(m,"main","Vektoren",PS_A11Y_CHECKBOX,bounds,false,true);ps_a11y_publish(m);ps_a11y_native_publish(b,true);
 if(checkbox)UIA_CHECK(IToggleProvider_Toggle(&checkbox->toggle)==UIA_E_ELEMENTNOTENABLED);
 ps_a11y_begin(m);ps_a11y_record_option(m,"main","Theme","Dark",bounds,true,true,false);bounds[0]=140;
 ps_a11y_record_option(m,"main","Theme","Light",bounds,true,false,false);ps_a11y_publish(m);ps_a11y_native_publish(b,true);
 uia_element *group=uia_named(b,NULL,"Theme"),*radio=uia_named(b,"Theme","Light");
 UIA_CHECK(group && radio && SUCCEEDED(ISelectionItemProvider_Select(&radio->item)));
 ps_a11y_begin(m);bounds[0]=10;ps_a11y_record_option(m,"main","Theme","Dark",bounds,true,true,false);bounds[0]=140;
 UIA_CHECK(ps_a11y_record_option(m,"main","Theme","Light",bounds,true,false,false));ps_a11y_publish(m);ps_a11y_native_publish(b,true);
 BOOL selected=FALSE;UIA_CHECK(radio && SUCCEEDED(ISelectionItemProvider_get_IsSelected(&radio->item,&selected)) && selected);
 SAFEARRAY *selection=NULL;UIA_CHECK(group && SUCCEEDED(ISelectionProvider_GetSelection(&group->selection,&selection)) && selection);
 if(selection){LONG lower,upper;UIA_CHECK(SUCCEEDED(SafeArrayGetLBound(selection,1,&lower)) && SUCCEEDED(SafeArrayGetUBound(selection,1,&upper)) && upper-lower==0);SafeArrayDestroy(selection);}
 double scalar=.5;ps_a11y_begin(m);ps_a11y_record_range(m,"main","Winkel θ",bounds,true,-1,1,.25,&scalar);ps_a11y_publish(m);ps_a11y_native_publish(b,true);
 uia_element *slider=uia_named(b,NULL,"Winkel θ");UIA_CHECK(slider && SUCCEEDED(IRangeValueProvider_SetValue(&slider->range,.75)));
 ps_a11y_begin(m);UIA_CHECK(ps_a11y_record_range(m,"main","Winkel θ",bounds,true,-1,1,.25,&scalar));ps_a11y_publish(m);ps_a11y_native_publish(b,true);
 UIA_CHECK(scalar==.75);if(slider)UIA_CHECK(IRangeValueProvider_SetValue(&slider->range,2)==E_INVALIDARG);
 char field[32]="Initial θ";ps_a11y_begin(m);
 ps_a11y_record_edit(m,"main","Suche",bounds,true,field,sizeof field);ps_a11y_publish(m);ps_a11y_native_publish(b,true);
 uia_element *edit=uia_named(b,NULL,"Suche");UIA_CHECK(edit!=NULL);BSTR field_value=NULL;
 if(edit){UIA_CHECK(SUCCEEDED(IValueProvider_get_Value(&edit->value,&field_value)) && !wcscmp(field_value,L"Initial θ"));SysFreeString(field_value);
  UIA_CHECK(SUCCEEDED(IValueProvider_SetValue(&edit->value,L"Größe 🌍")));}
 ps_a11y_begin(m);UIA_CHECK(ps_a11y_record_edit(m,"main","Suche",bounds,true,field,sizeof field));ps_a11y_publish(m);ps_a11y_native_publish(b,true);
 UIA_CHECK(!strcmp(field,"Größe 🌍"));if(edit){UIA_CHECK(IValueProvider_SetValue(&edit->value,L"line\nline")==E_INVALIDARG);
  UIA_CHECK(IValueProvider_SetValue(&edit->value,L"123456789012345678901234567890123456")==E_INVALIDARG);}
 ITextRangeProvider *field_range=NULL,*field_match=NULL;BSTR field_query=SysAllocString(L"🌍");
 if(edit){enum SupportedTextSelection supported=SupportedTextSelection_None;
  UIA_CHECK(SUCCEEDED(ITextProvider_get_SupportedTextSelection(&edit->text,&supported)) && supported==SupportedTextSelection_Single);
  UIA_CHECK(SUCCEEDED(ITextProvider_get_DocumentRange(&edit->text,&field_range)) && field_range);
 }
 if(field_range){field_value=NULL;UIA_CHECK(SUCCEEDED(ITextRangeProvider_GetText(field_range,-1,&field_value)) && !wcscmp(field_value,L"Größe 🌍"));SysFreeString(field_value);
  UIA_CHECK(SUCCEEDED(ITextRangeProvider_FindText(field_range,field_query,FALSE,FALSE,&field_match)) && field_match);}
 SysFreeString(field_query);
 if(field_match){UIA_CHECK(SUCCEEDED(ITextRangeProvider_Select(field_match)));
  ps_a11y_begin(m);ps_a11y_record_edit(m,"main","Suche",bounds,true,field,sizeof field);size_t start=0,end=0;
  UIA_CHECK(ps_a11y_edit_take_selection(m,0,&start,&end) && start==8 && end==12);UIA_CHECK(ps_a11y_edit_update(m,0,field,start,end));
  ps_a11y_publish(m);ps_a11y_native_publish(b,true);
  SAFEARRAY *ranges=NULL;UIA_CHECK(SUCCEEDED(ITextProvider_GetSelection(&edit->text,&ranges)) && ranges);
  if(ranges){ITextRangeProvider *selected_range=NULL;LONG zero=0;UIA_CHECK(SUCCEEDED(SafeArrayGetElement(ranges,&zero,&selected_range)) && selected_range);
   if(selected_range){field_value=NULL;UIA_CHECK(SUCCEEDED(ITextRangeProvider_GetText(selected_range,-1,&field_value)) && !wcscmp(field_value,L"🌍"));SysFreeString(field_value);ITextRangeProvider_Release(selected_range);}SafeArrayDestroy(ranges);}
  uint64_t focus=m->focused_id;UIA_CHECK(SUCCEEDED(ITextRangeProvider_ScrollIntoView(field_match,TRUE)));
  ps_a11y_begin(m);ps_a11y_record_edit(m,"main","Suche",bounds,true,field,sizeof field);bool top=false;
  UIA_CHECK(ps_a11y_edit_take_scroll(m,0,&start,&end,&top) && start==8 && end==12 && top);
  ps_a11y_publish(m);ps_a11y_native_publish(b,true);
  UIA_CHECK(m->focused_id==focus && m->nodes[0].text_start==8 && m->nodes[0].text_end==12 && !strcmp(field,"Größe 🌍"));
 }
 /* Read the whole document through COM ranges, independently of control labels. */
 const char *reading="Intro θ\nCode\n  x = 1; 🌍\nEnd";
 ps_a11y_begin(m);bounds[0]=10;bounds[1]=20;
 /* Equal version numbers across different owners must not share cached text. */
 uint64_t document_version=m->nodes[0].text_version;
 UIA_CHECK(ps_a11y_document_begin(m,document_version,"Dokument",reading,bounds));
 ps_a11y_record_edit(m,"main","Suche",bounds,true,field,sizeof field);
 UIA_CHECK(ps_a11y_document_box(m,6,8,bounds));
 ps_a11y_publish(m);ps_a11y_native_publish(b,true);
 ITextRangeProvider *whole=NULL,*clone=NULL,*found=NULL;BSTR read=NULL;
 UIA_CHECK(SUCCEEDED(ITextProvider_get_DocumentRange(&b->root->text,&whole)) && whole);
 if(whole)UIA_CHECK(SUCCEEDED(ITextRangeProvider_GetText(whole,-1,&read)) && read && !wcscmp(read,L"Intro θ\nCode\n  x = 1; 🌍\nEnd"));
 if(field_range){field_value=NULL;UIA_CHECK(SUCCEEDED(ITextRangeProvider_GetText(field_range,-1,&field_value)) && !wcscmp(field_value,L"Größe 🌍"));SysFreeString(field_value);}
 if(whole){BSTR again=NULL;UIA_CHECK(SUCCEEDED(ITextRangeProvider_GetText(whole,-1,&again)) && !wcscmp(again,L"Intro θ\nCode\n  x = 1; 🌍\nEnd"));SysFreeString(again);}
 SysFreeString(read);read=NULL;
 if(whole)UIA_CHECK(SUCCEEDED(ITextRangeProvider_Clone(whole,&clone)) && clone);
 BOOL equal=FALSE;if(clone)UIA_CHECK(SUCCEEDED(ITextRangeProvider_Compare(whole,clone,&equal)) && equal);
 if(clone){UIA_CHECK(SUCCEEDED(ITextRangeProvider_ExpandToEnclosingUnit(clone,TextUnit_Format)));
  UIA_CHECK(SUCCEEDED(ITextRangeProvider_GetText(clone,-1,&read)) && read && !wcscmp(read,L"Intro"));SysFreeString(read);read=NULL;}
 BSTR query=SysAllocString(L"Θ");
 if(whole)UIA_CHECK(SUCCEEDED(ITextRangeProvider_FindText(whole,query,FALSE,TRUE,&found)) && found);
 SysFreeString(query);
 if(found) {
  UIA_CHECK(SUCCEEDED(ITextRangeProvider_GetText(found,-1,&read)) && read && !wcscmp(read,L"θ"));SysFreeString(read);read=NULL;
  SAFEARRAY *rectangles=NULL;UIA_CHECK(SUCCEEDED(ITextRangeProvider_GetBoundingRectangles(found,&rectangles)) && rectangles);
  if(rectangles){LONG high=-1;SafeArrayGetUBound(rectangles,1,&high);UIA_CHECK(high==3);SafeArrayDestroy(rectangles);}
  UIA_CHECK(SUCCEEDED(ITextRangeProvider_ScrollIntoView(found,TRUE)));
  size_t target=99;UIA_CHECK(ps_a11y_document_take_scroll(m,document_version,&target) && target==6);
  UIA_CHECK(SUCCEEDED(ITextRangeProvider_ScrollIntoView(found,FALSE)) && !m->document_scroll_top);
  UIA_CHECK(ps_a11y_document_take_scroll(m,document_version,&target) && target==8);
  ITextRangeProvider_Release(found);found=NULL;
 }
 query=SysAllocString(L"🌍");
 if(whole)UIA_CHECK(SUCCEEDED(ITextRangeProvider_FindText(whole,query,FALSE,FALSE,&found)) && found);SysFreeString(query);
 if(found){int moved=0;UIA_CHECK(SUCCEEDED(ITextRangeProvider_MoveEndpointByUnit(found,TextPatternRangeEndpoint_Start,TextUnit_Character,1,&moved)) && moved==1);
  UIA_CHECK(SUCCEEDED(ITextRangeProvider_GetText(found,-1,&read)) && read && !SysStringLen(read));SysFreeString(read);read=NULL;ITextRangeProvider_Release(found);found=NULL;}
 SAFEARRAY *visible=NULL;UIA_CHECK(SUCCEEDED(ITextProvider_GetVisibleRanges(&b->root->text,&visible)) && visible);if(visible)SafeArrayDestroy(visible);
 ps_a11y_begin(m);UIA_CHECK(ps_a11y_document_begin(m,43,"Anders","New content",bounds));ps_a11y_publish(m);ps_a11y_native_publish(b,true);
 if(whole)UIA_CHECK(ITextRangeProvider_GetText(whole,-1,&read)==UIA_E_ELEMENTNOTAVAILABLE && !read);
 if(field_range){field_value=NULL;UIA_CHECK(ITextRangeProvider_GetText(field_range,-1,&field_value)==UIA_E_ELEMENTNOTAVAILABLE && !field_value);ITextRangeProvider_Release(field_range);}
 if(field_match)ITextRangeProvider_Release(field_match);
 ps_a11y_native_destroy(b);
 if(edit){UIA_CHECK(IValueProvider_SetValue(&edit->value,L"stale")==UIA_E_ELEMENTNOTAVAILABLE);uia_release(edit);}
 if(clone){UIA_CHECK(ITextRangeProvider_GetText(clone,-1,&read)==UIA_E_ELEMENTNOTAVAILABLE && !read);ITextRangeProvider_Release(clone);}
 if(whole)ITextRangeProvider_Release(whole);
 if(button){UIA_CHECK(IInvokeProvider_Invoke(&button->invoke)==UIA_E_ELEMENTNOTAVAILABLE);uia_release(button);}
 if(checkbox){UIA_CHECK(IToggleProvider_Toggle(&checkbox->toggle)==UIA_E_ELEMENTNOTAVAILABLE);uia_release(checkbox);}
 if(radio){UIA_CHECK(ISelectionItemProvider_Select(&radio->item)==UIA_E_ELEMENTNOTAVAILABLE);uia_release(radio);}
 if(group){selection=NULL;UIA_CHECK(ISelectionProvider_GetSelection(&group->selection,&selection)==UIA_E_ELEMENTNOTAVAILABLE);uia_release(group);}
 if(slider){UIA_CHECK(IRangeValueProvider_SetValue(&slider->range,.5)==UIA_E_ELEMENTNOTAVAILABLE);uia_release(slider);}
 ps_a11y_destroy(m);free(m);
#undef UIA_CHECK
 return ok;
}
