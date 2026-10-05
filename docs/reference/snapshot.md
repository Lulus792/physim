# C-Referenz: Aufgezeichnete Szenenzustände

Ein Snapshot verbindet Simulationszeit, Kanalwerte, Pausestatus und die vollständige Szene. Er enthält keine geliehenen Zeiger. Der Encoder benötigt PS_SNAPSHOT_MAX beschreibbare Bytes; der Decoder validiert das gesamte Format und erhält alle Ausgaben bei Fehlern. Die versionierten Datenblöcke teilen diesen Codec mit der Runner-Pipe.

[Anleitung und Beispiele](../data-format.md) · [Lernpfade](../guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/snapshot.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

```c
#define PS_SNAPSHOT_VERSION 2u
#define PS_SNAPSHOT_MAX 8192u
#define PS_SNAPSHOT_HEADER 24u
#define PS_SNAPSHOT_OBJECT_SIZE 176u
```

## Typen und Funktionen

### ps_snapshot

```c
typedef struct {
    double time, values[PS_MAX_CHANNELS];
    uint32_t count;
    ps_scene scene;
    bool paused;
} ps_snapshot;
```

Immutable physical state shared by IPC and optional run-file scene chunks.

## ps_snapshot_encode

Kodiert Zeit, Kanalwerte, Pausestatus und validierte Geometrie explizit little-endian; benötigt PS_SNAPSHOT_MAX Bytes und liefert bei ungültigen Eingaben 0.

```c
size_t ps_snapshot_encode(
    unsigned char *out,
    const ps_context *context,
    const ps_scene *scene,
    bool paused);
```

out provides PS_SNAPSHOT_MAX bytes. Returns encoded length, or 0 for invalid input. Time and all channel values must be finite; scene must be valid.

## ps_snapshot_decode

Dekodiert einen vollständigen Zustand mit Größen-, Zahlen-, Text- und Geometrieprüfung; Fehler lassen sämtliche Ausgaben unverändert.

```c
bool ps_snapshot_decode(
    const unsigned char *in,
    uint32_t size,
    double *time,
    double *values,
    uint32_t *count,
    ps_scene *scene,
    bool *paused);
```

Failure preserves all outputs. Provide space for PS_MAX_CHANNELS values.

## ps_snapshot_decode_version

Dekodiert die ausdrücklich genannte Szenenversion; Version 1 erhält Eltern-ID 0, Version 2 prüft Hierarchien. Fehler erhalten alle Ausgaben.

```c
bool ps_snapshot_decode_version(
    uint32_t version,
    const unsigned char *in,
    uint32_t size,
    double *time,
    double *values,
    uint32_t *count,
    ps_scene *scene,
    bool *paused);
```

Version 1 restores a flat scene with parent_id=0. Version 2 includes hierarchy. The declared version determines the exact payload size; unknown versions fail.
