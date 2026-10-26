#ifndef PHYSIM_PROTOCOL_UI_H
#define PHYSIM_PROTOCOL_UI_H
#include "ui.h"
#include "protocol_store.h"
#include "graphic_output.h"
typedef struct {
    uint64_t selected,original_id,last_seen;
    size_t original_size,original_length,page;
    char query[256],feedback[256],return_window[64],return_action[128],export_test_path[4096];
    char *original;
    ps_graphic_save *save;
    uint64_t source_request,navigation_request;
    struct nk_context *navigation_ui;
    unsigned return_frames;
    char original_page[12289];
    int origin,level;bool initialized,transcript,follow,resize,resize_cancelled,focus_return,details_more,original_closed,cursor_owned;
    float resize_original,resize_effective,resize_y;
    nk_uint scroll_x,scroll_y,rendered_scroll_y;
    float list_width;
    struct nk_rect bounds,origins[5],levels[5],rows[PS_PROTOCOL_CAPACITY],search,source,copy,store,close,separator;
    struct nk_rect panes[3];
    struct nk_rect transcript_toggle,original_toggle;
    uint64_t row_ids[PS_PROTOCOL_CAPACITY];size_t row_count;
    struct nk_user_font title_font;
    struct nk_user_font fonts[10];
} ps_protocol_ui;
#endif
