# C-Referenz: Strukturierte Diagnosen

Diagnosen sind eigene begrenzte UTF-8-Werte mit Fehlercode, Operation, Argument und Quellposition. Kein globaler Last-error-Zustand. Konstruktion und Decodierung sind transaktional, Dateien werden exklusiv erstellt. Experiment- und Analyse-Runner transportieren die Daten zusätzlich zur bisherigen Textausgabe.

[Anleitung und Beispiele](../diagnostics.md) · [Teil I – C](../c-guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/diagnostic.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

```c
#define PS_DIAGNOSTIC_VERSION 1u
#define PS_DIAGNOSTIC_LABEL_MAX 64u
#define PS_DIAGNOSTIC_SOURCE_MAX 1024u
#define PS_DIAGNOSTIC_MESSAGE_MAX 1024u
#define PS_DIAGNOSTIC_WIRE_MAX 2216u
```

## Typen und Funktionen

### ps_diagnostic

```c
typedef struct {
    uint32_t struct_size, version;
    ps_result code;
    uint32_t line, column;
    char operation[PS_DIAGNOSTIC_LABEL_MAX+1u], argument[PS_DIAGNOSTIC_LABEL_MAX+1u];
    char source[PS_DIAGNOSTIC_SOURCE_MAX+1u], message[PS_DIAGNOSTIC_MESSAGE_MAX+1u];
} ps_diagnostic;
```

Owned UTF-8, no global last-error state. Line/column are 1-based; zero means unknown. Column requires a line; coordinates require a source. Messages may contain LF/CR/tab, other fields have no controls. Empty labels are allowed.

## ps_diagnostic_clear

Setzt einen vorhandenen Wert auf eine gültige leere Diagnose mit Code PS_OK.

```c
void ps_diagnostic_clear(ps_diagnostic *diagnostic);
```

Clear is a valid PS_OK record with empty fields. Failure records exclude PS_OK, PS_EOF and PS_RECOVERED. All copies remain independent.

## ps_diagnostic_valid

Prüft Größe, Version, Fehlercode, UTF-8-Felder und konsistente Quellkoordinaten.

```c
bool ps_diagnostic_valid(const ps_diagnostic *diagnostic);
```

## ps_diagnostic_set

Kopiert einen vollständigen Fehler in den Ausgabe-Wert; ungültige Eingaben erhalten ihn unverändert.

```c
ps_result ps_diagnostic_set(
    ps_diagnostic *out,
    ps_result code,
    const char *operation,
    const char *argument,
    const char *source,
    uint32_t line,
    uint32_t column,
    const char *message);
```

Copy synchronously; NULL optional labels/source mean empty. Message required. Invalid input leaves output unchanged. Returns PS_OK after storing the error; the stored code is independent of this constructor's result.

## ps_diagnostic_format

Formatiert Fehlercode, Operation, Argument und Quellstelle als begrenztes UTF-8; PS_LIMIT meldet Kürzung.

```c
ps_result ps_diagnostic_format(
    const ps_diagnostic *diagnostic,
    char *out,
    size_t capacity);
```

NUL-terminated UTF-8, bounded truncation; PS_LIMIT if shortened. A clear record formats to an empty string. Invalid arguments preserve the output.

## ps_diagnostic_encode

Kodiert einen Fehler mit exakten Längen, little-endian Feldern und CRC; liefert die Bytezahl oder 0.

```c
size_t ps_diagnostic_encode(
    unsigned char *out,
    size_t capacity,
    const ps_diagnostic *diagnostic);
```

Versioned little-endian payload with exact lengths and CRC. Encode returns 0 for clear/invalid records or insufficient capacity; decode is transactional.

## ps_diagnostic_decode

Prüft den gesamten versionierten Fehlerpayload; Änderungen erfolgen erst nach vollständiger Validierung.

```c
ps_result ps_diagnostic_decode(
    const unsigned char *data,
    size_t size,
    ps_diagnostic *out);
```

## ps_diagnostic_save

Schreibt einen gültigen Fehler exklusiv in eine neue Binärdatei; vorhandene Dateien bleiben erhalten.

```c
ps_result ps_diagnostic_save(const char *path, const ps_diagnostic *diagnostic);
```

Exclusive binary sidecar. No overwrite; load validates complete file/CRC and leaves output unchanged on error. This is not a power-loss durability guarantee.

## ps_diagnostic_load

Lädt einen vollständigen Fehler und prüft Version, Grenzen, CRC und Text; Fehler erhalten die Ausgabe.

```c
ps_result ps_diagnostic_load(const char *path, ps_diagnostic *out);
```
