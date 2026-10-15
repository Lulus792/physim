/* Sparse, generation-owned native objects. They never occupy control slots.
 * Retaining queried objects until content replacement preserves their identity
 * even when a client temporarily releases an NSArray returned by AppKit. */
enum {MAC_TABLE=1,MAC_ROW,MAC_COLUMN,MAC_CELL};
static Class mac_table_classes[5];static Ivar mac_table_index_ivar;
static bool mac_table_classes_ready;
extern id const NSAccessibilityTableRole,NSAccessibilityRowRole,NSAccessibilityColumnRole,NSAccessibilityCellRole;
static unsigned mac_table_kind(id object) {
 Class cls=object_getClass(object);for(unsigned i=1;i<=4;i++)if(cls==mac_table_classes[i])return i;return 0;
}
static unsigned long mac_table_axis(id object) {
 unsigned long index;memcpy(&index,(char*)object+ivar_getOffset(mac_table_index_ivar),sizeof index);return index;
}
static bool mac_table_live_locked(id object,ps_a11y_native *b,ps_a11y_table *table,ps_a11y_table_cell *cell) {
 if(!b->model || !b->document || !b->model->document || b->table_object_version!=b->model->document->version)return false;unsigned kind=mac_table_kind(object);
 if(kind==MAC_CELL)return ps_a11y_table_cell_find(b->model,identifier(object),cell) && ps_a11y_table_get(b->model,cell->table,table);
 if(!kind || !ps_a11y_table_get(b->model,identifier(object),table))return false;
 return kind==MAC_TABLE || mac_table_axis(object)<(kind==MAC_ROW?table->rows:table->columns);
}
static id mac_table_object_locked(ps_a11y_native *b,unsigned kind,uint64_t id_value,unsigned long index);
static id mac_table_retained_parent_locked(ps_a11y_native *b,id object) {
 ps_a11y_table table;ps_a11y_table_cell cell;
 if(!mac_table_live_locked(object,b,&table,&cell))return nil;
 unsigned kind=mac_table_kind(object);id parent=kind==MAC_TABLE?b->document:
  kind==MAC_CELL?mac_table_object_locked(b,MAC_ROW,cell.table,(unsigned long)cell.row):mac_table_object_locked(b,MAC_TABLE,table.id,0);
 return parent?send0(parent,"retain"):nil;
}
static id mac_table_object_locked(ps_a11y_native *b,unsigned kind,uint64_t id_value,unsigned long index) {
 if(!b->model || !b->document || !b->model->document || b->table_object_version!=b->model->document->version || kind<1 || kind>4)return nil;
 ps_a11y_table t;ps_a11y_table_cell c;
 bool valid=kind==MAC_CELL?ps_a11y_table_cell_find(b->model,id_value,&c):
  ps_a11y_table_get(b->model,id_value,&t) && (kind==MAC_TABLE || index<(kind==MAC_ROW?t.rows:t.columns));
 if(!valid)return nil;
 if(!b->table_objects)b->table_objects=send0((id)objc_getClass("NSMutableDictionary"),"new");
 if(!b->table_objects)return nil;
 char key[128];snprintf(key,sizeof key,"%u:%llu:%lu",kind,(unsigned long long)id_value,index);id name=string(key);
 id object=((id(*)(id,SEL,id))objc_msgSend)(b->table_objects,sel_registerName("objectForKey:"),name);
 if(object)return object;
 object=send0((id)mac_table_classes[kind],"new");if(!object)return nil;
 SDL_AddAtomicInt(&b->references,1);
 memcpy((char*)object+ivar_getOffset(owner_ivar),&b,sizeof b);
 memcpy((char*)object+ivar_getOffset(id_ivar),&id_value,sizeof id_value);
 memcpy((char*)object+ivar_getOffset(mac_table_index_ivar),&index,sizeof index);
 ((void(*)(id,SEL,id,id))objc_msgSend)(b->table_objects,sel_registerName("setObject:forKey:"),object,name);
 id parent=mac_table_retained_parent_locked(b,object);send1(object,"setAccessibilityParent:",parent);if(parent)send0(parent,"release");
 send0(object,"release");return object;
}
static id mac_tables_prepare_locked(ps_a11y_native *b) {
 uint64_t version=b->model && b->model->document && b->model->document->active?b->model->document->version:0;
 if(version==b->table_object_version)return nil;
 id retired=b->table_objects?send0(send0(b->table_objects,"allValues"),"retain"):nil;
 if(b->table_objects)send0(b->table_objects,"removeAllObjects");b->table_object_version=version;return retired;
}
static void mac_tables_retired(id objects) {
 unsigned long count=objects?((unsigned long(*)(id,SEL))objc_msgSend)(objects,sel_registerName("count")):0;
 for(unsigned long i=0;i<count;i++) {
  id object=((id(*)(id,SEL,unsigned long))objc_msgSend)(objects,sel_registerName("objectAtIndex:"),i);
  send1(object,"setAccessibilityParent:",nil);NSAccessibilityPostNotification(object,string("AXUIElementDestroyed"));
 }
 if(objects)send0(objects,"release");
}
static BOOL mac_table_valid(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return NO;ps_a11y_table t;ps_a11y_table_cell c;
 SDL_LockMutex(b->mutex);bool valid=mac_table_live_locked(object,b,&t,&c);SDL_UnlockMutex(b->mutex);return valid;
}
static BOOL mac_table_enabled(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return NO;ps_a11y_table t;ps_a11y_table_cell c;
 SDL_LockMutex(b->mutex);bool enabled=document_viewable_locked(b) && mac_table_live_locked(object,b,&t,&c);SDL_UnlockMutex(b->mutex);return enabled;
}
static id mac_table_role(id object,SEL selector) {
 if(!mac_table_valid(object,selector))return nil;
 switch(mac_table_kind(object)){case MAC_TABLE:return NSAccessibilityTableRole;case MAC_ROW:return NSAccessibilityRowRole;
 case MAC_COLUMN:return NSAccessibilityColumnRole;default:return NSAccessibilityCellRole;}
}
static id mac_table_cell_string_locked(ps_a11y_native *b,const ps_a11y_table_cell *cell) {
 size_t length=cell->end-cell->begin;char *text=malloc(length+1);if(!text)return nil;
 memcpy(text,b->model->document->text+cell->begin,length);text[length]=0;id result=string(text);free(text);return result;
}
static id mac_table_label(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return nil;id value=nil;ps_a11y_table t;ps_a11y_table_cell c;
 SDL_LockMutex(b->mutex);
 if(mac_table_live_locked(object,b,&t,&c)) {
  unsigned kind=mac_table_kind(object);char label[384];
  if(kind==MAC_CELL)value=mac_table_cell_string_locked(b,&c);
  else if(kind==MAC_COLUMN && ps_a11y_table_column_header(b->model,t.id,mac_table_axis(object),&c))value=mac_table_cell_string_locked(b,&c);
  else {if(kind==MAC_TABLE)snprintf(label,sizeof label,"Tabelle: %s",b->model->document->name);
   else snprintf(label,sizeof label,"%s %lu",kind==MAC_ROW?"Zeile":"Spalte",mac_table_axis(object)+1);value=string(label);}
 }
 SDL_UnlockMutex(b->mutex);return value;
}
static id mac_table_identifier(id object,SEL selector) {
 (void)selector;char key[128];snprintf(key,sizeof key,"physim-table-%u-%llu-%lu",mac_table_kind(object),(unsigned long long)identifier(object),mac_table_axis(object));return string(key);
}
static id mac_table_parent(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return nil;
 SDL_LockMutex(b->mutex);id parent=mac_table_retained_parent_locked(b,object);SDL_UnlockMutex(b->mutex);
 return parent?send0(parent,"autorelease"):nil;
}
static bool mac_table_bounds_locked(ps_a11y_native *b,id object,float bounds[4]) {
 ps_a11y_table t;ps_a11y_table_cell c;if(!mac_table_live_locked(object,b,&t,&c))return false;
 memset(bounds,0,4*sizeof *bounds);if(!document_viewable_locked(b))return true;
 switch(mac_table_kind(object)) {
 case MAC_TABLE:return ps_a11y_table_bounds(b->model,t.id,bounds);
 case MAC_ROW:return ps_a11y_table_row_bounds(b->model,t.id,mac_table_axis(object),bounds);
 case MAC_COLUMN:return ps_a11y_table_column_bounds(b->model,t.id,mac_table_axis(object),bounds);
 default:memcpy(bounds,c.bounds,sizeof c.bounds);return true;
 }
}
static CGRect mac_table_frame(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return CGRectZero;float box[4];CGRect result=CGRectZero;
 SDL_LockMutex(b->mutex);
 if(mac_table_bounds_locked(b,object,box) && box[2]>0 && box[3]>0) {
  const float *view=b->model->document->viewport;CGRect frame=document_frame(b->document,NULL);
  if(!CGRectIsEmpty(frame))result=CGRectMake(frame.origin.x+box[0]-view[0],frame.origin.y+view[3]-(box[1]-view[1])-box[3],box[2],box[3]);
 }
 SDL_UnlockMutex(b->mutex);return result;
}
static id mac_table_hit(id object,SEL selector,CGPoint point) {
 (void)selector;if(!CGRectContainsPoint(mac_table_frame(object,NULL),point))return nil;
 ps_a11y_native *b=owner(object);if(!b)return nil;
 if(mac_table_kind(object)==MAC_COLUMN)return object;
 return mac_document_hit(b->document,NULL,point);
}
static id mac_table_collection(id object,unsigned collection,bool visible) {
 ps_a11y_native *b=owner(object);id array=send0((id)objc_getClass("NSMutableArray"),"array");if(!b)return array;
 SDL_LockMutex(b->mutex);ps_a11y_table t;ps_a11y_table_cell c;bool okay=true;
 if(mac_table_live_locked(object,b,&t,&c) && (!visible || document_viewable_locked(b))) {
  unsigned kind=mac_table_kind(object);size_t count=0;
  if(collection==MAC_ROW)count=t.rows;else if(collection==MAC_COLUMN)count=t.columns;
  else if(kind==MAC_TABLE)count=t.rows+t.columns;
  else if(kind==MAC_ROW)count=t.columns;
  else if(kind==MAC_CELL) {
   size_t order[PS_A11Y_MAX_NODES],used=0;
   for(size_t i=0;i<b->model->count;i++) {
    const ps_a11y_node *n=&b->model->nodes[i];ps_a11y_table_cell parent;
    if(n->role!=PS_A11Y_LINK || n->document_version!=c.version || (visible && n->offscreen) ||
       !ps_a11y_table_cell_for_range(b->model,n->document_begin,n->document_end,&parent) || parent.id!=c.id)continue;
    size_t at=used++;while(at && b->model->nodes[order[at-1]].document_begin>n->document_begin){order[at]=order[at-1];at--;}order[at]=i;
   }
   for(size_t i=0;i<used;i++){id link=element_for(b,b->model->nodes[order[i]].id);if(link)send1(array,"addObject:",link);}
  }
  for(size_t i=0;i<count;i++) {
   unsigned child_kind;unsigned long axis=0;uint64_t value=t.id;
   if(collection){child_kind=collection;axis=(unsigned long)i;}
   else if(kind==MAC_TABLE){child_kind=i<t.rows?MAC_ROW:MAC_COLUMN;axis=(unsigned long)(i<t.rows?i:i-t.rows);}
   else {child_kind=MAC_CELL;if(!ps_a11y_table_cell_get(b->model,t.id,mac_table_axis(object),i,&c)){okay=false;break;}value=c.id;}
   id child=mac_table_object_locked(b,child_kind,value,axis);if(!child){okay=false;break;}
   float bounds[4];if(!visible || (mac_table_bounds_locked(b,child,bounds) && bounds[2]>0 && bounds[3]>0))send1(array,"addObject:",child);
  }
 }
 SDL_UnlockMutex(b->mutex);return okay?array:nil;
}
static id mac_table_children(id object,SEL selector){(void)selector;return mac_table_collection(object,0,false);}
static id mac_table_visible_children(id object,SEL selector){(void)selector;return mac_table_collection(object,0,true);}
static id mac_table_rows(id object,SEL selector){return mac_table_collection(object,MAC_ROW,selector==sel_registerName("accessibilityVisibleRows"));}
static id mac_table_columns(id object,SEL selector){return mac_table_collection(object,MAC_COLUMN,selector==sel_registerName("accessibilityVisibleColumns"));}
static id mac_table_cell_at(id object,SEL selector,long column,long row) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b || column<0 || row<0)return nil;id value=nil;ps_a11y_table t;ps_a11y_table_cell c;
 SDL_LockMutex(b->mutex);
 if(mac_table_live_locked(object,b,&t,&c) && ps_a11y_table_cell_get(b->model,t.id,(size_t)row,(size_t)column,&c))
  value=send0(mac_table_object_locked(b,MAC_CELL,c.id,0),"retain");
 SDL_UnlockMutex(b->mutex);return value?send0(value,"autorelease"):nil;
}
static id mac_table_headers(id object,SEL selector) {
 ps_a11y_native *b=owner(object);id array=send0((id)objc_getClass("NSMutableArray"),"array");if(!b)return array;
 if(selector==sel_registerName("accessibilityRowHeaderUIElements"))return array;
 SDL_LockMutex(b->mutex);ps_a11y_table t;ps_a11y_table_cell c;
 if(mac_table_live_locked(object,b,&t,&c) && t.column_headers) {
  unsigned kind=mac_table_kind(object);size_t count=kind==MAC_TABLE?t.columns:1;
  for(size_t i=0;i<count;i++) {
   size_t column=kind==MAC_TABLE?i:kind==MAC_CELL?c.column:mac_table_axis(object);ps_a11y_table_cell header;
   if(ps_a11y_table_column_header(b->model,t.id,column,&header)) {
    id value=mac_table_object_locked(b,MAC_CELL,header.id,0);if(value)send1(array,"addObject:",value);
   }
  }
 }
 SDL_UnlockMutex(b->mutex);return array;
}
static id mac_table_column_header(id object,SEL selector) {
 (void)selector;id headers=mac_table_headers(object,sel_registerName("accessibilityColumnHeaderUIElements"));
 return ((unsigned long(*)(id,SEL))objc_msgSend)(headers,sel_registerName("count"))?
  ((id(*)(id,SEL,unsigned long))objc_msgSend)(headers,sel_registerName("objectAtIndex:"),0):nil;
}
static long mac_table_index(id object,SEL selector){return mac_table_valid(object,selector)?(long)mac_table_axis(object):-1;}
static long mac_table_dimension(id object,SEL selector) {
 ps_a11y_native *b=owner(object);if(!b)return 0;long count=0;ps_a11y_table t;ps_a11y_table_cell c;
 SDL_LockMutex(b->mutex);if(mac_table_live_locked(object,b,&t,&c))count=(long)(selector==sel_registerName("accessibilityRowCount")?t.rows:t.columns);
 SDL_UnlockMutex(b->mutex);return count;
}
static BOOL mac_table_ordered(id object,SEL selector){return mac_table_valid(object,selector);}
static ax_range mac_table_cell_coordinate(id object,SEL selector) {
 ps_a11y_native *b=owner(object);ax_range range={AX_NOT_FOUND,0};if(!b)return range;ps_a11y_table t;ps_a11y_table_cell c;
 SDL_LockMutex(b->mutex);if(mac_table_live_locked(object,b,&t,&c))range=(ax_range){selector==sel_registerName("accessibilityRowIndexRange")?(unsigned long)c.row:(unsigned long)c.column,1};
 SDL_UnlockMutex(b->mutex);return range;
}
static bool mac_table_cell_range_locked(id object,ps_a11y_native *b,ax_range *range) {
 ps_a11y_table t;ps_a11y_table_cell c;size_t begin,end;
 if(mac_table_kind(object)!=MAC_CELL || !mac_table_live_locked(object,b,&t,&c) || !document_index_locked(b,b->model->document) ||
    !ps_a11y_utf16_offset(&b->document_index,c.begin,&begin) || !ps_a11y_utf16_offset(&b->document_index,c.end,&end))return false;
 *range=(ax_range){begin,end-begin};return true;
}
static ax_range mac_table_shared_range(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);ax_range result={AX_NOT_FOUND,0};if(!b)return result;
 SDL_LockMutex(b->mutex);mac_table_cell_range_locked(object,b,&result);SDL_UnlockMutex(b->mutex);return result;
}
static id mac_table_shared_elements(id object,SEL selector) {
 ps_a11y_native *b=owner(object);id array=send0((id)objc_getClass("NSMutableArray"),"array");if(!b)return array;
 SDL_LockMutex(b->mutex);if(mac_table_valid(object,selector))send1(array,"addObject:",b->document);SDL_UnlockMutex(b->mutex);return array;
}
static long mac_table_character_count(id object,SEL selector) {
 ax_range range=mac_table_shared_range(object,selector);return range.location==AX_NOT_FOUND?0:(long)range.length;
}
static id mac_table_string_range(id object,SEL selector,ax_range range) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return nil;id value=nil;ax_range shared;size_t begin,end;
 SDL_LockMutex(b->mutex);
 if(mac_table_cell_range_locked(object,b,&shared) && range.location<=shared.length && range.length<=shared.length-range.location &&
    ps_a11y_utf16_range(&b->document_index,shared.location+range.location,range.length,&begin,&end)) {
  ps_a11y_table_cell slice={.begin=begin,.end=end};value=mac_table_cell_string_locked(b,&slice);
 }
 SDL_UnlockMutex(b->mutex);return value;
}
static ax_range mac_table_visible_range(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);ax_range result={0};if(!b)return result;
 SDL_LockMutex(b->mutex);ax_range shared;ps_a11y_table t;ps_a11y_table_cell c;
 if(document_viewable_locked(b) && mac_table_cell_range_locked(object,b,&shared) && mac_table_live_locked(object,b,&t,&c)) {
  const ps_a11y_document *d=b->model->document;size_t begin=c.end,end=c.begin,start,finish;
  for(size_t i=0;i<d->count;i++)if(d->boxes[i].begin>=c.begin && d->boxes[i].end<=c.end) {
   if(d->boxes[i].begin<begin)begin=d->boxes[i].begin;if(d->boxes[i].end>end)end=d->boxes[i].end;
  }
  if(end>begin && ps_a11y_utf16_offset(&b->document_index,begin,&start) && ps_a11y_utf16_offset(&b->document_index,end,&finish))result=(ax_range){start-shared.location,finish-start};
 }
 SDL_UnlockMutex(b->mutex);return result;
}
static CGRect mac_table_range_frame(id object,SEL selector,ax_range range) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return CGRectZero;ax_range shared;size_t begin,end;CGRect result=CGRectZero;
 SDL_LockMutex(b->mutex);
 if(document_viewable_locked(b) && mac_table_cell_range_locked(object,b,&shared) && range.location<=shared.length && range.length<=shared.length-range.location &&
    ps_a11y_utf16_range(&b->document_index,shared.location+range.location,range.length,&begin,&end))
  result=document_range_frame(b->document,NULL,(ax_range){shared.location+range.location,range.length});
 SDL_UnlockMutex(b->mutex);return result;
}
static void mac_table_reveal(id object,SEL selector,ax_range range) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return;ax_range shared;size_t begin,end;
 SDL_LockMutex(b->mutex);
 if(document_viewable_locked(b) && mac_table_cell_range_locked(object,b,&shared) && range.location<=shared.length && range.length<=shared.length-range.location &&
    ps_a11y_utf16_range(&b->document_index,shared.location+range.location,range.length,&begin,&end))ps_a11y_document_scroll(b->model,b->model->document->version,begin);
 SDL_UnlockMutex(b->mutex);
}
static BOOL mac_table_allowed(id object,SEL selector,SEL requested) {
 if(requested==sel_registerName("setAccessibilityVisibleCharacterRange:"))return mac_table_kind(object)==MAC_CELL && mac_table_enabled(object,selector);
 const char *name=sel_getName(requested);return strncmp(name,"setAccessibility",16) && strncmp(name,"accessibilityPerform",20);
}
static bool make_table_classes(void) {
 if(mac_table_classes_ready)return true;
 Class base=objc_allocateClassPair(element_class,"PhysimAccessibilityTableNode",0);if(!base)return false;
 if(!class_addIvar(base,"physimAxis",sizeof(unsigned long),3,"Q"))return false;
#define MT_METHOD(cls,name,fn,encoding) class_addMethod(cls,sel_registerName(name),(IMP)fn,encoding)
 MT_METHOD(base,"isAccessibilityElement",mac_table_valid,PS_AX_BOOL "@:");MT_METHOD(base,"isAccessibilityEnabled",mac_table_enabled,PS_AX_BOOL "@:");
 MT_METHOD(base,"accessibilityRole",mac_table_role,"@@:");MT_METHOD(base,"accessibilityLabel",mac_table_label,"@@:");
 MT_METHOD(base,"accessibilityIdentifier",mac_table_identifier,"@@:");MT_METHOD(base,"accessibilityParent",mac_table_parent,"@@:");
 MT_METHOD(base,"accessibilityChildren",mac_table_children,"@@:");MT_METHOD(base,"accessibilityVisibleChildren",mac_table_visible_children,"@@:");
 MT_METHOD(base,"accessibilitySelectedChildren",document_empty_children,"@@:");MT_METHOD(base,"isAccessibilityFocused",document_no_action,PS_AX_BOOL "@:");
 MT_METHOD(base,"setAccessibilityFocused:",document_no_focus,"v@:" PS_AX_BOOL);MT_METHOD(base,"setAccessibilityValue:",document_no_value,"v@:@");
 MT_METHOD(base,"accessibilityPerformPress",document_no_action,PS_AX_BOOL "@:");MT_METHOD(base,"accessibilityPerformIncrement",document_no_action,PS_AX_BOOL "@:");
 MT_METHOD(base,"accessibilityPerformDecrement",document_no_action,PS_AX_BOOL "@:");MT_METHOD(base,"isAccessibilitySelectorAllowed:",mac_table_allowed,PS_AX_BOOL "@::");
 MT_METHOD(base,"accessibilityFrame",mac_table_frame,"{CGRect={CGPoint=dd}{CGSize=dd}}@:");
 MT_METHOD(base,"accessibilityHitTest:",mac_table_hit,"@@:{CGPoint=dd}");
 objc_registerClassPair(base);mac_table_index_ivar=class_getInstanceVariable(base,"physimAxis");
 const char *names[]={NULL,"PhysimAccessibilityTable","PhysimAccessibilityRow","PhysimAccessibilityColumn","PhysimAccessibilityCell"};
 for(unsigned kind=1;kind<=4;kind++) {
  Class cls=objc_allocateClassPair(base,names[kind],0);if(!cls)return false;mac_table_classes[kind]=cls;
  if(kind==MAC_TABLE) {
   MT_METHOD(cls,"accessibilityRows",mac_table_rows,"@@:");MT_METHOD(cls,"accessibilityVisibleRows",mac_table_rows,"@@:");
   MT_METHOD(cls,"accessibilityColumns",mac_table_columns,"@@:");MT_METHOD(cls,"accessibilityVisibleColumns",mac_table_columns,"@@:");
   MT_METHOD(cls,"accessibilityChildrenInNavigationOrder",mac_table_rows,"@@:");MT_METHOD(cls,"accessibilityCellForColumn:row:",mac_table_cell_at,"@@:qq");
   MT_METHOD(cls,"accessibilityRowCount",mac_table_dimension,"q@:");MT_METHOD(cls,"accessibilityColumnCount",mac_table_dimension,"q@:");
   MT_METHOD(cls,"isAccessibilityOrderedByRow",mac_table_ordered,PS_AX_BOOL "@:");MT_METHOD(cls,"accessibilityColumnHeaderUIElements",mac_table_headers,"@@:");
   MT_METHOD(cls,"accessibilityRowHeaderUIElements",mac_table_headers,"@@:");MT_METHOD(cls,"accessibilitySelectedRows",document_empty_children,"@@:");
   MT_METHOD(cls,"accessibilitySelectedColumns",document_empty_children,"@@:");MT_METHOD(cls,"accessibilitySelectedCells",document_empty_children,"@@:");
  } else if(kind==MAC_ROW || kind==MAC_COLUMN) {
   MT_METHOD(cls,"accessibilityIndex",mac_table_index,"q@:");
   if(kind==MAC_COLUMN)MT_METHOD(cls,"accessibilityHeader",mac_table_column_header,"@@:");
  } else {
   MT_METHOD(cls,"accessibilityValue",mac_table_label,"@@:");MT_METHOD(cls,"accessibilityRowIndexRange",mac_table_cell_coordinate,AX_RANGE "@:");
   MT_METHOD(cls,"accessibilityColumnIndexRange",mac_table_cell_coordinate,AX_RANGE "@:");MT_METHOD(cls,"accessibilityColumnHeaderUIElements",mac_table_headers,"@@:");
   MT_METHOD(cls,"accessibilityRowHeaderUIElements",mac_table_headers,"@@:");MT_METHOD(cls,"accessibilitySharedCharacterRange",mac_table_shared_range,AX_RANGE "@:");
   MT_METHOD(cls,"accessibilitySharedTextUIElements",mac_table_shared_elements,"@@:");MT_METHOD(cls,"accessibilityNumberOfCharacters",mac_table_character_count,"q@:");
   MT_METHOD(cls,"accessibilityStringForRange:",mac_table_string_range,"@@:" AX_RANGE);MT_METHOD(cls,"setAccessibilityVisibleCharacterRange:",mac_table_reveal,"v@:" AX_RANGE);
   MT_METHOD(cls,"accessibilityVisibleCharacterRange",mac_table_visible_range,AX_RANGE "@:");
   MT_METHOD(cls,"accessibilityFrameForRange:",mac_table_range_frame,"{CGRect={CGPoint=dd}{CGSize=dd}}@:" AX_RANGE);
  }
  objc_registerClassPair(cls);
 }
#undef MT_METHOD
 mac_table_classes_ready=mac_table_index_ivar!=NULL;return mac_table_classes_ready;
}
static id mac_document_children(id object,bool visible) {
 ps_a11y_native *b=owner(object);id array=send0((id)objc_getClass("NSMutableArray"),"array");if(!b)return array;
 SDL_LockMutex(b->mutex);const ps_a11y_document *d=document_live_locked(object,b);
 if(d && (!visible || document_viewable_locked(b))) {
  id links=document_link_children(object,visible);unsigned long count=((unsigned long(*)(id,SEL))objc_msgSend)(links,sel_registerName("count"));
  size_t ti=0;unsigned long li=0;
  for(;;) {
   id link=nil;const ps_a11y_node *node=NULL;ps_a11y_table_cell cell;
   while(li<count) {
    link=((id(*)(id,SEL,unsigned long))objc_msgSend)(links,sel_registerName("objectAtIndex:"),li);node=ps_a11y_find(b->model,identifier(link));
    if(node && !ps_a11y_table_cell_for_range(b->model,node->document_begin,node->document_end,&cell))break;
    li++;link=nil;node=NULL;
   }
   ps_a11y_table table;bool has_table=false;
   while(ps_a11y_table_at(b->model,ti,&table)) {
    float bounds[4];if(!visible || (ps_a11y_table_bounds(b->model,table.id,bounds) && bounds[2]>0 && bounds[3]>0)){has_table=true;break;}ti++;
   }
   if(!has_table && !node)break;
   if(has_table && (!node || table.begin<=node->document_begin)){id element=mac_table_object_locked(b,MAC_TABLE,table.id,0);if(element)send1(array,"addObject:",element);ti++;}
   else {send1(array,"addObject:",link);li++;}
  }
 }
 SDL_UnlockMutex(b->mutex);return array;
}
static id mac_table_parent_for_link_locked(ps_a11y_native *b,const ps_a11y_node *node) {
 ps_a11y_table_cell cell;
 return ps_a11y_table_cell_for_range(b->model,node->document_begin,node->document_end,&cell)?mac_table_object_locked(b,MAC_CELL,cell.id,0):b->document;
}
static id mac_document_hit(id object,SEL selector,CGPoint point) {
 (void)selector;ps_a11y_native *b=object?owner(object):NULL;if(!b)return nil;id result=nil;
 SDL_LockMutex(b->mutex);const ps_a11y_document *d=document_live_locked(object,b);CGRect frame=document_frame(object,NULL);
 if(d && document_viewable_locked(b) && CGRectContainsPoint(frame,point)) {
  for(size_t i=b->model->count;i>0;i--) {
   const ps_a11y_node *node=&b->model->nodes[i-1];if(node->role!=PS_A11Y_LINK || node->document_version!=d->version)continue;
   id link=element_for(b,node->id);if(link && CGRectContainsPoint(link_frame(link,NULL),point)){result=link;break;}
  }
  float x=(float)(point.x-frame.origin.x+d->viewport[0]);
  /* Reflect Cocoa's bottom-up half-open rectangles into layout coordinates. */
  float y=nextafterf((float)(d->viewport[1]+d->viewport[3]-(point.y-frame.origin.y)),-INFINITY);
  ps_a11y_table_cell cell;
  if(!result && ps_a11y_table_cell_at_point(b->model,x,y,&cell))result=mac_table_object_locked(b,MAC_CELL,cell.id,0);
  if(!result)for(size_t i=0;i<ps_a11y_table_count(b->model);i++) {
   ps_a11y_table table;float bounds[4];
   if(ps_a11y_table_at(b->model,i,&table) && ps_a11y_table_bounds(b->model,table.id,bounds) && x>=bounds[0] && y>=bounds[1] && x<bounds[0]+bounds[2] && y<bounds[1]+bounds[3]){result=mac_table_object_locked(b,MAC_TABLE,table.id,0);break;}
  }
  if(!result)result=object;send0(result,"retain");
 }
 SDL_UnlockMutex(b->mutex);return result?send0(result,"autorelease"):nil;
}
