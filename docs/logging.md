# Logging

Experimente schreiben Meldungen über einen vom Host gehaltenen Logger. Die App
zeigt Schweregrad und Simulationszeit im Logbereich. C und Physim verwenden
denselben Kanal; die Messwerte und Snapshots enthalten keine Logtexte.

```c
ps_result result = ps_experiment_log(context, PS_LOG_INFO, "Experiment bereit");
/* Der Aufrufer entscheidet, ob ein Logfehler die Simulation beenden soll. */
```

```text
let accepted = logInfo("Experiment bereit")
logWarning("Schritt mit kleiner Zeitschrittweite")
```

`logDebug`, `logInfo`, `logWarning` und `logError` sind nur im Experiment
verfügbar und liefern `Bool`. Schweregrade sind beschreibend; `logError` allein
beendet keinen Lauf. Der Logger liest die aktuelle `context->time_s`. Beim
Eintritt in `step` ist das die Zeit vor dem Schritt. Meldungen aus `create`, der
Initialisierung und `destroy` können ebenfalls geschrieben werden. Das vollständige
Beispiel liegt in [C](../examples/logging/main.c) und
[Physim](../examples/language/logging.phys).

Die öffentliche [C-Referenz](reference/log.md) beschreibt auch eigene Sinks:
`ps_logger` enthält ausschließlich einen Benutzerzeiger und einen synchronen
Callback. Kein globaler Logger, keine implizite Ausgabe. Der Callback leiht den
Record nur während seines Aufrufs; kopiere ihn für spätere Verwendung. Sink und
Benutzerdaten müssen leben, solange der Logger benutzt wird. Parallele Aufrufe
benötigen Synchronisierung durch den Besitzer. Ein Callback mit Wert `NULL`
deaktiviert die Ausgabe und akzeptiert gültige Meldungen ohne I/O.

Jede Meldung benötigt einen bekannten Schweregrad (1 Debug, 2 Info, 3 Warning,
4 Error), eine endliche Zeit und 1 bis 1024 UTF-8-Bytes; der NUL-Abschluss
zählt nicht mit. C-Eingaben müssen NUL-terminiert sein.
Ungültiges UTF-8 und Steuerzeichen außer LF, CR und Tab werden abgewiesen. Die
Core-API liefert `PS_INVALID`; ein älterer ABI-3-Context ohne optionalen
Logger-Tail liefert `PS_VERSION`. Andere Felder und Versionsnummern der API/ABI 3
bleiben erhalten. Den vom Host gesetzten Logger im Experiment nicht ersetzen.

## Gespeicherte Meldungen und Grenzen

Der Runner erstellt `<output.psrun>.pslog` beim ersten akzeptierten Logeintrag.
Es ist UTF-8-JSONL, ein Objekt pro Zeile mit `level`, `time_s` und `message`:

```json
{"level":2,"time_s":0,"message":"Experiment bereit"}
```

JSON-Escaping erhält Anführungszeichen, Backslashes, Zeilenumbrüche und Tabs.
Die Datei wird exklusiv neu angelegt; eine bestehende Datei wird nicht verändert.
Jede Zeile wird mit `fflush` herausgeschrieben. Das ist keine Zusicherung gegen
Stromausfall; nach einem Prozessabbruch kann eine unvollständige letzte Zeile
zurückbleiben. Vollständige vorherige Zeilen bleiben separat von der Rohdatei
lesbar. Module ohne Meldungen erzeugen keine Sidecar-Datei. `--describe`
deaktiviert Logging vollständig, damit die Parameterliste unverändert bleibt.

Ein Runner akzeptiert höchstens 4096 Meldungen und 4 MiB unescaped Nachrichtentext
pro Lauf. Weitere Aufrufe liefern `PS_LIMIT` bzw. `false`. Beim regulären Beenden
folgt höchstens eine Warning mit der Zahl der verworfenen Meldungen. Schreibfehler
liefern `PS_IO` bzw. `false`; der Runner versucht danach keine weiteren
Logdatei-Ausgaben. Diese Rückgabewerte ändern nicht automatisch das Modellresultat.
Prüfe sie im Experiment, wenn vollständiges Logging erforderlich ist.

## Interaktives Protokoll

`--interactive --log-events` aktiviert zusätzlich `PS_MSG_LOG` (Typ 11) im
versionierten Wire-4-Datenstrom. Ohne dieses ausdrückliche Opt-in bleiben die
bisherigen Nachrichten erhalten; die Sidecar wird trotzdem geschrieben. Die App
fordert Logevents an. Eine Meldung kann vor HELLO erscheinen und `destroy`-Meldungen
stehen vor BYE. Der Payload enthält little-endian `u32 level`, `f64 time_s` und
UTF-8-Text ohne NUL; die Frame-Länge bestimmt die Textlänge. Empfänger müssen
Schweregrad, Zeit, Text und Länge validieren. Typ 10 bleibt SPEED.

In interaktiven Experimentmodulen sind stdout und stderr für den Runner reserviert.
Beliebige `printf`-/`fprintf`-Ausgaben werden nicht in Logevents umgewandelt und
können das Protokoll beschädigen. Verwende die Logging-API. Dieser Kanal ist keine
OS-Sandbox und begrenzt keine anderen Ressourcen des Moduls.
