# C-Referenz: String-Views

Ein View leiht Bytes und besitzt keinen Speicher. Der Quellpuffer muss leben. Indizes zählen Bytes, nicht Unicode-Zeichen; abschließende Nullzeichen sind nicht erforderlich. Verwende copy, wenn du einen eigenen terminierenden Puffer benötigst.

[Anleitung und Beispiele](../string-view.md) · [Teil I – C](../c-guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/string_view.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

Dieses Modul definiert keine zusätzlichen Zahlenkonstanten.

## Typen und Funktionen

### ps_string_view

```c
typedef struct {
    const char *data;
    size_t size;
} ps_string_view;
```

Borrowed read-only bytes, not necessarily NUL terminated. Caller guarantees a readable region of size bytes and keeps it alive; views never own memory. NULL is valid only with size=0. Embedded NULs are ordinary bytes. UTF-8 may be stored, but offsets, ordering and slicing are byte-based, not Unicode-aware.

## ps_string_view_valid

Prüft die Zeiger-/Längenkombination; prüft weder UTF-8 noch tatsächliche Pufferlebensdauer.

```c
bool ps_string_view_valid(ps_string_view view);
```

## ps_string_view_make

Leiht einen explizit begrenzten Bytebereich.

```c
ps_result ps_string_view_make(const char *data, size_t size, ps_string_view *out);
```

## ps_string_view_cstr

Leiht eine gültige nullterminierte Zeichenkette ohne ihr abschließendes Nullbyte.

```c
ps_result ps_string_view_cstr(const char *text, ps_string_view *out);
```

Requires a valid terminated C string; NULL is invalid.

## ps_string_view_compare

Vergleicht lexikografisch als vorzeichenlose Bytes; order wird -1, 0 oder 1.

```c
ps_result ps_string_view_compare(ps_string_view a, ps_string_view b, int *order);
```

Lexicographic unsigned-byte order; result is -1, 0 or 1. Invalid -> PS_INVALID. All checked operations leave outputs unchanged on error, including PS_EOF.

## ps_string_view_equal

Prüft gleiche Bytefolgen; ungültige Views sind niemals gleich.

```c
bool ps_string_view_equal(ps_string_view a, ps_string_view b);
```

Invalid views compare unequal, including to each other.

## ps_string_view_slice

Leiht einen Teilbereich anhand von Byteindex und Byteanzahl.

```c
ps_result ps_string_view_slice(
    ps_string_view view,
    size_t first,
    size_t count,
    ps_string_view *out);
```

## ps_string_view_find

Sucht die erste Teilfolge ab start; PS_EOF bedeutet keinen Treffer.

```c
ps_result ps_string_view_find(
    ps_string_view view,
    ps_string_view needle,
    size_t start,
    size_t *position);
```

First match at/after start<=view.size. Empty needle matches start, including the end. No match -> PS_EOF. Bounded reads, no allocation; worst case O(n*m).

## ps_string_view_split

Teilt am ersten nichtleeren Trennzeichen in zwei geliehene Views ohne das Trennzeichen.

```c
ps_result ps_string_view_split(
    ps_string_view view,
    ps_string_view delimiter,
    ps_string_view *before,
    ps_string_view *after);
```

Split at the first delimiter, excluding it from both outputs. Delimiter must be nonempty; outputs must be distinct objects. Leading/trailing empty fields are preserved. A missing delimiter yields PS_EOF with both outputs unchanged.

## ps_string_view_trim_ascii

Entfernt ASCII-Leerraum an beiden Enden ohne Allokation.

```c
ps_result ps_string_view_trim_ascii(ps_string_view view, ps_string_view *out);
```

Remove ASCII space, tab, CR, LF, form feed and vertical tab at both ends. Locale-independent; Unicode whitespace and embedded NULs remain unchanged.

## ps_string_view_copy

Kopiert Bytes und ein zusätzliches Nullbyte in einen ausreichend großen Zielpuffer.

```c
ps_result ps_string_view_copy(ps_string_view view, char *output, size_t capacity);
```

Copy every byte, then append NUL. Overlap is supported. capacity must exceed view.size; insufficient capacity -> PS_LIMIT, no partial write. Embedded NULs are preserved, so such output is not a single conventional C text string. Output storage must not overlap a descriptor used after the call.
