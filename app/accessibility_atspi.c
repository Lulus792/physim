#include "accessibility_native.h"
#ifdef __linux__
#include "text_validation.h"
#include <dbus/dbus.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define AX_ROOT "/org/a11y/atspi/accessible/root"
#define AX_CACHE "/org/a11y/atspi/cache"
#define AX_NULL "/org/a11y/atspi/null"
#define AX_PREFIX "org.a11y.atspi."
#define AX_WINDOWS 8u
typedef struct ax_server ax_server;
struct ps_a11y_native {
    ax_server *server;
    SDL_Window *window;
    ps_a11y_model *model;
    SDL_Mutex *mutex;
    unsigned window_id;
    int x, y, width, height;
    bool screen_position, visible;
    char title[256];
    ps_a11y_node previous[PS_A11Y_MAX_NODES];
    size_t previous_count;
};
struct ax_server {
    DBusConnection *connection;
    SDL_Thread *thread;
    SDL_Mutex *mutex;
    SDL_AtomicInt stopping;
    ps_a11y_native *windows[AX_WINDOWS];
    size_t count;
    int application_id;
    char address[2048];
};
static ax_server *server;
/* The UI thread creates/destroys windows; the D-Bus thread serializes queries
 * with server->mutex then the corresponding model mutex, always in that order. */
typedef struct {
    DBusMessage *message;
    DBusMessageIter root;
    bool okay;
} ax_reply;
static void basic(ax_reply *r, DBusMessageIter *iter, int type, const void *value) {
    if (r->okay && !dbus_message_iter_append_basic(iter, type, value))
        r->okay = false;
}
static void text(ax_reply *r, DBusMessageIter *iter, const char *value) {
    basic(r, iter, DBUS_TYPE_STRING, &value);
}
static void number(ax_reply *r, DBusMessageIter *iter, int value) {
    dbus_int32_t v = value;
    basic(r, iter, DBUS_TYPE_INT32, &v);
}
static void unsigned_number(ax_reply *r, DBusMessageIter *iter, unsigned value) {
    dbus_uint32_t v = value;
    basic(r, iter, DBUS_TYPE_UINT32, &v);
}
static void boolean(ax_reply *r, DBusMessageIter *iter, bool value) {
    dbus_bool_t v = value;
    basic(r, iter, DBUS_TYPE_BOOLEAN, &v);
}
static void open_container(ax_reply *r, DBusMessageIter *parent, int type, const char *signature,
                           DBusMessageIter *child) {
    if (r->okay && !dbus_message_iter_open_container(parent, type, signature, child))
        r->okay = false;
}
static void close_container(ax_reply *r, DBusMessageIter *parent, DBusMessageIter *child) {
    if (r->okay && !dbus_message_iter_close_container(parent, child))
        r->okay = false;
}
static void reference(ax_reply *r, DBusMessageIter *iter, const char *bus, const char *path) {
    DBusMessageIter child;
    open_container(r, iter, DBUS_TYPE_STRUCT, NULL, &child);
    text(r, &child, bus);
    basic(r, &child, DBUS_TYPE_OBJECT_PATH, &path);
    close_container(r, iter, &child);
}
static const char *bus_name(ax_server *s) {
    return dbus_bus_get_unique_name(s->connection);
}
static void path_for(char path[128], unsigned window, uint64_t node) {
    if (node)
        snprintf(path, 128, "/org/a11y/atspi/accessible/w%u/n%llu", window,
                 (unsigned long long)node);
    else
        snprintf(path, 128, "/org/a11y/atspi/accessible/w%u", window);
}
static void local_reference(ax_reply *r, DBusMessageIter *iter, ax_server *s, unsigned window,
                            uint64_t node) {
    char path[128];
    if (window)
        path_for(path, window, node);
    else
        strcpy(path, AX_ROOT);
    reference(r, iter, bus_name(s), path);
}
static bool find_object(ax_server *s, const char *path, ps_a11y_native **window, ps_a11y_node *node,
                        size_t *index) {
    *window = NULL;
    memset(node, 0, sizeof *node);
    *index = 0;
    if (!strcmp(path, AX_ROOT))
        return true;
    static const char prefix[] = "/org/a11y/atspi/accessible/w";
    if (strncmp(path, prefix, sizeof prefix - 1))
        return false;
    const char *cursor = path + sizeof prefix - 1;
    uint64_t values[2] = {0, 0};
    for (size_t part = 0; part < 2; part++) {
        if (*cursor < '1' || *cursor > '9')
            return false;
        uint64_t maximum = part ? UINT64_MAX : UINT32_MAX;
        do {
            unsigned digit = (unsigned)(*cursor - '0');
            if (values[part] > (maximum - digit) / 10)
                return false;
            values[part] = values[part] * 10 + digit;
            cursor++;
        } while (*cursor >= '0' && *cursor <= '9');
        if (!*cursor)
            break;
        if (part || strncmp(cursor, "/n", 2))
            return false;
        cursor += 2;
    }
    if (*cursor)
        return false;
    unsigned id = (unsigned)values[0];
    uint64_t key = values[1];
    for (size_t i = 0; i < s->count; i++)
        if (s->windows[i]->window_id == id) {
            *window = s->windows[i];
            *index = i;
            if (!key)
                return true;
            SDL_LockMutex((*window)->mutex);
            const ps_a11y_node *live = ps_a11y_find((*window)->model, key);
            if (live) {
                *node = *live;
                *index = (size_t)(live - (*window)->model->nodes);
            }
            SDL_UnlockMutex((*window)->mutex);
            return live != NULL;
        }
    return false;
}
static unsigned role(ps_a11y_native *w, const ps_a11y_node *node) {
    return !w ? 75 : !node->id ? 23 : node->role == PS_A11Y_BUTTON ? 43 : node->role == PS_A11Y_CHECKBOX ? 7 : 29;
}
static const char *role_name(unsigned r, bool localized) {
    switch (r) {
    case 75:
        return localized ? "Anwendung" : "application";
    case 23:
        return localized ? "Fenster" : "frame";
    case 7:
        return localized ? "Kontrollkästchen" : "check box";
    case 43:
        return localized ? "Schaltfläche" : "push button";
    default:
        return localized ? "Text" : "label";
    }
}
static size_t child_count(ax_server *s, ps_a11y_native *w, const ps_a11y_node *node) {
    if (!w)
        return s->count;
    if (node->id)
        return 0;
    SDL_LockMutex(w->mutex);
    size_t count = w->model->count;
    SDL_UnlockMutex(w->mutex);
    return count;
}
static void states(ax_reply *r, DBusMessageIter *iter, ps_a11y_native *w,
                   const ps_a11y_node *node) {
    /* AtspiStateType: checked=4, enabled=8, sensitive=24, showing=25,
     * visible=30, checkable=41.
     * Focusable is deliberately absent until programmatic focus is implemented. */
    unsigned first = 0;
    if (!node->id || node->enabled)
        first |= (1u << 8) | (1u << 24);
    if (!w || w->visible)
        first |= (1u << 25) | (1u << 30);
    if(node->role==PS_A11Y_CHECKBOX && node->checked)first |= 1u << 4;
    DBusMessageIter a;
    open_container(r, iter, DBUS_TYPE_ARRAY, "u", &a);
    unsigned_number(r, &a, first);
    unsigned_number(r, &a, node->role==PS_A11Y_CHECKBOX ? 1u << (41-32) : 0);
    close_container(r, iter, &a);
}
static void interfaces(ax_reply *r, DBusMessageIter *iter, ps_a11y_native *w,
                       const ps_a11y_node *node) {
    DBusMessageIter a;
    open_container(r, iter, DBUS_TYPE_ARRAY, "s", &a);
    text(r, &a, AX_PREFIX "Accessible");
    if (!w)
        text(r, &a, AX_PREFIX "Application");
    else
        text(r, &a, AX_PREFIX "Component");
    if (node->id && ps_a11y_actionable(node->role))
        text(r, &a, AX_PREFIX "Action");
    close_container(r, iter, &a);
}
static void parent_reference(ax_reply *r, DBusMessageIter *iter, ax_server *s, ps_a11y_native *w,
                             const ps_a11y_node *node) {
    if (!w)
        reference(r, iter, "org.a11y.atspi.Registry", AX_ROOT);
    else
        local_reference(r, iter, s, node->id ? w->window_id : 0, 0);
}
static void cache_item(ax_reply *r, DBusMessageIter *iter, ax_server *s, ps_a11y_native *w,
                       const ps_a11y_node *node, int index) {
    DBusMessageIter c;
    open_container(r, iter, DBUS_TYPE_STRUCT, NULL, &c);
    local_reference(r, &c, s, w ? w->window_id : 0, node->id);
    local_reference(r, &c, s, 0, 0);
    parent_reference(r, &c, s, w, node);
    number(r, &c, index);
    number(r, &c, (int)child_count(s, w, node));
    interfaces(r, &c, w, node);
    text(r, &c, !w ? "Physim" : !node->id ? w->title : node->label);
    unsigned_number(r, &c, role(w, node));
    text(r, &c, "");
    states(r, &c, w, node);
    close_container(r, iter, &c);
}
static void empty_properties(ax_reply *r, DBusMessageIter *iter) {
    DBusMessageIter a;
    open_container(r, iter, DBUS_TYPE_ARRAY, "{sv}", &a);
    close_container(r, iter, &a);
}
static void event_reference(ax_server *s, const char *path, const char *event, const char *detail,
                            int index, unsigned window, uint64_t node) {
    ax_reply r = {.message = dbus_message_new_signal(path, AX_PREFIX "Event.Object", event),
                  .okay = true};
    if (!r.message)
        return;
    dbus_message_iter_init_append(r.message, &r.root);
    text(&r, &r.root, detail);
    number(&r, &r.root, index);
    number(&r, &r.root, 0);
    DBusMessageIter v;
    open_container(&r, &r.root, DBUS_TYPE_VARIANT, "(so)", &v);
    local_reference(&r, &v, s, window, node);
    close_container(&r, &r.root, &v);
    empty_properties(&r, &r.root);
    if (r.okay)
        dbus_connection_send(s->connection, r.message, NULL);
    dbus_message_unref(r.message);
}
static void cache_signal(ax_server *s, ps_a11y_native *w, const ps_a11y_node *node, int index,
                         bool added) {
    ax_reply r = {.message = dbus_message_new_signal(AX_CACHE, AX_PREFIX "Cache",
                                                     added ? "AddAccessible" : "RemoveAccessible"),
                  .okay = true};
    if (!r.message)
        return;
    dbus_message_iter_init_append(r.message, &r.root);
    if (added)
        cache_item(&r, &r.root, s, w, node, index);
    else
        local_reference(&r, &r.root, s, w->window_id, node->id);
    if (r.okay)
        dbus_connection_send(s->connection, r.message, NULL);
    dbus_message_unref(r.message);
}
static DBusHandlerResult error_reply(DBusConnection *connection, DBusMessage *message,
                                     const char *name, const char *detail) {
    DBusMessage *reply = dbus_message_new_error(message, name, detail);
    if (!reply)
        return DBUS_HANDLER_RESULT_NEED_MEMORY;
    dbus_connection_send(connection, reply, NULL);
    dbus_message_unref(reply);
    return DBUS_HANDLER_RESULT_HANDLED;
}
static const char *property_signature(const char *iface, const char *name) {
    if (!strcmp(name, "version"))
        return "u";
    if (!strcmp(iface, AX_PREFIX "Accessible")) {
        if (!strcmp(name, "Parent"))
            return "(so)";
        if (!strcmp(name, "ChildCount"))
            return "i";
        if (!strcmp(name, "Name") || !strcmp(name, "Description") || !strcmp(name, "Locale") ||
            !strcmp(name, "AccessibleId") || !strcmp(name, "HelpText"))
            return "s";
    } else if (!strcmp(iface, AX_PREFIX "Application")) {
        if (!strcmp(name, "Id"))
            return "i";
        if (!strcmp(name, "InterfaceVersion"))
            return "u";
        if (!strcmp(name, "ToolkitName") || !strcmp(name, "ToolkitVersion") ||
            !strcmp(name, "Version") || !strcmp(name, "AtspiVersion"))
            return "s";
    } else if (!strcmp(iface, AX_PREFIX "Action") && !strcmp(name, "NActions"))
        return "i";
    return NULL;
}
static void property_value(ax_reply *r, DBusMessageIter *iter, ax_server *s, ps_a11y_native *w,
                           const ps_a11y_node *node, const char *iface, const char *name) {
    if (!strcmp(name, "version") || !strcmp(name, "InterfaceVersion")) {
        unsigned_number(r, iter, 1);
        return;
    }
    if (!strcmp(name, "Parent")) {
        parent_reference(r, iter, s, w, node);
        return;
    }
    if (!strcmp(name, "ChildCount")) {
        number(r, iter, (int)child_count(s, w, node));
        return;
    }
    if (!strcmp(name, "Id")) {
        number(r, iter, s->application_id);
        return;
    }
    if (!strcmp(name, "NActions")) {
        number(r, iter, node->id && ps_a11y_actionable(node->role) ? 1 : 0);
        return;
    }
    char id[128];
    if (w)
        path_for(id, w->window_id, node->id);
    else
        strcpy(id, AX_ROOT);
    const char *value = "";
    if (!strcmp(name, "Name"))
        value = !w ? "Physim" : !node->id ? w->title : node->label;
    else if (!strcmp(name, "AccessibleId"))
        value = id;
    else if (!strcmp(name, "Locale"))
        value = "de_DE.UTF-8";
    else if (!strcmp(iface, AX_PREFIX "Application")) {
        if (!strcmp(name, "ToolkitName"))
            value = "Physim";
        else if (!strcmp(name, "AtspiVersion"))
            value = "2.1";
        else
            value = "0.1";
    }
    text(r, iter, value);
}
static bool interface_supported(ps_a11y_native *w, const ps_a11y_node *node, const char *iface) {
    if (!strcmp(iface, AX_PREFIX "Accessible"))
        return true;
    if (!strcmp(iface, AX_PREFIX "Application"))
        return w == NULL;
    if (!strcmp(iface, AX_PREFIX "Component"))
        return w != NULL;
    if (!strcmp(iface, AX_PREFIX "Action"))
        return node->id && ps_a11y_actionable(node->role);
    return false;
}
static bool bounds(ps_a11y_native *w, const ps_a11y_node *node, unsigned coordinate, int out[4]) {
    bool screen = coordinate == 0 || (coordinate == 2 && !node->id);
    if (!w || coordinate > 2 || (screen && !w->screen_position))
        return false;
    double x = node->id ? floor(node->bounds[0]) : 0, y = node->id ? floor(node->bounds[1]) : 0;
    double width = node->id ? ceil(node->bounds[2]) : w->width,
           height = node->id ? ceil(node->bounds[3]) : w->height;
    if (screen) {
        x += w->x;
        y += w->y;
    }
    double values[] = {x, y, width, height};
    for (size_t i = 0; i < 4; i++) {
        if (!isfinite(values[i]) || values[i] < INT_MIN || values[i] > INT_MAX)
            return false;
        out[i] = (int)values[i];
    }
    return true;
}
static void event_state(ax_server *s, const char *path, const char *state, bool enabled) {
    ax_reply r = {.message =
                      dbus_message_new_signal(path, AX_PREFIX "Event.Object", "StateChanged"),
                  .okay = true};
    if (!r.message)
        return;
    dbus_message_iter_init_append(r.message, &r.root);
    text(&r, &r.root, state);
    number(&r, &r.root, enabled ? 1 : 0);
    number(&r, &r.root, 0);
    DBusMessageIter v;
    open_container(&r, &r.root, DBUS_TYPE_VARIANT, "i", &v);
    number(&r, &v, 0);
    close_container(&r, &r.root, &v);
    empty_properties(&r, &r.root);
    if (r.okay)
        dbus_connection_send(s->connection, r.message, NULL);
    dbus_message_unref(r.message);
}
static void event_bounds(ax_server *s, const char *path, ps_a11y_native *w,
                         const ps_a11y_node *node) {
    int rectangle[4];
    if (!bounds(w, node, 0, rectangle))
        return;
    ax_reply r = {.message =
                      dbus_message_new_signal(path, AX_PREFIX "Event.Object", "BoundsChanged"),
                  .okay = true};
    if (!r.message)
        return;
    dbus_message_iter_init_append(r.message, &r.root);
    text(&r, &r.root, "");
    number(&r, &r.root, 0);
    number(&r, &r.root, 0);
    DBusMessageIter v, c;
    open_container(&r, &r.root, DBUS_TYPE_VARIANT, "(iiii)", &v);
    open_container(&r, &v, DBUS_TYPE_STRUCT, NULL, &c);
    for (size_t i = 0; i < 4; i++)
        number(&r, &c, rectangle[i]);
    close_container(&r, &v, &c);
    close_container(&r, &r.root, &v);
    empty_properties(&r, &r.root);
    if (r.okay)
        dbus_connection_send(s->connection, r.message, NULL);
    dbus_message_unref(r.message);
}

static const char *accessible_xml =
    "<interface name='org.a11y.atspi.Accessible'><property name='Name' type='s' "
    "access='read'/><property name='Description' type='s' access='read'/><property name='Parent' "
    "type='(so)' access='read'/><property name='ChildCount' type='i' access='read'/><property "
    "name='Locale' type='s' access='read'/><property name='AccessibleId' type='s' "
    "access='read'/><property name='HelpText' type='s' access='read'/><property name='version' "
    "type='u' access='read'/>"
    "<method name='GetChildren'><arg type='a(so)' direction='out'/></method><method "
    "name='GetChildAtIndex'><arg type='i' direction='in'/><arg type='(so)' "
    "direction='out'/></method><method name='GetIndexInParent'><arg type='i' "
    "direction='out'/></method><method name='GetRole'><arg type='u' "
    "direction='out'/></method><method name='GetRoleName'><arg type='s' "
    "direction='out'/></method><method name='GetLocalizedRoleName'><arg type='s' "
    "direction='out'/></method><method name='GetState'><arg type='au' "
    "direction='out'/></method><method name='GetInterfaces'><arg type='as' "
    "direction='out'/></method><method name='GetApplication'><arg type='(so)' "
    "direction='out'/></method><method name='GetRelationSet'><arg type='a(ua(so))' "
    "direction='out'/></method><method name='GetAttributes'><arg type='a{ss}' "
    "direction='out'/></method></interface>";
static const char *application_xml =
    "<interface name='org.a11y.atspi.Application'><property name='ToolkitName' type='s' "
    "access='read'/><property name='ToolkitVersion' type='s' access='read'/><property "
    "name='Version' type='s' access='read'/><property name='AtspiVersion' type='s' "
    "access='read'/><property name='InterfaceVersion' type='u' access='read'/><property name='Id' "
    "type='i' access='readwrite'/><method name='GetLocale'><arg type='u' direction='in'/><arg "
    "type='s' direction='out'/></method><method name='GetApplicationBusAddress'><arg type='s' "
    "direction='out'/></method></interface>";
static const char *component_xml =
    "<interface name='org.a11y.atspi.Component'><method name='GetAccessibleAtPoint'><arg type='i' "
    "direction='in'/><arg type='i' direction='in'/><arg type='u' direction='in'/><arg type='(so)' "
    "direction='out'/></method><method name='GetExtents'><arg type='u' direction='in'/><arg "
    "type='(iiii)' direction='out'/></method><method name='GetPosition'><arg type='u' "
    "direction='in'/><arg type='i' direction='out'/><arg type='i' "
    "direction='out'/></method><method name='GetSize'><arg type='i' direction='out'/><arg type='i' "
    "direction='out'/></method><method name='Contains'><arg type='i' direction='in'/><arg type='i' "
    "direction='in'/><arg type='u' direction='in'/><arg type='b' direction='out'/></method><method "
    "name='GrabFocus'><arg type='b' direction='out'/></method><method name='GetLayer'><arg "
    "type='u' direction='out'/></method><method name='GetMDIZOrder'><arg type='n' "
    "direction='out'/></method><method name='GetAlpha'><arg type='d' "
    "direction='out'/></method></interface>";
static const char *action_xml =
    "<interface name='org.a11y.atspi.Action'><property name='NActions' type='i' "
    "access='read'/><method name='GetName'><arg type='i' direction='in'/><arg type='s' "
    "direction='out'/></method><method name='GetLocalizedName'><arg type='i' direction='in'/><arg "
    "type='s' direction='out'/></method><method name='GetDescription'><arg type='i' "
    "direction='in'/><arg type='s' direction='out'/></method><method name='GetKeyBinding'><arg "
    "type='i' direction='in'/><arg type='s' direction='out'/></method><method "
    "name='GetActions'><arg type='a(sss)' direction='out'/></method><method name='DoAction'><arg "
    "type='i' direction='in'/><arg type='b' direction='out'/></method></interface>";
static const char *properties_xml =
    "<interface name='org.freedesktop.DBus.Properties'><method name='Get'><arg type='s' "
    "direction='in'/><arg type='s' direction='in'/><arg type='v' direction='out'/></method><method "
    "name='GetAll'><arg type='s' direction='in'/><arg type='a{sv}' "
    "direction='out'/></method><method name='Set'><arg type='s' direction='in'/><arg type='s' "
    "direction='in'/><arg type='v' direction='in'/></method></interface>";

static DBusHandlerResult message_handler(DBusConnection *connection, DBusMessage *message,
                                         void *data) {
    if (dbus_message_get_type(message) != DBUS_MESSAGE_TYPE_METHOD_CALL)
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    ax_server *s = data;
    const char *path = dbus_message_get_path(message), *iface = dbus_message_get_interface(message),
               *method = dbus_message_get_member(message);
    if (!path || !iface || !method)
        return error_reply(connection, message, DBUS_ERROR_INVALID_ARGS, "Missing method identity");
    SDL_LockMutex(s->mutex);
    ps_a11y_native *w = NULL;
    ps_a11y_node node = {0};
    size_t index = 0;
    bool cache = !strcmp(path, AX_CACHE);
    if (!cache && !find_object(s, path, &w, &node, &index)) {
        SDL_UnlockMutex(s->mutex);
        return error_reply(connection, message, "org.a11y.atspi.Error.Defunct",
                           "The control is no longer visible");
    }
    ax_reply r = {.message = dbus_message_new_method_return(message), .okay = true};
    if (!r.message) {
        SDL_UnlockMutex(s->mutex);
        return DBUS_HANDLER_RESULT_NEED_MEMORY;
    }
    dbus_message_iter_init_append(r.message, &r.root);
    const char *failure = NULL;
    const char *detail = "Unsupported method or invalid argument signature";
    if (!strcmp(iface, "org.freedesktop.DBus.Introspectable") && !strcmp(method, "Introspect") &&
        dbus_message_has_signature(message, "")) {
        char xml[8192];
        int length = snprintf(
            xml, sizeof xml, "<node>%s%s%s%s%s</node>",
            cache ? "<interface name='org.a11y.atspi.Cache'><property name='version' type='u' "
                    "access='read'/><method name='GetItems'><arg type='a((so)(so)(so)iiassusau)' "
                    "direction='out'/></method></interface>"
                  : accessible_xml,
            cache ? ""
            : !w  ? application_xml
                  : component_xml,
            cache || !node.id || !ps_a11y_actionable(node.role) ? "" : action_xml, properties_xml,
            "<interface name='org.freedesktop.DBus.Introspectable'><method name='Introspect'><arg "
            "type='s' direction='out'/></method></interface>");
        if (length < 0 || (size_t)length >= sizeof xml)
            r.okay = false;
        else
            text(&r, &r.root, xml);
    } else if (cache && !strcmp(iface, AX_PREFIX "Cache") && !strcmp(method, "GetItems") &&
               dbus_message_has_signature(message, "")) {
        DBusMessageIter array;
        open_container(&r, &r.root, DBUS_TYPE_ARRAY, "((so)(so)(so)iiassusau)", &array);
        cache_item(&r, &array, s, NULL, &node, -1);
        for (size_t i = 0; i < s->count; i++) {
            ps_a11y_native *window = s->windows[i];
            SDL_LockMutex(window->mutex);
            ps_a11y_node empty = {0};
            cache_item(&r, &array, s, window, &empty, (int)i);
            for (size_t j = 0; j < window->model->count; j++)
                cache_item(&r, &array, s, window, &window->model->nodes[j], (int)j);
            SDL_UnlockMutex(window->mutex);
        }
        close_container(&r, &r.root, &array);
    } else if (!strcmp(iface, "org.freedesktop.DBus.Properties")) {
        const char *property_iface = NULL, *name = NULL;
        if (!strcmp(method, "Get") && dbus_message_has_signature(message, "ss") &&
            dbus_message_get_args(message, NULL, DBUS_TYPE_STRING, &property_iface,
                                  DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID)) {
            const char *signature = property_signature(property_iface, name);
            if (cache) {
                if (!strcmp(property_iface, AX_PREFIX "Cache") && !strcmp(name, "version")) {
                    DBusMessageIter value;
                    open_container(&r, &r.root, DBUS_TYPE_VARIANT, "u", &value);
                    unsigned_number(&r, &value, 1);
                    close_container(&r, &r.root, &value);
                } else
                    failure = DBUS_ERROR_UNKNOWN_PROPERTY;
            } else if (!interface_supported(w, &node, property_iface) || !signature)
                failure = DBUS_ERROR_UNKNOWN_PROPERTY;
            else {
                DBusMessageIter value;
                open_container(&r, &r.root, DBUS_TYPE_VARIANT, signature, &value);
                property_value(&r, &value, s, w, &node, property_iface, name);
                close_container(&r, &r.root, &value);
            }
        } else if (!strcmp(method, "GetAll") && dbus_message_has_signature(message, "s") &&
                   dbus_message_get_args(message, NULL, DBUS_TYPE_STRING, &property_iface,
                                         DBUS_TYPE_INVALID)) {
            const char *accessible[] = {"version",      "Name",       "Description",
                                        "Parent",       "ChildCount", "Locale",
                                        "AccessibleId", "HelpText",   NULL};
            const char *application[] = {"ToolkitName",
                                         "ToolkitVersion",
                                         "Version",
                                         "AtspiVersion",
                                         "InterfaceVersion",
                                         "Id",
                                         NULL};
            const char *action[] = {"version", "NActions", NULL};
            const char *component[] = {"version", NULL};
            const char **names = !strcmp(property_iface, AX_PREFIX "Application") ? application
                                 : !strcmp(property_iface, AX_PREFIX "Action")    ? action
                                 : !strcmp(property_iface, AX_PREFIX "Component") ? component
                                                                                  : accessible;
            if (cache && !strcmp(property_iface, AX_PREFIX "Cache")) {
                DBusMessageIter array, entry, value;
                open_container(&r, &r.root, DBUS_TYPE_ARRAY, "{sv}", &array);
                open_container(&r, &array, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
                text(&r, &entry, "version");
                open_container(&r, &entry, DBUS_TYPE_VARIANT, "u", &value);
                unsigned_number(&r, &value, 1);
                close_container(&r, &entry, &value);
                close_container(&r, &array, &entry);
                close_container(&r, &r.root, &array);
            } else if (cache || !interface_supported(w, &node, property_iface))
                failure = DBUS_ERROR_UNKNOWN_INTERFACE;
            else {
                DBusMessageIter array;
                open_container(&r, &r.root, DBUS_TYPE_ARRAY, "{sv}", &array);
                for (size_t i = 0; names[i]; i++) {
                    DBusMessageIter entry, value;
                    open_container(&r, &array, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
                    text(&r, &entry, names[i]);
                    open_container(&r, &entry, DBUS_TYPE_VARIANT,
                                   property_signature(property_iface, names[i]), &value);
                    property_value(&r, &value, s, w, &node, property_iface, names[i]);
                    close_container(&r, &entry, &value);
                    close_container(&r, &array, &entry);
                }
                close_container(&r, &r.root, &array);
            }
        } else if (!strcmp(method, "Set") && dbus_message_has_signature(message, "ssv")) {
            DBusMessageIter arguments, value;
            dbus_message_iter_init(message, &arguments);
            dbus_message_iter_get_basic(&arguments, &property_iface);
            dbus_message_iter_next(&arguments);
            dbus_message_iter_get_basic(&arguments, &name);
            dbus_message_iter_next(&arguments);
            dbus_message_iter_recurse(&arguments, &value);
            if (!cache && !w && !strcmp(property_iface, AX_PREFIX "Application") &&
                !strcmp(name, "Id") && dbus_message_iter_get_arg_type(&value) == DBUS_TYPE_INT32)
                dbus_message_iter_get_basic(&value, &s->application_id);
            else
                failure = DBUS_ERROR_PROPERTY_READ_ONLY;
        } else
            failure = DBUS_ERROR_INVALID_ARGS;
    } else if (!cache && !strcmp(iface, AX_PREFIX "Accessible")) {
        if (!strcmp(method, "GetChildAtIndex") && dbus_message_has_signature(message, "i")) {
            dbus_int32_t child;
            dbus_message_get_args(message, NULL, DBUS_TYPE_INT32, &child, DBUS_TYPE_INVALID);
            if (child < 0 || (size_t)child >= child_count(s, w, &node))
                failure = DBUS_ERROR_INVALID_ARGS;
            else if (!w)
                local_reference(&r, &r.root, s, s->windows[child]->window_id, 0);
            else {
                SDL_LockMutex(w->mutex);
                if ((size_t)child < w->model->count)
                    local_reference(&r, &r.root, s, w->window_id, w->model->nodes[child].id);
                else
                    failure = DBUS_ERROR_INVALID_ARGS;
                SDL_UnlockMutex(w->mutex);
            }
        } else if (!dbus_message_has_signature(message, ""))
            failure = DBUS_ERROR_INVALID_ARGS;
        else if (!strcmp(method, "GetChildren")) {
            DBusMessageIter array;
            open_container(&r, &r.root, DBUS_TYPE_ARRAY, "(so)", &array);
            if (!w)
                for (size_t i = 0; i < s->count; i++)
                    local_reference(&r, &array, s, s->windows[i]->window_id, 0);
            else if (!node.id) {
                SDL_LockMutex(w->mutex);
                for (size_t i = 0; i < w->model->count; i++)
                    local_reference(&r, &array, s, w->window_id, w->model->nodes[i].id);
                SDL_UnlockMutex(w->mutex);
            }
            close_container(&r, &r.root, &array);
        } else if (!strcmp(method, "GetIndexInParent"))
            number(&r, &r.root, w ? (int)index : -1);
        else if (!strcmp(method, "GetRole"))
            unsigned_number(&r, &r.root, role(w, &node));
        else if (!strcmp(method, "GetRoleName") || !strcmp(method, "GetLocalizedRoleName"))
            text(&r, &r.root, role_name(role(w, &node), !strcmp(method, "GetLocalizedRoleName")));
        else if (!strcmp(method, "GetState"))
            states(&r, &r.root, w, &node);
        else if (!strcmp(method, "GetInterfaces"))
            interfaces(&r, &r.root, w, &node);
        else if (!strcmp(method, "GetApplication"))
            local_reference(&r, &r.root, s, 0, 0);
        else if (!strcmp(method, "GetRelationSet") || !strcmp(method, "GetAttributes")) {
            DBusMessageIter array;
            open_container(&r, &r.root, DBUS_TYPE_ARRAY,
                           !strcmp(method, "GetRelationSet") ? "(ua(so))" : "{ss}", &array);
            close_container(&r, &r.root, &array);
        } else
            failure = DBUS_ERROR_UNKNOWN_METHOD;
    } else if (!cache && !w && !strcmp(iface, AX_PREFIX "Application")) {
        if (!strcmp(method, "GetLocale") && dbus_message_has_signature(message, "u"))
            text(&r, &r.root, "de_DE.UTF-8");
        else if (!strcmp(method, "GetApplicationBusAddress") &&
                 dbus_message_has_signature(message, ""))
            /* This provider uses the shared accessibility bus, not a private
             * application peer bus. Clients must keep their existing bus. */
            text(&r, &r.root, "");
        else
            failure = DBUS_ERROR_INVALID_ARGS;
    } else if (!cache && w && !strcmp(iface, AX_PREFIX "Component")) {
        unsigned coordinate = 1;
        int rectangle[4];
        if (!strcmp(method, "GetExtents") || !strcmp(method, "GetPosition")) {
            if (!dbus_message_has_signature(message, "u"))
                failure = DBUS_ERROR_INVALID_ARGS;
            else {
                dbus_message_get_args(message, NULL, DBUS_TYPE_UINT32, &coordinate,
                                      DBUS_TYPE_INVALID);
                if (!bounds(w, &node, coordinate, rectangle))
                    failure = DBUS_ERROR_NOT_SUPPORTED;
                else if (!strcmp(method, "GetPosition")) {
                    number(&r, &r.root, rectangle[0]);
                    number(&r, &r.root, rectangle[1]);
                } else {
                    DBusMessageIter value;
                    open_container(&r, &r.root, DBUS_TYPE_STRUCT, NULL, &value);
                    for (size_t i = 0; i < 4; i++)
                        number(&r, &value, rectangle[i]);
                    close_container(&r, &r.root, &value);
                }
            }
        } else if (!strcmp(method, "Contains") && dbus_message_has_signature(message, "iiu")) {
            dbus_int32_t x, y;
            dbus_message_get_args(message, NULL, DBUS_TYPE_INT32, &x, DBUS_TYPE_INT32, &y,
                                  DBUS_TYPE_UINT32, &coordinate, DBUS_TYPE_INVALID);
            if (!bounds(w, &node, coordinate, rectangle))
                failure = DBUS_ERROR_NOT_SUPPORTED;
            else
                boolean(&r, &r.root,
                        (int64_t)x >= rectangle[0] && (int64_t)y >= rectangle[1] &&
                            (int64_t)x < (int64_t)rectangle[0] + rectangle[2] &&
                            (int64_t)y < (int64_t)rectangle[1] + rectangle[3]);
        } else if (!strcmp(method, "GetAccessibleAtPoint") &&
                   dbus_message_has_signature(message, "iiu")) {
            dbus_int32_t x, y;
            dbus_message_get_args(message, NULL, DBUS_TYPE_INT32, &x, DBUS_TYPE_INT32, &y,
                                  DBUS_TYPE_UINT32, &coordinate, DBUS_TYPE_INVALID);
            if (!bounds(w, &node, coordinate, rectangle))
                failure = DBUS_ERROR_NOT_SUPPORTED;
            else if (!w->visible || (int64_t)x < rectangle[0] || (int64_t)y < rectangle[1] ||
                     (int64_t)x >= (int64_t)rectangle[0] + rectangle[2] ||
                     (int64_t)y >= (int64_t)rectangle[1] + rectangle[3])
                reference(&r, &r.root, "", AX_NULL);
            else if (node.id)
                local_reference(&r, &r.root, s, w->window_id, node.id);
            else {
                uint64_t hit = 0;
                SDL_LockMutex(w->mutex);
                for (size_t i = w->model->count; i > 0; i--) {
                    int child_bounds[4];
                    const ps_a11y_node *child = &w->model->nodes[i - 1];
                    /* Window parent coordinates use the screen; child parent
                     * coordinates use the window. Compare in the caller's space. */
                    unsigned child_coordinate = coordinate == 2 ? 0 : coordinate;
                    if (bounds(w, child, child_coordinate, child_bounds) &&
                        (int64_t)x >= child_bounds[0] && (int64_t)y >= child_bounds[1] &&
                        (int64_t)x < (int64_t)child_bounds[0] + child_bounds[2] &&
                        (int64_t)y < (int64_t)child_bounds[1] + child_bounds[3]) {
                        hit = child->id;
                        break;
                    }
                }
                SDL_UnlockMutex(w->mutex);
                local_reference(&r, &r.root, s, w->window_id, hit);
            }
        } else if (!dbus_message_has_signature(message, ""))
            failure = DBUS_ERROR_INVALID_ARGS;
        else if (!strcmp(method, "GetSize")) {
            if (!bounds(w, &node, 1, rectangle))
                failure = DBUS_ERROR_NOT_SUPPORTED;
            else {
                number(&r, &r.root, rectangle[2]);
                number(&r, &r.root, rectangle[3]);
            }
        } else if (!strcmp(method, "GetLayer"))
            unsigned_number(&r, &r.root, node.id ? 3 : 7);
        else if (!strcmp(method, "GetMDIZOrder")) {
            dbus_int16_t z = -1;
            basic(&r, &r.root, DBUS_TYPE_INT16, &z);
        } else if (!strcmp(method, "GetAlpha")) {
            double alpha = w->visible ? 1 : 0;
            basic(&r, &r.root, DBUS_TYPE_DOUBLE, &alpha);
        } else if (!strcmp(method, "GrabFocus"))
            boolean(&r, &r.root, false);
        else
            failure = DBUS_ERROR_UNKNOWN_METHOD;
    } else if (!cache && node.id && ps_a11y_actionable(node.role) &&
               !strcmp(iface, AX_PREFIX "Action")) {
        if (!strcmp(method, "GetActions") && dbus_message_has_signature(message, "")) {
            DBusMessageIter a, c;
            open_container(&r, &r.root, DBUS_TYPE_ARRAY, "(sss)", &a);
            open_container(&r, &a, DBUS_TYPE_STRUCT, NULL, &c);
            text(&r, &c, node.role==PS_A11Y_CHECKBOX?"Umschalten":"Klicken");
            text(&r, &c, "");
            text(&r, &c, "");
            close_container(&r, &a, &c);
            close_container(&r, &r.root, &a);
        } else if (dbus_message_has_signature(message, "i")) {
            dbus_int32_t action;
            dbus_message_get_args(message, NULL, DBUS_TYPE_INT32, &action, DBUS_TYPE_INVALID);
            if (action != 0)
                failure = DBUS_ERROR_INVALID_ARGS;
            else if (!strcmp(method, "DoAction")) {
                SDL_LockMutex(w->mutex);
                bool accepted = w->visible && ps_a11y_press(w->model, node.id);
                SDL_UnlockMutex(w->mutex);
                boolean(&r, &r.root, accepted);
            } else if (!strcmp(method, "GetName"))
                text(&r, &r.root, node.role==PS_A11Y_CHECKBOX?"toggle":"click");
            else if (!strcmp(method, "GetLocalizedName"))
                text(&r, &r.root, node.role==PS_A11Y_CHECKBOX?"Umschalten":"Klicken");
            else if (!strcmp(method, "GetDescription") || !strcmp(method, "GetKeyBinding"))
                text(&r, &r.root, "");
            else
                failure = DBUS_ERROR_UNKNOWN_METHOD;
        } else
            failure = DBUS_ERROR_INVALID_ARGS;
    } else if (!cache && !w && !strcmp(iface, AX_PREFIX "Socket") && !strcmp(method, "Embedded") &&
               dbus_message_has_signature(message, "s")) {
        /* The registry owns the parent; no application data can be changed here. */
    } else
        failure = DBUS_ERROR_UNKNOWN_METHOD;
    SDL_UnlockMutex(s->mutex);
    if (failure) {
        dbus_message_unref(r.message);
        return error_reply(connection, message, failure, detail);
    }
    if (!r.okay) {
        dbus_message_unref(r.message);
        return DBUS_HANDLER_RESULT_NEED_MEMORY;
    }
    dbus_connection_send(connection, r.message, NULL);
    dbus_message_unref(r.message);
    return DBUS_HANDLER_RESULT_HANDLED;
}
static int dispatcher(void *data) {
    ax_server *s = data;
    while (!SDL_GetAtomicInt(&s->stopping) &&
           dbus_connection_read_write_dispatch(s->connection, 20)) {
    }
    return 0;
}
static ax_server *server_create(void) {
    if (!dbus_threads_init_default())
        return NULL;
    const char *disabled = getenv("NO_AT_BRIDGE");
    if (disabled && !strcmp(disabled, "1"))
        return NULL;
    DBusError error;
    dbus_error_init(&error);
    DBusConnection *session = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
    if (!session) {
        dbus_error_free(&error);
        return NULL;
    }
    dbus_connection_set_exit_on_disconnect(session, FALSE);
    DBusMessage *request =
        dbus_message_new_method_call("org.a11y.Bus", "/org/a11y/bus", "org.a11y.Bus", "GetAddress");
    DBusMessage *reply =
        request ? dbus_connection_send_with_reply_and_block(session, request, 1000, &error) : NULL;
    if (request)
        dbus_message_unref(request);
    dbus_connection_close(session);
    dbus_connection_unref(session);
    const char *address = NULL;
    if (!reply ||
        !dbus_message_get_args(reply, NULL, DBUS_TYPE_STRING, &address, DBUS_TYPE_INVALID) ||
        strlen(address) >= 2048) {
        if (reply)
            dbus_message_unref(reply);
        dbus_error_free(&error);
        return NULL;
    }
    ax_server *s = calloc(1, sizeof *s);
    if (!s) {
        dbus_message_unref(reply);
        dbus_error_free(&error);
        return NULL;
    }
    strcpy(s->address, address);
    dbus_message_unref(reply);
    dbus_error_free(&error);
    dbus_error_init(&error);
    s->connection = dbus_connection_open_private(s->address, &error);
    s->mutex = SDL_CreateMutex();
    if (!s->connection || !s->mutex || !dbus_bus_register(s->connection, &error))
        goto fail;
    dbus_connection_set_exit_on_disconnect(s->connection, FALSE);
    static const DBusObjectPathVTable vtable = {.message_function = message_handler};
    if (!dbus_connection_register_fallback(s->connection, "/org/a11y/atspi", &vtable, s))
        goto fail;
    s->thread = SDL_CreateThread(dispatcher, "Physim AT-SPI", s);
    if (!s->thread)
        goto fail;
    request = dbus_message_new_method_call("org.a11y.atspi.Registry", AX_ROOT, AX_PREFIX "Socket",
                                           "Embed");
    if (!request)
        goto fail;
    ax_reply arguments = {.message = request, .okay = true};
    dbus_message_iter_init_append(request, &arguments.root);
    local_reference(&arguments, &arguments.root, s, 0, 0);
    reply = arguments.okay
                ? dbus_connection_send_with_reply_and_block(s->connection, request, 2000, &error)
                : NULL;
    dbus_message_unref(request);
    if (!reply)
        goto fail;
    dbus_message_unref(reply);
    dbus_error_free(&error);
    return s;
fail:
    if (getenv("PHYSIM_A11Y_TRACE"))
        fprintf(stderr, "AT-SPI initialization: %s: %s\n", error.name ? error.name : "local setup",
                error.message ? error.message : "allocation/thread/object registration failed");
    SDL_SetAtomicInt(&s->stopping, 1);
    if (s->connection)
        dbus_connection_close(s->connection);
    if (s->thread)
        SDL_WaitThread(s->thread, NULL);
    if (s->connection)
        dbus_connection_unref(s->connection);
    if (s->mutex)
        SDL_DestroyMutex(s->mutex);
    dbus_error_free(&error);
    free(s);
    return NULL;
}
static void update_window(ps_a11y_native *b) {
    b->screen_position = SDL_GetWindowPosition(b->window, &b->x, &b->y);
    SDL_GetWindowSize(b->window, &b->width, &b->height);
    b->visible = !(SDL_GetWindowFlags(b->window) & (SDL_WINDOW_HIDDEN | SDL_WINDOW_MINIMIZED));
    const char *title = SDL_GetWindowTitle(b->window);
    size_t bytes = strlen(title);
    if (bytes >= sizeof b->title) {
        bytes = sizeof b->title - 1;
        while (((unsigned char)title[bytes] & 0xc0) == 0x80)
            bytes--;
    }
    memcpy(b->title, title, bytes);
    b->title[bytes] = 0;
    if (!ps_text_valid(b->title, sizeof b->title, true))
        strcpy(b->title, "Physim");
}
ps_a11y_native *ps_a11y_native_create(SDL_Window *window, ps_a11y_model *model, SDL_Mutex *mutex) {
    if (!window || !model || !mutex)
        return NULL;
    ps_a11y_native *b = calloc(1, sizeof *b);
    if (!b)
        return NULL;
    if (!server)
        server = server_create();
    if (!server) {
        free(b);
        return NULL;
    }
    SDL_LockMutex(server->mutex);
    if (server->count == AX_WINDOWS) {
        SDL_UnlockMutex(server->mutex);
        free(b);
        return NULL;
    }
    b->server = server;
    b->window = window;
    b->model = model;
    b->mutex = mutex;
    b->window_id = SDL_GetWindowID(window);
    update_window(b);
    size_t index = server->count;
    server->windows[server->count++] = b;
    ps_a11y_node empty = {0};
    cache_signal(server, b, &empty, (int)index, true);
    event_reference(server, AX_ROOT, "ChildrenChanged", "add", (int)index, b->window_id, 0);
    SDL_UnlockMutex(server->mutex);
    return b;
}
void ps_a11y_native_publish(ps_a11y_native *b, bool changed) {
    if (!b)
        return;
    ax_server *s = b->server;
    SDL_LockMutex(s->mutex);
    bool old_visible = b->visible;
    int old_x = b->x, old_y = b->y, old_width = b->width, old_height = b->height;
    update_window(b);
    bool moved = old_x != b->x || old_y != b->y || old_width != b->width || old_height != b->height;
    if (changed || moved || old_visible != b->visible) {
        SDL_LockMutex(b->mutex);
        char window_path[128];
        path_for(window_path, b->window_id, 0);
        for (size_t i = 0; i < b->previous_count; i++)
            if (!ps_a11y_find(b->model, b->previous[i].id)) {
                cache_signal(s, b, &b->previous[i], (int)i, false);
                event_reference(s, window_path, "ChildrenChanged", "remove", (int)i, b->window_id,
                                b->previous[i].id);
            }
        for (size_t i = 0; i < b->model->count; i++) {
            const ps_a11y_node *node = &b->model->nodes[i];
            bool added = true;
            const ps_a11y_node *old = NULL;
            for (size_t j = 0; j < b->previous_count; j++)
                if (b->previous[j].id == node->id) {
                    added = false;
                    old = &b->previous[j];
                    break;
                }
            if (added) {
                cache_signal(s, b, node, (int)i, true);
                event_reference(s, window_path, "ChildrenChanged", "add", (int)i, b->window_id,
                                node->id);
            }
            char node_path[128];
            path_for(node_path, b->window_id, node->id);
            if (old && old->enabled != node->enabled) {
                event_state(s, node_path, "enabled", node->enabled);
                event_state(s, node_path, "sensitive", node->enabled);
            }
            if(old && node->role==PS_A11Y_CHECKBOX && old->checked!=node->checked)
                event_state(s,node_path,"checked",node->checked);
            if (moved || (old && memcmp(old->bounds, node->bounds, sizeof node->bounds)))
                event_bounds(s, node_path, b, node);
            if (old_visible != b->visible) {
                event_state(s, node_path, "showing", b->visible);
                event_state(s, node_path, "visible", b->visible);
            }
        }
        ps_a11y_node empty = {0};
        if (moved)
            event_bounds(s, window_path, b, &empty);
        if (old_visible != b->visible) {
            event_state(s, window_path, "showing", b->visible);
            event_state(s, window_path, "visible", b->visible);
        }
        memcpy(b->previous, b->model->nodes, b->model->count * sizeof *b->previous);
        b->previous_count = b->model->count;
        SDL_UnlockMutex(b->mutex);
    }
    SDL_UnlockMutex(s->mutex);
}
void ps_a11y_native_destroy(ps_a11y_native *b) {
    if (!b)
        return;
    ax_server *s = b->server;
    SDL_LockMutex(s->mutex);
    char window_path[128];
    path_for(window_path, b->window_id, 0);
    for (size_t i = 0; i < b->previous_count; i++)
        cache_signal(s, b, &b->previous[i], (int)i, false);
    size_t index = 0;
    while (index < s->count && s->windows[index] != b)
        index++;
    if (index < s->count) {
        ps_a11y_node empty = {0};
        cache_signal(s, b, &empty, (int)index, false);
        event_reference(s, AX_ROOT, "ChildrenChanged", "remove", (int)index, b->window_id, 0);
        memmove(s->windows + index, s->windows + index + 1,
                (s->count - index - 1) * sizeof *s->windows);
        s->count--;
    }
    SDL_UnlockMutex(s->mutex);
    SDL_DestroyMutex(b->mutex);
    free(b);
    if (!s->count) {
        SDL_SetAtomicInt(&s->stopping, 1);
        dbus_connection_close(s->connection);
        SDL_WaitThread(s->thread, NULL);
        dbus_connection_unref(s->connection);
        SDL_DestroyMutex(s->mutex);
        free(s);
        server = NULL;
    }
}
bool ps_a11y_native_press_label(ps_a11y_native *bridge, const char *label) {
    (void)bridge;
    (void)label;
    return false;
}
bool ps_a11y_native_test(SDL_Window *window) {
    (void)window;
    return false;
}
#else
typedef int ps_atspi_translation_unit;
#endif
