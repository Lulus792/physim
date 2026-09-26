# C-Referenz: Dynamische Arrays

Initialisieren, anhängen/einfügen, lesen und am Ende zerstören. Elemente werden byteweise kopiert, enthalten also keine automatisch verwalteten Unterobjekte. Wachstum kann alle Elementzeiger invalidieren. clear hält die Kapazität; destroy gibt den Speicher frei.

[Anleitung und Beispiele](../array.md) · [Lernpfade](../guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/array.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

Dieses Modul definiert keine zusätzlichen Zahlenkonstanten.

## Typen und Funktionen

### ps_array

```c
typedef struct {
    void *data;
    size_t count, capacity, element_size, maximum_count;
    ps_allocator allocator;
} ps_array;
```

Contiguous, byte-copyable elements. No constructors, destructors or deep copies. Treat metadata as read-only; data[0..count*element_size) is caller-writable. Do not copy a live owning struct. Initialize once, destroy before reinitializing. Element alignment may not exceed PS_MEMORY_ALIGNMENT; use sizeof(T) as stride. Allocator callbacks/user must outlive the array. External synchronization required.

## ps_array_init

Initialisiert ein leeres Array mit Elementgröße, Allocator und optionaler Maximalanzahl.

```c
ps_result ps_array_init(
    ps_allocator allocator,
    size_t element_size,
    size_t maximum_count,
    ps_array *out);
```

No allocation. maximum_count=0 selects SIZE_MAX/element_size. Invalid arguments -> PS_INVALID; overflowing maximum_count*element_size -> PS_LIMIT. All fallible operations preserve metadata, live elements and pointers on error. Allocator exhaustion -> PS_MEMORY, size/count limit -> PS_LIMIT.

## ps_array_reserve

Stellt mindestens die gewünschte Kapazität bereit, ohne die Elementanzahl zu ändern.

```c
ps_result ps_array_reserve(ps_array *array, size_t minimum_capacity);
```

Minimum capacity, geometric growth bounded by maximum_count. Growth invalidates all element pointers; peak memory can contain both old and new allocations.

## ps_array_resize

Ändert die Elementanzahl; neu sichtbare Bytes werden genullt.

```c
ps_result ps_array_resize(ps_array *array, size_t count);
```

Growing count zeroes the newly exposed bytes (not typed constructors). Shrinking/clear retain capacity; removed elements have no destructor callback.

## ps_array_insert

Fügt Elemente vor index ein und verschiebt den Rest; Selbsteinfügung wird unterstützt.

```c
ps_result ps_array_insert(
    ps_array *array,
    size_t index,
    const void *elements,
    size_t count);
```

Insert count readable elements at index<=array->count. NULL is allowed only for count=0. A source inside this array must start on an element boundary and lie wholly inside its live elements; self-insertion is supported, even on growth. In-place self-insertion before the end uses temporary allocated storage. Inserting/erasing shifts elements, invalidating positional references at/after index.

## ps_array_append

Hängt die angegebenen Elemente an das Ende an.

```c
ps_result ps_array_append(ps_array *array, const void *elements, size_t count);
```

## ps_array_erase

Entfernt einen zusammenhängenden Bereich und verschiebt nachfolgende Elemente.

```c
ps_result ps_array_erase(ps_array *array, size_t index, size_t count);
```

## ps_array_clear

Setzt die Anzahl auf null und behält den Speicher.

```c
void ps_array_clear(ps_array *array);
```

## ps_array_shrink

Reduziert die Kapazität auf die aktuelle Anzahl; kann bei nötiger Allokation fehlschlagen.

```c
ps_result ps_array_shrink(ps_array *array);
```

Reduce capacity to count; can allocate/fail unless already exact or empty. Destroy frees storage and zeroes the struct; destroying a zero struct is safe.

## ps_array_destroy

Gibt den Speicher frei und nullt den Arraydeskriptor.

```c
void ps_array_destroy(ps_array *array);
```
