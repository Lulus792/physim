# C-Referenz: Versionierte Run-Streams

Ein opaker Store besitzt begrenzte Reader-/Writer-Slots. Handles prüfen Besitzer und Generation; Schließen invalidiert alle Kopien. Reader geben kopierte Metadaten und atomar validierte Zeilen/Snapshots zurück. Writer finalisieren ausdrücklich oder bewahren durch Abort einen unvollständigen Präfix. Der Store und sein Allocator müssen alle Handle-Nutzungen überleben.

[Anleitung und Beispiele](../run-streams.md) · [Teil I – C](../c-guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/run_stream.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

```c
#define PS_RUN_STREAM_MAX_READERS 8u
#define PS_RUN_STREAM_MAX_WRITERS 8u
```

## Typen und Funktionen

### ps_run_store

```c
typedef struct ps_run_store ps_run_store;
```

### ps_run_read_handle

```c
typedef struct {
    const ps_run_store *owner;
    uint32_t slot, generation;
} ps_run_read_handle;
```

### ps_run_write_handle

```c
typedef struct {
    const ps_run_store *owner;
    uint32_t slot, generation;
} ps_run_write_handle;
```

### ps_run_read_info

```c
typedef struct {
    uint32_t channels;
    uint64_t samples;
    bool complete;
    char metadata[8192];
    ps_channel schema[PS_MAX_CHANNELS];
} ps_run_read_info;
```

### ps_run_write_info

```c
typedef struct {
    uint32_t channels;
    uint64_t samples;
} ps_run_write_info;
```

## ps_run_store_create

Erzeugt einen opaken Stream-Store aus einem expliziten Allocator; Ausgabe bleibt bei Fehlern unverändert.

```c
ps_result ps_run_store_create(ps_allocator allocator, ps_run_store **out);
```

Explicit allocation domain and owned open files; no mutable public internals. Descriptor copied by value; callbacks/user outlive store. Per-store external synchronization required. Handles belong to this live store; do not retain them after destruction. Closing and slot reuse never resurrect an old handle. At most eight readers and eight writers; exhausted generations retire slots. All constructors/open/describe operations preserve outputs on failure.

## ps_run_store_destroy

Schließt Reader, finalisiert noch lebende Writer und gibt alle Ressourcen frei; erster Abschlussfehler wird zurückgegeben.

```c
ps_result ps_run_store_destroy(ps_run_store *store);
```

Closes readers and finalizes live writers. All resources are released even on I/O failure; the first writer-finalization failure is returned.

## ps_run_reader_open

Öffnet einen Run-Reader in einem freien generationierten Slot; Ausgabe bleibt bei Fehlern unverändert.

```c
ps_result ps_run_reader_open(
    ps_run_store *store,
    const char *path,
    ps_run_read_handle *out);
```

## ps_run_reader_describe

Kopiert Kanalplan, Metadaten und aktuellen Lesestatus eines gültigen Reader-Handles.

```c
ps_result ps_run_reader_describe(
    ps_run_store *store,
    ps_run_read_handle handle,
    ps_run_read_info *out);
```

## ps_run_reader_next

Liest eine vollständig validierte Messzeile atomar; alle anderen Status erhalten Zeit, Werte und Anzahl.

```c
ps_result ps_run_reader_next(
    ps_run_store *store,
    ps_run_read_handle handle,
    double *time,
    double *values,
    size_t capacity,
    size_t *count);
```

capacity must hold the entire schema. PS_OK commits exactly one validated measurement row; every other status preserves time, values and count. EOF/recovered status remains terminal for this handle.

## ps_run_reader_snapshot_next

Liest den nächsten validierten Snapshot über denselben Cursor; Fehler erhalten die Ausgabe.

```c
ps_result ps_run_reader_snapshot_next(
    ps_run_store *store,
    ps_run_read_handle handle,
    ps_snapshot *out);
```

## ps_run_reader_release

Schließt den Reader und invalidiert sämtliche Kopien seiner Kennung.

```c
ps_result ps_run_reader_release(ps_run_store *store, ps_run_read_handle handle);
```

## ps_run_writer_create

Erzeugt eine exklusive Laufdatei in einem freien generationierten Writer-Slot.

```c
ps_result ps_run_writer_create(
    ps_run_store *store,
    const char *path,
    const ps_context *context,
    const char *name,
    ps_run_write_handle *out);
```

## ps_run_writer_describe

Kopiert Kanalzahl und Zahl geschriebener Messzeilen eines gültigen Writers.

```c
ps_result ps_run_writer_describe(
    ps_run_store *store,
    ps_run_write_handle handle,
    ps_run_write_info *out);
```

## ps_run_writer_append

Schreibt genau den deklarierten Kanalplan als endliche Messzeile; I/O-Fehler können einen unvollständigen Präfix hinterlassen.

```c
ps_result ps_run_writer_append(
    ps_run_store *store,
    ps_run_write_handle handle,
    double time,
    const double *values,
    size_t count);
```

Exactly the declared channel count, finite row. Existing run-format contracts apply; failed file writes can leave an incomplete recoverable tail.

## ps_run_writer_snapshot

Schreibt einen validierten Szenenzustand ohne neue Messzeile.

```c
ps_result ps_run_writer_snapshot(
    ps_run_store *store,
    ps_run_write_handle handle,
    const ps_context *context,
    const ps_scene *scene,
    bool paused);
```

## ps_run_writer_abort

Schließt ohne Erfolgsfooter und invalidiert die Kennung; geschriebene valide Präfixdaten bleiben wiederherstellbar.

```c
ps_result ps_run_writer_abort(ps_run_store *store, ps_run_write_handle handle);
```

Closes without a footer, preserving an incomplete recoverable prefix. Invalidates the handle even on fclose failure.

## ps_run_writer_release

Finalisiert Index und Footer und invalidiert die Kennung auch bei einem Abschlussfehler.

```c
ps_result ps_run_writer_release(ps_run_store *store, ps_run_write_handle handle);
```

Finalizes and invalidates the handle even when finalization returns an error.
