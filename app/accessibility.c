#include "accessibility.h"
#include "text_validation.h"
#include <math.h>
#include <string.h>
static uint64_t text_hash(uint64_t hash,const char *text) {
 while(*text) {hash^=(unsigned char)*text++;hash*=UINT64_C(1099511628211);}
 return hash;
}
void ps_a11y_init(ps_a11y_model *m) {memset(m,0,sizeof *m);m->next_id=1;}
void ps_a11y_begin(ps_a11y_model *m) {m->draft_count=0;m->dropped=0;m->building=true;}
const ps_a11y_node *ps_a11y_find(const ps_a11y_model *m,uint64_t id) {
 if(!id)return NULL;
 for(size_t i=0;i<m->count;i++)if(m->nodes[i].id==id)return &m->nodes[i];
 return NULL;
}
bool ps_a11y_press(ps_a11y_model *m,uint64_t id) {
 const ps_a11y_node *node=ps_a11y_find(m,id);
 if(!node || !ps_a11y_actionable(node->role) || !node->enabled || m->pending_press)return false;
 m->pending_press=id;return true;
}
bool ps_a11y_record_state(ps_a11y_model *m,const char *window,const char *label,
                     ps_a11y_role role,const float bounds[4],bool enabled,bool checked) {
 if(!m->building || !window || !label || !bounds || !*window || !*label ||
    (role!=PS_A11Y_TEXT && !ps_a11y_actionable(role)))return false;
 for(size_t i=0;i<4;i++)if(!isfinite(bounds[i]))return false;
 if(bounds[2]<=0 || bounds[3]<=0 || !ps_text_valid(window,PS_A11Y_WINDOW_BYTES,false))return false;
 /* The UI's longest supported text payload is bounded; do not scan unbounded
  * foreign memory or publish malformed UTF-8 to platform accessibility APIs. */
 if(!ps_text_valid(label,262145,true))return false;
 if(m->draft_count==PS_A11Y_MAX_NODES || !m->next_id){m->dropped++;return false;}
 ps_a11y_node node={0};node.key=text_hash(text_hash(UINT64_C(14695981039346656037),window),label);
 node.role=role;node.enabled=enabled;node.checked=role==PS_A11Y_CHECKBOX && checked;memcpy(node.bounds,bounds,sizeof node.bounds);
 strcpy(node.window,window);
 size_t bytes=strlen(label);if(bytes>=sizeof node.label) {
  bytes=sizeof node.label-1;
  while(((unsigned char)label[bytes]&0xc0)==0x80)bytes--;
 }
 memcpy(node.label,label,bytes);node.label[bytes]=0;
 for(size_t i=0;i<m->draft_count;i++) {
  const ps_a11y_node *other=&m->draft[i];
  if(other->key==node.key && other->role==role && !strcmp(other->window,window) &&
     !strcmp(other->label,node.label))node.occurrence++;
 }
 for(size_t i=0;i<m->count;i++) {
  const ps_a11y_node *old=&m->nodes[i];
  if(old->key==node.key && old->role==role && old->occurrence==node.occurrence &&
     !strcmp(old->window,window) && !strcmp(old->label,node.label)) {node.id=old->id;break;}
 }
 if(!node.id)node.id=m->next_id++;
 m->draft[m->draft_count++]=node;
 if(m->pending_press==node.id) {
  m->pending_press=0;
  if(role==PS_A11Y_CHECKBOX && enabled)m->draft[m->draft_count-1].checked=!checked;
  return ps_a11y_actionable(role) && enabled;
 }
 return false;
}
bool ps_a11y_record(ps_a11y_model *m,const char *window,const char *label,
                     ps_a11y_role role,const float bounds[4],bool enabled) {
 return ps_a11y_record_state(m,window,label,role,bounds,enabled,false);
}
bool ps_a11y_publish(ps_a11y_model *m) {
 bool changed=m->count!=m->draft_count;
 if(!m->building)return false;
 for(size_t i=0;i<m->draft_count && !changed;i++) {
  const ps_a11y_node *a=&m->nodes[i],*b=&m->draft[i];
  changed=a->id!=b->id || a->enabled!=b->enabled || a->checked!=b->checked || memcmp(a->bounds,b->bounds,sizeof a->bounds)!=0;
 }
 memcpy(m->nodes,m->draft,m->draft_count*sizeof *m->nodes);m->count=m->draft_count;
 /* A dispatcher may queue after this control was already recorded. Keep that
  * activation for its next visit, provided publication still exposes it. */
 const ps_a11y_node *pending=ps_a11y_find(m,m->pending_press);
 if(!pending || !ps_a11y_actionable(pending->role) || !pending->enabled)m->pending_press=0;
 m->building=false;return changed;
}
