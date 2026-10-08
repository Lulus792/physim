#include "accessibility.h"
#include "text_validation.h"
#include <math.h>
#include <float.h>
#include <string.h>
static uint64_t text_hash(uint64_t hash,const char *text) {
 while(*text) {hash^=(unsigned char)*text++;hash*=UINT64_C(1099511628211);}
 return hash;
}
void ps_a11y_init(ps_a11y_model *m) {memset(m,0,sizeof *m);m->next_id=1;m->keyboard_focus=true;}
void ps_a11y_begin(ps_a11y_model *m) {m->draft_count=0;m->dropped=0;m->selection_parent=0;m->selection_id=0;m->building=true;}
const ps_a11y_node *ps_a11y_find(const ps_a11y_model *m,uint64_t id) {
 if(!id)return NULL;
 for(size_t i=0;i<m->count;i++)if(m->nodes[i].id==id)return &m->nodes[i];
 return NULL;
}
bool ps_a11y_press(ps_a11y_model *m,uint64_t id) {
 const ps_a11y_node *node=ps_a11y_find(m,id);
 if(!node || !ps_a11y_actionable(node->role) || !node->enabled || m->pending_press ||
    (m->pending_focus && m->pending_focus!=id))return false;
 m->pending_press=id;m->pending_focus=id;return true;
}
static bool record(ps_a11y_model *m,const char *window,const char *label,
                     ps_a11y_role role,const float bounds[4],bool enabled,bool checked,uint64_t parent) {
 if(!m->building || !window || !label || !bounds || !*window || !*label ||
    (role!=PS_A11Y_TEXT && role!=PS_A11Y_RADIO_GROUP && !ps_a11y_actionable(role)))return false;
 for(size_t i=0;i<4;i++)if(!isfinite(bounds[i]))return false;
 if(bounds[2]<=0 || bounds[3]<=0 || !ps_text_valid(window,PS_A11Y_WINDOW_BYTES,false))return false;
 /* The UI's longest supported text payload is bounded; do not scan unbounded
  * foreign memory or publish malformed UTF-8 to platform accessibility APIs. */
 if(!ps_text_valid(label,262145,true))return false;
 if(m->draft_count==PS_A11Y_MAX_NODES || !m->next_id){m->dropped++;return false;}
 ps_a11y_node node={0};node.key=text_hash(text_hash(UINT64_C(14695981039346656037),window),label);
 node.role=role;node.parent=parent;node.enabled=enabled;node.focusable=enabled && ps_a11y_actionable(role);node.checked=(role==PS_A11Y_CHECKBOX || role==PS_A11Y_RADIO) && checked;memcpy(node.bounds,bounds,sizeof node.bounds);
 strcpy(node.window,window);
 size_t bytes=strlen(label);if(bytes>=sizeof node.label) {
  bytes=sizeof node.label-1;
  while(((unsigned char)label[bytes]&0xc0)==0x80)bytes--;
 }
 memcpy(node.label,label,bytes);node.label[bytes]=0;
 for(size_t i=0;i<m->draft_count;i++) {
  const ps_a11y_node *other=&m->draft[i];
  if(other->key==node.key && other->role==role && other->parent==parent && !strcmp(other->window,window) &&
     !strcmp(other->label,node.label))node.occurrence++;
 }
 for(size_t i=0;i<m->count;i++) {
  const ps_a11y_node *old=&m->nodes[i];
  if(old->key==node.key && old->role==role && old->parent==parent && old->occurrence==node.occurrence &&
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
 if(role==PS_A11Y_RADIO || role==PS_A11Y_RADIO_GROUP)return false;
 return record(m,window,label,role,bounds,enabled,checked,0);
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
 if(group_index==m->draft_count)record(m,window,group,PS_A11Y_RADIO_GROUP,bounds,enabled,false,0);
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
 bool pressed=record(m,window,label,PS_A11Y_RADIO,bounds,enabled,selected,parent);
 if(m->draft_count==before)return false;
 ps_a11y_node *node=&m->draft[m->draft_count-1];
 if(pressed || (activated && enabled && selected)){m->selection_parent=parent;m->selection_id=node->id;}
 if(m->selection_parent==parent)node->checked=node->id==m->selection_id;
 if(node->checked)for(size_t i=0;i+1<m->draft_count;i++)
     if(m->draft[i].parent==parent)m->draft[i].checked=false;
 return pressed;
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
 if(!node || !node->focusable || !node->enabled || (m->pending_press && m->pending_press!=id) ||
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
 bool changed=m->count!=m->draft_count;
 if(!m->building)return false;
 bool found=false;
 for(size_t i=0;i<m->draft_count;i++)if(m->draft[i].id==m->focused_id && m->draft[i].focusable)found=true;
 if(!found)m->focused_id=0;
 for(size_t i=0;i<m->draft_count;i++)m->draft[i].focused=m->draft[i].id==m->focused_id && m->keyboard_focus;

 for(size_t i=0;i<m->draft_count && !changed;i++) {
  const ps_a11y_node *a=&m->nodes[i],*b=&m->draft[i];
  changed=a->id!=b->id || a->enabled!=b->enabled || a->checked!=b->checked || a->focused!=b->focused || memcmp(a->bounds,b->bounds,sizeof a->bounds)!=0;
 }
 memcpy(m->nodes,m->draft,m->draft_count*sizeof *m->nodes);m->count=m->draft_count;
 /* A dispatcher may queue after this control was already recorded. Keep that
  * activation for its next visit, provided publication still exposes it. */
 const ps_a11y_node *pending=ps_a11y_find(m,m->pending_press);
 if(!pending || !ps_a11y_actionable(pending->role) || !pending->enabled)m->pending_press=0;
 const ps_a11y_node *focus=ps_a11y_find(m,m->pending_focus);
 if(!focus || !focus->focusable)m->pending_focus=0;
 m->building=false;return changed;
}
