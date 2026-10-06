# C-Referenz: Indizierte Laufdateien

Ein Index besitzt eine unverändert geöffnete Messdatei und Checkpoints aus einem expliziten Allocator. Das Öffnen prüft den gesamten lesbaren Präfix einmal und rekonstruiert auch alte oder unvollständige Dateien. Gezielte Messblöcke und Szenen verwenden diese geprüften Positionen. PS_OK und PS_RECOVERED liefern beide einen zu zerstörenden Handle; andere Fehler erhalten die Ausgabe.

[Anleitung und Beispiele](../run-index.md) · [Lernpfade](../guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/run_index.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

```c
#define PS_RUN_INDEX_VERSION 1u
#define PS_RUN_INDEX_STRIDE 256u
#define PS_RUN_INDEX_BLOCK 256u
```

## Typen und Funktionen

### ps_run_index

```c
typedef struct ps_run_index ps_run_index;
```

### ps_run_index_info

```c
typedef struct {
    uint32_t struct_size, version, channels;
    uint64_t samples, snapshots, checkpoints;
    bool complete, persisted;
    char metadata[8192];
    ps_channel schema[PS_MAX_CHANNELS];
} ps_run_index_info;
```

## ps_run_index_open

Öffnet und validiert einen Lauf mit begrenzten Allocator-Checkpoints; liefert auch bei PS_RECOVERED einen besitzenden Handle.

```c
ps_result ps_run_index_open(
    const char *path,
    ps_allocator allocator,
    size_t maximum_entries,
    ps_run_index **out);
```

Owns one read-only open file and allocator-backed checkpoints. Scans and validates the readable prefix once, including scenes; never changes the file. PS_OK = valid footer; PS_RECOVERED = validated prefix without a valid footer. Both return an owned handle. All other results preserve *out. maximum_entries is a required nonzero bound on sample checkpoints + scenes. Allocator callbacks/user must outlive the handle. No concurrent use/mutation; the open file remains the same file even if its path is renamed/replaced.

## ps_run_index_destroy

Schließt die unverändert geöffnete Datei und gibt alle Checkpoints an den ursprünglichen Allocator zurück.

```c
void ps_run_index_destroy(ps_run_index *index);
```

## ps_run_index_get_info

Kopiert Schema, Metadaten, Präfixzählungen und Status nach Prüfung der Ausgabegröße und Version.

```c
ps_result ps_run_index_get_info(const ps_run_index *index, ps_run_index_info *out);
```

Initialize out.struct_size=sizeof *out and out.version=PS_RUN_INDEX_VERSION. Copies metadata/schema and counts; invalid size/version preserves the output. persisted means the validated on-disk index exactly matches the reconstructed checkpoints, not merely a valid CRC.

## ps_run_index_read

Liest bis zu 256 Messzeilen ab einer nullbasierten Zeilennummer; alle Ausgaben bleiben bei Fehlern unverändert.

```c
ps_result ps_run_index_read(
    ps_run_index *index,
    uint64_t first,
    size_t count,
    double *times,
    double *values);
```

Zero-based rows, at most PS_RUN_INDEX_BLOCK per call. times[count] and values[count*channels] are caller-owned, disjoint row-major outputs. Atomic on error; PS_EOF for an unavailable range, PS_LIMIT for an oversized block. count=0 permits NULL outputs. Checks CRC/values again when reading.

## ps_run_index_snapshot

Liest eine aufgezeichnete Szene anhand ihrer nullbasierten Nummer mit erneuter CRC- und Snapshotprüfung.

```c
ps_result ps_run_index_snapshot(
    ps_run_index *index,
    uint64_t ordinal,
    ps_snapshot *out);
```

Zero-based recorded scene; validates the selected chunk again. PS_EOF for an unavailable scene. No allocation. Failed reads preserve the output.
