# C-Referenz: Dateien und Daten

Reader und Writer werden vom Aufrufer gehalten. Schließe jeden erfolgreich geöffneten Reader/Writer. Wiederhole ps_run_next bis zu einem Ergebnis ungleich PS_OK. PS_EOF bedeutet vollständiger Lauf, PS_RECOVERED einen lesbaren Teil eines beschädigten oder unvollständigen Laufs. ps_put/get_u32 benötigen vier Bytes, ps_put/get_f64 acht Bytes; sie kodieren little-endian.

[Anleitung und Beispiele](../data-format.md) · [Lernpfade](../guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/data.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

Dieses Modul definiert keine zusätzlichen Zahlenkonstanten.

## Typen und Funktionen

### ps_run_writer

```c
typedef struct {
    FILE *file;
    uint32_t channels;
    uint64_t samples;
} ps_run_writer;
```

### ps_run_reader

```c
typedef struct {
    FILE *file;
    uint32_t channels;
    uint64_t samples;
    bool complete;
    char metadata[8192];
    ps_channel schema[PS_MAX_CHANNELS];
} ps_run_reader;
```

## ps_run_create

Erzeugt eine neue Laufdatei mit Kanaldefinitionen und Metadaten; vorhandene Pfade werden geschützt.

```c
ps_result ps_run_create(
    ps_run_writer *writer,
    const char *path,
    const ps_context *context,
    const char *experiment_name);
```

Open exclusively: existing runs are never overwritten. Close all successful opens.

## ps_run_append

Schreibt einen Messpunkt mit Zeit und einem Wert je registriertem Kanal.

```c
ps_result ps_run_append(ps_run_writer *writer, double time_s, const double *values);
```

## ps_run_append_snapshot

Schreibt einen optionalen versionierten Szenenblock mit zugehörigen Werten, ohne den Messpunktzähler zu verändern. Kanalzahl und Szene müssen zum Lauf passen.

```c
ps_result ps_run_append_snapshot(
    ps_run_writer *writer,
    const ps_context *context,
    const ps_scene *scene,
    bool paused);
```

Optional versioned scene chunk. Does not add a measurement row or change its footer count. Existing readers skip it after CRC validation.

## ps_run_close

Finalisiert die Laufdatei mit Abschlussmarker und schließt den Writer.

```c
ps_result ps_run_close(ps_run_writer *writer);
```

Writes bounded CRC-protected index pages and the existing footer. Validates the recorded prefix once at finalization; no allocation. Previous readers skip index chunks. Failure closes the file without a successful footer.

## ps_run_open

Öffnet einen Lauf zum blockweisen Lesen und lädt sein Schema.

```c
ps_result ps_run_open(ps_run_reader *reader, const char *path);
```

## ps_run_next

Liest den nächsten vollständigen Messpunkt; nur PS_OK liefert neue Werte.

```c
ps_result ps_run_next(
    ps_run_reader *reader,
    double *time_s,
    double values[PS_MAX_CHANNELS]);
```

Streaming read: PS_EOF = finalized run; PS_RECOVERED = incomplete/corrupt tail. Time and values change only on PS_OK, after the entire sample is validated. Stop reading on any other result; the file cursor may already have advanced.

## ps_run_snapshot_next

Liest den nächsten validierten Szenenblock mit einem eigenen Reader, zählt übersprungene Messpunkte und prüft den Footer. Fehler lassen den ausgegebenen Snapshot unverändert.

```c
ps_result ps_run_snapshot_next(ps_run_reader *reader, ps_snapshot *snapshot);
```

Use a separate reader to stream snapshots. Measurement rows are validated and counted while scanning; PS_EOF/PS_RECOVERED have the same meanings as above. Legacy runs contain no snapshots. Failed reads leave snapshot unchanged.

## ps_run_reader_close

Schließt einen geöffneten Reader.

```c
void ps_run_reader_close(ps_run_reader *reader);
```

## ps_channel_status_index

Findet den zugehörigen .status-Kanal; index=-1 bedeutet keine Statuszuordnung.

```c
ps_result ps_channel_status_index(
    const ps_channel *schema,
    uint32_t count,
    uint32_t channel,
    int *index);
```

Optional measurement convention: <name>.status masks <name> and <name>.u. States are 0=not due, 1=valid, 2=dropped. Returns index=-1 when absent; rejects malformed/ambiguous names or a status channel with physical dimensions. Raw files/CSV and Series retain every row; consumers must apply this mask.

## ps_run_export_csv

Exportiert Rohzeit und sämtliche Kanäle als CSV, einschließlich Status und ungültiger Sensorzeilen.

```c
ps_result ps_run_export_csv(const char *input, const char *output);
```

## ps_crc32

Berechnet die CRC-32-Prüfsumme über size Bytes ohne Allokation.

```c
uint32_t ps_crc32(const unsigned char *data, size_t size);
```

CRC-32: reflected polynomial 0xedb88320, initial/final XOR 0xffffffff. No allocation or mutable state. data may be NULL only when size is zero.

## ps_put_u32

Schreibt einen 32-Bit-Wert little-endian in mindestens vier beschreibbare Bytes.

```c
void ps_put_u32(unsigned char *out, uint32_t value);
```

## ps_get_u32

Liest einen little-endian 32-Bit-Wert aus mindestens vier lesbaren Bytes.

```c
uint32_t ps_get_u32(const unsigned char *in);
```

## ps_put_f64

Schreibt einen Double-Wert little-endian in mindestens acht beschreibbare Bytes.

```c
void ps_put_f64(unsigned char *out, double value);
```

## ps_get_f64

Liest einen little-endian Double-Wert aus mindestens acht lesbaren Bytes.

```c
double ps_get_f64(const unsigned char *in);
```
