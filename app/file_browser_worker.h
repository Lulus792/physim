#ifndef PHYSIM_FILE_BROWSER_WORKER_H
#define PHYSIM_FILE_BROWSER_WORKER_H
#include "file_browser.h"
#include "completion.h"
#include "documentation.h"

enum { PS_BROWSER_PREVIEW_BYTES = 65536 };
typedef enum {
    PS_BROWSER_READ_DIRECTORY, PS_BROWSER_READ_TEXT,
    PS_BROWSER_CONFIRM, PS_BROWSER_MAKE_DIRECTORY
} ps_browser_operation;
typedef struct {
    ps_browser_operation operation;
    ps_browser_mode mode;
    char path[PS_BROWSER_PATH];
    char name[PS_BROWSER_PATH]; /* Only used for MAKE_DIRECTORY. */
    char reference_root[PS_BROWSER_PATH]; /* Optional bundled C-reference root for READ_TEXT. */
    size_t limit; /* Directory entries; zero selects the model default. */
} ps_browser_request;
typedef struct {
    uint64_t version;
    ps_browser_operation operation;
    ps_result status;
    char path[PS_BROWSER_PATH]; /* Requested path; created child on success. */
    ps_browser_listing listing;
    char *text; /* Owned valid UTF-8 prefix; NULL on error. */
    size_t length;
    uint64_t size; /* Size observed through the opened handle, not a promise. */
    bool truncated, selectable;
    ps_browser_create_result created;
    struct {
        ps_result status; /* Independent of source status; PS_EOF means no context. */
        const ps_completion_symbol *symbol; /* Static catalog; never owned. */
        size_t offset, first, end;
        ps_document *document; /* Owned parsed module, only on success. */
        ps_doc_api_preview preview;
    } api;
} ps_browser_result;
typedef struct ps_browser_worker ps_browser_worker;
typedef struct {
    uint64_t requested, discarded;
    bool loading;
} ps_browser_worker_state;

/* All functions except the private worker run on one owner thread. Exactly
 * one active job and the latest pending request are retained. Requests own
 * their path/name/reference root; neither an app pointer nor an SDL/GL/UI object crosses in.
 * poll never waits for unfinished I/O; it transfers an owned result only if
 * its version is still current. Initialize result to zero before first poll.
 * request/cancel retire pending results immediately, without joining a live
 * thread. Failed request validation preserves the prior request/state.
 * destroy cancels and joins; an active OS call can delay final shutdown.
 * MAKE_DIRECTORY is exclusive but cancellation cannot undo an OS mkdir that
 * already ran. Do not issue it speculatively while painting. */
ps_browser_worker *ps_browser_worker_create(void);
void ps_browser_worker_destroy(ps_browser_worker *worker);
ps_result ps_browser_worker_request(ps_browser_worker *worker,const ps_browser_request *request);
void ps_browser_worker_cancel(ps_browser_worker *worker);
bool ps_browser_worker_poll(ps_browser_worker *worker,ps_browser_result *result);
ps_browser_worker_state ps_browser_worker_read(const ps_browser_worker *worker);
void ps_browser_result_destroy(ps_browser_result *result);
#endif
