# C-Referenz: Logging mit explizitem Sink

Ein Logger verbindet einen benannten Schweregrad und eine endliche Modellzeit mit einem synchronen Sink. Kein globaler Logger und keine implizite Ausgabe. Kontextlogger gehören dem Host; ps_experiment_log verwendet die aktuelle Simulationszeit. Nachrichten sind begrenztes UTF-8; Rückgabewerte entscheiden über die Behandlung verworfener Meldungen.

[Anleitung und Beispiele](../logging.md) · [Lernpfade](../guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/log.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

```c
#define PS_LOG_MESSAGE_MAX 1024u
```

## Typen und Funktionen

### ps_log_level

```c
typedef enum { PS_LOG_DEBUG=1, PS_LOG_INFO, PS_LOG_WARNING, PS_LOG_ERROR } ps_log_level;
```

### ps_log_record

```c
typedef struct {
    ps_log_level level;
    double time_s;
    char message[PS_LOG_MESSAGE_MAX+1u];
} ps_log_record;
```

### ps_logger

```c
typedef struct {
    void *user;
    ps_result (*write)(void *user,const ps_log_record *record);
} ps_logger;
```

Explicit synchronous sink; no global state or implicit stdout/stderr output. The borrowed record exists only during write. Sink/user must outlive the logger, and calls require external synchronization. NULL write disables it.

## ps_log_record_valid

Prüft Schweregrad, endliche Modellzeit und terminierte UTF-8-Nachricht mit höchstens 1024 Bytes.

```c
bool ps_log_record_valid(const ps_log_record *record);
```

Finite logical time, bounded terminated UTF-8. Newline/CR/tab are permitted; other control characters, empty messages and unknown levels are invalid.

## ps_logger_emit

Validiert und kopiert eine Meldung, ruft den expliziten Sink synchron auf und gibt dessen Ergebnis zurück. Ein deaktivierter Sink akzeptiert gültige Meldungen ohne I/O.

```c
ps_result ps_logger_emit(
    const ps_logger *logger,ps_log_level level,double time_s,
    const char *message);
```

Validate before invoking the sink. A disabled sink succeeds without I/O. Failures from the sink propagate; the caller chooses whether they are fatal.

## ps_log_level_name

Liefert debug, info, warning oder error als unveränderlichen Bibliothekstext; unbekannte Werte liefern unknown.

```c
const char *ps_log_level_name(ps_log_level level);
```
