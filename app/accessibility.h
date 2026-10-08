#ifndef PS_APP_ACCESSIBILITY_H
#define PS_APP_ACCESSIBILITY_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#define PS_A11Y_MAX_NODES 256u
#define PS_A11Y_LABEL_BYTES 1024u
#define PS_A11Y_WINDOW_BYTES 96u
typedef enum {PS_A11Y_TEXT,PS_A11Y_BUTTON,PS_A11Y_CHECKBOX,PS_A11Y_RADIO,PS_A11Y_RADIO_GROUP} ps_a11y_role;
typedef struct {
 uint64_t id,key,parent;unsigned occurrence;ps_a11y_role role;
 char window[PS_A11Y_WINDOW_BYTES],label[PS_A11Y_LABEL_BYTES];
 float bounds[4];bool enabled,checked,focusable,focused;
} ps_a11y_node;
/* UI thread owns the draft; callers serialize all operations. Published nodes
 * are immutable until publish. IDs persist across geometry changes, expire when
 * controls disappear, and never get reused during this model's lifetime. */
typedef struct {
 ps_a11y_node nodes[PS_A11Y_MAX_NODES],draft[PS_A11Y_MAX_NODES];
 size_t count,draft_count;uint64_t next_id,pending_press,selection_parent,selection_id,pending_focus,focused_id;unsigned dropped;
 bool building,keyboard_focus;
} ps_a11y_model;
static inline bool ps_a11y_actionable(ps_a11y_role role) {
 return role==PS_A11Y_BUTTON || role==PS_A11Y_CHECKBOX || role==PS_A11Y_RADIO;
}
void ps_a11y_init(ps_a11y_model *model);
void ps_a11y_begin(ps_a11y_model *model);
/* Visible, clipped bounds in SDL logical window coordinates; positive sizes.
 * Copies UTF-8, clipping long labels at scalar boundaries. Invalid names/bounds
 * are omitted. A true return consumes an enabled queued button activation. */
bool ps_a11y_record(ps_a11y_model *model,const char *window,const char *label,
                     ps_a11y_role role,const float bounds[4],bool enabled);
/* Checkbox snapshots include the supplied state and reflect one accepted toggle. */
bool ps_a11y_record_state(ps_a11y_model *model,const char *window,const char *label,
                     ps_a11y_role role,const float bounds[4],bool enabled,bool checked);
bool ps_a11y_record_option(ps_a11y_model *model,const char *window,const char *group,
                     const char *label,const float bounds[4],bool enabled,bool selected,bool activated);
size_t ps_a11y_child_count(const ps_a11y_model *model,uint64_t parent);
const ps_a11y_node *ps_a11y_child_at(const ps_a11y_model *model,uint64_t parent,size_t index);
size_t ps_a11y_index(const ps_a11y_model *model,uint64_t id);
bool ps_a11y_publish(ps_a11y_model *model);
const ps_a11y_node *ps_a11y_find(const ps_a11y_model *model,uint64_t id);
/* Queue at most one press, delivered only on a live, enabled matching control
 * next frame. Stale, disabled and text nodes cannot activate anything. */
bool ps_a11y_press(ps_a11y_model *model,uint64_t id);
bool ps_a11y_focus(ps_a11y_model *model,uint64_t id);
void ps_a11y_blur(ps_a11y_model *model,uint64_t id);
/* direction is -1/+1; Tab stops visit only the selected radio in a group. */
bool ps_a11y_focus_move(ps_a11y_model *model,int direction,bool radio_only);

#endif
