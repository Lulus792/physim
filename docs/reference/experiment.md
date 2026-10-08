# C-Referenz: Experimente und Szenen

Ein Experiment exportiert ps_get_experiment. Der Host ruft create, reset, step, build_scene und destroy auf. ps_channel_add liefert einen Kanalindex oder -1; Messwerte werden über context->values[index] gesetzt. Kanäle nur einmal registrieren. Szenen werden pro Snapshot neu aufgebaut.

[Anleitung und Beispiele](../experiment-tutorial.md) · [Teil I – C](../c-guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/experiment.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

```c
#define PS_MAX_CHANNELS 16
#define PS_MAX_OBJECTS 32
#define PS_MAX_SCENE_POINTS 96
#define PS_MAX_PARAMETERS 16
#define PS_EXPERIMENT_SCENE_HIERARCHY UINT64_C(1)
#define PS_EXPERIMENT_ADAPTIVE_STEPS UINT64_C(2)
#define PS_EXPERIMENT_SCENE_FRAMES UINT64_C(4)
#define PS_EXPERIMENT_API_BASE_SIZE offsetof(ps_experiment_api, adaptive_step)
```

`PS_EXPORT` kennzeichnet den Moduleinstieg für den Export. Das SDK wählt dafür automatisch die passende Windows- beziehungsweise Unix-Deklaration.

## Typen und Funktionen

### ps_channel

```c
typedef struct {
    char name[48], unit[16], description[96];
    int8_t dimension[7];
} ps_channel;
```

ABI-3 scene extension: parent_id occupies the former ps_object tail padding. Modules must advertise this capability to publish parent relationships.

### ps_shape

```c
typedef enum {
    PS_SPHERE,
    PS_LINE,
    PS_BOX,
    PS_ARROW,
    PS_POINT,
    PS_PLANE,
    PS_POLYLINE,
    PS_LABEL,
    PS_GROUP,
    PS_FRAME
} ps_shape;
```

Named organizational node; no coordinate transform.

Named local coordinate frame: a=translation, b=scale, orientation=rotation.

### ps_object

```c
typedef struct {
    uint32_t shape, color;
    ps_vec3 a, b;
    double radius;
    ps_quat orientation;
    char text[64];
    uint32_t point_first, point_count;
    uint32_t id;
    uint32_t parent_id;
} ps_object;
```

Metres, Y up. Sphere: a=center, radius. Box: a=center, b=full XYZ extents; if an extent is nonpositive, a cube with half-size radius is used. Line/arrow: a=start, b=end, radius=shaft radius (0 selects 0.009 m). Color is RRGGBBAA. Alpha 0 hides geometry and labels, including picking. Mesh alpha 1..254 uses back-to-front triangle blending; intersecting transparent surfaces may show sorting artifacts. Alpha 255 is opaque.

Mesh rotation for box/plane (zero=identity); frame rotation must be nonzero.

UTF-8 label, terminated; empty for other shapes.

Polyline range in the scene point pool.

Optional stable ID within a run; 0 = anonymous. Unique per scene.

0 = root; otherwise a scene ID. Frame ancestors define local coordinates.

### ps_scene

```c
typedef struct {
    uint32_t count, point_count;
    ps_object objects[PS_MAX_OBJECTS];
    ps_vec3 points[PS_MAX_SCENE_POINTS];
} ps_scene;
```

### ps_parameter

```c
typedef struct {
    char name[48], description[96];
    double value, default_value, minimum, maximum;
    bool defined;
} ps_parameter;
```

### ps_parameter_unit

```c
typedef struct {
    int8_t dimension[7];
    double scale;
    char symbol[16];
    bool declared;
} ps_parameter_unit;
```

Owned display-unit metadata. Parameter values, defaults, bounds and overrides remain SI numbers; scale converts one display unit to SI. Undeclared is not the same as dimensionless (declare PS_ONE for that).

### ps_context

```c
typedef struct ps_context {
    uint32_t struct_size, api_version;
    void *user;
    double time_s, dt_s;
    uint64_t seed;
    ps_rng rng;
    uint32_t channel_count;
    ps_channel channels[PS_MAX_CHANNELS];
    double values[PS_MAX_CHANNELS];
    char error[256];
    char model_metadata[2048];
    uint32_t parameter_count;
    ps_parameter parameters[PS_MAX_PARAMETERS];
    ps_parameter_unit parameter_units[PS_MAX_PARAMETERS];
    ps_logger logger;
    ps_diagnostic diagnostic;
} ps_context;
```

Optional ABI-3 context extension; check struct_size before accessing it.

Optional ABI-3 tail. Old context/parameter layouts remain unchanged.

Optional ABI-3 tail. Host-owned logger; do not replace its descriptor.

Optional ABI-3 tail. Explicit failure record; prior fields keep their offsets.

### ps_step_interval

```c
typedef struct {
    double elapsed_s, next_s;
} ps_step_interval;
```

### ps_experiment_api

```c
typedef struct {
    uint32_t struct_size, abi_version;
    uint64_t capabilities;
    const char *name;
    ps_result (*create)(ps_context *context);
    ps_result (*reset)(ps_context *context);
    ps_result (*step)(ps_context *context, double dt_s);
    void (*build_scene)(ps_context *context, ps_scene *scene);
    void (*destroy)(ps_context *context);
    ps_result (*adaptive_step)(ps_context *context, double proposed_s, double minimum_s,
                               double maximum_s, ps_step_interval *interval);
} ps_experiment_api;
```

Optional ABI-3 tail, advertised with PS_EXPERIMENT_ADAPTIVE_STEPS. Accept one forward step of minimum_s <= elapsed_s <= proposed_s. next_s must lie in [minimum_s, maximum_s]. The host may lower minimum_s for the clipped final target-time step. Rejected numerical trials remain internal to the model and must not publish measurements/state. On success update the model and channel values, but not context->time_s. The host advances time and persists exactly one accepted sample.

### ps_experiment_entry

```c
typedef const ps_experiment_api *(*ps_experiment_entry)(void);
```

## ps_experiment_log

Schreibt über den Hostlogger mit der aktuellen Simulationszeit. Ein älterer Context ohne Logger-Tail liefert PS_VERSION.

```c
ps_result ps_experiment_log(
    const ps_context *context,ps_log_level level,const char *message);
```

Export ps_get_experiment from each module. Context and scene are owned by host. Module owns context->user and releases it in destroy, including failed create.

Log at the current host simulation time. Old contexts without the logger tail return PS_VERSION. A disabled logger succeeds. Logging never mutates channel values, RNG state or host time; sink errors are reported to caller.

## ps_experiment_fail

Speichert eine strukturierte Diagnose und einen kompatiblen Text im Hostcontext; liefert den gespeicherten Fehlercode.

```c
ps_result ps_experiment_fail(ps_context *context, const ps_diagnostic *diagnostic);
```

Publish a valid failure and return its stored code. Old contexts still receive a bounded legacy error string; only contexts with the optional tail store the full record. Invalid input is atomic. Read returns PS_VERSION for old contexts.

## ps_experiment_diagnostic

Liefert eine unabhängige Kopie der Experimentdiagnose; ein älterer Context ohne optionalen Tail meldet PS_VERSION.

```c
ps_result ps_experiment_diagnostic(const ps_context *context, ps_diagnostic *out);
```

## ps_channel_add

Kopiert einen eindeutigen SI-Kanal mit Skala 1 und begrenzten UTF-8-Metadaten ohne Kürzung; liefert Index oder -1 und erhält den Kontext bei Fehlern.

```c
int ps_channel_add(
    ps_context *context,
    const char *name,
    ps_unit unit,
    const char *description);
```

Declare a canonical SI channel: unit.scale must equal 1. Samples are SI numbers; convert display-unit inputs explicitly before storing in values. Copies bounded UTF-8 metadata without truncation: name 1..47 bytes, symbol 0..15, description 0..95. Name/symbol have no controls; descriptions permit tab/newline. Names must be unique. Returns index or -1, preserving the entire context on failure. No allocation or retained pointers. Legacy raw schemas retain their existing file-format semantics; this validates declarations.

## ps_channel_sample_quantity

Prüft Kanaldimension und Quantity, konvertiert deren Skala nach SI und aktualisiert atomar genau einen Kanalwert; Fehler erhalten den Kontext.

```c
ps_result ps_channel_sample_quantity(
    ps_context *context,
    uint32_t channel,
    ps_quantity value);
```

Typed sampling into the canonical SI channel. Dimensions must match; converts Quantity's declared scale before committing one value. Requires the complete values prefix; shorter contexts -> PS_VERSION. Invalid context/schema/index or Quantity -> PS_INVALID; conversion range failure -> PS_NUMERIC. Every failure preserves the entire context. No allocation or retained pointers. Phase/ownership checks belong to the calling runner or language host.

## ps_parameter_override

Hinterlegt vor create einen endlichen Wert für einen eindeutigen Parameternamen. Der Name muss später definiert werden; der Kontext benötigt die optionale ABI-3-Erweiterung.

```c
ps_result ps_parameter_override(ps_context *context, const char *name, double value);
```

## ps_parameter_define

Definiert einen Parameter mit Beschreibung, endlichem Standardwert und inklusiven Grenzen. Liefert den Override oder Standardwert über value; gültig beim Erzeugen des Experiments.

```c
ps_result ps_parameter_define(
    ps_context *context,
    const char *name,
    const char *description,
    double default_value,
    double minimum,
    double maximum,
    double *value);
```

## ps_parameter_finalize

Prüft nach create, ob jeder vorgegebene Override durch das Experiment definiert wurde.

```c
ps_result ps_parameter_finalize(const ps_context *context);
```

## ps_parameter_define_unit

Definiert SI-Parameterwerte mit deklarierter Anzeigeeinheit. Kopiert Symbol, Skala und Dimensionen in den optionalen ABI-3-Kontext-Tail; Grenzen müssen in der Anzeigeeinheit darstellbar sein. Fehler bewahren Kontext und Ausgabe.

```c
ps_result ps_parameter_define_unit(
    ps_context *context,
    const char *name,
    const char *description,
    ps_unit display_unit,
    double default_value,
    double minimum,
    double maximum,
    double *value);
```

Defines SI values with an owned display-unit declaration. Requires the unit tail in struct_size. Invalid declarations leave context and value unchanged.

## ps_parameter_unit_read

Liest die eigene Einheitendeklaration eines Parameters. Alte Kontexte und untypisierte Parameter liefern declared=false und Skala 1; dies behauptet keine Dimensionslosigkeit.

```c
ps_result ps_parameter_unit_read(
    const ps_context *context,
    uint32_t index,
    ps_parameter_unit *unit);
```

Old contexts and untyped parameters return undeclared metadata, scale 1. Outputs remain unchanged on error.

## ps_parameter_unit_parse

Liest parameter_unit/scale/dimension aus Laufmetadaten. Fehlende Deklarationen bleiben unbekannt; unvollständige, doppelte oder ungültige Felder ergeben PS_CORRUPT. Ausgabe bleibt bei Fehlern erhalten.

```c
ps_result ps_parameter_unit_parse(
    const char *metadata,
    const char *name,
    ps_parameter_unit *unit);
```

Reads the three parameter_unit/scale/dimension keys from run metadata. Missing declarations return undeclared; partial, duplicate or malformed keys return PS_CORRUPT. Numbers in run metadata are always SI.

## ps_scene_add

Fügt ein anonymes einfaches Objekt hinzu; Fehler sind hier nicht als Rückgabewert verfügbar.

```c
void ps_scene_add(
    ps_scene *scene,
    ps_shape shape,
    ps_vec3 a,
    ps_vec3 b,
    double radius,
    uint32_t rgba);
```

## ps_scene_push

Prüft und kopiert ein vollständig beschriebenes Szenenobjekt.

```c
ps_result ps_scene_push(ps_scene *scene, const ps_object *object);
```

Checked helpers are atomic on failure. Clear the whole scene before building it. Plane: a=center, b.x/b.z=full side lengths in local XZ; orientation rotates it. Polyline coordinates and label anchors use the coordinates of frame ancestors; without frames these remain world-space values.

## ps_scene_polyline

Kopiert mindestens zwei lokale Punkte (ohne Rahmen Weltpunkte) in den Szenenpunktpuffer und fügt einen Linienzug hinzu.

```c
ps_result ps_scene_polyline(
    ps_scene *scene,
    const ps_vec3 *points,
    size_t count,
    double radius,
    uint32_t rgba);
```

## ps_scene_label

Kopiert eine UTF-8-Beschriftung mit lokalem Anker (ohne Rahmen Weltanker) in die Szene.

```c
ps_result ps_scene_label(
    ps_scene *scene,
    ps_vec3 position,
    const char *text,
    uint32_t rgba);
```

## ps_scene_valid

Prüft den vollständigen Snapshot auf Form-, Zahlen-, Text-, ID- und Punktbereichsregeln.

```c
bool ps_scene_valid(const ps_scene *scene);
```

## ps_scene_group

Erzeugt eine benannte Gruppe mit eindeutiger ID und optionalem Elternknoten; fehlerhafte Beziehungen verändern die Szene nicht.

```c
ps_result ps_scene_group(
    ps_scene *scene,
    uint32_t id,
    uint32_t parent_id,
    const char *name);
```

Named groups require a nonzero unique ID. Parent 0 is the scene root. Checked relationships reject missing parents, self-parenting and cycles. Failure leaves the scene unchanged. Publish with PS_EXPERIMENT_SCENE_HIERARCHY.

## ps_scene_frame

Erzeugt einen expliziten TRS-Koordinatenrahmen mit eindeutiger ID und geprüftem Elternknoten. Skalierungen müssen endlich und ungleich null sein; Fehler erhalten die Szene.

```c
ps_result ps_scene_frame(
    ps_scene *scene,
    uint32_t id,
    uint32_t parent_id,
    const char *name,
    ps_vec3 translation,
    ps_quat rotation,
    ps_vec3 scale);
```

Explicit TRS frame. Scale components must be finite and nonzero (reflection is supported); rotation must be finite and nonzero, normalized internally. Descendants inherit frame transforms through organizational/geometry nodes. Group and geometry nodes do not supply additional transforms. T*R*S order; nested nonuniform scales can produce shear. No physical coupling. Publish with SCENE_HIERARCHY | SCENE_FRAMES. Existing object/scene layouts stay unchanged.

## ps_scene_transforms

Berechnet für jeden Szenenslot die zusammengesetzte Local-to-world-Matrix. Keine Allokation, kein gemeinsamer Cache; Fehler erhalten das Ausgabearray.

```c
ps_result ps_scene_transforms(const ps_scene *scene, ps_mat4 *out);
```

Returns one local-to-world matrix per scene slot. For a frame it includes its own TRS; for geometry its mesh/positions are transformed by frame ancestors. Provide PS_MAX_OBJECTS matrices. No allocation. Outputs unchanged on error: PS_INVALID for malformed scenes, PS_NUMERIC for unrepresentable composition or normal transform, PS_SINGULAR for a numerically singular composed basis.

## ps_scene_world_point

Konvertiert einen Punkt in der Koordinatenbasis des Eintrags in Weltkoordinaten; fehlerhafte Transformationen erhalten die Ausgabe.

```c
ps_result ps_scene_world_point(
    const ps_scene *scene,
    uint32_t index,
    ps_vec3 local,
    ps_vec3 *out);
```

## ps_scene_set_parent

Ändert die Eltern-ID eines benannten Szeneneintrags; fehlende IDs, Selbstbeziehungen und Zyklen werden transaktional abgewiesen.

```c
ps_result ps_scene_set_parent(ps_scene *scene, uint32_t child_id, uint32_t parent_id);
```

## ps_scene_parent_index

Liefert den Slot des Elternknotens oder -1 für Wurzeln, ungültige Slots und fehlende Eltern.

```c
int ps_scene_parent_index(const ps_scene *scene, uint32_t index);
```

Returns the parent's slot, or -1 for a root, invalid index or missing parent.

## ps_scene_add_id

Fügt ein einfaches Objekt mit optionaler stabiler ID hinzu und meldet Fehler.

```c
ps_result ps_scene_add_id(
    ps_scene *scene,
    uint32_t id,
    ps_shape shape,
    ps_vec3 a,
    ps_vec3 b,
    double radius,
    uint32_t rgba);
```

ID-bearing constructors. ID 0 is anonymous; duplicate nonzero IDs fail without changing the scene, including its polyline point pool.

## ps_scene_polyline_id

Fügt einen Linienzug mit stabiler ID hinzu; Fehler verändern weder Objekte noch Punktpuffer.

```c
ps_result ps_scene_polyline_id(
    ps_scene *scene,
    uint32_t id,
    const ps_vec3 *points,
    size_t count,
    double radius,
    uint32_t rgba);
```

## ps_scene_label_id

Fügt eine Beschriftung mit stabiler ID hinzu; doppelte nichtnull IDs werden abgewiesen.

```c
ps_result ps_scene_label_id(
    ps_scene *scene,
    uint32_t id,
    ps_vec3 position,
    const char *text,
    uint32_t rgba);
```
