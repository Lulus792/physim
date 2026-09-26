# C-Referenz: Experimente und Szenen

Ein Experiment exportiert ps_get_experiment. Der Host ruft create, reset, step, build_scene und destroy auf. ps_channel_add liefert einen Kanalindex oder -1; Messwerte werden über context->values[index] gesetzt. Kanäle nur einmal registrieren. Szenen werden pro Snapshot neu aufgebaut.

[Anleitung und Beispiele](../experiment-tutorial.md) · [Lernpfade](../guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/experiment.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

```c
#define PS_MAX_CHANNELS 16
#define PS_MAX_OBJECTS 32
#define PS_MAX_SCENE_POINTS 96
#define PS_MAX_PARAMETERS 16
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
    PS_LABEL
} ps_shape;
```

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
} ps_object;
```

Metres, Y up. Sphere: a=center, radius. Box: a=center, b=full XYZ extents; if an extent is nonpositive, a cube with half-size radius is used. Line/arrow: a=start, b=end, radius=shaft radius (0 selects 0.009 m). Color is RRGGBBAA. Alpha 0 hides geometry and labels, including picking. Mesh alpha 1..254 uses back-to-front triangle blending; intersecting transparent surfaces may show sorting artifacts. Alpha 255 is opaque.

Local-to-world rotation for box/plane. Zero means identity.

UTF-8 label, terminated; empty for other shapes.

Polyline range in the scene point pool.

Optional stable ID within a run; 0 = anonymous. Unique per scene.

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
} ps_context;
```

Optional ABI-3 context extension; check struct_size before accessing it.

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
} ps_experiment_api;
```

### ps_experiment_entry

```c
typedef const ps_experiment_api *(*ps_experiment_entry)(void);
```

## ps_channel_add

Registriert einen skalaren Messkanal und liefert seinen Index; -1 bedeutet Fehler. Namen, Einheitensymbol und Beschreibung passend zu den Kapazitäten halten.

```c
int ps_channel_add(
    ps_context *context,
    const char *name,
    ps_unit unit,
    const char *description);
```

Export ps_get_experiment from each module. Context and scene are owned by host. Module owns context->user and releases it in destroy, including failed create.

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

Checked helpers are atomic on failure. Clear the whole scene before building it. Plane: a=center, b.x/b.z=full side lengths in local XZ; orientation rotates it. Polyline coordinates are in world space; label positions are annotation anchors.

## ps_scene_polyline

Kopiert mindestens zwei Weltpunkte in den Szenenpunktpuffer und fügt einen Linienzug hinzu.

```c
ps_result ps_scene_polyline(
    ps_scene *scene,
    const ps_vec3 *points,
    size_t count,
    double radius,
    uint32_t rgba);
```

## ps_scene_label

Kopiert eine UTF-8-Beschriftung mit Weltanker in die Szene.

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
