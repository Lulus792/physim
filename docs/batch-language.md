# Archivierte Batch- und Monte-Carlo-Analysen

Physim 0.178.0 startet archivierte Laufserien direkt aus einer Analyse. `Batch`
ist eine besitzende Konfiguration und nach der Ausführung ein unveränderlicher
Ergebnis-Snapshot. Die gleichen Runner, Archive, Checkpoints und Statistikfunktionen
stehen C-Analysen über `ps_analysis_services` zur Verfügung. Der Analysehost stellt
die Dienste ausdrücklich bereit; der Core enthält keinen Prozesscontroller.

## Vollständiges SDK-Beispiel

Das SDK liefert `examples/language/batch_analysis.phys` und das kompilierte
Modul `bin/language-batch_analysis.so` beziehungsweise `.dll`. Der vollständige
C-Pfad steht in `examples/documentation/batch_analysis.c` und im kompilierten
Modul `bin/batch_analysis.so` beziehungsweise `.dll`. Beide Analysen starten
256 Würfe mit unsicheren Anfangsgeschwindigkeiten, liest eine rohe Laufdatei zurück
und erstellt Histogramm sowie Mittelwert, Stichprobenstreuung und Type-7-Quantile.
Hier ist der explizite Analyseinput ein kompiliertes Experimentmodul. `Dataset`
verlangt weiterhin eine Messdatei; dieses Beispiel benutzt dafür `RunIndex` auf den
neu erzeugten Archiven.

Linux/macOS mit einem neu installierten SDK:

```sh
sdk="$PWD/build/SDK"
mkdir -p "$PWD/Batch results"
"$sdk/bin/physim-analysis-runner" "$sdk/bin/language-batch_analysis.so" --runs \
    "$PWD/Batch results/report" "$sdk/bin/uncertain_projectile.so"
```

Für ein Physim-Experiment den letzten Pfad durch
`"$sdk/bin/language-uncertain_projectile.so"` ersetzen und ein neues
Ausgabeprefix wählen. Unter Windows heißen die Werkzeuge `.exe` und die Module
`.dll`; alle Modul- und Ausgabepfade müssen absolut sein. Vorhandene Serien und
Berichte werden erhalten. Der Ausgabeordner `report-series` muss neu sein.

Für dieselbe Auswertung in C das Analysemodul durch `"$sdk/bin/batch_analysis.so"`
ersetzen und wieder ein neues Ausgabeprefix wählen. Beide Quellen sind vollständig
und können direkt über `physim-build` in einem Analyseprojekt gebaut werden.

## Konfiguration und Besitz

```text
let request = Batch(module: experimentPath, directory: newDirectory,
                    channel: "position", runs: 6, steps: 100, dt: 0.1,
                    seed: 42, workers: 4)
let study = request.parameter("offset",0.4).target(0.7)
                   .adaptive(0.02,0.3).sweep("velocity",0.2,3)
let finished = study.run()
finished.requireSuccess()
report("Parameter study")
finished.series().histogram("Final positions",3)
```

`parameter`, `sweep`, `target`, `adaptive`, `limits` und `source` erzeugen neue
Werte. Ein Setter ersetzt das frühere Ergebnis nur in seiner neuen Kopie;
`request` und weitere gespeicherte Kopien bleiben unverändert. Arrays, optionale
Werte, Strukturfelder und Closures besitzen ihre Batch-Kopien automatisch.
`source(path)` archiviert die Quelldatei zusätzlich zum verwendeten Modul.
`outputPrefix()` kopiert das Prefix des aktuellen Analyseaufrufs. Für einen daraus
abgeleiteten Batch-Ordner das Analyseprefix ebenfalls absolut angeben.

Parameter und Sweepgrenzen verwenden SI-Werte; erklärte Anzeigeeinheiten ändern
sie nicht. Adaptive Läufe benötigen zuerst eine positive gemeinsame Zielzeit.
`steps` ist dann das akzeptierte Schrittbudget. Grenzen: 1000 Läufe, acht Worker,
100.000 Schritte je Lauf und fünf Millionen konfigurierte Samples insgesamt.
`limits(timeout,memoryMiB)` setzt das Limit je Runner; 0 MiB deaktiviert die
Speichergrenze. Die Plattformsemantik entspricht dem gewöhnlichen Seriencontroller.

Der Int64-Seed trägt die vollständige 64-Bit-Bitfolge, wie `Rng` und `Sensor`.
Negative Werte entsprechen den oberen uint64-Werten. `seed + runs - 1` darf
nicht überlaufen; die Konfiguration wird sonst abgewiesen.

## Ergebnisse und fehlende Messungen

`run()` startet eine neue Serie. `executed()` unterscheidet einen Ergebniswert
von einer noch nicht ausgeführten Konfiguration. `code()` und `error()` bewahren
auch Fehler des Controllers; `completed()`, `started()`, `reused()`, `valid()`
und `peakActive()` liefern die tatsächlich beobachteten Zähler.
`requireSuccess()` fordert einen vollständigen Erfolg und wirft sonst eine
abfangbare Diagnose. Ein erfolgreich abgefangenes Problem erzeugt keine
fehlenden Messwerte und macht keinen Gesamtbericht aus einer Teilserie.

`status(i)` und `statuses()` verwenden den nullbasierten Laufindex:
0 nicht fällig, 1 gültig, 2 verworfen. `finished(i)` bedeutet validiert und
journaliert, auch bei Status 2. `value(i)` verlangt Status 1. `values()` und
`series()` enthalten ausschließlich gültige Endwerte in Indexreihenfolge;
für ihre Zuordnung bei Lücken die Statuswerte verwenden. Eine leere Series wird
abgewiesen. `unit()` liefert die verifizierte kanonische SI-Einheit. Ihr Symbol
bleibt für die Modul-Lebensdauer gültig, auch nachdem die Batch-Kopie freigegeben wurde.

`runPath(i)` liefert den vorgesehenen Archivpfad. Der Pfad allein beweist keine
fertige Datei; `finished`, `status` und die normalen Reader-/Indexprüfungen entscheiden.
Die bestehenden Series-Funktionen liefern Histogramme, Statistik, Quantile und
Exporte; die Grenzen ihrer statistischen Aussage stehen im
[Monte-Carlo-Vertrag](monte-carlo.md).

## Pause und Wiederaufnahme

```text
let paused = request.runUntil(2)
assert(paused.cancelled() && paused.completed() == 2)
let resumed = Batch.resume(paused.directory(),anotherNewDirectory).run()
resumed.requireSuccess()
assert(resumed.reused() == 2)
```

`runUntil(n)` pausiert nach n validierten Abschlüssen einschließlich übernommener
Läufe. 0 oder die gesamte Laufzahl führt die Serie vollständig aus. Der Controller
beendet übrige Worker und bewahrt fertige Rohdateien und das geflushte Journal.
Eine Pause besitzt keinen Gesamtbericht. `Batch.resume` prüft Checkpoint,
Runner-Fingerprint, archiviertes Modul und Quellen; `run()` validiert Journal und
Rohdateien erneut. Die alte Serie bleibt schreibgeschützt, die Fortsetzung erhält
einen neuen Ordner. Geänderte Konfiguration oder beschädigte Checkpoints werden
abgewiesen. Ein harter Analyseprozess-Abbruch beendet aktive Worker über deren
Eltern-Pipeüberwachung; nur wirklich journalierte Archive sind wiederverwendbar.

## C-Hostvertrag und Kompatibilität

`analysis.h` erhält einen optionalen `run_host`-Tail innerhalb ABI 3. Alte
Einlauf-, Mehrlauf- und Diagnosetäler behalten ihre Positionen. Der Runner prüft
`struct_size` vor jedem Tail; `PS_ANALYSIS_API_DIAGNOSTIC_SIZE` beschreibt den
bisherigen Diagnosetail. Die Dienste sind synchron und dürfen nicht nach der
Rückkehr aus `run_host` behalten werden. Hostversion 1 und ausreichende Feldgröße
werden vor dem Lesen der Callbacks geprüft.

`run_batch(user,request,stop_after,result)` erhält eine geliehene Konfiguration;
der Host liefert den Pfad seines passenden Experiment-Runners. Ergebnisse können
bei einem Fehler validierte Teilfortschritte enthalten. `resume_batch` lädt die
archivierte Konfiguration in Speicher des Aufrufers. Ohne passenden Dienst sind
Sprachaufrufe mit einer `PS_VERSION`-Diagnose abfangbar. Eigenständige Programme
und Experimentmodule können Batch-Werte konfigurieren, aber keine Analyseserien
starten. [Vollständige C-Strukturen](reference/batch.md),
[registrierte Sprachsignaturen](reference/language-library.md).
