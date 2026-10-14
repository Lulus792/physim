#include "accessibility.h"
#include "text_validation.h"
#include <math.h>
#include <float.h>
#include <string.h>
#include <stdlib.h>
static uint64_t text_hash(uint64_t hash,const char *text) {
 while(*text) {hash^=(unsigned char)*text++;hash*=UINT64_C(1099511628211);}
 return hash;
}
void ps_a11y_init(ps_a11y_model *m) {memset(m,0,sizeof *m);m->next_id=1;m->next_text_version=1;m->keyboard_focus=true;}
void ps_a11y_destroy(ps_a11y_model *m) {
 if(!m)return;
 if(m->document){free(m->document->text);free(m->document);}
 if(m->document_draft){free(m->document_draft->text);free(m->document_draft);}
 m->document=m->document_draft=NULL;m->document_scroll_version=0;
}
void ps_a11y_begin(ps_a11y_model *m) {
 m->draft_count=0;m->edit_box_draft_count=0;m->dropped=0;m->selection_parent=0;m->selection_id=0;m->building=true;
 if(m->document_draft){m->document_draft->active=false;m->document_draft->count=0;m->document_draft->dropped=0;}
}
void ps_a11y_suspend(ps_a11y_model *m) {
 if(!m)return;m->suspended=true;m->keyboard_focus=false;m->focused_id=0;
 m->pending_press=m->pending_focus=m->pending_value_id=m->pending_text_id=0;
 m->pending_selection_id=m->pending_edit_scroll_id=m->document_scroll_version=0;
}
static bool text_boundary(const char *text,size_t length,size_t at) {
 return at<=length && (at==length || ((unsigned char)text[at]&0xc0)!=0x80);
}
bool ps_a11y_document_begin(ps_a11y_model *m,uint64_t version,const char *name,const char *text,const float viewport[4]) {
 if(!m || !m->building || !version || !name || !*name || !text || !viewport ||
    !ps_text_valid(name,256,false) || !ps_text_valid(text,PS_A11Y_DOCUMENT_BYTES+1,true))return false;
 for(unsigned i=0;i<4;i++)if(!isfinite(viewport[i]))return false;
 if(viewport[2]<=0 || viewport[3]<=0)return false;
 /* A version identifies content, independently of scrolling or glyph geometry. */
 if(m->document && m->document->version==version &&
    (strcmp(m->document->name,name) || strcmp(m->document->text,text)))return false;
 ps_a11y_document *draft=m->document_draft;
 if(draft && draft->version==version &&
    (strcmp(draft->name,name) || strcmp(draft->text,text)))return false;
 char *copy=NULL;bool new_text=!draft || draft->version!=version;
 if(new_text){size_t length=strlen(text);copy=malloc(length+1);if(!copy)return false;memcpy(copy,text,length+1);}
 if(!draft){draft=calloc(1,sizeof *draft);if(!draft){free(copy);return false;}m->document_draft=draft;}
 if(new_text){free(draft->text);draft->text=copy;draft->length=strlen(text);draft->version=version;strcpy(draft->name,name);}
 memcpy(draft->viewport,viewport,sizeof draft->viewport);draft->count=0;draft->dropped=0;draft->active=true;return true;
}
bool ps_a11y_document_box(ps_a11y_model *m,size_t begin,size_t end,const float bounds[4]) {
 ps_a11y_document *d=m && m->building?m->document_draft:NULL;
 if(!d || !d->active || !bounds || begin>=end || !text_boundary(d->text,d->length,begin) ||
    !text_boundary(d->text,d->length,end))return false;
 for(unsigned i=0;i<4;i++)if(!isfinite(bounds[i]))return false;
 if(bounds[2]<=0 || bounds[3]<=0)return false;
 float left=fmaxf(bounds[0],d->viewport[0]),top=fmaxf(bounds[1],d->viewport[1]);
 float right=fminf(bounds[0]+bounds[2],d->viewport[0]+d->viewport[2]);
 float bottom=fminf(bounds[1]+bounds[3],d->viewport[1]+d->viewport[3]);
 if(right<=left || bottom<=top)return false;
 if(d->count==PS_A11Y_TEXT_BOXES){d->dropped++;return false;}
 d->boxes[d->count++]=(ps_a11y_text_box){begin,end,{left,top,right-left,bottom-top}};return true;
}
bool ps_a11y_document_scroll(ps_a11y_model *m,uint64_t version,size_t byte) {
 const ps_a11y_document *d=m?m->document:NULL;
 if(!d || !d->active || m->suspended || !version || d->version!=version || m->document_scroll_version ||
    !text_boundary(d->text,d->length,byte))return false;
 m->document_scroll_version=version;m->document_scroll_byte=byte;m->document_scroll_top=true;return true;
}
bool ps_a11y_document_scroll_align(ps_a11y_model *m,uint64_t version,size_t byte,bool top) {
 if(!ps_a11y_document_scroll(m,version,byte))return false;m->document_scroll_top=top;return true;
}
bool ps_a11y_document_preserve_position(ps_a11y_model *m,uint64_t version) {
 const ps_a11y_document *d=m?m->document:NULL;
 if(!d || !d->active || !version || d->version!=version || !d->count)return false;
 size_t byte=d->boxes[0].begin;
 for(size_t i=1;i<d->count;i++)if(d->boxes[i].begin<byte)byte=d->boxes[i].begin;
 return ps_a11y_document_scroll_align(m,version,byte,true);
}
bool ps_a11y_document_take_scroll(ps_a11y_model *m,uint64_t version,size_t *byte) {
 if(!m || !byte || !version || version!=m->document_scroll_version)return false;
 *byte=m->document_scroll_byte;m->document_scroll_version=0;return true;
}
const ps_a11y_node *ps_a11y_find(const ps_a11y_model *m,uint64_t id) {
 if(!id)return NULL;
 for(size_t i=0;i<m->count;i++)if(m->nodes[i].id==id)return &m->nodes[i];
 return NULL;
}
bool ps_a11y_press(ps_a11y_model *m,uint64_t id) {
 const ps_a11y_node *node=ps_a11y_find(m,id);
 if(!node || m->suspended || !ps_a11y_actionable(node->role) || !node->enabled || m->pending_press ||
    (m->pending_focus && m->pending_focus!=id) || m->pending_value_id || m->pending_text_id || m->pending_selection_id)return false;
 m->pending_press=id;m->pending_focus=id;return true;
}
static bool record(ps_a11y_model *m,const char *window,const char *label,
                     ps_a11y_role role,const float bounds[4],bool enabled,bool checked,uint64_t parent,
                     uint64_t version,size_t begin,size_t end) {
 if(!m->building || !window || !label || !bounds || !*window || !*label ||
    (role!=PS_A11Y_TEXT && role!=PS_A11Y_RADIO_GROUP && role!=PS_A11Y_SLIDER && role!=PS_A11Y_EDIT && !ps_a11y_actionable(role)))return false;
 for(size_t i=0;i<4;i++)if(!isfinite(bounds[i]))return false;
 bool offscreen=role==PS_A11Y_LINK && bounds[2]==0 && bounds[3]==0;
 if((!offscreen && (bounds[2]<=0 || bounds[3]<=0)) || !ps_text_valid(window,PS_A11Y_WINDOW_BYTES,false))return false;
 /* The UI's longest supported text payload is bounded; do not scan unbounded
  * foreign memory or publish malformed UTF-8 to platform accessibility APIs. */
 if(!ps_text_valid(label,262145,true))return false;
 if(m->draft_count==PS_A11Y_MAX_NODES || !m->next_id){m->dropped++;return false;}
 ps_a11y_node node={0};node.key=text_hash(text_hash(UINT64_C(14695981039346656037),window),label);
 node.document_version=version;node.document_begin=begin;node.document_end=end;node.offscreen=offscreen;
 if(version){node.key=(node.key^version)*UINT64_C(1099511628211);node.key=(node.key^begin)*UINT64_C(1099511628211);node.key=(node.key^end)*UINT64_C(1099511628211);}
 node.role=role;node.parent=parent;node.enabled=enabled;node.focusable=enabled && (ps_a11y_actionable(role) || role==PS_A11Y_SLIDER || role==PS_A11Y_EDIT);node.checked=(role==PS_A11Y_CHECKBOX || role==PS_A11Y_RADIO) && checked;memcpy(node.bounds,bounds,sizeof node.bounds);
 strcpy(node.window,window);
 size_t bytes=strlen(label);if(bytes>=sizeof node.label) {
  bytes=sizeof node.label-1;
  while(((unsigned char)label[bytes]&0xc0)==0x80)bytes--;
 }
 memcpy(node.label,label,bytes);node.label[bytes]=0;
 for(size_t i=0;i<m->draft_count;i++) {
  const ps_a11y_node *other=&m->draft[i];
  if(other->key==node.key && other->role==role && other->parent==parent &&
     other->document_version==version && other->document_begin==begin && other->document_end==end && !strcmp(other->window,window) &&
     !strcmp(other->label,node.label))node.occurrence++;
 }
 for(size_t i=0;i<m->count;i++) {
  const ps_a11y_node *old=&m->nodes[i];
  if(old->key==node.key && old->role==role && old->parent==parent && old->occurrence==node.occurrence &&
     old->document_version==version && old->document_begin==begin && old->document_end==end &&
     !strcmp(old->window,window) && !strcmp(old->label,node.label)) {node.id=old->id;break;}
 }
 if(!node.id)node.id=m->next_id++;
 if(m->pending_focus==node.id) {
  m->pending_focus=0;if(node.focusable)m->focused_id=node.id;
 }
 if(node.id==m->focused_id && !node.focusable)m->focused_id=0;
 node.focused=node.id==m->focused_id && m->keyboard_focus;
 m->draft[m->draft_count++]=node;
 if(m->pending_press==node.id) {
  m->pending_press=0;
  if(role==PS_A11Y_CHECKBOX && enabled)m->draft[m->draft_count-1].checked=!checked;
  return ps_a11y_actionable(role) && enabled;
 }
 return false;
}
bool ps_a11y_record_state(ps_a11y_model *m,const char *window,const char *label,
                     ps_a11y_role role,const float bounds[4],bool enabled,bool checked) {
 /* Radio nodes need a named parent; only record_option creates these. */
 if(role==PS_A11Y_RADIO || role==PS_A11Y_RADIO_GROUP || role==PS_A11Y_SLIDER || role==PS_A11Y_EDIT)return false;
 if(role==PS_A11Y_LINK)return false; /* Must bind to a document range. */
 return record(m,window,label,role,bounds,enabled,checked,0,0,0,0);
}
bool ps_a11y_document_link(ps_a11y_model *m,const char *window,const char *name,
                          uint64_t version,size_t begin,size_t end,const float bounds[4],bool enabled) {
 const ps_a11y_document *d=m && m->building?m->document_draft:NULL;
 if(!d || !d->active || d->version!=version || begin>=end ||
    !text_boundary(d->text,d->length,begin) || !text_boundary(d->text,d->length,end))return false;
 return record(m,window,name,PS_A11Y_LINK,bounds,enabled,false,0,version,begin,end);
}
bool ps_a11y_document_link_target(ps_a11y_model *m,size_t index,const char *target) {
 if(!m || !m->building || index>=m->draft_count || m->draft[index].role!=PS_A11Y_LINK ||
    !target || !ps_text_valid(target,PS_A11Y_LABEL_BYTES,false))return false;
 memcpy(m->draft[index].document_target,target,strlen(target)+1);return true;
}
bool ps_a11y_record_option(ps_a11y_model *m,const char *window,const char *group,
                     const char *label,const float bounds[4],bool enabled,bool selected,bool activated) {
 if(!m->building || !window || !group || !label || !bounds || !*group || !*label ||
    !ps_text_valid(group,PS_A11Y_LABEL_BYTES,false) || !ps_text_valid(label,PS_A11Y_LABEL_BYTES,false))return false;
 for(size_t i=0;i<4;i++)if(!isfinite(bounds[i]))return false;
 if(bounds[2]<=0 || bounds[3]<=0 || !*window || !ps_text_valid(window,PS_A11Y_WINDOW_BYTES,false))return false;
 size_t group_index=m->draft_count;
 for(size_t i=0;i<m->draft_count;i++)if(m->draft[i].role==PS_A11Y_RADIO_GROUP &&
     !strcmp(m->draft[i].window,window) && !strcmp(m->draft[i].label,group)){group_index=i;break;}
 if(m->draft_count+(group_index==m->draft_count?2:1)>PS_A11Y_MAX_NODES ||
    !m->next_id || (group_index==m->draft_count && m->next_id==UINT64_MAX)){m->dropped++;return false;}
 if(group_index==m->draft_count)record(m,window,group,PS_A11Y_RADIO_GROUP,bounds,enabled,false,0,0,0,0);
 else {
  ps_a11y_node *g=&m->draft[group_index];
  double left=fmin(g->bounds[0],bounds[0]),top=fmin(g->bounds[1],bounds[1]);
  double right=fmax((double)g->bounds[0]+g->bounds[2],(double)bounds[0]+bounds[2]);
  double bottom=fmax((double)g->bounds[1]+g->bounds[3],(double)bounds[1]+bounds[3]);
  if(right-left>FLT_MAX || bottom-top>FLT_MAX)return false;
  g->bounds[0]=(float)left;g->bounds[1]=(float)top;g->bounds[2]=(float)(right-left);g->bounds[3]=(float)(bottom-top);g->enabled|=enabled;
 }
 uint64_t parent=m->draft[group_index].id;
 size_t before=m->draft_count;
 bool pressed=record(m,window,label,PS_A11Y_RADIO,bounds,enabled,selected,parent,0,0,0);
 if(m->draft_count==before)return false;
 ps_a11y_node *node=&m->draft[m->draft_count-1];
 if(pressed || (activated && enabled && selected)){m->selection_parent=parent;m->selection_id=node->id;}
 if(m->selection_parent==parent)node->checked=node->id==m->selection_id;
 if(node->checked)for(size_t i=0;i+1<m->draft_count;i++)
     if(m->draft[i].parent==parent)m->draft[i].checked=false;
 return pressed;
}
bool ps_a11y_record_range(ps_a11y_model *m,const char *window,const char *label,
                         const float bounds[4],bool enabled,double minimum,double maximum,
                         double increment,double *value) {
 if(!value || !isfinite(*value) || !isfinite(minimum) || !isfinite(maximum) ||
    !isfinite(increment) || increment<=0 || maximum<minimum || *value<minimum || *value>maximum)return false;
 size_t before=m->draft_count;
 record(m,window,label,PS_A11Y_SLIDER,bounds,enabled,false,0,0,0,0);
 if(m->draft_count==before)return false;
 ps_a11y_node *node=&m->draft[before];bool changed=false;
 if(node->id==m->pending_value_id) {
  if(enabled && m->pending_value>=minimum && m->pending_value<=maximum) {
   changed=*value!=m->pending_value;*value=m->pending_value;
  }
  m->pending_value_id=0;
 }
 node->value=*value;node->minimum=minimum;node->maximum=maximum;node->increment=increment;
 return changed;
}
bool ps_a11y_set_value(ps_a11y_model *m,uint64_t id,double value) {
 const ps_a11y_node *node=ps_a11y_find(m,id);
 if(!node || m->suspended || node->role!=PS_A11Y_SLIDER || !node->enabled || !isfinite(value) ||
    value<node->minimum || value>node->maximum || m->pending_value_id || m->pending_press || m->pending_text_id || m->pending_selection_id ||
    (m->pending_focus && m->pending_focus!=id))return false;
 m->pending_value_id=id;m->pending_value=value;m->pending_focus=id;return true;
}
bool ps_a11y_adjust(ps_a11y_model *m,uint64_t id,int direction) {
 const ps_a11y_node *node=ps_a11y_find(m,id);
 if(!node || node->role!=PS_A11Y_SLIDER || (direction!=-1 && direction!=1))return false;
 double value=direction>0?fmin(node->maximum,node->value+node->increment):fmax(node->minimum,node->value-node->increment);
 return ps_a11y_set_value(m,id,value);
}
bool ps_a11y_record_edit(ps_a11y_model *m,const char *window,const char *name,
                       const float bounds[4],bool enabled,char *text,size_t capacity) {
 if(!m || !text || capacity<2 || capacity>PS_A11Y_LABEL_BYTES || !ps_text_valid(text,capacity,false))return false;
 size_t before=m->draft_count;record(m,window,name,PS_A11Y_EDIT,bounds,enabled,false,0,0,0,0);
 if(m->draft_count==before)return false;ps_a11y_node *node=&m->draft[before];bool changed=false;
 if(node->id==m->pending_text_id) {
  if(enabled && ps_text_valid(m->pending_text,capacity,false)) {
   changed=strcmp(text,m->pending_text)!=0;strcpy(text,m->pending_text);
  }
  m->pending_text_id=0;
 }
 node->text_capacity=capacity;node->box_offset=m->edit_box_draft_count;
 const ps_a11y_node *old=ps_a11y_find(m,node->id);
 if(old){node->text_version=old->text_version;node->text_caret=old->text_caret;node->text_scroll=old->text_scroll;strcpy(node->text_value,old->text_value);}
 ps_a11y_edit_update(m,before,text,old?old->text_start:strlen(text),old?old->text_end:strlen(text));return changed;
}
bool ps_a11y_set_text(ps_a11y_model *m,uint64_t id,const char *text) {
 const ps_a11y_node *node=m?ps_a11y_find(m,id):NULL;
 if(!node || m->suspended || node->role!=PS_A11Y_EDIT || !node->enabled || !text ||
    !ps_text_valid(text,node->text_capacity,false) || m->pending_text_id || m->pending_selection_id || m->pending_press || m->pending_value_id ||
    (m->pending_focus && m->pending_focus!=id))return false;
 strcpy(m->pending_text,text);m->pending_text_id=id;return true;
}
bool ps_a11y_edit_update(ps_a11y_model *m,size_t index,const char *text,size_t start,size_t end) {
 if(!m || !m->building || index>=m->draft_count)return false;ps_a11y_node *node=&m->draft[index];
 if(node->role!=PS_A11Y_EDIT || !text || !ps_text_valid(text,node->text_capacity,false))return false;
 size_t length=strlen(text);bool changed=!node->text_version || strcmp(node->text_value,text);
 if(changed){if(!m->next_text_version)return false;node->text_version=m->next_text_version++;strcpy(node->text_value,text);node->text_caret=length;node->text_scroll=0;}
 if(!text_boundary(text,length,start) || !text_boundary(text,length,end)){if(!changed)return false;start=end=length;}
 node->text_start=start<end?start:end;node->text_end=start<end?end:start;return true;
}
bool ps_a11y_edit_select(ps_a11y_model *m,uint64_t id,uint64_t version,size_t start,size_t end) {
 const ps_a11y_node *node=m?ps_a11y_find(m,id):NULL;
 if(!node || m->suspended || node->role!=PS_A11Y_EDIT || !node->enabled || !version || node->text_version!=version ||
    m->pending_selection_id || m->pending_text_id || m->pending_press || m->pending_value_id ||
    (m->pending_focus && m->pending_focus!=id) || !text_boundary(node->text_value,strlen(node->text_value),start) ||
    !text_boundary(node->text_value,strlen(node->text_value),end))return false;
 m->pending_selection_id=id;m->pending_selection_version=version;m->pending_selection_start=start;m->pending_selection_end=end;
 m->pending_focus=id;return true;
}
bool ps_a11y_edit_take_selection(ps_a11y_model *m,size_t index,size_t *start,size_t *end) {
 if(!m || !m->building || index>=m->draft_count || !start || !end)return false;
 ps_a11y_node *node=&m->draft[index];if(node->id!=m->pending_selection_id)return false;
 bool valid=node->enabled && node->text_version==m->pending_selection_version;
 if(valid){*start=m->pending_selection_start;*end=m->pending_selection_end;}m->pending_selection_id=0;return valid;
}
bool ps_a11y_edit_box(ps_a11y_model *m,size_t index,size_t begin,size_t end,const float bounds[4]) {
 if(!m || !m->building || index>=m->draft_count || !bounds)return false;ps_a11y_node *node=&m->draft[index];
 if(node->role!=PS_A11Y_EDIT || begin>=end || !text_boundary(node->text_value,strlen(node->text_value),begin) ||
    !text_boundary(node->text_value,strlen(node->text_value),end))return false;
 for(unsigned i=0;i<4;i++)if(!isfinite(bounds[i]))return false;
 float left=fmaxf(bounds[0],node->bounds[0]),top=fmaxf(bounds[1],node->bounds[1]);
 float right=fminf(bounds[0]+bounds[2],node->bounds[0]+node->bounds[2]),bottom=fminf(bounds[1]+bounds[3],node->bounds[1]+node->bounds[3]);
 if(right<=left || bottom<=top)return false;
 if(m->edit_box_draft_count==PS_A11Y_EDIT_BOXES){m->dropped++;return false;}
 if(node->box_offset+node->box_count!=m->edit_box_draft_count)return false;
 m->edit_boxes_draft[m->edit_box_draft_count++]=(ps_a11y_text_box){begin,end,{left,top,right-left,bottom-top}};node->box_count++;return true;
}
bool ps_a11y_edit_scroll(ps_a11y_model *m,uint64_t id,uint64_t version,size_t start,size_t end,bool top) {
 const ps_a11y_node *node=m?ps_a11y_find(m,id):NULL;
 if(!node || m->suspended || node->role!=PS_A11Y_EDIT || !node->enabled || !version || node->text_version!=version ||
    m->pending_edit_scroll_id || start>end || !text_boundary(node->text_value,strlen(node->text_value),start) ||
    !text_boundary(node->text_value,strlen(node->text_value),end))return false;
 m->pending_edit_scroll_id=id;m->pending_edit_scroll_version=version;
 m->pending_edit_scroll_start=start;m->pending_edit_scroll_end=end;m->pending_edit_scroll_top=top;return true;
}
bool ps_a11y_edit_take_scroll(ps_a11y_model *m,size_t index,size_t *start,size_t *end,bool *top) {
 if(!m || !m->building || index>=m->draft_count || !start || !end || !top)return false;
 ps_a11y_node *node=&m->draft[index];if(node->id!=m->pending_edit_scroll_id)return false;
 bool valid=node->enabled && node->text_version==m->pending_edit_scroll_version;
 if(valid){*start=m->pending_edit_scroll_start;*end=m->pending_edit_scroll_end;*top=m->pending_edit_scroll_top;}
 m->pending_edit_scroll_id=0;return valid;
}
size_t ps_a11y_child_count(const ps_a11y_model *m,uint64_t parent) {
 size_t count=0;for(size_t i=0;i<m->count;i++)if(m->nodes[i].parent==parent)count++;return count;
}
const ps_a11y_node *ps_a11y_child_at(const ps_a11y_model *m,uint64_t parent,size_t index) {
 for(size_t i=0;i<m->count;i++)if(m->nodes[i].parent==parent){if(!index)return &m->nodes[i];index--;}
 return NULL;
}
size_t ps_a11y_index(const ps_a11y_model *m,uint64_t id) {
 const ps_a11y_node *node=ps_a11y_find(m,id);if(!node)return SIZE_MAX;
 size_t index=0;for(size_t i=0;i<m->count && m->nodes[i].id!=id;i++)if(m->nodes[i].parent==node->parent)index++;
 return index;
}
bool ps_a11y_record(ps_a11y_model *m,const char *window,const char *label,
                     ps_a11y_role role,const float bounds[4],bool enabled) {
 return ps_a11y_record_state(m,window,label,role,bounds,enabled,false);
}
bool ps_a11y_focus(ps_a11y_model *m,uint64_t id) {
 const ps_a11y_node *node=ps_a11y_find(m,id);
 if(!node || m->suspended || !node->focusable || !node->enabled || (m->pending_press && m->pending_press!=id) ||
    (m->pending_value_id && m->pending_value_id!=id) ||
    (m->pending_text_id && m->pending_text_id!=id) || (m->pending_selection_id && m->pending_selection_id!=id) ||
    (m->pending_focus && m->pending_focus!=id))return false;
 m->pending_focus=id;return true;
}
void ps_a11y_blur(ps_a11y_model *m,uint64_t id) {
 if(!id || m->focused_id==id)m->focused_id=0;
 if(!id || m->pending_focus==id)m->pending_focus=0;
}
bool ps_a11y_focus_move(ps_a11y_model *m,int direction,bool radio_only) {
 const ps_a11y_node *current=ps_a11y_find(m,m->focused_id);
 if(!current || (direction!=-1 && direction!=1))return false;
 size_t start=(size_t)(current-m->nodes);
 for(size_t offset=1;offset<=m->count;offset++) {
  size_t index=direction>0?(start+offset)%m->count:(start+m->count-offset)%m->count;
  const ps_a11y_node *next=&m->nodes[index];if(!next->focusable || !next->enabled)continue;
  if(radio_only && (next->role!=PS_A11Y_RADIO || next->parent!=current->parent))continue;
  if(!radio_only && next->role==PS_A11Y_RADIO && !next->checked) {
   bool selected=false;for(size_t j=0;j<m->count;j++)if(m->nodes[j].parent==next->parent && m->nodes[j].checked)selected=true;
   if(selected || ps_a11y_child_at(m,next->parent,0)!=next)continue;
  }
  return radio_only?ps_a11y_press(m,next->id):ps_a11y_focus(m,next->id);
 }
 return false;
}
bool ps_a11y_publish(ps_a11y_model *m) {
 bool changed=m->suspended || m->count!=m->draft_count;
 if(!m->building)return false;
 m->suspended=false;
 ps_a11y_document *old=m->document,*next=m->document_draft;
 bool old_active=old && old->active,next_active=next && next->active;
 if(old_active!=next_active || (next_active && (old->version!=next->version || old->count!=next->count ||
    memcmp(old->viewport,next->viewport,sizeof next->viewport) || memcmp(old->boxes,next->boxes,next->count*sizeof *next->boxes))))changed=true;
 m->document=next;m->document_draft=old;
 if(!next_active || next->version!=m->document_scroll_version)m->document_scroll_version=0;
 bool found=false;
 for(size_t i=0;i<m->draft_count;i++)if(m->draft[i].id==m->focused_id && m->draft[i].focusable)found=true;
 if(!found)m->focused_id=0;
 for(size_t i=0;i<m->draft_count;i++)m->draft[i].focused=m->draft[i].id==m->focused_id && m->keyboard_focus;

 for(size_t i=0;i<m->draft_count && !changed;i++) {
  const ps_a11y_node *a=&m->nodes[i],*b=&m->draft[i];
 changed=a->id!=b->id || a->enabled!=b->enabled || a->checked!=b->checked || a->focused!=b->focused ||
     a->value!=b->value || a->minimum!=b->minimum || a->maximum!=b->maximum || a->increment!=b->increment ||
     a->text_capacity!=b->text_capacity || a->text_version!=b->text_version || a->text_start!=b->text_start || a->text_end!=b->text_end || a->text_caret!=b->text_caret || a->text_active!=b->text_active ||
     a->box_count!=b->box_count || a->text_scroll!=b->text_scroll || a->offscreen!=b->offscreen ||
     a->document_version!=b->document_version || a->document_begin!=b->document_begin || a->document_end!=b->document_end ||
     strcmp(a->text_value,b->text_value) || memcmp(a->bounds,b->bounds,sizeof a->bounds)!=0;
 }
 memcpy(m->nodes,m->draft,m->draft_count*sizeof *m->nodes);m->count=m->draft_count;
 if(m->edit_box_count!=m->edit_box_draft_count || memcmp(m->edit_boxes,m->edit_boxes_draft,m->edit_box_draft_count*sizeof *m->edit_boxes))changed=true;
 memcpy(m->edit_boxes,m->edit_boxes_draft,m->edit_box_draft_count*sizeof *m->edit_boxes);m->edit_box_count=m->edit_box_draft_count;
 /* A dispatcher may queue after this control was already recorded. Keep that
  * activation for its next visit, provided publication still exposes it. */
 const ps_a11y_node *pending=ps_a11y_find(m,m->pending_press);
 if(!pending || !ps_a11y_actionable(pending->role) || !pending->enabled)m->pending_press=0;
 const ps_a11y_node *focus=ps_a11y_find(m,m->pending_focus);
 if(!focus || !focus->focusable)m->pending_focus=0;
 const ps_a11y_node *range=ps_a11y_find(m,m->pending_value_id);
 if(!range || range->role!=PS_A11Y_SLIDER || !range->enabled || m->pending_value<range->minimum || m->pending_value>range->maximum)m->pending_value_id=0;
 const ps_a11y_node *edit=ps_a11y_find(m,m->pending_text_id);
 if(!edit || edit->role!=PS_A11Y_EDIT || !edit->enabled || !ps_text_valid(m->pending_text,edit->text_capacity,false))m->pending_text_id=0;
 const ps_a11y_node *selected=ps_a11y_find(m,m->pending_selection_id);
 if(!selected || selected->role!=PS_A11Y_EDIT || !selected->enabled || selected->text_version!=m->pending_selection_version)m->pending_selection_id=0;
 const ps_a11y_node *scrolled=ps_a11y_find(m,m->pending_edit_scroll_id);
 if(!scrolled || scrolled->role!=PS_A11Y_EDIT || !scrolled->enabled || scrolled->text_version!=m->pending_edit_scroll_version)m->pending_edit_scroll_id=0;
 m->building=false;return changed;
}
