#ifndef PHYSIM_PROTOCOL_UI_H
#define PHYSIM_PROTOCOL_UI_H
#include "ui.h"
#include "protocol_store.h"
#include "graphic_output.h"
#include "text_wrap.h"
enum {
    PS_PROTOCOL_FONT_BODY,PS_PROTOCOL_FONT_SMALL,PS_PROTOCOL_FONT_ROW_TITLE,
    PS_PROTOCOL_FONT_DETAIL_TITLE,PS_PROTOCOL_FONT_HEADING,PS_PROTOCOL_FONT_CAPTION,
    PS_PROTOCOL_FONT_ORIGINAL,PS_PROTOCOL_FONT_NAVIGATION,PS_PROTOCOL_FONT_FILTER,
    PS_PROTOCOL_FONT_COUNT_LABEL,PS_PROTOCOL_FONT_ACTION,PS_PROTOCOL_FONT_COUNT
};
typedef struct {
    uint64_t selected,original_id,last_seen;
    size_t original_size,original_length;
    char query[256],feedback[256],return_window[64],return_action[128],export_test_path[4096];
    char *original;
    ps_graphic_save *save;
    uint64_t source_request,navigation_request;
    struct nk_context *navigation_ui;
    unsigned return_frames;
    ps_text_wrap *original_layout;
    struct nk_text_edit original_edit;
    struct nk_user_font original_layout_font;
    struct nk_rect original_bounds,original_viewport;
    nk_uint detail_scroll_x,detail_scroll_y,detail_scroll_target;
    uint64_t detail_record;
    bool detail_reveal_pending,detail_transcript;
    int origin,level;bool initialized,transcript,follow,resize,resize_cancelled,focus_return,details_more,original_closed,cursor_owned;
    float resize_original,resize_effective,resize_y;
    nk_uint scroll_x,scroll_y,rendered_scroll_y;
    float list_width;
    struct nk_rect bounds,origins[5],levels[5],rows[PS_PROTOCOL_CAPACITY],search,source,copy,store,close,separator;
    struct nk_rect panes[3];
    struct nk_rect detail_header,detail_badge,detail_meta,detail_title,detail_impact;
    struct nk_rect detail_properties[3],detail_labels[3],detail_values[3];
    unsigned detail_property_count;
    float detail_meta_font,detail_title_line,detail_impact_line,detail_property_line;
    char detail_meta_text[2300];
    struct nk_rect transcript_toggle,original_toggle;
    uint64_t row_ids[PS_PROTOCOL_CAPACITY];size_t row_count;
    struct nk_user_font title_font;
    struct nk_user_font fonts[PS_PROTOCOL_FONT_COUNT];
} ps_protocol_ui;
#endif
