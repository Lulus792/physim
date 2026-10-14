#ifndef PS_APP_ACCESSIBILITY_H
#define PS_APP_ACCESSIBILITY_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#define PS_A11Y_MAX_NODES 256u
#define PS_A11Y_LABEL_BYTES 1024u
#define PS_A11Y_WINDOW_BYTES 96u
#define PS_A11Y_DOCUMENT_BYTES (512u*1024u+8192u)
#define PS_A11Y_TEXT_BOXES 8192u
#define PS_A11Y_EDIT_BOXES 2048u
typedef struct {size_t begin,end;float bounds[4];} ps_a11y_text_box;
typedef struct {
 uint64_t version;char name[256];char *text;size_t length;
 float viewport[4];ps_a11y_text_box boxes[PS_A11Y_TEXT_BOXES];size_t count;unsigned dropped;
 bool active;
} ps_a11y_document;
typedef enum {PS_A11Y_TEXT,PS_A11Y_BUTTON,PS_A11Y_CHECKBOX,PS_A11Y_RADIO,PS_A11Y_RADIO_GROUP,PS_A11Y_SLIDER,PS_A11Y_EDIT,PS_A11Y_LINK} ps_a11y_role;
typedef struct {
 uint64_t id,key,parent;unsigned occurrence;ps_a11y_role role;
 char window[PS_A11Y_WINDOW_BYTES],label[PS_A11Y_LABEL_BYTES];
 float bounds[4];bool enabled,checked,focusable,focused;
 double value,minimum,maximum,increment;
 char text_value[PS_A11Y_LABEL_BYTES];size_t text_capacity;
 uint64_t text_version;size_t text_start,text_end,text_caret,box_offset,box_count;
 float caret_bounds[4];bool text_active;
 float text_scroll;
 uint64_t document_version;size_t document_begin,document_end;bool offscreen;
 char document_target[PS_A11Y_LABEL_BYTES];
} ps_a11y_node;
/* UI thread owns the draft; callers serialize all operations. Published nodes
 * are immutable until publish. IDs persist across geometry changes, expire when
 * controls disappear, and never get reused during this model's lifetime. */
typedef struct {
 ps_a11y_node nodes[PS_A11Y_MAX_NODES],draft[PS_A11Y_MAX_NODES];
 size_t count,draft_count;uint64_t next_id,pending_press,selection_parent,selection_id,pending_focus,focused_id;unsigned dropped;
 bool building,keyboard_focus,suspended;
 uint64_t pending_value_id;double pending_value;
 uint64_t pending_text_id;char pending_text[PS_A11Y_LABEL_BYTES];
 size_t pending_text_caret;uint64_t pending_text_version;bool pending_text_has_caret;
 uint64_t next_text_version,pending_selection_id,pending_selection_version;
 size_t pending_selection_start,pending_selection_end;
 uint64_t pending_edit_scroll_id,pending_edit_scroll_version;
 size_t pending_edit_scroll_start,pending_edit_scroll_end;bool pending_edit_scroll_top;
 ps_a11y_text_box edit_boxes[PS_A11Y_EDIT_BOXES],edit_boxes_draft[PS_A11Y_EDIT_BOXES];
 size_t edit_box_count,edit_box_draft_count;
 ps_a11y_document *document,*document_draft;
 uint64_t document_scroll_version;size_t document_scroll_byte;
 bool document_scroll_top;
} ps_a11y_model;
static inline bool ps_a11y_actionable(ps_a11y_role role) {
 return role==PS_A11Y_BUTTON || role==PS_A11Y_CHECKBOX || role==PS_A11Y_RADIO || role==PS_A11Y_LINK;
}
void ps_a11y_init(ps_a11y_model *model);
void ps_a11y_destroy(ps_a11y_model *model);
/* Full UTF-8 reading snapshot, separate from the bounded control labels. The
 * caller uses a new nonzero version whenever the document content changes.
 * Only the UI draft is writable; publication swaps immutable snapshots. */
bool ps_a11y_document_begin(ps_a11y_model *model,uint64_t version,const char *name,
                           const char *text,const float viewport[4]);
bool ps_a11y_document_box(ps_a11y_model *model,size_t begin,size_t end,const float bounds[4]);
bool ps_a11y_document_scroll(ps_a11y_model *model,uint64_t version,size_t byte);
bool ps_a11y_document_scroll_align(ps_a11y_model *model,uint64_t version,size_t byte,bool top);
/* Queue the earliest visible text byte before layout reflow. Existing requests win. */
bool ps_a11y_document_preserve_position(ps_a11y_model *model,uint64_t version);
bool ps_a11y_document_take_scroll(ps_a11y_model *model,uint64_t version,size_t *byte);
/* All document links, including offscreen ones, bind to immutable UTF-8 ranges.
 * Zero width and height mean offscreen. Returns one queued native activation. */
bool ps_a11y_document_link(ps_a11y_model *model,const char *window,const char *name,
                          uint64_t version,size_t begin,size_t end,const float bounds[4],bool enabled);
/* Bind the URI of the most recently recorded link without truncation. */
bool ps_a11y_document_link_target(ps_a11y_model *model,size_t index,const char *target);
void ps_a11y_begin(ps_a11y_model *model);
/* Preserve published content/IDs while unavailable. Cancel queued input;
 * only a completed new publication resumes native actions. */
void ps_a11y_suspend(ps_a11y_model *model);
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
/* Finite bounded scalar controls; names stay stable while values change.
 * Queued changes are applied only when the same visible enabled control is
 * recorded again. Invalid declarations preserve model and caller value. */
bool ps_a11y_record_range(ps_a11y_model *model,const char *window,const char *label,
                         const float bounds[4],bool enabled,double minimum,double maximum,
                         double increment,double *value);
bool ps_a11y_set_value(ps_a11y_model *model,uint64_t id,double value);
bool ps_a11y_adjust(ps_a11y_model *model,uint64_t id,int direction);
/* Single-line editable UTF-8 values; no truncation. Names and IDs remain stable
 * while values change. Accepted writes apply only on the next matching UI visit. */
bool ps_a11y_record_edit(ps_a11y_model *model,const char *window,const char *name,
                       const float bounds[4],bool enabled,char *text,size_t capacity);
bool ps_a11y_set_text(ps_a11y_model *model,uint64_t id,const char *text);
/* Replace the current version's selection and deliver its collapsed caret
 * atomically with the new value, including an identical-text replacement. */
bool ps_a11y_edit_replace(ps_a11y_model *model,uint64_t id,uint64_t version,const char *text);
/* Selection offsets are UTF-8 scalar boundaries. Requests expire on content
 * changes and never run against a replacement control. */
bool ps_a11y_edit_update(ps_a11y_model *model,size_t index,const char *text,size_t start,size_t end);
bool ps_a11y_edit_box(ps_a11y_model *model,size_t index,size_t begin,size_t end,const float bounds[4]);
bool ps_a11y_edit_select(ps_a11y_model *model,uint64_t id,uint64_t version,size_t start,size_t end);
bool ps_a11y_edit_take_selection(ps_a11y_model *model,size_t index,size_t *start,size_t *end);
/* Read-only reveal requests retain value, selection and focus. Byte ranges
 * expire if their control or content changes before the next UI visit. */
bool ps_a11y_edit_scroll(ps_a11y_model *model,uint64_t id,uint64_t version,size_t start,size_t end,bool top);
bool ps_a11y_edit_take_scroll(ps_a11y_model *model,size_t index,size_t *start,size_t *end,bool *top);
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
