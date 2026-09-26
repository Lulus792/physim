# C-Referenz: Allocator und Arena

Speicher gehört einem expliziten Allocator. Verwende denselben Allocator und die korrekte Größe beim Freigeben. Eine Arena nutzt einen Puffer des Aufrufers; Reset macht alle daraus vergebenen Bereiche logisch ungültig. PS_MEMORY_ALIGNMENT bezeichnet die unterstützte fundamentale Ausrichtung.

[Anleitung und Beispiele](../memory.md) · [Lernpfade](../guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/memory.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

```c
#define PS_MEMORY_ALIGNMENT _Alignof(ps_memory_alignment)
```

## Typen und Funktionen

### ps_memory_alignment

```c
typedef union {
    long double floating;
    long long integer;
    void *pointer;
} ps_memory_alignment;
```

UCRT's C headers do not expose max_align_t. These are the maximally aligned fundamental C scalar types in the supported MSVC/Clang-Cl Windows ABI.

### ps_allocator

```c
typedef struct {
    void *user;
    void *(*allocate)(void *user, size_t bytes);
    void (*deallocate)(void *user, void *pointer, size_t bytes);
} ps_allocator;
```

An allocation domain, passed explicitly and copied by value into owners. user and callback code must outlive every allocation. No global allocator. allocate returns NULL on failure or distinct writable storage for `bytes`, aligned to PS_MEMORY_ALIGNMENT. Both callbacks receive strictly positive sizes; deallocate receives the exact original allocation size. They must not throw, longjmp or reenter the object currently being allocated/destroyed.

## ps_allocator_default

Liefert den Standardallocator der Bibliothek.

```c
ps_allocator ps_allocator_default(void);
```

## ps_allocator_valid

Prüft, ob die erforderlichen Allokationscallbacks vorhanden sind.

```c
bool ps_allocator_valid(ps_allocator allocator);
```

## ps_memory_allocate

Reserviert bytes mit geprüfter Allokationsdomäne; null Bytes ergeben NULL.

```c
ps_result ps_memory_allocate(ps_allocator allocator, size_t bytes, void **out);
```

Checked helpers: PS_INVALID for invalid allocator/output or mismatched NULL/size; PS_LIMIT for count*size overflow; PS_MEMORY on allocation failure. Outputs remain unchanged on failure. A zero-byte request succeeds with NULL and does not invoke a callback. Newly allocated bytes are otherwise uninitialized.

## ps_memory_zero

Reserviert count Elemente der angegebenen Größe, prüft Multiplikationsüberlauf und nullt die Bytes.

```c
ps_result ps_memory_zero(ps_allocator allocator, size_t count, size_t size, void **out);
```

## ps_memory_free

Gibt einen Bereich mit zugehörigem Allocator und ursprünglicher Größe frei.

```c
void ps_memory_free(ps_allocator allocator, void *pointer, size_t bytes);
```

## ps_memory_resize

Ändert die Speichergröße unter Erhaltung des gemeinsamen Präfixes; Fehler lassen den ursprünglichen Speicher bestehen.

```c
ps_result ps_memory_resize(
    ps_allocator allocator,
    void *old_pointer,
    size_t old_bytes,
    size_t new_bytes,
    void **out);
```

Allocate-copy-free, preserving min(old_bytes,new_bytes); new tail uninitialized. On failure old storage/content and *out survive. On success old storage is freed, except for equal-size requests. new_bytes=0 frees old storage and returns NULL. old_bytes must match its allocation, and out must not lie inside old storage.

### ps_arena

```c
typedef struct {
    unsigned char *buffer;
    size_t capacity, used;
} ps_arena;
```

Fixed caller-owned storage, no heap fallback. Treat fields as read-only after init. Each request uses PS_MEMORY_ALIGNMENT, even with an unaligned buffer. Individual frees do not reclaim space; destroy users before reset. Resize consumes a new block. All pointers become invalid after reset/reinitialization. The arena and its buffer must outlive users; external synchronization required.

## ps_arena_init

Initialisiert eine feste Arena auf einem Puffer des Aufrufers; die vergebenen Bereiche werden intern ausgerichtet.

```c
ps_result ps_arena_init(ps_arena *arena, void *buffer, size_t capacity);
```

## ps_arena_allocator

Liefert einen Allocator, der Speicher fortlaufend aus der Arena vergibt.

```c
ps_allocator ps_arena_allocator(ps_arena *arena);
```

## ps_arena_reset

Setzt den Arena-Verbrauch zurück; bisher vergebene Bereiche dürfen nicht weiter benutzt werden.

```c
void ps_arena_reset(ps_arena *arena);
```
