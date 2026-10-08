#ifndef PS_APP_ACCESSIBILITY_H
#define PS_APP_ACCESSIBILITY_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#define PS_A11Y_MAX_NODES 256u
#define PS_A11Y_LABEL_BYTES 1024u
#define PS_A11Y_WINDOW_BYTES 96u
typedef enum {PS_A11Y_TEXT,PS_A11Y_BUTTON,PS_A11Y_CHECKBOX} ps_a11y_role;
typedef struct {
 uint64_t id,key;unsigned occurrence;ps_a11y_role role;
 char window[PS_A11Y_WINDOW_BYTES],label[PS_A11Y_LABEL_BYTES];
 float bounds[4];bool enabled,checked;
} ps_a11y_node;
/* UI thread owns the draft; callers serialize all operations. Published nodes
 * are immutable until publish. IDs persist across geometry changes, expire when
 * controls disappear, and never get reused during this model's lifetime. */
typedef struct {
 ps_a11y_node nodes[PS_A11Y_MAX_NODES],draft[PS_A11Y_MAX_NODES];
 size_t count,draft_count;uint64_t next_id,pending_press;unsigned dropped;
 bool building;
} ps_a11y_model;
static inline bool ps_a11y_actionable(ps_a11y_role role) {
 return role==PS_A11Y_BUTTON || role==PS_A11Y_CHECKBOX;
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
bool ps_a11y_publish(ps_a11y_model *model);
const ps_a11y_node *ps_a11y_find(const ps_a11y_model *model,uint64_t id);
/* Queue at most one press, delivered only on a live, enabled matching control
 * next frame. Stale, disabled and text nodes cannot activate anything. */
bool ps_a11y_press(ps_a11y_model *model,uint64_t id);
#endif
