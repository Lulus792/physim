#include "accessibility_native.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifdef __APPLE__
#include <objc/runtime.h>
#include <objc/message.h>
#include <CoreGraphics/CoreGraphics.h>
/* AppKit is linked, but this bridge remains C17. Typed runtime calls preserve
 * Objective-C's platform ABI without exposing Objective-C in the application. */
extern void NSAccessibilityPostNotification(id element,id notification);
struct ps_a11y_native {
 SDL_Window *window;ps_a11y_model *model;SDL_Mutex *mutex;SDL_AtomicInt references;
 id view,children;uint64_t ids[PS_A11Y_MAX_NODES],parents[PS_A11Y_MAX_NODES];bool checked[PS_A11Y_MAX_NODES];uint64_t focused_id;size_t count;
};
#if OBJC_BOOL_IS_BOOL
#define PS_AX_BOOL "B"
#else
#define PS_AX_BOOL "c"
#endif
static Class element_class;static Ivar owner_ivar,id_ivar;static IMP base_dealloc,base_frame;
static id send0(id object,const char *selector) {
 return ((id(*)(id,SEL))objc_msgSend)(object,sel_registerName(selector));
}
static void send1(id object,const char *selector,id value) {
 ((void(*)(id,SEL,id))objc_msgSend)(object,sel_registerName(selector),value);
}
static id string(const char *text) {
 return ((id(*)(id,SEL,const char *))objc_msgSend)((id)objc_getClass("NSString"),sel_registerName("stringWithUTF8String:"),text);
}
static ps_a11y_native *owner(id object) {
 ps_a11y_native *bridge;memcpy(&bridge,(char*)object+ivar_getOffset(owner_ivar),sizeof bridge);return bridge;
}
static uint64_t identifier(id object) {
 uint64_t value;memcpy(&value,(char*)object+ivar_getOffset(id_ivar),sizeof value);return value;
}
static bool node_copy(id object,ps_a11y_node *copy) {
 ps_a11y_native *b=owner(object);if(!b)return false;
 SDL_LockMutex(b->mutex);const ps_a11y_node *node=b->model?ps_a11y_find(b->model,identifier(object)):NULL;
 if(node)*copy=*node;
 SDL_UnlockMutex(b->mutex);return node!=NULL;
}
static void release_bridge(ps_a11y_native *b) {
 if(SDL_AddAtomicInt(&b->references,-1)==1) {SDL_DestroyMutex(b->mutex);free(b);}
}
static void element_dealloc(id object,SEL selector) {
 ps_a11y_native *b=owner(object);ps_a11y_native *none=NULL;
 memcpy((char*)object+ivar_getOffset(owner_ivar),&none,sizeof none);
 if(b)release_bridge(b);
 ((void(*)(id,SEL))base_dealloc)(object,selector);
}
static BOOL element_valid(id object,SEL selector) {(void)selector;ps_a11y_node node;return node_copy(object,&node);}
static BOOL element_enabled(id object,SEL selector) {(void)selector;ps_a11y_node node;return node_copy(object,&node)&&node.enabled;}
static id element_role(id object,SEL selector) {
 (void)selector;ps_a11y_node node;if(!node_copy(object,&node))return nil;
 return string(node.role==PS_A11Y_BUTTON?"AXButton":node.role==PS_A11Y_CHECKBOX?"AXCheckBox":node.role==PS_A11Y_RADIO?"AXRadioButton":node.role==PS_A11Y_RADIO_GROUP?"AXRadioGroup":"AXStaticText");
}
static id element_label(id object,SEL selector) {
 (void)selector;ps_a11y_node node;return node_copy(object,&node)?string(node.label):nil;
}
static id element_value(id object,SEL selector) {
 (void)selector;ps_a11y_node node;if(!node_copy(object,&node))return nil;
 if(node.role==PS_A11Y_CHECKBOX || node.role==PS_A11Y_RADIO)
  return ((id(*)(id,SEL,BOOL))objc_msgSend)((id)objc_getClass("NSNumber"),sel_registerName("numberWithBool:"),node.checked);
 return node.role==PS_A11Y_TEXT?string(node.label):nil;
}
static id element_identifier(id object,SEL selector) {
 (void)selector;char text[64];snprintf(text,sizeof text,"physim-control-%llu",(unsigned long long)identifier(object));return string(text);
}
static id element_for(ps_a11y_native *b,uint64_t id_value) {
 for(size_t i=0;i<b->count;i++)if(b->ids[i]==id_value)
  return ((id(*)(id,SEL,unsigned long))objc_msgSend)(b->children,sel_registerName("objectAtIndex:"),(unsigned long)i);
 return nil;
}
static id children_for(id object,bool selected) {
 ps_a11y_native *b=owner(object);if(!b)return nil;SDL_LockMutex(b->mutex);
 id children=send0((id)objc_getClass("NSMutableArray"),"array");
 const ps_a11y_node *parent=b->model?ps_a11y_find(b->model,identifier(object)):NULL;
 if(parent && parent->role==PS_A11Y_RADIO_GROUP)for(size_t i=0;i<b->model->count;i++) {
  const ps_a11y_node *child=&b->model->nodes[i];
  if(child->parent==parent->id && (!selected || child->checked)) {
   id element=element_for(b,child->id);if(element)send1(children,"addObject:",element);
  }
 }
 SDL_UnlockMutex(b->mutex);return children;
}
static id element_children(id object,SEL selector){(void)selector;return children_for(object,false);}
static id element_selected_children(id object,SEL selector){(void)selector;return children_for(object,true);}
static id element_parent(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return nil;SDL_LockMutex(b->mutex);
 const ps_a11y_node *node=b->model?ps_a11y_find(b->model,identifier(object)):NULL;
 id parent=node?send0(node->parent?element_for(b,node->parent):b->view,"retain"):nil;SDL_UnlockMutex(b->mutex);
 return parent?send0(parent,"autorelease"):nil;
}
static CGRect element_frame(id object,SEL selector) {
 ps_a11y_node node;if(!node_copy(object,&node))return CGRectZero;
 return ((CGRect(*)(id,SEL))base_frame)(object,selector);
}
static BOOL element_press(id object,SEL selector) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return NO;SDL_LockMutex(b->mutex);
 bool accepted=b->model && ps_a11y_press(b->model,identifier(object));SDL_UnlockMutex(b->mutex);return accepted;
}
static BOOL element_focused(id object,SEL selector) {(void)selector;ps_a11y_node node;return node_copy(object,&node)&&node.focused;}
static void element_set_focused(id object,SEL selector,BOOL focused) {
 (void)selector;ps_a11y_native *b=owner(object);if(!b)return;
 SDL_LockMutex(b->mutex);
 if(b->model && !(SDL_GetWindowFlags(b->window)&(SDL_WINDOW_HIDDEN|SDL_WINDOW_MINIMIZED))) {
  if(focused)ps_a11y_focus(b->model,identifier(object));else ps_a11y_blur(b->model,identifier(object));
 }
 SDL_UnlockMutex(b->mutex);
}
static BOOL element_allowed(id object,SEL selector,SEL requested) {
 (void)selector;
 if(requested==sel_registerName("accessibilityPerformPress")) {
  ps_a11y_node node;return node_copy(object,&node)&&ps_a11y_actionable(node.role)&&node.enabled;
 }
 if(requested==sel_registerName("setAccessibilityFocused:")){ps_a11y_node node;return node_copy(object,&node)&&node.focusable;}
 const char *name=sel_getName(requested);
 if(!strncmp(name,"setAccessibility",16) || !strncmp(name,"accessibilityPerform",20))return NO;
 return YES;
}
static bool make_class(void) {
 if(element_class)return true;
 Class base=objc_getClass("NSAccessibilityElement");if(!base)return false;
 element_class=objc_allocateClassPair(base,"PhysimAccessibilityElement",0);if(!element_class)return false;
 if(!class_addIvar(element_class,"physimOwner",sizeof(void*),3,"^v") ||
    !class_addIvar(element_class,"physimID",sizeof(uint64_t),3,"Q"))return false;
 base_dealloc=method_getImplementation(class_getInstanceMethod(base,sel_registerName("dealloc")));
 base_frame=method_getImplementation(class_getInstanceMethod(base,sel_registerName("accessibilityFrame")));
 class_addMethod(element_class,sel_registerName("dealloc"),(IMP)element_dealloc,"v@:");
 class_addMethod(element_class,sel_registerName("isAccessibilityElement"),(IMP)element_valid,PS_AX_BOOL "@:");
 class_addMethod(element_class,sel_registerName("isAccessibilityEnabled"),(IMP)element_enabled,PS_AX_BOOL "@:");
 class_addMethod(element_class,sel_registerName("isAccessibilityFocused"),(IMP)element_focused,PS_AX_BOOL "@:");
 class_addMethod(element_class,sel_registerName("setAccessibilityFocused:"),(IMP)element_set_focused,"v@:" PS_AX_BOOL);
 class_addMethod(element_class,sel_registerName("accessibilityRole"),(IMP)element_role,"@@:");
 class_addMethod(element_class,sel_registerName("accessibilityLabel"),(IMP)element_label,"@@:");
 class_addMethod(element_class,sel_registerName("accessibilityValue"),(IMP)element_value,"@@:");
 class_addMethod(element_class,sel_registerName("accessibilityIdentifier"),(IMP)element_identifier,"@@:");
 class_addMethod(element_class,sel_registerName("accessibilityParent"),(IMP)element_parent,"@@:");
 class_addMethod(element_class,sel_registerName("accessibilityChildren"),(IMP)element_children,"@@:");
 class_addMethod(element_class,sel_registerName("accessibilitySelectedChildren"),(IMP)element_selected_children,"@@:");
 class_addMethod(element_class,sel_registerName("accessibilityFrame"),(IMP)element_frame,"{CGRect={CGPoint=dd}{CGSize=dd}}@:");
 class_addMethod(element_class,sel_registerName("accessibilityPerformPress"),(IMP)element_press,PS_AX_BOOL "@:");
 class_addMethod(element_class,sel_registerName("isAccessibilitySelectorAllowed:"),(IMP)element_allowed,PS_AX_BOOL "@::");
 objc_registerClassPair(element_class);owner_ivar=class_getInstanceVariable(element_class,"physimOwner");id_ivar=class_getInstanceVariable(element_class,"physimID");
 return base_dealloc && base_frame && owner_ivar && id_ivar;
}
ps_a11y_native *ps_a11y_native_create(SDL_Window *window,ps_a11y_model *model,SDL_Mutex *mutex) {
 if(!window || !model || !mutex || !make_class())return NULL;
 id native=SDL_GetPointerProperty(SDL_GetWindowProperties(window),SDL_PROP_WINDOW_COCOA_WINDOW_POINTER,NULL);
 if(!native)return NULL;
 ps_a11y_native *b=calloc(1,sizeof *b);if(!b)return NULL;
 b->window=window;b->model=model;b->mutex=mutex;SDL_SetAtomicInt(&b->references,1);
 b->view=send0(send0(native,"contentView"),"retain");
 send1(b->view,"setAccessibilityRole:",string("AXGroup"));send1(b->view,"setAccessibilityLabel:",string("Physim"));
 ((void(*)(id,SEL,BOOL))objc_msgSend)(b->view,sel_registerName("setAccessibilityElement:"),YES);
 return b;
}
void ps_a11y_native_publish(ps_a11y_native *b,bool changed) {
 if(!b || !changed)return;
 id pool=send0(send0((id)objc_getClass("NSAutoreleasePool"),"alloc"),"init");
 id children=send0((id)objc_getClass("NSMutableArray"),"new");bool value_changed[PS_A11Y_MAX_NODES]={0};bool selection_changed[PS_A11Y_MAX_NODES]={0};
 SDL_LockMutex(b->mutex);
 uint64_t previous_focus=b->focused_id,new_focus=0;
 for(size_t i=0;i<b->model->count;i++)if(b->model->nodes[i].focused)new_focus=b->model->nodes[i].id;
 for(size_t i=0;i<b->model->count;i++) {
  const ps_a11y_node *node=&b->model->nodes[i];id element=element_for(b,node->id);
  if(node->role==PS_A11Y_RADIO_GROUP) {
   uint64_t old_selected=0,new_selected=0;
   for(size_t j=0;j<b->count;j++)if(b->parents[j]==node->id && b->checked[j])old_selected=b->ids[j];
   for(size_t j=0;j<b->model->count;j++)if(b->model->nodes[j].parent==node->id && b->model->nodes[j].checked)new_selected=b->model->nodes[j].id;
   selection_changed[i]=old_selected!=new_selected;
  }
  for(size_t j=0;j<b->count;j++)if(b->ids[j]==node->id)
   value_changed[i]=(node->role==PS_A11Y_CHECKBOX || node->role==PS_A11Y_RADIO) && node->checked!=b->checked[j];
  if(!element) {
   element=send0((id)element_class,"new");SDL_AddAtomicInt(&b->references,1);
   memcpy((char*)element+ivar_getOffset(owner_ivar),&b,sizeof b);memcpy((char*)element+ivar_getOffset(id_ivar),&node->id,sizeof node->id);
   send1(children,"addObject:",element);send0(element,"release");
  } else send1(children,"addObject:",element);
 }
 if(b->children)send0(b->children,"release");b->children=children;
 b->count=b->model->count;for(size_t i=0;i<b->count;i++){b->ids[i]=b->model->nodes[i].id;b->checked[i]=b->model->nodes[i].checked;b->parents[i]=b->model->nodes[i].parent;}
 id roots=send0((id)objc_getClass("NSMutableArray"),"array");int height;SDL_GetWindowSize(b->window,NULL,&height);
 BOOL flipped=((BOOL(*)(id,SEL))objc_msgSend)(b->view,sel_registerName("isFlipped"));
 for(size_t i=0;i<b->count;i++) {
  const ps_a11y_node *node=&b->model->nodes[i];id element=element_for(b,node->id);
  const ps_a11y_node *parent=node->parent?ps_a11y_find(b->model,node->parent):NULL;
  send1(element,"setAccessibilityParent:",parent?element_for(b,parent->id):b->view);
  if(!parent)send1(roots,"addObject:",element);
  CGRect bounds=parent?CGRectMake(node->bounds[0]-parent->bounds[0],
       parent->bounds[1]+parent->bounds[3]-node->bounds[1]-node->bounds[3],node->bounds[2],node->bounds[3])
       :CGRectMake(node->bounds[0],flipped?node->bounds[1]:height-node->bounds[1]-node->bounds[3],node->bounds[2],node->bounds[3]);
  ((void(*)(id,SEL,CGRect))objc_msgSend)(element,sel_registerName("setAccessibilityFrameInParentSpace:"),bounds);
 }
 send1(b->view,"setAccessibilityChildren:",roots);SDL_UnlockMutex(b->mutex);
 for(size_t i=0;i<b->count;i++)if(value_changed[i])NSAccessibilityPostNotification(element_for(b,b->ids[i]),string("AXValueChanged"));
 for(size_t i=0;i<b->count;i++)if(selection_changed[i])NSAccessibilityPostNotification(element_for(b,b->ids[i]),string("AXSelectedChildrenChanged"));
 b->focused_id=new_focus;
 if(previous_focus!=new_focus)NSAccessibilityPostNotification(new_focus?element_for(b,new_focus):b->view,string("AXFocusedUIElementChanged"));
 NSAccessibilityPostNotification(b->view,string("AXLayoutChanged"));send0(pool,"drain");
}
void ps_a11y_native_destroy(ps_a11y_native *b) {
 if(!b)return;
 SDL_LockMutex(b->mutex);b->model=NULL;SDL_UnlockMutex(b->mutex);
 for(size_t i=0;i<b->count;i++) {
  id element=((id(*)(id,SEL,unsigned long))objc_msgSend)(b->children,sel_registerName("objectAtIndex:"),(unsigned long)i);
  send1(element,"setAccessibilityParent:",nil);
 }
 id empty=send0((id)objc_getClass("NSArray"),"array");send1(b->view,"setAccessibilityChildren:",empty);
 if(b->children)send0(b->children,"release");send0(b->view,"release");b->view=nil;release_bridge(b);
}
bool ps_a11y_native_press_label(ps_a11y_native *b,const char *label) {
 if(!b || !label)return false;
 for(size_t i=0;i<b->count;i++) {
  id element=((id(*)(id,SEL,unsigned long))objc_msgSend)(b->children,sel_registerName("objectAtIndex:"),(unsigned long)i);
  ps_a11y_node node;
  if(node_copy(element,&node) && ps_a11y_actionable(node.role) && !strcmp(node.label,label))
   return ((BOOL(*)(id,SEL))objc_msgSend)(element,sel_registerName("accessibilityPerformPress"));
 }
 return false;
}
bool ps_a11y_native_focus_label(ps_a11y_native *b,const char *group,const char *label) {
 if(!b || !label)return false;
 for(size_t i=0;i<b->count;i++) {
  id element=element_for(b,b->ids[i]);ps_a11y_node node,parent;
  if(!node_copy(element,&node) || !node.focusable || strcmp(node.label,label))continue;
  if(group && (!node.parent || !node_copy(element_for(b,node.parent),&parent) || strcmp(group,parent.label)))continue;
  ((void(*)(id,SEL,BOOL))objc_msgSend)(element,sel_registerName("setAccessibilityFocused:"),YES);
  SDL_LockMutex(b->mutex);bool queued=b->model && (b->model->pending_focus==node.id || b->model->focused_id==node.id);SDL_UnlockMutex(b->mutex);
  return queued;
 }
 return false;
}
bool ps_a11y_native_press_choice(ps_a11y_native *b,const char *group,const char *label) {
 if(!b || !group || !label)return false;
 for(size_t i=0;i<b->count;i++) {
  id element=element_for(b,b->ids[i]);ps_a11y_node node,parent;
  if(node_copy(element,&node) && node.role==PS_A11Y_RADIO && !strcmp(node.label,label)) {
   id group_element=element_for(b,node.parent);
   if(group_element && node_copy(group_element,&parent) && !strcmp(parent.label,group))
    return ((BOOL(*)(id,SEL))objc_msgSend)(element,sel_registerName("accessibilityPerformPress"));
  }
 }
 return false;
}
bool ps_a11y_native_test(SDL_Window *window) {
 ps_a11y_model *model=malloc(sizeof *model);SDL_Mutex *mutex=SDL_CreateMutex();
 if(!model || !mutex){free(model);if(mutex)SDL_DestroyMutex(mutex);return false;}
 ps_a11y_init(model);ps_a11y_native *b=ps_a11y_native_create(window,model,mutex);
 if(!b){SDL_DestroyMutex(mutex);free(model);return false;}
 float bounds[]={10,20,100,24};ps_a11y_begin(model);ps_a11y_record(model,"main","Öffnen …",PS_A11Y_BUTTON,bounds,true);
 ps_a11y_record(model,"main","Bereit 🌍",PS_A11Y_TEXT,bounds,true);ps_a11y_record_state(model,"main","Vektoren",PS_A11Y_CHECKBOX,bounds,true,false);ps_a11y_publish(model);ps_a11y_native_publish(b,true);
 id element=send0(((id(*)(id,SEL,unsigned long))objc_msgSend)(b->children,sel_registerName("objectAtIndex:"),0),"retain");
 id label=send0(element,"accessibilityLabel");const char *utf8=((const char*(*)(id,SEL))objc_msgSend)(label,sel_registerName("UTF8String"));
 id role=send0(element,"accessibilityRole");const char *role_text=((const char*(*)(id,SEL))objc_msgSend)(role,sel_registerName("UTF8String"));
 SEL frame_selector=sel_registerName("accessibilityFrame");
 IMP frame_method=method_getImplementation(class_getInstanceMethod(object_getClass(element),frame_selector));
 CGRect frame=((CGRect(*)(id,SEL))frame_method)(element,frame_selector);
 bool okay=utf8 && !strcmp(utf8,"Öffnen …") && role_text && !strcmp(role_text,"AXButton") &&
     frame.size.width==100 && frame.size.height==24 &&
     ((BOOL(*)(id,SEL))objc_msgSend)(element,sel_registerName("isAccessibilityEnabled")) &&
     ((BOOL(*)(id,SEL))objc_msgSend)(element,sel_registerName("accessibilityPerformPress"));
 id text=((id(*)(id,SEL,unsigned long))objc_msgSend)(b->children,sel_registerName("objectAtIndex:"),1);
 const char *value=((const char*(*)(id,SEL))objc_msgSend)(send0(text,"accessibilityValue"),sel_registerName("UTF8String"));
 okay &= value && !strcmp(value,"Bereit 🌍") &&
     !((BOOL(*)(id,SEL))objc_msgSend)(text,sel_registerName("accessibilityPerformPress")) &&
     !((BOOL(*)(id,SEL,SEL))objc_msgSend)(text,sel_registerName("isAccessibilitySelectorAllowed:"),sel_registerName("setAccessibilityValue:"));
 ps_a11y_begin(model);okay &= ps_a11y_record(model,"main","Öffnen …",PS_A11Y_BUTTON,bounds,true);
 ps_a11y_record_state(model,"main","Vektoren",PS_A11Y_CHECKBOX,bounds,true,false);ps_a11y_publish(model);ps_a11y_native_publish(b,true);
 id checkbox=send0(((id(*)(id,SEL,unsigned long))objc_msgSend)(b->children,sel_registerName("objectAtIndex:"),1),"retain");
 const char *checkbox_role=((const char*(*)(id,SEL))objc_msgSend)(send0(checkbox,"accessibilityRole"),sel_registerName("UTF8String"));
 okay &= checkbox_role && !strcmp(checkbox_role,"AXCheckBox") &&
  !((BOOL(*)(id,SEL,SEL))objc_msgSend)(checkbox,sel_registerName("isAccessibilitySelectorAllowed:"),sel_registerName("setAccessibilityValue:")) &&
  !((BOOL(*)(id,SEL))objc_msgSend)(send0(checkbox,"accessibilityValue"),sel_registerName("boolValue")) &&
  ((BOOL(*)(id,SEL))objc_msgSend)(checkbox,sel_registerName("accessibilityPerformPress"));
 ps_a11y_begin(model);okay &= ps_a11y_record_state(model,"main","Vektoren",PS_A11Y_CHECKBOX,bounds,true,false);
 ps_a11y_publish(model);ps_a11y_native_publish(b,true);
 okay &= ((BOOL(*)(id,SEL))objc_msgSend)(send0(checkbox,"accessibilityValue"),sel_registerName("boolValue"));
 ps_a11y_begin(model);ps_a11y_record_state(model,"main","Vektoren",PS_A11Y_CHECKBOX,bounds,false,true);
 ps_a11y_publish(model);ps_a11y_native_publish(b,true);
 okay &= !((BOOL(*)(id,SEL))objc_msgSend)(checkbox,sel_registerName("accessibilityPerformPress"));

 ps_a11y_begin(model);bounds[0]=10;bounds[1]=20;
 ps_a11y_record_option(model,"main","Surface","16 px",bounds,true,true,false);
 bounds[0]=110;ps_a11y_record_option(model,"main","Surface","22 px",bounds,true,false,false);
 bounds[0]=10;bounds[1]=80;ps_a11y_record_option(model,"main","Code","16 px",bounds,true,true,false);
 bounds[0]=110;ps_a11y_record_option(model,"main","Code","22 px",bounds,true,false,false);
 ps_a11y_publish(model);ps_a11y_native_publish(b,true);
 uint64_t group_id=model->nodes[0].id,radio_id=model->nodes[2].id;
 id group_element=element_for(b,group_id),radio=send0(element_for(b,radio_id),"retain");
 const char *group_role=((const char*(*)(id,SEL))objc_msgSend)(send0(group_element,"accessibilityRole"),sel_registerName("UTF8String"));
 const char *radio_role=((const char*(*)(id,SEL))objc_msgSend)(send0(radio,"accessibilityRole"),sel_registerName("UTF8String"));
 okay &= group_role && !strcmp(group_role,"AXRadioGroup") && radio_role && !strcmp(radio_role,"AXRadioButton") &&
  send0(radio,"accessibilityParent")==group_element &&
  ((unsigned long(*)(id,SEL))objc_msgSend)(send0(group_element,"accessibilityChildren"),sel_registerName("count"))==2 &&
  ((unsigned long(*)(id,SEL))objc_msgSend)(send0(group_element,"accessibilitySelectedChildren"),sel_registerName("count"))==1 &&
  ((unsigned long(*)(id,SEL))objc_msgSend)(send0(b->view,"accessibilityChildren"),sel_registerName("count"))==2 &&
  !((BOOL(*)(id,SEL))objc_msgSend)(group_element,sel_registerName("accessibilityPerformPress"));
 CGRect radio_frame=((CGRect(*)(id,SEL))frame_method)(radio,frame_selector);
 CGRect group_frame=((CGRect(*)(id,SEL))frame_method)(group_element,frame_selector);
 okay &= fabs(radio_frame.origin.x-group_frame.origin.x-100)<.01 && radio_frame.size.height==24;
 okay &= ps_a11y_native_press_choice(b,"Surface","22 px");
 ps_a11y_begin(model);bounds[0]=10;bounds[1]=20;
 ps_a11y_record_option(model,"main","Surface","16 px",bounds,true,true,false);bounds[0]=110;
 okay &= ps_a11y_record_option(model,"main","Surface","22 px",bounds,true,false,false);
 bounds[0]=10;bounds[1]=80;ps_a11y_record_option(model,"main","Code","16 px",bounds,true,true,false);
 bounds[0]=110;ps_a11y_record_option(model,"main","Code","22 px",bounds,true,false,false);
 ps_a11y_publish(model);ps_a11y_native_publish(b,true);
 okay &= ((BOOL(*)(id,SEL))objc_msgSend)(send0(radio,"accessibilityValue"),sel_registerName("boolValue")) &&
  !ps_a11y_child_at(model,group_id,0)->checked && ps_a11y_child_at(model,model->nodes[3].id,0)->checked;
 id selected=send0(group_element,"accessibilitySelectedChildren");
 okay &= ((unsigned long(*)(id,SEL))objc_msgSend)(selected,sel_registerName("count"))==1 &&
  ((id(*)(id,SEL,unsigned long))objc_msgSend)(selected,sel_registerName("objectAtIndex:"),0)==radio;
 /* The selected child can leave the visible snapshot while its group remains. */
 ps_a11y_begin(model);bounds[0]=10;bounds[1]=20;
 ps_a11y_record_option(model,"main","Surface","16 px",bounds,true,false,false);
 ps_a11y_publish(model);ps_a11y_native_publish(b,true);
 okay &= ((unsigned long(*)(id,SEL))objc_msgSend)(send0(group_element,"accessibilitySelectedChildren"),sel_registerName("count"))==0 &&
  !((BOOL(*)(id,SEL))objc_msgSend)(radio,sel_registerName("accessibilityPerformPress"));
 ps_a11y_native_destroy(b);okay &= !((BOOL(*)(id,SEL))objc_msgSend)(element,sel_registerName("accessibilityPerformPress"));
 okay &= !((BOOL(*)(id,SEL))objc_msgSend)(checkbox,sel_registerName("accessibilityPerformPress"));
 okay &= !((BOOL(*)(id,SEL))objc_msgSend)(radio,sel_registerName("accessibilityPerformPress"));
 send0(radio,"release");send0(checkbox,"release");send0(element,"release");free(model);return okay;
}
#elif !defined(__linux__)
struct ps_a11y_native {int unused;};
ps_a11y_native *ps_a11y_native_create(SDL_Window *window,ps_a11y_model *model,SDL_Mutex *mutex) {(void)window;(void)model;(void)mutex;return NULL;}
void ps_a11y_native_publish(ps_a11y_native *bridge,bool changed) {(void)bridge;(void)changed;}
void ps_a11y_native_destroy(ps_a11y_native *bridge) {(void)bridge;}
bool ps_a11y_native_test(SDL_Window *window) {(void)window;return false;}
bool ps_a11y_native_press_label(ps_a11y_native *bridge,const char *label) {(void)bridge;(void)label;return false;}
bool ps_a11y_native_press_choice(ps_a11y_native *bridge,const char *group,const char *label) {(void)bridge;(void)group;(void)label;return false;}
bool ps_a11y_native_focus_label(ps_a11y_native *bridge,const char *group,const char *label) {(void)bridge;(void)group;(void)label;return false;}
#endif
