#ifndef PHYSIM_DIALOG_REQUEST_H
#define PHYSIM_DIALOG_REQUEST_H
#include "physim/core.h"
enum { PS_DIALOG_REQUEST_ROUTES=16,PS_DIALOG_REQUEST_PATH=4096 };
typedef struct {unsigned route;uint64_t version;} ps_dialog_ticket;
typedef struct ps_dialog_request ps_dialog_request;
typedef struct {
    ps_dialog_request *pending[PS_DIALOG_REQUEST_ROUTES];
    uint64_t versions[PS_DIALOG_REQUEST_ROUTES];
} ps_dialog_requests;
typedef struct {ps_dialog_ticket ticket;ps_result status;char *path;} ps_dialog_result;
/* Registry and transferred results have one UI-thread owner. Initialize to
 * zero; never copy a live registry. Each route has its independent generation.
 * A new begin retires only its preceding request. Failure preserves state/out.
 * The returned request is borrowed until cancel, begin or successful poll. */
ps_result ps_dialog_request_begin(ps_dialog_requests *requests,unsigned route,ps_dialog_request **out);
ps_dialog_ticket ps_dialog_request_ticket(const ps_dialog_request *request);
ps_dialog_request *ps_dialog_request_find(const ps_dialog_requests *requests,ps_dialog_ticket ticket);
bool ps_dialog_request_cancel(ps_dialog_requests *requests,ps_dialog_ticket ticket);
void ps_dialog_request_cancel_route(ps_dialog_requests *requests,unsigned route);
void ps_dialog_requests_cancel_all(ps_dialog_requests *requests);
bool ps_dialog_requests_pending(const ps_dialog_requests *requests);
/* A callback retains its own CPU owner before dispatch and releases it after
 * completion. No app, UI, registry, event queue or window pointer crosses in.
 * Exactly one completion claims the immutable path/status. Cancel/shutdown
 * retires the registry reference; an arbitrarily late callback stays safe. */
ps_dialog_request *ps_dialog_request_retain(ps_dialog_request *request);
void ps_dialog_request_release(ps_dialog_request *request);
bool ps_dialog_request_complete(ps_dialog_request *request,const char *path,bool error);
/* Nonblocking transfer only from the currently owned request on this route.
 * Result must initially be zero. Destroy/replace releases its transferred path. */
bool ps_dialog_request_poll(ps_dialog_requests *requests,unsigned route,ps_dialog_result *out);
void ps_dialog_result_destroy(ps_dialog_result *result);
#endif
