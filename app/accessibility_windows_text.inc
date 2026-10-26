/* Document and single-line edit TextPattern. UTF-8 geometry uses the same glyph advances
 * as the renderer; UTF-16 offsets are translated only at the Windows boundary. */
typedef struct {
 ITextRangeProvider range_iface;LONG references;uia_element *owner;
 uint64_t version,enclosing_hint;size_t start,end;
} uia_text_range;
static uia_text_range *uia_text_range_new(uia_element *owner,uint64_t version,size_t start,size_t end);
static HRESULT uia_reading_locked(uia_element *owner) {
 ps_a11y_native *b=owner->bridge;
 const ps_a11y_document *d=b->model?b->model->document:NULL;
 const ps_a11y_node *node=b->model && owner->id?ps_a11y_find(b->model,owner->id):NULL;
 if(owner->id?(!node || node->role!=PS_A11Y_EDIT):(!d || !d->active))return UIA_E_ELEMENTNOTAVAILABLE;
 const char *source=node?node->text_value:d->text;size_t length=node?strlen(source):d->length;uint64_t version=node?node->text_version:d->version;
 if(b->reading_text && b->reading_owner==owner->id && b->reading_version==version)return S_OK;
 int n=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,source,-1,NULL,0);if(!n)return E_INVALIDARG;
 BSTR text=SysAllocStringLen(NULL,(UINT)(n-1));size_t *bytes=malloc((size_t)n*sizeof *bytes);
 if(!text || !bytes){SysFreeString(text);free(bytes);return E_OUTOFMEMORY;}
 if(!MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,source,-1,text,n)){SysFreeString(text);free(bytes);return E_INVALIDARG;}
 size_t at=0,unit=0;
 while(at<length) {
  unsigned char c=(unsigned char)source[at];size_t count=c>=0xf0?4:c>=0xe0?3:c>=0xc0?2:1;
  bytes[unit++]=at;if(count==4)bytes[unit++]=at;at+=count;
 }
 bytes[unit]=at;
 SysFreeString(b->reading_text);free(b->reading_bytes);b->reading_text=text;b->reading_bytes=bytes;
 b->reading_length=unit;b->reading_version=version;b->reading_owner=owner->id;return S_OK;
}
static const ps_a11y_text_box *uia_boxes_locked(uia_element *owner,size_t *count) {
 ps_a11y_native *b=owner->bridge;if(!uia_window_viewable_locked(b)){*count=0;return NULL;}
 if(!owner->id){*count=b->model->document->count;return b->model->document->boxes;}
 const ps_a11y_node *node=ps_a11y_find(b->model,owner->id);*count=node->box_count;return b->model->edit_boxes+node->box_offset;
}
static size_t uia_utf16_locked(ps_a11y_native *b,size_t byte) {
 size_t lo=0,hi=b->reading_length;
 while(lo<hi){size_t mid=lo+(hi-lo)/2;if(b->reading_bytes[mid]<byte)lo=mid+1;else hi=mid;}return lo;
}
static uia_text_range *uia_range_owner(ITextRangeProvider *p){return (uia_text_range*)p;}
static HRESULT uia_text_range_live_locked(uia_text_range *r) {
 ps_a11y_native *b=r->owner->bridge;HRESULT result=uia_reading_locked(r->owner);
 return FAILED(result)?result:b->reading_version!=r->version?UIA_E_ELEMENTNOTAVAILABLE:S_OK;
}
static HRESULT STDMETHODCALLTYPE uia_tr_query(ITextRangeProvider *p,REFIID iid,void **out) {
 if(!out)return E_POINTER;*out=NULL;
 if(!IsEqualIID(iid,&IID_IUnknown) && !IsEqualIID(iid,&IID_ITextRangeProvider))return E_NOINTERFACE;
 *out=p;InterlockedIncrement(&uia_range_owner(p)->references);return S_OK;
}
static ULONG STDMETHODCALLTYPE uia_tr_addref(ITextRangeProvider *p){return (ULONG)InterlockedIncrement(&uia_range_owner(p)->references);}
static ULONG STDMETHODCALLTYPE uia_tr_release(ITextRangeProvider *p) {
 uia_text_range *r=uia_range_owner(p);LONG refs=InterlockedDecrement(&r->references);
 if(!refs){uia_release(r->owner);free(r);}return (ULONG)refs;
}
static HRESULT STDMETHODCALLTYPE uia_tr_clone(ITextRangeProvider *p,ITextRangeProvider **out) {
 if(!out)return E_POINTER;*out=NULL;uia_text_range *r=uia_range_owner(p);ps_a11y_native *b=r->owner->bridge;
 SDL_LockMutex(b->mutex);HRESULT result=uia_text_range_live_locked(r);
 uia_text_range *copy=SUCCEEDED(result)?uia_text_range_new(r->owner,r->version,r->start,r->end):NULL;
 if(copy)copy->enclosing_hint=r->enclosing_hint;SDL_UnlockMutex(b->mutex);
 if(FAILED(result))return result;if(!copy)return E_OUTOFMEMORY;*out=&copy->range_iface;return S_OK;
}
static bool uia_same_text_range(ITextRangeProvider *p,ITextRangeProvider *other) {
 return other && other->lpVtbl==p->lpVtbl && uia_range_owner(p)->owner==uia_range_owner(other)->owner &&
  uia_range_owner(p)->version==uia_range_owner(other)->version;
}
static bool uia_endpoint(enum TextPatternRangeEndpoint e){return e==TextPatternRangeEndpoint_Start || e==TextPatternRangeEndpoint_End;}
static size_t uia_endpoint_value(uia_text_range *r,enum TextPatternRangeEndpoint e){return e==TextPatternRangeEndpoint_Start?r->start:r->end;}
static HRESULT STDMETHODCALLTYPE uia_tr_compare(ITextRangeProvider *p,ITextRangeProvider *other,BOOL *out) {
 if(!out)return E_POINTER;*out=FALSE;uia_text_range *r=uia_range_owner(p);ps_a11y_native *b=r->owner->bridge;
 SDL_LockMutex(b->mutex);HRESULT result=uia_text_range_live_locked(r);
 if(SUCCEEDED(result) && uia_same_text_range(p,other)){uia_text_range *s=uia_range_owner(other);*out=r->start==s->start && r->end==s->end;}
 SDL_UnlockMutex(b->mutex);return result;
}
static HRESULT STDMETHODCALLTYPE uia_tr_compare_endpoints(ITextRangeProvider *p,enum TextPatternRangeEndpoint endpoint,
 ITextRangeProvider *other,enum TextPatternRangeEndpoint target,int *out) {
 if(!out)return E_POINTER;if(!uia_endpoint(endpoint) || !uia_endpoint(target) || !uia_same_text_range(p,other))return E_INVALIDARG;
 uia_text_range *r=uia_range_owner(p);ps_a11y_native *b=r->owner->bridge;SDL_LockMutex(b->mutex);
 HRESULT result=uia_text_range_live_locked(r);if(SUCCEEDED(result))*out=(int)uia_endpoint_value(r,endpoint)-(int)uia_endpoint_value(uia_range_owner(other),target);
 SDL_UnlockMutex(b->mutex);return result;
}
static size_t uia_character_next(ps_a11y_native *b,size_t at) {
 if(at>=b->reading_length)return b->reading_length;wchar_t c=b->reading_text[at++];
 if(c>=0xd800 && c<=0xdbff && at<b->reading_length)++at;
 while(at<b->reading_length){WORD type=0;if(!GetStringTypeW(CT_CTYPE3,b->reading_text+at,1,&type) || !(type&(C3_NONSPACING|C3_DIACRITIC)))break;++at;}
 return at;
}
static size_t uia_character_previous(ps_a11y_native *b,size_t at) {
 if(!at)return 0;--at;
 while(at){WORD type=0;GetStringTypeW(CT_CTYPE3,b->reading_text+at,1,&type);if(!(type&(C3_NONSPACING|C3_DIACRITIC)))break;--at;}
 if(b->reading_text[at]>=0xdc00 && b->reading_text[at]<=0xdfff && at)--at;return at;
}
static bool uia_word_space(wchar_t c){WORD type=0;GetStringTypeW(CT_CTYPE1,&c,1,&type);return (type&(C1_SPACE|C1_PUNCT))!=0;}
static enum TextUnit uia_supported_unit(enum TextUnit unit) {
 /* Unsupported formatting, visual-line and page units promote to the next
  * larger supported unit in the TextPattern hierarchy. */
 return unit==TextUnit_Character || unit==TextUnit_Word || unit==TextUnit_Paragraph || unit==TextUnit_Document?
  unit:unit==TextUnit_Format?TextUnit_Word:unit==TextUnit_Line?TextUnit_Paragraph:unit==TextUnit_Page?TextUnit_Document:(enum TextUnit)-1;
}
static size_t uia_unit_start(ps_a11y_native *b,size_t at,enum TextUnit unit) {
 if(unit==TextUnit_Document)return 0;
 if(at==b->reading_length && at)at=uia_character_previous(b,at);
 if(unit==TextUnit_Character){
  while(at<b->reading_length && at){WORD type=0;GetStringTypeW(CT_CTYPE3,b->reading_text+at,1,&type);
   if(!(type&(C3_NONSPACING|C3_DIACRITIC)))break;at=uia_character_previous(b,at);}
  if(at && at<b->reading_length && b->reading_text[at]>=0xdc00 && b->reading_text[at]<=0xdfff)--at;return at;
 }
 if(unit==TextUnit_Word) {
  bool space=at<b->reading_length && uia_word_space(b->reading_text[at]);
  while(at){size_t previous=uia_character_previous(b,at);if(uia_word_space(b->reading_text[previous])!=space)break;at=previous;}return at;
 }
 while(at && b->reading_text[at-1]!='\n')--at;return at;
}
static size_t uia_unit_end(ps_a11y_native *b,size_t at,enum TextUnit unit) {
 if(unit==TextUnit_Document)return b->reading_length;
 if(unit==TextUnit_Character)return uia_character_next(b,at);
 if(unit==TextUnit_Word) {
  bool space=at<b->reading_length && uia_word_space(b->reading_text[at]);
  while(at<b->reading_length && uia_word_space(b->reading_text[at])==space)at=uia_character_next(b,at);
  return at;
 }
 while(at<b->reading_length && b->reading_text[at]!='\n')at=uia_character_next(b,at);
 return at<b->reading_length?at+1:at;
}
static HRESULT STDMETHODCALLTYPE uia_tr_expand(ITextRangeProvider *p,enum TextUnit unit) {
 unit=uia_supported_unit(unit);if((int)unit<0)return E_INVALIDARG;
 uia_text_range *r=uia_range_owner(p);ps_a11y_native *b=r->owner->bridge;SDL_LockMutex(b->mutex);HRESULT result=uia_text_range_live_locked(r);
 if(SUCCEEDED(result)){r->enclosing_hint=0;r->start=uia_unit_start(b,r->start,unit);r->end=uia_unit_end(b,r->start,unit);}SDL_UnlockMutex(b->mutex);return result;
}
static HRESULT STDMETHODCALLTYPE uia_tr_get_attribute(ITextRangeProvider *p,TEXTATTRIBUTEID id,VARIANT *out) {
 if(!out)return E_POINTER;VariantInit(out);uia_text_range *r=uia_range_owner(p);ps_a11y_native *b=r->owner->bridge;
 SDL_LockMutex(b->mutex);HRESULT result=uia_text_range_live_locked(r);
 const ps_a11y_node *node=b->model?ps_a11y_find(b->model,r->owner->id):NULL;bool readonly=!node || !node->enabled || node->text_read_only;
 SDL_UnlockMutex(b->mutex);if(FAILED(result))return result;
 if(id==UIA_IsReadOnlyAttributeId){out->vt=VT_BOOL;out->boolVal=readonly?VARIANT_TRUE:VARIANT_FALSE;return S_OK;}
 IUnknown *unsupported=NULL;result=UiaGetReservedNotSupportedValue(&unsupported);
 if(SUCCEEDED(result)){out->vt=VT_UNKNOWN;out->punkVal=unsupported;}return result;
}
static HRESULT STDMETHODCALLTYPE uia_tr_find_attribute(ITextRangeProvider *p,TEXTATTRIBUTEID id,VARIANT value,BOOL backward,ITextRangeProvider **out) {
 (void)backward;if(!out)return E_POINTER;*out=NULL;
 VARIANT actual;HRESULT result=uia_tr_get_attribute(p,id,&actual);if(FAILED(result))return result;
 bool matches=actual.vt==VT_BOOL && value.vt==VT_BOOL && actual.boolVal==value.boolVal;VariantClear(&actual);
 return matches?uia_tr_clone(p,out):S_OK;
}
static HRESULT STDMETHODCALLTYPE uia_tr_find_text(ITextRangeProvider *p,BSTR text,BOOL backward,BOOL ignore_case,ITextRangeProvider **out) {
 if(!out)return E_POINTER;*out=NULL;if(!text || !SysStringLen(text))return E_INVALIDARG;
 uia_text_range *r=uia_range_owner(p);ps_a11y_native *b=r->owner->bridge;SDL_LockMutex(b->mutex);
 HRESULT result=uia_text_range_live_locked(r);size_t n=SysStringLen(text),found=SIZE_MAX;
 if(SUCCEEDED(result) && n<=r->end-r->start)
  for(size_t at=r->start;at+n<=r->end;at=uia_character_next(b,at)) {
   if(CompareStringOrdinal(b->reading_text+at,(int)n,text,(int)n,ignore_case)==CSTR_EQUAL){found=at;if(!backward)break;}
   if(at==b->reading_length)break;
  }
 uia_text_range *match=found!=SIZE_MAX?uia_text_range_new(r->owner,r->version,found,found+n):NULL;SDL_UnlockMutex(b->mutex);
 if(FAILED(result))return result;if(found!=SIZE_MAX && !match)return E_OUTOFMEMORY;if(match)*out=&match->range_iface;return S_OK;
}
static HRESULT STDMETHODCALLTYPE uia_tr_text(ITextRangeProvider *p,int maximum,BSTR *out) {
 if(!out)return E_POINTER;*out=NULL;if(maximum<-1)return E_INVALIDARG;
 uia_text_range *r=uia_range_owner(p);ps_a11y_native *b=r->owner->bridge;SDL_LockMutex(b->mutex);HRESULT result=uia_text_range_live_locked(r);
 if(SUCCEEDED(result)) {
  size_t n=r->end-r->start;if(maximum>=0 && n>(size_t)maximum)n=(size_t)maximum;
  if(n && r->start+n<b->reading_length && b->reading_text[r->start+n]>=0xdc00 && b->reading_text[r->start+n]<=0xdfff)--n;
  *out=SysAllocStringLen(b->reading_text+r->start,(UINT)n);if(!*out)result=E_OUTOFMEMORY;
 }
 SDL_UnlockMutex(b->mutex);return result;
}
static void uia_set_endpoint(uia_text_range *r,enum TextPatternRangeEndpoint endpoint,size_t value) {
 r->enclosing_hint=0;
 if(endpoint==TextPatternRangeEndpoint_Start){r->start=value;if(r->end<value)r->end=value;}
 else {r->end=value;if(r->start>value)r->start=value;}
}
static int uia_move_position(ps_a11y_native *b,size_t *at,enum TextUnit unit,int count) {
 int moved=0;int direction=count<0?-1:1;
 while(moved!=count) {
  size_t next=direction>0?uia_unit_end(b,*at,unit):*at?uia_unit_start(b,uia_character_previous(b,*at),unit):0;
  if(next==*at)break;*at=next;moved+=direction;
 }return moved;
}
static HRESULT STDMETHODCALLTYPE uia_tr_move_endpoint(ITextRangeProvider *p,enum TextPatternRangeEndpoint endpoint,enum TextUnit unit,int count,int *out) {
 if(!out)return E_POINTER;*out=0;unit=uia_supported_unit(unit);if(!uia_endpoint(endpoint) || (int)unit<0)return E_INVALIDARG;
 uia_text_range *r=uia_range_owner(p);ps_a11y_native *b=r->owner->bridge;SDL_LockMutex(b->mutex);HRESULT result=uia_text_range_live_locked(r);
 if(SUCCEEDED(result)){size_t at=uia_endpoint_value(r,endpoint);*out=uia_move_position(b,&at,unit,count);uia_set_endpoint(r,endpoint,at);}
 SDL_UnlockMutex(b->mutex);return result;
}
static HRESULT STDMETHODCALLTYPE uia_tr_move(ITextRangeProvider *p,enum TextUnit unit,int count,int *out) {
 if(!out)return E_POINTER;*out=0;unit=uia_supported_unit(unit);if((int)unit<0)return E_INVALIDARG;
 uia_text_range *r=uia_range_owner(p);ps_a11y_native *b=r->owner->bridge;SDL_LockMutex(b->mutex);HRESULT result=uia_text_range_live_locked(r);
 if(SUCCEEDED(result) && count) {
  bool degenerate=r->start==r->end;size_t start=degenerate?r->start:uia_unit_start(b,r->start,unit);
  if(degenerate)*out=uia_move_position(b,&start,unit,count);
  else {int direction=count<0?-1:1;
   while(*out!=count){size_t candidate=start;if(!uia_move_position(b,&candidate,unit,direction) || candidate>=b->reading_length)break;start=candidate;*out+=direction;}}
  r->enclosing_hint=0;r->start=start;r->end=degenerate?start:uia_unit_end(b,start,unit);
 }
 SDL_UnlockMutex(b->mutex);return result;
}
static HRESULT STDMETHODCALLTYPE uia_tr_endpoint_range(ITextRangeProvider *p,enum TextPatternRangeEndpoint endpoint,ITextRangeProvider *other,enum TextPatternRangeEndpoint target) {
 if(!uia_endpoint(endpoint) || !uia_endpoint(target) || !uia_same_text_range(p,other))return E_INVALIDARG;
 uia_text_range *r=uia_range_owner(p);ps_a11y_native *b=r->owner->bridge;SDL_LockMutex(b->mutex);HRESULT result=uia_text_range_live_locked(r);
 if(SUCCEEDED(result))uia_set_endpoint(r,endpoint,uia_endpoint_value(uia_range_owner(other),target));SDL_UnlockMutex(b->mutex);return result;
}
static HRESULT STDMETHODCALLTYPE uia_tr_no_selection(ITextRangeProvider *p) {
 uia_text_range *r=uia_range_owner(p);ps_a11y_native *b=r->owner->bridge;SDL_LockMutex(b->mutex);HRESULT result=uia_text_range_live_locked(r);
 SDL_UnlockMutex(b->mutex);return FAILED(result)?result:UIA_E_INVALIDOPERATION;
}
static HRESULT STDMETHODCALLTYPE uia_tr_select(ITextRangeProvider *p) {
 uia_text_range *r=uia_range_owner(p);ps_a11y_native *b=r->owner->bridge;SDL_LockMutex(b->mutex);HRESULT result=uia_text_range_live_locked(r);
 if(SUCCEEDED(result) && (!r->owner->id || !b->hwnd || !IsWindowVisible(b->hwnd) || IsIconic(b->hwnd) ||
    !ps_a11y_edit_select(b->model,r->owner->id,r->version,b->reading_bytes[r->start],b->reading_bytes[r->end])))result=UIA_E_INVALIDOPERATION;
 SDL_UnlockMutex(b->mutex);return result;
}
static const ps_a11y_node *uia_enclosing_link_locked(uia_text_range *r) {
 if(r->owner->id)return NULL;ps_a11y_native *b=r->owner->bridge;
 size_t begin=b->reading_bytes[r->start],end=b->reading_bytes[r->end];const ps_a11y_node *best=NULL;
 for(size_t i=0;i<b->model->count;i++) {
  const ps_a11y_node *node=&b->model->nodes[i];
  if(node->role!=PS_A11Y_LINK || node->document_version!=r->version || node->document_begin>begin || node->document_end<end ||
     (begin==end && begin==node->document_end))continue;
  if(!best || node->document_end-node->document_begin<best->document_end-best->document_begin)best=node;
 }
 return best;
}
static HRESULT STDMETHODCALLTYPE uia_tr_enclosing(ITextRangeProvider *p,IRawElementProviderSimple **out) {
 if(!out)return E_POINTER;*out=NULL;uia_text_range *r=uia_range_owner(p);ps_a11y_native *b=r->owner->bridge;
 SDL_LockMutex(b->mutex);HRESULT result=uia_text_range_live_locked(r);
 if(SUCCEEDED(result)) {
  const ps_a11y_node *link=uia_enclosing_link_locked(r);
  uint64_t id=link?link->id:r->owner->id?r->owner->id:
      uia_enclosing_table_locked(b,b->reading_bytes[r->start],b->reading_bytes[r->end]);
  if(!link && r->enclosing_hint && r->start==r->end) {
   ps_a11y_table_cell cell;
   if(ps_a11y_table_cell_find(b->model,r->enclosing_hint,&cell) && cell.begin==cell.end && cell.begin==b->reading_bytes[r->start])id=cell.id;
  }
  *out=uia_any_locked(b,id);if(!*out)result=E_OUTOFMEMORY;
 }
 SDL_UnlockMutex(b->mutex);return result;
}
static HRESULT STDMETHODCALLTYPE uia_tr_children(ITextRangeProvider *p,SAFEARRAY **out) {
 if(!out)return E_POINTER;*out=NULL;uia_text_range *r=uia_range_owner(p);ps_a11y_native *b=r->owner->bridge;
 SDL_LockMutex(b->mutex);HRESULT result=uia_text_range_live_locked(r);
 if(SUCCEEDED(result) && !r->owner->id) {
  size_t begin=b->reading_bytes[r->start],end=b->reading_bytes[r->end];
  const ps_a11y_node *link=uia_enclosing_link_locked(r);
  uint64_t parent=link?link->id:uia_enclosing_table_locked(b,begin,end);
  if(!link && r->enclosing_hint && r->start==r->end) {
   ps_a11y_table_cell cell;
   if(ps_a11y_table_cell_find(b->model,r->enclosing_hint,&cell) && cell.begin==cell.end && cell.begin==begin)parent=cell.id;
  }
  result=uia_range_children_locked(b,r->version,begin,end,parent,out);
 } else if(SUCCEEDED(result)){*out=SafeArrayCreateVector(VT_UNKNOWN,0,0);if(!*out)result=E_OUTOFMEMORY;}
 SDL_UnlockMutex(b->mutex);if(FAILED(result) && *out){SafeArrayDestroy(*out);*out=NULL;}return result;
}
static HRESULT STDMETHODCALLTYPE uia_tr_scroll(ITextRangeProvider *p,BOOL top) {
 uia_text_range *r=uia_range_owner(p);ps_a11y_native *b=r->owner->bridge;SDL_LockMutex(b->mutex);HRESULT result=uia_text_range_live_locked(r);
 if(SUCCEEDED(result) && (!b->hwnd || !IsWindowVisible(b->hwnd) || IsIconic(b->hwnd)))result=UIA_E_INVALIDOPERATION;
 if(SUCCEEDED(result)) {
  bool accepted=r->owner->id?ps_a11y_edit_scroll(b->model,r->owner->id,r->version,b->reading_bytes[r->start],b->reading_bytes[r->end],top!=FALSE):
   ps_a11y_document_scroll_align(b->model,r->version,b->reading_bytes[top?r->start:r->end],top!=FALSE);
  if(!accepted)result=UIA_E_INVALIDOPERATION;
 }
 SDL_UnlockMutex(b->mutex);return result;
}
static HRESULT STDMETHODCALLTYPE uia_tr_rectangles(ITextRangeProvider *p,SAFEARRAY **out) {
 if(!out)return E_POINTER;*out=NULL;uia_text_range *r=uia_range_owner(p);ps_a11y_native *b=r->owner->bridge;
 SDL_LockMutex(b->mutex);HRESULT result=uia_text_range_live_locked(r);POINT origin={0};
 if(SUCCEEDED(result) && !ClientToScreen(b->hwnd,&origin))result=UIA_E_ELEMENTNOTAVAILABLE;
 double *rectangles=SUCCEEDED(result)?malloc(4*PS_A11Y_TEXT_BOXES*sizeof *rectangles):NULL;size_t count=0;
 if(SUCCEEDED(result) && !rectangles)result=E_OUTOFMEMORY;
 if(SUCCEEDED(result)) {
  size_t box_count;const ps_a11y_text_box *boxes=uia_boxes_locked(r->owner,&box_count);size_t begin=b->reading_bytes[r->start],end=b->reading_bytes[r->end];
  const ps_a11y_node *node=ps_a11y_find(b->model,r->owner->id);
  if(node && uia_window_viewable_locked(b) && begin==end && begin==node->text_caret && node->caret_bounds[2]>0 && node->caret_bounds[3]>0) {
   const float *box=node->caret_bounds;double values[]={origin.x+box[0]*b->scale_x,origin.y+box[1]*b->scale_y,box[2]*b->scale_x,box[3]*b->scale_y};
   memcpy(rectangles,values,sizeof values);count=1;
  }
  for(size_t i=0;i<box_count;i++)if(boxes[i].begin<end && boxes[i].end>begin) {
   const float *box=boxes[i].bounds;double values[]={origin.x+box[0]*b->scale_x,origin.y+box[1]*b->scale_y,box[2]*b->scale_x,box[3]*b->scale_y};
   double *last=count?rectangles+4*(count-1):NULL;
   if(last && fabs(last[1]-values[1])<.1 && fabs(last[3]-values[3])<.1 && fabs(last[0]+last[2]-values[0])<.5)last[2]+=values[2];
   else {memcpy(rectangles+4*count,values,sizeof values);++count;}
  }
 }
 SDL_UnlockMutex(b->mutex);
 if(SUCCEEDED(result)) {
  *out=SafeArrayCreateVector(VT_R8,0,(ULONG)(4*count));if(!*out)result=E_OUTOFMEMORY;
  for(LONG i=0;SUCCEEDED(result) && i<(LONG)(4*count);i++)result=SafeArrayPutElement(*out,&i,&rectangles[i]);
  if(FAILED(result) && *out){SafeArrayDestroy(*out);*out=NULL;}
 }
 free(rectangles);return result;
}
static const ITextRangeProviderVtbl uia_text_range_vtable={uia_tr_query,uia_tr_addref,uia_tr_release,uia_tr_clone,uia_tr_compare,
 uia_tr_compare_endpoints,uia_tr_expand,uia_tr_find_attribute,uia_tr_find_text,uia_tr_get_attribute,uia_tr_rectangles,uia_tr_enclosing,
 uia_tr_text,uia_tr_move,uia_tr_move_endpoint,uia_tr_endpoint_range,uia_tr_select,uia_tr_no_selection,uia_tr_no_selection,uia_tr_scroll,uia_tr_children};
static uia_text_range *uia_text_range_new(uia_element *owner,uint64_t version,size_t start,size_t end) {
 uia_text_range *r=calloc(1,sizeof *r);if(!r)return NULL;r->range_iface.lpVtbl=&uia_text_range_vtable;
 r->references=1;r->owner=owner;r->version=version;r->start=start;r->end=end;uia_addref(owner);return r;
}
static HRESULT STDMETHODCALLTYPE uia_text_document(ITextProvider *p,ITextRangeProvider **out) {
 if(!out)return E_POINTER;*out=NULL;uia_element *e=UIA_OWNER(ITextProvider,text,p);ps_a11y_native *b=e->bridge;
 SDL_LockMutex(b->mutex);HRESULT result=uia_reading_locked(e);
 uia_text_range *r=SUCCEEDED(result)?uia_text_range_new(e,b->reading_version,0,b->reading_length):NULL;SDL_UnlockMutex(b->mutex);
 if(FAILED(result))return result;if(!r)return E_OUTOFMEMORY;*out=&r->range_iface;return S_OK;
}
static HRESULT STDMETHODCALLTYPE uia_text_selection(ITextProvider *p,SAFEARRAY **out) {
 if(!out)return E_POINTER;*out=NULL;uia_element *e=UIA_OWNER(ITextProvider,text,p);ps_a11y_native *b=e->bridge;
 SDL_LockMutex(b->mutex);HRESULT result=uia_reading_locked(e);
 if(SUCCEEDED(result)) {
  *out=SafeArrayCreateVector(VT_UNKNOWN,0,e->id?1:0);if(!*out)result=E_OUTOFMEMORY;
  if(SUCCEEDED(result) && e->id){const ps_a11y_node *node=ps_a11y_find(b->model,e->id);
   uia_text_range *r=uia_text_range_new(e,b->reading_version,uia_utf16_locked(b,node->text_start),uia_utf16_locked(b,node->text_end));LONG zero=0;
   if(!r)result=E_OUTOFMEMORY;else {result=SafeArrayPutElement(*out,&zero,&r->range_iface);uia_tr_release(&r->range_iface);}
  }
 }
 SDL_UnlockMutex(b->mutex);if(FAILED(result) && *out){SafeArrayDestroy(*out);*out=NULL;}return result;
}
static HRESULT STDMETHODCALLTYPE uia_text_supported_selection(ITextProvider *p,enum SupportedTextSelection *out) {
 if(!out)return E_POINTER;uia_element *e=UIA_OWNER(ITextProvider,text,p);ps_a11y_native *b=e->bridge;
 SDL_LockMutex(b->mutex);HRESULT result=uia_reading_locked(e);SDL_UnlockMutex(b->mutex);if(SUCCEEDED(result))*out=e->id?SupportedTextSelection_Single:SupportedTextSelection_None;return result;
}
static HRESULT STDMETHODCALLTYPE uia_text_visible(ITextProvider *p,SAFEARRAY **out) {
 if(!out)return E_POINTER;*out=NULL;uia_element *e=UIA_OWNER(ITextProvider,text,p);ps_a11y_native *b=e->bridge;
 SDL_LockMutex(b->mutex);HRESULT result=uia_reading_locked(e);
 size_t starts[PS_A11Y_TEXT_BOXES],ends[PS_A11Y_TEXT_BOXES],count=0;
 if(SUCCEEDED(result)) {
  size_t box_count;const ps_a11y_text_box *boxes=uia_boxes_locked(e,&box_count);
  for(size_t i=0;i<box_count;i++) {
   size_t begin=uia_utf16_locked(b,boxes[i].begin),end=uia_utf16_locked(b,boxes[i].end);
   if(count && begin<=ends[count-1]){if(end>ends[count-1])ends[count-1]=end;}
   else {starts[count]=begin;ends[count++]=end;}
  }
  if(!count){starts[0]=ends[0]=0;count=1;} /* No visible text: a degenerate range. */
  *out=SafeArrayCreateVector(VT_UNKNOWN,0,(ULONG)count);if(!*out)result=E_OUTOFMEMORY;
  for(LONG i=0;SUCCEEDED(result) && i<(LONG)count;i++) {
   uia_text_range *r=uia_text_range_new(e,b->reading_version,starts[i],ends[i]);
   if(!r){result=E_OUTOFMEMORY;break;}result=SafeArrayPutElement(*out,&i,&r->range_iface);uia_tr_release(&r->range_iface);
  }
 }
 SDL_UnlockMutex(b->mutex);if(FAILED(result) && *out){SafeArrayDestroy(*out);*out=NULL;}return result;
}
static HRESULT STDMETHODCALLTYPE uia_text_child(ITextProvider *p,IRawElementProviderSimple *child,ITextRangeProvider **out) {
 if(!out)return E_POINTER;*out=NULL;
 uia_element *e=UIA_OWNER(ITextProvider,text,p);
 if(e->id || !child)return E_INVALIDARG;
 ps_a11y_native *b=e->bridge;SDL_LockMutex(b->mutex);HRESULT result=uia_reading_locked(e);
 size_t begin=0,end=0;uint64_t identity=0;if(SUCCEEDED(result))result=uia_child_range_locked(b,child,&begin,&end,&identity);
 uia_text_range *r=SUCCEEDED(result)?uia_text_range_new(e,b->reading_version,
     uia_utf16_locked(b,begin),uia_utf16_locked(b,end)):NULL;
 if(r && begin==end)r->enclosing_hint=identity;
 if(SUCCEEDED(result)){if(r)*out=&r->range_iface;else result=E_OUTOFMEMORY;}
 SDL_UnlockMutex(b->mutex);return result;
}
static HRESULT STDMETHODCALLTYPE uia_text_point(ITextProvider *p,struct UiaPoint point,ITextRangeProvider **out) {
 if(!out)return E_POINTER;*out=NULL;if(!isfinite(point.x) || !isfinite(point.y))return E_INVALIDARG;
 uia_element *e=UIA_OWNER(ITextProvider,text,p);ps_a11y_native *b=e->bridge;SDL_LockMutex(b->mutex);HRESULT result=uia_reading_locked(e);
 POINT origin={0};if(SUCCEEDED(result) && !ClientToScreen(b->hwnd,&origin))result=UIA_E_ELEMENTNOTAVAILABLE;
 size_t byte=0;double best=HUGE_VAL;
 if(SUCCEEDED(result)) {
  size_t box_count;const ps_a11y_text_box *boxes=uia_boxes_locked(e,&box_count);
  for(size_t i=0;i<box_count;i++) {
   const float *box=boxes[i].bounds;double x=origin.x+box[0]*b->scale_x,y=origin.y+box[1]*b->scale_y,w=box[2]*b->scale_x,h=box[3]*b->scale_y;
   double dx=fmax(x-point.x,fmax(0,point.x-x-w)),dy=fmax(y-point.y,fmax(0,point.y-y-h)),distance=dx*dx+dy*dy;
   if(distance<best){best=distance;byte=point.x>x+w/2?boxes[i].end:boxes[i].begin;}
  }
 }
 size_t at=SUCCEEDED(result)?uia_utf16_locked(b,byte):0;
 uia_text_range *r=SUCCEEDED(result)?uia_text_range_new(e,b->reading_version,at,at):NULL;SDL_UnlockMutex(b->mutex);
 if(FAILED(result))return result;if(!r)return E_OUTOFMEMORY;*out=&r->range_iface;return S_OK;
}
static const ITextProviderVtbl uia_text_vtable={uia_text_query,uia_text_addref,uia_text_release,uia_text_selection,uia_text_visible,
 uia_text_child,uia_text_point,uia_text_document,uia_text_supported_selection};
static ITextProvider *uia_text2_base(ITextProvider2 *p){return &UIA_OWNER(ITextProvider2,text2,p)->text;}
static HRESULT STDMETHODCALLTYPE uia_text2_selection(ITextProvider2 *p,SAFEARRAY **out){return uia_text_selection(uia_text2_base(p),out);}
static HRESULT STDMETHODCALLTYPE uia_text2_visible(ITextProvider2 *p,SAFEARRAY **out){return uia_text_visible(uia_text2_base(p),out);}
static HRESULT STDMETHODCALLTYPE uia_text2_child(ITextProvider2 *p,IRawElementProviderSimple *child,ITextRangeProvider **out){return uia_text_child(uia_text2_base(p),child,out);}
static HRESULT STDMETHODCALLTYPE uia_text2_point(ITextProvider2 *p,struct UiaPoint point,ITextRangeProvider **out){return uia_text_point(uia_text2_base(p),point,out);}
static HRESULT STDMETHODCALLTYPE uia_text2_document(ITextProvider2 *p,ITextRangeProvider **out){return uia_text_document(uia_text2_base(p),out);}
static HRESULT STDMETHODCALLTYPE uia_text2_supported(ITextProvider2 *p,enum SupportedTextSelection *out){return uia_text_supported_selection(uia_text2_base(p),out);}
static HRESULT STDMETHODCALLTYPE uia_text2_annotation(ITextProvider2 *p,IRawElementProviderSimple *annotation,ITextRangeProvider **out) {
 (void)p;(void)annotation;if(!out)return E_POINTER;*out=NULL;return E_INVALIDARG;
}
static HRESULT STDMETHODCALLTYPE uia_text2_caret(ITextProvider2 *p,BOOL *active,ITextRangeProvider **out) {
 if(!out || !active)return E_POINTER;*out=NULL;*active=FALSE;uia_element *e=UIA_OWNER(ITextProvider2,text2,p);ps_a11y_native *b=e->bridge;
 SDL_LockMutex(b->mutex);HRESULT result=uia_reading_locked(e);uia_text_range *r=NULL;
 if(SUCCEEDED(result)){const ps_a11y_node *node=ps_a11y_find(b->model,e->id);size_t at=uia_utf16_locked(b,node->text_caret);
  r=uia_text_range_new(e,b->reading_version,at,at);*active=node->focused && node->text_active && uia_window_viewable_locked(b);
 }
 SDL_UnlockMutex(b->mutex);if(FAILED(result))return result;if(!r)return E_OUTOFMEMORY;*out=&r->range_iface;return S_OK;
}
static const ITextProvider2Vtbl uia_text2_vtable={uia_text2_query,uia_text2_addref,uia_text2_release,uia_text2_selection,uia_text2_visible,
 uia_text2_child,uia_text2_point,uia_text2_document,uia_text2_supported,uia_text2_annotation,uia_text2_caret};
