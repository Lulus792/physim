# C-Referenz: Hashmap

Erzeuge eine Map mit Wertgröße und expliziten Grenzen. set kopiert Schlüssel und Wert, get kopiert in deinen Ausgabepuffer. PS_EOF bedeutet fehlender Schlüssel. Besuchercallbacks dürfen lesen, aber keine Änderungen an derselben Map vornehmen. destroy gibt alle map-eigenen Speicherbereiche frei.

[Anleitung und Beispiele](../hashmap.md) · [Teil I – C](../c-guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/hashmap.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

Dieses Modul definiert keine zusätzlichen Zahlenkonstanten.

## Typen und Funktionen

### ps_hashmap

```c
typedef struct ps_hashmap ps_hashmap;
```

## ps_hashmap_create

Erzeugt eine Map mit kopierten Byteschlüsseln, fester Wertgröße und expliziten Kapazitätsgrenzen.

```c
ps_result ps_hashmap_create(
    ps_allocator allocator,
    size_t value_size,
    size_t maximum_entries,
    size_t maximum_key_bytes,
    ps_hashmap **out);
```

Owned byte keys (including empty keys/NULs), fixed-size byte-copyable values. No deep copies or element destructors. Values align to PS_MEMORY_ALIGNMENT. value_size and maximum_entries must be positive. maximum_key_bytes may be 0 to allow only the empty key. Allocator callbacks/user must outlive the map. FNV-1a with chained collisions, not a cryptographic or adversarial hash table. No internal synchronization. All limits are explicit; no hidden defaults.

## ps_hashmap_set

Kopiert einen neuen Schlüssel/Wert oder ersetzt einen bestehenden Wert.

```c
ps_result ps_hashmap_set(ps_hashmap *map, ps_string_view key, const void *value);
```

Insert or replace; keys and values are copied. Replacement does not allocate and works at the entry limit. Source values may be borrowed from this map. Failure preserves entries and traversal order. PS_LIMIT for size limits or overflow, PS_MEMORY for allocation failure, PS_INVALID for invalid arguments.

## ps_hashmap_get

Kopiert den gefundenen Wert in out; PS_EOF bedeutet fehlenden Schlüssel.

```c
ps_result ps_hashmap_get(const ps_hashmap *map, ps_string_view key, void *out);
```

Copy one value into caller-owned storage of value_size bytes; must not overlap map-owned storage. Missing key -> PS_EOF, output unchanged.

## ps_hashmap_erase

Entfernt den Eintrag zum Schlüssel und gibt seinen Speicher frei.

```c
ps_result ps_hashmap_erase(ps_hashmap *map, ps_string_view key);
```

## ps_hashmap_count

Liefert die aktuelle Eintragszahl; NULL ergibt null.

```c
size_t ps_hashmap_count(const ps_hashmap *map);
```

## ps_hashmap_reserve

Reserviert Buckets für die gewünschte Eintragszahl; einzelne Knoten werden weiterhin separat angelegt.

```c
ps_result ps_hashmap_reserve(ps_hashmap *map, size_t minimum_entries);
```

NULL -> 0

Preallocate buckets for this many entries (nodes still allocate individually). Clear frees entries but retains buckets. Neither invalidates external key copies.

## ps_hashmap_clear

Entfernt alle Einträge und behält die Bucketkapazität.

```c
ps_result ps_hashmap_clear(ps_hashmap *map);
```

### ps_string_view

```c
typedef ps_result (*ps_hashmap_visit_fn)(ps_string_view key, const void *value, void *user);
```

Visit each entry once; order unspecified and may change after mutation. Borrowed key/value pointers are read-only. Copy data to retain independently. Addresses survive reserve/other insertions, but not erase/clear/destroy of the entry; replacement changes the existing value in place. Visitor result other than PS_OK stops and is propagated; completed visits are not rolled back. Read operations allowed, mutation/reentrant visit -> PS_INVALID. Do not destroy the map or longjmp from a callback. destroy is a no-op while visiting, to prevent invalidation of an active traversal.

## ps_hashmap_visit

Besucht jeden Eintrag mit geliehenen schreibgeschützten Schlüssel-/Wertzeigern; die Reihenfolge ist nicht zugesichert.

```c
ps_result ps_hashmap_visit(ps_hashmap *map, ps_hashmap_visit_fn visit, void *user);
```

## ps_hashmap_destroy

Gibt Map, Schlüssel und Werte frei; während einer aktiven Traversierung wirkungslos.

```c
void ps_hashmap_destroy(ps_hashmap *map);
```
